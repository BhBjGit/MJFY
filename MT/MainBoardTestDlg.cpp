// MainBoardTestDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MT.h"
#include "MainBoardTestDlg.h"
#include "afxdialogex.h"




// CMainBoardTestDlg 对话框

IMPLEMENT_DYNAMIC(CMainBoardTestDlg, CDialogEx)

CMainBoardTestDlg::CMainBoardTestDlg(LPVOID p, CWnd* pParent /*=NULL*/)
	: CDialogEx(CMainBoardTestDlg::IDD, pParent)
{
	m_pParent=(CMTDlg*)p;
}

CMainBoardTestDlg::~CMainBoardTestDlg()
{
}

void CMainBoardTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMainBoardTestDlg, CDialogEx)
END_MESSAGE_MAP()


// CMainBoardTestDlg 消息处理程序
