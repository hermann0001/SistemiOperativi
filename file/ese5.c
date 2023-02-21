/*
Si scriva una funzione C con la seguente interfaccia 
void file_check(char *file_name, int num_threads). 
Tale funzione dovrà lanciare num_thread nuovi threads, in modo che ciascuno di
essi legga stringhe dallo standard input, e per ogni stringa letta verifichi l’occorrenza di 
tale stringa all’interno di ciascuna riga del file il cui path è identificato 
tramite il parametro file_name, e stampi la stringa su standard output in caso affermativo.
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>

#define abort(msg) do{printf(msg); exit(1);}while(0)
#define abort_arg(msg, arg) do{printf(msg,arg); exit(1);}while(0)
#define BUFSIZE 1024

pthread_mutex_t ptmutex;

typedef struct t{
	int fd;
	int index;
}THREAD_DATA;

void* child_func(void* arg){
	THREAD_DATA* th = (THREAD_DATA*)arg;
	ssize_t size_r;
	char* input_string;
	int found = 0;

	pthread_mutex_lock(&ptmutex);
	//read string from stdin
	printf("tid = %d, inserisci la stringa: ",th->index);
	scanf("%ms%*c", &input_string);
	pthread_mutex_unlock(&ptmutex);

	char* buffer = malloc(BUFSIZE);

	//read the file and search for matches
	do{
		//read from file exact n. bytes as input_string
		size_r = read(th->fd, buffer, BUFSIZE);  
		if(size_r == -1) abort("error on reading file\n");

		//compare the string
		if(strstr(buffer, input_string)){
			//if they're equals, set found to 1 and print the string
			found = 1;
			pthread_mutex_lock(&ptmutex);
			printf("tid = %d found the match with %s!\n", th->index, input_string);
			pthread_mutex_unlock(&ptmutex);
		}
		else 
		{
			//riposiziona l'indice di strlen(input_string)-1 dalla fine dell'ultima lettura (l'ultima lettura potrebbe aver tagliato la stringa)
			off_t indice = lseek(th->fd, 0, SEEK_CUR);
			lseek(th->fd, indice - (strlen(input_string) - 1), SEEK_SET);
		}
	}while((!found) && size_r > 0);

	free(input_string);
	free(buffer);
	pthread_exit(NULL);
}

void file_check(char* file_name, int num_threads){
	if(num_threads < 1) num_threads = 1;

	pthread_mutex_init(&ptmutex, NULL);
	
	//allocate the threads
	pthread_t* ctid = (pthread_t*)malloc(sizeof(pthread_t) * num_threads);
	THREAD_DATA* th = (THREAD_DATA*)malloc(sizeof(THREAD_DATA) * num_threads);

	for(int i = 0; i < num_threads; i++){
		th[i].fd = open(file_name, O_RDONLY);
		if(th[i].fd == -1) abort("error opening file\n");
		th[i].index = i;
	}

	//creating threads
	for(int i = 0; i < num_threads; i++) 
		pthread_create(ctid + i, NULL, child_func, th + i);

	//destroying threads
	for(int i = 0; i < num_threads; i++) 
		pthread_join(ctid[i], NULL);

	for(int i = 0; i < num_threads; i++)
		close(th[i].fd);

	pthread_mutex_destroy(&ptmutex);
	free(ctid);
	free(th);
}

int main(int argc, char* argv[]){

	if(argc != 3) abort("usage: <program> <filename> <num_threads>\n");

	char* filename = argv[1];
	int num_threads = atoi(argv[2]);


	//call the check
	file_check(filename, num_threads);
}

