// LedTestDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MT.h"
#include "Led2TestDlg.h"
#include "afxdialogex.h"
#include "SerialPortHelper.h"

#include <strsafe.h>

// CLed2TestDlg 对话框

IMPLEMENT_DYNAMIC(CLed2TestDlg, CDialogEx)

CLed2TestDlg::CLed2TestDlg(LPVOID p, TESTERTYPE type, BYTE	mode, CWnd* pParent /*=NULL*/)
	: CDialogEx(CLed2TestDlg::IDD, pParent),
m_Result(MJ_NORMAL),
m_nTotalNum(0),
m_nPassNum(0),
m_nFailNum(0)
{
	m_testType=type;
	m_bMaster=mode;
	m_pParent=(CMT2Dlg*)p;
}

CLed2TestDlg::~CLed2TestDlg()
{
	CloseSerialPort(m_hDevice);
}

void CLed2TestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLed2TestDlg, CDialogEx)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BTN_START, &CLed2TestDlg::OnBnClickedBtnStart)
END_MESSAGE_MAP()


// CLed2TestDlg 消息处理程序


HBRUSH CLed2TestDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	if( pWnd->GetDlgCtrlID()==IDC_STATIC)
	{
		pDC->SelectObject(&m_font);
		pDC->SetBkMode(TRANSPARENT);

		if( m_Result==MJ_SUCCESS )
		{
			pDC->SetTextColor(SUCCESS_COLOR);
		}
		else if(m_Result==MJ_FAILED)
		{
			pDC->SetTextColor(FAILED_COLOR);
		}
		else
		{
			pDC->SetTextColor(NORMAL_COLOR);
		}
		
	}
	else if( pWnd->GetDlgCtrlID()==IDC_EDIT_TOTAL)
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(LISTREPORT_COLOR);
	}
	return hbr;
}


BOOL CLed2TestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//init font
	m_font.CreateFont( 55,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_HEAVY,                   // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		DEFAULT_CHARSET,           // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		TEXT("黑体"));             // lpszFacename

	//top window
	CRect	rect;
	int		scrWidth=0;
	int		scrHeight=0;
	GetWindowRect(&rect);
	scrWidth=GetSystemMetrics(SM_CXSCREEN);
	scrHeight=GetSystemMetrics(SM_CYSCREEN);
	::SetWindowPos(this->m_hWnd, HWND_TOPMOST, (scrWidth-rect.Width())/2, (scrHeight-rect.Height())/2, rect.Width(), rect.Height(), SWP_SHOWWINDOW);


	//communicate with device(com),get com num
	this->RefreshDeviceList();

	//set title
	switch(m_testType)
	{
	case TP_LED:
		{
			//主机灯板检测模式
			if( m_bMaster==1)
			{
				SetWindowTextA(this->m_hWnd, "master Led-Test(2s)");
			}
			else if( m_bMaster==2)//节点灯板检测模式
			{
				SetWindowTextA(this->m_hWnd, "slave Led-Test(2s)");
			}			
		}
		break;

	case TP_WIFI:
		{
			SetWindowTextA(this->m_hWnd, "Wifi-Test(2s)");
		}
		break;

	case TP_MB:
		{
			SetWindowTextA(this->m_hWnd, "MainBoard-Test(2s)");
		}
		break;
	}

	CF_SetTitleWithVersion(this->m_hWnd);

	this->SetResultInfo();
	
	return TRUE; 
}

VOID			CLed2TestDlg::RefreshDeviceListR()
{
	wstring		wstr;
	VECSTR		vs;

	CloseSerialPort(m_hDevice);

	EnumCommPort(vs);
	for(VECSTR::iterator p=vs.begin(); p!=vs.end(); p++)
	{
		wstr=p->c_str();
		OpenSerialPort(wstr, m_hDevice);

		if(	m_testType==P_GetType(m_hDevice))
		{
			break;
		}

		CloseSerialPort(m_hDevice);
		m_hDevice=INVALID_HANDLE_VALUE;
	}

	this->ShowInfo((PWCHAR)m_wstrDes.c_str(), TRUE);
}


VOID			CLed2TestDlg::RefreshDeviceList()
{
	wstring		wstr;
	wstring		wstrTmp;
	VECSTR		vs;
	CComboBox*	pComBox=(CComboBox*)GetDlgItem(IDC_COMBO_LIST);
	BOOL		bFind=FALSE;
	TESTERTYPE	type=TP_NONE;

	EnumCommPort(vs);
	for(VECSTR::iterator p=vs.begin(); p!=vs.end(); p++)
	{
		wstr=p->c_str();
		OpenSerialPort(wstr, m_hDevice);

		type=P_GetType(m_hDevice);


		if(	m_testType==type)
		{
			bFind=TRUE;

			if( TP_LED==type)
			{
				//主机灯板检测模式
				if( m_bMaster==1)
				{
					wstrTmp=L"(master led mode).\r\n";
				}
				else if( m_bMaster==2)//节点灯板检测模式
				{
					wstrTmp=L"(slave led mode).\r\n";
				}
			}
			else if(TP_WIFI==type)
			{
				wstrTmp=L"(Wifi mode).\r\n";
			}
			else if(TP_MB==type)
			{
				wstrTmp=L"(main board mode).\r\n";
			}
			break;
		}

		
		CloseSerialPort(m_hDevice);
		m_hDevice=INVALID_HANDLE_VALUE;
	}

	if( bFind)
	{
		pComBox->AddString(wstr.c_str());
		pComBox->SetCurSel(0);
		m_wstrDes=L"open serial port ";
		m_wstrDes+=wstr.c_str();
		m_wstrDes+=L" success";
		m_wstrDes+=wstrTmp.c_str();
	}
	else
	{
		m_wstrDes=L"not found device.\r\n";
		this->SetBtnStatus(FALSE);
	}

	this->ShowInfo((PWCHAR)m_wstrDes.c_str(), TRUE);
}

unsigned WINAPI		_threadForCheck2(LPVOID p)
{
	CLed2TestDlg*	pThis=(CLed2TestDlg*)p;
	CHAR			csBuf[MAX_PATH]={0};
	SYSTEMTIME		st;
	
	if( pThis)
	{
		pThis->m_strJYTemp.clear();
		pThis->m_strJYFormatFileContent.clear();
		pThis->m_strFileContent.clear();
		pThis->m_strFileContent=pThis->m_strPSN;

		pThis->m_Result=MJ_NORMAL;
		pThis->SetResult();
		pThis->ShowInfo((PWCHAR)(pThis->m_wstrDes.c_str()), TRUE);

		GetLocalTime(&st);
		StringCchPrintfA(csBuf, MAX_PATH, "%.4d-%.2d-%.2d %.2d:%.2d:%.2d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

		switch(pThis->m_testType)
		{
		case TP_LED:
			{
				//主机灯板
				if( pThis->m_bMaster==1)
				{
					pThis->LedMasterCheck();
				}
				else if(pThis->m_bMaster==2)//节点灯板
				{
					pThis->LedSlaveCheck();
				}
				
				/*
				TestResult|F-灯板、wifi、主板、标定
				TesterDesc|灯板、wifi、主板、标定测试站
				TesterID|led、wifi、mb，sd
				PN|sn
				Line|1
				OperatorNum|0001
				TestDate|time
				Barcode|sn
				Detail item
				*/
				if( pThis->m_Result==MJ_SUCCESS)
				{
					pThis->m_strJYFormatFileContent="TestResult|P\r\n";
				}
				else
				{
					pThis->m_strJYFormatFileContent="TestResult|F\r\n";
				}

				//主机灯板
				if( pThis->m_bMaster==1)
				{
					pThis->m_strJYFormatFileContent+="TesterDesc|主机灯板测试站\r\nTesterID|0002\r\n";
				}
				else if(pThis->m_bMaster==2)//节点灯板
				{
					pThis->m_strJYFormatFileContent+="TesterDesc|节点灯板测试站\r\nTesterID|0002\r\n";
				}
				
				pThis->m_strJYFormatFileContent+="PN|";
				pThis->m_strJYFormatFileContent+=pThis->m_strPSN;
				pThis->m_strJYFormatFileContent+="Line|0001\r\nOperatorNum|0002\r\nTestDate|";
				pThis->m_strJYFormatFileContent+=csBuf;
				pThis->m_strJYFormatFileContent+="\r\n";
				pThis->m_strJYFormatFileContent+="Barcode|";
				pThis->m_strJYFormatFileContent+=pThis->m_strPSN;
				pThis->m_strJYFormatFileContent+="Detail item\r\n";
				pThis->m_strJYFormatFileContent+=pThis->m_strJYTemp;
			}
			break;

		case TP_MB:
			{
				pThis->MainBoardCheck();

				if( pThis->m_Result==MJ_SUCCESS)
				{
					pThis->m_strJYFormatFileContent="TestResult|P\r\n";
				}
				else
				{
					pThis->m_strJYFormatFileContent="TestResult|F\r\n";
				}

				pThis->m_strJYFormatFileContent+="TesterDesc|主板测试站\r\nTesterID|0002\r\n";
				pThis->m_strJYFormatFileContent+="PN|";
				pThis->m_strJYFormatFileContent+=pThis->m_strPSN;
				pThis->m_strJYFormatFileContent+="Line|0001\r\nOperatorNum|0002\r\nTestDate|";
				pThis->m_strJYFormatFileContent+=csBuf;
				pThis->m_strJYFormatFileContent+="\r\n";
				pThis->m_strJYFormatFileContent+="Barcode|";
				pThis->m_strJYFormatFileContent+=pThis->m_strPSN;
				pThis->m_strJYFormatFileContent+="Detail item\r\n";
				pThis->m_strJYFormatFileContent+=pThis->m_strJYTemp;
			}
			break;

		case TP_WIFI:
			{
				pThis->WifiCheck();

				if( pThis->m_Result==MJ_SUCCESS)
				{
					pThis->m_strJYFormatFileContent="TestResult|P\r\n";
				}
				else
				{
					pThis->m_strJYFormatFileContent="TestResult|F\r\n";
				}

				pThis->m_strJYFormatFileContent+="TesterDesc|WIFI测试站\r\nTesterID|0002\r\n";
				pThis->m_strJYFormatFileContent+="PN|";
				pThis->m_strJYFormatFileContent+=pThis->m_strPSN;
				pThis->m_strJYFormatFileContent+="Line|0001\r\nOperatorNum|0002\r\nTestDate|";
				pThis->m_strJYFormatFileContent+=csBuf;
				pThis->m_strJYFormatFileContent+="\r\n";
				pThis->m_strJYFormatFileContent+="Barcode|";
				pThis->m_strJYFormatFileContent+=pThis->m_strPSN;
				pThis->m_strJYFormatFileContent+="Detail item\r\n";
				pThis->m_strJYFormatFileContent+=pThis->m_strJYTemp;
			}
			break;
		}

		pThis->SetResult();
		pThis->SetBtnStatus(TRUE);

		CFileHelper::_()->SaveLogInfo(pThis->m_testType, (PCHAR)pThis->m_strFileContent.c_str(), pThis->m_strFileContent.length()+1);
		CFileHelper::_()->SaveJYLogInfo(pThis->m_testType, AIRUS2S, (PCHAR)pThis->m_strJYFormatFileContent.c_str(), pThis->m_strJYFormatFileContent.length()+1);
	}

	return 1;
}



/*
PCB板型号	机种代码	生产厂家（津亚）	PCB号	生产批次(生产日期)	流水号	1S样例				2代样例				Sport样例			FUN样例
主板		MJ			T					M		160506				X0001	MJBM16050600001		MJTM160506A0001		
灯阵板		MJ			T					L		160506				X00001	MJBL16050600001		MJTL160506A0001		
Wifi板		MJ			T					W		160506				X0001	MJBW16050600001		MJTW160506A0001		MJTW160506S0001		MJTW160506F0001
PM2.5节点	MJ			T					P		160506				X0001	MJTP160506N0001		
CO2节点		MJ			T					C		160506				X0001	MJTC160506N0001		
CH2O节点	MJ			T					H		160506				X0001	MJTH160506N0001		

*/
BOOL			CLed2TestDlg::CheckSN()
{
	CHAR	csBuf[MAX_PATH]={0};
	BOOL	bRet=FALSE;

	GetDlgItemTextA(this->m_hWnd, IDC_EDIT_PSN, csBuf, MAX_PATH);
	m_strPSN=csBuf;
	m_strPSN+="\r\n";

	if( TP_LED==m_testType)
	{
		if( !memcmp(csBuf, "MJTL", 4) &&
			strlen(csBuf)==15 )
		{
			bRet=TRUE;
		}
	}
	else if( TP_WIFI==m_testType)
	{
		if( !memcmp(csBuf, "MJTW", 4) &&
			strlen(csBuf)==15 )
		{
			bRet=TRUE;
		}
	}
	else if( TP_MB==m_testType)
	{
		if( !memcmp(csBuf, "MJTM", 4) &&
			strlen(csBuf)==15 )
		{
			bRet=TRUE;
		}
	}

	return bRet;
}

void CLed2TestDlg::OnBnClickedBtnStart()
{
	this->SetBtnStatus(FALSE);

	if( this->CheckSN())
	{
		this->RefreshDeviceListR();
		CloseHandle((HANDLE)_beginthreadex(NULL, NULL, _threadForCheck2, this, NULL, NULL));
	}
	else
	{
		this->ShowInfo(L"请输入合法PSN码.\r\n", FALSE);
		this->SetBtnStatus(TRUE);
	}
}


VOID  CLed2TestDlg::ShowInfo(PWCHAR pInfo, BOOL	bNew)
{
	wstring		wstr;
	if( bNew)
	{
		SetDlgItemTextW(IDC_EDIT_RESULT, pInfo);
	}
	else
	{
		CEdit* edit = (CEdit*)GetDlgItem(IDC_EDIT_RESULT);
		int nTextLen = edit->GetWindowTextLength();
		int nStrLen = wcslen(pInfo);

		edit->SetSel(nTextLen,nTextLen);
		edit->ReplaceSel(pInfo);
	}

	wstr=pInfo;
	m_strFileContent+=CF_W2AEx(wstr);
}


//在这里显示统计信息
VOID			CLed2TestDlg::SetResult()
{
	switch(m_Result)
	{
	case MJ_NORMAL:
		{
			SetDlgItemTextW(IDC_STATIC, L"...");
		}
		break;
	case MJ_FAILED:
		{
			m_nFailNum++;
			m_nTotalNum++;
			SetDlgItemTextW(IDC_STATIC, L"FAIL");
			this->ShowInfo(L"result: fail.\r\n", FALSE);
		}
		break;
	case MJ_SUCCESS:
		{
			m_nPassNum++;
			m_nTotalNum++;
			SetDlgItemTextW(IDC_STATIC, L"PASS");
			this->ShowInfo(L"result: pass.\r\n", FALSE);
		}
		break;
	}

	this->SetResultInfo();
}

//节点灯板检测模式
BOOL			CLed2TestDlg::LedSlaveCheck()
{
	BOOL		bRet=FALSE;
	CHAR		retDatas[SHORTLEN]={0};
	
	//等待5s
	Sleep(5*1000);

	if( IDYES==MessageBoxA(this->m_hWnd, "灯板是否按照红、绿、蓝、白顺序闪烁?", "standard", MB_YESNO))
	{
// 		if( P_GetCheckResult(this->m_hDevice, m_testType, (PBYTE)retDatas, SHORTLEN) )
// 		{
// 			/*
// 			-1	U14传感器故障
// 			-2 	U15传感器故障
// 			-3	U14/U15传感器故障
// 			0	正常
// 			*/
// 			m_Result=MJ_FAILED;
// 			switch(retDatas[1])
// 			{
// 			case 0:
// 				{
// 					this->ShowInfo(L"设备正常.\r\n", FALSE);
// 					this->m_strJYTemp+="U14传感器";
// 					this->m_strJYTemp+="|0|0|0|Pass\r\n";
// 					this->m_strJYTemp+="U15传感器";
// 					this->m_strJYTemp+="|0|0|0|Pass\r\n";
// 					this->m_strJYTemp+="U14/U15传感器";
// 					this->m_strJYTemp+="|0|0|0|Pass\r\n";
// 					this->m_strJYTemp+="发送数据";
// 					this->m_strJYTemp+="|0|0|0|Pass\r\n";
// 					this->m_strJYTemp+="灯板亮度";
// 					this->m_strJYTemp+="|0|0|0|Pass\r\n";
// 					this->m_strJYTemp+="灯板全亮";
// 					this->m_strJYTemp+="|0|0|0|Pass\r\n";
// 					m_Result=MJ_SUCCESS;
// 				}
// 				break;
// 			case -1:
// 				{
// 					this->ShowInfo(L"U14传感器故障.\r\n", FALSE);
// 					this->m_strJYTemp+="U14传感器";
// 					this->m_strJYTemp+="|0|0|0|Fail\r\n";
// 				}
// 				break;
// 			case -2:
// 				{
// 					this->ShowInfo(L"U15传感器故障.\r\n", FALSE);
// 					this->m_strJYTemp+="U15传感器";
// 					this->m_strJYTemp+="|0|0|0|Fail\r\n";
// 				}
// 				break;
// 			case -3:
// 				{
// 					this->ShowInfo(L"U14/U15传感器故障.\r\n", FALSE);
// 					this->m_strJYTemp+="U14/U15传感器";
// 					this->m_strJYTemp+="|0|0|0|Fail\r\n";
// 				}
// 				break;
// 			}
// 		}
// 		else
// 		{
// 			this->ShowInfo(L"发送数据异常.\r\n", FALSE);
// 			this->m_strJYTemp+="发送数据";
// 			this->m_strJYTemp+="|0|0|0|Fail\r\n";
// 			m_Result=MJ_FAILED;
// 		}

		this->ShowInfo(L"设备正常.\r\n", FALSE);
		this->m_strJYTemp+="灯板目检";
		this->m_strJYTemp+="|0|0|0|Pass\r\n";
		m_Result=MJ_SUCCESS;
	}
	else
	{
		//灯板目测异常
		this->ShowInfo(L"Led灯板目测异常(没有按照红、绿、蓝、白顺序闪烁).\r\n", FALSE);
		this->m_strJYTemp+="灯板目检";
		this->m_strJYTemp+="|0|0|0|Fail\r\n";
		m_Result=MJ_FAILED;
	}


	return	bRet;
}

BOOL			CLed2TestDlg::LedMasterCheck()
{
	BOOL		bRet=FALSE;
	CHAR		retDatas[SHORTLEN]={0};
	
	if( IDYES==MessageBoxA(this->m_hWnd, "Led灯板是否正常?", "standard", MB_YESNO))
	{
		if( IDYES==MessageBoxA(this->m_hWnd, "灯板亮度是否一致?", "standard", MB_YESNO))
		{
			if( P_GetCheckResult(this->m_hDevice, m_testType, (PBYTE)retDatas, SHORTLEN) )
			{
				/*
				-1	U14传感器故障
				-2 	U15传感器故障
				-3	U14/U15传感器故障
				0	正常
				*/
				m_Result=MJ_FAILED;
				switch(retDatas[1])
				{
				case 0:
					{
						this->ShowInfo(L"设备正常.\r\n", FALSE);
						this->m_strJYTemp+="U14传感器";
						this->m_strJYTemp+="|0|0|0|Pass\r\n";
						this->m_strJYTemp+="U15传感器";
						this->m_strJYTemp+="|0|0|0|Pass\r\n";
						this->m_strJYTemp+="U14/U15传感器";
						this->m_strJYTemp+="|0|0|0|Pass\r\n";
						this->m_strJYTemp+="发送数据";
						this->m_strJYTemp+="|0|0|0|Pass\r\n";
						this->m_strJYTemp+="灯板亮度";
						this->m_strJYTemp+="|0|0|0|Pass\r\n";
						this->m_strJYTemp+="灯板全亮";
						this->m_strJYTemp+="|0|0|0|Pass\r\n";
						m_Result=MJ_SUCCESS;
					}
					break;
				case -1:
					{
						this->ShowInfo(L"U14传感器故障.\r\n", FALSE);
						this->m_strJYTemp+="U14传感器";
						this->m_strJYTemp+="|0|0|0|Fail\r\n";
					}
					break;
				case -2:
					{
						this->ShowInfo(L"U15传感器故障.\r\n", FALSE);
						this->m_strJYTemp+="U15传感器";
						this->m_strJYTemp+="|0|0|0|Fail\r\n";
					}
					break;
				case -3:
					{
						this->ShowInfo(L"U14/U15传感器故障.\r\n", FALSE);
						this->m_strJYTemp+="U14/U15传感器";
						this->m_strJYTemp+="|0|0|0|Fail\r\n";
					}
					break;
				}
			}
			else
			{
				this->ShowInfo(L"发送数据异常.\r\n", FALSE);
				this->m_strJYTemp+="发送数据";
				this->m_strJYTemp+="|0|0|0|Fail\r\n";
				m_Result=MJ_FAILED;
			}
		}
		else
		{
			//灯板亮度不一致
			this->ShowInfo(L"Led灯板目测异常(灯板亮度不一致).\r\n", FALSE);
			this->m_strJYTemp+="灯板亮度";
			this->m_strJYTemp+="|0|0|0|Fail\r\n";
			m_Result=MJ_FAILED;
		}
	}
	else
	{
		//led灯板不全亮
		this->ShowInfo(L"Led灯板目测异常(灯板不全亮).\r\n", FALSE);
		this->m_strJYTemp+="灯板全亮";
		this->m_strJYTemp+="|0|0|0|Fail\r\n";
		m_Result=MJ_FAILED;
	}

	return	bRet;
}

BOOL			CLed2TestDlg::UpdaterMB(string strWorkDir)
{
	BOOL		bRet=FALSE;

	do 	
	{
		STARTUPINFOA			si={0};
		PROCESS_INFORMATION		pi={0};
		string		strFWPath;
		string		strBinFile=strWorkDir;
		string		strFWNewFile=strWorkDir;
		strFWPath=strWorkDir;
		strFWPath+="jflash_mainboard_hw.bat";
		strBinFile+="main20160222.bin";
		strFWNewFile+="main.bin";


		if( !PathFileExistsA((LPCSTR)strFWPath.c_str()) )
		{
			this->ShowInfo(L"没有找到测试版程序文件(jflash_mainboard_hw.bat).\r\n", FALSE);
			break;
		}
		if( !PathFileExistsA((LPCSTR)strBinFile.c_str()) )
		{
			this->ShowInfo(L"没有找到测试版程序文件(main20160222.bin).\r\n", FALSE);
			break;
		}


		//删除上次的main.bin文件
		DeleteFileA(strFWNewFile.c_str());

		//合并新版固件程序+psn  main.bin
		PBYTE	pFileContent=NULL;
		DWORD	dwOutLen=0;
		CF_GetFielContentEx((PCHAR)strBinFile.c_str(), &pFileContent, dwOutLen);
		memmove((pFileContent+MB_PSNOFFSETADDR), m_strPSN.c_str(), m_strPSN.length());
		CF_SaveFile((PCHAR)strFWNewFile.c_str(), pFileContent, dwOutLen, CREATE_ALWAYS);
		if( pFileContent)
		{
			delete[] pFileContent;
		}

		if( !PathFileExistsA((LPCSTR)strFWNewFile.c_str()) )
		{
			this->ShowInfo(L"没有找到测试版程序文件(main.bin).\r\n", FALSE);
			break;
		}

		this->ShowInfo(L"waiting for JFlash working...\r\n", FALSE);
		CreateProcessA(strFWPath.c_str(), NULL, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, strWorkDir.c_str(), &si, &pi);
		if( WAIT_TIMEOUT==WaitForSingleObject(pi.hProcess, MB_JFLASHTIMEOUTV) )
		{
			this->ShowInfo(L"JFlash 超时，请检查治具是否在工作.\r\n", FALSE);
			break;
		}

		CloseHandle(pi.hProcess);

		bRet=TRUE;
	}while(FALSE);

	return		bRet;
}


BOOL			CLed2TestDlg::MainBoardCheck()
{
	BOOL		bRet=FALSE;
	string		strFWPath;
	string		strWorkDir;
	wstring		strResult;
	CHAR		csBuf[MAX_PATH]={0};
	STARTUPINFOA			si={0};
	PROCESS_INFORMATION		pi={0};
	BYTE					retDatas[SHORTLEN]={0};
	LONG		nResult=0;

	si.cb=sizeof(STARTUPINFOA);
	si.dwFlags=STARTF_USESHOWWINDOW;
	si.wShowWindow=SW_HIDE;
	//第一步，升级量产版固件
	CF_GetModuleDir(csBuf);
	strFWPath=csBuf;
	strFWPath+="FW\\2s\\MB\\";
	strWorkDir=strFWPath;
	strFWPath+="jflash_mainboard_hw_test.bat";///

	do 
	{
		if( !PathFileExistsA((LPCSTR)strFWPath.c_str()) )
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...filePath(%s) not existing.\r\n", strFWPath.c_str());
			this->ShowInfo(L"没有找到测试版程序文件(测试).\r\n", FALSE);

			this->m_strJYTemp+="固件程序";
			this->m_strJYTemp+="|0|0|0|Fail\r\n";
			break;
		}

		this->ShowInfo(L"waiting for JFlash working...\r\n", FALSE);
		CreateProcessA(strFWPath.c_str(), NULL, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, strWorkDir.c_str(), &si, &pi);
		if( WAIT_TIMEOUT==WaitForSingleObject(pi.hProcess, MB_JFLASHTIMEOUTV) )
		{
			this->ShowInfo(L"JFlash 超时，请检查治具是否在工作.\r\n", FALSE);

			this->m_strJYTemp+="JFlash";
			this->m_strJYTemp+="|0|0|0|Fail\r\n";
			break;
		}

		CloseHandle(pi.hProcess);
		this->ShowInfo(L"checking...\r\n", FALSE);

		if( P_GetCheckResult(m_hDevice, m_testType, retDatas, SHORTLEN))
		{
			/*
			1  ： Boost 电源 +5V故障
			2  :  3.3V DC/DC 电源故障
			4  :  CO2传感器电压异常
			8  :  PM2.5传感器电源异常
			16 :  传感器风扇供电异常
			32：  TF卡供电异常
			64：  锂电池电压异常
			128： 呼吸灯红色通道异常
			256： 呼吸灯绿色通道异常
			512： 呼吸灯蓝色通道异常
			1024：电量计或加速度传感器异常
			*/
			m_Result=MJ_FAILED;
			nResult=*(PLONG)(&retDatas[1]);
			nResult=ntohl(nResult);
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...MB ret value:%d(%d %d %d %d).\r\n", 
												nResult, retDatas[1], retDatas[2], retDatas[3], retDatas[4]);///

			
			strResult.clear();
			if(nResult&(1))
			{
				strResult+=L"Boost 电源 +5V故障\r\n";
				this->m_strJYTemp+="Boost 电源 +5V";
				this->m_strJYTemp+="|0|0|0|Fail\r\n";
			}
			if(nResult&(2))
			{
				strResult+=L"3.3V DC/DC 电源故障\r\n";
				this->m_strJYTemp+="3.3V DC/DC 电源";
				this->m_strJYTemp+="|0|0|0|Fail\r\n";
			}
			if(nResult&(4))
			{
				strResult+=L"CO2传感器电压异常\r\n";
				this->m_strJYTemp+="CO2传感器电压";
				this->m_strJYTemp+="|0|0|0|Fail\r\n";
			}
			if(nResult&(8))
			{
				strResult+=L"PM2.5传感器电源异常\r\n";
				this->m_strJYTemp+="PM2.5传感器电源";
				this->m_strJYTemp+="|0|0|0|Fail\r\n";
			}
			if(nResult&(16))
			{
				strResult+=L"传感器风扇供电异常\r\n";
				this->m_strJYTemp+="传感器风扇供电";
				this->m_strJYTemp+="|0|0|0|Fail\r\n";
			}
			if(nResult&(32))
			{
				strResult+=L"TF卡供电异常\r\n";
				this->m_strJYTemp+="TF卡供电";
				this->m_strJYTemp+="|0|0|0|Fail\r\n";
			}
			if(nResult&(64))
			{
				strResult+=L"锂电池电压异常\r\n";
				this->m_strJYTemp+="锂电池电压";
				this->m_strJYTemp+="|0|0|0|Fail\r\n";
			}
			if(nResult&(128))
			{
				strResult+=L"呼吸灯红色通道异常\r\n";
				this->m_strJYTemp+="呼吸灯红色通道";
				this->m_strJYTemp+="|0|0|0|Fail\r\n";
			}
			if(nResult&(256))
			{
				strResult+=L"呼吸灯绿色通道异常\r\n";
				this->m_strJYTemp+="呼吸灯绿色通道";
				this->m_strJYTemp+="|0|0|0|Fail\r\n";
			}
			if(nResult&(512))
			{
				strResult+=L"呼吸灯蓝色通道异常\r\n";
				this->m_strJYTemp+="呼吸灯蓝色通道";
				this->m_strJYTemp+="|0|0|0|Fail\r\n";
			}
			if(nResult&(1024))
			{
				strResult+=L"电量计或加速度传感器异常\r\n";
				this->m_strJYTemp+="电量计或加速度传感器";
				this->m_strJYTemp+="|0|0|0|Fail\r\n";
			}
			if( nResult==0)
			{
				strResult=L"设备正常.\r\n";
				
				this->m_strJYTemp+="固件程序";
				this->m_strJYTemp+="|0|0|0|Pass\r\n";
				this->m_strJYTemp+="JFlash";
				this->m_strJYTemp+="|0|0|0|Pass\r\n";
				this->m_strJYTemp+="Boost 电源 +5V";
				this->m_strJYTemp+="|0|0|0|Pass\r\n";
				this->m_strJYTemp+="3.3V DC/DC 电源";
				this->m_strJYTemp+="|0|0|0|Pass\r\n";
				this->m_strJYTemp+="CO2传感器电压";
				this->m_strJYTemp+="|0|0|0|Pass\r\n";
				this->m_strJYTemp+="PM2.5传感器电源";
				this->m_strJYTemp+="|0|0|0|Pass\r\n";
				this->m_strJYTemp+="传感器风扇供电";
				this->m_strJYTemp+="|0|0|0|Pass\r\n";
				this->m_strJYTemp+="TF卡供电";
				this->m_strJYTemp+="|0|0|0|Pass\r\n";
				this->m_strJYTemp+="锂电池电压";
				this->m_strJYTemp+="|0|0|0|Pass\r\n";
				this->m_strJYTemp+="呼吸灯红色通道";
				this->m_strJYTemp+="|0|0|0|Pass\r\n";
				this->m_strJYTemp+="呼吸灯绿色通道";
				this->m_strJYTemp+="|0|0|0|Pass\r\n";
				this->m_strJYTemp+="呼吸灯蓝色通道";
				this->m_strJYTemp+="|0|0|0|Pass\r\n";
				this->m_strJYTemp+="电量计或加速度传感器";
				this->m_strJYTemp+="|0|0|0|Pass\r\n";
				this->m_strJYTemp+="主板发声功能";
				this->m_strJYTemp+="|0|0|0|Pass\r\n";
				this->m_strJYTemp+="发送数据";
				this->m_strJYTemp+="|0|0|0|Pass\r\n";
			}

			this->ShowInfo((PWCHAR)strResult.c_str(), FALSE);
			if( nResult!=0)
			{
				break;
			}

			//发生检查
			if( IDYES!=MessageBoxA(this->m_hWnd, "主板是否有声音", "standard", MB_YESNO))
			{
				this->ShowInfo(L"主板发声功能异常.\r\n", FALSE);
				this->m_strJYTemp+="主板发声功能";
				this->m_strJYTemp+="|0|0|0|Fail\r\n";
				break;
			}

			//升级量产版固件
			if( this->UpdaterMB(strWorkDir) )
			{
				m_Result=MJ_SUCCESS;
			}
		}
		else
		{
			this->ShowInfo(L"发送数据异常.\r\n", FALSE);
			this->m_strJYTemp+="发送数据";
			this->m_strJYTemp+="|0|0|0|Fail\r\n";
			m_Result=MJ_FAILED;
			break;
		}


		bRet=TRUE;
	} while (FALSE);


	return	bRet;
}


BOOL			CLed2TestDlg::WifiCheck()
{
	BOOL		bRet=FALSE;
	string		strFWPath;
	string		strWorkDir;
	CHAR		csBuf[MAX_PATH]={0};
	STARTUPINFOA			si={0};
	PROCESS_INFORMATION		pi={0};
	CHAR					retDatas[SHORTLEN]={0};

	si.cb=sizeof(STARTUPINFOA);
	si.dwFlags=STARTF_USESHOWWINDOW;
	si.wShowWindow=SW_HIDE;
	//第一步，升级量产版固件
	CF_GetModuleDir(csBuf);
	strFWPath=csBuf;
	strFWPath+="FW\\2s\\WIFI\\";
	strWorkDir=strFWPath;
	strFWPath+="jflash_wifiEx.bat";

	do 
	{
		//二代没有下载固件流程
// 		if( !PathFileExistsA((LPCSTR)strFWPath.c_str()) )
// 		{
// 			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...filePath(%s) not existing.\r\n", strFWPath.c_str());
// 			this->ShowInfo(L"JFlash 超时，请检查测试文件是否存在.\r\n", FALSE);
// 			this->m_strJYTemp+="升级文件";
// 			this->m_strJYTemp+="|0|0|0|Fail\r\n";
// 			break;
// 		}
// 
// 		this->ShowInfo(L"waiting for JFlash working...\r\n", FALSE);
// 		CreateProcessA(strFWPath.c_str(), NULL, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, strWorkDir.c_str(), &si, &pi);
// 
// 		if( WAIT_TIMEOUT==WaitForSingleObject(pi.hProcess, MB_JFLASHTIMEOUTV) )
// 		{
// 			this->ShowInfo(L"JFlash 超时，请检查治具是否在工作.\r\n", FALSE);
// 			this->m_strJYTemp+="JFlash";
// 			this->m_strJYTemp+="|0|0|0|Fail\r\n";
// 			break;
// 		}


//		CloseHandle(pi.hProcess);
		this->ShowInfo(L"checking...\r\n", FALSE);

		if( P_GetCheckResult(m_hDevice, m_testType, (PBYTE)retDatas, SHORTLEN))
		{
			if( P_GetCheckResult(m_hDevice, m_testType, (PBYTE)retDatas, SHORTLEN))
			{
				m_Result=MJ_FAILED;

				switch(retDatas[1])
				{
				case -1:
					{
						this->ShowInfo(L"电压异常.\r\n", FALSE);
						this->m_strJYTemp+="电压";
						this->m_strJYTemp+="|0|0|0|Fail\r\n";
					}
					break;

				case -2:
					{
						this->ShowInfo(L"wifi模块异常（无应答）.\r\n", FALSE);
						this->m_strJYTemp+="wifi模块";
						this->m_strJYTemp+="|0|0|0|Fail\r\n";
					}
					break;

				case 0:
					{
						m_Result=MJ_SUCCESS;
						this->ShowInfo(L"设备正常.\r\n", FALSE);
 
// 						this->m_strJYTemp+="升级文件";
// 						this->m_strJYTemp+="|0|0|0|Pass\r\n";
// 						this->m_strJYTemp+="JFlash";
// 						this->m_strJYTemp+="|0|0|0|Pass\r\n";
						this->m_strJYTemp+="发送数据";
						this->m_strJYTemp+="|0|0|0|Pass\r\n";
						this->m_strJYTemp+="wifi模块";
						this->m_strJYTemp+="|0|0|0|Pass\r\n";
						this->m_strJYTemp+="电压";
						this->m_strJYTemp+="|0|0|0|Pass\r\n";
					}
					break;
				}
			}
			else
			{
				this->ShowInfo(L"发送数据异常.\r\n", FALSE);
				this->m_strJYTemp+="发送数据";
				this->m_strJYTemp+="|0|0|0|Fail\r\n";
				m_Result=MJ_FAILED;
			}
		}
		else
		{
			this->ShowInfo(L"发送数据异常.\r\n", FALSE);
			this->m_strJYTemp+="发送数据";
			this->m_strJYTemp+="|0|0|0|Fail\r\n";
			m_Result=MJ_FAILED;
		}


		bRet=TRUE;
	} while (FALSE);


	return	bRet;
}

VOID			CLed2TestDlg::SetBtnStatus(BOOL	bEnable)
{
	CButton* pBtn=(CButton*)GetDlgItem(IDC_BTN_START);
	if( pBtn)
	{
		pBtn->EnableWindow(bEnable);
	}
}

BOOL CLed2TestDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if( pMsg->message==WM_KEYDOWN)
	{
		if( pMsg->wParam==VK_ESCAPE ||
			pMsg->wParam==VK_RETURN )
		{
			return TRUE;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


VOID			CLed2TestDlg::SetResultInfo()
{
	CHAR		csBuf[MAX_PATH]={0};
	double		a=(double)m_nPassNum;
	double		b=(double)m_nTotalNum;

	sprintf_s(csBuf, MAX_PATH, "测试总数:%.4d		问题设备:%.4d	\r\n正常设备:%.4d		直通率:%d%%", 
				m_nTotalNum, m_nFailNum, m_nPassNum, 
				m_nTotalNum==0?0:(int)((a*100.00)/b) );

	SetDlgItemTextA(this->m_hWnd, IDC_EDIT_TOTAL, csBuf);
}