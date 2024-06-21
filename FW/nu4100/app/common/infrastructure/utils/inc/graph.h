#ifndef __GRAPH__
#define __GRAPH__

#define GRAPHG_OK (0)
#define GRAPHG_ERR (-1)

#define GRAPHG_DIRECTED   (1)
#define GRAPHG_UNDIRECTED (0)

#define GRAPHG_DIR_IN (0)
#define GRAPHG_DIR_OUT (1)

typedef void *GRAPHG_handleT;

GRAPHG_handleT GRAPHG_create(unsigned int n, int directed);
int GRAPHG_delete(GRAPHG_handleT gh);
GRAPHG_handleT GRAPHG_duplicate(GRAPHG_handleT gh);
int GRAPHG_setVertex(GRAPHG_handleT gh, unsigned int id, void *usr);
int GRAPHG_getVertex(GRAPHG_handleT gh, unsigned int id, void **usr);
int GRAPHG_addEdge(GRAPHG_handleT gh, unsigned int id1, unsigned int id2);
int GRAPHG_hasEdge(GRAPHG_handleT gh, unsigned int id1, unsigned int id2);
int GRAPHG_remEdge(GRAPHG_handleT gh, unsigned int id1, unsigned int id2);
int GRAPHG_markEdge(GRAPHG_handleT gh, unsigned int id1, unsigned int id2);
int GRAPHG_unmarkEdge(GRAPHG_handleT gh, unsigned int id1, unsigned int id2);
int GRAPHG_remEdges(GRAPHG_handleT gh, unsigned int id, int dir);
int GRAPHG_markEdges(GRAPHG_handleT gh, unsigned int id, int dir);
int GRAPHG_unmarkEdges(GRAPHG_handleT gh, unsigned int id, int dir);
int GRAPHG_degree(GRAPHG_handleT gh, unsigned int id, int dir);
typedef void (*GRAPHG_vCb)(unsigned int id, int dir, void *arg);
int GRAPHG_iterate(GRAPHG_handleT gh, unsigned int id, GRAPHG_vCb func, void *arg, int dir);
typedef void (*GRAPHG_dfsCb)(unsigned int id, void *arg);
int GRAPHG_dfs(GRAPHG_handleT gh, unsigned int source, GRAPHG_dfsCb cb, void *arg);
typedef void (*GRAPHG_pathCb)(unsigned int len, unsigned int *path, void *arg);
int GRAPHG_dfsAllPaths(GRAPHG_handleT gh, unsigned int s, unsigned int t, GRAPHG_pathCb cb, void *arg);

unsigned int GRAPHG_getNumV(GRAPHG_handleT gh);
unsigned int GRAPHG_getNumE(GRAPHG_handleT gh);
void GRAPHG_show(GRAPHG_handleT gh);

#endif //__GRAPH__