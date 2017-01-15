#pragma once
#include <string>
using std::string;

class CDataBaseHelper
{
public:
	CDataBaseHelper(void);
	~CDataBaseHelper(void);

public:
	static	CDataBaseHelper*	g_instance;
	static	CDataBaseHelper*	_();

public:
	BOOL	ConnectionDB(string strIP, string strName, string strPSW );
	BOOL	AddDatas();
	BOOL	DelDatas();

};

