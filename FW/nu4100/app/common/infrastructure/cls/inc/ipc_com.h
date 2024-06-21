/****************************************************************************
 * 
 *   FileName: ipc_ctrl_com.h
 *
 *   Author:
 *
 *   Date: 
 *
 *   Description: IPC control com interface 
 *
 ****************************************************************************/
#ifndef _IPC_CTRL_COM_H_
#define _IPC_CTRL_COM_H_

/****************************************************************************
 ***************      I N C L U D E   F I L E S                 *************
 ****************************************************************************/
#include "cls_com.h"
#include "sys_defs.h"
#include "inu_cmd.h"
#if DEFSG_IS_NOT_HOST
#include "ipc_api.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/
// IPC interface static set-up values. Needs to be the same as Client process
#define SYS_DEFSG_CLS_IPC_TX_QUEUE_NAME               "/ClientToTppDataQue"
#define SYS_DEFSG_CLS_IPC_TX_MSGQ_MAX                 (3)
#define SYS_DEFSG_CLS_IPC_TX_MSGQ_SIZE                (MESSAGE_HEADER_SIZE + 4 + MESSAGE_FRAME_TOTAL_SIZE)

#define SYS_DEFSG_CLS_IPC_RX_QUEUE_NAME               "/TppToClientDataQue"
#define SYS_DEFSG_CLS_IPC_RX_MSGQ_MAX                 (SYS_DEFSG_CLS_IPC_TX_MSGQ_MAX)
#define SYS_DEFSG_CLS_IPC_RX_MSGQ_SIZE                (SYS_DEFSG_CLS_IPC_TX_MSGQ_SIZE)


/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/
typedef enum
{
   IPC_LYRP_STATUS_DOWN_E = 0,
   IPC_LYRP_STATUS_UP_E
} IPC_LYRP_ipcStatusE;



typedef struct
{
    OS_LYRG_msgQueT                     rxQue;
    int                                 rxQueStatus;
    OS_LYRG_msgQueT                     txQue;   
    int                                 txQueStatus; 
    volatile IPC_LYRP_ipcStatusE        ipcStatus;
} IPC_CTRL_COMG_paramsT;


/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/
ERRG_codeE IPC_LYRG_init( void );
ERRG_codeE IPC_LYRG_deinit( void );
ERRG_codeE IPC_LYRG_sendTx( UINT8* bufP, UINT32 len );

#ifdef __cplusplus
}
#endif
#endif /* _IPC_CTRL_COM_H_ */
