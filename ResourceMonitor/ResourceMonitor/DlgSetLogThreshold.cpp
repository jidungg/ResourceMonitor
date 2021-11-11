// DlgSetLogThreshold.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ResourceMonitor.h"
#include "DlgSetLogThreshold.h"
#include "afxdialogex.h"


// CDlgSetLogThreshold 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgSetLogThreshold, CDialogEx)

CDlgSetLogThreshold::CDlgSetLogThreshold(float * cpu, int *mem, CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SET_THRESHOLD, pParent)
{
	cpuThreshold = cpu;
	memThreshold = mem;
}

CDlgSetLogThreshold::~CDlgSetLogThreshold()
{
}



void CDlgSetLogThreshold::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_CPU, m_editCPUThreshold);
	DDX_Control(pDX, IDC_EDIT_MEM, m_editMemThreshold);
}


BEGIN_MESSAGE_MAP(CDlgSetLogThreshold, CDialogEx)
	ON_COMMAND(ID_LOG_SETTHRESHOLD, &CDlgSetLogThreshold::OnLogSetThreshold)
	ON_BN_CLICKED(IDOK, &CDlgSetLogThreshold::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgSetLogThreshold 메시지 처리기입니다.


void CDlgSetLogThreshold::OnLogSetThreshold()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}


void CDlgSetLogThreshold::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strCPU;
	CString strMem;

	m_editCPUThreshold.GetWindowTextW(strCPU);
	m_editMemThreshold.GetWindowTextW(strMem);

	*cpuThreshold = _wtof(strCPU);
	*memThreshold = _wtoi(strMem) * 1024;

	CDialogEx::OnOK();
}


BOOL CDlgSetLogThreshold::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	CString str;
	str.Format(_T("%.2f"), *cpuThreshold);
	m_editCPUThreshold.SetWindowTextW(str);
	str.Format(_T("%d"), (*memThreshold) / 1024);
	m_editMemThreshold.SetWindowTextW(str);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
