#pragma once

#include "MTDlg.h"
// CMainBoardTestDlg �Ի���

class CMainBoardTestDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMainBoardTestDlg)

public:
	CMainBoardTestDlg(LPVOID p, CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CMainBoardTestDlg();

// �Ի�������
	enum { IDD = IDD_DLG_MainBoardTest };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()


private:
	CMTDlg*	m_pParent;
};
