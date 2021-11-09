
// ResourceMonitorDoc.cpp : CResourceMonitorDoc Ŭ������ ����
//

#include "stdafx.h"
// SHARED_HANDLERS�� �̸� ����, ����� �׸� �� �˻� ���� ó���⸦ �����ϴ� ATL ������Ʈ���� ������ �� ������
// �ش� ������Ʈ�� ���� �ڵ带 �����ϵ��� �� �ݴϴ�.
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


// CResourceMonitorDoc ����/�Ҹ�

CResourceMonitorDoc::CResourceMonitorDoc() 
{
	// TODO: ���⿡ ��ȸ�� ���� �ڵ带 �߰��մϴ�.
	m_perfDataManager = new CPerfDataManager(this);

}

CResourceMonitorDoc::~CResourceMonitorDoc()
{
	//delete m_perfDataManager;
}

void CResourceMonitorDoc::UpdateViews(CResourceMonitorDoc *doc)
{
	POSITION pos = doc->GetFirstViewPosition();
	CResourceMonitorView* pView1 = (CResourceMonitorView*)doc->GetNextView(pos);
	CResourceMonitorView* pView2 = (CResourceMonitorView*)doc->GetNextView(pos);
	CResourceMonitorView* pView3 = (CResourceMonitorView*)doc->GetNextView(pos);
	CResourceMonitorView* pView4 = (CResourceMonitorView*)doc->GetNextView(pos);

	while (true)
	{

		pView1->UpdateView(doc->m_perfDataManager);
		pView2->UpdateView(doc->m_perfDataManager);
		pView3->UpdateView(doc->m_perfDataManager);
		pView4->UpdateView(doc->m_perfDataManager);
		Sleep(UPDATE_INTERVAL);
	}
}

void CResourceMonitorDoc::AddPeriodicLog(CResourceMonitorDoc *doc)
{
	POSITION pos = doc->GetFirstViewPosition();
	CResourceMonitorView* pView1 = (CResourceMonitorView*)doc->GetNextView(pos);
	CResourceMonitorView* pView2 = (CResourceMonitorView*)doc->GetNextView(pos);
	CResourceMonitorView* pView3 = (CResourceMonitorView*)doc->GetNextView(pos);
	CResourceMonitorView* pView4 = (CResourceMonitorView*)doc->GetNextView(pos);
	while (true)
	{

		pView1->AddPeriodicLog();
		pView2->AddPeriodicLog();
		pView3->AddPeriodicLog();
		pView4->AddPeriodicLog();
		Sleep(LOG_INTERVAL);
	}
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

BOOL CResourceMonitorDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	m_perfDataManager->m_pDoc = this;
	m_perfDataManager->StartThread();

	thread t1(UpdateViews,this);
	t1.detach();
	thread t2(AddPeriodicLog, this);
	t2.detach();
	// TODO: ���⿡ ���ʱ�ȭ �ڵ带 �߰��մϴ�.
	// SDI ������ �� ������ �ٽ� ����մϴ�.

	return TRUE;
}




// CResourceMonitorDoc serialization

void CResourceMonitorDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	}
	else
	{
		// TODO: ���⿡ �ε� �ڵ带 �߰��մϴ�.
	}
}

#ifdef SHARED_HANDLERS

// ����� �׸��� �����մϴ�.
void CResourceMonitorDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// ������ �����͸� �׸����� �� �ڵ带 �����Ͻʽÿ�.
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

// �˻� ó���⸦ �����մϴ�.
void CResourceMonitorDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// ������ �����Ϳ��� �˻� �������� �����մϴ�.
	// ������ �κ��� ";"�� ���еǾ�� �մϴ�.

	// ��: strSearchContent = _T("point;rectangle;circle;ole object;");
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

// CResourceMonitorDoc ����

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


