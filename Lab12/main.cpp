#include <iostream>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <sstream>


#include <sys/mman.h>
#include <sys/types.h>
#include <limits.h>
#include <semaphore.h>

using namespace std;

struct Params
{
    int interval_start;
    int interval_end;
    int* arr;
    int id;
};

void * findMin(void* args);
void create_threads(int threads_num, int interval_start, int interval_end, int* arr);
sem_t mutex;
char *map;
ostringstream result;

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
    cout << endl;

    // Array generating
    int* arr = new int[el_num];
    for (int i = 0; i < el_num; i++)
        arr[i] = rand() % 1000;


    int fp = open("result.txt", O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);
    if (fp == -1)
    {
        perror("Error: File Couldn't Be Opened");
        exit(0);
    }

    int FILE_SIZE = 10000;

    int res = lseek(fp, FILE_SIZE-1, SEEK_SET);

    if (res == -1)
    {
        close(fp);
        perror("Error: lseek() Couldn't Be Called To Expand The File");
        exit(0);
    }

    res = write(fp, "", 1);

    if (res != 1)
    {
        close(fp);
        perror("Error: Last Byte Of The File Wasn't Written");
        exit(0);
    }

    map = (char*)mmap(0, FILE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fp, 0);

    if (map == MAP_FAILED)
    {
        close(fp);
        perror("Error: File Wasn't Mapped");
        exit(0);
    }

    create_threads(threads_num, 0, el_num, arr);

    string tmp_str = result.str();
    char tmp[tmp_str.length()];
    strcpy(tmp, tmp_str.c_str());
    strcat(map, tmp);

    if (munmap(map, FILE_SIZE) == -1)
        perror("Error: File Wasn't Unmapped");

    close(fp);
    cout << "Done!" << endl;

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
    int min_el = INT_MAX;

    for (int i = interval_start; i < interval_end; i++)
    {
        if (arr[i] < min_el)
            min_el = arr[i];
    }

    result << "Thread " << id << endl;
    result << "For interval from " << interval_start << " to " << interval_end << endl;
    result << "Array:" << endl;
    for (int j = interval_start; j < interval_end; j++)
        result << "array[" << j << "] = " << arr[j] << endl;
    result << "Minimum value = " << min_el << endl;
    result << endl;

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
    int counter = 0;

    for (int i = 0; i < threads_num; i++)
    {
        params[i].interval_start = int(interval * i) + counter;
        params[i].arr = arr;
        params[i].id = i;

        if (left > 0)
        {
            counter++;
            left--;
        }

        params[i].interval_end = int(interval * (i + 1)) + counter;

        pthread_attr_init(&attr[i]);
        pthread_create(&tid[i], &attr[i], findMin, (void*)&params[i]);
    }

    for (int i = 0; i < threads_num; i++)
        pthread_join(tid[i], nullptr);
}
