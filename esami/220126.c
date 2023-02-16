/*
Si scriva una funzione di codice C con la seguente interfaccia:
int above_threshold(char *filename, int n, int threshold).
Tale funzione deve lanciare un numero di thread/processi pari a n per leggere dal file binario
filename una sequenza di int e contare quanti di questi hanno un valore maggiore di
threshold. Infine, la funzione stampa su standard output e ritorna il numero totale di int con
valore maggiore di threshold contenuti nel file
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>

#define FILENAME "dataset.bin"
#define THREAD_COUNT 4
#define TH 450245
#define BUFSIZE 1024
#define abort(msg) do{printf(msg); exit(1);}while(0)
#define abort_arg(msg, arg) do{printf(msg,arg); exit(1);}while(0)

int fd = 0, global_threshold;
unsigned count;

void* child_func(void* arg){
	int buffer[BUFSIZE];					//integers to read from file and compare
	ssize_t size_r;

	int j = 0;
	do{
		//read BUFSIZE * 4 integer at the time
		size_r = read(fd, buffer, sizeof(int) * BUFSIZE);		
		if(size_r < 0) abort("reading error\n");

		for(int i = 0; i < size_r / sizeof(int); i++)
			__sync_fetch_and_add(&count, buffer[i] > global_threshold);
	}while(size_r > 0);

	return NULL;
}

int above_threshold(char* filename, int n, int threshold){

	count = 0;
	//allocate thread ids
	pthread_t* ctid = (pthread_t*)malloc(sizeof(pthread_t) * n);

	//open file
	fd = open(filename, O_RDONLY);
	if(fd == -1) abort("error on opening file\n");

	//store the threshold
	global_threshold = threshold;

	//creating threads
	for(int i = 0; i < n; i++) pthread_create(ctid + i, NULL, child_func, NULL);

	//destroying threads
	for(int i = 0; i < n; i++) pthread_join(ctid[i], NULL);

	close(fd);
	
	return count;
}

int main(int argc, char* argv[]){
	//if(argc != 4) abort("usage: <nomeprogramma> <input-file> <num_threads> <threshold>\n");

	//test
	/* i created this file only to test the above_threshold function
	int integers_fd = open("integers", O_CREAT | O_TRUNC | O_WRONLY, 0660);
	for(int i = 0; i < 256; i++) write(integers_fd, &i, sizeof(int));
	*/

	int res = above_threshold(FILENAME, THREAD_COUNT, TH);

	printf("integers above threshold: %d\n", res);
}

