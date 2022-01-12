#include "stdAfx.h"
#include "PerfDataLogicalDisk.h"

CPerfDataLogicalDisk::CPerfDataLogicalDisk(void)
{

}

CPerfDataLogicalDisk::~CPerfDataLogicalDisk(void)
{
}

void CPerfDataLogicalDisk::Init(const PerfDataInfo& info, CResourceMonitorDoc* doc)
{
	m_table = new map<CString, LogicalDiskDataObj>;
	dataObj = new LogicalDiskDataObj;
	CPerfData::Init(info, doc);
}
void CPerfDataLogicalDisk::SetDataObj(int index)
{
	LogicalDiskDataObj* diskDataObj = (LogicalDiskDataObj*)dataObj;
	switch (index)
	{
	case DISK_ID_INDEX:
		//ID = _wtoi64(propertyVal.bstrVal);
		diskDataObj->deviceID = propertyVal.bstrVal;
		break;
	case DISK_FREESPACE_INDEX:
		diskDataObj->freeSpace = propertyVal.bstrVal;
		break;
	case DISK_SIZE_T_INDEX:
		diskDataObj->size = propertyVal.bstrVal;
		break;
	default:
		break;
	}
}
void CPerfDataLogicalDisk::SetTableInstance()
{
	LogicalDiskDataObj* diskDataObj = (LogicalDiskDataObj*)dataObj;

	//update Table
	(*m_table)[diskDataObj->deviceID] = *diskDataObj;
}
void CPerfDataLogicalDisk::ArrangeTable(){}