#pragma once

#include "MT3Dlg.h"
#include "ProtocolHelper.h"


using std::wstring;
// CLedTestDlg 对话框

enum	RESULTSTATUE	
{
	MJ_NORMAL=1,
	MJ_SUCCESS,
	MJ_FAILED
};


//PSN在主板量产版固件中的偏移地址
#define MB_PSNOFFSETADDR	(0x00011800)


//JFlash超时时间
#define MB_JFLASHTIMEOUTV	(1000*30)

class CLed3TestDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLed3TestDlg)

public:
	CLed3TestDlg(LPVOID p,  TESTERTYPE type, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLed3TestDlg();

	// 对话框数据
	enum { IDD = IDD_DLG_LED3_TEST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()


public:
	CMT3Dlg*		m_pParent;
	RESULTSTATUE	m_Result;
	CFont			m_font;

	HANDLE			m_hDevice;
	TESTERTYPE		m_testType;

	//测试类型及接口描述说明
	wstring			m_wstrDes;

	//psn
	string			m_strPSN;

	//记录文件
	string			m_strFileContent;

	//依据津亚格式要求存储事件文件
	string			m_strJYFormatFileContent;
	string			m_strJYTemp;

	//统计计数 
	INT				m_nTotalNum;
	INT				m_nPassNum;
	INT				m_nFailNum;

public:
	VOID			RefreshDeviceList();
	VOID			RefreshDeviceListR();
	VOID			ShowInfo(PWCHAR pInfo, BOOL	bNew);
	VOID			SetResult();

	//显示统计信息
	VOID			SetResultInfo();

	BOOL			LedCheck();
	BOOL			MainBoardCheck();
	BOOL			WifiCheck();
	BOOL			MainBoardSportCheck();

	BOOL			UpdaterMB(string strWorkDir);

	VOID			SetBtnStatus(BOOL	bEnable);

	//验证SN码的合法性
	BOOL			CheckSN();

public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnStart();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
