#pragma once


// CHintDlg �Ի���

class CHintDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CHintDlg)

public:
	CHintDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CHintDlg();

// �Ի�������
	enum { IDD = IDD_DLG_HINT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
