/****************************************************************************
 *
 *   FileName: conn_lyr.h
 *
 *   Author: Danny Bibi. Ram B.
 *
 *   Date: 
 *
 *   Description: CLS (COM Layer Stack) API 
 *   
 ****************************************************************************/
#ifndef _CONN_LYR_H_
#define _CONN_LYR_H_

/****************************************************************************
 ***************      I N C L U D E   F I L E S                 *************
 ****************************************************************************/
#include "cls_com.h"

/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

#define CONN_LYRG_TXID_IGNORE  ((UINT32)~0)

/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/
typedef struct
{
	UINT64 control;
} CONN_LYRG_clsHdrT;

typedef void (*CONN_LYRG_rxCbT)(void *argP, INT32 refId, void *bufferP);
typedef void (*CONN_LYRG_txCbT)(void *argP, INT32 refId, void *bufferP);
typedef void (*CONN_LYRG_ifOpsFunc)(CLS_COMG_ifOperations *ifOpsP);
typedef ERRG_codeE(*CONN_LYRG_timedAllocBufCbT)(void *me, UINT32 refId, UINT32 len, UINT32 timeoutMsec, void **bufDescP);

typedef struct
{
   void *ifCfg; //Interface specific 
   CLS_COMG_getIfOps getIfOps;
   UINT16 queueSize;
   void               *commP;
   CONN_LYRG_timedAllocBufCbT timedAllocBufCb;
} CONN_LYRG_openCfgT;

typedef struct
{
   void *ifCfg; //Interface specific 
   
} CONN_LYRG_cfgT;

typedef enum
{
   CONN_LYRG_TX_STATS_E = 1,
   CONN_LYRG_RX_STATS_E = 2,
   CONN_LYRG_ALL_STATS_E = 3
} CONN_LYRG_showModeE;

typedef void *CON_LYRG_handleT;

/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/
ERRG_codeE  CONN_LYRG_init(void);
ERRG_codeE  CONN_LYRG_open(CON_LYRG_handleT *handleP, CONN_LYRG_openCfgT *openCfgP, UINT32 id);
ERRG_codeE  CONN_LYRG_config(CON_LYRG_handleT handle, CONN_LYRG_cfgT *cfgP);
ERRG_codeE  CONN_LYRG_send(CON_LYRG_handleT handle, INT32 channelNum, void  *bufferP, UINT32 txid, UINT32 msgCode);
void        CONN_LYRG_rxRegister(CON_LYRG_handleT handle, CONN_LYRG_rxCbT func, void *arg);
void        CONN_LYRG_txRegister(CON_LYRG_handleT handle, CONN_LYRG_txCbT func, void *arg);
void        CONN_LYRG_chainHdrMode(UINT16 chainHdr);
void        CONN_LYRG_close(CON_LYRG_handleT handle);
void        CONN_LYRG_deinit(void);
UINT32      CONN_LYRG_getHdrMarshalSize(void);
void        CONN_LYRG_showStats(CON_LYRG_handleT handle, CONN_LYRG_showModeE mode);

#ifdef __cplusplus
}
#endif

#endif /* _CONN_LYR_H_*/
