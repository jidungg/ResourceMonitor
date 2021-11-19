
// ResourceMonitorView.h : CResourceMonitorView 클래스의 인터페이스
//

#pragma once
#include <vector>
#include <map>

using namespace std;

class CLogger;
class CPerfData;
class CResourceMonitorDoc;
class CPerfDataManager;


class CResourceMonitorView :public CScrollView
{
public:
	CResourceMonitorView();
	virtual ~CResourceMonitorView();

	struct SORTPARAM
	{
		int iSortColumn;
		bool bSortDirect;
		CListCtrl *pList;
		int flag = -1;
	};

	BOOL m_bInit = false;
	BOOL m_bAscending; // 오름, 내림차순 정렬 flag

	CString m_title;
	CListCtrl m_processList;
	CListCtrl m_farmeList;
	vector<CString> m_frameProps;		//frame에 사용할 WMI의 속성 이름
	vector<CString> m_tableProps;		//table에 사용할 WMI의 속성 이름
	vector<CString> m_frameCaptions;    //화면에 표시될 frame 속성 이름
	vector<CString> m_tableCaptions;    //화면에 표시될 table 속성 이름


	CResourceMonitorDoc* GetDocument() const;

	void InitTable();
	void InitFrame();
	void RemoveProcessFromList(std::vector<ULONGLONG>* exitedProcIDs);
	
	virtual void UpdateView(CPerfDataManager* dataManager) abstract;
	virtual void AddPeriodicLog() abstract;
	virtual void OnDraw(CDC* pDC);  
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHdnItemclickList1(NMHDR *pNMHDR, LRESULT *pResult);
	static int CALLBACK CompareItem(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort); // 정렬 관련 함수


#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void OnInitialUpdate(); 
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // ResourceMonitorView.cpp의 디버그 버전
inline CResourceMonitorDoc* CResourceMonitorView::GetDocument() const
   { return reinterpret_cast<CResourceMonitorDoc*>(m_pDocument); }
#endif

