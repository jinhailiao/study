//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			SGUI(Simple GUI)
// File:			sguipen.cpp
// Description:		SGUI for Power user elect energy acquire system;unsupport multi_thread
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2010-01-18  Create                  Kingsea
//---------------------------------------------------------------------------------
#include "sguipen.h"

C_SGUIPEN::C_SGUIPEN(int color):C_SGUIOBJ(C_SGUIOBJ::OBJ_T_PEN)
{
	m_color = color;
}

C_SGUIPEN::~C_SGUIPEN()
{
}

int C_SGUIPEN::SetPixel(S_WORD x, S_WORD y, C_SGUIBMP *pBMP)
{
	return pBMP->SetPixel(x, y, m_color);
}

int C_SGUIPEN::GetPixel(S_WORD x, S_WORD y, C_SGUIBMP *pBMP)
{
	m_color = pBMP->GetPixel(x, y);
	return m_color;
}

int C_SGUIPEN::DrawHLine(S_WORD x, S_WORD y, S_WORD w, C_SGUIBMP *pBMP)
{
	return pBMP->FillHLine(x, y, w, m_color);
}

int C_SGUIPEN::DrawVLine(S_WORD x, S_WORD y, S_WORD h, C_SGUIBMP *pBMP)
{
	for (S_WORD i = y; i < y + h; ++i)
		pBMP->SetPixel(x, i, m_color);

	return 0;
}


