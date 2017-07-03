// HcommDlg.h : 头文件
//

#pragma once

#include <string>
using std::string;
#include "CMSComm.h"

enum {LGR_ACK_AUTO, LGR_ACK_300, LGR_ACK_600, LGR_ACK_1200, LGR_ACK_2400, LGR_ACK_4800, LGR_ACK_9600};

// CHcommDlg 对话框
class CHcommDlg : public CDialog
{
// 构造
public:
	CHcommDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_HCOMM_DIALOG };
	enum {SPORT_COM1,SPORT_COM2,SPORT_COM3,SPORT_COM4};
	enum {SPORT_300,SPORT_600,SPORT_1200,SPORT_2400,SPORT_4800,SPORT_9600,
			SPORT_19200,SPORT_38400,SPORT_56000,SPORT_57600,SPORT_115200};
	enum {SPORT_NONE,SPORT_ODD,SPORT_EVEN};
	enum {SPORT_NUM8,SPORT_NUM7,SPORT_NUM6};
	enum {SPORT_STOP1,SPORT_STOP2};

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	int m_cx;
	bool m_FileSending;
	CFile SFile;

	HICON m_hIcon;
	HACCEL m_hAccel;
	HBITMAP m_hLightW;
	HBITMAP m_hLightR;

	CString m_StrReceive;
	CString m_StrSend;

	CMSComm m_Scomm;
	bool m_LGRlogin;

protected:
	int m_serialPort;
	int m_serialBaud;
	int m_serialCheck;
	int m_serialNum;
	int m_serialStop;

	int m_watchPort;
	int m_hexShow;
	int m_hexSend;
	int m_autoSend;
	int m_sendPeriod;

	int  m_autoFrame;
	int  m_meterRule;
	int  m_meterReg;
	string m_meterAddr;

public:
	int m_DltRuleCtrlCode;
	string m_OpCode;
	string m_Password;
	string m_FreezeTime;
	string m_WriteData;

public:
	int m_LgrAck;

private:
	bool FilterDigit0_9(char *strbuf);
	bool FilterDigit0_F(char *strbuf);
	int  Hex2String(CString &str, const char *pHex, int cnt);
	TCHAR *GetPortName(void);
	TCHAR *GetPortSetting(void);
	void EnableCtrlWhenFileSend(bool bEnable = true);
	void EnableCtrlWhenAutoSend(bool bEnable = true);
	void EnableControl(int *pCtrlID, int size, bool bEnable);

private:
	bool OnComboPort(WPARAM wParam,LPARAM lParam);
	bool OnComboBaud(WPARAM wParam,LPARAM lParam);
	bool OnComboCheck(WPARAM wParam,LPARAM lParam);
	bool OnComboNum(WPARAM wParam,LPARAM lParam);
	bool OnComboStop(WPARAM wParam,LPARAM lParam);

	bool OnCheckWatch(WPARAM wParam,LPARAM lParam);
	bool OnCheckHexshow(WPARAM wParam,LPARAM lParam);
	bool OnCheckHexsend(WPARAM wParam,LPARAM lParam);
	bool OnCheckAutosend(WPARAM wParam,LPARAM lParam);
	bool OnEditMSecond(WPARAM wParam,LPARAM lParam);

	bool OnCheckAutoFrame(WPARAM wParam,LPARAM lParam);
	bool OnButtonMSet(WPARAM wParam,LPARAM lParam);
	bool OnComboRule(WPARAM wParam,LPARAM lParam);
	bool OnEditAddr(WPARAM wParam,LPARAM lParam);
	bool OnComboDataID(WPARAM wParam,LPARAM lParam);

	bool OnEditReceive(WPARAM wParam,LPARAM lParam);
	bool OnEditSend(WPARAM wParam,LPARAM lParam);

	bool OnButtonClrReceive(WPARAM wParam,LPARAM lParam);
	bool OnButtonClrSend(WPARAM wParam,LPARAM lParam);
	bool OnButtonManualSend(WPARAM wParam,LPARAM lParam);
	bool OnButtonSendFile(WPARAM wParam,LPARAM lParam);
	bool OnButtonAbort(WPARAM wParam,LPARAM lParam);

	bool OnButtonOpen(WPARAM wParam,LPARAM lParam);
	bool OnEditPath(WPARAM wParam,LPARAM lParam);
	bool OnButtonLgrAck(WPARAM wParam,LPARAM lParam);

	void MakeFrameAndShow(void);
	bool ScommSendData(void);
	
protected:
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnCommand(WPARAM wParam,LPARAM lParam);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnClose();
	afx_msg void OnCommEvent();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);

	DECLARE_EVENTSINK_MAP()
	DECLARE_MESSAGE_MAP()
};
