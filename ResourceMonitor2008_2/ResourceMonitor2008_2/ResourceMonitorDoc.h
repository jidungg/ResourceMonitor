
// ResourceMonitorDoc.h : CResourceMonitorDoc Ŭ������ �������̽�
//
#pragma once
#include "./Logger/Logger.h"
#include <vector>


using namespace std;

class CPerfDataManager;
class CResourceMonitorView;
static CLogger sLogger;


class CResourceMonitorDoc : public CDocument
{
public:
	CResourceMonitorDoc();
	virtual ~CResourceMonitorDoc();
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

	// Thread �Լ�
	static UINT UpdateTimer(LPVOID);
	static UINT LogTimer(LPVOID doc);

	// Thread ���� �Լ�
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

protected: // serialization������ ��������ϴ�.
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
	// �˻� ó���⿡ ���� �˻� �������� �����ϴ� ����� �Լ�
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS

};
