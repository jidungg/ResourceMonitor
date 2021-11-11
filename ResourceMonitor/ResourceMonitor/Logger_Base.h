#pragma once

#define DAYS_LOG_KEEPING 365
#include <Afxmt.h>

class CLogger_Base
{
public:
	CLogger_Base(void);
	virtual ~CLogger_Base(void);

	///////example////////////////////////////
	//m_Loger.Create("D:\\HOI-6160\\LOG","PLC_IF");
	BOOL IndexFileLogging();
	BOOL Logging(CString strLog);
	BOOL HexLogging(char* pbuff, int nSize);
	BOOL IsCreate();

	void Create(CString strDir, CString strFileName);  // it's not have Destroy function.
	void Enable(BOOL bLogging);
	void UseDate(BOOL bUse) { m_bDateUse = bUse; }
	void AppendLogData(LPCTSTR szFmt, ...);
	void AppendLogData(CString strData);
protected:
	BOOL IsExistFolder(CString strPath);
	CString m_strLogName;
	CString m_strLogHeader;
	CString m_strLogFolder;

	CCriticalSection m_CS;
	CTime m_PreviousDate;
	BOOL  m_bLogging;
	CFile m_pFP;
	CFileException m_eExcept;
	BOOL m_bDateUse;
	BOOL m_bCreateFlag;

	virtual CString MakeFullPath(CString strPath, CString strFileName, SYSTEMTIME sysTime);
};
