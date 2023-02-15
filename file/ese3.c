/*
Scrivere un programma C invert che dato un file A ne inverte il contenuto e 
lo memorizza in nuovo file B. Il programma deve:

riportare il contenuto di A in memoria;

invertire la posizione di ciascun byte utilizzando un numero N di thread/processi concorrenti;

scrivere il risultato in un nuovo file B;

prendere A, B e N come argomenti da linea di comando.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>

#define BUFSIZE 256
#define abort(msg) do{printf(msg); exit(1);}while(0)
#define abort_arg(msg, arg) do{printf(msg,arg); exit(1);}while(0)

off_t filesize;
int effective_thread_count;
char *input_buffer, *output_buffer;

void* inverti(void* arg);
void parallel_invert(int thread_count);

int main(int argc, char* argv[]){
	if(argc != 4) abort("usage: <program name> <input_file> <output_file> <N thread/process>\n");

	char* in_file_path = argv[1];
	char* out_file_path = argv[2];
	int input_fd, output_fd;
	unsigned num_threads = atoi(argv[3]);
	if(num_threads < 1) num_threads = 1;

	input_fd = open(in_file_path, O_RDONLY);
	output_fd = open(out_file_path, O_CREAT | O_TRUNC | O_WRONLY, 0660);

	ssize_t size_r = 0, size_w = 0;

	if(input_fd == -1) abort("error on opening file a\n");
	if(output_fd == -1) abort("error on opening file b\n");

	/*calculate filesize*/
	filesize = lseek(input_fd, 0, SEEK_END);
	lseek(input_fd, 0, SEEK_SET);

	/*allocate buffers in memory*/
	input_buffer  = (char*)malloc(filesize+1);
	output_buffer = (char*)malloc(filesize+1);
	input_buffer[filesize]  = '\0';
	output_buffer[filesize] = '\0';

	if(!input_buffer || !output_buffer )abort("cannot allocate memory\n");

	/*read the file and store in a buffer*/
	while(size_r < filesize){
		//try with a single read, if it fails retry changing starting address
		ssize_t current_size_r = read(input_fd, input_buffer+size_r, filesize);
		if(current_size_r == -1) abort("error on reading fileA\n");
		size_r += current_size_r;
	}

	printf("filesize: %ld\n", filesize);
	for(int i = 0; i < filesize; i++) printf("byte pos: %d, val = %c\n", i, input_buffer[i]);




	/*invert the array*/
	parallel_invert(num_threads);



	//write on the output file
	while(size_w < filesize){
		size_t current_size_w = write(output_fd, output_buffer + size_w, filesize);
		if(current_size_w == -1) abort("error on write fileB\n");
		
		size_w += current_size_w;
	}

	//close descriptors
	close(input_fd);
	close(output_fd);
	free(input_buffer);
	free(output_buffer);
}

void* inverti(void* arg){
	//getting the index
	int index = *(int*)arg;

	//compute the slice of shared array to work on
	off_t slice_size = filesize / effective_thread_count;
	off_t remainder  = filesize % effective_thread_count;

	//give the remainder to the last thread
	off_t my_slice_size = slice_size;
	if(index == effective_thread_count-1) my_slice_size += remainder;

	printf("I am the thread with index %d, and slice_size %ld\n",index, my_slice_size);

	for(int i = 0; i < my_slice_size; i++){
		size_t start_byte = index * slice_size;				//skip the slice of other threads
		start_byte += i;									//the byte i want to copy into the output buffer
		size_t destination_byte = filesize - 1 - start_byte;
		output_buffer[destination_byte] = input_buffer[start_byte];
	}
	pthread_exit(NULL);
}

void parallel_invert(int thread_count){
	int maximum_thread_count = filesize;

	/*compute the minimum between thread_count and maximum_thread_count 
	so each thread can swap at least one item*/
	if(thread_count > maximum_thread_count)
		effective_thread_count = maximum_thread_count;
	else
		effective_thread_count = thread_count;

	printf("effective_thread_count: %d\n", effective_thread_count);
	//allocate memory for ctids
	pthread_t* ctid = (pthread_t*)malloc(sizeof(pthread_t) * effective_thread_count);
	//allocate memory for thread indexes
	int* indexes = (int*)malloc(sizeof(int) * effective_thread_count);

	for(int i = 0; i < effective_thread_count; i++){
		indexes[i] = i;
		pthread_create(ctid + i, NULL, inverti, indexes + i);
	}

	for(int i = 0; i < effective_thread_count; i++){
		pthread_join(ctid[i], NULL);
	}

	free(ctid);
	free(indexes);
}