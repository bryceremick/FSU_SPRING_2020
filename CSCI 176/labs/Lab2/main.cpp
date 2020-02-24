//////////////////////////////
//// Park -- this is a C++ version of the Pthread Hello program --correct version
////
//// compile and run:
//// $> g++ -o Hello-correct Hello-correct.cpp -lpthread
//// $>./Hello-correct 4
////   //4 is the number of threads to create - can be any
//////////////////////////////

#include <iostream>
#include <cstdlib> //for atoi()
#include <pthread.h> 
#include <vector>
using namespace std;

//globals --accessible to all threads
int thread_count;       //for command line arg
pthread_mutex_t mutex1, mutex2; //to protect c.s. (e.g., cout statement should be atomic)

struct arg_struct {
    int id;
    int begin;
    int end;
}args;

// void *compute(void* arg_struct); //prototype for a Thread (slave) function

void *compute(void *arg_struct);

template<typename T>
vector<T> slice(std::vector<T> const &, int, int);

const int SIZE = 1000;
int global_sum = 0;
vector<int> intArr;

///////////////////
int main(int argc, char* argv[]) 
{
  int thread_id; 
  thread_count = atoi(argv[1]);

  intArr.reserve(SIZE);
  for(int i = 0; i < SIZE; i++)
    intArr.push_back(i+1);

  pthread_t myThreads[thread_count]; //define threads 
  pthread_mutex_init(&mutex1, NULL); //initialize mutex1
  pthread_mutex_init(&mutex2, NULL); //initialize mutex2
  
  int arr_section_size = SIZE / thread_count;
  int arr_section_begin = 0;
  int arr_section_end = arr_section_size - 1;

  //creates a certain number of threads
  for(thread_id = 0; thread_id < thread_count; thread_id++)
  {
    // define thread arguments (thread id, array slice)
    args.id = thread_id;
    args.begin = arr_section_begin;
    args.end = arr_section_end;

    // create thread to compute sum of specified slice of array
    pthread_create(&myThreads[thread_id], NULL, &compute, (void*)&args); 

    // increment vector slice (to pass to next thread)
    arr_section_begin += arr_section_size;
    arr_section_end += arr_section_size - 1;
  }

  pthread_mutex_lock(&mutex1);   //before using cout statement, reserve mutex
  cout<<"Hello from the main thread"<<endl;
  pthread_mutex_unlock(&mutex1); //release mutex
  
  //wait until all threads finish
  for (thread_id = 0; thread_id < thread_count; thread_id++) 
     pthread_join(myThreads[thread_id], NULL); 

  pthread_mutex_lock(&mutex1);
  cout << "GLOBAL SUM: " << global_sum << endl;
  pthread_mutex_unlock(&mutex1);

  return 0;
}//main

void *compute(void *arguments) 
{
  struct arg_struct *args = (struct arg_struct *)arguments; // get arguments
  int my_rank = (long)args->id; // thread id
  vector<int> thread_arr_slice = slice(intArr, args->begin, args->end); // get array slice

  // sum up elements in array slice
  int sum_of_elems = 0;
  for (auto& n : thread_arr_slice)
    sum_of_elems += n;
  
  pthread_mutex_lock(&mutex1);   //reserve muterx before cout statement
  cout<<"Hello from thread_"<<my_rank<<" of "<<thread_count<<endl; 
  cout << sum_of_elems << endl;
  pthread_mutex_unlock(&mutex1); //release mutex

  pthread_mutex_lock(&mutex2); 
    global_sum += sum_of_elems;
  pthread_mutex_unlock(&mutex2);
   
  return NULL;
}//Hello

template<typename T>
std::vector<T> slice(std::vector<T> const &v, int m, int n)
{
    auto first = v.cbegin() + m;
    auto last = v.cbegin() + n + 1;

    std::vector<T> vec(first, last);
    return vec;
}