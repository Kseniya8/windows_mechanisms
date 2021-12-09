#pragma once

template<typename T>
T GetInput();
int GetInt();
wchar_t* convertCharArrayToLPCWTR(const char* charArray);
unsigned __stdcall ThreadProc(void* param);
void SyncSemaphore();
void SyncEvent();
void SyncWaitableTimer();
void SyncCriticalSection();
DWORD WINAPI ThreadFunction1(LPVOID lpParameter);
DWORD WINAPI ThreadFunction2(LPVOID lpParameter);
DWORD WINAPI ThreadFunction3(LPVOID lpParameter);
class Semaphore;