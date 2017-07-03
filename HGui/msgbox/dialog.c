//---------------------------------------------------------------------------------
// Copyright (c) Haisoft 2006-9-1
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HGui
// File:			dialog.c
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 3.0.0		2008-03-15	create					Kingsea
//---------------------------------------------------------------------------------
#include "haitype.h"
#include "dialog.h"
#include "_haiscene.h"
#include "haiscene.h"
#include "appids.h"
#include "_haievent.h"
#include "_haigdi.h"
#include "haigdi.h"
#include "haigui.h"
#include "haicaret.h"


// extern data
//
extern S_SCENE gMsgbox[];

//function prototype
//
S_BYTE _FindMsgBoxID(S_VOID);
S_VOID _SaveCurrentScene4MsgBox(S_BYTE MsgBoxID);
S_VOID _RestoreCurrentScene4MsgBox(S_BYTE MsgBoxID);
S_VOID _ClearSceneInfo4MsgBox(S_BYTE MsgBoxID);


//
//
S_DWORD HG_DialogBox(S_WORD x, S_WORD y, S_WORD w, S_WORD h, HG_DialogProc *DlgProc)
{
	S_BYTE b, MsgboxID, CaretShow;
	SP_SCENE pScene;
	S_EVT event;
	S_CARET BackCaret;

	
	if ((MsgboxID = _FindMsgBoxID()) == APPID_NULL)
		return 0;
	
	CaretShow = hai_HideCaret();
	HG_SaveCurCaret(&BackCaret, sizeof(S_CARET));
	
	_SaveCurrentScene4MsgBox(MsgboxID);
	pScene = hai_GetCurSceneAddr();
	hai_SetScene(pScene, MsgboxID, SID_MSGBOX, S_NULL, (Hai_SceneProc *)DlgProc);
	pScene->SceneRect.x = x;
	pScene->SceneRect.y = y;
	pScene->SceneRect.w = w;
	pScene->SceneRect.h = h;
//	hai_InitEventQueue();
	_hai_SetInvalidRectEx(x, y, w, h);
	hai_SendEvent(EVT_CREAT, 0, 0);
	hai_SendEvent(EVT_PAINT, 0, 0);
			
	while (1)
	{
		event = hai_GetEvent();
		if (event.EventType == EVT_QUIT)
			break;
		hai_EventPreprocess(&event);
		pScene->pSceneProc(&event);
		if (event.EventType == EVT_EXIT)
			break;
		else
			hai_EventEndprocess(&event);
	}

	b = hai_SetScreenFlush(SCREEN_FLUSH_OFF);
	_hai_DestroyAllCtrl(S_FALSE);
	hai_DestroyInputMethod();
//	if (pScene->pInputMethod)
//	{
//		_hai_SetInvalidRectEx(pScene->pInputMethod->x, pScene->pInputMethod->y, pScene->pInputMethod->w, pScene->pInputMethod->h);
//		_hai_DestroyInputMethod(pScene->pInputMethod);
//	}
	hai_SetScreenFlush(b);
	_RestoreCurrentScene4MsgBox(MsgboxID);
	_ClearSceneInfo4MsgBox(MsgboxID);
	hai_InvalidRectEx(x, y, w, h);

	HG_RestoreCurCaret(&BackCaret);
	if (CaretShow)
		hai_ShowCaret();
	
	return (S_DWORD)event.lParam;

}

S_VOID HG_DialogBoxPreproc(SP_EVT pEvt)
{
	S_BYTE b, fblink;
	SP_RECT pInvalidRect = hai_GetInvalidRect();
	SP_RECT pSceneRect = hai_GetSceneRect(S_NULL, S_NULL, S_NULL, S_NULL);
	S_SCENE scene, *pCurScene = hai_GetCurSceneAddr();

	if (pEvt->AppID != pCurScene->AppID)
		return;
	switch (pEvt->EventType)
	{
	case EVT_PAINT:
		if (!hai_IsRectCovered(pSceneRect, pInvalidRect))
		{
			fblink = _hai_HideCaret();
			scene = *pCurScene;
			*pCurScene = gMsgbox[pEvt->AppID];
			b = hai_SetScreenFlush(SCREEN_FLUSH_OFF);
			hai_SendEvent(EVT_PAINT, 0, 0);
			hai_SetScreenFlush(b);
			*pCurScene = scene;
			if (fblink)
				_hai_ShowCaret();
		}
		break;
		
	case EVT_TIMER:
		fblink = _hai_HideCaret();
		scene = *pCurScene;//time msg send to pre scene, 
		*pCurScene = gMsgbox[pEvt->AppID];
		b = hai_SetScreenFlush(SCREEN_FLUSH_OFF);
		hai_SendEvent(EVT_TIMER, 0, 0);
		hai_SetScreenFlush(b);
		*pCurScene = scene;
		if (fblink)
			_hai_ShowCaret();
		if (hai_IsNeedRedraw())
		{
			if (hai_IsRectIntersect(pInvalidRect, pSceneRect))
				hai_SendEvent(EVT_PAINT, 0, 0);
			else
				_hai_UpdateRect();
		}
		break;
	default:
		break;
	}
}

S_VOID HG_EndDialogBox(S_DWORD ReturnVal)
{
	_hai_SendQuitEvent(0, (LPARAM)ReturnVal);
}





