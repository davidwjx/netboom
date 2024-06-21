/****************************************************************************
 * 
 *   FileName: usb_ctrl_com.c
 *
 *   Author:
 *
 *   Date: 
 *
 *   Description: USB control com interface 
 *
 ****************************************************************************/
/****************************************************************************
 ***************       I N C L U D E    F I L E S             ***************
 ****************************************************************************/
#include "inu_types.h"
#include "err_defs.h"

#include "log.h"

#include "usb_ctrl_com.h"
#include "mem_map.h"
#include "cmem.h"
#include "helsinki.h"
#if DEFSG_IS_GP
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <termios.h>
#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 ***************       L O C A L    D E F N I T I O N S       ***************
 ****************************************************************************/
#define USB_NUM_INTERFACE 7
/****************************************************************************
 ***************            L O C A L    T Y P E D E F S      ***************
 ****************************************************************************/
typedef struct
{
  UINT8 buf[1024];
  UINT32 cnt;
  UINT32 idx;
} USB_CTRL_COMP_bufT;

typedef struct
{
   USB_CTRL_COMG_paramsT param;
   void* handle;
} USB_CTRL_COMP_interfaceInfT;

#define PG_CMD_WRITE _IOW('P', 1, int)
#define PG_CMD_READ  _IOR('P', 2, int)
#define PG_CMD_INTERRUPT  _IOR('P', 3, int)
#define PG_CMD_FRAME_AND_HEADER_SG_SEND _IOW('P', 4, int)

typedef struct {
   UINT32 physAddr;
   UINT32 size;
} USB_CTRL_COMP_ioctlArgT;
typedef struct {
   UINT8 * headerBuffer;            /*Header pointer */
   UINT32 headerSize;               /*Header size (Normally 172 bytes from testing)*/
   INT32 dmaBuf_fd;                /*Frame buffer file descriptor (Dma-Buf) , this is the DMA-Buf handle for the whole pool, we haven't managed to use because its for the complete memory pool but it may be useful to have!*/
   UINT8 * frameBuffer;             /*Virtual address of the frame buffer*/
   UINT32 frameBufferSize;         /*Size of the frame*/
   UINT32  physicalFrameBuffer;    /*Physical address of the frame buffer*/
} USB_CTRL_COMP_DMABuf_ioctlArgT;
/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/
static USB_CTRL_COMP_bufT remBuf;
USB_CTRL_COMP_interfaceInfT interfaceInfTbl[USB_NUM_INTERFACE];


/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/
/******************************************************************************
 ***************     P R E    D E F I N I T I O N     OF        ***************
 ***************     L O C A L         F U N C T I O N S        ***************
 ******************************************************************************/
static ERRG_codeE USB_CTRL_COMP_open(void **connHandle, void *openCfg);
static ERRG_codeE USB_CTRL_COMP_close(void *connHandle);
static ERRG_codeE USB_CTRL_COMP_send(void *connHandle, UINT8* pBuff, UINT32 len, UINT32 *byteLenSendP);
static ERRG_codeE USB_CTRL_COMP_sendv(void *connHandle,  void *bufP, UINT32 *bytesCntP);
static ERRG_codeE USB_CTRL_COMP_recv(void *connHandle, UINT8* pBuff, UINT32 len, UINT32 *byteLenRecvP);
//static ERRG_codeE USB_CTRL_COMP_peek(void *connHandle, UINT8* pBuff, UINT32 len, UINT32 *byteLenRecvP);

/****************************************************************************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/

/****************************************************************************
*
*  Function Name: USB_CTRL_COMP_open
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: 
*
****************************************************************************/
ERRG_codeE USB_CTRL_COMP_open(void **connHandle, void *openCfg)
{
   int totalSleep = 0;
   (void)connHandle;
   (void)openCfg;
   int fd;
   char str[80];
   USB_CTRL_COMG_paramsT *cfgP = (USB_CTRL_COMG_paramsT *)openCfg;
   LOGG_PRINT(LOG_DEBUG_E, NULL, "\n");
   sprintf (str, "/dev/pg%d",cfgP->usbInterfaceIdx);

   //retry, sometimes the Host is not ready yet, do not fail immidiently
   while(totalSleep < 5000)
   {
      fd = open (str, O_RDWR);
      if(fd < 0)
      {
         totalSleep += 5;
         OS_LYRG_usleep(5000);
      }
      else
      {
         break;
      }
   }
   if(fd < 0)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "device open failed\n");
      return USB_CTRL_COM__ERR_OPEN_FAILED;
   }

   LOGG_PRINT(LOG_INFO_E, NULL, "device opened  fd=%d\n",fd);

   memset(&remBuf,0,sizeof(remBuf));
   interfaceInfTbl[cfgP->usbInterfaceIdx].param = *cfgP;
   interfaceInfTbl[cfgP->usbInterfaceIdx].handle =(void *)fd;
   *connHandle = (void *)(&interfaceInfTbl[cfgP->usbInterfaceIdx]);
   return USB_CTRL_COM__RET_SUCCESS;

}

/****************************************************************************
*
*  Function Name: USB_CTRL_COMP_close
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: 
*
****************************************************************************/
// new compilers don't have stropts.h
#if HAVE_STROPTS_H
#include <stropts.h>
#endif

ERRG_codeE USB_CTRL_COMP_close(void *connHandle)
{
   int status;
   USB_CTRL_COMP_ioctlArgT arg;   
   int fd = (int)((USB_CTRL_COMP_interfaceInfT *)connHandle)->handle;
   LOGG_PRINT(LOG_DEBUG_E, NULL, "\n");

   if(fd >= 0)
   {
      printf("unblock try 1\n");
      ioctl(fd,PG_CMD_INTERRUPT,&arg);
      printf("unblock try 2\n");      
      status = close(fd);
      if(status < 0)
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "device close failed fd=%d\n", fd);
         return USB_CTRL_COM__ERR_CLOSE_FAILED;
      }
      LOGG_PRINT(LOG_INFO_E, NULL, "device closed fd=%d\n", fd);
   }
   
   return USB_CTRL_COM__RET_SUCCESS;
}

/**
 * @brief Transmits a Frame buffer and a header in a single URB using scatter gather 
 * 
 *
 * @param connHandle Connection handle
 * @param cmemfd DMA-Buf file descriptor
 * @param header header to transmit
 * @param headerSize Size of the header in bytes
 * @return Returns an error code
 */
ERRG_codeE USB_CTRL_COMP_sendDMABufWithHeader(void *connHandle, int frameBuffer_FD, UINT8 *header,UINT32 headerSize,
   UINT8 * framebuffer, UINT32 frameBufferSize, UINT32 frameBufferPhysical)
{
   USB_CTRL_COMP_DMABuf_ioctlArgT arg;
   ERRG_codeE  retVal = USB_CTRL_COM__RET_SUCCESS;
   int result = 0;
   int fd = (int)((USB_CTRL_COMP_interfaceInfT *)connHandle)->handle;
   int totalSize = frameBufferSize + headerSize;
   int remainder = totalSize %  URB_TRANSFER_SIZE  ;
   totalSize = totalSize ;

   if(fd < 0)
   {
      return USB_CTRL_COM__ERR_DEV_NOT_INIT;
   }
   /*Header buffer is used as the first scatter gather entry, dmaBuf_FD was supposed to be used for the whole memory pool but I found that this isn't possible
   because the CMEM DMA-Buf is for the whole memory pool and not just a frame buffer
   */
   arg.headerBuffer = header;
   arg.headerSize =  headerSize;
   arg.dmaBuf_fd = frameBuffer_FD; /* TODO: Use this instead of the frame buffer pointer */
   arg.frameBuffer = framebuffer;
   arg.frameBufferSize = frameBufferSize;   /*Make sure size is exactly a multiple of URB_TRANSFER_SIZE*/
   arg.physicalFrameBuffer =frameBufferPhysical;
      LOGG_PRINT(LOG_DEBUG_E,NULL,"Trying to send a header of %lu bytes with a frame of % lu bytes, totalSize %lu \n",headerSize,frameBufferSize,totalSize);
   if((result = ioctl (fd, PG_CMD_FRAME_AND_HEADER_SG_SEND, &arg))  != 0)
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "device send failed result=%d fd=%d\n", result, fd);
      retVal = USB_CTRL_COM__ERR_WRITE_FAILED;
   }
   return retVal;
}

/****************************************************************************
*
*  Function Name: USB_CTRL_COMP_send
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
ERRG_codeE USB_CTRL_COMP_sendPhy(void *connHandle, UINT8* bufP, UINT32 len, UINT32 *byteLenSendP)
{
   USB_CTRL_COMP_ioctlArgT arg;
   ERRG_codeE  retVal = USB_CTRL_COM__RET_SUCCESS;
   int result = 0;
   int fd = (int)((USB_CTRL_COMP_interfaceInfT *)connHandle)->handle;

   //LOGG_PRINT(LOG_DEBUG_E, NULL, "\n");
   //LOGG_PRINT(LOG_DEBUG_E, NULL, "device send fd=%d bufP=%x len=%d\n", fd, bufP,len);
   if(fd < 0)
   {
      return USB_CTRL_COM__ERR_DEV_NOT_INIT;
   }
   
   if (bufP) 
   {  
      arg.physAddr = (UINT32)bufP;
      arg.size = len;
      if((result = ioctl (fd, PG_CMD_WRITE, &arg))  != 0)
      {
         LOGG_PRINT(LOG_INFO_E, NULL, "device send failed result=%d fd=%d bufP=0x%x len=%d\n", result, fd, bufP, len);
         retVal = USB_CTRL_COM__ERR_WRITE_FAILED;
      }
      *byteLenSendP = len;
      return retVal;
   }

//   LOGG_PRINT(LOG_DEBUG_E, NULL, "-->done send %d\n", *byteLenSendP );
   return retVal;
}


/****************************************************************************
*
*  Function Name: USB_CTRL_COMP_send
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
ERRG_codeE USB_CTRL_COMP_send(void *connHandle, UINT8* bufP, UINT32 len, UINT32 *byteLenSendP)
{
   ERRG_codeE  retVal = USB_CTRL_COM__RET_SUCCESS;
   int result = 0;
   int fd = (int)((USB_CTRL_COMP_interfaceInfT *)connHandle)->handle;

   //LOGG_PRINT(LOG_DEBUG_E, NULL, "\n");
 //  LOGG_PRINT(LOG_DEBUG_E, NULL, "device send fd=%d bufP=%x len=%d\n", fd, bufP,len);

   if(fd < 0)
   {
      return USB_CTRL_COM__ERR_DEV_NOT_INIT;
   }    
 
   if((result = write (fd, bufP, len))  != (INT32)len)
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "device send failed result=%d fd=%d bufP=%p writeP=%p len=%d\n", result, fd, bufP, len);
      retVal = USB_CTRL_COM__ERR_WRITE_FAILED;
    }
   *byteLenSendP = result;

//   LOGG_PRINT(LOG_DEBUG_E, NULL, "-->done send %d\n", *byteLenSendP );
   return retVal;
}
/**
 * @brief This is a version of USB_CTRL_COMP_sendv that reduces the amount of URB messages by scatter gather DMAing the header and the frame
 * 
 * @param connHandle Connection handle
 * @param bufP Buffer
 * @param bytesCntP Bytes transmitted
 * @return ERRG_codeE Returns an error code
 */
ERRG_codeE USB_CTRL_COMP_sendvWithURBOptimization(void *connHandle,  void *bufP, UINT32 *bytesCntP)
{
   USB_CTRL_COMP_interfaceInfT *pInf = (USB_CTRL_COMP_interfaceInfT *)connHandle;
   UINT32 byteCnt = 0;
   UINT32 byteCntTotal = 0;
   UINT32 phyAddr;
   ERRG_codeE ret = USB_CTRL_COM__ERR_INVALID_ARGS;
   MEM_POOLG_bufDescT *currP = (MEM_POOLG_bufDescT *)bufP;

   UINT8 *header = NULL;
   UINT8 header_size = 0; 
   
   //This is not a naive implementation and uses scatter gather!!
   unsigned int totalSize = 0;
   int numberTransfers = 0;
   {
      /*Temporary pointer used to check the number of transfers and the total size*/
      MEM_POOLG_bufDescT *currPTemp = (MEM_POOLG_bufDescT *)bufP;
      while(currPTemp)
      {
         totalSize = totalSize+currPTemp->dataLen;
         currPTemp = currPTemp->nextP;
         numberTransfers++;
      }
   }
   if(totalSize != pInf->param.unBufferedReadSize)
   {
      LOGG_PRINT(LOG_ERROR_E,NULL,"URB optimization not possible for this style of sending due to size mismatch %lu,%lu \n", totalSize, pInf->param.unBufferedReadSize);
      return USB_CTRL_COM__ERR_WRITE_FAILED;
   }
   if(numberTransfers != 2)
   {
      LOGG_PRINT(LOG_ERROR_E,NULL,"URB optimization not possible for this style of sending %lu,%lu \n", numberTransfers, totalSize);
      return USB_CTRL_COM__ERR_WRITE_FAILED;
   }
   ret = MEM_POOLG_getDataPhyAddr(currP,&phyAddr);
   if (ERRG_SUCCEEDED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E,NULL,"URB optimization not possible for this style of sending, the header must be in virtual memory \n");
      return USB_CTRL_COM__ERR_WRITE_FAILED;
   }
   /* The header has been located correctly*/
   header = currP->dataP;
   header_size = currP->dataLen;
   /*Now we check that the frame buffer is located correctly and then try and send it together with the header*/
   currP = currP->nextP;
   int frame_bufferSize = currP->dataLen; /*Size of the frame buffer*/
   ret = MEM_POOLG_getDataPhyAddr(currP,&phyAddr);
   if (ERRG_SUCCEEDED(ret))
   {
      if(1)
      {
         LOGG_PRINT(LOG_DEBUG_E,NULL,"Sending DMA-Buf File descriptor %d with a header %p of size %lu, phy Addr %x\n",currP->fileDescriptor,header,header_size, phyAddr );
         ret = USB_CTRL_COMP_sendDMABufWithHeader(connHandle,currP->fileDescriptor,header,header_size,currP->dataP,currP->dataLen,phyAddr);
         byteCntTotal = header_size + frame_bufferSize;  /*Calculate the total bytes transmitted */
      }
      else
      {
         LOGG_PRINT(LOG_ERROR_E,NULL,"URB optimization not possible for this style of sending, the frame buffer must have a DMA-Buf file descriptor\n");
         return USB_CTRL_COM__ERR_WRITE_FAILED;
      }
   }
   else
   {
      LOGG_PRINT(LOG_ERROR_E,NULL,"URB optimization not possible for this style of sending, the frame buffer must have a known physical address\n");
      return USB_CTRL_COM__ERR_WRITE_FAILED;
   }
   
   
   *bytesCntP = byteCntTotal;   
   LOGG_PRINT(LOG_DEBUG_E,NULL,"Sending  %lu \n",byteCntTotal );
   return ret;
}


ERRG_codeE USB_CTRL_COMP_sendv(void *connHandle,  void *bufP, UINT32 *bytesCntP)
{
   UINT32 byteCnt = 0;
   UINT32 byteCntTotal = 0;
   UINT32 phyAddr;
   ERRG_codeE ret = USB_CTRL_COM__ERR_INVALID_ARGS;
   MEM_POOLG_bufDescT *currP = (MEM_POOLG_bufDescT *)bufP;
   
   //This is a naive implementation.
   //TODO: use scatter-gather mechanism when supported by driver
   while(currP)
   {
      byteCnt = 0;
      ret = MEM_POOLG_getDataPhyAddr(currP,&phyAddr);
      LOGG_PRINT(LOG_DEBUG_E,NULL,"Sending image from %p, with length %lu \n", currP->dataP, currP->dataLen);
      if (ERRG_SUCCEEDED(ret))
      {
         ret = USB_CTRL_COMP_sendPhy(connHandle, (UINT8*)phyAddr, currP->dataLen, &byteCnt);
      }
      else
      {
         ret = USB_CTRL_COMP_send(connHandle, currP->dataP, currP->dataLen, &byteCnt);
      }
      if(ERRG_FAILED(ret))
      {
         break;
      }
      byteCntTotal += byteCnt;
      currP = currP->nextP;
   }
   
  *bytesCntP = byteCntTotal;   
   LOGG_PRINT(LOG_DEBUG_E,NULL,"Sending  %lu \n",byteCntTotal );
   return ret;
}


/****************************************************************************
*
*  Function Name: USB_CTRL_COMP_recv
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
ERRG_codeE USB_CTRL_COMP_recv(void *connHandle, UINT8* bufP, UINT32 len, UINT32 *byteLenRecvP)
{
   enum {CACHE_INV_SIZE = 256};
   ERRG_codeE  retVal = USB_CTRL_COM__RET_SUCCESS;
   int fd =(int)((USB_CTRL_COMP_interfaceInfT *)connHandle)->handle;
   int result = 0;
   UINT8 *readP;
   UINT32 rem;
   UINT32 lenToRead;
   UINT32 remReadCnt = 0;
 //  LOGG_PRINT(LOG_DEBUG_E, NULL, "device recv fd=%d bufP=%x len=%d\n", fd, bufP,len);
   if(fd < 0)
      return USB_CTRL_COM__ERR_DEV_NOT_INIT;

   //Limitation of USB driver: reads in 1KB chunks on USB. This means we could overflow user buffer by 1KB
   //when reading non-multiples of 1KB.
   //So read up to the highest 1KB multiple, then read remainder into internal static 1KB buffer and copy only requested
   //data to user buffer. Read from internal buffer on next read.

   //Read from internal buffer first
   if(remBuf.cnt > 0)
   {
 //     LOGG_PRINT(LOG_DEBUG_E, NULL, "usb com read from internal cnt=%d at %d\n", remBuf.cnt, remBuf.idx);

      //Internal buffer contains data
      lenToRead = (len < remBuf.cnt) ?  len : remBuf.cnt;
      memcpy(bufP, &remBuf.buf[remBuf.idx], lenToRead);
      remBuf.cnt -= lenToRead;
      remBuf.idx += lenToRead;

      len -= lenToRead;
      bufP += lenToRead;
      if(len == 0)
      {
         *byteLenRecvP = lenToRead;
 //        LOGG_PRINT(LOG_DEBUG_E, NULL, "done (%d)\n", lenToRead);
         return USB_CTRL_COM__RET_SUCCESS;
      }
      remReadCnt = lenToRead;
   }
   //At this point internal buffer should be empty.
   assert_func((remBuf.cnt == 0), "usb com assert failed (remBuf.cnt == 0)\n");
   remBuf.idx = 0;

   readP = bufP;

   rem = len % 1024;
   lenToRead = len-rem;
   if(lenToRead)
   {
      LOGG_PRINT(LOG_DEBUG_E, NULL, "usb com reading readP=%p len=%d(%d)\n", readP,lenToRead,len);
      result = read(fd, readP, lenToRead);
      if(result < 0)
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "device read failed result=%d fd=%d bufP=%p(%p) len=%d(%d)\n", result, fd, bufP, readP, lenToRead,len);
         retVal = USB_CTRL_COM__ERR_READ_FAILED;
         return retVal;
      }
   }

   if(rem)
   {
 //     LOGG_PRINT(LOG_DEBUG_E, NULL, "usb com reading remainder  readP=%p len=%d,%d(%d)\n", readP+lenToRead, lenToRead, rem,len);
      result = read(fd, remBuf.buf, rem);
      if(result < 0)
      {
         LOGG_PRINT( LOG_ERROR_E, NULL, "device read remainder failed result=%d fd=%d bufP=%p(%p) len=%d(%d)\n", result, fd, bufP, readP, rem,len);
         retVal = USB_CTRL_COM__ERR_READ_FAILED;
         return retVal;
      }

      memcpy(bufP+lenToRead, remBuf.buf, rem);
      if((unsigned int)result > rem)
      {
         remBuf.cnt = (result-rem);
         remBuf.idx = rem;
      }
   }
   *byteLenRecvP = remReadCnt+len;

//   LOGG_PRINT(LOG_DEBUG_E, NULL, "done read=%d (extra=%d idx=%d)\n", *byteLenRecvP, remBuf.cnt,remBuf.idx);
   return retVal;
}



/****************************************************************************
*
*  Function Name: USB_CTRL_COMP_recvPhy
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
ERRG_codeE USB_CTRL_COMP_recvDma(void *connHandle, UINT8* bufPhyP, UINT8* bufP, UINT32 len, UINT32 *byteLenRecvP)
{
   USB_CTRL_COMP_ioctlArgT arg;
   enum {CACHE_INV_SIZE = 256};
   ERRG_codeE  retVal = USB_CTRL_COM__RET_SUCCESS;
   int fd =(int)((USB_CTRL_COMP_interfaceInfT *)connHandle)->handle;
   int result = 0;
   UINT8 *readP;
   UINT32 physAddr = 0;
   UINT32 rem;
   UINT32 lenToRead;
   UINT32 remReadCnt = 0;
//printf("USB_CTRL_COMP_recvDma: addrPhy = 0x%x, addrVir = 0x%x, len = %d, rem = %d\n",bufPhyP,bufP,len,remBuf.cnt);
 //  LOGG_PRINT(LOG_DEBUG_E, NULL, "device recv fd=%d bufP=%x len=%d\n", fd, bufP,len);
   if(fd < 0)
      return USB_CTRL_COM__ERR_DEV_NOT_INIT;

   //Limitation of USB driver: reads in 1KB chunks on USB. This means we could overflow user buffer by 1KB
   //when reading non-multiples of 1KB.
   //So read up to the highest 1KB multiple, then read remainder into internal static 1KB buffer and copy only requested
   //data to user buffer. Read from internal buffer on next read.
   
   physAddr = (UINT32)bufPhyP;

   //Read from internal buffer first
   if(remBuf.cnt > 0)
   {
 //     LOGG_PRINT(LOG_DEBUG_E, NULL, "usb com read from internal cnt=%d at %d\n", remBuf.cnt, remBuf.idx);

      //Internal buffer contains data
      lenToRead = (len < remBuf.cnt) ?  len : remBuf.cnt;
      memcpy(bufP, &remBuf.buf[remBuf.idx], lenToRead);
      CMEM_cacheWb((void *)(bufP), lenToRead);
      remBuf.cnt -= lenToRead;
      remBuf.idx += lenToRead;

      len -= lenToRead;
      bufP += lenToRead;
      physAddr += lenToRead;

      if(len == 0)
      {
         *byteLenRecvP = lenToRead;
 //        LOGG_PRINT(LOG_DEBUG_E, NULL, "done (%d)\n", lenToRead);
         return USB_CTRL_COM__RET_SUCCESS;
      }
      remReadCnt = lenToRead;
   }
   //At this point internal buffer should be empty.
   assert_func((remBuf.cnt == 0), "usb com assert failed (remBuf.cnt == 0)\n");
   remBuf.idx = 0;

   readP = (UINT8 *)physAddr;

   rem = len % 1024;
   lenToRead = len-rem;
   if(lenToRead)
   {
      LOGG_PRINT(LOG_DEBUG_E, NULL, "usb com reading readP=%p len=%d(%d)\n", readP,lenToRead,len);
      arg.physAddr = physAddr;
      arg.size = lenToRead;
      result = ioctl (fd, PG_CMD_READ, &arg);
      if(result < 0)
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "device read failed result=%d fd=%d bufP=%p(%p) len=%d(%d)\n", result, fd, bufP, readP, lenToRead,len);
         retVal = USB_CTRL_COM__ERR_READ_FAILED;
         return retVal;
      }
      //Workaround for caching coherency issue with cde buffer memory
      CMEM_cacheInv((void *)bufP, ((lenToRead < CACHE_INV_SIZE) ? lenToRead : CACHE_INV_SIZE));
   }

   if(rem)
   {
 //     LOGG_PRINT(LOG_DEBUG_E, NULL, "usb com reading remainder  readP=%p len=%d,%d(%d)\n", readP+lenToRead, lenToRead, rem,len);
      result = read(fd, remBuf.buf, rem);
      if(result < 0)
      {
         LOGG_PRINT( LOG_ERROR_E, NULL, "device read remainder failed result=%d fd=%d bufP=%p(%p) len=%d(%d)\n", result, fd, bufP, readP, rem,len);
         retVal = USB_CTRL_COM__ERR_READ_FAILED;
         return retVal;
      }

      memcpy(bufP+lenToRead, remBuf.buf, rem);
      // write back reminder data from cache to DDR for DMA usage
      CMEM_cacheWb((void *)(bufP+lenToRead), rem);
      if((unsigned int)result > rem)
      {
         remBuf.cnt = (result-rem);
         remBuf.idx = rem;
      }
   }
   *byteLenRecvP = remReadCnt+len;

//   LOGG_PRINT(LOG_DEBUG_E, NULL, "done read=%d (extra=%d idx=%d)\n", *byteLenRecvP, remBuf.cnt,remBuf.idx);
   return retVal;
}

void USB_CTRL_COMP_getCaps(void *connHandle, CLS_COMG_capsT *capsP)
{
   USB_CTRL_COMP_interfaceInfT *pInf = (USB_CTRL_COMP_interfaceInfT *)connHandle;
   capsP->flags  =  0;
   capsP->fixedSize = 0;  
   if (pInf->param.rxCap)
   {
      capsP->flags |= CLS_COMG_CAP_RX;
   }
   if (pInf->param.txCap)
   {
      capsP->flags |= CLS_COMG_CAP_TX;
   }
   if (!pInf->param.fixedSizeCap)
   {
      capsP->flags |= CLS_COMG_CAP_MODE_STREAM;
   }
   if(pInf->param.unBufferedReadCap && !pInf->param.fixedSizeCap )
   {
      capsP->flags  |= CLS_COMG_CAP_MODE_UNBUFFERED_SEND_V;
      capsP->unbufferedFixedSize = pInf->param.unBufferedReadSize ;
   }
   else if(pInf->param.fixedSizeCap)
   {
      capsP->fixedSize = pInf->param.fixedBuffSize; 
   }
   capsP->flags |= (CLS_COMG_CAP_SENDV | CLS_COMG_CAP_RX_CONT_RECV);
}

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/

/****************************************************************************
*
*  Function Name: USB_CTRL_COMG_getOps
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: 
*
****************************************************************************/
void USB_CTRL_COMG_getOps(CLS_COMG_ifOperations *ifOpsP)
{
   //OpsP->init     = &USB_CTRL_COMP_init;
   ifOpsP->open        = &USB_CTRL_COMP_open;
   ifOpsP->close       = &USB_CTRL_COMP_close;
   ifOpsP->getCaps     = &USB_CTRL_COMP_getCaps;
   ifOpsP->send        = &USB_CTRL_COMP_send;
   ifOpsP->sendv       = &USB_CTRL_COMP_sendv;
   ifOpsP->recv        = &USB_CTRL_COMP_recv;
   ifOpsP->recvContBuf = &USB_CTRL_COMP_recvDma;
   ifOpsP->peek        = NULL;
   ifOpsP->recv_unbuffered = NULL;
   ifOpsP->sendv_unbuffered = &USB_CTRL_COMP_sendvWithURBOptimization;
}

#ifdef __cplusplus
}
#endif
#endif
