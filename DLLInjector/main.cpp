#include "Injector.hpp"
#include "Privilege.hpp"

#include <Windows.h>
#include <iostream>

#define BUFSIZE		1024
//#define PRIVILEGE_DEBUG		1

void ErrorMessage(const char* msg);

const wchar_t* SelectNumber = L"0123";

int main(void) {
	setlocale(LC_ALL, "korean");
	_wsetlocale(LC_ALL, L"korean");

	injector* injector;
	TCHAR input[BUFSIZE] = { 0, };
	TCHAR dllPath[BUFSIZE] = { 0, };

	HANDLE snap = NULL;
	PROCESSENTRY32 pe32 = { 0, };
;
	HANDLE th = NULL;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &th)) ErrorMessage("OpenProcessToken");
	PRIVILEGE pv(th);

	if (!pv.SetPrivilege(SE_DEBUG_NAME, TRUE)) {
		ErrorMessage("Privileage");
	};
	


#ifdef PRIVILEGE_DEBUG
	INJECTOR* ij = new INJECTOR(9628);
	ij->Injection();
#endif

	printf_s("인젝션 DLL 경로 입력: "); _getws_s(dllPath);
	
	while (1) {
		std::cout << "[프로세스 방식]" << std::endl;
		std::cout << "1. 프로세스 제목" << std::endl;
		std::cout << "2. 실행파일 이름" << std::endl;
		std::cout << "3. Process ID" << std::endl;
		std::cout << "4. Global" << std::endl;
		std::cout << "입력: ";  _getws_s(input);

		auto select = _wtoi(input);
		switch (select) {
		case PROCESS_TITLE:
			std::cout << "[프로세스 제목 입력]" << std::endl;
			std::cout << "입력 : "; memset(input, 0, BUFSIZE); _getws_s(input);
			injector = new INJECTOR(input, PROCESS_TITLE);
			break;
		case EXEC_NAME:
			std::cout << "[실행파일 이름 입력]" << std::endl;
			std::cout << "입력 : "; memset(input, 0, BUFSIZE); _getws_s(input);
			injector = new INJECTOR(input, EXEC_NAME);
			break;
		case PROCESS_ID:
			std::cout << "[Process ID 입력]" << std::endl;
			std::cout << "입력 : "; memset(input, 0, BUFSIZE); _getws_s(input);
			injector = new INJECTOR(_wtoi(input));
			break;

		case GLOBAL:
			snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
			if (snap == NULL) ErrorMessage("snap");

			pe32.dwSize = sizeof(PROCESSENTRY32);

			if (!Process32First(snap, &pe32)) ErrorMessage("Process32First");

			while (1) {
				injector = new INJECTOR(pe32.th32ProcessID);
				wprintf_s(L"%s\n", pe32.szExeFile);
				if (!injector->SetDLLPath(dllPath)) ErrorMessage("DLL 경로 이상\n");
				if(!injector->Injection()) printf_s("Failed \n");
				delete(injector);
				auto result = Process32Next(snap, &pe32);
				if (result == NULL) break;
			}
			CloseHandle(snap);

		default:
			continue;
		};
		break;
	};

	if (!injector->SetDLLPath(dllPath)) ErrorMessage("DLL 경로 이상\n");
	if (injector == nullptr) ErrorMessage("injector");
	injector->Injection();
};

void ErrorMessage(const char* msg) {
	MessageBoxA(NULL, msg, "ERROR", NULL);
	exit(1);
};