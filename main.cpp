#include "Injector.hpp"

#include <Windows.h>
#include <iostream>

#define BUFSIZE		1024

int main(void) {
	setlocale(LC_ALL, "korean");
	_wsetlocale(LC_ALL, L"korean");

	TCHAR input[BUFSIZE] = { 0, };
	printf_s("���μ��� ���� �Է�: "); _getws_s(input);//fgetws(input,BUFSIZE,stdin);
	injector injector(input);

	while (1) {
		printf_s("������ DLL ��� �Է�: "); _getws_s(input);
		if (!injector.SetDLLPath(input)) {
			printf_s("DLL ��� �̻�\n");
			continue;
		};

		injector.Injection();
	};
	
	injector.Injection();
	return 0;
};