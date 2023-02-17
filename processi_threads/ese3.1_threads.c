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

#define P 20
#define C 20
#define SECONDS 5
#define QUEUE_LENGTH 64

QUEUE* coda;
pthread_barrier_t pbarrier;
pthread_mutex_t pmutex;
sem_t fase;

void* prod_enqueue(void* arg);
void* cons_dequeue(void* arg);

int main(int argc, char* argv[]){
	/*creo una coda vuota*/

	if(argc != 1) abort("usage: <nome programma>");

	coda = new_queue();
	emptyQueue(coda);

	pthread_t prod[P];
	pthread_t cons[C];

	int prod_creati = 0, cons_creati = 0; 			//variabili booleane per creare thread
	while(1)
	{
		int conta_creati = 0, conta_estratti = 0;

		/*prima fase*/
		printf("\n___________________________\n");
		printf("\n\nInizio della prima fase\n\n");
		pthread_barrier_init(&pbarrier, NULL, P);
		pthread_mutex_init(&pmutex, NULL);
		//sem_init(&fase,0,1);
		if(!prod_creati){
			for(int i = 0; i < P; i++) {
				int res = pthread_create(prod + i, NULL, prod_enqueue, &conta_creati);
				if(res != 0) abort_arg("errore nella creazione del thread, errore = %d\n",res);
			}
			prod_creati = 1;
		}

		/*terminazione dei thred produttori*/
		for(int i = 0; i < P; i++){
			int res = pthread_join(prod[i], NULL);
			if(res != 0) abort_arg("errore nella terminazione dei thread, errore = %d\n",res);
			prod_creati = 0;
		}
		//sem_wait(&fase);
		pthread_barrier_destroy(&pbarrier);
		pthread_mutex_destroy(&pmutex);
		if(isEmpty(coda)) printf("La coda è vuota\n");
		else stampa(coda);
		sleep(SECONDS);


		/*seconda fase*/
		printf("\n_____________________________\n");
		printf("\n\nInizio della seconda fase\n\n");

		pthread_barrier_init(&pbarrier, NULL, C);
		pthread_mutex_init(&pmutex, NULL);
		//sem_post(&fase);
		if(!cons_creati){
			for(int i = 0; i < C; i++) {
				int res = pthread_create(cons + i, NULL, cons_dequeue, &conta_estratti);
				if(res != 0) abort_arg("errore nella creazione del thread, errore = %d\n",res);
			}
			cons_creati = 1;
		}

		/*terminazione thread consumatori*/
		for(int i = 0; i < C; i++){
			int res = pthread_join(cons[i], NULL);
			if(res != 0) abort_arg("errore nella terminazione dei thread, errore = %d\n",res);
			cons_creati = 0;
		}

		pthread_barrier_destroy(&pbarrier);
		pthread_mutex_destroy(&pmutex);
		if(isEmpty(coda)) printf("La coda è vuota\n");
		else stampa(coda);
		//sem_destroy(&fase);
		sleep(SECONDS);
	}
}

void* cons_dequeue(void* arg){
	printf("tid = %lu in attesa della barrier\n",pthread_self());
	//aspetto che tutti i thread vengano creati
	pthread_barrier_wait(&pbarrier);				
	int i = *(int*)arg;
	while(1){
		printf("tid = %lu in attesa del lock\n", pthread_self());
		//sem_wait(&lock);
		pthread_mutex_lock(&pmutex);
		if(i < C){
			int x = dequeue(coda);
			__sync_fetch_and_add(&i, 1);
			printf("tid = %lu ho estratto dalla coda il numero %d, iterazione i = %d\n", pthread_self(), x, *i);
			pthread_mutex_unlock(&pmutex);
		}else {
			pthread_mutex_unlock(&pmutex);
			break;
		}
	}
	pthread_exit(0);
}

void* prod_enqueue(void* arg){
	printf("tid = %lu in attesa della barrier\n",pthread_self());
	pthread_barrier_wait(&pbarrier);				//aspetto che tutti i thread vengano creati
	volatile int* i = (int*)arg;

	while(1){
		int random = rand() % 100 + 1;
		printf("tid = %lu in attesa del lock\n", pthread_self());
		//sem_wait(&lock);
		pthread_mutex_lock(&pmutex);
		if(*i < C){
			enqueue(coda, random);
			__sync_fetch_and_add(i, 1);
			printf("tid = %lu ho messo in coda il numero %d, iterazione i = %d\n", pthread_self(), random, *i);
			pthread_mutex_unlock(&pmutex);
		}else {
			pthread_mutex_unlock(&pmutex);
			break;
		}
	}
	pthread_exit(0);
}
