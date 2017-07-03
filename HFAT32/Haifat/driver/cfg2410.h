/*
;---------------------------------------------------------------------------------
; Copyright (c) Haisoft 2007-9-1
; Author:				Kingsea
; Email:				jinhailiao@163.com
;-------------------------------------------------------------
; Project:			HaiBios
; File:					config.h
; Description:	
;-------------------------------------------------------------
; Reversion Histroy:
;-------------------------------------------------------------
; Version		date		operations				by who
; 1.0.0		2007-09-01	Create					Kingsea
;
;---------------------------------------------------------------------------------
*/

#ifndef __CFG2410_H__
#define __CFG2410_H__

//note: please carefully, refer boot2410.s, argdef.inc;
#define FCLK		120000000
//#define FCLK		202800000
#define HCLK		(FCLK/2)
#define PCLK		(FCLK/4)


//-------------------------------------------------------------
//Memory Layout
//-------------------------------------------------------------
#define SDRAM_START			0x30000000
#define RAMDISK_SIZE		0x02000000	/*32MB*/
#define SDRAM_END			0x34000000
#define RAMDISK_START		(SDRAM_END-RAMDISK_SIZE)
#define STACK_BASEADDR		(RAMDISK_START-0x1000)
#define ISR_STARTADDR		(RAMDISK_START-0x100)
#define MAIM_RUN_ADDR		SDRAM_START

#endif //#ifndef __CFG2410_H__

