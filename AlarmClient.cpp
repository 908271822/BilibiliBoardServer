
// AlarmClient.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "AlarmClient.h"
#include "AlarmClientDlg.h"
#include "xLogManager.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "server.hpp"
// CAlarmClientApp

BEGIN_MESSAGE_MAP(CAlarmClientApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CAlarmClientApp ����

CAlarmClientApp::CAlarmClientApp()
{
	// ֧����������������
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CAlarmClientApp ����

CAlarmClientApp theApp;


// CAlarmClientApp ��ʼ��

BOOL CAlarmClientApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()��  ���򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// ���� shell ���������Է��Ի������
	// �κ� shell ����ͼ�ؼ��� shell �б���ͼ�ؼ���
	CShellManager *pShellManager = new CShellManager;

	// ���Windows Native���Ӿ����������Ա��� MFC �ؼ�����������
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));
	HANDLE hMutex = NULL;
	hMutex = CreateMutex(NULL, FALSE, "CAlarmClientApp");
	if (hMutex != NULL)
	{
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			CloseHandle(hMutex);
			return FALSE;
		}
	}
	boost::thread *pThread = new boost::thread(&CAlarmClientApp::initHttpServer,this);
	Helper::setCurrentWorkPath();
	new xLogMessager("AlarmClient","log",true);
	CAlarmClientDlg dlg;
	m_pMainWnd = &dlg;
	dlg.Create(IDD_ALARMCLIENT_DIALOG);
 	RECT rcWorkArea;
 	//��ÿͻ����ù�����
 	if (SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0))
 	{
 		int nScrWidth = GetSystemMetrics(SM_CXSCREEN);
 		int nScrHeight = GetSystemMetrics(SM_CYSCREEN);
 		CRect rcWindow;
 		GetWindowRect(dlg.GetSafeHwnd(), rcWindow);
 		//����ƶ������½ǵ�����
 		CRect rcMoveRect;
 		rcMoveRect.left = rcWorkArea.right - rcWindow.Width();
 		rcMoveRect.right = rcWorkArea.right;
 		rcMoveRect.top = rcWorkArea.bottom - rcWindow.Height();
 		rcMoveRect.bottom = rcWorkArea.bottom;
 		MoveWindow(dlg.GetSafeHwnd(), rcMoveRect.TopLeft().x, rcMoveRect.TopLeft().y, rcMoveRect.Width(), rcMoveRect.Height(), FALSE);
 	}
	dlg.RunModalLoop();
	// ɾ�����洴���� shell ��������
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}
	delete xLogMessager::getSingletonPtr();
	pThread->interrupt();
	SafeDelete(pThread);
#ifndef _AFXDLL
	ControlBarCleanUp();
#endif

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}

void CAlarmClientApp::initHttpServer()
{
	http::server::server*  httpServer = new http::server::server("0.0.0.0", "23333", "\root");
	httpServer->run();
}

