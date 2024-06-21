/****************************************************************************
 *
 *   FileName: inu_alt.h
 *
 *   Author:
 *
 *   Date: 
 *
 *   Description: inu alternate module API 
 *   
 ****************************************************************************/
#ifndef INU_ALT_H
#define INU_ALT_H

#define ALTG_MAX_NUM_MODES (2)

typedef void* inu_altH;

ERRG_codeE ALTG_strobeHandle( inu_altH altH, void *sensorInfo1P);
int ALTG_getMode( inu_altH altH );
int ALTG_getThisFrameMode( inu_altH altH );
ERRG_codeE ALTG_start( inu_altH *altH, UINT32 numFramesPattern, UINT32 numFramesFlood );
ERRG_codeE ALTG_stop( inu_altH altH );
ERRG_codeE ALTG_init( );
BOOL ALTG_isActive( );

#endif
