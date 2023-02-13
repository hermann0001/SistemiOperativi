#ifndef __QUEUE_H__
#define __QUEUE_H__

typedef struct coda{
	int head, tail;
	unsigned length;
	int* elementi;
}QUEUE; 

QUEUE* new_queue();
void enqueue(QUEUE* coda, int x);
int dequeue(QUEUE* coda);
int isEmpty(QUEUE* coda);
void emptyQueue(QUEUE* coda);
void stampa(QUEUE* coda);

#endif