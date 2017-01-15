// MT4Dlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MT.h"
#include "MT4Dlg.h"
#include "afxdialogex.h"
#include "MainDlg.h"
#include "Led4TestDlg.h"
// CMT4Dlg �Ի���

IMPLEMENT_DYNAMIC(CMT4Dlg, CDialogEx)

CMT4Dlg::CMT4Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMT4Dlg::IDD, pParent)
{

}

CMT4Dlg::~CMT4Dlg()
{
}

void CMT4Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMT4Dlg, CDialogEx)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BTN_MainBoard, &CMT4Dlg::OnBnClickedBtnMainboard)
END_MESSAGE_MAP()


// CMT4Dlg ��Ϣ�������

void	CMT4Dlg::SetParent(LPVOID p)
{
	m_pParent=p;
}


void	CMT4Dlg::ShowParentWindow(BOOL bShow)
{
	CMainDlg* pThis=(CMainDlg*)m_pParent;

	pThis->ShowWindow(bShow);
}

BOOL CMT4Dlg::PreTranslateMessage(MSG* pMsg)
{
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


HBRUSH CMT4Dlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	if( pWnd->GetDlgCtrlID()==IDC_STATIC_HINT )
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(21,163,21));
	}

	return hbr;
}


void CMT4Dlg::OnBnClickedBtnMainboard()
{
	CLed4TestDlg	ld(this, TP_FUN_MB);
	this->ShowParentWindow(SW_HIDE);

	//if( IDCANCEL==ld.DoModal() )
	ld.DoModal();
	{
		this->ShowParentWindow(SW_SHOW);
	}
}
