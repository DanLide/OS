/*
Working with Windows API
A program has the ability to create threads and to set their priority
A program use semaphore objects for synchronization
A program writes result in a .txt file using file mapping
Finding a minimum value in a random arrays of int is divided into number of threads
*/

#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <iostream>
#include <limits.h>
#include <string>
using namespace std;

struct Params
{
	int interval_start;
	int interval_end;
	int* arr;
	int id;
};

struct FileMapping 
{
	HANDLE hFile;
	HANDLE hMapping;
	size_t fsize;
	unsigned char* dataPtr;
};

void CreateThreadsF(int thread_num, unsigned long long interval_start, unsigned long long interval_end, int* arr, DWORD priority);
DWORD WINAPI findMin(LPVOID args);
HANDLE hSemaphore;
FileMapping* mapping = (FileMapping*)malloc(sizeof(FileMapping));

int main()
{
	int option;
	int thread_num = 0;
	unsigned long long el_num;
	HANDLE* hThreads = NULL;

	while (1)
	{
		cout << "1 - Create n threads" << endl;
		cout << "2 - Exit program\n" << endl;
		cout << "Your choise:" << endl;
		cin >> option;

		switch (option)
		{
		case 1:
		{
			cout << "Enter a number of threads:" << endl;
			cin >> thread_num;
			if (thread_num > 0)
			{
				int button;
				DWORD priority;

				cout << "1 - Set lowest priority (-2)" << endl;
				cout << "2 - Set below normal priority (-1)" << endl;
				cout << "3 - Set normal priority (0)" << endl;
				cout << "4 - Set above normal priority (1)" << endl;
				cout << "5 - Set highest priority (2)" << endl;
				cout << "6 - Set realtime priority (-15)" << endl;
				cout << "Choose priority:" << endl;
				cin >> button;

				switch (button)
				{
				case 1:
				{
					priority = THREAD_PRIORITY_LOWEST;
					break;
				}
				case 2:
				{
					priority = THREAD_PRIORITY_BELOW_NORMAL;
					break;
				}
				case 3:
				{
					priority = THREAD_PRIORITY_NORMAL;
					break;
				}
				case 4:
				{
					priority = THREAD_PRIORITY_ABOVE_NORMAL;
					break;
				}
				case 5:
				{
					priority = THREAD_PRIORITY_HIGHEST;
					break;
				}
				case 6:
				{
					priority = THREAD_PRIORITY_IDLE;
					break;
				}
				default:
				{
					priority = THREAD_PRIORITY_NORMAL;
					break;
				}
				}

				cout << "Enter a number of array elements:" << endl;
				cin >> el_num;

				HANDLE hFile = CreateFile("output.txt", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
				DWORD dwFileSize = 0;
				HANDLE hMapping = CreateFileMapping(hFile, nullptr, PAGE_READWRITE, 0, 1000000, nullptr);
				unsigned char* dataPtr = (unsigned char*)MapViewOfFile(hMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
				mapping->hFile = hFile;
				mapping->hMapping = hMapping;
				mapping->dataPtr = dataPtr;
				mapping->fsize = (size_t)dwFileSize;

				int* arr = new int[el_num];
				for (int i = 0; i < el_num; i++)
					arr[i] = rand() % 1000;

				CreateThreadsF(thread_num, 0, el_num, arr, priority);

				cout << "Done!" << endl;

				UnmapViewOfFile(mapping->dataPtr);
				CloseHandle(mapping->hMapping);
				CloseHandle(mapping->hFile);
				free(mapping);
			}
			else
				cout << "Incorrect input, try again" << endl;
			break;
		}
		case 2:
			exit(0);
		default:
			break;
		}
	}
}

void CreateThreadsF(int thread_num, unsigned long long interval_start, unsigned long long interval_end, int* arr, DWORD priority)
{
	HANDLE* hThreads = new HANDLE[thread_num];
	Params* params = new Params[thread_num];
	int interval = (max(interval_start, interval_end) - min(interval_start, interval_end)) / thread_num;
	int left = (max(interval_start, interval_end) - min(interval_start, interval_end)) % thread_num;
	int button;

	hSemaphore = CreateSemaphore(NULL, 1, thread_num, NULL);

	if (hSemaphore == NULL)
	{
		cout << "CreateSemaphore error: " << GetLastError() << endl;
		exit(42);
	}

	for (int i = 0; i < thread_num; i++)
	{
		params[i].interval_start = int(interval * i);
		params[i].interval_end = int(interval * (i + 1));
		params[i].arr = arr;
		params[i].id = i;
		if (i == thread_num - 1)
			params[i].interval_end += left;

		hThreads[i] = CreateThread(NULL, 0, findMin, (LPVOID)&params[i], 0, 0);

		if (hThreads[i] == NULL)
		{
			cout << "CreateThread error: " << GetLastError() << endl;
			exit(43);
		}

		SuspendThread(hThreads[i]);
		SetThreadPriority(hThreads[i], priority);
		ResumeThread(hThreads[i]);
	}

	WaitForMultipleObjects(thread_num, hThreads, TRUE, INFINITE);

	for (int i = 0; i < thread_num; i++)
		CloseHandle(hThreads[i]);
	CloseHandle(hSemaphore);
}

DWORD WINAPI findMin(LPVOID args)
{
	DWORD dwWaitRes = WaitForSingleObject(hSemaphore, INFINITE);

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

	if (mapping->hFile == NULL)
		return 42;

	string s = "Thread " + to_string(id);
	char const* pchar = s.c_str();
	memcpy(mapping->dataPtr, strcat(_strdup(pchar), "\n"), strlen(pchar) + 1);
	mapping->dataPtr += strlen(pchar) + 1;

	s = "For interval from " + to_string(interval_start) + " to " + to_string(interval_end);
	pchar = s.c_str();
	memcpy(mapping->dataPtr, strcat(_strdup(pchar), "\n"), strlen(pchar) + 1);
	mapping->dataPtr += strlen(pchar) + 1;

	s = "Array:";
	pchar = s.c_str();
	memcpy(mapping->dataPtr, strcat(_strdup(pchar), "\n"), strlen(pchar) + 1);
	mapping->dataPtr += strlen(pchar) + 1;

	for (int j = interval_start; j < interval_end; j++)
	{
		s = "array[" + to_string(j) + "] = " + to_string(arr[j]);
		pchar = s.c_str();
		memcpy(mapping->dataPtr, strcat(_strdup(pchar), "\n"), strlen(pchar) + 1);
		mapping->dataPtr += strlen(pchar) + 1;
	}

	s = "Minimal value = " + to_string(min_el);
	pchar = s.c_str();
	memcpy(mapping->dataPtr, strcat(_strdup(pchar), "\n"), strlen(pchar) + 1);
	mapping->dataPtr += strlen(pchar) + 1;

	if (!ReleaseSemaphore(hSemaphore, 1, NULL))
		cout << "ReleaseSemaphore error: " << GetLastError() << endl;

	Sleep(10000);
	return 0;
}