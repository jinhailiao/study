//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			SGUI(Simple GUI)
// File:			sguictrl.h
// Description:		SGUI for Power user elect energy acquire system;unsupport multi_thread
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2010-06-06  Create                  Kingsea
//---------------------------------------------------------------------------------
#ifndef __SGUI_CTRL_H__
#define __SGUI_CTRL_H__
#include "sguiwnd.h"


class C_GUICTRL:public C_GUIWNDB
{
public:
	C_GUICTRL(void);
	virtual ~C_GUICTRL();

public:
	S_DWORD GetCtrlStyle(void){return m_flag;}
	S_DWORD GetCtrlID(void){return m_ID;}

protected:
	bool AddCtrl2ParentWnd(C_SGUIWND *pWnd);

	virtual int WndProcess(S_WORD msg, S_WORD wParam, S_DWORD lParam);
	virtual int DefWndProcess(S_WORD msg, S_WORD wParam, S_DWORD lParam);

protected:
	S_DWORD m_ID;
};

#define NUMEDIT_STYLE_DEC		0x00000000UL
#define NUMEDIT_STYLE_OCT		0x00000100UL
#define NUMEDIT_STYLE_HEX		0x00000200UL

class C_NUMEDIT:public C_GUICTRL
{
public:
	C_NUMEDIT(void);
	virtual ~C_NUMEDIT();

public:
	bool Create(const S_CHAR *pszCaption, S_DWORD dwStyle, const S_RECT& rect, C_SGUIWND* pParentWnd, S_DWORD nID); 

protected:
	virtual int WndProcess(S_WORD msg, S_WORD wParam, S_DWORD lParam);
	virtual int DefWndProcess(S_WORD msg, S_WORD wParam, S_DWORD lParam);

	bool IsOctChar(S_CHAR ch){return ch>='0'&&ch<='7';}
	bool IsDecChar(S_CHAR ch){return ch>='0'&&ch<='9';}
	bool IsHexChar(S_CHAR ch){return IsDecChar(ch) || ch>='A'&&ch<='F';}

	S_CHAR IncreaseChar(S_CHAR ch);
	S_CHAR DecreaseChar(S_CHAR ch);

protected:
	static const char m_SideW = 2;
	static const char m_SideH = 1;
};

struct S_VKBTN
{
	const S_CHAR *m_pName;
	const S_CHAR m_Val;
};
class C_VKBOARD:public C_GUICTRL
{
public:
	C_VKBOARD(void);
	virtual ~C_VKBOARD();

public:
	bool Create(S_DWORD dwStyle, const S_RECT& rect, C_SGUIWND* pParentWnd, S_DWORD nID); 

protected:
	virtual int WndProcess(S_WORD msg, S_WORD wParam, S_DWORD lParam);
	virtual int DefWndProcess(S_WORD msg, S_WORD wParam, S_DWORD lParam);

protected:
	virtual int GetHighlightRect(S_RECT &rect, const C_SGUIDC *pdc);
	virtual S_WORD GetHighlightChar(void);

protected:
	int m_RowCharCnt;
	int m_RowIdx;
	int m_ColIdx;

protected:
	static const int m_VkBoardW = 139;
	static const int m_VkBoardH = 64;
	static const S_CHAR *m_VkAsciiSet[5];
	static const S_VKBTN m_VkSpecialKey[5];
};

class C_TEXTEDIT:public C_GUICTRL
{
public:
	C_TEXTEDIT(void);
	virtual ~C_TEXTEDIT();

public:
	bool Create(const S_CHAR *pszCaption, S_DWORD dwStyle, const S_RECT& rect, C_SGUIWND* pParentWnd, S_DWORD nID); 

protected:
	virtual int WndProcess(S_WORD msg, S_WORD wParam, S_DWORD lParam);
	virtual int DefWndProcess(S_WORD msg, S_WORD wParam, S_DWORD lParam);

protected:
	int m_CaretIdx;
	
protected:
	static const char m_SideW = 2;
	static const char m_SideH = 1;
};

class C_BUTTON:public C_GUICTRL
{
public:
	C_BUTTON(void);
	virtual ~C_BUTTON();

public:
	bool Create(const S_CHAR *pszCaption, S_DWORD dwStyle, const S_RECT& rect, C_SGUIWND* pParentWnd, S_DWORD nID); 

protected:
	virtual int WndProcess(S_WORD msg, S_WORD wParam, S_DWORD lParam);
	virtual int DefWndProcess(S_WORD msg, S_WORD wParam, S_DWORD lParam);

protected:
	S_DWORD m_state;
};


#endif //__SGUI_CTRL_H__

