// LedTestDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MT.h"
#include "Led2TestDlg.h"
#include "afxdialogex.h"
#include "SerialPortHelper.h"

#include <strsafe.h>

// CLed2TestDlg �Ի���

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


// CLed2TestDlg ��Ϣ�������


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
		TEXT("����"));             // lpszFacename

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
			//�����ư���ģʽ
			if( m_bMaster==1)
			{
				SetWindowTextA(this->m_hWnd, "master Led-Test(2s)");
			}
			else if( m_bMaster==2)//�ڵ�ư���ģʽ
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
				//�����ư���ģʽ
				if( m_bMaster==1)
				{
					wstrTmp=L"(master led mode).\r\n";
				}
				else if( m_bMaster==2)//�ڵ�ư���ģʽ
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
				//�����ư�
				if( pThis->m_bMaster==1)
				{
					pThis->LedMasterCheck();
				}
				else if(pThis->m_bMaster==2)//�ڵ�ư�
				{
					pThis->LedSlaveCheck();
				}
				
				/*
				TestResult|F-�ư塢wifi�����塢�궨
				TesterDesc|�ư塢wifi�����塢�궨����վ
				TesterID|led��wifi��mb��sd
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

				//�����ư�
				if( pThis->m_bMaster==1)
				{
					pThis->m_strJYFormatFileContent+="TesterDesc|�����ư����վ\r\nTesterID|0002\r\n";
				}
				else if(pThis->m_bMaster==2)//�ڵ�ư�
				{
					pThis->m_strJYFormatFileContent+="TesterDesc|�ڵ�ư����վ\r\nTesterID|0002\r\n";
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

				pThis->m_strJYFormatFileContent+="TesterDesc|�������վ\r\nTesterID|0002\r\n";
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

				pThis->m_strJYFormatFileContent+="TesterDesc|WIFI����վ\r\nTesterID|0002\r\n";
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
PCB���ͺ�	���ִ���	�������ң����ǣ�	PCB��	��������(��������)	��ˮ��	1S����				2������				Sport����			FUN����
����		MJ			T					M		160506				X0001	MJBM16050600001		MJTM160506A0001		
�����		MJ			T					L		160506				X00001	MJBL16050600001		MJTL160506A0001		
Wifi��		MJ			T					W		160506				X0001	MJBW16050600001		MJTW160506A0001		MJTW160506S0001		MJTW160506F0001
PM2.5�ڵ�	MJ			T					P		160506				X0001	MJTP160506N0001		
CO2�ڵ�		MJ			T					C		160506				X0001	MJTC160506N0001		
CH2O�ڵ�	MJ			T					H		160506				X0001	MJTH160506N0001		

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
		this->ShowInfo(L"������Ϸ�PSN��.\r\n", FALSE);
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


//��������ʾͳ����Ϣ
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

//�ڵ�ư���ģʽ
BOOL			CLed2TestDlg::LedSlaveCheck()
{
	BOOL		bRet=FALSE;
	CHAR		retDatas[SHORTLEN]={0};
	
	//�ȴ�5s
	Sleep(5*1000);

	if( IDYES==MessageBoxA(this->m_hWnd, "�ư��Ƿ��պ졢�̡�������˳����˸?", "standard", MB_YESNO))
	{
// 		if( P_GetCheckResult(this->m_hDevice, m_testType, (PBYTE)retDatas, SHORTLEN) )
// 		{
// 			/*
// 			-1	U14����������
// 			-2 	U15����������
// 			-3	U14/U15����������
// 			0	����
// 			*/
// 			m_Result=MJ_FAILED;
// 			switch(retDatas[1])
// 			{
// 			case 0:
// 				{
// 					this->ShowInfo(L"�豸����.\r\n", FALSE);
// 					this->m_strJYTemp+="U14������";
// 					this->m_strJYTemp+="|0|0|0|Pass\r\n";
// 					this->m_strJYTemp+="U15������";
// 					this->m_strJYTemp+="|0|0|0|Pass\r\n";
// 					this->m_strJYTemp+="U14/U15������";
// 					this->m_strJYTemp+="|0|0|0|Pass\r\n";
// 					this->m_strJYTemp+="��������";
// 					this->m_strJYTemp+="|0|0|0|Pass\r\n";
// 					this->m_strJYTemp+="�ư�����";
// 					this->m_strJYTemp+="|0|0|0|Pass\r\n";
// 					this->m_strJYTemp+="�ư�ȫ��";
// 					this->m_strJYTemp+="|0|0|0|Pass\r\n";
// 					m_Result=MJ_SUCCESS;
// 				}
// 				break;
// 			case -1:
// 				{
// 					this->ShowInfo(L"U14����������.\r\n", FALSE);
// 					this->m_strJYTemp+="U14������";
// 					this->m_strJYTemp+="|0|0|0|Fail\r\n";
// 				}
// 				break;
// 			case -2:
// 				{
// 					this->ShowInfo(L"U15����������.\r\n", FALSE);
// 					this->m_strJYTemp+="U15������";
// 					this->m_strJYTemp+="|0|0|0|Fail\r\n";
// 				}
// 				break;
// 			case -3:
// 				{
// 					this->ShowInfo(L"U14/U15����������.\r\n", FALSE);
// 					this->m_strJYTemp+="U14/U15������";
// 					this->m_strJYTemp+="|0|0|0|Fail\r\n";
// 				}
// 				break;
// 			}
// 		}
// 		else
// 		{
// 			this->ShowInfo(L"���������쳣.\r\n", FALSE);
// 			this->m_strJYTemp+="��������";
// 			this->m_strJYTemp+="|0|0|0|Fail\r\n";
// 			m_Result=MJ_FAILED;
// 		}

		this->ShowInfo(L"�豸����.\r\n", FALSE);
		this->m_strJYTemp+="�ư�Ŀ��";
		this->m_strJYTemp+="|0|0|0|Pass\r\n";
		m_Result=MJ_SUCCESS;
	}
	else
	{
		//�ư�Ŀ���쳣
		this->ShowInfo(L"Led�ư�Ŀ���쳣(û�а��պ졢�̡�������˳����˸).\r\n", FALSE);
		this->m_strJYTemp+="�ư�Ŀ��";
		this->m_strJYTemp+="|0|0|0|Fail\r\n";
		m_Result=MJ_FAILED;
	}


	return	bRet;
}

BOOL			CLed2TestDlg::LedMasterCheck()
{
	BOOL		bRet=FALSE;
	CHAR		retDatas[SHORTLEN]={0};
	
	if( IDYES==MessageBoxA(this->m_hWnd, "Led�ư��Ƿ�����?", "standard", MB_YESNO))
	{
		if( IDYES==MessageBoxA(this->m_hWnd, "�ư������Ƿ�һ��?", "standard", MB_YESNO))
		{
			if( P_GetCheckResult(this->m_hDevice, m_testType, (PBYTE)retDatas, SHORTLEN) )
			{
				/*
				-1	U14����������
				-2 	U15����������
				-3	U14/U15����������
				0	����
				*/
				m_Result=MJ_FAILED;
				switch(retDatas[1])
				{
				case 0:
					{
						this->ShowInfo(L"�豸����.\r\n", FALSE);
						this->m_strJYTemp+="U14������";
						this->m_strJYTemp+="|0|0|0|Pass\r\n";
						this->m_strJYTemp+="U15������";
						this->m_strJYTemp+="|0|0|0|Pass\r\n";
						this->m_strJYTemp+="U14/U15������";
						this->m_strJYTemp+="|0|0|0|Pass\r\n";
						this->m_strJYTemp+="��������";
						this->m_strJYTemp+="|0|0|0|Pass\r\n";
						this->m_strJYTemp+="�ư�����";
						this->m_strJYTemp+="|0|0|0|Pass\r\n";
						this->m_strJYTemp+="�ư�ȫ��";
						this->m_strJYTemp+="|0|0|0|Pass\r\n";
						m_Result=MJ_SUCCESS;
					}
					break;
				case -1:
					{
						this->ShowInfo(L"U14����������.\r\n", FALSE);
						this->m_strJYTemp+="U14������";
						this->m_strJYTemp+="|0|0|0|Fail\r\n";
					}
					break;
				case -2:
					{
						this->ShowInfo(L"U15����������.\r\n", FALSE);
						this->m_strJYTemp+="U15������";
						this->m_strJYTemp+="|0|0|0|Fail\r\n";
					}
					break;
				case -3:
					{
						this->ShowInfo(L"U14/U15����������.\r\n", FALSE);
						this->m_strJYTemp+="U14/U15������";
						this->m_strJYTemp+="|0|0|0|Fail\r\n";
					}
					break;
				}
			}
			else
			{
				this->ShowInfo(L"���������쳣.\r\n", FALSE);
				this->m_strJYTemp+="��������";
				this->m_strJYTemp+="|0|0|0|Fail\r\n";
				m_Result=MJ_FAILED;
			}
		}
		else
		{
			//�ư����Ȳ�һ��
			this->ShowInfo(L"Led�ư�Ŀ���쳣(�ư����Ȳ�һ��).\r\n", FALSE);
			this->m_strJYTemp+="�ư�����";
			this->m_strJYTemp+="|0|0|0|Fail\r\n";
			m_Result=MJ_FAILED;
		}
	}
	else
	{
		//led�ư岻ȫ��
		this->ShowInfo(L"Led�ư�Ŀ���쳣(�ư岻ȫ��).\r\n", FALSE);
		this->m_strJYTemp+="�ư�ȫ��";
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
			this->ShowInfo(L"û���ҵ����԰�����ļ�(jflash_mainboard_hw.bat).\r\n", FALSE);
			break;
		}
		if( !PathFileExistsA((LPCSTR)strBinFile.c_str()) )
		{
			this->ShowInfo(L"û���ҵ����԰�����ļ�(main20160222.bin).\r\n", FALSE);
			break;
		}


		//ɾ���ϴε�main.bin�ļ�
		DeleteFileA(strFWNewFile.c_str());

		//�ϲ��°�̼�����+psn  main.bin
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
			this->ShowInfo(L"û���ҵ����԰�����ļ�(main.bin).\r\n", FALSE);
			break;
		}

		this->ShowInfo(L"waiting for JFlash working...\r\n", FALSE);
		CreateProcessA(strFWPath.c_str(), NULL, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, strWorkDir.c_str(), &si, &pi);
		if( WAIT_TIMEOUT==WaitForSingleObject(pi.hProcess, MB_JFLASHTIMEOUTV) )
		{
			this->ShowInfo(L"JFlash ��ʱ�������ξ��Ƿ��ڹ���.\r\n", FALSE);
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
	//��һ��������������̼�
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
			this->ShowInfo(L"û���ҵ����԰�����ļ�(����).\r\n", FALSE);

			this->m_strJYTemp+="�̼�����";
			this->m_strJYTemp+="|0|0|0|Fail\r\n";
			break;
		}

		this->ShowInfo(L"waiting for JFlash working...\r\n", FALSE);
		CreateProcessA(strFWPath.c_str(), NULL, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, strWorkDir.c_str(), &si, &pi);
		if( WAIT_TIMEOUT==WaitForSingleObject(pi.hProcess, MB_JFLASHTIMEOUTV) )
		{
			this->ShowInfo(L"JFlash ��ʱ�������ξ��Ƿ��ڹ���.\r\n", FALSE);

			this->m_strJYTemp+="JFlash";
			this->m_strJYTemp+="|0|0|0|Fail\r\n";
			break;
		}

		CloseHandle(pi.hProcess);
		this->ShowInfo(L"checking...\r\n", FALSE);

		if( P_GetCheckResult(m_hDevice, m_testType, retDatas, SHORTLEN))
		{
			/*
			1  �� Boost ��Դ +5V����
			2  :  3.3V DC/DC ��Դ����
			4  :  CO2��������ѹ�쳣
			8  :  PM2.5��������Դ�쳣
			16 :  ���������ȹ����쳣
			32��  TF�������쳣
			64��  ﮵�ص�ѹ�쳣
			128�� �����ƺ�ɫͨ���쳣
			256�� ��������ɫͨ���쳣
			512�� ��������ɫͨ���쳣
			1024�������ƻ���ٶȴ������쳣
			*/
			m_Result=MJ_FAILED;
			nResult=*(PLONG)(&retDatas[1]);
			nResult=ntohl(nResult);
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...MB ret value:%d(%d %d %d %d).\r\n", 
												nResult, retDatas[1], retDatas[2], retDatas[3], retDatas[4]);///

			
			strResult.clear();
			if(nResult&(1))
			{
				strResult+=L"Boost ��Դ +5V����\r\n";
				this->m_strJYTemp+="Boost ��Դ +5V";
				this->m_strJYTemp+="|0|0|0|Fail\r\n";
			}
			if(nResult&(2))
			{
				strResult+=L"3.3V DC/DC ��Դ����\r\n";
				this->m_strJYTemp+="3.3V DC/DC ��Դ";
				this->m_strJYTemp+="|0|0|0|Fail\r\n";
			}
			if(nResult&(4))
			{
				strResult+=L"CO2��������ѹ�쳣\r\n";
				this->m_strJYTemp+="CO2��������ѹ";
				this->m_strJYTemp+="|0|0|0|Fail\r\n";
			}
			if(nResult&(8))
			{
				strResult+=L"PM2.5��������Դ�쳣\r\n";
				this->m_strJYTemp+="PM2.5��������Դ";
				this->m_strJYTemp+="|0|0|0|Fail\r\n";
			}
			if(nResult&(16))
			{
				strResult+=L"���������ȹ����쳣\r\n";
				this->m_strJYTemp+="���������ȹ���";
				this->m_strJYTemp+="|0|0|0|Fail\r\n";
			}
			if(nResult&(32))
			{
				strResult+=L"TF�������쳣\r\n";
				this->m_strJYTemp+="TF������";
				this->m_strJYTemp+="|0|0|0|Fail\r\n";
			}
			if(nResult&(64))
			{
				strResult+=L"﮵�ص�ѹ�쳣\r\n";
				this->m_strJYTemp+="﮵�ص�ѹ";
				this->m_strJYTemp+="|0|0|0|Fail\r\n";
			}
			if(nResult&(128))
			{
				strResult+=L"�����ƺ�ɫͨ���쳣\r\n";
				this->m_strJYTemp+="�����ƺ�ɫͨ��";
				this->m_strJYTemp+="|0|0|0|Fail\r\n";
			}
			if(nResult&(256))
			{
				strResult+=L"��������ɫͨ���쳣\r\n";
				this->m_strJYTemp+="��������ɫͨ��";
				this->m_strJYTemp+="|0|0|0|Fail\r\n";
			}
			if(nResult&(512))
			{
				strResult+=L"��������ɫͨ���쳣\r\n";
				this->m_strJYTemp+="��������ɫͨ��";
				this->m_strJYTemp+="|0|0|0|Fail\r\n";
			}
			if(nResult&(1024))
			{
				strResult+=L"�����ƻ���ٶȴ������쳣\r\n";
				this->m_strJYTemp+="�����ƻ���ٶȴ�����";
				this->m_strJYTemp+="|0|0|0|Fail\r\n";
			}
			if( nResult==0)
			{
				strResult=L"�豸����.\r\n";
				
				this->m_strJYTemp+="�̼�����";
				this->m_strJYTemp+="|0|0|0|Pass\r\n";
				this->m_strJYTemp+="JFlash";
				this->m_strJYTemp+="|0|0|0|Pass\r\n";
				this->m_strJYTemp+="Boost ��Դ +5V";
				this->m_strJYTemp+="|0|0|0|Pass\r\n";
				this->m_strJYTemp+="3.3V DC/DC ��Դ";
				this->m_strJYTemp+="|0|0|0|Pass\r\n";
				this->m_strJYTemp+="CO2��������ѹ";
				this->m_strJYTemp+="|0|0|0|Pass\r\n";
				this->m_strJYTemp+="PM2.5��������Դ";
				this->m_strJYTemp+="|0|0|0|Pass\r\n";
				this->m_strJYTemp+="���������ȹ���";
				this->m_strJYTemp+="|0|0|0|Pass\r\n";
				this->m_strJYTemp+="TF������";
				this->m_strJYTemp+="|0|0|0|Pass\r\n";
				this->m_strJYTemp+="﮵�ص�ѹ";
				this->m_strJYTemp+="|0|0|0|Pass\r\n";
				this->m_strJYTemp+="�����ƺ�ɫͨ��";
				this->m_strJYTemp+="|0|0|0|Pass\r\n";
				this->m_strJYTemp+="��������ɫͨ��";
				this->m_strJYTemp+="|0|0|0|Pass\r\n";
				this->m_strJYTemp+="��������ɫͨ��";
				this->m_strJYTemp+="|0|0|0|Pass\r\n";
				this->m_strJYTemp+="�����ƻ���ٶȴ�����";
				this->m_strJYTemp+="|0|0|0|Pass\r\n";
				this->m_strJYTemp+="���巢������";
				this->m_strJYTemp+="|0|0|0|Pass\r\n";
				this->m_strJYTemp+="��������";
				this->m_strJYTemp+="|0|0|0|Pass\r\n";
			}

			this->ShowInfo((PWCHAR)strResult.c_str(), FALSE);
			if( nResult!=0)
			{
				break;
			}

			//�������
			if( IDYES!=MessageBoxA(this->m_hWnd, "�����Ƿ�������", "standard", MB_YESNO))
			{
				this->ShowInfo(L"���巢�������쳣.\r\n", FALSE);
				this->m_strJYTemp+="���巢������";
				this->m_strJYTemp+="|0|0|0|Fail\r\n";
				break;
			}

			//����������̼�
			if( this->UpdaterMB(strWorkDir) )
			{
				m_Result=MJ_SUCCESS;
			}
		}
		else
		{
			this->ShowInfo(L"���������쳣.\r\n", FALSE);
			this->m_strJYTemp+="��������";
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
	//��һ��������������̼�
	CF_GetModuleDir(csBuf);
	strFWPath=csBuf;
	strFWPath+="FW\\2s\\WIFI\\";
	strWorkDir=strFWPath;
	strFWPath+="jflash_wifiEx.bat";

	do 
	{
		//����û�����ع̼�����
// 		if( !PathFileExistsA((LPCSTR)strFWPath.c_str()) )
// 		{
// 			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...filePath(%s) not existing.\r\n", strFWPath.c_str());
// 			this->ShowInfo(L"JFlash ��ʱ����������ļ��Ƿ����.\r\n", FALSE);
// 			this->m_strJYTemp+="�����ļ�";
// 			this->m_strJYTemp+="|0|0|0|Fail\r\n";
// 			break;
// 		}
// 
// 		this->ShowInfo(L"waiting for JFlash working...\r\n", FALSE);
// 		CreateProcessA(strFWPath.c_str(), NULL, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, strWorkDir.c_str(), &si, &pi);
// 
// 		if( WAIT_TIMEOUT==WaitForSingleObject(pi.hProcess, MB_JFLASHTIMEOUTV) )
// 		{
// 			this->ShowInfo(L"JFlash ��ʱ�������ξ��Ƿ��ڹ���.\r\n", FALSE);
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
						this->ShowInfo(L"��ѹ�쳣.\r\n", FALSE);
						this->m_strJYTemp+="��ѹ";
						this->m_strJYTemp+="|0|0|0|Fail\r\n";
					}
					break;

				case -2:
					{
						this->ShowInfo(L"wifiģ���쳣����Ӧ��.\r\n", FALSE);
						this->m_strJYTemp+="wifiģ��";
						this->m_strJYTemp+="|0|0|0|Fail\r\n";
					}
					break;

				case 0:
					{
						m_Result=MJ_SUCCESS;
						this->ShowInfo(L"�豸����.\r\n", FALSE);
 
// 						this->m_strJYTemp+="�����ļ�";
// 						this->m_strJYTemp+="|0|0|0|Pass\r\n";
// 						this->m_strJYTemp+="JFlash";
// 						this->m_strJYTemp+="|0|0|0|Pass\r\n";
						this->m_strJYTemp+="��������";
						this->m_strJYTemp+="|0|0|0|Pass\r\n";
						this->m_strJYTemp+="wifiģ��";
						this->m_strJYTemp+="|0|0|0|Pass\r\n";
						this->m_strJYTemp+="��ѹ";
						this->m_strJYTemp+="|0|0|0|Pass\r\n";
					}
					break;
				}
			}
			else
			{
				this->ShowInfo(L"���������쳣.\r\n", FALSE);
				this->m_strJYTemp+="��������";
				this->m_strJYTemp+="|0|0|0|Fail\r\n";
				m_Result=MJ_FAILED;
			}
		}
		else
		{
			this->ShowInfo(L"���������쳣.\r\n", FALSE);
			this->m_strJYTemp+="��������";
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
	// TODO: �ڴ����ר�ô����/����û���
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

	sprintf_s(csBuf, MAX_PATH, "��������:%.4d		�����豸:%.4d	\r\n�����豸:%.4d		ֱͨ��:%d%%", 
				m_nTotalNum, m_nFailNum, m_nPassNum, 
				m_nTotalNum==0?0:(int)((a*100.00)/b) );

	SetDlgItemTextA(this->m_hWnd, IDC_EDIT_TOTAL, csBuf);
}