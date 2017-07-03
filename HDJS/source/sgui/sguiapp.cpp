//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			SGUI(Simple GUI)
// File:			sguiapp.cpp
// Description:		SGUI for Power user elect energy acquire system;unsupport multi_thread
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2010-01-18  Create                  Kingsea
//---------------------------------------------------------------------------------
#include "sguiapp.h"
#include "sguibase.h"
#include <time.h>

C_SGUIAPP *C_SGUIAPP::m_pGuiApp = NULL;

int C_SGUIAPP::SysInit(void)
{
	if (SGUI_LcdInit() != 0)
		return -1;
	if (SGUI_KeyboardInit() != 0)
		return -1;

	return 0;
}

int C_SGUIAPP::CloseScreen(void)
{
	SGUI_CloseScreen();
	return 0;
}

int C_SGUIAPP::OpenScreen(void)
{
	SGUI_OpenScreen();
	return 0;
}

void C_SGUIAPP::SetDesktopWnd(C_SGUIWND *pDesktopWnd)
{
	m_pCurWnd = pDesktopWnd;

 	/** push GM_CREATE msg
	 */
	S_GUIMSG aMsg;
	aMsg.pWnd = m_pCurWnd;
	aMsg.Msg = GM_CREATE;
	aMsg.wParam = 0;
	aMsg.lParam = 0;
	m_MsgQ.push(aMsg);
	/** push GM_SYSPAINT msg
	 */
	aMsg.pWnd = NULL;
	aMsg.Msg = GM_SYSPAINT;
	aMsg.wParam = 0;
	aMsg.lParam = 0;
	m_MsgQ.push(aMsg);
	/** push GM_PAINT msg
	 */
	pDesktopWnd->InvalidateRect(NULL);
}

int C_SGUIAPP::GuiAppRun(void)
{
	S_GUIMSG aMsg;

	while (true)
	{
		aMsg = GetGuiMsg();
		TranslateMsg(aMsg);
		DispatchGuiMsg(aMsg);
	}

	return 0;
}

S_GUIMSG C_SGUIAPP::GetGuiMsg(void)
{
	static time_t SysTick = time(NULL);
	time_t CurTime;

	S_GUIMSG aMsg;
	
	while (m_MsgQ.empty())
	{
		/**poll key
		 */
		aMsg = SGUI_PollMsg();
		if (aMsg.Msg != GM_UNKNOW)
		{
			m_MsgQ.push(aMsg);
			break;
		}
		/** time tick message
		 */
		CurTime = time(NULL);
		if (CurTime != SysTick)
		{
			SysTick = CurTime;
			if (m_pCurDlg != NULL) aMsg.pWnd = m_pCurDlg;
			else aMsg.pWnd = m_pCurWnd;
			aMsg.Msg = GM_TIME;
			aMsg.wParam = 0;
			aMsg.lParam = 0;
			m_MsgQ.push(aMsg);
			break;
		}
		SGUI_SleepMS(10);
	}

	aMsg = m_MsgQ.front();
	m_MsgQ.pop();

	return aMsg;
}

int C_SGUIAPP::SendGuiMsg(S_GUIMSG GuiMsg)
{
	if (GuiMsg.pWnd == NULL)
		return -1;

	return GuiMsg.pWnd->SendWndMsg(GuiMsg.Msg, GuiMsg.wParam, GuiMsg.lParam);
}

int C_SGUIAPP::PostGuiMsg(S_GUIMSG GuiMsg)
{
	m_MsgQ.push(GuiMsg);

	return 0;
}

int C_SGUIAPP::TranslateMsg(S_GUIMSG &msg)
{
	switch(msg.Msg)
	{
	case GM_TIME:
		if (m_pCurCaret != NULL)
			m_pCurCaret->SendWndMsg(msg.Msg, msg.wParam, msg.lParam);
		break;
	default:
		break;
	}
	return 0;
}

int C_SGUIAPP::DispatchGuiMsg(S_GUIMSG GuiMsg)
{
	C_WNDBASE *pWnd;

	if (GuiMsg.Msg == GM_UNKNOW)
		return -1;

	if (GuiMsg.pWnd != NULL)
	{
		return GuiMsg.pWnd->SendWndMsg(GuiMsg.Msg, GuiMsg.wParam, GuiMsg.lParam);
	}
	else if (m_pCurDlg != NULL)
	{
		if ((pWnd = m_pCurDlg->GetFocusCtrl()) != NULL)
			return pWnd->SendWndMsg(GuiMsg.Msg, GuiMsg.wParam, GuiMsg.lParam);
		else
			return SGuiApp->m_pCurDlg->SendWndMsg(GuiMsg.Msg, GuiMsg.wParam, GuiMsg.lParam);
	}
	else if (m_pCurWnd != NULL)
	{
		if ((pWnd = m_pCurWnd->GetFocusCtrl()) != NULL)
			return pWnd->SendWndMsg(GuiMsg.Msg, GuiMsg.wParam, GuiMsg.lParam);
		else
			return SGuiApp->m_pCurWnd->SendWndMsg(GuiMsg.Msg, GuiMsg.wParam, GuiMsg.lParam);
	}

	return -1;
}

int C_SGUIAPP::GotoWnd(C_SGUIWND *pWnd, S_DWORD UserData)
{
	/** Hide caret
	 */
	SGuiApp->SetAppCaret(NULL);

	m_WndStack.push(m_pCurWnd);//push current wnd
	m_pCurWnd = pWnd;

	EmptyMsgQ();
	/** push GM_CREATE msg
	 */
	S_GUIMSG aMsg;
	aMsg.pWnd = m_pCurWnd;
	aMsg.Msg = GM_CREATE;
	aMsg.wParam = 0;
	aMsg.lParam = UserData;
	m_MsgQ.push(aMsg);
	/** push GM_PAINT msg
	 */
	m_pCurWnd->InvalidateRect(NULL);

	return 0;
}

int C_SGUIAPP::CloseWnd(C_SGUIWND *pWnd)
{
	C_SGUIWND *pwnd = (pWnd? pWnd:m_pCurWnd);
	EmptyMsgQ();
	/** push GM_DESTROY msg
	 */
	S_GUIMSG aMsg;
	aMsg.pWnd = pwnd;
	aMsg.Msg = GM_DESTROY;
	aMsg.wParam = 0;
	aMsg.lParam = 0;
	m_MsgQ.push(aMsg);
	/** push GM_QUIT msg
	 */
	aMsg.pWnd = pwnd;
	aMsg.Msg = GM_QUIT;
	aMsg.wParam = 0;
	aMsg.lParam = 0;
	m_MsgQ.push(aMsg);
	return 0;
}

int C_SGUIAPP::PopWnd(void)
{
	if (m_WndStack.empty())
		return -1;

	delete m_pCurWnd;
	m_pCurWnd = m_WndStack.top();
	m_WndStack.pop();

	EmptyMsgQ();
	/** push GM_RESTORE msg
	 */
	S_GUIMSG aMsg;
	aMsg.pWnd = m_pCurWnd;
	aMsg.Msg = GM_RESTORE;
	aMsg.wParam = 0;
	aMsg.lParam = 0;
	m_MsgQ.push(aMsg);
	/** push GM_PAINT msg
	 */
	m_pCurWnd->InvalidateRect(NULL);

	/** Restore focus
	 */
	SGuiApp->SetAppCaret(NULL);
	aMsg.pWnd = m_pCurWnd->GetFocusCtrl();
	if (aMsg.pWnd == NULL)
        aMsg.pWnd = m_pCurWnd;
	aMsg.Msg = GM_SETFOCUS;
	aMsg.wParam = 0;
	aMsg.lParam = 0;
	m_MsgQ.push(aMsg);

	return 0;
}

int C_SGUIAPP::BeginDlg(C_SGUIWND *pCurDlg)
{
	if (m_pCurDlg != NULL)
		return -1;
	EmptyMsgQ();
	m_pCurDlg = pCurDlg;
	return 0;
}

int C_SGUIAPP::EndDlg(void)
{
	m_pCurDlg = NULL;

	EmptyMsgQ();
	/** push GM_RESTORE msg
	 */
	S_GUIMSG aMsg;
	aMsg.pWnd = m_pCurWnd;
	aMsg.Msg = GM_RESTORE;
	aMsg.wParam = 0;
	aMsg.lParam = 0;
	m_MsgQ.push(aMsg);
	/** push GM_PAINT msg
	 */
	m_pCurWnd->InvalidateRect(NULL);
	return 0;
}


int C_SGUIAPP::EmptyMsgQ(void)
{
	while (!m_MsgQ.empty())
		m_MsgQ.pop();
	return 0;
}

int C_SGUIAPP::EmptyWndStack(void)
{
	S_GUIMSG aMsg;

	if (m_pCurDlg != NULL)//wait Msgbox close
		return -1;
	if (m_WndStack.empty())
		return -1;

	//Query wnd auto back
	aMsg.pWnd = m_pCurWnd;
	aMsg.Msg = GM_AUTOBACK;
	if (SendGuiMsg(aMsg) != 0)//current wnd is not auto back still
		return -1;

	//clean msgQ
	EmptyMsgQ();
	
	while (!m_WndStack.empty())
	{
		/** restore user data first
		 */
		aMsg.pWnd = m_pCurWnd;
		aMsg.Msg = GM_RESTORE;
		SendGuiMsg(aMsg);
		/** destroy wnd
		 */
		aMsg.pWnd = m_pCurWnd;
		aMsg.Msg = GM_DESTROY;
		SendGuiMsg(aMsg);

		delete m_pCurWnd;
		m_pCurWnd = m_WndStack.top();
		m_WndStack.pop();
	}
	/** restore user data first
	 */
	aMsg.pWnd = m_pCurWnd;
	aMsg.Msg = GM_RESTORE;
	SendGuiMsg(aMsg);
	/** paint wnd
	 */
	m_pCurWnd->InvalidateRect(NULL);
	/** restore caret
	 */
	SGuiApp->SetAppCaret(NULL);
	aMsg.pWnd = m_pCurWnd->GetFocusCtrl();
	if (aMsg.pWnd == NULL)
		aMsg.pWnd = m_pCurWnd;
	aMsg.Msg = GM_SETFOCUS;
	aMsg.wParam = 0;
	aMsg.lParam = 0;
	m_MsgQ.push(aMsg);

	return 0;
}


