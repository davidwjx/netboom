/****************************************************************************
 * 
 *   FileName: msg_box.h
 *
 *   Author: Ram B
 *
 *   Date: 11/2012
 *
 *   Description: 2-process shared memory msg box interface
 *
 ****************************************************************************/
#ifndef MSG_BOX_H
#define MSG_BOX_H

/****************************************************************************
 ***************      I N C L U D E   F I L E S                 *************
 ****************************************************************************/

/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/

/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/

typedef enum
{
   MSG_BOXG_GP_CVE1_E         = 0,
   MSG_BOXG_LOOPBACK_E        = 1,
   MSG_BOXG_NUM_INSTANCES_E   = 2
} MSG_BOXG_instanceE;

typedef enum
{
   MSG_BOXG_RD_E = 0,
   MSG_BOXG_WR_E = 1
} MSG_BOXG_dirE;

typedef void * MSG_BOXG_handleT;

/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/
ERRG_codeE  MSG_BOXG_init(void);
ERRG_codeE  MSG_BOXG_open(MSG_BOXG_handleT *handleP, MSG_BOXG_instanceE inst);
ERRG_codeE  MSG_BOXG_add(MSG_BOXG_handleT handle, const void *bufP, UINT16 len);
ERRG_codeE  MSG_BOXG_rem(MSG_BOXG_handleT handle, void *bufP, UINT16 len);
ERRG_codeE  MSG_BOXG_peek(MSG_BOXG_handleT handle, void *bufP, UINT16 len);
UINT16      MSG_BOXP_getMsgSize(MSG_BOXG_handleT handle);
void        MSG_BOXG_show(UINT8 isWriter);

#endif //MSG_BOX_H
