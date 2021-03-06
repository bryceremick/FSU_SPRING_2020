//////////////////////////////
//// Bryce Remick -- Prog3 - Pthread Matrix Multiplication
////
//// compile and run:
//// $>g++ ./main.cpp -pthread
//// $>./a.out 4 10 10 10
////   //4 is the number of threads to create - can be any
//////////////////////////////

#include<iostream>
#include <cstdlib> //for atoi()
#include <pthread.h> 
#include <stdlib.h>
#include <chrono>
#include <cmath>
typedef std::chrono::high_resolution_clock Clock; // for time calcs
using namespace std;

// GLOBALS
 double **matA, **matB, **matC;     // matrics
 int L, m, n;                       // matrix sizes
 pthread_mutex_t mutex1;            // i/o mutex
 int thread_count;                  // # of threads

 void* slave(void* rank);           // thread function

int main(int argc, char const *argv[])
{
    thread_count = atoi(argv[1]); //tot number of threads - from command line
    pthread_t thread_arr[thread_count]; //define threads 
    pthread_mutex_init(&mutex1, NULL); //initialize mutex1
    long thread_id; 


    // get matrix dimensions from command line
    L = atoi(argv[2]);
    m = atoi(argv[3]);
    n = atoi(argv[4]);

    // start clock
    auto t1 = Clock::now();

    // Allocate Matrix A(L*m)
    matA = new double*[L];
    for (int i = 0; i < L; i++)
    {
         matA[i] = new double[m];
         for (int j = 0; j < m; j++)
            matA[i][j] = i + j + 1;
    }

    // Allocate Matrix B(m*n)
    matB = new double*[m];
    for (int i = 0; i < m; i++)
    {
         matB[i] = new double[n];
         for (int j = 0; j < n; j++)
            matB[i][j] = i + j + 1;
    }

    // Allocate Matrix C(L*n)
    matC = new double*[L];
    for(int i = 0; i < L; i++)
    {
        matC[i] = new double[n];
        for(int j = 0; j < n; j++)
        {
            matC[i][j] = 0;
        }
    }


// create threads (each one calls slave func)
for (thread_id = 0; thread_id < thread_count; thread_id++) 
	pthread_create(&thread_arr[thread_id], NULL, slave, (void*)thread_id);

// join threads
for (thread_id = 0; thread_id < thread_count; thread_id++)
	pthread_join(thread_arr[thread_id], NULL);

// print first 10 of Matrix C
pthread_mutex_lock(&mutex1);
cout << "=== FIRST 10 ===" << endl;
for (int i = 0; i < 10; i++) {
	for (int j = 0; j < 10 ; j++) {
		cout << matC[i][j] <<" ";
	}
	cout << endl;
}
pthread_mutex_unlock(&mutex1);

// print last 10 of Matrix C
pthread_mutex_lock(&mutex1);
cout << "=== LAST 10 ===" << endl;
for (int i = n - 10; i < n; i++) {
	for (int j = 0; j < 10; j++) {
		cout << matC[i][j] << " ";
	}
	cout << endl;
}
pthread_mutex_unlock(&mutex1);

// end clock and calc exe time
auto t2 = Clock::now();
auto t3 = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();

pthread_mutex_lock(&mutex1);
	cout << "TOTAL EXE TIME: " << t3 * (pow(10, -9)) << "seconds" << endl;
pthread_mutex_unlock(&mutex1);

    return 0;
}

// Thread callback function. Each thread calcs a portion of matrix C
void* slave(void* thread_id) {

	int id = (long)thread_id;

	pthread_mutex_lock(&mutex1);
	cout <<"Thread_" << id <<": " <<id <<" ~"<< L<<", step "<< thread_count << endl;
	pthread_mutex_unlock(&mutex1);

    for(int i = id; i < L; i += thread_count)
        for(int j = 0; j < n; j++)
            for(int k = 0; k < m; k++)
                matC[i][j] += matA[i][k] * matB[k][j];

 return NULL;
}