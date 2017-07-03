//---------------------------------------------------------------------------------
// Copyright (c) Haisoft 2006-9-1
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HGui
// File:			desktop.c
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2006-09-01	Create					Kingsea
// 3.0.0		2008-03-01	update					Kingsea
//---------------------------------------------------------------------------------

#include "schedule.h"
#include "haievent.h"
#include "stdio.h"
#include "haigdi.h"
#include "haitime.h"
#include "haihandle.h"
#include "bmpicon.h"
#include "virkey.h"
#include "haicaret.h"
#include "haimem.h"
#include "AppIDs.h"
#include "haictrl.h"
#include "haigui.h"
#include "haiscene.h"
#include "msgbox.h"
#include "dialog.h"


DEFINESCENE(SID_CTRLDEMO1)
MSGMAPBEGIN
	ONCREAT(Ctrl1OnCreat)
	ONMSG(EVT_PAINT, Ctrl1OnPaint)
	ONMSG(EVT_COMMAND, Ctrl1OnCommand)
	ONKEYDN(Ctrl1OnKey)
	ONMSG(EVT_CTRLCOLOR, Ctrl1OnCtrlColr)
	ONMSG(EVT_EXIT, Ctrl1OnExit)
	ONDESTROY(Ctrl1OnDestroy)
MSGMAPEND

#define C1_TEXTC_BLACK		0
#define C1_TEXTC_RED		1
#define C1_TEXTC_GREEN		2
#define C1_TEXTC_BLUE		3

#define C1_LINE_PERPAGE		11

S_COLOR TEXTC[] = {0x000000, 0xFF0000, 0x00FF00, 0x0000FF};
S_BYTE *DemoText[] =
{
	"01.在每一天我在流连",
	"02.这心漂泊每朝每夜",
	"03.多么想找到愿意相随同伴",
	"04.使这心莫再漂泊",
	"05.愿那一天你来临时",
	"06.轻轻给我你的接受",
	"07.给我知道眼眸里的人",
	"08.承诺的一切永没改变",
	"09.多少期望多少梦",
	"10.皆因心里多孤寂",
	"11.即使期望多飘渺",
	"12.期望已能令我跨进未来！",
	"13.没有得到我愿寻求",
	"14.得到的怎么不接受",
	"15.尽管想拥有但却只能期待",
	"16.始终只醉在心里",
	"17.丝丝期望渐飘渺",
	"18.编织千寸心里梦",
	"19.即使希望似梦幻",
	"20.人渐醉在梦里海市蜃楼",
	"21.让我编织海市蜃楼",
	"22.一天一天浅醉一生",
};

typedef struct tagCtrl1Dat
{
	S_BYTE PreColor;
	S_BYTE textColor;
	S_SHORT hl;
	S_DWORD totalLine;
	S_DWORD start;
	SH_CTRL hScrlBar;
}
S_CTRL1DAT, *SP_CTRL1DAT;

static S_VOID Ctrl1ShowText(SH_DC hDC, S_BYTE **pptext, S_SHORT start, S_SHORT total, S_SHORT x, S_SHORT y, S_SHORT rowSpace)
{
	S_SHORT i;
	SP_CTRL1DAT pcd = hai_GetSceneData(0);

	for (i = 0;i < total; i++)
	{
		hai_TextOut(hDC, (S_SHORT)(x+10), (S_SHORT)(y+i*rowSpace), pptext[start+i], 0);
	}
	hai_TextOut(hDC, x, (S_SHORT)(y+pcd->hl*rowSpace), "*", 0);
}
static void  Ctrl1OnCreat(SP_EVT pEvent)
{
	S_BYTE Welcome[40] = {"欢迎"};
	SP_CTRL1DAT pcd = hai_GetSceneData(sizeof(*pcd));

	strcat(Welcome, (S_BYTE *)pEvent->lParam);
	hai_CreateStatic(CS_TEXT_MIDDLE|CS_TEXT_CENTER, 1, 160, 6, 160, 20, Welcome, S_NULL);
	hai_CreateButton(BS_CHECKBTN, 2, 20, 30, 100, 18, "我是复选按钮.", 0, 0, 0);
	hai_CreateButton(BS_CHECKBTN, 3, 20, 50, 100, 18, "请选择!", 0, 0, 0);
	hai_CreateButton(BS_CHECKBTN, 4, 20, 70, 100, 18, "请点击我...", 0, 0, 0);
	hai_CreateButton(BS_CHECKBTN, 5, 20, 90, 100, 18, "谢谢!", 0, 0, 0);
	hai_CreateStatic(CS_TEXT_MIDDLE|CS_TEXT_CENTER, 6, 20, 140, 100, 20, "请选择文本颜色:", S_NULL);
	hai_CreateButton(BS_RADIOBTN, 7, 30, 164, 80, 18, "黑色...", 0, 0, 0);
	hai_CreateButton(BS_RADIOBTN, 8, 30, 184, 80, 18, "红色...", 0, 0, 0);
	hai_CreateButton(BS_RADIOBTN, 9, 30, 204, 80, 18, "绿色...", 0, 0, 0);
	hai_CreateButton(BS_RADIOBTN, 10,30, 224, 80, 18, "蓝色...", 0, 0, 0);
	hai_CreateButton(BS_PUSHBTN, 11, 116, 244, 50, 20, "应用", 0, 0, 0);
	hai_CreateListArea(0, 13, 202, 42, 248, 220, 20);
	pcd->textColor = C1_TEXTC_BLACK;
	pcd->PreColor  = C1_TEXTC_BLACK;
	pcd->hl        = 0;
	pcd->totalLine = 22;
	pcd->start     = 0;
	pcd->hScrlBar  = hai_CreateScrlBar(BS_VSCRLBAR, 12, 453, 41, 20, 220, pcd->totalLine, pcd->start, C1_LINE_PERPAGE);
	hai_ShowAllCtrl();
	hai_CheckRadioButton(hai_GetCurSceneCtrl(7));
}

static void  Ctrl1OnPaint(SP_EVT pEvent)
{
	S_DWORD textStart;
	SH_DC hDC;
	SH_BRUSH hBrush;
	S_PAINTSTRUCT Paint;
	SP_CTRL1DAT pcd = hai_GetSceneData(sizeof(*pcd));

	hDC = hai_BeginPaint(&Paint);
	hBrush = (SH_BRUSH)hai_GetStockObject(LTGRAY_BRUSH);
	hai_SelectObject(hDC, (SH_GDIOBJ)hBrush);
	hai_BitBlt(hDC, 0, 0, LCD_WIDTH, LCD_HEIGHT, S_NULL, 0, 0, WHITENESS);
	hai_BitBlt(hDC, 2, 2, LCD_WIDTH-4, LCD_HEIGHT-4, S_NULL, 0, 0, PATCOPY);
	hai_Draw3DBlock(hDC, 200, 40, 270, 222, S_FALSE, WHITE_BRUSH);
	hai_SetBkMode(hDC, BKM_TRANSPARENT);
	hai_SetTextColor(hDC, TEXTC[pcd->textColor]);
	hai_GetScrlBarInfo(pcd->hScrlBar, &textStart, S_NULL, S_NULL);
	Ctrl1ShowText(hDC, DemoText, (S_SHORT)textStart, 11, 208, 44, 20);
	hai_EndPaint(&Paint);
}

static void  Ctrl1OnCtrlColr(SP_EVT pEvent)
{
	switch (pEvent->wParam)
	{
	case 7:
	case 8:
	case 9:
	case 10:
		hai_SetTextColor((SH_DC)pEvent->lParam, TEXTC[pEvent->wParam-7]);
		break;
	default:
		break;
	}
}


static void  Ctrl1OnCommand(SP_EVT pEvent)
{
	S_BYTE CtrlID = HAI_GETLOBYTE(pEvent->wParam);
	S_BYTE Event  = HAI_GETHIBYTE(pEvent->wParam);
	S_WORD textStart;
	S_DWORD temp;
	SH_DC hDC;
	SP_CTRL1DAT pcd = hai_GetSceneData(sizeof(*pcd));

	switch (CtrlID)
	{
	case 7:
	case 8:
	case 9:
	case 10:
		if (Event == BS_CHECKBTN_CHECKED)
			pcd->PreColor = CtrlID-7;
		break;
	case 11:
		if (Event == BS_PUSHBTN_UP)
		{
			if (pcd->textColor != pcd->PreColor)
			{
				pcd->textColor = pcd->PreColor;
				hai_InvalidRect(S_NULL);
			}
		}
		break;
	case 12:
		hai_GetScrlBarInfo(pcd->hScrlBar, &temp, S_NULL, S_NULL);
		hDC = hai_GetSceneDC();
		textStart = (S_WORD)temp;
		switch (Event)
		{
		case SB_BOTTOM:
			if (textStart != pcd->totalLine - C1_LINE_PERPAGE)
			{
				hai_BitBlt(hDC, 204, 44, 240, 214, S_NULL, 0, 0, WHITENESS);
				hai_SetBkMode(hDC, BKM_TRANSPARENT);
				hai_SetTextColor(hDC, TEXTC[pcd->textColor]);
				Ctrl1ShowText(hDC, DemoText, (S_SHORT)(pcd->totalLine - C1_LINE_PERPAGE), 11, 208, 44, 20);
				hai_SetScrlBarInfo(pcd->hScrlBar, pcd->totalLine - C1_LINE_PERPAGE, pcd->totalLine, C1_LINE_PERPAGE);
				hai_UpdateWindow(200, 40, 270, 222);
			}
			break;
		case SB_LINEDOWN:
			if (textStart < pcd->totalLine - C1_LINE_PERPAGE)
			{
				hai_BitBlt(hDC, 204, 44, 240, 214, S_NULL, 0, 0, WHITENESS);
				hai_SetBkMode(hDC, BKM_TRANSPARENT);
				hai_SetTextColor(hDC, TEXTC[pcd->textColor]);
				Ctrl1ShowText(hDC, DemoText, (S_SHORT)(textStart+1), 11, 208, 44, 20);
				hai_SetScrlBarInfo(pcd->hScrlBar, textStart+1, pcd->totalLine, C1_LINE_PERPAGE);
				hai_UpdateWindow(200, 40, 270, 222);
			}
			break;
		case SB_LINEUP:
			if (textStart > 0)
			{
				hai_BitBlt(hDC, 204, 44, 240, 214, S_NULL, 0, 0, WHITENESS);
				hai_SetBkMode(hDC, BKM_TRANSPARENT);
				hai_SetTextColor(hDC, TEXTC[pcd->textColor]);
				Ctrl1ShowText(hDC, DemoText, (S_SHORT)(textStart-1), 11, 208, 44, 20);
				hai_SetScrlBarInfo(pcd->hScrlBar, textStart-1, pcd->totalLine, C1_LINE_PERPAGE);
				hai_UpdateWindow(200, 40, 270, 222);
			}
			break;
		case SB_PAGEDOWN:
			if (textStart  < pcd->totalLine - C1_LINE_PERPAGE)
			{
				if ((S_DWORD)(textStart+2*C1_LINE_PERPAGE) <= pcd->totalLine)
					textStart += C1_LINE_PERPAGE;
				else
					textStart  = (S_WORD)(pcd->totalLine - C1_LINE_PERPAGE);
				hai_BitBlt(hDC, 204, 44, 240, 214, S_NULL, 0, 0, WHITENESS);
				hai_SetBkMode(hDC, BKM_TRANSPARENT);
				hai_SetTextColor(hDC, TEXTC[pcd->textColor]);
				Ctrl1ShowText(hDC, DemoText, textStart, 11, 208, 44, 20);
				hai_SetScrlBarInfo(pcd->hScrlBar, textStart, pcd->totalLine, C1_LINE_PERPAGE);
				hai_UpdateWindow(200, 40, 270, 222);
			}
			break;
		case SB_PAGEUP:
			if (textStart > 0)
			{
				if (textStart >= C1_LINE_PERPAGE)
					textStart -= C1_LINE_PERPAGE;
				else
					textStart  = 0;
				hai_BitBlt(hDC, 204, 44, 240, 214, S_NULL, 0, 0, WHITENESS);
				hai_SetBkMode(hDC, BKM_TRANSPARENT);
				hai_SetTextColor(hDC, TEXTC[pcd->textColor]);
				Ctrl1ShowText(hDC, DemoText, textStart, 11, 208, 44, 20);
				hai_SetScrlBarInfo(pcd->hScrlBar, textStart, pcd->totalLine, C1_LINE_PERPAGE);
				hai_UpdateWindow(200, 40, 270, 222);
			}
			break;
		case SB_THUMBPOSITION:
		case SB_THUMBTRACK:
			hai_BitBlt(hDC, 204, 44, 240, 214, S_NULL, 0, 0, WHITENESS);
			hai_SetBkMode(hDC, BKM_TRANSPARENT);
			hai_SetTextColor(hDC, TEXTC[pcd->textColor]);
			Ctrl1ShowText(hDC, DemoText, (S_SHORT)(pEvent->lParam), 11, 208, 44, 20);
			hai_SetScrlBarInfo(pcd->hScrlBar, pEvent->lParam, pcd->totalLine, C1_LINE_PERPAGE);
			hai_UpdateWindow(200, 40, 270, 222);
			break;
		case SB_TOP:
			if (textStart != 0)
			{
				hai_BitBlt(hDC, 204, 44, 240, 214, S_NULL, 0, 0, WHITENESS);
				hai_SetBkMode(hDC, BKM_TRANSPARENT);
				hai_SetTextColor(hDC, TEXTC[pcd->textColor]);
				Ctrl1ShowText(hDC, DemoText, 0, 11, 208, 44, 20);
				hai_SetScrlBarInfo(pcd->hScrlBar, 0, pcd->totalLine, C1_LINE_PERPAGE);
				hai_UpdateWindow(200, 40, 270, 222);
			}
			break;
		default:
			break;
		}
		hai_DeleteDC(hDC);
		break;
	case 13:
		if (EVT_MOUSEDN == Event)
		{
			hDC = hai_GetSceneDC();
			hai_SetTextColor(hDC, TEXTC[pcd->textColor]);
			hai_TextOut(hDC, 208, (S_SHORT)(44+pcd->hl*20), " ", 0);
			hai_UpdateWindow(208, (S_SHORT)(44+pcd->hl*20), 6, 12);
			pcd->hl = (S_BYTE)(pEvent->lParam);
			hai_TextOut(hDC, 208, (S_SHORT)(44+pcd->hl*20), "*", 0);
			hai_DeleteDC(hDC);
			hai_UpdateWindow(208, (S_SHORT)(44+pcd->hl*20), 6, 12);
		}
		break;
	default:
		break;
	}
}

static void  Ctrl1OnKey(SP_EVT pEvent)
{
	SP_CTRL1DAT pcd = hai_GetSceneData(sizeof(*pcd));
	switch (pEvent->wParam)
	{
	case VK_PRIOR:
	case VK_NEXT:
	case VK_END:
	case VK_HOME:
	case VK_UP:
	case VK_DOWN:
		hai_SendCtrlEvt(pcd->hScrlBar, pEvent->EventType, pEvent->wParam, pEvent->lParam);
		break;
	case VK_ESCAPE:
		hai_PostEvent(EVT_DESTROY, 0, 0);
		break;
	case VK_RETURN:
		break;

	case '2':
		hai_DestroyCtrl(hai_GetCurSceneCtrl(2));
		break;
	case '3':
		hai_DestroyCtrl(hai_GetCurSceneCtrl(3));
		break;

	default:
		break;
	}
}

static void  Ctrl1OnExit(SP_EVT pEvent)
{
	hai_ReleaseSceneData();
}

static void  Ctrl1OnDestroy(SP_EVT pEvent)
{
	hai_ReleaseSceneData();
	RETURNSCENE(200, 201);
}



DEFINESCENE(SID_CTRLDEMO2)
MSGMAPBEGIN
	ONCREAT(CtrlDemo2OnCreat)
	ONMSG(EVT_PAINT, CtrlDemo2OnPaint)
	ONKEYDN(CtrlDemo2OnKey)
	ONMSG(EVT_COMMAND, CtrlDemo2OnCommand)
	ONMSG(EVT_TIMER, CtrlDemo2OnTimer)
	ONMSG(EVT_EXIT, CtrlDemo2OnExit)
	ONDESTROY(CtrlDemo2OnDestroy)
MSGMAPEND

static const S_BYTE *PicPath[] =
{
	WINOS_RES_PATH"mm1.bmp",
	WINOS_RES_PATH"mm2.bmp",
	WINOS_RES_PATH"mm3.bmp",
	WINOS_RES_PATH"mm4.bmp",
	WINOS_RES_PATH"mm5.bmp"
};

typedef struct tagCtrlData
{
	S_BYTE step;
	S_BYTE PicID;
	S_SHORT bright;
	SH_BITMAP hBmp;
}
S_CTRLDAT, *SP_CTRLDAT;

static void  CtrlDemo2OnCreat(SP_EVT pEvent)
{
	SP_CTRLDAT pCtrlDat = hai_GetSceneData(sizeof(*pCtrlDat));

	pCtrlDat->bright = 0;
	pCtrlDat->PicID  = 0;
	pCtrlDat->step   = 0;
	pCtrlDat->hBmp = hai_LoadBitmap((S_BYTE*)PicPath[pCtrlDat->PicID]);
	hai_CreateEditBox(0,                   1, 44,  48, 150, 20, "ctrldemo2", 22);
	hai_CreateEditBox(BS_EDITBOX_UNDERLINE,2, 44,  78, 150, 20, "CTRLDEMO2", 22);
	hai_CreateEditBox(BS_EDITBOX_BORDER,   3, 44, 108, 150, 20, "好好学习",  22);
	hai_CreateEditBox(BS_EDITBOX_3DBOX,    4, 44, 138, 150, 20, "天天向上",  22);
	hai_CreateEvtArea(0, 5, 220, 50, 250, 210, 0);
	hai_CreateProgress(PBS_RULE|PBS_HPROGRESS, 6, 270,  12, 197, 20, 20, 0, 0, 19, S_NULL, S_NULL);
	hai_CreateProgress(PBS_SLOT|PBS_HPROGRESS, 7,  80, 174, 122, 12, 20, 0, 0, 19, S_NULL, S_NULL);
	hai_CreateProgress(CS_GRAYED|PBS_BLOCK|PBS_HPROGRESS, 8,  80, 194, 128, 24, 12, 0, 0, 11, S_NULL, S_NULL);
	hai_ShowAllCtrl();
	hai_SetProgressCurPos(hai_GetCurSceneCtrl(6), 10);
	hai_SetSceneIME(INPUTMETHOD_ENABLE);
}

static void  CtrlDemo2OnPaint(SP_EVT pEvent)
{
	S_BITMAP Bitmap;
	SH_DC hDC, hDCmem;
	S_PAINTSTRUCT Paint;
	SP_CTRLDAT pCtrlDat = hai_GetSceneData(sizeof(*pCtrlDat));

	hDC = hai_BeginPaint(&Paint);
	hai_BitBlt(hDC, 0, 0, LCD_WIDTH, LCD_HEIGHT, S_NULL, 0, 0, WHITENESS);
	hai_TextOut(hDC, 210, 16, "亮度调整:", 0);
	hai_MoveTo(hDC, 214, 40, S_NULL);
	hai_LineTo(hDC, 214, 40+216+12);
	hai_LineTo(hDC, 214+250+12, 40+216+12);
	hai_LineTo(hDC, 214+250+12, 40);
	hai_LineTo(hDC, 214, 40);

	hai_TextOut(hDC, 40,  30, "编辑框:", 0);
	hai_TextOut(hDC, 10,  52, "名称:", 0);
	hai_TextOut(hDC, 10,  82, "性别:", 0);
	hai_TextOut(hDC, 10, 112, "属性:", 0);
	hai_TextOut(hDC, 10, 142, "描述:", 0);
	hai_TextOut(hDC, 10, 172, "可控进度条:", 0);
	hai_TextOut(hDC, 10, 202, "静态进度条:", 0);

	hDCmem = hai_CreateCompatibleDC(hDC);
	hai_GetObject((SH_GDIOBJ)pCtrlDat->hBmp, sizeof(Bitmap), &Bitmap);
	hai_SelectObject(hDCmem, (SH_GDIOBJ)pCtrlDat->hBmp);
	hai_SetBrightVal(hDCmem, pCtrlDat->bright);
	hai_BitBlt(hDC, (S_SHORT)((220+((250-Bitmap.width)>>1))), (S_SHORT)(50+((216-Bitmap.height)>>1)), Bitmap.width, Bitmap.height, hDCmem, 0, 0, BRIGHTCOPY);
	hai_DeleteDC(hDCmem);

	hai_SetTextColor(hDC, (S_COLOR)COLOR_GRAY);
	hai_TextOut(hDC, 20,  10, "温馨提示:按F1可切换输入法!", 0);
	hai_TextOut(hDC, 40, 230, "温馨提示:点击图片可切换,", 0);
	hai_TextOut(hDC, 94, 250, "请试一下!", 0);
	hai_EndPaint(&Paint);
}

static void  CtrlDemo2OnKey(SP_EVT pEvent)
{
	SH_CTRL hFocus = hai_GetCurSceneFocusCtrl();
	S_BYTE CtrlID  = hai_GetCurSceneCtrlID(hFocus);

	switch (pEvent->wParam)
	{
	case VK_ESCAPE:
		hai_PostEvent(EVT_DESTROY, 0, 0);
		break;
	case VK_UP:
		if (CtrlID >= 1 && CtrlID <= 4)
		{
			if (CtrlID > 1)
				CtrlID--;
			else if (CtrlID == 1)
				CtrlID = 4;
			hFocus = hai_GetCurSceneCtrl(CtrlID);
			hai_SetFocusCtrl(hFocus);
		}
		break;
	case VK_DOWN:
		if (CtrlID >= 1 && CtrlID <= 4)
		{
			if (CtrlID < 4)
				CtrlID++;
			else if (CtrlID == 4)
				CtrlID = 1;
			hFocus = hai_GetCurSceneCtrl(CtrlID);
			hai_SetFocusCtrl(hFocus);
		}
		break;
	default:
		break;
	}

}

static void  CtrlDemo2OnCommand(SP_EVT pEvent)
{
	S_BYTE CtrlID  = HAI_GETLOBYTE(pEvent->wParam);
	S_BYTE Event   = HAI_GETHIBYTE(pEvent->wParam);
	S_SHORT bright = HAI_GETLOWORD(pEvent->lParam);
	SP_CTRLDAT pCtrlDat = hai_GetSceneData(sizeof(*pCtrlDat));

	switch (CtrlID)
	{
	case 5:
		if (Event == EVT_MOUSEUP)
		{
			if (pCtrlDat->PicID == 4)
				pCtrlDat->PicID = 0;
			else
				pCtrlDat->PicID++;
			if (pCtrlDat->hBmp)
				hai_DeleteObject((SH_GDIOBJ)pCtrlDat->hBmp);
			pCtrlDat->hBmp = hai_LoadBitmap((S_BYTE*)PicPath[pCtrlDat->PicID]);
			hai_InvalidRect(S_NULL);
		}
		break;
	case 6:
		if (bright < 10)
		{
			if (bright == 0) bright = 1;
			pCtrlDat->bright = -((10-bright)*28);
		}
		else if (bright == 10)
			pCtrlDat->bright = 0;
		else if (bright > 10)
			pCtrlDat->bright = (bright-10)*28;
		hai_InvalidRect(S_NULL);
		break;
	default:
		break;
	}
}

static void  CtrlDemo2OnExit(SP_EVT pEvent)
{
	SP_CTRLDAT pCtrlDat = hai_GetSceneData(sizeof(*pCtrlDat));

	if (pCtrlDat->hBmp)
		hai_DeleteObject((SH_GDIOBJ)(pCtrlDat->hBmp));
	hai_ReleaseSceneData();
}

static void  CtrlDemo2OnDestroy(SP_EVT pEvent)
{
	CtrlDemo2OnExit(pEvent);
	RETURNSCENE(100, 101);
}

static void  CtrlDemo2OnTimer(SP_EVT pEvent)
{
	S_BYTE pos;
	SP_CTRLDAT pCtrlDat = hai_GetSceneData(sizeof(*pCtrlDat));

	pCtrlDat->step++;

	if (!(pCtrlDat->step%5)) 
	{
		pos = pCtrlDat->step/5;
		if (pos > 11) 
		{
			pos = 0;
			pCtrlDat->step = 0;
		}
		hai_SetProgressCurPos(hai_GetCurSceneCtrl(8), (S_SHORT)pos);
	}
}

DEFINESCENE(SID_TEST)
MSGMAPBEGIN
	ONCREAT(TestOnCreat)
	ONMSG(EVT_PAINT, TestOnPaint)
	ONKEYDN(TestOnKey)
	ONMSG(EVT_COMMAND, TestOnCommand)
	ONMSG(EVT_TIMER, TestOnTimer)
	ONMSG(EVT_EXIT, TestOnExit)
	ONDESTROY(TestOnDestroy)
MSGMAPEND

static const S_BYTE *TestPic[] =
{
	WINOS_RES_PATH"WinosSim.ico",
	WINOS_RES_PATH"mfc.ico",
	WINOS_RES_PATH"iconpro.ico",
	WINOS_RES_PATH"mm4.bmp",
	WINOS_RES_PATH"mm5.bmp",
	WINOS_RES_PATH"rle4.bmp",
	WINOS_RES_PATH"rle8.bmp"
};

typedef struct tagTestData
{
	SH_ICON   hIco;
	SH_BITMAP hbbb;
	SH_BITMAP hBmp;
}
S_TESTDAT, *SP_TESTDAT;

static void  TestOnCreat(SP_EVT pEvent)
{
	SP_TESTDAT pDat = hai_GetSceneData(sizeof(*pDat));
	pDat->hBmp = hai_LoadBitmap((S_BYTE *)TestPic[6]);
	pDat->hIco = hai_LoadIcon((S_BYTE *)TestPic[1]);
}

static void  TestOnPaint(SP_EVT pEvent)
{
	S_BITMAP Bitmap;
	SH_DC hDC, hDCmem;
	S_PAINTSTRUCT Paint;
	SP_TESTDAT pDat = hai_GetSceneData(sizeof(*pDat));

	hDC = hai_BeginPaint(&Paint);
	hai_BitBlt(hDC, 0, 0, LCD_WIDTH, LCD_HEIGHT, S_NULL, 0, 0, WHITENESS);
	hDCmem = hai_CreateCompatibleDC(hDC);
	hai_GetObject((SH_GDIOBJ)pDat->hBmp, sizeof(Bitmap), &Bitmap);
	hai_SelectObject(hDCmem, (SH_GDIOBJ)pDat->hBmp);
	hai_BitBlt(hDC, 0, 0, Bitmap.width, Bitmap.height, hDCmem, 0, 0, SRCCOPY);
	hai_DeleteDC(hDCmem);
	
	hai_DrawIcon(hDC, 400, 100, pDat->hIco);
	hai_EndPaint(&Paint);
}

static void  TestOnKey(SP_EVT pEvent)
{
	switch (pEvent->wParam)
	{
	case VK_ESCAPE:
		hai_PostEvent(EVT_DESTROY, 0, 0);
		break;
	default:
		break;
	}
}

static void  TestOnCommand(SP_EVT pEvent)
{
}

static void  TestOnExit(SP_EVT pEvent)
{
	SP_TESTDAT pDat = hai_GetSceneData(sizeof(*pDat));

	if (pDat->hBmp)
		hai_DeleteObject((SH_GDIOBJ)(pDat->hBmp));
	if (pDat->hIco)
		hai_DeleteObject((SH_GDIOBJ)(pDat->hIco));
	hai_ReleaseSceneData();
}

static void  TestOnDestroy(SP_EVT pEvent)
{
	TestOnExit(pEvent);
	RETURNSCENE(100, 101);
}

static void  TestOnTimer(SP_EVT pEvent)
{
}

/*
	hai_MoveTo(hDC, 4, 4, S_NULL);
	hai_LineTo(hDC, 200, 4);
	hai_SetBkMode(hDC, BKM_TRANSPARENT);
	hPen = (SH_PEN)hai_SelectObject(hDC, (SH_GDIOBJ)hai_CreatePen(1, COLOR_BLACK, PS_DOT));
	hai_MoveTo(hDC, 4, 8, S_NULL);
	hai_LineTo(hDC, 200, 8);
	hPen = (SH_PEN)hai_SelectObject(hDC, (SH_GDIOBJ)hai_CreatePen(1, COLOR_BLACK, PS_DASHDOT));
	hai_DeleteObject((SH_GDIOBJ)hPen);
	hai_MoveTo(hDC, 4, 12, S_NULL);
	hai_LineTo(hDC, 200, 12);
	hPen = (SH_PEN)hai_SelectObject(hDC, (SH_GDIOBJ)hai_CreatePen(1, COLOR_BLACK, PS_DASH));
	hai_DeleteObject((SH_GDIOBJ)hPen);
	hai_MoveTo(hDC, 4, 20, S_NULL);
	hai_LineTo(hDC, 200, 20);
	hPen = (SH_PEN)hai_SelectObject(hDC, (SH_GDIOBJ)hai_CreatePen(1, COLOR_BLACK, PS_DASHDOTDOT));
	hai_DeleteObject((SH_GDIOBJ)hPen);
	hai_MoveTo(hDC, 4, 30, S_NULL);
	hai_LineTo(hDC, 200, 30);
	hPen = (SH_PEN)hai_SelectObject(hDC, (SH_GDIOBJ)hai_CreatePen(1, COLOR_BLACK, PS_DASHDASHDOT));
	hai_DeleteObject((SH_GDIOBJ)hPen);
	hai_MoveTo(hDC, 4, 40, S_NULL);
	hai_LineTo(hDC, 200, 40);
	hPen = (SH_PEN)hai_SelectObject(hDC, (SH_GDIOBJ)hai_CreatePen(1, COLOR_BLACK, PS_DOT));
	hai_DeleteObject((SH_GDIOBJ)hPen);
	hai_MoveTo(hDC, 4, 50, S_NULL);
	hai_LineTo(hDC, 200, 230);
	hai_MoveTo(hDC, 4, 60, S_NULL);
	hai_LineTo(hDC, 200, 60);
	hai_MoveTo(hDC, 4, 70, S_NULL);
	hai_LineTo(hDC, 200, 70);
	hai_MoveTo(hDC, 4, 80, S_NULL);
	hai_LineTo(hDC, 200, 80);
	hPen = (SH_PEN)hai_SelectObject(hDC, (SH_GDIOBJ)hai_GetStockObject(BLACK_PEN));
	hai_DeleteObject((SH_GDIOBJ)hPen);
	hai_SelectObject(hDC, hai_GetStockObject(WHITE_BRUSH));

	hai_Rectangle(hDC, 101, 101, 106, 200);
	c1 = hai_SetTextColor(hDC, COLOR_GRAY10);
	c2 = hai_SetBkColor(hDC, COLOR_GRAY00);
	hai_SetBkMode(hDC, BKM_TRANSPARENT);
	hai_TextOut(hDC, 20, 210, "英雄模范基本原理!英雄模范基本原理!英雄\n模范基本原理!英雄模范基本原理!", 0);
	hai_SetTextColor(hDC, c1);
	hai_SetBkColor(hDC, c2);
*/
