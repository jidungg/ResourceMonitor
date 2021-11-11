#include "stdafx.h"
#include "PerfDataManager.h"
#include "PerfData.h"
#include "PerfDataPerProcess.h"
#include "PerfDataOS.h"
#include <vector>
#include <thread>
using namespace std;

CPerfDataManager::CPerfDataManager(CResourceMonitorDoc* doc)
{
	PerfDataInfo info;
	m_pDoc = doc;
	//idIndex는 ID속성의 인덱스
	//info.className = L"Win32_DiskDrive";
	//info.propertyNames = vector<CString>{ L"Size"  };
	//info.idIndex = 0;
	//info.isUnique = false;
	//m_win32DiskDrive = new CPerfData();
	//m_win32DiskDrive->Init(info, m_pDoc);


	info.className = L"Win32_PerfFormattedData_PerfProc_Process";
	info.propertyNames = vector<CString>{ L"IDProcess", L"Name" , L"WorkingSet", L"PercentProcessorTime", L"IOReadBytesPersec", L"IOWriteBytesPersec"};
	info.idIndex = 0;
	info.isUnique = false;
	m_win32PerfFormatProc = new CPerfDataPerProcess();
	m_win32PerfFormatProc->Init(info, m_pDoc);

	info.className = L"Win32_OperatingSystem";
	info.propertyNames = vector<CString>{ L"FreePhysicalMemory",L"TotalVisibleMemorySize"};
	info.idIndex = 1;
	info.isUnique = true;
	m_win32OperatingSystem = new CPerfDataOS();
	m_win32OperatingSystem->Init(info, m_pDoc);

}


CPerfDataManager::~CPerfDataManager()
{
	/*delete m_win32DiskDrive;
	m_win32DiskDrive = nullptr;*/

	delete m_win32PerfFormatProc;
	m_win32PerfFormatProc = nullptr;

	delete m_win32OperatingSystem;
	m_win32OperatingSystem = nullptr;
}

void CPerfDataManager::RefreshData()
{
	//thread t2(CPerfData::StartRefreshThread, m_win32DiskDrive);
	//t2.detach();
	m_win32PerfFormatProc->GetData();
	m_win32OperatingSystem->GetData();
}


//void CPerfDataManager::UpdateData()
//{
//	UpdatePerfProcData(m_perfProcData);
//	UpdateDiskDriveData(m_diskDriveData);
//}
//void CPerfDataManager::UpdatePerfProcData(DataObj& data)
//{
//	//m_win32PerfRawProc->SetData(data);
//}
//void CPerfDataManager::UpdateDiskDriveData(DataObj& data)
//{
//	//m_win32DiskDrive->SetData(data);
//}
