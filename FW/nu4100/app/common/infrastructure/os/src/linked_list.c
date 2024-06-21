/*linked_list.c
   Implementation of a linked list abstract data type.
*/
#include "inu_types.h"
#include "linked_list.h"
#include "stdio.h"

/****************************************************************************
*
*  Function Name: addNode
*
*  Description: adding a node to a linked list
*
*  Inputs: pointer to a node in the linked list and the size to be alloced 
*
*  Returns: 
*
****************************************************************************/
void addNode(Node *current, int size)
{
    current->next       = (Node*)malloc(sizeof(Node));
    current->next->val  = (UINT8 *)malloc(size);
}

/****************************************************************************
*
*  Function Name: deleteNode
*
*  Description: deleting a node from the linked list
*
*  Inputs: pointer to a node in the linked list
*
*  Returns: 
*
****************************************************************************/
void deleteNode(Node *current)
{
    Node *temp;
    // Now pointer points to a node and the node next to it has to be removed
    temp = current->next;
    //temp points to the node which has to be removed
    current->next = temp->next;
    //We removed the node which is next to the pointer (which is also temp)
    free(temp);
}