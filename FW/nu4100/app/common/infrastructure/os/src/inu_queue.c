
/*   queue.c

   Implementation of a FIFO queue abstract data type.
*/

#include "inu_types.h"
#include "inu_queue.h"
#include "stdio.h"

#ifdef __cplusplus
extern "C" {
#endif
#if DEFSG_IS_HOST_ANDROID || DEFSG_IS_HOST_LINUX_DESKTOP
void inu_init_queue(inu_queue *q)
{
        q->first = 0;
        q->last = QUEUESIZE-1;
        q->count = 0;
}

void inu_enqueue(inu_queue *q, void *x)
{
   if (q->count >= QUEUESIZE)
      printf("Warning: inu_queue overflow enqueue x=%p\n",x);
   else
   {
      q->last = (q->last+1) % QUEUESIZE;
      q->q[ q->last ] = x;    
      q->count = q->count + 1;
   }
}

void * inu_dequeue(inu_queue *q)
{
   void * x=0;

   if (q->count <= 0) printf("Warning: empty queue dequeue.\n");
   else
   {
      x = q->q[ q->first ];
      q->first = (q->first+1) % QUEUESIZE;
      q->count = q->count - 1;
   }

   return(x);
}

int inu_is_empty(inu_queue *q)
{
        if (q->count <= 0) return (1);
        else return (0);
}

#endif
#ifdef __cplusplus
}
#endif

