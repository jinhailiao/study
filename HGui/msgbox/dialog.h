//---------------------------------------------------------------------------------
// Copyright (c) Haisoft 2006-9-1
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HGui
// File:			dialog.h
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 3.0.0		2008-03-15	create					Kingsea
//---------------------------------------------------------------------------------
#ifndef __HG_DIALOG_H__
#define __HG_DIALOG_H__

#include "haitype.h"
#include "haievent.h"

//
// 这是用户对话框过程函数类型
//
typedef  S_VOID HG_DialogProc(SP_EVT);
typedef  S_VOID (*pHG_DialogProc)(SP_EVT);



//
//  function : S_VOID HG_DialogBoxPreproc(SP_EVT pEvt);
//description: This function is preprocess of Dialog CallBack,
//             在使用时请放在对话框过程消息处理的最前面.
//
S_VOID HG_DialogBoxPreproc(SP_EVT pEvt);

//
//  function : S_DWORD HG_DialogBox(S_WORD x, S_WORD y, S_WORD w, S_WORD h, HG_DialogProc *DlgProc);
//description: 对话框函数
//
S_DWORD HG_DialogBox(S_WORD x, S_WORD y, S_WORD w, S_WORD h, HG_DialogProc *DlgProc);

//
//  function : S_VOID  HG_EndDialogBox(S_DWORD ReturnVal);
//description: 在user dialog proc中放在EVT_DESTROY消息处理的最后,否则HG_Dialogbox函数
//             不会返回.ReturnVal是用户想要返回的值,这个值将由HG_Dialogbox返回.
//
S_VOID  HG_EndDialogBox(S_DWORD ReturnVal);



#endif //__HG_DIALOG_H__


