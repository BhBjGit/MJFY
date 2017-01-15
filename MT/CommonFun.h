#pragma once

#include <string>

using std::string;

#define		CF_IRKEYDOWN(vk_code)	((GetAsyncKeyState(vk_code) & 0x8000)? 1 : 0)

#define		CFMAXMEMLEN				(128*1024*1024)

#define		OUTPUT_DBG_INFO_ENABLE	

#define		CF_IRMAX(a,b)	\
(						\
	((a)>(b)?(a):(b))	\
)

#define		CF_IRMIN(a,b)	\
(						\
	((a)<(b)?(a):(b))	\
)

#define		CF_ALIGN(a, b)		(((a)/(b))+((a)%(b)?1:0))	


#define		KEY_DOWN(vk_code)	(((GetAsyncKeyState(vk_code))&0x8000)?1:0)
#define		KEY_UP(vk_code)		(((GetAsyncKeyState(vk_code))&0x8000)?0:1)

typedef enum	_DBGTYPE
{
	DBG_OUT=0,
	DBG_FILE,
	DBG_EVENT
}DBGTYPE;

//event
BOOL	CF_RunOnce(PCHAR	csKeyName);

//file 
bool	CF_GetFileContent(char* pFilePath, BYTE* pData, DWORD dwInLen, DWORD& dwOutLen);
void	CF_GetFielContentEx(char* pFilePath, PBYTE* pData, DWORD& dwOutLen);
bool	CF_SaveFile(char* pFilePath, BYTE* pData, DWORD dwInLen, DWORD dwCreationDisposition=OPEN_ALWAYS);
bool	CF_SelectFileForSave(char* pFilePath, char* pDesDir=NULL, char* pDesType=NULL, char* pDesName=NULL);
//pFilter:后缀名过滤   eg."Bin文件(*.Bin)\0*.Bin;*.CFG;*.txt\0"
void	CF_SelectFile(char* pFilePath, char* pFilter=NULL);


void	CF_GetModuleDir(char* pDir, INT len=MAX_PATH);


void	CF_GetModuleVer(char* csBuf, INT nLen);
void	CF_SetTitleWithVersion(HWND hwnd);


//trans
int		CF_Atoi(char * str);


//string
std::wstring CF_A2W(const std::string &s);
std::wstring CF_A2WEx(const std::string &s);
BOOL CF_StringToWString(const std::string &str,std::wstring &wstr);


std::string CF_W2A(const std::wstring &ws);
std::string CF_W2AEx(const std::wstring &ws);
BOOL CF_WStringToString(const std::wstring &wstr,std::string &str);

void CF_trim(string &s);


//debug
void	CF_SetDebugMode(unsigned char mode);
bool	CF_IsDebugMode();
int		CF_OutputDBGInfo(const char * format, ...);
int		CF_OutputDBGInfoEx(DBGTYPE	type, char* pFileName, const char * format, ...);
int		CF_OutputDBGFile(const char * format, ...);


unsigned long	CF_BCDtoDec(const unsigned char *bcd, int length) ;

//memory
bool			CF_IsValidAddress(LPVOID p);