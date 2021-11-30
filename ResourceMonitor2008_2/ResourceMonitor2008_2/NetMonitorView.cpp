// NetMonitorView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ResourceMonitor2008_2.h"
#include "NetMonitorView.h"
#include "ResourceMonitorDoc.h"
#include "PerfDataManager.h"
// CNetMonitorView

IMPLEMENT_DYNCREATE(CNetMonitorView, CResourceMonitorView)

CNetMonitorView::CNetMonitorView()
{
	m_title = _T("Network");
	m_frameCaptions.push_back( _T("Total I/O(KB/s):"));

	m_tableCaptions.push_back(_T("Recv(B/s)"));
	m_tableCaptions.push_back(_T("Send(B/s)"));
	m_tableCaptions.push_back(_T("Name"));
	m_tableCaptions.push_back(_T("PID"));

}

CNetMonitorView::~CNetMonitorView()
{
}


BEGIN_MESSAGE_MAP(CNetMonitorView, CResourceMonitorView)
END_MESSAGE_MAP()


// CNetMonitorView 그리기입니다.

void CNetMonitorView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO: 이 뷰의 전체 크기를 계산합니다.
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);

	CResourceMonitorDoc* pDoc = GetDocument();

	InitTable();
	InitFrame();
}
void CNetMonitorView::UpdateView(CPerfDataManager * dataManager)
{
	map<ULONGLONG, PerProcessDataObj>	*procTable = dataManager->m_win32PerfFormatProc->m_table;
	map<ULONGLONG,NetworkPerformanceItem> *netTable = dataManager->m_netPerfScanner->m_diffTable;

	CString bytesIn;
	CString bytesOut;
	CString id;
	CString name;

	//for (map<ULONGLONG, PerProcessDataObj>::iterator iter = procTable->begin(); iter != procTable->end(); iter++)
	//{

	//	id.Format(_T("%lu"), iter->first);
	//	CString name;
	//	name = iter->second.name;
	//	LVFINDINFO info;
	//	int nIndex;
	//	info.flags = LVFI_STRING;
	//	info.psz = id;
	//	if ((nIndex = m_processList.FindItem(&info)) == -1)
	//	{
	//		m_processList.InsertItem(0, id);

	//		m_processList.SetItemText(0, 1, name);

	//	}
	//	else
	//	{
	//		m_processList.SetItemText(nIndex, 1, name);

	//	}
	//	bytesIn = _T("0");
	//	bytesOut = _T("0");

	//	id.Empty();
	//	name.Empty();
	//}
	//nettable 순회하면서 bytesin, out 쓰기
	for(map<ULONGLONG,NetworkPerformanceItem>::iterator iter = netTable->begin(); iter != netTable->end(); iter++)
	{

		id.Format(_T("%d"), iter->first);
		map<ULONGLONG, PerProcessDataObj>::iterator foundIt = procTable->find(iter->first);
		if(foundIt != procTable->end())
		{
			name = foundIt->second.name;
		}
		bytesIn.Format(_T("%llu"),iter->second.BytesIn);
		bytesOut.Format(_T("%llu"),iter->second.BytesOut);

		LVFINDINFO info;
		int nIndex;
		info.flags = LVFI_STRING;
		info.psz = id;
		if ((nIndex = m_processList.FindItem(&info)) == -1)
		{
			m_processList.InsertItem(0, id);
			m_processList.SetItemText(0,1, name);
			m_processList.SetItemText(0,2, bytesIn);
			m_processList.SetItemText(0,3, bytesOut);

		}
		else
		{
			m_processList.SetItemText(nIndex,1, name);
			m_processList.SetItemText(nIndex,2, bytesIn);
			m_processList.SetItemText(nIndex,3, bytesOut);

		}

	}
}
void CNetMonitorView::AddPeriodicLog()
{
}
//void CNetMonitorView::OnDraw(CDC* pDC)
//{
//	CResourceMonitorDoc* pDoc = GetDocument();
//	// TODO: 여기에 그리기 코드를 추가합니다.
//}


// CNetMonitorView 진단입니다.

#ifdef _DEBUG

void CNetMonitorView::AssertValid() const
{
	CScrollView::AssertValid();
}

#ifndef _WIN32_WCE
void CNetMonitorView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

#endif
#endif //_DEBUG


// CNetMonitorView 메시지 처리기입니다.
