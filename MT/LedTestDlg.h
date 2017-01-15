#pragma once

#include "MTDlg.h"
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

class CLedTestDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLedTestDlg)

public:
	CLedTestDlg(LPVOID p,  TESTERTYPE type, CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CLedTestDlg();

// �Ի�������
	enum { IDD = IDD_DLG_LED_TEST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()


public:
	CMTDlg*			m_pParent;
	RESULTSTATUE	m_Result;
	CFont			m_font;

	HANDLE			m_hDevice;
	TESTERTYPE		m_testType;

	//�������ͼ��ӿ�����˵��
	wstring			m_wstrDes;

	//psn
	string			m_strPSN;

	//��¼log�ļ�
	string			m_strFileContent;

	//���ݽ��Ǹ�ʽҪ��洢�¼��ļ�
	string			m_strJYFormatFileContent;
	string			m_strJYTemp;//����¼����ϸ��


	//ͳ�Ƽ��� 
	INT				m_nTotalNum;
	INT				m_nPassNum;
	INT				m_nFailNum;

public:
	VOID			RefreshDeviceList();
	VOID			RefreshDeviceListR();
	VOID			ShowInfo(PWCHAR pInfo, BOOL	bNew);
	VOID			SetResult();

	BOOL			LedCheck();
	BOOL			MainBoardCheck();
	BOOL			WifiCheck();

	BOOL			UpdaterMB(string strWorkDir);

	VOID			SetBtnStatus(BOOL	bEnable);

	//��ʾͳ����Ϣ
	VOID			SetResultInfo();


	//��֤SN��ĺϷ���
	BOOL			CheckSN();

public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnStart();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
