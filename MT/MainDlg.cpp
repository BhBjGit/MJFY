// MainDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MT.h"
#include "MainDlg.h"
#include "afxdialogex.h"


// CMainDlg 对话框

IMPLEMENT_DYNAMIC(CMainDlg, CDialogEx)

CMainDlg::CMainDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMainDlg::IDD, pParent)
{

}

CMainDlg::~CMainDlg()
{
}

void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMainDlg, CDialogEx)
	ON_CBN_SELCHANGE(IDC_COMBO_DES, &CMainDlg::OnCbnSelchangeComboDes)
END_MESSAGE_MAP()


// CMainDlg 消息处理程序


BOOL CMainDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	this->Init();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


BOOL	CMainDlg::Init()
{
	CRect rect;

	CF_SetTitleWithVersion(this->m_hWnd);


	m_pStaticCtrl=(CStatic*)GetDlgItem(IDC_STATIC_PARENT);
	if( m_pStaticCtrl)
	{
		m_pStaticCtrl->GetClientRect(&rect);

		m_OneDlg.Create(IDD_MT_DIALOG, m_pStaticCtrl);
		m_SecondDlg.Create(IDD_MT2_DIALOG, m_pStaticCtrl);
		m_thirdDlg.Create(IDD_MT3_DIALOG, m_pStaticCtrl);
		m_fourDlg.Create(IDD_MT4_DIALOG, m_pStaticCtrl);


		m_OneDlg.SetParent(this);
		m_SecondDlg.SetParent(this);
		m_thirdDlg.SetParent(this);
		m_fourDlg.SetParent(this);

		m_OneDlg.ShowWindow(SW_SHOW);
	}

	m_pCombox=(CComboBox*)GetDlgItem(IDC_COMBO_DES);
	m_pCombox->AddString(L"第一代空气果检测程式");
	m_pCombox->AddString(L"第二代空气果检测程式");
	m_pCombox->AddString(L"空气果(SPORT)");
	m_pCombox->AddString(L"空气果(FUN)");

	m_pCombox->SetCurSel(0);

	return TRUE;
}

void CMainDlg::OnCbnSelchangeComboDes()
{
	WCHAR		wsBuf[MAX_PATH]={0};
	INT			nIndex=m_pCombox->GetCurSel();

	m_pCombox->GetLBText(nIndex, wsBuf);	
 	//OutputDebugStringW(wsBuf);

	if( wcsstr(wsBuf, L"第一代"))
	{
		m_SecondDlg.ShowWindow(SW_HIDE);
		m_OneDlg.ShowWindow(SW_SHOW);	
		m_thirdDlg.ShowWindow(SW_HIDE);
		m_fourDlg.ShowWindow(SW_HIDE);
	}
	else if( wcsstr(wsBuf, L"第二代"))
	{
		m_OneDlg.ShowWindow(SW_HIDE);	
		m_SecondDlg.ShowWindow(SW_SHOW);
		m_thirdDlg.ShowWindow(SW_HIDE);
		m_fourDlg.ShowWindow(SW_HIDE);
	}
	else if( wcsstr(wsBuf, L"(SPORT)"))
	{
		m_OneDlg.ShowWindow(SW_HIDE);	
		m_SecondDlg.ShowWindow(SW_HIDE);
		m_thirdDlg.ShowWindow(SW_SHOW);
		m_fourDlg.ShowWindow(SW_HIDE);
	}
	else if( wcsstr(wsBuf, L"(FUN)"))
	{
		m_OneDlg.ShowWindow(SW_HIDE);	
		m_SecondDlg.ShowWindow(SW_HIDE);
		m_thirdDlg.ShowWindow(SW_HIDE);
		m_fourDlg.ShowWindow(SW_SHOW);
	}
}


BOOL CMainDlg::PreTranslateMessage(MSG* pMsg)
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
