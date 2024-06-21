
#include "inu_common.h"
#include "nucfg_priv.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*walkCbT)(NUCFG_socH nusoc, void *arg, nublkH blk0, unsigned int i, nublkH blk1, unsigned int j);

#define WALK_SKIP (1)
#define WALK_DO   (0)


typedef struct
{
   UINT16 track[NUCFG_NUM_BLKS_E];
   UINT16 cnt;
} walkT ;

typedef struct
{
   walkT walk; 
   GRAPHG_handleT graph;
} buildWalkT ;

static int findNodeId(NUCFG_socH nusoc, GRAPHG_handleT graph, nublkH blk)
{
   int ret = -1;
   unsigned int id;
   nuBlkTypeE type,currType;
   unsigned int inst,currInst;
   nublkH currblk;

   nusoc_getBlkTypeInst(nusoc, blk, &type,&inst);

   for(id = 0; id < GRAPHG_getNumV(graph);id++)
   {
      GRAPHG_getVertex(graph, id, &currblk);
      nusoc_getBlkTypeInst(nusoc, currblk, &currType,&currInst);
      if((currType == type) && (currInst == inst))
      {
         ret = id;
         break;
      }
   }
   return ret;
}

static void rwalk(NUCFG_socH nusoc, nublkH blk, walkCbT cb, void *arg)
{
   unsigned int i,j;
   nublkH src;
   unsigned int numInputs;
   int ret;

   numInputs = nusoc_getBlkNumInputs(nusoc, blk);
   for(i = 0; i < numInputs; i++)
   {
      src = NULL; j = 0;
      nusoc_getBlkInput(nusoc, blk, i, &src, &j);
      ret = cb(nusoc, arg, blk,i,src,j);
      //note: skipping src will skip sub-tree rooted at src
      if((src) && (ret == WALK_DO))
      {
         rwalk(nusoc, src,cb,arg);
      }
   }
}

static int track(walkT *walk, nuBlkTypeE type, unsigned int inst)
{
   int skip = WALK_SKIP;
   if(!(walk->track[type] & (1<<inst)))
   {
      walk->track[type] |= (1<<inst);
      skip = WALK_DO;
   }
   return skip;
}

static int blkCntCb(NUCFG_socH nusoc, void *arg, nublkH blk, unsigned int i, nublkH src, unsigned int j)
{
   (void)i;
   (void)j;
   walkT *walk = (walkT *)arg;
   nuBlkTypeE type;
   unsigned int inst;
   int skip = WALK_SKIP;

   nusoc_getBlkTypeInst(nusoc, blk, &type,&inst);
   skip = track(walk,type,inst);
   if(skip != WALK_SKIP)
   {
      walk->cnt++;
   }

   if(src)
   {
      nusoc_getBlkTypeInst(nusoc, src, &type,&inst);
      skip = track(walk,type,inst);
      if(skip != WALK_SKIP)
         walk->cnt++;
   }

   return skip;
}

static int buildGraphCb(NUCFG_socH nusoc, void *arg, nublkH blk, unsigned int i, nublkH src, unsigned int j)
{
   (void)i;
   (void)j;
   buildWalkT* build = (buildWalkT *)arg;
   nuBlkTypeE type;
   unsigned int inst;
   int skip = WALK_SKIP;
   unsigned int currNodeId;

   nusoc_getBlkTypeInst(nusoc, blk, &type,&inst);
   skip = track(&build->walk,type,inst);
   if(skip != WALK_SKIP)
   {
      //blk node not visted create vertex 
      GRAPHG_setVertex(build->graph, build->walk.cnt, blk);
      currNodeId = build->walk.cnt;
      build->walk.cnt++;
   }
   else
   {
      //blk node already visted -  find its id in the graph
      currNodeId = findNodeId(nusoc, build->graph,blk);
   }

   if(src)
   {
      nusoc_getBlkTypeInst(nusoc, src, &type,&inst);
      skip = track(&build->walk,type,inst);
      if(skip != WALK_SKIP)
      {
         //src node has not been visited - ceate vertex and add edge
         GRAPHG_setVertex(build->graph, build->walk.cnt, src);
         GRAPHG_addEdge(build->graph, currNodeId, build->walk.cnt);
         build->walk.cnt++;
      }
      else
      {
         //src node already visited - find its node id in the graph and add edge
         GRAPHG_addEdge(build->graph, currNodeId, findNodeId(nusoc, build->graph,src));
      }
   }
   return skip;
}

/*
   Create single graph for the requested output channel of the soc 
*/
ERRG_codeE nugraph_create(NUCFG_socH nusoc, unsigned int outnum, GRAPHG_handleT *gh)
{
   ERRG_codeE ret = NUCFG__RET_SUCCESS;
   nublkH outblk;
   GRAPHG_handleT graph = NULL;
   walkT walk;
   buildWalkT build;

   //start from the requested soc output
   outblk = nusoc_getOutput(nusoc, outnum);

   //walk soc backwards towards input to detemine number of nodes for the graph
   memset(&walk, 0, sizeof(walk));
   rwalk(nusoc, outblk, blkCntCb, &walk);
   
   //initialize a graph
   graph = GRAPHG_create(walk.cnt,GRAPHG_DIRECTED);
   if(graph)
   {
      //walk soc again - this time updating graph nodes/edges
      memset(&build.walk, 0, sizeof(build.walk));
      build.graph = graph;
      rwalk(nusoc, outblk, buildGraphCb, &build);

      *gh = graph;
      /*if(walk.cnt > 1)
      {
         nugraph_show(nusoc,graph);
      }*/
   }
   else
   {
      ret = NUCFG__ERR_OUT_OF_RSRCS;
   }

   return ret;

}

void nugraph_delete(GRAPHG_handleT gh)
{
   if(gh)
      GRAPHG_delete(gh);
}

void nugraph_show(NUCFG_socH nusoc, GRAPHG_handleT gh)
{
   unsigned int i;
   nublkH blk;
   char str[32];

   for(i = 0; i < GRAPHG_getNumV(gh); i++)
   {
      GRAPHG_getVertex(gh,i,&blk);
      nusoc_getBlkStr(nusoc, blk, str);
      LOGG_PRINT(LOG_INFO_E, NULL, "[%2d]: %s\n", i,str);
   }
   GRAPHG_show(gh);
}

#ifdef __cplusplus
}
#endif