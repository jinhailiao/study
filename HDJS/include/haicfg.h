// E-mail:   jinhailiao@163.com
// Project:			HDJS
// File:			haicfg.h
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2009-11-13  Create                  Kingsea
//---------------------------------------------------------------------------------
#ifndef __HAICFG_H__
#define __HAICFG_H__

//
//file path config
//
#ifdef WIN32
#define CFG_FILE_BASE_PATH	"../../data/mnt/"
#else
#define CFG_FILE_BASE_PATH	"/mnt/"
#endif
#define CFG_FILE_PROG_PATH		CFG_FILE_BASE_PATH"dyjc/program/"
#define CFG_FILE_EDATA_PATH		CFG_FILE_BASE_PATH"data/energy/"
#define CFG_FILE_RESOURCE_PATH	CFG_FILE_BASE_PATH"resource/"
#define CFG_FILE_PROG_NAME		"hdjs"

//
//server config
//
#define SERVER_HEART_BEAT_INTERS 60
#define CHANNEL_UP_TOTAL_MAX		12
#define CHANNEL_DN_TOTAL_MAX		3
#define HTTX_CHANNEL_MAX		3
enum
{
	HTTX_ID_1,
	HTTX_ID_2,
	HTTX_ID_3,
	HTTX_ID_MAX
};

//
//master station config
//
#define VIPDATA_POINT_PERDAY 24
#define	GW_TARIFF_MAX		12
#define	GW_MP_MAX			1200

//
//include general include file
//
#include "osrelate.h"
#include "htrace.h"

#include "assert.h"

#endif //__HAICFG_H__

