//---------------------------------------------------------------------------------
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HFAT16/32
// File:			hfaterr.h
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2008-06-18  create                  Kingsea
//---------------------------------------------------------------------------------
#ifndef __HFATERR_H__
#define __HFATERR_H__

#define FATERR_END_OF_FILE			-1
#define FATERR_UNKNOWN_ERR2			-1
#define FATERR_NO_ERROR				0
#define FATERR_UNKNOWN_ERR			1
#define FATERR_DISK_INUSE			2
#define FATERR_INVALID_DISK			3
#define FATERR_DEVINIT_FAILED		4
#define FATERR_UNKNOWN_FILESYS		5
#define FATERR_DEV_OPERATE_ERR		6
#define FATERR_INVALID_VALUE		7
#define FATERR_INVALID_PATH			8
#define FATERR_INVALID_FILENAME		9
#define FATERR_EXISTED_FILENAME		10
#define FATERR_NO_DISK_SPACE		11
#define FATERR_PATH_TOO_LENGTH		12
#define FATERR_DIR_NOT_EMPTY		13
#define FATERR_REACHED_EOF		14
#define FATERR_READONLY_FILE		15
#define FATERR_READONLY_MODE		16
#define FATERR_WRITEONLY_MODE	17
#define FATERR_DEVOP_ABORT		18
#define FATERR_INUSE_FILE		19
#define FATERR_FILE_NOFIND		19

#define printferr(userinfo, errno)


#endif //__HFATERR_H__

