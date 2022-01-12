#pragma once
#include "PerfData.h"
#include "./WMI/PerfDataPerProcess.h"
#include "./WMI/PerfDataOS.h"
#include "./WMI/PerfDataLogicalDisk.h"
#include "./WMI/PerfDataOSProcessor.h"
#include "./Etw/Etw.h"

class CEtw;

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
	CPerfDataOSProcessor* m_win32OSProcessor;
	CEtw* m_etw;
	//void UpdateData();
	//void UpdatePerfProcData(DataObj& data);
	//void UpdateDiskDriveData(DataObj& data);

};

