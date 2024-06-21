/****************************************************************************
 *
 *   FileName: time.h
 *
 *   Author:
 *
 *   Date:
 *
 *   Description: Time sync module API
 *
 ****************************************************************************/
#ifndef INU_TIME_H
#define INU_TIME_H

#if DEFSG_IS_GP
void TIMEG_enableStats( void );
#endif
ERRG_codeE TIMEG_start( inu_device__set_time_t *setTimeParamsP, inu_device__getUsecTimeFuncT* getUsecTimeFunc);
ERRG_codeE TIMEG_stop( );

#endif
