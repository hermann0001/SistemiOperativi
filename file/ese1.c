/*
Nei sistemi operativi UNIX, /dev/urandom è un dispositivo a caratteri 
(char device) virtuale in grado di generare numeri casuali. Nello specifico, 
l’operazione di lettura dal relativo file produce byte casuali. 
Scrivere un programma C che genera un file con contenuto interamente randomico. 
Il programma:
prende come parametri da linea di comando: un numero N e una stringa S 
da usare come nome del file da creare;

crea un file S contenente N byte randomici;

utilizza il dispositivo /dev/random come sorgente di numeri pseudo-casuali.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define abort(msg) do{printf(msg); exit(1);}while(0)
#define abort_arg(msg, arg) do{printf(msg,arg); exit(1);}while(0)
#define URANDOM "/dev/urandom"
#define RANDOM  "/dev/random"

int main(int argc, char* argv[]){
	if(argc != 3) abort("usage: <nomeprogramma> <intero N> <stringa S>\n");

	int end = 0, size_r, size_w;
	size_t n = atoi(argv[1]);
	char* buffer[n];

	int output_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0660);
	int input_fd  = open(URANDOM, O_RDONLY, 0660);
	if(output_fd == -1) abort_arg("errore nell'apertura del file %s\n", argv[2]);
	if(input_fd == -1) abort_arg("errore nell'apertura del file %s\n", RANDOM);

	//scrivo su output e leggo da input
	size_r = read(input_fd, buffer, n);
	if(size_r == -1) abort("read error\n");
	
	size_w = write(output_fd, buffer, n);
	if(size_w == -1) abort("write error\n");

	if(close(input_fd) == -1) abort_arg("errore nella chiusura del file %s\n", argv[2]);
	if(close(output_fd) == -1) abort("errore nella chiusura del file /dev/urandom\n");

}