
/****************************************************************************
 *
 *   FileName: CDE_SCENARIOS.h
 *
 *   Author: Dima S.
 *
 *   Date: 
 *
 *   Description: DMA driver
 *   
 ****************************************************************************/
#ifndef __CDE_SCENARIOS_H__
#define __CDE_SCENARIOS_H__

#ifdef __cplusplus
      extern "C" {
#endif

/****************************************************************************
 ***************      I N C L U D E   F I L E S                 *************
 ****************************************************************************/
#include "PL330_dma_instruction.h"
#include "PL330_dma_misc.h"
#include "mem_pool.h"
/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/
#define CDE_SCENARIOS_MAX_LOOPS     250
#define CDE_SCENARIOS_USE_LAST_FLAG

/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/


ERRG_codeE CDE_SCENARIOSG_periphToMemoryInfinite( CDE_DRVG_channelParamsT* params, void* arg );
ERRG_codeE CDE_SCENARIOSG_memoryToMemoryOnce( CDE_DRVG_channelParamsT* params, void* arg );
#ifdef CDE_DRVG_IMPROVED_ISR_HANDLING
#ifdef CDE_DRVG_METADATA
UINT8 CDE_SCENARIOSG_setNewDest(CDE_DRVG_channelHandleT channelH, CDE_DRVG_channelHandleT metadataChannelHandle, MEM_POOLG_bufDescT **buffDescDoneListP, UINT32 *numP, CDE_DRVG_extIntParams *extIntParams);
#else
UINT8 CDE_SCENARIOSG_setNewDest(CDE_DRVG_channelHandleT channelH, MEM_POOLG_bufDescT **buffDescDoneListP, UINT32 *numP, CDE_DRVG_extIntParams *extIntParams);
#endif
#else
UINT8 CDE_SCENARIOSG_setNewDest(CDE_DRVG_channelHandleT channelH, CDE_DRVG_extIntParams *extIntParams);
#endif
ERRG_codeE CDE_SCENARIOSG_memoryToPeriph( CDE_DRVG_channelParamsT* params, void* arg );
void       CDE_SCENARIOSG_setMemToPeriphSrc(CDE_DRVG_channelHandleT channelH, UINT32 srcAddress);
void       CDE_SCENARIOSG_setDstSrc(CDE_DRVG_channelParamsT* params,unsigned int dstAddress, unsigned int srcAddress);
ERRG_codeE CDE_SCENARIOSG_periphToMemoryCyclic( CDE_DRVG_channelParamsT* params, void* arg );
ERRG_codeE CDE_SCENARIOSG_periphToMemoryDdrLess( CDE_DRVG_channelParamsT* params, void* arg );
#ifdef CDE_DRVG_VIRTUAL_CHANNELS
UINT8      CDE_SCENARIOSG_setEvent(CDE_DRVG_channelHandleT channelH);
#endif
UINT8      CDE_SCENARIOSG_waitEvent(CDE_DRVG_channelHandleT channelH, UINT8 loopNum, UINT8 index, UINT8 channelNum);
ERRG_codeE CDE_SCENARIOSG_periphToMemoryInfiniteExt( CDE_DRVG_channelParamsT* params, void* arg );

#endif //__CDE_SCENARIOS_H__
