#ifndef _INU_GRAPH_API_H_
#define _INU_GRAPH_API_H_

#include "inu2.h"

typedef struct
{
   char                   **calibrationPath;
   UINT16                   numOfPaths;
} inu_graph__calibrationPathT;

typedef struct
{
   inu_deviceH deviceH;
   const char *socxml_path;
   inu_graph__calibrationPathT *calibPathsP;
   const char *graphXMLpath;
   bool saveXmlMod;
   bool debugMode;
   char name[NUCFG_MAX_CONFIG_NAME];
} inu_graph__initParams;

ERRG_codeE inu_graph__new(inu_graphH *meH, inu_graph__initParams *initParamsP, inu_nucfgH *nucfgH);
ERRG_codeE inu_graph__renew(inu_graphH *meH, inu_graph__initParams *initParamsP, inu_nucfgH *nucfgH);
ERRG_codeE inu_graph__finalize(inu_graphH meH, inu_nucfgH nucfgH);
ERRG_codeE inu_graph__delete(inu_graphH graph);
ERRG_codeE inu_graph__close(inu_graphH graphH);
ERRG_codeE inu_graph__walkGraphNodes(inu_graphH meH, INT32(*node_cb)(inu_nodeH nodeH, void **arg), void **argP);
ERRG_codeE inu_graph__getOrigSensGroup(inu_nodeH srcNodeH, inu_nodeH *dstNodeH);
ERRG_codeE inu_graph__getOrigWriters(inu_nodeH srcNodeH, inu_nodeH *dstNodeH);
ERRG_codeE inu_graph__findNode(inu_graphH meH, const char *userName, inu_refH *inuRefP);
ERRG_codeE inu_graph__printNodes(inu_graphH meH, inu_refH *inuRefP);
ERRG_codeE inu_graph__offlineGetConfig(const char *socxml_path, inu_nucfgH *cfgH);

#endif
