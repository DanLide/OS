/*
DanLide, 11.2019

Working with Linux API
The program has the ability to create POSIX threads
Program use semaphore objects for synchronization
Finding of minimal element in int array is divided into number of threads
*/

#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <limits>
#include <semaphore.h>

using namespace std;

struct Params
{
    int interval_start;
    int interval_end;
    int* arr;
    int id;
};
struct Output
{
    int interval_start;
    int interval_end;
    int min_el;
    int* arr;
};

void * findMin(void* args);
void create_threads(int threads_num, int interval_start, int interval_end, int* arr);
Output* out = new Output[100];
sem_t mutex;

int main()
{
    // Creating semaphore
    sem_init(&mutex, 0, 1);

    int threads_num;
    cout << "Enter number of threads: ";
    cin >> threads_num;

    int el_num;
    cout << "Enter a number of array elements: ";
    cin >> el_num;

    // Array generating
    int* arr = new int[el_num];
    for (int i = 0; i < el_num; i++)
        arr[i] = rand() % 1000;

    create_threads(threads_num, 0, el_num, arr);

    // Destroying semaphore
    sem_destroy(&mutex);
}

void* findMin(void* args)
{
    //wait
    sem_wait(&mutex);

    Params* args1 = (Params*)args;
    int interval_start = args1->interval_start;
    int interval_end = args1->interval_end;
    int* arr = args1->arr;
    int id = args1->id;
    int min_el = INT8_MAX;

    for (int i = interval_start; i < interval_end; i++)
    {
        if (arr[i] < min_el)
            min_el = arr[i];
    }

    cout << "Thread " << id << endl;
    cout << "For interval from " << interval_start << " to " << interval_end << endl;
    cout << "Array:" << endl;
    for (int j = interval_start; j < interval_end; j++)
        cout << "array[" << j << "] = " << arr[j] << endl;
    cout << "Minimum value = " << min_el << endl;
    cout << endl;

    sleep(2);

    //signal
    sem_post(&mutex);

    pthread_exit(0);
}

void create_threads(int threads_num, int interval_start, int interval_end, int* arr)
{
    pthread_t* tid = new pthread_t[threads_num];
    pthread_attr_t* attr = new pthread_attr_t[threads_num];
    Params* params = new Params[threads_num];
    int interval = (max(interval_start, interval_end) - min(interval_start, interval_end)) / threads_num;
    int left = (max(interval_start, interval_end) - min(interval_start, interval_end)) % threads_num;

    for (int i = 0; i < threads_num; i++)
    {
        params[i].interval_start = int(interval * i);
        params[i].interval_end = int(interval * (i + 1));
        params[i].arr = arr;
        params[i].id = i;
        if (i == threads_num - 1)
            params[i].interval_end += left;

        pthread_attr_init(&attr[i]);
        pthread_create(&tid[i], &attr[i], findMin, (void*)&params[i]);
    }

    for (int i = 0; i < threads_num; i++)
        pthread_join(tid[i], nullptr);
}