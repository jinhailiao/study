// HcommDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Hcomm.h"
#include "HcommDlg.h"
#include ".\hcommdlg.h"
#include "meterrul.h"
#include "MSetDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define IDEVENT_SEND 100
#define DLG_HEIGHT	 501
#define DLG_WIDTH_MIN 586
#define SCOMM_SENDBUF_SIZE 64/*512*/

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CHcommDlg 对话框



CHcommDlg::CHcommDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHcommDlg::IDD, pParent)
{
	m_serialPort  = SPORT_COM1;
	m_serialBaud  = SPORT_1200;
	m_serialCheck = SPORT_EVEN;
	m_serialNum   = SPORT_NUM8;
	m_serialStop  = SPORT_STOP1;

	m_watchPort  = false;
	m_hexShow    = true;
	m_hexSend    = true;
	m_autoSend   = false;
	m_sendPeriod = 1000;

	m_autoFrame = true;
	m_meterRule = MRULE_645_1997;
	m_meterReg  = 0;
	m_meterAddr = "999999999999";

	if (m_meterRule == MRULE_645_1997)
		pCurMrule = &MRule97;
	else if (m_meterRule == MRULE_645_2007)
		pCurMrule = &MRule07;
	else if (m_meterRule == MRULE_LGR)
		pCurMrule = &MRuleLgr;

	m_DltRuleCtrlCode = 0;
//	m_OpCode = "";
//	m_Password = "";
	m_FreezeTime = string("\x99\x99\x99\x99");
//	m_WriteData = "";

	m_LGRlogin = false;
	m_LgrAck = LGR_ACK_AUTO;
	m_cx = 0;
	m_FileSending = false;

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hAccel = ::LoadAccelerators(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	m_hLightW = ::LoadBitmap(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_BITMAP_DARK));
	m_hLightR = ::LoadBitmap(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_BITMAP_LIGHT));
}

void CHcommDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMM_SPORT, m_Scomm);
	DDX_Text(pDX, IDC_EDIT_RECEIVE, m_StrReceive);
	DDX_Text(pDX, IDC_EDIT_SEND, m_StrSend);

	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_EDIT_RECEIVE);
	pEdit->LineScroll(pEdit->GetLineCount());
//	pEdit->SetSel(-1, -1);
}

BEGIN_MESSAGE_MAP(CHcommDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CHcommDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将\“关于...\”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标


	// TODO: 在此添加额外的初始化代码
	//加载电表规约信息
	C_MRULELOAD::LoadAll();

	const char *pstring;
	CComboBox *pCombo;
	CStatic *pStatic;
	CButton *pButton;
	CEdit *pEdit;
	//串口设置
	pCombo = (CComboBox *)GetDlgItem(IDC_COMBO_PORT);
	pCombo->SetCurSel(m_serialPort);
	//波特率设置
	pCombo = (CComboBox *)GetDlgItem(IDC_COMBO_BAUD);
	pCombo->SetCurSel(m_serialBaud);
	//校验位设置
	pCombo = (CComboBox *)GetDlgItem(IDC_COMBO_CHECK);
	pCombo->SetCurSel(m_serialCheck);
	//数据位设置
	pCombo = (CComboBox *)GetDlgItem(IDC_COMBO_NUM);
	pCombo->SetCurSel(m_serialNum);
	//停止位设置
	pCombo = (CComboBox *)GetDlgItem(IDC_COMBO_STOP);
	pCombo->SetCurSel(m_serialStop);
	//串口灯显示
	pStatic = (CStatic *)GetDlgItem(IDC_STATIC_LIGHT);
	pStatic->SetBitmap(m_hLightW);
	//十六进制显示
	pButton = (CButton *)GetDlgItem(IDC_CHECK_HEXSHOW);
	pButton->SetCheck(m_hexShow);
	//十六进制发送
	pButton = (CButton *)GetDlgItem(IDC_CHECK_HEXSEND);
	pButton->SetCheck(m_hexSend);
	//显示默认发送周期
	pEdit = (CEdit *)GetDlgItem(IDC_EDIT_SECOND);
	pEdit->SetWindowText("1000");
	//默认自动组帧
	pButton = (CButton *)GetDlgItem(IDC_CHECK_AUTOFRAME);
	pButton->SetCheck(m_autoFrame);
	//初始化电表规约名字
	pCombo = (CComboBox *)GetDlgItem(IDC_COMBO_RULE);
	for (int i = 0; i < MRULE_MAX; i++)
		pCombo->AddString(pMeterRule[i]);
	pCombo->SetCurSel(m_meterRule);
	//显示默认地址
	pEdit = (CEdit *)GetDlgItem(IDC_EDIT_ADDR);
	pEdit->SetWindowText(m_meterAddr.c_str());
	//初始化电表数据ID
	pCombo = (CComboBox *)GetDlgItem(IDC_COMBO_DATAID);
	for (int i = 0; i < pCurMrule->GetMRuleDataIDCount(); i++)
	{
		pstring = pCurMrule->GetMRuleDataID(i);
		pCombo->AddString(pstring);
	}
	pCombo->SetCurSel(m_meterReg);
	//显示电表数据ID名称
	pStatic = (CStatic *)GetDlgItem(IDC_STATIC_REGNAME);
	pstring = pCurMrule->GetMRuleRegName(m_meterReg);
	pStatic->SetWindowText(pstring);
	//相关窗口变灰
	pStatic = (CStatic *)GetDlgItem(IDC_STATIC_ADDR);
	pStatic->EnableWindow(m_autoFrame && pCurMrule->EnableAddressWnd(m_DltRuleCtrlCode));
	pEdit = (CEdit *)GetDlgItem(IDC_EDIT_ADDR);
	pEdit->EnableWindow(m_autoFrame && pCurMrule->EnableAddressWnd(m_DltRuleCtrlCode));
	pStatic = (CStatic *)GetDlgItem(IDC_STATIC_DATAID);
	pStatic->EnableWindow(m_autoFrame && pCurMrule->EnableDataIDWnd(m_DltRuleCtrlCode));
	pCombo = (CComboBox *)GetDlgItem(IDC_COMBO_DATAID);
	pCombo->EnableWindow(m_autoFrame && pCurMrule->EnableDataIDWnd(m_DltRuleCtrlCode));
	//自动滚屏
//	pEdit = (CEdit *)GetDlgItem(IDC_EDIT_RECEIVE);
//	pEdit->SetSel(-1, -1);
	//组帧
	MakeFrameAndShow();

	// Create the MSCOMM32 OLE Control.
//	int err = m_Scomm.Create(NULL,0,CRect(0,0,0,0),this,IDC_COMM_SPORT);

	return TRUE;  // 除非设置了控件的焦点，否则返回 TRUE
}

void CHcommDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CHcommDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
HCURSOR CHcommDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

static int SizeCtl[] = 
{
	IDC_STATIC_RECEIVE,
	IDC_EDIT_RECEIVE,
	IDC_STATIC_SEND,
	IDC_EDIT_SEND,
	IDC_STATIC_OP,
	IDC_EDIT_PATH
};
afx_msg void CHcommDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	RECT r;
	int dx = cx - m_cx;
	if (nType == SIZE_RESTORED && m_cx)
	{
		for (int i = 0; i < sizeof(SizeCtl)/sizeof(SizeCtl[0]); i++)
		{
			CWnd *pWnd = GetDlgItem(SizeCtl[i]);
			if (pWnd)
			{
				pWnd->GetWindowRect(&r);
				ScreenToClient(&r);
				r.right += dx;
				pWnd->MoveWindow(&r);
			}
		}
	}
	m_cx = cx;
}

afx_msg void CHcommDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
    lpMMI->ptMinTrackSize.x = DLG_WIDTH_MIN;
    lpMMI->ptMinTrackSize.y = DLG_HEIGHT;
	lpMMI->ptMaxTrackSize.y = DLG_HEIGHT;
	
	CDialog::OnGetMinMaxInfo(lpMMI);
}


//重载消息转换函数
BOOL CHcommDlg::PreTranslateMessage(MSG* pMsg)
{
	if (m_hAccel) 
	{//转换快捷键
		if (::TranslateAccelerator(m_hWnd, m_hAccel, pMsg)) 
		{
			return(TRUE);
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

//重载退出函数：按ESC不退出
void CHcommDlg::OnCancel()
{
//	CDialog::OnCancel();
	SendMessage(WM_SYSCOMMAND, (WPARAM)SC_MINIMIZE, (LPARAM)0);
}

//重载OK函数：按回车不退出
void CHcommDlg::OnOK()
{
}

//添加，退出函数
void CHcommDlg::OnClose()
{
	CDialog::OnCancel();
}

// add eventsink map to hook OnComm event
BEGIN_EVENTSINK_MAP(CHcommDlg, CDialog)
	ON_EVENT(CHcommDlg, IDC_COMM_SPORT, 1/* OnComm */, OnCommEvent, VTS_NONE)
END_EVENTSINK_MAP()

// CHcommDlg 消息处理程序
void CHcommDlg::OnCommEvent()
{
	long len;
	VARIANT var;
    COleSafeArray arr;
	BYTE CommBuf[2048];
	static CString LgrLogin = _T("");
	short evt = m_Scomm.GetCommEvent();

	memset(CommBuf, 0x00, sizeof(CommBuf));
	switch (evt)
	{
	case 1://send
		if ((len = SFile.Read(CommBuf, SCOMM_SENDBUF_SIZE)) == 0)
		{
			PostMessage(WM_COMMAND, (WPARAM)IDC_BUTTON_SENDFILE, (LPARAM)0);
		}
		else
		{
			CProgressCtrl *pProgress = (CProgressCtrl *)GetDlgItem(IDC_PROGRESS_SEND);
			pProgress->OffsetPos(1);
			CByteArray arrayBuf;
			for (long i = 0; i < len; i++)
				arrayBuf.Add(CommBuf[i]);
			m_Scomm.SetOutput(COleVariant(arrayBuf));
		}
		break;
	case 2://receive
		var = m_Scomm.GetInput();
        arr = var; //VARIANT ==> ColeSafeArray
        len = arr.GetOneDimSize();
        for(long k = 0; k < len; k++)
            arr.GetElement(&k,CommBuf+k);//convert to BYTE
		if (m_LGRlogin == true)//handle LGR meter login
		{
			CString Strtmp;
			Strtmp.Format(_T("%s"),CommBuf);
			LgrLogin += Strtmp;
			int off = LgrLogin.Find(_T("/LGZ"));
			int len   = LgrLogin.GetLength();
			if ((off >= 0) && (off+4 < len)) //receive lgr login packet
			{
				char baud = LgrLogin[off+4];//get baud flag
				PostMessage(WM_COMMAND, (WPARAM)IDC_BUTTON_LGRACK, (LPARAM)baud);
				LgrLogin = _T("");
				m_LGRlogin = false;
			}
			else if (len >= 256) //not find flag and the string is too long
			{
				LgrLogin = _T("");
				m_LGRlogin = false;//exit find lgr login flag
			}
		}
		if (m_StrReceive.GetLength() > 4096)//more than 4K
			m_StrReceive = _T("");//clear
		for(long k = 0; k < len; k++) //将数组转换为Cstring型变量
        {
			if (m_hexShow)
			{
				m_StrReceive += C_MRULE::Digit2Ascii((CommBuf[k]>>4)&0xF);
				m_StrReceive += C_MRULE::Digit2Ascii(CommBuf[k]&0xF);
				m_StrReceive += ' ';
			}
			else
			{
				if (CommBuf[k])//TODO:if DBCS have 0 .......
                    m_StrReceive.AppendChar(CommBuf[k]);
			}
        }
		UpdateData(false);
		break;
	default://other
		m_Scomm.SetOutBufferCount(0);
		break;
	}
}

BOOL CHcommDlg::OnCommand(WPARAM wParam,LPARAM lParam)
{
	switch ((int)(wParam&0xFFFF))
	{
	case IDC_COMBO_PORT:
		return OnComboPort(wParam, lParam);
	case IDC_COMBO_BAUD:
		return OnComboBaud(wParam, lParam);
	case IDC_COMBO_CHECK:
		return OnComboCheck(wParam, lParam);
	case IDC_COMBO_NUM:
		return OnComboNum(wParam, lParam);
	case IDC_COMBO_STOP:
		return OnComboStop(wParam, lParam);
	case IDC_CHECK_WATCH:
		return OnCheckWatch(wParam, lParam);
	case IDC_CHECK_HEXSHOW:
		return OnCheckHexshow(wParam, lParam);
	case IDC_CHECK_HEXSEND:
		return OnCheckHexsend(wParam, lParam);
	case IDC_CHECK_AUTOSEND:
		return OnCheckAutosend(wParam, lParam);
	case IDC_EDIT_SECOND:
		return OnEditMSecond(wParam,lParam);
	case IDC_CHECK_AUTOFRAME:
		return OnCheckAutoFrame(wParam,lParam);
	case IDC_BUTTON_MSET:
		return OnButtonMSet(wParam, lParam);
	case IDC_COMBO_RULE:
		return OnComboRule(wParam, lParam);
	case IDC_EDIT_ADDR:
		return OnEditAddr(wParam, lParam);
	case IDC_COMBO_DATAID:
		return OnComboDataID(wParam, lParam);
	case IDC_EDIT_RECEIVE:
		return OnEditReceive(wParam, lParam);
	case IDC_EDIT_SEND:
		return OnEditSend(wParam, lParam);
	case IDC_BUTTON_CLRRECEIVE:
		return OnButtonClrReceive(wParam, lParam);
	case IDC_BUTTON_CLRSEND:
		return OnButtonClrSend(wParam, lParam);
	case IDC_BUTTON_MANUALSEND:
		return OnButtonManualSend(wParam, lParam);
	case IDC_BUTTON_SENDFILE:
		return OnButtonSendFile(wParam, lParam);
	case IDC_BUTTON_ABOUT:
		return OnButtonAbort(wParam, lParam);
	case IDC_BUTTON_SELECTFILE:
		return OnButtonOpen(wParam, lParam);
	case IDC_EDIT_PATH:
		return OnEditPath(wParam, lParam);
	case IDC_BUTTON_LGRACK:
		return OnButtonLgrAck(wParam, lParam);
	default:
		return CDialog::OnCommand(wParam,lParam);
	}
	return FALSE;
}

bool CHcommDlg::OnComboPort(WPARAM wParam,LPARAM lParam)
{
	if (LOWORD(wParam) != IDC_COMBO_PORT)
		return false;

	CComboBox *pCB = (CComboBox *)GetDlgItem(IDC_COMBO_PORT);
	if (lParam == NULL)//快捷键选中
	{
		pCB->SetFocus();
	}
	else if (HIWORD(wParam) == CBN_SELCHANGE)
	{
		if (m_serialPort == pCB->GetCurSel())
			return false;
		m_serialPort = pCB->GetCurSel();
		if (m_watchPort)
		{
			OnCheckWatch((WPARAM)IDC_CHECK_WATCH,(LPARAM)NULL);
			OnCheckWatch((WPARAM)IDC_CHECK_WATCH,(LPARAM)NULL);
			pCB->SetFocus();
		}
	}
	return true;
}

bool CHcommDlg::OnComboBaud(WPARAM wParam,LPARAM lParam)
{
	if (LOWORD(wParam) != IDC_COMBO_BAUD)
		return false;

	CComboBox *pCB = (CComboBox *)GetDlgItem(IDC_COMBO_BAUD);
	if (lParam == NULL)//快捷键选中
	{
		pCB->SetFocus();
	}
	else if (HIWORD(wParam) == CBN_SELCHANGE)
	{
		if (m_serialBaud == pCB->GetCurSel())
			return false;
		m_serialBaud = pCB->GetCurSel();
		if (m_watchPort)
		{
			m_Scomm.SetSettings(GetPortSetting());
		}
	}
	return true;
}

bool CHcommDlg::OnComboCheck(WPARAM wParam,LPARAM lParam)
{
	if (LOWORD(wParam) != IDC_COMBO_CHECK)
		return false;

	CComboBox *pCB = (CComboBox *)GetDlgItem(IDC_COMBO_CHECK);
	if (lParam == NULL)//快捷键选中
	{
		pCB->SetFocus();
	}
	else if (HIWORD(wParam) == CBN_SELCHANGE)
	{
		if (m_serialCheck == pCB->GetCurSel())
			return false;
		m_serialCheck = pCB->GetCurSel();
		if (m_watchPort)
		{
			m_Scomm.SetSettings(GetPortSetting());
		}
	}
	return true;
}

bool CHcommDlg::OnComboNum(WPARAM wParam,LPARAM lParam)
{
	if (LOWORD(wParam) != IDC_COMBO_NUM)
		return false;

	CComboBox *pCB = (CComboBox *)GetDlgItem(IDC_COMBO_NUM);
	if (lParam == NULL)//快捷键选中
	{
		pCB->SetFocus();
	}
	else if (HIWORD(wParam) == CBN_SELCHANGE)
	{
		if (m_serialNum == pCB->GetCurSel())
			return false;
		m_serialNum = pCB->GetCurSel();
		if (m_watchPort)
		{
			m_Scomm.SetSettings(GetPortSetting());
		}
	}
	return true;
}

bool CHcommDlg::OnComboStop(WPARAM wParam,LPARAM lParam)
{
	if (LOWORD(wParam) != IDC_COMBO_STOP)
		return false;

	CComboBox *pCB = (CComboBox *)GetDlgItem(IDC_COMBO_STOP);
	if (lParam == NULL)//快捷键选中
	{
		pCB->SetFocus();
	}
	else if (HIWORD(wParam) == CBN_SELCHANGE)
	{
		if (m_serialStop == pCB->GetCurSel())
			return false;
		m_serialStop = pCB->GetCurSel();
		if (m_watchPort)
		{
			m_Scomm.SetSettings(GetPortSetting());
		}
	}
	return true;
}

bool CHcommDlg::OnCheckWatch(WPARAM wParam,LPARAM lParam)
{
	if (LOWORD(wParam) != IDC_CHECK_WATCH)
		return false;

	CButton *pBtn = (CButton *)GetDlgItem(IDC_CHECK_WATCH);
	if (lParam == NULL)//快捷键选中
	{
		pBtn->SetCheck(!m_watchPort);
		pBtn->SetFocus();
	}
	if (m_watchPort == pBtn->GetCheck())
		return false;

	m_watchPort = pBtn->GetCheck();
	if (m_watchPort) //监测串口
	{//检测串口是否被占用
		HANDLE hComm = CreateFile(GetPortName(),GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL/*|FILE_FLAG_OVERLAPPED*/,NULL);   
		if (INVALID_HANDLE_VALUE == hComm)   
		{   
			CString msg = GetPortName();
			msg += _T("被占用或打开失败！");
			AfxMessageBox(msg);
			m_watchPort = !m_watchPort;
			pBtn->SetCheck(m_watchPort);
			return FALSE;   
		}
		CloseHandle(hComm);

		m_Scomm.SetCommPort(m_serialPort+1); //选择串口
		m_Scomm.SetSettings(GetPortSetting()); //波特率9600，无校验，8个数据位，1个停止位 
		m_Scomm.SetInputMode(1); //1：表示以二进制方式检取数据
		m_Scomm.SetNullDiscard(false);//false 发送0x00、NULL
		m_Scomm.SetRThreshold(1); //参数1表示每当串口接收缓冲区中有多于或等于1个字符时将引发一个接收数据的OnComm事件
		m_Scomm.SetInBufferSize(1024);
		m_Scomm.SetOutBufferSize(1024);
		m_Scomm.SetInBufferCount(0);
		m_Scomm.SetInputLen(0); //设置当前接收区数据长度为0
//		m_Scomm.GetInput();//先预读缓冲区以清除残留数据
		m_Scomm.SetPortOpen(TRUE);//打开串口

		CStatic *pStatic = (CStatic *)GetDlgItem(IDC_STATIC_LIGHT);
		pStatic->SetBitmap(m_hLightR);
	}
	else
	{
		if(m_Scomm.GetPortOpen())
			m_Scomm.SetPortOpen(FALSE);
		CStatic *pStatic = (CStatic *)GetDlgItem(IDC_STATIC_LIGHT);
		pStatic->SetBitmap(m_hLightW);
	}

	return true;
}

bool CHcommDlg::OnCheckHexshow(WPARAM wParam,LPARAM lParam)
{
	if (LOWORD(wParam) != IDC_CHECK_HEXSHOW)
		return false;

	CButton *pBtn = (CButton *)GetDlgItem(IDC_CHECK_HEXSHOW);
	if (lParam == NULL)//快捷键选中
	{
		pBtn->SetCheck(!m_hexShow);
		pBtn->SetFocus();
	}
	m_hexShow = pBtn->GetCheck();

	return true;
}

bool CHcommDlg::OnCheckHexsend(WPARAM wParam,LPARAM lParam)
{
	if (LOWORD(wParam) != IDC_CHECK_HEXSEND)
		return false;

	CButton *pBtn = (CButton *)GetDlgItem(IDC_CHECK_HEXSEND);
	if (lParam == NULL)//快捷键选中
	{
		pBtn->SetCheck(!m_hexSend);
		pBtn->SetFocus();
	}
	m_hexSend = pBtn->GetCheck();

	return true;
}

bool CHcommDlg::OnCheckAutosend(WPARAM wParam,LPARAM lParam)
{
	if (LOWORD(wParam) != IDC_CHECK_AUTOSEND)
		return false;

	CButton *pBtn  = (CButton *)GetDlgItem(IDC_CHECK_AUTOSEND);
	if (lParam == NULL)//快捷键选中
	{
		pBtn->SetCheck(!m_autoSend);
		pBtn->SetFocus();
	}
	if (m_autoSend == pBtn->GetCheck())
		return false;

	m_autoSend = pBtn->GetCheck();
	if (m_autoSend)//选中
	{
		if (!m_watchPort)
		{
			bool ok = OnCheckWatch((WPARAM)IDC_CHECK_WATCH, (LPARAM)NULL);
			pBtn->SetFocus();
			if (ok == false)
			{
				m_autoSend = !m_autoSend;
				pBtn->SetCheck(m_autoSend);
				return false;
			}
		}
		GetDlgItem(IDC_STATIC_PERIOD)->EnableWindow();
		GetDlgItem(IDC_EDIT_SECOND)->EnableWindow();
		GetDlgItem(IDC_STATIC_SECOND)->EnableWindow();
		EnableCtrlWhenAutoSend(false);

		SetTimer(IDEVENT_SEND, m_sendPeriod, NULL);
	}
	else
	{
		GetDlgItem(IDC_STATIC_PERIOD)->EnableWindow(0);
		GetDlgItem(IDC_EDIT_SECOND)->EnableWindow(0);
		GetDlgItem(IDC_STATIC_SECOND)->EnableWindow(0);
		EnableCtrlWhenAutoSend(true);
		
		KillTimer(IDEVENT_SEND);
	}
	return true;
}

bool CHcommDlg::OnEditMSecond(WPARAM wParam,LPARAM lParam)
{
	if (LOWORD(wParam) != IDC_EDIT_SECOND)
		return false;
	if (m_autoSend == 0)//非自动发送，周期编辑框不可选
		return false;

	char  strbuf[32];
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_EDIT_SECOND);
	if (lParam == NULL)//快捷键选中
	{
		pEdit->SetFocus();
	}
	else if (HIWORD(wParam) == EN_KILLFOCUS)
	{
		pEdit->GetWindowText(strbuf, 32);
		int ms = atol(strbuf);
		if (ms <= 0 || m_sendPeriod == ms)
			return false;
		
		m_sendPeriod = ms;
		KillTimer(IDEVENT_SEND);
		SetTimer(IDEVENT_SEND, (UINT)m_sendPeriod, NULL);
	}
	else if (HIWORD(wParam) == EN_UPDATE)
	{
		pEdit->GetWindowText(strbuf, 32);
		if (FilterDigit0_9(strbuf) == true)
			pEdit->SetWindowText(strbuf);
	}
	else
		return false;

	return true;
}

bool CHcommDlg::OnCheckAutoFrame(WPARAM wParam,LPARAM lParam)
{
	if (LOWORD(wParam) != IDC_CHECK_AUTOFRAME)
		return false;

	CButton *pBtn  = (CButton *)GetDlgItem(IDC_CHECK_AUTOFRAME);
	if (lParam == NULL)
	{
		pBtn->SetCheck(!m_autoFrame);
		pBtn->SetFocus();
	}
	if (m_autoFrame == pBtn->GetCheck())
		return false;

	m_autoFrame = pBtn->GetCheck();
	GetDlgItem(IDC_BUTTON_MSET)->EnableWindow(m_autoFrame);
	GetDlgItem(IDC_STATIC_RULE)->EnableWindow(m_autoFrame);
	GetDlgItem(IDC_COMBO_RULE)->EnableWindow(m_autoFrame);
	GetDlgItem(IDC_STATIC_ADDR)->EnableWindow(m_autoFrame && pCurMrule->EnableAddressWnd(m_DltRuleCtrlCode));
	GetDlgItem(IDC_EDIT_ADDR)->EnableWindow(m_autoFrame && pCurMrule->EnableAddressWnd(m_DltRuleCtrlCode));
	GetDlgItem(IDC_STATIC_DATAID)->EnableWindow(m_autoFrame && pCurMrule->EnableDataIDWnd(m_DltRuleCtrlCode));
	GetDlgItem(IDC_COMBO_DATAID)->EnableWindow(m_autoFrame && pCurMrule->EnableDataIDWnd(m_DltRuleCtrlCode));
	if (m_autoFrame)//选中
		MakeFrameAndShow();//组帧

	return true;
}

bool CHcommDlg::OnButtonMSet(WPARAM wParam,LPARAM lParam)
{
	if (LOWORD(wParam) != IDC_BUTTON_MSET)
		return false;

	CButton *pBtn  = (CButton *)GetDlgItem(IDC_BUTTON_MSET);
	if (lParam == NULL)
		pBtn->SetFocus();

	if (m_meterRule == MRULE_LGR)
	{
		CMSetDlg dlg(this);
		dlg.DoModal();
	}
	else
	{
		CMDLTSetDlg dlg(this);
		if (dlg.DoModal() == IDOK)
		{
			GetDlgItem(IDC_STATIC_ADDR)->EnableWindow(m_autoFrame && pCurMrule->EnableAddressWnd(m_DltRuleCtrlCode));
			GetDlgItem(IDC_EDIT_ADDR)->EnableWindow(m_autoFrame && pCurMrule->EnableAddressWnd(m_DltRuleCtrlCode));
			GetDlgItem(IDC_STATIC_DATAID)->EnableWindow(m_autoFrame && pCurMrule->EnableDataIDWnd(m_DltRuleCtrlCode));
			GetDlgItem(IDC_COMBO_DATAID)->EnableWindow(m_autoFrame && pCurMrule->EnableDataIDWnd(m_DltRuleCtrlCode));
			MakeFrameAndShow();
		}
	}
	return true;
}

bool CHcommDlg::OnComboRule(WPARAM wParam,LPARAM lParam)
{
	if (LOWORD(wParam) != IDC_COMBO_RULE)
		return false;
	if (m_autoFrame == 0)//非自动组帧，电表规约不可选
		return false;

	CComboBox *pCB  = (CComboBox *)GetDlgItem(IDC_COMBO_RULE);
	if (lParam == NULL)//快捷键选中
	{
		pCB->SetFocus();
	}
	else if (HIWORD(wParam) == CBN_SELCHANGE)
	{
		if (m_meterRule == pCB->GetCurSel())
			return false;

		m_meterRule = pCB->GetCurSel();
		if (m_meterRule == MRULE_645_1997)
			pCurMrule = &MRule97;
		else if (m_meterRule == MRULE_645_2007)
			pCurMrule = &MRule07;
		else if (m_meterRule == MRULE_LGR)
			pCurMrule = &MRuleLgr;

		m_DltRuleCtrlCode = 0;
		m_meterReg  = 0;

		//初始化电表数据ID
		const char *pstring;
		CComboBox *pCombo = (CComboBox *)GetDlgItem(IDC_COMBO_DATAID);
		pCombo->ResetContent();
		for (int i = 0; i < pCurMrule->GetMRuleDataIDCount(); i++)
		{
			pstring = pCurMrule->GetMRuleDataID(i);
			pCombo->AddString(pstring);
		}
		pCombo->SetCurSel(m_meterReg);
		//显示电表数据ID名称
		pstring = pCurMrule->GetMRuleRegName(m_meterReg);
		GetDlgItem(IDC_STATIC_REGNAME)->SetWindowText(pstring);
		//地址和数据项是否灰
		GetDlgItem(IDC_STATIC_ADDR)->EnableWindow(m_autoFrame && pCurMrule->EnableAddressWnd(m_DltRuleCtrlCode));
		GetDlgItem(IDC_EDIT_ADDR)->EnableWindow(m_autoFrame && pCurMrule->EnableAddressWnd(m_DltRuleCtrlCode));
		GetDlgItem(IDC_STATIC_DATAID)->EnableWindow(m_autoFrame && pCurMrule->EnableDataIDWnd(m_DltRuleCtrlCode));
		GetDlgItem(IDC_COMBO_DATAID)->EnableWindow(m_autoFrame && pCurMrule->EnableDataIDWnd(m_DltRuleCtrlCode));
		//atuo frame
		MakeFrameAndShow();
	}
	return true;
}

bool CHcommDlg::OnEditAddr(WPARAM wParam,LPARAM lParam)
{
	if (LOWORD(wParam) != IDC_EDIT_ADDR)
		return false;
	if (m_autoFrame == 0)//非自动组帧，电表地址不可选
		return false;

	char  strbuf[32];
	CEdit *pEdit  = (CEdit *)GetDlgItem(IDC_EDIT_ADDR);
	if (lParam == NULL)//快捷键选中
	{
		pEdit->SetFocus();
	}
	else if (HIWORD(wParam) == EN_KILLFOCUS)
	{
		pEdit->GetWindowText(strbuf, 32);
		if (m_meterAddr.compare(strbuf))
		{
			m_meterAddr = strbuf;
			//auto frame
			MakeFrameAndShow();
		}
	}
	else if (HIWORD(wParam) == EN_UPDATE)
	{
		pEdit->GetWindowText(strbuf, 32);
		if (FilterDigit0_F(strbuf) == true)
			pEdit->SetWindowText(strbuf);
	}
	else
		return false;

	return true;
}

bool CHcommDlg::OnComboDataID(WPARAM wParam,LPARAM lParam)
{
	if (LOWORD(wParam) != IDC_COMBO_DATAID)
		return false;
	if (m_autoFrame == 0)//非自动组帧，电表规约不可选
		return false;

	CComboBox *pCB  = (CComboBox *)GetDlgItem(IDC_COMBO_DATAID);
	if (lParam == NULL)//快捷键选中
	{
		pCB->SetFocus();
	}
	else if (HIWORD(wParam) == CBN_SELCHANGE)
	{
		if (m_meterReg == pCB->GetCurSel())
			return false;

		m_meterReg = pCB->GetCurSel();
		GetDlgItem(IDC_STATIC_REGNAME)->SetWindowText(pCurMrule->GetMRuleRegName(m_meterReg));
		//atuo frame
		MakeFrameAndShow();
	}
	return true;
}

bool CHcommDlg::OnEditReceive(WPARAM wParam,LPARAM lParam)
{
	if (LOWORD(wParam) != IDC_EDIT_RECEIVE)
		return false;

	CEdit *pEdit  = (CEdit *)GetDlgItem(IDC_EDIT_RECEIVE);
	if (lParam == NULL)//快捷键选中
	{
		pEdit->SetFocus();
	}
	return true;
}

bool CHcommDlg::OnEditSend(WPARAM wParam,LPARAM lParam)
{
	if (LOWORD(wParam) != IDC_EDIT_SEND)
		return false;

	CEdit *pEdit  = (CEdit *)GetDlgItem(IDC_EDIT_SEND);
	if (lParam == NULL)//快捷键选中
	{
		pEdit->SetFocus();
	}
	else if (HIWORD(wParam) == EN_KILLFOCUS)
	{
		UpdateData(true);
	}
	return true;
}

bool CHcommDlg::OnButtonClrReceive(WPARAM wParam,LPARAM lParam)
{
	if (LOWORD(wParam) != IDC_BUTTON_CLRRECEIVE)
		return false;

	if (lParam == NULL)
		GetDlgItem(IDC_BUTTON_CLRRECEIVE)->SetFocus();
	m_StrReceive = _T("");
	UpdateData(false);

	return true;
}

bool CHcommDlg::OnButtonClrSend(WPARAM wParam,LPARAM lParam)
{
	if (LOWORD(wParam) != IDC_BUTTON_CLRSEND)
		return false;

	if (lParam == NULL)
		GetDlgItem(IDC_BUTTON_CLRSEND)->SetFocus();
	m_StrSend = _T("");
	UpdateData(false);

	return true;
}

bool CHcommDlg::OnButtonManualSend(WPARAM wParam,LPARAM lParam)
{
	if (LOWORD(wParam) != IDC_BUTTON_MANUALSEND)
		return false;

	if (lParam == NULL)
		GetDlgItem(IDC_BUTTON_MANUALSEND)->SetFocus();

	//check port if open
	if (!m_watchPort)
	{
		bool ok = OnCheckWatch((WPARAM)IDC_CHECK_WATCH,(LPARAM)NULL);
		GetDlgItem(IDC_BUTTON_MANUALSEND)->SetFocus();
		if (ok == false)
			return false;
	}
	//send frame
	bool ok = ScommSendData();
	if (m_meterRule==MRULE_LGR && m_meterReg == 0)
		m_LGRlogin = true;

	return ok;
}

bool CHcommDlg::ScommSendData(void)
{
	UpdateData(TRUE);
	if (m_hexSend)
	{
		BYTE H, L;
		CByteArray ByteArray;
		for (int i = 0; i < m_StrSend.GetLength(); i++)
		{
			if (m_StrSend[i] == ' ')
				continue;
			H = (BYTE)C_MRULE::Ascii2Digit(m_StrSend[i]);
			if ((i+1 < m_StrSend.GetLength()) && m_StrSend[i+1] != ' ')
			{
				L = (BYTE)C_MRULE::Ascii2Digit(m_StrSend[++i]);
				H = (H<<4)|L;
			}
			ByteArray.Add(H);
		}
		m_Scomm.SetOutput(COleVariant(ByteArray));
	}
	else
	{
		m_Scomm.SetOutput(COleVariant(m_StrSend));
	}
	UpdateData(FALSE);
	return true;
}

bool CHcommDlg::OnButtonSendFile(WPARAM wParam,LPARAM lParam)
{
	if (LOWORD(wParam) != IDC_BUTTON_SENDFILE)
		return false;

	CButton *pbtn = (CButton *)GetDlgItem(IDC_BUTTON_SENDFILE);
	if (pbtn->IsWindowEnabled() == false)
		return false;

	if (lParam == NULL)
		pbtn->SetFocus();

	CProgressCtrl *pProgress = (CProgressCtrl *)GetDlgItem(IDC_PROGRESS_SEND);
	if (m_FileSending == false)
	{
		if (!m_watchPort)
		{
			bool ok = OnCheckWatch((WPARAM)IDC_CHECK_WATCH,(LPARAM)NULL);
			GetDlgItem(IDC_BUTTON_SENDFILE)->SetFocus();
			if (ok == false)
				return false;
		}
		CString FileName;
		GetDlgItem(IDC_EDIT_PATH)->GetWindowText(FileName);
		SFile.Open(FileName, CFile::modeRead);
		if (SFile.m_hFile == CFile::hFileNull)
		{
			CString msg(FileName);
			msg += _T("打开失败！");
			AfxMessageBox(msg);
			return false;
		}
		unsigned int FileLen = (unsigned int)SFile.GetLength();
		if (FileLen == 0)
		{
			SFile.Close();
			CString msg(FileName);
			msg += _T("是空文件！");
			AfxMessageBox(msg);
			return false;
		}

		BYTE SendBuf[516];
		memset(SendBuf, 0x00, sizeof(SendBuf));
		if (SFile.Read(SendBuf, SCOMM_SENDBUF_SIZE) == 0)
		{
			SFile.Close();
			return false;
		}
		CString StrBuf(SendBuf);

		EnableCtrlWhenFileSend(false);
		UpdateWindow();
		
		pProgress->ShowWindow(SW_SHOW);
		int block = FileLen/SCOMM_SENDBUF_SIZE;
		if (block == 0) block = 1;
		pProgress->SetRange(0, block+1);
		pProgress->SetPos(0);
		pProgress->OffsetPos(1);
		pbtn->SetWindowText(_T("取消发送(&J)"));
		if (m_autoSend)
			KillTimer(IDEVENT_SEND);
		m_Scomm.SetSThreshold(1);
		m_Scomm.SetOutput(COleVariant(StrBuf));

		m_FileSending = true;
	}
	else
	{
		m_Scomm.SetSThreshold(0);
		SFile.Close();
		EnableCtrlWhenFileSend(true);
		pProgress->ShowWindow(SW_HIDE);
		pbtn->SetWindowText(_T("发送文件(&J)"));
		if (m_autoSend)
			SetTimer(IDEVENT_SEND, m_sendPeriod, NULL);
		m_FileSending = false;
	}
	return true;
}

bool CHcommDlg::OnButtonAbort(WPARAM wParam,LPARAM lParam)
{
	if (LOWORD(wParam) != IDC_BUTTON_ABOUT)
		return false;

	if (lParam == NULL)
		GetDlgItem(IDC_BUTTON_ABOUT)->SetFocus();
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();

	return true;
}

bool CHcommDlg::OnButtonOpen(WPARAM wParam,LPARAM lParam)
{
	if (LOWORD(wParam) != IDC_BUTTON_SELECTFILE)
		return false;

	if (lParam == NULL)
		GetDlgItem(IDC_BUTTON_SELECTFILE)->SetFocus();

	CFileDialog dlg(TRUE,_T("txt"),NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	_T("文本文件 (*.txt)|*.txt|数据文件 (*.bin)|*.bin|All Files (*.*)|*.*||"),this);

	if (dlg.DoModal()==IDOK)
	{
		CString strFile = dlg.GetPathName();
		GetDlgItem(IDC_EDIT_PATH)->SetWindowText(strFile);
		GetDlgItem(IDC_BUTTON_SENDFILE)->EnableWindow();
	}

	return true;
}

bool CHcommDlg::OnEditPath(WPARAM wParam,LPARAM lParam)
{
	if (LOWORD(wParam) != IDC_EDIT_PATH)
		return false;

	CWnd *pwnd = GetDlgItem(IDC_EDIT_PATH);
	if (lParam == NULL)
		pwnd->SetFocus();
	else if (HIWORD(wParam) == EN_CHANGE)
	{
		if (pwnd->GetWindowTextLength())
            GetDlgItem(IDC_BUTTON_SENDFILE)->EnableWindow();
		else
			GetDlgItem(IDC_BUTTON_SENDFILE)->EnableWindow(0);
	}
	return true;
}

bool CHcommDlg::OnButtonLgrAck(WPARAM wParam,LPARAM lParam)
{
	if (LOWORD(wParam) != IDC_BUTTON_LGRACK)
		return false;

	BYTE baud = (BYTE)lParam;
	if (m_LgrAck != LGR_ACK_AUTO)//不是自动跳波特率
		baud = (m_LgrAck-1)+'0';

	CString ack;
	ack.AppendChar(0x06);
	ack.AppendChar('0');
	ack.AppendChar(baud);
	ack.AppendChar('0');
	ack.AppendChar(0x0D);
	ack.AppendChar(0x0A);

	m_Scomm.SetOutput(COleVariant(ack));

	CComboBox *pcb = (CComboBox *)GetDlgItem(IDC_COMBO_BAUD);
	pcb->SetCurSel((int)C_MRULE::Ascii2Digit(baud));

	m_Scomm.SetSettings(GetPortSetting());

	return true;
}

void CHcommDlg::OnTimer(UINT nIDEvent)
{
	ScommSendData();
}

void CHcommDlg::MakeFrameAndShow(void)
{
	unsigned char code = 0;
	string frame;
	S_MRULEINFO MRuleInfo;
	MRuleInfo.m_MeterAddr = m_meterAddr;
	MRuleInfo.m_DataID    = pCurMrule->GetMRuleDataID(m_meterReg);
	MRuleInfo.m_OpCode    = m_OpCode;
	MRuleInfo.m_Password  = m_Password;
	MRuleInfo.m_data      = m_WriteData;
	MRuleInfo.m_FreezeTime= m_FreezeTime;

	const S_DLTCTRLCODE *pCtrlCode = pCurMrule->GetCtrlCodeGroup();
	if (pCtrlCode != NULL)
		code = pCtrlCode[m_DltRuleCtrlCode].m_CtrlCode;
	int cnt = pCurMrule->MakeMeterRuleFrame(frame, code, MRuleInfo);

	m_StrSend.Empty();
	Hex2String(m_StrSend, frame.c_str(), (int)frame.size());
	UpdateData(false);
}

bool CHcommDlg::FilterDigit0_9(char *strbuf)
{
	bool flag = false;
	char *psrc = strbuf, *pdst = strbuf;

	while (*psrc)
	{
		if (*psrc&0x80)
		{
			flag = true;
			psrc += 2;
		}
		else
		{
			if (*psrc >= '0' && *psrc <= '9')
                *pdst++ = *psrc++;
			else
				psrc++,flag = true;
		}
	}
	*pdst = '\0';

	return flag;
}

bool CHcommDlg::FilterDigit0_F(char *strbuf)
{
	bool flag = false;
	char *psrc = strbuf, *pdst = strbuf;

	while (*psrc)
	{
		if (*psrc&0x80)
		{
			flag = true;
			psrc += 2;
		}
		else
		{
			if ((*psrc >= '0' && *psrc <= '9')
				|| (*psrc >= 'A' && *psrc <= 'F')
				|| (*psrc >= 'a' && *psrc <= 'f'))
                *pdst++ = *psrc++;
			else
				psrc++,flag = true;
		}
	}
	*pdst = '\0';
	if (strlen(strbuf) > 12)
	{
		strbuf[12] = '\0';
		flag = true;
	}
	return flag;
}

int CHcommDlg::Hex2String(CString &str, const char *pHex, int cnt)
{
	for (int i = 0; i < cnt; i++)
	{
		str += C_MRULE::Digit2Ascii((pHex[i]>>4)&0xF);
		str += C_MRULE::Digit2Ascii(pHex[i]&0xF);
		str += ' ';
	}

	return str.GetLength();
}

TCHAR *CHcommDlg::GetPortName(void)
{
	static TCHAR PortName[6] = "COM1";

	if (m_serialPort < 9)
	{
		PortName[3] = m_serialPort+1+'0';
		PortName[4] = '\0';
	}
	else
	{
		PortName[3] = '1';
		PortName[4] = ((m_serialPort+1)-10) + '0';
		PortName[5] = '\0';
	}

	return PortName;
}

TCHAR *CHcommDlg::GetPortSetting(void)
{
	static TCHAR PortSet[32];
	TCHAR parity[]  = {'n','o','e'};
	TCHAR DataNum[] = {'8','7','6'};
	TCHAR StopBit[] = {'1','2'};

	CComboBox *pCB = (CComboBox *)GetDlgItem(IDC_COMBO_BAUD);
	pCB->GetLBText(m_serialBaud, PortSet);
	int len = lstrlen(PortSet);
	PortSet[len+0] = ',';
	PortSet[len+1] = parity[m_serialCheck];
	PortSet[len+2] = ',';
	PortSet[len+3] = DataNum[m_serialNum];
	PortSet[len+4] = ',';
	PortSet[len+5] = StopBit[m_serialStop];
	PortSet[len+6] = '\0';

	return PortSet;
}

static int EnableCtrl4FileSend[] =
{
	IDC_COMBO_PORT,
	IDC_COMBO_BAUD,
	IDC_COMBO_CHECK,
	IDC_COMBO_NUM,
	IDC_COMBO_STOP,
	IDC_CHECK_WATCH,
	IDC_CHECK_HEXSHOW,
	IDC_CHECK_HEXSEND,
	IDC_CHECK_AUTOSEND,
	IDC_BUTTON_MANUALSEND
};

void CHcommDlg::EnableControl(int *pCtrlID, int size, bool bEnable)
{
	for (int i = 0; i < size; i++)
	{
		CWnd *pWnd = GetDlgItem(pCtrlID[i]);
		pWnd->EnableWindow(bEnable);
	}
}

void CHcommDlg::EnableCtrlWhenFileSend(bool bEnable)
{
	EnableControl(EnableCtrl4FileSend, (int)(sizeof(EnableCtrl4FileSend)/sizeof(EnableCtrl4FileSend[0])), bEnable);
	if (m_autoSend)
	{
		CWnd *pWnd = GetDlgItem(IDC_EDIT_SECOND);
		pWnd->EnableWindow(bEnable);
	}
}

static int EnableCtrl4AutoSend[] =
{
	IDC_COMBO_PORT,
	IDC_COMBO_BAUD,
	IDC_COMBO_CHECK,
	IDC_COMBO_NUM,
	IDC_COMBO_STOP,
	IDC_CHECK_WATCH,
	IDC_BUTTON_MANUALSEND,
	IDC_EDIT_PATH,
	IDC_BUTTON_SELECTFILE
};

void CHcommDlg::EnableCtrlWhenAutoSend(bool bEnable)
{
	EnableControl(EnableCtrl4AutoSend, (int)(sizeof(EnableCtrl4AutoSend)/sizeof(EnableCtrl4AutoSend[0])), bEnable);
	if (bEnable == false)
	{
		CWnd *pWnd = GetDlgItem(IDC_BUTTON_SENDFILE);
		pWnd->EnableWindow(bEnable);
	}
	else
	{
		CWnd *pWnd = GetDlgItem(IDC_EDIT_PATH);
		if (pWnd->GetWindowTextLength() != 0)
		{
			pWnd = GetDlgItem(IDC_BUTTON_SENDFILE);
			pWnd->EnableWindow(bEnable);
		}
	}
}
