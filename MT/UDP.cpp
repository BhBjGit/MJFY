#include "StdAfx.h"
#include "UDP.h"
#include <winsock2.h>

#pragma comment(lib, "Ws2_32.lib")


BOOL	UDP_SendBoardcastUsed(IN PBYTE	pDatas, IN INT nLen, IN USHORT	uPort)
{
	BOOL			bRet=FALSE;
	SOCKET			s;
	sockaddr_in		addrto;
	WSADATA			wsdata;
	BOOL			optval=TRUE;
	char *smsg="this is a test";

	//����SOCKET�⣬�汾Ϊ2.0
	WSAStartup(0x0202,&wsdata);

	s=socket(AF_INET,SOCK_DGRAM,0);

	//Ȼ��ֵ����ַ�������������ϵĹ㲥��ַ������Ϣ��
	addrto.sin_family=AF_INET;
	addrto.sin_addr.s_addr=INADDR_BROADCAST;
	addrto.sin_port=htons(uPort);
	//���ø��׽���Ϊ�㲥���ͣ�
	setsockopt(s,SOL_SOCKET,SO_BROADCAST,(char FAR *)&optval,sizeof(optval));

	//�ӹ㲥��ַ������Ϣ
	int ret=sendto(s,(char*)pDatas, nLen ,0,(sockaddr*)&addrto,sizeof(addrto));

	if(ret==SOCKET_ERROR)
	{
		CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"...ret err(0x%x)\r\n", GetLastError());
	}
	else
	{		
		bRet=TRUE;
	}

	closesocket(s);
	WSACleanup();
	
	return	bRet;
}

BOOL	UDP_RecvBoardcastUsed(OUT PBYTE	pDatas, IN OUT INT nLen, IN USHORT	uPort)
{
	BOOL		bRet=FALSE;
	SOCKET		s;
	sockaddr_in from;
	sockaddr_in	addr;
	WSADATA		wsdata;
	BOOL		optval=TRUE;

	//����SOCKET�⣬�汾Ϊ2.0
	WSAStartup(0x0202,&wsdata);

	//Ȼ��ֵ��������ַ��һ���������׽��֣�һ�������������ϵĹ㲥��ַ������Ϣ��
	addr.sin_family=AF_INET;
	addr.sin_addr.s_addr=htonl(INADDR_ANY);
	addr.sin_port=htons(uPort);

	int fromlength=sizeof(SOCKADDR);

	//��UDP��ʼ���׽���
	s=socket(AF_INET,SOCK_DGRAM,0);

	//���ø��׽���Ϊ�㲥���ͣ�
	setsockopt(s,SOL_SOCKET,SO_BROADCAST,(char FAR *)&optval,sizeof(optval));

	DWORD TimeOut=1000*10;   //���÷��ͳ�ʱ10��
	setsockopt(s,SOL_SOCKET,SO_RCVTIMEO,(char FAR *)&TimeOut,sizeof(TimeOut));


	bind(s,(sockaddr *)&addr,sizeof(sockaddr_in));

	//�ӹ㲥��ַ������Ϣ��ע�������󶨵ĵ�ַ�ͽ�����Ϣ�ĵ�ַ�ǲ�һ����
	if( SOCKET_ERROR != (nLen=recvfrom(s, (char*)pDatas, nLen,0,(struct sockaddr FAR *)&from,(int FAR *)&fromlength)))
	{ 
		pDatas[nLen]='\0';
		bRet=TRUE;
	}
	_ASSERTE( _CrtCheckMemory( ) );
	closesocket(s);
	WSACleanup();
	return	bRet;
}