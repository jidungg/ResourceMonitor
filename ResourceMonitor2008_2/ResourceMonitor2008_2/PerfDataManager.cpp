#include "stdafx.h"
#include "PerfDataManager.h"
#include "PerfData.h"
#include <vector>
using namespace std;

CPerfDataManager::CPerfDataManager(CResourceMonitorDoc* doc)
{
	PerfDataInfo info;
	m_pDoc = doc;
	//idIndex는 ID속성의 인덱스
	info.className = L"Win32_LogicalDIsk";
	info.propertyNames.push_back(L"DeviceID");
	info.propertyNames.push_back(L"FreeSpace");
	info.propertyNames.push_back(L"Size");
	info.idIndex = 0;
	info.isUnique = false;
	m_win32DiskDrive = new CPerfDataLogicalDisk();
	m_win32DiskDrive->Init(info, m_pDoc);

	info.propertyNames.clear();

	info.className = L"Win32_PerfFormattedData_PerfProc_Process";
	info.propertyNames.push_back( L"IDProcess" );
	info.propertyNames.push_back( L"Name" );
	info.propertyNames.push_back(L"WorkingSet");
	info.propertyNames.push_back(L"PercentProcessorTime");
	info.propertyNames.push_back(L"IOReadBytesPersec");
	info.propertyNames.push_back(L"IOWriteBytesPersec");
	info.propertyNames.push_back(L"PrivateBytes");
	info.propertyNames.push_back(L"WorkingSetPrivate");

	info.idIndex = 0;
	info.isUnique = false;
	m_win32PerfFormatProc = new CPerfDataPerProcess();
	m_win32PerfFormatProc->Init(info, m_pDoc);

	info.propertyNames.clear();

	info.className = L"Win32_OperatingSystem";
	info.propertyNames.push_back(L"FreePhysicalMemory");
	info.propertyNames.push_back(L"TotalVisibleMemorySize");
	info.idIndex = 1;
	info.isUnique = true;
	m_win32OperatingSystem = new CPerfDataOS();
	m_win32OperatingSystem->Init(info, m_pDoc);

	info.propertyNames.clear();

	info.className = L"Win32_PerfFormattedData_PerfOS_Processor";
	info.propertyNames;
	info.propertyNames.push_back(L"PercentProcessorTime");
	info.propertyNames.push_back(L"Name");
	info.idIndex = 0;
	info.isUnique = false;
	m_win32OSProcessor = new CPerfDataOSProcessor();
	m_win32OSProcessor->Init(info, m_pDoc);

}


CPerfDataManager::~CPerfDataManager()
{
	/*delete m_win32DiskDrive;
	m_win32DiskDrive = nullptr;*/

	delete m_win32PerfFormatProc;
	m_win32PerfFormatProc = NULL;

	delete m_win32OperatingSystem;
	m_win32OperatingSystem = NULL;

	delete m_win32OSProcessor;
	m_win32OSProcessor = NULL;
}

void CPerfDataManager::RefreshData()
{

	m_win32PerfFormatProc->GetData();
	m_win32OperatingSystem->GetData();
	m_win32DiskDrive->GetData();
	m_win32OSProcessor->GetData(); //CPerfDataOSProcessor클래스 사용 안할 시 주석
}


