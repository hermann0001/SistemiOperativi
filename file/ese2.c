/*
Dato un file binario contenente un sequenza di 2^15 interi di tipo short, 
scrivere un programma che crea N processi o threads, 
i quali leggono il contenuto del file ed individuano il valore minimo e massimo 
contenuto nel file. Nel fornire una soluzione rispettare i seguenti vincoli:

-ciascun intero non può essere letto da più di un thread/processo;

-ciascun thread/processo può leggere il medesimo intero al più una volta;

-ciascun thread/processo può allocare memoria nell’heap per al più 512 byte;

-N è un parametro definito a tempo di compilazione o tramite linea di comando;

-N è minore o uguale a 8;

-è ammesso allocare di variabili globali (data) e locali (stack) per memorizzare 
tipi primitivi (puntatori, int, short, char, long, etc.) per al più 128 byte;
per generare il file utilizzare la soluzione dell’esercizio 3.1.
*/

//sizeof(short) = 2; 
//2^15 interi short = 32K * 2 = 64K

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <limits.h>

#define N 4
#define BUFSIZE 256
#define DIM 65536      //64K
#define FILENAME "prova.bin"
#define abort(msg) do{printf(msg); exit(1);}while(0)
#define abort_arg(msg, arg) do{printf(msg,arg); exit(1);}while(0)

volatile int end = 0;
typedef struct lock_sem{
		pthread_mutex_t mutex;
		pthread_barrier_t barrier;
	}LOCKS;
long size_r;
volatile short min = SHRT_MAX;
volatile short max = SHRT_MIN;
int fd;


void* leggi_int(void* arg);

int main(int argc, char* argv[]){

	LOCKS* pt = (LOCKS*)malloc(sizeof(LOCKS));
	pthread_t* ctid = (pthread_t*)malloc(sizeof(pthread_t)*N);

	fd = open(FILENAME, O_RDONLY);
	if(fd == -1) abort("errore nell'apertura del file\n");

	/*controllo che il file sia di dim pari a 64K*/
	//if(lseek(fd, 0, SEEK_END) != DIM) abort("expected 64K file\n");
	//lseek(fd, 0 , SEEK_SET);

	ssize_t r;
	short num;
	int i = 0;
	while(r = read(fd, &num, sizeof(short)) > 0){ 
		printf("r = %ld\n", r);
		printf("pos: %d, numero letto: %d\n",i, num);
		i++;
	}
	 


	pthread_barrier_init(&(pt->barrier));
	pthread_mutex_init(&(pt->mutex));
	for(int i = 0; i < N; i++) pthread_create(ctid + i, NULL, leggi_int, (void*)pt);

	for(int i = 0; i < N; i++) pthread_join(ctid[i], NULL);

}

void * leggi_int(void* arg){
	LOCKS* pt = (LOCKS*)arg		//passo come parametro i locks
	short* buffer = (short*)malloc(sizeof(short)*BUFSIZE);
	pthread_barrier_wait(&(pt->barrier));

	while(!end){
		pthread_mutex_lock(&(pt->mutex));
		size_r = read(fd, buffer, BUFSIZE*sizeof(short))
		if(size_r == 0) 
			printf("size_r = %d\n", size_r);
			for(int i = 0; i < size_r/sizeof(short); i++){
				if(buffer[i] < min)
					__sync_val_compare_and_swap(&min, min, buffer[i]);	//comparando &min e min otterrò sempre lo swap
				if(buffer[i] > max)
					__sync_val_compare_and_swap(&max, max, buffer[i]);  //comparando &max e max otterrò sempre lo swap
			}
		pthread_mutex_unlock	
	}
}
