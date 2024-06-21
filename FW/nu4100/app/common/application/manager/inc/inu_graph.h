#ifndef _INU_GRAPH_H_
#define _INU_GRAPH_H_

#include "inu2.h"
#include "inu2_internal.h"
#include "inu_ref.h"
#include "inu_node.h"
#include "inu2_types.h"

#include "inu_data.h"
#include "inu_function.h"

//#define NO_THREAD

typedef struct inu_graph
{
	inu_ref ref;

	void *privP;
}inu_graph;

typedef struct
{
	inu_ref__CtorParams ref_params;
	const char *socxml_path;
	inu_graph__calibrationPathT *calibPathsP;
	const char *graphXMLpath;
	bool saveXmlMod;
}inu_graph__CtorParams;


typedef struct
{
   UINT32 ret;
   UINT8  name[128];
}inu_graph__send_config_db_params;


//#define DATA_DEBUG
#ifdef DATA_DEBUG
extern UINT32 inu_debug_data_map;
#endif

ERRG_codeE inu_graph__forward_data_msg(inu_function *me, inu_data *data);
ERRG_codeE inu_graph__function_complete_msg(inu_function *me);
ERRG_codeE inu_graph__set_function_start_msg(inu_function *function, inu_function__startParamsT *startParamP);
ERRG_codeE inu_graph__set_function_stop_msg(inu_function *function, inu_function__stopParamsT *stopParamP);
ERRG_codeE inu_graph__sendOperateMsgWorkThr(inu_function *function, inu_function__operateParamsT *inputParamsP, int workThr);
ERRG_codeE inu_graph__sendDspAckMsgWorkThr(inu_function *function, inu_function__operateParamsT *operateParamsP, int workThr, UINT32 dspSource);
void inu_graph__insertNode(inu_graphH meH, inu_nodeH nodeH);
void *inu_graph__getXMLDB( inu_graph *me );
void *inu_graph__getSeqDB( inu_graph *me );
void inu_graph__showGraph(inu_graphH meH);
void inu_graph__initEngine( void );
void inu_graph__deinitEngine( void );
ERRG_codeE inu_graph_reconfigDb(inu_graphH meH);


void inu_graph__vtable_init(void);
const inu_ref__VTable *inu_graph__vtable_get(void);
#ifdef __cplusplus
extern "C" {
	ERRG_codeE inu_graph__setupExtIntInfo(inu_graph* graph, char* swGraphXMLpath);
}
#endif
bool inu_graph__isExtInterleaveChannel(UINT32 chId);
void inu_graph__markExtInterleaveChannel(UINT32 chId);
#if DEFSG_IS_GP
ERRG_codeE inu_graph__forward_data_msg_HandleImmediately(inu_function *me, inu_data *data);
ERRG_codeE inu_graph__function_complete_msg_immedate(inu_function *me);
#endif


#endif

