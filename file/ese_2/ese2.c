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
//2^15 interi short = 32K * 2 = 64K bytes

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <limits.h>

#define N 8
#define BUFSIZE 256
#define FILESIZE 65536      //64K
#define FILENAME "shorts.bin"

#define abort(msg) do{printf(msg); exit(1);}while(0)
#define abort_arg(msg, arg) do{printf(msg,arg); exit(1);}while(0)
#define usage(s,h) do{printf("stack_usage: %ld, heap_usage: %ld\n", s, h);}while(0);

volatile short min = SHRT_MAX;
volatile short max = SHRT_MIN;

void* leggi_int(void* arg){
	short index = *(short*)arg;
	
	//splitting up the file in N parts, counting as shorts given to the various thread to compute
	int shorts_slice = (FILESIZE / sizeof(short)) / N;
	int remainder = (FILESIZE / sizeof(short)) % N;
	int my_shorts_slice = shorts_slice;
	if(index == N - 1) my_shorts_slice += remainder;

	//open file
	int fd = open(FILENAME, O_RDONLY);
	if(fd == -1) abort("error reading\n");
	

	//reposition the index to the start of my slice
	if(lseek(fd, (index * (shorts_slice * sizeof(short))), SEEK_SET) == -1)
		abort("error replacing file_index\n");

	//this buffer can contain BUFSIZE shorts
	size_t CHUNK_SIZE = BUFSIZE * sizeof(short);
	short* buffer = (short*)malloc(CHUNK_SIZE);
	size_t bytes_read = 0;


	//read CHUNK_SIZE bytes
	while(bytes_read < (my_shorts_slice * sizeof(short))){
		size_t size_r = read(fd, buffer, CHUNK_SIZE);
		if(size_r == -1) abort("error reading\n");

		//stack_usage: 66, heap_usage: 512
		//iterate over the shorts
		for(short i = 0; i < size_r / sizeof(short); i++){
			//stack_usage: 68, heap_usage: 512
			if(buffer[i] < min)
				__sync_bool_compare_and_swap(&min, min, buffer[i]);
			else if(buffer[i] > max)
				__sync_bool_compare_and_swap(&max, max, buffer[i]);
		}
		bytes_read += size_r;

		//if im not the last thread AND the next read will overflow to the next slice, resize the CHUNK_SIZE
		if((index != N-1) && ((bytes_read + CHUNK_SIZE) > (my_shorts_slice * sizeof(short))))
			CHUNK_SIZE = my_shorts_slice * sizeof(short) - bytes_read;
	}
	//stack_usage: 58, heap_usage: 512
	free(buffer);
	//stack_usage: 58, heap_usage: 0
}

int main(int argc, char* argv[]){
	pthread_t* ctid = (pthread_t*)malloc(sizeof(pthread_t) * N);
	short* indexes = (short*)malloc(sizeof(short) * N);

	for(short i = 0; i < N; i++) {
		indexes[i] = i;
		pthread_create(ctid + i, NULL, leggi_int, indexes + i);
	}

	for(short i = 0; i < N; i++) 
		pthread_join(ctid[i], NULL);

	printf("minimum: %d, maximum: %d\n", min, max);
	
	free(ctid);
}



