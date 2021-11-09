#pragma once
#include "PerfData.h"

#define PROCESS_ID_INDEX 0
#define PROCESS_NAME_INDEX 1
#define PROCESS_WORKINGSET_INDEX 2
#define PROCESS_USEAGERATE_INDEX 3
#define PROCESS_IOREAD_INDEX 4
#define PROCESS_IOWRITE_INDEX 5

struct PerProcessDataObj : public DataObj
{
	CString name;
	CString workingSet;
	CString usageRate;
	CString ioRead;
	CString ioWrite;
	virtual void Clear()
	{
		//SysFreeString(name);
		//SysFreeString(workingSet);
		//SysFreeString(usageRate);
	}
};

class CPerfDataPerProcess :
	public CPerfData
{
public:
	CPerfDataPerProcess();
	~CPerfDataPerProcess();
	int GetNumberOfCores();
public:

	int nCores;
	vector<ULONGLONG> exitedProcIDs;
	double usingPercent;
	double idlePercent;
	map<ULONGLONG, PerProcessDataObj>	*m_table;

	virtual void Init(const PerfDataInfo& info, CResourceMonitorDoc* doc);
	virtual void GetData();
};

