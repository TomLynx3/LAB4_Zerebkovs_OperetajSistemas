#undef UNICODE

#include "CommonTools.h"
#include <windows.h>


#pragma warning (disable:4996)

bool CommonTools::BrowseFileName(HWND hWnd, char* FileName)
{
	OPENFILENAME ofn = { 0 };
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFilter ="Executable Files (*.exe)\0*.exe\0"
		"All Files (*.*)\0*.*\0";
	ofn.lpstrFile = FileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	ofn.lpstrDefExt ="exe";
	return GetOpenFileName(&ofn);
}

DWORD CommonTools::FileTimeToMilliseconds(FILETIME ft)
{
	ULARGE_INTEGER i;
	i.LowPart = ft.dwLowDateTime;
	i.HighPart = ft.dwHighDateTime;
	return i.QuadPart / 10000;
}


