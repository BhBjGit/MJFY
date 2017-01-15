#pragma once


BOOL	UDP_SendBoardcastUsed(IN PBYTE	pDatas, IN INT nLen, IN USHORT	uPort);
BOOL	UDP_RecvBoardcastUsed(OUT PBYTE	pDatas, IN OUT INT nLen, IN USHORT	uPort);