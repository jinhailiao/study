//---------------------------------------------------------------------------------
// Copyright (c) Haisoft 2006-9-1
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HGui
// File:			haievent.c
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2006-09-01	Create					Kingsea
// 2.0.0		2006-09-06	update					Kingsea
// 3.0.0		2008-03-01	update					Kingsea
//---------------------------------------------------------------------------------

#include "haitype.h"
#include "_haievent.h"
#include "haievent.h"
#include "_haiscene.h"
#include "haiscene.h"
#include "string.h"
#include "haitime.h"
#include "_schedule.h"
#include "virkey.h"
#include "haicaret.h"
#include "msgbox.h"
#include "appids.h"
#include "schedule.h"
#include "haigdi.h"
#include "haigui.h"

#define   EVTQUEUE_LENGTH_MAX     10

typedef struct  tagEvtQueue
{
	S_BYTE   head;
	S_BYTE   tail;
	S_EVT  EventQ[EVTQUEUE_LENGTH_MAX];
}
S_EVTQ, *SP_EVTQ;

static S_EVTQ  EventQueue;

S_VOID  hai_InitEventQueue(S_VOID)
{
	memset(&EventQueue, 0, sizeof(EventQueue));
}

static S_BOOL  hai_IsEventQueueEmpty(S_VOID)
{
	return (EventQueue.head == EventQueue.tail);
}

static S_BOOL  hai_IsEventQueueFull(S_VOID)
{
	return (EventQueue.head == ((EventQueue.tail+1) % EVTQUEUE_LENGTH_MAX));
}

static S_BOOL  hai_InEventQueue(S_EVT event)
{
	if (hai_IsEventQueueFull())
		return S_FALSE;

	EventQueue.EventQ[EventQueue.tail] = event;
	EventQueue.tail = (EventQueue.tail+1) % EVTQUEUE_LENGTH_MAX;
	return S_TRUE;
}

static S_EVT  hai_OutEventQueue(S_VOID)
{
	S_EVT event = {0,0,0,0};

	if (hai_IsEventQueueEmpty())
		return event;

	event = EventQueue.EventQ[EventQueue.head];
	EventQueue.head = (EventQueue.head+1) % EVTQUEUE_LENGTH_MAX;
	return event;
}

static S_BOOL  hai_HeadEventQueue(S_EVT event)
{
	if (hai_IsEventQueueFull())
	{
		if (EventQueue.tail == 0)
			EventQueue.tail = EVTQUEUE_LENGTH_MAX-1;
		else
			EventQueue.tail--;
	}

	if (EventQueue.head == 0)
		EventQueue.head = EVTQUEUE_LENGTH_MAX-1;
	else
		EventQueue.head--;
	EventQueue.EventQ[EventQueue.head] = event;
		
	return S_TRUE;
}

static S_EVT  hai_ReadEventQueue(S_BOOL IsMove)
{
	S_EVT event = {0,0,0,0};

	if (hai_IsEventQueueEmpty())
		return event;

	event = EventQueue.EventQ[EventQueue.head];
	if (IsMove)
		EventQueue.head = (EventQueue.head+1) % EVTQUEUE_LENGTH_MAX;
	return event;
}

static S_BOOL hai_QueryEventQueue(S_BYTE minEvent, S_BYTE maxEvent)
{
	S_BYTE cnt;
	
	for (cnt = EventQueue.head; cnt != EventQueue.tail; cnt = (cnt+1)%EVTQUEUE_LENGTH_MAX)
		if (EventQueue.EventQ[cnt].EventType >= minEvent && EventQueue.EventQ[cnt].EventType <= maxEvent)
			return S_TRUE;
	return S_FALSE;
}

S_BOOL hai_ClearEventQueue(S_BYTE minEvent, S_BYTE maxEvent)
{
	S_BYTE cnt;
	
	for (cnt = EventQueue.head; cnt != EventQueue.tail; cnt = (cnt+1)%EVTQUEUE_LENGTH_MAX)
		if (EventQueue.EventQ[cnt].EventType >= minEvent && EventQueue.EventQ[cnt].EventType <= maxEvent)
			EventQueue.EventQ[cnt].EventType = EVT_NULL;
	return S_TRUE;
}

S_BOOL  hai_PostEvent(S_BYTE EventType, WPARAM wParam, LPARAM lParam)
{
	S_BOOL  ok;
	S_EVT event;

	if (EventType == EVT_QUIT)
		return S_FALSE;
	if (EventType == EVT_PAINT)
	{
		if (!hai_IsNeedRedraw())
			return S_FALSE;
		if (hai_QueryEventQueue(EVT_PAINT, EVT_PAINT))
			return S_FALSE;
	}
	else if (EventType == EVT_TIMER)
	{
		if (hai_QueryEventQueue(EVT_TIMER, EVT_TIMER))
			return S_FALSE;
	}

	event.AppID   = hai_GetCurrentAppID();
	event.EventType = EventType;
	event.wParam  = wParam;
	event.lParam  = lParam;

	ok = hai_InEventQueue(event);

	return ok;
}

S_BOOL hai_SendEvent(S_BYTE EventType, WPARAM wParam, LPARAM lParam)
{
	S_EVT event;
	SP_SCENE pScene;

	if (EventType == EVT_QUIT)
		return S_FALSE;
	if (EventType == EVT_DESTROY)
	{
		return hai_PostEvent(EVT_DESTROY, wParam, lParam);
	}
	if (EventType == EVT_PAINT)
	{
		if (!hai_IsNeedRedraw())
			return S_FALSE;
		if (hai_QueryEventQueue(EVT_PAINT, EVT_PAINT))
			return S_FALSE;
	}

	pScene = hai_GetCurSceneAddr();
	event.AppID     = hai_GetCurrentAppID();
	event.EventType = EventType;
	event.wParam    = wParam;
	event.lParam    = lParam;

	pScene->pSceneProc(&event);

	return S_TRUE;
}

S_VOID hai_Sleep(S_VOID)
{
}

S_EVT hai_PeekEvent(S_BOOL IsMove)
{
	S_EVT  event;
	S_DWORD span;
	static S_DWORD CurTick = 0;

	if (!CurTick)
		CurTick = hai_GetTimeTick();

	if (hai_IsPreSceneQueueEmpty())
	{
		if (hai_IsEventQueueEmpty())
			hai_PollEvent();
		event = hai_ReadEventQueue(IsMove);
	}
	else
	{
		event.EventType = EVT_EXIT;
		event.AppID = hai_GetCurrentAppID();
		event.wParam = 0;
		event.lParam = 0;
		hai_SetScreenFlush(SCREEN_FLUSH_OFF);
	}
	if (event.EventType >= EVT_KEYDN && event.EventType <= EVT_MOUSEMV)
	{
		CurTick = hai_GetTimeTick();
	}
	else
	{
		span = hai_GetTimeSpan(CurTick);
		if (span >= 120)
		{
			hai_Sleep();
			CurTick = hai_GetTimeTick();
		}
	}

	return event;
}

S_EVT hai_GetEvent(S_VOID)
{
	S_EVT  event;

	do
	{
		event = hai_PeekEvent(S_TRUE);
		
	}while (event.EventType == EVT_NULL);

	return event;
}

S_VOID  hai_InitSceneEvent(S_BYTE SceneOp, WPARAM  wParam, LPARAM lParam)
{
	S_EVT event;

	event.AppID = hai_GetCurrentAppID();
	event.wParam = 0;
	event.lParam = 0;
	event.EventType = EVT_PAINT;
	_hai_SetInvalidRect(S_NULL);
	hai_HeadEventQueue(event);
	
	event.wParam = wParam;
	event.lParam = lParam;
	if (SceneOp == SOP_GOTOSCENE)
		event.EventType = EVT_CREAT;
	else if (SceneOp == SOP_RTNSCENE)
		event.EventType = EVT_RESTORE;
	hai_HeadEventQueue(event);
}

S_VOID  _hai_SendQuitEvent(WPARAM wParam, LPARAM lParam)
{
	S_EVT event;

	event.AppID   = hai_GetCurrentAppID();
	event.EventType = EVT_QUIT;
	event.wParam  = wParam;
	event.lParam  = lParam;

	hai_HeadEventQueue(event);
}

S_BOOL  hai_PostTimerEvent(S_BYTE EvtType, WPARAM wParam, LPARAM lParam)
{
	return hai_PostEvent(EvtType, wParam, lParam);
}

S_BOOL hai_PostMouseEvent(S_BYTE EventType, S_WORD x, S_WORD y)
{
	return hai_PostEvent(EventType, 0, HAI_COMBDWORD(y,x));
}

S_BOOL _hai_IsSysKey(S_WORD key)
{
	return (key >= VK_F1 && key <= VK_F12);
}

S_VOID  _hai_BlinkCaret(S_VOID);
SP_CTRL _hai_InitInputMethod(S_BYTE InputMethodType);

S_VOID  hai_EventPreprocess(SP_EVT pEvent)
{
	S_SHORT x, y;
	SP_CTRL pCtrl;
	SP_RECT pSceneR;

	switch (pEvent->EventType)
	{
	case EVT_CREAT:
		break;
	case EVT_PAINT:
		break;	
	case EVT_TIMER:
		break;	
	case EVT_KEYDN:
		if (pEvent->wParam == VK_TAB)
			if (_hai_SetNextFocusCtrl())
			{
				pEvent->EventType = EVT_NULL;
				break;
			}
	case EVT_KEYUP:
	case EVT_CHAR:
		if (pCtrl=hai_GetCurrentInputMethod())
		{
			pCtrl->CtrlProc((SH_CTRL)pCtrl, pEvent->EventType, pEvent->wParam, pEvent->lParam);
			pEvent->EventType = EVT_NULL;
		}
		else if (pCtrl=(SP_CTRL)hai_GetCurSceneFocusCtrl())//focus ctrl
		{
			pCtrl->CtrlProc((SH_CTRL)pCtrl, pEvent->EventType, pEvent->wParam, pEvent->lParam);
			pEvent->EventType = EVT_NULL;
		}
		break;
	case EVT_SYSKEY:
		switch (pEvent->wParam)
		{
		case VK_F1:
			if (hai_SetSceneIME(INPUTMETHOD_GET) == INPUTMETHOD_DISABLE)
				break;
			if (hai_GetCurrentInputMethod())
			{
				hai_DestroyInputMethod();
				hai_SendEvent(EVT_IPMCHANGE, INPUTM_TYPE_NONE, 0);
			}
			else
			{
				hai_CreateInputMethod(INPUTM_TYPE_WUBI);
				pCtrl = hai_GetCurrentInputMethod();
				pCtrl->CtrlProc((SH_CTRL)pCtrl, EVT_PAINT, 0, 0);
				hai_SendEvent(EVT_IPMCHANGE, INPUTM_TYPE_WUBI, 0);
			}
			break;
		case VK_F2:
			hai_MsgBox("", "一个是蔺苑仙芭啊,一个是美玉无瑕啊.", MB_BTNOKCANCEL);
			break;
		case VK_F3:
			hai_MsgBox("天天快乐", "一个是水中月,\n一个是镜中花.", MB_BTNOK);
			break;
		case VK_F4:
			hai_MsgBox("红楼梦", "若说没奇缘,今生偏又遇着他", MB_DELAY);
			break;
		case VK_F5:
			hai_MsgBox("", "想眼中,能有多少泪珠儿?怎经得秋流到冬尽,春流到夏!\n啊......", MB_DELAY);
			break;
		case VK_F6:
			break;
		case VK_F7:
			break;
		case VK_F8:
			break;
		case VK_F9:
			PREGOTOSCENE(APPID_DESKTOP, SID_DESKTOP, 0, 0);
			break;
		case VK_F10:
			break;
		case VK_F11:
			break;
		case VK_F12:
			break;
		default:
			break;
		}
		break;
	case EVT_DESTROY:
		break;
	case EVT_MOUSEDN:
		x = HAI_GETLOWORD(pEvent->lParam);
		y = HAI_GETHIWORD(pEvent->lParam);
		pSceneR = hai_GetSceneRect(S_NULL, S_NULL, S_NULL, S_NULL);
		if (!hai_IsPointInRect(x, y, pSceneR))
			break;
		x -= pSceneR->x; y -= pSceneR->y;
		
		pCtrl = (SP_CTRL)hai_GetSpecifiedCtrl(x, y);
		if (pCtrl)
		{
			(pCtrl->CtrlProc)((SH_CTRL)pCtrl, pEvent->EventType, pEvent->wParam, (LPARAM)HAI_COMBDWORD(y, x));
			pEvent->EventType = EVT_NULL;
		}
		else
		{
			pCtrl = (SP_CTRL)hai_GetCurSceneFocusCtrl();
			if (pCtrl)
			{
				pCtrl->focus = 0;
				pCtrl->CtrlProc((SH_CTRL)pCtrl, EVT_FOCUS, (WPARAM)HAI_COMBWORD(HG_KILLFOCUS, SCENE_FOCUS), 0);
				pCtrl->CtrlProc((SH_CTRL)pCtrl, EVT_PAINT, 0, 0);
				hai_SendEvent(EVT_FOCUS, (WPARAM)HAI_COMBWORD(HG_GETFOCUS, pCtrl->CtrlID), 0);
			}
		}
		break;
	case EVT_MOUSEUP:
		x = HAI_GETLOWORD(pEvent->lParam);
		y = HAI_GETHIWORD(pEvent->lParam);
		pSceneR = hai_GetSceneRect(S_NULL, S_NULL, S_NULL, S_NULL);
		if (!hai_IsPointInRect(x, y, pSceneR))
			break;
		x -= pSceneR->x; y -= pSceneR->y;
		
		pCtrl = (SP_CTRL)hai_SetCapture(S_NULL);
		if (pCtrl)
		{
			(pCtrl->CtrlProc)((SH_CTRL)pCtrl, pEvent->EventType, pEvent->wParam, (LPARAM)HAI_COMBDWORD(y, x));
			pEvent->EventType = EVT_NULL;
		}
		else
		{
			pCtrl = (SP_CTRL)hai_GetSpecifiedCtrl(x, y);
			if (pCtrl)
			{
				(pCtrl->CtrlProc)((SH_CTRL)pCtrl, pEvent->EventType, pEvent->wParam, (LPARAM)HAI_COMBDWORD(y, x));
				pEvent->EventType = EVT_NULL;
			}
		}
		break;
	case EVT_MOUSEDB:
		x = HAI_GETLOWORD(pEvent->lParam);
		y = HAI_GETHIWORD(pEvent->lParam);
		pSceneR = hai_GetSceneRect(S_NULL, S_NULL, S_NULL, S_NULL);
		if (!hai_IsPointInRect(x, y, pSceneR))
			break;
		x -= pSceneR->x; y -= pSceneR->y;

		pCtrl = (SP_CTRL)hai_SetCapture(S_NULL);
		if (pCtrl)
		{
			(pCtrl->CtrlProc)((SH_CTRL)pCtrl, pEvent->EventType, pEvent->wParam, (LPARAM)HAI_COMBDWORD(y, x));
			pEvent->EventType = EVT_NULL;
		}
		break;
	case EVT_MOUSEMV:
		x = HAI_GETLOWORD(pEvent->lParam);
		y = HAI_GETHIWORD(pEvent->lParam);
		pSceneR = hai_GetSceneRect(S_NULL, S_NULL, S_NULL, S_NULL);
		if (!hai_IsPointInRect(x, y, pSceneR))
			break;
		x -= pSceneR->x; y -= pSceneR->y;

		pCtrl = (SP_CTRL)hai_SetCapture(S_NULL);
		if (pCtrl)
		{
			(pCtrl->CtrlProc)((SH_CTRL)pCtrl, pEvent->EventType, pEvent->wParam, (LPARAM)HAI_COMBDWORD(y, x));
			pEvent->EventType = EVT_NULL;
		}
		else
		{
			pCtrl = (SP_CTRL)hai_GetSpecifiedCtrl(x, y);
			if (pCtrl)
			{
				(pCtrl->CtrlProc)((SH_CTRL)pCtrl, pEvent->EventType, pEvent->wParam, (LPARAM)HAI_COMBDWORD(y, x));
				pEvent->EventType = EVT_NULL;
			}
		}
		break;
	case EVT_RESTORE:
		break;
	case EVT_COMMAND:
		break;
	default:
		break;
	}
}

S_VOID  hai_EventEndprocess(SP_EVT pEvent)
{
	static S_BYTE CaretCnt = 0;

	switch (pEvent->EventType)
	{
	case EVT_TIMER:
		if (CaretCnt++ >= hai_GetCaretBlinkTime())
		{
			_hai_BlinkCaret();
			CaretCnt = 0;
		}
		break;
	default:
		break;
	}
}


