#pragma once
#include "PerfData.h"
#include "PerfDataPerProcess.h"
#include "PerfDataOS.h"
#include "PerfDataLogicalDisk.h"
#include "NetworkPerformanceScanner.h"

class CPerfDataManager
{
public:
	CPerfDataManager(CResourceMonitorDoc* doc);
	~CPerfDataManager();

	void RefreshData();

	CResourceMonitorDoc* m_pDoc;

	CPerfDataLogicalDisk* m_win32DiskDrive;
	CPerfDataPerProcess* m_win32PerfFormatProc;
	CPerfDataOS* m_win32OperatingSystem;
	NetworkPerformanceScanner* m_netPerfScanner;

	//void UpdateData();
	//void UpdatePerfProcData(DataObj& data);
	//void UpdateDiskDriveData(DataObj& data);

};

