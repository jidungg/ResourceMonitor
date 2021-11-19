#pragma once
#include "afxwin.h"


// CDlgSetLogInterval 대화 상자입니다.

class CDlgSetLogInterval : public CDialog
{
	DECLARE_DYNAMIC(CDlgSetLogInterval)

public:
	CDlgSetLogInterval(int *n);
	CDlgSetLogInterval(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgSetLogInterval();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SET_LOG_INTERVAL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	CEdit m_editInterval;
	int *interval;
};
