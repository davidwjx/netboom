#pragma once
#include "err_defs.h"
#include <vsi3a/vsi3a_types.h>
#ifdef __cplusplus
extern "C" {
#endif
#define HELSINKI_SYNC_PERIOD 1
#define HELSINKI_UART_PERIOD 6 /*Send over UART every 5 frames to reduce NU4K CPU load*/
ERRG_codeE HELSINKI_ISP_Synchronizer();
void HELSINKI_ISP_Sync_Init();
void HELSINKI_notifyISPSyncThread(UINT32 frameID, UINT64 timestamp_ns);
void HELSINKI_ISP_SYNCHRONIZER_set_AWBUartSyncState(bool awbSync);
void HELSINKI_ISP_SYNCHRONIZER_set_AEUartSyncState(bool aeSync);
int HELSINKI_ISP_SYNCHRONIZER_AWB_callback(struct Vsi3AMetaWb *wb_ptr);
int HELSINKI_ISP_SYNCHRONIZER_AE_callback(struct Vsi3AMetaExp *wb_ptr);
#ifdef __cplusplus
}
#endif
