//this program generates file with random characters
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>

#define POTENTIAL_SIZE 150000
#define FILE_COUNT 4

int main(){

	for(int i = 0; i < FILE_COUNT; i++){
		char filename[] = "prova .txt";
		filename[5] = (i + 1) + '0';

		int fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0660);
		//char* characters = malloc(POTENTIAL_SIZE);
		//int random = open("/dev/random", O_RDONLY);
		//ssize_t size_r = 0;
		//read(random, characters, POTENTIAL_SIZE);

		char string[49]     = "ciaociaociaociaociaociaociaociaociaociaociaociaoc";
		//char not_palindrome[49] = "zxcvbzxcvbzxcvbzxcvbzxcvbzxcvbzxcvbzxcvbzxcvbzxcv";

		for(int i = 0; i < 10; i++){
			write(fd, string, 49);
			write(fd, "\n", 1);
		}
		close(fd);
	}
}