#pragma once

#include "ProtocolHelper.h"


enum	FILEPATHTYPE
{
	AIRUS1S=1,
	AIRUS2S,
};

class CFileHelper
{
public:
	CFileHelper(void);
	~CFileHelper(void);


	static		CFileHelper*	_();
	static		CFileHelper* m_instance;
	//存放log日志
	BOOLEAN		CFileHelper::SaveLogInfo(TESTERTYPE type, PCHAR	pFileData, INT nInLen, FILEPATHTYPE fType=AIRUS1S);

	//按照津亚格式存放结果
	BOOLEAN		CFileHelper::SaveJYLogInfo(TESTERTYPE type, FILEPATHTYPE fType, PCHAR	pFileData, INT nInLen);
};

