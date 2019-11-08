/*
Working with Windows API
A program has the ability to create threads and to set their priority
A program use run-time dll connection (dynamic)
A program use semaphore objects for synchronization
Finding a minimum value in a random arrays of int is divided into number of threads
*/

#include <windows.h>
#include <iostream>
#include <limits.h>
using namespace std;

typedef int(CALLBACK* LPFNDLLFUNC1)();

int main()
{
	HINSTANCE hDLL;
	LPFNDLLFUNC1 lpfnDllFunc1;
	HRESULT hrReturnVal;

	hDLL = LoadLibrary("C:\\Users\\danil\\source\\repos\\Semaphore\\Debug\\Semaphore.dll");

	if (hDLL != NULL)
	{
		lpfnDllFunc1 = (LPFNDLLFUNC1)GetProcAddress(hDLL, "MainFunc");

		if (NULL != lpfnDllFunc1)
			hrReturnVal = lpfnDllFunc1();
		else
			hrReturnVal = ERROR_DELAY_LOAD_FAILED;
		
		cout << "Returned value: " << hrReturnVal << endl;
		FreeLibrary(hDLL);
	}
}