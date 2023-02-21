/*
Scrivere un programma in C che:

prende inizialmente una stringa da input (può contenere anche spazi bianchi) e l
a salva in un buffer

crea due thread figli che contribuiscono a stampare la stringa inversa 
della stringa acquisita dal processo padre.

Il processo padre termina solo dopo che i thread figli hanno terminato.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#include <pthread.h>

#define abort(msg) do{printf(msg); exit(1);}while(0)

void* child_func(void* buffer);

int main(int argc, char* argv[]){

	if(argc != 1) abort("usage: <nome programma> <stringa>\n");

	pthread_t ctid1, ctid2;
	char buffer[256];

	printf("Inserisci una stringa\n");
	scanf("%[^\n]%*c", buffer);
	int taglia = strlen(buffer);

	printf("tid = %lu, sono il parent e sto per generare due thread figli\n", pthread_self());

	int child1 = pthread_create(&ctid1, NULL, child_func, buffer);
	int child2 = pthread_create(&ctid2, NULL, child_func, buffer);

	if(child1 != 0) abort("errore nella creazione del primo thread\n");
	if(child2 != 0) abort("errore nella creazione del secondo thread\n");

	printf("tid = %lu, sono il parent e aspetto la terminazione dei figli\n", pthread_self());
	pthread_join(ctid1, NULL);
	pthread_join(ctid2, NULL);

	printf("tid = %lu, i miei figli hanno terminato, ora termino anche io\n", pthread_self());
	exit(0);
}

void* child_func(void* arg){
	char* stringa = (char*)arg;
	int taglia = strlen(stringa);
	for(int i = 0; i < taglia/2; i++){
		stringa[i] ^= stringa[taglia-1-i];
		stringa[taglia-1-i] ^= stringa[i];
		stringa[i] ^= stringa[taglia-1-i];
	}

	printf("tid = %lu, sono il child e sto per stampare la stringa invertita\nstringa: %s\n", pthread_self(),stringa);

	pthread_exit(0);
}