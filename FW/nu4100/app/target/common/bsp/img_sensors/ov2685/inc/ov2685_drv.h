/****************************************************************************
 *
 *   FileName: ov2685_drv.h
 *
 *   Author: Arnon C.
 *
 *   Date: 
 *
 *   Description: OV2685 layer
 *
 ****************************************************************************/
#ifndef OV2685_DRV_H
#define OV2685_DRV_H

#include "inu_common.h"

#ifdef __cplusplus
      extern "C" {
#endif


/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/
ERRG_codeE  OV2685_DRVG_init(IO_PALG_apiCommandT *palP);

#ifdef __cplusplus
}
#endif


#endif //OV2685_DRV_H
