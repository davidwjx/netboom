/****************************************************************************
 *
 *   FileName: app_init.h
 *
 *   Author:
 *
 *   Date:
 *
 *   Description:
 *
 ****************************************************************************/
#ifndef APP_INIT_H
#define APP_INIT_H

#ifdef __cplusplus
      extern "C" {
#endif

/****************************************************************************
***************      I N C L U D E   F I L E S                 **************
****************************************************************************/
#include "io_pal.h"
#include "inu2.h"
#include "inu2_types.h"

/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ***************************************************************************/
 
/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/
typedef void (*APP_INITG_initCbT)(void *arg);

/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
****************************************************************************/

/****************************************************************************
 ***************      E X T E R N A L       D A T A           ***************
****************************************************************************/

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
****************************************************************************/
ERRG_codeE APP_INITG_dispatch_init();
ERRG_codeE APP_INITG_registerInitCb(APP_INITG_initCbT cb, void *arg);
ERRG_codeE APP_INITG_init(int      wdTimeout,
                              UINT32   clientDdrBufSize,
                              UINT32   *clientDdrBufPhyAddrP,
                              UINT32   *clientDdrBufVirtAddrP,
                              inu_device_standalone_mode_e standAloneMode,
                              inu_deviceH   *devicePtr,
                              UINT32 interface_select,
                              UINT32 port);
ERRG_codeE APP_INITG_close(inu_deviceH deviceH);
void       APP_INITG_statsMonitor( inu_deviceH deviceH );

#ifdef __cplusplus
}
#endif

#endif //	APP_INIT_H
