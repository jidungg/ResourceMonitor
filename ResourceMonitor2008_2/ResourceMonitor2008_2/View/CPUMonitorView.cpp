// CPUMonitorView.cpp : 구현 파일입니다.
//
#include "stdafx.h"

#include "../ResourceMonitor2008_2.h"
#include "CPUMonitorView.h"
#include "../ResourceMonitorDoc.h"
#include "../PerfData/PerfDataManager.h"
// CCPUMonitorView

IMPLEMENT_DYNCREATE(CCPUMonitorView, CResourceMonitorView)

CCPUMonitorView::CCPUMonitorView()
{

	m_title = _T("CPU");
	m_frameCaptions.push_back(_T("Usage rate"));

	m_tableCaptions.push_back(_T("Average"));
	m_tableCaptions.push_back(_T("Usage"));
	m_tableCaptions.push_back(_T("Name"));
	m_tableCaptions.push_back(_T("PID"));
	m_frameWidth = FRAME_WIDTH;
}

CCPUMonitorView::~CCPUMonitorView()
{
}


BEGIN_MESSAGE_MAP(CCPUMonitorView, CResourceMonitorView)
END_MESSAGE_MAP()


// CCPUMonitorView 그리기입니다.

void CCPUMonitorView::OnInitialUpdate()
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
void CCPUMonitorView::UpdateView(CPerfDataManager * dataManager)
{
	CPerfDataPerProcess *perfData = dataManager->m_win32PerfFormatProc;
	CPerfDataOSProcessor *pefDataOSProc = dataManager->m_win32OSProcessor;		//PerfDataOSProcessor 클래스 사용 안할 시 주석

	map<ULONG, PerProcessDataObj>	*table = perfData->m_table;
	map<ULONG, OSProcessorDataObj>	*tableOSProc = pefDataOSProc->m_table;		//PerfDataOSProcessor 클래스 사용 안할 시 주석

	for (map<ULONG, PerProcessDataObj>::iterator iter = table->begin(); iter != table->end();iter++)
	{
		CString id;
		id.Format(_T("%lu"), iter->first);
		CString name;
		name = iter->second.name;
		CString percent;
		percent = iter->second.usageRate;
		CString meanPercent;
		meanPercent = iter->second.meanUsageRate;
		
		//PerfDataOSProcessor 클래스 사용 안할 시 주석해제
		//if (id.Compare(_T("0")) == 0 )
		//{
		//	if (m_farmeList.GetItemCount() == 0)
		//	{
		//		m_farmeList.InsertItem(0, _T(" "));
		//	}
		//	m_farmeList.SetItemText(0, 1, percent);
		//}

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
		m_processList.SetItemText(nIndex, 2, percent);
		m_processList.SetItemText(nIndex, 3, meanPercent);

	}

	//PerfDataOSProcessor 클래스 사용 안할 시 주석
	for (map<ULONG, OSProcessorDataObj>::iterator iter = tableOSProc->begin(); iter != tableOSProc->end();iter++)
	{
		CString total;
		total = iter->second.cpuTotal;
		CString id;
		id = iter->second.name;
		if(id.Compare(_T("_Total")) == 0)
		{
			if (m_farmeList.GetItemCount() == 0)
			{
				m_farmeList.InsertItem(0, _T(" "));
			}
			m_farmeList.SetItemText(0, 1, total);
		}

	}

}
void CCPUMonitorView::AddPeriodicLog()
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
	CLogger::GetInstance().AddLog(CLogger::LOG_CPU, str);

	pHeaderCtrl = m_processList.GetHeaderCtrl();
	int tableColCount = pHeaderCtrl->GetItemCount();
	int tableItemCount = m_processList.GetItemCount();
	for (int j = 0; j < tableItemCount; j++)
	{
		float useRate = _wtof(m_processList.GetItemText(j, 2));
		
		if (useRate >= GetDocument()->m_cpuThreshold)
		{
			str = _T("");
			for (int i = 0; i < tableColCount; ++i)
			{

				str += m_tableCaptions[tableColCount - i - 1] + _T(": ");
				str += m_processList.GetItemText(j, i) + _T(", ");
			}
			CLogger::GetInstance().AddLog(CLogger::LOG_CPU, str);
			str.Empty();
		}

	}

}

//void CCPUMonitorView::OnDraw(CDC* pDC)
//{
//	CResourceMonitorDoc* pDoc = GetDocument();
//	// TODO: 여기에 그리기 코드를 추가합니다.
//	
//}

// CCPUMonitorView 진단입니다.

#ifdef _DEBUG

void CCPUMonitorView::AssertValid() const
{
	CScrollView::AssertValid();
}

#ifndef _WIN32_WCE
void CCPUMonitorView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

#endif
#endif //_DEBUG


// CCPUMonitorView 메시지 처리기입니다.
