/* linked_list.h
   Header file for linked list implementation
*/
#ifndef _LINKED_LIST_
#define _LINKED_LIST_

typedef struct Node 
{
    void *val;
    struct Node *next;
}Node;

void addNode(Node *current, int size);
void deleteNode(Node *current);

#endif
