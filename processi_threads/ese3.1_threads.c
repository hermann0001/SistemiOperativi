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
#include <time.h>
#include "queue.h"  				//la coda è implementata tramite array di interi e può contenere 128 elementi

#define abort(msg) do{printf(msg); exit(1);}while(0)
#define abort_arg(msg, arg) do{printf(msg,arg); exit(1);}while(0)

#define P 2
#define C 10
#define SECONDS 10
#define QUEUE_LENGTH 128

QUEUE* coda;
pthread_mutex_t pmutex;
pthread_barrier_t pbarrier;
//srand(time(NULL));
volatile unsigned contatore_prod = 0;

void* prod_enqueue(void* arg);
void* cons_dequeue(void* arg);
void acquire(){
	pthread_mutex_lock(&pmutex);
	printf("tid = %lu lock acquisito\n", pthread_self());
}
void release(){
	pthread_mutex_unlock(&pmutex);
	printf("tid = %lu lock rilasciato\n", pthread_self());
}

int main(int argc, char* argv[]){
	/*creo una coda vuota*/

	if(argc != 1) abort("usage: <nome programma>");

	coda = new_queue();
	emptyQueue(coda);

	pthread_t prod[P];
	pthread_t cons[C];
	pthread_mutex_init(&pmutex, NULL);

	int prod_creati = 0, cons_creati = 0; 			//variabili booleane per creare thread

	//while(1)
	//{
		/*prima fase*/
		printf("Inizio della prima fase, procedo a creare i thread produttori\n");
		pthread_barrier_init(&pbarrier, NULL, P);
		if(!prod_creati){
			for(int i = 0; i < P; i++) {
				int res = pthread_create(prod + i, NULL, prod_enqueue, NULL);
				if(res != 0) abort_arg("errore nella creazione del thread, errore = %d\n",res);
			}
			prod_creati = 1;
		}

		sleep(SECONDS);

		for(int i = 0; i < P; i++){
			int res = pthread_join(prod[i], NULL);
			if(res != 0) abort_arg("errore nella terminazione dei thread, errore = %d\n",res);
			prod_creati = 0;
		}

		pthread_barrier_destroy(&pbarrier);

		for(int i = 0; i < QUEUE_LENGTH; i++) printf("intero pos.%d = %d\n",i, coda->elementi[i]);
	//}
}

void* prod_enqueue(void* arg){

	printf("tid = %lu in attesa della barrier\n",pthread_self());
	pthread_barrier_wait(&pbarrier);
	
	while(contatore_prod < C){
		int random = rand() % 1000 + 1;
		printf("tid = %lu in attesa del lock\n", pthread_self());
		acquire();
		enqueue(coda, random);
		printf("tid = %lu ho messo in coda il numero %d\n", pthread_self(), random);
		__sync_fetch_and_add(&contatore_prod, 1);
		printf("__sync_fetch_and_and(%d,%d) = %d\n", contatore_prod, C, __sync_fetch_and_add(&contatore_prod, C));
		printf("tid = %lu contatore incrementato: %d\n",pthread_self(), contatore_prod);
		release();
	}

	pthread_exit(0);
}
