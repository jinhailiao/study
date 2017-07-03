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
// �����û��Ի�����̺�������
//
typedef  S_VOID HG_DialogProc(SP_EVT);
typedef  S_VOID (*pHG_DialogProc)(SP_EVT);



//
//  function : S_VOID HG_DialogBoxPreproc(SP_EVT pEvt);
//description: This function is preprocess of Dialog CallBack,
//             ��ʹ��ʱ����ڶԻ��������Ϣ�������ǰ��.
//
S_VOID HG_DialogBoxPreproc(SP_EVT pEvt);

//
//  function : S_DWORD HG_DialogBox(S_WORD x, S_WORD y, S_WORD w, S_WORD h, HG_DialogProc *DlgProc);
//description: �Ի�����
//
S_DWORD HG_DialogBox(S_WORD x, S_WORD y, S_WORD w, S_WORD h, HG_DialogProc *DlgProc);

//
//  function : S_VOID  HG_EndDialogBox(S_DWORD ReturnVal);
//description: ��user dialog proc�з���EVT_DESTROY��Ϣ��������,����HG_Dialogbox����
//             ���᷵��.ReturnVal���û���Ҫ���ص�ֵ,���ֵ����HG_Dialogbox����.
//
S_VOID  HG_EndDialogBox(S_DWORD ReturnVal);



#endif //__HG_DIALOG_H__


