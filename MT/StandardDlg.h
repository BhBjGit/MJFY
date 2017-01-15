#pragma once

#include "MTDlg.h"

#include "ProtocolHelper.h"

#include <vector>
#include <map>

using std::map;
using std::wstring;
using std::string;


//UDP 每个设备的超时时间
#define		UDPRECVMAXTIMEVALUE		(1000*20)

//typedef vector<wstring>	VECWSTR;

typedef	struct _AIRSD 
{
	//当前设备是否标定过
	BOOL	bStatus;

	string	strMac;
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
	INT		nPM25OffsetL;// pm2.5低段偏移值  	int
	INT		nPM25OffsetM;// pm2.5中段偏移值  	int
	INT		nPM25OffsetH;// pm2.5高段偏移值  	int
	DOUBLE	dPM25KL;// pm2.5低段K值		double
	DOUBLE	dPM25KM;// pm2.5中段K值		double
	DOUBLE	dPM25KH;// pm2.5高段K值		double
	INT		nPM25Ori;// pm2.5传感器上传原始数据	int
	double	dPM25Mana;// pm2.5经过kalman滤波后数据    double
	INT		nPm25BaseLine;
	INT		nPm25Noise;
	INT		nVoice;// 空气果声音设置		int(范围：0~100)
	INT		nSoc;// 空气果剩余电量		int(范围：0~100)
	INT		nOnline;// 空气果联网状态		int

	string	strSend;
}AIRSD,*PAIRSD;

typedef	map<wstring,AIRSD>	MAPAIRSD;


#define		PM25	(0)
#define		CO2		(1)
#define		CH2O	(2)
#define		TEMP	(3)
#define		HUMITY	(4)

//单个设备发送标定数据的次数
#define		SENDSDCOMMANDNUM	(0x3)

//发送标定命令后等待时间		
#define		SENDSDCOMMANDSLEEP	(30*1000)


//每次从设备和goldern sample读取次数，以便进行平均值计算
#define		READDATANUM			(0x5)




class CStandardDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CStandardDlg)

public:
	CStandardDlg(LPVOID p, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CStandardDlg();

// 对话框数据
	enum { IDD = IDD_DLG_StandardizationDlg };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	CMTDlg*	m_pParent;
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
	SHORT			m_SDMeanValue[READDATANUM][5];

	//标定值
	MAPAIRSD		m_mapAirSD;

	//检测返回值
	MAPAIRSD		m_mapRetAirSD;

	//每次取出的buffer，计算平均值
	MAPAIRSD		m_mapReadBuf[READDATANUM];

	//记录文件
	string			m_strFileContent;

	//goldern sample值
	string			m_strGSV;

	//是否显示通讯数据
	BOOL			m_isShow;

	//读GS线程结束条件
	BOOL			m_ReadThreadEnd;

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
	BOOL			ParseRetDatas(PBYTE pDatas, wstring& wstrMac, MAPAIRSD* pMap, BOOL	bCheckMode=FALSE);
	BOOL			FindKeyStr(string strOri, string strKey, char end,string& strRet);

	//发送广播  bEntry TRUE进入标定模式  FALSE退出标定模式
	BOOL			SendUDPEntrySD(BOOL	bEntry);

	//读取数据
	//bCheckMode 是否是校验模式，如果是，根据规则，只读取一次即可，不用计算平均值
	BOOL			RecvUDPDatas(MAPAIRSD*	mapV, BOOL	bCheckMode=FALSE);

	//检查标定数据是否正确设置
	BOOL 			CheckSDDatas();



	//是否所有device都返回数据(first)
	BOOL			IsRecvAllDeviceData(MAPAIRSD*	mapV);

	//返回数据后，计算所有设备的标定值(first)
	VOID			EnunAllDataAndSD();

	//MAC:C8-93-46-C7-34-48  ->  C89346C73448
	VOID			ForamtionMaxStr(wstring& wstr);


	//按照津亚要求的对接格式文档
	VOID			SaveJYFormationFile(MAPAIRSD::iterator p, string strResult, BOOL bPass);

public:
	afx_msg void OnBnClickedButtonStart();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnEnChangeEditMac();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedBtnClear();
};
