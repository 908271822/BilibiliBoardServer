
// AlarmClient.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CAlarmClientApp: 
// �йش����ʵ�֣������ AlarmClient.cpp
//

class CAlarmClientApp : public CWinApp
{
public:
	CAlarmClientApp();

// ��д
public:
	virtual BOOL InitInstance();
	void initHttpServer();
// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CAlarmClientApp theApp;