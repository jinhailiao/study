// Hcomm.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error �ڰ������� PCH �Ĵ��ļ�֮ǰ������stdafx.h��
#endif

#include "resource.h"		// ������


// CHcommApp:
// �йش����ʵ�֣������ Hcomm.cpp
//

class CHcommApp : public CWinApp
{
public:
	CHcommApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CHcommApp theApp;
