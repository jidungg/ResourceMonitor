// MemMonitorView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ResourceMonitor.h"
#include "MemMonitorView.h"
#include "ResourceMonitorDoc.h"
#include "PerfDataManager.h"

#include <vector>

// CMemMonitorView

IMPLEMENT_DYNCREATE(CMemMonitorView, CResourceMonitorView)

CMemMonitorView::CMemMonitorView()
{
	m_title = _T("Memory");

	m_frameCaptions.push_back(_T("Total Memory(MB):"));
	m_frameCaptions.push_back(_T("Using Memory(MB):"));

	m_tableCaptions.push_back(_T("Usage(KB)"));
	m_tableCaptions.push_back(_T("Name"));
	m_tableCaptions.push_back(_T("PID"));

}

CMemMonitorView::~CMemMonitorView()
{
}


BEGIN_MESSAGE_MAP(CMemMonitorView, CResourceMonitorView)
END_MESSAGE_MAP()


// CMemMonitorView 그리기입니다.

void CMemMonitorView::OnInitialUpdate()
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
void CMemMonitorView::UpdateView(CPerfDataManager * dataManager)
{
	map<ULONGLONG, PerProcessDataObj>	*perfTable = (map<ULONGLONG, PerProcessDataObj> *)dataManager->m_win32PerfFormatProc->m_table;

	map<ULONGLONG, OSDataObj> *osTable =dataManager->m_win32OperatingSystem->m_table;
	for (auto iter = perfTable->begin(); iter != perfTable->end(); iter++)
	{
		CString id;
		id.Format(_T("%lu"), iter->first);
		CString name;
		name = iter->second.name;
		CString workingSet;
		ULONGLONG a = _wtoi64(iter->second.workingSet) / 1024;
		workingSet.Format(_T("%llu"), a);

		LVFINDINFO info;
		int nIndex;
		info.flags = LVFI_STRING;
		info.psz = id;



		if ((nIndex = m_processList.FindItem(&info)) == -1)
		{
			m_processList.InsertItem(0, id);

			m_processList.SetItemText(0, 1, name);
			m_processList.SetItemText(0, 2, workingSet);
		}
		else
		{
			m_processList.SetItemText(nIndex, 1, name);
			m_processList.SetItemText(nIndex, 2, workingSet);
		}
		id.Empty();
		name.Empty();
		workingSet.Empty();

	}

	for (auto iter = osTable->begin(); iter != osTable->end(); iter++)
	{
		CString total;
		ULONGLONG a = iter->first;
		total.Format(_T("%llu"), a / 1024);
		CString nowUsing;
		ULONGLONG b = _wtoi64(iter->second.freePhysicalMemory);
		nowUsing.Format(_T("%llu"), ((a - b) / 1024));


		if (m_farmeList.GetItemCount() == 0)
		{
			m_farmeList.InsertItem(0, _T(" "));
			m_farmeList.SetItemText(0, 1, total);
			m_farmeList.SetItemText(0, 2, nowUsing);
		}
		else
		{
			m_farmeList.SetItemText(0, 1, total);
			m_farmeList.SetItemText(0, 2, nowUsing);
		}
		total.Empty();
		nowUsing.Empty();
	}

}

void CMemMonitorView::AddPeriodicLog()
{
	if (!m_farmeList) return;

	CString str = _T("");
	CHeaderCtrl *pHeaderCtrl;

	pHeaderCtrl = m_farmeList.GetHeaderCtrl();
	int frameColCount = pHeaderCtrl->GetItemCount() -1;
	for (int i = 0; i< frameColCount; ++i)
	{
		str += m_frameCaptions[i] ;
		str += m_farmeList.GetItemText(0, i+1) + _T(", ");
	}
	sLogger.AddLog(CLogger::LOG_MEMORY, str);
	str.Empty();

	pHeaderCtrl = m_processList.GetHeaderCtrl();
	int tableColCount = pHeaderCtrl->GetItemCount();
	int tableItemCount = m_processList.GetItemCount();
	for (int j = 0; j < tableItemCount; j++)
	{
		ULONGLONG memUsage = _wtoi64(m_processList.GetItemText(j, 2));
		if (memUsage >= GetDocument()->m_memThreshold)
		{
			str = _T("");
			for (int i = 0; i < tableColCount; ++i)
			{
				str += m_tableCaptions[tableColCount - i - 1] + _T(": ");
				str += m_processList.GetItemText(j, i) + _T(", ");
			}
			sLogger.AddLog(CLogger::LOG_MEMORY, str);
			str.Empty();
		}

	}
}
//void CMemMonitorView::OnDraw(CDC* pDC)
//{
//	CResourceMonitorDoc* pDoc = GetDocument();
//	// TODO: 여기에 그리기 코드를 추가합니다.
//}


// CMemMonitorView 진단입니다.

#ifdef _DEBUG

void CMemMonitorView::AssertValid() const
{
	CScrollView::AssertValid();
}

#ifndef _WIN32_WCE
void CMemMonitorView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

#endif
#endif //_DEBUG


// CMemMonitorView 메시지 처리기입니다.
