
#ifndef __CDC_MNGR_H__
#define __CDC_MNGR_H__

#ifdef __cplusplus
      extern "C" {
#endif

#include "inu_defs.h"
#include "ve_mngr.h"
#include "inu_fw_update_api.h"

#define CDC_MNGRG_DEFAULT_UART_BAUDRATE         (921600)

typedef void* CDC_MNGRG_handle;
typedef void (*CDC_MNGRG_logCallback)(const char* str);

ERRG_codeE CDC_MNGRG_init(void);
ERRG_codeE CDC_MNGRG_deinit(void);
ERRG_codeE CDC_MNGRG_open(CDC_MNGRG_handle* handle, UINT32 port, UINT32 baudrate);
ERRG_codeE CDC_MNGRG_close(CDC_MNGRG_handle handle);
ERRG_codeE CDC_MNGRG_log(const char *str);

#if DEFSG_IS_HOST
ERRG_codeE CDC_MNGRG_ping(CDC_MNGRG_handle handle);
ERRG_codeE CDC_MNGRG_logEnable(CDC_MNGRG_handle handle, BOOLEAN enable, CDC_MNGRG_logCallback logCallback);
ERRG_codeE CDC_MNGRG_swReset(CDC_MNGRG_handle handle);

ERRG_codeE CDC_MNGRG_enableBokeh(CDC_MNGRG_handle handle, VE_MNGRG_bokehParamsT* bokehParams);
ERRG_codeE CDC_MNGRG_disableBokeh(CDC_MNGRG_handle handle, VE_MNGRG_disableParamsT* stopParams);
ERRG_codeE CDC_MNGRG_setBokehParams(CDC_MNGRG_handle handle, VE_MNGRG_bokehParamsT* bokehParams);
ERRG_codeE CDC_MNGRG_getBokehParams(CDC_MNGRG_handle handle, VE_MNGRG_bokehParamsT* bokehParams);
ERRG_codeE CDC_MNGRG_sendVeCmd(VE_MNGRG_messageT* veMsg, UINT8* output, UINT32 outputLen);

ERRG_codeE CDC_MNGRG_sendFwUpdateCmd(INU_FW_UPDATEG_messageT* fWUpdateCmd, UINT8* output, UINT32 outputLen);

ERRG_codeE CDC_MNGRG_initVeMngr(CDC_MNGRG_handle handle);
ERRG_codeE CDC_MNGRG_startStream(CDC_MNGRG_handle handle);
ERRG_codeE CDC_MNGRG_stopStream(CDC_MNGRG_handle handle);
ERRG_codeE CDC_MNGRG_test(CDC_MNGRG_handle handle);
#endif

#ifdef __cplusplus
}
#endif

#endif //__CDC_MNGR_H__

