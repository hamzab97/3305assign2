/******************************************************************************
*
******************************************************************************/

#include "simulate.h"

int memory, max_memory;
FILE *fp;
linked_stack_t *jobs;
job_t *job; //initialize object job
pthread_t arrayOfThreads[NUMBER_OF_THREADS]; //create array of pthreads


void simulate(int memory_value, linked_stack_t *stack)
{
  //receive integer that defines the amount of memory the system will manage
  //stack will contain all the jobs you will need to run
  //each elment on the stack contains the job
  //assign struct job to each element popped
  max_memory = memory_value; //max memory the system will manage
  memory = max_memory; //assign the same value to current memory to initialize
  while (stack->size > 0){
    //while stack is not empty
    //create job struct from element popped
    job = pop(stack);
    printf("\ntotal memory available: %d\n", max_memory);
    printf("current memory avialable: %d\n", memory);
    printf("job number: %d\n", job->number);
    if ((job->required_memory) > max_memory){
      //call print exceed memory with file pointer and total memory available as paramters
      printf("exceeded memory , job memory: %d\n", job->required_memory);
      print_exceed_memory(fp, job->number);
      printf("\n");
    }
    else if ((job->required_memory) > memory){
      //required job memory is more than the current memory available
      //push bnack on stack
      printf("exceeded current memory. memory needed: %d. memory avail: %d\n", job->required_memory, memory);
      print_insufficient_memory(fp, job->number);
      push(stack, job);
      printf("\n");
    }
    else{
      pthread_create(&thread, NULL, dowork, job, &max_memory, &memory);
      // dowork(job, &max_memory, &memory);
    // printf("required memory for job: %d\n", job->required_memory);
    }
    pthread_join(&thread, NULL);//join the thread
  }
  printf("\nEOP\n");//end of program
}

void dowork(job_t *job, int *totalMemory, int *currentMemory){
  printf("do work method called\n");
  currentMemory = currentMemory - job->required_memory; //allocate memory
  print_starting(fp, job->number); //print starting job %number
  sleep(job->required_time);
  print_completed(fp, job->number);
  currentMemory = currentMemory + job->required_memory; //deallocate memory
}

/*
allocate memory: whenever new task is spawned, it requires x memory
allocate that memory by reserving that amount for the task while it runs
then reallocate it
*/
