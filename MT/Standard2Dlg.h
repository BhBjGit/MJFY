#pragma once

#include "MT2Dlg.h"

#include "ProtocolHelper.h"

#include <vector>
#include <map>

using std::map;
using std::wstring;
using std::string;


//UDP 每个设备的超时时间
//#define		UDPRECVMAXTIMEVALUE		(1000*30)

//typedef vector<wstring>	VECWSTR;

typedef	struct _AIRSD2
{
	//当前设备是否标定过
	BOOL	bStatus;

	string	strMac;//key
	string	strMcvVer;
	string	strWfVer;
	string  strPsn;
	INT		nPM25;// 当前pm2.5            int		//
	INT		nCo2;// 当前co2              int		//
	double	dTemp;// 当前温度	        double		//
	double	dTempOffset;//
	double	dHumi;// 当前湿度	        double		//
	double	dHumiOffset;//
	INT		nCo2Offset;// co2偏移值 	        int

	INT		nPM25Offset1;// pm2.5低段偏移值  	int
	INT		nPM25Offset2;// pm2.5中段偏移值  	int
	INT		nPM25Offset3;// pm2.5高段偏移值  	int
	INT		nPM25Offset4;// 2s
	INT		nPM25Offset5;// 2s
	INT		nPM25Offset6;// 2s
	INT		nPM25Offset7;// 2s
	INT		nPM25Offset8;// 2s
	INT		nPM25Offset9;// 2s
	INT		nPM25Offset10;//2s

	DOUBLE	dPM25K1;// pm2.5低段K值		double
	DOUBLE	dPM25K2;// pm2.5中段K值		double
	DOUBLE	dPM25K3;// pm2.5高段K值		double
	DOUBLE	dPM25K4;// 2s
	DOUBLE	dPM25K5;// 2s
	DOUBLE	dPM25K6;// 2s
	DOUBLE	dPM25K7;// 2s
	DOUBLE	dPM25K8;// 2s
	DOUBLE	dPM25K9;// 2s
	DOUBLE	dPM25K10;//2s

	//甲醛
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

	INT		nPM25Ori;// pm2.5传感器上传原始数据	int
	double	dPM25Mana;// pm2.5经过kalman滤波后数据    double
	INT		nPm25BaseLine;
	INT		nPm25Noise;

	INT		nVoice;// 空气果声音设置		int(范围：0~100)
	INT		nSoc;// 空气果剩余电量		int(范围：0~100)
	INT		nOnline;// 空气果联网状态		int

	//本次标定是否成功
	bool	bIsSuccess;


	//new logical
	BOOL	bSDEnd;//该设备是否标定结束
	BOOL	bReadReady;//读取线程是否读取到该设备上传的数据包
	string	strOri;//读取线程读取到的原始字符串（jason）
	string	strSDErrSend;//如果本次标定失败，发送该数据将K值置1，防止爆表
	string	strSend;//生成的标定数据包
}AIRSD2,*PAIRSD2;

typedef	map<wstring,AIRSD2>	MAPAIRSD2;


typedef struct _CONFIGVALUE
{
	INT		nGsFrequence;//获取goldern sample的频率
	INT		nRetryNum;//获取原始数据的次数，计算平均值
	INT		nWaitValue;//发送完标定数据后需等待的时间（ms）
	INT		nWaitDeviceValue;//接收单个设备数据时间(接收超时值)
	DOUBLE	dPm25Error;
	INT		nCo2;
	INT		nTMP;
	INT		nHumi;

	DOUBLE	dTmpOffset;//温度偏移值
	DOUBLE	dHumiOffset;//湿度偏移值
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

//单个设备发送标定数据的次数
#define		SENDSDCOMMANDNUM	(0x3)


//每次从设备和goldern sample读取次数，以便进行平均值计算
#define		READDATANUM			(0x5)


//单个设备每次标定读取原始数据最大次数
#define		READDATAMAXNUM		(1000)

class CStandard2Dlg : public CDialogEx
{
	DECLARE_DYNAMIC(CStandard2Dlg)

public:
	CStandard2Dlg(LPVOID p, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CStandard2Dlg();

	// 对话框数据
	enum { IDD = IDD_DLG_Standardization2Dlg };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	CMT2Dlg*	m_pParent;


public:
	virtual BOOL OnInitDialog();

public:
	CListCtrl*		m_listReport;
	CComboBox*		m_pComBox;

	HANDLE			m_deviceHandle;

	//listReport 显示信息索引
	INT				m_nListReportIndex;

	//标定基值
	SHORT			m_SDValue[5];
	//SHORT			m_SDMeanValue[READDATANUM][5];
	SHORT			(*m_SDMeanValue)[5];//

	//标定值
	MAPAIRSD2		m_mapAirSD;

	//检测返回值
	MAPAIRSD2		m_mapRetAirSD;

	//配置文件信息
	CONFIGVALUE		m_configS;

	//每次取出的buffer，计算平均值
	//MAPAIRSD		m_mapReadBuf[READDATANUM];
	MAPAIRSD2*		m_mapReadBuf;

	//记录文件
	string			m_strFileContent;

	//goldern sample值
	string			m_strGSV;

	//是否显示通讯数据
	BOOL			m_isShow;

	//统计计数 
	INT				m_nTotalNum;
	INT				m_nPassNum;
	INT				m_nFailNum;

	//读GS线程结束条件
	BOOL			m_ReadThreadEnd;

	//*******************************修改新逻辑，每条设备单独标定
	//存储第一次读取的数据值，用于标定使用
	MAPAIRSD2		m_mapRecvAirSD;

	//读取线程退出标识
	BOOL			m_bReadThreadEnd;

	//读取线程句柄和分析线程句柄，析构函数调用时终止并释放
	HANDLE			m_hReadCore;
	PHANDLE			m_hS;
	INT				m_nSCount;

	//读取所需缓冲区
	PBYTE			m_pReadBuf;


public:
	VOID			RefreshDeviceList();
	VOID			SetBtnStatus(BOOL	bEnable);
	VOID			SetInputMacStatus(BOOL bEnable);
	VOID			InitListCtrl();

	//以索引为index读取goldern sample的值，以计算平均值
	VOID			ReadGSValue(INT nIndex);

	//calcu gs mean value
	VOID			CalGSMeanValue();

	//设置UI提示信息
	VOID			SetShowInfo(LPCTSTR wcsBuf, BOOL	bSpecial=FALSE);

	//bCheckMode 是否是校验模式，如果是，根据规则，只读取一次即可，不用计算平均值
	BOOL			ParseRetDatas(PBYTE pDatas, wstring& wstrMac, MAPAIRSD2* pMap, BOOL	bCheckMode=FALSE);
	//新流程
	BOOL			ParseSingleDatas(string strOri, PAIRSD2 pAirD);
	BOOL			FindKeyStr(string strOri, string strKey, char end,string& strRet);

	//新流程，计算原始待标定数据的平均值
	VOID			CalOriMeanValue(AIRSD2& airDes, PAIRSD2	pAirSour, INT nCount);

	//发送广播  bEntry TRUE进入标定模式  FALSE退出标定模式
	BOOL			SendUDPEntrySD(BOOL	bEntry);

	//读取数据
	//bCheckMode 是否是校验模式，如果是，根据规则，只读取一次即可，不用计算平均值
	BOOL			RecvUDPDatas(MAPAIRSD2*	mapV, BOOL	bCheckMode=FALSE);

	//检查标定数据是否正确设置
	BOOL 			CheckSDDatas();
	//新流程，检查标定数据是否正确
	BOOL			CheckSDDatasEx(AIRSD2& as);

	//新流程，是否所有设备都标定完毕
	BOOL			IsAllDeviceSDEnd();


	//是否所有device都返回数据(first)
	BOOL			IsRecvAllDeviceData(MAPAIRSD2*	mapV);

	//返回数据后，计算所有设备的标定值(first)
	VOID			EnunAllDataAndSD();
	//新流程，计算标定数据
	VOID			GetSDDatasAndFormation(AIRSD2& airDes);

	//MAC:C8-93-46-C7-34-48  ->  C89346C73448
	VOID			ForamtionMacStr(wstring& wstr);

	//读取配置参数
	BOOL			ReadFromIni();

	//根据读取的配置参数初始化变量或释放
	BOOL			InitOrDestoryParams(BOOL	bInit);

	//显示成功或失败设备个数
	VOID			ShowResultInfo();

	//按照MAC地址显示MCU版本号
	VOID			ShowMCUVer(LPSTR p, BOOL bNew=TRUE);

	//按照津亚要求的对接格式文档
	VOID			SaveJYFormationFile(MAPAIRSD2::iterator p, string strResult, BOOL bPass);
	VOID			SaveJYFormationFileEx(AIRSD2 as, string strResult, BOOL bPass);
public:
	afx_msg void OnBnClickedButtonStart();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnEnChangeEditMac();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedBtnClear();
};
