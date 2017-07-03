//---------------------------------------------------------------------------------
// Copyright (c) Haisoft 2006-9-1
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HGui
// File:			_schedule.h
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2006-09-01	Create					Kingsea
// 2.0.0		2006-09-06  update                  Kingsea
// 3.0.0		2008-03-01	update					Kingsea
//---------------------------------------------------------------------------------

#ifndef  __SCHEDULE_H__
#define  __SCHEDULE_H__

#include  "haitype.h"
#include  "_haiscene.h"

#define  SOP_NULL     0
#define  SOP_GOTOSCENE  1
#define  SOP_RTNSCENE   2

S_VOID  hai_InitSceneOperateFlag(S_VOID);
S_VOID  hai_SetDefaultAppAndScene(S_VOID);
S_VOID  hai_Schedule(S_VOID);
S_VOID  hai_SwitchApp(S_BYTE AppID, S_BYTE SceneID, S_VOID *pSceneProc, WPARAM  wParam, LPARAM lParam);
S_VOID  hai_GotoScene(S_VOID *pSceneProc, S_BYTE SceneID, WPARAM  wParam, LPARAM lParam);
S_VOID  hai_ReturnPreScene(WPARAM  wParam, LPARAM lParam);
S_VOID  hai_InitSceneOperateFlag(S_VOID);
S_VOID  hai_SetSceneOperateFlag(S_BYTE flag);
S_BYTE  hai_GetSceneOperateFlag(S_VOID);
S_VOID _hai_ReturnPreScene(S_VOID);


#endif
