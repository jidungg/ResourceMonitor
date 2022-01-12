#pragma once
#include "afxwin.h"


// CDlgSetLogThreshold 대화 상자입니다.

class CDlgSetLogThreshold : public CDialog
{
	DECLARE_DYNAMIC(CDlgSetLogThreshold)

public:
	CDlgSetLogThreshold(float *cpu, int *mem, ULONG *net, ULONG *disk);   // 표준 생성자입니다.
	CDlgSetLogThreshold(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgSetLogThreshold();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SET_THRESHOLD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_editCPUThreshold;
	CEdit m_editMemThreshold;
	CEdit m_editNetThreshold;
	CEdit m_editDiskThreshold;

	float *cpuThreshold;
	int *memThreshold;
	ULONG *netThreshold;
	ULONG *diskThreshold;
	
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();

};
