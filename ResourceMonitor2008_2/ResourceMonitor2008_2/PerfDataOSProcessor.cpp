#include "stdAfx.h"
#include "PerfDataOSProcessor.h"

CPerfDataOSProcessor::CPerfDataOSProcessor(void)
{
}

CPerfDataOSProcessor::~CPerfDataOSProcessor(void)
{
}

void CPerfDataOSProcessor::Init(const PerfDataInfo & info, CResourceMonitorDoc * doc)
{
	m_table = new map<ULONGLONG, OSProcessorDataObj>;
	dataObj = new OSProcessorDataObj;
	CPerfData::Init(info,doc);
}

void CPerfDataOSProcessor::SetDataObj(int index)
{
	OSProcessorDataObj* osProcDataObj = (OSProcessorDataObj*)dataObj;
	switch (index)
	{
	case OSPROC_CPU_TOTAL:
		osProcDataObj->cpuTotal = propertyVal.bstrVal;
		break;
	case OSPROC_CPU_NAME:
		osProcDataObj->name = propertyVal.bstrVal;
		ID = _wtoi64(propertyVal.bstrVal);
		break;

	default:
		break;
	}
}

void CPerfDataOSProcessor::SetTableInstance()
{
	OSProcessorDataObj* osProcDataObj = (OSProcessorDataObj*)dataObj;
	if (m_table->empty())
	{
		m_table->insert(make_pair(ID, *osProcDataObj));
	}
	else
	{
		(*m_table)[ID] = *osProcDataObj;
	}
}

void CPerfDataOSProcessor::ArrangeTable()
{
}

