#include "inu_common.h"
#include "cls_com.h"

#include "mipi_mngr.h"

typedef struct
{
   INU_DEFSG_mipiInstE inst;
   UINT32 virtualChannel;
   UINT32 mipiVideoFormat;
   UINT16 horizontal;
   UINT16 vertical;
   UINT16 fps;
} MIPI_COMG_ifParamsT;

typedef struct
{
   UINT16 horizontal;
   UINT16 vertical;
   UINT16 fps;
} MIPI_COMG_ifCfgT;


/****************************************************************************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/
static ERRG_codeE MIPI_COMP_openInst(MIPI_COMG_ifParamsT *cfgP)
{
   ERRG_codeE ret = MIPI_COM__RET_SUCCESS;
(void)cfgP;
#if 0
   mipiInstConfigT *mipiCfgP;
   mipiVscCfgT *vscCfgP;
   UINT32 hb, vb;

   //TODO: use the IAE function to calculate blanking
   ret = MIPI_DRVP_calcBlanking(cfgP->fps, cfgP->vertical, cfgP->horizontal,&hb,&vb);
   if(ERRG_FAILED(ret))
      return ret;
      
   mipiCfgP = MIPI_MNGRG_getCfg(cfgP->inst);
   mipiCfgP->svt.pktSizeBytes    = cfgP->horizontal * 2;
   mipiCfgP->svt.mipiVideoFormat = cfgP->mipiVideoFormat;
   mipiCfgP->svt.virtualChannel  = cfgP->virtualChannel;
   mipiCfgP->dphy.enable         = 1;
   mipiCfgP->dmaTx.enable        = 1;
   ret = MIPI_MNGRG_cfg(cfgP->inst,mipiCfgP);

   if(ERRG_SUCCEEDED(ret))
   {
      vscCfgP = MIPI_MNGRG_getVscCfg(INU_DEFSG_VSC_1_E);
      vscCfgP->horizontal = cfgP->horizontal;
      vscCfgP->vertical = cfgP->vertical;
      vscCfgP->horizontalBlank = (UINT16)hb;
      vscCfgP->verticalBlank= (UINT16)vb;
      ret = MIPI_MNGRG_cfgVsc(INU_DEFSG_VSC_1_E,vscCfgP);
   }
   #endif
   return ret;

}

ERRG_codeE MIPI_COMG_open(void **handle, void *openCfg)
{
   ERRG_codeE ret;
   ret = MIPI_COMP_openInst((MIPI_COMG_ifParamsT *)openCfg);
   if(ERRG_SUCCEEDED(ret))
      *handle = (void *)((MIPI_COMG_ifParamsT *)openCfg)->inst;

   return ret;
}

ERRG_codeE MIPI_COMG_close(void *handle)
{
   ERRG_codeE ret = MIPI_COM__RET_SUCCESS;
   (void)handle;

#if 0
   INU_DEFSG_mipiInstE inst = (INU_DEFSG_mipiInstE)handle;
   mipiInstConfigT *mipiCfgP;

   mipiCfgP = MIPI_MNGRG_getCfg(inst);
   mipiCfgP->dphy.enable = 0;
   ret = MIPI_MNGRG_cfg(inst,mipiCfgP);
#endif
   return ret;

}

ERRG_codeE MIPI_COMG_config(void *handle, void *cfg)
{
   ERRG_codeE ret = MIPI_COM__RET_SUCCESS;
   (void)handle;
   (void)cfg;
#if 0
   INU_DEFSG_mipiInstE inst = (INU_DEFSG_mipiInstE)handle;
   mipiInstConfigT *currCfgP;
   mipiVscCfgT *currVscP;
   MIPI_COMG_ifCfgT *ifCfgP = (MIPI_COMG_ifCfgT *)cfg;
   
   UINT32 hb, vb;

   currCfgP = MIPI_MNGRG_getCfg(inst);
   currVscP = MIPI_MNGRG_getVscCfg(INU_DEFSG_VSC_1_E);
   if((currVscP->horizontal != ifCfgP->horizontal) ||(currVscP->horizontal != ifCfgP->horizontal))
   {
      ret = MIPI_DRVP_calcBlanking(ifCfgP->fps, ifCfgP->vertical, ifCfgP->horizontal,&hb,&vb);
      if(ERRG_SUCCEEDED(ret))
      {
         currVscP->horizontal = ifCfgP->horizontal;
         currVscP->vertical = ifCfgP->vertical;
         currVscP->horizontalBlank = hb;
         currVscP->verticalBlank= vb;
         MIPI_MNGRG_cfgVsc(INU_DEFSG_VSC_1_E, currVscP);
         currCfgP->svt.pktSizeBytes = ifCfgP->horizontal*2;
         currCfgP->dmaTx.enable     = 1;
         MIPI_MNGRG_cfg(inst, currCfgP);
      }
   }
#endif
   return ret;
}

ERRG_codeE MIPI_COMG_send(void *handle, UINT8* bufP, UINT32 len, UINT32 *byteSentP)
{
   ERRG_codeE ret = MIPI_COM__RET_SUCCESS;
   (void)handle;
   (void)bufP;
   (void)len;
   (void)byteSentP;
#if 0
   INU_DEFSG_mipiInstE inst = (INU_DEFSG_mipiInstE)handle;
   UINT32 flags =(MIPI_MNGRG_TX_PAD|MIPI_MNGRG_TX_HDR);


   ret =MIPI_MNGRG_tx(inst, bufP, len,flags, NULL,NULL);

   if(ERRG_SUCCEEDED(ret))
      *byteSentP = len;
#endif

   return ret;
}

ERRG_codeE MIPI_COMG_sendv(void *handle,  void *bufP, UINT32 *byteSentP)
{
   ERRG_codeE ret = MIPI_COM__RET_SUCCESS;
   (void)handle;
   (void)bufP;
   (void)byteSentP;
#if 0
   INU_DEFSG_mipiInstE inst = (INU_DEFSG_mipiInstE)handle;
   UINT32 flags =(MIPI_MNGRG_TX_PAD|MIPI_MNGRG_TX_HDR);
   
   ret = MIPI_MNGRG_txChain(inst, (MEM_POOLG_bufDescT *)bufP,flags, NULL, NULL);
   if(ERRG_SUCCEEDED(ret))
      *byteSentP = MEM_POOLG_getChainLen( (MEM_POOLG_bufDescT *)bufP);
#endif
   return ret;
}

void MIPI_COMG_getCaps(void *handle, CLS_COMG_capsT *capsP)
{
   (void)handle;
   capsP->flags  = 0;
   capsP->flags |= CLS_COMG_CAP_TX;
   capsP->flags |= CLS_COMG_CAP_SENDV;
   capsP->fixedSize = 0;
}

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/

/****************************************************************************
*
*  Function Name: MIPI_COMG_getOps
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: 
*
****************************************************************************/
void MIPI_COMG_getOps(CLS_COMG_ifOperations *ifOpsP)
{
   ifOpsP->open     = &MIPI_COMG_open;
   ifOpsP->close   =  &MIPI_COMG_close;
   ifOpsP->config   = &MIPI_COMG_config;
   ifOpsP->getCaps  = &MIPI_COMG_getCaps;
   ifOpsP->send     = &MIPI_COMG_send;
   ifOpsP->sendv    = &MIPI_COMG_sendv;
   ifOpsP->recv     = NULL;
   ifOpsP->peek     = NULL;
   ifOpsP->recv_unbuffered = NULL;
   ifOpsP->sendv_unbuffered = NULL;
}


