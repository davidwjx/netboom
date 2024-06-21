#ifndef _INU_NODE_H_
#define _INU_NODE_H_

#include "inu_ref.h"

#define INU_NODE_MAX_NUM_NODES (10)
struct inu_node;

/**
* node new input
*/
typedef void (inu_node__NewInput)(struct inu_node *me, struct inu_node *input);

/**
* node new output
*/
typedef void (inu_node__NewOutput)(struct inu_node *me, struct inu_node *output);

/**
* node removed input
*/
typedef void (inu_node__RemovedInput)(struct inu_node *me, struct inu_node *input);

/**
* node removed output
*/
typedef void (inu_node__RemovedOutput)(struct inu_node *me, struct inu_node *output);


/**
* Virtual functions table (vtable)
*/
typedef struct
{
   inu_ref__VTable            ref_vtable;
   inu_node__NewInput         *p_newInput;
   inu_node__NewOutput        *p_newOutput;
   inu_node__RemovedInput     *p_removeInput;
   inu_node__RemovedOutput    *p_removeOutput;
} inu_node__VTable;

typedef struct inu_node
{
   inu_ref  ref;
   void     *privP;
}inu_node;

typedef struct
{
   inu_ref__CtorParams ref_params;
   inu_ref__id_t       graph_ref_id;
}inu_node__CtorParams;

typedef struct
{
   inu_ref__id_t       input;
   inu_ref__id_t       output;
}inu_node__bindNodeT;


void inu_node__vtableInitDefaults(inu_node__VTable *vtableP);
const inu_node__VTable *inu_node__vtable_get(void);

inu_node *inu_node__new(inu_node__CtorParams *ctorParamsP);
void inu_node__delete(inu_node *node);
ERRG_codeE inu_node__ctor(inu_node *me, inu_node__CtorParams *ctorParamsP);
unsigned int inu_node__getNumOutputs(inu_node *me);
void inu_node__insertGraph(inu_nodeH meH,inu_graphH graphH);
void inu_node__removeGraph(inu_nodeH meH);

#endif
