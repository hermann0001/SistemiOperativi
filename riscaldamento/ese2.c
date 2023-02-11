/* Scrivere un programma che:
--prende una stringa passata come primo argomento (char* argv[]) al programma stesso
quando questo viene eseguito
--copia tale stringa all'interno di un buffer di dimensione fissa facendo attenzione
a non superare il limite imposto dalla taglia, e stamparla quindi sullo schermo
--rigira la stringa (primo carattere in ultima posizione, secondo carattere in penultima...)
senza fare utilizzo di un ulteriore buffer
--stampa anche questa stringa sullo schermo
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define abort(msg) do{printf(msg); exit(1);}while(0)

int main(int argc, char* argv[]){

	if(argc != 2)
		abort("usage: ese2 <stringa>\n");

	int taglia = strlen(argv[1]);
	char buffer[taglia];
	strcpy(buffer, argv[1]);
	
	for(int i = 0; i < taglia; i++)
		buffer[i] = argv[1][i];

	printf("La stringa e' %s\n", buffer);

	for(int i = 0; i < taglia/2; i++){
		buffer[i] ^= buffer[taglia-1-i];
		buffer[taglia-1-i] ^= buffer[i];
		buffer[i] ^= buffer[taglia-1-i];
	}

	printf("la stringa rigirata e' %s\n", buffer);
}