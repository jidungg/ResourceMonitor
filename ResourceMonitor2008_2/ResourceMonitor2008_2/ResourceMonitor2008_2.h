
// ResourceMonitor2008_2.h : ResourceMonitor2008_2 ���� ���α׷��� ���� �� ��� ����
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"       // �� ��ȣ�Դϴ�.


// CResourceMonitorApp:
// �� Ŭ������ ������ ���ؼ��� ResourceMonitor.cpp�� �����Ͻʽÿ�.
//

class CResourceMonitorApp : public CWinApp
{
public:
	CResourceMonitorApp();

	CSingleDocTemplate* pDocTemplate;

	// �������Դϴ�.
	virtual BOOL InitInstance();

	// �����Դϴ�.
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CResourceMonitorApp theApp;