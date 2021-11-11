﻿#pragma once


// CDlgSetLogInterval 대화 상자

class CDlgSetLogInterval : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSetLogInterval)

public:
	CDlgSetLogInterval(int *n, CWnd* pParent = nullptr);   // 표준 생성자입니다.

	virtual ~CDlgSetLogInterval();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SET_LOG_INTERVAL };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_btnOK;
	int *interval;
	afx_msg void OnBnClickedOk();
};
