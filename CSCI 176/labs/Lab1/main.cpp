#include <unistd.h>
#include <cstdlib>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <chrono>
typedef std::chrono::high_resolution_clock Clock;
using namespace std;

int fibo_i(int n);
int fibo_r(int n);

int main(int argc, char *argv[])
{

    int n = atoi(argv[1]);
    int pid1, pid2;         // process ids
    int fd1[2], fd2[2];     // pipe descriptors

    pipe(fd1);              // init pipe1 
    pipe(fd2);              // init pipe2

    pid1 = fork();
    pid2 = fork();

    if (pid1 > 0 && pid2 > 0)       // parent
    {
        int response1, response2;
        write(fd1[1], &n, sizeof(int));     // write n to pipe1 (to child)
        write(fd2[1], &n, sizeof(int));     // write n to pipe2 (to child)
        close(fd1[1]);                      // close write end of pipe 3
        close(fd2[1]);                      // close write end of pipe 2

        wait(NULL); 
        
        read(fd1[0], &response1, sizeof(int));
        read(fd2[0], &response2, sizeof(int));
        cout << "Child1(Recursive) Response Time: " << response1 << " nanoseconds" << endl;
        cout << "Child2(Iterative) Response Time: " << response2 << " nanoseconds" << endl;

        exit(0);
    }
    else if (pid1 == 0 && pid2 > 0) // first child
    {
        int num;
        int fib_results;
        read(fd1[0], &num, sizeof(int));    // read n FROM parent through pipe
        close(fd1[0]);

         auto t1 = Clock::now();

        fib_results = fibo_r(num);          // do recursive fib

        auto t2 = Clock::now();
        auto t3 = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();

        cout << "Child1(Recursive) Results: " << fib_results << endl;
        write(fd1[1], &t3, sizeof(int));    // write time TO parent from pipe
        close(fd1[1]);
        exit(0);
    }
    else if (pid1 > 0 && pid2 == 0) // second child
    {
        int num;
        int fib_results;
        read(fd2[0], &num, sizeof(int));    // read n FROM parent through pipe
        close(fd2[0]);

        auto t1 = Clock::now();
        
        fib_results = fibo_i(num);          // do iterative fib

        auto t2 = Clock::now();

        auto t3 = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();

        cout << "Child2(Iterative) Results: " << fib_results << endl;
        write(fd2[1], &t3, sizeof(int));    // write time TO parent from pipe
        close(fd2[1]);
        exit(0);
    }
    else
    { // third child (grandchild)
        exit(0);
    }

    return 0;
}

int fibo_i(int n) //iterative version
{
    int f1 = 1, f2 = 1, temp;
    int i;

    if (n == 1 || n == 2)
        return 1;
    else
    {
        for (i = 3; i <= n; i++)
        {
            temp = f1 + f2;
            f1 = f2;
            f2 = temp;
        }
        return temp;
    } //else
}
int fibo_r(int n) //recursive version
{
    if (n == 1 || n == 2)
        return 1;
    else
        return (fibo_r(n - 1) + fibo_r(n - 2));
}