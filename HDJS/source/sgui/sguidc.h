//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			SGUI(Simple GUI)
// File:			sguidc.h
// Description:		SGUI for Power user elect energy acquire system;unsupport multi_thread
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2010-01-19  Create                  Kingsea
//---------------------------------------------------------------------------------
#ifndef __SGUI_DC_H__
#define __SGUI_DC_H__
#include "haidef.h"
#include "sguiobj.h"
#include "sguibmp.h"
#include "sguipen.h"
#include "sguifont.h"
#include "sguibase.h"

enum
{
	SGUI_OBJ_UNKNOW,

	SGUI_OBJ_BPEN,
	SGUI_OBJ_WPEN,
	SGUI_OBJ_F12x12,
	SGUI_OBJ_F16x16,
	SGUI_OBJ_E5x8,

	SGUI_OBJ_MAX
};

class C_WNDBASE;
class C_SGUIDC:public C_SGUIOBJ
{
public:
	C_SGUIDC(C_WNDBASE *pWnd = NULL);
	virtual ~C_SGUIDC();

	virtual int DrawPixel(S_WORD x, S_WORD y);
	virtual int DrawHLine(S_WORD x, S_WORD y, S_WORD w);
	virtual int DrawVLine(S_WORD x, S_WORD y, S_WORD h);
	virtual int DrawRect(const S_RECT &rRect);
	virtual int FillRect(const S_RECT &rRect);
	virtual int ReverseRect(const S_RECT &rRect);
	virtual int DrawString(S_WORD x, S_WORD y, const char *pText);
	virtual int GetStringExtent(const char *pText);

	C_SGUIOBJ *GetStockGuiObj(S_BYTE ObjName);
	C_SGUIOBJ *SelectObject(C_SGUIOBJ *pObj);

	int GetFontHeight(S_WORD font)const{if (m_pFont) return m_pFont->GetHeight(font);return 0;}
	int GetFontWidth(S_WORD font)const{if (m_pFont) return m_pFont->GetWidth(font);return 0;}

	bool FlushScreenEn(bool enable);
	bool FlushScreen(void);

protected:
	int ClientToScreen(S_RECT &rRect);

protected:
	S_RECT m_Rect;
	C_WNDBASE  *m_pWnd;
	C_SGUIPEN  *m_pPen;
	C_SGUIFONT *m_pFont;
	C_SGUIBMP  *m_pBMP;
};

#endif //__SGUI_DC_H__

