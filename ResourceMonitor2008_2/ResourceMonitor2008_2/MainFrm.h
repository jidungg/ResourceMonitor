
// MainFrm.h : CMainFrame 클래스의 인터페이스
//

#pragma once

class CResourceMonitorDoc;
class Version_Dialog;

class CMainFrame : public CFrameWnd
{
public:
	CMainFrame();
	virtual ~CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnSetLogInterval();

	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	


protected:  // 컨트롤 모음이 포함된 멤버입니다.
	CSplitterWnd	  m_wndSplitter;
	CToolBar          m_wndToolBar;
	CStatusBar        m_wndStatusBar;
	
	Version_Dialog*		m_pVersionDlg;

	CResourceMonitorDoc* m_pDoc;
	// 생성된 메시지 맵 함수	
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()


#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	
public:
	afx_msg void OnLogSetinterval();
	afx_msg void OnLogSetthreshold();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnMenuVersion();
	afx_msg void OnLogSetlogpath();
};


