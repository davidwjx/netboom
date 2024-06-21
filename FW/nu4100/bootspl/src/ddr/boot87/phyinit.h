#ifndef _PHYINIT_H_
#define _PHYINIT_H_

#include "dwc_ddrphy_apb.h"


// Debug level Defines
//#define DDR_MSG_DETAILED_DEBUG
//#define DDR_MSG_COARSE_DEBUG
//#define DDR_MSG_ASSERTION_MESSAGES
//#define DDR_MSG_FIRMWARE_COMPLETE



typedef struct
{
	unsigned int  message_index;
	char*		  message;
}phyinit_debug_message_t;

char* phyinit_debug_get_string( unsigned int string_index );
void  phyinit_C_initPhyConfig(unsigned int freq);
void  phyinit_D_loadIMEM(void);
void  phyinit_F_loadDMEM(unsigned int freq);
void  phyinit_D2_loadIMEM(void);
void  phyinit_F2_loadDMEM(unsigned int freq);
unsigned int phyinit_G_waitFwDone(unsigned int train_2D);
void  phyinit_I_loadPIEImage(unsigned int freq);
unsigned int ddrm_phy_init_seq(unsigned int freq);
unsigned int ddrm_umctl_init_seq (unsigned int freq);
#endif	//_PHYINIT_H_
