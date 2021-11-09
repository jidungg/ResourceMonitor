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
	virtual void GetData();

	

	//map<ULONG, DataObj>		*table;
	IWbemObjectAccess       **apEnumAccess = NULL;

	DWORD                   dwNumReturned = 0;


protected:
	void Refresh();
	void Cleanup();

	void AddEnumerator(LPCTSTR class_name);

	int						m_nProps;
	int						m_idIndex;
	long                    lID = 0;
	bool					isUnique;
	DWORD                   dwNumObjects = 0;//사실상 쓸모 없음

	HRESULT					hr = S_OK;
	IWbemRefresher			*pRefresher = NULL;
	IWbemConfigureRefresher *pConfig = NULL;
	IWbemHiPerfEnum			*pEnum = NULL;
	IWbemServices           *pNameSpace = NULL;
	IWbemLocator            *pWbemLocator = NULL;
	CResourceMonitorDoc		*m_pDoc;

	BSTR                    bstrNameSpace = NULL;

	CString					className;
	vector<CString>			propertyNames;

	mutex					m;
private :




};


