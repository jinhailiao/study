//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			SGUI(Simple GUI)
// File:			sguidlog.h
// Description:		SGUI for Power user elect energy acquire system;unsupport multi_thread
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2010-06-06  Create                  Kingsea
//---------------------------------------------------------------------------------
#ifndef __SGUI_DLOG_H__
#define __SGUI_DLOG_H__
#include "sguiwnd.h"

class C_SGUIDLG:public C_SGUIWND
{
public:
	C_SGUIDLG(void);
	virtual ~C_SGUIDLG();

public:
	int DoModel(void);

protected:
	virtual int WndProcess(S_WORD msg, S_WORD wParam, S_DWORD lParam);
	virtual int DefWndProcess(S_WORD msg, S_WORD wParam, S_DWORD lParam);

	int EndDlg(void);

private:
	bool m_DlgRun;
};

#define MB_TITLE_MASK			0x80000000UL
#define MB_TITLE_CENTER		0x00000000UL
#define MB_TITLE_LEFT			0x80000000UL

#define MB_TEXT_MASK			0x40000000UL
#define MB_TEXT_CENTER			0x40000000UL
#define MB_TEXT_LEFT			0x00000000UL

#define MB_BTN_MASK			0x03000000UL
#define MB_BTN_NULL				0x00000000UL
#define MB_BTN_OK				0x02000000UL
#define MB_BTN_OKCANCEL		0x03000000UL

#define MB_BTN_FOCUS_MASK		0x08000000UL
#define MB_BTN_FOCUS_YESOK	0x00000000UL
#define MB_BTN_FOCUS_NOCAN	0x08000000UL

#define MB_RTN_NULL			0
#define MB_RTN_YESOK			1
#define MB_RTN_NO				2
#define MB_RTN_CANCEL			MB_RTN_NO
#define MB_RTN_ERROR			-1

#define MSGBOX_DELAY_MAXMS	(30*1000)

class C_MSGBOX:public C_SGUIDLG
{
public:
	C_MSGBOX(const S_CHAR *ptitle, const S_CHAR *ptext, S_DWORD dwFlag);
	virtual ~C_MSGBOX();

protected:
	virtual int WndProcess(S_WORD msg, S_WORD wParam, S_DWORD lParam);
	virtual int DefWndProcess(S_WORD msg, S_WORD wParam, S_DWORD lParam);

protected:
	int DrawTitle(C_SGUIDC *pdc);
	int DrawText(C_SGUIDC *pdc);
	int CreateButton(void);
	int DestroyButton(void);
	
protected:
	string m_Title;
	S_DWORD m_DelayMS;
	static const S_DWORD Btn_OK_ID = 1;
	static const S_DWORD Btn_Cancel_ID = 2;
};

int SGui_MsgBox(const S_CHAR *ptitle, const S_CHAR *ptext, S_DWORD dwFlag);


#endif //__SGUI_DLOG_H__

