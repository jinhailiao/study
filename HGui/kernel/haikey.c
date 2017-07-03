//---------------------------------------------------------------------------------
// Copyright (c) Haisoft 2006-9-1
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HGui
// File:			haikey.c
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 2.0.0		2008-02-06  Create                  Kingsea
// 3.0.0		2008-03-01	update					Kingsea
//---------------------------------------------------------------------------------

#include "haitype.h"
#include "_haievent.h"
#include "haievent.h"
#include "virkey.h"
#include "haikey.h"



#define KEY_BUFFER_LENGTH_MAX		10
#define KEY_DOWN_FLAG				0x8000


static S_BYTE gUserInputEn = USERIP_ENABLE;

static volatile S_WORD KeyBuffer[KEY_BUFFER_LENGTH_MAX];
static volatile S_BYTE KeyCnt = 0;

S_BYTE HG_SetUserInputFunction(S_BYTE ip)
{
	S_BYTE back = gUserInputEn;
	
	if (ip != USERIP_GET)
		gUserInputEn = ip;

	return back;	
}


S_VOID _hai_KeyISR(S_WORD key, S_WORD KeyDnFlag)
{
	if (gUserInputEn == USERIP_DISABLE)
		return;
	
	if (KeyCnt < KEY_BUFFER_LENGTH_MAX)
	{
		KeyBuffer[KeyCnt++] = key|KeyDnFlag;
	}
}

static const S_WORD NumShift[] = {')', '!', '@', '#', '$', '%', '^', '&', '*', '('};
S_VOID _hai_PollKey(S_VOID)
{
	static S_BOOL CapsLock = 0;
	static S_BOOL ShiftLock = 0;
	S_BYTE cnt, i;
	S_WORD key;
	S_WORD Buffer[KEY_BUFFER_LENGTH_MAX];

	if (!KeyCnt)
		return;

	_ENTER_CRITICAL_;
	cnt = KeyCnt;
	KeyCnt = 0;
	memcpy(Buffer, (void *)KeyBuffer, cnt*sizeof(S_WORD));
	_EXIT_CRITICAL_;

	for (i = 0; i < cnt; i++)
	{
		key = Buffer[i];
		if ((key&KEY_DOWN_FLAG) == KEY_DOWN_FLAG)
		{
			key = key&(~KEY_DOWN_FLAG);
			if (key == VK_CAPITAL)
				CapsLock = !CapsLock;
			else if (key == VK_SHIFT)
				ShiftLock = 1;
			
			hai_PostEvent(EVT_KEYDN, key, 0);
			if (_hai_IsSysKey(key))
			{
				hai_PostEvent(EVT_SYSKEY, key, 0);
			}
			else if (key >= 'A' && key <= 'Z' || key >= '0' && key <= '9')
			{
				if (key >= 'A' && key <= 'Z' && !CapsLock)
					key += ('a' - 'A');
				if (ShiftLock)
				{
					if (key >= '0' && key <= '9')
						key = NumShift[key-'0'];
					else if (key >= 'a' && key <= 'z')
						key -= ('a' - 'A');
					else if (key >= 'A' && key <= 'Z')
						key += ('a' - 'A');
					ShiftLock = 0;
				}
				hai_PostEvent(EVT_CHAR, key, 0);
			}
			else if (key == VK_SPACE)
			{
				hai_PostEvent(EVT_CHAR, key, 0);
			}
		}
		else
		{
			hai_PostEvent(EVT_KEYUP, key, 0);
		}
	}

}

typedef struct tagMouseBuffer
{
	S_BYTE MouseOp;
	S_WORD MouseX;
	S_WORD MouseY;
}
S_MOUSEBUFFER, *SP_MOUSEBUFFER;

#define MOUSE_BUFFER_LENGTH_MAX		10
static volatile S_MOUSEBUFFER MouseBuffer[MOUSE_BUFFER_LENGTH_MAX];
static volatile S_BYTE MouseCnt = 0;
const S_BYTE MseEvtType[] = 
{
	EVT_MOUSEDN, EVT_MOUSEMV, EVT_MOUSEUP, EVT_MOUSEDB
};

S_VOID _hai_MouseISR(S_BYTE MouseOp, S_WORD x, S_WORD y)
{
	if (gUserInputEn == USERIP_DISABLE)
		return;
	
	if (MouseCnt < MOUSE_BUFFER_LENGTH_MAX)
	{
		MouseBuffer[MouseCnt].MouseOp = MseEvtType[MouseOp];
		MouseBuffer[MouseCnt].MouseX = x;
		MouseBuffer[MouseCnt].MouseY = y;
		MouseCnt++;
	}
}

S_VOID _hai_PollMouse(S_VOID)
{
	S_BYTE cnt, i;
	S_MOUSEBUFFER Buffer[MOUSE_BUFFER_LENGTH_MAX];

	if (!MouseCnt)
		return;

	_ENTER_CRITICAL_;
	cnt = MouseCnt;
	MouseCnt = 0;
	memcpy(Buffer, (void *)MouseBuffer, cnt*sizeof(S_MOUSEBUFFER));
	_EXIT_CRITICAL_;

	for (i = 0; i < cnt; i++)
	{
		hai_PostMouseEvent(Buffer[i].MouseOp, Buffer[i].MouseX, Buffer[i].MouseY);
	}
}

typedef struct tagTimerBuffer
{
	S_BYTE TimerType;
	S_BYTE TimerID;
}
S_TIMERBUFFER, *SP_TIMERBUFFER;

#define TIMER_BUFFER_LENGTH_MAX		10
static volatile S_TIMERBUFFER TimerBuffer[TIMER_BUFFER_LENGTH_MAX];
static volatile S_BYTE TimerCnt = 0;

S_VOID _hai_TimerEventISR(S_VOID)
{
	//check timer
	if (TimerCnt < 1)
	{
		TimerBuffer[TimerCnt].TimerType = EVT_TIMER;
		TimerBuffer[TimerCnt].TimerID = 0;
		TimerCnt++;
	}
	
	//check alarm
}

S_VOID _hai_PollTimer(S_VOID)
{
	S_BYTE cnt, i;
	S_TIMERBUFFER Buffer[TIMER_BUFFER_LENGTH_MAX];

	if (!TimerCnt)
		return;

	_ENTER_CRITICAL_;
	cnt = TimerCnt;
	TimerCnt = 0;
	memcpy(Buffer, (void *)TimerBuffer, cnt*sizeof(S_TIMERBUFFER));
	_EXIT_CRITICAL_;

	for (i = 0; i < cnt; i++)
	{
		hai_PostTimerEvent(Buffer[i].TimerType, Buffer[i].TimerID, 0);
	}
}


S_VOID hai_PollEvent(S_VOID)
{
	_hai_PollKey();
	_hai_PollMouse();
	_hai_PollTimer();
}

S_VOID hai_ClearUserEvent(S_VOID)
{
	S_BOOL hai_ClearEventQueue(S_BYTE minEvent, S_BYTE maxEvent);
	
	_hai_PollKey();
	_hai_PollMouse();
	hai_ClearEventQueue(EVT_KEYDN, EVT_MOUSEMV);
}

