#pragma once

#include <Windows.h>
#include <iostream>

#define BUFSIZE		1024

BOOL CALLBACK EnumWindowsProc(_In_ HWND hwnd, _In_ LPARAM lParam);

typedef class INJECTOR {
private:
	TCHAR processTitle[BUFSIZE] = { 0, };
	DWORD pid = NULL;
	HANDLE processHandle = NULL, remoteThreadHandle = NULL;
	void* allocMemoryAddr = NULL;
	TCHAR dllPath[MAX_PATH] = { 0, };
	LPTHREAD_START_ROUTINE remoteThreadFunc = NULL;

	void ErrorMessage(const char* msg) {
		MessageBoxA(NULL, msg, "ERROR", NULL);
		exit(1);
	};

public:
	INJECTOR(TCHAR* title) {
		if (title == NULL) ErrorMessage("Title is wrong");
		wcscpy_s(processTitle, title);

		if (EnumWindows(EnumWindowsProc, (LPARAM)this)) printf_s("[INJECTOR] : Find Process \n");
		if (pid == NULL) ErrorMessage("PID is not exist");
		processHandle = OpenProcess(PROCESS_ALL_ACCESS, TRUE, pid);
		if (processHandle == NULL) {
			printf("Process Open Failed\n");
		};

	};

	~INJECTOR() {
		CloseHandle(this->processHandle);
		CloseHandle(this->remoteThreadHandle);
		TerminateThread(this->remoteThreadHandle, 0);
	};

	BOOL Injection() {
		allocMemoryAddr = VirtualAllocEx(processHandle, NULL, BUFSIZE, MEM_COMMIT, PAGE_READWRITE);
		if (allocMemoryAddr == NULL) ErrorMessage("Mem Alloc Failed");
		if (!WriteProcessMemory(this->processHandle, this->allocMemoryAddr, this->dllPath, BUFSIZE, NULL)) ErrorMessage("Mem Write Failed");
		auto modHandle = GetModuleHandle(L"kernel32.dll");
		remoteThreadFunc = (LPTHREAD_START_ROUTINE)GetProcAddress(modHandle, "LoadLibraryW");
		if (remoteThreadFunc == NULL) ErrorMessage("Function load Failed");

		remoteThreadHandle = CreateRemoteThread(this->processHandle, NULL, 0, this->remoteThreadFunc, this->allocMemoryAddr, NULL, NULL);
 		if (remoteThreadHandle == NULL) ErrorMessage("Create Remote Thread Failed");

		WaitForSingleObject(this->remoteThreadHandle, INFINITY);
		return TRUE;
	};

	BOOL SetDLLPath(TCHAR* path) {
		memset(this->dllPath, 0, MAX_PATH);
		wcscpy_s(this->dllPath, path);
		return true;
	};
	TCHAR* GetDLLPath() { return this->dllPath; };

	void SetPID(DWORD PID) { this->pid = PID; };
	DWORD GetPID() { return this->pid; };

	TCHAR* GetTitle() { return this->processTitle; };

}injector;

BOOL CALLBACK EnumWindowsProc(_In_ HWND hwnd, _In_ LPARAM lParam) {
	TCHAR currentTitle[BUFSIZE] = { 0, };
	TCHAR title[BUFSIZE] = { 0, };

	INJECTOR* injector = (INJECTOR*)lParam;

	DWORD pid = NULL;


	wcscpy_s(title, injector->GetTitle());

	while (1) {
		GetWindowText(hwnd, currentTitle, BUFSIZE);
		//wprintf(L"%s", currentTitle, BUFSIZE);
		auto result = wcscmp(currentTitle, title);
		if (result == 0) {
			GetWindowThreadProcessId(hwnd, &pid);
			if (pid == NULL) return TRUE;

			injector->SetPID(pid);
			return FALSE;
		}
		hwnd = GetNextWindow(hwnd, GW_HWNDNEXT);
	}
	return TRUE;
};