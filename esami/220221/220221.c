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
#include <string.h>


#define abort(msg) do{printf(msg), exit(1);}while(0);
#define FILENAME "stringset.txt"
#define CHARSET "abcde"
#define BUFSIZE 2000
#define LENGTH 50

unsigned count;
unsigned string_count;
unsigned thread_count;
off_t filesize;

typedef struct t{
	int fd;
	int id;
}THREAD_DATA;

char* charset_global;

int char_contained(char c, char* s){
	for(int i = 0; s[i] != '\0'; i++){
		if(c == s[i]) return 1;
	}
	return 0;

}
int is_subset(char* string, char* s){
	for(int i = 0; string[i] != '\0'; i++){
		if(!char_contained(string[i], s))
			return 0;
	}
	return 1;
}

void* child_func(void* arg){
	THREAD_DATA th = *(THREAD_DATA*)arg;

	//assigning strings to each thread
	unsigned strings = string_count / thread_count;
	unsigned remainder = string_count % thread_count;
	unsigned my_strings = strings;

	//give remainder to the last thread
	if(remainder > 0){
		if(th.id == thread_count - 1)  my_strings += remainder;
	}

	printf("tid = %d, my strings: %d\n", th.id, my_strings);


	char* buffer = malloc(my_strings * LENGTH);
	
	off_t offset = lseek(th.fd, (th.id * LENGTH * strings), SEEK_SET);

	ssize_t size_r = read(th.fd, buffer, LENGTH * my_strings);
	if(size_r == -1) abort("error on reading\n");



	for(int i = 0; i < my_strings; i++)
		__sync_fetch_and_add(&count, is_subset(buffer + (i * LENGTH), charset_global));
}

int count_valid_string(char* filename,char *charset, int n){
	count = 0;
	if(n < 1) n = 1;
	thread_count = n;
	charset_global = (char*)malloc(strlen(charset));
	strcpy(charset_global, charset);

	pthread_t* ctid = (pthread_t*)malloc(sizeof(pthread_t) * thread_count);
	THREAD_DATA* th = (THREAD_DATA*)malloc(sizeof(THREAD_DATA) * thread_count);

	for(int i = 0; i < n; i++){
		th[i].fd = open(filename, O_RDONLY);
		th[i].id = i;
	}

	filesize = lseek(th[0].fd, 0, SEEK_END);
	lseek(th[0].fd, 0, SEEK_SET);

	string_count = filesize / LENGTH;
	if((filesize % LENGTH) > 0) abort("maybe you're assumption was wrong...\n");

	for(int i = 0; i < n; i++) pthread_create(ctid + i, NULL, child_func, th + i);

	for(int i = 0; i < n; i++) pthread_join(ctid[i], NULL);

	for(int i = 0; i < n; i++) close(th[i].fd);
	free(ctid);
	free(th);

	printf("count = %d\n", count);
	return count;
}

int main(int argc, char* argv[]){
	

	//creating test file
	
	int test = open("stringset.txt", O_APPEND | O_WRONLY, 0660);
	char* buffer = malloc(LENGTH);
	/*
	buffer = "abcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcd\0";

	write(test, buffer, LENGTH);
	*/
	/*
	for(int i = 0; i < 10000000; i++){
		for(int j = 0; j < LENGTH-1; j++)
			buffer[j] = 'a' + (random() % 26);
		buffer[LENGTH - 1] = '\0';
		write(test, buffer, LENGTH);
	}*/

	count_valid_string(FILENAME, CHARSET, 16);
}

//works! even with a 500.5MB stringset for a total of 10.010.503 strings
//but there is a strong assumption, the program will not work if the stringset 
//does not contain exactly strings of 50 characters null terminated each...