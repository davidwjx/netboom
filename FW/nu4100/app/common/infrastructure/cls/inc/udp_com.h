/****************************************************************************
 * 
 *   FileName: udp_com.h
 *
 *   Author:
 *
 *   Date: 
 *
 *   Description: UDP com interface 
 *
 ****************************************************************************/
#ifndef _UDP_COM_H_
#define _UDP_COM_H_

/****************************************************************************
 ***************      I N C L U D E   F I L E S                 *************
 ****************************************************************************/
#include "cls_com.h"
#include "socket.h"

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/

/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/

typedef struct
{
   int         portTx;
   int         portRx;
   const void  *ipAddrPeerP;
} UDP_COMG_interfaceParamsT;



/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/
void UDP_COMG_getOps(CLS_COMG_ifOperations *ifOpsP);

#ifdef __cplusplus
}
#endif

#endif /* _UDP_COM_H_ */
