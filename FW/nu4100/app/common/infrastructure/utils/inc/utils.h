/****************************************************************************
 *
 * FileName: utils.h
 *
 * Author: Benny V
 *
 * Date: 04/07/13
 *
 * Description: general purpose utilities functions
 *
 ****************************************************************************/
 
#ifndef UTILS_H
#define UTILS_H

#ifdef __cplusplus
      extern "C" {
#endif

#define UTILSG_CRC_TABLE_SIZE         256

/****************************************************************************
 ***************      I N C L U D E   F I L E S                 *************
 ****************************************************************************/

/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/

/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/

/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/
// memory functions
void     UTILSG_initBufferVal(UINT8* bufPtr, UINT32 bufSize, UINT8 val);
void     UTILSG_initBufferCounter(UINT16* bufPtr, UINT32 bufSize, UINT16 initVal, UINT32 wrapAround);
void     UTILSG_initBufferSpecialPatten(UINT16* bufPtr, UINT32 width, UINT32 height);
UINT32   UTILSG_compBufToVal(UINT8* bufPtr, UINT32 bufSize, UINT8 val);
UINT32   UTILSG_compBufToCounter(UINT16* bufPtr, UINT32 bufSize, UINT16 initVal, UINT32 wrapAround, UINT32 *failurePixelP);
UINT32   UTILSG_compBuffers(UINT8* buf1Ptr, UINT8* buf2Ptr, UINT32 bufSize);
void     UTILSG_printBufValues(UINT16* bufPtr, UINT32 bufSize);
void     UTILSG_dump_hex( UINT8 *buf,int len);
void     UTILSG_crcInit(UINT8 *crcTableP);
UINT8    UTILSG_calcCrc(UINT8 const message[], int nBytes, UINT8 *crcTableP);
#if DEFSG_IS_GP
void     UTILSG_print_trace (void);
#endif
// IO functions
void UTILSG_writeToFile(UINT8* bufPtr, UINT32 bufSize, char fileName[]);

#ifdef __cplusplus
}
#endif

#endif // UTILS_H
