// WifiTestDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MT.h"
#include "WifiTestDlg.h"
#include "afxdialogex.h"


// CWifiTestDlg 对话框

IMPLEMENT_DYNAMIC(CWifiTestDlg, CDialogEx)

CWifiTestDlg::CWifiTestDlg(LPVOID p, CWnd* pParent /*=NULL*/)
	: CDialogEx(CWifiTestDlg::IDD, pParent)
{
	m_pParent=(CMTDlg*)p;
}

CWifiTestDlg::~CWifiTestDlg()
{
}

void CWifiTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CWifiTestDlg, CDialogEx)
END_MESSAGE_MAP()


// CWifiTestDlg 消息处理程序
