#pragma once

#include "MT2Dlg.h"

#include "ProtocolHelper.h"

#include <vector>
#include <map>

using std::map;
using std::wstring;
using std::string;


//UDP ÿ���豸�ĳ�ʱʱ��
//#define		UDPRECVMAXTIMEVALUE		(1000*30)

//typedef vector<wstring>	VECWSTR;

typedef	struct _AIRSD2
{
	//��ǰ�豸�Ƿ�궨��
	BOOL	bStatus;

	string	strMac;//key
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

	INT		nPM25Offset1;// pm2.5�Ͷ�ƫ��ֵ  	int
	INT		nPM25Offset2;// pm2.5�ж�ƫ��ֵ  	int
	INT		nPM25Offset3;// pm2.5�߶�ƫ��ֵ  	int
	INT		nPM25Offset4;// 2s
	INT		nPM25Offset5;// 2s
	INT		nPM25Offset6;// 2s
	INT		nPM25Offset7;// 2s
	INT		nPM25Offset8;// 2s
	INT		nPM25Offset9;// 2s
	INT		nPM25Offset10;//2s

	DOUBLE	dPM25K1;// pm2.5�Ͷ�Kֵ		double
	DOUBLE	dPM25K2;// pm2.5�ж�Kֵ		double
	DOUBLE	dPM25K3;// pm2.5�߶�Kֵ		double
	DOUBLE	dPM25K4;// 2s
	DOUBLE	dPM25K5;// 2s
	DOUBLE	dPM25K6;// 2s
	DOUBLE	dPM25K7;// 2s
	DOUBLE	dPM25K8;// 2s
	DOUBLE	dPM25K9;// 2s
	DOUBLE	dPM25K10;//2s

	//��ȩ
	DOUBLE		dCh2oCalibration;//2s
	DOUBLE		dch2oOriginal;//2s
	DOUBLE		dCh2oK1;//2s
	DOUBLE		dCh2oK2;//2s
	DOUBLE		dCh2oK3;//2s
	DOUBLE		dCh2oK4;//2s
	DOUBLE		dCh2oK5;//2s
	DOUBLE		dCh2oOffset1;//2s
	DOUBLE		dCh2oOffset2;//2s
	DOUBLE		dCh2oOffset3;//2s
	DOUBLE		dCh2oOffset4;//2s
	DOUBLE		dCh2oOffset5;//2s

	INT		nPM25Ori;// pm2.5�������ϴ�ԭʼ����	int
	double	dPM25Mana;// pm2.5����kalman�˲�������    double
	INT		nPm25BaseLine;
	INT		nPm25Noise;

	INT		nVoice;// ��������������		int(��Χ��0~100)
	INT		nSoc;// ������ʣ�����		int(��Χ��0~100)
	INT		nOnline;// ����������״̬		int

	//���α궨�Ƿ�ɹ�
	bool	bIsSuccess;


	//new logical
	BOOL	bSDEnd;//���豸�Ƿ�궨����
	BOOL	bReadReady;//��ȡ�߳��Ƿ��ȡ�����豸�ϴ������ݰ�
	string	strOri;//��ȡ�̶߳�ȡ����ԭʼ�ַ�����jason��
	string	strSDErrSend;//������α궨ʧ�ܣ����͸����ݽ�Kֵ��1����ֹ����
	string	strSend;//���ɵı궨���ݰ�
}AIRSD2,*PAIRSD2;

typedef	map<wstring,AIRSD2>	MAPAIRSD2;


typedef struct _CONFIGVALUE
{
	INT		nGsFrequence;//��ȡgoldern sample��Ƶ��
	INT		nRetryNum;//��ȡԭʼ���ݵĴ���������ƽ��ֵ
	INT		nWaitValue;//������궨���ݺ���ȴ���ʱ�䣨ms��
	INT		nWaitDeviceValue;//���յ����豸����ʱ��(���ճ�ʱֵ)
	DOUBLE	dPm25Error;
	INT		nCo2;
	INT		nTMP;
	INT		nHumi;

	DOUBLE	dTmpOffset;//�¶�ƫ��ֵ
	DOUBLE	dHumiOffset;//ʪ��ƫ��ֵ
}CONFIGVALUE,*PCONFIGVALUE;


typedef struct _SINGLEDATAS
{
	LPVOID	p;
	wstring	wstrMac;
}SINGLEDATAS,*PSINGLEDATAS;


#define		PM25	(0)
#define		CO2		(1)
#define		CH2O	(2)
#define		TEMP	(3)
#define		HUMITY	(4)

//�����豸���ͱ궨���ݵĴ���
#define		SENDSDCOMMANDNUM	(0x3)


//ÿ�δ��豸��goldern sample��ȡ�������Ա����ƽ��ֵ����
#define		READDATANUM			(0x5)


//�����豸ÿ�α궨��ȡԭʼ����������
#define		READDATAMAXNUM		(1000)

class CStandard2Dlg : public CDialogEx
{
	DECLARE_DYNAMIC(CStandard2Dlg)

public:
	CStandard2Dlg(LPVOID p, CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CStandard2Dlg();

	// �Ի�������
	enum { IDD = IDD_DLG_Standardization2Dlg };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

private:
	CMT2Dlg*	m_pParent;


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
	//SHORT			m_SDMeanValue[READDATANUM][5];
	SHORT			(*m_SDMeanValue)[5];//

	//�궨ֵ
	MAPAIRSD2		m_mapAirSD;

	//��ⷵ��ֵ
	MAPAIRSD2		m_mapRetAirSD;

	//�����ļ���Ϣ
	CONFIGVALUE		m_configS;

	//ÿ��ȡ����buffer������ƽ��ֵ
	//MAPAIRSD		m_mapReadBuf[READDATANUM];
	MAPAIRSD2*		m_mapReadBuf;

	//��¼�ļ�
	string			m_strFileContent;

	//goldern sampleֵ
	string			m_strGSV;

	//�Ƿ���ʾͨѶ����
	BOOL			m_isShow;

	//ͳ�Ƽ��� 
	INT				m_nTotalNum;
	INT				m_nPassNum;
	INT				m_nFailNum;

	//��GS�߳̽�������
	BOOL			m_ReadThreadEnd;

	//*******************************�޸����߼���ÿ���豸�����궨
	//�洢��һ�ζ�ȡ������ֵ�����ڱ궨ʹ��
	MAPAIRSD2		m_mapRecvAirSD;

	//��ȡ�߳��˳���ʶ
	BOOL			m_bReadThreadEnd;

	//��ȡ�߳̾���ͷ����߳̾����������������ʱ��ֹ���ͷ�
	HANDLE			m_hReadCore;
	PHANDLE			m_hS;
	INT				m_nSCount;

	//��ȡ���軺����
	PBYTE			m_pReadBuf;


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
	BOOL			ParseRetDatas(PBYTE pDatas, wstring& wstrMac, MAPAIRSD2* pMap, BOOL	bCheckMode=FALSE);
	//������
	BOOL			ParseSingleDatas(string strOri, PAIRSD2 pAirD);
	BOOL			FindKeyStr(string strOri, string strKey, char end,string& strRet);

	//�����̣�����ԭʼ���궨���ݵ�ƽ��ֵ
	VOID			CalOriMeanValue(AIRSD2& airDes, PAIRSD2	pAirSour, INT nCount);

	//���͹㲥  bEntry TRUE����궨ģʽ  FALSE�˳��궨ģʽ
	BOOL			SendUDPEntrySD(BOOL	bEntry);

	//��ȡ����
	//bCheckMode �Ƿ���У��ģʽ������ǣ����ݹ���ֻ��ȡһ�μ��ɣ����ü���ƽ��ֵ
	BOOL			RecvUDPDatas(MAPAIRSD2*	mapV, BOOL	bCheckMode=FALSE);

	//���궨�����Ƿ���ȷ����
	BOOL 			CheckSDDatas();
	//�����̣����궨�����Ƿ���ȷ
	BOOL			CheckSDDatasEx(AIRSD2& as);

	//�����̣��Ƿ������豸���궨���
	BOOL			IsAllDeviceSDEnd();


	//�Ƿ�����device����������(first)
	BOOL			IsRecvAllDeviceData(MAPAIRSD2*	mapV);

	//�������ݺ󣬼��������豸�ı궨ֵ(first)
	VOID			EnunAllDataAndSD();
	//�����̣�����궨����
	VOID			GetSDDatasAndFormation(AIRSD2& airDes);

	//MAC:C8-93-46-C7-34-48  ->  C89346C73448
	VOID			ForamtionMacStr(wstring& wstr);

	//��ȡ���ò���
	BOOL			ReadFromIni();

	//���ݶ�ȡ�����ò�����ʼ���������ͷ�
	BOOL			InitOrDestoryParams(BOOL	bInit);

	//��ʾ�ɹ���ʧ���豸����
	VOID			ShowResultInfo();

	//����MAC��ַ��ʾMCU�汾��
	VOID			ShowMCUVer(LPSTR p, BOOL bNew=TRUE);

	//���ս���Ҫ��ĶԽӸ�ʽ�ĵ�
	VOID			SaveJYFormationFile(MAPAIRSD2::iterator p, string strResult, BOOL bPass);
	VOID			SaveJYFormationFileEx(AIRSD2 as, string strResult, BOOL bPass);
public:
	afx_msg void OnBnClickedButtonStart();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnEnChangeEditMac();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedBtnClear();
};
