//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			SGUI(Simple GUI)
// File:			sguiwnd.h
// Description:		SGUI for Power user elect energy acquire system;unsupport multi_thread
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2010-01-18  Create                  Kingsea
//---------------------------------------------------------------------------------
#ifndef __SGUI_WND_H__
#define __SGUI_WND_H__
#include "haidef.h"
#include "sguiobj.h"
#include "sguimsg.h"
#include "sguidc.h"

#define WNDF_VISIBLE		0x00000001UL
#define WNDF_DISABLED		0x00000002UL
#define WNDF_FOCUS			0x00000004UL

class C_WNDBASE:public C_SGUIOBJ
{
public:
	C_WNDBASE(void);
	virtual ~C_WNDBASE();

public:
	int SendWndMsg(S_WORD msg, S_WORD wParam, S_DWORD lParam);
	S_RECT GetWndRect(void){return m_WndRect;}
	void SetWndRect(const S_RECT &rRect){m_WndRect = rRect;}
	S_DWORD SetWndLong(S_DWORD flag){S_DWORD old = m_flag; m_flag = flag; return old;}
	S_DWORD GetWndLong(void){return m_flag;}

	void ShowWnd(bool bShow = true);
	void EnableWnd(bool bEnable = true);

	bool InvalidRect(void){return m_InvalidRect.w && m_InvalidRect.h;}
	int ClientToScreen(S_RECT &rect);
	void UpdateWnd(void);

protected:
	virtual int WndProcess(S_WORD msg, S_WORD wParam, S_DWORD lParam);
	virtual int DefWndProcess(S_WORD msg, S_WORD wParam, S_DWORD lParam);

protected:
	S_DWORD m_flag;
	S_RECT m_WndRect;
	S_RECT m_InvalidRect;
	C_WNDBASE *m_pParent;
};

class C_SGUICARET;
class C_GUIWNDB:public C_WNDBASE
{
public:
	C_GUIWNDB(void);
	virtual ~C_GUIWNDB();

public:
	int SetWndText(const string &rWndText){m_WndText = rWndText;return 0;}
	const string &GetWndText(void){return m_WndText;}

	int CreateCaret(S_WORD w, S_WORD h);
	int SetCaretPos(S_WORD x, S_WORD y);
	int GetCaretPos(S_WORD &x, S_WORD &y);
	int ShowCaret(void);
	int HideCaret(void);

protected:
	virtual int WndProcess(S_WORD msg, S_WORD wParam, S_DWORD lParam);
	virtual int DefWndProcess(S_WORD msg, S_WORD wParam, S_DWORD lParam);

protected:
	string m_WndText;
	C_SGUICARET *m_pCurCaret;
};

class C_GUICTRL;
class C_SGUIWND:public C_GUIWNDB
{
public:
	C_SGUIWND(void);
	virtual ~C_SGUIWND();

	C_SGUIDC *BeginPaint(void);
	void EndPaint(C_SGUIDC *pDC);

	void *GetWndData(int size);
	void ReleaseWndData(void);

	int InvalidateRect(const S_RECT *pRect);
	C_WNDBASE *GetFocusCtrl(void);
	C_WNDBASE *SetFocusCtrl(C_GUICTRL *pFocusCtrl);
	C_GUICTRL *GetWndCtrl(int nID);

	bool AddControl(C_GUICTRL *pCtrl){m_CtrlQ.push_back(pCtrl);return true;}

protected:
	virtual int WndProcess(S_WORD msg, S_WORD wParam, S_DWORD lParam);
	virtual int DefWndProcess(S_WORD msg, S_WORD wParam, S_DWORD lParam);

protected:
	char *m_pWndData;
	vector<C_GUICTRL*> m_CtrlQ;
};

#endif //__SGUI_WND_H__

