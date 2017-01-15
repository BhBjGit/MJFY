#pragma  once

#include	<vector>
using		std::vector;
using		std::wstring;
typedef vector<wstring>	VECSTR;

//遍历系统所有COM端口
VOID	EnumCommPort(VECSTR&	vecComStr);

//打开相应的COM口
BOOL	OpenSerialPort(wstring strCom, HANDLE& hCom);

//关闭相应COM口
VOID	CloseSerialPort(HANDLE hCom);

/**
与COM口通信
	hCom:串口句柄
	pInData:写入缓冲区
	dwInDataLen写入缓冲区长度
	bRead:是否需要读取数据
	POutData:接收数据缓冲区
	dwOutDataLen:接收缓冲区长度
	dwOutRetLen:接收缓冲区实际读取长度
	dwSleep:等待延时
*/
BOOL	WriteComm(
				  HANDLE hCom,  
				  PBYTE pInData, 
				  DWORD	dwInDataLen, 
				  BOOL bRead, 
				  PBYTE POutData, 
				  DWORD dwOutDataLen, 
				  DWORD&	dwOutRetLen, 
				  DWORD dwSleep);

/**
根据业务逻辑继续读取数据，超时设置为3min
	hCom:串口句柄
	POutData:接收数据缓冲区
	dwOutDataLen:接收缓冲区长度
	dwOutRetLen:接收缓冲区实际读取长度
	pExpectationData:根据协议期望的返回值
	dwExpectationLen:期望值长度
*/
BOOL	ReadCommTrans(
					  HANDLE hCom, 
					  PBYTE POutData, 
					  DWORD dwOutDataLen, 
					  DWORD&	dwOutRetLen,
					  PBYTE	pExpectationData,
					  DWORD	dwExpectationLen);


BOOL	ReadCommData(
						HANDLE hCom, 
						PBYTE POutData, 
						DWORD dwOutDataLen, 
						DWORD&	dwOutRetLen);
