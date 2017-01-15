
// MTDlg.cpp : ʵ���ļ�
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


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
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

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMTDlg �Ի���




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


// CMTDlg ��Ϣ�������

BOOL CMTDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	CF_SetTitleWithVersion(this->m_hWnd);
	//this->OperateServiceConfig(FALSE);


	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CMTDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
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
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
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
	// TODO: �ڴ����ר�ô����/����û���
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