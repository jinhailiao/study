// GW007Dlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "GW007.h"
#include "GW007Dlg.h"
#include ".\gw007dlg.h"
#include "gwmsrule.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CGW007Dlg �Ի���



CGW007Dlg::CGW007Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGW007Dlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hAccel = ::LoadAccelerators(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_ACCELERATOR_MAIN));

	m_ProtocolSN = 0;
	m_AfnSN = 0;
}

void CGW007Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CGW007Dlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CGW007Dlg ��Ϣ�������

BOOL CGW007Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��\������...\���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	if (LoadMasterRule(m_Rules, "rule.ini") != 0)
	{
		MessageBox(_T("��Լ��������ʧ��"), _T("����"));
	}

	CComboBox *pCombo;
	//�淶����
	pCombo = (CComboBox *)GetDlgItem(IDC_COMBO_PROTOCOL);
	for (int i = 0; i < m_Rules.m_RuleCnt; i++)
	{
		if (m_Rules.m_RuleItem[i].m_Enable == false)
			break;
		pCombo->AddString(m_Rules.m_RuleItem[i].m_Name);
	}
	
	InitCtrl();

	return TRUE;  // ���������˿ؼ��Ľ��㣬���򷵻� TRUE
}

void CGW007Dlg::InitCtrl(void)
{
	CComboBox *pCombo;
	CEdit *pEdit;

	m_ProtocolSN = 0;
	m_AfnSN = 0;

	//�淶����
	pCombo = (CComboBox *)GetDlgItem(IDC_COMBO_PROTOCOL);
	pCombo->SetCurSel(m_ProtocolSN);
	//Ӧ�ù������趨
	pCombo = (CComboBox *)GetDlgItem(IDC_COMBO_AFN);
	pCombo->ResetContent();
	S_RULE &CurRule = m_Rules.m_RuleItem[m_ProtocolSN];
	for (int i = 0; i < CurRule.m_AfnCnt; i++)
	{
		if (CurRule.m_AfnItem[i].m_Enable == false)
			break;
		pCombo->AddString(CurRule.m_AfnItem[i].m_Name);
	}
	pCombo->SetCurSel(m_AfnSN);

	//SEQ
	pEdit = (CEdit *)GetDlgItem(IDC_EDIT_SEQ);
	pEdit->SetWindowText(_T("0"));
	//��ʱ
	pEdit = (CEdit *)GetDlgItem(IDC_EDIT_DELAY);
	pEdit->SetWindowText(_T("0"));
	//��վ��ַ
	pEdit = (CEdit *)GetDlgItem(IDC_EDIT_MASTER);
	pEdit->SetWindowText(_T("1"));
	//�ն˵�ַ
	pEdit = (CEdit *)GetDlgItem(IDC_EDIT_TERM);
	pEdit->SetWindowText(_T("50010001"));
	//��Ϣ��
	pEdit = (CEdit *)GetDlgItem(IDC_EDIT_DA);
	pEdit->SetWindowText(_T("0"));
	//��Ϣ��
	pEdit = (CEdit *)GetDlgItem(IDC_EDIT_DT);
	pEdit->SetWindowText(_T("1"));
	//PN
	pEdit = (CEdit *)GetDlgItem(IDC_EDIT_PN);
	pEdit->SetWindowText(_T("0"));
	//FN
	pEdit = (CEdit *)GetDlgItem(IDC_EDIT_FN);
	pEdit->SetWindowText(_T("1"));
	//����
	pEdit = (CEdit *)GetDlgItem(IDC_EDIT_PW);
	pEdit->SetWindowText(_T(""));
	if (CurRule.m_AfnItem[m_AfnSN].m_PW == true)
		pEdit->EnableWindow(true);
	else
		pEdit->EnableWindow(false);
	//���ݿ�
	pEdit = (CEdit *)GetDlgItem(IDC_EDIT_DATA);
	pEdit->SetWindowText(_T(""));
	//���ݿ�
	pEdit = (CEdit *)GetDlgItem(IDC_EDIT_FRAME);
	pEdit->SetWindowText(_T("�༭��������ʮ��������ʮ���������룬����ʮ�������롣"));
}

//������Ϣת������
BOOL CGW007Dlg::PreTranslateMessage(MSG* pMsg)
{
	if (m_hAccel) 
	{//ת����ݼ�
		if (::TranslateAccelerator(m_hWnd, m_hAccel, pMsg)) 
		{
			return(TRUE);
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CGW007Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CGW007Dlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ��������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
HCURSOR CGW007Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CGW007Dlg::OnCommand(WPARAM wParam,LPARAM lParam)
{
	switch ((int)(wParam&0xFFFF))
	{
	case IDC_COMBO_PROTOCOL:
		return OnComboProtocol(wParam,lParam);
	case IDC_COMBO_AFN:
		return OnComboAfn(wParam,lParam);
	case IDC_EDIT_SEQ:
		return OnEditSeq(wParam,lParam);
	case IDC_EDIT_DELAY:
		return OnEditDelay(wParam,lParam);
	case IDC_EDIT_MASTER:
		return OnEditMaster(wParam,lParam);
	case IDC_EDIT_TERM:
		return OnEditTerm(wParam,lParam);
	case IDC_EDIT_DA:
		return OnEditDA(wParam,lParam);
	case IDC_EDIT_DT:
		return OnEditDT(wParam,lParam);
	case IDC_EDIT_PN:
		return OnEditPN(wParam,lParam);
	case IDC_EDIT_FN:
		return OnEditFN(wParam,lParam);
	case IDC_EDIT_PW:
		return OnEditPW(wParam,lParam);
	case IDC_EDIT_DATA:
		return OnEditData(wParam,lParam);
	case IDC_EDIT_FRAME:
		return OnEditFrame(wParam,lParam);
	case IDC_BUTTON_MAKE:
		return OnButtonMake(wParam,lParam);
	case IDC_BUTTON_RESET:
		return OnButtonReset(wParam,lParam);
	case IDC_BUTTON_ABOUT:
		return OnButtonAbout(wParam,lParam);
	default:
		return CDialog::OnCommand(wParam,lParam);
	}
	return FALSE;
}

bool CGW007Dlg::GetFrameData(S_FrameData &FrameData)
{
	char Buffer[4096];
	CEdit *pEdit = NULL;
	string::iterator iter;

	FrameData.m_AfnSN = m_AfnSN;

	pEdit = (CEdit *)GetDlgItem(IDC_EDIT_SEQ);
	pEdit->GetWindowText(Buffer, sizeof(Buffer));
	FrameData.m_strSEQ.assign(Buffer);

	pEdit = (CEdit *)GetDlgItem(IDC_EDIT_DELAY);
	pEdit->GetWindowText(Buffer, sizeof(Buffer));
	FrameData.m_strDelay.assign(Buffer);

	pEdit = (CEdit *)GetDlgItem(IDC_EDIT_MASTER);
	pEdit->GetWindowText(Buffer, sizeof(Buffer));
	FrameData.m_strMaster.assign(Buffer);

	pEdit = (CEdit *)GetDlgItem(IDC_EDIT_TERM);
	pEdit->GetWindowText(Buffer, sizeof(Buffer));
	FrameData.m_strTerm.assign(Buffer);
	iter = remove(FrameData.m_strTerm.begin(), FrameData.m_strTerm.end(), ' ');
	FrameData.m_strTerm.erase(iter, FrameData.m_strTerm.end());

	pEdit = (CEdit *)GetDlgItem(IDC_EDIT_DA);
	pEdit->GetWindowText(Buffer, sizeof(Buffer));
	FrameData.m_strDA.assign(Buffer);
	iter = remove(FrameData.m_strDA.begin(), FrameData.m_strDA.end(), ' ');
	FrameData.m_strDA.erase(iter, FrameData.m_strDA.end());

	pEdit = (CEdit *)GetDlgItem(IDC_EDIT_DT);
	pEdit->GetWindowText(Buffer, sizeof(Buffer));
	FrameData.m_strDT.assign(Buffer);
	iter = remove(FrameData.m_strDT.begin(), FrameData.m_strDT.end(), ' ');
	FrameData.m_strDT.erase(iter, FrameData.m_strDT.end());

	pEdit = (CEdit *)GetDlgItem(IDC_EDIT_PW);
	pEdit->GetWindowText(Buffer, sizeof(Buffer));
	FrameData.m_strPW.assign(Buffer);
	iter = remove(FrameData.m_strPW.begin(), FrameData.m_strPW.end(), ' ');
	FrameData.m_strPW.erase(iter, FrameData.m_strPW.end());

	if ((FrameData.m_strPW.size() % 2) != 0)
	{
		MessageBox(_T("���볤�ȴ�����Ҫż��������!"), _T("����"));
		return false;
	}

	pEdit = (CEdit *)GetDlgItem(IDC_EDIT_DATA);
	pEdit->GetWindowText(Buffer, sizeof(Buffer) - 1);
	FrameData.m_strData.assign(Buffer);
	iter = remove(FrameData.m_strData.begin(), FrameData.m_strData.end(), ' ');
	FrameData.m_strData.erase(iter, FrameData.m_strData.end());
	
	if ((FrameData.m_strData.size() % 2) != 0)
	{
		MessageBox(_T("���ݵ�Ԫ���ȴ�����Ҫż��������!"), _T("����"));
		return false;
	}

	return true;
}

bool CGW007Dlg::OnComboProtocol(WPARAM wParam,LPARAM lParam)
{
	if (LOWORD(wParam) != IDC_COMBO_PROTOCOL)
		return FALSE;

	CComboBox *pCB = (CComboBox *)GetDlgItem(IDC_COMBO_PROTOCOL);
	if (lParam == NULL)//��ݼ�ѡ��
	{
		pCB->SetFocus();
	}
	else if (HIWORD(wParam) == CBN_SELCHANGE)
	{
		if (m_ProtocolSN == pCB->GetCurSel())
			return FALSE;
		m_ProtocolSN = pCB->GetCurSel();
		m_AfnSN = 0;

		//Ӧ�ù������趨
		CComboBox *pCombo = (CComboBox *)GetDlgItem(IDC_COMBO_AFN);
		pCombo->ResetContent();
		S_RULE &CurRule = m_Rules.m_RuleItem[m_ProtocolSN];
		for (int i = 0; i < CurRule.m_AfnCnt; i++)
		{
			if (CurRule.m_AfnItem[i].m_Enable == false)
				break;
			pCombo->AddString(CurRule.m_AfnItem[i].m_Name);
		}
		pCombo->SetCurSel(m_AfnSN);
		//����
		CEdit *pEdit = (CEdit *)GetDlgItem(IDC_EDIT_PW);
		pEdit->SetWindowText(_T(""));
		S_RULE *pCurRule = &m_Rules.m_RuleItem[m_ProtocolSN];
		if (pCurRule->m_AfnItem[m_AfnSN].m_PW == true)
			pEdit->EnableWindow(true);
		else
			pEdit->EnableWindow(false);
	}
	return TRUE;
}

bool CGW007Dlg::OnComboAfn(WPARAM wParam,LPARAM lParam)
{
	if (LOWORD(wParam) != IDC_COMBO_AFN)
		return FALSE;

	CComboBox *pCB = (CComboBox *)GetDlgItem(IDC_COMBO_AFN);
	if (lParam == NULL)//��ݼ�ѡ��
	{
		pCB->SetFocus();
	}
	else if (HIWORD(wParam) == CBN_SELCHANGE)
	{
		if (m_AfnSN == pCB->GetCurSel())
			return FALSE;
		m_AfnSN = pCB->GetCurSel();

		//����
		CEdit *pEdit = (CEdit *)GetDlgItem(IDC_EDIT_PW);
//		pEdit->SetWindowText(_T(""));
		S_RULE *pCurRule = &m_Rules.m_RuleItem[m_ProtocolSN];
		if (pCurRule->m_AfnItem[m_AfnSN].m_PW == true)
			pEdit->EnableWindow(true);
		else
			pEdit->EnableWindow(false);
	}
	return TRUE;
}

bool CGW007Dlg::OnEditSeq(WPARAM wParam,LPARAM lParam)
{
	if (LOWORD(wParam) != IDC_EDIT_SEQ)
		return FALSE;

	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_EDIT_SEQ);
	if (lParam == NULL)//��ݼ�ѡ��
	{
		pEdit->SetFocus();
	}
	return TRUE;
}

bool CGW007Dlg::OnEditDelay(WPARAM wParam,LPARAM lParam)
{
	if (LOWORD(wParam) != IDC_EDIT_DELAY)
		return FALSE;

	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_EDIT_DELAY);
	if (lParam == NULL)//��ݼ�ѡ��
	{
		pEdit->SetFocus();
	}
	return TRUE;
}

bool CGW007Dlg::OnEditMaster(WPARAM wParam,LPARAM lParam)
{
	if (LOWORD(wParam) != IDC_EDIT_MASTER)
		return FALSE;

	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_EDIT_MASTER);
	if (lParam == NULL)//��ݼ�ѡ��
	{
		pEdit->SetFocus();
	}
	return TRUE;
}

bool CGW007Dlg::OnEditTerm(WPARAM wParam,LPARAM lParam)
{
	if (LOWORD(wParam) != IDC_EDIT_TERM)
		return FALSE;

	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_EDIT_TERM);
	if (lParam == NULL)//��ݼ�ѡ��
	{
		pEdit->SetFocus();
	}
	return TRUE;
}

bool CGW007Dlg::OnEditDA(WPARAM wParam,LPARAM lParam)
{
	if (LOWORD(wParam) != IDC_EDIT_DA)
		return FALSE;

	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_EDIT_DA);
	if (lParam == NULL)//��ݼ�ѡ��
	{
		pEdit->SetFocus();
	}
	return TRUE;
}

bool CGW007Dlg::OnEditDT(WPARAM wParam,LPARAM lParam)
{
	if (LOWORD(wParam) != IDC_EDIT_DT)
		return FALSE;

	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_EDIT_DT);
	if (lParam == NULL)//��ݼ�ѡ��
	{
		pEdit->SetFocus();
	}
	return TRUE;
}

bool CGW007Dlg::OnEditPN(WPARAM wParam,LPARAM lParam)
{
	if (LOWORD(wParam) != IDC_EDIT_PN)
		return FALSE;

	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_EDIT_PN);
	if (lParam == NULL)//��ݼ�ѡ��
	{
		pEdit->SetFocus();
	}
	else if (HIWORD(wParam) == EN_UPDATE)
	{
		char Buffer[32];
		pEdit->GetWindowText(Buffer, 32);
		int PN = atoi(Buffer);
		int DA = C_DAPN::PN2DA(PN);
		sprintf(Buffer, "%04X", KGE_SWAPWORD(DA));
		pEdit = (CEdit *)GetDlgItem(IDC_EDIT_DA);
		pEdit->SetWindowText(Buffer);
	}
	return TRUE;
}

bool CGW007Dlg::OnEditFN(WPARAM wParam,LPARAM lParam)
{
	if (LOWORD(wParam) != IDC_EDIT_FN)
		return FALSE;

	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_EDIT_FN);
	if (lParam == NULL)//��ݼ�ѡ��
	{
		pEdit->SetFocus();
	}
	else if (HIWORD(wParam) == EN_UPDATE)
	{
		char Buffer[32];
		pEdit->GetWindowText(Buffer, 32);
		int FN = atoi(Buffer);
		int DT = C_DTFN::FN2DT(FN);
		sprintf(Buffer, "%04X", KGE_SWAPWORD(DT));
		pEdit = (CEdit *)GetDlgItem(IDC_EDIT_DT);
		pEdit->SetWindowText(Buffer);
	}
	return TRUE;
}

bool CGW007Dlg::OnEditPW(WPARAM wParam,LPARAM lParam)
{
	if (LOWORD(wParam) != IDC_EDIT_PW)
		return FALSE;

	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_EDIT_PW);
	if (lParam == NULL)//��ݼ�ѡ��
	{
		pEdit->SetFocus();
	}
	return TRUE;
}

bool CGW007Dlg::OnEditData(WPARAM wParam,LPARAM lParam)
{
	if (LOWORD(wParam) != IDC_EDIT_DATA)
		return FALSE;

	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_EDIT_DATA);
	if (lParam == NULL)//��ݼ�ѡ��
	{
		pEdit->SetFocus();
	}
	return TRUE;
}

bool CGW007Dlg::OnEditFrame(WPARAM wParam,LPARAM lParam)
{
	if (LOWORD(wParam) != IDC_EDIT_FRAME)
		return FALSE;

	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_EDIT_FRAME);
	if (lParam == NULL)//��ݼ�ѡ��
	{
		pEdit->SetFocus();
	}
	return TRUE;
}

bool CGW007Dlg::OnButtonMake(WPARAM wParam,LPARAM lParam)
{
	if (LOWORD(wParam) != IDC_BUTTON_MAKE)
		return FALSE;

	CButton *pBtn  = (CButton *)GetDlgItem(IDC_BUTTON_MAKE);
	if (lParam == NULL)
	{
		pBtn->SetFocus();
	}
	S_FrameData FrameData;
	if (GetFrameData(FrameData) == true)
	{
		string strFrame;
		if (C_FRAME::MakeFrame(m_Rules.m_RuleItem[m_ProtocolSN], FrameData, strFrame) > 0)
		{
			C_String2Char frameAscii;
			for_Stat(strFrame.begin(), strFrame.end(), frameAscii);
			CEdit *pEdit = (CEdit *)GetDlgItem(IDC_EDIT_FRAME);
			pEdit->SetWindowText(frameAscii.m_strAscii.c_str());
			
			char Buffer[32];
			pEdit = (CEdit *)GetDlgItem(IDC_EDIT_SEQ);
			pEdit->GetWindowText(Buffer, sizeof(Buffer));
			int seq = atoi(Buffer) + 1;
			sprintf(Buffer, "%d", seq);
			pEdit->SetWindowText(Buffer);
		}
	}

	return TRUE;
}

bool CGW007Dlg::OnButtonReset(WPARAM wParam,LPARAM lParam)
{
	if (LOWORD(wParam) != IDC_BUTTON_RESET)
		return FALSE;

	CButton *pBtn  = (CButton *)GetDlgItem(IDC_BUTTON_RESET);
	if (lParam == NULL)
	{
		pBtn->SetFocus();
	}
	InitCtrl();

	return TRUE;
}

bool CGW007Dlg::OnButtonAbout(WPARAM wParam,LPARAM lParam)
{
	if (LOWORD(wParam) != IDC_BUTTON_ABOUT)
		return FALSE;

	CButton *pBtn  = (CButton *)GetDlgItem(IDC_BUTTON_ABOUT);
	if (lParam == NULL)
	{
		pBtn->SetFocus();
	}

	CAboutDlg dlgAbout;
	dlgAbout.DoModal();

	return TRUE;
}


