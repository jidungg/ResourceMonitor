
// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "ResourceMonitor.h"
#include "ResourceMonitorView.h"
#include "CPUMonitorView.h"
#include "MemMonitorView.h"
#include "DiskMonitorView.h"
#include "NetMonitorView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SYSCOMMAND()
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
	m_wndSplitter.CreateStatic(this, 2, 2);
	m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CCPUMonitorView), CSize(500, 500), pContext);
	m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CMemMonitorView), CSize(300, 300), pContext);
	m_wndSplitter.CreateView(1, 0, RUNTIME_CLASS(CDiskMonitorView), CSize(500, 500), pContext);
	m_wndSplitter.CreateView(1, 1, RUNTIME_CLASS(CNetMonitorView), CSize(300, 300), pContext);

	//SetActiveView((CView *)m_wndSplitter.GetPane(0, 0));

	return TRUE;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.
	if (cs.hMenu != NULL)
	{
		::DestroyMenu(cs.hMenu);
		cs.hMenu = NULL;
	}


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
		CDocument* doc =  GetActiveDocument();

	}
	CFrameWnd::OnSysCommand(nID, lParam);

}
