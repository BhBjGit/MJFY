// StandardDlg.cpp : ÊµÏÖÎÄ¼þ
//

#include "stdafx.h"
#include "MT.h"
#include "StandardDlg.h"
#include "afxdialogex.h"
#include "SerialPortHelper.h"
#include <strsafe.h>
#include "UDP.h"

//macµØÖ·³¤¶È
#define		SD_MACADDRLEN				(12)
//·¢ËÍ¹ã²¥ÖØÊÔ´ÎÊý
#define		SD_MAXUPDBOARCOSTNUM		(0x3)

//UPDÉÏ´«Êý¾ÝÔ¤¶¨Òå×î´ó³¤¶È
#define		SD_UDPRECVMAXLEN			(0xFFFF)

//¶Ë¿ÚºÅ
#define		SD_UDPPORTSEND				(33332)
#define		SD_UDPPORTRECV				(33334)

// CStandardDlg ¶Ô»°¿ò

IMPLEMENT_DYNAMIC(CStandardDlg, CDialogEx)

CStandardDlg::CStandardDlg(LPVOID p, CWnd* pParent /*=NULL*/)
	: CDialogEx(CStandardDlg::IDD, pParent),
m_nListReportIndex(0),
m_isShow(FALSE),
m_ReadThreadEnd(FALSE)
{
	m_pParent=(CMTDlg*)p;
	m_deviceHandle=INVALID_HANDLE_VALUE;
}

CStandardDlg::~CStandardDlg()
{
	m_ReadThreadEnd=TRUE;
	CloseHandle(m_deviceHandle);
}

void CStandardDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CStandardDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_START, &CStandardDlg::OnBnClickedButtonStart)
	ON_WM_CTLCOLOR()
	ON_EN_CHANGE(IDC_EDIT_MAC, &CStandardDlg::OnEnChangeEditMac)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_CLEAR, &CStandardDlg::OnBnClickedBtnClear)
END_MESSAGE_MAP()


// CStandardDlg ÏûÏ¢´¦Àí³ÌÐò

VOID			CStandardDlg::InitListCtrl()
{
	m_listReport=(CListCtrl*)GetDlgItem(IDC_LIST_INFO);
	if( m_listReport)
	{
		m_listReport->SetExtendedStyle(LVS_EX_FULLROWSELECT| //ÔÊÐíÕûÐÐÑ¡ÖÐ
										LVS_EX_HEADERDRAGDROP|  //ÔÊÐíÕûÁÐÍÏ¶¯
										LVS_EX_GRIDLINES|     //»­³öÍø¸ñÏß
										LVS_EX_ONECLICKACTIVATE|     //µ¥»÷Ñ¡ÖÐÏî
										LVS_EX_FLATSB);    //±âÆ½·ç¸ñÏÔÊ¾¹ö¶¯Ìõ

		m_listReport->InsertColumn(0, L"information",LVCFMT_CENTER,320*7);
		//m_listReport.InsertColumn(1,"id",LVCFMT_CENTER,90);

		m_listReport->SetTextColor(LISTREPORT_COLOR);		
	}
}

unsigned _stdcall _ThreadForRefreshGS(LPVOID p)
{
	CStandardDlg* pThis=(CStandardDlg*)p;
	CComboBox*	pComBox=pThis->m_pComBox;
	HANDLE		hDevice=pThis->m_deviceHandle;
	CHAR		csBuf[MAX_PATH]={0};
	BYTE		retDatas[SHORTLEN]={0};
	SHORT		SDValue[5];

	//get standard value
	while( !pThis->m_ReadThreadEnd)
	{
		if( P_GetSDResult(hDevice, /*TP_STAND,*/ retDatas, SHORTLEN))
		{
			memmove(SDValue, retDatas+1, sizeof(SDValue));
			SDValue[PM25]=ntohs(SDValue[PM25]);
			SDValue[CO2]=ntohs(SDValue[CO2]);
			SDValue[CH2O]=ntohs(SDValue[CH2O]);
			SDValue[HUMITY]=ntohs(SDValue[HUMITY]);
			SDValue[TEMP]=ntohs(SDValue[TEMP]);

			StringCchPrintfA(csBuf, MAX_PATH, "PM2.5:	%d ug/m3\r\nCO2:	%d ppm\r\nCH2O:	%f mg/m3\r\nTemp:	%f ¡æ\r\nHumity:	%f%%\r\n", 
				SDValue[PM25], SDValue[CO2],
				((FLOAT)(SDValue[CH2O])/1000.0),
				((FLOAT)(SDValue[TEMP])/10), ((FLOAT)(SDValue[HUMITY])/10) );

			SetDlgItemTextA(pThis->m_hWnd, IDC_EDIT_ORIINFO, csBuf);
		}

		Sleep(2000);
	}


	OutputDebugStringA(__FUNCTION__"...end\r\n");
	return 1;
}

BOOL CStandardDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//top window
	CRect	rect;
	int		scrWidth=0;
	int		scrHeight=0;
	GetWindowRect(&rect);
	scrWidth=GetSystemMetrics(SM_CXSCREEN);
	scrHeight=GetSystemMetrics(SM_CYSCREEN);
	::SetWindowPos(this->m_hWnd, HWND_TOPMOST, (scrWidth-rect.Width())/2, (scrHeight-rect.Height())/2, rect.Width(), rect.Height(), SWP_SHOWWINDOW);

	this->InitListCtrl();
	this->SetBtnStatus(FALSE);

	//communicate with device(com),get com num
	this->RefreshDeviceList();
	_beginthreadex(NULL, NULL, _ThreadForRefreshGS, this, NULL, NULL);
	//SetTimer(1, 1500, NULL);

	SetWindowTextA(this->m_hWnd, "SD(1s)");
	CF_SetTitleWithVersion(this->m_hWnd);

	////////////TEST MAC addr
	//SetDlgItemTextA(this->m_hWnd, IDC_EDIT_MAC, "C89346C60721\r\nC89346C61D51");//C89346C60721\r\n
	////////////

	return TRUE;  // return TRUE unless you set the focus to a control
	// Òì³£: OCX ÊôÐÔÒ³Ó¦·µ»Ø FALSE
}



VOID		printfMap(MAPAIRSD pMap)
{
	for(MAPAIRSD::iterator p=pMap.begin(); p!=pMap.end(); p++)
	{
		CF_OutputDBGInfo("mac:%s  temp:%f  temp offset:%f humi:%f humi offset:%f  co2:%d co2 offset:%d pm25:%d  pm25K1:%f pm25k2:%f  pm25k3:%f\r\n",
							p->second.strMac.c_str(),				
							p->second.dTemp, p->second.dTempOffset,
							p->second.dHumi, p->second.dHumiOffset,
							p->second.nCo2, p->second.nCo2Offset, 
							p->second.nPM25, 
							p->second.dPM25KL, p->second.dPM25KM, p->second.dPM25KH
							);
	}
}

VOID		FormatMapData(wstring wstrMac, MAPAIRSD* pMap, wstring wstrDes, BOOL bSend)
{
	MAPAIRSD::iterator	p=pMap->find(wstrMac);
	WCHAR				wcsBuf[MAX_PATH]={0};

	wstrDes.clear();
	if(p!=pMap->end())
	{
		if( bSend)
		{

		}
		else
		{
			StringCchPrintfW(wcsBuf, MAX_PATH, L"co2:%d co2 offset:%d temp:%f temp offset:%f humi:%f humi offset:%f pm25:%d pm25 kalman:%f pm25 ori:%d pm25 base_line:%d pm25 noise:%d", 
																	p->second.nCo2, p->second.nCo2Offset,
																	p->second.dTemp, p->second.dTempOffset, 
																	p->second.dHumi, p->second.dHumiOffset,
																	p->second.nPM25, p->second.dPM25Mana, p->second.nPM25Ori, 
																	p->second.nPm25BaseLine, p->second.nPm25Noise);
		}
		

		wstrDes=wcsBuf;
	}
}

VOID			CStandardDlg::RefreshDeviceList()
{
	wstring		wstr;
	VECSTR		vs;
	CComboBox*	pComBox=(CComboBox*)GetDlgItem(IDC_COMBO_DEVICENAME);
	BOOL		bFind=FALSE;
	TESTERTYPE	type=TP_NONE;
	HANDLE		hDevice=INVALID_HANDLE_VALUE;
	string		str;
	CHAR		csBuf[MAX_PATH]={0};
	BYTE		retDatas[SHORTLEN]={0};

	this->m_pComBox=pComBox;
	EnumCommPort(vs);
	for(VECSTR::iterator p=vs.begin(); p!=vs.end(); p++)
	{
		wstr=p->c_str();
		OpenSerialPort(wstr, /*m_hDevice*/hDevice);

		type=P_GetType(hDevice);


		if(	TP_STAND==type)
		{
			//get standard value
			if( P_GetSDResult(hDevice, /*TP_STAND,*/ retDatas, SHORTLEN))
			{
				/*
				PM2.5 : 2×Ö½Ú³¤¶ÈÎÞ·ûºÅÕûÊý£¬¸ßÎ»ÔÚÇ°  µ¥Î»ÊÇug/m3 ±íÊ¾µ±Ç°PM2.5Å¨¶È

				CO2  : 2×Ö½Ú³¤¶ÈÎÞ·ûºÅÕûÊý£¬¸ßÎ»ÔÚÇ°  µ¥Î»ÊÇppm ±íÊ¾µ±Ç°CO2Å¨¶È

				CH2O : 2×Ö½Ú³¤¶ÈÎÞ·ûºÅÕûÊý£¬¸ßÎ»ÔÚÇ°  µ¥Î»ÊÇmg/m3 ±íÊ¾µ±Ç°¼×È©Å¨¶È

				Temp: 2×Ö½Ú³¤¶ÈÓÐ·ûºÅÕûÊý£¬¸ßÎ»ÔÚÇ°  µ¥Î»ÊÇ0.1ÉãÊÏ¶È ±íÊ¾µ±Ç°ÎÂ¶È¶È

				Humity: 2×Ö½Ú³¤¶ÈÎÞ·ûºÅÕûÊý£¬¸ßÎ»ÔÚÇ°  µ¥Î»ÊÇ0.1 °Ù·Ö±È£¨25Ôò±íÊ¾µ±Ç°Êª¶ÈÎª2.5%£©±íÊ¾µ±Ç°¿ÕÆøË®·ÖÏà¶ÔÊª¶È

				*/
				//ÉÏ´«Êý¾ÝÎª´ó¶ËµØÖ·Ä£Ê½£¬ÐèÒªÇÐ»»ÖÁÐ¡¶Ë 
				memmove(m_SDValue, retDatas+1, sizeof(m_SDValue));
				m_SDValue[PM25]=ntohs(m_SDValue[PM25]);
				m_SDValue[CO2]=ntohs(m_SDValue[CO2]);
				m_SDValue[CH2O]=ntohs(m_SDValue[CH2O]);
				m_SDValue[HUMITY]=ntohs(m_SDValue[HUMITY]);
				m_SDValue[TEMP]=ntohs(m_SDValue[TEMP]);
			
				StringCchPrintfA(csBuf, MAX_PATH, "PM2.5:	%d ug/m3\r\nCO2:	%d ppm\r\nCH2O:	%d mg/m3\r\nTemp:	%f ¡æ\r\nHumity:	%f%%\r\n", 
										m_SDValue[PM25], m_SDValue[CO2],m_SDValue[CH2O],
										((FLOAT)(m_SDValue[TEMP])/10), ((FLOAT)(m_SDValue[HUMITY])/10) );
				str=csBuf;
				SetDlgItemTextA(this->m_hWnd, IDC_EDIT_ORIINFO, str.c_str());

				m_strGSV=csBuf;
			}
			else
			{
				SetDlgItemTextA(this->m_hWnd, IDC_EDIT_ORIINFO, "device no responding.\r\n");
				this->SetBtnStatus(FALSE);
				this->SetInputMacStatus(FALSE);
			}

			//CloseSerialPort(hDevice);
			m_deviceHandle=hDevice;
			bFind=TRUE;
			break;
		}

		CloseSerialPort(hDevice);
	}

	if( bFind)
	{
		pComBox->AddString(wstr.c_str());
		pComBox->SetCurSel(0);
// 		m_wstrDes=L"open serial port ";
// 		m_wstrDes+=wstr.c_str();
// 		m_wstrDes+=L" success";
// 		m_wstrDes+=wstrTmp.c_str();
	}
	else
	{
		SetDlgItemTextA(this->m_hWnd, IDC_EDIT_ORIINFO, "not found device.\r\n");
		this->SetBtnStatus(FALSE);
		this->SetInputMacStatus(FALSE);
	}
}


VOID			CStandardDlg::ReadGSValue(INT nIndex)
{
	CComboBox*	pComBox=(CComboBox*)GetDlgItem(IDC_COMBO_DEVICENAME);
	HANDLE		hDevice=m_deviceHandle;
	CHAR		csBuf[MAX_PATH]={0};
	BYTE		retDatas[SHORTLEN]={0};

	//get standard value
	if( P_GetSDResult(hDevice, /*TP_STAND,*/ retDatas, SHORTLEN))
	{
		//ÉÏ´«Êý¾ÝÎª´ó¶ËµØÖ·Ä£Ê½£¬ÐèÒªÇÐ»»ÖÁÐ¡¶Ë 
		memmove(m_SDMeanValue[nIndex], retDatas+1, sizeof(m_SDValue));
		m_SDMeanValue[nIndex][PM25]=ntohs(m_SDMeanValue[nIndex][PM25]);
		m_SDMeanValue[nIndex][CO2]=ntohs(m_SDMeanValue[nIndex][CO2]);
		m_SDMeanValue[nIndex][CH2O]=ntohs(m_SDMeanValue[nIndex][CH2O]);
		m_SDMeanValue[nIndex][HUMITY]=ntohs(m_SDMeanValue[nIndex][HUMITY]);
		m_SDMeanValue[nIndex][TEMP]=ntohs(m_SDMeanValue[nIndex][TEMP]);

		StringCchPrintfA(csBuf, MAX_PATH, "PM2.5:	%d ug/m3\r\nCO2:	%d ppm\r\nCH2O:	%d mg/m3\r\nTemp:	%f ¡æ\r\nHumity:	%f%%\r\n", 
			m_SDMeanValue[nIndex][PM25], m_SDMeanValue[nIndex][CO2],m_SDMeanValue[nIndex][CH2O],
			((FLOAT)(m_SDMeanValue[nIndex][TEMP])/10), ((FLOAT)(m_SDMeanValue[nIndex][HUMITY])/10) );

		SetDlgItemTextA(this->m_hWnd, IDC_EDIT_ORIINFO, csBuf);

		//
		CF_OutputDBGInfo(__FUNCTION__"...index:%d %s\r\n", nIndex, csBuf);

		m_strGSV=csBuf;
	}

}


VOID			CStandardDlg::SetBtnStatus(BOOL	bEnable)
{
	CButton* pBtn=(CButton*)GetDlgItem(IDC_BUTTON_START);
	CButton* pBtnClear=(CButton*)GetDlgItem(IDC_BTN_CLEAR);

	if( pBtn)
	{
		pBtn->EnableWindow(bEnable);
		pBtnClear->EnableWindow(bEnable);
	}
}

VOID			CStandardDlg::SetInputMacStatus(BOOL bEnable)
{
	CEdit* pEdit=(CEdit*)GetDlgItem(IDC_EDIT_MAC);
	if( pEdit)
	{
		pEdit->EnableWindow(bEnable);
	}
}


BOOL			CStandardDlg::FindKeyStr(string strOri, string strKey, char end,string& strRet)
{
	BOOL				bRet=FALSE;
	size_t nIndexBegin=strOri.find(strKey);

	if( nIndexBegin!=string::npos)
	{
		size_t	nIndexEnd=strOri.find(end, nIndexBegin+strKey.length());

		if( nIndexEnd!=string::npos)
		{
			string	strMac=strOri.substr(nIndexBegin+strKey.length(), nIndexEnd-nIndexBegin-strKey.length());
			CF_trim(strMac);
			strRet=strMac;

			bRet=TRUE;
		}		
	}

	return bRet;
}

BOOL			CStandardDlg::ParseRetDatas(PBYTE pDatas, wstring& wstrMac, MAPAIRSD* pMap, BOOL	bCheckMode)
{
	/*
	[4992] {"p":"udp_enter_mode","param":{"enable":1,"show_screen":1,"breath_swtich":1,"breath_color":0,"macs":[{"mac":"C89346C60721"}]}}
	[4992] {"p":"udp_data","param":{"mac":"C89346C60721","psn":"MJBM16022003052ÿ","mcu_version":"2016012501","wifi_version":"2016012202","voice":76,"pm25":45,"pm2.5_offset_1":0,"pm2.5_offset_2":0,"pm2.5_offset_3":0,"pm2.5_k1":0.661237,"pm2.5_k2":0.661237,"pm2.5_k3":0.661237,"pm2.5_primitive":65,"pm2.5_kalman":68.127192,"pm2.5_base_line":318,"pm2.5_noise":323,"co2_offset":-115,"temp_offset":-2.100000,"humi_offset":0,"co2":541,"temperature":24.820000,"humidity":56.690000,"soc":100,"online":1}}
	*/

	string				strRecv=(PCHAR)pDatas;
	string				strKey;
	MAPAIRSD::iterator	pMapIterator;
	MAPAIRSD::iterator	pMapKey;
	//ÁÙÊ±»º³åÇøµÄÊý¾ÝÊÇ·ñ¶¼ÒÑ¾­Ìî³äÍê±Ï
	int					i=0;
	BOOL				bRet=FALSE;

	//TEST
 	//strRecv="{\"p\":\"udp_data\",\"param\":	{\"mac\":\"C89346C60721\", \"psn\":\"MJBM16022003052\",	\"mcu_version\":\"2015121101\",\"wifi_version\":\"2015121102\",	\"pm2.5\":120,\"co2\":1234,	\"temperature\":21.3,	\"humidity\":27.5,\"co2_offset\":0,	\"pm2.5_offset_1\":1,	\"pm2.5_offset_2\":1,	\"pm2.5_offset_3\":1,	\"pm2.5_k1\":1.0,	\"pm2.5_k2\":2.0,	\"pm2.5_k3\":3.0,	\"pm2.5_primitive\":33,\"pm2.5_kalman\":35.54,	\"voice\":20,	\"soc\":100,\"online\":1,}}";
 
	OutputDebugStringA((PCHAR)pDatas);///

	do 
	{
		///****************MAC*********************/
		strKey="mac\":\"";
		bRet=this->FindKeyStr(strRecv, strKey, '\"', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"not find :%s\r\n", strKey.c_str());
			break;
		}

		wstrMac=CF_A2W(strKey);

		//pMapIterator=pMap->find(wstrMac);
		for(i=0; i<READDATANUM; i++)
		{
			pMapIterator=m_mapReadBuf[i].find(wstrMac);
			if( pMapIterator==m_mapReadBuf[i].end())
			{
				break;
			}

			if( !pMapIterator->second.bStatus)
			{
				break;
			}
		}

		CF_OutputDBGInfo(__FUNCTION__"...%ws, index:%d\r\n", wstrMac.c_str(), i);///

		if( i<READDATANUM && pMapIterator==m_mapReadBuf[i].end())
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not found key in MAPS:%ws\r\n", wstrMac.c_str());
			break;
		}

		//ÁÙÊ±»º³åÇøÄÚµÄ5Ö¡Êý¾ÝÒÑ¾­Ìî³äÍê±Ï£¬¼ÆËãÆäÆ½¾ùÖµ
		if( i>=READDATANUM)
		{
			pMapKey=pMap->find(wstrMac);
			if( pMapKey==pMap->end())
			{
				CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not found key in MAP:%ws\r\n", wstrMac.c_str());
				break;
			}


			//ÒÑ¾­¼ì²â¹ýÁË£¬Ìø¹ý
			if( pMapKey->second.bStatus )
			{
				bRet=FALSE;
				CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"already standardization.\r\n");
				break;
			}

			pMapKey->second.bStatus=TRUE;

			//bCheckMode ÊÇ·ñÊÇÐ£ÑéÄ£Ê½£¬Èç¹ûÊÇ£¬¸ù¾Ý¹æÔò£¬Ö»¶ÁÈ¡Ò»´Î¼´¿É£¬²»ÓÃ¼ÆËãÆ½¾ùÖµ
			if( bCheckMode)
			{
				pMapKey->second.dHumi	=	(m_mapReadBuf[0])[wstrMac].dHumi;
				pMapKey->second.dTemp	=	(m_mapReadBuf[0])[wstrMac].dTemp;
				pMapKey->second.nCo2	=	(m_mapReadBuf[0])[wstrMac].nCo2;
				pMapKey->second.nPM25	=	(m_mapReadBuf[0])[wstrMac].nPM25;
			}
			else
			{
				pMapKey->second.dHumi=(	(m_mapReadBuf[0])[wstrMac].dHumi + (m_mapReadBuf[1])[wstrMac].dHumi +(m_mapReadBuf[2])[wstrMac].dHumi + (m_mapReadBuf[3])[wstrMac].dHumi +(m_mapReadBuf[4])[wstrMac].dHumi)/5.0;
				pMapKey->second.dTemp=(	(m_mapReadBuf[0])[wstrMac].dTemp + (m_mapReadBuf[1])[wstrMac].dTemp +(m_mapReadBuf[2])[wstrMac].dTemp + (m_mapReadBuf[3])[wstrMac].dTemp +(m_mapReadBuf[4])[wstrMac].dTemp)/5.0;
				pMapKey->second.nCo2=(	(m_mapReadBuf[0])[wstrMac].nCo2 + (m_mapReadBuf[1])[wstrMac].nCo2 +(m_mapReadBuf[2])[wstrMac].nCo2 + (m_mapReadBuf[3])[wstrMac].nCo2 +(m_mapReadBuf[4])[wstrMac].nCo2)/5.0;
				pMapKey->second.nPM25=(	(m_mapReadBuf[0])[wstrMac].nPM25 + (m_mapReadBuf[1])[wstrMac].nPM25 +(m_mapReadBuf[2])[wstrMac].nPM25 + (m_mapReadBuf[3])[wstrMac].nPM25 +(m_mapReadBuf[4])[wstrMac].nPM25)/5.0;
			}

			
			pMapKey->second.dHumiOffset=(m_mapReadBuf[0])[wstrMac].dHumiOffset;
			pMapKey->second.dPM25KH=(m_mapReadBuf[0])[wstrMac].dPM25KH;
			pMapKey->second.dPM25KL=(m_mapReadBuf[0])[wstrMac].dPM25KL;
			pMapKey->second.dPM25KM=(m_mapReadBuf[0])[wstrMac].dPM25KM;
			pMapKey->second.dTempOffset=(m_mapReadBuf[0])[wstrMac].dTempOffset;
			pMapKey->second.nCo2Offset=(m_mapReadBuf[0])[wstrMac].nCo2Offset;
			pMapKey->second.nOnline=(m_mapReadBuf[0])[wstrMac].nOnline;
			pMapKey->second.dPM25Mana=(m_mapReadBuf[0])[wstrMac].dPM25Mana;
			pMapKey->second.nPm25BaseLine=(m_mapReadBuf[0])[wstrMac].nPm25BaseLine;
			pMapKey->second.nPm25Noise=(m_mapReadBuf[0])[wstrMac].nPm25Noise;
			pMapKey->second.nPM25OffsetH=(m_mapReadBuf[0])[wstrMac].nPM25OffsetH;
			pMapKey->second.nPM25OffsetL=(m_mapReadBuf[0])[wstrMac].nPM25OffsetL;
			pMapKey->second.nPM25OffsetM=(m_mapReadBuf[0])[wstrMac].nPM25OffsetM;
			pMapKey->second.nPM25Ori=(m_mapReadBuf[0])[wstrMac].nPM25Ori;
			pMapKey->second.nSoc=(m_mapReadBuf[0])[wstrMac].nSoc;
			pMapKey->second.nVoice=(m_mapReadBuf[0])[wstrMac].nVoice;
			pMapKey->second.strMac=(m_mapReadBuf[0])[wstrMac].strMac;
			pMapKey->second.strMcvVer=(m_mapReadBuf[0])[wstrMac].strMcvVer;
			pMapKey->second.strPsn=(m_mapReadBuf[0])[wstrMac].strPsn;
			pMapKey->second.strSend=(m_mapReadBuf[0])[wstrMac].strSend;
			pMapKey->second.strWfVer=(m_mapReadBuf[0])[wstrMac].strWfVer;

			bRet=TRUE;
			break;
		}

		
		//¸úÉè±¸Í¬²½¶ÁÈ¡goldern sample±ê¶¨Öµ
		this->ReadGSValue(i);

		//¿ªÊ¼Ìî³ä
		pMapIterator->second.strMac=strKey;
	

		///****************mcu_version*********************/
		strKey="mcu_version\":\"";
		bRet=this->FindKeyStr(strRecv, strKey, '\"', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			//break;
		}
		pMapIterator->second.strMcvVer=strKey;

		///****************wifi_version*********************/
		strKey="wifi_version\":\"";
		bRet=this->FindKeyStr(strRecv, strKey, '\"', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			//break;
		}
		pMapIterator->second.strWfVer=strKey;

		///****************PSN*********************/
		strKey="psn\":\"";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			//break;
		}
		pMapIterator->second.strPsn=strKey;

		///****************pm2.5*********************/
		strKey="pm25\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			break;
		}
		pMapIterator->second.nPM25=strtol(strKey.c_str(), NULL, 10);
		//CF_OutputDBGInfo("...PM25:%d\r\n", pMapIterator->second.nPM25);///

		///****************pm2.5´«¸ÐÆ÷ÉÏ´«Ô­Ê¼Êý¾Ý*********************/
		strKey="pm2.5_primitive\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			break;
		}
		pMapIterator->second.nPM25Ori=strtol(strKey.c_str(), NULL, 10);
		//CF_OutputDBGInfo("...PM25 ori:%d\r\n", pMapIterator->second.nPM25Ori);///

		///****************pm2.5¾­¹ýkalmanÂË²¨ºóÊý¾Ý*********************/
		strKey="pm2.5_kalman\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			break;
		}
		pMapIterator->second.dPM25Mana=strtod(strKey.c_str(), NULL);
		//CF_OutputDBGInfo("...PM25 mana:%f\r\n", pMapIterator->second.dPM25Mana);///

		///****************pm2.5µÍ¶ÎÆ«ÒÆÖµ*********************/
		strKey="pm2.5_offset_1\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			break;
		}
		pMapIterator->second.nPM25OffsetL=strtol(strKey.c_str(), NULL, 10);
		//CF_OutputDBGInfo("...PM25 offset1:%d\r\n", pMapIterator->second.nPM25OffsetL);///

		///****************pm2.5ÖÐ¶ÎÆ«ÒÆÖµ*********************/
		strKey="pm2.5_offset_2\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			break;
		}
		pMapIterator->second.nPM25OffsetM=strtol(strKey.c_str(), NULL, 10);
		//CF_OutputDBGInfo("...PM25 offset2:%d\r\n", pMapIterator->second.nPM25OffsetM);///

		///****************pm2.5¸ß¶ÎÆ«ÒÆÖµ*********************/
		strKey="pm2.5_offset_3\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			break;
		}
		pMapIterator->second.nPM25OffsetH=strtol(strKey.c_str(), NULL, 10);
		//CF_OutputDBGInfo("...PM25 offset3:%d\r\n", pMapIterator->second.nPM25OffsetH);///

		///****************pm2.5µÍ¶ÎKÖµ*********************/
		strKey="pm2.5_k1\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			break;
		}
		pMapIterator->second.dPM25KL=strtod(strKey.c_str(), NULL);
		//CF_OutputDBGInfo("...PM25 K1:%f\r\n", pMapIterator->second.dPM25KL);///

		///****************pm2.5ÖÐ¶ÎKÖµ*********************/
		strKey="pm2.5_k2\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			break;
		}
		pMapIterator->second.dPM25KM=strtod(strKey.c_str(), NULL);
		//CF_OutputDBGInfo("...PM25 k2:%f\r\n", pMapIterator->second.dPM25KM);///

		///****************pm2.5¸ß¶ÎKÖµ*********************/
		strKey="pm2.5_k3\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			break;
		}
		pMapIterator->second.dPM25KH=strtod(strKey.c_str(), NULL);
		//CF_OutputDBGInfo("...PM25 k3:%f\r\n", pMapIterator->second.dPM25KH);///

		///****************pm2.5 base line*********************/
		strKey="pm2.5_base_line\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			break;
		}
		pMapIterator->second.nPm25BaseLine=strtol(strKey.c_str(), NULL, 10);
		//CF_OutputDBGInfo("...PM25 base line:%f\r\n", pMapIterator->second.dPM25KH);///

		///****************pm2.5 noise*********************/
		strKey="pm2.5_noise\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			break;
		}
		pMapIterator->second.nPm25Noise=strtol(strKey.c_str(), NULL, 10);
		//CF_OutputDBGInfo("...PM25 noise:%f\r\n", pMapIterator->second.dPM25KH);///

		///****************co2*********************/
		strKey="co2\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			break;
		}
		pMapIterator->second.nCo2=strtol(strKey.c_str(), NULL, 10);
		//CF_OutputDBGInfo("...co2:%d\r\n", pMapIterator->second.nCo2);///

		///****************co2Æ«ÒÆÖµ*********************/
		strKey="co2_offset\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			break;
		}
		pMapIterator->second.nCo2Offset=strtol(strKey.c_str(), NULL, 10);
		//CF_OutputDBGInfo("...co2 offset:%d\r\n", pMapIterator->second.nCo2Offset);///
		

		///****************ÎÂ¶È*********************/
		strKey="temperature\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			break;
		}
		pMapIterator->second.dTemp=strtod(strKey.c_str(), NULL);
		//CF_OutputDBGInfo("...tmperature:%f\r\n", pMapIterator->second.dTemp);///

		///****************ÎÂ¶ÈÆ«ÒÆ*********************/
		strKey="temp_offset\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			break;
		}
		pMapIterator->second.dTempOffset=strtod(strKey.c_str(), NULL);
		//CF_OutputDBGInfo("...temp offset:%f\r\n", pMapIterator->second.dTempOffset);///

		///****************Êª¶È*********************/
		strKey="humidity\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			break;
		}
		pMapIterator->second.dHumi=strtod(strKey.c_str(), NULL);
		//CF_OutputDBGInfo("...Humi:%f\r\n", pMapIterator->second.dHumi);///

		///****************Êª¶ÈÆ«ÒÆ*********************/
		strKey="humi_offset\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			break;
		}
		pMapIterator->second.dHumiOffset=strtod(strKey.c_str(), NULL);
		//CF_OutputDBGInfo("...Humi offset:%f\r\n", pMapIterator->second.dHumiOffset);///


		///****************voice*********************/
		strKey="voice\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			break;
		}
		pMapIterator->second.nVoice=strtol(strKey.c_str(), NULL, 10);
		//CF_OutputDBGInfo("...voice:%d\r\n", pMapIterator->second.nVoice);///


		///****************Ê£ÓàµçÁ¿*********************/
		strKey="soc\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			break;
		}
		pMapIterator->second.nSoc=strtol(strKey.c_str(), NULL, 10);
		//CF_OutputDBGInfo("...soc:%d\r\n", pMapIterator->second.nSoc);///

		///****************Online*********************/
		strKey="online\":";
		bRet=this->FindKeyStr(strRecv, strKey, '}', strKey);//×îºóÒ»¸ö£¬·Ö¸ô·ûÎª'}'
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			break;
		}
		pMapIterator->second.nOnline=strtol(strKey.c_str(), NULL, 10);
		//CF_OutputDBGInfo("...online:%d\r\n", pMapIterator->second.nOnline);///


		//OK
		pMapIterator->second.bStatus=TRUE;
		bRet=FALSE;

	} while (FALSE);


	return bRet;
}


BOOL			CStandardDlg::IsRecvAllDeviceData(MAPAIRSD*	mapV)
{
	BOOL		bRet=FALSE;

	if( mapV->size() > 0 )
	{
		bRet=TRUE;
	}

	for(MAPAIRSD::iterator p=mapV->begin(); p!=mapV->end(); p++)
	{
		if( !(p->second.bStatus) )
		{
			bRet=FALSE;
			break;
		}
	}

	return  bRet;
}

VOID			CStandardDlg::EnunAllDataAndSD()
{
	/*
	{"p":"udp_set_config","param":{"temp_offset":0.0,"humi_offset":0.0,"pm2.5_k1":1.0,"pm2.5_k2":1.0,"pm2.5_k3":1.0,"pm2.5_offset_1":0,
	"pm2.5_offset_2":0,"pm2.5_offset_3":0,"co2_offset":0,"breath_swtich":1,"breath_color":1,"voice":20,"psn":"MJBM15120301817",
	"err":123,"macs":[{"mac":"C893464E2520"},{"mac":"C893464E0F26"},{"mac":"C893464E29D7"},{"mac":"C893464E2734"}]}}
	*/
	char csBuf[MAX_PATH]={0};
	for(MAPAIRSD::iterator p=this->m_mapAirSD.begin(); p!=this->m_mapAirSD.end(); p++)
	{
		p->second.dPM25KH=0.0;
		p->second.dPM25KL=0.0;
		p->second.dPM25KM=0.0;

		p->second.strSend="{\"p\":\"udp_set_config\",\"param\":{\"temp_offset\":";


		/*ÎÂ¶ÈÆ«ÒÆÖµ£º-2.10 ÉãÊÏ¶È    Êª¶ÈÆ«ÒÆÁ¿£º+10.0*/
		//temp_offset
		StringCchPrintfA(csBuf, MAX_PATH, "%.1f", -2.1);
		p->second.strSend+=csBuf;


		//humi_offset
		p->second.strSend+=",\"humi_offset\":";
		StringCchPrintfA(csBuf, MAX_PATH, "%.1f", 10.0);
		p->second.strSend+=csBuf;


		/*
		PS : PM25´«¸ÐÆ÷µÄGold SampleµÄÖµk1,  ¿ÕÆø¹ûÊµ²âÖµk2(kaman)
		»Ø´«µÄÖµ Kx= k1/k2  
		*/
		//pm2.5_k1
		double	d=( (double)(this->m_SDValue[PM25]) ) / (p->second.dPM25Mana);

		p->second.dPM25KL=d;
		p->second.dPM25KM=d;
		p->second.dPM25KH=d;

		CF_OutputDBGInfo("%d %f %f \r\n", m_SDValue[PM25], p->second.dPM25Mana, d);///
		p->second.strSend+=",\"pm2.5_k1\":";
		StringCchPrintfA(csBuf, MAX_PATH, "%.3f",d);
		p->second.strSend+=csBuf;

		//pm2.5_k2
		p->second.strSend+=",\"pm2.5_k2\":";
		StringCchPrintfA(csBuf, MAX_PATH, "%.3f", d);
		p->second.strSend+=csBuf;

		//pm2.5_k3
		p->second.strSend+=",\"pm2.5_k3\":";
		StringCchPrintfA(csBuf, MAX_PATH, "%.3f", d);
		p->second.strSend+=csBuf;

		//pm2.5_offset_1
		p->second.strSend+=",\"pm2.5_offset_1\":";
		StringCchPrintfA(csBuf, MAX_PATH, "%d", p->second.nPM25OffsetL);
		p->second.strSend+=csBuf;

		//pm2.5_offset_2
		p->second.strSend+=",\"pm2.5_offset_2\":";
		StringCchPrintfA(csBuf, MAX_PATH, "%d", p->second.nPM25OffsetM);
		p->second.strSend+=csBuf;

		//pm2.5_offset_3
		p->second.strSend+=",\"pm2.5_offset_3\":";
		StringCchPrintfA(csBuf, MAX_PATH, "%d", p->second.nPM25OffsetH);
		p->second.strSend+=csBuf;

		/*
		CO2´«¸ÐÆ÷µÄGold SampleµÄÖµk1,  ¿ÕÆø¹ûÊµ²âÖµk2
		»Ø´«µÄÖµ CO2_Offset = k2-k1  
		*/
		//p->second.nCo2Offset=m_SDValue[CO2]-p->second.nCo2;
		//co2_offset
		p->second.strSend+=",\"co2_offset\":";
		StringCchPrintfA(csBuf, MAX_PATH, "%d", p->second.nCo2Offset);
		p->second.strSend+=csBuf;

		//breath_swtich 0 ¹Ø±ÕºôÎüµÆ  1 ´ò¿ªºôÎüµÆ
		p->second.strSend+=",\"breath_swtich\":";
		StringCchPrintfA(csBuf, MAX_PATH, "%d", 1);
		p->second.strSend+=csBuf;

		//breath_color 0---×ÏÉ«   1---ÂÌÉ« 2---ºìÉ« 3---»ÆÉ« 4---À¶É« 5---ÇàÉ«	
		p->second.strSend+=",\"breath_color\":";
		StringCchPrintfA(csBuf, MAX_PATH, "%d", 3);
		p->second.strSend+=csBuf;

		//voice
		p->second.strSend+=",\"voice\":";
		StringCchPrintfA(csBuf, MAX_PATH, "%d", p->second.nVoice);
		p->second.strSend+=csBuf;

		//PSN
		p->second.strSend+=",\"psn\":\"";
		p->second.strSend+=p->second.strPsn;


		//err 0x0000----------ÎÞ´íÎó
		p->second.strSend+=",\"err\":";
		StringCchPrintfA(csBuf, MAX_PATH, "%d", 0);
		p->second.strSend+=csBuf;

		//PSN
		p->second.strSend+=",\"macs\":[{\"mac\":\"";
		p->second.strSend+=p->second.strMac;
		p->second.strSend+="\"}]}}";
	}
}

VOID			CStandardDlg::SaveJYFormationFile(MAPAIRSD::iterator p, string strResult, BOOL bPass)
{
	/*
		TestResult|F-µÆ°å¡¢wifi¡¢Ö÷°å¡¢±ê¶¨
		TesterDesc|µÆ°å¡¢wifi¡¢Ö÷°å¡¢±ê¶¨²âÊÔÕ¾
		TesterID|led¡¢wifi¡¢mb£¬sd
		PN|sn
		Line|1
		OperatorNum|0001
		TestDate|time
		Barcode|sn
		Detail item
	*/

	string			strJYFormatFileContent;
	CHAR			csBuf[MAX_PATH]={0};
	SYSTEMTIME		st;

	GetLocalTime(&st);
	StringCchPrintfA(csBuf, MAX_PATH, "%.4d-%.2d-%.2d %.2d:%.2d:%.2d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

	//if( p)
	{
		if( bPass)
		{
			strJYFormatFileContent="TestResult|P\r\n";
		}
		else
		{
			strJYFormatFileContent="TestResult|F\r\n";
		}

		strJYFormatFileContent+="TesterDesc|±ê¶¨¹¤Õ¾\r\nTesterID|0001\r\n";
		strJYFormatFileContent+="PN|";
		strJYFormatFileContent+=p->second.strPsn;
		strJYFormatFileContent+="Line|0001\r\nOperatorNum|0001\r\nTestDate|";
		strJYFormatFileContent+=csBuf;
		strJYFormatFileContent+="\r\n";
		strJYFormatFileContent+="Barcode|";
		strJYFormatFileContent+=p->second.strPsn;
		strJYFormatFileContent+="Detail item\r\n";
		strJYFormatFileContent+=strResult;


		CFileHelper::_()->SaveJYLogInfo(TP_STAND, AIRUS1S, (PCHAR)strJYFormatFileContent.c_str(), strJYFormatFileContent.length()+1);
	}
}

BOOL 			CStandardDlg::CheckSDDatas()
{
	INT			nErr=0;
	BOOL		bRet=FALSE;
	WCHAR		wcsBuf[MAX_PATH]={0};
	wstring		wstrDes;

	//´æ´¢½òÑÇ¸ñÊ½ÎÄ¼þ
	BOOL			bJYPass=FALSE;
	string			strJYTemp;
	
	for(MAPAIRSD::iterator p=m_mapRetAirSD.begin(); p!=m_mapRetAirSD.end(); p++)
	{
		nErr=0;
		wstrDes=L"-->Check(";
		wstrDes+=p->first;
		wstrDes+=L")";
		this->SetShowInfo(wstrDes.c_str());


// 		if(  abs(p->second.nPm25BaseLine-p->second.nPm25Noise) > 60 )
// 		{
// 			StringCchPrintfW(wcsBuf, MAX_PATH, L"%s, PM25 base line:%d, PM25 noise:%d \r\n", 
// 											L" PM2.5»ùÏßÆ«ÒÆ¹ý´ó(60)", p->second.nPm25BaseLine, p->second.nPm25Noise);
// 			this->SetShowInfo(wcsBuf);
// 			nErr+=0x01;
// 		}

		
		if( abs( ( (double)(p->second.nPM25)-((double)m_SDValue[PM25]) ) / ((double)m_SDValue[PM25]) ) >0.15 )
		{
			StringCchPrintfW(wcsBuf, MAX_PATH, L"%s, PM25 mana:%f,	ref PM25:%d \r\n", 
											L" PM2.5¶ÁÊýÎó²î´ó(15%)", p->second.dPM25Mana,  m_SDValue[PM25] );
			this->SetShowInfo(wcsBuf);
			nErr+=0x02;

			strJYTemp="PM2.5¶ÁÊýÎó²î|0|0|0|Fail\r\n";
		}



		if( p->second.dPM25Mana==0)
		{
			StringCchPrintfW(wcsBuf, MAX_PATH, L"%s, PM25 mana:%f \r\n", 
												L" PM2.5¶ÁÊýÎªÁã", p->second.dPM25Mana);
			this->SetShowInfo(wcsBuf);
			nErr+=0x04;

			strJYTemp="PM2.5¶ÁÊýÎªÁã|0|0|0|Fail\r\n";
		}

		if( p->second.dPM25KL<0.2 ||
			p->second.dPM25KL>5.0)
		{
			StringCchPrintfW(wcsBuf, MAX_PATH, L"%s, PM25 K:%f \r\n", 
				L" ±ê¶¨ÏµÊýKÖµÒì³££¨0.2<>5£©", p->second.dPM25KL);
			this->SetShowInfo(wcsBuf);
			nErr+=0x00002000;

			strJYTemp="±ê¶¨ÏµÊýKÖµ|0|0|0|Fail\r\n";
		}

		
		if( abs(m_SDValue[CO2] - p->second.nCo2) > 300)
		{
			StringCchPrintfW(wcsBuf, MAX_PATH, L"%s, co2:%d, ref co2:%d \r\n", 
												L" CO2¶ÁÊýÎó²î´ó(300)", p->second.nCo2, m_SDValue[CO2] );
			this->SetShowInfo(wcsBuf);
			nErr+=0x10;

			strJYTemp="CO2¶ÁÊýÎó²î|0|0|0|Fail\r\n";
		}

// 		if( p->second.nCo2==m_mapAirSD[p->first].nCo2)
// 		{
// 			StringCchPrintfW(wcsBuf, MAX_PATH, L"%s, co2:%d, last co2:%d \r\n", 
// 										L" CO2¶ÁÊýÎÞ±ä»¯", p->second.nCo2, m_mapAirSD[p->first].nCo2 );
// 			this->SetShowInfo(wcsBuf);
// 			nErr+=0x20;
// 		}


		double	dTmp=((double)(m_SDValue[TEMP]))/10;
		if( abs(p->second.dTemp- dTmp) > 5)
		{
			StringCchPrintfW(wcsBuf, MAX_PATH, L"%s, temp:%f, ref temp:%f \r\n", 
										L" ÎÂ¶È¶ÁÊýÎó²î´ó(5)", p->second.dTemp,  dTmp);
			this->SetShowInfo(wcsBuf);
			nErr+=0x40;

			strJYTemp="ÎÂ¶È¶ÁÊýÎó²î|0|0|0|Fail\r\n";
		}

		double	dHumi=((double)m_SDValue[HUMITY])/10;
		if(  abs((p->second.dHumi-dHumi)) > 20)
		{
			StringCchPrintfW(wcsBuf, MAX_PATH, L"%s, humi:%f, ref humi:%f \r\n", 
									L" Êª¶È¶ÁÊýÎó²î´ó", p->second.dHumi, dHumi );
			this->SetShowInfo(wcsBuf);
			nErr+=0x80;

			strJYTemp="Êª¶È¶ÁÊýÎó²î|0|0|0|Fail\r\n";
		}

		{
			StringCchPrintfW(wcsBuf, MAX_PATH, L" errID:%d\r\n",  nErr );
			this->SetShowInfo(wcsBuf);
		}


		string strSend=m_mapAirSD[p->first].strSend;


		//ÉèÖÃ´íÎóÂë
		char	csBuf[MAX_PATH]={0};
		StringCchPrintfA(csBuf, MAX_PATH, "%d", nErr);

		string strKey=",\"err\":";
		size_t pos=strSend.find(strKey);
		
		if( pos!=string::npos)
		{
			size_t end=strSend.find(',', pos+1);
			if( end!=string::npos)
			{
				strSend.replace(pos+strKey.length(), end-pos-strKey.length(), csBuf);
			}
		}

		//¸ù¾ÝerrÉèÖÃºôÎüµÆÑÕÉ«
		BYTE	bErrID=0;
		if( !nErr)
		{
			bErrID=1;//ºôÎüµÆÂÌÉ«
			bJYPass=TRUE;

			strJYTemp="PM2.5¶ÁÊýÎó²î|0|0|0|True\r\n";
			strJYTemp="PM2.5¶ÁÊýÎªÁã|0|0|0|True\r\n";
			strJYTemp="±ê¶¨ÏµÊýKÖµ|0|0|0|True\r\n";
			strJYTemp="CO2¶ÁÊýÎó²î|0|0|0|True\r\n";
			strJYTemp="ÎÂ¶È¶ÁÊýÎó²î|0|0|0|True\r\n";
			strJYTemp="Êª¶È¶ÁÊýÎó²î|0|0|0|True\r\n";
		}
		else
		{
			bErrID=2;//ºôÎüµÆºìÉ«
			bJYPass=FALSE;
		}
		StringCchPrintfA(csBuf, MAX_PATH, "%d", bErrID);
		strKey=",\"breath_color\":";
		pos=strSend.find(strKey);
		if( pos!=string::npos)
		{
			size_t end=strSend.find(',', pos+1);
			if( end!=string::npos)
			{
				strSend.replace(pos+strKey.length(), end-pos-strKey.length(), csBuf);
			}
		}
	
		p->second.strSend=strSend;

		//save as JY formation
		this->SaveJYFormationFile(p, strJYTemp, bJYPass);
	}

	return bRet;
}


VOID			CStandardDlg::SetShowInfo(LPCTSTR wcsBuf, BOOL	bSpecial)
{
	string		str;
	wstring		wstr;
	if( wcsBuf)
	{
		if(!bSpecial)
		{
			m_listReport->InsertItem(m_nListReportIndex++, (LPCTSTR)wcsBuf);
		}

		wstr=wcsBuf;

		str ="------------------------------------------------------------------------------\r\n";
		//str=CF_W2A(wstr);
		str+=CF_W2AEx(wstr);
		str+="\r\n";
		str+="------------------------------------------------------------------------------\r\n";
	
		OutputDebugStringA(str.c_str());///
		m_strFileContent+=str;
	}
}


BOOL			CStandardDlg::SendUDPEntrySD(BOOL	bEntry)
{
	BOOL		bRet=FALSE;
	WCHAR		wcsBuf[MAX_PATH]={0};
	wstring		wstrDes;
	string		strSend;
	/*
		{"p":"udp_enter_mode","param":{"enable":1,"show_screen":1,"breath_swtich":1,"breath_color":0,"macs":
		[
		{"mac":"C893464E2731"},
		{"mac":"C893464E2520"},
		{"mac":"C893464E29D7"},
		{"mac":"C893464E0F26"}
		]}}
		*/

	if( bEntry )
	{
		wstrDes=L"{\"p\":\"udp_enter_mode\",\"param\":{\"enable\":1,\"show_screen\":1,\"breath_swtich\":1,\"breath_color\":0,\"macs\":[";
	}
	else
	{
		wstrDes=L"{\"p\":\"udp_enter_mode\",\"param\":{\"enable\":0,\"show_screen\":1,\"breath_swtich\":1,\"breath_color\":0,\"macs\":[";
	}


	int nCount=1;
	for(MAPAIRSD::iterator p=m_mapAirSD.begin(); p!=m_mapAirSD.end(); p++)
	{
		wstrDes+=L"{\"mac\":\"";
		wstrDes+=p->first.c_str();
		wstrDes+=L"\"}";
		if( (++nCount)<=m_mapAirSD.size())
		{
			wstrDes+=L",";
		}
	}
	wstrDes+=L"]}}";

	strSend=CF_W2A(wstrDes);
	OutputDebugStringA(strSend.c_str());///


	//****************UDP boardcast
	for(int i=0; i<SD_MAXUPDBOARCOSTNUM; i++)
	{
		//send
		bRet=UDP_SendBoardcastUsed((PBYTE)strSend.c_str(), strSend.length(), SD_UDPPORTSEND);
		if( !bRet )
		{
			StringCchPrintfW(wcsBuf, MAX_PATH, L"Send enter setting mode command err(ID:0x%x), index:%d.\r\n", GetLastError(), i );
			SetShowInfo(wcsBuf);
		}
		else
		{
			SetShowInfo(L"Send enter setting mode command(about 30s).");
			SetShowInfo(wstrDes.c_str(), !this->m_isShow);
			break;
		}
	}
				
	return	bRet;
}



BOOL			CStandardDlg::RecvUDPDatas(MAPAIRSD*	mapV, BOOL	bCheckMode)
{
	BOOL		bRet=FALSE;
	DWORD		dwElapse=0;
	DWORD		dwBegin=0;
	wstring		wstrDes;
	wstring		wstrMacS;
	string		strRecv;
	PBYTE		pRecvData=new BYTE[SD_UDPRECVMAXLEN];	

	
	do 
	{		
		if( !pRecvData)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...allocate memory err(errID:0x%x).\r\n", GetLastError() );
			m_listReport->SetTextColor(FAILED_COLOR);
			SetShowInfo(L"allocate memory err.");
			break;
		}

		ZeroMemory(pRecvData, SD_UDPRECVMAXLEN);

		//¶ÁÈ¡UDP¹ã²¥Öµ£¬ÅÐ¶ÏËùÓÐMAC¾ùÒÑ·µ»ØÊý¾Ý£¬³¬Ê±ÉèÖÃÎª UDPRECVMAXTIMEVALUE*n
		dwElapse=UDPRECVMAXTIMEVALUE*m_mapAirSD.size();
		dwBegin=GetTickCount();

		while(1)
		{
			//recv, timeout 10s
			if( !UDP_RecvBoardcastUsed(pRecvData, SD_UDPRECVMAXLEN, SD_UDPPORTRECV))
			{
				CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"Recv UDP boardcast err(ID:0x%x).\r\n", GetLastError() );
				m_listReport->SetTextColor(FAILED_COLOR);
				SetShowInfo(L"½ÓÊÕÊý¾ÝÊ§°Ü .");
				break;
			}

			if( (GetTickCount()-dwBegin)>dwElapse )
			{
				m_listReport->SetTextColor(FAILED_COLOR);
				SetShowInfo(L"³¬Ê±£¡ÔÚ¹æ¶¨Ê±¼äÄÚ£¬²»ÊÇËùÓÐÉè±¸¶¼·µ»ØÊý¾Ý .");
				break;
			}

			//unchecked
			if( ParseRetDatas(pRecvData, wstrDes, mapV, bCheckMode) )
			{
				wstrMacS=L"--->recv(";
				wstrMacS+=wstrDes.c_str();
				wstrMacS+=L")";
				SetShowInfo(wstrMacS.c_str());
				strRecv=(PCHAR)pRecvData;
				wstrDes=CF_A2W(strRecv);
				SetShowInfo(wstrDes.c_str(), !m_isShow);


				if( IsRecvAllDeviceData(mapV) )
				{
					bRet=TRUE;
					break;
				}
			}	
		}

	} while (0);


	if( pRecvData)
	{
		delete[] pRecvData;
	}

	return	bRet;
}

VOID			CStandardDlg::CalGSMeanValue()
{
	CHAR	csBuf[MAX_PATH]={0};

	//for(int i=0; i<READDATANUM; i++)
	{
		m_SDValue[PM25]=0;
		m_SDValue[CO2]=0;
		m_SDValue[CH2O]=0;
		m_SDValue[TEMP]=0;
		m_SDValue[HUMITY]=0;
	}

	for(int i=0; i<READDATANUM; i++)
	{
		m_SDValue[PM25]+=m_SDMeanValue[i][PM25];
		m_SDValue[CO2]+=m_SDMeanValue[i][CO2];
		m_SDValue[CH2O]+=m_SDMeanValue[i][CH2O];
		m_SDValue[TEMP]+=m_SDMeanValue[i][TEMP];
		m_SDValue[HUMITY]+=m_SDMeanValue[i][HUMITY];
	}
	
	
	m_SDValue[PM25]/=5;
	m_SDValue[CO2]/=5;
	m_SDValue[CH2O]/=5;
	m_SDValue[TEMP]/=5;
	m_SDValue[HUMITY]/=5;

	StringCchPrintfA(csBuf, MAX_PATH, "PM2.5:	%d ug/m3\r\nCO2:	%d ppm\r\nCH2O:	%d mg/m3\r\nTemp:	%f ¡æ\r\nHumity:	%f%%\r\n", 
						m_SDValue[PM25], m_SDValue[CO2],m_SDValue[CH2O],
						((FLOAT)(m_SDValue[TEMP])/10), ((FLOAT)(m_SDValue[HUMITY])/10) );

	//SetDlgItemTextA(this->m_hWnd, IDC_EDIT_ORIINFO, csBuf);
	string	str=csBuf;
	SetShowInfo((LPCTSTR)CF_A2WEx(str).c_str());
}


VOID			CStandardDlg::ForamtionMaxStr(wstring& wstr)
{
	if( wstr.length()>20 )
	{
		wstring	strMac=wstr;
		WCHAR	wsMacBuf[MAX_PATH]={0};
		wsMacBuf[0]=strMac.at(4);
		wsMacBuf[1]=strMac.at(5);

		wsMacBuf[2]=strMac.at(7);
		wsMacBuf[3]=strMac.at(8);

		wsMacBuf[4]=strMac.at(10);
		wsMacBuf[5]=strMac.at(11);

		wsMacBuf[6]=strMac.at(13);
		wsMacBuf[7]=strMac.at(14);

		wsMacBuf[8]=strMac.at(16);
		wsMacBuf[9]=strMac.at(17);

		wsMacBuf[10]=strMac.at(19);
		wsMacBuf[11]=strMac.at(20);
		wsMacBuf[12]=L'\0';

		wstr=wsMacBuf;
	}
}

unsigned __stdcall _ThreadForCheck(LPVOID p)
{
	CStandardDlg* pThis=(CStandardDlg*)p;
	
	WCHAR	wcsBuf[1024]={0};
	wstring	wstrDes;
	wstring	wstrMacS;

	AIRSD	as;

	//recv data
	string	strRecv;

	//¼ÆÊ±Í³¼Æ
	DWORD	dwElapse=0;

	//set report list index
	pThis->m_nListReportIndex=0;

	BOOL	bSuccess=FALSE;

	pThis->SetBtnStatus(FALSE);

	pThis->m_listReport->SetTextColor(LISTREPORT_COLOR);		
	GetDlgItemTextW(pThis->m_hWnd, IDC_EDIT_MAC, wcsBuf, 1000);
	pThis->m_listReport->DeleteAllItems();
	
	pThis->m_mapAirSD.clear();
	pThis->m_mapRetAirSD.clear();
	for(int i=0; i<READDATANUM; i++)
	{
		pThis->m_mapReadBuf[i].clear();
	}
	
	pThis->m_strFileContent.clear();
	
	pThis->m_strFileContent=pThis->m_strGSV;

	as.bStatus=FALSE;
	
	do 
	{
		//parse macS
		wstrMacS=wcsBuf;
	
		if( wstrMacS.length()>0)
		{
			size_t	pos=0;
			size_t	begin=0;
			wstrMacS+=L"\r\n";

			while(1)
			{
				pos=wstrMacS.find(L"\r\n", pos);
				if( pos!=wstring::npos)
				{
					wstrDes=wstrMacS.substr(begin, pos-begin);

					pThis->ForamtionMaxStr(wstrDes);
					if( wstrDes.length()==SD_MACADDRLEN)
					{
						pThis->m_mapAirSD.insert(std::pair<wstring, AIRSD>(wstrDes, as));
						pThis->m_mapRetAirSD.insert(std::pair<wstring, AIRSD>(wstrDes, as));

						for(int i=0; i<READDATANUM; i++)
						{
							pThis->m_mapReadBuf[i].insert(std::pair<wstring, AIRSD>(wstrDes, as));
						}
					}
					pos+=2;
					begin=pos;
				}
				else
				{
					break;
				}
			}
		}

		if( pThis->m_mapAirSD.size() <=0 )
		{
			pThis->m_listReport->SetTextColor(FAILED_COLOR);
			pThis->SetShowInfo(L"no found mac address.");
			break;
		}	
		
		//end parse


		//****************UDP boardcast
		//send
		if( !pThis->SendUDPEntrySD(TRUE) )
		{
			break;
		}

		Sleep(SENDSDCOMMANDSLEEP);
		
		//recv
		if( !pThis->RecvUDPDatas(&pThis->m_mapAirSD) )
		{
			break;
		}


		//¼ÆËãgoldern sampleµÄÆ½¾ùÖµ
		pThis->CalGSMeanValue();

		///////////////////
		//MessageBoxA(pThis->m_hWnd, "ok", "ok", MB_YESNO);
		///////////////////

		//**********************send sd data
		pThis->EnunAllDataAndSD();

		for(MAPAIRSD::iterator p=pThis->m_mapAirSD.begin(); p!=pThis->m_mapAirSD.end(); p++)
		{
			wstrMacS=L"--->send(";
			wstrMacS+=CF_A2W(p->second.strMac).c_str();
			wstrMacS+=L")";

			OutputDebugStringA(p->second.strSend.c_str());///////

			pThis->SetShowInfo(wstrMacS.c_str());
			pThis->SetShowInfo(CF_A2W(p->second.strSend).c_str(), !pThis->m_isShow);


			for(int r=0; r<SENDSDCOMMANDNUM; r++)
			{
				if( !UDP_SendBoardcastUsed((PBYTE)p->second.strSend.c_str(), p->second.strSend.length(), SD_UDPPORTSEND) )
				{
					CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"Send UDP boardcast err(ID:0x%x).\r\n", GetLastError() );
					//break;
				}

				Sleep(200);
			}
		}


		///////////////////
		//MessageBoxA(pThis->m_hWnd, "ok", "ok", MB_YESNO);
		///////////////////


		//**************************check data
		//½«ÉÏ´ÎµÄ»º³åÇøÖÃFALSE£¬Ö»ÒªÇó¶ÁÈ¡Ò»´Î
		for(MAPAIRSD::iterator p=(pThis->m_mapReadBuf[0]).begin(); p!=pThis->m_mapReadBuf[0].end(); p++)
		{
			p->second.bStatus=FALSE;
		}
		//recv
		if( !pThis->RecvUDPDatas(&pThis->m_mapRetAirSD, TRUE) )
		{
			break;
		}

		printfMap(pThis->m_mapAirSD);
		printfMap(pThis->m_mapRetAirSD);

		pThis->CheckSDDatas();
		for(MAPAIRSD::iterator p=pThis->m_mapRetAirSD.begin(); p!=pThis->m_mapRetAirSD.end(); p++)
		{
			wstrMacS=L"--->send(";
			wstrMacS+=CF_A2W(p->second.strMac).c_str();
			wstrMacS+=L")";

			OutputDebugStringA(p->second.strSend.c_str());///////

			pThis->SetShowInfo(wstrMacS.c_str());
			pThis->SetShowInfo(CF_A2W(p->second.strSend).c_str(), !pThis->m_isShow);

			for(int r=0; r<SENDSDCOMMANDNUM; r++)
			{
				if( !UDP_SendBoardcastUsed((PBYTE)p->second.strSend.c_str(), p->second.strSend.length(), SD_UDPPORTSEND) )
				{
					CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"Send UDP boardcast err(ID:0x%x).\r\n", GetLastError() );
					//break;
				}

				Sleep(200);
			}
		}

		pThis->SetShowInfo(L"±ê¶¨½áÊø.");
		
	} while (0);
	
	
	pThis->SetBtnStatus(TRUE);

	CFileHelper::_()->SaveLogInfo(TP_STAND, (PCHAR)pThis->m_strFileContent.c_str(), pThis->m_strFileContent.length()+1);
	OutputDebugStringA(__FUNCTION__"...thread end.\r\n");

	return 1;
}

void CStandardDlg::OnBnClickedButtonStart()
{
	CloseHandle((HANDLE)_beginthreadex(NULL, NULL, _ThreadForCheck, this, NULL, NULL) );
}


HBRUSH CStandardDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	if( pWnd->GetDlgCtrlID()==IDC_EDIT_ORIINFO)
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(21,163,21));
	}

	if( pWnd->GetDlgCtrlID()==IDC_STATIC_MACHINT)
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(21,21,163));
	}
	return hbr;
}


void CStandardDlg::OnEnChangeEditMac()
{
	CEdit*	edit=(CEdit*)GetDlgItem(IDC_EDIT_MAC);
	WCHAR	wcsBuf[1024]={0};

	if(edit)
	{
		edit->GetWindowTextW(wcsBuf, 1024);
		if( wcslen(wcsBuf)>0)
		{
			this->SetBtnStatus(TRUE);
		}
		else
		{
			this->SetBtnStatus(FALSE);
		}
	}
}


BOOL CStandardDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ÔÚ´ËÌí¼Ó×¨ÓÃ´úÂëºÍ/»òµ÷ÓÃ»ùÀà
	if( pMsg->message==WM_KEYDOWN)
	{
		if( pMsg->wParam==VK_ESCAPE || pMsg->wParam==VK_TAB) 
		{
			return TRUE;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CStandardDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ÔÚ´ËÌí¼ÓÏûÏ¢´¦Àí³ÌÐò´úÂëºÍ/»òµ÷ÓÃÄ¬ÈÏÖµ
	if( nIDEvent==1)
	{
		wstring		wstr;
		VECSTR		vs;
		CComboBox*	pComBox=(CComboBox*)GetDlgItem(IDC_COMBO_DEVICENAME);
		TESTERTYPE	type=TP_NONE;
		HANDLE		hDevice=INVALID_HANDLE_VALUE;
		string		str;
		CHAR		csBuf[MAX_PATH]={0};
		BYTE		retDatas[SHORTLEN]={0};
		SHORT		SDValue[5];

		EnumCommPort(vs);
		for(VECSTR::iterator p=vs.begin(); p!=vs.end(); p++)
		{
			wstr=p->c_str();
			OpenSerialPort(wstr, /*m_hDevice*/hDevice);

			type=P_GetType(hDevice);


			if(	TP_STAND==type)
			{
				//get standard value
				if( P_GetSDResult(hDevice, /*TP_STAND,*/ retDatas, SHORTLEN))
				{
					memmove(SDValue, retDatas+1, sizeof(SDValue));
					SDValue[PM25]=ntohs(SDValue[PM25]);
					SDValue[CO2]=ntohs(SDValue[CO2]);
					SDValue[CH2O]=ntohs(SDValue[CH2O]);
					SDValue[HUMITY]=ntohs(SDValue[HUMITY]);
					SDValue[TEMP]=ntohs(SDValue[TEMP]);

					StringCchPrintfA(csBuf, MAX_PATH, "PM2.5:	%d ug/m3\r\nCO2:	%d ppm\r\nCH2O:	%d mg/m3\r\nTemp:	%f ¡æ\r\nHumity:	%f%%\r\n", 
						SDValue[PM25], SDValue[CO2],SDValue[CH2O],
						((FLOAT)(SDValue[TEMP])/10), ((FLOAT)(SDValue[HUMITY])/10) );
					str=csBuf;
					SetDlgItemTextA(this->m_hWnd, IDC_EDIT_ORIINFO, str.c_str());

				}

				CloseSerialPort(hDevice);
				break;
			}

			CloseSerialPort(hDevice);
		}


	}

	CDialogEx::OnTimer(nIDEvent);
}


void CStandardDlg::OnBnClickedBtnClear()
{
	SetDlgItemTextW(IDC_EDIT_MAC, L"");
}
