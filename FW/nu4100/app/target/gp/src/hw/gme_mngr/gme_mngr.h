/****************************************************************************
 *
 *   FileName: gme_mngr.h
 *
 *   Author:  Benny V.
 *
 *   Date:
 *
 *   Description: gme control
 *
 ****************************************************************************/

#ifndef GME_MNGR_H
#define GME_MNGR_H

#ifdef __cplusplus
      extern "C" {
#endif

/****************************************************************************
 ***************               I N C L U D E   F I L E S        *************
 ****************************************************************************/

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
ERRG_codeE GME_MNGRG_open();
ERRG_codeE GME_MNGRG_close();
ERRG_codeE GME_MNGRG_init(UINT32 memVirtAddr);
ERRG_codeE GME_MNGRG_resetHw(BOOL resetI2c);
ERRG_codeE GME_MNGRG_powerUp(int usbSpeed);
ERRG_codeE GME_MNGRG_powerDown();
ERRG_codeE GME_MNGRG_enableCevaClk();
ERRG_codeE GME_MNGRG_disableCevaClk();
ERRG_codeE GME_MNGRG_resetCevaUnit();
void       GME_MNGRG_I2Sconfigslave();
void       GME_MNGRG_I2SconfigMaster();
ERRG_codeE GME_MNGRG_setUART0ClkSrc();
UINT32     GME_MNGRG_getHwVersion();
void       GME_MNGRG_setModelType(INU_DEFSG_moduleTypeE modelType);
void       GME_MNGRG_setBaseVersion(INU_DEFSG_baseVersionE baseVersionNewVal);
INU_DEFSG_moduleTypeE GME_MNGRG_getModelType(void);
INU_DEFSG_moduleTypeE GME_MNGRG_getFullModelType(void);
INU_DEFSG_baseVersionE GME_MNGRG_getBaseVersion(void);
ERRG_codeE GME_MNGRG_resetIdve( bool cdeReset );

void GME_MNGRG_shutterSeq();
//void GME_MNGRG_backFromShutterSeq();

#ifdef __cplusplus
}
#endif

#endif //   GME_MNGR_H

