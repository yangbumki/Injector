#pragma once

#include <Windows.h>
#include <iostream>
#include <TlHelp32.h>
#include "Privilege.hpp"

#define BUFSIZE		1024

#define PROCESS_TITLE	1
#define EXEC_NAME		2
#define PROCESS_ID		3
#define GLOBAL			4

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

	void AutoHandling() {
		processHandle = OpenProcess(PROCESS_ALL_ACCESS, TRUE, pid);
	};
public:
	INJECTOR() {

	};

	INJECTOR(TCHAR* title, int type = PROCESS_TITLE) {
		if (title == NULL) ErrorMessage("Title is wrong");
		wcscpy_s(processTitle, title);

		if (type == PROCESS_TITLE) {
			if (EnumWindows(EnumWindowsProc, (LPARAM)this)) printf_s("[INJECTOR] : Find Process \n");
			if (pid == NULL) ErrorMessage("PID is not exist");
			processHandle = OpenProcess(PROCESS_ALL_ACCESS, TRUE, pid);
			if (processHandle == NULL) {
				printf("Process Open Failed\n");
			};
		}
		else if (type == EXEC_NAME) {
			auto snap = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
			PROCESSENTRY32 pe32;
			
			memset(&pe32, 0, sizeof(PROCESSENTRY32));
			pe32.dwSize = sizeof(PROCESSENTRY32);
			
			while (TRUE) {
				if (pe32.th32ProcessID == NULL) 
					if (!Process32First(snap, &pe32)) ErrorMessage("Process32First");
				
				auto result = _wcsicmp(title, pe32.szExeFile);
				if (result == 0 && result != 0xffffffff) {
					pid = pe32.th32ProcessID;
					break;
				};
				Process32Next(snap, &pe32);
			}
		}

		processHandle = OpenProcess(PROCESS_ALL_ACCESS, TRUE, pid);
	};

	INJECTOR(int pid, int type = PROCESS_TITLE) {
		HANDLE th = NULL;
		if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &th)) exit(1);
		PRIVILEGE pv(th);
		pv.SetPrivilege(SE_DEBUG_NAME, TRUE);
		if (pid < 0) ErrorMessage("PID is not exist");
		this->pid = pid;
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
		if (allocMemoryAddr == NULL) {
			auto errCode = GetLastError();
			printf("%d", errCode);
			return FALSE;
			/*ErrorMessage("Mem Alloc Failed");*/
		}
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

	void SetPID(DWORD PID) { this->pid = PID; AutoHandling();  };
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