
// ResourceMonitorDoc.cpp : CResourceMonitorDoc 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "ResourceMonitor.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "ResourceMonitorView.h"
#include "ResourceMonitorDoc.h"
#include "PerfDataManager.h"
#include "Logger.h"
#include <propkey.h>
#include <vector>
#include <thread>


using namespace std;
// CResourceMonitorDoc

IMPLEMENT_DYNCREATE(CResourceMonitorDoc, CDocument)

BEGIN_MESSAGE_MAP(CResourceMonitorDoc, CDocument)
END_MESSAGE_MAP()


// CResourceMonitorDoc 생성/소멸

CResourceMonitorDoc::CResourceMonitorDoc() 
{
	// TODO: 여기에 일회성 생성 코드를 추가합니다.
	m_perfDataManager = new CPerfDataManager(this);

}

CResourceMonitorDoc::~CResourceMonitorDoc()
{
	delete m_perfDataManager;
	m_perfDataManager = nullptr;
}

UINT CResourceMonitorDoc::Update(LPVOID doc)
{
	CResourceMonitorDoc* pDoc = (CResourceMonitorDoc*)doc;
	POSITION pos = pDoc->GetFirstViewPosition();
	CResourceMonitorView* pView1 = (CResourceMonitorView*)pDoc->GetNextView(pos);
	CResourceMonitorView* pView2 = (CResourceMonitorView*)pDoc->GetNextView(pos);
	CResourceMonitorView* pView3 = (CResourceMonitorView*)pDoc->GetNextView(pos);
	CResourceMonitorView* pView4 = (CResourceMonitorView*)pDoc->GetNextView(pos);
	CPerfDataManager *dataManager = pDoc->m_perfDataManager;

	while (true)
	{
		if (pDoc->m_isExit == TRUE)
		{
			break;
		}
		{
			dataManager->RefreshData();

		}


		pView1->UpdateView(dataManager);
		pView2->UpdateView(dataManager);
		pView3->UpdateView(dataManager);
		pView4->UpdateView(dataManager);

		Sleep(UPDATE_INTERVAL);
	}
	TRACE("Update Func Out!!\n");
	return EXIT_CODE;
}



UINT CResourceMonitorDoc::AddPeriodicLog(LPVOID doc)
{
	CResourceMonitorDoc * pDoc = (CResourceMonitorDoc *)doc;
	POSITION pos = pDoc->GetFirstViewPosition();
	CResourceMonitorView* pView1 = (CResourceMonitorView*)pDoc->GetNextView(pos);
	CResourceMonitorView* pView2 = (CResourceMonitorView*)pDoc->GetNextView(pos);
	CResourceMonitorView* pView3 = (CResourceMonitorView*)pDoc->GetNextView(pos);
	CResourceMonitorView* pView4 = (CResourceMonitorView*)pDoc->GetNextView(pos);
	while (true)
	{
		if (pDoc->m_isExit == TRUE)
		{
			break;
		}
		pView1->AddPeriodicLog();
		pView2->AddPeriodicLog();
		pView3->AddPeriodicLog();
		pView4->AddPeriodicLog();
		Sleep(LOG_INTERVAL);
	}
	TRACE("AddPeriodicLog Func Out!!\n");
	return EXIT_CODE;
}

void CResourceMonitorDoc::AtExitProcess(vector<ULONGLONG>* exitedProcIDs)
{
	if (this == nullptr)return;
	POSITION pos = this->GetFirstViewPosition();
	CResourceMonitorView* pView1 = (CResourceMonitorView*)this->GetNextView(pos);
	CResourceMonitorView* pView2 = (CResourceMonitorView*)this->GetNextView(pos);
	CResourceMonitorView* pView3 = (CResourceMonitorView*)this->GetNextView(pos);
	CResourceMonitorView* pView4 = (CResourceMonitorView*)this->GetNextView(pos);

	pView1->RemoveProcessFromList(exitedProcIDs);
	pView2->RemoveProcessFromList(exitedProcIDs);
	pView3->RemoveProcessFromList(exitedProcIDs);
	pView4->RemoveProcessFromList(exitedProcIDs);
}

void CResourceMonitorDoc::ExitThread()
{
	DWORD dwRetCode = WaitForSingleObject(m_updaterThread->m_hThread, INFINITE);
	if (dwRetCode == WAIT_OBJECT_0)
	{
		TRACE("m_updaterThread is terminated normally!\n");
	}
	else if (dwRetCode == WAIT_TIMEOUT) {
		::TerminateThread(m_updaterThread->m_hThread, 0);
		TRACE("m_updaterThread is terminated FORCELY!");
	}

	delete m_updaterThread;
	m_updaterThread = nullptr;

	dwRetCode = WaitForSingleObject(m_loggerThread->m_hThread, LOG_INTERVAL);
	if (dwRetCode == WAIT_OBJECT_0)
	{
		TRACE("m_loggerThread is terminated normally!\n");
	}
	else if (dwRetCode == WAIT_TIMEOUT) {
		::TerminateThread(m_loggerThread->m_hThread, 0);
		TRACE("m_loggerThread is terminated FORCELY!");
	}

	delete m_loggerThread;
	m_loggerThread = nullptr;
}

BOOL CResourceMonitorDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	m_perfDataManager->m_pDoc = this;


	 //thread 생성
	m_updaterThread = AfxBeginThread(Update, this, 0, 0, CREATE_SUSPENDED, 0);
	m_updaterThread->m_bAutoDelete = FALSE;
	m_updaterThread->ResumeThread();

	m_loggerThread = AfxBeginThread(AddPeriodicLog, this, 0, 0, CREATE_SUSPENDED, 0);
	m_loggerThread->m_bAutoDelete = FALSE;
	m_loggerThread->ResumeThread();

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



void AddLog(CLogger::LogDirectory nflag, LPCTSTR lpszFormat, ...)
{
	
	CString str;
	switch (nflag)
	{
	case m_Logger.LOG_CPU:
		str = "PerfData_CPU";
		break;
	case m_Logger.LOG_MEMORY:
		str = "PerfData_Memory";
		break;
	case m_Logger.LOG_DISK:
		str = "PerfData_Disk";
		break;
	case m_Logger.LOG_NETWORK:
		str = "PerfData_Network";
		break;
	}
	if (m_Logger.IsCreate(nflag) != TRUE)
	{
		m_Logger.Create(nflag, LOG_DIRECTORY, str);
	}

	va_list args;
	va_start(args, lpszFormat);

	int nBuf;
	TCHAR szBuffer[4096];

	nBuf = _vsntprintf_s(szBuffer, _countof(szBuffer), lpszFormat, args);

	va_end(args);

	m_Logger.AppendLogData(nflag, szBuffer);
}


