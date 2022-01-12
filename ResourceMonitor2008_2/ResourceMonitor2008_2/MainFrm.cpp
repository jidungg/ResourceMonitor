
// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "ResourceMonitor2008_2.h"
#include "./View/ResourceMonitorView.h"
#include "./View/CPUMonitorView.h"
#include "./View/MemMonitorView.h"
#include "./View/DiskMonitorView.h"
#include "./View/NetMonitorView.h"
#include "MainFrm.h"
#include "ResourceMonitorDoc.h"
#include "./PerfData/PerfDataManager.h"
#include "./Logger/DlgSetLogInterval.h"
#include "./Logger/DlgSetLogThreshold.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SYSCOMMAND()

	ON_COMMAND(ID_LOG_SETINTERVAL, &CMainFrame::OnLogSetinterval)
	ON_COMMAND(ID_LOG_SETTHRESHOLD, &CMainFrame::OnLogSetthreshold)
	ON_WM_TIMER()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 상태 줄 표시기
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 생성/소멸

CMainFrame::CMainFrame()
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.

}

CMainFrame::~CMainFrame()
{

}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/,
	CCreateContext* pContext)
{
	CRect rect;
	GetClientRect(&rect);

	m_wndSplitter.CreateStatic(this, 2, 2);
	m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CCPUMonitorView), CSize(rect.Width()/2, rect.Height()/2), pContext);
	m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CMemMonitorView), CSize(rect.Width()/2, rect.Height()/2), pContext);
	m_wndSplitter.CreateView(1, 0, RUNTIME_CLASS(CDiskMonitorView), CSize(rect.Width()/2, rect.Height()/2), pContext);
	m_wndSplitter.CreateView(1, 1, RUNTIME_CLASS(CNetMonitorView), CSize(rect.Width()/2, rect.Height()/2), pContext);

	return TRUE;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;



	return TRUE;
}

// CMainFrame 진단

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
		
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 메시지 처리기



void CMainFrame::OnSysCommand(UINT nID, LPARAM lParam)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (nID == SC_CLOSE)
	{
		if (MessageBox(L"Program Exit?", L"Resource Monitor EXIT", MB_YESNO) == IDYES)
		{
			// exit evnet
			CResourceMonitorDoc* d = (CResourceMonitorDoc*)GetActiveDocument();
			// thread loop 탈출 조건 설정
			d->m_isExit = TRUE; 
			CPerfDataManager* dm = d->m_perfDataManager;

			// m_table claer 후 erase
			dm->m_win32PerfFormatProc->CleanUpOnce();
			dm->m_win32PerfFormatProc->Cleanup();
			dm->m_win32PerfFormatProc->m_table->clear();
			dm->m_win32PerfFormatProc->m_table->erase(dm->m_win32PerfFormatProc->m_table->begin(), dm->m_win32PerfFormatProc->m_table->end());
			delete dm->m_win32PerfFormatProc->m_table;
			delete dm->m_win32PerfFormatProc->dataObj;

			dm->m_win32OperatingSystem->Cleanup();
			dm->m_win32OperatingSystem->m_table->clear();
			dm->m_win32OperatingSystem->m_table->erase(dm->m_win32OperatingSystem->m_table->begin(), dm->m_win32OperatingSystem->m_table->end());
			delete dm->m_win32OperatingSystem->m_table;
			delete dm->m_win32OperatingSystem->dataObj;
	
			dm->m_etw->CleanUp();

			// thread 종료
			d->ExitThread();


			// PerfDataManager 소멸자 호출
			d->m_perfDataManager->~CPerfDataManager();

			// CResourceMonitorDoc 소멸자 호출
			d->~CResourceMonitorDoc();
			CFrameWnd::OnSysCommand(nID, lParam);
		}
	}
	else {
		CFrameWnd::OnSysCommand(nID, lParam);
	}


}


void CMainFrame::OnLogSetinterval()
{
	CResourceMonitorDoc* doc = (CResourceMonitorDoc*)GetActiveDocument();
	CDlgSetLogInterval dlg(&(doc->m_logInterval));
	dlg.DoModal();
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}

void CMainFrame::OnLogSetthreshold()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CResourceMonitorDoc* doc = (CResourceMonitorDoc*)GetActiveDocument();
	CDlgSetLogThreshold dlg(&(doc->m_cpuThreshold), &(doc->m_memThreshold), &(doc->m_networkThreshold), &(doc->m_diskThreshold));
	dlg.DoModal();
}

void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	switch(nIDEvent)
	{

	}
	CFrameWnd::OnTimer(nIDEvent);
}


