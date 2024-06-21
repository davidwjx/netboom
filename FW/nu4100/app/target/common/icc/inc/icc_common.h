/****************************************************************************
 *
 *   FileName: icc_common.h
 *
 *   Author: Arnon C. & Noam P
 *
 *   Date: 6/5/2015
 *
 *   Description: Define shared icc bitween GP and CEVA 
 *   
 ****************************************************************************/
#ifndef ICC_COMMON_H
#define ICC_COMMON_H

#include "err_defs.h"

#define ICC_TEST              (0)

typedef enum
{
   ICCG_CMD_TARGET_DSPA    = 0, //XM4
   ICCG_CMD_TARGET_DSPB    = 1, //EV6X
   ICCG_CMD_TARGET_MAX_NUM 
} ICC_COMMONG_dspTargetT;


// ICC_COMMONG_iccCmdReqAckMask if set, its CMD/MSG. if not its ACK
#define ICC_COMMONG_iccCmdReqAckMask  			(0x80000000)
#define ICC_COMMONG_iccCmdMsgDescMask 			(0x0000FFFF)
#define ICC_COMMONG_JOB_DESCRIPTOR_POOL_SIZE (20)
#define ICC_COMMONG_JOB_DESCRIPTOR_MASK		(0xFFFF)
#define ICC_COMMONG_JOB_DESCRIPTOR_INVALID	(0xFFFF)

#define ICC_COMMONG_ICTL0_DSPA_INT_NUM  (0) //GP interrupt line from XM4
#define ICC_COMMONG_ICTL0_DSPB_INT_NUM  (1) //GP interrupt line from EV6X


typedef void (*ICC_COMMONG_cmdCallbackT)(UINT16 jobDescriptorIndex, ERRG_codeE result, ICC_COMMONG_dspTargetT dspTarget );

#endif

