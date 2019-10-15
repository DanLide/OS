/*
Working with Windows API
The program has the ability to create, suspend, resume and terminate processes
The calculation of the Taylor series is divided into number of processes
*/

#include <windows.h>
#include <iostream>
#include <string>
using namespace std;

struct ProcessData
{
	STARTUPINFO* sinfo;
	PROCESS_INFORMATION* pinfo;
};

bool dead_processes[8];
ProcessData* createProcessFunc(int processes_num, double interval_start, double interval_end, int steps_num);
BOOL changeProcessPriority(ProcessData process);

int main()
{
	ProcessData* processes = NULL;
	int n = 0;
	double interval_start = -15, interval_end = -5;
	int processes_num = 0, option = 0, process_num = 0;
	DWORD start = NULL;
	int steps_num;

	while (1)
	{
		cout << "1 - Create processes" << endl;
		cout << "2 - Suspend process" << endl;
		cout << "3 - Resume process" << endl;
		cout << "4 - Terminate process" << endl;
		cout << "5 - Change priority" << endl;
		cout << "6 - Show execution time" << endl;

		cout << "Choose an option: ";
		cin >> option;
		switch (option)
		{
		case 1:
		{
			cout << "Enter a number of processes you want to create: ";
			cin >> processes_num;
			if (processes_num > 0)
			{
				start = GetTickCount();
				cout << "Enter a number of steps while function tabulation: ";
				cin >> steps_num;
				processes = createProcessFunc(processes_num, interval_start, interval_end, steps_num);
			}
			else
				cout << "Incorrect input, try again" << endl;
			break;
		}
		case 2:
		{
			cout << "Enter a number of process you want to stop: ";
			cin >> process_num;

			if (process_num > 0 && process_num <= processes_num && !dead_processes[process_num - 1])
			{
				if (processes[process_num - 1].pinfo == NULL)
					cout << "NULL";
				if (SuspendThread(processes[process_num - 1].pinfo->hThread) == 0)
					cout << "Done!" << endl;
				else
				{
					cout << "Error while thread suspending" << endl;
					cout << GetLastError() << endl;
				}

			}
			else
				cout << "Incorrect input or process is dead" << endl;
			break;
		}
		case 3:
		{
			cout << "Enter a number of process you want to resume: ";
			cin >> process_num;

			if (process_num > 0 && process_num <= processes_num && !dead_processes[process_num - 1])
			{

				if (ResumeThread(processes[process_num - 1].pinfo->hThread))
					cout << "Done!" << endl;
				else
					cout << "Error while thread resumption" << endl;
			}
			else
				cout << "Incorrect input or process is dead" << endl;
			break;
		}
		case 4:
		{
			cout << "Enter a number of process you want to terminate: ";
			cin >> process_num;

			if (process_num > 0 && process_num <= processes_num && !dead_processes[process_num - 1])
			{
				if (TerminateProcess(processes[process_num - 1].pinfo->hProcess, 0))
				{
					cout << "Done!" << endl;
					dead_processes[process_num - 1] = true;
				}
				else
					cout << "Error while process temination" << endl;
			}
			else
				cout << "Incorrect input or process is dead" << endl;
			break;
		}
		case 5:
		{
			cout << "Enter a number of process to change it's priority: ";
			cin >> process_num;

			if (process_num > 0 && process_num <= processes_num && !dead_processes[process_num - 1])
			{
				if (changeProcessPriority(processes[process_num - 1]))
					cout << "Done!" << endl;
				else
					cout << "Error while changing process priority" << endl;
			}
			else
				cout << "Incorrect input or process is dead" << endl;
			break;
		}
		case 6:
		{
			for (int i = 0; i < processes_num; i++)
			{
				if (dead_processes[i])
					continue;
				WaitForSingleObject(processes[i].pinfo->hProcess, INFINITE);
				DWORD finish = GetTickCount();
				cout << "PID: " <<  processes[i].pinfo->dwProcessId << " - Time: " << (finish - start) / 1000 << "seconds" << endl;
			}
			break;
		}
		default:
		{
			cout << "Incorrect input, try again" << endl;
			break;
		}
		}
	}
}

ProcessData* createProcessFunc(int processes_num, double interval_start, double interval_end, int steps_num)
{
	ProcessData* processes = new ProcessData[processes_num];
	STARTUPINFO* sinfo = new STARTUPINFO[processes_num];
	PROCESS_INFORMATION* pinfo = new PROCESS_INFORMATION[processes_num];
	double interval = (max(abs(interval_start), abs(interval_end)) - min(abs(interval_start), abs(interval_end))) / processes_num;
	double increment_step = (max(abs(interval_start), abs(interval_end)) - min(abs(interval_start), abs(interval_end))) / steps_num;

	for (int i = 0; i < processes_num; i++)
	{
		string str = "C:\\DanLide\\c,cpp\\OS\\TaylorSeries\\Debug\\TaylorSeries.exe " + to_string(interval_start + i * interval) + " " +
			to_string(interval_start + (i + 1) * interval) + " " + to_string(increment_step);
		LPSTR cmdLine = const_cast<char*>(str.c_str());
		ZeroMemory(&sinfo[i], sizeof(sinfo[i]));
		sinfo[i].cb = sizeof(sinfo[i]);
		ZeroMemory(&pinfo[i], sizeof(pinfo));

		if (!CreateProcessA(NULL, cmdLine, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &sinfo[i], &pinfo[i]))
		{
			cout << "Create process error: " << GetLastError() << endl;
			exit(1);
		}

		processes[i].pinfo = &pinfo[i];
		processes[i].sinfo = &sinfo[i];
	}

	return processes;
}

BOOL changeProcessPriority(ProcessData process)
{
	cout << "Select an option:" << endl;
	cout << "1 - Set a low priority" << endl;
	cout << "2 - Set a below normal priority" << endl;
	cout << "3 - Set a normal priority" << endl;
	cout << "4 - Set an above normal priority" << endl;
	cout << "5 - Set a high priority" << endl;
	cout << "6 - Set a realtime priority" << endl;

	int priority, option;

	cout << "Choose an option: ";
	cin >> option;

	switch (option)
	{
	case 1:
	{
		priority = IDLE_PRIORITY_CLASS;
		break;
	}
	case 2:
	{
		priority = BELOW_NORMAL_PRIORITY_CLASS;
		break;
	}
	case 3:
	{
		priority = NORMAL_PRIORITY_CLASS;
		break;
	}
	case 4:
	{
		priority = ABOVE_NORMAL_PRIORITY_CLASS;
		break;
	}
	case 5:
	{
		priority = HIGH_PRIORITY_CLASS;
		break;
	}
	case 6:
	{
		priority = REALTIME_PRIORITY_CLASS;
		break;
	}
	default:
	{
		priority = NORMAL_PRIORITY_CLASS;
	}
	}

	return SetPriorityClass(process.pinfo->hProcess, priority);
}
