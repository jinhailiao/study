//---------------------------------------------------------------------------------
// Copyright (c) Haisoft 2006-9-1
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HGui
// File:			haicaret.h
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 2.0.0		2006-09-06  create                  Kingsea
// 3.0.0		2008-03-01	update					Kingsea
//---------------------------------------------------------------------------------
#ifndef _HAICARET_H_
#define _HAICARET_H_

#include "haitype.h"
#include "haihandle.h"

typedef struct tagCaret
{
	S_BYTE CaretData[20];
}
S_CARET, *SP_CARET;

#define CARET_COPY_MODE		1

//only system call
S_BOOL  _hai_ShowCaret(S_VOID);
//only system call
S_BOOL  _hai_HideCaret(S_VOID);

S_VOID  hai_InitCaret(S_VOID);
S_BOOL  hai_ShowCaret(S_VOID);
S_BOOL  hai_HideCaret(S_VOID);
S_VOID  hai_GetCaretPos(S_SHORT *x, S_SHORT *y);
S_VOID  hai_SetCaretPos(S_SHORT x, S_SHORT y);
S_VOID  hai_SetCaretWidthHeight(S_WORD w, S_WORD h);
S_VOID  hai_SetCaretBlinkTime(S_BYTE  TenthSecond);
S_BYTE  hai_GetCaretBlinkTime(S_VOID);
S_WORD HG_SaveCurCaret(S_VOID *Buffer, S_WORD BufSize);
S_VOID HG_RestoreCurCaret(S_VOID *Buffer);


#endif //_HAICARET_H_


