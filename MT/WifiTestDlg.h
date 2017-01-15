#pragma once

#include "MTDlg.h"
// CWifiTestDlg 对话框

class CWifiTestDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CWifiTestDlg)

public:
	CWifiTestDlg(LPVOID p, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CWifiTestDlg();

// 对话框数据
	enum { IDD = IDD_DLG_WIFITEST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()


private:
	CMTDlg*	m_pParent;
};
