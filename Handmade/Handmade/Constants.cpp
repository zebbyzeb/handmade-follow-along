#include <string>
#include "Constants.h"
#include <Windows.h>

const std::string Constants::WindowClassName = "HandmadeHeroWindowClass";

const std::string Constants::WindowName = "Handmade Hero";

wchar_t* Constants::WideString(std::string s) {
	int requiredBufferSize = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, 0, 0);

	wchar_t* pWideString = new wchar_t[requiredBufferSize];
	MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, pWideString, requiredBufferSize);

	return pWideString;
}