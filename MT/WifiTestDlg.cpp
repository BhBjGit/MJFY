// WifiTestDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MT.h"
#include "WifiTestDlg.h"
#include "afxdialogex.h"


// CWifiTestDlg �Ի���

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


// CWifiTestDlg ��Ϣ�������
