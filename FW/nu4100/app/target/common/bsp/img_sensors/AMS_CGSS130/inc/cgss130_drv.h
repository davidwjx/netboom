/****************************************************************************
 *
 *   FileName: CGSS130_drv.h
 *
 *   Author: Elad R.
 *
 *   Date: 
 *
 *   Description: CGSS130 layer
 *
 ****************************************************************************/
#ifndef CGSS130_DRV_H
#define CGSS130_DRV_H

#include "inu_common.h"

#ifdef __cplusplus
      extern "C" {
#endif


/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/
ERRG_codeE  CGSS130_DRVG_init(IO_PALG_apiCommandT *palP);

#ifdef __cplusplus
}
#endif


#endif //CGSS130_DRV_H
