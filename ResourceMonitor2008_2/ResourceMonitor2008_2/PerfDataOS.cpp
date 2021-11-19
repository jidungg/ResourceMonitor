#include "stdafx.h"
#include "PerfDataOS.h"


CPerfDataOS::CPerfDataOS()
{
}


CPerfDataOS::~CPerfDataOS()
{
}

void CPerfDataOS::Init(const PerfDataInfo & info, CResourceMonitorDoc * doc)
{
	m_table = new map<ULONGLONG, OSDataObj>;
	dataObj = new OSDataObj;
	CPerfData::Init(info, doc);
}

void CPerfDataOS::SetDataObj(int index)
{
	OSDataObj* osDataObj = (OSDataObj*)dataObj;
	switch (index)
	{
	case OS_FREEPHYSICALMEM:
		osDataObj->freePhysicalMemory = propertyVal.bstrVal;
		break;
	case OS_TOTALVISIBLEMEM:
		osDataObj->totalVisibleMemory = propertyVal.bstrVal;
		ID = _wtoi64(propertyVal.bstrVal);
		break;

	default:
		break;
	}
}

void CPerfDataOS::SetTableInstance()
{
	OSDataObj* osDataObj = (OSDataObj*)dataObj;
	if (m_table->empty())
	{
		m_table->insert(make_pair(ID, *osDataObj));
	}
	else
	{
		(*m_table)[ID] = *osDataObj;
	}
}

void CPerfDataOS::ArrangeTable()
{
}
