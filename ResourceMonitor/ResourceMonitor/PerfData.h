#pragma once

#include <Wbemidl.h>
#include <vector>
#include <map>
#include <mutex>

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

	virtual void Init(const PerfDataInfo& info, CResourceMonitorDoc* doc);
	void GetData();
	void Cleanup();
	void CleanUpOnce();

	//map<ULONG, DataObj>		*table;
	IWbemObjectAccess       **apEnumAccess = NULL;

	DWORD                   dwNumReturned = 0;
	DataObj					*dataObj;


protected:
	void Refresh();


	virtual void SetDataObj(int index) abstract;
	virtual void SetTableInstance() abstract;
	virtual void ArrangeTable()abstract;

	void AddEnumerator(LPCTSTR class_name);

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

	CResourceMonitorDoc		*m_pDoc;



	CString					className;
	vector<CString>			propertyNames;


	VARIANT propertyVal;
	ULONGLONG ID;

private :




};


