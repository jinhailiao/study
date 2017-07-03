//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			SGUI(Simple GUI)
// File:			sguiapp.h
// Description:		SGUI for Power user elect energy acquire system;unsupport multi_thread
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2010-01-18  Create                  Kingsea
//---------------------------------------------------------------------------------
#ifndef __SGUI_APP_H__
#define __SGUI_APP_H__
#include "haidef.h"
#include "sguiobj.h"
#include "sguiwnd.h"
#include "sguimsg.h"
#include "sguicart.h"

typedef stack< C_SGUIWND* > C_WNDSTACK;
typedef queue< S_GUIMSG > C_MSGQ;

class C_SGUIAPP:public C_SGUIOBJ
{
public:
	C_SGUIAPP(void):C_SGUIOBJ(C_SGUIOBJ::OBJ_T_APP){m_pCurWnd = NULL;m_pCurCaret = NULL;}
	virtual ~C_SGUIAPP(){}

public:
	static void SetCurApp(C_SGUIAPP *pGuiApp){m_pGuiApp = pGuiApp;}
	static C_SGUIAPP *GetCurApp(void){return m_pGuiApp;}

public:
	int SysInit(void);
	int CloseScreen(void);
	int OpenScreen(void);

	int SendGuiMsg(S_GUIMSG GuiMsg);
	int PostGuiMsg(S_GUIMSG GuiMsg);
	int DispatchGuiMsg(S_GUIMSG GuiMsg);
	void SetDesktopWnd(C_SGUIWND *pDesktopWnd);
	S_GUIMSG GetGuiMsg(void);

	int GotoWnd(C_SGUIWND *pWnd, S_DWORD UserData = 0);
	int CloseWnd(C_SGUIWND *pWnd);
	int PopWnd(void);

	int BeginDlg(C_SGUIWND *pCurDlg);
	int EndDlg(void);

	C_SGUICARET *GetAppCaret(void){return m_pCurCaret;}
	void SetAppCaret(C_SGUICARET *pCaret){m_pCurCaret = pCaret;}

	virtual int GuiAppRun(void);
	virtual int TranslateMsg(S_GUIMSG &msg);

protected:
	int EmptyMsgQ(void);
	int EmptyWndStack(void);

protected:
	static C_SGUIAPP *m_pGuiApp;
	
protected:
	C_SGUICARET *m_pCurCaret;
	C_SGUIWND *m_pCurDlg;
	C_SGUIWND *m_pCurWnd;
	C_WNDSTACK m_WndStack;
	C_MSGQ m_MsgQ;
};


#define SGuiApp	C_SGUIAPP::GetCurApp()
#define GOTOWND(WndT, UsrData)	 do{C_SGUIWND *pWnd = new WndT; SGuiApp->GotoWnd(pWnd, (S_DWORD)(UsrData));}while(0)
#define RETURNWND()	do{SGuiApp->CloseWnd(NULL);}while(0)

#endif //__SGUI_APP_H__

