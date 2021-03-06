// CLogger.cpp : 구현 파일입니다.
//

#include "stdafx.h"
//#include "HOI_5160H_STAGEMASTER.h"
#include "Logger.h"


// CLogger
CLogger* CLogger::_instance = NULL;
CLogger::CLogger()
{
	LSTATUS lstRes = ERROR_SUCCESS;
	DWORD dwDisp = 0;

	lstRes = ::RegCreateKeyEx( HKEY_LOCAL_MACHINE,LOG_PATH_REG,0, NULL
		, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS | KEY_WOW64_64KEY
		,NULL,&m_hLogPathRegKey, &dwDisp );

	if ( ERROR_SUCCESS == lstRes ) 
	{ 
		//AfxMessageBox( _T( "Create" ) ); 
	} 
	else 
	{ 
		//AfxMessageBox( _T( "Create Fail" ) ); 
	}

 
	lstRes = ::RegOpenKeyEx( HKEY_LOCAL_MACHINE, LOG_PATH_REG, 
		0, KEY_ALL_ACCESS | KEY_WOW64_64KEY, &m_hLogPathRegKey ); 
	if ( ERROR_SUCCESS == lstRes ) 
	{ 
		//AfxMessageBox( _T( "open" ) ); 
	} 
	else if(lstRes == ERROR_FILE_NOT_FOUND)
	{ 
		//AfxMessageBox( _T( "open Fail" ) ); 
	}

	CString sValName = LOG_PATH_REG_VALUENAME;
	DWORD dwBufSiz = 0;
	TCHAR atcvalue[MAX_PATH];
	ZeroMemory( atcvalue, sizeof( atcvalue ) );
	lstRes = ::RegQueryValueEx( m_hLogPathRegKey, sValName, NULL, NULL, NULL, &dwBufSiz );
	if ( ERROR_SUCCESS == lstRes ) {
		TCHAR atcvalue[MAX_PATH];
		ZeroMemory( atcvalue, sizeof( atcvalue ) );
		lstRes = ::RegQueryValueEx( m_hLogPathRegKey, sValName, NULL, NULL, (LPBYTE)atcvalue, &dwBufSiz );
		if ( ERROR_SUCCESS == lstRes ) 
		{
			m_sLogPath.Format( _T( "%s" ), atcvalue );
			//AfxMessageBox( m_sLogPath );
		}
		else 
		{ 
			AfxMessageBox( m_sLogPath ); 
		} 
	}else
	{
		SetLogPath(LOG_DIRECTORY);
	}


}

CLogger::~CLogger()
{
	LSTATUS lstRes = ERROR_SUCCESS;
	if ( NULL != m_hLogPathRegKey ) 
	{ 
		lstRes = ::RegCloseKey( m_hLogPathRegKey );
		m_hLogPathRegKey = NULL; 
	}

}

void CLogger::AppendLogData(int nflag, LPCTSTR szFmt, ...)
{
	CSingleLock sLock(&m_CS);
	CString strLogData, strTemp;

	// Format the message text
	va_list argptr;
	va_start(argptr, szFmt);
	strLogData.FormatV(szFmt, argptr);
	va_end(argptr);


	SYSTEMTIME sTime;
	::GetLocalTime( &sTime );								

	if(m_bDateUse == TRUE)
	{
		//strLogData.Format("[%02d:%02d:%02d.%02d]		", sTime.wHour, sTime.wMinute, sTime.wSecond, sTime.wMilliseconds/10);
		strTemp.Format(_T("[%02d:%02d:%02d:%02d] "), sTime.wHour, sTime.wMinute, sTime.wSecond, sTime.wMilliseconds/10);	
		strLogData = strTemp + strLogData;						// 기존 데이터앞에 날짜 데이터를 넣는다. 
	}

	//strLogData += szFmt;						// 기존 데이터앞에 날짜 데이터를 넣는다. 

	sLock.Lock ();								
	// 파일 생성에 성공하면 기록을 남긴다.	
	CString strFilleName;
	strFilleName = MakeFullPath(m_strLogFolder, m_strLogFileName[nflag-1], sTime, nflag);
	if( m_pFP.Open(strFilleName, CFile::modeWrite | CFile::modeCreate | CFile::modeNoTruncate | CFile::shareDenyNone , &m_eExcept ) ) 
	{
		USES_CONVERSION;
		ULONGLONG len = m_pFP.SeekToEnd();
		if(len == 0 && m_strLogHeader.GetLength() != 0) // file. make header.
		{
			m_pFP.Write(W2A(m_strLogHeader), m_strLogHeader.GetLength());
			m_pFP.Write("\r\n", 2);
		}
		int a = strLogData.GetLength();

		//m_pFP.Write(strLogData, strLogData.GetLength());		// 가장 끝에 가서
		m_pFP.Write(W2A(strLogData), strLogData.GetLength());		// 가장 끝에 가서
		//m_pFP.Write("12345678901234567890123456789012345678901234567890", a);
		m_pFP.Write("\r\n", 2);									// 지정한 바이트 수 만큼 저장한다.
		m_pFP.Close();
	}

	sLock.Unlock();
}


CString CLogger::MakeFullPath(CString strPath, CString strFileName, SYSTEMTIME sysTime, int nflag)
{
	CString strTemp; 
	CString strFullPath = strPath;

	CTime oldLogTime, curTime;
	CString strDeleteFolderDir;

	CTimeSpan spanDay(DAYS_LOG_KEEPING, 0, 0, 0 );  
	curTime = CTime::GetCurrentTime();
	oldLogTime = curTime - spanDay;
	strDeleteFolderDir = strFullPath  + oldLogTime.Format ("%Y%m");
	if (IsExistFolder(strDeleteFolderDir) ) ::RemoveDirectory(strDeleteFolderDir);	//기간만료 로그디렉토리 삭제

	if (!IsExistFolder(strPath) ) ::CreateDirectory(strPath,NULL);					//기본로그디렉토리(폴더) 생성

	strTemp.Format(_T("\\%04d%02d%02d"), sysTime.wYear, sysTime.wMonth, sysTime.wDay);			
	strFullPath = strFullPath + strTemp;							
    if (!IsExistFolder(strFullPath) ) ::CreateDirectory(strFullPath, NULL);			//날자폴더생성 	

	switch(nflag)
	{
	case LOG_CPU:
		strFullPath = strFullPath + _T("\\CPU");
		break;
	case LOG_MEMORY:
		strFullPath = strFullPath + _T("\\Memory");
		break;
	case LOG_DISK:
		strFullPath = strFullPath + _T("\\Disk");
		break;
	case LOG_NETWORK:
		strFullPath = strFullPath + _T("\\Network");
		break;
	case LOG_PROCESS :
		strFullPath = strFullPath + _T("\\Process");
		break;
	}
	if (!IsExistFolder(strFullPath) ) ::CreateDirectory(strFullPath, NULL);

	/*strFullPath = strFullPath + "\\" + strFileName;
	if (!IsExistFolder(strFullPath) ) ::CreateDirectory(strFullPath, NULL);			//파일이름폴더생성	*/

	strTemp.Format(_T("%s_%04d%02d%02d%02d.txt"), strFileName, sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour);
	strFullPath = strFullPath + "\\" + strTemp;										//로그파일경로생성
	
	return(strFullPath);
}

void CLogger::AddLog(CLogger::LogDirectory nflag, LPCTSTR lpszFormat, ...)
{

	CString str;
	switch (nflag)
	{
	case LOG_CPU:
		str = "PerfData_CPU";
		break;
	case LOG_MEMORY:
		str = "PerfData_Memory";
		break;
	case LOG_DISK:
		str = "PerfData_Disk";
		break;
	case LOG_NETWORK:
		str = "PerfData_Network";
		break;
	case LOG_PROCESS:
		str = "PerfData_Process";
		break;
	}

	//if (IsCreate(nflag) != TRUE)
	{
		Create(nflag, m_sLogPath, str);
	}

	va_list args;
	va_start(args, lpszFormat);

	int nBuf;
	TCHAR szBuffer[4096];

	nBuf = _vsntprintf_s(szBuffer, _countof(szBuffer), lpszFormat, args);

	va_end(args);

	AppendLogData(nflag, szBuffer);
}

void CLogger::Create(int nflag, CString strDir, CString strFileName)
{
	m_strLogFileName[nflag - 1] = strFileName;
	m_strLogFolder = strDir;
	m_bCreateFileFlag[nflag - 1] = TRUE;
}

BOOL CLogger::IsCreate(int nflag)
{
	return m_bCreateFileFlag[nflag - 1];
	//return _waccess_s();
}

void CLogger::SetLogPath(CString path)
{
	m_sLogPath = path;

	LSTATUS lstRes = ERROR_SUCCESS;
	CString sValName = LOG_PATH_REG_VALUENAME;
	CString sVal = path;
	TCHAR atcvalue[MAX_PATH];
	ZeroMemory( atcvalue, sizeof( atcvalue ) );
	_tcscpy_s( atcvalue, sVal.GetLength() + 1, sVal.GetBuffer() );
	lstRes = ::RegSetValueEx( m_hLogPathRegKey,sValName, 0, REG_SZ, (LPBYTE)atcvalue, 2*(sVal.GetLength()+1) );

}
