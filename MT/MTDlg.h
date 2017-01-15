
// MTDlg.h : 头文件
//

#pragma once


// CMTDlg 对话框
class CMTDlg : public CDialogEx
{
// 构造
public:
	CMTDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_MT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

public:
	void	SetParent(LPVOID p);
	void	ShowParentWindow(BOOL bShow);
// 实现
protected:
	HICON	m_hIcon;

	LPVOID	m_pParent;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()


public:
	//afx_msg void OnBnClickedBtnConnect();
	afx_msg void OnBnClickedBtnMainboard();
	afx_msg void OnBnClickedBtnWifi();
	afx_msg void OnBnClickedBtnLed();
	afx_msg void OnBnClickedBtnSd();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);


private:
	//VOID	OperateServiceConfig(BOOL	bSet);
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
