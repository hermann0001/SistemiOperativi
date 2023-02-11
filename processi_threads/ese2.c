/*
Scrivere un programma in C che:

prende inizialmente una stringa da input (può contenere anche spazi bianchi) e l
a salva in un buffer

fork-are 2 processi figli che contribuiscono a stampare la stringa inversa 
della stringa acquisita dal processo padre.

Il processo padre termina solo dopo che i processi figli hanno terminato.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#define abort(msg) do{printf(msg); exit(1);}while(0)

int main(int argc, char* argv[]){
	if(argc != 1) abort("usage: <nome programma> <stringa>\n");

	pid_t child1, child2;
	int status1, status2;

	char* buffer[256];

	scanf("%[^\n]%*c", buffer);


	int taglia = strlen(buffer);

	printf("sono il parent e sto per creare il primo figlio! pid = %d\n", getpid());

	child1 = fork();

	if(child1 < 0) abort("errore nella creazione del primo figlio\n");
	else if(child1 == 0){
		sleep(3);
		printf("\nsono il primo figlio! pid = %d\n", getpid());

		printf("sono il primo figlio, procedo ad invertire la stringa\n");
		for(int i = taglia; i >= 0; i--)
			printf("%c", buffer[i]);
		printf("\n\n");

		exit(1);
	} else {
		printf("sono il parent e sto per creare il secondo figlio! pid = %d\n", getpid());
		child2 = fork();
		if(child2 < 0) abort("errore nella creazione del secondo figlio\n");
		else if(child2 == 0){
			sleep(5);
			printf("\nsono il secondo figlio! pid = %d\n", getpid());

			printf("sono il secondo figlio, procedo ad invertire la stringa\n");
			for(int i = taglia; i >= 0; i--)
				printf("%c", buffer[i]);
			printf("\n\n");

		} else{
			printf("sono il parent e sto aspettando la terminazione dei miei figli\n");
			if(child1 != waitpid(child1, &status1, 0)) abort("errore nella terminazione del primo figlio\n");
			if(child2 != waitpid(child2, &status2, 0)) abort("errore nella terminazione del secondo figlio\n");

			exit(0);
		}
	}
}
