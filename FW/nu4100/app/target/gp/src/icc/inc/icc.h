/****************************************************************************
 *
 *   FileName: icc.h
 *
 *   Author: Arnon C. & Noam P
 *
 *   Date: 
 *
 *   Description: ICC API
 *   
 ****************************************************************************/
#ifndef ICC_H
#define ICC_H

/****************************************************************************
 ***************      L O C A L         T Y P E D E F S     ***************
 ****************************************************************************/
typedef enum
{
	ICCP_IDLE            = 0,
	ICCP_WAIT_FOR_ACK    = 1
} ICCP_cmdStateT;

#define ICCP_JOB_DESCRIPTOR_STACK_MAXSIZE (ICC_COMMONG_JOB_DESCRIPTOR_POOL_SIZE)


/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/
typedef enum
{
   ICCG_CMD_REQ            = 0,
   ICCG_CMD_ACK               ,
} ICCG_cmdTypeT;

typedef enum
{
   ICCG_MSG_REQ            = 0,
   ICCG_MSG_ACK               ,
} ICCG_msgTypeT;

typedef struct
{
   ICCG_cmdTypeT           cmdType;
   UINT32                  descriptorHandle;
   ICC_COMMONG_dspTargetT  dspTarget;
} ICCG_cmdT;

typedef struct
{
   ICCG_msgTypeT           msgType;
   UINT32                  descriptorHandle;
   ICC_COMMONG_dspTargetT  dspSource;
} ICCG_msgT;

typedef struct ICCG_jobDescriptorElem
{
   UINT16 jobDescriptorPoolIndex;
   struct ICCG_jobDescriptorElem *next;
} ICCG_jobDescriptorElemT;


/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/


/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/
ERRG_codeE ICCG_iccInit( void );
ERRG_codeE ICCG_iccClose(void);
ERRG_codeE ICCG_sendIccCmd(ICCG_cmdT *msgP);
ERRG_codeE ICCG_sendIccMsg(ICCG_msgT *msgP);
ERRG_codeE ICCG_getJobDescriptor( UINT16 *index );
ERRG_codeE ICCG_returnJobDescriptor( UINT32 index );


#endif //ICC_H

