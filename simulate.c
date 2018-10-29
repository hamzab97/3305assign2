/******************************************************************************
*
******************************************************************************/

#include "simulate.h"

int memory, max_memory;
FILE *fp;
pthread_t arrayOfThreads[NUMBER_OF_THREADS]; //create array of pthreads
// pthread_mutex_t mutexLock = PTHREAD_MUTEX_INITIALIZER;

void simulate(int memory_value, linked_stack_t *stack){
  //receive integer that defines the amount of memory the system will manage
  //stack will contain all the jobs you will need to run
  //each elment on the stack contains the job
  //assign struct job to each element popped
  max_memory = memory_value; //max memory the system will manage
  memory = max_memory; //assign the same value to current memory to initialize
  while (stack->size > 0){ //while stack size is greater than 0
    for (int i = 0; i < NUMBER_OF_THREADS; i++){ //create new thread
      pthread_create(&arrayOfThreads[i], NULL, checkStack, stack); //create thread processs at index i
    }

    for (i = 0; i < NUMBER_OF_THREADS; i++){ //wait until thread is completed
      pthread_join(&arrayOfThreads[i], NULL); //create thread processs at index i
    }
  }
  printf("\nEOP\n");//end of program
}

void *checkStack(linked_stack_t *stack){
  printf("\ncheck stack function called\n");
  if (stack->size > 0){
    //check if stack is not empty
    //create job struct from element popped
    job_t *job = pop(stack); //initialize object job
    printf("total memory available: %d\n", max_memory);
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
      dowork(job); //call do work method
    }
  }
}

void dowork(job_t *job){ //void pointer method
  printf("do work method called\n");
  // pthread_mutex_lock(&mutexLock);
  memory = memory - job->required_memory; //allocate memory
  print_starting(fp, job->number); //print starting job %number
  sleep(job->required_time);
  print_completed(fp, job->number);
  memory = memory + job->required_memory; //deallocate memory
  // pthread_mutex_unlock(&mutexLock);
}

/*
allocate memory: whenever new task is spawned, it requires x memory
allocate that memory by reserving that amount for the task while it runs
then reallocate it
*/
