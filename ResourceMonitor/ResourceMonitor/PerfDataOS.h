#pragma once
#include "PerfData.h"

#define OS_FREEPHYSICALMEM 0
#define OS_TOTALVISIBLEMEM 1

struct OSDataObj : public DataObj
{
	CString freePhysicalMemory;
	CString totalVisibleMemory;
	virtual void Clear()
	{
		//SysFreeString(freePhysicalMemory);
		//SysFreeString(totalVisibleMemory);
	}
};

class CPerfDataOS :
	public CPerfData
{
public:
	CPerfDataOS();
	~CPerfDataOS();

	map<ULONGLONG, OSDataObj>	*m_table;

	virtual void Init(const PerfDataInfo& info, CResourceMonitorDoc* doc);
	virtual void GetData();
};

