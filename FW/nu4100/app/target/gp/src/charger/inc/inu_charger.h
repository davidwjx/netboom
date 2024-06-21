/****************************************************************************
 *
 *   FileName: inu_charger.h
 *
 *   Author:
 *
 *   Date: 
 *
 *   Description: inu charger module API 
 *   
 ****************************************************************************/
#ifndef INU_CHARGER_H
#define INU_CHARGER_H

typedef enum
{
   INU_CHARGER__AT828_E     = 0,
} inu_charger__chargerModuleE;


typedef struct
{
   UINT32 uartBaudRate;
   UINT32 uartPortNum;
   inu_charger__chargerModuleE chargerModule;
   UINT32 debugEnable;
} inu_charger__init_charger_t;


ERRG_codeE CHARGERG_start( inu_charger__init_charger_t *initChargerParamsP );
ERRG_codeE CHARGERG_stop( );
ERRG_codeE CHARGERG_readReg(UINT8 reg, UINT8 *regValP);
ERRG_codeE CHARGERG_writeReg(UINT8 reg, UINT8 regVal);

#endif
