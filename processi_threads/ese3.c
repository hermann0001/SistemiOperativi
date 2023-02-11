/*
Scrivere un programma in C che:

prende inizialmente N (a piacere) stringhe rappresentanti N directory corrette

fork-a quindi N processi che andranno ad eseguire il comando ls su una directory differente.

Il processo padre termina dopo i processi figli
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#define abort(msg) do{printf(msg); exit(1);}while(0)
#define COMANDO "ls"

int main(int argc, char* argv[]){
	if(argc < 1) abort("usage: <nomeprogramma> <stringhe>...\n");
	pid_t child;
	int status;
	char argomento[256];

	while(1){
		printf("Inserisci la directory da listare\n");
		int res = scanf("%s", argomento);
		if(res == EOF) abort("Errore nella lettura\n");

		child = fork();
		if(child < 0) abort("errore nella creazione del figlio\n");
		else if(child == 0)
			execlp(COMANDO, COMANDO, argomento, NULL);
		else wait(&status);
	}

	exit(1);
}