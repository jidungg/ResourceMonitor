#pragma once


#include "ResourceMonitorView.h"
// CDiskMonitorView 뷰입니다.

class CDiskMonitorView : public CResourceMonitorView
{
	DECLARE_DYNCREATE(CDiskMonitorView)

protected:
	CDiskMonitorView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CDiskMonitorView();

public:
	virtual void UpdateView(class CPerfDataManager* dataManager);
	virtual void AddPeriodicLog();
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	//virtual void OnDraw(CDC* pDC);      // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual void OnInitialUpdate();     // 생성된 후 처음입니다.

	DECLARE_MESSAGE_MAP()
};


