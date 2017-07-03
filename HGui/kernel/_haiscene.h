//---------------------------------------------------------------------------------
// Copyright (c) Haisoft 2006-9-1
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HGui
// File:			_haiscene.h
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2006-09-01	Create					Kingsea
// 2.0.0		2006-09-06  update                  Kingsea
// 3.0.0		2008-03-01	update					Kingsea
//---------------------------------------------------------------------------------

#ifndef  __HAISCENE_H__
#define  __HAISCENE_H__


#include  "haitype.h"
#include  "haievent.h"
#include  "_haictrl.h"

#define   SCENE_STK_MAX    100

typedef  S_VOID Hai_SceneProc(SP_EVT);
typedef  S_VOID (*pHai_SceneProc)(SP_EVT);

typedef struct  tagScene
{
	S_BYTE  AppID;
	S_BYTE  SceneID;
	S_BYTE  Reserved1;
	S_BYTE  InputEnable;
	S_RECT  SceneRect;
	S_CTRL  *pInputMethod;
	S_CTRL  *CtrlListHead;
	S_CTRL  *CtrlListEnd;
	S_VOID  *pSceneData;
	Hai_SceneProc  *pSceneProc;
}
S_SCENE, *SP_SCENE;

typedef struct  tagSceneStk
{
	S_CHAR  top;
	S_SCENE   Scene[SCENE_STK_MAX];
}
S_SCENESTK, *SP_SCENESTK;



S_VOID  hai_InitSceneStk(S_VOID);
S_BOOL  hai_IsSceneStkEmpty(S_VOID);
S_BOOL  hai_IsSceneStkFull(S_VOID);
S_BOOL  hai_PushSceneStk(SP_SCENE pScene);
S_BOOL  hai_PopSceneStk(SP_SCENE pScene);
S_BOOL  hai_DelSceneStkTop(S_VOID);
S_VOID  hai_SetScene(SP_SCENE pScene, S_BYTE AppID, S_BYTE SceneId, S_VOID *pSceneData, Hai_SceneProc *pSceneProc);
S_VOID  hai_DefSceneProc(SP_EVT pEvent);
SP_CTRL hai_GetCurSceneCtrlListHead(S_VOID);
SP_CTRL hai_GetCurSceneCtrlListEnd(S_VOID);
S_VOID  hai_SetCurSceneCtrlListHead(SP_CTRL pCtrlListHead);
S_VOID  hai_SetCurSceneCtrlListEnd(SP_CTRL pCtrlListEnd);
S_BOOL  hai_IsNeedRedraw(S_VOID);
S_BOOL  _hai_SetInvalidRect(SP_RECT pRect);
S_BOOL  _hai_SetInvalidRectEx(S_WORD x, S_WORD y, S_WORD w, S_WORD h);
SP_RECT  hai_GetInvalidRect(S_VOID);
SP_SCENE  hai_GetCurSceneAddr(S_VOID);
Hai_SceneProc *hai_GetCurSceneProc(S_VOID);
S_BOOL _hai_ObjectUpdateRect(S_WORD x, S_WORD y, S_WORD w, S_WORD h);
S_VOID  hai_InitPreSceneQueue(S_VOID);
S_BOOL  hai_IsPreSceneQueueEmpty(S_VOID);
S_BOOL  hai_IsPreSceneQueueFull(S_VOID);
S_BOOL  hai_InPreSceneQueue(SP_SCENE pScene);
S_BOOL  hai_OutPreSceneQueue(SP_SCENE pScene);


#endif
