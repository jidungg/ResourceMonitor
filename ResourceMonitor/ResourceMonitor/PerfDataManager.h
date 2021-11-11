#pragma once
#include "PerfData.h"
#include "PerfDataPerProcess.h"
#include "PerfDataOS.h"

class CPerfDataManager
{
public:
	CPerfDataManager(CResourceMonitorDoc* doc);
	~CPerfDataManager();
	void RefreshData();
public :
	CPerfDataPerProcess* m_win32PerfFormatProc;
	CPerfData* m_win32DiskDrive;
	CPerfDataOS* m_win32OperatingSystem;

	CResourceMonitorDoc* m_pDoc;

private:


};

