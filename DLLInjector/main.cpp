#include "Injector.hpp"

#include <Windows.h>
#include <iostream>

#define BUFSIZE		1024

const wchar_t* SelectNumber = L"0123";



int main(void) {
	setlocale(LC_ALL, "korean");
	_wsetlocale(LC_ALL, L"korean");

	injector* injector;
	TCHAR input[BUFSIZE] = { 0, };
	while (1) {
		std::cout << "[���μ��� ���]" << std::endl;
		std::cout << "1. ���μ��� ����" << std::endl;
		std::cout << "2. �������� �̸�" << std::endl;
		std::cout << "3. Process ID" << std::endl;
		std::cout << "�Է�: ";  _getws_s(input);

		auto select = _wtoi(input);
		switch (select) {
		case PROCESS_TITLE:
			std::cout << "[���μ��� ���� �Է�]" << std::endl;
			std::cout << "�Է� : "; memset(input, 0, BUFSIZE); _getws_s(input);
			injector = new INJECTOR(input, PROCESS_TITLE);
			break;
		case EXEC_NAME:
			std::cout << "[�������� �̸� �Է�]" << std::endl;
			std::cout << "�Է� : "; memset(input, 0, BUFSIZE); _getws_s(input);
			injector = new INJECTOR(input, EXEC_NAME);
			break;
		case PROCESS_ID:
			std::cout << "[Process ID �Է�]" << std::endl;
			std::cout << "�Է� : "; memset(input, 0, BUFSIZE); _getws_s(input);
			injector = new INJECTOR(_wtoi(input));
			break;
		default:
			continue;
		};
		break;
	};
	(input);

	while (1) {
		printf_s("������ DLL ��� �Է�: "); _getws_s(input);
		if (!injector->SetDLLPath(input)) {
			printf_s("DLL ��� �̻�\n");
			continue;
		};

		if (injector->Injection()) break;
	};
	
	return 0;
};