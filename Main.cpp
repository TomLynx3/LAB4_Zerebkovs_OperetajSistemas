#undef UNICODE
#include <windows.h>
#include <stdio.h>
#include "resource.h"
#include <inttypes.h>
#include <process.h>
#include <wingdi.h>
#include "TBuffer.h"
#include "CommonTools.h"



#pragma warning (disable:4996)


BOOL CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM);


enum class ProcessState { START,TERMINATE };

bool Terminate = false;

HANDLE handle;


HWND hMainWnd = 0;

static TBuffer Buffer = TBuffer();


LRESULT CALLBACK GraphWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
	switch (Msg)
	{
	case WM_PAINT:
	{
		int midProc = Buffer.GetMidProc();
		int maxProc = Buffer.GetMaxProc();
		char midProcStr[5];
		char maxProcStr[5];

		sprintf(midProcStr, "%.2d%%",
			midProc);

		sprintf(maxProcStr, "%.2d%%",
			maxProc);

		SetDlgItemText(hMainWnd, IDC_MID, midProcStr);
		SetDlgItemText(hMainWnd, IDC_MAX, maxProcStr);


		PAINTSTRUCT ps;
		HDC dc = BeginPaint(hWnd, &ps);
		RECT rect;

		GetClientRect(hWnd, &rect);


		Buffer.Draw(dc, rect);


		EndPaint(hWnd, &ps);;

		ReleaseDC(hWnd, dc);
		return 0;

	}
	case WM_MOUSEWHEEL:
	{
		
		short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);

		if (zDelta > 0 && Buffer.GetMaxProc()<=90) {

			Buffer.IncreaseProcessPercentage();


			InvalidateRect(GetDlgItem(hMainWnd, IDC_GRAPH), NULL, FALSE);
			
		}
		else if (zDelta < 0 && Buffer.GetMaxProc() >= 30) {

			Buffer.DecreaseProcessPercentage();

			InvalidateRect(GetDlgItem(hMainWnd, IDC_GRAPH), NULL, FALSE);
		}	

		return 0;
	}
	
	}

	return DefWindowProc(hWnd, Msg, wParam, lParam);

}




int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = GraphWndProc;
	wc.lpszClassName = "GRAPH";
	RegisterClass(&wc);
	
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAINDIALOG), NULL, (DLGPROC)MainWndProc);
	return 0; 
}


void HandleBrowse(HWND hWnd) {
	char filename[MAX_PATH] = "";
	if (CommonTools::BrowseFileName(hWnd, filename)) {
		SetDlgItemText(hWnd, IDC_COMMANDLINE, filename);
	}

}

bool RunProcess(LPSTR command) {
	STARTUPINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	PROCESS_INFORMATION pi;

	if (CreateProcess(
		NULL,
		command,
		NULL,
		NULL,
		FALSE,
		0,
		NULL,
		NULL,
		&si,
		&pi)
		)
	{
		handle = pi.hProcess;

		CloseHandle(pi.hThread);


		return true;
	}
	return false;
}




void DisableEnableControls(HWND hWnd, ProcessState state) {

	EnableWindow(GetDlgItem(hWnd, IDC_START), state == ProcessState::TERMINATE);
	EnableWindow(GetDlgItem(hWnd, IDC_TERMINATE), state == ProcessState::START);

}

void HandleTerminate(HWND hWnd) {
	DWORD exitCode = 0;
	if (handle != 0) {
		Terminate = true;
		TerminateProcess(handle, exitCode);
		DisableEnableControls(hWnd, ProcessState::TERMINATE);
		CloseHandle(handle);
		
	}

}


DWORD WINAPI ProcessThread(LPVOID lpParameter) {

	char filename[MAX_PATH] = "";

	GetDlgItemText(hMainWnd, IDC_COMMANDLINE, filename, MAX_PATH);
	Terminate = false;
	Buffer.Clear();

	if (RunProcess(filename)) {

		SetTimer(hMainWnd, NULL, 125, NULL);

		DisableEnableControls(hMainWnd, ProcessState::START);


		FILETIME creation_time, exit_time, kernel_time, kernel_time1, user_time, user_time1;

		while ( !Terminate) {
			GetProcessTimes(handle,&creation_time,&exit_time, &kernel_time, &user_time);

			DWORD tu1 = CommonTools::FileTimeToMilliseconds(user_time);
			DWORD tk1 = CommonTools::FileTimeToMilliseconds(kernel_time);
			DWORD tr1 = GetTickCount();

			Sleep(100);

			GetProcessTimes(handle, &creation_time, &exit_time, &kernel_time1, &user_time1);

			DWORD tu2 = CommonTools::FileTimeToMilliseconds(user_time1);
			DWORD tk2 = CommonTools::FileTimeToMilliseconds(kernel_time1);
			DWORD tr2 = GetTickCount();

			int tu = 100 * (tu2 - tu1) / (tr2 - tr1);
			int tk = 100 * (tk2 - tk1) / (tr2 - tr1);

			Buffer.Put(TBufItem{ tu,tk });

			DWORD exitCode;
			GetExitCodeProcess(handle, &exitCode);

			if (exitCode == NULL) {
				Terminate = true;
				HandleTerminate(hMainWnd);
				break;
			}

		}

		KillTimer(hMainWnd, NULL);

	}

	return 0;
}


 

void HandleStart(HWND hWnd) {
	char filename[MAX_PATH] = "";
	GetDlgItemText(hWnd, IDC_COMMANDLINE, filename, MAX_PATH);
	if (RunProcess(filename)) {
		
		SetTimer(hWnd, NULL, 125, NULL);

		DisableEnableControls(hWnd, ProcessState::START);

	}
}


BOOL CALLBACK MainWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {

	switch (Msg) {
	case WM_INITDIALOG:
		hMainWnd = hWnd;
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			DestroyWindow(hWnd);
			return TRUE;
		case IDC_BROWSE:
			HandleBrowse(hWnd);
			return TRUE;
		case IDC_START:
			CloseHandle(CreateThread(NULL, 0, ProcessThread, NULL, 0, NULL));
			return TRUE;
		case IDC_TERMINATE:
			HandleTerminate(hWnd);
			return TRUE;
		}	
		return FALSE;
	case WM_DESTROY:
		PostQuitMessage(0);
		return TRUE;
	case WM_TIMER:
		InvalidateRect(GetDlgItem(hWnd, IDC_GRAPH), NULL, FALSE);
		return TRUE;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		return TRUE;

	case WM_LBUTTONDOWN:

		SetFocus(GetDlgItem(hWnd, IDC_GRAPH));
		return TRUE;
		
	}
	return FALSE;
}