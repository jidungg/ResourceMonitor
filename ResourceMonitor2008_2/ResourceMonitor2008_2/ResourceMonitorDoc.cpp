
// ResourceMonitorDoc.cpp : CResourceMonitorDoc 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "ResourceMonitor2008_2.h"
#endif

#include "./View/ResourceMonitorView.h"
#include "ResourceMonitorDoc.h"
#include "./PerfData/PerfDataManager.h"
#include "./Logger/Logger.h"
#include <propkey.h>
#include <vector>
#include "./PerfData/Etw/Etw.h"
#include <time.h>
#include <Lm.h>
#pragma comment(lib,"netapi32.lib")
#pragma warning(disable:4996)

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;
// CResourceMonitorDoc

IMPLEMENT_DYNCREATE(CResourceMonitorDoc, CDocument)

BEGIN_MESSAGE_MAP(CResourceMonitorDoc, CDocument)

END_MESSAGE_MAP()

DWORD CResourceMonitorDoc::WinMajorVersion;
DWORD CResourceMonitorDoc::WinMinorVersion;
// CResourceMonitorDoc 생성/소멸


CResourceMonitorDoc::CResourceMonitorDoc() 
{
	// TODO: 여기에 일회성 생성 코드를 추가합니다.
	m_perfDataManager = new CPerfDataManager(this);

	m_isExit= FALSE;
	m_logInterval= LOG_INTERVAL;
	m_cpuThreshold = LOG_CPU_THRESHOLD;
	m_memThreshold = LOG_MEM_THRESHOLD;
	m_networkThreshold = LOG_NET_THRESHOLD;
	m_diskThreshold = LOG_DISK_THRESHOLD;
	
}

CResourceMonitorDoc::~CResourceMonitorDoc()
{
	delete m_perfDataManager;
	m_perfDataManager = NULL;
}


UINT CResourceMonitorDoc::UpdateTimer(LPVOID doc)
{
	CResourceMonitorDoc* pDoc = (CResourceMonitorDoc*)doc;
	clock_t start, end;
	int time;
	while (pDoc->m_isExit == FALSE)
	{

		start = clock();
		pDoc->m_perfDataManager->RefreshData();
		pDoc->m_pView1->UpdateView(pDoc->m_perfDataManager);
		pDoc->m_pView2->UpdateView(pDoc->m_perfDataManager);
		pDoc->m_pView3->UpdateView(pDoc->m_perfDataManager);
		pDoc->m_pView4->UpdateView(pDoc->m_perfDataManager);
		end = clock();
		time = end-start;
		time = UPDATE_INTERVAL-time > 0 ? UPDATE_INTERVAL-time : 0;
		Sleep(time);
	}
	return EXIT_CODE;
}

UINT CResourceMonitorDoc::LogTimer(LPVOID doc)
{
	CResourceMonitorDoc* pDoc = (CResourceMonitorDoc*)doc;
	clock_t start, end;
	int time;
	while (pDoc->m_isExit == FALSE)
	{
		start = clock();
		pDoc->m_pView1->AddPeriodicLog();
		pDoc->m_pView2->AddPeriodicLog();
		pDoc->m_pView3->AddPeriodicLog();
		pDoc->m_pView4->AddPeriodicLog();
		end = clock();
		time = end-start;
		time = pDoc->m_logInterval-time > 0 ? pDoc->m_logInterval-time : 0;
		Sleep(time);
	}
	return EXIT_CODE;
}
void CResourceMonitorDoc::AtExitProcess(vector<ULONG>* exitedProcIDs)
{
	m_pView1->RemoveProcessFromList(exitedProcIDs);
	m_pView2->RemoveProcessFromList(exitedProcIDs);
	m_pView3->RemoveProcessFromList(exitedProcIDs);
	m_pView4->RemoveProcessFromList(exitedProcIDs);
}
void CResourceMonitorDoc::AtNetworkOut(vector<ULONG>* exitedProcIDs)
{
	m_pView4->RemoveProcessFromList(exitedProcIDs);
}
void CResourceMonitorDoc::AtDiskOut(vector<ULONG>* exitedProcIDs)
{
	m_pView3->RemoveProcessFromList(exitedProcIDs);
}

void CResourceMonitorDoc::ExitThread()
{
	DWORD dwRetCode = WaitForSingleObject(m_updateTimerThread->m_hThread, INFINITE);
	if (dwRetCode == WAIT_OBJECT_0)
	{
		TRACE("m_updaterThread is terminated normally!\n");
	}
	else if (dwRetCode == WAIT_TIMEOUT)
	{
		::TerminateThread(m_updateTimerThread->m_hThread, 0);
		TRACE("m_updaterThread is terminated FORCELY!");
	}

	delete m_updateTimerThread;
	m_updateTimerThread = NULL;

	dwRetCode = WaitForSingleObject(m_logTimerThread->m_hThread, LOG_INTERVAL);
	if (dwRetCode == WAIT_OBJECT_0)
	{
		TRACE("m_loggerThread is terminated normally!\n");
	}
	else if (dwRetCode == WAIT_TIMEOUT)
	{
		::TerminateThread(m_logTimerThread->m_hThread, 0);
		TRACE("m_loggerThread is terminated FORCELY!");
	}

	delete m_logTimerThread;
	m_logTimerThread = NULL;
}
BOOL CResourceMonitorDoc::GetOSVersion(DWORD &major, DWORD &minor)
{
	LPWKSTA_INFO_100 pBuf =NULL;
	if(NERR_Success != NetWkstaGetInfo(NULL,100,(LPBYTE*)&pBuf))
	{
		return FALSE;
	}
	WinMajorVersion=pBuf->wki100_ver_major;
	WinMinorVersion=pBuf->wki100_ver_minor;
	NetApiBufferFree(pBuf);
}
BOOL CResourceMonitorDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	if (this == NULL)return FALSE;

	GetOSVersion(CResourceMonitorDoc::WinMajorVersion, CResourceMonitorDoc::WinMinorVersion);

	POSITION pos = this->GetFirstViewPosition();
	m_pView1 = (CResourceMonitorView*)this->GetNextView(pos);
	m_pView2 = (CResourceMonitorView*)this->GetNextView(pos);
	m_pView3 = (CResourceMonitorView*)this->GetNextView(pos);
	m_pView4 = (CResourceMonitorView*)this->GetNextView(pos);

	m_perfDataManager->m_pDoc = this;
	m_perfDataManager->m_etw->SetUp();

	 //thread 생성

	m_updateTimerThread = AfxBeginThread(UpdateTimer, this, 0, 0, CREATE_SUSPENDED, 0);
	m_updateTimerThread->m_bAutoDelete = FALSE;
	m_updateTimerThread->ResumeThread();



	m_logTimerThread = AfxBeginThread(LogTimer, this, 0, 0, CREATE_SUSPENDED, 0);
	m_logTimerThread->m_bAutoDelete = FALSE;
	m_logTimerThread->ResumeThread();

	// TODO: 여기에 재초기화 코드를 추가합니다.
	// SDI 문서는 이 문서를 다시 사용합니다.

	return TRUE;
}




// CResourceMonitorDoc serialization

void CResourceMonitorDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 여기에 저장 코드를 추가합니다.
	}
	else
	{
		// TODO: 여기에 로딩 코드를 추가합니다.
	}
}

#ifdef SHARED_HANDLERS

// 축소판 그림을 지원합니다.
void CResourceMonitorDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 문서의 데이터를 그리려면 이 코드를 수정하십시오.
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 검색 처리기를 지원합니다.
void CResourceMonitorDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 문서의 데이터에서 검색 콘텐츠를 설정합니다.
	// 콘텐츠 부분은 ";"로 구분되어야 합니다.

	// 예: strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CResourceMonitorDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CResourceMonitorDoc 진단

#ifdef _DEBUG
void CResourceMonitorDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CResourceMonitorDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

