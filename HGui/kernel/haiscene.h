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
//description: ���ص�ǰ������APPID��
S_BYTE  hai_GetCurrentAppID(S_VOID);

//
//  function : S_VOID  *hai_GetSceneData(S_DWORD size);
//description: ��ȡ��������,�ɶ�ε���,ֻ���ڵ�һ�ε���ʱ������ڴ�
//             ֮�󳡾��˳�����Ҫʱ,����hai_ReleaseSceneData()�ͷų�������
S_VOID  *hai_GetSceneData(S_DWORD size);

//
//  function : S_BOOL  hai_ReleaseSceneData(S_VOID);
//description: 
S_BOOL  hai_ReleaseSceneData(S_VOID);

//
//  function : S_BOOL  hai_InvalidRect(SP_RECT pRect);
//description: pRect.x��pRect.yΪ��Ļ��������
S_BOOL  hai_InvalidRect(SP_RECT pRect);

//
//  function : S_BOOL  hai_InvalidRectEx(S_WORD x, S_WORD y, S_WORD w, S_WORD h);
//description: ����x, yΪ��Ļ��������
S_BOOL  hai_InvalidRectEx(S_WORD x, S_WORD y, S_WORD w, S_WORD h);

//
//  function : S_BOOL  hai_UpdateWindow(S_WORD x, S_WORD y, S_WORD w, S_WORD h);
//description: ����x, yΪ��Ļ��������
S_BOOL  hai_UpdateWindow(S_WORD x, S_WORD y, S_WORD w, S_WORD h);

//
//  function : S_BOOL hai_SetSceneIME(S_BOOL InputFunction);
//description: ����InputFunctionȡֵΪINPUTMETHOD_DISABLE, INPUTMETHOD_ENABLE ��INPUTMETHOD_GET
S_BOOL hai_SetSceneIME(S_BYTE InputFunction);

//
//  function : SP_RECT hai_GetSceneRect(S_WORD *x, S_WORD *y, S_WORD *w, S_WORD *h);
//description: ����x, y, w, h�粻��Ҫ,�ɴ�S_NULL;
//             ���س�������ָ��;
SP_RECT hai_GetSceneRect(S_WORD *x, S_WORD *y, S_WORD *w, S_WORD *h);

#endif

