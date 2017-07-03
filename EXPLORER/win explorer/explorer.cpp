/***************************************************************************/
/* Copyright (C) 2006, Haisoft有限公司                                     */
/* All rights reserved.                                                    */

/* 文件名称: Explore.cpp                                                   */
/* 摘    要：仿WINDOWS中的扫雷的游戏 ，加入了玩家管理功能                  */

/* 当前版本: 2.0                                                           */
/* 作    者: 廖金海                                                        */
/* 完成日期: 2006.11.1                                                     */
/***************************************************************************/


/************************* include files ***********************************/
#include "stdafx.h"
#include "resource.h"
#include "assert.h"
#include "stdio.h"

/************************** macro define ***********************************/
#define MAX_LOADSTRING		16
#define C_BTNFACE			0xC0C0C0
#define C_BTNHIGHLIGHT		0xFFFFFF
#define C_BTNSHADOW			0x808080
#define GRIDWIDTH			16
#define EB_PRESSENTER		0x8000
#define EB_PRESSESC			0x8001

/************************* data structure **********************************/
struct GRID             /* 方格的数据结构 */
{
    int mine;           /* -1：有雷； 0-8：周围八格雷的情况 */
    int status;         /* 1：初始化；2：已排雷；3：标雷；4：怀疑；0：踩雷; 5:显示雷; 6: 标雷错误*/
    int check;          /* 1：标示正确；0：标示错误或无标示 */
};

struct USER             /* 玩家的数据结构 */
{
	TCHAR name[12];        /* 姓名 */
	int  time[3];        /* 不同级别的最少时间 */
};

struct HERO             /* 扫雷英雄的数据结构 */       
{
    TCHAR name[12];       /* 扫雷英雄的姓名 */ 
    int time;           /* 扫雷英雄所用的最少时间 */
};

struct FACEPLATE
{
	WORD x;
	WORD y;
	WORD w;
};

/************************* global constant *********************************/

/************************* global variable *********************************/
HWND hWND;
HWND hwndpler, hwndedit, hwndlevel, hwndlevel2, hwndscore, hwndscore2;
HWND hwndreset, hwndmine, hwndtime, hwndhero, hwndabout;
HBRUSH hBrushwhite, hBrushblack;
HINSTANCE hInst;											// current instance
WNDPROC fnEditProc;

TCHAR szTitle[MAX_LOADSTRING] = TEXT("探险");			// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING]  = TEXT("W_EXPLORER");	// The title bar text

struct FACEPLATE FacePlate[3] =
{
	{106, 150, 183},
	{46,  105, 291},
	{10,  75,  363}
};
WORD Level = 0;
WORD GridLine[3] = {10, 16, 20};
WORD MineNum[3]  = {15, 40, 80};
TCHAR MineStr[3][6]  = {TEXT("15"),TEXT("40"),TEXT("80")};
TCHAR LevelStr[3][6] = {TEXT("低级"), TEXT("中级"), TEXT("高级")};

struct GRID board[20][20];        /* 雷区: 20*20; */
struct USER player, players[10];  /* player为当前玩家，players为储存的玩家*/
struct HERO hero[3];              /* hero为扫雷英雄排行榜 */
int    gamestart, gametime, timestart;		  /* gamestart:游戏开始； gametime:游戏用时；*/
int    MineRestNum;               /* 剩余雷的数目 */

/************************ function prototype *******************************/
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	RecardProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK	HeroProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK	EditProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

BOOL Draw3DRect(HDC hDC, WORD x, WORD y, WORD cx, WORD cy, BOOL IsUp);
BOOL DrawGridding(HDC hDC, WORD x, WORD y, WORD GridCnt);
void DrawMine(HDC hDC, WORD x, WORD y);
void DrawRedRect(HDC hDC, WORD x, WORD y);
void DrawRedFlag(HDC hDC, WORD x, WORD y);
void DataInit(void);
int  Win(int gridnum);
int  PlayGameL(HDC hDC, int startx, int starty, int x, int y, int gridnum);
int  PlayGameLDblClk(HDC hDC, int startx, int starty, int x, int y, int gridnum);
void PlayGameR(HDC hDC, int startx, int starty, int x, int y);   /* 右击方格的处理 */
void OpenGrid(HDC hDC, int startx, int starty, int i, int j, int gridnum);
void DisplayOperateResult(HDC hDC, int status, int boardX, int boardY, int i, int j);
void GameOver(int gridnum);
void PlayerInit(void);
void CountMine(int flag);/* 运算剩余雷数：flag=-1标了一个雷,flag=1标了一问号 */
void WriteDisk(void);        /* 游戏结束，数据写盘 */
void Record(int level);  /* 记录成绩 */
void GameMenu(HWND hwnd, int x, int y);
void NameInputOver(void);
void SelectPlayer(void);

/************************** main function **********************************/
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	MSG msg;
	
	MyRegisterClass(hInstance);
	
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}
	
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	return msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	
	wcex.cbSize = sizeof(WNDCLASSEX); 
	
	wcex.style			= CS_DBLCLKS;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= DLGWINDOWEXTRA;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_EXPLORER);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_BTNFACE+1);
	wcex.lpszMenuName	= (LPCSTR)NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);
	
	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	RECT rc;
	int  x, y;
	
	hInst = hInstance; // Store instance handle in our global variable
	
	hWND = GetDesktopWindow();
	GetClientRect(hWND, &rc);
	x = (rc.right - rc.left) - 390;
	y = (rc.bottom - rc.top) - 480;
	if (x < 0) x = 0;
	if (y < 0) y = 0;
	x /= 2;
	y /= 2;
	
	hWND = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX,
		x, y, 390, 480, NULL, NULL, hInstance, NULL);
	
	if (!hWND)
	{
		return FALSE;
	}
	
	ShowWindow(hWND, nCmdShow);
	UpdateWindow(hWND);
	
	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int i, j;
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hDC;
	WORD x, y;
	WORD operate;
	WORD id;
	WORD boardX = FacePlate[Level].x;
	WORD boardY = FacePlate[Level].y;
	TCHAR str[14];
	
	switch (message) 
	{
	case WM_CREATE:
		hwndpler   = CreateWindow(TEXT("BUTTON"), TEXT("玩家"),WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_CENTER|WS_TABSTOP, 14, 24, 40, 24, hWnd, (HMENU)1, hInst, NULL);
		hwndedit   = CreateWindow(TEXT("EDIT"), TEXT("无名"),WS_CHILD|WS_VISIBLE|ES_LEFT|WS_BORDER, 58, 24, 80, 24, hWnd, (HMENU)2, hInst, NULL);
		hwndlevel  = CreateWindow(TEXT("BUTTON"), TEXT("级别"),WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_CENTER|WS_TABSTOP, 140, 12, 40, 24, hWnd, (HMENU)3, hInst, NULL);
		hwndlevel2 = CreateWindow(TEXT("STATIC"), TEXT("低级"),WS_CHILD|WS_VISIBLE|SS_CENTER, 182, 14, 44, 20, hWnd, (HMENU)4, hInst, NULL);
		hwndscore  = CreateWindow(TEXT("STATIC"), TEXT("成绩"),WS_CHILD|WS_VISIBLE|SS_CENTER, 140, 38, 40, 20, hWnd, (HMENU)5, hInst, NULL);
		hwndscore2 = CreateWindow(TEXT("STATIC"), TEXT("999秒"),WS_CHILD|WS_VISIBLE|SS_CENTER,182, 38, 60, 20, hWnd, (HMENU)6, hInst, NULL);
		hwndhero   = CreateWindow(TEXT("BUTTON"), TEXT("英雄榜"),WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_CENTER|WS_TABSTOP, 228, 12, 60, 24, hWnd, (HMENU)7, hInst, NULL);
		hwndreset  = CreateWindow(TEXT("BUTTON"), TEXT("重置"),WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_CENTER|WS_TABSTOP, 290, 12, 40, 24, hWnd, (HMENU)8, hInst, NULL);
		hwndabout  = CreateWindow(TEXT("BUTTON"), TEXT("关于"),WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_CENTER|WS_TABSTOP, 332, 12, 40, 24, hWnd, (HMENU)9, hInst, NULL);
		hwndmine   = CreateWindow(TEXT("STATIC"), TEXT("15"),WS_CHILD|WS_VISIBLE|SS_CENTER, 264, 38, 40, 20, hWnd, (HMENU)10, hInst, NULL);
		hwndtime   = CreateWindow(TEXT("STATIC"), TEXT("000"),WS_CHILD|WS_VISIBLE|SS_CENTER, 320, 38, 40, 20, hWnd, (HMENU)11, hInst, NULL);
		
		fnEditProc = (WNDPROC)SetWindowLong(hwndedit, GWL_WNDPROC, (LONG)EditProc);
		hBrushwhite = (HBRUSH)GetStockObject(WHITE_BRUSH);
		hBrushblack = (HBRUSH)GetStockObject(BLACK_BRUSH);
		
		DataInit();
		PlayerInit();
		
		SetTimer(hWnd, 1, 1000, NULL);
		break;
		
	case WM_SETFOCUS:
		//			SetFocus(hwndpler);
		break;
	case WM_KILLFOCUS:
		//			if ((HWND)wParam != hwndedit)
		//				SetFocus(hWnd);
		break;
		
	case WM_CTLCOLORSTATIC:
		id = (WORD)GetWindowLong((HWND)lParam, GWL_ID);
		if (id == 4 || id == 6)
			return (LRESULT)hBrushwhite;
		if (id == 10 || id == 11)
		{
			SetTextColor((HDC)wParam, RGB(255, 0, 0));
			SetBkColor((HDC)wParam, RGB(0, 0, 0));
			return (LRESULT)hBrushblack;
		}
		break;
	case WM_CTLCOLOREDIT:
		return (LRESULT)hBrushwhite;
		
	case WM_COMMAND:
		wmId    = LOWORD(wParam); 
		wmEvent = HIWORD(wParam); 
		switch (wmId)
		{
		case 1:
			SetFocus(hWnd);
			SelectPlayer();
			break;
		case 2:
			if (wmEvent == EB_PRESSESC)
			{
				SetWindowText(hwndedit, player.name);
				SetFocus(hWnd);
				break;
			}
			if (wmEvent == EB_PRESSENTER)
			{
				NameInputOver();
				SetFocus(hWnd);
			}
			break;
		case 200:
		case 201:
		case 202:
		case 3:
			if (wmId != 3)
				Level = wmId-200;
			else
				Level = (Level+1)%3;
			SetWindowText(hwndlevel2, LevelStr[Level]);
		case 8:
			wsprintf(str, TEXT("%3d秒"), player.time[Level]);
			SetWindowText(hwndscore2, str);
			SetWindowText(hwndmine, MineStr[Level]);
			SetWindowText(hwndtime, TEXT("000"));
			DataInit();
			InvalidateRect(hWnd, NULL, TRUE);
			SetFocus(hWnd);
			break;
		case 7:
			DialogBox(hInst, (LPCTSTR)IDD_HERODLG, hWnd, (DLGPROC)HeroProc);
			SetFocus(hWnd);
			break;
		case 9:
			DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
			SetFocus(hWnd);
			break;
		case 400:
		case 401:
		case 402:
		case 403:
		case 404:
		case 405:
		case 406:
		case 407:
		case 408:
		case 409:
			i = wmId - 400;
			player     = players[i];
			players[i] = players[0];
			players[0] = player;
			SetWindowText(hwndedit, player.name);
			SendMessage(hWnd, WM_COMMAND, 8, 0);
			break;
			
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
		case WM_PAINT:
			hDC = BeginPaint(hWnd, &ps);
			Draw3DRect(hDC, 1, 1, 381, 445, TRUE);
			Draw3DRect(hDC, 10, 10, 363, 50, FALSE);
			Draw3DRect(hDC, boardX, boardY, FacePlate[Level].w, FacePlate[Level].w, FALSE);
			for (i = 0; i < GridLine[Level]; i++)
				for (j = 0; j < GridLine[Level]; j++)
					DisplayOperateResult(hDC, board[i][j].status, boardX, boardY, i, j);
				DrawGridding(hDC, boardX+2, boardY+2, GridLine[Level]);
				EndPaint(hWnd, &ps);
				break;
				
		case WM_LBUTTONDOWN:
			if (!gamestart)
				break;
			hDC = GetDC(hWnd);
			x = LOWORD(lParam); y = HIWORD(lParam);
			if (x > boardX && x < boardX + FacePlate[Level].w && y > boardY && y < boardY+ FacePlate[Level].w)
			{
				timestart = 1;
				operate = PlayGameL(hDC, boardX, boardY, x, y, GridLine[Level]);
				if (operate)           /* 左击正确 */
				{
					if (Win(GridLine[Level]))   /* 判断一下是否赢了 */
					{
						gamestart = 0;       /* 为0，游戏计时停止 */
						timestart = 0;
						Record(Level);/* 判断是否破纪录 */
					}
				}
				else         /* 游戏失败的处理 */
				{
					gamestart = 0;   /* 记时停止 */
					timestart = 0;
					GameOver(GridLine[Level]);/* 打开剩余格子 */
					InvalidateRect(hWnd, NULL, TRUE);
				}
			}
			ReleaseDC(hWnd, hDC);
			break;
			
		case WM_RBUTTONDOWN:
			x = LOWORD(lParam); y = HIWORD(lParam);
			if (!gamestart || !timestart && (x <= boardX || x >= boardX + FacePlate[Level].w || y <= boardY || y >= boardY+ FacePlate[Level].w))//
			{
				POINT point;
				
				point.x = x; point.y = y;
				ClientToScreen(hWnd, &point);
				GameMenu(hWnd, point.x, point.y);
				break;
			}
			hDC = GetDC(hWnd);
			if (x > boardX && x < boardX + FacePlate[Level].w && y > boardY && y < boardY+ FacePlate[Level].w)
			{
				timestart = 1;
				PlayGameR(hDC, boardX, boardY, x, y);   /* 右击方格的处理 */
				if (Win(GridLine[Level]))                 /* 判断是否赢 */
				{
					gamestart = 0;                     /* 暂停记时 */
					timestart = 0;
					Record(Level);    /* 是否破纪录 */
				}
			}
			ReleaseDC(hWnd, hDC);
			break;
			
		case WM_LBUTTONDBLCLK:
			if (!gamestart)
				break;
			hDC = GetDC(hWnd);
			x = LOWORD(lParam); y = HIWORD(lParam);
			if (x > boardX && x < boardX + FacePlate[Level].w && y > boardY && y < boardY+ FacePlate[Level].w)
			{
				i = (x - boardX) / (GRIDWIDTH+2); /* 鼠标指向格子的行列下标值 */
				j = (y - boardY) / (GRIDWIDTH+2);
				
				if (board[i][j].status == 2)
				{
					operate = PlayGameLDblClk(hDC, boardX, boardY, x, y, GridLine[Level]);
					if (operate)           /* 双击正确 */
					{
						if (Win(GridLine[Level]))   /* 判断一下是否赢了 */
						{
							gamestart = 0;       /* 为0，游戏计时停止 */
							timestart = 0;
							Record(Level);/* 判断是否破纪录 */
						}
					}
					else         /* 游戏失败的处理 */
					{
						gamestart = 0;   /* 记时停止 */
						timestart = 0;
						GameOver(GridLine[Level]);/* 打开剩余格子 */
						InvalidateRect(hWnd, NULL, TRUE);
					}
				}
			}
			ReleaseDC(hWnd, hDC);
			break;
			
		case WM_CHAR:
			switch (wParam)
			{
			case ' ':
				timestart = 0;
				break;
			case '\t':
				SetFocus(hwndedit);
				break;
			}
			break;
			
			case WM_TIMER:
				if (timestart)
				{
					if (gametime < 999)
						gametime++;
					wsprintf(str, TEXT("%03d"), gametime);
					SetWindowText(hwndtime, str);
				}
				break;
				
			case WM_DESTROY:
				KillTimer(hWnd, 1);
				WriteDisk();
				PostQuitMessage(0);
				break;
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;
		
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
    return FALSE;
}

BOOL Draw3DRect(HDC hDC, WORD x, WORD y, WORD cx, WORD cy, BOOL IsUp)
{
	HPEN hPen;
	COLORREF TopColor, BottomColor;
	
	if (IsUp)
	{
		TopColor    = C_BTNHIGHLIGHT;
		BottomColor = C_BTNSHADOW;
	}
	else
	{
		TopColor    = C_BTNSHADOW;
		BottomColor = C_BTNHIGHLIGHT;
	}
	hPen = CreatePen(PS_SOLID, 1, TopColor);
	assert(hPen);
	SelectObject(hDC, hPen);
	
	MoveToEx(hDC, x, y, NULL);
	LineTo(hDC, x+cx, y);
	MoveToEx(hDC, x, y+1, NULL);
	LineTo(hDC, x+cx, y+1);
	
	MoveToEx(hDC, x, y, NULL);
	LineTo(hDC, x, y+cy);
	MoveToEx(hDC, x+1, y, NULL);
	LineTo(hDC, x+1, y+cy);
	
	DeleteObject(hPen);
	hPen = CreatePen(PS_SOLID, 1, BottomColor);
	assert(hPen);
	SelectObject(hDC, hPen);
	
	MoveToEx(hDC, x+cx-1, y+2, NULL);
	LineTo(hDC, x+cx-1, y+cy);
	MoveToEx(hDC, x+cx, y+1, NULL);
	LineTo(hDC, x+cx, y+cy);
	
	MoveToEx(hDC, x+2, y+cy-1, NULL);
	LineTo(hDC, x+cx, y+cy-1);
	MoveToEx(hDC, x+1, y+cy, NULL);
	LineTo(hDC, x+cx+1, y+cy);
	
	SelectObject(hDC, GetStockObject(BLACK_PEN));
	DeleteObject(hPen);
	
	return TRUE;
}
void DrawRedRect(HDC hDC, WORD x, WORD y)
{
	HBRUSH hBrush;
	
	hBrush = CreateSolidBrush(RGB(255,0,0));
	hBrush = (HBRUSH)SelectObject(hDC, hBrush);
	Rectangle(hDC, x, y, x+GRIDWIDTH, y+GRIDWIDTH);
	hBrush = (HBRUSH)SelectObject(hDC, hBrush);
	DeleteObject(hBrush);
}

void DrawRedFlag(HDC hDC, WORD x, WORD y)
{
	HBRUSH hBrush;
	HPEN   hPen;
	
	hBrush = CreateSolidBrush(RGB(255,0,0));
	hBrush = (HBRUSH)SelectObject(hDC, hBrush);
	hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
	hPen = (HPEN)SelectObject(hDC, hPen);
	Rectangle(hDC, x+4, y+4, x+GRIDWIDTH-4, y+GRIDWIDTH-6);
	MoveToEx(hDC, x+GRIDWIDTH-4, y+4, NULL);
	LineTo(hDC, x+GRIDWIDTH-4, y+GRIDWIDTH-2);
	MoveToEx(hDC, x+GRIDWIDTH-5, y+4, NULL);
	LineTo(hDC, x+GRIDWIDTH-5, y+GRIDWIDTH-2);
	hBrush = (HBRUSH)SelectObject(hDC, hBrush);
	DeleteObject(hBrush);
	hPen = (HPEN)SelectObject(hDC, hPen);
	DeleteObject(hPen);
}

void DrawMine(HDC hDC, WORD x, WORD y)
{
	HBRUSH hBrush;
	
	hBrush = (HBRUSH)SelectObject(hDC, GetStockObject(BLACK_BRUSH));
	Ellipse(hDC, x+3, y+3, x+GRIDWIDTH-2, y+GRIDWIDTH-2);
	SelectObject(hDC, hBrush);
	MoveToEx(hDC, x+1, y+GRIDWIDTH/2, NULL);
	LineTo(hDC, x+GRIDWIDTH, y+GRIDWIDTH/2);
	MoveToEx(hDC, x+GRIDWIDTH/2, y+1, NULL);
	LineTo(hDC, x+GRIDWIDTH/2, y+GRIDWIDTH);
}

BOOL DrawGridding(HDC hDC, WORD x, WORD y, WORD GridCnt)
{
	WORD i;
	
	for (i = 0; i < GridCnt+1; i++)
	{
		MoveToEx(hDC, x-1+i*(GRIDWIDTH+2), y, NULL);
		LineTo(hDC, x-1+i*(GRIDWIDTH+2), y-1+GridCnt*(GRIDWIDTH+2));
	}
	for (i = 0; i < GridCnt+1; i++)
	{
		MoveToEx(hDC, x, y-1+i*(GRIDWIDTH+2), NULL);
		LineTo(hDC, x-1+GridCnt*(GRIDWIDTH+2), y-1+i*(GRIDWIDTH+2));
	}
	return TRUE;
}

void DataInit(void)   /* 初始化为level级别的数据 */
{
	int i, j, m, n;
	int gridnum;		/* gridnum为行列格子数 */
	SYSTEMTIME SysTime;
	
	gridnum     = GridLine[Level];				/* 根据level决定行列格子数和雷数 */
	MineRestNum = MineNum[Level];
	
	gametime  = 0;    
	timestart = 0;
	gamestart = 1;
	
	for (i = 0; i < gridnum; i++)        /* 方格的初始各值 */
	{
		for (j = 0; j < gridnum; j++)
        {
			board[i][j].status = 1;
			board[i][j].mine = board[i][j].check = 0;
        }
	}
	GetLocalTime(&SysTime);
	srand(SysTime.wMinute+SysTime.wMilliseconds);                      /* 随机布雷 */
	for (m = 0; m < MineNum[Level];)
	{
		i = rand() % gridnum;
		j = rand() % gridnum;
		if (board[i][j].mine != -1)
		{
			board[i][j].mine = -1;
			m++;
		}
	}
	
	for (i = 0; i < gridnum; i++)        /* 无雷格记录周围的八个格子的雷数 */
	{
		for (j = 0; j < gridnum; j++)
        {
			if (board[i][j].mine != -1)
			{
				for (m = i-1; m <= i+1; m++)
				{
					if ((m < 0) || (m >= gridnum)) 
						continue;
					for (n = j-1; n <= j+1; n++)
					{
						if ((n < 0) || (n >= gridnum)) 
							continue;
						if (board[m][n].mine == -1) 
							board[i][j].mine++;
					}
				}
			}
        }
	}
}

/* 如果左击方格的周围八个方格无雷，则全打开，递归。i,j为当前格的坐标*/
void OpenGrid(HDC hDC, int startx, int starty, int i, int j, int gridnum)
{
	TCHAR num[2] = {0,0};
	int x, y;
	
	if (board[i][j].mine != 0)  /* 左击的方格值不为0，则返回 */
	{
		return;
	}
	
	for (x = i-1; x <= i+1; x++) /* 打开周围八格 */
	{
		if ((x < 0) || (x >= gridnum))   /* x下标越界继续 */
		{
			continue;
		}
		
		for (y = j-1; y <= j+1; y++)
		{
			if ((y < 0) || (y >= gridnum))  /* y下标越界继续 */
			{
				continue;
			}
			
			if (board[x][y].mine != 0)          /* 周围八格有雷的情况 */
			{
				if (board[x][y].status == 2)
					continue;
				if (board[x][y].status == 3)   /* 如标雷则错误，雷数加1 */
				{
					MineRestNum++;
				}
				board[x][y].status = 2;
				board[x][y].check = 1;
				DisplayOperateResult(hDC, board[x][y].status, startx, starty, x, y);
			}
			
			else if (board[x][y].status != 2)/*周围八格无雷的情况，且没打开*/
			{
				if (board[x][y].status == 3)/* 如标雷则错误，雷数加1 */
				{
					MineRestNum++;
				}
				board[x][y].status = 2;
				board[x][y].check = 1;
				DisplayOperateResult(hDC, board[x][y].status, startx, starty, x, y);
				OpenGrid(hDC, startx, starty, x, y, gridnum);     /* 递归调用 */
			}
		}
	}
	CountMine(0);
}

/* 左击雷区进行的处理 */
int PlayGameL(HDC hDC, int startx, int starty, int x, int y, int gridnum)
{
	TCHAR num[4];
	int i, j;
	
	i = (x - startx) / (GRIDWIDTH+2); /* 鼠标指向格子的行列下标值 */
	j = (y - starty) / (GRIDWIDTH+2);
	
	if (board[i][j].status == 1)              /* 指向初始状态的格子 */
	{
		if (board[i][j].mine != -1)          /* 指向无雷格 */
		{
			if (board[i][j].mine != 0) /*周围八格有若干个雷的情况,打开本身*/
			{
				board[i][j].status = 2;
				board[i][j].check = 1;
				Draw3DRect(hDC, startx+2+i*(GRIDWIDTH+2), starty+2+j*(GRIDWIDTH+2), GRIDWIDTH, GRIDWIDTH, FALSE);
				num[0] = board[i][j].mine+48;
				num[1] = 0;
				SetBkMode(hDC, TRANSPARENT);
				TextOut(hDC, startx+7+i*(GRIDWIDTH+2), starty+3+j*(GRIDWIDTH+2), num, lstrlen(num));
			}
			else                    /* 周围八格无雷，打开本身和周围八个格 */
			{                    /* 递归 */
				OpenGrid(hDC, startx, starty, i, j, gridnum);
			} 
			
			return 1;
		}
		else                        /* 左击错误，标记有雷，返回0 */
		{
			board[i][j].status = 0;
			board[i][j].check = 0;
			return 0;
		}
	}
	else        /* 其它格 */
	{
		return 1;
	}
}

int PlayGameLDblClk(HDC hDC, int startx, int starty, int x, int y, int gridnum)
{
	//	TCHAR num[4];
	int i, j;
	
	i = (x - startx) / (GRIDWIDTH+2); /* 鼠标指向格子的行列下标值 */
	j = (y - starty) / (GRIDWIDTH+2);
	
	for (x = i-1; x <= i+1; x++) /* 打开周围八格 */
	{
		if ((x < 0) || (x >= gridnum))   /* x下标越界继续 */
		{
			continue;
		}
		
		for (y = j-1; y <= j+1; y++)
		{
			if ((y < 0) || (y >= gridnum))  /* y下标越界继续 */
			{
				continue;
			}
			
			if (board[x][y].status != 2 && board[x][y].status != 3)
			{
				if (board[x][y].mine == -1)
				{
					board[x][y].status = 0;
					DisplayOperateResult(hDC, board[x][y].status, startx, starty, x, y);
					return 0;
				}
				board[x][y].status = 2;
				board[x][y].check = 1;
				DisplayOperateResult(hDC, board[x][y].status, startx, starty, x, y);
				if (board[x][y].mine == 0)
					OpenGrid(hDC, startx, starty, x, y, gridnum);
			}
		}
	}
	return 1;
}

void PlayGameR(HDC hDC, int startx, int starty, int x, int y)   /* 右击方格的处理 */
{
	int i, j;
	
	i = (x - startx) / (GRIDWIDTH+2);  /* 鼠标指向方格的坐标 */
	j = (y - starty) / (GRIDWIDTH+2);
	
	if (board[i][j].status == 1)     /* 标雷 */
	{
		board[i][j].status = 3;
		if (board[i][j].mine == -1)
		{
			board[i][j].check = 1;
		}
		DrawRedFlag(hDC, i*(GRIDWIDTH+2)+startx+2, j*(GRIDWIDTH+2)+starty+2);
		CountMine(-1);
		return;
	}
	
	if (board[i][j].status == 3)    /* 标？：表示此格有疑问 */
	{
		board[i][j].status = 4;
		board[i][j].check = 0;
		SetBkMode(hDC, TRANSPARENT);
		TextOut(hDC, i*(GRIDWIDTH+2)+startx+6, j*(GRIDWIDTH+2)+starty+2, TEXT("?"), 1);
		CountMine(1);
		return;
	}
	
	if (board[i][j].status == 4)    /* 解除？号 */
	{
		HPEN   hPen;
		HBRUSH hBrush;
		//		DWORD c = GetPixel(hDC, 50, 40);
		
		board[i][j].status = 1;
		hPen   = CreatePen(PS_SOLID, 1, 0xD8E9EC);
		hBrush = CreateSolidBrush(0xD8E9EC);
		hPen   = (HPEN)SelectObject(hDC, hPen);
		hBrush = (HBRUSH)SelectObject(hDC, hBrush);
		Rectangle(hDC, i*(GRIDWIDTH+2)+startx+4, j*(GRIDWIDTH+2)+starty+4, i*(GRIDWIDTH+2)+startx+1+GRIDWIDTH, j*(GRIDWIDTH+2)+starty+1+GRIDWIDTH);
		hPen   = (HPEN)SelectObject(hDC, hPen);
		hBrush = (HBRUSH)SelectObject(hDC, hBrush);
		DeleteObject(hPen);
		DeleteObject(hBrush);
		return;
	}
}

int Win(int gridnum)   /* 检测一下是否赢了 */
{
	int i, j;
	
	for (i = 0; i < gridnum; i++)
	{
		for (j = 0; j < gridnum; j++)
		{
			if (board[i][j].check == 0) 
				return 0;
		}
	}
	
	return 1;
}

void GameOver(int gridnum)  /* 游戏失败的处理 */
{
	int i, j;
	
	for (i = 0; i < gridnum; i++)
	{
		for (j = 0; j < gridnum; j++)
		{
			if ((board[i][j].mine == -1) && (board[i][j].status == 1))
            {                              /* 未标示为雷的方格显示雷 */
				board[i][j].status = 5;
			}
			else if ((board[i][j].mine != -1) && (board[i][j].status == 3))
			{                         /* 标错了的画 X */
				board[i][j].status = 6;
			}
		}
	}
}

void PlayerInit(void)
{
	int i, j;
	int m = 0, n = 0;
	FILE *fp;
	
	lstrcpy(player.name, TEXT("无名"));            /* 当前玩家缺省为noname */
	player.time[0] = player.time[1] = player.time[2] = 999;
	
	if ((fp = fopen("c:\\windows\\profile.epl", "rb")) != NULL)/*读已存的玩家*/
	{
		m = fread(hero, sizeof(hero[0]), 3, fp);
		n = fread(players, sizeof(player), 10, fp);
		
		for (i = 0; i < 3; i++)       /* 以下为验证读入数据的正确性和完整性 */
		{
			hero[i].name[10] = 0;
			if ((hero[i].time <= 0) || (hero[i].time > 999))
			{
				hero[i].time = 999;
			}
		}
		for (i = 0; i < 10; i++)
		{
			players[i].name[10] = 0;
			for (j = 0; j < 3; j++)
			{
				if ((players[i].time[j] <= 0) || (players[i].time[j] > 999))
				{
					players[i].time[j] = 999;
				}
			}
		}
		fclose(fp);
	}
	
	if ((m != 3) || (n != 10))   /* 读入错误, 就重写 */
	{
		for (i = 0; i < 3; i++)
		{
			lstrcpy(hero[i].name,TEXT("无名"));
			hero[i].time = 999;
		}
		for (i = 0; i < 10; i++)
		{
			players[i] = player;
		}
	}
}

void CountMine(int flag)/* 运算剩余雷数：flag=-1标了一个雷,flag=1标了一问号 */
{
	int temp;
	TCHAR mnum[8];
	
	if (flag == 1)
	{
		MineRestNum++;
	}
	else if (flag == -1)
	{
		MineRestNum--;
	}
	
	if (MineRestNum < 0)   /* 标的雷比实际雷多的情况 */
	{
		temp = 0;
	}
	else
	{
		temp = MineRestNum;
	}
	
	wsprintf(mnum, TEXT("%02d"), temp);
	SetWindowText(hwndmine, mnum);
}

void WriteDisk(void)        /* 游戏结束，数据写盘 */
{
	int i;
	FILE *fp;
	
	for (i = 0; i < 10; i++)
	{
		if (lstrcmp(players[i].name, TEXT("无名")) == 0)
		{
			players[i].time[0] = players[i].time[1]
				= players[i].time[2] = 999;
		}
	}
	
	if ((fp = fopen("c:\\windows\\profile.epl", "wb")) != NULL)
	{
		fwrite(hero, sizeof(struct HERO), 3, fp);
		fwrite(players, sizeof(struct USER), 10, fp);
		fclose(fp);
	}
}

LRESULT CALLBACK RecardProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
	}
	return FALSE;
}

void Record(int level)  /* 记录成绩 */
{
	int i;
	TCHAR  str[8];
	
	if (player.time[level] > gametime)    /* 记录最少的秒数 */
	{
		player.time[level] = gametime;
		for (i = 0; i < 10; i++)
		{
			if (lstrcmp(player.name, players[i].name) == 0)
			{
				players[i].time[level] = gametime;
				break;
			}
		}
		wsprintf(str, TEXT("%03d秒"), gametime);
		SetWindowText(hwndscore2, str);
	}
	
	if (hero[level].time > gametime)               /* 破纪录 */
	{
		hero[level].time = gametime;              /* 更新纪录 */
		lstrcpy(hero[level].name, player.name);
		DialogBox(hInst, (LPCTSTR)IDD_RECARD, hWND, (DLGPROC)RecardProc);
	}
}

LRESULT CALLBACK HeroProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int i;
	HDC hDC;
	TCHAR str[8];
	
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	case WM_PAINT:
		hDC = GetDC(hDlg);
		SetBkMode(hDC, TRANSPARENT);
		TextOut(hDC, 20, 10, TEXT("级别"), lstrlen(TEXT("级别")));
		TextOut(hDC, 100, 10, TEXT("玩家"), lstrlen(TEXT("玩家")));
		TextOut(hDC, 180, 10, TEXT("成绩"), lstrlen(TEXT("成绩")));
		for (i = 0; i < 3; i++)
		{
			TextOut(hDC, 20, 35+i*20, LevelStr[i], lstrlen(LevelStr[i]));
			TextOut(hDC, 90, 35+i*20, hero[i].name, lstrlen(hero[i].name));
			wsprintf(str, TEXT("%03d秒"), hero[i].time);
			TextOut(hDC, 175, 35+i*20, str, lstrlen(str));
		}
		ReleaseDC(hDlg, hDC);
		break;
	default:
		break;
	}
	return FALSE;
}

LRESULT CALLBACK EditProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_CHAR && (wParam == '\r' || wParam == 27/*ESC*/))
	{
		if (wParam == '\r')
			SendMessage(hWND, WM_COMMAND, EB_PRESSENTER<<16|2, lParam);
		else
			SendMessage(hWND, WM_COMMAND, EB_PRESSESC<<16|2, lParam);
		return TRUE;
	}
	return CallWindowProc(fnEditProc, hDlg, message, wParam, lParam);
}

void GameMenu(HWND hwnd, int x, int y)
{
	int i, j;
	HMENU hMenu, hPopMenu, hSubMenu;
	TCHAR ScoreStr[20];
	
	hMenu = CreatePopupMenu();
	
	hPopMenu = CreatePopupMenu();
	if (!lstrcmp(players[0].name, TEXT("无名")))
	{
		AppendMenu(hMenu, MF_POPUP|MF_GRAYED, (DWORD)hPopMenu, TEXT("选择玩家"));
	}
	else
	{
		AppendMenu(hMenu, MF_POPUP, (DWORD)hPopMenu, TEXT("选择玩家"));
		for (i = 0; i < 10; i++)
		{
			if (!lstrcmp(players[i].name, TEXT("无名")))
				break;
			AppendMenu(hPopMenu, MF_STRING, 400+i, players[i].name);
		}
	}
	
	hPopMenu = CreatePopupMenu();
	AppendMenu(hMenu, MF_POPUP, (DWORD)hPopMenu, TEXT("选择级别"));
	AppendMenu(hPopMenu, MF_STRING, 200+0, TEXT("低级"));
	AppendMenu(hPopMenu, MF_STRING, 200+1, TEXT("中级"));
	AppendMenu(hPopMenu, MF_STRING, 200+2, TEXT("高级"));
	
	hPopMenu = CreatePopupMenu();
	if (!lstrcmp(players[0].name, TEXT("无名")))
	{
		AppendMenu(hMenu, MF_POPUP|MF_GRAYED, (DWORD)hPopMenu, TEXT("成绩表"));
	}
	else
	{
		AppendMenu(hMenu, MF_POPUP, (DWORD)hPopMenu, TEXT("成绩表"));
		for (i = 0; i < 10; i++)
		{
			if (!lstrcmp(players[i].name, TEXT("无名")))
				break;
			hSubMenu = CreatePopupMenu();
			AppendMenu(hPopMenu, MF_POPUP, (DWORD)hSubMenu, players[i].name);
			for (j = 0; j < 3; j++)
			{
				memset(ScoreStr, 0, 20*sizeof(TCHAR));
				wsprintf(ScoreStr, "%s   ", LevelStr[j]);
				wsprintf(ScoreStr+lstrlen(ScoreStr), "%03d秒", players[i].time[j]);
				AppendMenu(hSubMenu, MF_STRING, 0, ScoreStr);
			}
		}
	}
	
	AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
	AppendMenu(hMenu, MF_STRING, 7, TEXT("英雄榜"));
	AppendMenu(hMenu, MF_STRING, 8, TEXT("重置"));
	AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
	AppendMenu(hMenu, MF_STRING, 9, TEXT("关于..."));
	
	TrackPopupMenu(hMenu, TPM_LEFTALIGN, x, y, 0, hwnd, NULL);
	
	for (i = 0; i < 2; i++)
	{
		hPopMenu = GetSubMenu(hMenu, i);
		DestroyMenu(hPopMenu);
	}
	hPopMenu = GetSubMenu(hMenu, 2);
	for (i = 0; i < 10; i++)
	{
		if (!lstrcmp(players[i].name, TEXT("无名")))
			break;
		hSubMenu = GetSubMenu(hPopMenu, i);
		DestroyMenu(hSubMenu);
	}
	DestroyMenu(hPopMenu);
	
	DestroyMenu(hMenu);	
}

void DisplayOperateResult(HDC hDC, int status, int boardX, int boardY, int i, int j)
{
	TCHAR str[14];
	
	SetBkMode(hDC, TRANSPARENT);
	switch (status)
	{
	case 0:
		DrawRedRect(hDC, boardX+2+i*(GRIDWIDTH+2), boardY+2+j*(GRIDWIDTH+2));
		Draw3DRect(hDC, boardX+2+i*(GRIDWIDTH+2), boardY+2+j*(GRIDWIDTH+2), GRIDWIDTH, GRIDWIDTH, FALSE);
		DrawMine(hDC, boardX+2+i*(GRIDWIDTH+2), boardY+2+j*(GRIDWIDTH+2));
		break;
	case 1:
		Draw3DRect(hDC, boardX+2+i*(GRIDWIDTH+2), boardY+2+j*(GRIDWIDTH+2), GRIDWIDTH, GRIDWIDTH, TRUE);
		break;
	case 2:
		Draw3DRect(hDC, boardX+2+i*(GRIDWIDTH+2), boardY+2+j*(GRIDWIDTH+2), GRIDWIDTH, GRIDWIDTH, FALSE);
		if (board[i][j].mine > 0)
		{
			str[0] = board[i][j].mine+48;
			str[1] = 0;
			TextOut(hDC, boardX+6+i*(GRIDWIDTH+2), boardY+3+j*(GRIDWIDTH+2), str, lstrlen(str));
		}
		break;
	case 3:
		Draw3DRect(hDC, boardX+2+i*(GRIDWIDTH+2), boardY+2+j*(GRIDWIDTH+2), GRIDWIDTH, GRIDWIDTH, TRUE);
		DrawRedFlag(hDC, boardX+2+i*(GRIDWIDTH+2), boardY+2+j*(GRIDWIDTH+2));
		break;
	case 4:
		Draw3DRect(hDC, boardX+2+i*(GRIDWIDTH+2), boardY+2+j*(GRIDWIDTH+2), GRIDWIDTH, GRIDWIDTH, TRUE);
		DrawRedFlag(hDC, boardX+2+i*(GRIDWIDTH+2), boardY+2+j*(GRIDWIDTH+2));
		TextOut(hDC, boardX+6+i*(GRIDWIDTH+2), boardY+3+j*(GRIDWIDTH+2), TEXT("?"), lstrlen(TEXT("?")));
		break;
	case 5:
		Draw3DRect(hDC, boardX+2+i*(GRIDWIDTH+2), boardY+2+j*(GRIDWIDTH+2), GRIDWIDTH, GRIDWIDTH, FALSE);
		DrawMine(hDC, boardX+2+i*(GRIDWIDTH+2), boardY+2+j*(GRIDWIDTH+2));
		break;
	case 6:
		Draw3DRect(hDC, boardX+2+i*(GRIDWIDTH+2), boardY+2+j*(GRIDWIDTH+2), GRIDWIDTH, GRIDWIDTH, TRUE);
		DrawRedFlag(hDC, boardX+2+i*(GRIDWIDTH+2), boardY+2+j*(GRIDWIDTH+2));
		TextOut(hDC, boardX+3+i*(GRIDWIDTH+2), boardY+3+j*(GRIDWIDTH+2), TEXT("×"), lstrlen(TEXT("×")));
		break;
	default:
		Draw3DRect(hDC, boardX+2+i*(GRIDWIDTH+2), boardY+2+j*(GRIDWIDTH+2), GRIDWIDTH, GRIDWIDTH, TRUE);
		break;
	}
}

void NameInputOver(void)
{
	int i;
	TCHAR str[12];

	GetWindowText(hwndedit, str, 12);
	if (*str & lstrcmp(player.name, str))
	{
		lstrcpy(player.name, str);
		for (i = 0; i < 10; i++)
		{
			if (!lstrcmp(players[i].name, str))
				break;
		}
		if (i == 10)                         /* 无同名 */
		{ 
			for (i = 9; i > 0; i--)          /* 去掉最后一个 */
			{
				players[i] = players[i-1];
			}
			/* 新名放入0号位置 */
			player.time[0] = player.time[1] = player.time[2] = 999;
			players[0] = player;
		}
		else                                 /* 同名视为查找 */
		{
			player     = players[i];
			players[i] = players[0];
			players[0] = player;
		}
		SendMessage(hWND, WM_COMMAND, 8, (LPARAM)hwndreset);
	}
	SetWindowText(hwndedit, player.name);
}

void SelectPlayer(void)
{
	int i;
	HMENU hPopMenu;
	POINT point = {14, 48};
	
	hPopMenu = CreatePopupMenu();
	if (lstrcmp(players[0].name, TEXT("无名")))
	{
		for (i = 0; i < 10; i++)
		{
			if (!lstrcmp(players[i].name, TEXT("无名")))
				break;
			AppendMenu(hPopMenu, MF_STRING, 400+i, players[i].name);
		}
		ClientToScreen(hWND, &point);
		TrackPopupMenu(hPopMenu, TPM_LEFTALIGN, point.x, point.y, 0, hWND, NULL);
	}
	DestroyMenu(hPopMenu);
}

