
// ResourceMonitorView.cpp : CResourceMonitorView 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "ResourceMonitor.h"
#endif
#include "ResourceMonitorDoc.h"
#include "ResourceMonitorView.h"
#include "PerfData.h"
#include <vector>

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CResourceMonitorView

//IMPLEMENT_DYNCREATE(CResourceMonitorView, CScrollView)

BEGIN_MESSAGE_MAP(CResourceMonitorView, CScrollView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CScrollView::OnFilePrintPreview)
	ON_WM_SIZE()
	// 3.
	ON_NOTIFY(HDN_ITEMCLICK, 0, &CResourceMonitorView::OnHdnItemclickList1)
END_MESSAGE_MAP()



void CResourceMonitorView::InitTable()
{
	CRect rect;
	GetClientRect(&rect);
	m_processList.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT, CRect(0, FRAME_WIDTH, rect.Width(), rect.Height()), this, 100);
	m_processList.SetColumnWidth(m_tableCaptions.size(), LVSCW_AUTOSIZE_USEHEADER);
	m_processList.SetExtendedStyle(LVS_EX_DOUBLEBUFFER);

	for (auto iter = m_tableCaptions.begin() ; iter != m_tableCaptions.end(); iter++)
	{
		m_processList.InsertColumn(0, *iter, LVCFMT_LEFT, 150, 0);
	}

}
void CResourceMonitorView::InitFrame()
{
	CRect rect;
	GetClientRect(&rect);
	m_farmeList.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT, CRect(0, 0, rect.Width(), FRAME_WIDTH), this, 100);

	m_farmeList.SetExtendedStyle(LVS_EX_DOUBLEBUFFER | LVSICF_NOSCROLL);
	m_farmeList.ShowScrollBar(SB_BOTH, FALSE);

	m_farmeList.InsertColumn(0, m_title, LVCFMT_LEFT, 200, 0);
	int i = 1;
	for (auto iter = m_frameCaptions.begin(); iter != m_frameCaptions.end(); i++, iter++)
	{
		m_farmeList.InsertColumn(i, *iter, LVCFMT_LEFT, 200, 0);
		//m_farmeList.SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);
	}
	m_bInit = true;
}

void CResourceMonitorView::RemoveProcessFromList(vector<ULONGLONG>* exitedProcIDs)
{
	for (auto iter = exitedProcIDs->begin(); iter != exitedProcIDs->end(); iter++)
	{
		LVFINDINFO info;
		int nIndex;
		info.flags = LVFI_STRING;
		CString str;
		str.Format(_T("%llu"), *iter);
		info.psz = str;

		if ((nIndex = m_processList.FindItem(&info)) != -1)
		{
			m_processList.DeleteItem(nIndex);
		}
		str.Empty();
	}


}

// 리스트컨트롤 컬럼 클릭시 데이터 정렬
void CResourceMonitorView::OnHdnItemclickList1(NMHDR * pNMHDR, LRESULT * pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	// 클릭한 컬럼의 인덱스
	int nColumn = pNMLV->iItem;

	// 현 리스트 컨트롤에 있는 데이터 총 자료 개수만큼 저장
	for (int i = 0; i < (m_processList.GetItemCount()); i++)
	{
		m_processList.SetItemData(i, i);
	}

	// 정렬 방식 저장
	m_bAscending = !m_bAscending;

	// 정렬 관련 구조체 변수 생성 및 데이터 초기화
	SORTPARAM sortparams;
	sortparams.pList = &m_processList;
	sortparams.iSortColumn = nColumn;
	sortparams.bSortDirect = m_bAscending;
	
	// | 0	|  1   |     2		 |
	// | PID| NAME | USAGE(rate) |

	// PID 정렬
	if (nColumn == 0)
		sortparams.flag = 0;

	// NAME은 알파벳 정렬
	if (nColumn == 1)
		sortparams.flag = 1;

	// Usage(rate) 정렬
	if (nColumn == 2 || nColumn == 3)
		sortparams.flag = 2;

	// 정렬 함수 호출
	m_processList.SortItems(&CompareItem, (LPARAM)&sortparams);
	*pResult = 0;
}

int CResourceMonitorView::CompareItem(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CListCtrl *pList = ((SORTPARAM*)lParamSort)->pList;
	int iSortColumn = ((SORTPARAM*)lParamSort)->iSortColumn;
	bool bSortDirect = ((SORTPARAM*)lParamSort)->bSortDirect;
	int flag = ((SORTPARAM*)lParamSort)->flag;

	LVFINDINFO info1, info2;
	info1.flags = LVFI_PARAM;
	info1.lParam = lParam1;

	info2.flags = LVFI_PARAM;
	info2.lParam = lParam2;

	int irow1 = pList->FindItem(&info1, -1);
	int irow2 = pList->FindItem(&info2, -1);

	CString strItem1 = pList->GetItemText(irow1, iSortColumn);
	CString strItem2 = pList->GetItemText(irow2, iSortColumn);

	// PID 정렬
	if (flag == 0)
	{
		int iItem1 = _tstoi(strItem1);
		int iItem2 = _tstoi(strItem2);

		if (bSortDirect)
		{
			return iItem1 == iItem2 ? 0 : iItem1 > iItem2;
		}
		else
		{
			return iItem1 == iItem2 ? 0 : iItem1 < iItem2;
		}
	}
	// NAME 정렬
	else if (flag == 1)
	{
		return	bSortDirect ? strcmp(LPSTR(LPCTSTR(strItem1)), LPSTR(LPCTSTR(strItem2))) : -strcmp(LPSTR(LPCTSTR(strItem1)), LPSTR(LPCTSTR(strItem2)));
	}
	// RATE 정렬
	else
	{
		double dItem1 = _wtof(strItem1);
		double dItem2 = _wtof(strItem2);

		if (bSortDirect)
		{
			return dItem1 == dItem2 ? 0 : dItem1 > dItem2;
		}
		else
		{
			return dItem1 == dItem2 ? 0 : dItem1 < dItem2;
		}
	}

	
}




CResourceMonitorView::CResourceMonitorView()
{
	// TODO: 여기에 생성 코드를 추가합니다.
}

CResourceMonitorView::~CResourceMonitorView()
{
}

BOOL CResourceMonitorView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CScrollView::PreCreateWindow(cs);
}



// CResourceMonitorView 그리기


void CResourceMonitorView::OnDraw(CDC* pDC)
{
	CResourceMonitorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.

}

void CResourceMonitorView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO: 이 뷰의 전체 크기를 계산합니다.
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);



}


// CResourceMonitorView 인쇄

BOOL CResourceMonitorView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CResourceMonitorView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CResourceMonitorView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}


// CResourceMonitorView 진단

#ifdef _DEBUG
void CResourceMonitorView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CResourceMonitorView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#include "CPUMonitorView.h"
CResourceMonitorDoc* CResourceMonitorView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{

	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CResourceMonitorDoc)));
	return (CResourceMonitorDoc*)m_pDocument;
}
#endif //_DEBUG


// CResourceMonitorView 메시지 처리기


void CResourceMonitorView::OnSize(UINT nType, int cx, int cy)
{
	CScrollView::OnSize(nType, cx, cy);
	if (!m_bInit)
		return;
	m_processList.MoveWindow(0, FRAME_WIDTH, cx, cy-FRAME_WIDTH,TRUE);
	m_farmeList.MoveWindow(0, 0, cx, FRAME_WIDTH, TRUE);
	m_farmeList.ShowScrollBar(SB_BOTH, FALSE);
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

}

