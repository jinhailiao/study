// MSetDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Hcomm.h"
#include "MSetDlg.h"
#include "HcommDlg.h"
#include "meterrul.h"

// CMSetDlg 对话框

IMPLEMENT_DYNAMIC(CMSetDlg, CDialog)
CMSetDlg::CMSetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMSetDlg::IDD, pParent)
{
}

CMSetDlg::~CMSetDlg()
{
}

void CMSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMSetDlg, CDialog)
END_MESSAGE_MAP()


// CMSetDlg 消息处理程序
BOOL CMSetDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CHcommDlg *pWnd = (CHcommDlg *)GetParent();
	CComboBox *pCB = (CComboBox *)GetDlgItem(IDC_COMBO_LGRBAUD);
	pCB->SetCurSel(pWnd->m_LgrAck);

	return true;
}

//重载OK函数
void CMSetDlg::OnOK()
{
	CHcommDlg *pWnd = (CHcommDlg *)GetParent();
	CComboBox *pCB = (CComboBox *)GetDlgItem(IDC_COMBO_LGRBAUD);
	pWnd->m_LgrAck = pCB->GetCurSel();

	CDialog::OnOK();
}

BOOL CMSetDlg::OnCommand(WPARAM wParam,LPARAM lParam)
{
	switch ((int)(wParam&0xFFFF))
	{
	case IDC_COMBO_LGRBAUD:
		return OnComboLgrBaud(wParam,lParam);
	default:
        return CDialog::OnCommand(wParam,lParam);
	}

	return true;
}

BOOL CMSetDlg::OnComboLgrBaud(WPARAM wParam,LPARAM lParam)
{
	if (LOWORD(wParam) != IDC_COMBO_LGRBAUD)
		return false;

	if (HIWORD(wParam) == CBN_SELCHANGE)
	{
		CComboBox *pCB = (CComboBox *)GetDlgItem(IDC_COMBO_LGRBAUD);
		CHcommDlg *pWnd = (CHcommDlg *)GetParent();
//		pWnd->m_LgrAck = pCB->GetCurSel();
	}

	return true;
}

// CMDLTSetDlg 对话框

IMPLEMENT_DYNAMIC(CMDLTSetDlg, CDialog)
CMDLTSetDlg::CMDLTSetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMDLTSetDlg::IDD, pParent)
{
}

CMDLTSetDlg::~CMDLTSetDlg()
{
}

void CMDLTSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMDLTSetDlg, CDialog)
END_MESSAGE_MAP()


// CMSetDlg 消息处理程序
BOOL CMDLTSetDlg::OnInitDialog()
{
	char buffer[12];
	CDialog::OnInitDialog();

	CStatic *pStatic;
	CComboBox *pCB;
	CEdit *pEdit;
	CHcommDlg *pWnd = (CHcommDlg *)GetParent();

	pStatic = (CStatic *)GetDlgItem(IDC_STATIC_MSET_DLT_NAME);
	pStatic->SetWindowText(pCurMrule->GetMSetName());
	pCB = (CComboBox *)GetDlgItem(IDC_COMBO_CTRL_CODE);
	for (const S_DLTCTRLCODE *pCtrlCode = pCurMrule->GetCtrlCodeGroup(); pCtrlCode->m_CtrlCode; ++pCtrlCode)
		pCB->AddString(pCtrlCode->m_pName);
	pCB->SetCurSel(pWnd->m_DltRuleCtrlCode);

	pEdit = (CEdit *)GetDlgItem(IDC_EDIT_OP_CODE);
	pEdit->SetWindowText(pWnd->m_OpCode.c_str());
	pEdit = (CEdit *)GetDlgItem(IDC_EDIT_PASSWORD);
	pEdit->SetWindowText(pWnd->m_Password.c_str());

	pCB = (CComboBox *)GetDlgItem(IDC_COMBO_FREEZE_MON);
	pCB->AddString("99");
	for (int i = 1; i < 13; i++)
	{
		sprintf(buffer, "%02d", i);
		pCB->AddString(buffer);
	}
	pCB->SelectString(0, C_MRULE::BCD2String(pWnd->m_FreezeTime[0], buffer));

	pCB = (CComboBox *)GetDlgItem(IDC_COMBO_FREEZE_DAY);
	pCB->AddString("99");
	for (int i = 1; i < 32; i++)
	{
		sprintf(buffer, "%02d", i);
		pCB->AddString(buffer);
	}
	pCB->SelectString(0, C_MRULE::BCD2String(pWnd->m_FreezeTime[1], buffer));

	pCB = (CComboBox *)GetDlgItem(IDC_COMBO_FREEZE_HOUR);
	pCB->AddString("99");
	for (int i = 1; i < 25; i++)
	{
		sprintf(buffer, "%02d", i);
		pCB->AddString(buffer);
	}
	pCB->SelectString(0, C_MRULE::BCD2String(pWnd->m_FreezeTime[2], buffer));

	pCB = (CComboBox *)GetDlgItem(IDC_COMBO_FREEZE_MIN);
	pCB->AddString("99");
	for (int i = 0; i < 60; i++)
	{
		sprintf(buffer, "%02d", i);
		pCB->AddString(buffer);
	}
	pCB->SelectString(0, C_MRULE::BCD2String(pWnd->m_FreezeTime[3], buffer));

	pStatic = (CStatic *)GetDlgItem(IDC_STATIC_DATA_NAME);
	if (pCurMrule->IsPasswordCtrlCode(pWnd->m_DltRuleCtrlCode) == true)
        pStatic->SetWindowText("新密码");
	else
        pStatic->SetWindowText("数  据");
	pEdit = (CEdit *)GetDlgItem(IDC_EDIT_WRITE_DATA);
	pEdit->SetWindowText(pWnd->m_WriteData.c_str());

	EnableCtrlWnd(pWnd->m_DltRuleCtrlCode);

	return true;
}

//重载OK函数
void CMDLTSetDlg::OnOK()
{
	char buffer[64];
	CComboBox *pCB;
	CEdit *pEdit;
	CHcommDlg *pWnd = (CHcommDlg *)GetParent();

	pCB = (CComboBox *)GetDlgItem(IDC_COMBO_CTRL_CODE);
	pWnd->m_DltRuleCtrlCode = pCB->GetCurSel();

	pEdit = (CEdit *)GetDlgItem(IDC_EDIT_OP_CODE);
	pEdit->GetWindowText(buffer, sizeof(buffer));
	pWnd->m_OpCode = buffer;
	pEdit = (CEdit *)GetDlgItem(IDC_EDIT_PASSWORD);
	pEdit->GetWindowText(buffer, sizeof(buffer));
	pWnd->m_Password = buffer;

	pCB = (CComboBox *)GetDlgItem(IDC_COMBO_FREEZE_MON);
	pCB->GetLBText(pCB->GetCurSel(), buffer);
	pWnd->m_FreezeTime[0] = C_MRULE::String2BCD(buffer);
	pCB = (CComboBox *)GetDlgItem(IDC_COMBO_FREEZE_DAY);
	pCB->GetLBText(pCB->GetCurSel(), buffer);
	pWnd->m_FreezeTime[1] = C_MRULE::String2BCD(buffer);
	pCB = (CComboBox *)GetDlgItem(IDC_COMBO_FREEZE_HOUR);
	pCB->GetLBText(pCB->GetCurSel(), buffer);
	pWnd->m_FreezeTime[2] = C_MRULE::String2BCD(buffer);
	pCB = (CComboBox *)GetDlgItem(IDC_COMBO_FREEZE_MIN);
	pCB->GetLBText(pCB->GetCurSel(), buffer);
	pWnd->m_FreezeTime[3] = C_MRULE::String2BCD(buffer);

	pEdit = (CEdit *)GetDlgItem(IDC_EDIT_WRITE_DATA);
	pEdit->GetWindowText(buffer, sizeof(buffer));
	pWnd->m_WriteData = buffer;

	CDialog::OnOK();
}

BOOL CMDLTSetDlg::OnCommand(WPARAM wParam,LPARAM lParam)
{
	switch ((int)(wParam&0xFFFF))
	{
	case IDC_COMBO_CTRL_CODE:
		return OnComboBoxCtrlCode(wParam,lParam);
	default:
		return CDialog::OnCommand(wParam,lParam);
	}

	return true;
}

BOOL CMDLTSetDlg::OnComboBoxCtrlCode(WPARAM wParam,LPARAM lParam)
{
	if (LOWORD(wParam) != IDC_COMBO_CTRL_CODE)
		return false;

	if (HIWORD(wParam) == CBN_SELCHANGE)
	{
		CHcommDlg *pWnd = (CHcommDlg *)GetParent();
		CComboBox *pCB  = (CComboBox *)GetDlgItem(IDC_COMBO_CTRL_CODE);
		if (pWnd->m_DltRuleCtrlCode == pCB->GetCurSel())
			return false;

		pWnd->m_DltRuleCtrlCode = pCB->GetCurSel();

		CStatic *pStatic = (CStatic *)GetDlgItem(IDC_STATIC_DATA_NAME);
		if (pCurMrule->IsPasswordCtrlCode(pWnd->m_DltRuleCtrlCode) == true)
			pStatic->SetWindowText("新密码");
		else
			pStatic->SetWindowText("数  据");
		EnableCtrlWnd(pWnd->m_DltRuleCtrlCode);
	}

	return true;
}

const int FreezeTimeWndID[] =
{
	IDC_STATIC_FREEZE_TITLE,
	IDC_STATIC_FREEZE_MON,
	IDC_STATIC_FREEZE_DAY,
	IDC_STATIC_FREEZE_HOUR,
	IDC_STATIC_FREEZE_MIN,
	IDC_COMBO_FREEZE_MON,
	IDC_COMBO_FREEZE_DAY,
	IDC_COMBO_FREEZE_HOUR,
	IDC_COMBO_FREEZE_MIN,
};

BOOL CMDLTSetDlg::EnableCtrlWnd(int CtrlCodeSN)
{
	CWnd *pWnd;

	for (int i = 0; i < sizeof(FreezeTimeWndID)/sizeof(FreezeTimeWndID[0]); ++i)
	{
		pWnd = GetDlgItem(FreezeTimeWndID[i]);
		pWnd->EnableWindow(pCurMrule->EnableFreezeWnd(CtrlCodeSN));
	}

	pWnd = GetDlgItem(IDC_STATIC_OP_CODE);
	pWnd->EnableWindow(pCurMrule->EnableOpCodeWnd(CtrlCodeSN));
	pWnd = GetDlgItem(IDC_EDIT_OP_CODE);
	pWnd->EnableWindow(pCurMrule->EnableOpCodeWnd(CtrlCodeSN));

	pWnd = GetDlgItem(IDC_STATIC_PASSWORD);
	pWnd->EnableWindow(pCurMrule->EnablePasswordWnd(CtrlCodeSN));
	pWnd = GetDlgItem(IDC_EDIT_PASSWORD);
	pWnd->EnableWindow(pCurMrule->EnablePasswordWnd(CtrlCodeSN));

	pWnd = GetDlgItem(IDC_STATIC_DATA_NAME);
	pWnd->EnableWindow(pCurMrule->EnableWriteDataWnd(CtrlCodeSN));
	pWnd = GetDlgItem(IDC_EDIT_WRITE_DATA);
	pWnd->EnableWindow(pCurMrule->EnableWriteDataWnd(CtrlCodeSN));

	return true;
}


