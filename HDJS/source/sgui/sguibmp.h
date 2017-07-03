//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			SGUI(Simple GUI)
// File:			sguibmp.h
// Description:		SGUI for Power user elect energy acquire system;unsupport multi_thread
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2010-01-19  Create                  Kingsea
//---------------------------------------------------------------------------------
#ifndef __SGUI_BMP_H__
#define __SGUI_BMP_H__
#include "haidef.h"
#include "sguiobj.h"

class C_SGUIBMP:public C_SGUIOBJ
{
public:
	C_SGUIBMP(S_WORD w, S_WORD h, S_WORD bpp, void *pBuff);
	virtual ~C_SGUIBMP();

	void *GetBmpData(void){return m_pBuff;}

	virtual int SetPixel(S_WORD x, S_WORD y, int color)=0;
	virtual int GetPixel(S_WORD x, S_WORD y)=0;
	virtual int FlipPixel(S_WORD x, S_WORD y)=0;
	virtual int FlipHLine(S_WORD x, S_WORD y, S_WORD w)=0;
	virtual int FillHLine(S_WORD x, S_WORD y, S_WORD w, int color)=0;

protected:
	S_WORD m_Width;
	S_WORD m_Height;
	S_WORD m_BPP;
	void *m_pBuff;
};

class C_SGUISCRN:public C_SGUIBMP
{
public:
	C_SGUISCRN(void);
	virtual ~C_SGUISCRN();

	virtual int SetPixel(S_WORD x, S_WORD y, int color);
	virtual int GetPixel(S_WORD x, S_WORD y);
	virtual int FlipPixel(S_WORD x, S_WORD y);
	virtual int FlipHLine(S_WORD x, S_WORD y, S_WORD w);
	virtual int FillHLine(S_WORD x, S_WORD y, S_WORD w, int color);
};

extern C_SGUISCRN SGUI_LcdScrn;

#endif //__SGUI_BMP_H__

