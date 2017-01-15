#include "stdafx.h"
#include "CommonFun.h"


#include <strsafe.h>

#pragma comment(lib, "Version.lib")
#pragma comment(lib, "Comdlg32.lib")



/*hkey_current_user\software\irtouch	IsDebugMode	*/
void	CF_SetDebugMode(unsigned char mode)
{

}

bool	CF_IsDebugMode()
{
	bool bRet=false;

	return bRet;
}

int CF_OutputDBGInfo(const char * format, ...)
{
#ifdef OUTPUT_DBG_INFO_ENABLE
	char buffer[1024] = {0};

	va_list arg_ptr;

	va_start(arg_ptr, format);

	int nCnt = vsprintf_s(buffer, _countof(buffer), format, arg_ptr);

	OutputDebugStringA(buffer);

	va_end(arg_ptr);

	return nCnt;
#else
	return 0;
#endif
}

int CF_OutputDBGInfoEx(DBGTYPE	type, char* pFileName, const char * format, ...)
{
	char buffer[1024] = {0};

	va_list arg_ptr;

	va_start(arg_ptr, format);

	int nCnt = vsprintf_s(buffer, _countof(buffer), format, arg_ptr);

	//
	if( DBG_OUT==type )
	{
		OutputDebugStringA(buffer);
	}
	else if( DBG_FILE==type )
	{
		//char buffer[1024] = {0};
		char path[MAX_PATH]={0};
		SYSTEMTIME	stt={0};

		GetLocalTime(&stt);
		StringCchPrintfA(buffer, _countof(buffer), "%.4d/%.2d/%.2d %.2d:%.2d ", 
			stt.wYear, stt.wMonth, stt.wDay,
			stt.wHour, stt.wMinute );
		int nCnt = vsprintf_s(buffer+strlen(buffer), _countof(buffer)-strlen(buffer), format, arg_ptr);

		CF_GetModuleDir(path, MAX_PATH);
		if( pFileName && strlen(pFileName) )
		{
			StringCchCatA(path, MAX_PATH, pFileName);
		}
		else
		{
			StringCchCatA(path, MAX_PATH, "IRDbgFile.ica");
		}
		
		StringCchCatA(buffer, MAX_PATH, "\r\n");
		CF_SaveFile(path, (PBYTE)buffer, strlen(buffer), OPEN_ALWAYS);
	}
	else if( DBG_EVENT==type )
	{
		HANDLE  hEventSource;
		LPSTR  lpszStrings[1];

		lpszStrings[0] = buffer;

		hEventSource = RegisterEventSourceA(NULL, "TPLMathing");
		if (hEventSource != NULL)
		{
			ReportEventA(hEventSource, EVENTLOG_INFORMATION_TYPE, 0, 0, NULL, 1, 0, (LPCSTR*) &lpszStrings[0], NULL);
			DeregisterEventSource(hEventSource);
		}
	}
	else
	{

	}

	va_end(arg_ptr);

	return nCnt;
}



int CF_OutputDBGFile(const char * format, ...)
{
	char buffer[1024] = {0};
	char path[MAX_PATH]={0};
	SYSTEMTIME	stt={0};

	va_list arg_ptr;

	va_start(arg_ptr, format);

	GetLocalTime(&stt);
	StringCchPrintfA(buffer, _countof(buffer), "%.4d/%.2d/%.2d %.2d:%.2d ", 
						stt.wYear, stt.wMonth, stt.wDay,
						stt.wHour, stt.wMinute );
	int nCnt = vsprintf_s(buffer+strlen(buffer), _countof(buffer)-strlen(buffer), format, arg_ptr);

	CF_GetModuleDir(path, MAX_PATH);
	StringCchCatA(path, MAX_PATH, "IRDbgFile.ica");
	StringCchCatA(buffer, MAX_PATH, "\r\n");
	CF_SaveFile(path, (PBYTE)buffer, strlen(buffer), OPEN_ALWAYS);

	va_end(arg_ptr);

	return nCnt;
}


BOOL	CF_RunOnce(PCHAR	csKeyName)
{
	HANDLE	mutex;
	BOOL	bRet=TRUE;

	mutex=CreateMutexA(NULL, TRUE, csKeyName);

	if( ERROR_ALREADY_EXISTS==GetLastError())
	{
		bRet=FALSE;
	}
	else
	{
		bRet=TRUE;
	}

	//CloseHandle(mutex);
	return bRet;
}



void	CF_GetFielContentEx(char* pFilePath, PBYTE* pData, DWORD& dwOutLen)
{
	CF_GetFileContent(pFilePath, NULL, 0, dwOutLen);

	if( dwOutLen>0 )
	{
		if( *pData )
		{
			delete [](*pData);
		}

		(*pData)=new BYTE[dwOutLen];
		//dwOutLen=0;

		if( *pData )
		{
			CF_GetFileContent(pFilePath, *pData, dwOutLen, dwOutLen);
		}
	}
}

bool	CF_GetFileContent(char* pFilePath, BYTE* pData, DWORD dwInLen, DWORD& dwOutLen)
{
	bool	bRet=false;
	HANDLE	hFile=INVALID_HANDLE_VALUE;
	DWORD	dwFileLen=0;
	PBYTE	pFileData=NULL;

	if( pFilePath )
	{
		hFile = CreateFileA(pFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if( INVALID_HANDLE_VALUE!=hFile)
		{
			dwFileLen=GetFileSize(hFile, NULL);

			if( pData )
			{
				pFileData=new BYTE[dwFileLen];

				if( pFileData )
				{
					if( ReadFile(hFile, pFileData, dwFileLen, &dwOutLen, NULL) )
					{
						bRet=true;
						memcpy(pData, pFileData, dwOutLen>dwInLen?dwInLen:dwOutLen);					
					}

					delete []pFileData;
				}
			}
			else
			{
				dwOutLen=dwFileLen;
			}

			CloseHandle(hFile);
		}
	}

	return bRet;
}

bool	CF_SelectFileForSave(char* pFilePath, char* pDesDir, char* pDesType, char* pDesName)
{
	CHAR	szBuffer[MAX_PATH]={0};
	OPENFILENAMEA ofn= {0}; 

	ofn.lStructSize = sizeof(ofn); 
	ofn.hwndOwner = NULL; 
	ofn.lpstrFilter = "所有文件(*.*)\0*.*\0";
	if( pDesDir)
	{
		ofn.lpstrInitialDir=pDesDir;
	}
	else
	{
		ofn.lpstrInitialDir = "C:\\";
	}

	if( pDesName)
	{
		StringCchCopyA(szBuffer, MAX_PATH, pDesName);
	}

	ofn.lpstrFile=szBuffer;
	ofn.nMaxFile = MAX_PATH; 
	//ofn.lpstrFileTitle = szFileTitle; 
	//ofn.nMaxFileTitle = sizeof(szFileTitle); 
	ofn.nFilterIndex = 0; 
	ofn.lpstrTitle="save file";
	ofn.Flags =  OFN_SHOWHELP | OFN_OVERWRITEPROMPT| OFN_EXPLORER ;//标志如果是多选要加上OFN_ALLOWMULTISELECT

	if( pDesType)
	{
		ofn.lpstrDefExt=pDesType;	
	}
	else
	{
		ofn.lpstrDefExt="txt";
	}

	BOOL bSel = GetSaveFileNameA(&ofn); 

	if( pFilePath  && bSel)
	{
		//memcpy(pFilePath, szBuffer, strlen(szBuffer));
		strcpy(pFilePath, szBuffer);
	}

	return bSel;
}


bool	CF_SaveFile(char* pFilePath, byte* pData, DWORD dwInLen, DWORD dwCreationDisposition)
{
	bool	bRet=false;
	HANDLE	hFile=INVALID_HANDLE_VALUE;
	DWORD	dwOutLen=0;

	if( pFilePath && pData )
	{
		hFile = CreateFileA(pFilePath, GENERIC_WRITE, FILE_SHARE_READ, NULL, dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, NULL);
		if( INVALID_HANDLE_VALUE!=hFile)
		{
			//判断大小,超过3M就删除旧内容
			LARGE_INTEGER	fileSize;
			if( GetFileSizeEx(hFile, &fileSize) )
			{
				if( fileSize.LowPart>3*1024*1024 )
				{
					CloseHandle(hFile);
					hFile = CreateFileA(pFilePath, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
				}
			}

			if( INVALID_HANDLE_VALUE!=hFile)
			{
				if( INVALID_SET_FILE_POINTER != SetFilePointer(hFile, 0, 0, FILE_END) )
				{
					if( WriteFile(hFile, pData, dwInLen, &dwOutLen, NULL) )
					{
						bRet=true;			
					}
				}
				else
				{
					WriteFile(hFile, "SetFilePoint error\r\n", strlen("SetFilePoint error\r\n"), &dwOutLen, NULL);
				}

				CloseHandle(hFile);
			}
		}
	}

	return bRet;
}

//pFilter:后缀名过滤   eg."Bin文件(*.Bin)\0*.Bin;*.CFG;*.txt\0"
void	CF_SelectFile(char* pFilePath, char* pFilter)
{
	CHAR szBuffer[MAX_PATH] = {0}; 
	OPENFILENAMEA ofn= {0}; 
	ofn.lStructSize = sizeof(ofn); 
	ofn.hwndOwner = NULL; 
	ofn.lpstrFilter = pFilter?pFilter:"所有文件(*.*)\0*.*\0";
	ofn.lpstrInitialDir = "C:\\";
	ofn.lpstrFile = szBuffer;
	ofn.nMaxFile = sizeof(szBuffer)/sizeof(*szBuffer); 
	ofn.nFilterIndex = 0; 
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER ;//标志如果是多选要加上OFN_ALLOWMULTISELECT
	BOOL bSel = GetOpenFileNameA(&ofn); 

	if( pFilePath )
	{
		memcpy(pFilePath, szBuffer, strlen(szBuffer));
	}
}


void	CF_GetModuleDir(char* pDir, INT len)
{
	CHAR	buf[MAX_PATH]={0};
	PCHAR	p=NULL;
	
	if( pDir )
	{
		GetModuleFileNameA(NULL, buf, MAX_PATH);
		p=strrchr(buf, '\\');
		if( p )
		{
			*(p+1)='\0';
			StringCchCopyA(pDir, len, buf);
		}
	}
}

void	CF_GetModuleVer(char* csBuf, INT nLen)
{
	char csBufPath[MAX_PATH]={0};

	if( GetModuleFileNameA(NULL, csBufPath, MAX_PATH) )
	{
		VS_FIXEDFILEINFO   *VInfo; 
		UINT  nLen = 0; 
		char   *buf; 
		int   ma,   mj,   r,   b; 
		nLen =   GetFileVersionInfoSizeA(csBufPath,   0); 
		buf=   new   char[nLen]; 
		try   
		{ 
			if(GetFileVersionInfoA(csBufPath,   0,   nLen,   buf)   !=   0) 
			{ 
				if(VerQueryValueA(buf,   "\\",   (LPVOID*)(&VInfo),   &nLen))  
				{ 
					ma=   VInfo-> dwFileVersionMS   >>   16; 
					mj=   VInfo-> dwFileVersionMS   &   0xffff; 
					r=   VInfo-> dwFileVersionLS   >>   16; 
					b=   VInfo-> dwFileVersionLS   &   0xffff; 

					sprintf_s(csBuf, MAX_PATH, "   V:%d.%d.%d.%d", ma, mj, r, b);
				} 
			} 
		} 
		catch(...)
		{ 
		} 

		delete   []   buf;
	}
}


void CF_SetTitleWithVersion(HWND hwnd)
{
	char csVersion[MAX_PATH]={0};
	char csBuf[MAX_PATH]={0};

	CF_GetModuleVer(csVersion, MAX_PATH);
	GetWindowTextA(hwnd, csBuf, MAX_PATH);
	strcat_s(csBuf, csVersion);
	SetWindowTextA(hwnd, csBuf);
}


//bcd码转十进制
unsigned long power(int base, int times) 
{ 
	int i; 
	unsigned long rslt = 1; 

	for(i=0; i<times; i++) 
	{
		rslt *= base; 
	}

	return rslt; 
}


unsigned long CF_BCDtoDec(const unsigned char *bcd, int length) 
{ 
	int i, tmp; 
	unsigned long dec = 0; 

	for(i=0; i<length; i++) 
	{ 
		tmp = ((bcd[i]>>4)&0x0F)*10 + (bcd[i]&0x0F);    

		dec += tmp * power(100, length-1-i);
	} 

	return dec; 
} 

//std C++
BOOL CF_StringToWString(const std::string &str,std::wstring &wstr)
{    
	int nLen = (int)str.length();    
	wstr.resize(nLen,L' ');

	int nResult = MultiByteToWideChar(CP_ACP,0,(LPCSTR)str.c_str(),nLen,(LPWSTR)wstr.c_str(),nLen);

	if (nResult == 0)
	{
		return FALSE;
	}

	return TRUE;
}
//wstring高字节不为0，返回FALSE
BOOL CF_WStringToString(const std::wstring &wstr,std::string &str)
{    
	int nLen = (int)wstr.length();    
	str.resize(nLen,' ');

	int nResult = WideCharToMultiByte(CP_ACP,0,(LPCWSTR)wstr.c_str(),nLen,(LPSTR)str.c_str(),nLen,NULL,NULL);

	if (nResult == 0)
	{
		return FALSE;
	}

	return TRUE;
}


std::wstring CF_A2WEx(const std::string &s)
{
	std::wstring	wstr;
	setlocale(LC_ALL, "chs"); 
	const char* _Source = s.c_str();
	size_t _Dsize = s.size() + 1;
	wchar_t *_Dest = new wchar_t[_Dsize];
	wmemset(_Dest, 0, _Dsize);
	mbstowcs(_Dest,_Source,_Dsize);
	std::wstring result = _Dest;
	delete []_Dest;
	setlocale(LC_ALL, "C");
	/*return result;*/
	wstr=result;
	return wstr;
}

//只拷贝低字节至string中
std::string CF_W2AEx(const std::wstring &ws)
{
	std::string		str;
	std::string		curLocale = setlocale(LC_ALL, NULL);        // curLocale = "C";
	setlocale(LC_ALL, "chs");
	const wchar_t* _Source = ws.c_str();
	size_t _Dsize = 2 * ws.size() + 1;
	char *_Dest = new char[_Dsize];
	memset(_Dest,0,_Dsize);
	wcstombs(_Dest,_Source,_Dsize);
	std::string result = _Dest;
	delete []_Dest;
	setlocale(LC_ALL, curLocale.c_str());
	/*return result;*/
	str=result;
	return str;
}

std::wstring CF_A2W(const std::string &str)
{
	std::wstring wstr(str.length(),L' ');
	std::copy(str.begin(), str.end(), wstr.begin());
	return wstr; 
}

//只拷贝低字节至string中
std::string CF_W2A(const std::wstring &wstr)
{
	std::string str(wstr.length(), ' ');
	std::copy(wstr.begin(), wstr.end(), str.begin());
	return str; 
}

/** char to wchar
std::string ws2s(const std::wstring& ws)
{
std::string curLocale = setlocale(LC_ALL, NULL);        // curLocale = "C";
setlocale(LC_ALL, "chs");
const wchar_t* _Source = ws.c_str();
size_t _Dsize = 2 * ws.size() + 1;
char *_Dest = new char[_Dsize];
memset(_Dest,0,_Dsize);
wcstombs(_Dest,_Source,_Dsize);
std::string result = _Dest;
delete []_Dest;
setlocale(LC_ALL, curLocale.c_str());
return result;
}

std::wstring s2ws(const std::string& s)
{
setlocale(LC_ALL, "chs"); 
const char* _Source = s.c_str();
size_t _Dsize = s.size() + 1;
wchar_t *_Dest = new wchar_t[_Dsize];
wmemset(_Dest, 0, _Dsize);
mbstowcs(_Dest,_Source,_Dsize);
std::wstring result = _Dest;
delete []_Dest;
setlocale(LC_ALL, "C");
return result;
}
*/


void CF_trim(string &s) 
{
	if (s.empty()) 
	{
		return ;
	}

	s.erase(0,s.find_first_not_of(" \r\n	"));
	s.erase(s.find_last_not_of(" \r\n	") + 1);
}


int CF_Atoi(char * str)
{
	int		nBase=0;
	PCHAR	pTmp=NULL;
	PCHAR   p=NULL;

	if( pTmp = strstr(str, "{") )
	{
		p=pTmp+1;
	}
	else
	{
		p=str;
	}

	if( strstr(p, "0x") )
	{
		nBase=16;
	}
	else
	{
		nBase=10;
	}

	return strtol(p, NULL, nBase);
}


bool	CF_IsValidAddress(LPVOID p)
{
	bool	bRet=false;

	MEMORY_BASIC_INFORMATION mbi;
	VirtualQuery(p, &mbi, sizeof(MEMORY_BASIC_INFORMATION));

	if( mbi.Protect==PAGE_EXECUTE_READWRITE ||
		mbi.Protect==PAGE_EXECUTE_WRITECOPY ||
		mbi.Protect==PAGE_READWRITE			||
		mbi.Protect==PAGE_WRITECOPY)
	{
		bRet=true;
	}

	return bRet;
}
