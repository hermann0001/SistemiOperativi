/*
Si scriva una funzione di codice C con la seguente interfaccia:
int check_palindrome(char *filename, int n).
Tale funzione deve lanciare un numero di thread/processi pari a n per leggere dal file
filename una sequenza di stringhe e contare quante di queste siano palindrome. Infine, la
funzione stampa su standard output e ritorna il numero totale di stringhe identificato.
Il file è tale per cui, ciascuna stringa è lunga 50 caratteri incluso il terminatore di linea.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>

#define abort(msg) do{printf(msg); exit(1);}while(0)
#define LENGTH 50
#define CHUNK_SIZE 1000 //read 20 string at the time

int thread_count;
int fd;
volatile int string_count;

int is_palindrome(char* string){
	int size = strlen(string);
	if((size + 1) != LENGTH) abort("the string is not 50 characters long\n");

	//go from 0 -> 23 and 48->25
	//no check needed for 25th character or "middle character"
	//example: RADAR --- D is the middle character
	for(int i = 0; i < size/2; i++)
		if(string[i] != string[size-i-1]) return 0;
	
	return 1;
}

void* child_func(void* arg){
	char* buffer = malloc(CHUNK_SIZE);
	ssize_t size_r;

	while((size_r = read(fd, buffer, CHUNK_SIZE)) > 0){
		if(size_r == -1) abort("error reading\n");

		ssize_t start_byte = 0;
		while(start_byte < size_r){
			__sync_fetch_and_add(&string_count, is_palindrome(buffer + start_byte));
			start_byte += LENGTH;
		}
	}

	pthread_exit(NULL);
}

int check_palindrome(char* filename, int n){
	if(n < 1) n = 1;
	thread_count = n;
	string_count = 0;
	fd = open(filename, O_RDONLY);

	pthread_t* ctid = (pthread_t*)malloc(sizeof(pthread_t) * thread_count);
	if(fd == -1) abort("error opening file\n");

	for(int i = 0; i < thread_count; i++) pthread_create(ctid + i, NULL, child_func, NULL);

	for(int i = 0; i < thread_count; i++) pthread_join(ctid[i], NULL);

	close(fd);
	free(ctid);

	printf("palindromi: %d\n", string_count);
	return string_count;
}

int main(){

	check_palindrome("palindrome", 4);

}