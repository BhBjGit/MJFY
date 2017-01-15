#include	"stdafx.h"
#include	"SerialPortHelper.h"
#include	<process.h>
#include	<vector>

#define			COMMCATCH					(5*1024)
#define			REGPATHFORSERIALPORT		(L"SOFTWARE\\DCITS\\")
#define			REGPATHFORSERIALPORTVALUE	(L"serialPortNum")



BOOL			g_bStop;

VOID EnumCommPort(VECSTR&	vecComStr)    
{   
	HKEY hKey;    
	if(ERROR_SUCCESS ==::RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"Hardware\\DeviceMap\\SerialComm",NULL, KEY_READ, &hKey ))
	{    
		int   i=0;    
		WCHAR  RegKeyName[128],SerialPortName[128]; 
		DWORD  dwLong,dwSize;
		while(TRUE)   
		{   
			dwLong = dwSize=sizeof(RegKeyName);   
			if(ERROR_NO_MORE_ITEMS==::RegEnumValueW(hKey,i,RegKeyName,&dwLong,NULL,NULL,(LPBYTE)SerialPortName, &dwSize))
			{
				break;   
			}
  
			if(!memcmp(RegKeyName,L"\\Device\\",wcslen(L"\\Device\\")*sizeof(WCHAR)) )
			{
				CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...find->%ws\r\n", SerialPortName);///
				vecComStr.push_back(SerialPortName);
			}
			i++;   
		}

		RegCloseKey(hKey);  
	}
}

BOOL	OpenSerialPort(wstring strCom, HANDLE& hCom)
{
	BOOL	bRet=FALSE;
	wstring	wstrPath=L"\\\\.\\";
	wstrPath+=strCom;
	PWCHAR  pSerialName = (PWCHAR)wstrPath.c_str();
	pSerialName=wcslwr(pSerialName);

	while( wcsstr(pSerialName,L"com") )
	{
		COMMTIMEOUTS	TimeOuts={0};
		DCB				serialDCB={0};
		serialDCB.DCBlength=sizeof(DCB);

		CF_OutputDBGInfoEx(GDBGTYPE, NULL, "open serial:%ws\r\n", pSerialName);
/*		memset(pSendBuf, 'r', dwLamNum);*/
		hCom=CreateFileW(pSerialName,//..........................
			GENERIC_READ|GENERIC_WRITE, 
			0, //独占方式
			NULL,
			OPEN_EXISTING, 
			0, //同步方式
			NULL);
		if(hCom==INVALID_HANDLE_VALUE)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...open serial fail.ErrID:0x%x", GetLastError());
			break;
		}

		//config
		if( !GetCommState(hCom, &serialDCB) )
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...GetCommState fail.ErrID:0x%x", GetLastError());
			break;
		}
		serialDCB.BaudRate=CBR_115200;
		serialDCB.fParity=0;
		serialDCB.Parity=NOPARITY;
		serialDCB.ByteSize=8;
		serialDCB.StopBits=ONESTOPBIT;
		if( !SetCommState(hCom, &serialDCB) )
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...SetCommState fail.ErrID:0x%x", GetLastError());
			break;
		}

		if( !SetupComm(hCom, COMMCATCH, COMMCATCH) )
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...SetupComm fail.ErrID:0x%x", GetLastError());
			break;
		}

		TimeOuts.ReadIntervalTimeout=MAXDWORD;
		TimeOuts.ReadTotalTimeoutMultiplier=0;
		TimeOuts.ReadTotalTimeoutConstant=0;

		TimeOuts.WriteTotalTimeoutMultiplier=100;
		TimeOuts.WriteTotalTimeoutConstant=500;
		SetCommTimeouts(hCom,&TimeOuts);

		bRet=TRUE;
		break;
	}

	return bRet;
}

VOID	CloseSerialPort(HANDLE hCom)
{
	CloseHandle(hCom);
}

BOOL	WriteComm(
				  HANDLE hCom,  
				  PBYTE pInData, 
				  DWORD	dwInDataLen, 
				  BOOL bRead, 
				  PBYTE POutData, 
				  DWORD dwOutDataLen, 
				  DWORD&	dwOutRetLen, 
				  DWORD dwSleep)
{
	DWORD	dwReadLen=0;
	DWORD	dwWriteLen=0;
	COMSTAT ComStat;
	DWORD	dwErrorFlags;
	BOOL	bRet=FALSE;

	while(pInData && dwInDataLen)
	{
		ClearCommError(hCom,&dwErrorFlags,&ComStat);
		PurgeComm(hCom, PURGE_TXABORT|
					PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);

		if( !WriteFile(hCom,pInData,dwInDataLen,&dwWriteLen,NULL) )
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...write serial port failed.ErrID:0x%x", GetLastError());
			break;
		}

		if( bRead && POutData && dwOutDataLen )
		{
			Sleep(dwSleep);

			if(!ReadFile(hCom,POutData,dwOutDataLen,&dwReadLen,NULL))
			{
				CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...read serial port failed.ErrID:0x%x", GetLastError());
				break;
			}
			dwOutRetLen=dwReadLen;
		}

		bRet=TRUE;
		break;
	}

	return bRet;
}

/**
交易类型超时为3min，这里按照这个值进行循环读取 POSDEALTIMEOUT
*/
BOOL	ReadCommTrans(
					  HANDLE hCom, 
					  PBYTE POutData, 
					  DWORD dwOutDataLen, 
					  DWORD&	dwOutRetLen,
					  PBYTE	pExpectationData,
					  DWORD	dwExpectationLen)
{
	DWORD				dwReadLen=0;
	BOOL				bRet=FALSE;
	//500ms重试一次
	const	INT			nSleep=500;
	INT					nRetryCount=3;

	while( POutData && dwOutDataLen	&& pExpectationData && dwExpectationLen )
	{
		for(int i=0; i<nRetryCount; i++)
		{
			Sleep(nSleep);
			ReadFile(hCom,POutData,dwOutDataLen,&dwReadLen,NULL);


			//判断返回包是否正确
			if( memcmp(POutData, pExpectationData, dwExpectationLen) )
			{
				CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...ret value not equsl %s", pExpectationData);///
			}
			else
			{
				CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...ret value equsl %s", pExpectationData);///
				dwOutRetLen=dwReadLen;
				bRet=TRUE;
				break;
			}
		}

		break;
	}

	return bRet;
}


BOOL	ReadCommData(
					HANDLE hCom, 
					PBYTE POutData, 
					DWORD dwOutDataLen, 
					DWORD&	dwOutRetLen)
{
	return ReadFile(hCom,POutData,dwOutDataLen,&dwOutRetLen,NULL);
}