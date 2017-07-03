#pragma once


// CMSetDlg 对话框

class CMSetDlg : public CDialog
{
	DECLARE_DYNAMIC(CMSetDlg)

public:
	CMSetDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMSetDlg();

// 对话框数据
	enum { IDD = IDD_MSET_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

protected:
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual BOOL OnCommand(WPARAM wParam,LPARAM lParam);

private:
	BOOL OnComboLgrBaud(WPARAM wParam,LPARAM lParam);
	
protected:
	DECLARE_MESSAGE_MAP()
};

// CMDLTSetDlg 对话框

class CMDLTSetDlg : public CDialog
{
	DECLARE_DYNAMIC(CMDLTSetDlg)

public:
	CMDLTSetDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMDLTSetDlg();

// 对话框数据
	enum { IDD = IDD_MSET_DLT_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

protected:
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual BOOL OnCommand(WPARAM wParam,LPARAM lParam);

protected:
	BOOL OnComboBoxCtrlCode(WPARAM wParam,LPARAM lParam);

	BOOL EnableCtrlWnd(int CtrlCodeSN);

protected:
	DECLARE_MESSAGE_MAP()
};


