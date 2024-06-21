
#ifndef __PL330_DMA_MISC_H__
#define __PL330_DMA_MISC_H__

#include "inu_types.h"


#define	PL330_DMA_FRAME_DONE_INT_BIT			0x01
#define	PL330_DMA_CHANNEL_DONE_INT_BIT			0x100

#define	PL330_DMA_FRAME_DONE_INT_OFFSET			0
#define	PL330_DMA_CHANNEL_DONE_INT_OFFSET		8



typedef enum
{
	PL330_DMA_STATUS_STOPPED_E					   =	0	,
	PL330_DMA_STATUS_EXECUTING_E				   =	1	,
	PL330_DMA_STATUS_CACHE_MISS_E				   =	2	,
	PL330_DMA_STATUS_UPDATING_PC_E				=	3	,
	PL330_DMA_STATUS_WAITING_FOR_EVENT_E		=	4	,
	PL330_DMA_STATUS_AT_BARRIER_E				   =	5	,
	PL330_DMA_STATUS_RESERVED0_E				   =	6	,
	PL330_DMA_STATUS_WAITING_FOR_PERIPHERAL_E	=	7	,
	PL330_DMA_STATUS_KILLING_E					   =	8	,
	PL330_DMA_STATUS_COMPLETING_E				   =	9	,
	PL330_DMA_STATUS_RESERVED1_E				   =	10	,
	PL330_DMA_STATUS_RESERVED2_E				   =	11	,
	PL330_DMA_STATUS_RESERVED3_E				   =	12	,
	PL330_DMA_STATUS_RESERVED4_E				   =	13	,
	PL330_DMA_STATUS_FAULTING_COMPLETING_E		=	14	,
	PL330_DMA_STATUS_FAULTING_E					=	15	,
}PL330_DMA_StatusE;



typedef struct
{
	UINT8 isInterrupt;
	
}PL330_DMA_EventParamsT;


void 	PL330_DMA_initBaseAddress(UINT32 dmaRegistersBaseAddr);
UINT32 	PL330_DMA_getBaseAddress(void);

void 	PL330_DMA_enableFrameDoneInterrupt(UINT8 core, UINT8 channel);
void 	PL330_DMA_enableChannelDoneInterrupt(UINT8 core, UINT8 channel);
void 	PL330_DMA_disableFrameDoneInterrupt(UINT8 core, UINT8 channel);
void 	PL330_DMA_disableChannelDoneInterrupt(UINT8 core, UINT8 channel);
void 	PL330_DMA_clearFrameDoneInterrupt(UINT8 core, UINT8 channel);
void 	PL330_DMA_clearChannelDoneInterrupt(UINT8 core, UINT8 channel);

void 	PL330_DMA_setEvent0FromChannel(UINT8 core, UINT8 channel);
void 	PL330_DMA_setEvent1FromChannel(UINT8 core, UINT8 channel);
void 	PL330_DMA_waitForEvent0(UINT8 core, UINT8 channel);
void 	PL330_DMA_waitForEvent1(UINT8 core, UINT8 channel);

UINT32	PL330_DMA_getInterruptStatus(UINT8 core);
PL330_DMA_StatusE 	PL330_DMA_getChannelStatus(UINT8 core, UINT8 channel);
UINT32   PL330_DMA_getSAR(UINT8 core, UINT8 channel);
UINT32   PL330_DMA_getDAR(UINT8 core, UINT8 channel);
UINT32	 PL330_DMA_getPC(UINT8 core, UINT8 channel);

UINT32   PL330_DMA_getFaultStatusManager(UINT8 core);
UINT32   PL330_DMA_getFaultStatusChannel(UINT8 core);
UINT32   PL330_DMA_getFaultTypeChannel(UINT8 core, UINT8 channel);


#define PL330_DMA_REG_WR(_core_, _reg_offset_, _data_)		(*(volatile UINT32*)(PL330_DMA_getBaseAddress() + ((_core_)*0x1000) + (_reg_offset_)) = (_data_))
#define PL330_DMA_REG_RD(_core_, _reg_offset_)				(*(volatile UINT32*)(PL330_DMA_getBaseAddress() + ((_core_)*0x1000) + (_reg_offset_)))

#endif	//__PL330_DMA_MISC_H__


