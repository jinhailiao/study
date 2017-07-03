//---------------------------------------------------------------------------------
// Copyright (c) Haisoft 2006-9-1
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HGui
// File:			winmain.c
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2006-09-01	Create					Kingsea
// 2.0.0		2006-09-06  update                  Kingsea
// 3.0.0		2008-03-01	update					Kingsea
//---------------------------------------------------------------------------------

#include "_haievent.h"
#include "_haiscene.h"
#include "_haigdi.h"
#include "_schedule.h"
#include "haimem.h"
#include "_haictrl.h"


#define HEAP_SIZE_8M	0x800000
#define HEAP_SIZE_4M	0x400000
#define HEAP_SIZE_2M	0x200000
#define HAI_HEAP_SIZE	HEAP_SIZE_4M


S_DWORD _gMemHeap[HAI_HEAP_SIZE/sizeof(S_DWORD)];
S_VOID	hai_InitDateTime(S_VOID);
S_VOID  hai_InitCaret(S_VOID);
S_VOID  _InitMsgBoxSystem(S_VOID);
S_VOID  hai_InitPreSceneQueue(S_VOID);


int main(int argc, char *argv[])
{
	hai_InitEventQueue();
	hai_InitSceneStk();
	hai_InitDateTime();
	hai_InitSceneOperateFlag();
	hai_SetDefaultAppAndScene();
	hai_InitMemHeap(_gMemHeap,HAI_HEAP_SIZE);
	_hai_InitGDISystem();
	hai_InitCaret();
	_hai_InitCtrlSystem();
	_InitMsgBoxSystem();
	hai_InitPreSceneQueue();

	hai_Schedule();

	return 1;
}

