// DlgSetLogThreshold.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ResourceMonitor2008_2.h"
#include "DlgSetLogThreshold.h"


// CDlgSetLogThreshold 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgSetLogThreshold, CDialog)


CDlgSetLogThreshold::CDlgSetLogThreshold(float *cpu, int *mem)
	: CDialog(CDlgSetLogThreshold::IDD, NULL)
{
	cpuThreshold = cpu;
	memThreshold = mem;
}

CDlgSetLogThreshold::CDlgSetLogThreshold(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSetLogThreshold::IDD, pParent)
{

}

CDlgSetLogThreshold::~CDlgSetLogThreshold()
{
}

void CDlgSetLogThreshold::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_CPU, m_editCPUThreshold);
	DDX_Control(pDX, IDC_EDIT_MEM, m_editMemThreshold);
}


BEGIN_MESSAGE_MAP(CDlgSetLogThreshold, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgSetLogThreshold::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgSetLogThreshold 메시지 처리기입니다.

BOOL CDlgSetLogThreshold::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	CString str;
	str.Format(_T("%.2f"), *cpuThreshold);
	m_editCPUThreshold.SetWindowTextW(str);
	str.Format(_T("%d"), (*memThreshold) / 1024);
	m_editMemThreshold.SetWindowTextW(str);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
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
	OnOK();
}
