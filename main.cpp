#include "Injector.hpp"

#include <Windows.h>
#include <iostream>

#define BUFSIZE		1024

int main(void) {
	setlocale(LC_ALL, "korean");
	_wsetlocale(LC_ALL, L"korean");

	TCHAR input[BUFSIZE] = { 0, };
	printf_s("프로세스 제목 입력: "); _getws_s(input);//fgetws(input,BUFSIZE,stdin);
	injector injector(input);

	while (1) {
		printf_s("인젝션 DLL 경로 입력: "); _getws_s(input);
		if (!injector.SetDLLPath(input)) {
			printf_s("DLL 경로 이상\n");
			continue;
		};

		injector.Injection();
	};
	
	injector.Injection();
	return 0;
};