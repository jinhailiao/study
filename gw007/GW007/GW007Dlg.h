// GW007Dlg.h : 头文件
//

#pragma once

#include "inirule.h"
#include "frame.h"

// CGW007Dlg 对话框
class CGW007Dlg : public CDialog
{
// 构造
public:
	CGW007Dlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_GW007_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	HACCEL m_hAccel;


	S_RULES m_Rules;
	int m_ProtocolSN;
	int m_AfnSN;

protected:
	void InitCtrl(void);
	bool GetFrameData(S_FrameData &FrameData);

	bool OnComboProtocol(WPARAM wParam,LPARAM lParam);
	bool OnComboAfn(WPARAM wParam,LPARAM lParam);
	bool OnEditSeq(WPARAM wParam,LPARAM lParam);
	bool OnEditDelay(WPARAM wParam,LPARAM lParam);
	bool OnEditMaster(WPARAM wParam,LPARAM lParam);
	bool OnEditTerm(WPARAM wParam,LPARAM lParam);
	bool OnEditDA(WPARAM wParam,LPARAM lParam);
	bool OnEditDT(WPARAM wParam,LPARAM lParam);
	bool OnEditPN(WPARAM wParam,LPARAM lParam);
	bool OnEditFN(WPARAM wParam,LPARAM lParam);
	bool OnEditPW(WPARAM wParam,LPARAM lParam);
	bool OnEditData(WPARAM wParam,LPARAM lParam);
	bool OnEditFrame(WPARAM wParam,LPARAM lParam);
	bool OnButtonMake(WPARAM wParam,LPARAM lParam);
	bool OnButtonReset(WPARAM wParam,LPARAM lParam);
	bool OnButtonAbout(WPARAM wParam,LPARAM lParam);

protected:
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnCommand(WPARAM wParam,LPARAM lParam);

	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeEdit11();
};
