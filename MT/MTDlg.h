
// MTDlg.h : ͷ�ļ�
//

#pragma once


// CMTDlg �Ի���
class CMTDlg : public CDialogEx
{
// ����
public:
	CMTDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_MT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

public:
	void	SetParent(LPVOID p);
	void	ShowParentWindow(BOOL bShow);
// ʵ��
protected:
	HICON	m_hIcon;

	LPVOID	m_pParent;

	// ���ɵ���Ϣӳ�亯��
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
