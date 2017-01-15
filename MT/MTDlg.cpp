
// MTDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MT.h"
#include "MTDlg.h"
#include "afxdialogex.h"
#include <strsafe.h>

#include "LedTestDlg.h"
#include "MainBoardTestDlg.h"
#include "StandardDlg.h"
#include "WifiTestDlg.h"

#include "MainDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
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

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMTDlg 对话框




CMTDlg::CMTDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMTDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMTDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMTDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//ON_BN_CLICKED(IDC_BTN_CONNECT, &CMTDlg::OnBnClickedBtnConnect)
	ON_BN_CLICKED(IDC_BTN_MainBoard, &CMTDlg::OnBnClickedBtnMainboard)
	ON_BN_CLICKED(IDC_BTN_WIFI, &CMTDlg::OnBnClickedBtnWifi)
	ON_BN_CLICKED(IDC_BTN_LED, &CMTDlg::OnBnClickedBtnLed)
	ON_BN_CLICKED(IDC_BTN_SD, &CMTDlg::OnBnClickedBtnSd)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CMTDlg 消息处理程序

BOOL CMTDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
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
	CF_SetTitleWithVersion(this->m_hWnd);
	//this->OperateServiceConfig(FALSE);


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMTDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMTDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
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
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMTDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



// void CMTDlg::OnBnClickedBtnConnect()
// {
// 	//this->OperateServiceConfig(TRUE);
// }


void CMTDlg::OnBnClickedBtnMainboard()
{
// 	CMainBoardTestDlg	mbd(this);
// 	mbd.DoModal();
	CLedTestDlg	ld(this, TP_MB);
	this->ShowParentWindow(SW_HIDE);

	//if( IDCANCEL==ld.DoModal() )
	ld.DoModal();
	{
		this->ShowParentWindow(SW_SHOW);
	}
}


void CMTDlg::OnBnClickedBtnWifi()
{
// 	CWifiTestDlg	wfd(this);
// 	wfd.DoModal();
	CLedTestDlg	ld(this, TP_WIFI);
	this->ShowParentWindow(SW_HIDE);

	//if( IDCANCEL==ld.DoModal() )
	ld.DoModal();
	{
		this->ShowParentWindow(SW_SHOW);
	}
}


void CMTDlg::OnBnClickedBtnLed()
{
	CLedTestDlg	ld(this, TP_LED);
	this->ShowParentWindow(SW_HIDE);

	//if( IDCANCEL==ld.DoModal() )
	ld.DoModal();
	{
		this->ShowParentWindow(SW_SHOW);
	}
}


void CMTDlg::OnBnClickedBtnSd()
{
	CStandardDlg	sd(this);
	this->ShowParentWindow(SW_HIDE);

	//if( IDCANCEL==sd.DoModal() )
	sd.DoModal();
	{
		this->ShowParentWindow(SW_SHOW);
	}
}


HBRUSH CMTDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	if( pWnd->GetDlgCtrlID()==IDC_STATIC_HINT )
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(21,163,21));
	}


	return hbr;
}

/*
VOID	CMTDlg::OperateServiceConfig(BOOL	bSet)
{
	CHAR			csDir[MAX_PATH]={0};
	CHAR			csUsername[MAX_PATH]={0};
	CHAR			csPsw[MAX_PATH]={0};
	CHAR			csIP[MAX_PATH]={0};
	DWORD			dwIP=0;
	CIPAddressCtrl*	pIpAddress=(CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS);

	CF_GetModuleDir(csDir);
	StringCchCatA(csDir, MAX_PATH, "config.ini");

	if( bSet )
	{
		GetDlgItemTextA(this->m_hWnd, IDC_EDIT_USERNAME, csUsername, MAX_PATH);
		GetDlgItemTextA(this->m_hWnd, IDC_EDIT_PSW, csPsw, MAX_PATH);
		pIpAddress->GetAddress(dwIP);
		itoa(dwIP, csIP, 10);

		WritePrivateProfileStringA("SET", "username", csUsername, csDir);
		WritePrivateProfileStringA("SET", "psw", csPsw, csDir);
		WritePrivateProfileStringA("SET", "ip", csIP, csDir);
	}
	else
	{
		GetPrivateProfileStringA("SET", "username", " ", csUsername, MAX_PATH, csDir);
		GetPrivateProfileStringA("SET", "psw", " ", csPsw, MAX_PATH, csDir);
		GetPrivateProfileStringA("SET", "ip", " ", csIP, MAX_PATH, csDir);
		dwIP=atoi(csIP);

		SetDlgItemTextA(this->m_hWnd, IDC_EDIT_USERNAME, csUsername);
		SetDlgItemTextA(this->m_hWnd, IDC_EDIT_PSW, csPsw);
		pIpAddress->SetAddress(dwIP);
	}
}
*/

	BOOL CMTDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if( pMsg->message==WM_KEYDOWN)
	{
		if( pMsg->wParam==VK_ESCAPE ||
			pMsg->wParam==VK_RETURN )
		{
			return TRUE;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


void	CMTDlg::SetParent(LPVOID p)
{
	m_pParent=p;
}


void	CMTDlg::ShowParentWindow(BOOL bShow)
{
	CMainDlg* pThis=(CMainDlg*)m_pParent;

	pThis->ShowWindow(bShow);
}