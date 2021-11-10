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

//void CPerfDataOS::GetData()
//{
//
//	dataObj.flag = true;
//
//	CPerfData::Refresh();
//	for (unsigned int i = 0; i < dwNumReturned; i++)
//	{
//		for (size_t j = 0; j < m_nProps; ++j)
//		{
//			if (FAILED(hr = apEnumAccess[i]->Get(propertyNames[j], 0, &propertyVal, 0, 0)))
//			{
//				Cleanup();
//				break;
//			}
//
//			VariantClear(&propertyVal);
//		}
//
//
//
//		//clear
//
//		dataObj.Clear();
//		apEnumAccess[i]->Release();
//		apEnumAccess[i] = nullptr;
//	}
//
//
//	if (nullptr != apEnumAccess)
//	{
//		delete[] apEnumAccess;
//		apEnumAccess = nullptr;
//	}
//
//}

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
		m_table->insert({ ID, *osDataObj });
	}
	else
	{
		(*m_table)[ID] = *osDataObj;
	}
}

void CPerfDataOS::ArrangeTable()
{
}
