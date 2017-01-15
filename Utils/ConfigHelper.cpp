#include "stdafx.h"
#include "ConfigHelper.h"

CConfigHelper*	CConfigHelper::g_instance=NULL;

CConfigHelper::CConfigHelper(void)
{
}


CConfigHelper::~CConfigHelper(void)
{
}


CConfigHelper*	CConfigHelper::_()
{
	if( !g_instance)
	{
		g_instance=new CConfigHelper();
	}

	return g_instance;
}



