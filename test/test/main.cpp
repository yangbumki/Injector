#include <windows.h>
#include <iostream>

int main() {
	wchar_t* testString = (wchar_t*)L"1541";
	int convertedNum = _wtoi(testString);
	printf_s("Converted testStirng : %d\n", convertedNum);
	return 0;
};