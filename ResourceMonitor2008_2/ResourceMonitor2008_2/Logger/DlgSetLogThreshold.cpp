// DlgSetLogThreshold.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../ResourceMonitor2008_2.h"
#include "DlgSetLogThreshold.h"


// CDlgSetLogThreshold 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgSetLogThreshold, CDialog)


CDlgSetLogThreshold::CDlgSetLogThreshold(float *cpu, int *mem, ULONG *net, ULONG *disk)
	: CDialog(CDlgSetLogThreshold::IDD, NULL)
{
	cpuThreshold = cpu;
	memThreshold = mem;
	netThreshold = net;
	diskThreshold = disk;
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
	DDX_Control(pDX, IDC_EDIT_NET, m_editNetThreshold);
	DDX_Control(pDX, IDC_EDIT_DISK, m_editDiskThreshold);
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
	str.Format(_T("%d"), (*netThreshold) );
	m_editNetThreshold.SetWindowTextW(str);
	str.Format(_T("%d"), (*diskThreshold) );
	m_editDiskThreshold.SetWindowTextW(str);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgSetLogThreshold::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strCPU;
	CString strMem;
	CString strNet;
	CString strDisk;

	m_editCPUThreshold.GetWindowTextW(strCPU);
	m_editMemThreshold.GetWindowTextW(strMem);
	m_editNetThreshold.GetWindowTextW(strNet);
	m_editDiskThreshold.GetWindowTextW(strDisk);

	*cpuThreshold = _wtof(strCPU);
	*memThreshold = _wtoi(strMem) * 1024;
	*netThreshold = _wtoi(strNet);
	*netThreshold = _wtoi(strDisk);

	OnOK();
}
