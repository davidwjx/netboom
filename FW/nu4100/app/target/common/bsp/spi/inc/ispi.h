/****************************************************************************
 *
 *   FileName: ISPI.h
 *
 *   Author: Yaron A..
 *
 *   Date: 
 *
 *   Description: ISPIdriver wrapper
 *   
 ****************************************************************************/
#ifndef ISPI_DRV_H
#define ISPI_DRV_H

/****************************************************************************
 ***************      I N C L U D E   F I L E S                 *************
 ****************************************************************************/

#include "inu_types.h"

#ifdef __cplusplus
      extern "C" {
#endif



/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/

/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/

/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/
ERRG_codeE SPI_DRVG_open(UINT8 flashNum,char *deviceName);
ERRG_codeE SPI_DRVG_close(UINT8 flashNum);
ERRG_codeE SPI_DRVG_spiTransaction(UINT8 flashNum,BYTE *pTxBuffer, UINT32 txLen, BYTE *pRxBuffer, UINT32 rxLen);
ERRG_codeE SPI_DRVG_fullDuplexspiTrans (BYTE *pTxBuffer, BYTE *pRxBuffer, UINT32 len);
int SPI_DRVG_read(BYTE *pRxBuffer, UINT32 *len);
ERRG_codeE SPI_DRVG_setupMode(int mode);
ERRG_codeE SPI_DRVG_setupSpeed(int speed);
ERRG_codeE SPI_DRVG_setupLsb(int lsbFirst);


#ifdef __cplusplus
}
#endif


#endif //ISPI_DRV_H
