/*
 * RTOS Autumn 2019
 * Assignment 3
 * Compilation method: For compiling this source code, you should use two flags, -pthread and -lrt
 * e.g:     gcc Prog_1.c -o prog_1 -pthread -lrt -Wall
 * When executing Prog_1, provide the output txt file name
 * i.e:     ./prog_1 output.txt
 * The program will use output.txt as the output
 *

 The input data of the cpu scheduling algorithm is:
--------------------------------------------------------
Process ID           Arrive time          Burst time
    1			              8	    	            10
    2                   10                  3
    3                   14                  7
    4                   9                   5
    5                   16                  4
    6                   21                  6
    7                   26                  2
--------------------------------------------------------

*/

#include <pthread.h> /* pthread functions and data structures for pipe */
#include <unistd.h>  /* for POSIX API */
#include <stdlib.h>  /* for exit() function */
#include <stdio.h>   /* standard I/O routines */
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

#define NUM_PROCESSES 7
#define FIFO_PATH "/tmp/fifo"

typedef struct
{
  int *fd;
  int *numElements;
  int elementSizeInBytes;
} fifo_t;

typedef struct
{
  int pId;
  int arriveTime;
  int burstTime;
  int remainingBurstTime;
} TProcessData;

typedef struct RR_Params
{
  char const *const outFile;
  fifo_t *fifo;
  TProcessData *processData;
} ThreadParams;

sem_t writeToFileSem; // Semaphore
pthread_t tidA, tidB; // Thread IDs
pthread_attr_t attr;  // Thread attributes

/*
 * fifo_t * fifo - pointer to a fifo structure
 * char * description - pointer to an array of characters which describe the value
 * float value - value to write to the FIFO
 */
void writeToFIFO(fifo_t *fifo, char *description, float value)
{
  char buffer[fifo->elementSizeInBytes];
  if (snprintf(buffer, sizeof(buffer), "%s: %f.\n", description, value) < 0)
  {
    perror("Error writing to buffer.");
    exit(1);
  }
  if (write(*fifo->fd, buffer, sizeof(buffer)) == -1)
  {
    perror("Failed to write to FIFO.");
    exit(1);
  }
  *fifo->numElements += 1;
}

/* this function calculates CPU SRTF scheduling, writes waiting time and turn-around time to th FIFO */
void *worker1(void *params)
{
  ThreadParams *parameters = params;
  TProcessData *processData = parameters->processData;

  unsigned int time = 0; // Represents CPU ticks

  // CPU Calculation Descriptors
  char averageWaitingTimeDescription[] = "Average Waiting Time";
  char averageTurnaroundTimeDescription[] = "Average Turn-around Time";

  // Initialize Variables To Calculate Average Waiting Time and Average Turn-around Time
  float waitingTime = 0;
  float turnaroundTime = 0;
  float averageWaitingTime = 0;
  float averageTurnaroundTime = 0;

  int indexOfSmallestCpuBurstTime;
  int numProcessesComplete = 0;

  while (numProcessesComplete != NUM_PROCESSES)
  {
    // Find The Process with the Smallest Burst Time
    indexOfSmallestCpuBurstTime = -1;
    bool first = true;
    int i;
    for (i = 0; i < NUM_PROCESSES; i++)
    {
      if (time >= processData[i].arriveTime && processData[i].remainingBurstTime > 0)
      {
        if (first || processData[i].remainingBurstTime < processData[indexOfSmallestCpuBurstTime].remainingBurstTime)
        {
          indexOfSmallestCpuBurstTime = i;
          first = false;
        }
      }
    }

    // Simulate CPU Clock
    ++time;

    if (indexOfSmallestCpuBurstTime != -1) // Check For A Valid Index
    {
      processData[indexOfSmallestCpuBurstTime].remainingBurstTime -= 1; // Simulate CPU Burst

      if (processData[indexOfSmallestCpuBurstTime].remainingBurstTime == 0) // Check For Completed Process
      {
        // Summate Wait Time (wait time = end time - arrival time - burst time)
        waitingTime += time - processData[indexOfSmallestCpuBurstTime].arriveTime - processData[indexOfSmallestCpuBurstTime].burstTime;

        // Summate Turn-around Time (turn-around time = end time - arrive time)
        turnaroundTime += time - processData[indexOfSmallestCpuBurstTime].arriveTime;

        // Increment The Number of Processes CompletesimulateCpuSchedulerSem
        ++numProcessesComplete;
      }
    }
  }

  averageWaitingTime = waitingTime / NUM_PROCESSES;       // Calculate Average Waiting Time
  averageTurnaroundTime = turnaroundTime / NUM_PROCESSES; // Calculate Average Turn-around Time

  // Push Average Waiting Time To FIFO
  writeToFIFO(parameters->fifo, averageWaitingTimeDescription, averageWaitingTime);
  writeToFIFO(parameters->fifo, averageTurnaroundTimeDescription, averageTurnaroundTime);

  sem_post(&writeToFileSem);

  return 0;
}

/* reads the waiting time and turn-around time through the FIFO and writes to text file */
void *worker2(void *params)
{
  ThreadParams *parameters = params;

  FILE *outFile = fopen(parameters->outFile, "w");
  if (!outFile)
  {
    fprintf(stderr, "Invalid output file %s: %s\n", parameters->outFile, strerror(errno));
    exit(1);
  }

  sem_wait(&writeToFileSem);

  // Read From FIFO
  char buffer[parameters->fifo->elementSizeInBytes];
  for (int i = 0; i < *parameters->fifo->numElements; i++)
  {
    if (read(*parameters->fifo->fd, buffer, sizeof(buffer)) == -1) // Read Into Buffer
    {
      perror("Failed To Read From FIFO.");
      exit(1);
    }
    fputs(buffer, outFile); // Write To Output File
    if (ferror(outFile))
    {
      printf("Error Writing to File.\n");
    }
  }

  if (fclose(outFile) == EOF) // Close The File
  {
    perror("Failed To Close File.");
    exit(1);
  }

  // Cancel Threads On Completion
  pthread_cancel(tidA);
  pthread_cancel(tidB);

  return 0;
}

/*
 * process_data_t * processData - pointer to an array of struct which represents the processes and their data
 */
void initializeData(TProcessData *processData)
{
  processData[0].pId = 1;
  processData[0].arriveTime = 8;
  processData[0].burstTime = 10;
  processData[0].remainingBurstTime = 10;

  processData[1].pId = 2;
  processData[1].arriveTime = 10;
  processData[1].burstTime = 3;
  processData[1].remainingBurstTime = 3;

  processData[2].pId = 3;
  processData[2].arriveTime = 14;
  processData[2].burstTime = 7;
  processData[2].remainingBurstTime = 7;

  processData[3].pId = 4;
  processData[3].arriveTime = 9;
  processData[3].burstTime = 5;
  processData[3].remainingBurstTime = 5;

  processData[4].pId = 5;
  processData[4].arriveTime = 16;
  processData[4].burstTime = 4;
  processData[4].remainingBurstTime = 4;

  processData[5].pId = 6;
  processData[5].arriveTime = 21;
  processData[5].burstTime = 6;
  processData[5].remainingBurstTime = 6;

  processData[6].pId = 7;
  processData[6].arriveTime = 26;
  processData[6].burstTime = 2;
  processData[6].remainingBurstTime = 2;
}

/* this main function creates named pipe and threads */
int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    printf("Please supply 1 argument: the output file name.\n");
    exit(1);
  }

  if (sem_init(&writeToFileSem, 0, 0))
  {
    perror("Error Initializing Semaphore.\n");
    exit(1);
  }

  pthread_attr_init(&attr);

  /* creating a named pipe(FIFO) with read/write permission */
  if (mkfifo(FIFO_PATH, 0666) == -1)
  {
    perror("Issue creating mkfifo.\n");
    exit(1);
  }

  int fd = open(FIFO_PATH, O_RDWR);
  if (fd == -1)
  {
    perror("Error opening FIFO\n");
    exit(1);
  }

  /* initialize the parameters */
  int numElements = 0;
  fifo_t fifo = {&fd, &numElements, 100};

  TProcessData *processData;
  processData = malloc(sizeof(TProcessData) * NUM_PROCESSES);
  initializeData(processData);

  ThreadParams threadData = {argv[1], &fifo, processData};

  /* create threads */
  if (pthread_create(&tidA, &attr, worker1, &threadData))
  {
    perror("Error creating worker1 thread");
    exit(-1);
  }

  if (pthread_create(&tidB, &attr, worker2, &threadData))
  {
    perror("Error creating worker2 thread");
    exit(-1);
  }

  /* wait for the threads to exit */
  if (pthread_join(tidA, NULL) != 0)
    printf("Issue joining worker1 thread\n");
  if (pthread_join(tidB, NULL) != 0)
    printf("Issue joining worker2 thread\n");

  // Close FIFO
  if (close(fd) == -1)
  {
    perror("Issue closing FIFO\n");
    exit(1);
  }

  // Delete FIFO name and file from filesystem
  if (unlink(FIFO_PATH) == -1)
  {
    perror("Issue deleting FIFO\n");
    exit(1);
  }

  free(processData);

  return 0;
}