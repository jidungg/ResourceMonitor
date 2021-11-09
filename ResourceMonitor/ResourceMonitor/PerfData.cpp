
#include "stdafx.h"
#include "PerfData.h"

#define _WIN32_DCOM

#include <iostream>
#include <thread>
#include <Wbemidl.h>
#include "ResourceMonitorView.h"
#include <vector>
#include <map>

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif

#pragma comment(lib, "wbemuuid.lib")

using namespace std;

CPerfData::CPerfData()
{
}

CPerfData::~CPerfData()
{
}


void CPerfData::Init(const PerfDataInfo& info, CResourceMonitorDoc* doc)
{
	className = info.className;
	propertyNames = info.propertyNames;
	m_nProps = info.propertyNames.size();

	m_idIndex = info.idIndex;
	isUnique = info.isUnique;
	m_pDoc = doc;
	if (FAILED(hr = CoInitializeEx(NULL, COINIT_MULTITHREADED)))
	{
		Cleanup();
		return;
	}

	if (FAILED(hr = CoInitializeSecurity(
		NULL,
		-1,
		NULL,
		NULL,
		RPC_C_AUTHN_LEVEL_NONE,
		RPC_C_IMP_LEVEL_IMPERSONATE,
		NULL, EOAC_NONE, 0)))
	{
		Cleanup();
	}

	if (FAILED(hr = CoCreateInstance(
		CLSID_WbemLocator,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator,
		(void**)&pWbemLocator)))
	{
		Cleanup();
		return;
	}

	// Connect to the desired namespace.
	bstrNameSpace = SysAllocString(L"\\\\.\\root\\cimv2");
	if (NULL == bstrNameSpace)
	{
		hr = E_OUTOFMEMORY;
		Cleanup();
		return;
	}
	if (FAILED(hr = pWbemLocator->ConnectServer(
		bstrNameSpace,
		NULL, // User name
		NULL, // Password
		NULL, // Locale
		0L,   // Security flags
		NULL, // Authority
		NULL, // Wbem context
		&pNameSpace)))
	{
		Cleanup();
		return;
	}
	pWbemLocator->Release();
	pWbemLocator = NULL;
	SysFreeString(bstrNameSpace);
	bstrNameSpace = NULL;

	if (FAILED(hr = CoCreateInstance(
		CLSID_WbemRefresher,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IWbemRefresher,
		(void**)&pRefresher)))
	{
		Cleanup();
		return;
	}

	if (FAILED(hr = pRefresher->QueryInterface(
		IID_IWbemConfigureRefresher,
		(void **)&pConfig)))
	{
		Cleanup();
		return;
	}

	AddEnumerator(className);
}

void CPerfData::AddEnumerator(LPCTSTR class_name)
{
	// Add an enumerator to the refresher.
	if (FAILED(hr = pConfig->AddEnum(
		pNameSpace,
		class_name,
		0,
		NULL,
		&pEnum,
		&lID)))
	{
		Cleanup();
		return;
	}
	pConfig->Release();
	pConfig = NULL;
}



void CPerfData::Refresh()
{
	dwNumReturned = 0;
	dwNumObjects = 0;

	if (FAILED(hr = pRefresher->Refresh(0L)))
	{
		Cleanup();
	}
	

	hr = pEnum->GetObjects(0L,
		dwNumObjects,
		apEnumAccess,
		&dwNumReturned);
	// If the buffer was not big enough,
	// allocate a bigger buffer and retry.
	if (hr == WBEM_E_BUFFER_TOO_SMALL
		&& dwNumReturned > dwNumObjects)
	{
		apEnumAccess = new IWbemObjectAccess*[dwNumReturned];
		if (NULL == apEnumAccess)
		{
			hr = E_OUTOFMEMORY;
			Cleanup();
		}
		SecureZeroMemory(apEnumAccess,
			dwNumReturned * sizeof(IWbemObjectAccess*));
		dwNumObjects = dwNumReturned;

		if (FAILED(hr = pEnum->GetObjects(0L,
			dwNumObjects,
			apEnumAccess,
			&dwNumReturned)))
		{
			Cleanup();
		}
	}
	else
	{
		if (hr == WBEM_S_NO_ERROR)
		{
			hr = WBEM_E_NOT_FOUND;
			Cleanup();
		}
	}
	
}



void CPerfData::GetData()
{
//	DataObj obj;
//	obj.flag = true;
//	while (true)
//	{
//		Refresh();
//		for (unsigned int i = 0; i < dwNumReturned; i++)
//		{
//			for (int j = 0; j < m_nProps; ++j)
//			{
//				if (FAILED(hr = apEnumAccess[i]->Get(propertyNames[j],0,&propertyVal[j],0,0)))
//				{
//					Cleanup();
//					break;
//				}
//				if (j != m_idIndex)
//				{
//					obj.vals.push_back(propertyVal[j]);
//				}
//			}
//			//(*table)[propertyVal[m_idIndex].bstrVal] = obj;
//			//(table->empty()) && 
//			if (table->empty())
//			{
//				ULONGLONG ull = _wtoi64(propertyVal[m_idIndex].bstrVal);
//				table->insert({ull , obj });		
//			}
//			else
//			{
//				if (!isUnique)
//				{
//					(*table)[propertyVal[m_idIndex].ullVal]=obj;
//				}
//				else
//				{
//					table->begin()->second = obj;
//				}
//			}
//			obj.vals.clear();
//			for (auto iter = propertyVal.begin(); iter < propertyVal.end(); iter++)
//			{
//				VariantClear(&(*iter));
//			}
//			//VariantClear(&propertyVal[j]);
//			apEnumAccess[i]->Release();
//			apEnumAccess[i] = NULL;
//			
//		}
//
//		if (NULL != apEnumAccess)
//		{
//			delete[] apEnumAccess;
//			apEnumAccess = NULL;
//		}
//		for (auto iter = obj.vals.begin(); iter < obj.vals.end(); iter++)
//		{
//			VariantClear(&(*iter));
//		}
//		// Sleep for a second.
//		Sleep(UPDATE_INTERVAL);
//	}
//	// Cleanup 과정 추가
//	Cleanup();
}

void CPerfData::Cleanup()
{

	if (NULL != bstrNameSpace)
	{
		SysFreeString(bstrNameSpace);
	}

	if (NULL != apEnumAccess)
	{
		for (unsigned int i = 0; i < dwNumReturned; i++)
		{
			if (apEnumAccess[i] != NULL)
			{
				apEnumAccess[i]->Release();
				apEnumAccess[i] = NULL;
			}
		}
		delete[] apEnumAccess;
	}
	if (NULL != pWbemLocator)
	{
		pWbemLocator->Release();
	}
	if (NULL != pNameSpace)
	{
		pNameSpace->Release();
	}
	if (NULL != pEnum)
	{
		pEnum->Release();
	}
	if (NULL != pConfig)
	{
		pConfig->Release();
	}
	if (NULL != pRefresher)
	{
		pRefresher->Release();
	}

	CoUninitialize();

	if (FAILED(hr))
	{
		wprintf(L"Error status=%08x\n", hr);
	}
}
