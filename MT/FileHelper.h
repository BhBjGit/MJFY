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
	//���log��־
	BOOLEAN		CFileHelper::SaveLogInfo(TESTERTYPE type, PCHAR	pFileData, INT nInLen, FILEPATHTYPE fType=AIRUS1S);

	//���ս��Ǹ�ʽ��Ž��
	BOOLEAN		CFileHelper::SaveJYLogInfo(TESTERTYPE type, FILEPATHTYPE fType, PCHAR	pFileData, INT nInLen);
};

