/*
Si scriva un programma C tale che il main thread legge ciclicamente da standard input una stringa P
che corrisponde ad un percorso di file. Per ciascun percorso P viene creato un nuovo thread, il quale
legge il file al percorso P cercando il carattere ‘a’.
Qualora il main thread riceve un percorso non valido o un numero di percorsi pari a N (a piacere) il
programma stampa su standard output il numero di file che contengono il carattere ‘a’.
Nota: la lettura da standard input del main thread dovrà essere concorrente alla lettura dei file da
parte dei thread child.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>

#define N 5
#define CHARACTER 'a'
#define BUFSIZE 1024

#define abort(msg) do{printf(msg); exit(1);}while(0)

volatile unsigned count;

void* child_func(void* arg){
	int fd = *(int*)arg;
	ssize_t size_r;
	char* buffer = (char*)malloc(BUFSIZE);
	int found = 0;

	while((size_r = read(fd, buffer, BUFSIZE)) > 0 && !found){
		if(size_r == -1) abort("error reading\n");
		for(int i = 0; i < size_r && !found; i++)
			found = buffer[i] == CHARACTER;
	}
	__sync_fetch_and_add(&count, found);

	pthread_exit(NULL);
}

int main(){

	char* path;
	pthread_t* ctid = (pthread_t*)malloc(sizeof(pthread_t) * N);
	int* fd = (int*)malloc(sizeof(int) * N);
	int i;
	count = 0;

	for(i = 0; i < N; i++){
		printf("insert file path\n");
		scanf("%ms", &path);

		if((fd[i] = open(path, O_RDONLY)) == -1){
			printf("invalid file path\n");
			break;
		}

		pthread_create(ctid + i, NULL, child_func, fd + i);
		free(path);
	}

	for(int j = 0; j < i; j++)
		pthread_join(ctid[j], NULL);

	printf("count: %d\n", count);
}