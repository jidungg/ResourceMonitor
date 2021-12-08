#pragma once
#include "PerfData.h"

#define PROCESS_ID_INDEX 0
#define PROCESS_NAME_INDEX 1
#define PROCESS_WORKINGSET_INDEX 2
#define PROCESS_USEAGERATE_INDEX 3
#define PROCESS_IOREAD_INDEX 4
#define PROCESS_IOWRITE_INDEX 5
#define PROCESS_VIRTUAL_INDEX 6
#define PROCESS_PRIVATE_INDEX 7

struct PerProcessDataObj : public DataObj
{
	CString name;
	CString workingSet;
	CString usageRate;
	CString ioRead;
	CString ioWrite;
	CString virtualBytes;
	CString privateBytes;
	CString meanUsageRate;
	int averageLength;
	double prevAvg;
	PerProcessDataObj()
	{
		averageLength = 1;
		prevAvg = 0;
	}
	virtual void Clear()
	{
		name.Empty();
		workingSet.Empty();
		usageRate.Empty();
		ioRead.Empty();
		ioWrite.Empty();
		virtualBytes.Empty();
		privateBytes.Empty();
		meanUsageRate.Empty();
	}
};

class CPerfDataPerProcess :public CPerfData
{
public:
	CPerfDataPerProcess();
	~CPerfDataPerProcess();

	int nCores;

	int GetNumberOfCores();
	double CumulativeAverage (int &length, double prevAvg, double newNumber);
	double idlePercent;
	double usingPercent;
	vector<ULONGLONG> exitedProcIDs;
	map<ULONGLONG, PerProcessDataObj>	*m_table;

	virtual void Init(const PerfDataInfo& info, CResourceMonitorDoc* doc);
	virtual void SetDataObj(int index);
	virtual void SetTableInstance();
	virtual void ArrangeTable();


};

