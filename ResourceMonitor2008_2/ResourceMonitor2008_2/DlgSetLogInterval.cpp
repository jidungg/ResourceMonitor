// DlgSetLogInterval.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ResourceMonitor2008_2.h"
#include "DlgSetLogInterval.h"


// CDlgSetLogInterval 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgSetLogInterval, CDialog)

CDlgSetLogInterval::CDlgSetLogInterval(int *n)
	: CDialog(CDlgSetLogInterval::IDD, NULL)
{
	interval = n;
}

CDlgSetLogInterval::CDlgSetLogInterval(CWnd* pParent)
	: CDialog(CDlgSetLogInterval::IDD, pParent)
{
}
CDlgSetLogInterval::~CDlgSetLogInterval()
{
}

void CDlgSetLogInterval::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_editInterval);
}


BEGIN_MESSAGE_MAP(CDlgSetLogInterval, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgSetLogInterval::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgSetLogInterval 메시지 처리기입니다.

void CDlgSetLogInterval::OnBnClickedOk()
{
	CString str;
	m_editInterval.GetWindowTextW(str);
	*interval = _wtoi(str);
	OnOK();
}

BOOL CDlgSetLogInterval::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	CString str;
	str.Format(_T("%d"), *interval);
	m_editInterval.SetWindowTextW(str);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

