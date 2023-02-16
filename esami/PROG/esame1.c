#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

#define FILENAME "dataset.bin"
#define NUM_THREADS 4
#define NUM_THRESHOLDS 3

int thresholds[NUM_THRESHOLDS] = {0x0, 0x40000000, 0xc0000000};


int above_threshold(char *filename, int count, int threshold){
   int total_count = 0;
   return total_count;
}


int main(){
  int res;
  int i;
  for(i=0;i<NUM_THRESHOLDS;i++){
    res = above_threshold(FILENAME, NUM_THREADS, thresholds[i]);
    printf("The correct answer for %d is %d\n", thresholds[i], res);
  }
}

