//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			SGUI(Simple GUI)
// File:			sguifont.h
// Description:		SGUI for Power user elect energy acquire system;unsupport multi_thread
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2010-01-19  Create                  Kingsea
//---------------------------------------------------------------------------------
#ifndef __SGUI_FONT_H__
#define __SGUI_FONT_H__
#include "haidef.h"
#include "sguiobj.h"
#include "sguibmp.h"

class C_SGUIFONT:public C_SGUIOBJ
{
public:
	C_SGUIFONT(void):C_SGUIOBJ(C_SGUIOBJ::OBJ_T_FONT){m_style = 0; }
	virtual ~C_SGUIFONT(){}

public:
	virtual int GetWidth(S_WORD font) = 0;
	virtual int GetHeight(S_WORD font) = 0;
	virtual int DrawChar(S_WORD x, S_WORD y, char ch, C_SGUIBMP *pBMP) = 0;
	virtual int DrawFont(S_WORD x, S_WORD y, S_WORD ch, C_SGUIBMP *pBMP) = 0;

protected:
	int m_style;
};

class C_SGUIF12x12:public C_SGUIFONT
{
public:
	C_SGUIF12x12(void);
	virtual ~C_SGUIF12x12();

public:
	virtual int GetWidth(S_WORD font);
	virtual int GetHeight(S_WORD font);
	virtual int DrawChar(S_WORD x, S_WORD y, char ch, C_SGUIBMP *pBMP);
	virtual int DrawFont(S_WORD x, S_WORD y, S_WORD ch, C_SGUIBMP *pBMP);

private:
	int GetFontMold(S_WORD font, S_BYTE *pFontBuf);

private:
	FILE *m_fpFont;
};

class C_SGUIF16x16:public C_SGUIFONT
{
public:
	C_SGUIF16x16(void);
	virtual ~C_SGUIF16x16();

public:
	virtual int GetWidth(S_WORD font);
	virtual int GetHeight(S_WORD font);
	virtual int DrawChar(S_WORD x, S_WORD y, char ch, C_SGUIBMP *pBMP);
	virtual int DrawFont(S_WORD x, S_WORD y, S_WORD ch, C_SGUIBMP *pBMP);

private:
	int GetFontMold(S_WORD font, S_BYTE *pFontBuf);

private:
	FILE *m_fpFont;
};

class C_SGUIE5x8:public C_SGUIFONT
{
public:
	C_SGUIE5x8(void);
	virtual ~C_SGUIE5x8();

public:
	virtual int GetWidth(S_WORD font);
	virtual int GetHeight(S_WORD font);
	virtual int DrawChar(S_WORD x, S_WORD y, char ch, C_SGUIBMP *pBMP);
	virtual int DrawFont(S_WORD x, S_WORD y, S_WORD ch, C_SGUIBMP *pBMP);

private:
	int GetFontMold(S_WORD font, S_BYTE *pFontBuf);
};


#endif //__SGUI_FONT_H__

