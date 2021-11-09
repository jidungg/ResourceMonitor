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
	CPerfData::Init(info, doc);
}

void CPerfDataOS::GetData()
{
	OSDataObj obj;
	obj.flag = true;
	ULONGLONG ID;
	VARIANT propertyVal;

	CPerfData::Refresh();
	for (int i = 0; i < dwNumReturned; i++)
	{
		for (size_t j = 0; j < m_nProps; ++j)
		{
			if (FAILED(hr = apEnumAccess[i]->Get(propertyNames[j], 0, &propertyVal, 0, 0)))
			{
				Cleanup();
				break;
			}
			switch (j)
			{
			case OS_FREEPHYSICALMEM:
				obj.freePhysicalMemory = propertyVal.bstrVal;
				break;
			case OS_TOTALVISIBLEMEM:
				obj.totalVisibleMemory = propertyVal.bstrVal;
				ID = _wtoi64(propertyVal.bstrVal) ;
				break;

			default:
				break;
			}
			VariantClear(&propertyVal);
		}

		if (m_table->empty())
		{
			m_table->insert({ ID, obj });
		}
		else
		{
			(*m_table)[ID] = obj;
		}

		//clear

		obj.Clear();
		apEnumAccess[i]->Release();
		apEnumAccess[i] = NULL;
	}


	if (NULL != apEnumAccess)
	{
		delete[] apEnumAccess;
		apEnumAccess = NULL;
	}
	// Sleep for a second.


	
}
