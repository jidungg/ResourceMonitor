#pragma once
#include "afxwin.h"


// CDlgSetLogThreshold 대화 상자입니다.

class CDlgSetLogThreshold : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSetLogThreshold)

public:
	CDlgSetLogThreshold(float * cpu, int *mem, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgSetLogThreshold();

	float *cpuThreshold;
	int *memThreshold;
	CEdit m_editCPUThreshold;
	CEdit m_editMemThreshold;

	afx_msg void OnLogSetThreshold();
	afx_msg void OnBnClickedOk();

	virtual BOOL OnInitDialog();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SET_THRESHOLD };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	
};
