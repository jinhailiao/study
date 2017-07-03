//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			SGUI(Simple GUI)
// File:			sguibase.h
// Description:		SGUI for Power user elect energy acquire system;unsupport multi_thread
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2010-01-18  Create                  Kingsea
//---------------------------------------------------------------------------------
#ifndef __SGUIBASE_H__
#define __SGUIBASE_H__

#include "sguimsg.h"

//
//
//
//#define AS_SCREEN

//
//
//
#define SGUI_LCD_WIDTH		160
#define SGUI_LCD_HEIGHT		160
#define SGUI_LCD_BPP		1

#define SGUI_LCD_LINE	(SGUI_LCD_WIDTH * SGUI_LCD_BPP / 8)
#define SGUI_LCD_SIZE	(SGUI_LCD_LINE * SGUI_LCD_HEIGHT)

//
//
//
extern char SGUI_UsrBuf[];

//
//
//
int SGUI_LcdInit(void);
int SGUI_KeyboardInit(void);
void SGUI_flushScreen(S_WORD x, S_WORD y, S_WORD w, S_WORD h);
bool SGUI_FlushScreenEn(bool enable);
void SGUI_CloseScreen(void);
void SGUI_OpenScreen(void);
void SGUI_backlight(int on);
S_GUIMSG SGUI_PollMsg(void);

int SGUI_SleepMS(int ms);


#endif //__SGUIBASE_H__



