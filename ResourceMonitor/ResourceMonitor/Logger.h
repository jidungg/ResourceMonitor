#pragma once

// CLogger 명령 대상입니다.
#include "Logger_Base.h"
#include <Afxmt.h>

class CLogger : public CLogger_Base
{
public:
	CLogger();
	virtual ~CLogger();


	//Logger Add
	void AppendLogData(int nflag, LPCTSTR szFmt, ...);
	//void Create(CString strDir, CString strFileName);  // it's not have Destroy function.
	void Create(int nflag, CString strDir, CString strFileName); 
	BOOL IsCreate(int nflag);

	enum LogDirectory{
		LOG_CPU = 1,
		LOG_MEMORY,
		LOG_DISK,
		LOG_NETWORK,
	};
	BOOL m_bCreateFileFlag[LOG_NETWORK];
	CString m_strLogFileName[LOG_NETWORK];

protected:
	virtual CString MakeFullPath(CString strPath, CString strFileName, SYSTEMTIME sysTime, int nflag);
};


