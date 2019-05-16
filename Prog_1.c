/* 
* RTOS Autumn 2019
* Assignment 3 Program_1 template
*
*/

#include <pthread.h> 	/* pthread functions and data structures for pipe */
#include <unistd.h> 	/* for POSIX API */
#include <stdlib.h> 	/* for exit() function */
#include <stdio.h>		/* standard I/O routines */
#include <stdbool.h>
#include <string.h>
#include <semaphore.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/stat.h>

typedef struct RR_Params {
  //add your variables here
  
} ThreadParams;



/* this function calculates CPU SRTF scheduling, writes waiting time and turn-around time to th FIFO */
void *worker1(void *params)
{
   // add your code here
}

/* reads the waiting time and turn-around time through the FIFO and writes to text file */
void *worker2()
{
   // add your code here
}

/* this main function creates named pipe and threads */
int main(void)
{
	/* creating a named pipe(FIFO) with read/write permission */
	// add your code 

	/* initialize the parameters */
	 // add your code 
	
	/* create threads */
	 // add your code
	
	/* wait for the thread to exit */
	//add your code
	
	return 0;
}
