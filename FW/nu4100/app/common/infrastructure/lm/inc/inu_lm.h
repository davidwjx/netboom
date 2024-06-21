/****************************************************************************
 *
 *   FileName: lm.h
 *
 *   Author:  Arnon C.
 *
 *   Date: 
 *
 *   Description: 
 *   
 ****************************************************************************/
#ifndef INU_LM_H
#define INU_LM_H

#include "inu_types.h"

#ifdef __cplusplus
extern "C" {
#endif
/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/

/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/
typedef void * LINK_MONITORG_handleT;

typedef struct
{
	BOOLEAN                    linkMonitor;
	int(*linkEventCallBack)(int, int, void*);
	void                      *linkEventCBparam;
	void					  *ref;
} LINK_MONITORG_cfg;


/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/
void LINK_MONITORG_rxEvent(LINK_MONITORG_handleT lmH);
int LINK_MONITORG_create(LINK_MONITORG_handleT *lmH, const LINK_MONITORG_cfg *cfgP);
int LINK_MONITORG_delete(LINK_MONITORG_handleT lmH);


#ifdef __cplusplus
}
#endif
#endif //INU_LM_H


