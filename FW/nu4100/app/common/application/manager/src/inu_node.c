#include "inu2_internal.h"
#include "err_defs.h"
#include "inu2_types.h"
#include "mem_pool.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "internal_cmd.h"
#include "assert.h"

#define LINK_LIST_CONNECTIVITY


typedef struct inu_node__edge
{
   struct inu_node *node;
   struct inu_node__edge *next;
}inu_node__edge;


typedef struct
{
#ifdef LINK_LIST_CONNECTIVITY
   inu_node__edge *inputNodes;
   inu_node__edge *outputNodes;
#else
   struct inu_node *inputNodesList[INU_NODE_MAX_NUM_NODES];
   struct inu_node *outputNodesList[INU_NODE_MAX_NUM_NODES];
#endif
   void     *graphP;
}inu_node__privData;

static inu_node__VTable _vtable;
static bool _bool_vtable_initialized = 0;
static const char* name = "INU_NODE";

static const char* inu_node__name(inu_ref *me)
{
   FIX_UNUSED_PARAM_WARN(me);
   return name;
}

static void inu_node__dtor(inu_ref *me)
{
   inu_node__privData *privP = (inu_node__privData*)((inu_node*)me)->privP;
   inu_ref__vtable_get()->p_dtor((inu_ref*)me);
#ifdef LINK_LIST_CONNECTIVITY
   inu_node__edge *runner;
   runner = privP->inputNodes;
   while(runner)
   {
      //call the input node to remove connection to me
      ((inu_node__VTable*)runner->node->ref.p_vtable)->p_removeOutput(runner->node, (inu_node*)me);

      //remove the edge
      privP->inputNodes = privP->inputNodes->next;
      free(runner);
      runner = privP->inputNodes;
   }
   runner = privP->outputNodes;
   while(runner)
   {
      //call the output node to remove connection to me   
      ((inu_node__VTable*)runner->node->ref.p_vtable)->p_removeInput(runner->node, (inu_node*)me);

      privP->outputNodes = privP->outputNodes->next;
      free(runner);
      runner = privP->outputNodes;
   }
#else
   int i;
   //notify inputs and outputs of my removal
   for (i = 0; i < INU_NODE_MAX_NUM_NODES; i++)
   {
      if (privP->inputNodesList[i] != NULL)
      {
         ((inu_node__VTable*)privP->inputNodesList[i]->ref.p_vtable)->p_removeOutput(privP->inputNodesList[i], (inu_node*)me);
      }

      if (privP->outputNodesList[i] != NULL)
      {
         ((inu_node__VTable*)privP->outputNodesList[i]->ref.p_vtable)->p_removeInput(privP->outputNodesList[i], (inu_node*)me);
      }
   }
#endif
   free(privP);
}

/* Constructor */
ERRG_codeE inu_node__ctor(inu_node *me, inu_node__CtorParams *ctorParamsP)
{
   ERRG_codeE ret;
   ret = inu_ref__ctor(&me->ref, &ctorParamsP->ref_params);
   //alloc the private data struct and init it
   me->privP = (inu_node__privData*)malloc(sizeof(inu_node__privData));
   if (!me->privP)
   {
      return INU_NODE__ERR_OUT_OF_MEM;
   }
   memset(me->privP, 0x00, sizeof(inu_node__privData));
   inu_node__insertGraph(me,inu_device__getRefById((inu_device*)inu_ref__getDevice((inu_ref*)me),ctorParamsP->graph_ref_id));
   return ret;
}

static int inu_node__rxIoctl(inu_ref *me, void *msgP, int msgCode)
{
   inu_ref__vtable_get()->p_rxSyncCtrl(me, msgP, msgCode);

   switch (msgCode)
   {
      case(INTERNAL_CMDG_BIND_NODES_E):
      {
         inu_node *outputNode = (inu_node*)inu_device__getRefById((inu_device*)inu_ref__getDevice(me),((inu_ref__container*)msgP)->id);
         inu_node__bindNodes(me, outputNode);
         break;
      }

      default:
      break;
   }
   return 0;
}

static void inu_node__addNewEdge(inu_node__edge **edgeListHead , inu_node *node)
{
   inu_node__edge  *newEdge, *runner;
   newEdge = (inu_node__edge*)malloc(sizeof(inu_node__edge));
   if (!newEdge)
      assert(0);
   newEdge->node = node;
   newEdge->next = NULL;
   //add inputs at the end, to maintain the same order of creation   
   //check if head is empty
   if (!(*edgeListHead))
   {
      *edgeListHead = newEdge;
   }
   else
   {
      runner = *edgeListHead;
      //find last in list
      while(runner->next)
      {
         runner = runner->next;
      }
      runner->next = newEdge;
   }
}

static void inu_node__newInput(inu_node *me, inu_node *input)
{
   inu_node__privData *privP = (inu_node__privData*)me->privP;
#ifdef LINK_LIST_CONNECTIVITY
   inu_node__addNewEdge(&privP->inputNodes, input);
#else
   int i;
   //printf("_newInput: for %s (%d), input is %s (%d)\n",me->ref.p_vtable->p_name((inu_ref*)me),me->ref.id,input->ref.p_vtable->p_name((inu_ref*)input),input->ref.id);
   for (i = 0; i < INU_NODE_MAX_NUM_NODES; i++)
   {
      if (privP->inputNodesList[i] == NULL)
      {
         privP->inputNodesList[i] = input;
         break;
      }
   }

   if (i == INU_NODE_MAX_NUM_NODES)
      assert(0);
#endif
}

static void inu_node__newOutput(inu_node *me, inu_node *output)
{
   inu_node__privData *privP = (inu_node__privData*)me->privP;
#ifdef LINK_LIST_CONNECTIVITY
   inu_node__addNewEdge(&privP->outputNodes, output);
#else
   int i;
   //printf("_newOutput: for %s (%d), input is %s (%d)\n",me->ref.p_vtable->p_name((inu_ref*)me),me->ref.id,output->ref.p_vtable->p_name((inu_ref*)output),output->ref.id);
   for (i = 0; i < INU_NODE_MAX_NUM_NODES; i++)
   {
      if (privP->outputNodesList[i] == NULL)
      {
         privP->outputNodesList[i] = output;
         break;
      }
   }

   if (i == INU_NODE_MAX_NUM_NODES)
      assert(0);
#endif
}

static void inu_node__removeEdge(inu_node__edge **edgeListHead , inu_node *node)
{
   inu_node__edge  *edge, *runner;
   runner = *edgeListHead;
   if (!runner)
      assert(0);

   //check if the node is the head
   if (runner->node == node)
   {
      edge = runner;
      *edgeListHead = runner->next;
      free(edge);
      return;
   }
   else
   {
      while(runner->next)
      {
         if(runner->next->node == node)
         {
            edge = runner->next;
            runner->next = runner->next->next;
            free(edge);
            return;
         }
         runner = runner->next;
      }
   }

   //did not find the node
   assert(0);
}

static void inu_node__removeInput(inu_node *me, inu_node *input)
{
   inu_node__privData *privP = (inu_node__privData*)me->privP;
#ifdef LINK_LIST_CONNECTIVITY
   inu_node__removeEdge(&privP->inputNodes, input);
#else
   int i;

   for (i = 0; i < INU_NODE_MAX_NUM_NODES; i++)
   {
      if (privP->inputNodesList[i] == input)
      {
         privP->inputNodesList[i] = NULL;
         break;
      }
   }
#endif
}

static void inu_node__removeOutput(inu_node *me, inu_node *output)
{
   inu_node__privData *privP = (inu_node__privData*)me->privP;
#ifdef LINK_LIST_CONNECTIVITY
   inu_node__removeEdge(&privP->outputNodes, output);
#else
   int i;

   for (i = 0; i < INU_NODE_MAX_NUM_NODES; i++)
   {
      if (privP->outputNodesList[i] == output)
      {
         privP->outputNodesList[i] = NULL;
         break;
      }
   }
#endif
}

void inu_node__vtableInitDefaults(inu_node__VTable *vtableP)
{
   inu_ref__vtableInitDefaults(&vtableP->ref_vtable);
   vtableP->ref_vtable.p_name        = inu_node__name;
   vtableP->ref_vtable.p_dtor        = inu_node__dtor;
   vtableP->ref_vtable.p_ctor        = (inu_ref__Ctor*)inu_node__ctor;
   vtableP->ref_vtable.p_rxSyncCtrl  = inu_node__rxIoctl;
   
   vtableP->p_newInput     = inu_node__newInput;
   vtableP->p_newOutput    = inu_node__newOutput;
   vtableP->p_removeInput  = inu_node__removeInput;
   vtableP->p_removeOutput = inu_node__removeOutput;
}

static void inu_node__vtable_init()
{
   if (!_bool_vtable_initialized) 
   {
      inu_node__vtableInitDefaults(&_vtable);
      _bool_vtable_initialized = true;
   }
}

const inu_node__VTable *inu_node__vtable_get(void)
{
   inu_node__vtable_init();
   return &_vtable;
}

ERRG_codeE inu_node__bindNodes(inu_nodeH inputH, inu_nodeH outputH)
{
   ERRG_codeE ret = INU_NODE__RET_SUCCESS;
   inu_node *output = (inu_node*)outputH;
   inu_node *input = (inu_node*)inputH;
#if DEFSG_IS_HOST
   inu_ref__container container;
#endif

   ((inu_node__VTable*)output->ref.p_vtable)->p_newInput(output, input);
   ((inu_node__VTable*)input->ref.p_vtable)->p_newOutput(input, output);

#if DEFSG_IS_HOST
   container.id = ((inu_node*)outputH)->ref.id;
   ret = inu_ref__sendCtrlSync((inu_ref*)inputH, INTERNAL_CMDG_BIND_NODES_E, &container, INU_REF__SYNC_DFLT_TIMEOUT_MSEC);
#endif
   return ret;
}

inu_nodeH inu_node__getNextInList(inu_node__edge *edgeListHead , inu_node *node)
{
   inu_node__edge *runner = edgeListHead;
   //if no nodes connected
   if (!runner)
   {
      return NULL;
   }

   //first node
   if (node == NULL)
   {
      return runner->node;
   }

   while(runner)
   {
      if(runner->node == node)
      {
         if(!runner->next)
         {
            return NULL;
         }
         else
         {
            return runner->next->node;
         }
      }
      runner = runner->next;
   }
   return NULL;
}

inu_nodeH inu_node__getNextOutputNode(inu_nodeH meH, inu_nodeH currentH)
{
   inu_node__privData *privP = (inu_node__privData*)((inu_node*)meH)->privP;
#ifdef LINK_LIST_CONNECTIVITY
   return inu_node__getNextInList(privP->outputNodes,(inu_node*)currentH);   
#else
   int i;
   if (currentH == NULL)
      return privP->outputNodesList[0];

   for (i = 0; i < INU_NODE_MAX_NUM_NODES - 1; i++)
   {
      if (privP->outputNodesList[i] == currentH)
      {
         return privP->outputNodesList[i+1];
      }
   }
   return NULL;   
#endif
}

inu_nodeH inu_node__getNextInputNode(inu_nodeH meH, inu_nodeH currentH)
{
   inu_node__privData *privP = (inu_node__privData*)((inu_node*)meH)->privP;
#ifdef LINK_LIST_CONNECTIVITY
   return inu_node__getNextInList(privP->inputNodes, (inu_node*)currentH);
#else
   int i;
   if (currentH == NULL)
      return privP->inputNodesList[0];

   for (i = 0; i < INU_NODE_MAX_NUM_NODES - 1; i++)
   {
      if (privP->inputNodesList[i] == currentH)
      {
         return privP->inputNodesList[i + 1];
      }
   }
   return NULL;   
#endif
}

unsigned int inu_node__getNumOutputs(inu_node *me)
{
   inu_node__privData *privP = (inu_node__privData*)me->privP;
   int numOutputs = 0;
#ifdef LINK_LIST_CONNECTIVITY
   inu_node__edge *runner;
   runner = privP->outputNodes;
   while(runner)
   {
      numOutputs++;
      runner = runner->next;
   }
#else
   int i;


   for (i = 0; i < INU_NODE_MAX_NUM_NODES - 1; i++)
   {
      if (privP->outputNodesList[i])
      {
         numOutputs++;
      }
   }
#endif
   return numOutputs;
}

inu_graphH inu_node__getGraph(inu_nodeH meH)
{
   inu_node__privData *privP = (inu_node__privData*)((inu_node*)meH)->privP;
   return (privP->graphP);
}

void inu_node__insertGraph(inu_nodeH meH,inu_graphH graphH)
{
   inu_node__privData *privP = (inu_node__privData*)((inu_node*)meH)->privP;
   privP->graphP = graphH;
}

void inu_node__removeGraph(inu_nodeH meH)
{
   inu_node__privData *privP = (inu_node__privData*)((inu_node*)meH)->privP;
   privP->graphP = NULL;
}
