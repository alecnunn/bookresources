
#include <iostream>
#include <process.h>
#include <windows.h>

#include <conio.h>

using namespace std;

#pragma pack(push,4)

__int64 g_total_val = 0;

int g_num_calculations = 0;
bool g_do_floating_point = false;
CRITICAL_SECTION g_val_update;


// Perform some nonsense calculations to burn up CPU cycles
void DoCalculations()
{
	int val = 0;
	if(g_do_floating_point)
	{
		for(int i = 0; i < g_num_calculations; i++)
		{
			for(int j = 0; j < 1000000; j++)
			{
				val += int((float)i * (float)j * 
					((float)j - (float)i - 0.25f) / (val + i + j + 1));
			}
		}
		EnterCriticalSection(&g_val_update);
		g_total_val += val;
		LeaveCriticalSection(&g_val_update);
	}
	else
	{
		for(int i = 0; i < g_num_calculations; i++)
		{
			for(int j = 0; j < 1000000; j++)
			{
				val += i * j * (j - i) / (val + i + j + 1);
			}
		}
		EnterCriticalSection(&g_val_update);
		g_total_val += val;
		LeaveCriticalSection(&g_val_update);
	}
}

// This function is passed to createthread()
void ThreadFunction(LPVOID lpv)
{
	HANDLE hEvent = (HANDLE)lpv;
	DoCalculations();
	SetEvent(hEvent);
}

// Start the threaded timing tests
int main()
{
	char c;
	cout << "Do floating-point calculations (y/n)? ";
	cin >> c;
	if(c == 'y')
		g_do_floating_point = true;

	int threads = 0;
	cout << "How many total threads do you wish" <<
		" to create (including the main thread)? ";
	cin >> threads;

	// Allocate an array of handles if we have 
	// more than one thread
	HANDLE* pHandles = 0;
	if(threads > 1)
	{
		pHandles = new HANDLE[threads - 1];
	}

	cout << "How many millions of calculation loops" << 
		" should each thread perform? ";
	cin >> g_num_calculations;
	cout << "Now performing timing calculations...";

	InitializeCriticalSection(&g_val_update);

	// Get the start time
	unsigned int start_time = timeGetTime();

	// Perform all actual calculations - 
	// one per thread.
	int i;
	if(threads > 1)
	{
		for(i = 0; i < threads - 1; i++)
		{
			pHandles[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
			if(_beginthread(&ThreadFunction, 4096, pHandles[i]) == -1)
				return -1;
		}
		DoCalculations();
		// Wait for all other threads to finish before continuing
		WaitForMultipleObjects(threads - 1, pHandles, TRUE, INFINITE);
	}
	else
	{
		// Do a simple set of calculations for the 
		// single-threaded path
		DoCalculations();
	}

	// Get the end time
	unsigned int end_time = timeGetTime();

	// We no longer need this critical section
	DeleteCriticalSection(&g_val_update);

	// Close and delete handles used for synchronization
	if(threads > 1)
	{
		for(i = 0; i < threads - 1; i++)
		{
			CloseHandle(pHandles[i]);
		}

		delete[] pHandles;
	}

	cout << " Finished!" << endl;
	cout << "Performed all calculations in " << end_time - start_time <<
		" milliseconds" << endl;

	getch();

	return 0;
}

#pragma pack(pop,4)
