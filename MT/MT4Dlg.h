#pragma once


// CMT4Dlg �Ի���

class CMT4Dlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMT4Dlg)

public:
	CMT4Dlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CMT4Dlg();

// �Ի�������
	enum { IDD = IDD_MT4_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
	LPVOID	m_pParent;

public:
	void	SetParent(LPVOID p);
	void	ShowParentWindow(BOOL bShow);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedBtnMainboard();
};
