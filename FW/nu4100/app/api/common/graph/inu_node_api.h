#ifndef _INU_NODE_API_H_
#define _INU_NODE_API_H_

ERRG_codeE inu_node__bindNodes(inu_nodeH inputH, inu_nodeH outputH);
inu_nodeH inu_node__getNextOutputNode(inu_nodeH meH, inu_nodeH current);
inu_nodeH inu_node__getNextInputNode(inu_nodeH meH, inu_nodeH current);
inu_graphH inu_node__getGraph(inu_nodeH meH);

#endif //_INU_NODE_API_H_