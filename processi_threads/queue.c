#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

QUEUE* new_queue(){
	QUEUE* c = (QUEUE*)malloc(sizeof(QUEUE));
	c->length = 64;
	c->elementi = (int*)calloc(c->length, sizeof(int));
	c->head = 0;
	c->tail = 0;
	return c;
}

void enqueue(QUEUE* coda, int x){
	if(coda->head == coda->tail + 1 || (coda->head == 0 && (coda->tail == coda->length - 1))){
		printf("errore overflow della coda\n");
		exit(1);
	}

	coda->elementi[coda->tail] = x;
	coda->tail++;
	if(coda->tail == coda->length - 1)
		coda->tail = 0;
}

int dequeue(QUEUE* coda){
	if(coda->head == coda->tail){
		printf("coda vuota\n");
		exit(1);
	}
	else{
		int x = coda->elementi[coda->head];
		if(coda->head == coda->length-1)
			coda->head = 0;
		else
			coda->head++;
		return x;
	}
}

int isEmpty(QUEUE* coda){
	return coda->head == coda->tail;
}

void emptyQueue(QUEUE* coda){
	coda->head = coda->tail = 0;
}

void stampa(QUEUE* coda){
	for(int i = coda->head; i <= coda->tail; i++){
		printf("elemento in pos. %d: %d\n",i, coda->elementi[i]);
	}
}