#pragma once
#include "Perfdata.h"


#define DISK_ID_INDEX 0
#define DISK_FREESPACE_INDEX 1
#define DISK_SIZE_T_INDEX 2

struct LogicalDiskDataObj : public DataObj
{
	CString deviceID;
	CString freeSpace;
	CString size;

	virtual void Clear()
	{
		deviceID.Empty();
	}
};
class CPerfDataLogicalDisk :
	public CPerfData
{
public:
	CPerfDataLogicalDisk(void);
	~CPerfDataLogicalDisk(void);

	virtual void Init(const PerfDataInfo& info, CResourceMonitorDoc* doc);
	virtual void SetDataObj(int index);
	virtual void SetTableInstance();
	virtual void ArrangeTable();

	map<CString, LogicalDiskDataObj>	*m_table;
};
