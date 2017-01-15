#include "StdAfx.h"
#include "ProtocolHelper.h"
#include "SerialPortHelper.h"


BYTE	GetCheckSum(PDEVICE_REPORT_STRUCT packet)
{
	BYTE	checkSum=0;
	PBYTE	p=(PBYTE)packet;

	if( packet)
	{		
		for(int i=0; i<7; i++)
		{
			checkSum+=p[i];
		}

		for(int i=0; i<packet->len; i++)
		{
			checkSum+=packet->data[i];
		}
	}

	checkSum=(~checkSum)+1;
	return checkSum;
}

BOOL	CheckDatas(PDEVICE_REPORT_STRUCT packet)
{
	BYTE	checkSum=packet->checkSum;
	PBYTE	p=(PBYTE)packet;

	for(int i=0; i<6; i++)
	{
		checkSum+=p[i];
	}

	for(int i=0; i<packet->len; i++)
	{
		checkSum+=packet->data[i];
	}

	return !checkSum;
}

VOID	__(PDEVICE_REPORT_STRUCT pPacket)
{
	if( pPacket)
	{
		PBYTE	p=(PBYTE)pPacket;

		for(int i=0; i<PACKETEXTRALEN; i++)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, "0x%.2x	", p[i]);
		}

		p=pPacket->data;
		for(int i=0; i<pPacket->len; i++)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, "0x%.2x	", p[i]);
		}
		CF_OutputDBGInfoEx(GDBGTYPE, NULL, "\r\n");
	}
}

//调用者负责释放包空间
BOOL	GeneratorPacket(BYTE commandID, PBYTE pDatas, SHORT nLen, OUT PDEVICE_REPORT_STRUCT* ppPacket)
{
	BOOL		bRet=FALSE;

	do 
	{
		CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"params:commandID:%x	 datas addr:%x  len:%d\r\n", 
									commandID, pDatas, nLen);

		if( !ppPacket )
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"invalid params\r\n");
			break;
		}

		
		PBYTE					p=new BYTE[nLen+PACKETEXTRALEN];
		PDEVICE_REPORT_STRUCT	pPacket=NULL;
		if( !p)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"allocation memory err.errID(0x%.2x).len(%d)\r\n",
											GetLastError(), nLen+PACKETEXTRALEN);
			break;
		}

		pPacket=(PDEVICE_REPORT_STRUCT)p;
		pPacket->reportID=REPORTID;
		pPacket->commandID=commandID;
		pPacket->flag=0x00;
		pPacket->len=nLen;
		pPacket->checkSum=0;
		if( pDatas && nLen>0)
		{
			memmove(pPacket->data, pDatas, nLen);
		}

		pPacket->checkSum=GetCheckSum(pPacket);

		//__(pPacket);
		*ppPacket=pPacket;
		bRet=TRUE;
	} while (FALSE);

	return bRet;
}

BOOL	ExecuteCommand(HANDLE	h, PDEVICE_REPORT_STRUCT	inPacket, PDEVICE_REPORT_STRUCT&	outPacket)
{
	BOOL		bRet=FALSE;
	PBYTE		pRetDatas=new BYTE[PACKETMAXDATALEN];
	DWORD		dwRetLen=0;

	INT			nRetryRetNum=PACKETRETRYNUMP;
	INT			nRetSleep=SLEEPTIMEVALUE;

	//握手减少等待时间和次数
	if(inPacket->commandID==PACKETCOMMAND_HANDSHAKE)
	{
		nRetryRetNum=0x10;
		nRetSleep=100;
	}

	do 
	{
		if( !pRetDatas)
		{
			CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"allocation memory err.errID(0x%.2x).len(%d)\r\n",
				GetLastError(), PACKETMAXDATALEN);
			break;
		}

		ZeroMemory(pRetDatas, PACKETMAXDATALEN);

		//retry
		for(int i=0; i<PACKETRETRYNUM; i++)
		{
			if( WriteComm(h, (PBYTE)inPacket, inPacket->len+PACKETEXTRALEN, FALSE, NULL, NULL, dwRetLen, NULL) )
			{			
				__(inPacket);///

				for(int j=0; j<nRetryRetNum; j++)
				{
					//sleep
					Sleep(nRetSleep);
					if( ReadCommData(h, pRetDatas, PACKETMAXDATALEN, dwRetLen) )
					{
						outPacket=(PDEVICE_REPORT_STRUCT)pRetDatas;
						__(outPacket);///

						if( ((outPacket->reportID)&0x00FF)==((REPORTID)&0x00FF) &&
							outPacket->commandID==inPacket->commandID &&
							outPacket->flag==0x10	&&
							CheckDatas(outPacket) )
						{
							bRet=TRUE;
							//break;
							goto EXECUTECOMMANDEND;
						}
						else
						{
							CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"check err(%d):reprotID:%.4x  command:%x=%x  flag:%x  checkSum result:%x\r\n",
								j,
								outPacket->reportID ,
								outPacket->commandID,inPacket->commandID ,
								outPacket->flag,
								CheckDatas(outPacket) );
						}
					}
				}
			}
			else
			{
				CF_OutputDBGInfoEx(GDBGTYPE, NULL, __FUNCTION__"write err(0x%.2x)\r\n", GetLastError());
			}
		}
	} while (FALSE);
		

EXECUTECOMMANDEND:
	if( pRetDatas && !bRet )
	{
		delete[] pRetDatas;
		outPacket=NULL;
	}

	return bRet;
}

//0xA1
TESTERTYPE	P_GetType(HANDLE	h)
{
	PDEVICE_REPORT_STRUCT	inPacket=NULL;
	PDEVICE_REPORT_STRUCT	outPacket=NULL;
	PBYTE					p=NULL;
	TESTERTYPE				tp=TP_NONE;

	if( GeneratorPacket(PACKETCOMMAND_HANDSHAKE, NULL, 0, &inPacket) )
	{
		if( ExecuteCommand(h, inPacket, outPacket))
		{
			tp=(TESTERTYPE)outPacket->data[0];

			//release
			p=(PBYTE)outPacket;
			delete[] p;
		}

		//release
		p=(PBYTE)inPacket;
		delete[] p;
	}

	return tp;
}

//0xB1
BOOL	P_GetCheckResult(IN HANDLE h, IN TESTERTYPE tp, OUT PBYTE pDatas, IN INT nInLen)
{
	PDEVICE_REPORT_STRUCT	inPacket=NULL;
	PDEVICE_REPORT_STRUCT	outPacket=NULL;
	BYTE					nType=(BYTE)tp;
	BOOL					bRet=FALSE;

	if( GeneratorPacket(PACKETCOMMAND_AUTOCHECK, &nType, 1, &inPacket) )
	{
		if( ExecuteCommand(h, inPacket, outPacket))
		{
			if( pDatas&&nInLen)
			{
				memmove_s(pDatas, nInLen, outPacket->data, outPacket->len);
				bRet=TRUE;
			}

			//release
			delete[] outPacket;
		}

		//release
		delete[] inPacket;
	}

	return bRet;
}


//D1
BOOL	P_GetSDResult(IN HANDLE h, /*IN TESTERTYPE tp,*/ OUT PBYTE pDatas, IN INT nInLen)
{
	PDEVICE_REPORT_STRUCT	inPacket=NULL;
	PDEVICE_REPORT_STRUCT	outPacket=NULL;
	//BYTE					nType=(BYTE)tp;
	BOOL					bRet=FALSE;

	if( GeneratorPacket(PACKETCOMMAND_STANDARD, NULL, 0, &inPacket) )
	{
		if( ExecuteCommand(h, inPacket, outPacket))
		{
			if( pDatas&&nInLen)
			{
				memmove_s(pDatas, nInLen, outPacket->data, outPacket->len);
				bRet=TRUE;
			}

			//release
			delete[] outPacket;
		}

		//release
		delete[] inPacket;
	}

	return bRet;
}

//E1
BOOL	P_SportMBUpdate(IN HANDLE h, OUT PBYTE pDatas, IN INT nInLen)
{
	PDEVICE_REPORT_STRUCT	inPacket=NULL;
	PDEVICE_REPORT_STRUCT	outPacket=NULL;
	//BYTE					nType=(BYTE)tp;
	BOOL					bRet=FALSE;

	if( GeneratorPacket(PACKETCOMMAND_SPORTMBUPDATE, NULL, 0, &inPacket) )
	{
		if( ExecuteCommand(h, inPacket, outPacket))
		{
			if( pDatas&&nInLen)
			{
				memmove_s(pDatas, nInLen, outPacket->data, outPacket->len);
				bRet=TRUE;
			}

			//release
			delete[] outPacket;
		}

		//release
		delete[] inPacket;
	}

	return bRet;
}

//F1
BOOL	P_SportMBUpdateState(IN HANDLE h,  OUT PBYTE pDatas, IN INT nInLen)
{
	PDEVICE_REPORT_STRUCT	inPacket=NULL;
	PDEVICE_REPORT_STRUCT	outPacket=NULL;
	//BYTE					nType=(BYTE)tp;
	BOOL					bRet=FALSE;

	if( GeneratorPacket(PACKETCOMMAND_SPORTMBUPSTATE, NULL, 0, &inPacket) )
	{
		if( ExecuteCommand(h, inPacket, outPacket))
		{
			if( pDatas&&nInLen)
			{
				memmove_s(pDatas, nInLen, outPacket->data, outPacket->len);
				bRet=TRUE;
			}

			//release
			delete[] outPacket;
		}

		//release
		delete[] inPacket;
	}

	return bRet;
}