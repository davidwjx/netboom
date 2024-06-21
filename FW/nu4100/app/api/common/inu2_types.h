#ifndef _INU2_TYPES_H_
#define _INU2_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
   INU__XM4_CORE_E  = 0,
   INU__EV62_CORE_E = 1,
   INU__GP_CORE_E   = 2,
   INU__NUM_CORES_E = 3
} inu_coresE;


enum inu_graph_state_e
{
   INU_GRAPH_STATE_UNVERIFIED = 0,
   INU_GRAPH_STATE_VERIFIED,
   INU_GRAPH_STATE_RUNNING,
};

enum inu_walksoc_action_e
{
   INU_WALKSOC_CONTINUE,
   INU_WALKSOC_QUIT,
};

typedef void* inu_refH;
typedef void* inu_deviceH;
typedef void* inu_nodeH;
typedef void* inu_graphH;
typedef void* inu_dataH;
typedef void* inu_functionH;
typedef void* inu_load_networkH;
typedef void* inu_load_backgroundH;
typedef void* inu_nucfgH;

#ifdef __cplusplus
}
#endif

#endif //_INU2_TYPES_H_
