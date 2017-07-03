//---------------------------------------------------------------------------------
// Copyright (c) Haisoft 2006-9-1
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HGui
// File:			haiwnd.c
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2006-09-01	Create					Kingsea
// 2.0.0		2006-09-06  update                  Kingsea
// 3.0.0		2008-03-01	update					Kingsea
//---------------------------------------------------------------------------------

#include "haitype.h"
#include "haievent.h"
#include "_haiscene.h"
#include "haimem.h"
#include "_haigdi.h"
#include "haigdi.h"
#include "haigui.h"


#define   PRESCNQ_LENGTH_MAX     10
//#define INVALIDRECT_NOREDRAW		-1

typedef struct  tagPreSceneQ
{
	S_BYTE   head;
	S_BYTE   tail;
	S_SCENE  PreScnQ[PRESCNQ_LENGTH_MAX];
}
S_PRESCNQ, *SP_PRESCNQ;


static S_SCENE		CurScene;
static S_SCENESTK	SceneStk;
static S_RECT		InvalidRect;
static S_PRESCNQ	PreSceneQ;

S_BOOL  hai_InvalidRectEx(S_WORD x, S_WORD y, S_WORD w, S_WORD h);

S_VOID  hai_InitPreSceneQueue(S_VOID)
{
	memset(&PreSceneQ, 0, sizeof(S_PRESCNQ));
}

S_BOOL  hai_IsPreSceneQueueEmpty(S_VOID)
{
	return (PreSceneQ.head == PreSceneQ.tail);
}

S_BOOL  hai_IsPreSceneQueueFull(S_VOID)
{
	return (PreSceneQ.head == ((PreSceneQ.tail+1) % PRESCNQ_LENGTH_MAX));
}

S_BOOL  hai_InPreSceneQueue(SP_SCENE pScene)
{
	if (hai_IsPreSceneQueueFull())
		return S_FALSE;

	PreSceneQ.PreScnQ[PreSceneQ.tail] = *pScene;
	PreSceneQ.tail = (PreSceneQ.tail+1) % PRESCNQ_LENGTH_MAX;
	return S_TRUE;
}

S_BOOL  hai_OutPreSceneQueue(SP_SCENE pScene)
{
	if (hai_IsPreSceneQueueEmpty())
		return S_FALSE;

	*pScene = PreSceneQ.PreScnQ[PreSceneQ.head];
	PreSceneQ.head = (PreSceneQ.head+1) % PRESCNQ_LENGTH_MAX;
	return S_TRUE;
}

S_VOID  hai_InitSceneStk(S_VOID)
{
	memset(&SceneStk, 0, sizeof(SceneStk));
}

S_BOOL  hai_IsSceneStkEmpty(S_VOID)
{
	return (SceneStk.top == 0);
}

S_BOOL  hai_IsSceneStkFull(S_VOID)
{
	return (SceneStk.top+1 == SCENE_STK_MAX);
}

S_BOOL  hai_PushSceneStk(SP_SCENE pScene)
{
	if (hai_IsSceneStkFull())
		return S_FALSE;
	
	SceneStk.top++;
	SceneStk.Scene[SceneStk.top] = *pScene;

	return S_TRUE;
}

S_BOOL  hai_PopSceneStk(SP_SCENE pScene)
{
	if (hai_IsSceneStkEmpty())
		return S_FALSE;

	*pScene = SceneStk.Scene[SceneStk.top];
	SceneStk.top--;

	return S_TRUE;
}

S_BOOL  hai_DelSceneStkTop(S_VOID)
{
	S_BYTE ScrnFlush;
	S_SCENE  Scene, *pScene;

	if (hai_IsSceneStkEmpty())
		return S_FALSE;

	pScene = hai_GetCurSceneAddr();
	Scene = *pScene;
	if (!hai_PopSceneStk(pScene))
	{
		*pScene = Scene;
		return S_FALSE;
	}

	ScrnFlush = hai_SetScreenFlush(SCREEN_FLUSH_OFF);
	_hai_DestroyAllCtrl(S_FALSE);
	_hai_DestroyInputMethod(pScene->pInputMethod);
	hai_SetScreenFlush(ScrnFlush);
	
	*pScene = Scene;

	return S_TRUE;
}

S_VOID  hai_SetScene(SP_SCENE pScene, S_BYTE AppID, S_BYTE SceneID, S_VOID *pSceneData, Hai_SceneProc *pSceneProc)
{
	memset(pScene, 0x00, sizeof(S_SCENE));

	pScene->AppID    = AppID;
	pScene->SceneID  = SceneID;
	pScene->pSceneData = pSceneData;
	pScene->pSceneProc = pSceneProc;
	pScene->SceneRect.x = 0;
	pScene->SceneRect.y = 0;
	pScene->SceneRect.w = LCD_WIDTH;
	pScene->SceneRect.h = LCD_HEIGHT;
}

SP_RECT hai_GetSceneRect(S_WORD *x, S_WORD *y, S_WORD *w, S_WORD *h)
{
	if (x) *x = CurScene.SceneRect.x;
	if (y) *y = CurScene.SceneRect.y;
	if (w) *w = CurScene.SceneRect.w;
	if (h) *h = CurScene.SceneRect.h;

	return &CurScene.SceneRect;
}

SP_SCENE  hai_GetCurSceneAddr(S_VOID)
{
	return &CurScene;
}

S_BYTE  hai_GetCurrentAppID(S_VOID)
{
	return CurScene.AppID;
}

S_VOID  *hai_GetSceneData(S_DWORD size)
{
	if (CurScene.pSceneData == S_NULL)
	{
		CurScene.pSceneData = hai_MemAlloc(size);
		if (CurScene.pSceneData)
			memset(CurScene.pSceneData, 0x00, size);
	}
	
	return CurScene.pSceneData;
}

S_BOOL  hai_ReleaseSceneData(S_VOID)
{
	if (CurScene.pSceneData == S_NULL)
		return S_FALSE;
	hai_MemFree(CurScene.pSceneData);
	CurScene.pSceneData = S_NULL;
	return S_TRUE;
}

SP_CTRL hai_GetCurSceneCtrlListHead(S_VOID)
{
	return CurScene.CtrlListHead;
}

SP_CTRL hai_GetCurSceneCtrlListEnd(S_VOID)
{
	return CurScene.CtrlListEnd;
}

S_VOID hai_SetCurSceneCtrlListHead(SP_CTRL pCtrlListHead)
{
	CurScene.CtrlListHead = pCtrlListHead;
}

S_VOID hai_SetCurSceneCtrlListEnd(SP_CTRL pCtrlListEnd)
{
	CurScene.CtrlListEnd = pCtrlListEnd;
}

Hai_SceneProc *hai_GetCurSceneProc(S_VOID)
{
	return CurScene.pSceneProc;
}

SP_CTRL hai_GetCurrentInputMethod(S_VOID)
{
	return CurScene.pInputMethod;
}

S_BOOL hai_CreateInputMethod(S_BYTE InputMethodType)
{
	if (CurScene.pInputMethod)
		return S_FALSE;

//	if (CurScene.InputEnable == INPUTMETHOD_ENABLE)
		CurScene.pInputMethod = _hai_CreateInputMethod(InputMethodType);

	return CurScene.pInputMethod? S_TRUE:S_FALSE;
}

S_BOOL hai_DestroyInputMethod(S_VOID)
{
	SP_CTRL pCtrl;
	S_BOOL ok = S_FALSE;
	
	if (CurScene.pInputMethod)
	{
		pCtrl = CurScene.pInputMethod;
		CurScene.pInputMethod = S_NULL;
		if (hai_IsNeedRedraw())
			_hai_SetInvalidRectEx(pCtrl->x, pCtrl->y, pCtrl->w, pCtrl->h);
		else
			hai_InvalidRectEx(pCtrl->x, pCtrl->y, pCtrl->w, pCtrl->h);
		ok = _hai_DestroyInputMethod(pCtrl);
	}

	return ok;
}

S_BOOL hai_SetSceneIME(S_BYTE InputFunction)
{
	S_BOOL old = CurScene.InputEnable;

	if (InputFunction==INPUTMETHOD_GET || old==InputFunction)
		return old;

	if (CurScene.InputEnable == INPUTMETHOD_ENABLE)
	{
		if (CurScene.pInputMethod)
			hai_DestroyInputMethod();
		CurScene.pInputMethod = S_NULL;
	}
	CurScene.InputEnable = InputFunction;

	return old;
}

S_BOOL  hai_IsNeedRedraw(S_VOID)
{
	return (InvalidRect.w != 0 && InvalidRect.h != 0);
}

S_BOOL  _hai_SetInvalidRect(SP_RECT pRect)
{
	if (pRect)
	{
		if (pRect->x >= LCD_WIDTH || pRect->y >= LCD_HEIGHT || pRect->w == 0 || pRect->h == 0)
			return S_FALSE;
		if (pRect->w > LCD_WIDTH-pRect->x)
			pRect->w = LCD_WIDTH-pRect->x;
		if (pRect->h > LCD_HEIGHT-pRect->y)
			pRect->h = LCD_HEIGHT-pRect->y;
	}
	
	if (!pRect)
	{
		InvalidRect.x = 0;
		InvalidRect.y = 0;
		InvalidRect.w = LCD_WIDTH;
		InvalidRect.h = LCD_HEIGHT;
	}
	else if (InvalidRect.w == 0 || InvalidRect.h == 0)
	{
		InvalidRect.x = pRect->x;
		InvalidRect.y = pRect->y;
		InvalidRect.w = pRect->w;
		InvalidRect.h = pRect->h;
	}
	else
	{
		hai_RectAdd(&InvalidRect, pRect, &InvalidRect);
	}
	return S_TRUE;
}

S_BOOL  _hai_SetInvalidRectEx(S_WORD x, S_WORD y, S_WORD w, S_WORD h)
{
	S_RECT rect;

	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;

	return _hai_SetInvalidRect(&rect);
}

S_BOOL  hai_InvalidRect(SP_RECT pRect)
{
	if (_hai_SetInvalidRect(pRect))
		return hai_SendEvent(EVT_PAINT, 0, 0);
	else
		return S_FALSE;
}

S_BOOL  hai_InvalidRectEx(S_WORD x, S_WORD y, S_WORD w, S_WORD h)
{
	S_RECT rect;

	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;

	return hai_InvalidRect(&rect);
}

S_BOOL  hai_UpdateWindow(S_WORD x, S_WORD y, S_WORD w, S_WORD h)
{
	S_RECT rect;

	rect.x = x;	rect.y = y;
	rect.w = w;	rect.h = h;

	if (_hai_SetInvalidRect(&rect))
		return _hai_UpdateRect();
	else
		return S_FALSE;
}

SP_RECT  hai_GetInvalidRect(S_VOID)
{
	return &InvalidRect;
}

S_BOOL _hai_ObjectUpdateRect(S_WORD x, S_WORD y, S_WORD w, S_WORD h)
{
	S_WORD sx, sy;

	hai_GetSceneRect(&sx, &sy, S_NULL, S_NULL);
	if (!hai_IsNeedRedraw())
		hai_UpdateWindow((S_WORD)(sx+x), (S_WORD)(sy+y), w, h);
	else
		_hai_SetInvalidRectEx((S_WORD)(sx+x), (S_WORD)(sy+y), w, h);

	return S_TRUE;
}

S_VOID  hai_DefSceneProc(SP_EVT pEvent)
{
	switch (pEvent->EventType)
	{
	case EVT_CREAT:
		break;
	case EVT_PAINT:
		break;	
	case EVT_TIMER:
		break;	
	case EVT_KEYDN:
		break;	
	case EVT_SYSKEY:
		break;
	case EVT_DESTROY:
		break;
	case EVT_MOUSEDN:
		break;
	case EVT_MOUSEUP:
		break;
	case EVT_MOUSEDB:
		break;
	case EVT_MOUSEMV:
		break;
	case EVT_RESTORE:
		break;
	case EVT_COMMAND:
		break;
	default:
		break;
	}
}
