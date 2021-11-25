// NetMonitorView.cpp : ���� �����Դϴ�.
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


// CNetMonitorView �׸����Դϴ�.

void CNetMonitorView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO: �� ���� ��ü ũ�⸦ ����մϴ�.
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);

	CResourceMonitorDoc* pDoc = GetDocument();

	InitTable();
	InitFrame();
}
void CNetMonitorView::UpdateView(CPerfDataManager * dataManager)
{
	map<ULONGLONG, PerProcessDataObj>	*procTable = dataManager->m_win32PerfFormatProc->m_table;
	map<INT,NetworkPerformanceItem> *netTable = dataManager->m_netPerfScanner->m_table;

	CString bytesIn;
	CString bytesOut;

	for (map<ULONGLONG, PerProcessDataObj>::iterator iter = procTable->begin(); iter != procTable->end(); iter++)
	{
		CString id;
		id.Format(_T("%lu"), iter->first);
		CString name;
		name = iter->second.name;
		LVFINDINFO info;
		int nIndex;
		info.flags = LVFI_STRING;
		info.psz = id;
		if ((nIndex = m_processList.FindItem(&info)) == -1)
		{
			m_processList.InsertItem(0, id);

			m_processList.SetItemText(0, 1, name);

		}
		else
		{
			m_processList.SetItemText(nIndex, 1, name);

		}
		bytesIn = _T("0");
		bytesOut = _T("0");

		id.Empty();
		name.Empty();
	}
	
	for(map<INT,NetworkPerformanceItem>::iterator iter = netTable->begin(); iter != netTable->end(); iter++)
	{
		CString id;
		id.Format(_T("%d"), iter->first);

		bytesIn.Format(_T("%llu"),iter->second.BytesIn);
		bytesOut.Format(_T("%llu"),iter->second.BytesOut);
		LVFINDINFO info;
		int nIndex;
		info.flags = LVFI_STRING;
		info.psz = id;

		if ((nIndex = m_processList.FindItem(&info)) != -1)
		{
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
//	// TODO: ���⿡ �׸��� �ڵ带 �߰��մϴ�.
//}


// CNetMonitorView �����Դϴ�.

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


// CNetMonitorView �޽��� ó�����Դϴ�.
