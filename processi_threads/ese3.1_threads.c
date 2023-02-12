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
#define P 10
#define C 10
#define SECONDS 10
#define QUEUE_LENGTH 128

QUEUE* coda;
pthread_mutex_t pmutex;
//srand(time(NULL));
volatile unsigned contatore_prod = 0;

void* prod_enqueue(void* arg);
void* cons_dequeue(void* arg);
void acquire(){pthread_mutex_lock(&pmutex);}
void release(){pthread_mutex_unlock(&pmutex);}

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
		if(!prod_creati){
			for(int i = 0; i < P; i++) {
				if(!(pthread_create(prod + i, NULL, prod_enqueue, NULL)))
					abort("errore nella creazione dei thread \n");
			}
			prod_creati = 1;
			sleep(SECONDS);
		}

		for(int i = 0; i < P; i++){
			if(!(pthread_join(prod[i], NULL)))
				abort("errore nella terminazione dei thread\n");
			prod_creati = 0;
		}

		for(int i = 0; i < QUEUE_LENGTH; i++) printf("intero pos.%d = %d",i, coda->elementi[i]);
	//}
}

void* prod_enqueue(void* arg){
	
	while(contatore_prod < C){
		int random = rand() % 1000 + 1;
		printf("tid = %lu; sono il thread figlio in attesa del lock\n", pthread_self());
		acquire();
		enqueue(coda, random);
		__sync_fetch_and_add(&contatore_prod, 1);
		printf("tid=%lu; sono il thread figlio e ho messo in coda il numero %d\n", pthread_self(), random);
		release();
		printf("tid=%lu; sono il thread figlio e ho appena rilasciato il lock\n", pthread_self());
	}

	pthread_exit(0);
}
