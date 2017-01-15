// MT2Dlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MT.h"
#include "MT2Dlg.h"
#include "afxdialogex.h"

#include "Led2TestDlg.h"
#include "Standard2Dlg.h"

#include "MainDlg.h"

// CMT2Dlg 对话框

IMPLEMENT_DYNAMIC(CMT2Dlg, CDialogEx)

CMT2Dlg::CMT2Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMT2Dlg::IDD, pParent)
{

}

CMT2Dlg::~CMT2Dlg()
{
}

void CMT2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMT2Dlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_SD, &CMT2Dlg::OnBnClickedBtnSd)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BTN_WIFI, &CMT2Dlg::OnBnClickedBtnWifi)
	ON_BN_CLICKED(IDC_BTN_MainBoard, &CMT2Dlg::OnBnClickedBtnMainboard)
	ON_BN_CLICKED(IDC_BTN_LED, &CMT2Dlg::OnBnClickedBtnLed)
	ON_BN_CLICKED(IDC_BTN_LEDA, &CMT2Dlg::OnBnClickedBtnLeda)
END_MESSAGE_MAP()


// CMT2Dlg 消息处理程序





HBRUSH CMT2Dlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	if( pWnd->GetDlgCtrlID()==IDC_STATIC_HINT )
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(21,163,21));
	}

	return hbr;
}


BOOL CMT2Dlg::PreTranslateMessage(MSG* pMsg)
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


void CMT2Dlg::OnBnClickedBtnWifi()
{
	CLed2TestDlg	ld(this, TP_WIFI);
	this->ShowParentWindow(SW_HIDE);

	//if( IDCANCEL==ld.DoModal() )
	ld.DoModal();
	{
		this->ShowParentWindow(SW_SHOW);
	}
}


void CMT2Dlg::OnBnClickedBtnMainboard()
{
	CLed2TestDlg	ld(this, TP_MB);
	this->ShowParentWindow(SW_HIDE);

	//if( IDCANCEL==ld.DoModal() )
	ld.DoModal();
	{
		this->ShowParentWindow(SW_SHOW);
	}
}

//节点灯板检测
void CMT2Dlg::OnBnClickedBtnLed()
{
	CLed2TestDlg	ld(this, TP_LED, 2);
	this->ShowParentWindow(SW_HIDE);

	//if( IDCANCEL==ld.DoModal() )
	ld.DoModal();
	{
		this->ShowParentWindow(SW_SHOW);
	}
}

//主机灯板检测
void CMT2Dlg::OnBnClickedBtnLeda()
{
	CLed2TestDlg	ld(this, TP_LED, 1);
	this->ShowParentWindow(SW_HIDE);

	//if( IDCANCEL==ld.DoModal() )
	ld.DoModal();
	{
		this->ShowParentWindow(SW_SHOW);
	}
}


void CMT2Dlg::OnBnClickedBtnSd()
{
	CStandard2Dlg	sd(this);
	this->ShowParentWindow(SW_HIDE);

	//if( IDCANCEL==sd.DoModal() )
	sd.DoModal();
	{
		this->ShowParentWindow(SW_SHOW);
	}
}


void	CMT2Dlg::SetParent(LPVOID p)
{
	m_pParent=p;
}


void	CMT2Dlg::ShowParentWindow(BOOL bShow)
{
	CMainDlg* pThis=(CMainDlg*)m_pParent;

	pThis->ShowWindow(bShow);
}


