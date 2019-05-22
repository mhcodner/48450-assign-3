/*
* RTOS Autumn 2019
* Assignment 3 Program_2
* For compiling this, no flags are necessary but good to check for warnings
* e.g.    gcc Prog_2.c -o prog_2 -Wall
* When executing prog_2, please provide the number of frames for the document as a parameter
* e.g.    ./prog_2 4
*
*/

#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>

//Number of pagefaults in the program
int pageFaults = 0;

//Function declaration
void SignalHandler(int signal);

/**
 Main routine for the program. In charge of setting up threads and the FIFO.

 @param argc Number of arguments passed to the program.
 @param argv array of values passed to the program.
 @return returns 0 upon successful completion, -1 for errors.
 */
int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    printf("Please provide 1 argument: the number of frames in the document\n");
    printf("e.g. %s 4\n", argv[0]);
    exit(-1);
  }

  //Register Ctrl+c(SIGINT) signal and call the signal handler for the function.
  signal(SIGINT, SignalHandler);

  int i;
  // reference number
  int REFERENCESTRINGLENGTH = 24;
  //Argument from the user on the frame size, such as 4 frames in the document
  int frameSize = atoi(argv[1]);
  //Frame where we will be storing the references. -1 is equivalent to an empty value
  unsigned int frame[REFERENCESTRINGLENGTH];
  //Reference string from the assignment outline
  int referenceString[24] = {7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3, 0, 3, 2, 1, 2, 0, 1, 7, 0, 1, 7, 5};
  //Next position to write a new value to.
  int nextWritePosition = 0;
  //Boolean value for whether there is a match or not.
  bool match = false;
  //Current value of the reference string.
  int currentValue;

  //Initialise the empty frame with -1 to simulate empty values.
  for (i = 0; i < frameSize; i++)
  {
    frame[i] = -1;
  }

  //Loop through the reference string values.
  for (i = 0; i < REFERENCESTRINGLENGTH; i++)
  {
    //add your code here
  }

  //Sit here until the ctrl+c signal is given by the user.
  while (1)
  {
    sleep(1);
  }

  return 0;
}

/**
 Performs the final print when the signal is received by the program.

 @param signal An integer values for the signal passed to the function.
 */
void SignalHandler(int signal)
{
  if (signal == SIGINT)
  {
    printf("\nTotal page faults: %d\n", pageFaults);
    exit(0);
  }
}
