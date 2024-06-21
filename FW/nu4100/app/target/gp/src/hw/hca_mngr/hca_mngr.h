/****************************************************************************
 *
 *   FileName: hca_mngr.h
 *
 *   Author:  Arnon C.
 *
 *   Date:
 *
 *   Description: Hardware Clock Adjust manager
 *
 ****************************************************************************/

#ifndef HCA_MNGR_H
#define HCA_MNGR_H

#ifdef __cplusplus
      extern "C" {
#endif

/****************************************************************************
 ***************               I N C L U D E   F I L E S        *************
 ****************************************************************************/

/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/
typedef void* HCA_MNGRG_consumerHandle;

/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/
typedef enum
{
   HCA_MNGRG_IAE = 0,
   HCA_MNGRG_DPE,
   HCA_MNGRG_PPE,
   HCA_MNGRG_CVA,
   HCA_MNGRG_MAX_NUM_HW_UNITS   
}HCA_MNGRG_hwUnitE;


/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/
ERRG_codeE HCA_MNGRG_calcRequiredCiifClock( void );
ERRG_codeE HCA_MNGRG_allocConsumer( HCA_MNGRG_consumerHandle *handleP, char *name );
ERRG_codeE HCA_MNGRG_addRequirementToConsumer( HCA_MNGRG_consumerHandle handle, UINT32 hwUnit );
ERRG_codeE HCA_MNGRG_addInputClockOfConsumer( HCA_MNGRG_consumerHandle handle, UINT32 inputClock );
ERRG_codeE HCA_MNGRG_removeConsumer( HCA_MNGRG_consumerHandle handle );
void       HCA_MNGRG_disableEnable( UINT32 disableEnable );
ERRG_codeE HCA_MNGRG_showStats( void );
ERRG_codeE HCA_MNGRG_init( void );

#ifdef __cplusplus
}
#endif

#endif //   HCG_MNGR_H

