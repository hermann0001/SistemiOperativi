#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>

int main(){
	printf("sizeof(pthread_mutex_t)   = %ld\n", sizeof(pthread_mutex_t));
	printf("sizeof(pthread_barrier_t) = %ld\n", sizeof(pthread_barrier_t));
	printf("sizeof(pthread_t)         = %ld\n", sizeof(pthread_t));
	printf("sizeof(size_t)            = %ld\n", sizeof(size_t));
	printf("sizeof(ssize_t)           = %ld\n", sizeof(ssize_t));
	printf("sizeof(off_t)             = %ld\n", sizeof(off_t));
	printf("sizeof(int)               = %ld\n", sizeof(int));
	printf("sizeof(unsigned int)      = %ld\n", sizeof(unsigned int));
	printf("sizeof(short)             = %ld\n", sizeof(short));
	printf("sizeof(unsigned short)    = %ld\n", sizeof(unsigned short));
	printf("sizeof(long)              = %ld\n", sizeof(long));
	printf("sizeof(unsigned long)     = %ld\n", sizeof(unsigned long));
	printf("sizeof(void)              = %ld\n", sizeof(void));
	printf("sizeof(void*)             = %ld\n", sizeof(void*));
	printf("sizeof(char)              = %ld\n", sizeof(char));
	printf("sizeof(char*)             = %ld\n", sizeof(char*));
	printf("64K in byte               =  %d\n", 1<<16);

	printf("\nEOF = %d\n", EOF);
}