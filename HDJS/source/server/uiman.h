//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			HDJS
// File:			uiman.h
// Description:		UI manager
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2010-01-18  Create                  Kingsea
//---------------------------------------------------------------------------------
#ifndef __SERVERUIMAN_H__
#define __SERVERUIMAN_H__

#include "server.h"
#include "sguiwnd.h"
#include "sguiapp.h"
#include "sguibase.h"

class C_UIMAPP:public C_SGUIAPP
{
public:
	virtual int TranslateMsg(S_GUIMSG &msg);

	void OpenBacklight(void){SGUI_backlight(1);}
	void CloseBacklight(void){SGUI_backlight(0);}
};

class C_UIMWND:public C_SGUIWND
{
public:
	C_UIMWND(void){m_WndRect.y=16;m_WndRect.h-=16;};
	virtual int DefWndProcess(S_WORD msg, S_WORD wParam, S_DWORD lParam);
};

class C_DESKTOP:public C_UIMWND
{
public:
	virtual int WndProcess(S_WORD msg, S_WORD wParam, S_DWORD lParam);
};

class C_MENUWND:public C_UIMWND
{
public:
	virtual int WndProcess(S_WORD msg, S_WORD wParam, S_DWORD lParam);
};

class C_CTRLWND:public C_UIMWND
{
public:
	virtual int WndProcess(S_WORD msg, S_WORD wParam, S_DWORD lParam);
};

class C_VKWND:public C_UIMWND
{
public:
	virtual int WndProcess(S_WORD msg, S_WORD wParam, S_DWORD lParam);
};

class C_MSGBOXWND:public C_UIMWND
{
public:
	virtual int WndProcess(S_WORD msg, S_WORD wParam, S_DWORD lParam);
};

class C_MSGSENDWND:public C_UIMWND
{
public:
	virtual int WndProcess(S_WORD msg, S_WORD wParam, S_DWORD lParam);
};

class C_UIMAN: public C_SERVER
{
public:
	C_UIMAN(void);
	~C_UIMAN();

public:
	int BeforeService(void);
	int Servicing(void);
	int AfterService(void);
};



#endif //__SERVERUIMAN_H__

