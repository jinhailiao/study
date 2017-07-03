//---------------------------------------------------------------------------------
// Copyright (c) Haisoft 2006-9-1
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HGui
// File:			haiscene.h
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2006-09-01	Create					Kingsea
// 2.0.0		2006-09-06  update                  Kingsea
// 3.0.0		2008-03-01	update					Kingsea
//---------------------------------------------------------------------------------

#ifndef  _HAISCENE_H_
#define  _HAISCENE_H_

#include "haitype.h"

//
//  function : S_BYTE  hai_GetCurrentAppID(S_VOID);
//description: 返回当前场景的APPID号
S_BYTE  hai_GetCurrentAppID(S_VOID);

//
//  function : S_VOID  *hai_GetSceneData(S_DWORD size);
//description: 获取场景数据,可多次调用,只有在第一次调用时会分配内存
//             之后场景退出或须要时,调用hai_ReleaseSceneData()释放场景数据
S_VOID  *hai_GetSceneData(S_DWORD size);

//
//  function : S_BOOL  hai_ReleaseSceneData(S_VOID);
//description: 
S_BOOL  hai_ReleaseSceneData(S_VOID);

//
//  function : S_BOOL  hai_InvalidRect(SP_RECT pRect);
//description: pRect.x和pRect.y为屏幕绝对坐标
S_BOOL  hai_InvalidRect(SP_RECT pRect);

//
//  function : S_BOOL  hai_InvalidRectEx(S_WORD x, S_WORD y, S_WORD w, S_WORD h);
//description: 参数x, y为屏幕绝对坐标
S_BOOL  hai_InvalidRectEx(S_WORD x, S_WORD y, S_WORD w, S_WORD h);

//
//  function : S_BOOL  hai_UpdateWindow(S_WORD x, S_WORD y, S_WORD w, S_WORD h);
//description: 参数x, y为屏幕绝对坐标
S_BOOL  hai_UpdateWindow(S_WORD x, S_WORD y, S_WORD w, S_WORD h);

//
//  function : S_BOOL hai_SetSceneIME(S_BOOL InputFunction);
//description: 参数InputFunction取值为INPUTMETHOD_DISABLE, INPUTMETHOD_ENABLE 或INPUTMETHOD_GET
S_BOOL hai_SetSceneIME(S_BYTE InputFunction);

//
//  function : SP_RECT hai_GetSceneRect(S_WORD *x, S_WORD *y, S_WORD *w, S_WORD *h);
//description: 参数x, y, w, h如不须要,可传S_NULL;
//             返回场景矩形指针;
SP_RECT hai_GetSceneRect(S_WORD *x, S_WORD *y, S_WORD *w, S_WORD *h);

#endif

