#include "stdafx.h"
#include "Logger_Base.h"

CLogger_Base::CLogger_Base(void)
{
	m_bLogging =TRUE;
	m_bDateUse = TRUE;
	m_strLogName = _T("");
	m_strLogFolder = _T("");
	m_bCreateFlag = FALSE;
}

CLogger_Base::~CLogger_Base(void)
{
}


BOOL CLogger_Base::Logging(CString strLog)
{
    if(m_bLogging == TRUE)
	{
		AppendLogData(strLog);
	}
	return TRUE;
}

BOOL CLogger_Base::IndexFileLogging()
{
	CSingleLock sLock(&m_CS);
	SYSTEMTIME sTime;
	::GetLocalTime( &sTime );								
	
	sLock.Lock ();	
	CString strFilleName;
	strFilleName = MakeFullPath(m_strLogFolder, m_strLogName, sTime);
	if( m_pFP.Open(strFilleName, CFile::modeWrite | CFile::modeCreate | CFile::modeNoTruncate | CFile::shareDenyNone , &m_eExcept ) ) 
	{
		m_pFP.Close();
	}

	sLock.Unlock();

	return TRUE;
}

void CLogger_Base::Enable(BOOL bLogging)
{
	m_bLogging = bLogging;
}

char N_T[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
BOOL CLogger_Base::HexLogging(char* pbuff, int nSize)
{
	char H;
	char L;
	CString strLog;

	for( int i=0; i<nSize; i++)
	{
		H = *(pbuff+i);
		H = (0x0F & H>>4);
		L = *(pbuff+i);
		L = (0x0F & L);

		H = N_T[H];
		L = N_T[L];

		strLog += H;
		strLog += L;
	}
	
	if(m_bLogging == TRUE)
	{
		AppendLogData(strLog);
	}

	return TRUE;
}

void CLogger_Base::AppendLogData(CString strData)
{
	CSingleLock sLock(&m_CS);
	CString strLogData;

	SYSTEMTIME sTime;
	::GetLocalTime( &sTime );		
	if(m_bDateUse == TRUE)
	{
		strLogData.Format(_T("[%02d:%02d:%02d.%02d] "), sTime.wHour, sTime.wMinute, sTime.wSecond, sTime.wMilliseconds/10);
	}
	
	strLogData += strData;						// 기존 데이터앞에 날짜 데이터를 넣는다. 
	
  	sLock.Lock ();								
	// 파일 생성에 성공하면 기록을 남긴다.	
	CString strFilleName;
	strFilleName = MakeFullPath(m_strLogFolder, m_strLogName, sTime);
	if( m_pFP.Open(strFilleName, CFile::modeWrite | CFile::modeCreate | CFile::modeNoTruncate | CFile::shareDenyNone , &m_eExcept ) ) 
	{
		ULONGLONG len = m_pFP.SeekToEnd();
		if(len == 0 && m_strLogHeader.GetLength() != 0) //file. make header.
		{
			m_pFP.Write(m_strLogHeader, m_strLogHeader.GetLength());
			m_pFP.Write("\r\n", 2);
		}
		m_pFP.Write(strLogData, strLogData.GetLength());		// 가장 끝에 가서
		m_pFP.Write("\r\n", 2);									// 지정한 바이트 수 만큼 저장한다.
		m_pFP.Close();
	}

	sLock.Unlock();
	///////////////////////////////////////////////
	
}

void CLogger_Base::AppendLogData(LPCTSTR szFmt, ...)
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
	strFilleName = MakeFullPath(m_strLogFolder, m_strLogName, sTime);
	if( m_pFP.Open(strFilleName, CFile::modeWrite | CFile::modeCreate | CFile::modeNoTruncate | CFile::shareDenyNone , &m_eExcept ) ) 
	{
		ULONGLONG len = m_pFP.SeekToEnd();
		if(len == 0 && m_strLogHeader.GetLength() != 0) //file. make header.
		{
			m_pFP.Write(m_strLogHeader, m_strLogHeader.GetLength());
			m_pFP.Write("\r\n", 2);
		}
		m_pFP.Write(strLogData, strLogData.GetLength());		// 가장 끝에 가서
		m_pFP.Write("\r\n", 2);									// 지정한 바이트 수 만큼 저장한다.
		m_pFP.Close();
	}

	sLock.Unlock();
	///////////////////////////////////////////////
}

CString CLogger_Base::MakeFullPath(CString strPath, CString strFileName, SYSTEMTIME sysTime)
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

	/*strFullPath = strFullPath + "\\" + strFileName;
	if (!IsExistFolder(strFullPath) ) ::CreateDirectory(strFullPath, NULL);			//파일이름폴더생성	*/

	strTemp.Format(_T("%s_%04d%02d%02d%02d.txt"), strFileName, sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour);
	strFullPath = strFullPath + "\\" + strTemp;										//로그파일경로생성
	
	return(strFullPath);
}

BOOL CLogger_Base::IsExistFolder(CString strPath)
{
	HANDLE hFind;
    WIN32_FIND_DATA fd;

    if ((hFind = ::FindFirstFile((LPCTSTR)strPath, &fd)) != INVALID_HANDLE_VALUE) {	
		::FindClose(hFind);	
		return TRUE;
    }

	::FindClose(hFind);
	return FALSE;	
}

void CLogger_Base::Create(CString strDir, CString strFileName)
{
	m_strLogName = strFileName;
	m_strLogFolder = strDir;
	m_bCreateFlag = TRUE;
}

BOOL CLogger_Base::IsCreate()
{
	return m_bCreateFlag;
}