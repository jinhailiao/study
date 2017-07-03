// GW007Dlg.h : ͷ�ļ�
//

#pragma once

#include "inirule.h"
#include "frame.h"

// CGW007Dlg �Ի���
class CGW007Dlg : public CDialog
{
// ����
public:
	CGW007Dlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_GW007_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
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
	// ���ɵ���Ϣӳ�亯��
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
