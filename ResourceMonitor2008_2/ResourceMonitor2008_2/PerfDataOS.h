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
		freePhysicalMemory.Empty();
		totalVisibleMemory.Empty();
	}
};

class CPerfDataOS :public CPerfData
{
public:
	CPerfDataOS();
	~CPerfDataOS();

	OSDataObj dataObjOS;
	map<ULONGLONG, OSDataObj>	*m_table;

	virtual void Init(const PerfDataInfo& info, CResourceMonitorDoc* doc);
	virtual void SetDataObj(int index);
	virtual void SetTableInstance();
	virtual void ArrangeTable();

};

