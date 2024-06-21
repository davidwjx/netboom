/****************************************************************************
 *
 *   FileName: marshal_.h
 *
 *   Author:  Ram B.
 *
 *   Date: 
 *
 *   Description: Marshalling API 
 *   
 ****************************************************************************/
#ifndef MARHSAL_H
#define MARHSAL_H

#include "inu_cmd.h"
#include "internal_cmd.h"

#ifdef __cplusplus
extern "C" {
#endif
/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/
#define  MARSHALG_DO             (1)
#define  MARSHALG_UNDO           (0)

/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/

/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/
void MARSHALG_init();
void MARSHALG_deInit();
void MARSHALG_u16(INT16 dir, UINT16 *dataP, UINT8 *bufP);
void MARSHALG_u32(INT16 dir, UINT32 *dataP, UINT8 *bufP);
void MARSHALG_u64(INT16 dir, UINT64 *dataP, UINT8 *bufP);
void MARSHALG_convertIoctl(INT16 dir, INUG_ioctlCmdE cmd, UINT8* pStIoctl, UINT8* pBufOut, UINT32* pLen);
UINT32 MARSHALG_getSize(INUG_ioctlCmdE cmd);
UINT32 MARSHALG_getMarshalSize(INUG_ioctlCmdE cmd);
UINT32 MARSHALG_getMaxSize(INUG_serviceIdE sid);
UINT32 MARSHALG_getMaxMarshalSize(INUG_serviceIdE sid);
UINT32 MARSHALG_getTblHash(void);
void MARSHALG_showIoctlTbl(int flat);
void MARSHALG_showMaxSizes(void);
void MARSHALG_showTbl(int flat);

#ifdef __cplusplus
}
#endif
#endif //MARHSAL_H


