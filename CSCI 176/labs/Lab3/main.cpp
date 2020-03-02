#include<iostream>
#include <cstdlib> //for atoi()
#include <pthread.h> 
#include <stdlib.h>
#include <chrono>
#include <cmath>
typedef std::chrono::high_resolution_clock Clock; // for time calcs
using namespace std;

void allocateMat(double **, int , int );
void printMat(double **, int , int );

// GLOBALS
 double **matA, **matB, **matC;
 int L, m, n;
 pthread_mutex_t mutex1;
 int thread_count;

int main(int argc, char const *argv[])
{
    thread_count = atoi(argv[1]); //tot number of threads - from command line
    pthread_t thread_arr[thread_count]; //define threads 
    pthread_mutex_init(&mutex1, NULL); //initialize mutex1

    L = atoi(argv[2]);
    m = atoi(argv[3]);
    n = atoi(argv[4]);

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
            for(int k = 0; k < m; k++)
            matC[i][j] = matA[i][k] * matB[k][j];
        }
    }


    printMat(matA, L, m);
    cout << "----------------------------------------" << endl;
    printMat(matB, m, n);
    cout << "----------------------------------------" << endl;
    printMat(matC, L, n);

    return 0;
}
void printMat(double **mat, int rows, int cols)
{
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
        {
            cout << mat[i][j] << " ";
            if (j == cols - 1)
                cout << endl;
        }
}