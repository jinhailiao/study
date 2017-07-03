//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			SGUI(Simple GUI)
// File:			sguipen.h
// Description:		SGUI for Power user elect energy acquire system;unsupport multi_thread
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2010-01-19  Create                  Kingsea
//---------------------------------------------------------------------------------
#ifndef __SGUI_PEN_H__
#define __SGUI_PEN_H__
#include "haidef.h"
#include "sguiobj.h"
#include "sguibmp.h"

class C_SGUIPEN:public C_SGUIOBJ
{
public:
	C_SGUIPEN(int color);
	virtual ~C_SGUIPEN();

	int ReverseColor(void){m_color = ~m_color; return m_color;}

	virtual int SetPixel(S_WORD x, S_WORD y, C_SGUIBMP *pBMP);
	virtual int GetPixel(S_WORD x, S_WORD y, C_SGUIBMP *pBMP);
	virtual int DrawHLine(S_WORD x, S_WORD y, S_WORD w, C_SGUIBMP *pBMP);
	virtual int DrawVLine(S_WORD x, S_WORD y, S_WORD h, C_SGUIBMP *pBMP);

protected:
	int m_color;
};


#endif //__SGUI_PEN_H__

