#pragma once


// CMSetDlg �Ի���

class CMSetDlg : public CDialog
{
	DECLARE_DYNAMIC(CMSetDlg)

public:
	CMSetDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CMSetDlg();

// �Ի�������
	enum { IDD = IDD_MSET_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

protected:
	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual BOOL OnCommand(WPARAM wParam,LPARAM lParam);

private:
	BOOL OnComboLgrBaud(WPARAM wParam,LPARAM lParam);
	
protected:
	DECLARE_MESSAGE_MAP()
};

// CMDLTSetDlg �Ի���

class CMDLTSetDlg : public CDialog
{
	DECLARE_DYNAMIC(CMDLTSetDlg)

public:
	CMDLTSetDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CMDLTSetDlg();

// �Ի�������
	enum { IDD = IDD_MSET_DLT_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

protected:
	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual BOOL OnCommand(WPARAM wParam,LPARAM lParam);

protected:
	BOOL OnComboBoxCtrlCode(WPARAM wParam,LPARAM lParam);

	BOOL EnableCtrlWnd(int CtrlCodeSN);

protected:
	DECLARE_MESSAGE_MAP()
};


