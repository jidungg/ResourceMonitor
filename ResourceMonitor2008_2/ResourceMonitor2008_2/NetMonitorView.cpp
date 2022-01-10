// NetMonitorView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ResourceMonitor2008_2.h"
#include "NetMonitorView.h"
#include "ResourceMonitorDoc.h"
#include "PerfDataManager.h"
#include "Etw.h"

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
	m_frameWidth = FRAME_WIDTH;
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

	map<ULONG, ProcessNetworkData>	*table = &dataManager->m_etw->networkMap;
	for (map<ULONG, ProcessNetworkData>::iterator iter = table->begin(); iter != table->end(); iter++)
	{
		CString id;
		id.Format(_T("%lu"), iter->first);
		//CString name;
		//name = iter->second.name;
		CString ioReceive;
		ioReceive.Format(_T("%lu"),iter->second.receiveBytes);
		CString ioSend;
		ioSend.Format(_T("%lu"), iter->second.sendBytes) ;

		LVFINDINFO info;
		int nIndex;
		info.flags = LVFI_STRING;
		info.psz = id;
		if ((nIndex = m_processList.FindItem(&info)) == -1)
		{
			m_processList.InsertItem(0, id);

			//m_processList.SetItemText(0, 1, name);
			m_processList.SetItemText(0, 2, ioSend);
			m_processList.SetItemText(0, 3, ioReceive);

		}
		else
		{
			//m_processList.SetItemText(nIndex, 1, name);
			m_processList.SetItemText(nIndex, 2, ioSend);
			m_processList.SetItemText(nIndex, 3, ioReceive);
		}
		id.Empty();
		//name.Empty();

	}	
	CString totalIO;
	totalIO.Format(_T("%lu"),dataManager->m_etw->m_totalIO/1024);
	if(m_farmeList.GetItemCount() == 0)
	{
		m_farmeList.InsertItem(0,_T(""));
	}
	m_farmeList.SetItemText(0,1,totalIO);

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
