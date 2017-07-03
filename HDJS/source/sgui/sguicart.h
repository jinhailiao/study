//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			SGUI(Simple GUI)
// File:			sguicart.h
// Description:		SGUI for Power user elect energy acquire system;unsupport multi_thread
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2010-01-18  Create                  Kingsea
//---------------------------------------------------------------------------------
#ifndef __SGUICARET_H__
#define __SGUICARET_H__

#include "sguiwnd.h"

class C_SGUICARET:public C_WNDBASE
{
public:
	C_SGUICARET(C_WNDBASE *pPwnd, S_WORD w, S_WORD h);
	~C_SGUICARET();

public:
	bool GetCaretStatus(void){return m_show;}

protected:
	virtual int WndProcess(S_WORD msg, S_WORD wParam, S_DWORD lParam);

protected:
	bool m_show;
};

#endif //__SGUICARET_H__



