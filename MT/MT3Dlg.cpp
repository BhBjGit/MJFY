// MT3Dlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MT.h"
#include "MT3Dlg.h"
#include "afxdialogex.h"
#include "MainDlg.h"
#include "Led3TestDlg.h"

// CMT3Dlg 对话框

IMPLEMENT_DYNAMIC(CMT3Dlg, CDialogEx)

CMT3Dlg::CMT3Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMT3Dlg::IDD, pParent)
{

}

CMT3Dlg::~CMT3Dlg()
{
}

void CMT3Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMT3Dlg, CDialogEx)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BTN_MainBoard, &CMT3Dlg::OnBnClickedBtnMainboard)
END_MESSAGE_MAP()


// CMT3Dlg 消息处理程序


void	CMT3Dlg::SetParent(LPVOID p)
{
	m_pParent=p;
}


void	CMT3Dlg::ShowParentWindow(BOOL bShow)
{
	CMainDlg* pThis=(CMainDlg*)m_pParent;

	pThis->ShowWindow(bShow);
}

BOOL CMT3Dlg::PreTranslateMessage(MSG* pMsg)
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


HBRUSH CMT3Dlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	if( pWnd->GetDlgCtrlID()==IDC_STATIC_HINT )
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(21,163,21));
	}

	return hbr;
}


void CMT3Dlg::OnBnClickedBtnMainboard()
{
	CLed3TestDlg	ld(this, TP_SPORT_MB);
	this->ShowParentWindow(SW_HIDE);

	//if( IDCANCEL==ld.DoModal() )
	ld.DoModal();
	{
		this->ShowParentWindow(SW_SHOW);
	}
}
