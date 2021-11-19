// CDlgSetLogInterval.cpp: 구현 파일
//

#include "stdafx.h"
#include "ResourceMonitor.h"
#include "CDlgSetLogInterval.h"
#include "afxdialogex.h"
#include "ResourceMonitorDoc.h"

// CDlgSetLogInterval 대화 상자

IMPLEMENT_DYNAMIC(CDlgSetLogInterval, CDialogEx)

CDlgSetLogInterval::CDlgSetLogInterval(int *n, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SET_LOG_INTERVAL, pParent)
{
	interval = n;
}

CDlgSetLogInterval::~CDlgSetLogInterval()
{
}

void CDlgSetLogInterval::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_editInterval);
}


BEGIN_MESSAGE_MAP(CDlgSetLogInterval, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgSetLogInterval::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgSetLogInterval 메시지 처리기


void CDlgSetLogInterval::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString str;
	m_editInterval.GetWindowTextW(str);
	*interval = _wtoi(str);
	CDialogEx::OnOK();
}


BOOL CDlgSetLogInterval::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	CString str;
	str.Format(_T("%d"), *interval);
	m_editInterval.SetWindowTextW(str);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
