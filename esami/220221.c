/*
Si scriva una funzione di codice C con la seguente interfaccia:
int count_valid_string (char *filename, char *charset, int n).
Tale funzione deve lanciare un numero di thread/processi pari a n per leggere dal file
filename una sequenza di stringhe e, per ciascuna di esse, contare quante di queste
contengono tutti i caratteri contenuti nella stringa charset. Infine, la funzione stampa su
standard output e ritorna il numero totale di stringhe identificato.
Il file è tale per cui, ciascuna stringa è lunga 50 caratteri incluso il terminatore di linea.
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <pthread.h>

#define abort(msg) do{printf(msg), exit(1);}while(0);
#define FILENAME "stringset.txt"
#define CHARSET "abde"
#define BUFSIZE 2000
#define LENGTH 50

int fd = 0;
unsigned count;

int isSubset(char* string, char* set){w
	int found = 0;
	for(int i = 0; string[i] != '\0'; i++){
		int j = 0;
		while(!found && set[j] != '\0'){
			found = string[i] == set[j];
			j++;
		}
	}
	return found;
}

void* child_func(void* arg){
	char* charset = (char*)arg;

	char* buffer = (char*)malloc(BUFSIZE);

	ssize_t size_r;
	do{
		size_r = read(fd, buffer, BUFSIZE);
		if(size_r == -1) abort("error on reading");

		for(int i = 0; i < LENGTH; i++){
			is_subset(buffer, CHARSET);

		}

	}while(size_r > 0)

}

int count_valid_string(char* filename,char *charset, int n){

	pthread_t* ctid = (pthread_t*)malloc(sizeof(pthread_t) * n);
	

	fd = open(filename, O_RDONLY);
	if(fd == -1) abort("error on reading\n");

	for(int i = 0; i < n; i++) pthread_create(ctid + i, NULL, child_func, charset);

	for(int i = 0; i < n; i++) pthread_join(ctid[i], NULL);

	close(fd);
	free(ctid);

	printf("count = %d\n", count);
	return count;
}




int main(int argc, char* argv[]){
	count_valid_string(FILENAME, CHARSET, 1);

}