/*
DanLide, 11.2019

Working with Linux API
The program has the ability to create processes and set their priority
Output is made to the console and isn't synchronized
The calculation of the Taylor series is divided into number of processes
*/

#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/resource.h>
#include <cmath>

using namespace std;
void taylor_series(double interval_start, double interval_end, double increment_step);
unsigned int factorial(unsigned int n);

int main()
{
    int processes_num;
    cout << "Enter number of processes: ";
    cin >> processes_num;

    int priority;
    cout << "Enter process priority (from -20 to 19): ";
    cin >> priority;
    setpriority(PRIO_PROCESS, 0, priority);

    double interval_start, interval_end;
    cout << "Enter A and B: ";
    cin >> interval_start >> interval_end;

    int steps_num;
    cout << "Enter steps num: ";
    cin >> steps_num;

    double* startValues = new double[processes_num];
    double interval = (max(abs(interval_start), abs(interval_end)) - min(abs(interval_start), abs(interval_end))) / processes_num;
    double increment_step = (max(abs(interval_start), abs(interval_end)) - min(abs(interval_start), abs(interval_end))) / steps_num;

    clock_t start = clock();
    double point = interval_start;
    int pid = 1;

    for (int i = 0; i < processes_num; i++)
    {
        if (pid != 0)
            pid = fork();
        startValues[i] = point;
        point += interval;
    }

    int curr_id = 0;
    if (pid == 0)
    {
        cout << "Process #" << getpid() << endl;
        curr_id = getpid() - getppid();
        curr_id -= 1;
        cout << "Current ID: " << curr_id << endl;
        taylor_series(startValues[curr_id], startValues[curr_id] + interval, increment_step);
    }

    for (int i = 0; i < processes_num; i++)
        wait(NULL);

    clock_t end = clock();
    if (pid == 0)
        cout << "Execution time (process " << curr_id << "): " << double(end - start) / double(CLOCKS_PER_SEC) << " seconds" << endl;

}

void taylor_series(double interval_start, double interval_end, double increment_step)
{
    double sum = 1.0;
    float k = 0.5;
    double numerator = 1.0;
    double curr_sum = 0.0;

    cout << "Interval from x = " << interval_start << " to x = " << interval_end << ":" << endl;
    cout << "Increment step: " << increment_step << endl;

    for (double i = interval_start; i <= interval_end; i += increment_step)
    {
        for (int n = 1; n <= 5; n++)
        {
            numerator *= (k - n + 1);
            curr_sum = numerator * pow(i, n) / factorial(n);
            sum += curr_sum;
            n++;
        }
        cout << "Series sum for x = " << i << ": " << sum << endl;

        sum = 1.0;
        numerator = 1.0;
        curr_sum = 0.0;
    }
}

unsigned int factorial(unsigned int n)
{
    if (n == 0)
        return 1;
    return n * factorial(n - 1);
}

