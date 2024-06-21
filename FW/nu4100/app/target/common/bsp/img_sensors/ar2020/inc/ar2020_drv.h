/****************************************************************************
 *
 *   FileName: AR430_drv.h
 *
 *   Author: Benny V.
 *
 *   Date:
 *
 *   Description: AR430 layer
 *
 ****************************************************************************/
#ifndef AR2020_DRV_H
#define AR2020_DRV_H

#include "inu_common.h"

#ifdef __cplusplus
      extern "C" {
#endif


/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/
ERRG_codeE  AR2020_DRVG_init(IO_PALG_apiCommandT *palP);
ERRG_codeE  AR2021_DRVG_init(IO_PALG_apiCommandT *palP);

#ifdef __cplusplus
}
#endif


#endif //AR430_DRV_H
