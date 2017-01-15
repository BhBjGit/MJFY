#pragma once


// CMT2Dlg �Ի���

class CMT2Dlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMT2Dlg)

public:
	CMT2Dlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CMT2Dlg();

// �Ի�������
	enum { IDD = IDD_MT2_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnSd();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedBtnWifi();
	afx_msg void OnBnClickedBtnMainboard();
	afx_msg void OnBnClickedBtnLed();

public:
	LPVOID	m_pParent;

public:
	void	SetParent(LPVOID p);
	void	ShowParentWindow(BOOL bShow);
	afx_msg void OnBnClickedBtnLeda();
};
