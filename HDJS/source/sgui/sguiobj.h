//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			SGUI(Simple GUI)
// File:			sguiobj.h
// Description:		SGUI for Power user elect energy acquire system;unsupport multi_thread
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2010-01-18  Create                  Kingsea
//---------------------------------------------------------------------------------
#ifndef __SGUI_OBJ_H__
#define __SGUI_OBJ_H__

class C_SGUIOBJ
{
public:
	enum OBJ_T {
	OBJ_T_UNKNOW,
	OBJ_T_APP,
	OBJ_T_WNDB,
	OBJ_T_WND,
	OBJ_T_DC,
	OBJ_T_FONT,
	OBJ_T_PEN,
	OBJ_T_BMP,
	OBJ_T_CTRL,
	OBJ_T_CARET,

	OBJ_T_MAX
	};
public:
	C_SGUIOBJ(OBJ_T t){m_ObjT = t;}
	virtual ~C_SGUIOBJ(){}

	OBJ_T GetObjType(void){return m_ObjT;}
	
protected:
	OBJ_T m_ObjT;
};

#endif //__SGUI_OBJ_H__

