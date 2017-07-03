//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			HDJS
// File:			uiman.cpp
// Description:		UI manager
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2010-01-18  Create                  Kingsea
//---------------------------------------------------------------------------------
#include "uiman.h"
#include "htrace.h"
#include "context.h"
#include "haicfg.h"
#include "sguiapp.h"
#include "sguimsg.h"
#include "sguidc.h"
#include "sguictrl.h"
#include "sguidlog.h"
#include "sysipc.h"

C_UIMAPP  theUIMApp;
C_DESKTOP theDesktop;

#define CLOSE_LIGHT_SECOND	30
#define BACK_DESKOP_SECOND	60
#define SYS_FLUSH_TIMEOUT	4

int C_UIMAPP::TranslateMsg(S_GUIMSG &msg)
{
	static int CloseLight = -1;
	static int BackDeskop = -1;
	static int SysFlush   = SYS_FLUSH_TIMEOUT;
	switch (msg.Msg)
	{
	case GM_KEYDN:
	case GM_KEYUP:
		CloseLight = CLOSE_LIGHT_SECOND;
		BackDeskop = BACK_DESKOP_SECOND;
		OpenBacklight();
		break;
	case GM_TIME:
		if (CloseLight > 0)
		{
			CloseLight--;
		}
		else if (CloseLight == 0)
		{
			CloseBacklight();
			CloseLight = -1;
		}
		if (BackDeskop > 0)
		{
			BackDeskop--;
		}
		else if (BackDeskop == 0)
		{
			if (EmptyWndStack() == 0)
				msg.pWnd = NULL, msg.Msg = GM_UNKNOW;
			BackDeskop = BACK_DESKOP_SECOND;
		}
		SysFlush--;
		if (SysFlush <= 0)
		{
			S_GUIMSG aMsg;
			aMsg.Msg = GM_SYSPAINT;
			m_MsgQ.push(aMsg);
			SysFlush = SYS_FLUSH_TIMEOUT;
		}
		break;
	default:
		break;
	}
	return C_SGUIAPP::TranslateMsg(msg);
}

int C_UIMWND::DefWndProcess(S_WORD msg, S_WORD wParam, S_DWORD lParam)
{
	switch (msg)
	{
	case GM_SYSPAINT:{
		char asct[64];
		bool en = theUIMApp.FlushScreenEn(false);
		C_SGUIDC dc(NULL);
		time_t current = time(NULL);
		tm *ptm = localtime(&current);
		sprintf(asct, " .il  G  23 !        %02d:%02d", ptm->tm_hour, ptm->tm_min);
		dc.DrawString(1, 1, asct);
		theUIMApp.FlushScreenEn(en);
		}
		break;
	default:
		return C_SGUIWND::DefWndProcess(msg, wParam, lParam);
	}
	return 0;
}

int C_CTRLWND::WndProcess(S_WORD msg, S_WORD wParam, S_DWORD lParam)
{
	switch (msg)
	{
	case GM_CREATE:{
		S_RECT rect1 = {10, 20, 34, 16};
		S_RECT rect2 = {50, 20, 34, 16};
		C_NUMEDIT *pNumEdit1 = new C_NUMEDIT;
		C_NUMEDIT *pNumEdit2 = new C_NUMEDIT;
		pNumEdit1->Create("89ABC", NUMEDIT_STYLE_HEX, rect1, this, 1);
		pNumEdit2->Create("89ABC", 0x00, rect2, this, 2);
		SetFocusCtrl(pNumEdit1);
		}
		break;
	case GM_PAINT:{
		C_SGUIDC *pdc = BeginPaint();
		EndPaint(pdc);
		}
		break;
	case GM_DESTROY:
		delete (GetWndCtrl(1));
		delete (GetWndCtrl(2));
		break;
	case GM_KEYUP:
		if (wParam == GVK_LEFT)
			SetFocusCtrl(GetWndCtrl(1));
		else if (wParam == GVK_RIGHT)
			SetFocusCtrl(GetWndCtrl(2));
		else if (wParam == GVK_CANCEL)
			SGuiApp->CloseWnd(NULL);
		else if (wParam == GVK_OK)
			SGui_MsgBox("提示", "  消息框弹出了！", 0x00);
		break;
	default:
		return DefWndProcess(msg, wParam, lParam);
	}
	return 0;
}

int C_MSGBOXWND::WndProcess(S_WORD msg, S_WORD wParam, S_DWORD lParam)
{
	switch (msg)
	{
	case GM_CREATE:
		break;
	case GM_PAINT:{
		C_SGUIDC *pdc = BeginPaint();
		EndPaint(pdc);
		}
		break;
	case GM_DESTROY:
		break;
	case GM_KEYUP:
		if (wParam == GVK_UP)
			SGui_MsgBox("提示", "  消息框弹出了！", MB_BTN_OK);
		else if (wParam == GVK_DN)
			SGui_MsgBox("提示", "  消息框弹出了！", MB_BTN_OKCANCEL);
		else if (wParam == GVK_CANCEL)
			SGuiApp->CloseWnd(NULL);
		else if (wParam == GVK_OK)
			SGui_MsgBox("提示", "  消息框弹出了！", 0x00);
		break;
	default:
		return DefWndProcess(msg, wParam, lParam);
	}
	return 0;
}

int C_MSGSENDWND::WndProcess(S_WORD msg, S_WORD wParam, S_DWORD lParam)
{
	switch (msg)
	{
	case GM_CREATE:{
		S_RECT rect1 = {60, 18, 70, 16};
		S_RECT rect2 = {10, 70, 34, 16};
		S_RECT rect3 = {10, 50, 140, 16};
		C_NUMEDIT *pNumEdit1 = new C_NUMEDIT;
		C_VKBOARD *pVk = new C_VKBOARD;
		C_TEXTEDIT *pEdit = new C_TEXTEDIT;
		pNumEdit1->Create("13560303396", NUMEDIT_STYLE_DEC, rect1, this, 1);
		pVk->Create(0x00, rect2, this, 2);
		pEdit->Create("                     ", 0x00, rect3, this, 3);
		SetFocusCtrl(pNumEdit1);
		}break;
	case GM_PAINT:{
		C_SGUIDC *pdc = BeginPaint();
		pdc->DrawString(10, 2, "虚拟键盘的\"Ent\"发送消息");
		pdc->DrawString(10, 20, "手机号:");
		pdc->DrawString(10, 36, "发送内容:");
		EndPaint(pdc);
		}
		break;
	case GM_DESTROY:
		delete (GetWndCtrl(1));
		delete (GetWndCtrl(2));
		delete (GetWndCtrl(3));
		break;
	case GM_KEYUP:
		if (wParam == GVK_OK)
			SetFocusCtrl(GetWndCtrl(2));
		break;
	case GM_CHAR:
		if (wParam == GVK_ENTER)
		{
			string msg;
			C_GUICTRL *pCtrl = GetWndCtrl(1);
			msg.append(pCtrl->GetWndText());
			pCtrl = GetWndCtrl(3);
			msg.append(pCtrl->GetWndText());
			C_SYSIPC::Send2Server(HDJS_SERID_PPP, C_SYSIPC::SYSIPC_TYPE_MSG_SEND, msg);
		}
		else if (wParam == GVK_ESC)
			SetFocusCtrl(GetWndCtrl(1));
		else if (wParam == GVK_F1)
			;
		else
		{
			C_GUICTRL *pCtrl = GetWndCtrl(3);
			pCtrl->SendWndMsg(GM_CHAR, wParam, lParam);
		}
		break;
	case GM_TIME:{
		int SerID;S_BYTE MsgT;string msg;
		if ((C_SYSIPC::Query4Server(SerID, MsgT, msg) >= 0) && MsgT == C_SYSIPC::SYSIPC_TYPE_MSG_SEND)
		{
			string strDA, strUD;
			strDA.append(msg.begin(), msg.begin()+14);
			strUD.assign(msg.begin()+14, msg.end());
			SGui_MsgBox(strDA.c_str(), strUD.c_str(), MB_BTN_OK);
		}
		}break;
	default:
		return DefWndProcess(msg, wParam, lParam);
	}
	return 0;
}

int C_VKWND::WndProcess(S_WORD msg, S_WORD wParam, S_DWORD lParam)
{
	switch (msg)
	{
	case GM_CREATE:{
		S_RECT rect1 = {10, 70, 34, 16};
		S_RECT rect2 = {12, 10, 134, 16};
		C_VKBOARD *pVk = new C_VKBOARD;
		C_TEXTEDIT *pEdit = new C_TEXTEDIT;
		pVk->Create(0x00, rect1, this, 1);
		pEdit->Create("          ", 0x00, rect2, this, 2);
		SetFocusCtrl(pVk);
		}
		break;
	case GM_PAINT:{
		C_SGUIDC *pdc = BeginPaint();
		EndPaint(pdc);
		}
		break;
	case GM_DESTROY:
		delete (GetWndCtrl(1));
		delete (GetWndCtrl(2));
		break;
	case GM_KEYUP:
		if (wParam == GVK_OK)
		{
			SetFocusCtrl(GetWndCtrl(1));
		}
		else if (wParam == GVK_CANCEL)
		{
			SGuiApp->CloseWnd(NULL);
		}
		break;
	case GM_CHAR:
		if (wParam == GVK_ENTER)
		{
		}
		else if (wParam == GVK_ESC)
		{
			SetFocusCtrl(GetWndCtrl(2));
		}
		else if (wParam == GVK_F1)
		{
		}
		else
		{
			C_GUICTRL *pCtrl = GetWndCtrl(2);
			pCtrl->SendWndMsg(GM_CHAR, wParam, lParam);
		}
		break;
	default:
		return DefWndProcess(msg, wParam, lParam);
	}
	return 0;
}



static char *WeekName[] =
{
	"星期日","星期一","星期二","星期三","星期四","星期五","星期六"
};
int C_DESKTOP::WndProcess(S_WORD msg, S_WORD wParam, S_DWORD lParam)
{
	switch (msg)
	{
	case GM_CREATE:
		break;
	case GM_PAINT:{
		int size;
		char strBuf[64];
		C_SGUIDC *pdc = BeginPaint();
		time_t current = time(NULL);
		tm *ptm = localtime(&current);
		pdc->SelectObject(pdc->GetStockGuiObj(SGUI_OBJ_F16x16));
		size = pdc->GetStringExtent("003台区集中器");
		pdc->DrawString((160-size)/2, 30, "003台区集中器");
		sprintf(strBuf, "%d-%02d-%02d", ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday);
		size = pdc->GetStringExtent(strBuf);
		pdc->DrawString((160-size)/2, 60, strBuf);
		size = pdc->GetStringExtent(WeekName[ptm->tm_wday]);
		pdc->DrawString((160-size)/2, 80, WeekName[ptm->tm_wday]);
		pdc->SelectObject(pdc->GetStockGuiObj(SGUI_OBJ_F12x12));
		pdc->DrawString(20, 130, "工作正常...");
		EndPaint(pdc);
		}
		break;
	case GM_KEYUP:
		loget<<"C_DESKTOP GM_KEYUP response."<<endl;
		GOTOWND(C_MENUWND, 0);
		break;
	default:
		return DefWndProcess(msg, wParam, lParam);
	}
	return 0;
}

struct S_UIMENU
{
	const S_CHAR *m_pMenu[8];
	S_UIMENU *m_pSubMenu[8];
	S_UIMENU *m_pPMenu;
	S_BYTE m_MenuID;
};

extern S_UIMENU MainMenu;
S_UIMENU GathQuery =
{
	{"1-1 测量点有功总电能", "1-2 测量点有功峰电能", "1-3 测量点有功平电能", "1-4 测量点有功谷电能", "", "", "", ""},
	{NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL},
	&MainMenu, 1
};
S_UIMENU ParamQuery =
{
	{"2-1 测量点参数查询", "2-2 通信参数查询", "2-3 抄表参数查询", "2-4 中继路由信息查询", "2-5 中继路由信息查询", "", "", ""},
	{NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL},
	&MainMenu, 2
};
S_UIMENU VIPQuery =
{
	{"3-1 重点户电能示值", "3-2 重点户有功曲线", "3-3 重点户无功曲线", "3-4 重点户电流曲线", "3-5 重点户电压曲线", "", "", ""},
	{NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL},
	&MainMenu, 3
};
S_UIMENU StatQuery =
{
	{"4-1 抄表统计", "4-2 抄表失败清单", "4-3 数据通信统计", "4-4 中继路由信息统计", "", "", "", ""},
	{NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL},
	&MainMenu, 4
};
S_UIMENU ParamSet =
{
	{"5-1 通信参数设置", "5-2 数字编辑框", "5-3 文本编辑框", "5-4 消息框", "5-5 发送短消息", "", "", ""},
	{NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL},
	&MainMenu, 5
};
S_UIMENU Debug =
{
	{"6-1 实时抄表", "6-2 表号搜索", "6-3 新增电能表地址", "6-4 未搜到电能表地址", "6-5 主动上报", "", "", ""},
	{NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL},
	&MainMenu, 6
};
S_UIMENU MainMenu =
{
	{"1、抄表查询", "2、参数查询", "3、重点用户", "4、统计查询", "5、参数设置", "6、现场调试", "", ""},
	{&GathQuery,&ParamQuery,&VIPQuery,&StatQuery,&ParamSet,&Debug,NULL,NULL},
	NULL, 0
};

struct S_MENUWDATA
{
	S_UIMENU *m_pCurMenu;
	int m_Select;
	int m_TotalM;
};
int C_MENUWND::WndProcess(S_WORD msg, S_WORD wParam, S_DWORD lParam)
{
	S_MENUWDATA *pData = (S_MENUWDATA *)GetWndData(sizeof(S_MENUWDATA));

	switch (msg)
	{
	case GM_CREATE:
		pData->m_pCurMenu = &MainMenu;
		pData->m_Select = 0;
		pData->m_TotalM = 0;
		break;
	case GM_PAINT:{
		C_SGUIDC *pdc = BeginPaint();
		for (int i = 0; pData->m_pCurMenu->m_pMenu[i][0]; i++, pData->m_TotalM=i)
			pdc->DrawString(18, i*18+20, pData->m_pCurMenu->m_pMenu[i]);
		S_RECT rect = {16, pData->m_Select*18+18, 130, 16};
		pdc->ReverseRect(rect);
		EndPaint(pdc);
		}
		break;
	case GM_KEYUP:
		if (wParam == GVK_DN)
		{
			pData->m_Select++;
			if (pData->m_Select >= pData->m_TotalM)
				pData->m_Select = 0;
			InvalidateRect(NULL);
		}
		else if (wParam == GVK_UP)
		{
			pData->m_Select--;
			if (pData->m_Select <= -1)
				pData->m_Select = pData->m_TotalM-1;
			InvalidateRect(NULL);
		}
		else if (wParam == GVK_CANCEL)
		{
			if (pData->m_pCurMenu->m_pPMenu)
			{
				pData->m_Select = pData->m_pCurMenu->m_MenuID - 1; 
				pData->m_pCurMenu = pData->m_pCurMenu->m_pPMenu;
				InvalidateRect(NULL);
			}
			else
			{
				logit<<"C_MENUWND Get key!"<<endl;
				SGuiApp->CloseWnd(this);
				pData->m_Select = 0;
			}
		}
		else if (wParam == GVK_OK)
		{
			if (pData->m_pCurMenu->m_pSubMenu[pData->m_Select] != NULL)
			{
				pData->m_pCurMenu = pData->m_pCurMenu->m_pSubMenu[pData->m_Select];
				pData->m_Select = 0;
				InvalidateRect(NULL);
			}
			else
			{
				if (pData->m_pCurMenu->m_MenuID == 5 && pData->m_Select == 1)
				{
					GOTOWND(C_CTRLWND, 0x00);
				}
				else if (pData->m_pCurMenu->m_MenuID == 5 && pData->m_Select == 2)
				{
					GOTOWND(C_VKWND, 0x00);
				}
				else if (pData->m_pCurMenu->m_MenuID == 5 && pData->m_Select == 3)
				{
					GOTOWND(C_MSGBOXWND, 0x00);
				}
				else if (pData->m_pCurMenu->m_MenuID == 5 && pData->m_Select == 4)
				{
					GOTOWND(C_MSGSENDWND, 0x00);
				}
			}
		}
		break;
	case GM_DESTROY:
		ReleaseWndData();
		break;
	default:
		return DefWndProcess(msg, wParam, lParam);
	}
	return 0;
}

C_UIMAN::C_UIMAN(void)
{
	m_pServInfo = &(CONTEXT.m_ServInfo[HDJS_SERID_UIMAN]);
}

C_UIMAN::~C_UIMAN()
{
}

int C_UIMAN::BeforeService(void)
{
	if (theUIMApp.SysInit() != 0)
		return -1;
	C_SGUIAPP::SetCurApp(&theUIMApp);
	theUIMApp.SetDesktopWnd(&theDesktop);

	C_SERVER::SetCurrentServId(HDJS_SERID_UIMAN);
	C_HTRACE::SetSystemTraceSetting(&CONTEXT.m_TraceSet[HDJS_SERID_UIMAN]);
	return 0;
}

int C_UIMAN::Servicing(void)
{
	S_GUIMSG aMsg;
	while (1)
	{
		HeartBeat();
		aMsg = theUIMApp.GetGuiMsg();
		theUIMApp.TranslateMsg(aMsg);
		theUIMApp.DispatchGuiMsg(aMsg);
	}
	return 0;
}

int C_UIMAN::AfterService(void)
{
	return 0;
}

