#pragma once

#include "MTDlg.h"

#include "ProtocolHelper.h"

#include <vector>
#include <map>

using std::map;
using std::wstring;
using std::string;


//UDP ÿ���豸�ĳ�ʱʱ��
#define		UDPRECVMAXTIMEVALUE		(1000*20)

//typedef vector<wstring>	VECWSTR;

typedef	struct _AIRSD 
{
	//��ǰ�豸�Ƿ�궨��
	BOOL	bStatus;

	string	strMac;
	string	strMcvVer;
	string	strWfVer;
	string  strPsn;
	INT		nPM25;// ��ǰpm2.5            int		//
	INT		nCo2;// ��ǰco2              int		//
	double	dTemp;// ��ǰ�¶�	        double		//
	double	dTempOffset;//
	double	dHumi;// ��ǰʪ��	        double		//
	double	dHumiOffset;//
	INT		nCo2Offset;// co2ƫ��ֵ 	        int
	INT		nPM25OffsetL;// pm2.5�Ͷ�ƫ��ֵ  	int
	INT		nPM25OffsetM;// pm2.5�ж�ƫ��ֵ  	int
	INT		nPM25OffsetH;// pm2.5�߶�ƫ��ֵ  	int
	DOUBLE	dPM25KL;// pm2.5�Ͷ�Kֵ		double
	DOUBLE	dPM25KM;// pm2.5�ж�Kֵ		double
	DOUBLE	dPM25KH;// pm2.5�߶�Kֵ		double
	INT		nPM25Ori;// pm2.5�������ϴ�ԭʼ����	int
	double	dPM25Mana;// pm2.5����kalman�˲�������    double
	INT		nPm25BaseLine;
	INT		nPm25Noise;
	INT		nVoice;// ��������������		int(��Χ��0~100)
	INT		nSoc;// ������ʣ�����		int(��Χ��0~100)
	INT		nOnline;// ����������״̬		int

	string	strSend;
}AIRSD,*PAIRSD;

typedef	map<wstring,AIRSD>	MAPAIRSD;


#define		PM25	(0)
#define		CO2		(1)
#define		CH2O	(2)
#define		TEMP	(3)
#define		HUMITY	(4)

//�����豸���ͱ궨���ݵĴ���
#define		SENDSDCOMMANDNUM	(0x3)

//���ͱ궨�����ȴ�ʱ��		
#define		SENDSDCOMMANDSLEEP	(30*1000)


//ÿ�δ��豸��goldern sample��ȡ�������Ա����ƽ��ֵ����
#define		READDATANUM			(0x5)




class CStandardDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CStandardDlg)

public:
	CStandardDlg(LPVOID p, CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CStandardDlg();

// �Ի�������
	enum { IDD = IDD_DLG_StandardizationDlg };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

private:
	CMTDlg*	m_pParent;
public:
	virtual BOOL OnInitDialog();

public:
	CListCtrl*		m_listReport;
	CComboBox*		m_pComBox;

	HANDLE			m_deviceHandle;

	//listReport ��ʾ��Ϣ����
	INT				m_nListReportIndex;

	//�궨��ֵ
	SHORT			m_SDValue[5];
	SHORT			m_SDMeanValue[READDATANUM][5];

	//�궨ֵ
	MAPAIRSD		m_mapAirSD;

	//��ⷵ��ֵ
	MAPAIRSD		m_mapRetAirSD;

	//ÿ��ȡ����buffer������ƽ��ֵ
	MAPAIRSD		m_mapReadBuf[READDATANUM];

	//��¼�ļ�
	string			m_strFileContent;

	//goldern sampleֵ
	string			m_strGSV;

	//�Ƿ���ʾͨѶ����
	BOOL			m_isShow;

	//��GS�߳̽�������
	BOOL			m_ReadThreadEnd;

public:
	VOID			RefreshDeviceList();
	VOID			SetBtnStatus(BOOL	bEnable);
	VOID			SetInputMacStatus(BOOL bEnable);
	VOID			InitListCtrl();

	//������Ϊindex��ȡgoldern sample��ֵ���Լ���ƽ��ֵ
	VOID			ReadGSValue(INT nIndex);

	//calcu gs mean value
	VOID			CalGSMeanValue();

	//����UI��ʾ��Ϣ
	VOID			SetShowInfo(LPCTSTR wcsBuf, BOOL	bSpecial=FALSE);

	//bCheckMode �Ƿ���У��ģʽ������ǣ����ݹ���ֻ��ȡһ�μ��ɣ����ü���ƽ��ֵ
	BOOL			ParseRetDatas(PBYTE pDatas, wstring& wstrMac, MAPAIRSD* pMap, BOOL	bCheckMode=FALSE);
	BOOL			FindKeyStr(string strOri, string strKey, char end,string& strRet);

	//���͹㲥  bEntry TRUE����궨ģʽ  FALSE�˳��궨ģʽ
	BOOL			SendUDPEntrySD(BOOL	bEntry);

	//��ȡ����
	//bCheckMode �Ƿ���У��ģʽ������ǣ����ݹ���ֻ��ȡһ�μ��ɣ����ü���ƽ��ֵ
	BOOL			RecvUDPDatas(MAPAIRSD*	mapV, BOOL	bCheckMode=FALSE);

	//���궨�����Ƿ���ȷ����
	BOOL 			CheckSDDatas();



	//�Ƿ�����device����������(first)
	BOOL			IsRecvAllDeviceData(MAPAIRSD*	mapV);

	//�������ݺ󣬼��������豸�ı궨ֵ(first)
	VOID			EnunAllDataAndSD();

	//MAC:C8-93-46-C7-34-48  ->  C89346C73448
	VOID			ForamtionMaxStr(wstring& wstr);


	//���ս���Ҫ��ĶԽӸ�ʽ�ĵ�
	VOID			SaveJYFormationFile(MAPAIRSD::iterator p, string strResult, BOOL bPass);

public:
	afx_msg void OnBnClickedButtonStart();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnEnChangeEditMac();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedBtnClear();
};
