// DiskMonitorView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../ResourceMonitor2008_2.h"
#include "DiskMonitorView.h"
#include "../ResourceMonitorDoc.h"
#include "../PerfData/PerfDataManager.h"
//#include "../PerfDataPerfDataLogicalDisk.h"
#include "../PerfData/Etw/Etw.h"

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

	map<ULONG, ProcessDiskData>	*perfTable = &dataManager->m_etw->diskMap;
	map<CString, LogicalDiskDataObj>	*diskTable =dataManager->m_win32DiskDrive->m_table;
	map<ULONG, PerProcessDataObj> * processTable = dataManager->m_win32PerfFormatProc->m_table;

	for (map<ULONG, ProcessDiskData>::iterator iter = perfTable->begin(); iter != perfTable->end(); iter++)
	{
		CString id;
		id.Format(_T("%lu"), iter->first);
		CString name;
		name = (*processTable)[iter->first].name;
		CString ioRead;
		ioRead.Format(_T("%lu"),iter->second.readBtyes /iter->second.averageLength);
		CString ioWrite;
		ioWrite.Format(_T("%lu"), iter->second.writeBytes / iter->second.averageLength) ;

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
		m_processList.SetItemText(nIndex, 2, ioRead);
		m_processList.SetItemText(nIndex, 3, ioWrite);
		
	}


	for (map<CString, LogicalDiskDataObj>::iterator iter = diskTable->begin(); iter != diskTable->end(); iter++)
	{
		CString id;
		id = iter->first;


		CString totalSpace;
		ULONGLONG itotalSpace = _wtoi64(iter->second.size);
		totalSpace.Format(_T("%llu"), itotalSpace/1024/1024);

		CString usingSapce;
		ULONGLONG iusingSpace = itotalSpace - _wtoi64(iter->second.freeSpace);
		usingSapce.Format(_T("%llu"), iusingSpace/1024/1024);


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
	CLogger::GetInstance().AddLog(CLogger::LOG_DISK, str);

	pHeaderCtrl = m_processList.GetHeaderCtrl();
	int tableColCount = pHeaderCtrl->GetItemCount();
	int tableItemCount = m_processList.GetItemCount();
	for (int j = 0; j < tableItemCount; j++)
	{
		ULONG readBytes = _wtof(m_processList.GetItemText(j, 2));
		ULONG writeBytes = _wtof(m_processList.GetItemText(j, 3));
		if (readBytes >= GetDocument()->m_diskThreshold  
			|| writeBytes >= GetDocument()->m_diskThreshold)
		{
			str = _T("");
			for (int i = 0; i < tableColCount; ++i)
			{

				str += m_tableCaptions[tableColCount - i - 1] + _T(": ");
				str += m_processList.GetItemText(j, i) + _T(", ");
			}
			CLogger::GetInstance().AddLog(CLogger::LOG_DISK, str);
			str.Empty();
		}

	}
}

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
