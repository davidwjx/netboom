#ifndef _INU_FACTORY_H_
#define _INU_FACTORY_H_

#include "err_defs.h"
#include "inu_ref.h"

typedef enum
{
   INU_FACTORY__CONN_USB_IF0_E = 0,
   INU_FACTORY__CONN_USB_IF1_E ,
   INU_FACTORY__CONN_USB_IF2_E ,
   INU_FACTORY__CONN_USB_IF3_E ,
   INU_FACTORY__CONN_USB_IF4_E ,
   INU_FACTORY__CONN_USB_IF5_E ,
   INU_FACTORY__CONN_UART_E,
   INU_FACTORY__CONN_TCP_E,
   INU_FACTORY__CONN_UDP_E,
   INU_FACTORY__CONN_CDC_E,
   INU_FACTORY__CONN_STUB_E,
   INU_FACTORY__CONN_UVC1_E,
   INU_FACTORY__CONN_UVC2_E,
   INU_FACTORY__CONN_UVC3_E,
   INU_FACTORY__CONN_UVC4_E,
   INU_FACTORY__CONN_UVC_MAX_E=INU_FACTORY__CONN_UVC4_E,
   INU_FACTORY__NUM_CONNECTIONS_E,
   INU_FACTORY__INVALID,
} inu_factory__connIdE;

#define INU_FACTORY__CONN_NUM_UVC (INU_FACTORY__CONN_UVC_MAX_E - INU_FACTORY__CONN_UVC1_E + 1)

void           inu_factory__init();
ERRG_codeE     inu_factory__delete(inu_ref *me, int force);
ERRG_codeE     inu_factory__new(inu_ref **me, void *ctorParamsP, inu_ref__types ref_type);
inu_ref__types inu_factory__remoteNewCmdToType(UINT32 remoteNewCmd);
void          *inu_factory__getConnConfig(inu_factory__connIdE id);
void          *inu_factory__getConnHandle(inu_factory__connIdE id);
void          *inu_factory__getCommHandle(inu_factory__connIdE id);
ERRG_codeE     inu_factory__changeDataComm(inu_ref *me, inu_factory__connIdE connId);
int            inu_factory__releaseComm(void *commP);
void           inu_factory__restoreAllComm(inu_deviceH deviceH);
void           inu_factory__releaseAllComm( void );
UINT32         inu_factory__getRefSize(inu_ref__types ref_type);
void           inu_factory__initChanConfigTbl(inu_device_interface_0_t *deviceIf0);

#endif //__INU_FACTORY_H__
