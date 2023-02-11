/*
Scrivere un programma in C che:

prende inizialmente una stringa da input (può contenere anche spazi bianchi) e 
la salva in un buffer

fork-a un processo figlio che manda in stampa la stessa stringa acquisita dal processo padre.

Il processo padre termina solo dopo che il processo figlio ha terminato 
(verificare che tale ordine è rispettato stampando i PID dei processi)
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#define abort(msg) do{printf(msg); exit(1);}while(0)

int main(int argc, char* argv[]){

	if(argc != 2)
		abort("usage: <stringa>\n");

	printf("sono il processo %d e sto per creare un figlio\n", getpid());
	pid_t child = fork();
	int status;

	if(child < 0) abort("i cannot create the child\n");
	else if (child == 0){
		sleep(5);
		printf("sono il figlio! pid: %d\n", getpid());
		printf("%s\n",argv[1]);
		exit(1);
	} else{
		printf("sono il padre e sto aspettando la terminazione di mio figlio\n");
		if(wait(&status) == -1) abort("errore\n");
		printf("My child has invoked exit(%d)\n", WEXITSTATUS(status));
	}

	printf("mio figlio è morto\n");
	exit(0);
}