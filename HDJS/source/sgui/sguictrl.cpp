//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			SGUI(Simple GUI)
// File:			sguictrl.cpp
// Description:		SGUI for Power user elect energy acquire system;unsupport multi_thread
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2010-06-06  Create                  Kingsea
//---------------------------------------------------------------------------------
#include "sguictrl.h"

//
//
//
C_GUICTRL::C_GUICTRL(void)
{
}

C_GUICTRL::~C_GUICTRL()
{
}

bool C_GUICTRL::AddCtrl2ParentWnd(C_SGUIWND *pWnd)
{
	if (pWnd == NULL)
		return false;
	return pWnd->AddControl(this);
}

int C_GUICTRL::WndProcess(S_WORD msg, S_WORD wParam, S_DWORD lParam)
{
	return C_GUIWNDB::WndProcess(msg, wParam, lParam);
}

int C_GUICTRL::DefWndProcess(S_WORD msg, S_WORD wParam, S_DWORD lParam)
{
	return C_GUIWNDB::DefWndProcess(msg, wParam, lParam);
}

//
//
//
C_NUMEDIT::C_NUMEDIT(void)
{
}
C_NUMEDIT::~C_NUMEDIT()
{
}

bool C_NUMEDIT::Create(const S_CHAR *pszCaption, S_DWORD dwStyle, const S_RECT& rect, C_SGUIWND* pParentWnd, S_DWORD nID)
{
	if (AddCtrl2ParentWnd(pParentWnd) == false)
		return false;

	m_WndText = pszCaption;
	m_WndRect = rect;
	m_pParent = pParentWnd;
	m_flag = dwStyle | WNDF_VISIBLE;
	m_ID = nID;

	for (size_t i = 0; i < m_WndText.size(); i++)
	{
		if (m_WndText[i] >= 'a' && m_WndText[i] <= 'f')
			m_WndText[i] -= 'a' - 'A';
		if ((GetCtrlStyle()&NUMEDIT_STYLE_OCT)==NUMEDIT_STYLE_OCT)
		{
			if (!IsOctChar(m_WndText[i])) m_WndText[i] = '0';
		}
		else if ((GetCtrlStyle()&NUMEDIT_STYLE_HEX)==NUMEDIT_STYLE_HEX)
		{
			if (!IsHexChar(m_WndText[i])) m_WndText[i] = '0';
		}
		else
		{
			if (!IsDecChar(m_WndText[i])) m_WndText[i] = '0';
		}
	}
	if (m_WndText.empty())
		m_WndText.push_back('0');

	SendWndMsg(GM_CREATE, 0x00, 0x00);
	return true;
}

int C_NUMEDIT::WndProcess(S_WORD msg, S_WORD wParam, S_DWORD lParam)
{
	switch (msg)
	{
	case GM_CREATE:{
		C_SGUIDC *pdc = new C_SGUIDC(this);
		CreateCaret(pdc->GetFontWidth('0'), 1);
		SetCaretPos(m_SideW+0*pdc->GetFontWidth('0'), m_SideH+pdc->GetFontHeight('0'));
		ShowCaret();
		delete pdc;
		}
		break;
	case GM_PAINT:{
		C_SGUIDC *pdc = new C_SGUIDC(this);
		S_RECT WndRect = {0, 0, m_WndRect.w, m_WndRect.h};
		pdc->DrawRect(WndRect);
		pdc->DrawString(m_SideW, m_SideH, m_WndText.c_str());
		delete pdc;
		}
		break;
	case GM_KEYUP:{
		S_WORD x, y;
		C_SGUIDC *pdc = new C_SGUIDC(this);
		GetCaretPos(x, y);
		x = (x - 2) / pdc->GetFontWidth('0');
		if (wParam == GVK_UP)
		{
			m_WndText[x] = IncreaseChar(m_WndText[x]);
			pdc->DrawString(m_SideW, m_SideH, m_WndText.c_str());
		}
		else if(wParam == GVK_DN)
		{
			m_WndText[x] = DecreaseChar(m_WndText[x]);
			pdc->DrawString(m_SideW, m_SideH, m_WndText.c_str());
		}
		else if (wParam == GVK_LEFT)
		{
			if (x == 0)
				m_pParent->SendWndMsg(GM_KEYUP, wParam, lParam);
			else
				SetCaretPos(m_SideW + (x-1) * pdc->GetFontWidth('0'), y);
		}
		else if (wParam == GVK_RIGHT)
		{
			if (x == m_WndText.size() - 1)
				m_pParent->SendWndMsg(GM_KEYUP, wParam, lParam);
			else
				SetCaretPos(m_SideW + (x+1) * pdc->GetFontWidth('0'), y);
		}
		else
		{
			m_pParent->SendWndMsg(GM_KEYUP, wParam, lParam);
		}
		delete pdc;}
		break;
	default:
		return C_GUICTRL::WndProcess(msg, wParam, lParam);
	}
	return 0;
}

int C_NUMEDIT::DefWndProcess(S_WORD msg, S_WORD wParam, S_DWORD lParam)
{
	return C_GUICTRL::DefWndProcess(msg, wParam, lParam);
}

S_CHAR C_NUMEDIT::IncreaseChar(S_CHAR ch)
{
	if ((GetCtrlStyle()&NUMEDIT_STYLE_OCT)==NUMEDIT_STYLE_OCT)
	{
		return (ch >= '7')? '0':(ch+1);
	}
	else if ((GetCtrlStyle()&NUMEDIT_STYLE_HEX)==NUMEDIT_STYLE_HEX)
	{
		if (ch == '9') return 'A';
		else if (ch == 'F' || ch == 'f') return '0';
		else return ch+1;
	}

	return (ch >= '9')? '0':(ch+1);
}

S_CHAR C_NUMEDIT::DecreaseChar(S_CHAR ch)
{
	if ((GetCtrlStyle()&NUMEDIT_STYLE_OCT)==NUMEDIT_STYLE_OCT)
	{
		return (ch == '0')? '7':(ch-1);
	}
	else if ((GetCtrlStyle()&NUMEDIT_STYLE_HEX)==NUMEDIT_STYLE_HEX)
	{
		if (ch == '0') return 'F';
		else if (ch == 'A' || ch == 'a') return '9';
		else return ch-1;
	}

	return (ch == '0')? '9':(ch-1);
}


//
//
//
const S_CHAR *C_VKBOARD::m_VkAsciiSet[5] =
{
	"ABCDEFGHIJKLMNOPQRS",
	"TUVWXYZabcdefghijkl",
	"mnopqrstuvwxyz@#$%&",
	"1234567890 <>[](){}",
	"*^~_+-=|!:;,.?'\"\\ /"
};
const S_VKBTN C_VKBOARD::m_VkSpecialKey[5] =
{
	{"ESC", GVK_ESC}, {"Del", GVK_BACKSPACE}, {"Ent", GVK_ENTER}, {"Tab", GVK_TAB}, {"F1 ", GVK_F1}
};

C_VKBOARD::C_VKBOARD(void)
{
}

C_VKBOARD::~C_VKBOARD()
{
}

bool C_VKBOARD::Create(S_DWORD dwStyle, const S_RECT& rect, C_SGUIWND* pParentWnd, S_DWORD nID)
{
	if (AddCtrl2ParentWnd(pParentWnd) == false)
		return false;

	m_WndRect = rect;
	m_WndRect.w = m_VkBoardW;
	m_WndRect.h = m_VkBoardH;
	m_pParent = pParentWnd;
	m_flag = dwStyle | WNDF_VISIBLE;
	m_ID = nID;

	SendWndMsg(GM_CREATE, 0x00, 0x00);
	return true;
}

int C_VKBOARD::WndProcess(S_WORD msg, S_WORD wParam, S_DWORD lParam)
{
	switch (msg)
	{
	case GM_CREATE:
		m_RowIdx = 0;
		m_ColIdx = 0;
		m_RowCharCnt = (int)strlen(m_VkAsciiSet[0]);
		break;
	case GM_PAINT:{
		C_SGUIDC *pdc = new C_SGUIDC(this);
		S_RECT WndRect = {0, 0, m_WndRect.w, m_WndRect.h};
		pdc->DrawRect(WndRect);
		for (int i = 0; i < 5; i++)
			pdc->DrawString(2, 2+i*pdc->GetFontHeight('0'), m_VkAsciiSet[i]);
		for (int i = 0; i < 5; i++)
			pdc->DrawString(2+3+m_RowCharCnt*pdc->GetFontWidth('0'), 2+i*pdc->GetFontHeight('0'), m_VkSpecialKey[i].m_pName);
		pdc->DrawVLine(2+1+m_RowCharCnt*pdc->GetFontWidth('0'), 1+0*pdc->GetFontHeight('0'), 3+5*pdc->GetFontHeight('0'));
		for (int i = 1; i < 5; i++)
			pdc->DrawHLine(1+3+m_RowCharCnt*pdc->GetFontWidth('0'), 2+i*pdc->GetFontHeight('0'), 2+3*pdc->GetFontWidth('0'));
		GetHighlightRect(WndRect, pdc);
		pdc->ReverseRect(WndRect);
		delete pdc;
		}
		break;
	case GM_KEYUP:
		if (wParam == GVK_DN || wParam == GVK_UP || wParam == GVK_LEFT || wParam == GVK_RIGHT)
		{
			S_RECT Rect;
			C_SGUIDC *pdc = new C_SGUIDC(this);
			GetHighlightRect(Rect, pdc);
			pdc->ReverseRect(Rect);
			if (wParam == GVK_DN)
				m_ColIdx = (m_ColIdx>=4)?0:m_ColIdx+1;
			else if (wParam == GVK_UP)
				m_ColIdx = (m_ColIdx<=0)?4:m_ColIdx-1;
			else if (wParam == GVK_LEFT)
				m_RowIdx = (m_RowIdx<=0)?m_RowCharCnt:m_RowIdx-1;
			else if (wParam == GVK_RIGHT)
				m_RowIdx = (m_RowIdx>=m_RowCharCnt)?0:m_RowIdx+1;
			GetHighlightRect(Rect, pdc);
			pdc->ReverseRect(Rect);
			delete pdc;
		}
		else if (wParam == GVK_OK)
		{
			m_pParent->SendWndMsg(GM_CHAR, GetHighlightChar(), lParam);
		}
		else
		{
			m_pParent->SendWndMsg(GM_KEYUP, wParam, lParam);
		}
		break;
	default:
		return C_GUICTRL::WndProcess(msg, wParam, lParam);
	}
	return 0;
}

int C_VKBOARD::DefWndProcess(S_WORD msg, S_WORD wParam, S_DWORD lParam)
{
	return C_GUICTRL::DefWndProcess(msg, wParam, lParam);
}

int C_VKBOARD::GetHighlightRect(S_RECT &rect, const C_SGUIDC *pdc)
{
	if (m_RowIdx == (m_RowCharCnt + 1) - 1)
	{
		rect.x = 2+3+m_RowCharCnt*pdc->GetFontWidth('0');
		rect.y = 3+m_ColIdx*pdc->GetFontHeight('0');
		rect.w = 3*pdc->GetFontWidth('0');
		rect.h = pdc->GetFontHeight('0')-1;
	}
	else
	{
		rect.x = 2+m_RowIdx*pdc->GetFontWidth('0');
		rect.y = 2+m_ColIdx*pdc->GetFontHeight('0');
		rect.w = pdc->GetFontWidth('0');
		rect.h = pdc->GetFontHeight('0');
	}
	return 0;
}

S_WORD C_VKBOARD::GetHighlightChar(void)
{
	if (m_RowIdx == (m_RowCharCnt + 1) - 1)
		return (S_WORD)m_VkSpecialKey[m_ColIdx].m_Val;
	return (S_WORD)m_VkAsciiSet[m_ColIdx][m_RowIdx];
}

//
//
//
C_TEXTEDIT::C_TEXTEDIT(void)
{
}

C_TEXTEDIT::~C_TEXTEDIT()
{
}

bool C_TEXTEDIT::Create(const S_CHAR *pszCaption, S_DWORD dwStyle, const S_RECT& rect, C_SGUIWND* pParentWnd, S_DWORD nID)
{
	if (AddCtrl2ParentWnd(pParentWnd) == false)
		return false;

	m_WndText = pszCaption;
	m_WndRect = rect;
	m_pParent = pParentWnd;
	m_flag = dwStyle | WNDF_VISIBLE;
	m_ID = nID;

	if (m_WndText.empty())
		m_WndText.append("  ");
	SendWndMsg(GM_CREATE, 0x00, 0x00);
	return true;
}

int C_TEXTEDIT::WndProcess(S_WORD msg, S_WORD wParam, S_DWORD lParam)
{
	switch (msg)
	{
	case GM_CREATE:{
		C_SGUIDC *pdc = new C_SGUIDC(this);
		m_CaretIdx = 0;
		CreateCaret(pdc->GetFontWidth('0'), 1);
		SetCaretPos(m_SideW+m_CaretIdx*pdc->GetFontWidth('0'), m_SideH+pdc->GetFontHeight('0'));
		ShowCaret();
		delete pdc;
		}
		break;
	case GM_PAINT:{
		C_SGUIDC *pdc = new C_SGUIDC(this);
		S_RECT WndRect = {0, 0, m_WndRect.w, m_WndRect.h};
		pdc->DrawRect(WndRect);
		pdc->DrawString(m_SideW, m_SideH, m_WndText.c_str());
		delete pdc;
		}
		break;
	case GM_KEYUP:
		if (wParam == GVK_LEFT)
		{
			C_SGUIDC *pdc = new C_SGUIDC(this);
			if (m_CaretIdx != 0)
			{
				m_CaretIdx -= 1;
				SetCaretPos(m_SideW+m_CaretIdx*pdc->GetFontWidth('0'), m_SideH+pdc->GetFontHeight('0'));
			}
			delete pdc;
		}
		else if (wParam == GVK_RIGHT)
		{
			C_SGUIDC *pdc = new C_SGUIDC(this);
			if (m_CaretIdx < (int)m_WndText.size())
			{
				m_CaretIdx += 1;
				SetCaretPos(m_SideW+m_CaretIdx*pdc->GetFontWidth('0'), m_SideH+pdc->GetFontHeight('0'));
			}
			delete pdc;
		}
		else
		{
			m_pParent->SendWndMsg(GM_KEYUP, wParam, lParam);
		}
		break;
	case GM_CHAR:
		if (wParam == GVK_BACKSPACE)
		{
			C_SGUIDC *pdc = new C_SGUIDC(this);
			if (m_CaretIdx != 0)
			{
				size_t i = m_CaretIdx - 1;
				for (; i < m_WndText.size()-1; i++)
					m_WndText[i] = m_WndText[i+1];
				m_WndText[i] = ' ';
				pdc->DrawString(m_SideW, m_SideH, m_WndText.c_str());
				m_CaretIdx -= 1;
				SetCaretPos(m_SideW+m_CaretIdx*pdc->GetFontWidth('0'), m_SideH+pdc->GetFontHeight('0'));
			}
			delete pdc;
		}
		else if (wParam == GVK_ENTER || wParam == GVK_ESC || wParam == GVK_F1)
		{
			m_pParent->SendWndMsg(GM_CHAR, wParam, lParam);
		}
		else
		{
			if (wParam == GVK_TAB) wParam = ' ';
			C_SGUIDC *pdc = new C_SGUIDC(this);
			if (m_CaretIdx < (int)m_WndText.size() )
			{
				m_WndText[m_CaretIdx] = (S_CHAR)wParam;
				pdc->DrawString(m_SideW, m_SideH, m_WndText.c_str());
				m_CaretIdx += 1;
				SetCaretPos(m_SideW+m_CaretIdx*pdc->GetFontWidth('0'), m_SideH+pdc->GetFontHeight('0'));
			}
			delete pdc;
		}
		break;
	default:
		return C_GUICTRL::WndProcess(msg, wParam, lParam);
	}
	return 0;
}

int C_TEXTEDIT::DefWndProcess(S_WORD msg, S_WORD wParam, S_DWORD lParam)
{
	return C_GUICTRL::DefWndProcess(msg, wParam, lParam);
}


//
//
//
C_BUTTON::C_BUTTON(void)
{
	m_state = 0;
}

C_BUTTON::~C_BUTTON()
{
}

bool C_BUTTON::Create(const S_CHAR *pszCaption, S_DWORD dwStyle, const S_RECT& rect, C_SGUIWND* pParentWnd, S_DWORD nID)
{
	if (AddCtrl2ParentWnd(pParentWnd) == false)
		return false;

	m_WndText = pszCaption;
	m_WndRect = rect;
	m_pParent = pParentWnd;
	m_flag = dwStyle | WNDF_VISIBLE;
	m_ID = nID;

	SendWndMsg(GM_CREATE, 0x00, 0x00);
	return true;
}

int C_BUTTON::WndProcess(S_WORD msg, S_WORD wParam, S_DWORD lParam)
{
	switch (msg)
	{
	case GM_CREATE:
		break;
	case GM_PAINT:{
		C_SGUIDC *pdc = new C_SGUIDC(this);
		S_RECT WndRect = {0, 0, m_WndRect.w, m_WndRect.h};
		pdc->SelectObject(pdc->GetStockGuiObj(SGUI_OBJ_WPEN));
		pdc->FillRect(WndRect);
		pdc->SelectObject(pdc->GetStockGuiObj(SGUI_OBJ_BPEN));
		pdc->DrawRect(WndRect);
		pdc->DrawString(4, 2, m_WndText.c_str());
		if ((GetWndLong()&WNDF_FOCUS) == WNDF_FOCUS)
		{
			WndRect.x = 1, WndRect.y = 1;
			WndRect.w = WndRect.w - 2, WndRect.h = WndRect.h - 2;
			pdc->ReverseRect(WndRect);
		}
		delete pdc;
		}
		break;
	case GM_KEYUP:
		if (wParam == GVK_OK)
		{
			m_pParent->SendWndMsg(GM_CMD, (S_WORD)m_ID, 0x00);
		}
		else
		{
			m_pParent->SendWndMsg(msg, wParam, lParam);
		}
		break;
	default:
		return DefWndProcess(msg, wParam, lParam);
	}
	return 0;
}

int C_BUTTON::DefWndProcess(S_WORD msg, S_WORD wParam, S_DWORD lParam)
{
	return C_GUICTRL::DefWndProcess(msg, wParam, lParam);
}



