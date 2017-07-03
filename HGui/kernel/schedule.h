//---------------------------------------------------------------------------------
// Copyright (c) Haisoft 2006-9-1
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HGui
// File:			schedule.h
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2006-09-01	Create					Kingsea
// 2.0.0		2006-09-06  update                  Kingsea
// 3.0.0		2008-03-01	update					Kingsea
//---------------------------------------------------------------------------------

#ifndef  _SCHEDULE_H_
#define  _SCHEDULE_H_

#include "haitype.h"
#include "haihandle.h"


#define  DEFINESCENE(SceneID)		void hai_AppProc##SceneID(SP_EVT pEvent) \
									{
#define  MSGMAPBEGIN					if (0)  {}

#define  ONCREAT(Function)				else if (pEvent->EventType == EVT_CREAT) \
										{  \
											static void Function(SP_EVT); \
											Function(pEvent); \
										}
#define  ONPAINT(Function)				else if (pEvent->EventType == EVT_PAINT) \
										{  \
											static void Function(SP_EVT); \
											Function(pEvent); \
										}
#define  ONTIMER(Function)				else if (pEvent->EventType == EVT_TIMER) \
										{  \
											static void Function(SP_EVT); \
											Function(pEvent); \
										}
#define  ONKEYDN(Function)				else if (pEvent->EventType == EVT_KEYDN) \
										{  \
											static void Function(SP_EVT); \
											Function(pEvent); \
										}
#define  ONKEYUP(Function)				else if (pEvent->EventType == EVT_KEYUP) \
										{  \
											static void Function(SP_EVT); \
											Function(pEvent); \
										}
#define  ONCHAR(Function)				else if (pEvent->EventType == EVT_CHAR) \
										{  \
											static void Function(SP_EVT); \
											Function(pEvent); \
										}
#define  ONDESTROY(Function)			else if (pEvent->EventType == EVT_DESTROY) \
										{  \
											static void Function(SP_EVT); \
											Function(pEvent); \
										}
#define  ONMSG(MsgName, Function)		else if (pEvent->EventType == MsgName) \
										{  \
											static void Function(SP_EVT); \
											Function(pEvent); \
										}
#define  ONUSERMSG(MsgName, Function)	else if (pEvent->EventType == MsgName) \
										{  \
											static void Function(SP_EVT); \
											Function(pEvent); \
										}
#define  MSGMAPEND						else \
										{	\
											extern void hai_DefSceneProc(SP_EVT); \
											hai_DefSceneProc(pEvent); \
										}	\
									  }


#define  GOTOSCENE(SceneID, wParam, lParam)	do {  \
								extern void hai_GotoScene(void *, S_BYTE, WPARAM, LPARAM); \
								void hai_AppProc##SceneID(SP_EVT); \
								hai_GotoScene(hai_AppProc##SceneID, SceneID, wParam, lParam); \
							} while (0)

#define  SWITCHAPP(AppID, SceneID, wParam, lParam)  do { \
										extern void hai_SwitchApp(S_BYTE, S_BYTE, void *, WPARAM, LPARAM); \
										extern void hai_AppProc##SceneID(SP_EVT); \
										hai_SwitchApp(AppID, SceneID, hai_AppProc##SceneID, wParam, lParam); \
							} while (0)

#define  RETURNSCENE(wParam, lParam)		{ \
								extern void hai_ReturnPreScene(WPARAM, LPARAM); \
								hai_ReturnPreScene(wParam, lParam); \
							} while (0)

#define  PREGOTOSCENE(AppID, SceneID, wParam, lParam)  do { \
										extern void hai_PreGotoScene(S_BYTE, S_BYTE, void *, WPARAM, LPARAM); \
										extern void hai_AppProc##SceneID(SP_EVT); \
										hai_PreGotoScene(AppID, SceneID, hai_AppProc##SceneID, wParam, lParam); \
							} while (0)

#define CALLPREGOTOSCENE() do { \
										extern S_VOID hai_CallPreGotoScene(S_VOID);\
										hai_CallPreGotoScene();\
							} while (0)

#endif//_SCHEDULE_H_


