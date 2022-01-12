// DlgSetLogThreshold.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "../ResourceMonitor2008_2.h"
#include "DlgSetLogThreshold.h"


// CDlgSetLogThreshold ��ȭ �����Դϴ�.

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


// CDlgSetLogThreshold �޽��� ó�����Դϴ�.

BOOL CDlgSetLogThreshold::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
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
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CDlgSetLogThreshold::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
