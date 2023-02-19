/*
Si scriva un programma C che prende come argomenti una sequenza di nomi di file.
Per ciascuna coppia di file, il programma crea un nuovo thread/processo il quale:
• verifica se i due possono essere letti e se i rispettivi byte in posizione 0 sono uguali tra loro;
• scrive su standard output il nome dei due file analizzati e l’esito del controllo.
Il programma prima di terminare scrive su standard output il numero di coppie di file che hanno
superato il controllo.
Nota: il controllo di coppie distinte deve essere concorrente
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <pthread.h>
#include <string.h>

#define abort(msg) do{printf(msg); exit(1);}while(0)

volatile unsigned count;

void* child_func(void* args){
	char** files = (char**)args;

	char* file1 = files[0];
	char* file2 = files[1];

	int fd[2];
	fd[0] = open(file1, O_RDONLY);
	fd[1] = open(file2, O_RDONLY);
	if(fd[0] == -1|| fd[1] == -1) abort("cannot read files\n");

	char bytes[2];
	
	if((read(fd[0], &bytes[0], 1)) == -1) abort("reading error\n");
	if((read(fd[1], &bytes[1], 1)) == -1) abort("reading error\n");

	int check = bytes[0] == bytes[1];
	__sync_fetch_and_add(&count, check);
	printf("%s, %s, check: %d\n", file1, file2, check); 

	close(fd[0]); 
	close(fd[1]);
	pthread_exit(NULL); 
}

int main(int argc, char* argv[]){
	if(argc < 3 ||(argc % 2) != 1) abort("insert couples of filename\n");
	count = 0;
	int couples = (argc - 1) / 2;
	pthread_t* ctid = (pthread_t*)malloc(sizeof(pthread_t) * couples);

	char*** filename_couple =(char***)malloc(sizeof(char**) * couples);
	for(int i = 0; i < couples; i++){
		filename_couple[i] = (char**)malloc(sizeof(char*) * 2);
	}

	for(int i = 1; i < argc; i+=2){
		filename_couple[i/2][(i%2) - 1] = argv[i];
		filename_couple[i/2][i%2]       = argv[i+1];
		pthread_create(ctid + (i/2), NULL, child_func, filename_couple[(i/2)]);
	}

	for(int i = 0; i < couples; i++)
		pthread_join(ctid[i], NULL);

	free(filename_couple);
	printf("couples count: %d\n", count);
	free(ctid);
}
