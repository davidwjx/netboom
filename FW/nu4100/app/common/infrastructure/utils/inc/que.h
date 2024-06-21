/****************************************************************************
 *
 *   FileName: que.h
 *
 *   Author:  
 *
 *   Date: 
 *
 *   Description: simple que api
 *
 ****************************************************************************/
#ifndef _QUE_H_
#define _QUE_H_

#ifdef __cplusplus
      extern "C" {
#endif

/****************************************************************************
 ***************      I N C L U D E   F I L E S                 *************
 ****************************************************************************/

/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/
#define QUEG_MAX_ELEMENTS (16)

/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/
typedef const void * QUEG_elemT;
typedef struct
{
   unsigned int maxCnt; //must be less than max elements

   //internal - don't touch
   QUEG_elemT que[QUEG_MAX_ELEMENTS];
   unsigned int head;
   unsigned int tail;
   unsigned int cnt;
   
} QUEG_queT;

/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/
void QUEG_init(QUEG_queT *queP);
int QUEG_enque(QUEG_queT *queP,QUEG_elemT elem);
int QUEG_deque(QUEG_queT *queP, QUEG_elemT *elemP);
unsigned int QUEG_numElements(QUEG_queT *queP);
void QUEG_show(QUEG_queT *queP);


#ifdef __cplusplus
}
#endif

#endif // _QUE_H_

