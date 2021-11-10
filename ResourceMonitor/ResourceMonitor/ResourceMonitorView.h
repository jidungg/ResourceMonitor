
// ResourceMonitorView.h : CResourceMonitorView 클래스의 인터페이스
//

#pragma once
#include <vector>
#include <map>
#include <mutex>

class CLogger;
class CPerfData;
class CResourceMonitorDoc;
class CPerfDataManager;
class CResourceMonitorView :public CScrollView
{
	//DECLARE_DYNCREATE(CResourceMonitorView)
protected:

	// serialization에서만 만들어집니다.
	CResourceMonitorView();
	virtual ~CResourceMonitorView();

	// 특성입니다.
public:
	bool m_bInit = false;

	CString m_title;
	std::vector<CString> m_frameProps; //frame에 사용할 WMI의 속성 이름
	std::vector<CString> m_tableProps; //table에 사용할 WMI의 속성 이름
	std::vector<CString> m_frameCaptions;  //화면에 표시될 frame 속성 이름
	std::vector<CString> m_tableCaptions;  //화면에 표시될 table 속성 이름
	int colCount;
	CListCtrl m_processList;
	CListCtrl m_farmeList;
	std::mutex m;

	// 1.
	BOOL m_bAscending;
	struct SORTPARAM
	{
		int iSortColumn;
		bool bSortDirect;
		CListCtrl *pList;
		int flag = -1;
	};

// 작업입니다.
public:
	CResourceMonitorDoc* GetDocument() const;

	virtual void UpdateView(CPerfDataManager* dataManager) abstract;
	virtual void AddPeriodicLog()abstract;
	void InitTable();
	void InitFrame();
	void RemoveProcessFromList(std::vector<ULONGLONG>* exitedProcIDs);
	
	// 2.
	afx_msg void OnHdnItemclickList1(NMHDR *pNMHDR, LRESULT *pResult);
	static int CALLBACK CompareItem(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);


// 재정의입니다.
public:

	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // 생성 후 처음 호출되었습니다.
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	

// 구현입니다.
public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

#ifndef _DEBUG  // ResourceMonitorView.cpp의 디버그 버전
inline CResourceMonitorDoc* CResourceMonitorView::GetDocument() const
   { return reinterpret_cast<CResourceMonitorDoc*>(m_pDocument); }
#endif

