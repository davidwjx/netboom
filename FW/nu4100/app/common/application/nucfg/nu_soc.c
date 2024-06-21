#include "inu_common.h"
#include "nucfg_priv.h"

#include "nu4k.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void * privSocH;

typedef struct _NUCFG_socT
{
   privSocH psoc;

   ERRG_codeE (*init)(XMLDB_dbH, void **);
   void (*deinit)(void *);
   unsigned int (*getNumOutputs)(privSocH);
   nublkH (*getOutput)(privSocH, unsigned int index);
   unsigned int (*getNumInputs)(privSocH);
   nublkH (*getInput)(privSocH,unsigned int index);

   void (*getBlkTypeInst)(privSocH,nublkH, nuBlkTypeE *, unsigned int *);
   NUCFG_hwId (*getBlkHwName)(privSocH,nublkH);
   void(*getChHwId)(privSocH, nublkH, char*);
   unsigned int (*getBlkNumInputs)(privSocH,nublkH);
   void (*getBlkInput)(privSocH,nublkH, unsigned int, nublkH *, unsigned int *);
   unsigned int (*getBlkNumOutputs)(privSocH,nublkH);
   void (*setDpeFuncParam)(nu4kH h, unsigned int inst, XMLModesG_dpeFuncT *dpeFuncParamsP);
   void (*getBlkOutputRes)(privSocH,nublkH, unsigned int, NUCFG_resT *res);
   NUCFG_formatE (*getBlkOutputFormat)(privSocH,nublkH, unsigned int);
   NUCFG_formatDiscriptorU (*getBlkOutputFormatDisc)(privSocH,nublkH, unsigned int, NUCFG_formatDiscriptorU *fmtDisc);
   int (*isInputBlk)(privSocH,nublkH);
   void (*updateBlkOutput)(privSocH,nublkH, unsigned int, nusoc_updateT *);
   void (*getInterleaveInfoFromDb)(nu4kH h, nublkH blk, UINT32 *numIntP, NUCFG_interModeE *intMode);
   void(*setPathInterleave)(nu4kH h, const nublkH blkh, void **valP, int enable);
   XMLDB_pathE (*getXmlDbPath)(nu4kH h, nuBlkTypeE type);

   unsigned int (*getBlkStr)(privSocH,nublkH, char *);
   void(*updateBlkSclSize)(privSocH, nublkH, UINT32, UINT32, UINT32*);
   void(*updateDppSclSize)(privSocH, nublkH, UINT32, UINT32, UINT32*);
   ERRG_codeE(*setBlkCropSize)(privSocH, nublkH, unsigned int, NUCFG_resT *);
   ERRG_codeE(*setIauBypass)(privSocH, nublkH, UINT32*);
   ERRG_codeE(*setChannelFormat)(privSocH, nublkH, UINT32, NUCFG_changeFromatE);
   ERRG_codeE(*getIspLinkToWriterData)(privSocH, UINT32, UINT32*, UINT32*, UINT32*, UINT32*);
} NUCFG_socT;

ERRG_codeE nusoc_init(XMLDB_dbH db, NUCFG_socH *h)
{
   //allocate structure for this soc
   ERRG_codeE ret = NUCFG__RET_SUCCESS;
   NUCFG_socT *socp = (NUCFG_socT *)malloc(sizeof(NUCFG_socT));
   if(!socp)
   {
      ret = NUCFG__ERR_OUT_OF_RSRCS;
      goto cleanup;
   }

   //compile time/runtime switch for other soc version
   socp->init                    = nu4k_init;
   socp->deinit                  = nu4k_deInit;
   socp->getNumOutputs           = nu4k_getNumOutputs;
   socp->getNumInputs            = nu4k_getNumInputs;
   socp->getInput                = nu4k_getInput;
   socp->getOutput               = nu4k_getOutput;
   socp->getBlkTypeInst          = nu4k_getBlkTypeInst;
   socp->getBlkHwName            = nu4k_getBlkHwName;
   socp->getBlkNumInputs         = nu4k_getBlkNumInputs;
   socp->getBlkInput             = nu4k_getBlkInput;
   socp->getBlkNumOutputs        = nu4k_getBlkNumOutputs;
   socp->getBlkOutputRes         = nu4k_getBlkOutputRes;
   socp->getBlkOutputFormat      = nu4k_getBlkOutputFormat;
   socp->getBlkOutputFormatDisc  = nu4k_getBlkOutputFormatDiscriptor;
   socp->isInputBlk              = nu4k_isInputBlk;
   socp->updateBlkOutput         = nu4k_updateBlkOutput;
   socp->getInterleaveInfoFromDb = nu4k_getInterleaveInfoFromDb;
   socp->setPathInterleave       = nu4k_setPathInterleave;
   socp->getXmlDbPath            = nu4k_getXmlDbPath;
   socp->getChHwId               = nu4k_getChHwId;
   socp->getBlkStr               = nu4k_getBlkStr;
   socp->setDpeFuncParam         = nu4k_setDpeFuncParam;
   socp->updateBlkSclSize        = nu4k_updateBlkSclSize;
   socp->setBlkCropSize          = nu4k_setBlkCropSize;
   socp->setIauBypass            = nu4k_setIauBypass;
   socp->setChannelFormat        = nu4k_setChannelFormat;
   socp->getIspLinkToWriterData  = nu4k_getIspLinkToWriterData;
   //init private soc 
   socp->psoc = NULL;
   ret = socp->init(db, &socp->psoc);
   if(ERRG_FAILED(ret))
   {
      goto cleanup;
   }

   *h = socp;
   return ret;

cleanup:
   
   if(socp)
      free(socp);
   *h = NULL;
   return ret;
}

void nusoc_deInit(NUCFG_socH h)
{
   h->deinit(h->psoc);
   free((NUCFG_socT *)h);
}
unsigned int nusoc_getNumOutputs(NUCFG_socH h)
{
   return h->getNumOutputs(h->psoc);
}

void nusoc_getBlkTypeInst(NUCFG_socH h ,nublkH blk, nuBlkTypeE *type, unsigned int *inst)
{
   h->getBlkTypeInst(h->psoc,blk,type,inst);
}

void nusoc_getChHwId(NUCFG_socH h, nublkH blk, char *buf )
{
   h->getChHwId(h->psoc, blk, buf);
}

NUCFG_hwId nusoc_getBlkHwId(NUCFG_socH h, nublkH blk)
{
   return h->getBlkHwName(h->psoc,blk);
}

nublkH nusoc_getOutput(NUCFG_socH h, unsigned int inst)
{
   return h->getOutput(h->psoc,inst);
}

unsigned int nusoc_getNumInputs(NUCFG_socH h)
{
   return h->getNumInputs(h->psoc);
}

nublkH nusoc_getInput(NUCFG_socH h, unsigned int inst)
{
   return h->getInput(h->psoc,inst);
}

unsigned int nusoc_getBlkNumInputs(NUCFG_socH h, nublkH blk)
{
   return h->getBlkNumInputs(h->psoc,blk);
}

void nusoc_getBlkInput(NUCFG_socH h, nublkH blk0, unsigned int i, nublkH *blk1, unsigned int *j)
{
   h->getBlkInput(h->psoc,blk0,i,blk1,j);
}
unsigned int nusoc_getBlkNumOutputs(NUCFG_socH h, nublkH blk)
{
   return h->getBlkNumOutputs(h->psoc,blk);
}
void nusoc_getBlkOutputRes(NUCFG_socH h,nublkH blk, unsigned int i, NUCFG_resT *res)
{
   h->getBlkOutputRes(h->psoc,blk,i,res);
}
NUCFG_formatE nusoc_getBlkOutputFormat(NUCFG_socH h,nublkH blk, unsigned int i)
{
   return h->getBlkOutputFormat(h->psoc,blk,i);
}

NUCFG_formatDiscriptorU nusoc_getBlkOutputFormatDiscriptor(NUCFG_socH h,nublkH blk, unsigned int i, NUCFG_formatDiscriptorU *fmtDisc)
{
   return h->getBlkOutputFormatDisc(h->psoc,blk,i, fmtDisc);
}

int nusoc_isInputBlk(NUCFG_socH h,nublkH blk)
{
   return h->isInputBlk(h->psoc, blk);
}

int nusoc_isSameBlk(NUCFG_socH h, nublkH blk0, nublkH blk1)
{
   nuBlkTypeE type0,type1;
   unsigned int inst0,inst1;
   nusoc_getBlkTypeInst(h,blk0,&type0,&inst0);
   nusoc_getBlkTypeInst(h,blk1,&type1,&inst1);
   return ((type0 == type1) && (inst0 == inst1));
}

int nusoc_getOutSelect(NUCFG_socH h, nublkH blk_from, nublkH blk_to)
{
   int ret = -1;
   unsigned int i;
   nublkH tmpblk;
   unsigned int tmpinst;
  
   for(i = 0; i < nusoc_getBlkNumInputs(h, blk_to); i++)
   {
      tmpblk = NULL;
      nusoc_getBlkInput(h, blk_to, i, &tmpblk, &tmpinst);
      if(tmpblk && nusoc_isSameBlk(h, blk_from,tmpblk))
      {
         ret = i;
         break;
      }
   }
   return ret;
}

void nusoc_getInterleaveInfoFromDb(NUCFG_socH h, nublkH blk, UINT32 *numIntP, NUCFG_interModeE *intMode)
{
	h->getInterleaveInfoFromDb(h->psoc, blk, numIntP, intMode);
}

void nusoc_setPathInterleave(NUCFG_socH h, const nublkH blkh, void **arg, int enable)
{
	h->setPathInterleave(h->psoc, blkh, arg, enable);
}

XMLDB_pathE nusoc_getXmlDbPath(NUCFG_socH h, nuBlkTypeE type)
{
	return h->getXmlDbPath(h->psoc, type);
}

void nusoc_updateBlkOutput(NUCFG_socH h, nublkH blk,unsigned int i, nusoc_updateT *params)
{
   if(h->updateBlkOutput)
      h->updateBlkOutput(h->psoc, blk, i, params);
}

unsigned int nusoc_getBlkStr(NUCFG_socH h, nublkH blk, char *str)
{
   return h->getBlkStr(h->psoc,blk,str);
}

void nusoc_setDpeFuncParam(NUCFG_socH h, unsigned int inst, XMLModesG_dpeFuncT *dpeFuncParamsP)
{
   h->setDpeFuncParam(h->psoc, inst, dpeFuncParamsP);
}

void nusoc_updateBlkSclSize(NUCFG_socH h, nublkH blk, UINT32 hSize, UINT32 vSize, UINT32 *numOfPpuSclOnPath)
{
   h->updateBlkSclSize(h->psoc, blk, hSize, vSize, numOfPpuSclOnPath);
}


ERRG_codeE nusoc_setBlkCropSize(NUCFG_socH h, nublkH blk, unsigned int i, NUCFG_resT *res)
{
   return h->setBlkCropSize(h->psoc, blk, i, res);
}

ERRG_codeE nusoc_setIauBypass(NUCFG_socH h, nublkH blk, UINT32 *numIaus)
{
   return h->setIauBypass(h->psoc, blk, numIaus);
}

ERRG_codeE nusoc_setChannelFormat(NUCFG_socH h, nublkH blk, UINT32 channel, NUCFG_changeFromatE mode)
{
   return h->setChannelFormat(h->psoc, blk, channel, mode);
}

ERRG_codeE nusoc_getIspLinkToWriterData(NUCFG_socH h, UINT32 writerId, UINT32 *width, UINT32 *height, UINT32 * sensorId, UINT32 * ispRdNum)
{
    return h->getIspLinkToWriterData(h->psoc, writerId, width, height, sensorId, ispRdNum);
}

const char *nusoc_hwId2Str(NUCFG_hwId hwid)
{
   return (const char *)hwid;
}

NUFLD_blkE nusoc_convert2Fld(nuBlkTypeE blk)
{
   switch(blk)
   {
   case(NUCFG_BLK_GEN_E):             return NUFLD_GEN_E;
   case(NUCFG_BLK_SEN_E):             return NUFLD_SENSOR_E;
   case(NUCFG_BLK_MEDIATOR_E):        return NUFLD_MEDIATOR_E;
   case(NUCFG_BLK_SLU_PARALLEL_E):    return NUFLD_SLU_PARALLEL_E;
   case(NUCFG_BLK_SLU_E):             return NUFLD_SLU_E;
   case(NUCFG_BLK_MIPI_RX_E):         return NUFLD_MIPI_RX_E;
   case(NUCFG_BLK_PAR_RX_E):          return NUFLD_PAR_RX_E;
   case(NUCFG_BLK_MIPI_MUX_E):        return NUFLD_MIPI_MUX_E;
   case(NUCFG_BLK_IAU_E):             return NUFLD_IAU_E;
   case(NUCFG_BLK_IAU_COLOR_E):       return NUFLD_IAU_COLOR_E;
   case(NUCFG_BLK_HIST_E):            return NUFLD_HIST_E;
   case(NUCFG_BLK_PPU_NOSCL_E):       return NUFLD_PPU_NOSCL_E;
   case(NUCFG_BLK_PPU_SCL_E):         return NUFLD_PPU_SCL_E;
   case(NUCFG_BLK_PPU_HYBSCL_E):      return NUFLD_PPU_HYBSCL_E;
   case(NUCFG_BLK_PPU_HYB_E):         return NUFLD_PPU_HYB_E;
   case(NUCFG_BLK_AXIRD_E):           return NUFLD_AXIRD_E;
   case(NUCFG_BLK_INTERLEAVER_E):     return NUFLD_AXIRD_E;
   case(NUCFG_BLK_AXIWR_E):           return NUFLD_AXIWR_E;
   case(NUCFG_BLK_DPE_E):             return NUFLD_DPE_E;
   case(NUCFG_BLK_DPP_E):             return NUFLD_DPP_E;
   case(NUCFG_BLK_CVA_E):             return NUFLD_CVA_E;
   case(NUCFG_BLK_DPHY_TX_E):         return NUFLD_DPHY_TX_E;
   case(NUCFG_BLK_VSC_CSI_TX_E):      return NUFLD_VSC_CSI_TX_E;
   case(NUCFG_BLK_ISP_E):             return NUFLD_ISP_E;
   case(NUCFG_BLK_ISPRD_E):           return NUFLD_META_ISP_RD_E;
   
   default: return NUFLD_INVALID_E;
   }
}
nuBlkTypeE nusoc_convert2Blk(NUFLD_blkE fld)
{
   switch (fld)
   {
   case(NUFLD_SLU_PARALLEL_E): return NUCFG_BLK_SLU_PARALLEL_E;
   case(NUFLD_SLU_E):        return NUCFG_BLK_SLU_E;
   case(NUFLD_IAU_E):        return NUCFG_BLK_IAU_E;
   case(NUFLD_IAU_COLOR_E):  return NUCFG_BLK_IAU_COLOR_E;
   case(NUFLD_HIST_E):       return NUCFG_BLK_HIST_E;
   case(NUFLD_PPU_NOSCL_E):  return NUCFG_BLK_PPU_NOSCL_E;
   case(NUFLD_PPU_SCL_E):    return NUCFG_BLK_PPU_SCL_E;
   case(NUFLD_PPU_HYBSCL_E): return NUCFG_BLK_PPU_HYBSCL_E;
   case(NUFLD_PPU_HYB_E):    return NUCFG_BLK_PPU_HYB_E;
   case(NUFLD_SENSOR_E):     return NUCFG_BLK_SEN_E;
   case(NUFLD_AXIRD_E):      return NUCFG_BLK_AXIRD_E;
   case(NUFLD_AXIWR_E):      return NUCFG_BLK_AXIWR_E;
   case(NUFLD_DPE_E):        return NUCFG_BLK_DPE_E;
   case(NUFLD_DPP_E):        return NUCFG_BLK_DPP_E;
   case(NUFLD_DPHY_TX_E):    return NUCFG_BLK_DPHY_TX_E;
   case(NUFLD_VSC_CSI_TX_E): return NUCFG_BLK_VSC_CSI_TX_E;
   default: return NUCFG_NUM_BLKS_E;
   }
}


#ifdef __cplusplus
}
#endif
