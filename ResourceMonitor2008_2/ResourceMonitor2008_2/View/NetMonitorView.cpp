// NetMonitorView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../ResourceMonitor2008_2.h"
#include "NetMonitorView.h"
#include "../ResourceMonitorDoc.h"
#include "../PerfData/PerfDataManager.h"

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

	map<ULONG, ProcessNetworkData>	*networkTable = &dataManager->m_etw->networkMap;
	map<ULONG, PerProcessDataObj> * processTable = dataManager->m_win32PerfFormatProc->m_table;
	for (map<ULONG, ProcessNetworkData>::iterator iter = networkTable->begin(); iter != networkTable->end(); iter++)
	{
		CString id;
		id.Format(_T("%lu"), iter->first);
		CString name;
		name = (*processTable)[iter->first].name;
		CString ioReceive;
		ioReceive.Format(_T("%lu"),iter->second.receiveBytes / iter->second.averageLength);
		CString ioSend;
		ioSend.Format(_T("%lu"), iter->second.sendBytes /iter->second.averageLength) ;

		LVFINDINFO info;
		int nIndex;
		info.flags = LVFI_STRING;
		info.psz = id;
		if ((nIndex = m_processList.FindItem(&info)) == -1)
		{
			nIndex = m_processList.GetItemCount();
			m_processList.InsertItem(nIndex, id);

		}
		m_processList.SetItemText(nIndex, 1, name);
		m_processList.SetItemText(nIndex, 2, ioSend);
		m_processList.SetItemText(nIndex, 3, ioReceive);
		

	}	
	CString totalIO;
	totalIO.Format(_T("%lu"),dataManager->m_etw->m_netTotalIO/1024/dataManager->m_etw->m_totlaAvgLength);
	if(m_farmeList.GetItemCount() == 0)
	{
		m_farmeList.InsertItem(0,_T(""));
	}
	m_farmeList.SetItemText(0,1,totalIO);

}
void CNetMonitorView::AddPeriodicLog()
{
	if (!m_farmeList) return;
	CString str = _T("");
	CHeaderCtrl *pHeaderCtrl;

	pHeaderCtrl = m_farmeList.GetHeaderCtrl();
	int frameColCount = pHeaderCtrl->GetItemCount() - 1;
	for (int i = 0; i< frameColCount; ++i)
	{
		str += m_frameCaptions[i] + _T(": ");
		str += m_farmeList.GetItemText(0, i + 1) + _T(", ");
	}
	CLogger::GetInstance().AddLog(CLogger::LOG_NETWORK, str);

	pHeaderCtrl = m_processList.GetHeaderCtrl();
	int tableColCount = pHeaderCtrl->GetItemCount();
	int tableItemCount = m_processList.GetItemCount();
	for (int j = 0; j < tableItemCount; j++)
	{
		ULONG receiveBytes = _wtof(m_processList.GetItemText(j, 2));
		ULONG sendBytes = _wtof(m_processList.GetItemText(j, 3));
		if (receiveBytes >= GetDocument()->m_networkThreshold  
			|| sendBytes >= GetDocument()->m_networkThreshold)
		{
			str = _T("");
			for (int i = 0; i < tableColCount; ++i)
			{

				str += m_tableCaptions[tableColCount - i - 1] + _T(": ");
				str += m_processList.GetItemText(j, i) + _T(", ");
			}
			CLogger::GetInstance().AddLog(CLogger::LOG_NETWORK, str);
			str.Empty();
		}

	}
}


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
