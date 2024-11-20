#include <stdio.h>
#include <stdlib.h>

int main(){
	char stringa[5];
	char a;	

	printf("inserisci una stringa\n");
	scanf("%5[^\n]%*c", stringa);
	printf("stringa: %s\n", stringa);
	printf("%d\n", stringa[0]);
	printf("%d\n", stringa[1]);
	printf("%d\n", stringa[2]);
	printf("%d\n", stringa[3]);
	printf("%d\n", stringa[4]);
	printf("%x\n", stringa[5]);

	fflush(stdin);
	printf("insert char:");
	scanf("%c", &a);
	printf("char: %c\n", a);
}