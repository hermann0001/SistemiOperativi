/*********************************************************************
 *  _____  ______          _____  __  __ ______ 
 * |  __ \|  ____|   /\   |  __ \|  \/  |  ____|
 * | |__) | |__     /  \  | |  | | \  / | |__   
 * |  _  /|  __|   / /\ \ | |  | | |\/| |  __|  
 * | | \ \| |____ / ____ \| |__| | |  | | |____ 
 * |_|  \_\______/_/    \_\_____/|_|  |_|______|
 * 
 * *******************************************************************
 * 
 * 
 * DO NOT FORGET TO FILL THE FOLLOWING WITH YOUR PERSONAL DATA
 * First Name:
 * Last Name:
 * Student Id:
 * 
 * 
 * ***********************************/


/***************************************************************************************************
SPECIFICA (ITA):

Implementare una programma tale che prende come argomento 2 nomi di file F1 e F2 
- Il main thread crea due thread/processi T1 e T2
- il T1 e T2 leggono ripetutamente da standard input stringhe (senza spazi) senza alcun ordine preciso
- T1 scrive la stringa letta in coda al file F1
- T2 scrive la stringa letta in testa al file F2
- T1 e T2 terminano quando lo stream su standard input è terminato
- quando T1 e T2 terminano, il main thread stampa a schermo il numero di byte complessivamente scritta da T1 e T2 e termina

// Nota: Assumere che le stringhe hanno tutte la stessa taglia


SPECS (ENG):

Implement a program that takes 2 filenames F1 and F2 as arguments
- The main thread creates two threads/processes T1 and T2
- T1 and T2 repeatedly read from standard input strings (without spaces) without any precise order
- T1 writes the string read at the end of file F1
- T2 writes the string read at the head of the file F2
- T1 and T2 end when the standard input stream is finished
- when T1 and T2 terminate, the main thread prints the total number of bytes written by T1 and T2 and terminates

// Note: you can assume that strings keeps the same number of chars


****************************************************************************************************/


/***********************
 * Additional exercise:
 * make T1 and T2 access the standard input in a strictly alternating manner (T1, T2, T1, T2 and so on)
 **********************/


/*****************************
 * READY TO USE HEADERS
 *****************************/





#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>

#define abort_with_mes(x) do{printf("%s\n", x);exit(1);}while(0)

typedef struct exam
{
 unsigned long result;   
 long (*write_func)(int fd, const void* buf, unsigned long size); // // What's happening here?
 int fd;
} th_data_struct;


long head_write(int fd, const void* buf, unsigned long size){
    char *tmp = malloc(size);               // Exploits the assumption from the specification
    unsigned long written = 0;
    unsigned long to_be_copied = lseek(fd, 0, SEEK_END); // go to the end of file
    size++; // not required just to print nicely formatted strings
    while(to_be_copied != 0){               
        lseek(fd, -size, SEEK_CUR);         // Exploits the assumption from the specification
        read(fd, tmp, size);                // read a string. There is an assumption here...
        written += write(fd, tmp, size);    // write again last string (overwriting last rewritten string). There is an assumption here...
        lseek(fd, -2*size, SEEK_CUR);       // Exploits the assumption from the specification
        to_be_copied -= size;               // We replicated last string, proceed with the next  one
        
    }
    written += write(fd, buf, strlen(buf));
    free(tmp);
    return written;
}

void* thread_func(void *args){
    th_data_struct *th_data = (th_data_struct*) args;
    char *buf;
    int res = 0;
    while(1){
        res = scanf("%ms", &buf);
        if(res < 0) return NULL;
        th_data->result += 1+th_data->write_func(th_data->fd, buf, strlen(buf));     // // What's happening here? // There is also an assumption here...
        write(th_data->fd, "\n", 1);  // just to keep string in different lines. Not required for solving the problem
        free(buf);
    }
}


int main(int argc, char** argv) {
  unsigned long i = 0, total_bytes = 0;
  pthread_t tids[2];
  th_data_struct *th_data = malloc(2*sizeof(th_data_struct));

  if(argc != 3) abort_with_mes("command usage: exam <file1> <file2>");

  for(i=0;i<2;i++) {
    th_data[i].result = 0;                                                  // clean field
    
    th_data[i].fd = open(argv[1+i], O_RDWR | O_CREAT | O_TRUNC, 0600);      // open the file for child threads
    if(th_data[i].fd < 0) abort_with_mes("cannot create and open a file");

    if(i) th_data[i].write_func = head_write;   // What's happening here?
    else  th_data[i].write_func = write;         // What's happening here?
  }

  for(i=0;i<2;i++) pthread_create(tids+i, NULL, thread_func, th_data + i);
  
  for(i=0;i<2;i++) pthread_join(tids[i], NULL);
  
  for(i=0;i<2;i++) close(th_data[i].fd);

  for(i=0;i<2;i++) total_bytes += th_data[i].result;
  
  printf("total written bytes: %lu\n", total_bytes);
  
  free(th_data);

}

