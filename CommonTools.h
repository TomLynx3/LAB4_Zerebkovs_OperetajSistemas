#include <windows.h>


#pragma once

class CommonTools
{
public:
	static bool BrowseFileName(HWND hWnd, char* FileName);
	static DWORD FileTimeToMilliseconds(FILETIME ft);
};

