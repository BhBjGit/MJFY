#pragma once

#include "MT3Dlg.h"
#include "ProtocolHelper.h"


using std::wstring;
// CLedTestDlg �Ի���

enum	RESULTSTATUE	
{
	MJ_NORMAL=1,
	MJ_SUCCESS,
	MJ_FAILED
};


//PSN������������̼��е�ƫ�Ƶ�ַ
#define MB_PSNOFFSETADDR	(0x00011800)


//JFlash��ʱʱ��
#define MB_JFLASHTIMEOUTV	(1000*30)

class CLed3TestDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLed3TestDlg)

public:
	CLed3TestDlg(LPVOID p,  TESTERTYPE type, CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CLed3TestDlg();

	// �Ի�������
	enum { IDD = IDD_DLG_LED3_TEST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()


public:
	CMT3Dlg*		m_pParent;
	RESULTSTATUE	m_Result;
	CFont			m_font;

	HANDLE			m_hDevice;
	TESTERTYPE		m_testType;

	//�������ͼ��ӿ�����˵��
	wstring			m_wstrDes;

	//psn
	string			m_strPSN;

	//��¼�ļ�
	string			m_strFileContent;

	//���ݽ��Ǹ�ʽҪ��洢�¼��ļ�
	string			m_strJYFormatFileContent;
	string			m_strJYTemp;

	//ͳ�Ƽ��� 
	INT				m_nTotalNum;
	INT				m_nPassNum;
	INT				m_nFailNum;

public:
	VOID			RefreshDeviceList();
	VOID			RefreshDeviceListR();
	VOID			ShowInfo(PWCHAR pInfo, BOOL	bNew);
	VOID			SetResult();

	//��ʾͳ����Ϣ
	VOID			SetResultInfo();

	BOOL			LedCheck();
	BOOL			MainBoardCheck();
	BOOL			WifiCheck();
	BOOL			MainBoardSportCheck();

	BOOL			UpdaterMB(string strWorkDir);

	VOID			SetBtnStatus(BOOL	bEnable);

	//��֤SN��ĺϷ���
	BOOL			CheckSN();

public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnStart();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
