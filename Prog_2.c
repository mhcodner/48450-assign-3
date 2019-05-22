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

#define LENGTH 24

//Number of pagefaults in the program
int pageFaults = 0;
//Argument from the user on the frame size, such as 4 frames in the document
int frameSize;

//Function declarations
void SignalHandler(int signal);
int IndexOfMax(unsigned int age[]);
int FrameContains(unsigned int haystack[], int needle);
void PrintFrame(unsigned int frame[]);

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

  if (!(frameSize = atoi(argv[1])) || frameSize < 0)
  {
    printf("Please provide a valid number greater than 0\n");
    exit(-1);
  }

  //Register Ctrl+c(SIGINT) signal and call the signal handler for the function.
  signal(SIGINT, SignalHandler);
  
  // reference number
  int i;

  //Frame where we will be storing the references. -1 is equivalent to an empty value
  unsigned int frame[frameSize];
  //Reference string from the assignment outline
  int referenceString[LENGTH] = {7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3, 0, 3, 2, 1, 2, 0, 1, 7, 0, 1, 7, 5};
  //Next position to write a new value to.
  int nextWritePosition = 0;
  //Current value of the reference string.
  int currentValue;

  unsigned int age[3] = {0, 0, 0};

  //Initialise the empty frame with -1 to simulate empty values.
  for (i = 0; i < frameSize; i++)
  {
    frame[i] = -1;
  }

  //Loop through the reference string values.
  for (i = 0; i < LENGTH; i++)
  {
    currentValue = referenceString[i];

    // We only change the frames if the value isn't already there
    if ((FrameContains(frame, currentValue)) == -1)
    {
      nextWritePosition = frame[i % frameSize] == -1 ? i % frameSize : IndexOfMax(age);
      frame[nextWritePosition] = currentValue;
      age[nextWritePosition] = 0;
      pageFaults++;
    }

    // Increase age of everything except for empty frames and what we just wrote
    for (int j = 0; j < frameSize; j++)
      if (frame[j] != -1 && j != nextWritePosition)
        age[j]++;

    PrintFrame(frame);
  }

  //Sit here until the ctrl+c signal is given by the user.
  while (1)
  {
    sleep(1);
  }

  return 0;
}

/**
 Returns the index of the highest value in the array

 @param age the array used to store age of frames
 @return the index of the highest value in the array
 */
int IndexOfMax(unsigned int age[])
{
  int max = 0;

  for (int i = 0; i < frameSize; i++)
    if (age[i] > age[max])
      max = i;

  return max;
}

/**
 Checks if the frame contains the value

 @param haystack the array to search through
 @param needle the value you are searching for
 @return the index of the value or -1 if it doesn't exist
 */
int FrameContains(unsigned int haystack[], int needle)
{
  for (int i = 0; i < frameSize; i++)
    if (haystack[i] == needle)
      return i;

  return -1;
}

/**
 Prints the frames in its current state

 @param frame the frames to be printed
 */
void PrintFrame(unsigned int frame[])
{
  printf("Current frame state: [");

  for (int i = 0; i < frameSize - 1; i++)
    printf("%d, ", frame[i]);

  printf("%d]\n", frame[frameSize - 1]);
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
