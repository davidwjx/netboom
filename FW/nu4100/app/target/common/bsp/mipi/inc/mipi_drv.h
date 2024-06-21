/****************************************************************************
 *
 *   FileName: mipi_drv.h
 *
 *   Author: Arnon C.
 *
 *   Date: 
 *
 *   Description: Inuitive MIPI Driver
 *   
 ****************************************************************************/
#ifndef MIPI_DRV_H
#define MIPI_DRV_H

typedef struct
{
   UINT16 enable;
   UINT16 direction;    //0-tx, 1- rx
   UINT16 pllMhz;
   UINT16 format;
   UINT16 laneEnable;   //bit0 clklane, bit1 -lane0, bit2 - lane1
} mipiDphyCfgT;

typedef struct
{
   INU_DEFSG_mipiTxSourceE vscSrc;
   UINT32 pktSizeBytes;
   UINT32 mipiVideoFormat;
   UINT32 virtualChannel;
   UINT16 flags; //bit0- hclock_stop, bit1-vclock_stop, bit2- auto_incr
   UINT32 timerA;
   UINT32 timerB;
   UINT32 timerC;
} mipiSvtCfgT;

typedef struct
{
   UINT16 enable;
   UINT16 horizontal;
   UINT16 vertical;
   UINT16 horizontalBlank;
   UINT16 verticalBlank;
}mipiVscCfgT;

typedef struct
{
   UINT16 enable;
}dmaTxCfgT;

typedef struct
{
   INU_DEFSG_mipiInstE  mipiInst;
   mipiDphyCfgT         dphy;
   mipiSvtCfgT          svt;
   dmaTxCfgT            dmaTx;
} mipiInstConfigT;

typedef struct
{
   UINT16 enable;
   UINT32 inputEnableMap;
   UINT32 chSelectMap;
} mipiViCfgT;


/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/

/****************************************************************************
**********************     G L O B A L         F U N C T I O N S    *********************
 ****************************************************************************/
ERRG_codeE MIPI_DRVG_init(void);
ERRG_codeE MIPI_DRVG_cfgRx(mipiInstConfigT mipiCfg, UINT8 is4lanes);
ERRG_codeE MIPI_DRVG_resetRx(mipiInstConfigT mipiCfg);

ERRG_codeE MIPI_DRVG_rxClkEn(INU_DEFSG_mipiInstE inst);
UINT32     MIPI_DRVG_getIaeVirtBase(void);
UINT32     MIPI_DRVG_getPpeVirtBase(void);
ERRG_codeE MIPI_DRVG_cfgTx( mipiInstConfigT *mipiCfgP );
ERRG_codeE MIPI_DRVG_resetTx(mipiInstConfigT *mipiCfgP);


//ERRG_codeE MIPI_DRVG_cfgVsc(INU_DEFSG_vscE vsc, mipiVscCfgT *vscP);
//ERRG_codeE MIPI_DRVG_cfgVi(mipiViCfgT *cfgP);
ERRG_codeE MIPI_DRVP_calcBlanking(UINT32 fps, UINT32 vert, UINT32 horizs, UINT32 *hblank, UINT32 *vbalnk);
//ERRG_codeE MIPI_DRVG_openTxDma(void);
//ERRG_codeE MIPI_DRVG_txDma(UINT8 *bufP, UINT32 len, MIPI_DRVG_txdoneT doneCb, void *arg);
//void       MIPI_DRVG_closeTxDma(void);
void       MIPI_DRVG_showRegs(void);

#endif

