/*
DanLide, 11.2019

Working with Linux API
The program has the ability to create POSIX threads
Output is made to the console after all created threads end to work (no sync)
Finding of minimal element in int array is divided into number of threads
*/

#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <limits>

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

int main()
{
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

    for (int i = 0; i < threads_num; i++)
    {
        cout << "Thread " << i << endl;
        cout << "For interval from " << out[i].interval_start << " to " << out[i].interval_end << endl;
        cout << "Array:" << endl;
        for (int j = out[i].interval_start; j < out[i].interval_end; j++)
            cout << "array[" << j << "] = " << out[i].arr[j] << endl;
        cout << "Minimum value = " << out[i].min_el << endl;
        cout << endl;
    }
}

void* findMin(void* args)
{
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

    out[id].arr = arr;
    out[id].interval_start = interval_start;
    out[id].interval_end = interval_end;
    out[id].min_el = min_el;

    sleep(5);
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