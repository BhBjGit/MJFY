#pragma once

#include "MTDlg.h"
#include "MT2Dlg.h"
#include "MT3Dlg.h"
#include "MT4Dlg.h"

// CMainDlg 对话框

class CMainDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMainDlg)

public:
	CMainDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMainDlg();

// 对话框数据
	enum { IDD = IDD_DLG_MAIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

private:
	BOOL		Init();

private:
	CStatic*	m_pStaticCtrl;
	CComboBox*	m_pCombox;
	CMTDlg		m_OneDlg;
	CMT2Dlg		m_SecondDlg;
	CMT3Dlg		m_thirdDlg;
	CMT4Dlg		m_fourDlg;

public:
	afx_msg void OnCbnSelchangeComboDes();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
