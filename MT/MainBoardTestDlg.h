#pragma once

#include "MTDlg.h"
// CMainBoardTestDlg 对话框

class CMainBoardTestDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMainBoardTestDlg)

public:
	CMainBoardTestDlg(LPVOID p, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMainBoardTestDlg();

// 对话框数据
	enum { IDD = IDD_DLG_MainBoardTest };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()


private:
	CMTDlg*	m_pParent;
};
