/****************************************************************************
 *
 *   FileName: hcd_mngr.h
 *
 *   Author:  Arnon C.
 *
 *   Date:
 *
 *   Description: Hardware Clock Gating manager
 *
 ****************************************************************************/

#ifndef HCG_MNGR_H
#define HCG_MNGR_H

#ifdef __cplusplus
      extern "C" {
#endif

/****************************************************************************
 ***************               I N C L U D E   F I L E S        *************
 ****************************************************************************/

/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/
#define MULTIPILE_DOWN_EVENTS

typedef void* HCG_MNGRG_voterHandle;
/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/
typedef enum
{
   HCG_MNGRG_DMA0 = 0,
   HCG_MNGRG_DMA1,
   HCG_MNGRG_DMA2,
   HCG_MNGRG_IAE,
   HCG_MNGRG_DPE,
   HCG_MNGRG_PPE,
   HCG_MNGRG_CVA,
   HCG_MNGRG_MAX_NUM_HW_UNITS   
}HCG_MNGRG_hwUnitE;

typedef enum
{
   HCG_MNGRG_HW_EVENT_SENSOR_TIMER_1_E,
   HCG_MNGRG_HW_EVENT_SENSOR_TIMER_2_E, 
   HCG_MNGRG_HW_EVENT_SENSOR_TIMER_3_E,
   HCG_MNGRG_HW_EVENT_SENSOR_TIMER_4_E,
   HCG_MNGRG_HW_EVENT_SENSOR_TIMER_5_E,
   HCG_MNGRG_HW_EVENT_SENSOR_TIMER_6_E,
   HCG_MNGRG_HW_EVENT_IAE_SLU0_FRAME_START_ISR_E,
   HCG_MNGRG_HW_EVENT_IAE_SLU1_FRAME_START_ISR_E,
   HCG_MNGRG_HW_EVENT_IAE_SLU2_FRAME_START_ISR_E,
   HCG_MNGRG_HW_EVENT_IAE_SLU3_FRAME_START_ISR_E,
   HCG_MNGRG_HW_EVENT_IAE_SLU4_FRAME_START_ISR_E,
   HCG_MNGRG_HW_EVENT_IAE_SLU5_FRAME_START_ISR_E,
   HCG_MNGRG_HW_EVENT_IAE_SLU0_FRAME_END_ISR_E,
   HCG_MNGRG_HW_EVENT_IAE_IAU0_FRAME_END_ISR_E = HCG_MNGRG_HW_EVENT_IAE_SLU0_FRAME_END_ISR_E + 6,  
   HCG_MNGRG_HW_EVENT_IAE_IAU_HIST0_RDY_ISR_E  = HCG_MNGRG_HW_EVENT_IAE_IAU0_FRAME_END_ISR_E + 4,
   HCG_MNGRG_HW_EVENT_IAE_IAU_HIST1_RDY_ISR_E,
   HCG_MNGRG_HW_EVENT_WRITER_0_E,
   HCG_MNGRG_HW_EVENT_WRITER_1_E,
   HCG_MNGRG_HW_EVENT_WRITER_2_E,
   HCG_MNGRG_HW_EVENT_CDE_ISR_CORE0_CH0_E,
   HCG_MNGRG_HW_EVENT_CDE_ISR_CORE0_CH1_E,
   HCG_MNGRG_HW_EVENT_CDE_ISR_CORE0_CH2_E,
   HCG_MNGRG_HW_EVENT_CDE_ISR_CORE0_CH3_E,
   HCG_MNGRG_HW_EVENT_CDE_ISR_CORE0_CH4_E,
   HCG_MNGRG_HW_EVENT_CDE_ISR_CORE0_CH5_E,
   HCG_MNGRG_HW_EVENT_CDE_ISR_CORE0_CH6_E,
   HCG_MNGRG_HW_EVENT_CDE_ISR_CORE0_CH7_E,
   HCG_MNGRG_HW_EVENT_CDE_ISR_CORE1_CH0_E,
   HCG_MNGRG_HW_EVENT_CDE_ISR_CORE1_CH1_E,
   HCG_MNGRG_HW_EVENT_CDE_ISR_CORE1_CH2_E,
   HCG_MNGRG_HW_EVENT_CDE_ISR_CORE1_CH3_E,
   HCG_MNGRG_HW_EVENT_CDE_ISR_CORE1_CH4_E,
   HCG_MNGRG_HW_EVENT_CDE_ISR_CORE1_CH5_E,
   HCG_MNGRG_HW_EVENT_CDE_ISR_CORE1_CH6_E,
   HCG_MNGRG_HW_EVENT_CDE_ISR_CORE1_CH7_E,
   HCG_MNGRG_HW_EVENT_CDE_ISR_CORE2_CH0_E,
   HCG_MNGRG_HW_EVENT_CDE_ISR_CORE2_CH1_E,
   HCG_MNGRG_HW_EVENT_CDE_ISR_CORE2_CH2_E,
   HCG_MNGRG_HW_EVENT_CDE_ISR_CORE2_CH3_E,
   HCG_MNGRG_HW_EVENT_CDE_ISR_CORE2_CH4_E,
   HCG_MNGRG_HW_EVENT_CDE_ISR_CORE2_CH5_E,
   HCG_MNGRG_HW_EVENT_CDE_ISR_CORE2_CH6_E,
   HCG_MNGRG_HW_EVENT_CDE_ISR_CORE2_CH7_E,
   HCG_MNGRG_HW_EVENT_MAX_NUM_E,
   HCG_MNGRG_HW_EVENT_INVALID_E = 0xFF,
} HCG_MNGRG_hwEventE;

#define HCG_MNGRG_HW_IDVE_UNITS ((1 << HCG_MNGRG_IAE) | (1 << HCG_MNGRG_DPE) | (1 << HCG_MNGRG_PPE) | (1 << HCG_MNGRG_CVA))
#define HCG_MNGRG_HW_ALL_UNITS  ((HCG_MNGRG_HW_IDVE_UNITS) | (1 << HCG_MNGRG_DMA0) | (1 << HCG_MNGRG_DMA1) | (1 << HCG_MNGRG_DMA2))


/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/
ERRG_codeE HCG_MNGRG_allocVoter( HCG_MNGRG_voterHandle *handleP, char *name, UINT32 enDynVoting );
void       HCG_MNGRG_deallocVoter( HCG_MNGRG_voterHandle handle );
ERRG_codeE HCG_MNGRG_registerHwUnitVote( HCG_MNGRG_voterHandle handle, HCG_MNGRG_hwUnitE hwUnit );
void       HCG_MNGRG_registerHwUnitUpEvent( HCG_MNGRG_voterHandle handle, UINT32 event );
void       HCG_MNGRG_registerHwUnitDownEvent( HCG_MNGRG_voterHandle handle, UINT32 event, UINT32 numOccurrences,UINT32 frrVecSize );
void       HCG_MNGRG_registerEndEvent(HCG_MNGRG_voterHandle handle, HCG_MNGRG_hwEventE event );
void       HCG_MNGRG_registerStartEvent(HCG_MNGRG_voterHandle handle, HCG_MNGRG_hwEventE event );
void       HCG_MNGRG_startVoterWithTimer( HCG_MNGRG_voterHandle handle, UINT32 sensorIndex );
void       HCG_MNGRG_startVoter( HCG_MNGRG_voterHandle handle );
UINT32     HCG_MNGRG_voteUnit( HCG_MNGRG_hwUnitE unit );
UINT32     HCG_MNGRG_devoteUnit( HCG_MNGRG_hwUnitE unit );
UINT32     HCG_MNGRG_voteUnits( UINT32 units );
UINT32     HCG_MNGRG_devoteUnits( UINT32 units );
void       HCG_MNGRG_processEvent( HCG_MNGRG_hwEventE event, UINT64 timeUs, UINT32 numEvents );
void       HCG_MNGRG_startPrimePeriod( void );
ERRG_codeE HCG_MNGRG_initVoteSystem( void );
void       HCG_MNGRG_showStats( void );



#ifdef __cplusplus
}
#endif

#endif //   HCG_MNGR_H

