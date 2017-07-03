//---------------------------------------------------------------------------------
// Copyright (c) Haisoft 2006-9-1
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HGui
// File:			schedule.c
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
#include "_haievent.h"
#include "_haiscene.h"
#include "haiscene.h"
#include "_schedule.h"
#include "haigdi.h"
#include "appids.h"

WPARAM	gwParam = 0;
LPARAM	glParam = 0;

static S_BYTE SceneOperate = 0;



S_VOID  hai_InitSceneOperateFlag(S_VOID)
{
	SceneOperate = SOP_NULL;
}
S_VOID  hai_SetSceneOperateFlag(S_BYTE flag)
{
	SceneOperate = flag;
}
S_BYTE  hai_GetSceneOperateFlag(S_VOID)
{
	return SceneOperate;
}

S_VOID  hai_SetDefaultAppAndScene(S_VOID)
{
	SP_SCENE  pScene = hai_GetCurSceneAddr();
	extern void hai_AppProcSID_HAILOGO(SP_EVT);
	
	gwParam = 0; glParam = 0;
	hai_SetScene(pScene, APPID_HAILOGO, SID_HAILOGO, S_NULL, hai_AppProcSID_HAILOGO);
	hai_InitSceneEvent(SOP_GOTOSCENE, 0, 0);
}

S_VOID  hai_Schedule(S_VOID)
{
	S_EVT   event;
	SP_SCENE  pScene = hai_GetCurSceneAddr();

	while (1)
	{
		while (1)
		{
			event = hai_GetEvent();

			if (event.EventType == EVT_QUIT)
				break;
			hai_EventPreprocess(&event);

			pScene->pSceneProc(&event);

			if (event.EventType == EVT_EXIT)
			{
				_hai_ReturnPreScene();//must successful
			}
			else if (event.EventType == EVT_DESTROY)
			{
				if (hai_GetSceneOperateFlag() == SOP_GOTOSCENE)
					hai_DelSceneStkTop();
			}
			else
				hai_EventEndprocess(&event);
		}
		hai_InitSceneEvent(hai_GetSceneOperateFlag(), gwParam, glParam);
		gwParam = 0;
		glParam = 0;
	}
}

S_VOID  hai_SwitchApp(S_BYTE AppID, S_BYTE SceneID, S_VOID *pSceneProc, WPARAM  wParam, LPARAM lParam)
{
	S_BOOL    ok;
	SP_SCENE  pScene = hai_GetCurSceneAddr();

	ok = hai_PushSceneStk(pScene);
	if (!ok)
		return;
	
	gwParam = wParam;
	glParam = lParam;

	hai_SetScene(pScene, AppID, SceneID, S_NULL, pSceneProc);
	hai_SetSceneOperateFlag(SOP_GOTOSCENE);
	_hai_SendQuitEvent(0, 0);
	return;
}
 
S_VOID  hai_GotoScene(S_VOID *pSceneProc, S_BYTE SceneID, WPARAM  wParam, LPARAM lParam)
{
	hai_SwitchApp(hai_GetCurrentAppID(), SceneID, pSceneProc, wParam, lParam);
}

S_VOID _hai_ReturnPreScene(S_VOID)
{
	S_BYTE ScrnFlush;
	SP_SCENE  pScene = hai_GetCurSceneAddr();

	ScrnFlush = hai_SetScreenFlush(SCREEN_FLUSH_OFF);
	_hai_DestroyAllCtrl(S_FALSE);
	_hai_DestroyInputMethod(pScene->pInputMethod);
	hai_SetScreenFlush(ScrnFlush);

	hai_PopSceneStk(pScene);
}

S_VOID  hai_ReturnPreScene(WPARAM  wParam, LPARAM lParam)
{
	if (hai_IsSceneStkEmpty())
		return;

	_hai_ReturnPreScene();

	gwParam = wParam;
	glParam = lParam;
	hai_SetSceneOperateFlag(SOP_RTNSCENE);
	_hai_SendQuitEvent(0, 0);
	return;
}

S_VOID  hai_PreGotoScene(S_BYTE AppID, S_BYTE SceneID, S_VOID *pSceneProc, WPARAM  wParam, LPARAM lParam)
{
	S_SCENE  Scene;

	if (SceneID  == SID_NULL)
	{
		hai_InitPreSceneQueue();
		return;
	}
	
	memset(&Scene, 0x00, sizeof(Scene));
	Scene.AppID = AppID;
	Scene.SceneID = SceneID;
	Scene.pSceneProc = pSceneProc;
	
	if (!hai_InPreSceneQueue(&Scene))
		return;
	
	gwParam = wParam;
	glParam = lParam;

	return;
}

S_VOID hai_CallPreGotoScene(S_VOID)
{
	S_SCENE scene;
	SP_SCENE pScene = hai_GetCurSceneAddr();

	while (!hai_IsPreSceneQueueEmpty())
	{
		hai_OutPreSceneQueue(&scene);
		if (scene.AppID != pScene->AppID || scene.SceneID != pScene->SceneID)
		{
			if (hai_PushSceneStk(pScene))
			{
				*pScene = scene;
			}
		}
	}

	hai_SetScreenFlush(SCREEN_FLUSH_ON);
	if (pScene->SceneID == SID_DESKTOP)
		hai_SendEvent(EVT_RESTORE, gwParam, glParam);
	else
		hai_SendEvent(EVT_CREAT, gwParam, glParam);
	hai_InvalidRect(S_NULL);
	
	gwParam = 0, glParam = 0;

	return;
}



