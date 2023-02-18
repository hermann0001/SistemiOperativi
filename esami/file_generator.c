//this program generates file with random characters
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>

#define POTENTIAL_SIZE 150000

int main(){

	for(int i = 0; i < 4; i++){
		char filename[] = "prova .txt";
		filename[5] = (i + 1) + '0';

		int fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0660);
		char* characters = malloc(POTENTIAL_SIZE);
		int random = open("/dev/random", O_RDONLY);
		ssize_t size_r = 0;
		read(random, characters, POTENTIAL_SIZE);
		for(int i = 0; i < POTENTIAL_SIZE; i++){
			if(/*characters[i] >= 32 && characters[i] <= 126 && */characters[i] == 'c')
				write(fd, &characters[i], 1);
		}
	}
}