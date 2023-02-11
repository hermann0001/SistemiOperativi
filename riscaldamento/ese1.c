/* Scrivere un programma che:
--prende una stringa da tastiera e la inserisce all'interno di un buffer allocato
  dinamicamente nell'heap da parte della funzione scanf()
--copia poi tale stringa all'interno di un secondo buffer allocato sullo stack della taglia
  necessaria a contenerla
--libera quindi il buffer allocato nell'heap
--stampa sullo schermo la stringa copiata nel buffer allocato sullo stack
*/
#include <stdio.h>
#include <stdlib.h>

int main(void){

	char* stringa_heap = malloc(sizeof(char));
	int taglia = 0;  

	printf("Scrivi una stringa di caratteri\n");
	scanf("%ms %*s", &stringa_heap);

	while(stringa_heap[taglia++] != '\0');

	for(int i = 0; i < taglia; i++)
		stringa_stack[i] = stringa_heap[i];

	free(stringa_heap);

	printf("Ecco la stringa:%s\n", stringa_stack);

	return 0;
}