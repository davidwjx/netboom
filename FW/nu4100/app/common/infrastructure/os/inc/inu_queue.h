
/*	queue.h

	Header file for queue implementation	
*/


#define QUEUESIZE       1000

typedef struct {
        void *q[QUEUESIZE+1];          /* body of queue */
        int first;                      /* position of first element */
        int last;                       /* position of last element */
        int count;                      /* number of queue elements */
} inu_queue;

void inu_init_queue(inu_queue *q);
void inu_enqueue(inu_queue *q, void *x);
void *inu_dequeue(inu_queue *q);
int inu_is_empty(inu_queue *q);


