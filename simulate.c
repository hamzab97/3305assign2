/******************************************************************************
*
Name: Hamza Bana
Email: hbana3@uwo.ca
simulate.c contains all the methods for the simlate file and job processing
******************************************************************************/

#include "simulate.h"

int memory, max_memory;
FILE *fp;
pthread_t arrayOfThreads[NUMBER_OF_THREADS]; //create array of pthreads
// pthread_mutex_t mutexLock = PTHREAD_MUTEX_INITIALIZER;

/*
void simulate accepts a memory value and a stack containing all the jobs as parameters
the method initializes the values for file opening, total memory and current memory
then a while loop is started which keeps track of the stack size and executes until the stack is empty (all the jobs have been processed)
inside the while loop, a thread is created for each of the jobs and executes
*/
void simulate(int memory_value, linked_stack_t *stack){
  //receive integer that defines the amount of memory the system will manage
  //stack will contain all the jobs you will need to run
  //each elment on the stack contains the job
  //assign struct job to each element popped
  fp = fopen(SYSTEM_OUTPUT, "w"); //popen system file
  max_memory = memory_value; //max memory the system will manage
  memory = max_memory; //assign the same value to current memory to initialize
  while (stack->size > 0){ //while stack size is greater than 0
    int i = 0;
    while (i < NUMBER_OF_THREADS && stack->size > 0){
      pthread_create(&arrayOfThreads[i], NULL, checkStack, (void*)stack);
      i++;
    }
    for (int j = 0; j < i; j++){ //create new thread
      pthread_join(arrayOfThreads[j], NULL); //create thread processs at index i
    }
  }
  printf("\n\n");//end of program
}

/*
check stack accepts void pointer to stack as parameter
method is called by each thread and operates on the stack to obtain and check the job
method used to make sure suffiient memory is present to perform the operation
then calls a do work method to actually execute the job
*/

void *checkStack(void* stack){
  if (((linked_stack_t*)stack)->size > 0){
    //check if stack is not empty
    //create job struct from element popped
    job_t *job = pop(((linked_stack_t*)stack));; //initialize object job
    if ((job->required_memory) > max_memory){
      //call print exceed memory with file pointer and total memory available as paramters
      print_exceed_memory(fp, job->number);
    }
    else if ((job->required_memory) > memory){
      //required job memory is more than the current memory available
      //push bnack on stack
      print_insufficient_memory(fp, job->number);
      push(stack, job);
    }
    else{
      dowork(job); //call do work method
    }
  }
}

/*
method called for jobs that have enough system requirements to get executed
executes the job and prints appropriate statements 
*/

void dowork(job_t *job){ //void  method
  memory = memory - job->required_memory; //allocate memory
  print_allocate_memory(fp, memory, job->required_memory); //print that memory is allocated
  print_starting(fp, job->number); //print starting job %number
  sleep(job->required_time);
  print_completed(fp, job->number);
  memory = memory + job->required_memory; //deallocate memory
  print_deallocate_memory(fp, memory, job->required_memory); //print once memory deallocated
}

/*
allocate memory: whenever new task is spawned, it requires x memory
allocate that memory by reserving that amount for the task while it runs
then reallocate it
*/
