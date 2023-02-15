/*
Si scriva il codice di una funzione C con la seguente interfaccia 
void tunnel(int descriptors[], int count) tale che, se eseguita,
porti l’applicazione a gestire, per ogni file-descriptor dell’array 
descriptors l’inoltro del flusso dei dati in ingresso verso il canale di 
standard-output dell’applicazione. Il parametro count indica di quanti elementi
è costituito l’array descriptors. L’inoltro dovrà essere attuato in modo 
concorrente per i diversi canali.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>

#define STDOUT 1
#define BUFSIZE
#define abort(msg) do{printf(msg); exit(1);}while(0)
#define abort_arg(msg, arg) do{printf(msg,arg); exit(1);}while(0)

pthread_mutex_t ptmutex;

void* child_func(void* arg){
	int fd = *(int*)arg;		//file descriptor

	ssize_t size_r, size_w;

	//compute filesize
	off_t filesize = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);

	char* buffer = (char*)malloc(filesize);
	int end = 0;

	while(!end){
		//read the file
		size_r = read(fd, buffer + size_r, filesize);
		if(size_r == -1) abort("error on reading\n");

		//has EOF reached?
		end = size_r == 0;

		pthread_mutex_lock(&ptmutex);
		//write to stdout
		size_w = write(STDOUT, buffer + size_w, size_r);
		if(size_w == -1) abort("error on writing\n");

		if(size_w != size_r) printf("not all bytes were written!\n");
		pthread_mutex_unlock(&ptmutex);
	}
	free(buffer);

	pthread_exit(NULL);
}

void tunnel(int descriptors[], int count){
	pthread_t* ctid = (pthread_t*)malloc(sizeof(pthread_t) * count);
	pthread_mutex_init(&ptmutex, NULL);

	//creatig the threads
	for(int i = 0; i < count; i++) pthread_create(ctid + i, NULL, child_func, &descriptors[i]);
	
	//destroying the threads
	for(int i = 0; i < count; i++) pthread_join(ctid[i], NULL);

	pthread_mutex_destroy(&ptmutex);

	free(ctid);
}

int main(int argc, char* argv[]){
	if(argc < 1) abort("too few arguments, insert at least 1 filename\n");

	int count = argc - 1;
	char* filenames[count];

	//getting the filenames from prompt
	for(int i = 0; i < count; i++) filenames[i] = argv[i+1];

	//opening the files
	int fd[count];
	for(int i = 0; i < count; i++){
		fd[i] = open(filenames[i], O_RDONLY);
		if(fd[i] == -1) abort_arg("error on opening file %d", i);
	}

	printf("tunneling...\n");
	tunnel(fd, count);

	//closing the files
	for(int i = 0; i < count; i++) close(fd[i]);
}