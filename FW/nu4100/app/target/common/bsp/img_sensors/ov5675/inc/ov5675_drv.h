/****************************************************************************
 *
 *   FileName: OV5675_drv.h
 *
 *   Author: Benny V.
 *
 *   Date: 
 *
 *   Description: OV5675 layer
 *
 ****************************************************************************/
#ifndef OV5675_DRV_H
#define OV5675_DRV_H

#include "inu_common.h"

#ifdef __cplusplus
      extern "C" {
#endif


/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/
ERRG_codeE  OV5675_DRVG_init(IO_PALG_apiCommandT *palP);

#ifdef __cplusplus
}
#endif


#endif //OV5675_DRV_H
