/****************************************************************************
 *
 *   FileName: i2s.h
 *
 *   Author: Arnon C.
 *
 *   Date: 
 *
 *   Description: driver to I2S
 *   
 ****************************************************************************/
#ifndef I2S_H
#define I2S_H


#ifdef __cplusplus
      extern "C" {
#endif

typedef enum
{
   I2SP_MODULE_MASTER_MODE_E = 0,
   I2SP_MODULE_SLAVE_MODE_E  = 1,
} I2SG_modeE;

extern   ERRG_codeE I2SG_getDmaDoneBufDesc(UINT32 currentBufferIndex, MEM_POOLG_bufDescT **bufDescP);
extern	ERRG_codeE I2SG_open(UINT32 i2sRegistersBaseAddr, I2SG_modeE mode);
extern 	ERRG_codeE I2SG_init();


#ifdef __cplusplus
}
#endif

#endif //   I2S_H

