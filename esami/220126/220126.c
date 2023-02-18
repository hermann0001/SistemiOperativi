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
#define THREADS 8
#define TH 0
#define BUFSIZE
#define abort(msg) do{printf(msg); exit(1);}while(0)
#define abort_arg(msg, arg) do{printf(msg,arg); exit(1);}while(0)

off_t filesize;
unsigned thread_count;
int fd = 0, global_threshold;
unsigned count;

void* child_func(void* arg){
	int index = *(int*)arg;
	off_t slice_size = filesize / thread_count;
	off_t remainder  = filesize % thread_count;

	//giving the remainder to the last thread
	off_t my_slice_size = slice_size;
	if(index == thread_count - 1) my_slice_size += remainder;

	printf("I am the thread with index %d, and slice_size %ld\n",index, my_slice_size);

	//the file offset is set to the starting slice of thread i
	off_t starting_offset = lseek(fd, index * slice_size, SEEK_SET);
	if(starting_offset == -1) abort_arg("error in replacing starting offset, errno:%d\n", errno);

	off_t end_slice = (index + 1) * slice_size;
	printf("calcolata l'end slice: %ld\n", end_slice);

	int val;				//integer to read from file and compare
	int stop = 0;			
	ssize_t size_r;

	do{
		//read 1 integer at the time
		size_r = read(fd, &val, sizeof(int));

		//if the value read is above threshold, increase counter
		if(val > global_threshold)
			__sync_fetch_and_add(&count, 1);
		
		//compute the current offset
		off_t offset = lseek(fd, 0, SEEK_CUR);
		if(offset == -1) abort_arg("error in computing current offset, errno:%d\n",errno);

		//if i reached the end of my slice, i stop
		if(offset == end_slice) stop = 1;
	}while(size_r > 0 && !stop);

	pthread_exit(NULL);
}

int above_threshold(char* filename, int n, int threshold){

	thread_count = n;
	count = 0;
	//allocate thread ids
	pthread_t* ctid = (pthread_t*)malloc(sizeof(pthread_t) * thread_count);

	//open file
	fd = open(filename, O_RDONLY);
	if(fd == -1) abort("error on opening file\n");

	//compute filesize
	filesize = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);

	//store the threshold
	global_threshold = threshold;

	//creating threads
	int indexes[thread_count];
	for(int i = 0; i < thread_count; i++) {
		indexes[i] = i;	
		pthread_create(ctid + i, NULL, child_func, indexes + i);
	}

	//destroying threads
	for(int i = 0; i < thread_count; i++) pthread_join(ctid[i], NULL);

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

	int res = above_threshold(FILENAME, THREADS, TH);

	printf("integers above threshold: %d\n", res);
}

