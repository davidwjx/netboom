#ifndef _CHARGER_H_
#define _CHARGER_H_


#ifdef __cplusplus
extern "C" {
#endif

ERRG_codeE CHARGERG_enterPowerSave( UINT8 wake_up_sec );
ERRG_codeE CHARGERG_enterPowerDown( UINT8 wake_up_sec );
ERRG_codeE CHARGERG_batteryStatus( UINT8 *levelP );
ERRG_codeE CHARGERG_chargerStatus( UINT8 *levelP );

#ifdef __cplusplus
}
#endif

#endif 

