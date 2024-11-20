/*
Implementare un programma tale che prende come argomento N nomi di file F1,F2,..,Fn
- il main thread crea N thread/processi Ti, con i in {1,2,...,n}
- il main thread ripetutamente:
  1. legge da standard input una stringa S (senza spazi bianchi)
  2. ridireziona S ad un thread child senza ordine prestabilito
  3. torna ad 1
- ciascun thread Ti
  -crea un nuovo file vuoto Fi
  -per ciascuna stringa S ricevuta, scrive S sul Fi
- quando il main thread legge la stringa 'quit'
  -comunica a ciascun thread di terminare
  -termina la sua esecuzione


*************************************************************************************************+


/**********************
* Additional excercise:
* make T1 and T2 access the standard input in a strictly alternating manner (T1,T2,T1,T2,T1,...)
***********************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>

#define abort(msg) do{printf(msg); exit(1);}while(0);

typedef struct th{
	char* filename;
	int index;
	char* s;
}THREAD_DATA;

sem_t sready;

volatile int stop = 0;

void* child_func(void* arg){
	THREAD_DATA* th = (THREAD_DATA*)arg;

	int fd = open(th->filename, O_WRONLY | O_CREAT | O_TRUNC, 0660);
	if(fd == -1) abort("error opening file");

	while(!stop){
		//sem_wait(&sready);
		while(th->s == NULL);
		int taglia = strlen(th->s);
		ssize_t size_r = write(fd, th->s, taglia);
		if(size_r == -1)abort("error writing\n");

	}
}


int main(int argc, char* argv[]){
	if(argc < 2) abort("usage: <nomeprogramma> <files...\n");

	int file_count = argc - 1;

	pthread_t* ctid = (pthread_t*)malloc(sizeof(pthread_t) * file_count);
	THREAD_DATA* th = (THREAD_DATA*)malloc(sizeof(THREAD_DATA) * file_count);
	sem_init(&sready, 0, 0);

	//init thread data
	for(int i = 1; i < argc; i++){
		th[i - 1].filename = (char*)malloc(sizeof(char) * strlen(argv[i]));
		strcpy(th[i - 1].filename, argv[i]);
		th[i - 1].index = i;
	}

	for(int i = 0; i < file_count; i++)
		pthread_create(ctid + i, NULL, child_func, th + i);

	char* input_string;
	
	while(1){
		printf("insersici una stringa\n");
		scanf("%ms", &input_string);
		if(strcmp(input_string, "quit") == 0){
			sem_post(&sready);
			__sync_bool_compare_and_swap(&stop, stop, 1);
			break;
		}
		int i = rand() % file_count;
		th[i].s = (char*)malloc(sizeof(char) * strlen(input_string));
		strcpy(th[i].s, input_string);
		//sem_post(&sready);
		printf("thread %d, takes %s\n", i+1, input_string);
		free(input_string);
	}

	
	for(int i = 0; i < file_count; i++)
		pthread_join(ctid[i], NULL);

	free(th);
	free(ctid);
}