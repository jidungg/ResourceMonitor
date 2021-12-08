#pragma once

#include <Wbemidl.h>
#include <vector>
#include <map>

using namespace std;

class CResourceMonitorDoc;

struct PerfDataInfo
{
	int idIndex;
	CString	className;
	vector<CString>	propertyNames;
	bool isUnique ;
};

struct DataObj
{
	bool flag ;
	virtual void Clear() {}
};

class CPerfData
{
public:
	CPerfData();
	~CPerfData();

	DataObj					*dataObj;
	DWORD                   dwNumReturned;
	IWbemObjectAccess       **apEnumAccess;
	
	void GetData();
	void Cleanup();
	void CleanUpOnce();
	virtual void Init(const PerfDataInfo& info, CResourceMonitorDoc* doc);
	
protected:

	CResourceMonitorDoc		*m_pDoc ;
	size_t					m_nProps;
	int						m_idIndex;
	long                    lID;

	DWORD                   dwNumObjects;//사실상 쓸모 없음

	HRESULT					hr;
	IWbemRefresher			*pRefresher;
	IWbemConfigureRefresher *pConfig;
	IWbemHiPerfEnum			*pEnum;
	IWbemLocator            *pWbemLocator;
	BSTR                    bstrNameSpace;
	IWbemServices           *pNameSpace;


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


