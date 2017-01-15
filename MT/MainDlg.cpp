// MainDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MT.h"
#include "MainDlg.h"
#include "afxdialogex.h"


// CMainDlg �Ի���

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


// CMainDlg ��Ϣ�������


BOOL CMainDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	this->Init();

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
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
	m_pCombox->AddString(L"��һ������������ʽ");
	m_pCombox->AddString(L"�ڶ�������������ʽ");
	m_pCombox->AddString(L"������(SPORT)");
	m_pCombox->AddString(L"������(FUN)");

	m_pCombox->SetCurSel(0);

	return TRUE;
}

void CMainDlg::OnCbnSelchangeComboDes()
{
	WCHAR		wsBuf[MAX_PATH]={0};
	INT			nIndex=m_pCombox->GetCurSel();

	m_pCombox->GetLBText(nIndex, wsBuf);	
 	//OutputDebugStringW(wsBuf);

	if( wcsstr(wsBuf, L"��һ��"))
	{
		m_SecondDlg.ShowWindow(SW_HIDE);
		m_OneDlg.ShowWindow(SW_SHOW);	
		m_thirdDlg.ShowWindow(SW_HIDE);
		m_fourDlg.ShowWindow(SW_HIDE);
	}
	else if( wcsstr(wsBuf, L"�ڶ���"))
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
