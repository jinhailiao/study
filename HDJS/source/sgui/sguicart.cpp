//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			SGUI(Simple GUI)
// File:			sguicart.cpp
// Description:		SGUI for Power user elect energy acquire system;unsupport multi_thread
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2010-01-18  Create                  Kingsea
//---------------------------------------------------------------------------------
#include "sguicart.h"

C_SGUICARET::C_SGUICARET(C_WNDBASE *pPwnd, S_WORD w, S_WORD h)
{
	m_ObjT = C_SGUIOBJ::OBJ_T_CARET;
	m_pParent = pPwnd;
	m_WndRect.x = 0;
	m_WndRect.y = 0;
	m_WndRect.w = w;
	m_WndRect.h = h;
	m_flag = 0;
	m_show = false;
}

C_SGUICARET::~C_SGUICARET()
{
}

int C_SGUICARET::WndProcess(S_WORD msg, S_WORD wParam, S_DWORD lParam)
{
	C_SGUIDC dc(this);
	S_RECT rect = m_WndRect;
	rect.x = 0; rect.y = 0;
	switch (msg)
	{
	case GM_CREATE:
		break;
	case GM_PAINT:
	case GM_TIME:
		if ((m_flag & WNDF_VISIBLE) && !(m_flag & WNDF_DISABLED))
		{
			m_show = !m_show;
			dc.ReverseRect(rect);
		}
		break;
	default:
		return C_WNDBASE::DefWndProcess(msg, wParam, lParam);
	}
	return 0;
}

