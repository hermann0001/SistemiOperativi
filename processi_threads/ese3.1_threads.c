/*
Una applicazione ha p thread produttori e c thread consumatori. 
L’applicazione lavora in due fasi. Nella prima fase i thread produttori Pi inseriscono 
in una coda vuota c elementi. Nella seconda fase i thread consumatori Ci estraggono 
ciascuno un elemento dalla coda a partire da C1 a Cc. Le due fasi si alternano per un 
tempo infinito. Scrivere in pseudocodice il corpo delle funzioni eseguite da un generico 
thread Pi e Ci assumendo che:

la coda è inizialmente vuota;

l’implementazione disponibile della coda NON è thread safe;

è ammesso solo l’utilizzo di semafori;

non è ammesso utilizzare altre variabili condivise ad eccezione di eventuali semafori e 
della coda.
*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <time.h>
#include "queue.h"  				//la coda è implementata tramite array di interi e può contenere 128 elementi

#define abort(msg) do{printf(msg); exit(1);}while(0)
#define abort_arg(msg, arg) do{printf(msg,arg); exit(1);}while(0)

#define P 2
#define C 2
#define SECONDS 5
#define QUEUE_LENGTH 64
#define THREAD_SHARED 0
#define NUM_FASI 4

QUEUE* coda;

typedef struct t{
	int index;
	int* counter;			//counter for operations, if it's P it refers to enqueues, if it's consumer it refers to deuques
}THREAD_DATA;

sem_t cmut, pmut;
sem_t first_phase, second_phase;
sem_t cons_order[C];
//sem = 0 -> block; sem > 0 -> go!

pthread_barrier_t cbarrier;
pthread_barrier_t pbarrier;

void* prod_enqueue(void* arg);
void* cons_dequeue(void* arg);

int main(){
	/*creo una coda vuota*/
	coda = new_queue();
	emptyQueue(coda);

	pthread_t* prod = (pthread_t*)malloc(sizeof(pthread_t) * P);
	pthread_t* cons = (pthread_t*)malloc(sizeof(pthread_t) * C);
	THREAD_DATA* prod_data = (THREAD_DATA*)malloc(sizeof(THREAD_DATA) * P);
	THREAD_DATA* cons_data = (THREAD_DATA*)malloc(sizeof(THREAD_DATA) * C);
	
	int conta_creati = 0, conta_estratti = 0;

	sem_init(&cmut, THREAD_SHARED, 1);
	sem_init(&pmut, THREAD_SHARED, 1);
	sem_init(&first_phase, THREAD_SHARED, C);
	sem_init(&second_phase, THREAD_SHARED, 0);

	pthread_barrier_init(&pbarrier, NULL, P);
	pthread_barrier_init(&cbarrier, NULL, P);

	//init for producers
	for(int i = 0; i < P; i++){
		prod_data[i].index = i+1;
		prod_data[i].counter = &conta_creati;
	}

	//init for consumers
	for(int i = 0; i < C; i++){
		cons_data[i].index = i+1;
		cons_data[i].counter = &conta_estratti;
		sem_init(cons_order + i, THREAD_SHARED, (i == 0));			//only first cons has sem init to 1
		printf("sem no. %d inited with %d\n", i, i==0);
	}

	/*creazione thread produttori*/
	for(int i = 0; i < P; i++)
		pthread_create(prod + i, NULL, prod_enqueue, prod_data + i);	


	/*creazione thread consumatori*/
	for(int i = 0; i < C; i++)
		pthread_create(cons + i, NULL, cons_dequeue, cons_data + i);


	/*terminazione dei thread produttori*/
	for(int i = 0; i < P; i++)
		pthread_join(prod[i], NULL);
	
	/*terminazione thread consumatori*/
	for(int i = 0; i < C; i++)
		pthread_join(cons[i], NULL);

	sem_destroy(&first_phase); sem_destroy(&second_phase);
	sem_destroy(&pmut); sem_destroy(&cmut);

	pthread_barrier_destroy(&pbarrier);
	pthread_barrier_destroy(&cbarrier);

	free(prod_data); free(prod);
	free(cons_data); free(cons);
}

void* prod_enqueue(void* arg){
	THREAD_DATA* th = (THREAD_DATA*)arg;
	pthread_barrier_wait(&pbarrier);

	int i = 0;
	while(i < NUM_FASI){
		sem_wait(&first_phase);
		sem_wait(&pmut);			//producer mutual exclusion
		//START C.S
		int x = rand() % 50;
		enqueue(coda, x);
		printf("P%d, produced value: %d\n", th->index, x);
		__sync_fetch_and_add(th->counter, 1);
		if(*(th->counter) == C){
			printf("let consumer phase begin\n");
			for(int i = 0; i < C; i++)
				sem_post(&second_phase);	//second phase can begin
			__sync_bool_compare_and_swap(th->counter, *(th->counter), 0);
		}
		//END C.S
		sem_post(&pmut);			//producer mutual exclusion
		i++;
	}
	pthread_exit(0);
}

void* cons_dequeue(void* arg){
	THREAD_DATA* th = (THREAD_DATA*)arg;
	pthread_barrier_wait(&cbarrier);

	int i = 0;
	while(i < NUM_FASI){
		sem_wait(&second_phase);	//wait for first phase to finish
		printf("C%d stuck here\n", th->index);
		sem_wait(cons_order + (th->index - 1)); //wait for semaphore and index (t->index - 1)
		printf("C%d decremented sem no. %d\n", th->index, th->index - 1);
		sem_wait(&cmut);			//consumers mutual exclusion
		//START C.S.
		int x = dequeue(coda);
		printf("C%d, consumed: %d\n",th->index, x);
		__sync_fetch_and_add(th->counter, 1);
		if(*(th->counter) == C){
			printf("let producer phase begin\n");
			for(int i = 0; i < P; i++)
				sem_post(&first_phase);	//first phase can begin
			__sync_bool_compare_and_swap(th->counter, *(th->counter), 0);
		}
		//END C.S.
		sem_post(cons_order + (th->index  % C));
		sem_post(&cmut);			//consumers mutual exclusion
		printf("C%d incremented sem no. %d\n", th->index,th->index % C);
		i++;
	}
	pthread_exit(0);
}


//not deterministic, it maybe work, maybe not...