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

#define STDOUT 1
#define FILENAME "dataset.bin"
#define THREADS 7
#define abort(msg) do{printf(msg); exit(1);}while(0)
#define abort_arg(msg, arg) do{printf(msg,arg); exit(1);}while(0)
#define CHUNK_SIZE 4096 //4K

off_t filesize;
int global_threshold;
unsigned count, thread_count;
long total_integers;

typedef struct td{
	int fd;
	int index;
}THREAD_DATA;

void* child_func(void* arg){
	THREAD_DATA th = *(THREAD_DATA*)arg;
	ssize_t size_r = 0, 
	size_t BUFSIZE;

	long slice_integers = total_integers / thread_count;
	long remainder      = total_integers % thread_count;

	//giving the remainder to the last thread
	long my_integers = slice_integers;
	if(th.index == (thread_count - 1)) my_integers += remainder;

	//the file offset is set to the starting slice of thread i
	off_t starting_offset = lseek(th.fd, th.index * slice_integers * sizeof(int), SEEK_SET);
	if(starting_offset == -1) abort_arg("error in replacing starting offset, errno:%d\n", errno);
	
	size_t BUFSIZE = CHUNK_SIZE * sizeof(int);
	//choosing bufsize
	if(BUFSIZE > my_integers * sizeof(int)) BUFSIZE = my_integers * sizeof(int);
	
	int buffer[CHUNK_SIZE];

	//try read bufsize ints
	do{
	    ssize_t current_size_r = read(th.fd, buffer, BUFSIZE);
		if(current_size_r == -1) abort_arg("error reading errno:%d\n", errno);

		//if the value read is above threshold, increase counter
		for(int i = 0; i < current_size_r / sizeof(int); i++)
			__sync_fetch_and_add(&count, buffer[i] > global_threshold);
		
		size_r += current_size_r;
		size_t remaining_bytes = (my_integers * sizeof(int)) - size_r;
		if(remaining_bytes < BUFSIZE && remaining_bytes > 0)
			BUFSIZE = remaining_bytes;		
	}while(size_r < my_integers * sizeof(int));

	//check if some thread read bytes belonging to other threads
	if(size_r > my_integers * sizeof(int)) abort("I read something I wasn't supposed to...\n");

	pthread_exit(NULL);
}

int above_threshold(char* filename, int n, int threshold){
	int i = 0;
	if(n < 1) n = 1;
	thread_count = n;
	count = 0;
	global_threshold = threshold;

	//allocate thread ids
	pthread_t* ctid = (pthread_t*)malloc(sizeof(pthread_t) * thread_count);

	//allocate thread data
	THREAD_DATA* th = (THREAD_DATA*)malloc(sizeof(THREAD_DATA) * thread_count);

	//initialize thread data
	for(i = 0; i < thread_count; i++){
		th[i].index = i;
		th[i].fd = open(filename, O_RDONLY);
	}

	//compute ftotal_integers
	filesize = lseek(th[0].fd, 0, SEEK_END);
	lseek(th[0].fd, 0, SEEK_SET);
	total_integers = filesize / sizeof(int);

	if((filesize % sizeof(int))) abort("the file does not contain integers...\n");

	//creating threads
	for(i = 0; i < thread_count; i++) 
		pthread_create(ctid + i, NULL, child_func, th + i);
	

	//destroying threads
	for(int i = 0; i < thread_count; i++) 
		pthread_join(ctid[i], NULL);

	//close descriptors
	for(i = 0; i < thread_count; i++)
		close(th[i].fd);

	return count;
}

int main(int argc, char* argv[]){
	//if(argc != 4) abort("usage: <nomeprogramma> <input-file> <num_threads> <threshold>\n");

	//test
	/* i created this file only to test the above_threshold function
	int integers_fd = open("integers", O_CREAT | O_TRUNC | O_WRONLY, 0660);
	for(int i = 0; i < 256; i++) write(integers_fd, &i, sizeof(int));
	*/

	int res;
	res = above_threshold(FILENAME, THREADS, 0);
	printf("threshold: %d, integers above: %d, expected: %d\n", 0, res, 1049779);
	res = above_threshold(FILENAME, THREADS, 1073741824);
	printf("threshold: %d, integers above: %d, expected: %d\n", 1073741824, res, 524716);
	res = above_threshold(FILENAME, THREADS, -1073741824);
	printf("threshold: %d, integers above: %d, expected: %d\n", -1073741824, res, 1574575);

	
}

