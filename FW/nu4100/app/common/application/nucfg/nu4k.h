#ifndef __NU4K_H__
#define __NU4K_H__

#include "nucfg_priv.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void *nu4kH;
//nu4k API 
extern ERRG_codeE nu4k_init(XMLDB_dbH db, nu4kH *);
extern void nu4k_deInit(nu4kH);
extern unsigned int nu4k_getNumOutputs(nu4kH);
extern nublkH nu4k_getOutput(nu4kH, unsigned int inst);
extern unsigned int nu4k_getNumInputs(nu4kH);
extern nublkH nu4k_getInput(nu4kH, unsigned int inst);
extern void nu4k_getBlkTypeInst(nu4kH, nublkH blk, nuBlkTypeE *type, unsigned int *inst);
extern NUCFG_hwId nu4k_getBlkHwName(nu4kH,nublkH blk);
extern void nu4k_getChHwId(nu4kH h, nublkH blk, char *buf);
extern unsigned int nu4k_getBlkNumInputs(nu4kH,nublkH);
extern void nu4k_getBlkInput(nu4kH,nublkH, unsigned int, nublkH *, unsigned int*);
extern unsigned int nu4k_getBlkNumOutputs(nu4kH,nublkH);
extern void nu4k_getBlkOutputRes(nu4kH,nublkH, unsigned int, NUCFG_resT *res);
extern NUCFG_formatE nu4k_getBlkOutputFormat(nu4kH,nublkH, unsigned int);
extern NUCFG_formatDiscriptorU nu4k_getBlkOutputFormatDiscriptor(nu4kH,nublkH, unsigned int, NUCFG_formatDiscriptorU*);
extern int nu4k_isInputBlk(nu4kH,nublkH);
extern void nu4k_updateBlkOutput(nu4kH,nublkH, unsigned int, nusoc_updateT *params);
extern unsigned int nu4k_getBlkStr(nu4kH,nublkH blk, char *str);
extern void nu4k_getInterleaveInfoFromDb(nu4kH h, nublkH blk, UINT32 *numIntP, NUCFG_interModeE *intMode);
extern void nu4k_setPathInterleave(nu4kH h, const nublkH blkh, void **valP, int enable);
extern XMLDB_pathE nu4k_getXmlDbPath(nu4kH h, nuBlkTypeE type);
extern void nu4k_setDpeFuncParam(nu4kH h, unsigned int inst, XMLModesG_dpeFuncT *dpeFuncParamsP);
extern void nu4k_updateBlkSclSize(nu4kH h, nublkH blk, UINT32 hSize, UINT32 vSize, UINT32 *numOfPpuSclOnPath);
ERRG_codeE nu4k_setBlkCropSize(nu4kH h, nublkH blk, unsigned int outNum, NUCFG_resT *res);
ERRG_codeE nu4k_setIauBypass(nu4kH h, nublkH blk, UINT32 *iauNums);
ERRG_codeE nu4k_setChannelFormat(nu4kH h, nublkH blk, UINT32 channel, NUCFG_changeFromatE mode);
ERRG_codeE nu4k_getIspLinkToWriterData(nu4kH h, UINT32 writerId, UINT32* width, UINT32* height, UINT32* ispRdId, UINT32* ispRdNum);


#ifdef __cplusplus
}
#endif

#endif //__NU4K_H__

