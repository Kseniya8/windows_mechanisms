#include <Windows.h>
#include <iostream>
#include <stdio.h>
#include <process.h>
#include <mutex>
#include<TCHAR.H>
#include "Func.h"
using namespace std;
mutex mainMutex;

enum class menuItem {
	methodEnter = 0, createProcess, beginTread, mutex, semaphore, event, waitableTimer, CRITICAL_SECTION, quit
};

menuItem GetMenuItem() {
	return static_cast<menuItem>(GetInt());
}

//LONG cMax = 1; // maximum amount of threads
//int alive_threads = 3; // amount of threads 

void DisplayMainMenu() {
	cout << endl << "To start, please select method : " << endl;
	cout << "1 - create Process" << endl;
	cout << "2 - begin Tread with function Wait" << endl;
	cout << "Syncing" << endl;
	cout << "3 - mutex" << endl;
	cout << "4 - semaphore" << endl;
	cout << "5 - event" << endl;
	cout << "6 - waitable timer" << endl;
	cout << "7 - CRITICAL_SECTION" << endl;
	cout << "8 - quit" << endl;
	cout << "Method: ";
}

auto printChar(int n, char c) {
	for (int i = 0; i < n; ++i) {
		std::cout << c;
	}
	cout << endl;
}

auto printCharMut(int n, char c) {
	mainMutex.lock();
	for (int i = 0; i < n; ++i) {
		std::cout << c;
	}
	cout << endl;
	mainMutex.unlock();
}

void Start() {

	menuItem methodEnter = static_cast<menuItem>(0);

	cout << "//Litvinova Ksenia gr.484, LabWork №1 windows" << endl << endl;
	cout << "Hello, Dear User! " << endl;
	cout << "OS Windows" << endl;
	cout << "So let's begin!" << endl;
	int priority = 0;
	string strPriority = "";

	do {
		DisplayMainMenu();
		methodEnter = GetMenuItem();
		switch (methodEnter) {
		case menuItem::createProcess: {
			system("cls");
			wchar_t* wString = convertCharArrayToLPCWTR("Notepad");
			STARTUPINFO si = { sizeof(si) };
			PROCESS_INFORMATION pi;
			cout << "Enter priority: " << endl;
			cout << "1 - IDLE_PRIORITY_CLASS" << endl;
			cout << "2 - NORMAL_PRIORITY_CLASS" << endl;
			cout << "3 - HIGH_PRIORITY_CLASS" << endl;
			cout << "4 - REALTIME_PRIORITY_CLASS" << endl;
			do {
				priority = GetInt();
				if (priority == 1) {
					CreateProcess(NULL, wString, NULL, NULL, FALSE, IDLE_PRIORITY_CLASS, NULL, NULL, &si, &pi);
					strPriority = "IDLE_PRIORITY_CLASS";
				}
				else if (priority == 2) {
					CreateProcess(NULL, wString, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);
					strPriority = "NORMAL_PRIORITY_CLASS";
				}
				else if (priority == 3) {
					CreateProcess(NULL, wString, NULL, NULL, FALSE, HIGH_PRIORITY_CLASS, NULL, NULL, &si, &pi);
					strPriority = "HIGH_PRIORITY_CLASS";
				}
				else if (priority == 4) {
					CreateProcess(NULL, wString, NULL, NULL, FALSE, REALTIME_PRIORITY_CLASS, NULL, NULL, &si, &pi);
					strPriority = "REALTIME_PRIORITY_CLASS";
				}
				else
					cout << "Input error" << endl;
			} 			
			while (strPriority == "");
			cout << "The process is started with priority" << strPriority << endl;
			Sleep(700);
			break;
		}
		case menuItem::beginTread: {
			system("cls");
			HANDLE hThread;
			unsigned dwThread; /* создаем новый поток */
			cout << "Creating new thread" << endl;
			hThread = (HANDLE)_beginthreadex(
				NULL, 0, ThreadProc, new int[128], 0, &dwThread
			);
			WaitForSingleObject(hThread, INFINITE); // ожидание завершения потока
			cout << "Thread is done" << endl;
			CloseHandle(hThread);
			break;
		}
		case menuItem::mutex : {
			system("cls");
			cout << "Working mode without mutex" << endl;
			auto a = std::thread{ printChar, 10, '+' };
			auto b = std::thread{ printChar, 10, '-' };
			a.join();
			b.join();
			cout << "Working mode with mutex" << endl;
			a = std::thread{ printCharMut, 10, '+' };
			b = std::thread{ printCharMut, 10, '-' };
			a.join();
			b.join();
			break;
		}
		case menuItem::semaphore: {
			system("cls");
			SyncSemaphore();
			break;
		}
		case menuItem::event: {
			system("cls");
			SyncEvent();
			break;
		}
		case menuItem::waitableTimer: {
			system("cls");
			SyncWaitableTimer();
			break;
		}
		case menuItem::CRITICAL_SECTION: {
			system("cls");
			SyncCriticalSection();
			break;
		}
		case menuItem::quit:
			cout << "Goodbye!" << endl;
			break;

		default:
			system("cls");
			cout << "Wrong number! Try again!" << endl;
			break;
		}
	} while (methodEnter != menuItem::quit);
}
