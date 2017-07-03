//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			HDJS
// File:			osio.cpp
// Description:		OS io port operator,like watchdog
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2009-11-21  Create                  Kingsea
//---------------------------------------------------------------------------------
#include "osrelate.h"

#ifdef WIN32

int io_Watchdog(){return 0;}
int KgeioWriteCmd(char *pCmd, int state){return 0;}
int KgeioReadState(char *pCmd){return 0;}
int RtcioReadTimeStamp(struct tm *pTime){return -1;}

#else

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/watchdog.h>

#include <string.h>

#define KGESG_IO_MAGIC        'K'

/* led control, no data exchanged */
#define KGESG_IO_RUNLED_ON             _IO(KGESG_IO_MAGIC, 1)
#define KGESG_IO_RUNLED_OFF            _IO(KGESG_IO_MAGIC, 2)

/* left-box (debugging box) */
#define KGESG_IO_LEFTBOX_RST_HIGH    _IO(KGESG_IO_MAGIC, 3)
#define KGESG_IO_LEFTBOX_RST_LOW     _IO(KGESG_IO_MAGIC, 4)
#define KGESG_IO_LEFTBOX_SET_HIGH      _IO(KGESG_IO_MAGIC, 5)
#define KGESG_IO_LEFTBOX_SET_LOW       _IO(KGESG_IO_MAGIC, 6)

/* so-called state-value from Right-box */
#define KGESG_IO_GET_STATE0   _IOR(KGESG_IO_MAGIC, 7, int)
#define KGESG_IO_GET_STATE1   _IOR(KGESG_IO_MAGIC, 8, int)
#define KGESG_IO_GET_STATE2   _IOR(KGESG_IO_MAGIC, 9, int)
#define KGESG_IO_GET_STATE3   _IOR(KGESG_IO_MAGIC, 10, int)
#define KGESG_IO_GET_STATE4   _IOR(KGESG_IO_MAGIC, 11, int)

/* so-called switch input */
#define KGESG_IO_GET_SW1      _IOR(KGESG_IO_MAGIC, 12, int)
#define KGESG_IO_GET_SW2      _IOR(KGESG_IO_MAGIC, 13, int)

/* the fucking Meng Jie Dian : the state of case-cover */
#define KGESG_IO_GET_CASECOVER    _IOR(KGESG_IO_MAGIC, 14, int)
#define KGESG_IO_GPRSIGT_HIGH     _IO(KGESG_IO_MAGIC, 15)
#define KGESG_IO_GPRSIGT_LOW      _IO(KGESG_IO_MAGIC, 16)
#define KGESG_IO_GPRSPWR_HIGH     _IO(KGESG_IO_MAGIC, 17)
#define KGESG_IO_GPRSPWR_LOW      _IO(KGESG_IO_MAGIC, 18)
#define KGESG_IO_GPRSRST_HIGH     _IO(KGESG_IO_MAGIC, 19)
#define KGESG_IO_GPRSRST_LOW      _IO(KGESG_IO_MAGIC, 20)
#define KGESG_IO_FEED_WDT         _IO(KGESG_IO_MAGIC, 21)
#define KGESG_IO_WDT_EN           _IO(KGESG_IO_MAGIC, 22)
#define KGESG_IO_WDT_DIS          _IO(KGESG_IO_MAGIC, 23)
#define KGESG_IO_ESAM_RST         _IO(KGESG_IO_MAGIC, 24)
#define KGESG_IO_GET_PWRDW    _IOR(KGESG_IO_MAGIC, 30, int)
#define KGESG_IO_GET_PWRUP    _IOR(KGESG_IO_MAGIC, 31, int)
#define RTC_RD_TIMESTAMP		_IOR('p', 0x13, struct rtc_time) /* Read timestamp time */ 

#define INVALID_CMD_CODE -1
#define KGEIO_DEV_NAME "/dev/kgeio"

static int get_write_cmd_code(char *cmd_str, int state)
{
	static struct {
		char *cmd_str;
		int  state;
		int  cmd_code;
	} mapping[] = {
		{ "esamrst", 0, KGESG_IO_ESAM_RST },
		{ "esamrst", 1, KGESG_IO_ESAM_RST },
		{ "runled", 1, KGESG_IO_RUNLED_ON },
		{ "runled", 0, KGESG_IO_RUNLED_OFF },
		{ "leftrst", 1, KGESG_IO_LEFTBOX_RST_HIGH },
		{ "leftrst", 0, KGESG_IO_LEFTBOX_RST_LOW },
		{ "leftset", 1, KGESG_IO_LEFTBOX_SET_HIGH },
		{ "leftset", 0, KGESG_IO_LEFTBOX_SET_LOW },
		{ "gprsigt", 1, KGESG_IO_GPRSIGT_HIGH },
		{ "gprsigt", 0, KGESG_IO_GPRSIGT_LOW },
		{ "gprspwr", 1, KGESG_IO_GPRSPWR_HIGH },
		{ "gprspwr", 0, KGESG_IO_GPRSPWR_LOW },
		{ "gprsrst", 1, KGESG_IO_GPRSRST_HIGH },
		{ "gprsrst", 0, KGESG_IO_GPRSRST_LOW },
		{ "wdt",     1, KGESG_IO_FEED_WDT },
		{ "wdt",     0, KGESG_IO_FEED_WDT },
	};

	state = state ? 1 : 0;
	
	for (unsigned int i = 0; i < sizeof(mapping)/sizeof(mapping[0]); i++) {
		if (!strcmp(cmd_str, mapping[i].cmd_str) && state == mapping[i].state)
			return mapping[i].cmd_code;
	}

	return INVALID_CMD_CODE;
}

static int get_read_cmd_code(char *cmd_str)
{
	static struct {
		char *cmd_str;
		int  cmd_code;
	} mapping[] = {
		{ "s0", KGESG_IO_GET_STATE0 },
		{ "s1", KGESG_IO_GET_STATE1 },
		{ "s2", KGESG_IO_GET_STATE2 },
		{ "s3", KGESG_IO_GET_STATE3 },
		{ "s4", KGESG_IO_GET_STATE4 },
		{ "sw1", KGESG_IO_GET_SW1 },
		{ "sw2", KGESG_IO_GET_SW2 },
		{ "pwroff", KGESG_IO_GET_PWRDW },
		{ "pwron",  KGESG_IO_GET_PWRUP },
		{ "cover",  KGESG_IO_GET_CASECOVER }
	};

	for (unsigned int i = 0; i < sizeof(mapping)/sizeof(mapping[0]); i++) {
		if (!strcmp(cmd_str, mapping[i].cmd_str))
			return mapping[i].cmd_code;
	}

	return INVALID_CMD_CODE;
}

int KgeioWriteCmd(char *pCmd, int state)
{
	int fd, err, CmdCode;

	if ((CmdCode = get_write_cmd_code(pCmd, state)) == INVALID_CMD_CODE)
	{
		printf("Kgeio INVALID Command String!\n");
		return -1;
	}

	if ((fd = open(KGEIO_DEV_NAME, O_RDWR)) < 0)
	{
		printf(KGEIO_DEV_NAME" open failed!\n");
		return -1;
	}
		
	if ((err = ioctl(fd, CmdCode, 0)) != 0)
	{
		printf("kgeio write operate failed!!!\n");
	}
	close(fd);

	return err;
}

int KgeioReadState(char *pCmd)
{
	int fd, CmdCode, state;

	if ((CmdCode = get_read_cmd_code(pCmd)) == INVALID_CMD_CODE)
	{
		printf("Kgeio invalid Command:%s!\n", pCmd);
		return -1;
	}
	if ((fd = open(KGEIO_DEV_NAME, O_RDWR)) < 0)
	{
		printf(KGEIO_DEV_NAME" open failed!\n");
		return -1;
	}
	if (ioctl(fd, CmdCode, &state) < 0)
	{
		printf("kgeio read operate failed!!!\n");
		state = -1;
	}
	close(fd);

	return state;
}

#define RTC_DEV		"/dev/rtc0"
#include <linux/rtc.h> 
#include <time.h>

int RtcioReadTimeStamp(struct tm *pTime)
{
	int fd, err = -1;
	struct rtc_time rtctime;

	if ((fd = open(RTC_DEV, O_RDONLY)) < 0)
	{
		printf(RTC_DEV" open failed!!!\n");
		return -1;
	}

	memset(&rtctime, 0x00, sizeof(rtctime));
	if ((err = ioctl(fd, RTC_RD_TIMESTAMP, &rtctime)) < 0)
	{
		printf(RTC_DEV" read operate failed!!!\n");
	}
	else
	{
		pTime->tm_mday = rtctime.tm_mday;
		pTime->tm_mon  = rtctime.tm_mon;
		pTime->tm_year = rtctime.tm_year;
		pTime->tm_hour = rtctime.tm_hour;
		pTime->tm_min  = rtctime.tm_min;
		pTime->tm_sec  = rtctime.tm_sec;
	}

	close(fd);

	return err; 
}


int io_Watchdog(void)
{
#define DEV_WDG	"/dev/watchdog"
	int fd, dummy, ok = -1;
	
	if ((fd = open(DEV_WDG, O_WRONLY)) == -1)
	{
		printf(DEV_WDG" open failed!!!\n");
		return -1;
	}
	else
	{
		ok = ioctl(fd, WDIOC_KEEPALIVE, &dummy); 
		close(fd);
	}

	return ok;
}
#endif

int io_PowerOnGprsModem(int delayS)
{
	if (KgeioWriteCmd("gprspwr", 1) != 0) return -1;//power on
	for(int i = 0; i < 2; i++)//delay 2 second
		SleepMS(1000);
	if (KgeioWriteCmd("gprsigt", 0) != 0) return -1;//Ignite 0
	if (KgeioWriteCmd("gprsrst", 1) != 0) return -1;//reset
	for(int i = 0; i < 2; i++)//delay 2 second
		SleepMS(1000);
	if (KgeioWriteCmd("gprsigt", 1) != 0) return -1;//Ignite 1
	for(int i = 0; i < delayS; i++)//delay N second
		SleepMS(1000);

	return 0;
}

int io_PowerOffGprsModem(int delayS)
{
	KgeioWriteCmd("gprspwr", 0);//power of
	for(int i = 0; i < delayS; i++)//delay N second
		SleepMS(1000);
		
	return 0;
}


