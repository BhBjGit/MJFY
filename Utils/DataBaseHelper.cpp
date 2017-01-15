#include "StdAfx.h"
#include "DataBaseHelper.h"

CDataBaseHelper*	CDataBaseHelper::g_instance=NULL;

CDataBaseHelper::CDataBaseHelper(void)
{
}


CDataBaseHelper::~CDataBaseHelper(void)
{
}



CDataBaseHelper*	CDataBaseHelper::_()
{
	if( !g_instance)
	{
		g_instance=new CDataBaseHelper();
	}

	return	g_instance;
}


BOOL	ConnectionDB(string strIP, string strName, string strPSW )
{
	BOOL	bRet=FALSE;

	return	bRet;
}

BOOL	AddDatas()
{
	BOOL	bRet=FALSE;

	return	bRet;
}

BOOL	DelDatas()
{
	BOOL	bRet=FALSE;

	return	bRet;
}