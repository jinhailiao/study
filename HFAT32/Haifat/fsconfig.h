//---------------------------------------------------------------------------------
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HFAT16/32
// File:			ftconfig.h
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2008-06-01  create                  Kingsea
//---------------------------------------------------------------------------------
#ifndef __FT_CONFIG_H__
#define __FT_CONFIG_H__

#define ADD_RAM_DISK_DRIVER
#define ADD_RAM_DISK2_DRIVER
#define ADD_RAM_DISK3_DRIVER
//#define ADD_S3C2410_SDI
//#define ADD_S3C2410_NAND

//#define HAVE_INLINE
#ifdef HAVE_INLINE
#define _INLINE_
#endif

#define RAMDISK_SIMULATOR

#define DEVICE_SUPPORT_MAX		10
#define DEVICE_QUERY_MAX		6

#define SECTOR_BUFFER_SIZE		(512+16+4)/* 16:for nand flash spare space; 4: for .... */

#define AppSchedule()


//#define HAVE_MALLOC
#ifdef HAVE_MALLOC
void *malloc(size_t size);
void free(void *memblock);
#define hai_FatMemAlloc(size)	malloc(size)
#define hai_FatMemFree(memblock)	free(memblock)
#else
void  *hai_MemAlloc(unsigned long size);
unsigned char hai_MemFree(void *pMem);
#define hai_FatMemAlloc(size)	hai_MemAlloc(size)
#define hai_FatMemFree(memblock)	hai_MemFree(memblock)
#endif


#endif//__FT_CONFIG_H__

