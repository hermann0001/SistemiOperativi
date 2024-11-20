/*
Si scriva una funzione di codice C con la seguente interfaccia:
int filter_and_tunnel(int descriptors[], int count, char c, int fd_log)
Tale funzione porta l’applicazione a gestire, per ogni file-descriptor dell’array descriptors l’inoltro e
filtro del flusso dei dati in ingresso verso il file-descriptor fd_log.
Nello specifico:
• il parametro count indica di quanti elementi è costituito l’array descriptors
• gli stream producono sequenze di 49 caratteri terminanti con ‘\n’, chiamate linee
• la funzione ridireziona su fd_log le sole linee che iniziano per il carattere c
• l’inoltro dovrà essere attuato in modo concorrente per i diversi canali
• l’inoltro termina non appena si riceve una linea vuota
• quando tutti gli stream sono terminati, la funzione stampa il numero minimo di linee
redirezionate su fd_log da un file descriptor
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <pthread.h>

#define abort(msg) do{printf(msg); exit(1);}while(0);
#define LINE_SIZE 50

typedef struct t{
	int stream;
	int log;
	int line_count;
	char character;
}THREAD_DATA;

void* child_func(void* arg){
	THREAD_DATA* th = (THREAD_DATA*)arg;
	ssize_t size_r, size_w;

	char line[LINE_SIZE];

	while((size_r = read(th->stream, line, LINE_SIZE)) > 0){
		if(size_r == -1) abort("error reading\n");

		//empty line
		if(line[0] == '\n') break;

		//if the line start with the character c, redirect it
		if(line[0] == th->character){
			size_w = write(th->log, line, LINE_SIZE);
			if(size_w == -1) abort("error writing\n");
			th->line_count++;
		}
	}
	pthread_exit(NULL);
}


int filter_and_tunnel(int descriptors[], int count, char c, int fd_log){
	pthread_t* ctid = (pthread_t*)malloc(sizeof(pthread_t) * count);
	THREAD_DATA* th = (THREAD_DATA*)malloc(sizeof(THREAD_DATA) * count);


	for(int i = 0; i < count; i++){
		th[i].stream     = descriptors[i];
		th[i].log        = fd_log;
		th[i].character  = c;
		th[i].line_count = 0;
		pthread_create(ctid + i, NULL, child_func, th + i);
	}

	for(int i = 0; i < count; i++)
		pthread_join(ctid[i], NULL);

	//evaluating minimum between x and y: y ^ ((x ^ y) & -(x < y))
	int min = th[0].line_count;
	printf("line count: %d\n", th[0].line_count);

	for(int i = 1 ; i < count; i++)
		min = min ^ ((th[i].line_count ^ min) & -(th[i].line_count < min));

	for(int i = 0; i < count; i++)
		close(descriptors[i]);

	free(ctid);
	free(th);

	printf("n. min of redirect lines: %d\n", min);
	return min;
}

int main(){
	int fd[4];
	int log;
	fd[0] = open("prova1.txt", O_RDONLY);
	fd[1] = open("prova2.txt", O_RDONLY);
	fd[2] = open("prova3.txt", O_RDONLY);
	fd[3] = open("prova4.txt", O_RDONLY);
	log = open("log.txt", O_CREAT | O_WRONLY | O_TRUNC, 0660);
	filter_and_tunnel(fd, 4, 'c', log);

}