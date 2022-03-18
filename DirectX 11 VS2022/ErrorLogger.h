#pragma once
//#include "StringConverter.h"
#include <string>
#include <Windows.h>
class ErrorLogger
{
public:
	static void Log(std::wstring message);
	static void Log(HRESULT hr, std::wstring message);
};