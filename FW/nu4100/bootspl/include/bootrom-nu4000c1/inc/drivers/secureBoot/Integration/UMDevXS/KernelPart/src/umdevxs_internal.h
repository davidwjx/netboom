/* umdevxs_internal.h
 *
 * Linux UMDevXS Driver Internal Interfaces.
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

#ifndef INCLUDE_GUARD_UMDEVXS_INTERNAL_H
#define INCLUDE_GUARD_UMDEVXS_INTERNAL_H

#include "c_umdevxs.h"

#include "umdevxs_cmd.h"        // UMDevXS_CmdRsp_t
#include "basic_defs.h"         // bool
#include "umdevxs_bufadmin.h"   // BufAdmin_Record_t

#include <linux/kernel.h>       // printk
#include <linux/mm_types.h>     // vm_area_struct

// Logging API
#include "log.h"

/*----------------------------------------------------------------------------
 * DMABuf_HostAddress_t
 *
 * Buffer address that can be used by the host to access the buffer. This
 * address has been put in a data structure to make it type-safe.
 */
typedef struct
{
    void * p;
} DMABuf_HostAddress_t;


/*----------------------------------------------------------------------------
 * DMABuf_DevAddress_t
 *
 * Device (aka physical or bus) address associated with a DMA buffer. This
 * address has been put in a data structure to make it type-safe.
 */
typedef struct
{
    void * p;
} DMABuf_DevAddress_t;


/*----------------------------------------------------------------------------
 * DMABuf_Status_t
 *
 * Return values for all the API functions.
 */
typedef enum
{
    DMABUF_STATUS_OK,
    DMABUF_ERROR_BAD_ARGUMENT,
    DMABUF_ERROR_INVALID_HANDLE,
    DMABUF_ERROR_OUT_OF_MEMORY
} DMABuf_Status_t;


/*----------------------------------------------------------------------------
 * DMABuf_Properties_t
 *
 * Buffer properties. When allocating a buffer, these are the requested
 * properties for the buffer. When registering an externally allocated buffer,
 * these properties describe the buffer.
 *
 * For both uses, the data structure should be initialized to all zeros
 * before filling in some or all of the fields. This ensures forward
 * compatibility in case this structure is extended with more fields.
 *
 * Example usage:
 *     DMABuf_Properties_t Props = {0};
 *     Props.fIsCached = true;
 */
typedef struct
{
    uint32_t Size;       // size of the buffer
    uint8_t Alignment;   // buffer start address alignment, for example
    // 4 for 32bit
    uint8_t Bank;        // can be used to indicate on-chip memory
    bool fCached;        // true = SW needs to do coherency management
} DMABuf_Properties_t;

// DMA resource
#define UMDEVXS_DMARESOURCE_MAGIC 0xD71A65


// Handles
#define UMDEVXS_HANDLECLASS_DEVICE  1
// #define UMDEVXS_HANDLECLASS_DMABUF  2
#define UMDEVXS_HANDLECLASS_SMBUF   3

static inline int
UMDevXS_Handle_Make(
    int Class,
    int Index)
{
    if (Class < 0 || Class > 7)
    {
        return -1;
    }

    if (Index < 0 || Index > 0x0FFFFFFF)
    {
        return -1;
    }

    return Index * 8 + Class;
}


static inline int
UMDevXS_Handle_GetClass(
    int Handle)
{
    return (Handle & 7);
}

static inline int
UMDevXS_Handle_GetIndex(
    int Handle)
{
    return Handle / 8;
}


// PCI Device support
#ifndef UMDEVXS_REMOVE_PCI
int
UMDevXS_PCIDev_Init(void);

void
UMDevXS_PCIDev_UnInit(void);

void
UMDevXS_PCIDev_HandleCmd_Find(
    UMDevXS_CmdRsp_t * const CmdRsp_p,
    unsigned int BAR,
    unsigned int SubsetStart);

int
UMDevXS_PCIDev_Map(
    unsigned int BAR,
    unsigned int SubsetStart,       // defined
    unsigned int SubsetSize,        // defined
    unsigned int Length,            // requested
    struct vm_area_struct * vma_p);

#ifndef UMDEVXS_REMOVE_DEVICE_PCICFG
void
UMDevXS_PCIDev_HandleCmd_Read32(
    UMDevXS_CmdRsp_t * const CmdRsp_p);

void
UMDevXS_PCIDev_HandleCmd_Write32(
    UMDevXS_CmdRsp_t * const CmdRsp_p);
#endif // UMDEVXS_REMOVE_DEVICE_PCICFG
#endif

// OF device support
#ifndef UMDEVXS_REMOVE_DEVICE_OF
int
UMDevXS_OFDev_Init(void);

void
UMDevXS_OFDev_UnInit(void);

int
UMDevXS_OFDev_Map(
    unsigned int SubsetStart,
    unsigned int SubsetSize,
    unsigned int Length,
    struct vm_area_struct * vma_p);

void*
UMDevXS_OFDev_GetReference(void);
#endif // UMDEVXS_REMOVE_DEVICE_OF

// Character Device support
int
UMDevXS_ChrDev_Init(void);

void
UMDevXS_ChrDev_UnInit(void);


// Shared Memory support
#ifndef UMDEVXS_REMOVE_SMBUF
int
UMDevXS_SMBuf_Init(void);

void
UMDevXS_SMBuf_UnInit(void);

void
UMDevXS_SMBuf_HandleCmd(
    void * AppID,
    UMDevXS_CmdRsp_t * const CmdRsp_p);

int
UMDevXS_SMBuf_Map(
    int HandleIndex,
    unsigned int Length,
    struct vm_area_struct * vma_p);

void
UMDevXS_SMBuf_CleanUp(
    void * AppID);
#endif


// Simulation Device support
#ifndef UMDEVXS_REMOVE_SIMULATION
void
UMDevXS_SimDev_HandleCmd_Find(
    UMDevXS_CmdRsp_t * const CmdRsp_p,
    void * Reference_p,
    unsigned int LastByteOffset);

int
UMDevXS_SimDev_Map(
    void * Reference_p,
    unsigned long * RawAddr_p);
#endif


// Device support
#ifndef UMDEVXS_REMOVE_DEVICE
int
UMDevXS_Device_Find(
    const char * szName_p);

void
UMDevXS_Device_HandleCmd_Find(
    UMDevXS_CmdRsp_t * const CmdRsp_p);

void
UMDevXS_Device_HandleCmd_Enum(
    UMDevXS_CmdRsp_t * const CmdRsp_p);

int
UMDevXS_Device_Map(
    unsigned int DeviceNr,
    unsigned int Length,
    struct vm_area_struct * vma_p);
#endif


// Interrupts
void
UMDevXS_Interrupt_Init(
    const int nIRQ);

void
UMDevXS_Interrupt_UnInit(void);

int
UMDevXS_Interrupt_WaitWithTimeout(
    const unsigned int Timeout_ms);


#endif /* INCLUDE_GUARD_UMDEVXS_INTERNAL_H */

/* end of file umdevxs_internal.h */
