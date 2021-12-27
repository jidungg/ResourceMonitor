#pragma once
#include "perfdata.h"

#define OSPROC_CPU_TOTAL 0
#define OSPROC_CPU_NAME 1

struct OSProcessorDataObj : public DataObj
{
	CString cpuTotal;
	CString name;
	virtual void Clear()
	{
		cpuTotal.Empty();
		name.Empty();
	}
};

class CPerfDataOSProcessor : public CPerfData
{
public:
	CPerfDataOSProcessor(void);
	~CPerfDataOSProcessor(void);
	
	OSProcessorDataObj dataObjOS;
	map<ULONGLONG, OSProcessorDataObj>	*m_table;

	virtual void Init(const PerfDataInfo& info, CResourceMonitorDoc* doc);
	virtual void SetDataObj(int index);
	virtual void SetTableInstance();
	virtual void ArrangeTable();
};
