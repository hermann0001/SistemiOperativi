//this program generates file with random characters
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>

#define POTENTIAL_SIZE 150000
#define FILE_COUNT 1

int main(){

	for(int i = 0; i < FILE_COUNT; i++){
		char filename[] = "palindrome";
		//filename[5] = (i + 1) + '0';

		int fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0660);
		//char* characters = malloc(POTENTIAL_SIZE);
		//int random = open("/dev/random", O_RDONLY);
		//ssize_t size_r = 0;
		//read(random, characters, POTENTIAL_SIZE);

		char palindrome[49]     = "abcdabcdabcdabcdabcdabcdedcbadcbadcbadcbadcbadcba";
		char not_palindrome[49] = "zxcvbzxcvbzxcvbzxcvbzxcvbzxcvbzxcvbzxcvbzxcvbzxcv";

		for(int i = 0; i < 8000; i++){
			/*if(characters[i] >= 32 && characters[i] <= 126 && characters[i] == 'c')*/
				write(fd, palindrome, 49);
				write(fd, "\0", 1);
				write(fd, not_palindrome,49);
				write(fd, "\0", 1);
		}
	}
}