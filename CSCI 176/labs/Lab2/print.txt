//////////////////////////////
//// Bryce Remick -- Prog2 - Pthread array accumulation
////
//// compile and run:
//// $>g++ ./main.cpp -pthread
//// $>./a.out 4 
////   //4 is the number of threads to create - can be any
//////////////////////////////

#include <iostream>
#include <cstdlib> //for atoi()
#include <pthread.h> 
#include <stdlib.h>
#include <chrono>
#include <cmath>
typedef std::chrono::high_resolution_clock Clock; // for time calcs
using namespace std;

//globals --accessible to all threads
int thread_count;       //for command line arg
pthread_mutex_t mutex1, mutex2; //to protect c.s. (e.g., cout statement should be atomic)

void *compute(void *input);     //prototype for a Thread (slave) function

// arg struct to pass into Pthread function
struct arg_struct {
    int id;
    int begin;
    int end;
};
const int SIZE = 500000000;  // array size
double global_sum = 0;     // global accumulator
int intArr[SIZE];           // array of ints

///////////////////
int main(int argc, char* argv[]) 
{
    auto t1 = Clock::now();
  long thread_id; //long is needed for type casting to void* type
  thread_count = atoi(argv[1]); //tot number of threads - from command line

  for(int i = 0; i < SIZE; i++)     // populate array
    intArr[i] = i+1;

  pthread_t myThreads[thread_count]; //define threads 
  pthread_mutex_init(&mutex1, NULL); //initialize mutex1
  pthread_mutex_init(&mutex2, NULL); //initialize mutex2

    // define initial array slice for first thread
  int arr_section_size = SIZE / thread_count;
  int arr_section_begin = 0;
  int arr_section_end = arr_section_size;
  
  //creates a certain number of threads
  for(thread_id = 0; thread_id < thread_count; thread_id++) 
  { 
      // create new argument struct
      struct arg_struct *myArgs = (struct arg_struct *)malloc(sizeof(struct arg_struct));
      myArgs->id = thread_id;
      // define array slice for thread to work on
        myArgs->begin = arr_section_begin;
        myArgs->end = arr_section_end;
        // create thread and pass in argument struct
      pthread_create(&myThreads[thread_id], NULL, compute, (void*)myArgs);

      // increment array slice (to pass to next thread)
    arr_section_begin += (arr_section_size);
    arr_section_end += arr_section_size;
  }

  pthread_mutex_lock(&mutex1);   //before using cout statement, reserve mutex
  cout<<"Hello from the main thread"<<endl;
  pthread_mutex_unlock(&mutex1); //release mutex
  
  //wait until all threads finish
  for (thread_id = 0; thread_id < thread_count; thread_id++) 
     pthread_join(myThreads[thread_id], NULL); 

    auto t2 = Clock::now();
    auto t3 = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
    // print out sums
 pthread_mutex_lock(&mutex1);
  cout << "GLOBAL SUM: " << global_sum << endl;
  cout << "EXE TIME: " << t3 * (pow(10, -9)) << "seconds" << endl;
  pthread_mutex_unlock(&mutex1);
  return 0;
}//main

/**
 * PARAMETERS: struct of arguments
 * RETURN: NULL
 * computes the sum of a section of the global array, and then updates global_sum
*/
void *compute(void *arguments) 
{
//   int my_rank = (long)rank; //rank is void* type, so can cast to (long) type only; 
    struct arg_struct *args = (struct arg_struct *)arguments; 

  double sum_of_elems = 0;

  // sum up elements in array slice
  for (int i = args->begin; i < args->end; i++)
    sum_of_elems += intArr[i];
  
  pthread_mutex_lock(&mutex1);   //reserve mutex before cout statement
  cout<<"Hello from thread_"<<args->id<<" of "<<thread_count<<endl; //thread_count is global
  cout << "Array Slice Size: " << args->end - args->begin << endl;
  cout <<"Array Slice Sum: " << sum_of_elems << endl;
   cout << "----------------------------------------" << endl;
  pthread_mutex_unlock(&mutex1); //release mutex

   // update global_sum
  pthread_mutex_lock(&mutex2); 
    global_sum += sum_of_elems;
  pthread_mutex_unlock(&mutex2);
   
  return NULL;
}//Hello
