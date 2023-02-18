/***************************************************************************************************
SPECIFICA (ITA):

Implementare una programma tale che prende come argomento 2 nomi di file F1 e F2 
- Il main thread crea due thread/processi T1 e T2
- il T1 e T2 leggono ripetutamente da standard input stringhe (senza spazi) senza alcun ordine preciso
- T1 scrive la stringa letta in coda al file F1
- T2 scrive la stringa letta in testa al file F2
- T1 e T2 terminano quando lo stream su standard input è terminato
- quando T1 e T2 terminano, il main thread stampa a schermo il numero di byte complessivamente scritta da T1 e T2 e termina

// Nota: Assumere che le stringhe hanno tutte la stessa taglia


SPECS (ENG):

Implement a program that takes 2 fileames F1 and F2 as arguments
- The main thread creates two threads/processes T1 and T2
- T1 and T2 repeatedly read from standard input strings (without spaces) without any precise order
- T1 writes the string read at the end of file F1
- T2 writes the string read at the head of the file F2
- T1 and T2 end when the standard input stream is finished
- when T1 and T2 terminate, the main thread prints the total number of bytes written by T1 and T2 and terminates

// Note: you can assume that strings keeps the same number of chars


****************************************************************************************************/


/***********************
 * Additional exercise:
 * make T1 and T2 access the standard input in a strictly alternating manner (T1, T2, T1, T2 and so on)
 **********************/


/*****************************
 * READY TO USE HEADERS
 *****************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>
#include <errno.h>

#define abort_with_mes(msg,arg) do{printf(msg, arg);exit(1);}while(0)
#define abort(msg) do{printf(msg);exit(1);}while(0);

typedef struct t{
	int fd;
	int id;
	ssize_t (*write_func)(int fd, const void* buffer, size_t size);
}THREAD_DATA;

sem_t sem[2];
size_t total_bytes;


ssize_t head_write(int fd, const void* buffer, size_t size){
	ssize_t size_r = 0, size_w = 0;

	//compute filesize
	off_t filesize = lseek(fd, 0, SEEK_END);

	//reposition file index at the beginning
	lseek(fd, 0, SEEK_SET);

	//start reading the file and store it in tmp
	char* tmp = malloc(filesize + 1);
	
	//read the file and store it in tmp
	while(size_r < filesize){
		ssize_t current_size_r = read(fd, tmp + size_r, filesize - size_r);
		if(current_size_r == -1) abort("error on reading fileB\n");

		size_r += current_size_r;
	}


	//write the string at the beginning and write all file back
	lseek(fd, 0, SEEK_SET);
	ssize_t string_size = write(fd, buffer, size);
	if(string_size == -1) abort("error on writing string\n");

	//put new line
	write(fd, "\n", 1);

	while(size_w < filesize){
		ssize_t current_size_w = write(fd, tmp + size_w, filesize - size_w);
		if(current_size_w == -1) abort("error on re-writing back file\n");

		size_w += current_size_w;
	}

	free(tmp);
	return string_size;
}

void* child_func(void* arg){
	//th referes to th[0] if im T1 or refers to th[1] if i'm T2
	THREAD_DATA th = *(THREAD_DATA*)arg;
	char* input_string;
	int res = 0;
	while(1){
		sem_wait(&sem[th.id]);
		printf("i'm thread %d, press CTRL+D to stop\n",th.id + 1);
		res = scanf("%ms", &input_string);
		sem_post(&sem[(th.id+1)%2]);
		if(res < 0) pthread_exit(NULL);
		size_t taglia = strlen(input_string);

		//execute write func
		ssize_t size_w = th.write_func(th.fd, input_string, taglia);
		//check if T1 simple goes on error
		if(size_w == -1) abort("error writing new string\n");

		//increase total bytes counter
		__sync_fetch_and_add(&total_bytes, size_w);

		//put a new line character at the end
		if(write(th.fd, "\n", 1) == -1) abort("error on writing new line character\n");
		//empty the input_string
		free(input_string);
	}

	pthread_exit(NULL);
}



int main(int argc, char* argv[]){
	if(argc != 3) abort("usage: 230207 <tail> <head>\n");

	pthread_t ctid[2];
	THREAD_DATA* th = (THREAD_DATA*)malloc(sizeof(THREAD_DATA) * 2);
	total_bytes = 0;

	//open file A for T1
	th[0].fd = open(argv[1], O_WRONLY | O_CREAT | O_APPEND, 0660);
	//open file B for
	th[1].fd = open(argv[2], O_RDWR | O_CREAT, 0660);

	//assign function pointer
	th[0].write_func = write;
	th[1].write_func = head_write;

	//if sempaphore = 0 -> STOP; else semaphore > 0 -> GO
	for(int i = 0; i < 2; i++){
		th[i].id = i;
		sem_init(&sem[i], 0, (i + 1) % 2);
	}


	//create the threads
	for(int i = 0; i < 2; i++) pthread_create(ctid + i, NULL, child_func, th + i);

	//wait for threads to finish
	for(int i = 0; i < 2; i++) pthread_join(ctid[i], NULL);

	printf("total bytes written: %ld\n", total_bytes);
	
	//close file descriptors
	for(int i = 0; i < 2; i++) close(th[0].fd);

	for(int i = 0; i < 2; i++) sem_destroy(&sem[i]);

	free(th);	
}