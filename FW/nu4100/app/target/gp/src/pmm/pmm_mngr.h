
/****************************************************************************
 *
 *   FileName: pmm_mngr.h
 *
 *   Author:  Eli G.
 *
 *   Date:
 *
 *   Description: DSP Power Manager
 *
 ****************************************************************************/

#ifndef __PMM_MNGR_H__
#define __PMM_MNGR_H__

#include "inu2.h"

ERRG_codeE pmm_mngr__dspPmmActivate(UINT32 dspTarget);
ERRG_codeE pmm_mngr__dspPmmDeactivate(UINT32 dspTarget);

#endif //__PMM_MNGR_H__