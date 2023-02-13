/*
Scrivere un programma in C che:

prende inizialmente una stringa da input (può contenere anche spazi bianchi) e 
la salva in un buffer

crea un thread figlio che manda in stampa la stessa stringa acquisita dal processo padre.

Il processo padre termina solo dopo che il thread figlio ha terminato 
(verificare che tale ordine è rispettato stampando i PID dei processi).
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#include <pthread.h>

#define abort(msg) do{printf(msg); exit(1);}while(0)

void* child_func(void* arg){
	char* message;
	message = (char*)arg;
	printf("Sono il thread figlio, tid = %lu\n", pthread_self());
	printf("Procedo a stampare la stringa: %s\n", message);
	pthread_exit(0);
}

int main(int argc, char* argv[]){
	char buffer[256];
	printf("inserisci una stringa\n");
	scanf("%[^\n]%*c", buffer);

	pthread_t ctid;
	int child; //*status_ptr, status_val;
	//status_ptr = &status_val;

	printf("sto per creare un thread, tid = %lu\n", pthread_self());
	child = pthread_create(&ctid, NULL,child_func, (void*)buffer);
	if(child != 0) abort("cannot create thread child");
	else {
		printf("sono il parent thread e aspetto che il thread figlio termini l'esecuzione, tid = %lu\n", pthread_self());
		pthread_join(ctid, NULL);
		printf("mio figlio ha completato...\n");	
	}
	exit(0);
}