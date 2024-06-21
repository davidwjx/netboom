/****************************************************************************
 *
 *   FileName: sched.h
 *
 *   Author: Arnon C.
 *
 *   Date: 05/19/15
 *
 *   Description: scheduler - task manager
 *   
 ****************************************************************************/
#ifndef SCHEDULER_H
#define SCHEDULER_H

#ifdef __cplusplus
		  extern "C" {
#endif

/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/
#include "inu_common.h"

// Threads Priorities
#define ICC_THREAD_PRIORITY         0
#define CNN_HAL_THREAD_PRIORITY     1
#define YOLO_PP_THREAD_PRIORITY     5
#define FACE_SSD_PP_THREAD_PRIORITY 5
#define VGF_THREAD_PRIORITY         6
#define CNN_PRE_THREAD_PRIORITY     1

// Threads CPU
#define CNN_DISPATCH_CPU        1
#define ICC_THREAD_CPU          0
#define CNN_HAL_THREAD_CPU	     1
#define YOLO_PP_THREAD_CPU	     1
#define FACE_SSD_PP_THREAD_CPU  1
#define VGF_THREAD_CPU          0
#define CNN_PRE_THREAD_CPU      0

#define SCHED_SW_IRQ PSP_EXCPT_SW_IRQ 





/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/  
typedef void (*SCHEDG_algEntryPointT)( void *jobDescriptorP, UINT16 jobDescriptor, inu_function__operateParamsT *paramsP );

/****************************************************************************
 ***************      E X T E R N A L       D A T A           ***************
 ****************************************************************************/

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/
void SCHEDG_pushNewJob( UINT32 jobDescriptor );
void SCHEDG_entryPoint( UINT32 dspSource );
void SCHEDG_init();
void SCHEDG_pushFinishedJob(UINT32 jobDescriptor);
void SCHEDG_preempt(void);
//	ERRG_codeE SCHEDG_registerTaskEntryPoint(UINT32 taskId, SCHEDG_algEntryPointT taskEntryPoint);

#endif

#ifdef __cplusplus
}

#endif //	SCHEDULER_H


