/* umdevxs_chrdev.c
 *
 * Character Device interface for the Linux UMDevXS driver.
 */

/*****************************************************************************
* Copyright (c) 2009-2018 INSIDE Secure B.V. All Rights Reserved.
*
* This confidential and proprietary software may be used only as authorized
* by a licensing agreement from INSIDE Secure.
*
* The entire notice above must be reproduced on all authorized copies that
* may only be made to the extent permitted by a licensing agreement from
* INSIDE Secure.
*
* For more information or support, please go to our online support system at
* https://customersupport.insidesecure.com.
* In case you do not have an account for this system, please send an e-mail
* to ESSEmbeddedHW-Support@insidesecure.com.
*****************************************************************************/

#include "c_umdevxs.h"              // config options

#include "basic_defs.h"             // IDENTIFIER_NOT_USED
#include "umdevxs_internal.h"
#include "umdevxs_cmd.h"            // UMDevXS_CmdRsp_t

#include <linux/fs.h>
#include <linux/mm.h>               // remap_pfn_range, PAGE_SHIFT
#include <linux/module.h>           // THIS_MODULE
#include <linux/uaccess.h>          // copy_to/from_user, access_ok
#include <linux/errno.h>            // EIO

// character device related
static const char UMDevXS_ChrDev_module_name[] = UMDEVXS_MODULENAME"_c";
static int UMDevXS_ChrDev_major_nr = 0;


/*----------------------------------------------------------------------------
 * UMDevXS_ChrDev_fop_open
 */
static int
UMDevXS_ChrDev_fop_open(
    struct inode * inode,
    struct file * file_p)
{
    LOG_INFO(
        UMDEVXS_LOG_PREFIX
        "UMDevXS_ChrDev_fop_open: "
        "file_p=%p\n",
        file_p);

    IDENTIFIER_NOT_USED(inode);
    IDENTIFIER_NOT_USED(file_p);

    return 0;       // 0 = success
}


/*----------------------------------------------------------------------------
 * UMDevXS_ChrDev_fop_release
 */
static int
UMDevXS_ChrDev_fop_release(
    struct inode * inode,
    struct file * file_p)
{
    LOG_INFO(
        UMDEVXS_LOG_PREFIX
        "UMDevXS_ChrDev_fop_release: "
        "file_p=%p\n",
        file_p);

#ifndef UMDEVXS_REMOVE_SMBUF
    UMDevXS_SMBuf_CleanUp(file_p);
#endif

    IDENTIFIER_NOT_USED(inode);
    IDENTIFIER_NOT_USED(file_p);

    return 0;   // 0 = success
}


/*----------------------------------------------------------------------------
 * UMDevXS_ChrDev_fop_mmap
 */
static int
UMDevXS_ChrDev_fop_mmap(
    struct file * file_p,
    struct vm_area_struct * vma_p)
{
    unsigned int Length = vma_p->vm_end - vma_p->vm_start;
    int Handle = vma_p->vm_pgoff;
    int Class = UMDevXS_Handle_GetClass(Handle);
    int Index = UMDevXS_Handle_GetIndex(Handle);

    LOG_INFO(
        UMDEVXS_LOG_PREFIX
        "fop_mmap: "
        "file_p=%p, Handle 0x%x [Class=%d, Index=0x%x] Length=%u (0x%x)\n",
        file_p,
        Handle,
        Class,
        Index,
        Length, Length);

    // get the handle class
    switch(Class)
    {
#ifndef UMDEVXS_REMOVE_DEVICE
    case UMDEVXS_HANDLECLASS_DEVICE:
    {
        int res;

        res = UMDevXS_Device_Map(Index, Length, vma_p);

        if (res == 0)
        {
            // mapping successful
            return 0;       // ## RETURN ##
        }

        LOG_INFO(
            UMDEVXS_LOG_PREFIX
            "fop_mmap: "
            "UMDevXS_Device_Map returned %d\n",
            res);
    }
    break;
#endif

#ifndef UMDEVXS_REMOVE_SMBUF
    case UMDEVXS_HANDLECLASS_SMBUF:
    {
        int res;
        res = UMDevXS_SMBuf_Map(Index, Length, vma_p);

        if (res == 0)
        {
            // mapping successful
            return 0;       // ## RETURN ##
        }

        LOG_INFO(
            UMDEVXS_LOG_PREFIX
            "fop_mmap: "
            "UMDevXS_SMBuf_Map returned %d\n",
            res);
    }
    break;
#endif

    //case UMDEVXS_HANDLECLASS_DMABUF:
    default:
        LOG_INFO(
            UMDEVXS_LOG_PREFIX
            "fop_mmap: "
            "Invalid handle (0x%x, %d, %d)\n",
            Handle,
            Class,
            Index);

        return -EIO;
    }

    IDENTIFIER_NOT_USED(file_p);
    IDENTIFIER_NOT_USED(Index);
    IDENTIFIER_NOT_USED(Length);

    return -EIO;
}


/*----------------------------------------------------------------------------
 * UMDevXS_ChrDev_fop_read
 *
 * Read interface is used to wait for interrupts.
 */
static ssize_t
UMDevXS_ChrDev_fop_read(
    struct file * file_p,
    char * buf,
    size_t count,
    loff_t * ppos)
{
    int res;

    if (file_p == NULL || buf == NULL)
    {
        return -EIO;
    }

    LOG_INFO(
        UMDEVXS_LOG_PREFIX
        "UMDevXS_ChrDev_fop_read: "
        "file_p=%p, "
        "count=%d, ppos=%d\n",
        file_p,
        (int)count,
        (int)ppos);

#ifndef UMDEVXS_REMOVE_INTERRUPT
    res = UMDevXS_Interrupt_WaitWithTimeout(count);
#else
    res = -1;
#endif

    if (res < 0)
    {
        return -EIO;
    }

    IDENTIFIER_NOT_USED(file_p);
    IDENTIFIER_NOT_USED(buf);
    IDENTIFIER_NOT_USED(count);
    IDENTIFIER_NOT_USED(ppos);

    return res;
}


/*----------------------------------------------------------------------------
 * UMDevXS_ChrDev_fop_write
 *
 * Write interface is used for Cmd/Rsp passing.
 */
static ssize_t
UMDevXS_ChrDev_fop_write(
    struct file * file_p,
    const char * buf,
    size_t count,
    loff_t * ppos)
{
    if (file_p == NULL || buf == NULL)
    {
        return -EIO;
    }

    if (count != sizeof(UMDevXS_CmdRsp_t))
    {
        LOG_INFO(
            UMDEVXS_LOG_PREFIX
            "UMDevXS_ChrDev_fop_write: "
            "Expected count=%d\n",
            sizeof(UMDevXS_CmdRsp_t));

        return -EINVAL;     // ## RETURN ##
    }

    if (!access_ok(VERIFY_WRITE, buf, sizeof(UMDevXS_CmdRsp_t)))
    {
        LOG_INFO(
            UMDEVXS_LOG_PREFIX
            "UMDevXS_ChrDev_fop_write: "
            "Failed on access_ok\n");

        return -EINVAL;     // ## RETURN ##
    }

    {
        UMDevXS_CmdRsp_t CmdRsp;
        UMDevXS_CmdRsp_t CmdRsp_CopyForErrorReport;

        // copy the CmdRsp from application space into kernel space
        if (copy_from_user(
                    &CmdRsp,
                    buf,
                    sizeof(UMDevXS_CmdRsp_t)) != 0)
        {
            LOG_INFO(
                UMDEVXS_LOG_PREFIX
                "UMDevXS_ChrDev_fop_write: "
                "Failed on copy_from_user\n");

            return -EINVAL;     // ## RETURN ##
        }

        if (CmdRsp.Magic != UMDEVXS_CMDRSP_MAGIC)
        {
            LOG_INFO(
                UMDEVXS_LOG_PREFIX
                "UMDevXS_ChrDev_fop_write: "
                "Failed on Magic check\n");

            return -EINVAL;     // ## RETURN ##
        }

        memcpy(&CmdRsp_CopyForErrorReport, &CmdRsp, sizeof(UMDevXS_CmdRsp_t));

        LOG_INFO(
            UMDEVXS_LOG_PREFIX
            "Cmd: "
            "Opcode=%d, Handle=0x%x, uint1/2/3=%u/%u/%u, ptr1=%p\n",
            CmdRsp.Opcode,
            CmdRsp.Handle,
            CmdRsp.uint1,
            CmdRsp.uint2,
            CmdRsp.uint3,
            CmdRsp.ptr1);

        // handle the request
        switch(CmdRsp.Opcode)
        {
#ifndef UMDEVXS_REMOVE_SMBUF
        case UMDEVXS_OPCODE_SMBUF_ALLOC:
        case UMDEVXS_OPCODE_SMBUF_REGISTER:
        case UMDEVXS_OPCODE_SMBUF_FREE:
        case UMDEVXS_OPCODE_SMBUF_ATTACH:
        case UMDEVXS_OPCODE_SMBUF_DETACH:
        case UMDEVXS_OPCODE_SMBUF_GETBUFINFO:
        case UMDEVXS_OPCODE_SMBUF_SETBUFINFO:
        case UMDEVXS_OPCODE_SMBUF_COMMIT:
        case UMDEVXS_OPCODE_SMBUF_REFRESH:
            UMDevXS_SMBuf_HandleCmd(file_p, &CmdRsp);
            break;
#endif
#ifndef UMDEVXS_REMOVE_DEVICE
        case UMDEVXS_OPCODE_DEVICE_FIND:
            UMDevXS_Device_HandleCmd_Find(&CmdRsp);
            break;

        case UMDEVXS_OPCODE_DEVICE_ENUM:
            UMDevXS_Device_HandleCmd_Enum(&CmdRsp);
            break;

#ifndef UMDEVXS_REMOVE_DEVICE_PCICFG
        case UMDEVXS_OPCODE_DEVICE_PCICFG_READ32:
            UMDevXS_PCIDev_HandleCmd_Read32(&CmdRsp);
            break;

        case UMDEVXS_OPCODE_DEVICE_PCICFG_WRITE32:
            UMDevXS_PCIDev_HandleCmd_Write32(&CmdRsp);
            break;
#endif // UMDEVXS_REMOVE_DEVICE_PCICFG
#endif
        default:
            LOG_CRIT(
                UMDEVXS_LOG_PREFIX
                "Cmd (%d) not supported\n",
                CmdRsp.Opcode);
            CmdRsp.Error = 1;
            break;
        }

        // copy the result back to the application space
        {
            char * writebuf_p = (char *)buf;    // get rid of const attribute

            if (copy_to_user(
                        writebuf_p,
                        &CmdRsp,
                        sizeof(UMDevXS_CmdRsp_t)) != 0)
            {
                LOG_INFO(
                    UMDEVXS_LOG_PREFIX
                    "UMDevXS_ChrDev_fop_write: "
                    "Failed on copy_to_user\n");
            }
        }

        LOG_INFO(
            UMDEVXS_LOG_PREFIX
            "Rsp: "
            "Error=%d, Handle=0x%x, uint1/2/3=%u/%u/%u, ptr1=%p\n",
            CmdRsp.Error,
            CmdRsp.Handle,
            CmdRsp.uint1,
            CmdRsp.uint2,
            CmdRsp.uint3,
            CmdRsp.ptr1);
#ifndef LOG_INFO_ENABLED
// (no duplicate logging when INFO logging is enabled)
#ifdef UMDEVXS_CHRDEV_LOG_ERRORS
        if (CmdRsp.Error)
        {
            LOG_WARN(
                UMDEVXS_LOG_PREFIX
                "Cmd: "
                "Opcode=%d, Handle=0x%x, uint1/2/3=%u/%u/%u, ptr1=%p\n",
                CmdRsp_CopyForErrorReport.Opcode,
                CmdRsp_CopyForErrorReport.Handle,
                CmdRsp_CopyForErrorReport.uint1,
                CmdRsp_CopyForErrorReport.uint2,
                CmdRsp_CopyForErrorReport.uint3,
                CmdRsp_CopyForErrorReport.ptr1);
            LOG_WARN(
                UMDEVXS_LOG_PREFIX
                "Rsp: "
                "Error=%d, Handle=0x%x, uint1/2/3=%u/%u/%u, ptr1=%p\n",
                CmdRsp.Error,
                CmdRsp.Handle,
                CmdRsp.uint1,
                CmdRsp.uint2,
                CmdRsp.uint3,
                CmdRsp.ptr1);
        }
#endif /* UMDEVXS_CHRDEV_LOG_ERRORS */
#endif /* !LOG_INFO */
    }

    IDENTIFIER_NOT_USED(ppos);

    // extremely important to return 'count' upon success
    // otherwise we can get called forever
    return count;
}


/*----------------------------------------------------------------------------
 * File Operations structure
 *
 * Used by the character device.
 * Contains pointers to the handler functions.
 */
static struct file_operations UMDevXS_ChrDev_fops =
{
#ifdef _MSC_VER
    // microsoft compiler does not support partial initializers
    // NOTE: struct must have fields in this order
    UMDevXS_ChrDev_fop_read,
    UMDevXS_ChrDev_fop_write,
    UMDevXS_ChrDev_fop_mmap,
    UMDevXS_ChrDev_fop_open,
    UMDevXS_ChrDev_fop_release
#else
    .owner = THIS_MODULE,
    .open = UMDevXS_ChrDev_fop_open,
    .release = UMDevXS_ChrDev_fop_release,

    // mmap support is used to map a buffer into user space
    .mmap = UMDevXS_ChrDev_fop_mmap,

    // read is used in a blocking fashion to wait for interrupts
    .read = UMDevXS_ChrDev_fop_read,

    // write is used for cmd/rsp passing
    .write = UMDevXS_ChrDev_fop_write
#endif
};


/*----------------------------------------------------------------------------
 * UMDevXS_ChrDev_Init
 *
 * This routine hooks the character device.
 *
 * Return Value:
 *     0    Success
 *     <0   Error code return by a kernel call
 */
int
UMDevXS_ChrDev_Init(void)
{
    int Status;

    // register the character device side
    Status = register_chrdev(
                 UMDevXS_ChrDev_major_nr,
                 UMDevXS_ChrDev_module_name,
                 &UMDevXS_ChrDev_fops);

    if (Status < 0)
    {
        LOG_CRIT(
            UMDEVXS_LOG_PREFIX
            "Failed to register the chrdev (%d)\n",
            Status);

        return Status;
    }

    if (UMDevXS_ChrDev_major_nr == 0)
    {
        UMDevXS_ChrDev_major_nr = Status;
    }

    LOG_INFO(
        UMDEVXS_LOG_PREFIX
        "created device '%s'"
        ", major=%d\n",
        UMDevXS_ChrDev_module_name,
        UMDevXS_ChrDev_major_nr);

    return 0;   // 0 = success
}


/*----------------------------------------------------------------------------
 * UMDevXS_ChrDev_UnInit
 *
 * This routine unhooks the character device.
 */
void
UMDevXS_ChrDev_UnInit(void)
{
    if (UMDevXS_ChrDev_major_nr)
    {
        unregister_chrdev(
            UMDevXS_ChrDev_major_nr,
            UMDevXS_ChrDev_module_name);

        LOG_INFO(
            UMDEVXS_LOG_PREFIX
            "destroyed device '%s'"
            ", major=%d\n",
            UMDevXS_ChrDev_module_name,
            UMDevXS_ChrDev_major_nr);

        UMDevXS_ChrDev_major_nr = 0;
    }
}


/* end of file umdevxs_chrdev.c */
