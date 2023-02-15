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

pthread_mutex_t ptmutex;
off_t filesize;

void* child_func(void* arg){
	int fd = *(int*)arg;
	ssize_t size_r;
	char* input_string = malloc(filesize);		//input_string cant be more than file size


	//read string from stdin
	pthread_mutex_lock(&ptmutex);
	printf("tid = %lu, inserisci la stringa: ",pthread_self());
	scanf("%s", input_string);
	printf("stringa: %s\n",input_string);
	pthread_mutex_unlock(&ptmutex);

	int size_string = strlen(input_string);

	char* buffer = malloc(size_string);

	off_t offset = 1;
	int found = 0;

	pthread_mutex_lock(&ptmutex);
	lseek(fd, 0, SEEK_SET);							//replace file index if other threads moved it
	//read the file and search for matches
	do{
		//read from file exact n. bytes as input_string
		size_r = read(fd, buffer, size_string);  
		if(size_r == -1) abort("error on reading file\n");

		//compare the string
		if(strcmp(input_string, buffer) == 0){
			//if they're equals, set found to 1 and print the string
			found = 1;
			printf("tid = %lu found the match: s1 = %s, s2 = %s\n",pthread_self(), input_string, buffer);
		}
		else
			lseek(fd, offset++, SEEK_SET);	//if no match was found, the file index is replaced by an offset of 1
	}while((!found) && (size_r >= size_string));
	pthread_mutex_unlock(&ptmutex);

	free(buffer);
	free(input_string);
	pthread_exit(NULL);
}

void file_check(char* file_name, int num_threads){

	//allocate the threads
	pthread_t* ctid = (pthread_t*)malloc(sizeof(pthread_t) * num_threads);
	pthread_mutex_init(&ptmutex, NULL);

	//opening the file
	int fd = open(file_name, O_RDONLY);
	if(fd == -1) abort("error opening file\n");

	//compute filesize
	filesize = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);

	//creating threads, descriptor as argument
	for(int i = 0; i < num_threads; i++) pthread_create(ctid + i, NULL, child_func, &fd);

	//destroying threads
	for(int i = 0; i < num_threads; i++) pthread_join(ctid[i], NULL);

	pthread_mutex_destroy(&ptmutex);
	close(fd);
	free(ctid);
}

int main(int argc, char* argv[]){

	if(argc != 3) abort("usage: <program> <filename> <num_threads>\n");

	char* filename = argv[1];
	int num_threads = atoi(argv[2]);


	//call the check
	file_check(filename, num_threads);
}

