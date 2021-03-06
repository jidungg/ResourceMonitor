// MemMonitorView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../ResourceMonitor2008_2.h"
#include "MemMonitorView.h"
#include "../ResourceMonitorDoc.h"
#include "../PerfData/PerfDataManager.h"

#include <vector>

// CMemMonitorView

IMPLEMENT_DYNCREATE(CMemMonitorView, CResourceMonitorView)

CMemMonitorView::CMemMonitorView()
{
	m_title = _T("Memory");

	m_frameCaptions.push_back(_T("Total Memory(MB):"));
	m_frameCaptions.push_back(_T("Using Memory(MB):"));

	m_tableCaptions.push_back(_T("Private(KB)"));
	m_tableCaptions.push_back(_T("WorkingSet(KB)"));
	m_tableCaptions.push_back(_T("Virtual(KB)"));
	m_tableCaptions.push_back(_T("Name"));
	m_tableCaptions.push_back(_T("PID"));
	m_frameWidth = FRAME_WIDTH;
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
	for (map<ULONGLONG, PerProcessDataObj>::iterator iter = perfTable->begin(); iter != perfTable->end(); iter++)
	{
		CString id;
		id.Format(_T("%lu"), iter->first);
		CString name;
		name = iter->second.name;
		CString workingSet;
		ULONGLONG iworkingSet = _wtoi64(iter->second.workingSet) / 1024;
		workingSet.Format(_T("%llu"), iworkingSet);
		CString virtualBytes;
		ULONGLONG ivirtualBytes = _wtoi64(iter->second.virtualBytes) / 1024;
		virtualBytes.Format(_T("%llu"), ivirtualBytes);
		CString privateBytes;
		ULONGLONG iprivateBytes = _wtoi64(iter->second.privateBytes) / 1024;
		privateBytes.Format(_T("%llu"), iprivateBytes);

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
		m_processList.SetItemText(nIndex, 2, virtualBytes);
		m_processList.SetItemText(nIndex, 3, workingSet);
		m_processList.SetItemText(nIndex, 4, privateBytes);
		
		id.Empty();
		name.Empty();
		workingSet.Empty();
		virtualBytes.Empty();
		privateBytes.Empty();
	}

	for (map<ULONGLONG, OSDataObj>::iterator iter = osTable->begin(); iter != osTable->end(); iter++)
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
	CLogger::GetInstance().AddLog(CLogger::LOG_MEMORY, str);
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
			CLogger::GetInstance().AddLog(CLogger::LOG_MEMORY, str);
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
