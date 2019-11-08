#pragma once
#ifdef SEMAPHORE_EXPORTS
#define SEMAPHORE_API __declspec(dllexport)
#else
#define SEMAPHORE_API __declspec(dllimport)
#endif
#include <Windows.h>

extern "C" SEMAPHORE_API int MainFunc();
extern "C" SEMAPHORE_API void CreateThreadsF(int thread_num, unsigned long long interval_start, unsigned long long interval_end, int* arr, DWORD priority);
extern "C" SEMAPHORE_API DWORD WINAPI findMin(LPVOID args);