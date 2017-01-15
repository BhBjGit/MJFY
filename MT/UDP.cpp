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

	//启动SOCKET库，版本为2.0
	WSAStartup(0x0202,&wsdata);

	s=socket(AF_INET,SOCK_DGRAM,0);

	//然后赋值给地址，用来从网络上的广播地址接收消息；
	addrto.sin_family=AF_INET;
	addrto.sin_addr.s_addr=INADDR_BROADCAST;
	addrto.sin_port=htons(uPort);
	//设置该套接字为广播类型，
	setsockopt(s,SOL_SOCKET,SO_BROADCAST,(char FAR *)&optval,sizeof(optval));

	//从广播地址发送消息
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

	//启动SOCKET库，版本为2.0
	WSAStartup(0x0202,&wsdata);

	//然后赋值给两个地址，一个用来绑定套接字，一个用来从网络上的广播地址接收消息；
	addr.sin_family=AF_INET;
	addr.sin_addr.s_addr=htonl(INADDR_ANY);
	addr.sin_port=htons(uPort);

	int fromlength=sizeof(SOCKADDR);

	//用UDP初始化套接字
	s=socket(AF_INET,SOCK_DGRAM,0);

	//设置该套接字为广播类型，
	setsockopt(s,SOL_SOCKET,SO_BROADCAST,(char FAR *)&optval,sizeof(optval));

	DWORD TimeOut=1000*10;   //设置发送超时10秒
	setsockopt(s,SOL_SOCKET,SO_RCVTIMEO,(char FAR *)&TimeOut,sizeof(TimeOut));


	bind(s,(sockaddr *)&addr,sizeof(sockaddr_in));

	//从广播地址接收消息，注意用来绑定的地址和接收消息的地址是不一样的
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