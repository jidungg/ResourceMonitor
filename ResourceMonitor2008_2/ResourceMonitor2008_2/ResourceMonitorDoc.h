
// ResourceMonitorDoc.h : CResourceMonitorDoc 클래스의 인터페이스
//
#pragma once
#include "Logger.h"
#include <vector>


using namespace std;

class CPerfDataManager;
static CLogger sLogger;

class CResourceMonitorDoc : public CDocument
{
public:
	CResourceMonitorDoc();
	virtual ~CResourceMonitorDoc();

	BOOL m_isExit ;
	CWinThread* m_updaterThread;
	CWinThread* m_loggerThread;
	CPerfDataManager* m_perfDataManager;
	int m_logInterval ;
	float m_cpuThreshold ;
	int m_memThreshold;

	// Thread 함수
	static UINT Update(LPVOID);
	static UINT AddPeriodicLog(LPVOID doc);

	// Thread 종료 함수
	void ExitThread(); 
	void AtExitProcess(std::vector<ULONGLONG>* exitedProcIDs);

	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

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
