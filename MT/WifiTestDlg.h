#pragma once

#include "MTDlg.h"
// CWifiTestDlg �Ի���

class CWifiTestDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CWifiTestDlg)

public:
	CWifiTestDlg(LPVOID p, CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CWifiTestDlg();

// �Ի�������
	enum { IDD = IDD_DLG_WIFITEST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()


private:
	CMTDlg*	m_pParent;
};
