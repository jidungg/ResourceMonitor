
// MainFrm.h : CMainFrame 클래스의 인터페이스
//

#pragma once


class CMainFrame : public CFrameWnd
{
public:
	CMainFrame();
	virtual ~CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnSetLogInterval();
	afx_msg void OnLogSetthreshold();

	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

protected:  // 컨트롤 모음이 포함된 멤버입니다.
	CSplitterWnd	  m_wndSplitter;
	CToolBar          m_wndToolBar;
	CStatusBar        m_wndStatusBar;

	// 생성된 메시지 맵 함수	
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()


#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	
};


