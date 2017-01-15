#pragma once


#define			REPORTID			(0xEAAE)
#define			PACKETEXTRALEN		(0x07)

//返回包中所支持最大的数据长度
#define			PACKETMAXDATALEN	(0x400)
//重试次数
#define			PACKETRETRYNUM		(0x03)
#define			PACKETRETRYNUMP		(0x20*5)
//读等待时间
#define			SLEEPTIMEVALUE		(100)

#define			PACKETCOMMAND_HANDSHAKE			(0xA1)
#define			PACKETCOMMAND_AUTOCHECK			(0xB1)
#define			PACKETCOMMAND_STANDARD			(0xD1)
#define			PACKETCOMMAND_SPORTMBUPDATE		(0xE1)
#define			PACKETCOMMAND_SPORTMBUPSTATE	(0xF1)

enum	TESTERTYPE
{
	TP_LED=1,
	TP_WIFI,
	TP_MB,
	TP_STAND,
	TP_SPORT_MB,
	TP_FUN_MB,
	TP_NONE
};

#include <pshpack1.h>

typedef struct	_DEVICE_REPORT_STRUCT
{
	SHORT		reportID;
	BYTE		commandID;
	BYTE		flag;
	SHORT		len;
	BYTE		checkSum;
	BYTE		data[1];
}DEVICE_REPORT_STRUCT,*PDEVICE_REPORT_STRUCT;

#include <poppack.h>


TESTERTYPE	P_GetType(IN HANDLE	h);

BOOL		P_GetCheckResult(IN HANDLE h, IN TESTERTYPE tp, OUT PBYTE pDatas, IN INT nInLen);

BOOL		P_GetSDResult(IN HANDLE h, /*IN TESTERTYPE tp,*/ OUT PBYTE pDatas, IN INT nInLen);


BOOL		P_SportMBUpdate(IN HANDLE h, OUT PBYTE pDatas, IN INT nInLen);
BOOL		P_SportMBUpdateState(IN HANDLE h,  OUT PBYTE pDatas, IN INT nInLen);

