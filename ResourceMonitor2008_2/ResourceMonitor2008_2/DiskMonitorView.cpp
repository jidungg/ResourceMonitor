// DiskMonitorView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ResourceMonitor2008_2.h"
#include "DiskMonitorView.h"
#include "ResourceMonitorDoc.h"
#include "PerfDataManager.h"
#include "PerfDataLogicalDisk.h"

// CDiskMonitorView
using namespace std;

IMPLEMENT_DYNCREATE(CDiskMonitorView, CResourceMonitorView)

CDiskMonitorView::CDiskMonitorView()
{
	m_title = _T("Disk");
	m_frameCaptions.push_back(_T("Total(MB): "));
	m_frameCaptions.push_back(_T("Using(MB): "));

	m_tableCaptions.push_back(_T("Write(B/s)"));
	m_tableCaptions.push_back(_T("Read(B/s)"));
	m_tableCaptions.push_back(_T("Name"));
	m_tableCaptions.push_back(_T("PID"));

	m_frameWidth = 3* FRAME_WIDTH_ITEM;
}

CDiskMonitorView::~CDiskMonitorView()
{
}


BEGIN_MESSAGE_MAP(CDiskMonitorView, CResourceMonitorView)
END_MESSAGE_MAP()


// CDiskMonitorView 그리기입니다.

void CDiskMonitorView::OnInitialUpdate()
{/*
	map<CString, LogicalDiskDataObj>	*diskTable =dataManager->m_win32DiskDrive->m_table;
	int diskCount = diskTable->size();*/

	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO: 이 뷰의 전체 크기를 계산합니다.
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);

	CResourceMonitorDoc* pDoc = GetDocument();

	InitTable();
	InitFrame();
}
void CDiskMonitorView::OnSize(UINT nType, int cx, int cy)
{
	CScrollView::OnSize(nType, cx, cy);
	if (!m_bInit)
		return;
	m_processList.MoveWindow(0, 3* FRAME_WIDTH_ITEM, cx, cy-(3* FRAME_WIDTH_ITEM),TRUE);
	m_farmeList.MoveWindow(0, 0, cx, 3* FRAME_WIDTH_ITEM, TRUE);
	m_farmeList.ShowScrollBar(SB_BOTH, FALSE);
}
void CDiskMonitorView::UpdateView(CPerfDataManager * dataManager)
{

	map<ULONGLONG, PerProcessDataObj>	*perfTable =dataManager->m_win32PerfFormatProc->m_table;
	map<CString, LogicalDiskDataObj>	*diskTable =dataManager->m_win32DiskDrive->m_table;

	for (map<ULONGLONG, PerProcessDataObj>::iterator iter = perfTable->begin(); iter != perfTable->end(); iter++)
	{
		CString id;
		id.Format(_T("%lu"), iter->first);
		CString name;
		name = iter->second.name;
		CString ioRead;
		ioRead = iter->second.ioRead;
		CString ioWrite;
		ioWrite = iter->second.ioWrite;

		LVFINDINFO info;
		int nIndex;
		info.flags = LVFI_STRING;
		info.psz = id;

		if ((nIndex = m_processList.FindItem(&info)) == -1)
		{
			m_processList.InsertItem(0, id);

			m_processList.SetItemText(0, 1, name);
			m_processList.SetItemText(0, 2, ioRead);
			m_processList.SetItemText(0, 3, ioWrite);
		}
		else
		{
			m_processList.SetItemText(nIndex, 1, name);
			m_processList.SetItemText(nIndex, 2, ioRead);
			m_processList.SetItemText(nIndex, 3, ioWrite);
		}
		id.Empty();
		name.Empty();
		ioRead.Empty();
		ioWrite.Empty();
	}
    //lstItems.AddTail(L"Item 1");  
    //lstItems.AddTail(L"Item 2"); 

	//m_farmeList.lstItems.RemoveAll();

	for (map<CString, LogicalDiskDataObj>::iterator iter = diskTable->begin(); iter != diskTable->end(); iter++)
	{
		CString id;
		id = iter->first;

		//m_farmeList.lstItems.AddTail(id);

		//if(id.Compare(m_farmeList.GetItemText(0,0)) != 0)
		//{
		//	continue;
		//}
		CString totalSpace;
		ULONGLONG itotalSpace = _wtoi64(iter->second.size);
		totalSpace.Format(_T("%llu"), itotalSpace/1024/1024);

		CString usingSapce;
		ULONGLONG iusingSpace = itotalSpace - _wtoi64(iter->second.freeSpace);
		usingSapce.Format(_T("%llu"), iusingSpace/1024/1024);



		//if (m_farmeList.GetItemCount() == 0)
		//{
		//	m_farmeList.InsertItem(0, id);
		//	m_farmeList.SetItemText(0, 1, totalSpace);
		//	m_farmeList.SetItemText(0, 2, usingSapce);
		//}else
		//{
		//	m_farmeList.SetItemText(0, 0, id);
		//	m_farmeList.SetItemText(0, 1, totalSpace);
		//	m_farmeList.SetItemText(0, 2, usingSapce);
		//}
		LVFINDINFO info;
		int nIndex;
		info.flags = LVFI_STRING;
		info.psz = id;

		if ((nIndex = m_farmeList.FindItem(&info)) == -1)
		{
			m_farmeList.InsertItem(0, id);

			m_farmeList.SetItemText(0, 1, totalSpace);
			m_farmeList.SetItemText(0, 2, usingSapce);
		}
		else
		{
			m_farmeList.SetItemText(nIndex, 1, totalSpace);
			m_farmeList.SetItemText(nIndex, 2, usingSapce);
		}
		
	}

}

void CDiskMonitorView::AddPeriodicLog()
{
}

//void CDiskMonitorView::OnDraw(CDC* pDC)
//{
//	CResourceMonitorDoc* pDoc = GetDocument();
//	// TODO: 여기에 그리기 코드를 추가합니다.
//
//}


// CDiskMonitorView 진단입니다.

#ifdef _DEBUG
void CDiskMonitorView::AssertValid() const
{
	CScrollView::AssertValid();
}

#ifndef _WIN32_WCE
void CDiskMonitorView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}


#endif
#endif //_DEBUG


// CDiskMonitorView 메시지 처리기입니다.
