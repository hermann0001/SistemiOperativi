/*
Si scriva un programma C che prende come argomenti un numero intero N e una sequenza di nomi
di file.
Per ciascun file, il programma crea un nuovo thread/processo il quale iterativamente:
• legge N byte del file
• conta quanti caratteri uguali a ‘c’ sono presenti nei N byte appena letti.
I child prima di terminare stampano su standard output: il nome del file, il numero di byte letti ed il
numero di caratteri uguali a ‘c’ letti. Un child termina la sua esecuzione se:
• o legge tutto il file
• o il numero di caratteri uguali a ‘c’ complessivamente letti da tutti i thread è maggiore o
uguale a 20.
Il programma termina stampando su standard output il numero di caratteri uguali a ‘c’ letti
complessivamente da tutti i thread.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>

#define abort(msg) do{printf(msg), exit(1);}while(0);
#define CHARACTER 'c'
#define THRESHOLD 20

size_t n;
pthread_barrier_t ptbarrier;
volatile unsigned global_count;

void* child_func(void* arg){
	char* filename = (char*)arg;
	ssize_t current_size_r = 1, size_r = 0;
	unsigned my_count = 0;

	int fd = open(filename, O_RDONLY);
	if(fd == -1)abort("erroro on opening\n");
	char* buffer = malloc(n);

	pthread_barrier_wait(&ptbarrier);

	while(current_size_r > 0){
		current_size_r = read(fd, buffer, n);
		if(size_r == -1) abort("error reading\n");
		size_r += current_size_r;

		for(int i = 0; i < n; i++){
			if(buffer[i] == CHARACTER){
				if(global_count >= THRESHOLD){
					printf("file: %s, byte read: %ld, n. char = '%c': %d\n", filename, size_r, CHARACTER ,my_count);
					close(fd);
					pthread_exit(NULL);
				}
				__sync_fetch_and_add(&global_count, 1);
				my_count++;
			}
		}
	}
	printf("file: %s, byte read: %ld, n. char = '%c': %d\n", filename, size_r, CHARACTER, my_count);
	close(fd);
	pthread_exit(NULL);
}

int main(int argc, char* argv[]){
	if(argc < 3) abort("usage: 220905 <integer N> <file1> ...\n");
	
	n = atoi(argv[1]);
	global_count = 0;
	int file_count = argc - 2;
	pthread_barrier_init(&ptbarrier, NULL, file_count);

	pthread_t* ctid = (pthread_t*)malloc(sizeof(pthread_t) * file_count);

	for(int i = 0; i < file_count; i++)
		pthread_create(ctid + i, NULL, child_func, argv[i+2]);
	

	for(int i = 0; i < file_count; i++)
		pthread_join(ctid[i], NULL);

	printf("global counter: %d\n", global_count);	

	free(ctid);
	pthread_barrier_destroy(&ptbarrier);
}
