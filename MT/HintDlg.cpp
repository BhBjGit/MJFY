// HintDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MT.h"
#include "HintDlg.h"
#include "afxdialogex.h"


// CHintDlg 对话框

IMPLEMENT_DYNAMIC(CHintDlg, CDialogEx)

CHintDlg::CHintDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CHintDlg::IDD, pParent)
{

}

CHintDlg::~CHintDlg()
{
}

void CHintDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CHintDlg, CDialogEx)
END_MESSAGE_MAP()


// CHintDlg 消息处理程序


BOOL CHintDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CRect	rect;
	int		scrWidth=0;
	int		scrHeight=0;
	GetWindowRect(&rect);
	scrWidth=GetSystemMetrics(SM_CXSCREEN);
	scrHeight=GetSystemMetrics(SM_CYSCREEN);
	::SetWindowPos(this->m_hWnd, HWND_TOPMOST, (scrWidth-rect.Width())/2, (scrHeight-rect.Height())/2, rect.Width(), rect.Height(), SWP_SHOWWINDOW);


	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
