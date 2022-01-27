
// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "ResourceMonitor2008_2.h"
#include "./View/ResourceMonitorView.h"
#include "./View/CPUMonitorView.h"
#include "./View/MemMonitorView.h"
#include "./View/DiskMonitorView.h"
#include "./View/NetMonitorView.h"
#include "MainFrm.h"
#include "ResourceMonitorDoc.h"
#include "./PerfData/PerfDataManager.h"
#include "./Logger/DlgSetLogInterval.h"
#include "./Logger/DlgSetLogThreshold.h"
#include "Version_Dialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SYSCOMMAND()

	ON_COMMAND(ID_LOG_SETINTERVAL, &CMainFrame::OnLogSetinterval)
	ON_COMMAND(ID_LOG_SETTHRESHOLD, &CMainFrame::OnLogSetthreshold)
	ON_WM_TIMER()
	ON_COMMAND(ID_Menu, &CMainFrame::OnMenuVersion)
	ON_COMMAND(ID_LOG_SETLOGPATH, &CMainFrame::OnLogSetlogpath)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 상태 줄 표시기
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};



// CMainFrame 생성/소멸

CMainFrame::CMainFrame()
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.

}

CMainFrame::~CMainFrame()
{

}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;


	m_pVersionDlg = new Version_Dialog("",600,400,0,0);
	m_pVersionDlg->Create(this,false);
	m_pVersionDlg->ViewVersion(this);
	m_pVersionDlg->ShowWindow(SW_HIDE);
	return 0;
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/,
	CCreateContext* pContext)
{
	CRect rect;
	GetClientRect(&rect);

	m_wndSplitter.CreateStatic(this, 2, 2);
	m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CCPUMonitorView), CSize(rect.Width()/2, rect.Height()/2), pContext);
	m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CMemMonitorView), CSize(rect.Width()/2, rect.Height()/2), pContext);
	m_wndSplitter.CreateView(1, 0, RUNTIME_CLASS(CDiskMonitorView), CSize(rect.Width()/2, rect.Height()/2), pContext);
	m_wndSplitter.CreateView(1, 1, RUNTIME_CLASS(CNetMonitorView), CSize(rect.Width()/2, rect.Height()/2), pContext);


	return TRUE;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
    cs.style &= ~(LONG) FWS_ADDTOTITLE;


	return TRUE;
}

// CMainFrame 진단

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
		
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 메시지 처리기



void CMainFrame::OnSysCommand(UINT nID, LPARAM lParam)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (nID == SC_CLOSE)
	{
		if (MessageBox(L"Program Exit?", L"Resource Monitor EXIT", MB_YESNO) == IDYES)
		{
			// exit evnet
			CResourceMonitorDoc* d = (CResourceMonitorDoc*)GetActiveDocument();
			// thread loop 탈출 조건 설정
			d->m_isExit = TRUE; 
			CPerfDataManager* dm = d->m_perfDataManager;

			// m_table claer 후 erase
			dm->m_win32PerfFormatProc->CleanUpOnce();
			dm->m_win32PerfFormatProc->Cleanup();
			dm->m_win32PerfFormatProc->m_table->clear();
			dm->m_win32PerfFormatProc->m_table->erase(dm->m_win32PerfFormatProc->m_table->begin(), dm->m_win32PerfFormatProc->m_table->end());
			delete dm->m_win32PerfFormatProc->m_table;
			delete dm->m_win32PerfFormatProc->dataObj;

			dm->m_win32OperatingSystem->Cleanup();
			dm->m_win32OperatingSystem->m_table->clear();
			dm->m_win32OperatingSystem->m_table->erase(dm->m_win32OperatingSystem->m_table->begin(), dm->m_win32OperatingSystem->m_table->end());
			delete dm->m_win32OperatingSystem->m_table;
			delete dm->m_win32OperatingSystem->dataObj;
	
			dm->m_etw->CleanUp();

			// thread 종료
			d->ExitThread();


			// PerfDataManager 소멸자 호출
			d->m_perfDataManager->~CPerfDataManager();

			// CResourceMonitorDoc 소멸자 호출
			d->~CResourceMonitorDoc();
			CFrameWnd::OnSysCommand(nID, lParam);
		}
	}
	else {
		CFrameWnd::OnSysCommand(nID, lParam);
	}


}


void CMainFrame::OnLogSetinterval()
{
	CResourceMonitorDoc* doc = (CResourceMonitorDoc*)GetActiveDocument();
	CDlgSetLogInterval dlg(&(doc->m_logInterval));
	dlg.DoModal();
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}

void CMainFrame::OnLogSetthreshold()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CResourceMonitorDoc* doc = (CResourceMonitorDoc*)GetActiveDocument();
	CDlgSetLogThreshold dlg(&(doc->m_cpuThreshold), &(doc->m_memThreshold), &(doc->m_networkThreshold), &(doc->m_diskThreshold));
	dlg.DoModal();
}

void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	switch(nIDEvent)
	{

	}
	CFrameWnd::OnTimer(nIDEvent);
}



void CMainFrame::OnMenuVersion()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.


	m_pVersionDlg->ShowWindow(SW_SHOW);
}

bool Select_directory(CWnd* window,
                                   const char* dialog_title,
                                   WCHAR* directory_name)
{
   bool okay = false;

   LPMALLOC pMalloc;

   if (::SHGetMalloc(&pMalloc) == NOERROR)  // get shell's default
      {                                     // allocator.
      BROWSEINFO bi;
      bi.pidlRoot = NULL;
      bi.lpfn = NULL;
      bi.lParam = 0;
      bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_RETURNFSANCESTORS;
      bi.hwndOwner = window->GetSafeHwnd();
      bi.pszDisplayName = (LPWSTR)(LPCSTR)directory_name;
      bi.lpszTitle = (LPCWSTR)(LPCSTR)dialog_title;

      LPITEMIDLIST pidl = ::SHBrowseForFolder(&bi);
      if (pidl != NULL)
         {
         if (::SHGetPathFromIDList(pidl, (LPWSTR)(LPCSTR)directory_name))
            {
            okay = true;
            }
         pMalloc->Free(pidl);
         }
      pMalloc->Release();  // release the shell's allocator
      }

   return okay;
}

bool Find_fullpathname(const char* relative_path,
                                    char* full_path)
{
   const bool okay = (::_fullpath(full_path, relative_path,
                                  _MAX_PATH) != NULL) ? true : false;
   if (!okay)
      {
      ::MessageBox(NULL, (LPCWSTR)(LPCSTR)relative_path,(LPCWSTR)(LPCSTR) "Not a valid relative path",
                   MB_OK | MB_ICONINFORMATION);
      }
   else
      {
      // For consistency, always ensure that the directory has a
      // trailing '\'.

      const char backslash = '\\';

      char* append_here =
                   full_path + static_cast<int>(::strlen(full_path));
      const char last_char = *(append_here - 1);

      if (last_char != backslash)
         {
         if (last_char == '/')
            {
            append_here--;         // will replace '/' with backslash
            }
         *append_here++ = backslash;
         *append_here = '\0';
         }
      }

   return okay;
}
void CMainFrame::OnLogSetlogpath()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	bool okay = false;
	void* d_value = 0;
	CString p_str;

	WCHAR directory_name[_MAX_PATH + 4];

	okay = Select_directory(this, (LPSTR)(LPCSTR)_T("Select directory"), directory_name);

	if (okay)
	{
		CLogger::GetInstance().SetLogPath(directory_name);

		//okay = Find_fullpathname((LPCSTR)(LPCTSTR)p_str, (LPSTR)(LPCSTR)directory_name);

		//if (okay)
		//{
		//	// Check the directory_name really is a directory.

		//	const DWORD attrib = ::GetFileAttributes((LPCWSTR)(LPCSTR)directory_name);
		//	if ((attrib != 0xFFFFFFFF) &&
		//		(attrib & FILE_ATTRIBUTE_DIRECTORY))
		//	{
		//		p_str = directory_name;  // Save in member variable.
		//	}
		//	else
		//	{
		//		okay = false;
		//		::MessageBox(NULL, (LPCWSTR)(LPCSTR)directory_name,
		//			(LPCWSTR)(LPCSTR)"Not a valid directory!",
		//			MB_OK | MB_ICONINFORMATION);
		//	}
		//}
	}
}
