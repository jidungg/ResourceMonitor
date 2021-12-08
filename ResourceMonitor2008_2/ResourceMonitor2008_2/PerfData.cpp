
#include "stdafx.h"
#include "PerfData.h"

#define _WIN32_DCOM

#include <iostream>
#include <Wbemidl.h>
#include "ResourceMonitorView.h"
#include <vector>
#include <map>
#pragma comment(lib, "wbemuuid.lib")

using namespace std;

CPerfData::CPerfData()
{
	dwNumReturned = 0;
	apEnumAccess = NULL;
	lID = 0;
	dwNumObjects = 0;
	hr = S_OK;
	pRefresher = NULL;
	pConfig = NULL;
	pEnum = NULL;
	pWbemLocator = NULL;
	bstrNameSpace = NULL;
	pNameSpace = NULL;
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
		//Cleanup();
		//return;
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
	dataObj->flag = true;
	CPerfData::Refresh();

	for (unsigned int i = 0; i < dwNumReturned; i++)
	{
		for (size_t j = 0; j < m_nProps; ++j)
		{

			if (FAILED(hr = apEnumAccess[i]->Get(propertyNames[j], 0, &propertyVal, 0, 0)))
			{
				Cleanup();
				break;
			}
			SetDataObj(j);
			VariantClear(&propertyVal);

		}
		SetTableInstance();
		//clear obj & apEnumAccess
		dataObj->Clear();
		apEnumAccess[i]->Release();
		apEnumAccess[i] = NULL;
	}
	ArrangeTable();

	if (NULL != apEnumAccess)
	{
		delete[] apEnumAccess;
		apEnumAccess = NULL;
	}
}

void CPerfData::Cleanup()
{
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



	if (FAILED(hr))
	{
		wprintf(L"Error status=%08x\n", hr);
	}
}

void CPerfData::CleanUpOnce()
{
	if (NULL != bstrNameSpace)
	{
		SysFreeString(bstrNameSpace);
	}
	if (NULL != pWbemLocator)
	{
		pWbemLocator->Release();
	}
	if (NULL != pNameSpace)
	{
		pNameSpace->Release();
	}
	CoUninitialize();
}