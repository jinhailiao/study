//---------------------------------------------------------------------------------
// Copyright (c) Haisoft 2006-9-1
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HGui
// File:			_haievent.h
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2006-09-01	Create					Kingsea
// 2.0.0		2006-09-06  update                  Kingsea
// 3.0.0		2008-03-01	update					Kingsea
//---------------------------------------------------------------------------------

#ifndef  __HAIEVENT_H__
#define  __HAIEVENT_H__

#include "haitype.h"
#include "haievent.h"

//
//note: below function only call by system
S_BOOL _hai_IsSysKey(S_WORD key);
S_VOID  _hai_SendQuitEvent(WPARAM wParam, LPARAM lParam);
S_VOID  hai_InitEventQueue(S_VOID);
S_VOID  hai_InitSceneEvent(S_BYTE SceneOp, WPARAM  wParam, LPARAM lParam);//only system call
S_VOID  hai_EventPreprocess(SP_EVT pEvent);
S_VOID  hai_EventEndprocess(SP_EVT pEvent);
S_VOID  hai_PollEvent(S_VOID);
S_VOID hai_ClearUserEvent(S_VOID);


#endif
