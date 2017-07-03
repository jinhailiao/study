//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			SGUI(Simple GUI)
// File:			sguibase.cpp
// Description:		SGUI for Power user elect energy acquire system;unsupport multi_thread
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2010-01-18  Create                  Kingsea
//---------------------------------------------------------------------------------
#include "sguibase.h"
#include "osrelate.h"

char SGUI_UsrBuf[SGUI_LCD_SIZE];
char *SGUI_fb;

#ifdef WIN32

void fbfresh(void)
{
	S_BYTE C[] = {'\xFF', '\x00'};
	for (int m = 0; m < SGUI_LCD_HEIGHT; ++m)
	{
		S_BYTE *pSrc = (S_BYTE *)SGUI_UsrBuf + m * SGUI_LCD_LINE;
		S_BYTE *pDst = (S_BYTE *)SGUI_fb + m * (480*3);
		for (int n = 0; n < SGUI_LCD_LINE; ++n)
		{
			S_BYTE pixels = *(pSrc + n);
			*(pDst+0) = C[(pixels>>0)&0x01];*(pDst+1) = C[(pixels>>0)&0x01];*(pDst+2) = C[(pixels>>0)&0x01];pDst += 3;
			*(pDst+0) = C[(pixels>>1)&0x01];*(pDst+1) = C[(pixels>>1)&0x01];*(pDst+2) = C[(pixels>>1)&0x01];pDst += 3;
			*(pDst+0) = C[(pixels>>2)&0x01];*(pDst+1) = C[(pixels>>2)&0x01];*(pDst+2) = C[(pixels>>2)&0x01];pDst += 3;
			*(pDst+0) = C[(pixels>>3)&0x01];*(pDst+1) = C[(pixels>>3)&0x01];*(pDst+2) = C[(pixels>>3)&0x01];pDst += 3;
			*(pDst+0) = C[(pixels>>4)&0x01];*(pDst+1) = C[(pixels>>4)&0x01];*(pDst+2) = C[(pixels>>4)&0x01];pDst += 3;
			*(pDst+0) = C[(pixels>>5)&0x01];*(pDst+1) = C[(pixels>>5)&0x01];*(pDst+2) = C[(pixels>>5)&0x01];pDst += 3;
			*(pDst+0) = C[(pixels>>6)&0x01];*(pDst+1) = C[(pixels>>6)&0x01];*(pDst+2) = C[(pixels>>6)&0x01];pDst += 3;
			*(pDst+0) = C[(pixels>>7)&0x01];*(pDst+1) = C[(pixels>>7)&0x01];*(pDst+2) = C[(pixels>>7)&0x01];pDst += 3;
		}
	}
}

int fbinit(void)
{
	return 0;
}

void backlight(int on) //on =1 ¿ª
{
}

void KeyboardInit()
{
}

char SGUI_key = 0;
int ScanKeyboard(void)
{
	int key = -1;

	if (SGUI_key)
	{
		key = SGUI_key;
		SGUI_key = 0;
	}
	return key;
}

#else

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <sys/ioctl.h>

#define FBDEV  "/dev/fb0"
#define BUTTONDEV "/dev/kgeio"
#define LIGHTDEV "/dev/kgeio"


#define ST7529FB_IOC_MAGIC          'S'
#define ST7529FB_IOC_RESET          _IO(ST7529FB_IOC_MAGIC, 0)
#define ST7529FB_IOC_REFRESH        _IO(ST7529FB_IOC_MAGIC, 1)
#define ST7529FB_IOC_BACKLIGHT      _IO(ST7529FB_IOC_MAGIC, 2)

#define KGESG_IO_MAGIC        'K'
#define KGESG_IO_LCDBL_ON             _IO(KGESG_IO_MAGIC, 28)
#define KGESG_IO_LCDBL_OFF            _IO(KGESG_IO_MAGIC, 29)

int SGUI_fd = -1;
int SGUI_kd = -1;

static const unsigned char SGUI_BitReverse[256] =
{
	'\x00',	'\x80',	'\x40',	'\xc0',	'\x20',	'\xa0',	'\x60',	'\xe0',
	'\x10',	'\x90',	'\x50',	'\xd0',	'\x30',	'\xb0',	'\x70',	'\xf0',
	'\x08',	'\x88',	'\x48',	'\xc8',	'\x28',	'\xa8',	'\x68',	'\xe8',
	'\x18',	'\x98',	'\x58',	'\xd8',	'\x38',	'\xb8',	'\x78',	'\xf8',
	'\x04',	'\x84',	'\x44',	'\xc4',	'\x24',	'\xa4',	'\x64',	'\xe4',
	'\x14',	'\x94',	'\x54',	'\xd4',	'\x34',	'\xb4',	'\x74',	'\xf4',
	'\x0c',	'\x8c',	'\x4c',	'\xcc',	'\x2c',	'\xac',	'\x6c',	'\xec',
	'\x1c',	'\x9c',	'\x5c',	'\xdc',	'\x3c',	'\xbc',	'\x7c',	'\xfc',
	'\x02',	'\x82',	'\x42',	'\xc2',	'\x22',	'\xa2',	'\x62',	'\xe2',
	'\x12',	'\x92',	'\x52',	'\xd2',	'\x32',	'\xb2',	'\x72',	'\xf2',
	'\x0a',	'\x8a',	'\x4a',	'\xca',	'\x2a',	'\xaa',	'\x6a',	'\xea',
	'\x1a',	'\x9a',	'\x5a',	'\xda',	'\x3a',	'\xba',	'\x7a',	'\xfa',
	'\x06',	'\x86',	'\x46',	'\xc6',	'\x26',	'\xa6',	'\x66',	'\xe6',
	'\x16',	'\x96',	'\x56',	'\xd6',	'\x36',	'\xb6',	'\x76',	'\xf6',
	'\x0e',	'\x8e',	'\x4e',	'\xce',	'\x2e',	'\xae',	'\x6e',	'\xee',
	'\x1e',	'\x9e',	'\x5e',	'\xde',	'\x3e',	'\xbe',	'\x7e',	'\xfe',
	'\x01',	'\x81',	'\x41',	'\xc1',	'\x21',	'\xa1',	'\x61',	'\xe1',
	'\x11',	'\x91',	'\x51',	'\xd1',	'\x31',	'\xb1',	'\x71',	'\xf1',
	'\x09',	'\x89',	'\x49',	'\xc9',	'\x29',	'\xa9',	'\x69',	'\xe9',
	'\x19',	'\x99',	'\x59',	'\xd9',	'\x39',	'\xb9',	'\x79',	'\xf9',
	'\x05',	'\x85',	'\x45',	'\xc5',	'\x25',	'\xa5',	'\x65',	'\xe5',
	'\x15',	'\x95',	'\x55',	'\xd5',	'\x35',	'\xb5',	'\x75',	'\xf5',
	'\x0d',	'\x8d',	'\x4d',	'\xcd',	'\x2d',	'\xad',	'\x6d',	'\xed',
	'\x1d',	'\x9d',	'\x5d',	'\xdd',	'\x3d',	'\xbd',	'\x7d',	'\xfd',
	'\x03',	'\x83',	'\x43',	'\xc3',	'\x23',	'\xa3',	'\x63',	'\xe3',
	'\x13',	'\x93',	'\x53',	'\xd3',	'\x33',	'\xb3',	'\x73',	'\xf3',
	'\x0b',	'\x8b',	'\x4b',	'\xcb',	'\x2b',	'\xab',	'\x6b',	'\xeb',
	'\x1b',	'\x9b',	'\x5b',	'\xdb',	'\x3b',	'\xbb',	'\x7b',	'\xfb',
	'\x07',	'\x87',	'\x47',	'\xc7',	'\x27',	'\xa7',	'\x67',	'\xe7',
	'\x17',	'\x97',	'\x57',	'\xd7',	'\x37',	'\xb7',	'\x77',	'\xf7',
	'\x0f',	'\x8f',	'\x4f',	'\xcf',	'\x2f',	'\xaf',	'\x6f',	'\xef',
	'\x1f',	'\x9f',	'\x5f',	'\xdf',	'\x3f',	'\xbf',	'\x7f',	'\xff'
};
void fbfresh(void)
{
	for (int m = 0; m < SGUI_LCD_HEIGHT; ++m)
	{
		S_BYTE *pSrc = (S_BYTE *)SGUI_UsrBuf + m * SGUI_LCD_LINE;
		S_BYTE *pDst = (S_BYTE *)SGUI_fb + (SGUI_LCD_HEIGHT - 1 - m) * SGUI_LCD_LINE;
		*(pDst+0) = SGUI_BitReverse[*(pSrc+0)];
		*(pDst+1) = SGUI_BitReverse[*(pSrc+1)];
		*(pDst+2) = SGUI_BitReverse[*(pSrc+2)];
		*(pDst+3) = SGUI_BitReverse[*(pSrc+3)];
		*(pDst+4) = SGUI_BitReverse[*(pSrc+4)];
		*(pDst+5) = SGUI_BitReverse[*(pSrc+5)];
		*(pDst+6) = SGUI_BitReverse[*(pSrc+6)];
		*(pDst+7) = SGUI_BitReverse[*(pSrc+7)];
		*(pDst+8) = SGUI_BitReverse[*(pSrc+8)];
		*(pDst+9) = SGUI_BitReverse[*(pSrc+9)];
		*(pDst+10) = SGUI_BitReverse[*(pSrc+10)];
		*(pDst+11) = SGUI_BitReverse[*(pSrc+11)];
		*(pDst+12) = SGUI_BitReverse[*(pSrc+12)];
		*(pDst+13) = SGUI_BitReverse[*(pSrc+13)];
		*(pDst+14) = SGUI_BitReverse[*(pSrc+14)];
		*(pDst+15) = SGUI_BitReverse[*(pSrc+15)];
		*(pDst+16) = SGUI_BitReverse[*(pSrc+16)];
		*(pDst+17) = SGUI_BitReverse[*(pSrc+17)];
		*(pDst+18) = SGUI_BitReverse[*(pSrc+18)];
		*(pDst+19) = SGUI_BitReverse[*(pSrc+19)];
	}
	if (SGUI_fd > 0)
		ioctl(SGUI_fd,ST7529FB_IOC_REFRESH);
}

int fbinit(void)
{
	SGUI_fd = open(FBDEV, O_RDWR);
	if(SGUI_fd<0)	{
		printf(FBDEV"Open failed!\n");
	}

	SGUI_fb = (char *)mmap(0, SGUI_LCD_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, SGUI_fd, 0);
	if(SGUI_fb < 0){
		printf("Map %s failed!\n", FBDEV);
	}

	memset(SGUI_UsrBuf, 0x00, SGUI_LCD_SIZE);
	fbfresh();

	return 0;
}

void backlight(int on) //on =1 ¿ª
{
	int SGUI_ld = open(LIGHTDEV, O_RDWR);
	if (SGUI_ld >= 0)
	{
		if (on)
			ioctl(SGUI_ld,KGESG_IO_LCDBL_ON);
		else
			ioctl(SGUI_ld,KGESG_IO_LCDBL_OFF);
		close(SGUI_ld);
	}
	else
	{
		printf(LIGHTDEV "open failed!\n");
	}
}

void KeyboardInit()
{
	SGUI_kd = open(BUTTONDEV, O_RDWR);
	if (SGUI_kd < 0)
		printf(BUTTONDEV"open failed!\n");
}

int ScanKeyboard(void)
{
	int event;
	fd_set rset;
	struct timeval tv;

	if (SGUI_kd <= 0)
		return -1;

	FD_ZERO(&rset);
	FD_SET(SGUI_kd, &rset);

	tv.tv_sec = 0;
	tv.tv_usec = 100000;

	if (select(SGUI_kd + 1, &rset, NULL, NULL, &tv) > 0) 
	{
		read(SGUI_kd, &event, 1);
		return (event&0xFF);
	}

	return -1;
}

#endif

int SGUI_LcdInit(void)
{
	fbinit();
	return 0;
}

int SGUI_KeyboardInit(void)
{
	KeyboardInit();
	return 0;
}

S_GUIMSG SGUI_PollMsg(void)
{
	S_GUIMSG aMsg;
	int key = ScanKeyboard();
	if (key != -1)
	{
		aMsg.Msg = GM_KEYUP;
		aMsg.wParam = (S_WORD)key;
	}
	return aMsg;
}

void SGUI_backlight(int on)
{
	backlight(on);
}

static bool flushScrnEnable = true;
void SGUI_flushScreen(S_WORD x, S_WORD y, S_WORD w, S_WORD h)
{
	if (flushScrnEnable == true)
		fbfresh();
}

bool SGUI_FlushScreenEn(bool enable)
{
	bool old = flushScrnEnable;
	flushScrnEnable = enable;
//	if (flushScrnEnable == true)
//		fbfresh();
	return old;
}

void SGUI_CloseScreen(void)
{
}

void SGUI_OpenScreen(void)
{
}

int SGUI_SleepMS(int ms)
{
	SleepMS(ms);
	return 0;
}




