//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			SGUI(Simple GUI)
// File:			sguibmp.cpp
// Description:		SGUI for Power user elect energy acquire system;unsupport multi_thread
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2010-01-18  Create                  Kingsea
//---------------------------------------------------------------------------------
#include "sguibmp.h"
#include "sguibase.h"

C_SGUIBMP::C_SGUIBMP(S_WORD w, S_WORD h, S_WORD bpp, void *pBuff):C_SGUIOBJ(C_SGUIOBJ::OBJ_T_BMP)
{
	m_Width  = w;
	m_Height = h;
	m_BPP    = bpp;
	m_pBuff  = pBuff;
}

C_SGUIBMP::~C_SGUIBMP()
{
}


C_SGUISCRN::C_SGUISCRN(void):C_SGUIBMP(SGUI_LCD_WIDTH,SGUI_LCD_HEIGHT,SGUI_LCD_BPP,SGUI_UsrBuf)
{
}

C_SGUISCRN::~C_SGUISCRN()
{
}

int C_SGUISCRN::SetPixel(S_WORD x, S_WORD y, int color)
{
	if (x < m_Width && y < m_Height)
	{
		char *p = (char *)m_pBuff + y * SGUI_LCD_LINE + (x >> 3);
		x = x & 0x07;//byte offset
		if (color&0x01)
			*p |=  (1 << x);
		else
			*p &= ~(1 << x);

		return 0;
	}
	return -1;
}

int C_SGUISCRN::GetPixel(S_WORD x, S_WORD y)
{
	if (x < m_Width && y < m_Height)
	{
		char *p = (char *)m_pBuff + y * SGUI_LCD_LINE + (x >> 3);
		x = x & 0x07;//byte offset
		return (int)((*p>>x)&0x01);
	}
	return 0;
}

int C_SGUISCRN::FlipPixel(S_WORD x, S_WORD y)
{
	if (x < m_Width && y < m_Height)
	{
		S_BYTE *p = (S_BYTE*)m_pBuff + y * SGUI_LCD_LINE + x / 8;
		x = x & 0x07;//byte offset
		S_BYTE color = (1 << x) & *p;
		if (color)
			*p &= ~color;
		else
			*p |= 1 << x;

		return 0;
	}
	return -1;
}

int C_SGUISCRN::FlipHLine(S_WORD x, S_WORD y, S_WORD w)
{
	if (x + w > m_Width || y > m_Height)
		return -1;

	for (int i = x; i < x + w; ++i)
		FlipPixel(i, y);
	return 0;
}

int C_SGUISCRN::FillHLine(S_WORD x, S_WORD y, S_WORD w, int color)
{
	if (x + w > m_Width || y > m_Height)
		return -1;

	S_BYTE loff, roff, cnt, C = 0xFF;
	S_WORD X;
	S_BYTE *p = (S_BYTE*)m_pBuff + y * SGUI_LCD_LINE + (x >> 3);

	if ((loff = (x&0x07)) != 0)
	{
		cnt = 8 - loff;
		if (cnt > w) cnt = (S_BYTE)w;
		roff = 8 - loff - cnt;
		C >>= loff; C <<= loff; 
		C <<= roff; C >>= roff;
		
		if (color&0x01) *p++ |=  C;
		else *p++ &= ~C;
		x += cnt, w -= cnt;
	}
	
	C = (color&0x01)? 0xFF:0x00;
	for (X = x; X + 8 <= x + w; X += 8)
		*p++  = C;

	C = 0xFF;
	if (X < x + w)
	{
		cnt = (x + w) - X;
		roff = 8 - cnt;
		C <<= roff; C >>= roff;
		if (color&0x01) *p |=  C;
		else *p &= ~C;
	}

	return 0;
}


C_SGUISCRN SGUI_LcdScrn;


