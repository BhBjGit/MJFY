#pragma once

#include "MT2Dlg.h"
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

class CLed2TestDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLed2TestDlg)

public:
	CLed2TestDlg(LPVOID p,  TESTERTYPE type, BYTE	mode=1, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLed2TestDlg();

	// 对话框数据
	enum { IDD = IDD_DLG_LED2_TEST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()


public:
	CMT2Dlg*		m_pParent;
	RESULTSTATUE	m_Result;
	CFont			m_font;

	HANDLE			m_hDevice;
	TESTERTYPE		m_testType;

	//主从模式 master(1)/slave(2) mode,针对于灯板检测，是主机模式还是节点模式（检测方式不一样）
	BYTE			m_bMaster;

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

	//节点灯板检测模式
	BOOL			LedSlaveCheck();
	//主机灯板检测模式
	BOOL			LedMasterCheck();

	BOOL			MainBoardCheck();
	BOOL			WifiCheck();

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
