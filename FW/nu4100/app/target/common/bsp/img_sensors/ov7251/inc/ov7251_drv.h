/****************************************************************************
 *
 *   FileName: ov7251_drv.h
 *
 *   Author: Benny V.
 *
 *   Date: 
 *
 *   Description: OV7251 layer
 *
 ****************************************************************************/
#ifndef OV7251_DRV_H
#define OV7251_DRV_H

#include "inu_common.h"

#ifdef __cplusplus
      extern "C" {
#endif


/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/
ERRG_codeE  OV7251_DRVG_init(IO_PALG_apiCommandT *palP);

#ifdef __cplusplus
}
#endif


#endif //OV7251_DRV_H
