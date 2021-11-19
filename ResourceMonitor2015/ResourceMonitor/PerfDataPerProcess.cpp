#include "stdafx.h"
#include "PerfDataPerProcess.h"
#include "PerfDataManager.h"
#include "ResourceMonitorDoc.h"

CPerfDataPerProcess::CPerfDataPerProcess()
{
	nCores = GetNumberOfCores();

}


CPerfDataPerProcess::~CPerfDataPerProcess()
{
}

void CPerfDataPerProcess::Init(const PerfDataInfo & info, CResourceMonitorDoc * doc)
{
	m_table = new map<ULONGLONG, PerProcessDataObj>;
	dataObj = new PerProcessDataObj;
	CPerfData::Init(info, doc);
}

void CPerfDataPerProcess::SetDataObj(int index)
{
	PerProcessDataObj* procDataObj = (PerProcessDataObj*)dataObj;
	switch (index)
	{
	case PROCESS_ID_INDEX:
		ID = propertyVal.ullVal;
		break;
	case PROCESS_NAME_INDEX:
		procDataObj->name = propertyVal.bstrVal;
		break;
	case PROCESS_WORKINGSET_INDEX:
		procDataObj->workingSet = propertyVal.bstrVal;
		break;
	case PROCESS_USEAGERATE_INDEX:
		procDataObj->usageRate = propertyVal.bstrVal;
		break;
	case PROCESS_IOREAD_INDEX:
		procDataObj->ioRead = propertyVal.bstrVal;
		break;
	case PROCESS_IOWRITE_INDEX:
		procDataObj->ioWrite = propertyVal.bstrVal;
		break;
	default:
		break;
	}
}
void CPerfDataPerProcess::SetTableInstance()
{
	PerProcessDataObj* procDataObj = (PerProcessDataObj*)dataObj;
	//calc usingPercent
	usingPercent = _wtoi64(procDataObj->usageRate);
	if ((procDataObj->name.Compare(_T("Idle")) == 0))
	{
		idlePercent = _wtoi64(procDataObj->usageRate);
	}
	if (procDataObj->name.Compare(_T("_Total")) == 0)
	{
		usingPercent = abs(_wtoi64(procDataObj->usageRate) - idlePercent);
	}
	usingPercent = (usingPercent / nCores);
	procDataObj->usageRate.Format(_T("%.02f"), usingPercent);

	//update Table
	(*m_table)[ID] = *procDataObj;
}

void CPerfDataPerProcess::ArrangeTable()
{
	//false를 찾고
	for (auto iter = m_table->begin(); iter != m_table->end(); iter++)
	{
		if (iter->second.flag == false)
		{
			exitedProcIDs.push_back(iter->first);
		}
	}
	//false인애 없애주고 
	for (auto iter = exitedProcIDs.begin(); iter != exitedProcIDs.end(); iter++)
	{
		m_table->erase(*iter);
	}
	if (exitedProcIDs.size() > 0)
	{
		m_pDoc->AtExitProcess(&exitedProcIDs);
	}

	//table에 다 false로 바꿔
	for (auto iter = m_table->begin(); iter != m_table->end(); iter++)
	{
		iter->second.flag = false;
	}
	exitedProcIDs.clear();
	vector<ULONGLONG>().swap(exitedProcIDs);

}
int CPerfDataPerProcess::GetNumberOfCores()
{
	PSYSTEM_LOGICAL_PROCESSOR_INFORMATION pProcessorInformations = NULL;
	DWORD length = 0;

	BOOL result = GetLogicalProcessorInformation(pProcessorInformations, &length);
	if (!result)
	{
		if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)

		{
			pProcessorInformations = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)new BYTE[length];
		}
	}
	result = GetLogicalProcessorInformation(pProcessorInformations, &length);
	if (!result)
	{		// er
		return -1;

	}
	int numOfCores = 0;
	for (int i = 0; i < length / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION); i++)

	{
		if (pProcessorInformations[i].Relationship == RelationProcessorCore)

			numOfCores++;
	}
	delete[] pProcessorInformations;
	pProcessorInformations = nullptr;

	return numOfCores;
}

