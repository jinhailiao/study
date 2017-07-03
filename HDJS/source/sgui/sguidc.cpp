//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			SGUI(Simple GUI)
// File:			sguidc.cpp
// Description:		SGUI for Power user elect energy acquire system;unsupport multi_thread
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2010-01-18  Create                  Kingsea
//---------------------------------------------------------------------------------
#include "sguidc.h"
#include "sguibase.h"
#include "sguiwnd.h"


C_SGUIDC::C_SGUIDC(C_WNDBASE *pWnd):C_SGUIOBJ(C_SGUIOBJ::OBJ_T_DC)
{
	if (pWnd == NULL)
	{//screen dc
		m_Rect.x = 0; m_Rect.w = SGUI_LCD_WIDTH;
		m_Rect.y = 0; m_Rect.h = SGUI_LCD_HEIGHT;
	}
	else
	{
		m_Rect = pWnd->GetWndRect();
	}
	m_pWnd  = pWnd;
	m_pBMP  = &SGUI_LcdScrn;
	SelectObject(GetStockGuiObj(SGUI_OBJ_BPEN));
	SelectObject(GetStockGuiObj(SGUI_OBJ_F12x12));
}

C_SGUIDC::~C_SGUIDC()
{
}

int C_SGUIDC::DrawPixel(S_WORD x, S_WORD y)
{
	S_RECT rect = {x, y, 1, 1};
	ClientToScreen(rect);

	if (m_pPen->SetPixel(rect.x, rect.y, m_pBMP) == 0)
		FlushScreen();
	return 0;
}

int C_SGUIDC::DrawHLine(S_WORD x, S_WORD y, S_WORD w)
{
	S_RECT rect = {x, y, w, 1};
	ClientToScreen(rect);

	if (m_pPen->DrawHLine(rect.x, rect.y, rect.w, m_pBMP) == 0)
		FlushScreen();
	return 0;
}

int C_SGUIDC::DrawVLine(S_WORD x, S_WORD y, S_WORD h)
{
	S_RECT rect = {x, y, 1, h};
	ClientToScreen(rect);

	if (m_pPen->DrawVLine(rect.x, rect.y, rect.h, m_pBMP) == 0)
		FlushScreen();
	return 0;
}

int C_SGUIDC::DrawRect(const S_RECT &rRect)
{
	S_RECT rect = rRect;
	ClientToScreen(rect);

	m_pPen->DrawHLine(rect.x, rect.y, rect.w, m_pBMP);
	m_pPen->DrawHLine(rect.x, rect.y + rect.h - 1, rect.w, m_pBMP);
	m_pPen->DrawVLine(rect.x, rect.y, rect.h, m_pBMP);
	m_pPen->DrawVLine(rect.x + rect.w - 1, rect.y, rect.h, m_pBMP);

	FlushScreen();
	return 0;
}

int C_SGUIDC::FillRect(const S_RECT &rRect)
{//no brush ,then use pen
	S_RECT rect = rRect;
	ClientToScreen(rect);

	for (int i = rect.y; i < rect.y + rect.h; ++i)
		m_pPen->DrawHLine(rect.x, i, rect.w, m_pBMP);

	FlushScreen();
	return 0;
}

int C_SGUIDC::ReverseRect(const S_RECT &rRect)
{
	S_RECT rect = rRect;
	ClientToScreen(rect);

	for (int i = rect.y; i < rect.y + rect.h; ++i)
		m_pBMP->FlipHLine(rect.x, i, rect.w);
	FlushScreen();
	return 0;
}

int C_SGUIDC::DrawString(S_WORD x, S_WORD y, const char *pText)
{
	S_RECT rect = {x, y, 1, 1};
	ClientToScreen(rect);
	x = rect.x, y = rect.y;

	while (S_BYTE ch = *pText++)
	{
		if (ch & 0x80)//Hz font
		{
			S_WORD Hz = ((S_WORD)ch<<8) + ((S_WORD)(*pText++)&0xFF);
			m_pFont->DrawFont(x, y, Hz, m_pBMP);
			x += m_pFont->GetWidth(Hz);
		}
		else//english font
		{
			m_pFont->DrawChar(x, y, ch, m_pBMP);
			x += m_pFont->GetWidth(ch);
		}
	}

	FlushScreen();
	return 0;
}

int C_SGUIDC::GetStringExtent(const char *pText)
{
	if (pText == NULL)
		return 0;

	int size = 0;
	while (S_BYTE ch = *pText++)
	{
		if (ch & 0x80)//Hz font
		{
			S_WORD Hz = ((S_WORD)ch<<8) + ((S_WORD)(*pText++)&0xFF);
			size += m_pFont->GetWidth(Hz);
		}
		else//english font
		{
			size += m_pFont->GetWidth(ch);
		}
	}
	return size;
}

C_SGUIOBJ *C_SGUIDC::GetStockGuiObj(S_BYTE ObjName)
{
	static C_SGUIPEN SGUI_BPen(1);
	static C_SGUIPEN SGUI_WPen(0);
	static C_SGUIF12x12 SGUI_F12x12;
	static C_SGUIF16x16 SGUI_F16x16;
	static C_SGUIE5x8 SGUI_E5x8;

	switch (ObjName)
	{
	case SGUI_OBJ_BPEN:
		return &SGUI_BPen;
	case SGUI_OBJ_WPEN:
		return &SGUI_WPen;
	case SGUI_OBJ_F12x12:
		return &SGUI_F12x12;
	case SGUI_OBJ_F16x16:
		return &SGUI_F16x16;
	case SGUI_OBJ_E5x8:
		return &SGUI_E5x8;
	default:
		break;
	}
	return NULL;
}

C_SGUIOBJ *C_SGUIDC::SelectObject(C_SGUIOBJ *pObj)
{
	C_SGUIOBJ *pOld = NULL;

	if (pObj == NULL)
		return pOld;

	switch (pObj->GetObjType())
	{
	case C_SGUIOBJ::OBJ_T_PEN:
		pOld = m_pPen;
		m_pPen = dynamic_cast<C_SGUIPEN*>(pObj);
		break;
	case C_SGUIOBJ::OBJ_T_FONT:
		pOld = m_pFont;
		m_pFont = dynamic_cast<C_SGUIFONT*>(pObj);
		break;
	default:
		break;
	}
	return pOld;
}

bool C_SGUIDC::FlushScreenEn(bool enable)
{
	return SGUI_FlushScreenEn(enable);
}

bool C_SGUIDC::FlushScreen(void)
{
	if (m_pBMP->GetBmpData() != SGUI_UsrBuf)
		return false;
	
	if (m_pWnd == NULL)
	{
		SGUI_flushScreen(0, 0, SGUI_LCD_WIDTH, SGUI_LCD_HEIGHT);
	}
	else
	{
		if (m_pWnd->InvalidRect() == false)
			m_pWnd->UpdateWnd();
	}
	return true;
}

int C_SGUIDC::ClientToScreen(S_RECT &rRect)
{
	if (m_pWnd == NULL)
	{
		rRect.x += m_Rect.x, rRect.y += m_Rect.y;
		if (rRect.x+rRect.w > m_Rect.x+m_Rect.w)
			rRect.w = m_Rect.x+m_Rect.w - rRect.x;
		if (rRect.y+rRect.h > m_Rect.y+m_Rect.h)
			rRect.h = m_Rect.y+m_Rect.h - rRect.y;
	}
	else
	{
		m_pWnd->ClientToScreen(rRect);
	}
	return 0;
}



