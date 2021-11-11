#pragma once

#include <Wbemidl.h>
#include <vector>
#include <map>

using namespace std;

class CResourceMonitorDoc;

struct PerfDataInfo
{
	int idIndex = 0;
	CString	className;
	vector<CString>	propertyNames;
	bool isUnique = false;
};

struct DataObj
{
	bool flag = false;
	virtual void Clear() {}
};

class CPerfData
{
public:
	CPerfData();
	~CPerfData();

	DataObj					*dataObj;
	DWORD                   dwNumReturned = 0;
	IWbemObjectAccess       **apEnumAccess = NULL;
	
	void GetData();
	void Cleanup();
	void CleanUpOnce();
	virtual void Init(const PerfDataInfo& info, CResourceMonitorDoc* doc);
	
protected:

	CResourceMonitorDoc		*m_pDoc;
	size_t					m_nProps;
	int						m_idIndex;
	long                    lID = 0;
	bool					isUnique;
	DWORD                   dwNumObjects = 0;//사실상 쓸모 없음

	HRESULT					hr = S_OK;
	IWbemRefresher			*pRefresher = NULL;
	IWbemConfigureRefresher *pConfig = NULL;
	IWbemHiPerfEnum			*pEnum = NULL;
	IWbemLocator            *pWbemLocator = NULL;
	BSTR                    bstrNameSpace = NULL;
	IWbemServices           *pNameSpace = NULL;


	CString					className;
	vector<CString>			propertyNames;

	VARIANT					propertyVal;
	ULONGLONG				ID;

	virtual void SetDataObj(int index) abstract;
	virtual void SetTableInstance() abstract;
	virtual void ArrangeTable() abstract;
	void Refresh();
	void AddEnumerator(LPCTSTR class_name);

private :




};


