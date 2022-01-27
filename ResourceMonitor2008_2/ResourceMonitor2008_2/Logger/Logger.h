#pragma once

// CLogger 명령 대상입니다.
#include "Logger_Base.h"
#include <Afxmt.h>
#define LOG_PATH_REG _T("SOFTWARE\\RESMON_LOGPATH")
#define LOG_PATH_REG_VALUENAME  _T( "LOGPATH" )
class CLogger : public CLogger_Base
{
public:

	virtual ~CLogger();
	static CLogger& GetInstance()
	{
		if(_instance == NULL)
		{
			_instance = new CLogger();
		}

		return *_instance;
	}


	enum LogDirectory{
		LOG_CPU = 1,
		LOG_MEMORY,
		LOG_DISK,
		LOG_NETWORK,
		LOG_PROCESS
	};

	BOOL m_bCreateFileFlag[LOG_PROCESS];
	CString m_strLogFileName[LOG_PROCESS];
	CString m_sLogPath;
	HKEY m_hLogPathRegKey;

	//Logger Add
	void AddLog(CLogger::LogDirectory nflag, LPCTSTR lpszFormat, ...);
	void AppendLogData(int nflag, LPCTSTR szFmt, ...);
	//void Create(CString strDir, CString strFileName);  // it's not have Destroy function.
	void Create(int nflag, CString strDir, CString strFileName); 
	BOOL IsCreate(int nflag);
	void SetLogPath(CString path);


protected:
	CLogger();
	static CLogger* _instance;
	virtual CString MakeFullPath(CString strPath, CString strFileName, SYSTEMTIME sysTime, int nflag);
};


