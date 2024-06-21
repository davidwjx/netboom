/****************************************************************************
 *
 *   FileName: client_api_int.h
 *
 *   Author:  Benny V.
 *
 *   Date: 
 *
 *   Description: API for client process
 *
 ****************************************************************************/
#ifndef CLIENT_API_INT_H
#define CLIENT_API_INT_H

#ifdef __cplusplus
extern "C" {
#endif
/****************************************************************************
 ***************      I N C L U D E   F I L E S                **************
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
#if DEFSG_IS_GP
ERRG_codeE CLIENT_API_INTG_gpChanStart(INU_DEFSG_sysChannelIdE channelId);
ERRG_codeE CLIENT_API_INTG_gpChanStop(INU_DEFSG_sysChannelIdE channelId);
ERRG_codeE CLIENT_API_INTG_gpFromHostMsgRcv(UINT8 *dataP, UINT32 dataLen);
ERRG_codeE CLIENT_API_INTG_gpFromHwDmaDoneRcv(void *dmaCbParamsP, void *arg);
void       CLIENT_API_INTG_gpDmaI2SCb(void *currBufDescP, UINT64 timestamp);
ERRG_codeE CLIENT_API_INTG_gpFromHwIMURcv(INU_CMDG_posSensorsClientInfoT imuCbParamsP);
#else
#endif

#endif // CLIENT_API_INT_H



