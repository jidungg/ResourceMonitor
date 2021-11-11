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
		name.Empty();
		workingSet.Empty();
		usageRate.Empty();
		ioRead.Empty();
		ioWrite.Empty();
	}
};

class CPerfDataPerProcess :public CPerfData
{
public:
	CPerfDataPerProcess();
	~CPerfDataPerProcess();

	int nCores;
	int GetNumberOfCores();
	double idlePercent;
	double usingPercent;
	vector<ULONGLONG> exitedProcIDs;
	map<ULONGLONG, PerProcessDataObj>	*m_table;

	virtual void Init(const PerfDataInfo& info, CResourceMonitorDoc* doc);
	virtual void SetDataObj(int index);
	virtual void SetTableInstance();
	virtual void ArrangeTable();


};

