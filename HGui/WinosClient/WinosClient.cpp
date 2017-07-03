// WinosClient.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"


#define MAX_LOADSTRING 100
#define WM_APP_START	(WM_USER+1)
#define WM_SIM_EXITS	(WM_USER+2)
#define WM_SCREENFLAG	(WM_USER+3)
#define BMP_WIDTHBYTES(w, bits) (((w*bits+31)>>5)<<2)

extern "C" BYTE *pLcdDisRam;
extern "C" int year, mon, day, hour, minute, second;


// Global Variables:
HINSTANCE hInst;
HANDLE hMutex    = NULL;
HANDLE hFileMap  = NULL;
HWND hWndSim     = NULL;


TCHAR szTitle[MAX_LOADSTRING];								// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];								// The title bar text

// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int ScreenW = 480, ScreenH = 272;


int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	//check the application was existing
	hMutex = CreateMutex(NULL, TRUE, _T("Haisoft_WinOS_Client_Mutex"));
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		CloseHandle(hMutex);
		return 0;
	}
	hWndSim = FindWindow(_T("HAISORT_WINOS_SIM"), _T("Hai_WinosSim"));
	if (hWndSim == NULL)
	{
		MessageBox(NULL, _T("Please run WinosSim.exe first!"), _T("ERROR"), MB_OK|MB_ICONERROR);
		CloseHandle(hMutex);
		return 0;
	}

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_WINOSCLIENT, szWindowClass, MAX_LOADSTRING);

	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_WINOSCLIENT);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_WINOSCLIENT);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindowEx(WS_EX_TOOLWINDOW, szWindowClass, szTitle, WS_POPUP ,0, 0, 0, 0, NULL, NULL, hInstance, NULL);
//   hWnd = CreateWindow(szWindowClass, szTitle, WS_POPUP,
//     0, 0, 0, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
extern "C" void  hai_DoTimeISR(void);
extern "C" void  _hai_MouseISR(BYTE MouseOp, WORD x, WORD y);
extern "C" void  _hai_KeyISR(WORD key, WORD KeyDnFlag);

extern "C" int main(int argc, char *argv[]);

DWORD WINAPI StartApp(LPVOID lpParameter)
{
	return main(0, NULL);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HANDLE hThread;
	DWORD ThreadID;
	TCHAR time[20];
	TCHAR date[20];

	switch (message) 
	{
		case WM_CREATE:
			GetTimeFormat(0, 0, 0, "hh':'mm':'ss tt", time, 20); 
			GetDateFormat(0, 0, 0, "dd',' MM dd yy", date, 20);
			hour = (time[0]-'0') * 10 + (time[1]-'0');
			minute = (time[3]-'0') * 10 + (time[4]-'0');
			second = (time[6]-'0') * 10 + (time[7]-'0');
			year = (date[10] - '0') * 10 + (date[11]-'0') + 2000;
			mon  = (date[4] - '0') * 10 + (date[5]-'0');
			day  = (date[7] - '0') * 10 + (date[8]-'0');
			hFileMap = CreateFileMapping((HANDLE)0xFFFFFFFF, NULL, PAGE_READWRITE, 0, BMP_WIDTHBYTES(ScreenW,24)*ScreenH, _T("Haisoft_WinOS_FileMap"));
			pLcdDisRam = (BYTE *)MapViewOfFile(hFileMap, FILE_MAP_WRITE, 0, 0, BMP_WIDTHBYTES(ScreenW,24)*ScreenH);
			if (hFileMap == NULL || pLcdDisRam == NULL)
			{
				MessageBox(NULL, _T("Communication is failed!"), _T("ERROR"), MB_OK|MB_ICONERROR);
				SendMessage(hWnd, WM_DESTROY, 0, 0);
				break;
			}
			hThread = CreateThread(NULL, 0x40000, StartApp, NULL, 0, &ThreadID);
			if (hThread == NULL)
			{
				MessageBox(NULL, _T("The function of CreateThread is failed!"), _T("ERROR"), MB_OK|MB_ICONERROR);
				SendMessage(hWnd, WM_DESTROY, 0, 0);
				break;
			}
			CloseHandle(hThread);
			Sleep(10);
			PostMessage(hWndSim, WM_APP_START, (WPARAM)hWnd, 0);
			break;

		case WM_LBUTTONDOWN:
			_hai_MouseISR(0, LOWORD(lParam), HIWORD(lParam));
			break;

		case WM_MOUSEMOVE:
			_hai_MouseISR(1, LOWORD(lParam), HIWORD(lParam));
			break;

		case WM_LBUTTONUP:
			_hai_MouseISR(2, LOWORD(lParam), HIWORD(lParam));
			break;

		case WM_LBUTTONDBLCLK:
			_hai_MouseISR(3, LOWORD(lParam), HIWORD(lParam));
			break;

		case WM_KEYDOWN:
			_hai_KeyISR(wParam, 0x8000);
			break;

		case WM_KEYUP:
			_hai_KeyISR(wParam, 0x0000);
			break;

		case WM_TIMER:
			hai_DoTimeISR();
			break;
	
		case WM_SCREENFLAG:
			break;

		case WM_SIM_EXITS:
			MessageBox(NULL, _T("WinosSim.exe exits!"), _T("ERROR"), MB_OK|MB_ICONERROR);
			SendMessage(hWnd, WM_DESTROY, 0, 0);
			break;

		case WM_DESTROY:
			if (hMutex)
				CloseHandle(hMutex);
			if (pLcdDisRam)
				UnmapViewOfFile(pLcdDisRam);
			if (hFileMap)
				CloseHandle(hFileMap);
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}
