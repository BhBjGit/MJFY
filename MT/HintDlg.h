#pragma once


// CHintDlg 对话框

class CHintDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CHintDlg)

public:
	CHintDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CHintDlg();

// 对话框数据
	enum { IDD = IDD_DLG_HINT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
