#pragma once


// CMT3Dlg 对话框

class CMT3Dlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMT3Dlg)

public:
	CMT3Dlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMT3Dlg();

// 对话框数据
	enum { IDD = IDD_MT3_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

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
