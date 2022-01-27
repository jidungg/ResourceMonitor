
// ResourceMonitorDoc.h : CResourceMonitorDoc 클래스의 인터페이스
//
#pragma once
#include "./Logger/Logger.h"
#include <vector>


using namespace std;

class CPerfDataManager;
class CResourceMonitorView;


#define IS_WINVERSION_UPPER_8 (CResourceMonitorDoc::WinMajorVersion >= 6 && CResourceMonitorDoc::WinMinorVersion >=2)||CResourceMonitorDoc::WinMajorVersion >= 10

class CResourceMonitorDoc : public CDocument
{
public:
	CResourceMonitorDoc();
	virtual ~CResourceMonitorDoc();
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	BOOL GetOSVersion(DWORD &major, DWORD &minor);

	// Thread 함수
	static UINT UpdateTimer(LPVOID);
	static UINT LogTimer(LPVOID doc);

	// Thread 종료 함수
	void ExitThread(); 

	void AtExitProcess(std::vector<ULONG>* exitedProcIDs);
	void AtNetworkOut(std::vector<ULONG>*);
	void AtDiskOut(std::vector<ULONG>*);


	CPerfDataManager* m_perfDataManager;
	CResourceMonitorView* m_pView1;
	CResourceMonitorView* m_pView2;
	CResourceMonitorView* m_pView3;
	CResourceMonitorView* m_pView4;

	int m_logInterval ;
	float m_cpuThreshold ;
	int m_memThreshold;
	ULONG m_networkThreshold;
	ULONG m_diskThreshold;


	BOOL m_isExit ;
	CWinThread* m_updateTimerThread;
	CWinThread* m_logTimerThread;

	static DWORD WinMajorVersion;
	static DWORD WinMinorVersion;
protected: // serialization에서만 만들어집니다.
	DECLARE_DYNCREATE(CResourceMonitorDoc)
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif


#ifdef SHARED_HANDLERS
	// 검색 처리기에 대한 검색 콘텐츠를 설정하는 도우미 함수
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS

};
