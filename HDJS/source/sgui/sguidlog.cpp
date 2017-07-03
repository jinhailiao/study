//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			SGUI(Simple GUI)
// File:			sguidlog.cpp
// Description:		SGUI for Power user elect energy acquire system;unsupport multi_thread
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2010-06-06  Create                  Kingsea
//---------------------------------------------------------------------------------
#include "sguiapp.h"
#include "sguidlog.h"
#include "sguictrl.h"

//
//
//
C_SGUIDLG::C_SGUIDLG(void)
{
	m_DlgRun = true;
}

C_SGUIDLG::~C_SGUIDLG()
{
}

int C_SGUIDLG::DoModel(void)
{
	int ok = MB_RTN_NULL;
	if (SGuiApp->BeginDlg(this) != 0)
		return MB_RTN_ERROR;

	C_SGUICARET *pCaret = SGuiApp->GetAppCaret();
	if (pCaret && pCaret->GetCaretStatus())
		pCaret->SendWndMsg(GM_PAINT, 0, 0);
	SGuiApp->SetAppCaret(NULL);

	SendWndMsg(GM_CREATE, 0x00, 0x00);
//	SendWndMsg(GM_PAINT, 0x00, 0x00);
	InvalidateRect(NULL);
	S_GUIMSG aMsg;

	while (m_DlgRun)
	{
		aMsg = SGuiApp->GetGuiMsg();
		SGuiApp->TranslateMsg(aMsg);
		ok = SGuiApp->DispatchGuiMsg(aMsg);
	}

	SGuiApp->EndDlg();
	SGuiApp->SetAppCaret(pCaret);
	return ok;
}

int C_SGUIDLG::EndDlg(void)
{
	SendWndMsg(GM_DESTROY, 0x00, 0x00);
	m_DlgRun = false;
	return 0;
}

int C_SGUIDLG::WndProcess(S_WORD msg, S_WORD wParam, S_DWORD lParam)
{
	return DefWndProcess(msg, wParam, lParam);
}

int C_SGUIDLG::DefWndProcess(S_WORD msg, S_WORD wParam, S_DWORD lParam)
{
	return C_SGUIWND::DefWndProcess(msg, wParam, lParam);
}


//
//
//
C_MSGBOX::C_MSGBOX(const S_CHAR *ptitle, const S_CHAR *ptext, S_DWORD dwFlag)
{
	m_Title = ptitle;
	m_WndText = ptext;
	m_pParent = NULL;
	m_flag = dwFlag | WNDF_VISIBLE;
	m_DelayMS = 0;
}

C_MSGBOX::~C_MSGBOX()
{
}

int C_MSGBOX::WndProcess(S_WORD msg, S_WORD wParam, S_DWORD lParam)
{
	switch (msg)
	{
	case GM_CREATE:
		m_WndRect.x = 20;
		m_WndRect.w = 120;
		if ((GetWndLong()&MB_BTN_MASK) == MB_BTN_NULL)
			m_WndRect.y = 60, m_WndRect.h = 60;
		else
			m_WndRect.y = 50, m_WndRect.h = 80;
		CreateButton();
		break;
	case GM_PAINT:{
		C_SGUIDC *pdc = BeginPaint();
		S_RECT WndRect = {0, 0, m_WndRect.w, m_WndRect.h};
		pdc->DrawRect(WndRect);
		pdc->DrawHLine(1, 16, m_WndRect.w-2);
		DrawTitle(pdc);
		DrawText(pdc);
		EndPaint(pdc);
		}	
		break;
	case GM_KEYUP:
		if ((GetWndLong()&MB_BTN_MASK) == MB_BTN_NULL)
		{
			EndDlg();
			return MB_RTN_NULL;
		}
		else if ((GetWndLong()&MB_BTN_MASK) == MB_BTN_OKCANCEL)
		{
			if (wParam == GVK_LEFT)
				SetFocusCtrl(GetWndCtrl(Btn_OK_ID));
			else if (wParam == GVK_RIGHT)
				SetFocusCtrl(GetWndCtrl(Btn_Cancel_ID));
		}
		break;
	case GM_CMD:
		if (wParam == Btn_OK_ID)//OK
		{
			EndDlg();
			return MB_RTN_YESOK;
		}
		else if (wParam == Btn_Cancel_ID)//Cancel
		{
			EndDlg();
			return MB_RTN_CANCEL;
		}
		break;
	case GM_TIME:
		m_DelayMS += GM_TIME_INTERVAL_MS;
		if (m_DelayMS > MSGBOX_DELAY_MAXMS)
		{
			EndDlg();
			return MB_RTN_NULL;
		}
		break;
	case GM_DESTROY:
		DestroyButton();
		break;
	default:
		return DefWndProcess(msg, wParam, lParam);
	}
	return 0;
}

int C_MSGBOX::DefWndProcess(S_WORD msg, S_WORD wParam, S_DWORD lParam)
{
	return C_SGUIDLG::DefWndProcess(msg, wParam, lParam);
}

int C_MSGBOX::DrawTitle(C_SGUIDC *pdc)
{
	int TitleX = 2;
	if ((GetWndLong()&MB_TITLE_MASK) == MB_TITLE_CENTER)
	{
		int TitleW = pdc->GetStringExtent(m_Title.c_str());
		if (TitleW < m_WndRect.w) TitleX = (m_WndRect.w - TitleW) / 2;
	}
	return pdc->DrawString(TitleX, 2, m_Title.c_str());
}

int C_MSGBOX::DrawText(C_SGUIDC *pdc)
{
	int TextX = 2;
	if ((GetWndLong()&MB_TEXT_MASK) == MB_TEXT_CENTER)
	{
		int TextW = pdc->GetStringExtent(m_WndText.c_str());
		if (TextW < m_WndRect.w) TextX = (m_WndRect.w - TextW) / 2;
	}
	return pdc->DrawString(TextX, 30, m_WndText.c_str());
}

int C_MSGBOX::CreateButton(void)
{
	if ((GetWndLong()&MB_BTN_MASK) == MB_BTN_OK)
	{
		C_BUTTON *pbtnOK = new C_BUTTON;
		S_RECT rect;
		rect.w = 32; rect.h = 16;
		rect.x = (m_WndRect.w - rect.w) / 2;
		rect.y = 60;
		pbtnOK->Create("确定", 0x00, rect, this, Btn_OK_ID);
		SetFocusCtrl(pbtnOK);
	}
	else if ((GetWndLong()&MB_BTN_MASK) == MB_BTN_OKCANCEL)
	{
		C_BUTTON *pbtnOK = new C_BUTTON;
		C_BUTTON *pbtnCancel = new C_BUTTON;
		S_RECT rect;
		rect.w = 32; rect.h = 16;
		rect.x = (m_WndRect.w - (rect.w + rect.w + 10)) / 2;
		rect.y = 60;
		pbtnOK->Create("确定", 0x00, rect, this, Btn_OK_ID);
		rect.x += rect.w + 10;
		pbtnCancel->Create("取消", 0x00, rect, this, Btn_Cancel_ID);
		if ((GetWndLong()&MB_BTN_FOCUS_MASK) == MB_BTN_FOCUS_MASK)
			SetFocusCtrl(pbtnCancel);
		else
			SetFocusCtrl(pbtnOK);
	}
	return 0;
}

int C_MSGBOX::DestroyButton(void)
{
	if ((GetWndLong()&MB_BTN_MASK) == MB_BTN_OK)
	{
		delete (GetWndCtrl(Btn_OK_ID));
	}
	else if ((GetWndLong()&MB_BTN_MASK) == MB_BTN_OKCANCEL)
	{
		delete (GetWndCtrl(Btn_OK_ID));
		delete (GetWndCtrl(Btn_Cancel_ID));
	}
	return 0;
}


int SGui_MsgBox(const S_CHAR *ptitle, const S_CHAR *ptext, S_DWORD dwFlag)
{
	C_MSGBOX MsgBox(ptitle, ptext, dwFlag);
	return MsgBox.DoModel();
}



