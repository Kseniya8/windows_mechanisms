#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <Windows.h>
#include <mutex>
#include <semaphore>
#include <process.h>
#include<conio.h>
#include "Sema.h"

using namespace std;

Semaphore* s;
LONG cMax = 1; // maximum amount of threads
int alive_threads = 3; // amount of threads 

template<typename T>

T GetInput() {
	T userInput;
	cin >> userInput;
	if (cin.fail()) {
		cout << "Oops incorrect data entry, try again." << endl << endl;
		while (cin.fail()) {
			cin.clear();
			cin.ignore(INT_MAX, '\n');
			cin >> userInput;
		}
	}
	cin.ignore(INT_MAX, '\n');
	return userInput;
}

int GetInt() {
	return GetInput<int>();
}

DWORD WINAPI ThreadFunction1(LPVOID lpParameter) {
	s->WaitForSemaphore(INFINITE); // check whether we can start new thread
	cout << "Semaphore occupied by thread 1.Starting thread:\n"; // starting
	int num = 0;
	for (int i = 0; i < 5; i++) {
		cout << num << "\n"; // making calculations
		num++;
		Sleep(100);
	}
	cout << "Thread 1 completed!\n";
	s->LeaveSemaphore(); // leaving semaphore
	cout << "Semaphore was realesed \n";
	alive_threads--; // 1 thread is dead, reduce the amount of alive threads
	return(0);
}

DWORD WINAPI ThreadFunction2(LPVOID lpParameter) {
	s->WaitForSemaphore(INFINITE);
	cout << "Semaphore occupied by thread 2.Starting thread:\n";
	int num = 100;
	for (int i = 0; i < 5; i++) {
		cout << num << "\n";
		num++;
		Sleep(100);
	}
	cout << "Thread 2 completed!\n";
	s->LeaveSemaphore();
	cout << "Semaphore was realesed\n";
	alive_threads--;
	return(0);

}

DWORD WINAPI ThreadFunction3(LPVOID lpParameter) {
	s->WaitForSemaphore(INFINITE);
	cout << "Semaphore occupied by thread 3.Starting thread:\n";
	int num = 500;
	for (int i = 0; i < 5; i++) {
		cout << num << "\n";
		num++;
		Sleep(100);
	}
	cout << "Thread 3 completed!\n";
	s->LeaveSemaphore();
	cout << "Semaphore was realesed \n";
	alive_threads--;
	return(0);
}

wchar_t* convertCharArrayToLPCWTR(const char* charArray) {
	wchar_t* wString = new wchar_t[4096];
	MultiByteToWideChar(CP_ACP, 0, charArray, -1, wString, 4096);
	return wString;
}

unsigned __stdcall ThreadProc(void* param) // __stdcall используется для вызова win32 api
{
	/* вновь созданный поток будет выполнять эту функцию */
	cout << "Thread is working" << endl;
	Sleep(1000);
	delete[](int*)param;
	return 0;
	/* завершение функции = завершение потока */
}

void SyncSemaphore() {
	s = new Semaphore(cMax); // creating semaphore
	CreateThread(NULL, 0, ThreadFunction1, NULL, 0, NULL);
	CreateThread(NULL, 0, ThreadFunction2, NULL, 0, NULL);
	CreateThread(NULL, 0, ThreadFunction3, NULL, 0, NULL);
	while (alive_threads != 0) {} // check whether all threads died
	s ->~Semaphore(); // destroying semaphore
	alive_threads = 3;
}

HANDLE g_hEventInitComplete = NULL;
unsigned __stdcall TreadProc_event(void* pArguments) {
	cout << (const char*)pArguments << " is waiting for execution \n";
	WaitForSingleObject(g_hEventInitComplete, INFINITE);
	cout << (const char*)pArguments << " is working \n";
	Sleep(1000);
	cout << (const char*)pArguments << " is complete \n";
	_endthreadex(0);
	return 0;
}

void SyncEvent() {
	cout << "Using Event mechanism." << endl;
	g_hEventInitComplete = CreateEvent(NULL, FALSE, FALSE, L"First");
	HANDLE workThread_event = (HANDLE)_beginthreadex(NULL, 0, TreadProc_event, (void*)"Thread", 0, NULL);
	Sleep(1000);
	if (g_hEventInitComplete) {
		SetEvent(g_hEventInitComplete);
		cout << "Got singnal from event \n";
		ResetEvent(g_hEventInitComplete);
		CloseHandle(g_hEventInitComplete);
	}
	if (workThread_event) {
		WaitForSingleObject(workThread_event, INFINITE);
		CloseHandle(workThread_event);
	}
}

void SyncWaitableTimer() {
	const int nTimerUnitsPerSecond = 10000000;
	int time = 5; //время ожидания таймера
	int i = 0;
	LARGE_INTEGER li;
	li.QuadPart = -(time * nTimerUnitsPerSecond); //5 секунд (куски по 100нс, отрицательное)
	HANDLE hTimer15 = CreateWaitableTimer(NULL, FALSE, NULL);

	 if (SetWaitableTimer(hTimer15, &li, 1000, NULL, NULL, FALSE))//запускаем таймер
	 {
		printf(("Notepad will start in 5 seconds. Waiting 5 seconds...\n"));
		WaitForSingleObject(hTimer15, INFINITE);//ждем 5 секунд
		printf("Start #%d. \n", i + 1);
		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_SHOW;
		WCHAR str[] = L"notepad";
		CreateProcessW(NULL, str, NULL, NULL, FALSE, CREATE_DEFAULT_ERROR_MODE, NULL, NULL, &si, &pi);//запускаем блокнот
		CancelWaitableTimer(hTimer15);//останавливаем таймер
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		i++;
	 }
}

CRITICAL_SECTION crit_sect;
unsigned __stdcall CRTITICAL_SECTION_func(void* pArguments) {
	cout << (const char*)pArguments << " is waiting for execution \n";
	EnterCriticalSection(&crit_sect);
	cout << (const char*)pArguments << " has occupied critical section. Work start\n";
	Sleep(1000);
	LeaveCriticalSection(&crit_sect);
	cout << (const char*)pArguments << " is complete. Leaving critical section \n";
	_endthreadex(0);
	return 0;
}

void SyncCriticalSection() {
	cout << "Using CriticalSection mechanism." << endl;
	InitializeCriticalSection(&crit_sect);
	HANDLE workThread_cs = (HANDLE)_beginthreadex(NULL, 0, CRTITICAL_SECTION_func, (void*)"Thread 1", 0, NULL);
	HANDLE workThread_1_cs = (HANDLE)_beginthreadex(NULL, 0, CRTITICAL_SECTION_func, (void*)"Thread 2", 0, NULL);
	if (workThread_cs) {
		WaitForSingleObject(workThread_cs, INFINITE);
		CloseHandle(workThread_cs);
	}
	if (workThread_1_cs) {
		WaitForSingleObject(workThread_1_cs, INFINITE);
		CloseHandle(workThread_1_cs);
	}
	DeleteCriticalSection(&crit_sect);
}

