// StandardDlg.cpp : ÊµÏÖÎÄ¼þ
//

#include "stdafx.h"
#include "MT.h"
#include "Standard2Dlg.h"
#include "afxdialogex.h"
#include "SerialPortHelper.h"
#include <strsafe.h>
#include "UDP.h"

#include "HintDlg.h"

//macµØÖ·³¤¶È
#define		SD_MACADDRLEN				(12)
//·¢ËÍ¹ã²¥ÖØÊÔ´ÎÊý
#define		SD_MAXUPDBOARCOSTNUM		(0x3)

//UPDÉÏ´«Êý¾ÝÔ¤¶¨Òå×î´ó³¤¶È
#define		SD_UDPRECVMAXLEN			(0xFFFF)

//¶Ë¿ÚºÅ
#define		SD_UDPPORTSEND				(33332)
#define		SD_UDPPORTRECV				(33334)





void InitAirDatas(PAIRSD2 p)
{
	if( p)
	{
		p->bStatus=FALSE;
		p->bSDEnd=FALSE;
		p->bReadReady=FALSE;
		p->strOri.clear();
		p->dCh2oCalibration=0.0;
		p->dCh2oK1=0.0;
		p->dCh2oK2=0.0;
		p->dCh2oK3=0.0;
		p->dCh2oK4=0.0;
		p->dCh2oK5=0.0;
		p->dCh2oOffset1=0.0;
		p->dCh2oOffset2=0.0;
		p->dCh2oOffset3=0.0;
		p->dCh2oOffset4=0.0;
		p->dCh2oOffset5=0.0;
		p->dch2oOriginal=0.0;
		p->dHumi=0.0;
		p->dHumiOffset=0.0;
		p->dPM25K1=0.0;
		p->dPM25K2=0.0;
		p->dPM25K3=0.0;
		p->dPM25K4=0.0;
		p->dPM25K5=0.0;
		p->dPM25K6=0.0;
		p->dPM25K7=0.0;
		p->dPM25K8=0.0;
		p->dPM25K9=0.0;
		p->dPM25K10=0.0;
		p->dPM25Mana=0.0;
		p->dTemp=0.0;
		p->dTempOffset=0.0;
		p->nCo2=0;
		p->nCo2Offset=0;
		p->nOnline=0;
		p->nPM25=0;
		p->nPm25BaseLine=0;
		p->nPm25Noise=0;
		p->nPM25Offset1=0;
		p->nPM25Offset2=0;
		p->nPM25Offset3=0;
		p->nPM25Offset4=0;
		p->nPM25Offset5=0;
		p->nPM25Offset6=0;
		p->nPM25Offset7=0;
		p->nPM25Offset8=0;
		p->nPM25Offset9=0;
		p->nPM25Offset10=0;
		p->nPM25Ori=0;
		p->nSoc=0;
		p->nVoice=0;
	}
}


// CStandard2Dlg ¶Ô»°¿ò

IMPLEMENT_DYNAMIC(CStandard2Dlg, CDialogEx)

CStandard2Dlg::CStandard2Dlg(LPVOID p, CWnd* pParent /*=NULL*/)
	: CDialogEx(CStandard2Dlg::IDD, pParent),
m_nListReportIndex(0),
m_isShow(FALSE),
m_nTotalNum(0),
m_nPassNum(0),
m_nFailNum(0),
m_ReadThreadEnd(FALSE),
m_hS(NULL)
{
	m_pParent=(CMT2Dlg*)p;
	m_deviceHandle=INVALID_HANDLE_VALUE;

	m_pReadBuf=new BYTE[SD_UDPRECVMAXLEN];
}

CStandard2Dlg::~CStandard2Dlg()
{
	CloseHandle(m_deviceHandle);

	TerminateThread(m_hReadCore, 0);

	if( m_hS)
	{
		for(int i=0; i<m_nSCount; i++)
		{
			TerminateThread(m_hS[i], 0);
		}
		delete[] m_hS;
	}

	Sleep(1000);

	if( m_pReadBuf)
	{
		delete[] m_pReadBuf;
	}

	this->InitOrDestoryParams(FALSE);

	m_ReadThreadEnd=TRUE;
}

void CStandard2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CStandard2Dlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_START, &CStandard2Dlg::OnBnClickedButtonStart)
	ON_WM_CTLCOLOR()
	ON_EN_CHANGE(IDC_EDIT_MAC, &CStandard2Dlg::OnEnChangeEditMac)
	ON_BN_CLICKED(IDC_BTN_CLEAR, &CStandard2Dlg::OnBnClickedBtnClear)
END_MESSAGE_MAP()


// CStandard2Dlg ÏûÏ¢´¦Àí³ÌÐò

VOID			CStandard2Dlg::InitListCtrl()
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

VOID			CStandard2Dlg::CalOriMeanValue(AIRSD2& airDes, PAIRSD2	pAirSour, INT nCount)
{
	CHAR		csBuf[MAX_PATH]={0};
	INT			index=0;
	string		str;

	airDes.dHumi			=0;
	airDes.dTemp			=0;
	airDes.nCo2				=0;
	airDes.nPM25			=0;
	airDes.dch2oOriginal	=0;
	airDes.dPM25Mana		=0;

	str="abcde:";
	str+=pAirSour->strMac;

	for(int r=0; r<nCount; r++)
	{
		airDes.dHumi			+=pAirSour[r].dHumi;
		airDes.dTemp			+=pAirSour[r].dTemp;
		airDes.nCo2				+=pAirSour[r].nCo2;
		airDes.nPM25			+=pAirSour[r].nPM25;
		airDes.dch2oOriginal	+=pAirSour[r].dch2oOriginal;
		airDes.dPM25Mana		+=pAirSour[r].dPM25Mana;

		index+=sprintf_s(csBuf+index, MAX_PATH-index, ":%f,	", pAirSour[r].dPM25Mana);
	}

	str+=csBuf;
	OutputDebugStringA(str.c_str());///

	airDes.dHumi			/=nCount;
	airDes.dTemp			/=nCount;
	airDes.nCo2				/=nCount;
	airDes.nPM25			/=nCount;
	airDes.dch2oOriginal	/=nCount;
	airDes.dPM25Mana		/=nCount;


	airDes.dHumiOffset=pAirSour[nCount-1].dHumiOffset;
	airDes.dPM25K1=pAirSour[nCount-1].dPM25K1;
	airDes.dPM25K2=pAirSour[nCount-1].dPM25K2;
	airDes.dPM25K3=pAirSour[nCount-1].dPM25K3;
	airDes.dPM25K4=pAirSour[nCount-1].dPM25K4;
	airDes.dPM25K5=pAirSour[nCount-1].dPM25K5;
	airDes.dPM25K6=pAirSour[nCount-1].dPM25K6;
	airDes.dPM25K7=pAirSour[nCount-1].dPM25K7;
	airDes.dPM25K8=pAirSour[nCount-1].dPM25K8;
	airDes.dPM25K9=pAirSour[nCount-1].dPM25K9;
	airDes.dPM25K10=pAirSour[nCount-1].dPM25K10;
	airDes.dTempOffset=pAirSour[nCount-1].dTempOffset;
	airDes.nCo2Offset=pAirSour[nCount-1].nCo2Offset;
	airDes.nOnline=pAirSour[nCount-1].nOnline;
	//airDes.dPM25Mana=pAirSour[nCount-1].dPM25Mana;
	CF_OutputDBGInfo("...PM25 mana:%f  %f\r\n", airDes.dPM25Mana,  pAirSour[nCount-1].dPM25Mana);///
	airDes.nPm25BaseLine=pAirSour[nCount-1].nPm25BaseLine;
	airDes.nPm25Noise=pAirSour[nCount-1].nPm25Noise;
	airDes.nPM25Offset1=pAirSour[nCount-1].nPM25Offset1;
	airDes.nPM25Offset2=pAirSour[nCount-1].nPM25Offset2;
	airDes.nPM25Offset3=pAirSour[nCount-1].nPM25Offset3;
	airDes.nPM25Offset4=pAirSour[nCount-1].nPM25Offset4;
	airDes.nPM25Offset5=pAirSour[nCount-1].nPM25Offset5;
	airDes.nPM25Offset6=pAirSour[nCount-1].nPM25Offset6;
	airDes.nPM25Offset7=pAirSour[nCount-1].nPM25Offset7;
	airDes.nPM25Offset8=pAirSour[nCount-1].nPM25Offset8;
	airDes.nPM25Offset9=pAirSour[nCount-1].nPM25Offset9;
	airDes.nPM25Offset10=pAirSour[nCount-1].nPM25Offset10;
	airDes.nPM25Ori=pAirSour[nCount-1].nPM25Ori;
	airDes.nSoc=pAirSour[nCount-1].nSoc;
	airDes.nVoice=pAirSour[nCount-1].nVoice;
	airDes.strMac=pAirSour[nCount-1].strMac;
	airDes.strMcvVer=pAirSour[nCount-1].strMcvVer;
	airDes.strPsn=pAirSour[nCount-1].strPsn;
	airDes.strSend=pAirSour[nCount-1].strSend;
	airDes.strWfVer=pAirSour[nCount-1].strWfVer;


	airDes.dCh2oCalibration=pAirSour[nCount-1].dCh2oCalibration;
	airDes.dCh2oK1=pAirSour[nCount-1].dCh2oK1;
	airDes.dCh2oK2=pAirSour[nCount-1].dCh2oK2;
	airDes.dCh2oK3=pAirSour[nCount-1].dCh2oK3;
	airDes.dCh2oK4=pAirSour[nCount-1].dCh2oK4;
	airDes.dCh2oK5=pAirSour[nCount-1].dCh2oK5;
	airDes.dCh2oOffset1=pAirSour[nCount-1].dCh2oOffset1;
	airDes.dCh2oOffset2=pAirSour[nCount-1].dCh2oOffset2;
	airDes.dCh2oOffset3=pAirSour[nCount-1].dCh2oOffset3;
	airDes.dCh2oOffset4=pAirSour[nCount-1].dCh2oOffset4;
	airDes.dCh2oOffset5=pAirSour[nCount-1].dCh2oOffset5;
}


unsigned _stdcall	_ThreadForSingleDevice(LPVOID p)
{
	PSINGLEDATAS	pDatas=(PSINGLEDATAS)p;
	CStandard2Dlg*	pThis=(CStandard2Dlg*)pDatas->p;
	wstring			wstrMac=pDatas->wstrMac;
	wstring			wstrTmp;
	string			strOri;
	PAIRSD2			airsdS=new AIRSD2[pThis->m_configS.nRetryNum];

	//½ÓÊÕÔ­Ê¼´ý±ê¶¨Êý¾Ý´ÎÊý
	INT				nRecvIndex=0;

	//Ã¿´Î·¢ËÍUDPÊ±¼ä¼ä¸ô
	const INT	nSendElapse=600;


	wstrTmp=L"**start recv data**:";
	wstrTmp+=wstrMac;
	pThis->SetShowInfo(wstrTmp.c_str());


	if(!airsdS)
	{
		//½«¶ÁÈ¡Ö÷Ïß³ÌÍË³ö
		//pThis->m_bReadThreadEnd=TRUE;

		wstrTmp=wstrMac;
		wstrTmp+=L":·ÖÅä¿Õ¼äÊ§°Ü";
		pThis->SetShowInfo(wstrTmp.c_str());
		return 1;
	}

	for(int i=0; i<pThis->m_configS.nRetryNum; i++)
	{
		InitAirDatas(airsdS+i);
	}
	

	//¸ÃÉè±¸Î´±ê¶¨Íê³É, parsing
	AIRSD2&	AirSD=pThis->m_mapRecvAirSD[wstrMac];
	while( !AirSD.bSDEnd)
	{
		if( AirSD.bReadReady)
		{
			strOri=AirSD.strOri;
			AirSD.bReadReady=FALSE;

			//parse datas
			pThis->ParseSingleDatas(strOri, airsdS+nRecvIndex);

			if( (++nRecvIndex)>=pThis->m_configS.nRetryNum )
			{
				//°´ÕÕnRetryNum·ÖÎöÊý¾ÝÍê±Ï£¬½á¹ûÈ¡ÆäÆ½¾ùÖµ
				pThis->CalOriMeanValue(AirSD, airsdS, pThis->m_configS.nRetryNum);

				wstrTmp=L"--->recv(";
				wstrTmp+=wstrMac.c_str();
				wstrTmp+=L")";
				pThis->SetShowInfo(wstrTmp.c_str());

				//ÏÔÊ¾mcu°æ±¾
				OutputDebugStringA("-----------------------------------------------------\r\n");
				OutputDebugStringW(L"mac:");
				OutputDebugStringW(wstrMac.c_str());
				string strMcuVer	=	CF_W2AEx(wstrMac);
				CF_OutputDBGInfo("\r\n mac__:%s \r\n ver:%s", strMcuVer.c_str(), AirSD.strMcvVer.c_str());

				strMcuVer			+=	":";
				strMcuVer			+=	AirSD.strMcvVer;
				strMcuVer			+=	"\r\n";

				CF_OutputDBGInfo("\r\n result:%s\r\n", strMcuVer.c_str());
				pThis->ShowMCUVer((LPSTR)strMcuVer.c_str(), FALSE);
				OutputDebugStringA("++++++++++++++++++++++++++++++++++++++++++++++++++++++\r\n");

				
				break;
			}
		}

		Sleep(70);
	}


	//¼ÆËãgoldern sampleµÄÆ½¾ùÖµ
	pThis->CalGSMeanValue();

	//¼ÆËã±ê¶¨Êý¾Ý
	if( !AirSD.bSDEnd)
	{
		pThis->GetSDDatasAndFormation(AirSD);
	}

	//·¢ËÍ±ê¶¨Êý¾Ý
	{
		wstrTmp=L"--->send(";
		wstrTmp+=CF_A2W(AirSD.strMac).c_str();
		wstrTmp+=L")";

		//OutputDebugStringA(p->second.strSend.c_str());///////

		pThis->SetShowInfo(wstrTmp.c_str());
		pThis->SetShowInfo(CF_A2W(AirSD.strSend).c_str(), !pThis->m_isShow);


		for(int r=0; r<SENDSDCOMMANDNUM; r++)
		{
			if( !UDP_SendBoardcastUsed((PBYTE)AirSD.strSend.c_str(), AirSD.strSend.length(), SD_UDPPORTSEND) )
			{
				CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"Send UDP boardcast err(ID:0x%x).\r\n", GetLastError() );
				//break;
			}

			Sleep(nSendElapse);
		}
	}

	//½ÓÊÕÑéÖ¤Êý¾Ý
	while( !AirSD.bSDEnd)
	{
		if( AirSD.bReadReady)
		{
			strOri=AirSD.strOri;
			AirSD.bReadReady=FALSE;


			wstrTmp=L"--->recv(";
			wstrTmp+=wstrMac.c_str();
			wstrTmp+=L")";
			pThis->SetShowInfo(wstrTmp.c_str());
			pThis->SetShowInfo(CF_A2W(strOri).c_str(), !pThis->m_isShow);

			//parse datas
			pThis->ParseSingleDatas(strOri, /*airsdS+0*/&AirSD);

			//ÅÐ¶ÏÑéÖ¤Êý¾ÝÕý·ñÕýÈ·
			pThis->CheckSDDatasEx(AirSD);
			break;
		}

		Sleep(70);
	}

	//send
	{
		wstrTmp=L"--->send(";
		wstrTmp+=CF_A2W(AirSD.strMac).c_str();
		wstrTmp+=L")";

		//OutputDebugStringA(p->second.strSend.c_str());///////

		pThis->SetShowInfo(wstrTmp.c_str());
		pThis->SetShowInfo(CF_A2W(AirSD.strSend).c_str(), !pThis->m_isShow);

		for(int r=0; r<SENDSDCOMMANDNUM; r++)
		{
			if( !UDP_SendBoardcastUsed((PBYTE)AirSD.strSend.c_str(), AirSD.strSend.length(), SD_UDPPORTSEND) )
			{
				CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"Send UDP boardcast err(ID:0x%x).\r\n", GetLastError() );
				//break;
			}

			Sleep(nSendElapse);
		}
	}
	
	//¸ÃÉè±¸±ê¶¨½áÊø
	AirSD.bSDEnd=TRUE;

	wstrTmp=L"--->(***)End(";
	wstrTmp+=CF_A2W(AirSD.strMac).c_str();
	wstrTmp+=L")";
	pThis->SetShowInfo(wstrTmp.c_str());
	
	if( airsdS )
	{
		delete[] airsdS;
	}

	return 1;
}

unsigned _stdcall	_ThreadCore(LPVOID p)
{
	CStandard2Dlg*	pThis=(CStandard2Dlg*)p;
	
	WCHAR			wcsBuf[1024]={0};
	wstring			wstrDes;
	wstring			wstrMacS;
	string			strRecv;
	string			strKey;
	
	//µ¥¸ö½ø³Ì¹²ÏíÊý¾Ý
	SINGLEDATAS		singD;

	//goldern sample ¶ÁÈ¡Ë÷ÒýÖµ
	INT				nReadIndex=0;

	AIRSD2			as;
	InitAirDatas(&as);

	pThis->SetBtnStatus(FALSE);

	pThis->m_bReadThreadEnd=FALSE;

	//set report list index
	pThis->m_nListReportIndex=0;
	pThis->m_listReport->SetTextColor(LISTREPORT_COLOR);		
	pThis->m_listReport->DeleteAllItems();
	GetDlgItemTextW(pThis->m_hWnd, IDC_EDIT_MAC, wcsBuf, 1000);
	
 	pThis->m_mapRecvAirSD.clear();

	for(int i=0; i<pThis->m_configS.nRetryNum; i++)
	{
		pThis->m_mapReadBuf[i].clear();
	}
	
	pThis->m_strFileContent.clear();
	pThis->m_strFileContent=pThis->m_strGSV;


	do 
	{
		PBYTE		pRecvData=pThis->m_pReadBuf;	
		DWORD		dwElapse=0;
		DWORD		dwBegin=0;

		if( !pRecvData)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...allocate memory err(errID:0x%x).\r\n", GetLastError() );
			pThis->m_listReport->SetTextColor(FAILED_COLOR);
			pThis->SetShowInfo(L"allocate memory err.");
			break;
		}

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

					pThis->ForamtionMacStr(wstrDes);
					if( wstrDes.length()==SD_MACADDRLEN)
					{
						/**
						2sÖ÷»ú²»´øco2´«¸ÐÆ÷£¬ÎªÁË¼æÈÝÐÔ¼°ºóÐøÅÐ¶Ï£¨co2ÓëGSÖµ²îÖµ²»ÄÜ´óÓÚ300£©ÕýÈ·£¬
						½«¶ÁÈ¡ÉèÖÃÖµ³õÊ¼»¯³ÉGSÖµ
						*/
						as.bStatus=FALSE;
						as.dTemp=((FLOAT)(pThis->m_SDValue[TEMP])/10.0);
						as.dHumi=((FLOAT)(pThis->m_SDValue[HUMITY])/10.0);
						as.nCo2=pThis->m_SDValue[CO2];
						as.nPM25=pThis->m_SDValue[PM25];
						as.dch2oOriginal=((FLOAT)(pThis->m_SDValue[CH2O])/1000.0);

						pThis->m_mapRecvAirSD.insert(std::pair<wstring, AIRSD2>(wstrDes, as));
						pThis->m_mapRetAirSD.insert(std::pair<wstring, AIRSD2>(wstrDes, as));
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

		if( pThis->m_mapRecvAirSD.size() <=0 )
		{
			pThis->m_listReport->SetTextColor(FAILED_COLOR);
			pThis->SetShowInfo(L"no found mac address.");
			break;
		}	
		//end parse

		//ÓÃÓÚÍË³öÊ±ÖÕÖ¹Ïß³Ì²¢ÊÍ·Å¿Ø¼þ
		pThis->m_nSCount=pThis->m_mapRecvAirSD.size();
		pThis->m_hS=new HANDLE[pThis->m_nSCount];


		//´´½¨·ÖÎöÏß³Ì£¨Õë¶ÔÓÚµ¥¸öÉè±¸£©
		singD.p=p;
		int threadIndex=0;
		for(MAPAIRSD2::iterator r=pThis->m_mapRecvAirSD.begin(); r!=pThis->m_mapRecvAirSD.end(); r++)
		{			
			singD.wstrMac=r->first;
			pThis->m_hS[threadIndex++]=(HANDLE)_beginthreadex(NULL, NULL, _ThreadForSingleDevice, (LPVOID)&singD, NULL, NULL);
			Sleep(300);
		}

		//****************UDP boardcast
		//send
		if( !pThis->SendUDPEntrySD(TRUE) )
		{
			break;
		}

		Sleep(pThis->m_configS.nWaitValue);

		//¶ÁÈ¡UDP¹ã²¥Öµ£¬ÅÐ¶ÏËùÓÐMAC¾ùÒÑ·µ»ØÊý¾Ý£¬³¬Ê±ÉèÖÃÎª UDPRECVMAXTIMEVALUE*n
		dwElapse=pThis->m_configS.nWaitDeviceValue * pThis->m_mapRecvAirSD.size();
		dwBegin=GetTickCount();


		while(!pThis->m_bReadThreadEnd)
		{
			//recv data from device
			{
				ZeroMemory(pRecvData, SD_UDPRECVMAXLEN);

				//recv, timeout 10s
				if( !UDP_RecvBoardcastUsed(pRecvData, SD_UDPRECVMAXLEN, SD_UDPPORTRECV))
				{
					CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"Recv UDP boardcast err(ID:0x%x).\r\n", GetLastError() );
					pThis->m_listReport->SetTextColor(FAILED_COLOR);
					pThis->SetShowInfo(L"½ÓÊÕÊý¾ÝÊ§°Ü .");
					break;
				}

				if( (GetTickCount()-dwBegin)>dwElapse )
				{
					pThis->m_listReport->SetTextColor(FAILED_COLOR);
					pThis->SetShowInfo(L"³¬Ê±£¡ÔÚ¹æ¶¨Ê±¼äÄÚ£¬Éè±¸Î´·µ»ØÊý¾Ý .");
					break;
				}


				//parse MAC
				strRecv=(PCHAR)pRecvData;
				strKey="mac\":\"";
				if( !(pThis->FindKeyStr(strRecv, strKey, '\"', strKey)) )
				{
					CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"not find :%s(%s)\r\n", strRecv.c_str(), strKey.c_str());
					OutputDebugStringA(strRecv.c_str());
					pThis->SetShowInfo(L"not found key 'mac'.");
					continue;
				}

				wstrDes=CF_A2W(strKey);

				//Èç¹û¸ÃÉè±¸Ã»ÓÐ±ê¶¨½áÊø
				if( !pThis->m_mapRecvAirSD[wstrDes].bSDEnd)
				{
					pThis->ReadGSValue( (nReadIndex++)%(pThis->m_configS.nRetryNum) );
					pThis->m_mapRecvAirSD[wstrDes].strOri=strRecv;
					pThis->m_mapRecvAirSD[wstrDes].bReadReady=TRUE;
				}
				else
				{
					wstring wstrTmp;
					wstrTmp=L"MAC:";
					wstrTmp+=wstrDes;
					wstrTmp+=L"End.\r\n";
					pThis->SetShowInfo(wstrTmp.c_str(), !pThis->m_isShow);
				}

				pThis->m_bReadThreadEnd=pThis->IsAllDeviceSDEnd();
			}

			Sleep(70);
		}//end while(!pThis->m_bReadThreadEnd)

	}while(FALSE);


	delete[] pThis->m_hS;
	pThis->m_hS=NULL;
	pThis->SetShowInfo(L"±ê¶¨½áÊø.");
	pThis->SetBtnStatus(TRUE);
	pThis->ShowResultInfo();
	CFileHelper::_()->SaveLogInfo(TP_STAND, (PCHAR)pThis->m_strFileContent.c_str(), pThis->m_strFileContent.length()+1, AIRUS2S);

	//MessageBoxA(pThis->m_hWnd, "±ê¶¨½áÊø.", "INFO", MB_OK);
// 	CHintDlg dlg(CWnd::FromHandle(pThis->m_hWnd));
// 	dlg.DoModal();

	return 1;
}



unsigned _stdcall _ThreadForRefreshGS2(LPVOID p)
{
	CStandard2Dlg* pThis=(CStandard2Dlg*)p;
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

		Sleep(pThis->m_configS.nGsFrequence);
	}


	OutputDebugStringA(__FUNCTION__"...end\r\n");
	return 1;
}

BOOL CStandard2Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//get config
	if( !this->ReadFromIni())
	{
		CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...get config err.\r\n");
		MessageBoxA(this->m_hWnd, "Çë¼ì²éÅäÖÃÎÄ¼þ£¡", "INFO", MB_OK);
		CDialogEx::OnCancel();
	}

	//init params
	if( !this->InitOrDestoryParams(TRUE))
	{
		CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...init param err.\r\n");
		MessageBoxA(this->m_hWnd, "³õÊ¼»¯²ÎÊýÊ§°Ü£¡", "INFO", MB_OK);
		CDialogEx::OnCancel();
	}
	

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
	_beginthreadex(NULL, NULL, _ThreadForRefreshGS2, this, NULL, NULL);

	SetWindowTextA(this->m_hWnd, "SD(2s)");
	CF_SetTitleWithVersion(this->m_hWnd);

	//ÏÔÊ¾±ê¶¨Í³¼ÆÐÅÏ¢
	this->ShowResultInfo();

	////////////TEST MAC addr  MAC:C8-93-46-AA-7F-7F
	//SetDlgItemTextA(this->m_hWnd, IDC_EDIT_MAC, "MAC:C8-93-46-AA-7E-9D\r\nMAC:C8-93-46-AA-7D-F6\r\nMAC:C8-93-46-AA-7F-11\r\nMAC:C8-93-46-AA-7F-23\r\nMAC:C8-93-46-AA-7E-EB\r\n");
	//SetDlgItemTextA(this->m_hWnd, IDC_EDIT_MAC, "MAC:C8-93-46-AA-7F-7F");
	////////////

	return TRUE;  // return TRUE unless you set the focus to a control
	// Òì³£: OCX ÊôÐÔÒ³Ó¦·µ»Ø FALSE
}



VOID		printfMap2(MAPAIRSD2 pMap)
{
	for(MAPAIRSD2::iterator p=pMap.begin(); p!=pMap.end(); p++)
	{
		CF_OutputDBGInfo("mac:%s  temp:%f  temp offset:%f humi:%f humi offset:%f  co2:%d co2 offset:%d pm25:%d  pm25K1:%f pm25k2:%f  pm25k3:%f\r\n",
							p->second.strMac.c_str(),				
							p->second.dTemp, p->second.dTempOffset,
							p->second.dHumi, p->second.dHumiOffset,
							p->second.nCo2, p->second.nCo2Offset, 
							p->second.nPM25, 
							p->second.dPM25K1, p->second.dPM25K2, p->second.dPM25K3
							);
	}
}

VOID		FormatMapData2(wstring wstrMac, MAPAIRSD2* pMap, wstring wstrDes, BOOL bSend)
{
	MAPAIRSD2::iterator	p=pMap->find(wstrMac);
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

VOID			CStandard2Dlg::RefreshDeviceList()
{
	wstring		wstr;
	VECSTR		vs;
	CComboBox*	pComBox=(CComboBox*)GetDlgItem(IDC_COMBO_DEVICENAME);
	BOOL		bFind=FALSE;
	TESTERTYPE	type=TP_NONE;
	HANDLE		hDevice=INVALID_HANDLE_VALUE;
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
			
				StringCchPrintfA(csBuf, MAX_PATH, "PM2.5:	%d ug/m3\r\nCO2:	%d ppm\r\nCH2O:	%f mg/m3\r\nTemp:	%f ¡æ\r\nHumity:	%f%%\r\n", 
										m_SDValue[PM25], m_SDValue[CO2],
										((FLOAT)(m_SDValue[CH2O])/1000.0),
										((FLOAT)(m_SDValue[TEMP])/10), ((FLOAT)(m_SDValue[HUMITY])/10) );

				SetDlgItemTextA(this->m_hWnd, IDC_EDIT_ORIINFO, csBuf);

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


VOID			CStandard2Dlg::ReadGSValue(INT nIndex)
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

		StringCchPrintfA(csBuf, MAX_PATH, "PM2.5:	%d ug/m3\r\nCO2:	%d ppm\r\nCH2O:	%f mg/m3\r\nTemp:	%f ¡æ\r\nHumity:	%f%%\r\n", 
			m_SDMeanValue[nIndex][PM25], m_SDMeanValue[nIndex][CO2],
			((FLOAT)(m_SDMeanValue[nIndex][CH2O])/1000.0),
			((FLOAT)(m_SDMeanValue[nIndex][TEMP])/10), ((FLOAT)(m_SDMeanValue[nIndex][HUMITY])/10) );

		//SetDlgItemTextA(this->m_hWnd, IDC_EDIT_ORIINFO, csBuf);

		//
		CF_OutputDBGInfo(__FUNCTION__"...index:%d %s\r\n", nIndex, csBuf);

		m_strGSV=csBuf;
	}

}


VOID			CStandard2Dlg::SetBtnStatus(BOOL	bEnable)
{
	CButton* pBtn=(CButton*)GetDlgItem(IDC_BUTTON_START);
	CButton* pBtnClear=(CButton*)GetDlgItem(IDC_BTN_CLEAR);

	if( pBtn)
	{
		pBtn->EnableWindow(bEnable);
		pBtnClear->EnableWindow(bEnable);
	}
}

VOID			CStandard2Dlg::SetInputMacStatus(BOOL bEnable)
{
	CEdit* pEdit=(CEdit*)GetDlgItem(IDC_EDIT_MAC);
	if( pEdit)
	{
		pEdit->EnableWindow(bEnable);
	}
}


BOOL			CStandard2Dlg::FindKeyStr(string strOri, string strKey, char end,string& strRet)
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

BOOL			CStandard2Dlg::ParseRetDatas(PBYTE pDatas, wstring& wstrMac, MAPAIRSD2* pMap, BOOL	bCheckMode)
{
	/*
	[4992] {"p":"udp_enter_mode","param":{"enable":1,"show_screen":1,"breath_swtich":1,"breath_color":0,"macs":[{"mac":"C89346C60721"}]}}
	[4992] {"p":"udp_data","param":{"mac":"C89346C60721","psn":"MJBM16022003052ÿ","mcu_version":"2016012501","wifi_version":"2016012202","voice":76,"pm25":45,"pm2.5_offset_1":0,"pm2.5_offset_2":0,"pm2.5_offset_3":0,"pm2.5_k1":0.661237,"pm2.5_k2":0.661237,"pm2.5_k3":0.661237,"pm2.5_primitive":65,"pm2.5_kalman":68.127192,"pm2.5_base_line":318,"pm2.5_noise":323,"co2_offset":-115,"temp_offset":-2.100000,"humi_offset":0,"co2":541,"temperature":24.820000,"humidity":56.690000,"soc":100,"online":1}}
	*/

	string				strRecv=(PCHAR)pDatas;
	string				strKey;
	MAPAIRSD2::iterator	pMapIterator;
	MAPAIRSD2::iterator	pMapKey;
	//ÁÙÊ±»º³åÇøµÄÊý¾ÝÊÇ·ñ¶¼ÒÑ¾­Ìî³äÍê±Ï
	int					i=0;
	BOOL				bRet=FALSE;

	int					nRetryNum=m_configS.nRetryNum;

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
		for(i=0; i<nRetryNum; i++)
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

		if( i<nRetryNum && pMapIterator==m_mapReadBuf[i].end())
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not found key in MAPS:%ws\r\n", wstrMac.c_str());
			break;
		}

		//ÁÙÊ±»º³åÇøÄÚµÄnRetryNumÖ¡Êý¾ÝÒÑ¾­Ìî³äÍê±Ï£¬¼ÆËãÆäÆ½¾ùÖµ
		if( i>=nRetryNum)
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
				pMapKey->second.dch2oOriginal	=	(m_mapReadBuf[0])[wstrMac].dch2oOriginal;
				pMapKey->second.dPM25Mana		=	(m_mapReadBuf[0])[wstrMac].dPM25Mana;
			}
			else
			{
				pMapKey->second.dHumi			=0;
				pMapKey->second.dTemp			=0;
				pMapKey->second.nCo2			=0;
				pMapKey->second.nPM25			=0;
				pMapKey->second.dch2oOriginal	=0;
				pMapKey->second.dPM25Mana		=0;
					
				for(int r=0; r<nRetryNum; r++)
				{
					pMapKey->second.dHumi			+=(m_mapReadBuf[r])[wstrMac].dHumi;
					pMapKey->second.dTemp			+=(m_mapReadBuf[r])[wstrMac].dTemp;
					pMapKey->second.nCo2			+=(m_mapReadBuf[r])[wstrMac].nCo2;
					pMapKey->second.nPM25			+=(m_mapReadBuf[r])[wstrMac].nPM25;
					pMapKey->second.dch2oOriginal	+=(m_mapReadBuf[r])[wstrMac].dch2oOriginal;
					pMapKey->second.dPM25Mana		+=(m_mapReadBuf[r])[wstrMac].dPM25Mana;
				}

				pMapKey->second.dHumi			/=nRetryNum;
				pMapKey->second.dTemp			/=nRetryNum;
				pMapKey->second.nCo2			/=nRetryNum;
				pMapKey->second.nPM25			/=nRetryNum;
				pMapKey->second.dch2oOriginal	/=nRetryNum;
				pMapKey->second.dPM25Mana		/=nRetryNum;
			}

			
			pMapKey->second.dHumiOffset=(m_mapReadBuf[0])[wstrMac].dHumiOffset;
			pMapKey->second.dPM25K1=(m_mapReadBuf[0])[wstrMac].dPM25K1;
			pMapKey->second.dPM25K2=(m_mapReadBuf[0])[wstrMac].dPM25K2;
			pMapKey->second.dPM25K3=(m_mapReadBuf[0])[wstrMac].dPM25K3;
			pMapKey->second.dPM25K4=(m_mapReadBuf[0])[wstrMac].dPM25K4;
			pMapKey->second.dPM25K5=(m_mapReadBuf[0])[wstrMac].dPM25K5;
			pMapKey->second.dPM25K6=(m_mapReadBuf[0])[wstrMac].dPM25K6;
			pMapKey->second.dPM25K7=(m_mapReadBuf[0])[wstrMac].dPM25K7;
			pMapKey->second.dPM25K8=(m_mapReadBuf[0])[wstrMac].dPM25K8;
			pMapKey->second.dPM25K9=(m_mapReadBuf[0])[wstrMac].dPM25K9;
			pMapKey->second.dPM25K10=(m_mapReadBuf[0])[wstrMac].dPM25K10;
			pMapKey->second.dTempOffset=(m_mapReadBuf[0])[wstrMac].dTempOffset;
			pMapKey->second.nCo2Offset=(m_mapReadBuf[0])[wstrMac].nCo2Offset;
			pMapKey->second.nOnline=(m_mapReadBuf[0])[wstrMac].nOnline;
			//pMapKey->second.dPM25Mana=(m_mapReadBuf[0])[wstrMac].dPM25Mana;
			CF_OutputDBGInfo("...PM25 mana:%f  %f\r\n", pMapKey->second.dPM25Mana,  (m_mapReadBuf[0])[wstrMac].dPM25Mana);///
			pMapKey->second.nPm25BaseLine=(m_mapReadBuf[0])[wstrMac].nPm25BaseLine;
			pMapKey->second.nPm25Noise=(m_mapReadBuf[0])[wstrMac].nPm25Noise;
			pMapKey->second.nPM25Offset1=(m_mapReadBuf[0])[wstrMac].nPM25Offset1;
			pMapKey->second.nPM25Offset2=(m_mapReadBuf[0])[wstrMac].nPM25Offset2;
			pMapKey->second.nPM25Offset3=(m_mapReadBuf[0])[wstrMac].nPM25Offset3;
			pMapKey->second.nPM25Offset4=(m_mapReadBuf[0])[wstrMac].nPM25Offset4;
			pMapKey->second.nPM25Offset5=(m_mapReadBuf[0])[wstrMac].nPM25Offset5;
			pMapKey->second.nPM25Offset6=(m_mapReadBuf[0])[wstrMac].nPM25Offset6;
			pMapKey->second.nPM25Offset7=(m_mapReadBuf[0])[wstrMac].nPM25Offset7;
			pMapKey->second.nPM25Offset8=(m_mapReadBuf[0])[wstrMac].nPM25Offset8;
			pMapKey->second.nPM25Offset9=(m_mapReadBuf[0])[wstrMac].nPM25Offset9;
			pMapKey->second.nPM25Offset10=(m_mapReadBuf[0])[wstrMac].nPM25Offset10;
			pMapKey->second.nPM25Ori=(m_mapReadBuf[0])[wstrMac].nPM25Ori;
			pMapKey->second.nSoc=(m_mapReadBuf[0])[wstrMac].nSoc;
			pMapKey->second.nVoice=(m_mapReadBuf[0])[wstrMac].nVoice;
			pMapKey->second.strMac=(m_mapReadBuf[0])[wstrMac].strMac;
			pMapKey->second.strMcvVer=(m_mapReadBuf[0])[wstrMac].strMcvVer;
			pMapKey->second.strPsn=(m_mapReadBuf[0])[wstrMac].strPsn;
			pMapKey->second.strSend=(m_mapReadBuf[0])[wstrMac].strSend;
			pMapKey->second.strWfVer=(m_mapReadBuf[0])[wstrMac].strWfVer;


			pMapKey->second.dCh2oCalibration=(m_mapReadBuf[0])[wstrMac].dCh2oCalibration;
			pMapKey->second.dCh2oK1=(m_mapReadBuf[0])[wstrMac].dCh2oK1;
			pMapKey->second.dCh2oK2=(m_mapReadBuf[0])[wstrMac].dCh2oK2;
			pMapKey->second.dCh2oK3=(m_mapReadBuf[0])[wstrMac].dCh2oK3;
			pMapKey->second.dCh2oK4=(m_mapReadBuf[0])[wstrMac].dCh2oK4;
			pMapKey->second.dCh2oK5=(m_mapReadBuf[0])[wstrMac].dCh2oK5;
			pMapKey->second.dCh2oOffset1=(m_mapReadBuf[0])[wstrMac].dCh2oOffset1;
			pMapKey->second.dCh2oOffset2=(m_mapReadBuf[0])[wstrMac].dCh2oOffset2;
			pMapKey->second.dCh2oOffset3=(m_mapReadBuf[0])[wstrMac].dCh2oOffset3;
			pMapKey->second.dCh2oOffset4=(m_mapReadBuf[0])[wstrMac].dCh2oOffset4;
			pMapKey->second.dCh2oOffset5=(m_mapReadBuf[0])[wstrMac].dCh2oOffset5;

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
		CF_OutputDBGInfo("...PM25 mana:%f\r\n", pMapIterator->second.dPM25Mana);///

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

		///****************pm2.5Æ«ÒÆÖµ0*********************/
		strKey="pm2.5_offset_0\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			break;
		}
		pMapIterator->second.nPM25Offset1=strtol(strKey.c_str(), NULL, 10);
		//CF_OutputDBGInfo("...PM25 offset3:%d\r\n", pMapIterator->second.nPM25OffsetH);///


		///****************pm2.5Æ«ÒÆÖµ1*********************/
		strKey="pm2.5_offset_1\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			break;
		}
		pMapIterator->second.nPM25Offset2=strtol(strKey.c_str(), NULL, 10);
		//CF_OutputDBGInfo("...PM25 offset1:%d\r\n", pMapIterator->second.nPM25OffsetL);///

		///****************pm2.5Æ«ÒÆÖµ2*********************/
		strKey="pm2.5_offset_2\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			break;
		}
		pMapIterator->second.nPM25Offset3=strtol(strKey.c_str(), NULL, 10);
		//CF_OutputDBGInfo("...PM25 offset2:%d\r\n", pMapIterator->second.nPM25OffsetM);///

		///****************pm2.5Æ«ÒÆÖµ3*********************/
		strKey="pm2.5_offset_3\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			break;
		}
		pMapIterator->second.nPM25Offset4=strtol(strKey.c_str(), NULL, 10);
		//CF_OutputDBGInfo("...PM25 offset3:%d\r\n", pMapIterator->second.nPM25OffsetH);///

		///****************pm2.5Æ«ÒÆÖµ4*********************/
		strKey="pm2.5_offset_4\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			break;
		}
		pMapIterator->second.nPM25Offset5=strtol(strKey.c_str(), NULL, 10);
		//CF_OutputDBGInfo("...PM25 offset3:%d\r\n", pMapIterator->second.nPM25OffsetH);///

		///****************pm2.5Æ«ÒÆÖµ5*********************/
		strKey="pm2.5_offset_5\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			break;
		}
		pMapIterator->second.nPM25Offset6=strtol(strKey.c_str(), NULL, 10);
		//CF_OutputDBGInfo("...PM25 offset3:%d\r\n", pMapIterator->second.nPM25OffsetH);///

		///****************pm2.5Æ«ÒÆÖµ6*********************/
		strKey="pm2.5_offset_6\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			break;
		}
		pMapIterator->second.nPM25Offset7=strtol(strKey.c_str(), NULL, 10);
		//CF_OutputDBGInfo("...PM25 offset3:%d\r\n", pMapIterator->second.nPM25OffsetH);///

		///****************pm2.5Æ«ÒÆÖµ7*********************/
		strKey="pm2.5_offset_7\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			break;
		}
		pMapIterator->second.nPM25Offset8=strtol(strKey.c_str(), NULL, 10);
		//CF_OutputDBGInfo("...PM25 offset3:%d\r\n", pMapIterator->second.nPM25OffsetH);///

		///****************pm2.5Æ«ÒÆÖµ8*********************/
		strKey="pm2.5_offset_8\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			break;
		}
		pMapIterator->second.nPM25Offset9=strtol(strKey.c_str(), NULL, 10);
		//CF_OutputDBGInfo("...PM25 offset3:%d\r\n", pMapIterator->second.nPM25OffsetH);///

		///****************pm2.5Æ«ÒÆÖµ9*********************/
		strKey="pm2.5_offset_9\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			break;
		}
		pMapIterator->second.nPM25Offset10=strtol(strKey.c_str(), NULL, 10);
		//CF_OutputDBGInfo("...PM25 offset3:%d\r\n", pMapIterator->second.nPM25OffsetH);///

		///****************pm2.5 K1*********************/
		strKey="pm2.5_k_0\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			break;
		}
		pMapIterator->second.dPM25K1=strtod(strKey.c_str(), NULL);
		//CF_OutputDBGInfo("...PM25 K1:%f\r\n", pMapIterator->second.dPM25KL);///

		///****************pm2.5 K2*********************/
		strKey="pm2.5_k_1\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			break;
		}
		pMapIterator->second.dPM25K2=strtod(strKey.c_str(), NULL);
		//CF_OutputDBGInfo("...PM25 k2:%f\r\n", pMapIterator->second.dPM25KM);///

		///****************pm2.5 K3*********************/
		strKey="pm2.5_k_2\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			break;
		}
		pMapIterator->second.dPM25K3=strtod(strKey.c_str(), NULL);
		//CF_OutputDBGInfo("...PM25 k3:%f\r\n", pMapIterator->second.dPM25KH);///

		///****************pm2.5 K4*********************/
		strKey="pm2.5_k_3\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			break;
		}
		pMapIterator->second.dPM25K4=strtod(strKey.c_str(), NULL);
		//CF_OutputDBGInfo("...PM25 k3:%f\r\n", pMapIterator->second.dPM25KH);///

		///****************pm2.5 K5*********************/
		strKey="pm2.5_k_4\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			break;
		}
		pMapIterator->second.dPM25K5=strtod(strKey.c_str(), NULL);
		//CF_OutputDBGInfo("...PM25 k3:%f\r\n", pMapIterator->second.dPM25KH);///

		///****************pm2.5 K6*********************/
		strKey="pm2.5_k_5\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			break;
		}
		pMapIterator->second.dPM25K6=strtod(strKey.c_str(), NULL);
		//CF_OutputDBGInfo("...PM25 k3:%f\r\n", pMapIterator->second.dPM25KH);///

		///****************pm2.5 K7*********************/
		strKey="pm2.5_k_6\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			break;
		}
		pMapIterator->second.dPM25K7=strtod(strKey.c_str(), NULL);
		//CF_OutputDBGInfo("...PM25 k3:%f\r\n", pMapIterator->second.dPM25KH);///

		///****************pm2.5 K8*********************/
		strKey="pm2.5_k_7\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			break;
		}
		pMapIterator->second.dPM25K8=strtod(strKey.c_str(), NULL);
		//CF_OutputDBGInfo("...PM25 k3:%f\r\n", pMapIterator->second.dPM25KH);///

		///****************pm2.5 K9*********************/
		strKey="pm2.5_k_8\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			break;
		}
		pMapIterator->second.dPM25K9=strtod(strKey.c_str(), NULL);
		//CF_OutputDBGInfo("...PM25 k3:%f\r\n", pMapIterator->second.dPM25KH);///

		///****************pm2.5 K10*********************/
		strKey="pm2.5_k_9\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			break;
		}
		pMapIterator->second.dPM25K10=strtod(strKey.c_str(), NULL);
		//CF_OutputDBGInfo("...PM25 k3:%f\r\n", pMapIterator->second.dPM25KH);///

		

		///****************co2*********************/
		strKey="co2\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			//break;
		}
		else
		{
			pMapIterator->second.nCo2=strtol(strKey.c_str(), NULL, 10);
		}
		//CF_OutputDBGInfo("...co2:%d\r\n", pMapIterator->second.nCo2);///

		///****************co2Æ«ÒÆÖµ*********************/
		strKey="co2_offset\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			//break;
		}
		else
		{
			pMapIterator->second.nCo2Offset=strtol(strKey.c_str(), NULL, 10);
		}
		//CF_OutputDBGInfo("...co2 offset:%d\r\n", pMapIterator->second.nCo2Offset);///
		

		///****************ch20Original*********************/
		strKey="ch2o_original\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			break;
		}
		else
		{
			pMapIterator->second.dch2oOriginal=strtod(strKey.c_str(), NULL);
		}
		
		//CF_OutputDBGInfo("...co2:%d\r\n", pMapIterator->second.nCo2);///

		///****************ch20Calibration*********************/
		strKey="ch2o_calibration\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			break;
		}
		else
		{
			pMapIterator->second.dCh2oCalibration=strtod(strKey.c_str(), NULL);
		}
		
		//CF_OutputDBGInfo("...co2:%d\r\n", pMapIterator->second.nCo2);///

		///****************ch20	K1*********************/
		strKey="ch2o_k_0\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			break;
		}
		else
		{
			pMapIterator->second.dCh2oK1=strtod(strKey.c_str(), NULL);
		}
		
		//CF_OutputDBGInfo("...co2:%d\r\n", pMapIterator->second.nCo2);///

		///****************ch20	K2*********************/
		strKey="ch2o_k_1\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			break;
		}
		else
		{
			pMapIterator->second.dCh2oK2=strtod(strKey.c_str(), NULL);
		}
		
		//CF_OutputDBGInfo("...co2:%d\r\n", pMapIterator->second.nCo2);///

		///****************ch20	K3*********************/
		strKey="ch2o_k_2\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			break;
		}
		else
		{
			pMapIterator->second.dCh2oK3=strtod(strKey.c_str(), NULL);
		}
		
		//CF_OutputDBGInfo("...co2:%d\r\n", pMapIterator->second.nCo2);///

		///****************ch20	K4*********************/
		strKey="ch2o_k_3\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			break;
		}
		else
		{
			pMapIterator->second.dCh2oK4=strtod(strKey.c_str(), NULL);
		}
		
		//CF_OutputDBGInfo("...co2:%d\r\n", pMapIterator->second.nCo2);///

		///****************ch20	K5*********************/
		strKey="ch2o_k_4\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			break;
		}
		else
		{
			pMapIterator->second.dCh2oK5=strtod(strKey.c_str(), NULL);
		}
		
		//CF_OutputDBGInfo("...co2:%d\r\n", pMapIterator->second.nCo2);///

		///****************ch20	offset1*********************/
		strKey="ch2o_offset_0\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			break;
		}
		else
		{
			pMapIterator->second.dCh2oOffset1=strtod(strKey.c_str(), NULL);
		}
		
		//CF_OutputDBGInfo("...co2:%d\r\n", pMapIterator->second.nCo2);///

		///****************ch20	offset2*********************/
		strKey="ch2o_offset_1\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			break;
		}
		else
		{
			pMapIterator->second.dCh2oOffset2=strtod(strKey.c_str(), NULL);
		}
		
		//CF_OutputDBGInfo("...co2:%d\r\n", pMapIterator->second.nCo2);///

		///****************ch20	offset3*********************/
		strKey="ch2o_offset_2\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			break;
		}
		else
		{
			pMapIterator->second.dCh2oOffset3=strtod(strKey.c_str(), NULL);
		}
		
		//CF_OutputDBGInfo("...co2:%d\r\n", pMapIterator->second.nCo2);///

		///****************ch20	offset4*********************/
		strKey="ch2o_offset_3\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			break;
		}
		else
		{
			pMapIterator->second.dCh2oOffset4=strtod(strKey.c_str(), NULL);
		}
		
		//CF_OutputDBGInfo("...co2:%d\r\n", pMapIterator->second.nCo2);///

		///****************ch20	offset5*********************/
		strKey="ch2o_offset_4\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			break;
		}
		else
		{
			pMapIterator->second.dCh2oOffset5=strtod(strKey.c_str(), NULL);
		}
		
		//CF_OutputDBGInfo("...co2:%d\r\n", pMapIterator->second.nCo2);///

		///****************ÎÂ¶È*********************/
		strKey="temperature\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			break;
		}
		else
		{
			pMapIterator->second.dTemp=strtod(strKey.c_str(), NULL);
		}
		
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


BOOL			CStandard2Dlg::ParseSingleDatas(string strOri, PAIRSD2 pAirD)
{
	/*
	[4992] {"p":"udp_enter_mode","param":{"enable":1,"show_screen":1,"breath_swtich":1,"breath_color":0,"macs":[{"mac":"C89346C60721"}]}}
	[4992] {"p":"udp_data","param":{"mac":"C89346C60721","psn":"MJBM16022003052ÿ","mcu_version":"2016012501","wifi_version":"2016012202","voice":76,"pm25":45,"pm2.5_offset_1":0,"pm2.5_offset_2":0,"pm2.5_offset_3":0,"pm2.5_k1":0.661237,"pm2.5_k2":0.661237,"pm2.5_k3":0.661237,"pm2.5_primitive":65,"pm2.5_kalman":68.127192,"pm2.5_base_line":318,"pm2.5_noise":323,"co2_offset":-115,"temp_offset":-2.100000,"humi_offset":0,"co2":541,"temperature":24.820000,"humidity":56.690000,"soc":100,"online":1}}
	*/

	string				strRecv=strOri;
	wstring				wstr;
	string				strKey;
	BOOL				bRet=FALSE;
	PAIRSD2				oriDatas=pAirD;

	wstr=CF_A2W(strRecv);
	SetShowInfo(wstr.c_str(), !m_isShow);

	do 
	{
		///****************MAC*********************/
		strKey="mac\":\"";
		bRet=this->FindKeyStr(strRecv, strKey, '\"', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"not find :%s(%s)\r\n", strKey.c_str(), strRecv.c_str());
			break;
		}

		//¿ªÊ¼Ìî³ä
		oriDatas->strMac=strKey;
	

		///****************mcu_version*********************/
		strKey="mcu_version\":\"";
		bRet=this->FindKeyStr(strRecv, strKey, '\"', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			//break;
		}
		oriDatas->strMcvVer=strKey;

		///****************wifi_version*********************/
		strKey="wifi_version\":\"";
		bRet=this->FindKeyStr(strRecv, strKey, '\"', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			//break;
		}
		oriDatas->strWfVer=strKey;

		///****************PSN*********************/
		strKey="psn\":\"";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			//break;
		}
		oriDatas->strPsn=strKey;

		///****************pm2.5*********************/
		strKey="pm25\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			//break;
		}
		oriDatas->nPM25=strtol(strKey.c_str(), NULL, 10);
		//CF_OutputDBGInfo("...PM25:%d\r\n", pMapIterator->second.nPM25);///

		///****************pm2.5´«¸ÐÆ÷ÉÏ´«Ô­Ê¼Êý¾Ý*********************/
		strKey="pm2.5_primitive\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			//break;
		}
		oriDatas->nPM25Ori=strtol(strKey.c_str(), NULL, 10);
		//CF_OutputDBGInfo("...PM25 ori:%d\r\n", pMapIterator->second.nPM25Ori);///

		///****************pm2.5¾­¹ýkalmanÂË²¨ºóÊý¾Ý*********************/
		strKey="pm2.5_kalman\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			//break;
		}
		oriDatas->dPM25Mana=strtod(strKey.c_str(), NULL);
		CF_OutputDBGInfo("...PM25 mana:%f\r\n", oriDatas->dPM25Mana);///

		///****************pm2.5 base line*********************/
		strKey="pm2.5_base_line\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			//break;
		}
		oriDatas->nPm25BaseLine=strtol(strKey.c_str(), NULL, 10);
		//CF_OutputDBGInfo("...PM25 base line:%f\r\n", pMapIterator->second.dPM25KH);///

		///****************pm2.5 noise*********************/
		strKey="pm2.5_noise\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			//break;
		}
		oriDatas->nPm25Noise=strtol(strKey.c_str(), NULL, 10);
		//CF_OutputDBGInfo("...PM25 noise:%f\r\n", pMapIterator->second.dPM25KH);///

		///****************pm2.5Æ«ÒÆÖµ0*********************/
		strKey="pm2.5_offset_0\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			//break;
		}
		oriDatas->nPM25Offset1=strtol(strKey.c_str(), NULL, 10);
		//CF_OutputDBGInfo("...PM25 offset3:%d\r\n", pMapIterator->second.nPM25OffsetH);///


		///****************pm2.5Æ«ÒÆÖµ1*********************/
		strKey="pm2.5_offset_1\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			//break;
		}
		oriDatas->nPM25Offset2=strtol(strKey.c_str(), NULL, 10);
		//CF_OutputDBGInfo("...PM25 offset1:%d\r\n", pMapIterator->second.nPM25OffsetL);///

		///****************pm2.5Æ«ÒÆÖµ2*********************/
		strKey="pm2.5_offset_2\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			//break;
		}
		oriDatas->nPM25Offset3=strtol(strKey.c_str(), NULL, 10);
		//CF_OutputDBGInfo("...PM25 offset2:%d\r\n", pMapIterator->second.nPM25OffsetM);///

		///****************pm2.5Æ«ÒÆÖµ3*********************/
		strKey="pm2.5_offset_3\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			//break;
		}
		oriDatas->nPM25Offset4=strtol(strKey.c_str(), NULL, 10);
		//CF_OutputDBGInfo("...PM25 offset3:%d\r\n", pMapIterator->second.nPM25OffsetH);///

		///****************pm2.5Æ«ÒÆÖµ4*********************/
		strKey="pm2.5_offset_4\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			//break;
		}
		oriDatas->nPM25Offset5=strtol(strKey.c_str(), NULL, 10);
		//CF_OutputDBGInfo("...PM25 offset3:%d\r\n", pMapIterator->second.nPM25OffsetH);///

		///****************pm2.5Æ«ÒÆÖµ5*********************/
		strKey="pm2.5_offset_5\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			//break;
		}
		oriDatas->nPM25Offset6=strtol(strKey.c_str(), NULL, 10);
		//CF_OutputDBGInfo("...PM25 offset3:%d\r\n", pMapIterator->second.nPM25OffsetH);///

		///****************pm2.5Æ«ÒÆÖµ6*********************/
		strKey="pm2.5_offset_6\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			//break;
		}
		oriDatas->nPM25Offset7=strtol(strKey.c_str(), NULL, 10);
		//CF_OutputDBGInfo("...PM25 offset3:%d\r\n", pMapIterator->second.nPM25OffsetH);///

		///****************pm2.5Æ«ÒÆÖµ7*********************/
		strKey="pm2.5_offset_7\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			//break;
		}
		oriDatas->nPM25Offset8=strtol(strKey.c_str(), NULL, 10);
		//CF_OutputDBGInfo("...PM25 offset3:%d\r\n", pMapIterator->second.nPM25OffsetH);///

		///****************pm2.5Æ«ÒÆÖµ8*********************/
		strKey="pm2.5_offset_8\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			//break;
		}
		oriDatas->nPM25Offset9=strtol(strKey.c_str(), NULL, 10);
		//CF_OutputDBGInfo("...PM25 offset3:%d\r\n", pMapIterator->second.nPM25OffsetH);///

		///****************pm2.5Æ«ÒÆÖµ9*********************/
		strKey="pm2.5_offset_9\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			//break;
		}
		oriDatas->nPM25Offset10=strtol(strKey.c_str(), NULL, 10);
		//CF_OutputDBGInfo("...PM25 offset3:%d\r\n", pMapIterator->second.nPM25OffsetH);///

		///****************pm2.5 K1*********************/
		strKey="pm2.5_k_0\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			//break;
		}
		oriDatas->dPM25K1=strtod(strKey.c_str(), NULL);
		//CF_OutputDBGInfo("...PM25 K1:%f\r\n", pMapIterator->second.dPM25KL);///

		///****************pm2.5 K2*********************/
		strKey="pm2.5_k_1\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			//break;
		}
		oriDatas->dPM25K2=strtod(strKey.c_str(), NULL);
		//CF_OutputDBGInfo("...PM25 k2:%f\r\n", pMapIterator->second.dPM25KM);///

		///****************pm2.5 K3*********************/
		strKey="pm2.5_k_2\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			//break;
		}
		oriDatas->dPM25K3=strtod(strKey.c_str(), NULL);
		//CF_OutputDBGInfo("...PM25 k3:%f\r\n", pMapIterator->second.dPM25KH);///

		///****************pm2.5 K4*********************/
		strKey="pm2.5_k_3\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			//break;
		}
		oriDatas->dPM25K4=strtod(strKey.c_str(), NULL);
		//CF_OutputDBGInfo("...PM25 k3:%f\r\n", pMapIterator->second.dPM25KH);///

		///****************pm2.5 K5*********************/
		strKey="pm2.5_k_4\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			//break;
		}
		oriDatas->dPM25K5=strtod(strKey.c_str(), NULL);
		//CF_OutputDBGInfo("...PM25 k3:%f\r\n", pMapIterator->second.dPM25KH);///

		///****************pm2.5 K6*********************/
		strKey="pm2.5_k_5\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			//break;
		}
		oriDatas->dPM25K6=strtod(strKey.c_str(), NULL);
		//CF_OutputDBGInfo("...PM25 k3:%f\r\n", pMapIterator->second.dPM25KH);///

		///****************pm2.5 K7*********************/
		strKey="pm2.5_k_6\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			//break;
		}
		oriDatas->dPM25K7=strtod(strKey.c_str(), NULL);
		//CF_OutputDBGInfo("...PM25 k3:%f\r\n", pMapIterator->second.dPM25KH);///

		///****************pm2.5 K8*********************/
		strKey="pm2.5_k_7\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			//break;
		}
		oriDatas->dPM25K8=strtod(strKey.c_str(), NULL);
		//CF_OutputDBGInfo("...PM25 k3:%f\r\n", pMapIterator->second.dPM25KH);///

		///****************pm2.5 K9*********************/
		strKey="pm2.5_k_8\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			//break;
		}
		oriDatas->dPM25K9=strtod(strKey.c_str(), NULL);
		//CF_OutputDBGInfo("...PM25 k3:%f\r\n", pMapIterator->second.dPM25KH);///

		///****************pm2.5 K10*********************/
		strKey="pm2.5_k_9\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			//break;
		}
		oriDatas->dPM25K10=strtod(strKey.c_str(), NULL);
		//CF_OutputDBGInfo("...PM25 k3:%f\r\n", pMapIterator->second.dPM25KH);///

		

		///****************co2*********************/
		strKey="co2\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			//break;
		}
		else
		{
			oriDatas->nCo2=strtol(strKey.c_str(), NULL, 10);
		}
		//CF_OutputDBGInfo("...co2:%d\r\n", pMapIterator->second.nCo2);///

		///****************co2Æ«ÒÆÖµ*********************/
		strKey="co2_offset\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			//break;
		}
		else
		{
			oriDatas->nCo2Offset=strtol(strKey.c_str(), NULL, 10);
		}
		//CF_OutputDBGInfo("...co2 offset:%d\r\n", pMapIterator->second.nCo2Offset);///
		

		///****************ch20Original*********************/
		strKey="ch2o_original\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			//break;
		}
		else
		{
			oriDatas->dch2oOriginal=strtod(strKey.c_str(), NULL);
		}
		
		//CF_OutputDBGInfo("...co2:%d\r\n", pMapIterator->second.nCo2);///

		///****************ch20Calibration*********************/
		strKey="ch2o_calibration\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			//break;
		}
		else
		{
			oriDatas->dCh2oCalibration=strtod(strKey.c_str(), NULL);
		}
		
		//CF_OutputDBGInfo("...co2:%d\r\n", pMapIterator->second.nCo2);///

		///****************ch20	K1*********************/
		strKey="ch2o_k_0\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			//break;
		}
		else
		{
			oriDatas->dCh2oK1=strtod(strKey.c_str(), NULL);
		}
		
		//CF_OutputDBGInfo("...co2:%d\r\n", pMapIterator->second.nCo2);///

		///****************ch20	K2*********************/
		strKey="ch2o_k_1\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			//break;
		}
		else
		{
			oriDatas->dCh2oK2=strtod(strKey.c_str(), NULL);
		}
		
		//CF_OutputDBGInfo("...co2:%d\r\n", pMapIterator->second.nCo2);///

		///****************ch20	K3*********************/
		strKey="ch2o_k_2\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			//break;
		}
		else
		{
			oriDatas->dCh2oK3=strtod(strKey.c_str(), NULL);
		}
		
		//CF_OutputDBGInfo("...co2:%d\r\n", pMapIterator->second.nCo2);///

		///****************ch20	K4*********************/
		strKey="ch2o_k_3\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			//break;
		}
		else
		{
			oriDatas->dCh2oK4=strtod(strKey.c_str(), NULL);
		}
		
		//CF_OutputDBGInfo("...co2:%d\r\n", pMapIterator->second.nCo2);///

		///****************ch20	K5*********************/
		strKey="ch2o_k_4\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			//break;
		}
		else
		{
			oriDatas->dCh2oK5=strtod(strKey.c_str(), NULL);
		}
		
		//CF_OutputDBGInfo("...co2:%d\r\n", pMapIterator->second.nCo2);///

		///****************ch20	offset1*********************/
		strKey="ch2o_offset_0\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			//break;
		}
		else
		{
			oriDatas->dCh2oOffset1=strtod(strKey.c_str(), NULL);
		}
		
		//CF_OutputDBGInfo("...co2:%d\r\n", pMapIterator->second.nCo2);///

		///****************ch20	offset2*********************/
		strKey="ch2o_offset_1\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			//break;
		}
		else
		{
			oriDatas->dCh2oOffset2=strtod(strKey.c_str(), NULL);
		}
		
		//CF_OutputDBGInfo("...co2:%d\r\n", pMapIterator->second.nCo2);///

		///****************ch20	offset3*********************/
		strKey="ch2o_offset_2\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			//break;
		}
		else
		{
			oriDatas->dCh2oOffset3=strtod(strKey.c_str(), NULL);
		}
		
		//CF_OutputDBGInfo("...co2:%d\r\n", pMapIterator->second.nCo2);///

		///****************ch20	offset4*********************/
		strKey="ch2o_offset_3\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			//break;
		}
		else
		{
			oriDatas->dCh2oOffset4=strtod(strKey.c_str(), NULL);
		}
		
		//CF_OutputDBGInfo("...co2:%d\r\n", pMapIterator->second.nCo2);///

		///****************ch20	offset5*********************/
		strKey="ch2o_offset_4\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			//break;
		}
		else
		{
			oriDatas->dCh2oOffset5=strtod(strKey.c_str(), NULL);
		}
		
		//CF_OutputDBGInfo("...co2:%d\r\n", pMapIterator->second.nCo2);///

		///****************ÎÂ¶È*********************/
		strKey="temperature\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			//break;
		}
		else
		{
			oriDatas->dTemp=strtod(strKey.c_str(), NULL);
		}
		
		//CF_OutputDBGInfo("...tmperature:%f\r\n", pMapIterator->second.dTemp);///

		///****************ÎÂ¶ÈÆ«ÒÆ*********************/
		strKey="temp_offset\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			//break;
		}
		oriDatas->dTempOffset=strtod(strKey.c_str(), NULL);
		//CF_OutputDBGInfo("...temp offset:%f\r\n", pMapIterator->second.dTempOffset);///

		///****************Êª¶È*********************/
		strKey="humidity\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			//break;
		}
		oriDatas->dHumi=strtod(strKey.c_str(), NULL);
		//CF_OutputDBGInfo("...Humi:%f\r\n", pMapIterator->second.dHumi);///

		///****************Êª¶ÈÆ«ÒÆ*********************/
		strKey="humi_offset\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			//break;
		}
		oriDatas->dHumiOffset=strtod(strKey.c_str(), NULL);
		//CF_OutputDBGInfo("...Humi offset:%f\r\n", pMapIterator->second.dHumiOffset);///


		///****************voice*********************/
		strKey="voice\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			//break;
		}
		oriDatas->nVoice=strtol(strKey.c_str(), NULL, 10);
		//CF_OutputDBGInfo("...voice:%d\r\n", pMapIterator->second.nVoice);///


		///****************Ê£ÓàµçÁ¿*********************/
		strKey="soc\":";
		bRet=this->FindKeyStr(strRecv, strKey, ',', strKey);
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			//break;
		}
		oriDatas->nSoc=strtol(strKey.c_str(), NULL, 10);
		//CF_OutputDBGInfo("...soc:%d\r\n", pMapIterator->second.nSoc);///

		///****************Online*********************/
		strKey="online\":";
		bRet=this->FindKeyStr(strRecv, strKey, '}', strKey);//×îºóÒ»¸ö£¬·Ö¸ô·ûÎª'}'
		if( !bRet)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...not find :%s\r\n", strKey.c_str());
			//break;
		}
		oriDatas->nOnline=strtol(strKey.c_str(), NULL, 10);
		//CF_OutputDBGInfo("...online:%d\r\n", pMapIterator->second.nOnline);///


		//OK
		oriDatas->bStatus=TRUE;
		bRet=FALSE;

	} while (FALSE);


	return bRet;
}

BOOL			CStandard2Dlg::IsRecvAllDeviceData(MAPAIRSD2*	mapV)
{
	BOOL		bRet=FALSE;

	if( mapV->size() > 0 )
	{
		bRet=TRUE;
	}

	for(MAPAIRSD2::iterator p=mapV->begin(); p!=mapV->end(); p++)
	{
		if( !(p->second.bStatus) )
		{
			bRet=FALSE;
			break;
		}
	}

	return  bRet;
}

VOID			CStandard2Dlg::EnunAllDataAndSD()
{
	/*
	{"p":"udp_set_config","param":{"temp_offset":0.0,"humi_offset":0.0,"pm2.5_k1":1.0,"pm2.5_k2":1.0,"pm2.5_k3":1.0,"pm2.5_offset_1":0,
	"pm2.5_offset_2":0,"pm2.5_offset_3":0,"co2_offset":0,"breath_swtich":1,"breath_color":1,"voice":20,"psn":"MJBM15120301817",
	"err":123,"macs":[{"mac":"C893464E2520"},{"mac":"C893464E0F26"},{"mac":"C893464E29D7"},{"mac":"C893464E2734"}]}}
	*/
	string	strTmp;
	char	csBuf[MAX_PATH]={0};


	for(MAPAIRSD2::iterator p=this->m_mapAirSD.begin(); p!=this->m_mapAirSD.end(); p++)
	{
		strTmp.clear();

		p->second.dPM25K1=0.0;
		p->second.dPM25K2=0.0;
		p->second.dPM25K3=0.0;

		p->second.strSend="{\"p\":\"udp_set_config\",\"param\":{\"temp_offset\":";


		/*ÎÂ¶ÈÆ«ÒÆÖµ£º-2.10 ÉãÊÏ¶È    Êª¶ÈÆ«ÒÆÁ¿£º+10.0*/
		//temp_offset
		StringCchPrintfA(csBuf, MAX_PATH, "%.1f", /*-2.1*/m_configS.dTmpOffset);
		p->second.strSend+=csBuf;


		//humi_offset
		p->second.strSend+=",\"humi_offset\":";
		StringCchPrintfA(csBuf, MAX_PATH, "%.1f", /*10.0*/m_configS.dHumiOffset);
		p->second.strSend+=csBuf;

		//////////////////////
		p->second.strSDErrSend=p->second.strSend;


		/*
		PS : PM25´«¸ÐÆ÷µÄGold SampleµÄÖµk1,  ¿ÕÆø¹ûÊµ²âÖµk2(kaman)
		»Ø´«µÄÖµ Kx= k1/k2  
		*/
		//pm2.5_k1
		double	d=( (double)(this->m_SDValue[PM25]) ) / (p->second.dPM25Mana);


		p->second.dPM25K1=d;
		p->second.dPM25K2=d;
		p->second.dPM25K3=d;
		p->second.dPM25K3=d;
		p->second.dPM25K4=d;
		p->second.dPM25K5=d;
		p->second.dPM25K6=d;
		p->second.dPM25K7=d;
		p->second.dPM25K8=d;
		p->second.dPM25K9=d;
		p->second.dPM25K10=d;

		CF_OutputDBGInfo("%d %f %f \r\n", m_SDValue[PM25], p->second.dPM25Mana, d);///

	{
		//pm25_k1~pm25_k10
			for(int i=0; i<10; i++)
			{
				StringCchPrintfA(csBuf, MAX_PATH, ",\"pm2.5_k_%d\":",i);
				p->second.strSend+=csBuf;
				StringCchPrintfA(csBuf, MAX_PATH, "%.1f",d);
				p->second.strSend+=csBuf;
			}

			/////////////////////////////////////////½«pm25KÖµÖÃ1,·ÀÖ¹±¬±í
			for(int i=0; i<10; i++)
			{
				StringCchPrintfA(csBuf, MAX_PATH, ",\"pm2.5_k_%d\":",i);
				p->second.strSDErrSend+=csBuf;
				StringCchPrintfA(csBuf, MAX_PATH, "%.1f",1.0);
				p->second.strSDErrSend+=csBuf;
			}
	}

		//pm25_offset_1~pm25_offset_10
// 		for(int i=0; i<10; i++)
// 		{
// 			StringCchPrintfA(csBuf, MAX_PATH, ",\"pm2.5_offset_%d\":", i);
// 			strTmp+=csBuf;
// 			StringCchPrintfA(csBuf, MAX_PATH, "%.1f",/*p->second.nPM25Offset1*/0.0);
// 			strTmp+=csBuf;
// 		}

		//nCh2oK1~nCh2oK5
		for(int i=0; i<5; i++)
		{
			StringCchPrintfA(csBuf, MAX_PATH, ",\"ch2o_k_%d\":", i);
			strTmp+=csBuf;

// 			if( i==0)
// 			{
// 				StringCchPrintfA(csBuf, MAX_PATH, "%f",p->second.dCh2oK1);
// 			}
// 			else if( i==1)
// 			{
// 				StringCchPrintfA(csBuf, MAX_PATH, "%f",p->second.dCh2oK2);
// 			}
// 			else if( i==2)
// 			{
// 				StringCchPrintfA(csBuf, MAX_PATH, "%f",p->second.dCh2oK3);
// 			}
// 			else if( i==3)
// 			{
// 				StringCchPrintfA(csBuf, MAX_PATH, "%f",p->second.dCh2oK4);		
// 			}
// 			else if( i==4)
// 			{
// 				StringCchPrintfA(csBuf, MAX_PATH, "%f",p->second.dCh2oK5);
// 			}
			
			StringCchPrintfA(csBuf, MAX_PATH, "%f", 1.0);
			strTmp+=csBuf;
		}

		//nCh2oOffset1~nCh2oOffset5
		for(int i=0; i<5; i++)
		{
			StringCchPrintfA(csBuf, MAX_PATH, ",\"ch2o_offset_%d\":", i);
			strTmp+=csBuf;

			if( i==0)
			{
				StringCchPrintfA(csBuf, MAX_PATH, "%d",p->second.dCh2oOffset1);
			}
			else if( i==1)
			{
				StringCchPrintfA(csBuf, MAX_PATH, "%d",p->second.dCh2oOffset2);
			}
			else if( i==2)
			{
				StringCchPrintfA(csBuf, MAX_PATH, "%d",p->second.dCh2oOffset3);
			}
			else if( i==3)
			{
				StringCchPrintfA(csBuf, MAX_PATH, "%d",p->second.dCh2oOffset4);		
			}
			else if( i==4)
			{
				StringCchPrintfA(csBuf, MAX_PATH, "%d",p->second.dCh2oOffset5);
			}

			strTmp+=csBuf;
		}


		/*
		CO2´«¸ÐÆ÷µÄGold SampleµÄÖµk1,  ¿ÕÆø¹ûÊµ²âÖµk2
		»Ø´«µÄÖµ CO2_Offset = k2-k1  
		*/
		//p->second.nCo2Offset=m_SDValue[CO2]-p->second.nCo2;
		//co2_offset
		strTmp+=",\"co2_offset\":";
		StringCchPrintfA(csBuf, MAX_PATH, "%d", p->second.nCo2Offset);
		strTmp+=csBuf;

		//breath_swtich 0 ¹Ø±ÕºôÎüµÆ  1 ´ò¿ªºôÎüµÆ
		strTmp+=",\"breath_swtich\":";
		StringCchPrintfA(csBuf, MAX_PATH, "%d", 1);
		strTmp+=csBuf;

		//breath_color 0---×ÏÉ«   1---ÂÌÉ« 2---ºìÉ« 3---»ÆÉ« 4---À¶É« 5---ÇàÉ«	
		strTmp+=",\"breath_color\":";
		StringCchPrintfA(csBuf, MAX_PATH, "%d", 3);
		strTmp+=csBuf;

		//voice
		strTmp+=",\"voice\":";
		StringCchPrintfA(csBuf, MAX_PATH, "%d", p->second.nVoice);
		strTmp+=csBuf;

		//PSN
// 		p->second.strSend+=",\"psn\":\"";
// 		p->second.strSend+=p->second.strPsn;


		//err 0x0000----------ÎÞ´íÎó
		strTmp+=",\"err\":";
		StringCchPrintfA(csBuf, MAX_PATH, "%d", 0);
		strTmp+=csBuf;

		//PSN
		strTmp+=",\"macs\":[{\"mac\":\"";
		strTmp+=p->second.strMac;
		strTmp+="\"}]}}";

		p->second.strSend+=strTmp;
		p->second.strSDErrSend+=strTmp;
	}
}


//VOID			CStandard2Dlg::EnunAllDataAndSD()
VOID			CStandard2Dlg::GetSDDatasAndFormation(AIRSD2& airDes)
{
	/*
	{"p":"udp_set_config","param":{"temp_offset":0.0,"humi_offset":0.0,"pm2.5_k1":1.0,"pm2.5_k2":1.0,"pm2.5_k3":1.0,"pm2.5_offset_1":0,
	"pm2.5_offset_2":0,"pm2.5_offset_3":0,"co2_offset":0,"breath_swtich":1,"breath_color":1,"voice":20,"psn":"MJBM15120301817",
	"err":123,"macs":[{"mac":"C893464E2520"},{"mac":"C893464E0F26"},{"mac":"C893464E29D7"},{"mac":"C893464E2734"}]}}
	*/
	string	strTmp;
	char	csBuf[MAX_PATH]={0};


	//for(MAPAIRSD2::iterator p=this->m_mapAirSD.begin(); p!=this->m_mapAirSD.end(); p++)
	{
		strTmp.clear();

		airDes.dPM25K1=0.0;
		airDes.dPM25K2=0.0;
		airDes.dPM25K3=0.0;

		airDes.strSend="{\"p\":\"udp_set_config\",\"param\":{\"temp_offset\":";


		/*ÎÂ¶ÈÆ«ÒÆÖµ£º-2.10 ÉãÊÏ¶È    Êª¶ÈÆ«ÒÆÁ¿£º+10.0*/
		//temp_offset
		StringCchPrintfA(csBuf, MAX_PATH, "%.1f", /*-2.1*/m_configS.dTmpOffset);
		airDes.strSend+=csBuf;


		//humi_offset
		airDes.strSend+=",\"humi_offset\":";
		StringCchPrintfA(csBuf, MAX_PATH, "%.1f", /*10.0*/m_configS.dHumiOffset);
		airDes.strSend+=csBuf;

		//////////////////////
		airDes.strSDErrSend=airDes.strSend;


		/*
		PS : PM25´«¸ÐÆ÷µÄGold SampleµÄÖµk1,  ¿ÕÆø¹ûÊµ²âÖµk2(kaman)
		»Ø´«µÄÖµ Kx= k1/k2  
		*/
		//pm2.5_k1
		double	d=( (double)(this->m_SDValue[PM25]) ) / (airDes.dPM25Mana);

		//d=1.0;
		airDes.dPM25K1=d;
		airDes.dPM25K2=d;
		airDes.dPM25K3=d;
		airDes.dPM25K3=d;
		airDes.dPM25K4=d;
		airDes.dPM25K5=d;
		airDes.dPM25K6=d;
		airDes.dPM25K7=d;
		airDes.dPM25K8=d;
		airDes.dPM25K9=d;
		airDes.dPM25K10=d;

		CF_OutputDBGInfo(__FUNCTION__"...%d %f %f \r\n", m_SDValue[PM25], airDes.dPM25Mana, d);///

	{
		//pm25_k1~pm25_k10
			for(int i=0; i<10; i++)
			{
				StringCchPrintfA(csBuf, MAX_PATH, ",\"pm2.5_k_%d\":",i);
				airDes.strSend+=csBuf;
				StringCchPrintfA(csBuf, MAX_PATH, "%.3f",d);
				airDes.strSend+=csBuf;
			}

			/////////////////////////////////////////½«pm25KÖµÖÃ1,·ÀÖ¹±¬±í
			for(int i=0; i<10; i++)
			{
				StringCchPrintfA(csBuf, MAX_PATH, ",\"pm2.5_k_%d\":",i);
				airDes.strSDErrSend+=csBuf;
				StringCchPrintfA(csBuf, MAX_PATH, "%.1f",1.0);
				airDes.strSDErrSend+=csBuf;
			}
	}

		//pm25_offset_1~pm25_offset_10
// 		for(int i=0; i<10; i++)
// 		{
// 			StringCchPrintfA(csBuf, MAX_PATH, ",\"pm2.5_offset_%d\":", i);
// 			strTmp+=csBuf;
// 			StringCchPrintfA(csBuf, MAX_PATH, "%.1f",/*p->second.nPM25Offset1*/0.0);
// 			strTmp+=csBuf;
// 		}

		//nCh2oK1~nCh2oK5
		for(int i=0; i<5; i++)
		{
			StringCchPrintfA(csBuf, MAX_PATH, ",\"ch2o_k_%d\":", i);
			strTmp+=csBuf;

// 			if( i==0)
// 			{
// 				StringCchPrintfA(csBuf, MAX_PATH, "%f",p->second.dCh2oK1);
// 			}
// 			else if( i==1)
// 			{
// 				StringCchPrintfA(csBuf, MAX_PATH, "%f",p->second.dCh2oK2);
// 			}
// 			else if( i==2)
// 			{
// 				StringCchPrintfA(csBuf, MAX_PATH, "%f",p->second.dCh2oK3);
// 			}
// 			else if( i==3)
// 			{
// 				StringCchPrintfA(csBuf, MAX_PATH, "%f",p->second.dCh2oK4);		
// 			}
// 			else if( i==4)
// 			{
// 				StringCchPrintfA(csBuf, MAX_PATH, "%f",p->second.dCh2oK5);
// 			}
			
			StringCchPrintfA(csBuf, MAX_PATH, "%f", 1.0);
			strTmp+=csBuf;
		}

		//nCh2oOffset1~nCh2oOffset5
		for(int i=0; i<5; i++)
		{
			StringCchPrintfA(csBuf, MAX_PATH, ",\"ch2o_offset_%d\":", i);
			strTmp+=csBuf;

			if( i==0)
			{
				StringCchPrintfA(csBuf, MAX_PATH, "%d",airDes.dCh2oOffset1);
			}
			else if( i==1)
			{
				StringCchPrintfA(csBuf, MAX_PATH, "%d",airDes.dCh2oOffset2);
			}
			else if( i==2)
			{
				StringCchPrintfA(csBuf, MAX_PATH, "%d",airDes.dCh2oOffset3);
			}
			else if( i==3)
			{
				StringCchPrintfA(csBuf, MAX_PATH, "%d",airDes.dCh2oOffset4);		
			}
			else if( i==4)
			{
				StringCchPrintfA(csBuf, MAX_PATH, "%d",airDes.dCh2oOffset5);
			}

			strTmp+=csBuf;
		}


		/*
		CO2´«¸ÐÆ÷µÄGold SampleµÄÖµk1,  ¿ÕÆø¹ûÊµ²âÖµk2
		»Ø´«µÄÖµ CO2_Offset = k2-k1  
		*/
		//p->second.nCo2Offset=m_SDValue[CO2]-p->second.nCo2;
		//co2_offset
		strTmp+=",\"co2_offset\":";
		StringCchPrintfA(csBuf, MAX_PATH, "%d", airDes.nCo2Offset);
		strTmp+=csBuf;

		//breath_swtich 0 ¹Ø±ÕºôÎüµÆ  1 ´ò¿ªºôÎüµÆ
		strTmp+=",\"breath_swtich\":";
		StringCchPrintfA(csBuf, MAX_PATH, "%d", 1);
		strTmp+=csBuf;

		//breath_color 0---×ÏÉ«   1---ÂÌÉ« 2---ºìÉ« 3---»ÆÉ« 4---À¶É« 5---ÇàÉ«	
		strTmp+=",\"breath_color\":";
		StringCchPrintfA(csBuf, MAX_PATH, "%d", 3);
		strTmp+=csBuf;

		//voice
		strTmp+=",\"voice\":";
		StringCchPrintfA(csBuf, MAX_PATH, "%d", airDes.nVoice);
		strTmp+=csBuf;

		//PSN
// 		p->second.strSend+=",\"psn\":\"";
// 		p->second.strSend+=p->second.strPsn;


		//err 0x0000----------ÎÞ´íÎó
		strTmp+=",\"err\":";
		StringCchPrintfA(csBuf, MAX_PATH, "%d", 0);
		strTmp+=csBuf;

		//PSN
		strTmp+=",\"macs\":[{\"mac\":\"";
		strTmp+=airDes.strMac;
		strTmp+="\"}]}}";

		airDes.strSend+=strTmp;
		airDes.strSDErrSend+=strTmp;
	}
}


VOID			CStandard2Dlg::SaveJYFormationFile(MAPAIRSD2::iterator p, string strResult, BOOL bPass)
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

		strJYFormatFileContent+="TesterDesc|±ê¶¨¹¤Õ¾\r\nTesterID|0002\r\n";
		strJYFormatFileContent+="PN|";
		strJYFormatFileContent+=p->second.strPsn;
		strJYFormatFileContent+="Line|0001\r\nOperatorNum|0002\r\nTestDate|";
		strJYFormatFileContent+=csBuf;
		strJYFormatFileContent+="\r\n";
		strJYFormatFileContent+="Barcode|";
		strJYFormatFileContent+=p->second.strPsn;
		strJYFormatFileContent+="Detail item\r\n";
		strJYFormatFileContent+=strResult;


		CFileHelper::_()->SaveJYLogInfo(TP_STAND, AIRUS2S, (PCHAR)strJYFormatFileContent.c_str(), strJYFormatFileContent.length()+1);
	}
}


VOID			CStandard2Dlg::SaveJYFormationFileEx(AIRSD2 as, string strResult, BOOL bPass)
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

		strJYFormatFileContent+="TesterDesc|±ê¶¨¹¤Õ¾\r\nTesterID|0002\r\n";
		strJYFormatFileContent+="PN|";
		strJYFormatFileContent+=as.strPsn;
		strJYFormatFileContent+="Line|0001\r\nOperatorNum|0002\r\nTestDate|";
		strJYFormatFileContent+=csBuf;
		strJYFormatFileContent+="\r\n";
		strJYFormatFileContent+="Barcode|";
		strJYFormatFileContent+=as.strPsn;
		strJYFormatFileContent+="Detail item\r\n";
		strJYFormatFileContent+=strResult;


		CFileHelper::_()->SaveJYLogInfo(TP_STAND, AIRUS2S, (PCHAR)strJYFormatFileContent.c_str(), strJYFormatFileContent.length()+1);
	}
}

BOOL 			CStandard2Dlg::CheckSDDatas()
{
	INT			nErr=0;
	BOOL		bRet=FALSE;
	WCHAR		wcsBuf[MAX_PATH]={0};
	wstring		wstrDes;
	
	//´æ´¢½òÑÇ¸ñÊ½ÎÄ¼þ
	BOOL			bJYPass=FALSE;
	string			strJYTemp;

	for(MAPAIRSD2::iterator p=m_mapRetAirSD.begin(); p!=m_mapRetAirSD.end(); p++)
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

		
		if( abs( ( (double)(p->second.nPM25)-((double)m_SDValue[PM25]) ) / ((double)m_SDValue[PM25]) ) >m_configS.dPm25Error )
		{
			StringCchPrintfW(wcsBuf, MAX_PATH, L"%s(%f), PM25:%d,	ref PM25:%d \r\n", 
											L" PM2.5¶ÁÊýÎó²î´ó",  m_configS.dPm25Error, p->second.nPM25,  m_SDValue[PM25] );
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

		if( p->second.dPM25K1<0.2 ||
			p->second.dPM25K1>5.0)
		{
			StringCchPrintfW(wcsBuf, MAX_PATH, L"%s, PM25 K:%f \r\n", 
				L" ±ê¶¨ÏµÊýKÖµÒì³££¨0.2<>5£©", p->second.dPM25K1);
			this->SetShowInfo(wcsBuf);
			nErr+=0x00002000;

			strJYTemp="±ê¶¨ÏµÊýKÖµ|0|0|0|Fail\r\n";
		}

		
		if( abs(m_SDValue[CO2] - p->second.nCo2) > m_configS.nCo2)
		{
			StringCchPrintfW(wcsBuf, MAX_PATH, L"%s(%d), co2:%d, ref co2:%d \r\n", 
												L" CO2¶ÁÊýÎó²î´ó",  m_configS.nCo2, p->second.nCo2, m_SDValue[CO2] );
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
		if( abs(p->second.dTemp- dTmp) > m_configS.nTMP)
		{
			StringCchPrintfW(wcsBuf, MAX_PATH, L"%s(%d), temp:%f, ref temp:%f \r\n", 
										L" ÎÂ¶È¶ÁÊýÎó²î´ó", m_configS.nTMP, p->second.dTemp,  dTmp);
			this->SetShowInfo(wcsBuf);
			nErr+=0x40;

			strJYTemp="ÎÂ¶È¶ÁÊýÎó²î|0|0|0|Fail\r\n";
		}

		double	dHumi=((double)m_SDValue[HUMITY])/10;
		if(  abs((p->second.dHumi-dHumi)) > m_configS.nHumi)
		{
			StringCchPrintfW(wcsBuf, MAX_PATH, L"%s(%d), humi:%f, ref humi:%f \r\n", 
									L" Êª¶È¶ÁÊýÎó²î´ó", m_configS.nHumi,p->second.dHumi, dHumi );
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
			this->m_nPassNum++;//³É¹¦+1
			this->m_nTotalNum++;

			bJYPass=TRUE;
			p->second.bIsSuccess=TRUE;

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
			this->m_nFailNum++;//Ê§°Ü+1
			this->m_nTotalNum++;

			bJYPass=FALSE;
			p->second.bIsSuccess=FALSE;
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

		//µ±±ê¶¨´íÎóÊ±£¬·¢ËÍ´ËUDP°ü£¬±£Ö¤PM25 KÖµºãÎª1.
		{
			strSend=m_mapAirSD[p->first].strSDErrSend;

			strKey=",\"err\":";
			pos=strSend.find(strKey);

			if( pos!=string::npos)
			{
				size_t end=strSend.find(',', pos+1);
				if( end!=string::npos)
				{
					strSend.replace(pos+strKey.length(), end-pos-strKey.length(), csBuf);
				}
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

			OutputDebugStringA("++++++++++++++++++++++++++++++++++\r\n");
			OutputDebugStringA(strSend.c_str());
			OutputDebugStringA(m_mapAirSD[p->first].strSDErrSend.c_str());
			OutputDebugStringA("++++++++++++++++++++++++++++++++++\r\n");

			m_mapAirSD[p->first].strSDErrSend=strSend;
		}
		//

		//save as JY formation
		this->SaveJYFormationFile(p, strJYTemp, bJYPass);
	}

	return bRet;
}


BOOL			CStandard2Dlg::CheckSDDatasEx(AIRSD2& as)
{
	INT			nErr=0;
	BOOL		bRet=FALSE;
	WCHAR		wcsBuf[MAX_PATH]={0};
	wstring		wstrDes;

	//´æ´¢½òÑÇ¸ñÊ½ÎÄ¼þ
	BOOL			bJYPass=FALSE;
	string			strJYTemp;

	//mac
	wstring		wstrMac=CF_A2W(as.strMac);

	{
		nErr=0;
		wstrDes=L"-->Check(";
		wstrDes+=wstrMac;
		wstrDes+=L")";
		this->SetShowInfo(wstrDes.c_str());


		// 		if(  abs(p->second.nPm25BaseLine-p->second.nPm25Noise) > 60 )
		// 		{
		// 			StringCchPrintfW(wcsBuf, MAX_PATH, L"%s, PM25 base line:%d, PM25 noise:%d \r\n", 
		// 											L" PM2.5»ùÏßÆ«ÒÆ¹ý´ó(60)", p->second.nPm25BaseLine, p->second.nPm25Noise);
		// 			this->SetShowInfo(wcsBuf);
		// 			nErr+=0x01;
		// 		}


		if( abs( ( (double)(as.nPM25)-((double)m_SDValue[PM25]) ) / ((double)m_SDValue[PM25]) ) >m_configS.dPm25Error )
		{
			StringCchPrintfW(wcsBuf, MAX_PATH, L"%s(%f), PM25:%d,	ref PM25:%d \r\n", 
				L" PM2.5¶ÁÊýÎó²î´ó",  m_configS.dPm25Error, as.nPM25,  m_SDValue[PM25] );
			this->SetShowInfo(wcsBuf);
			nErr+=0x02;

			strJYTemp="PM2.5¶ÁÊýÎó²î|0|0|0|Fail\r\n";
		}



		if( as.dPM25Mana==0)
		{
			StringCchPrintfW(wcsBuf, MAX_PATH, L"%s, PM25 mana:%f \r\n", 
				L" PM2.5¶ÁÊýÎªÁã", as.dPM25Mana);
			this->SetShowInfo(wcsBuf);
			nErr+=0x04;

			strJYTemp="PM2.5¶ÁÊýÎªÁã|0|0|0|Fail\r\n";
		}

		if( as.dPM25K1<0.2 ||
			as.dPM25K1>5.0)
		{
			StringCchPrintfW(wcsBuf, MAX_PATH, L"%s, PM25 K:%f \r\n", 
				L" ±ê¶¨ÏµÊýKÖµÒì³££¨0.2<>5£©", as.dPM25K1);
			this->SetShowInfo(wcsBuf);
			nErr+=0x00002000;

			strJYTemp="±ê¶¨ÏµÊýKÖµ|0|0|0|Fail\r\n";
		}


		if( abs(m_SDValue[CO2] - as.nCo2) > m_configS.nCo2)
		{
			StringCchPrintfW(wcsBuf, MAX_PATH, L"%s(%d), co2:%d, ref co2:%d \r\n", 
				L" CO2¶ÁÊýÎó²î´ó",  m_configS.nCo2, as.nCo2, m_SDValue[CO2] );
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
		if( abs(as.dTemp- dTmp) > m_configS.nTMP)
		{
			StringCchPrintfW(wcsBuf, MAX_PATH, L"%s(%d), temp:%f, ref temp:%f \r\n", 
				L" ÎÂ¶È¶ÁÊýÎó²î´ó", m_configS.nTMP, as.dTemp,  dTmp);
			this->SetShowInfo(wcsBuf);
			nErr+=0x40;

			strJYTemp="ÎÂ¶È¶ÁÊýÎó²î|0|0|0|Fail\r\n";
		}

		double	dHumi=((double)m_SDValue[HUMITY])/10;
		if(  abs((as.dHumi-dHumi)) > m_configS.nHumi)
		{
			StringCchPrintfW(wcsBuf, MAX_PATH, L"%s(%d), humi:%f, ref humi:%f \r\n", 
				L" Êª¶È¶ÁÊýÎó²î´ó", m_configS.nHumi,as.dHumi, dHumi );
			this->SetShowInfo(wcsBuf);
			nErr+=0x80;

			strJYTemp="Êª¶È¶ÁÊýÎó²î|0|0|0|Fail\r\n";
		}

		{
			StringCchPrintfW(wcsBuf, MAX_PATH, L" errID:%d\r\n",  nErr );
			this->SetShowInfo(wcsBuf);
		}


		//¸ù¾ÝerrÉèÖÃºôÎüµÆÑÕÉ«
		BYTE	bErrID=0;
		string strSend;

		if( !nErr)
		{
			bErrID=1;//ºôÎüµÆÂÌÉ«
			this->m_nPassNum++;//³É¹¦+1
			this->m_nTotalNum++;

			bJYPass=TRUE;
			as.bIsSuccess=TRUE;

			strSend=as.strSend;

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
			this->m_nFailNum++;//Ê§°Ü+1
			this->m_nTotalNum++;

			bJYPass=FALSE;
			as.bIsSuccess=FALSE;

			//±ê¶¨Ê§°Ü£¬·¢ËÍpm25_KÖµºãÎª1£¬·ÀÖ¹±¬±í  V11.0.1.37 ½Úµã±ê¶¨£¬È¥µô¸Ã¹¦ÄÜ¡£
			//strSend=as.strSDErrSend;
			strSend=as.strSend;
		}


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

		//ÉèÖÃºôÎüµÆÑÕÉ«
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

		as.strSend=strSend;

		if( nErr)
		{
			OutputDebugStringA("++++++++++++++++++++++++++++++++++\r\n");
			OutputDebugStringA(strSend.c_str());
			OutputDebugStringA(as.strSDErrSend.c_str());
			OutputDebugStringA("++++++++++++++++++++++++++++++++++\r\n");
		}

		//save as JY formation
		this->SaveJYFormationFileEx(as, strJYTemp, bJYPass);
	}

	return bRet;
}

VOID			CStandard2Dlg::SetShowInfo(LPCTSTR wcsBuf, BOOL	bSpecial)
{
	string		str;
	wstring		wstr;
	if( wcsBuf)
	{
		if(!bSpecial)
		{
			m_listReport->InsertItem(m_nListReportIndex++, (LPCTSTR)wcsBuf);

			//auto scroll
			int nCount = m_listReport->GetItemCount();
			if (nCount > 0)
			{
				m_listReport->EnsureVisible(nCount-1, FALSE); 
			}
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


// BOOL			CStandard2Dlg::SendUDPEntrySD(BOOL	bEntry)
// {
// 	BOOL		bRet=FALSE;
// 	WCHAR		wcsBuf[MAX_PATH]={0};
// 	wstring		wstrDes;
// 	string		strSend;
// 	/*
// 		{"p":"udp_enter_mode","param":{"enable":1,"show_screen":1,"breath_swtich":1,"breath_color":0,"macs":
// 		[
// 		{"mac":"C893464E2731"},
// 		{"mac":"C893464E2520"},
// 		{"mac":"C893464E29D7"},
// 		{"mac":"C893464E0F26"}
// 		]}}
// 		*/
// 
// 	if( bEntry )
// 	{
// 		wstrDes=L"{\"p\":\"udp_enter_mode\",\"param\":{\"enable\":1,\"show_screen\":1,\"breath_swtich\":1,\"breath_color\":0,\"macs\":[";
// 	}
// 	else
// 	{
// 		wstrDes=L"{\"p\":\"udp_enter_mode\",\"param\":{\"enable\":0,\"show_screen\":1,\"breath_swtich\":1,\"breath_color\":0,\"macs\":[";
// 	}
// 
// 
// 	int nCount=1;
// 	for(MAPAIRSD2::iterator p=m_mapRecvAirSD.begin(); p!=m_mapRecvAirSD.end(); p++)
// 	{
// 		wstrDes+=L"{\"mac\":\"";
// 		wstrDes+=p->first.c_str();
// 		wstrDes+=L"\"}";
// 		if( (++nCount)<=m_mapRecvAirSD.size())
// 		{
// 			wstrDes+=L",";
// 		}
// 	}
// 	wstrDes+=L"]}}";
// 
// 	strSend=CF_W2A(wstrDes);
// 	//OutputDebugStringA(strSend.c_str());///
// 
// 
// 	//****************UDP boardcast
// 	for(int i=0; i<SD_MAXUPDBOARCOSTNUM; i++)
// 	{
// 		//send
// 		bRet=UDP_SendBoardcastUsed((PBYTE)strSend.c_str(), strSend.length(), SD_UDPPORTSEND);
// 		Sleep(700);
// 	}
// 
// 	if( !bRet )
// 	{
// 		StringCchPrintfW(wcsBuf, MAX_PATH, L"Send enter setting mode command err(ID:0x%x), count:%d.\r\n", GetLastError(), SD_MAXUPDBOARCOSTNUM );
// 		SetShowInfo(wcsBuf);
// 	}
// 	else
// 	{
// 		SetShowInfo(L"Send enter setting mode command(about xs).");
// 		SetShowInfo(wstrDes.c_str(), !this->m_isShow);
// 	}
// 				
// 	return	bRet;
// }


BOOL			CStandard2Dlg::SendUDPEntrySD(BOOL	bEntry)
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


//	int nCount=1;
	for(MAPAIRSD2::iterator p=m_mapRecvAirSD.begin(); p!=m_mapRecvAirSD.end(); p++)
	{
		if( bEntry )
		{
			wstrDes=L"{\"p\":\"udp_enter_mode\",\"param\":{\"enable\":1,\"show_screen\":1,\"breath_swtich\":1,\"breath_color\":0,\"macs\":[";
		}
		else
		{
			wstrDes=L"{\"p\":\"udp_enter_mode\",\"param\":{\"enable\":0,\"show_screen\":1,\"breath_swtich\":1,\"breath_color\":0,\"macs\":[";
		}

		wstrDes+=L"{\"mac\":\"";
		wstrDes+=p->first.c_str();
		wstrDes+=L"\"}";
// 		if( (++nCount)<=m_mapRecvAirSD.size())
// 		{
// 			wstrDes+=L",";
// 		}

		wstrDes+=L"]}}";
		SetShowInfo(wstrDes.c_str(), !this->m_isShow);

		strSend=CF_W2A(wstrDes);
		//****************UDP boardcast
		for(int i=0; i<SD_MAXUPDBOARCOSTNUM; i++)
		{
			//send
			bRet=UDP_SendBoardcastUsed((PBYTE)strSend.c_str(), strSend.length(), SD_UDPPORTSEND);
			Sleep(700);
		}

		//Ã¿¸öÉè±¸´í¿ª1s
		Sleep(1000);
	}

	SetShowInfo(L"Send enter setting mode command(about xs).");
				
	return	bRet;
}



BOOL			CStandard2Dlg::RecvUDPDatas(MAPAIRSD2*	mapV, BOOL	bCheckMode)
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
		dwElapse=m_configS.nWaitDeviceValue*m_mapAirSD.size()*this->m_configS.nRetryNum;
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

VOID			CStandard2Dlg::CalGSMeanValue()
{
	CHAR	csBuf[MAX_PATH]={0};
	INT		nRetryNum=m_configS.nRetryNum;

	m_SDValue[PM25]=0;
	m_SDValue[CO2]=0;
	m_SDValue[CH2O]=0;
	m_SDValue[TEMP]=0;
	m_SDValue[HUMITY]=0;


	for(int i=0; i<nRetryNum; i++)
	{
		m_SDValue[PM25]+=m_SDMeanValue[i][PM25];
		m_SDValue[CO2]+=m_SDMeanValue[i][CO2];
		m_SDValue[CH2O]+=m_SDMeanValue[i][CH2O];
		m_SDValue[TEMP]+=m_SDMeanValue[i][TEMP];
		m_SDValue[HUMITY]+=m_SDMeanValue[i][HUMITY];
	}
	
	
	m_SDValue[PM25]/=nRetryNum;
	m_SDValue[CO2]/=nRetryNum;
	m_SDValue[CH2O]/=nRetryNum;
	m_SDValue[TEMP]/=nRetryNum;
	m_SDValue[HUMITY]/=nRetryNum;

	StringCchPrintfA(csBuf, MAX_PATH, "PM2.5:	%d ug/m3\r\nCO2:	%d ppm\r\nCH2O:	%f mg/m3\r\nTemp:	%f ¡æ\r\nHumity:	%f%%\r\n", 
						m_SDValue[PM25], m_SDValue[CO2],
						((FLOAT)(m_SDValue[CH2O])/1000.0),
						((FLOAT)(m_SDValue[TEMP])/10), ((FLOAT)(m_SDValue[HUMITY])/10) );

	//SetDlgItemTextA(this->m_hWnd, IDC_EDIT_ORIINFO, csBuf);
	string	str=csBuf;
	SetShowInfo((LPCTSTR)CF_A2WEx(str).c_str());
}


VOID			CStandard2Dlg::ForamtionMacStr(wstring& wstr)
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


unsigned __stdcall _ThreadForCheck2(LPVOID p)
{
	CStandard2Dlg* pThis=(CStandard2Dlg*)p;
	
	WCHAR	wcsBuf[1024]={0};
	wstring	wstrDes;
	wstring	wstrMacS;

	//Ã¿´Î·¢ËÍUDPÊ±¼ä¼ä¸ô
	const INT	nSendElapse=600;

	AIRSD2	as;

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
	for(int i=0; i<pThis->m_configS.nRetryNum; i++)
	{
		pThis->m_mapReadBuf[i].clear();
	}
	
	pThis->m_strFileContent.clear();
	
	pThis->m_strFileContent=pThis->m_strGSV;

	InitAirDatas(&as);
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

					pThis->ForamtionMacStr(wstrDes);
					if( wstrDes.length()==SD_MACADDRLEN)
					{
						pThis->m_mapAirSD.insert(std::pair<wstring, AIRSD2>(wstrDes, as));
						pThis->m_mapRetAirSD.insert(std::pair<wstring, AIRSD2>(wstrDes, as));

						for(int i=0; i<pThis->m_configS.nRetryNum; i++)
						{
							/**
							2sÖ÷»ú²»´øco2´«¸ÐÆ÷£¬ÎªÁË¼æÈÝÐÔ¼°ºóÐøÅÐ¶Ï£¨co2ÓëGSÖµ²îÖµ²»ÄÜ´óÓÚ300£©ÕýÈ·£¬
							½«¶ÁÈ¡ÉèÖÃÖµ³õÊ¼»¯³ÉGSÖµ
							*/
							AIRSD2 ass;
							InitAirDatas(&ass);
							ass.bStatus=FALSE;
							ass.dTemp=((FLOAT)(pThis->m_SDValue[TEMP])/10.0);
							ass.dHumi=((FLOAT)(pThis->m_SDValue[HUMITY])/10.0);
							ass.nCo2=pThis->m_SDValue[CO2];
							ass.nPM25=pThis->m_SDValue[PM25];
							ass.dch2oOriginal=((FLOAT)(pThis->m_SDValue[CH2O])/1000.0);

							//CF_OutputDBGInfo(__FUNCTION__".................co2:%d\r\n", ass.nCo2);///

							pThis->m_mapReadBuf[i].insert(std::pair<wstring, AIRSD2>(wstrDes, ass));
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

		Sleep(pThis->m_configS.nWaitValue);
		
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

		for(MAPAIRSD2::iterator p=pThis->m_mapAirSD.begin(); p!=pThis->m_mapAirSD.end(); p++)
		{
			wstrMacS=L"--->send(";
			wstrMacS+=CF_A2W(p->second.strMac).c_str();
			wstrMacS+=L")";

			//OutputDebugStringA(p->second.strSend.c_str());///////

			pThis->SetShowInfo(wstrMacS.c_str());
			pThis->SetShowInfo(CF_A2W(p->second.strSend).c_str(), !pThis->m_isShow);


			for(int r=0; r<SENDSDCOMMANDNUM; r++)
			{
				if( !UDP_SendBoardcastUsed((PBYTE)p->second.strSend.c_str(), p->second.strSend.length(), SD_UDPPORTSEND) )
				{
					CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"Send UDP boardcast err(ID:0x%x).\r\n", GetLastError() );
					//break;
				}

				Sleep(nSendElapse);
			}
		}


		///////////////////
		//MessageBoxA(pThis->m_hWnd, "ok", "ok", MB_YESNO);
		///////////////////


		//**************************check data
		//½«ÉÏ´ÎµÄ»º³åÇøÖÃFALSE£¬Ö»ÒªÇó¶ÁÈ¡Ò»´Î
		for(MAPAIRSD2::iterator p=(pThis->m_mapReadBuf[0]).begin(); p!=pThis->m_mapReadBuf[0].end(); p++)
		{
			p->second.bStatus=FALSE;
		}
		//recv
		if( !pThis->RecvUDPDatas(&pThis->m_mapRetAirSD, TRUE) )
		{
			break;
		}

		printfMap2(pThis->m_mapAirSD);
		printfMap2(pThis->m_mapRetAirSD);

		pThis->CheckSDDatas();
		for(MAPAIRSD2::iterator p=pThis->m_mapRetAirSD.begin(); p!=pThis->m_mapRetAirSD.end(); p++)
		{
			wstrMacS=L"--->send(";
			wstrMacS+=CF_A2W(p->second.strMac).c_str();
			wstrMacS+=L")";

			//OutputDebugStringA(p->second.strSend.c_str());///////

			pThis->SetShowInfo(wstrMacS.c_str());
			pThis->SetShowInfo(CF_A2W(p->second.strSend).c_str(), !pThis->m_isShow);

			for(int r=0; r<SENDSDCOMMANDNUM; r++)
			{
				if( !UDP_SendBoardcastUsed((PBYTE)p->second.strSend.c_str(), p->second.strSend.length(), SD_UDPPORTSEND) )
				{
					CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"Send UDP boardcast err(ID:0x%x).\r\n", GetLastError() );
					//break;
				}

				Sleep(nSendElapse);
			}

			//Èç¹û±ê¶¨Ê§°Ü£¬½«pm25kÖµÖÃ1£¬·ÀÖ¹ÔÙ´Î±ê¶¨ºó³ö´í
			if( !p->second.bIsSuccess)
			{
				CF_OutputDBGInfo(__FUNCTION__"..sd err............\r\n");
				MAPAIRSD2::iterator pF=pThis->m_mapAirSD.find(CF_A2W(p->second.strMac));
				if( pF==pThis->m_mapAirSD.end())
				{
					CF_OutputDBGInfo(__FUNCTION__"..not found %ws\r\n", p->second.strMac.c_str());
				}
				else
				{
					CF_OutputDBGInfo(__FUNCTION__"..err:send k==1. %s\r\n", pF->second.strSDErrSend.c_str());
					UDP_SendBoardcastUsed((PBYTE)pF->second.strSDErrSend.c_str(), pF->second.strSDErrSend.length(), SD_UDPPORTSEND);
					Sleep(nSendElapse);
					UDP_SendBoardcastUsed((PBYTE)pF->second.strSDErrSend.c_str(), pF->second.strSDErrSend.length(), SD_UDPPORTSEND);
					Sleep(nSendElapse);
				}
			}
		}

		pThis->SetShowInfo(L"±ê¶¨½áÊø.");
		
	} while (0);
	
	
	pThis->SetBtnStatus(TRUE);

	pThis->ShowResultInfo();

	CFileHelper::_()->SaveLogInfo(TP_STAND, (PCHAR)pThis->m_strFileContent.c_str(), pThis->m_strFileContent.length()+1, AIRUS2S);
	OutputDebugStringA(__FUNCTION__"...thread end.\r\n");

	return 1;
}

void CStandard2Dlg::OnBnClickedButtonStart()
{
	this->ShowMCUVer("");
	m_hReadCore=(HANDLE)_beginthreadex(NULL, NULL, /*_ThreadForCheck2*/_ThreadCore, this, NULL, NULL);
}


HBRUSH CStandard2Dlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

	if( pWnd->GetDlgCtrlID()==IDC_EDIT_RESLUTNUM)
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(21,21,90));
	}
	return hbr;
}


void CStandard2Dlg::OnEnChangeEditMac()
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


BOOL CStandard2Dlg::PreTranslateMessage(MSG* pMsg)
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

void CStandard2Dlg::OnBnClickedBtnClear()
{
	SetDlgItemTextW(IDC_EDIT_MAC, L"");
}


BOOL			CStandard2Dlg::ReadFromIni()
{
	BOOL		bRet=FALSE;

	do 
	{
		CHAR	csRetBuf[MAX_PATH]={0};
		CHAR	csBuf[MAX_PATH]={0};
		CF_GetModuleDir(csBuf);
		StringCchCatA(csBuf, MAX_PATH, "MTConfig.ini");

		if(!GetPrivateProfileStringA("2", "GS_refresh", NULL, csRetBuf, MAX_PATH, csBuf))
		{
			break;
		}
		m_configS.nGsFrequence=strtol(csRetBuf, NULL, 10);

		if(!GetPrivateProfileStringA("2", "ReadNum", NULL, csRetBuf, MAX_PATH, csBuf))
		{
			break;
		}
		m_configS.nRetryNum=strtol(csRetBuf, NULL, 10);

		if(!GetPrivateProfileStringA("2", "WaitValue", NULL, csRetBuf, MAX_PATH, csBuf))
		{
			break;
		}
		m_configS.nWaitValue=strtol(csRetBuf, NULL, 10);


		if(!GetPrivateProfileStringA("2", "WaitDeviceValue", NULL, csRetBuf, MAX_PATH, csBuf))
		{
			break;
		}
		m_configS.nWaitDeviceValue=strtol(csRetBuf, NULL, 10);

		if(!GetPrivateProfileStringA("2", "PM25_error", NULL, csRetBuf, MAX_PATH, csBuf))
		{
			break;
		}
		m_configS.dPm25Error=strtod(csRetBuf, NULL);

		if(!GetPrivateProfileStringA("2", "CO2", NULL, csRetBuf, MAX_PATH, csBuf))
		{
			break;
		}
		m_configS.nCo2=strtol(csRetBuf, NULL, 10);

		if(!GetPrivateProfileStringA("2", "TMP", NULL, csRetBuf, MAX_PATH, csBuf))
		{
			break;
		}
		m_configS.nTMP=strtol(csRetBuf, NULL, 10);

		if(!GetPrivateProfileStringA("2", "HUMI", NULL, csRetBuf, MAX_PATH, csBuf))
		{
			break;
		}
		m_configS.nHumi=strtol(csRetBuf, NULL, 10);


		if(!GetPrivateProfileStringA("2", "TmpOffset", NULL, csRetBuf, MAX_PATH, csBuf))
		{
			break;
		}
		m_configS.dTmpOffset=strtod(csRetBuf, NULL);

		if(!GetPrivateProfileStringA("2", "HumiOffset", NULL, csRetBuf, MAX_PATH, csBuf))
		{
			break;
		}
		m_configS.dHumiOffset=strtod(csRetBuf, NULL);

		bRet=TRUE;
		CF_OutputDBGInfoEx(DBG_OUT, NULL, "gs:%d  retry num:%d  wait value:%d	wait device value:%d  pm25:%f  co2:%d  tmp:%d  humi:%d  tmp offset:%f  humi offset:%f\r\n", 
				m_configS.nGsFrequence, m_configS.nRetryNum, m_configS.nWaitValue, m_configS.nWaitDeviceValue,
				m_configS.dPm25Error, m_configS.nCo2,
				m_configS.nTMP, m_configS.nHumi,
				m_configS.dTmpOffset,
				m_configS.dHumiOffset);

	} while (FALSE);


	return		bRet;
}


BOOL			CStandard2Dlg::InitOrDestoryParams(BOOL	bInit)
{
	BOOL	bRet=FALSE;

	if( bInit)
	{
		do 
		{
			if( m_configS.nRetryNum>READDATAMAXNUM	||
				m_configS.nRetryNum<0 )
			{
				CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...invalid params(nReryNum:%d)\r\n",  m_configS.nRetryNum	);
				break;
			}

			m_SDMeanValue=new SHORT[m_configS.nRetryNum][5];
			m_mapReadBuf=new MAPAIRSD2[m_configS.nRetryNum];

			if(!m_SDMeanValue || !m_mapReadBuf)
			{
				if( m_SDMeanValue)
				{
					delete[] m_SDMeanValue;
					m_SDMeanValue=NULL;
				}
				if( m_mapReadBuf)
				{
					delete[] m_mapReadBuf;
					m_mapReadBuf=NULL;
				}

				CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...allocate memory err(0x%x), len:%d\r\n",  
											GetLastError(), m_configS.nRetryNum);
				break;
			}

			ZeroMemory((PBYTE)m_SDMeanValue, sizeof(SHORT)*m_configS.nRetryNum*5);

			bRet=TRUE;

		} while (FALSE);
	}
	else
	{
		if( m_SDMeanValue)
		{
			delete[] m_SDMeanValue;
			m_SDMeanValue=NULL;
		}
		if( m_mapReadBuf)
		{
			delete[] m_mapReadBuf;
			m_mapReadBuf=NULL;
		}

		bRet=TRUE;
	}

	return	bRet;
}


VOID			CStandard2Dlg::ShowResultInfo()
{
	CHAR	csBuf[MAX_PATH]={0};
	StringCchPrintfA(csBuf, MAX_PATH, "±ê¶¨³É¹¦:%.4d¸ö\r\n±ê¶¨Ê§°Ü:%.4d¸ö\r\n±ê¶¨×ÜÊý:%.4d¸ö\r\n", m_nPassNum, m_nFailNum, m_nTotalNum);
	OutputDebugStringA(csBuf);
	SetDlgItemTextA(this->m_hWnd, IDC_EDIT_RESLUTNUM, csBuf);
}

BOOL			CStandard2Dlg::IsAllDeviceSDEnd()
{
	BOOL		bRet=TRUE;

	for(MAPAIRSD2::iterator r=this->m_mapRecvAirSD.begin(); r!=this->m_mapRecvAirSD.end(); r++)
	{			
		if( !r->second.bSDEnd)
		{
			bRet=FALSE;
			break;
		}
	}

	return bRet;
}



VOID			CStandard2Dlg::ShowMCUVer(LPSTR p, BOOL bNew)
{
	if(bNew)
	{
		SetDlgItemTextA(this->m_hWnd, IDC_EDIT_MCUVER, "");
	}
	else
	{
		CEdit *edit = (CEdit*)GetDlgItem(IDC_EDIT_MCUVER);
		int length = edit->GetWindowTextLength();
		int Infor_length = strlen(p);

		edit->SetSel(length,length);
		edit->ReplaceSel((LPCTSTR)p);
	}
}