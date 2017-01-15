#include "StdAfx.h"
#include "FileHelper.h"
#include <strsafe.h>

CFileHelper* CFileHelper::m_instance=NULL;

CFileHelper::CFileHelper(void)
{
}


CFileHelper::~CFileHelper(void)
{
}

CFileHelper*	CFileHelper::_()
{
	if( !m_instance)
	{
		m_instance=new CFileHelper();
	}
	return m_instance;
}


BOOLEAN		CFileHelper::SaveLogInfo(TESTERTYPE type, PCHAR	pFileData, INT nInLen, FILEPATHTYPE fType)
{

	BOOLEAN		bRet=FALSE;
	CHAR		csDir[MAX_PATH]={0};
	CHAR		csFile[MAX_PATH]={0};
	SYSTEMTIME	st={0};
	string		strDir;
	string		strDot;

	switch(type)
	{
	case TP_LED:
		{
			if( fType==AIRUS1S)
			{
				strDot="led1s\\";
			}
			else if( fType==AIRUS2S)
			{
				strDot="led2s\\";
			}
		}
		break;

	case TP_WIFI:
		{
			if( fType==AIRUS1S)
			{
				strDot="wifi1s\\";
			}
			else if( fType==AIRUS2S)
			{
				strDot="wifi2s\\";
			}
		}
		break;

	case TP_MB:
		{			
			if( fType==AIRUS1S)
			{
				strDot="mainboard1s\\";
			}
			else if( fType==AIRUS2S)
			{
				strDot="mainboard2s\\";
			}
		}
		break;

	case TP_STAND:
		{			
			if( fType==AIRUS1S)
			{
				strDot="sd1s\\";
			}
			else if( fType==AIRUS2S)
			{
				strDot="sd2s\\";
			}
		}
		break;

	default:
		strDot="none\\";
	}

	CF_GetModuleDir(csDir, MAX_PATH);
	strDir=csDir;

	GetLocalTime(&st);
	StringCchPrintfA(csFile, MAX_PATH, "%.4d-%.2d-%.2d\\", st.wYear, st.wMonth, st.wDay);
	strDir+="LOG\\";
	strDir+=strDot.c_str();
	strDir+=csFile;

	if( !PathFileExistsA(strDir.c_str()))
	{
		strDir=csDir;
		strDir+="LOG\\";
		CreateDirectoryA(strDir.c_str(), NULL);

		strDir+=strDot.c_str();
		CreateDirectoryA(strDir.c_str(), NULL);

		strDir+=csFile;
		CreateDirectoryA(strDir.c_str(), NULL);
	}

	do
	{
		StringCchPrintfA(csFile, MAX_PATH, "%.4d-%.2d-%.2d(%.2d.%.2d.%.2d).log", 
			st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);


		strDir+=csFile;

		OutputDebugStringA(strDir.c_str());

		bRet=CF_SaveFile((PCHAR)strDir.c_str(), (PBYTE)pFileData, nInLen, CREATE_ALWAYS);
	}while(FALSE);

	return bRet;
}



BOOLEAN		CFileHelper::SaveJYLogInfo(TESTERTYPE type, FILEPATHTYPE fType, PCHAR	pFileData, INT nInLen)
{

	BOOLEAN		bRet=FALSE;
	CHAR		csDir[MAX_PATH]={0};
	CHAR		csFile[MAX_PATH]={0};
	SYSTEMTIME	st={0};
	string		strDir;
	string		strDot;

// 	switch(type)
// 	{
// 	case TP_LED:
// 		{
// 			if( fType==AIRUS1S)
// 			{
// 				strDot="led1s\\";
// 			}
// 			else if( fType==AIRUS2S)
// 			{
// 				strDot="led2s\\";
// 			}
// 		}
// 		break;
// 
// 	case TP_WIFI:
// 		{
// 			if( fType==AIRUS1S)
// 			{
// 				strDot="wifi1s\\";
// 			}
// 			else if( fType==AIRUS2S)
// 			{
// 				strDot="wifi2s\\";
// 			}
// 		}
// 		break;
// 
// 	case TP_MB:
// 		{			
// 			if( fType==AIRUS1S)
// 			{
// 				strDot="mainboard1s\\";
// 			}
// 			else if( fType==AIRUS2S)
// 			{
// 				strDot="mainboard2s\\";
// 			}
// 		}
// 		break;
// 
// 	case TP_STAND:
// 		{			
// 			if( fType==AIRUS1S)
// 			{
// 				strDot="sd1s\\";
// 			}
// 			else if( fType==AIRUS2S)
// 			{
// 				strDot="sd2s\\";
// 			}
// 		}
// 		break;
// 
// 	default:
// 		strDot="none\\";
// 	}

	CF_GetModuleDir(csDir, MAX_PATH);
	strDir=csDir;

	GetLocalTime(&st);
	StringCchPrintfA(csFile, MAX_PATH, "%.4d-%.2d-%.2d\\", st.wYear, st.wMonth, st.wDay);
	strDir+="JY\\";
	//strDir+=strDot.c_str();
	strDir+=csFile;

	if( !PathFileExistsA(strDir.c_str()))
	{
		strDir=csDir;
		strDir+="JY\\";
		CreateDirectoryA(strDir.c_str(), NULL);

// 		strDir+=strDot.c_str();
// 		CreateDirectoryA(strDir.c_str(), NULL);

		strDir+=csFile;
		CreateDirectoryA(strDir.c_str(), NULL);
	}

	do
	{
		StringCchPrintfA(csFile, MAX_PATH, "%.2d.%.2d.%.2d.etf", st.wHour, st.wMinute, st.wSecond);

		strDir+=csFile;
		OutputDebugStringA(strDir.c_str());

		bRet=CF_SaveFile((PCHAR)strDir.c_str(), (PBYTE)pFileData, nInLen, CREATE_ALWAYS);
	}while(FALSE);

	return bRet;
}