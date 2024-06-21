#ifndef __INU_GRAPH_CREATOR__
#define __INU_GRAPH_CREATOR__
#include "err_defs.h"
#include "inu_graph.h"

#ifdef __cplusplus
extern "C" {
#endif
//static graph
ERRG_codeE inu_graph__createPostProcessNodes(inu_graph* graph, char* graphXMLpath);

ERRG_codeE inu_graph__createSocChannel(inu_graph* graph, inu_refH* inu_refP, unsigned int chId, const char* name);
ERRG_codeE inu_graph__createIspChannel(inu_graph* graph, inu_refH* inu_refP, unsigned int chId, const char *name);
ERRG_codeE inu_graph__createImage(inu_graph* graph, inu_refH* inu_refP, inu_functionH sourceFunctionH, inu_image__descriptor* imageDescriptor, unsigned int chId, UINT16 numLinesPerChunk);
ERRG_codeE inu_graph__createHistogram(inu_graph* graph, inu_refH* inu_refP, unsigned int chId, const char *name);
ERRG_codeE inu_graph__createHistData(inu_graph* graph, inu_refH* inu_refP, inu_functionH sourceFunctionH, unsigned int chId);
ERRG_codeE inu_graph__createSensorGroup(inu_graph* graph, inu_refH* inu_refP, inu_sensor__parameter_list_t *masterParamsList, unsigned int chId);
ERRG_codeE inu_graph__createSensor(inu_graph* graph, inu_refH* inu_refP, inu_sensor__parameter_list_t *paramsList, unsigned int chId);
ERRG_codeE inu_graph__createWriter(inu_graph* graph, inu_refH* inu_refP, inu_image__descriptor* imageDescriptor, unsigned int chId, const char *name);
ERRG_codeE inu_graph__createCvaChannel(inu_graph *graph, inu_refH *inu_refP, unsigned int chId, const char *name);
ERRG_codeE inu_graph__createCvaData(inu_graph *graph, inu_refH *inu_refP, inu_functionH sourceFunctionH, inu_cva_data__descriptor_t *cvaDescriptor, unsigned int chId);
ERRG_codeE inu_graph__createMipiChannel(inu_graph* graph, inu_refH* inu_refP, unsigned int chId, const char *name);


#ifdef __cplusplus
}
#endif

#endif //__INU_GRAPH_CREATOR__

