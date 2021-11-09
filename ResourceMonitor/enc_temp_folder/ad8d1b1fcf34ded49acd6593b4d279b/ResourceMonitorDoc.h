
// ResourceMonitorDoc.h : CResourceMonitorDoc 클래스의 인터페이스
//


#pragma once
#include "Logger.h"
#include "Logger_Base.h"
#include <vector>

class CPerfDataManager;
static CLogger m_Logger;
static CLogger_Base m_LoggerBase;
class CResourceMonitorDoc : public CDocument
{
protected: // serialization에서만 만들어집니다.
	CResourceMonitorDoc();
	DECLARE_DYNCREATE(CResourceMonitorDoc)

// 특성입니다.
public:
	CPerfDataManager* m_perfDataManager;

// 작업입니다.
public:
	static void UpdateViews(CResourceMonitorDoc *doc);
	static void AddPeriodicLog(CResourceMonitorDoc *doc);
	void AtExitProcess(std::vector<ULONGLONG>* exitedProcIDs);
// 재정의입니다.
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// 구현입니다.
public:
	virtual ~CResourceMonitorDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// 검색 처리기에 대한 검색 콘텐츠를 설정하는 도우미 함수
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};

void AddLog(CLogger::LogDirectory nflag, LPCTSTR lpszFormat, ...);
