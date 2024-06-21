#include "inu_common.h"

/*
* Adjacency matrix implementation of a directed/undirected graphs.
*/
#include "graph.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define _EDGE        (1)
#define _NO_EDGE     (0)
#define _MARKED_EDGE (2)

#define GRAPHP_IS_EDGE(e)           (e==_EDGE)
#define GRAPHP_IS_NO_EDGE(e)        (e==_NO_EDGE)
#define GRAPHP_IS_MARKED_EDGE(e)   (e==_MARKED_EDGE)

#define GRAPHP_SET_EDGE(e)   (e=_EDGE) 
#define GRAPHP_CLEAR_EDGE(e) (e=_NO_EDGE) 
#define GRAPHP_MARK_EDGE(e)  (e=_MARKED_EDGE) 

typedef struct
{
   void *usr;
   int dfs_marked;
   int dfs_onPath;
} GRAPHP_vertexT;

typedef struct
{
   unsigned int n;
   int directed;
   int **adjm;
   GRAPHP_vertexT *vlist;
   unsigned int e;

   unsigned int *dfs_path;
   unsigned int dfs_pathLen;
} GRAPHP_graphT;

/*
* create a graph
*/
GRAPHG_handleT GRAPHG_create(unsigned int n, int directed)
{
   unsigned int i;
   GRAPHP_graphT *g;
   
   g = (GRAPHP_graphT *)malloc(sizeof(GRAPHP_graphT));
   if(!g)
      goto err;
   
   g->n = n;
   g->e = 0;
   g->directed = directed;
   g->vlist = (GRAPHP_vertexT *)calloc(n,sizeof(GRAPHP_vertexT));
   g->adjm = (int **)calloc(n,sizeof(int *));
   if(!g->adjm)
      goto err;

   for(i=0; i<n; i++)
   {
      g->adjm[i] = (int *)calloc(n,sizeof(int));
      if(!g->adjm[i])
         goto err;
   }

   g->dfs_path = (unsigned int *)calloc(n,sizeof(unsigned int));
   if(!g->dfs_path)
         goto err;

   return g;

err:
   GRAPHG_delete(g);
   return NULL;
}
/*
delete a graph
*/
int GRAPHG_delete(GRAPHG_handleT gh)
{
   GRAPHP_graphT *g = (GRAPHP_graphT *)gh;
   unsigned int i;

   if(g)
   {
      if(g->vlist)
      {
         free(g->vlist);
         g->vlist = NULL;
      }

      if(g->adjm)
      {
         for(i=0; i<g->n; i++)
         {
            if(g->adjm[i])
            {
               free(g->adjm[i]);
               g->adjm[i] = NULL;
            }
         }
         free(g->adjm);
         g->adjm = NULL;
      }

      if(g->dfs_path)
      {
         free(g->dfs_path);
         g->dfs_path = NULL;
      }

      free(g);
   }

   return GRAPHG_OK;
}

GRAPHG_handleT GRAPHG_duplicate(GRAPHG_handleT gh)
{
   GRAPHP_graphT *orig = (GRAPHP_graphT *) gh;
   GRAPHP_graphT *dup;
   unsigned int i,j;

   dup = (GRAPHP_graphT *)GRAPHG_create(orig->n,orig->directed);
   dup->e = orig->e;   
   for(i=0; i< orig->n;i++)
   {
      memcpy(&dup->vlist[i], &orig->vlist[i],sizeof(GRAPHP_vertexT));
      for(j=0;j<orig->n;j++)
         dup->adjm[i][j] = orig->adjm[i][j];
   }
   return dup;
}
/*
* set user data in vertex
*/
int GRAPHG_setVertex(GRAPHG_handleT gh, unsigned int id, void *usr)
{
   GRAPHP_graphT *g = (GRAPHP_graphT *)gh;

   if(id >= g->n)
      return GRAPHG_ERR;

   g->vlist[id].usr = usr;

   return GRAPHG_OK;
}

int GRAPHG_getVertex(GRAPHG_handleT gh, unsigned int id, void **usr)
{
   GRAPHP_graphT *g = (GRAPHP_graphT *)gh;

   if(id >= g->n)
      return GRAPHG_ERR;

   *usr = g->vlist[id].usr;

   return GRAPHG_OK;
}
/*
* add an edge
*/
int GRAPHG_addEdge(GRAPHG_handleT gh, unsigned int id1, unsigned int id2)
{
   GRAPHP_graphT *g = (GRAPHP_graphT *)gh;

   if((id1 >= g->n) || (id2 >= g->n))
      return GRAPHG_ERR;

   GRAPHP_SET_EDGE(g->adjm[id1][id2]); //id1->id2
   if(!g->directed)
      GRAPHP_SET_EDGE(g->adjm[id2][id1]);
   
   g->e++;

   return GRAPHG_OK;
}
int GRAPHG_hasEdge(GRAPHG_handleT gh, unsigned int id1, unsigned int id2)
{
   GRAPHP_graphT *g = (GRAPHP_graphT *)gh;

   if((id1 >= g->n) || (id2 >= g->n))
      return 0;
   
   return (GRAPHP_IS_EDGE(g->adjm[id1][id2]));
}
int GRAPHG_remEdge(GRAPHG_handleT gh, unsigned int id1, unsigned int id2)
{
   GRAPHP_graphT *g = (GRAPHP_graphT *)gh;

   if((id1 >= g->n) || (id2 >= g->n))
      return GRAPHG_ERR;

   GRAPHP_CLEAR_EDGE(g->adjm[id1][id2]);
   if(!g->directed)
      GRAPHP_CLEAR_EDGE(g->adjm[id2][id1]);

   g->e--;

   return GRAPHG_OK;
}

int GRAPHG_markEdge(GRAPHG_handleT gh, unsigned int id1, unsigned int id2)
{
   GRAPHP_graphT *g = (GRAPHP_graphT *)gh;

   if((id1 >= g->n) || (id2 >= g->n))
      return GRAPHG_ERR;

   if(GRAPHP_IS_EDGE(g->adjm[id1][id2]))
   {
       GRAPHP_MARK_EDGE(g->adjm[id1][id2]);
   }

   return GRAPHG_OK;
}

int GRAPHG_unmarkEdge(GRAPHG_handleT gh, unsigned int id1, unsigned int id2)
{
   GRAPHP_graphT *g = (GRAPHP_graphT *)gh;

   if((id1 >= g->n) || (id2 >= g->n))
      return GRAPHG_ERR;

   if(GRAPHP_IS_MARKED_EDGE(g->adjm[id1][id2]))
   {
       GRAPHP_SET_EDGE(g->adjm[id1][id2]);
   }
   return GRAPHG_OK;
}

int GRAPHG_remOutEdges(GRAPHG_handleT gh, unsigned int id)
{
   GRAPHP_graphT *g = (GRAPHP_graphT *)gh;
   unsigned int i;
   unsigned int cnt = 0;

   if(id >= g->n)
      return GRAPHG_ERR;

   for(i = 0; i < g->n; i++)
   {
      if(GRAPHP_IS_EDGE(g->adjm[id][i]))
      {
         GRAPHP_CLEAR_EDGE(g->adjm[id][i]);
         if(!g->directed)
            GRAPHP_CLEAR_EDGE(g->adjm[i][id]); 
         g->e--;
         cnt++;
      }
   }
   return cnt;
}

int GRAPHG_remInEdges(GRAPHG_handleT gh, unsigned int id)
{
   GRAPHP_graphT *g = (GRAPHP_graphT *)gh;
   unsigned int i;
   unsigned int cnt = 0;

   if(id >= g->n)
      return cnt;

   for(i = 0; i < g->n; i++)
   {
      if(GRAPHP_IS_EDGE(g->adjm[i][id]))
      {
         GRAPHP_CLEAR_EDGE(g->adjm[i][id]);
         if(!g->directed)
            GRAPHP_CLEAR_EDGE(g->adjm[id][i]);
         g->e--;
         cnt++;
      }
   }
   return cnt;
}
int GRAPHG_remEdges(GRAPHG_handleT gh, unsigned int id, int dir)
{
   if(dir == GRAPHG_DIR_IN)
      return GRAPHG_remInEdges(gh,id);
   else
      return GRAPHG_remOutEdges(gh,id);
}

int GRAPHG_inDegree(GRAPHG_handleT gh, unsigned int id)
{
   GRAPHP_graphT *g = (GRAPHP_graphT *)gh;
   unsigned int i;
   unsigned int inDegree = 0;

   if(id >= g->n)
      return 0;

   for(i = 0; i < g->n; i++)
   {
      if(GRAPHP_IS_EDGE(g->adjm[i][id]))
         inDegree++;
   }

   return inDegree;
}

int GRAPHG_outDegree(GRAPHG_handleT gh, unsigned int id)
{
   GRAPHP_graphT *g = (GRAPHP_graphT *)gh;
   unsigned int i;
   unsigned int outDegree = 0;

   if(id >= g->n)
      return 0;

   for(i = 0; i < g->n; i++)
   {
      if(GRAPHP_IS_EDGE(g->adjm[id][i]))
         outDegree++;
   }

   return outDegree;
}

int GRAPHG_degree(GRAPHG_handleT gh, unsigned int id, int dir)
{
   if(dir == GRAPHG_DIR_IN)
      return GRAPHG_inDegree(gh,id);
   else
      return GRAPHG_outDegree(gh,id);
}

int GRAPHG_iterateOut(GRAPHG_handleT gh, unsigned int id, GRAPHG_vCb func, void *arg)
{
   GRAPHP_graphT *g = (GRAPHP_graphT *)gh;
   unsigned int i;
   unsigned int outDegree = 0;

   if((id >= g->n) || (!func))
      return 0;

   for(i = 0; i < g->n; i++)
   {
      if(GRAPHP_IS_EDGE(g->adjm[id][i]))
      {
         func(i, GRAPHG_DIR_IN, arg);
         outDegree++;
      }
   }
    
   return outDegree;
}
int GRAPHG_iterateIn(GRAPHG_handleT gh, unsigned int id, GRAPHG_vCb func, void *arg)
{
   GRAPHP_graphT *g = (GRAPHP_graphT *)gh;
   unsigned int i;
   unsigned int inDegree = 0;

   if((id >= g->n) || (!func))
      return 0;

   for(i = 0; i < g->n; i++)
   {
      if(GRAPHP_IS_EDGE(g->adjm[i][id]))
      {
         func(i, GRAPHG_DIR_OUT, arg);
         inDegree++;
      }
   }
    
   return inDegree;
}

int GRAPHG_iterate(GRAPHG_handleT gh, unsigned int id, GRAPHG_vCb func, void *arg, int dir)
{
   if(dir == GRAPHG_DIR_IN)
      return GRAPHG_iterateIn(gh,id,func,arg);
   else
      return GRAPHG_iterateOut(gh,id,func,arg);
}

static void dfsInit(GRAPHG_handleT gh)
{
   GRAPHP_graphT *g = (GRAPHP_graphT *)gh;
   unsigned int i;
   
   for(i = 0; i < g->n; i++)
      g->vlist[i].dfs_marked = 0;
}

static void dfs(GRAPHG_handleT gh, unsigned int v, GRAPHG_dfsCb cb, void *arg)
{
   GRAPHP_graphT *g = (GRAPHP_graphT *)gh;
   unsigned int i;
   
   g->vlist[v].dfs_marked = 1;
   cb(v, arg);

   for(i = 0; i < g->n; i++)
   {
      if(GRAPHP_IS_EDGE(g->adjm[v][i]))
      {
         if(!g->vlist[i].dfs_marked)
            dfs(gh, i, cb, arg);
      }
   }
}

/*
  DFS on the graph. Not reentrant. For directed graphs.
*/
int GRAPHG_dfs(GRAPHG_handleT gh, unsigned int source, GRAPHG_dfsCb cb, void *arg)
{
   GRAPHP_graphT *g = (GRAPHP_graphT *)gh;
   
   if((source >= g->n) || (!cb) || (!g->directed))
      return GRAPHG_ERR;

   dfsInit(gh);
   dfs(gh,source,cb,arg);

   return GRAPHG_OK;
}

static void dfsAllPathsInit(GRAPHG_handleT gh)
{
   GRAPHP_graphT *g = (GRAPHP_graphT *)gh;
   unsigned int i;
   
   g->dfs_pathLen = 0;
   for(i = 0; i < g->n; i++)
      g->vlist[i].dfs_onPath = 0;
}

static void dfsAllPaths(GRAPHG_handleT gh, unsigned int v, unsigned int t, GRAPHG_pathCb cb, void *arg)
{
   GRAPHP_graphT *g = (GRAPHP_graphT *)gh;
   unsigned int i;
   
   //add v to current path
   g->dfs_path[g->dfs_pathLen++] = v;
   g->vlist[v].dfs_onPath = 1;

   if(v == t)
   {
      //end of path - invoke callback and reset path len
      cb(g->dfs_pathLen,g->dfs_path,arg);
   }
   else
   {
      //dfs over all out neighbours
      for(i = 0; i < g->n; i++)
      {
         if(GRAPHP_IS_EDGE(g->adjm[v][i]))
         {
            if(!g->vlist[i].dfs_onPath)
               dfsAllPaths(gh, i, t, cb, arg);
         }
      }
   }
   //finished dfs from v, need to remove from path 
   if(g->dfs_pathLen > 0)
      g->dfs_pathLen--;
   g->vlist[v].dfs_onPath = 0;
}

/*
  Find all simple paths from s to t.
  Call user callback for each path on from s to t in the graph. 
  Not reentrant. For directed graphs.
*/
int GRAPHG_dfsAllPaths(GRAPHG_handleT gh, unsigned int s, unsigned int t, GRAPHG_pathCb cb, void *arg)
{
   GRAPHP_graphT *g = (GRAPHP_graphT *)gh;

   if((s >= g->n) || (t >= g->n) || (!g->directed))
      return GRAPHG_ERR;

   dfsAllPathsInit(gh);
   dfsAllPaths(gh,s,t,cb, arg);

   return GRAPHG_OK;
}

int GRAPHG_markInEdges(GRAPHG_handleT gh, unsigned int id)
{
   GRAPHP_graphT *g = (GRAPHP_graphT *)gh;
   unsigned int i;
   unsigned int inDegree = 0;

   if(id >= g->n)
      return 0;

   for(i = 0; i < g->n; i++)
   {
      if(GRAPHP_IS_EDGE(g->adjm[i][id]))
      {
         GRAPHP_MARK_EDGE(g->adjm[i][id]);
         inDegree++;
      }
   }

   return inDegree;
}

int GRAPHG_markOutEdges(GRAPHG_handleT gh, unsigned int id)
{
   GRAPHP_graphT *g = (GRAPHP_graphT *)gh;
   unsigned int i;
   unsigned int outDegree = 0;

   if(id >= g->n)
      return 0;

   for(i = 0; i < g->n; i++)
   {
      if(GRAPHP_IS_EDGE(g->adjm[id][i]))
      {
         GRAPHP_MARK_EDGE(g->adjm[id][i]);
         outDegree++;
      }
   }

   return outDegree;
}

int GRAPHG_unmarkInEdges(GRAPHG_handleT gh, unsigned int id)
{
   GRAPHP_graphT *g = (GRAPHP_graphT *)gh;
   unsigned int i;
   unsigned int inDegree = 0;

   if(id >= g->n)
      return 0;

   for(i = 0; i < g->n; i++)
   {
      if(GRAPHP_IS_MARKED_EDGE(g->adjm[i][id]))
      {
         GRAPHP_SET_EDGE(g->adjm[i][id]);
         inDegree++;
      }
   }

   return inDegree;
}

int GRAPHG_unmarkOutEdges(GRAPHG_handleT gh, unsigned int id)
{
   GRAPHP_graphT *g = (GRAPHP_graphT *)gh;
   unsigned int i;
   unsigned int outDegree = 0;

   if(id >= g->n)
      return 0;

   for(i = 0; i < g->n; i++)
   {
      if(GRAPHP_IS_MARKED_EDGE(g->adjm[id][i]))
      {
         GRAPHP_SET_EDGE(g->adjm[id][i]);
         outDegree++;
      }
   }

   return outDegree;
}

/*
Mark edges so they are not erased but are not counted in degree or iteration.
* 
*/
int GRAPHG_markEdges(GRAPHG_handleT gh, unsigned int id, int dir)
{
   if(dir == GRAPHG_DIR_IN)
      return GRAPHG_markInEdges(gh,id);
   else
      return GRAPHG_markOutEdges(gh,id);
}

int GRAPHG_unmarkEdges(GRAPHG_handleT gh, unsigned int id, int dir)
{
   if(dir == GRAPHG_DIR_IN)
      return GRAPHG_unmarkInEdges(gh,id);
   else
      return GRAPHG_unmarkOutEdges(gh,id);
}

unsigned int GRAPHG_getNumV(GRAPHG_handleT gh)
{
   return ((GRAPHP_graphT *)gh)->n;
}

unsigned int GRAPHG_getNumE(GRAPHG_handleT gh)
{
   return ((GRAPHP_graphT *)gh)->e;
}

void GRAPHG_show(GRAPHG_handleT gh)
{
   GRAPHP_graphT *g = (GRAPHP_graphT *)gh;
   unsigned int i,j;
   printf("-------------------\n");
   printf("graph handle=%p: n=%u e=%u %s\n", gh, g->n, g->e, (g->directed)? "DIRECTED":"UNDIRECTED");
   printf("adjacency matrix:\n    ");
   for(i = 0; i < g->n; i++)
      printf("[%2u]  ",i);
   for(i = 0; i < g->n; i++)
   {
      printf("\n[%2u]  ",i);  
      for(j = 0; j < g->n; j++)
         printf("%2u    ", g->adjm[i][j]);
   }
   /*printf("\nuser data:\n");
   for(i = 0; i < g->n;  i++)
      printf("[%u]0x%x\n", i, (UINT32)g->vlist[i].usr);*/
   printf("\n-------------------\n");
}

//#define INTERNAL_TEST
#ifdef INTERNAL_TEST

#define graph_printf(level, fmt,  ...) \
   do { \
      if (level > verbose_level) \
         break; \
      printf(fmt, ##__VA_ARGS__); \
   } while(0)

#include <time.h>
#define MAX_VERTICES (16)
#define MAX_NUM_VERTICES (5)
#define MAX_NUM_EDGES (20)
#define DIRECTED (1)

#define VERBOSE_HIGH 2
#define VERBOSE_MED  1
#define VERBOSE_LOW  0
static int verbose_level = VERBOSE_LOW;

static void GRAPHG_iterator(unsigned int id, int dir, void *arg)
{
    graph_printf(VERBOSE_HIGH, "  iterator: [%d]arg=%d dir=%d\n", id,(unsigned int)arg,dir);
}

int basic_graph_test(unsigned int maxv, unsigned int maxe, int dir)
{
   GRAPHG_handleT gh = NULL;
   unsigned int n;
   unsigned int i;
   unsigned int *test_data;   
   int ret;

   struct my_edge
   {
      unsigned int id1;
      unsigned int id2;
   };
   struct my_edge *edge_list = NULL;

   n = maxv;
   graph_printf(VERBOSE_LOW, "graph_test: creating graph with %d vertices %s\n", n, (dir)?"directed" : "undirected");

   //allocate user data
   test_data = (unsigned int *)calloc(n, sizeof(unsigned int));
   if(!test_data)
      goto test_err;
   for(i = 0; i < n; i++)
      test_data[i] = i+1000;
   //allocate edge list
   edge_list = (struct my_edge *)calloc(maxe, sizeof(struct my_edge));
   if(!edge_list)
      goto test_err;

   //create a graph 
   gh = GRAPHG_create(n , dir);
   if(!gh)
      goto test_err;
   
   //Initialize vertices with user data
   for(i = 0; i < n; i++)
   {
      ret = GRAPHG_setVertex(gh, i, &test_data[i]);
      if(ret != GRAPHG_OK)
         goto test_err;
   }
   graph_printf(VERBOSE_HIGH, "graph_test: set vertices with user data\n");
   if(verbose_level >= VERBOSE_MED)
      GRAPHG_show(gh);   

   //Add random edges   
   for(i = 0; i < maxe; i++)
   {
      unsigned int id1, id2;
      id1 = rand() % n;   
      id2 = rand() % n;

      ret = GRAPHG_addEdge(gh,id1,id2);
      if(ret != GRAPHG_OK)
         goto test_err;

      graph_printf(VERBOSE_HIGH,"graph_test: added edge %d->%d\n", id1,id2);
      if(!GRAPHG_hasEdge(gh,id1,id2))
      {
         graph_printf(VERBOSE_MED,"graph_test:: error GRAPHG_hasEdge() returned false for %d->%d\n", id1, id2);
         goto test_err;
      }
      
      ret = GRAPHG_remEdge(gh,id1,id2);
      if(ret != GRAPHG_OK)
         goto test_err;
      if(GRAPHG_hasEdge(gh,id1,id2))
      {
         graph_printf(VERBOSE_MED,"graph_test:: error deleted edge and GRAPHG_hasEdge() returned true for %d->%d\n", id1, id2);
         goto test_err;
      }
      
      //add it back
      ret = GRAPHG_addEdge(gh,id1,id2);
      if(ret != GRAPHG_OK)
         goto test_err;

      edge_list[i].id1 = id1;
      edge_list[i].id2 = id2;
   }
   
   graph_printf(VERBOSE_MED,"added %d random edges\n", GRAPHG_getNumE(gh));
   if(verbose_level >= VERBOSE_HIGH)
      GRAPHG_show(gh);
   
   //Test deleting edges
   for(i=0;i<n;i++)
   {
      int degree = GRAPHG_degree(gh,i,GRAPHG_DIR_IN);
      int cnt=GRAPHG_remInEdges(gh,i);
      if(cnt != degree)
      {
         graph_printf(VERBOSE_MED, "graph_test: error in_degree differs from num deleted in edges for vertex %d\n",i);
         goto test_err;
      }
      else
      {
         graph_printf(VERBOSE_HIGH, "graph_test: deleted %d in_edges from vertex %d\n", cnt,i);
      }
      if(GRAPHG_degree(gh,i,GRAPHG_DIR_IN))
      {
         graph_printf(VERBOSE_MED, "graph_test: error GRAPHG_inDegree() returned %d(not 0)\n",GRAPHG_degree(gh,i,GRAPHG_DIR_IN));
         goto test_err;
      }
      degree = GRAPHG_degree(gh,i,GRAPHG_DIR_OUT);
      cnt=GRAPHG_remEdges(gh,i,GRAPHG_DIR_OUT);
      if(cnt != degree)
      {
         graph_printf(VERBOSE_MED, "graph_test: error out_degree differs from num deleted in edges for vertex %d\n",i);
         goto test_err;
      }
      else
      {
         graph_printf(VERBOSE_HIGH, "graph_test: deleted %d out_edges from vertex %d\n", cnt,i);
      }

      if(GRAPHG_degree(gh,i,GRAPHG_DIR_OUT))

      {
         graph_printf(VERBOSE_MED, "graph_test: GRAPHG_outDegree() returned %d(not 0)\n",GRAPHG_degree(gh,i,GRAPHG_DIR_OUT));
         goto test_err;
      }
   }

   if(verbose_level >= VERBOSE_HIGH)
      GRAPHG_show(gh);

   //Add back all the edges
   graph_printf(VERBOSE_HIGH,"graph_test: adding back all edges\n");
   for(i=0;i<maxe;i++)
   {
      GRAPHG_addEdge(gh, edge_list[i].id1, edge_list[i].id2);
   }
   if(verbose_level >= VERBOSE_HIGH)
      GRAPHG_show(gh);

   //test iterating through in/out edges
   for(i=0;i<n;i++)
   {
      int cnt;      
      cnt = GRAPHG_outDegree(gh,i);
      if(cnt)
      {
         graph_printf(VERBOSE_HIGH,"graph_test: iterating %i out\n",i);
         if(GRAPHG_iterate(gh,i,GRAPHG_iterator,(void *)i, GRAPHG_DIR_OUT) != cnt)
         {
            graph_printf(VERBOSE_MED,"graph_test: error out_degree differs from iterator count\n");
            goto test_err;
         }
      }
      cnt = GRAPHG_inDegree(gh,i);
      if(cnt)
      {
         graph_printf(VERBOSE_HIGH,"graph_test: iterating %i in\n",i);
         if(GRAPHG_iterate(gh,i,GRAPHG_iterator,(void *)i, GRAPHG_DIR_IN) != cnt)
         {
            graph_printf(VERBOSE_MED,"graph_test: error in_degree differs from iterator count\n");
            goto test_err;
         }
      }
   }

   if(test_data)
      free(test_data);
   if(edge_list)
      free(edge_list);
   GRAPHG_delete(gh);
   return GRAPHG_OK;

test_err:
   if(test_data)
      free(test_data);
   if(edge_list)
      free(edge_list);
   GRAPHG_delete(gh);
   return GRAPHG_ERR;
}

static unsigned int test_arg;

static void test_dfs_cb(unsigned int id, void *arg)
{
   graph_printf(VERBOSE_LOW, "dfs: %d\n",id);
   if(arg)
      (*(unsigned int *)arg)++;
}

static void test_dfs_allpaths_cb(unsigned int len, unsigned int *path, void *arg)
{
   unsigned int i;
   if(arg)
      (*(unsigned int *)arg)++;

   graph_printf(VERBOSE_LOW, "dfs path:");
   for(i = 0; i < len; i++)
   {
      graph_printf(VERBOSE_LOW, "->%d",path[i]);
   }
   graph_printf(VERBOSE_LOW, "\n");
}

int dfs_graph_test(unsigned int maxv, unsigned int maxe)
{
   GRAPHG_handleT gh = NULL;
   unsigned int n;
   unsigned int i;
   unsigned int *test_data;   
   int ret;
   int dir = DIRECTED;

   n = maxv;
   graph_printf(VERBOSE_LOW, "graph_test: creating graph with %d vertices %s\n", n, (dir)?"directed" : "undirected");

   //allocate user data
   test_data = (unsigned int *)calloc(n, sizeof(unsigned int));
   if(!test_data)
      goto test_err;
   for(i = 0; i < n; i++)
      test_data[i] = i+1000;

   //create a graph 
   gh = GRAPHG_create(n , dir);
   if(!gh)
      goto test_err;
   
   //Initialize vertices with user data
   for(i = 0; i < n; i++)
   {
      ret = GRAPHG_setVertex(gh, i, &test_data[i]);
      if(ret != GRAPHG_OK)
         goto test_err;
   }
   graph_printf(VERBOSE_HIGH, "graph_test: set vertices with user data\n");
   if(verbose_level >= VERBOSE_MED)
      GRAPHG_show(gh);

   //Add random edges   
   for(i = 0; i < maxe; i++)
   {
      unsigned int id1, id2;
      id1 = rand() % n;   
      id2 = rand() % n;
      if(id1 == id2)
         continue;

      ret = GRAPHG_addEdge(gh,id1,id2);
      if(ret != GRAPHG_OK)
         goto test_err;
   }
   
   graph_printf(VERBOSE_MED,"added %d random edges\n", GRAPHG_getNumE(gh));
   GRAPHG_show(gh);
   
   //Run dfs on the random graph - check by output
   test_arg = 0;
   ret = GRAPHG_dfs(gh,0, test_dfs_cb, &test_arg);
   if(ret != GRAPHG_OK)
       goto test_err;
   graph_printf(VERBOSE_LOW,"dfs visited %d vertices\n",test_arg);
   //again
   test_arg = 0;
   ret = GRAPHG_dfs(gh,1, test_dfs_cb, &test_arg);
   if(ret != GRAPHG_OK)
       goto test_err;
   graph_printf(VERBOSE_LOW,"dfs visited %d vertices\n",test_arg);

#if 1
   //dfs on 2 paths
   test_arg = 0;
   ret = GRAPHG_dfsAllPaths(gh, 0, n-1, test_dfs_allpaths_cb, &test_arg);
   if(ret != GRAPHG_OK)
        goto test_err;
   graph_printf(VERBOSE_LOW,"dfs allpaths found %d paths from 0 to %d\n",test_arg, n-1);

   test_arg =0;
   ret = GRAPHG_dfsAllPaths(gh, n/2, n/2+1, test_dfs_allpaths_cb, &test_arg);
   if(ret != GRAPHG_OK)
        goto test_err;
   graph_printf(VERBOSE_LOW,"dfs allpaths found %d paths from %d to %d\n",test_arg, n/2,n/2+1);
#endif

   if(test_data)
      free(test_data);
   GRAPHG_delete(gh);
   return GRAPHG_OK;

test_err:
   if(test_data)
      free(test_data);
   GRAPHG_delete(gh);
   return GRAPHG_ERR;
}

int graph_tester(void)
{
   unsigned int n,e;
   int ret;
   
   srand((unsigned int)time(NULL));
   //n = (rand() % MAX_VERTICES) + 1;   
   //e = (rand() % n*n) + 1;
   n = MAX_NUM_VERTICES;
   e = MAX_NUM_EDGES;

   ret = basic_graph_test(n,e,DIRECTED);
   if(ret == GRAPHG_OK)
      graph_printf(VERBOSE_LOW,"basic graph test passed\n");
   else
      graph_printf(VERBOSE_LOW,"basic graph test failed\n");

   if(ret == GRAPHG_OK)
   {
      ret = dfs_graph_test(n,e);
      if(ret == GRAPHG_OK)
         graph_printf(VERBOSE_LOW,"dfs graph test ran - check output\n");
      else
         graph_printf(VERBOSE_LOW,"dfs graph test failed\n");
   }

   return ret;
}

#endif