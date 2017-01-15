#pragma  once

#include	<vector>
using		std::vector;
using		std::wstring;
typedef vector<wstring>	VECSTR;

//����ϵͳ����COM�˿�
VOID	EnumCommPort(VECSTR&	vecComStr);

//����Ӧ��COM��
BOOL	OpenSerialPort(wstring strCom, HANDLE& hCom);

//�ر���ӦCOM��
VOID	CloseSerialPort(HANDLE hCom);

/**
��COM��ͨ��
	hCom:���ھ��
	pInData:д�뻺����
	dwInDataLenд�뻺��������
	bRead:�Ƿ���Ҫ��ȡ����
	POutData:�������ݻ�����
	dwOutDataLen:���ջ���������
	dwOutRetLen:���ջ�����ʵ�ʶ�ȡ����
	dwSleep:�ȴ���ʱ
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
����ҵ���߼�������ȡ���ݣ���ʱ����Ϊ3min
	hCom:���ھ��
	POutData:�������ݻ�����
	dwOutDataLen:���ջ���������
	dwOutRetLen:���ջ�����ʵ�ʶ�ȡ����
	pExpectationData:����Э�������ķ���ֵ
	dwExpectationLen:����ֵ����
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
