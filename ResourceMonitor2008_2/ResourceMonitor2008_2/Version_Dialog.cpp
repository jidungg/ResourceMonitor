//
//:  Implementation of the Version_Dialog class.

#include "stdafx.h"
#include "resource.h"
#include "Version_Dialog.h"


// -------------------- Function Version_Dialog ------------------
Version_Dialog::Version_Dialog(const char* title, int width, int height, int position_x, int position_y) :
	d_font_name("MS Sans Serif"), d_font_size(8), d_template_buffer(0)
{
	d_dialog_template.style = WS_CAPTION | WS_VISIBLE | WS_DLGFRAME | WS_POPUP | DS_MODALFRAME | DS_SETFONT | WS_SYSMENU;
	
	d_dialog_template.dwExtendedStyle = 0;
	d_dialog_template.cdit = 0;
	
	d_dialog_template.x = static_cast<short>(position_x);
	d_dialog_template.y = static_cast<short>(position_y);
	d_dialog_template.cx = static_cast<short>(width);
	d_dialog_template.cy = static_cast<short>(height);
	
	return;
}

Version_Dialog::~Version_Dialog()
{
	delete m_pEdit;
	m_pEdit = NULL;
}

// -------------------- Version_Dialog message map ---------------

BEGIN_MESSAGE_MAP(Version_Dialog, CDialog)
	
END_MESSAGE_MAP()


BOOL Version_Dialog::OnInitDialog()
{
	m_pEdit = new CEdit;
	m_pEdit->Create(ES_LEFT | ES_READONLY | ES_MULTILINE | ES_AUTOHSCROLL | WS_VSCROLL | WS_HSCROLL, CRect(10, 5, 1200, 800), this, 1000);
	m_pEdit->ShowWindow(SW_SHOW);

	CString str;
	CString strTest;
	CStringList strList;
	CStdioFile File;
	str.Format(_T("Version.txt"));
	if (File.Open((LPCTSTR)(str), CFile::modeRead | CFile::typeText))
	{
		CString line;

		while (NULL != File.ReadString(line))
		{
			if (line.GetLength() <= 0)
			{
				strTest += _T("\r\n");
				continue;
			}

			strTest += line + _T("\r\n");
		}

		File.Close();
	}

	m_pEdit->SetWindowText(strTest);

	return TRUE;
}

BOOL Version_Dialog::Create(CWnd* pWndParent, bool bChild)
{
	BOOL bRet = FALSE;

	d_dialog_template.cdit = 0;

	if ((Build_template_buffer(d_dialog_template, (char *)(LPCTSTR)d_title, (char *)(LPCTSTR)d_font_name, d_font_size, &d_template_buffer)))
	{
		if (CreateIndirect((DLGTEMPLATE*)d_template_buffer, pWndParent))
		{
			bRet = TRUE;
		}
	}
	if ( bChild == true )
	{
		SetParent(pWndParent);
	}

	delete[](d_template_buffer);
	return bRet;
}

bool Version_Dialog::Build_template_buffer(const DLGTEMPLATE& dialog_template, const char* dialog_caption, const char* dialog_font_name, const WORD dialog_font_size, unsigned char** buffer)
{
	int buf_size = sizeof(DLGTEMPLATE) + 3 * sizeof(WORD) +
		sizeof(WCHAR) * (strlen(dialog_caption) + 1 +
			strlen(dialog_font_name) + 1);

	buf_size = (buf_size + 3) & ~3;

	if (*buffer)
	{
		delete[](*buffer);
	}

	bool okay = true;

	try
	{
		*buffer = new unsigned char[buf_size];
	}

	catch (CMemoryException* e)
	{
		*buffer = 0;
		e->Delete();
		okay = false;
	}

	if (okay)
	{
		unsigned char* pdest = *buffer;
		memset(pdest, 0, buf_size);

		memcpy(pdest, &dialog_template, sizeof(DLGTEMPLATE));
		pdest += sizeof(DLGTEMPLATE);
		*(WORD*)pdest = 0;            // The dialog has no menu.
		*(WORD*)(pdest + 1) = 0;      // The default window class is
		pdest += 2 * sizeof(WORD);    // required.

		if (pdest)  // Wrote the dialog title successfully?
		{
			*(WORD*)pdest = dialog_font_size;
			pdest += sizeof(WORD);
		}

		if (!pdest)
		{
			okay = false;
		}
	}

	return okay;
}

void Version_Dialog::ViewVersion(CWnd* pWnd)
{
	CString strCurrUser;

#ifdef _DEBUG
	strCurrUser.Format(_T("  ♣ Version : [ %s ], Date of change [ %s ], Mode [ Debug ]  ♣"), GetVersion(), GetDateOfChange());
#endif

#ifdef NDEBUG
	strCurrUser.Format(_T("  ♣ Current User : [ %s ] as [ %s ], Version : [ %s ], Date of change [ %s ], Mode [ Release ]  ♣"), theApp.m_vecUserInfo->at(theApp.m_iCurUserIdx).m_strId, strAuthority, GetVersion(), GetDateOfChange());
#endif

	((CFrameWnd*)pWnd)->SetWindowText(strCurrUser);
}

CString Version_Dialog::GetVersion()
{
#pragma comment (lib, "Version.lib")

	CString strVersion;

	// 현재 실행된 프로그램의 경로를 저장할 변수이다.
	TCHAR temp_path[MAX_PATH] = { 0, };

	// 현재 실행된 프로그램의 경로를 얻는다.
	::GetModuleFileName(AfxGetInstanceHandle(), temp_path, sizeof(temp_path));

	// 버전 정보를 얻기 위해 사용할 핸들값을 저장하는 변수이다.
	DWORD h_version_handle;

	// 버전정보는 항목을 사용자가 추가/삭제 할수 있기 때문에 고정된 크기가 아니다.
	// 따라서 현재 프로그램의 버전정보에 대한 크기를 얻어서 그 크기에 맞는 메모리를 할당하고 작업해야한다.
	DWORD version_info_size = GetFileVersionInfoSize(temp_path, &h_version_handle);

	// 버전정보를 저장하기 위한 시스템 메모리를 생성한다. ( 핸들 형식으로 생성 )
	HANDLE h_memory = GlobalAlloc(GMEM_MOVEABLE, version_info_size);

	if (h_memory != NULL) {
		// 핸들 형식의 메모리를 사용하기 위해서 해당 핸들에 접근할수 있는 주소를 얻는다.
		LPVOID p_info_memory = ::GlobalLock(h_memory);

		if (p_info_memory != NULL) {
			// 현재 프로그램의 버전 정보를 가져온다.
			::GetFileVersionInfo(temp_path, h_version_handle, version_info_size, p_info_memory);

			// 버전 정보에 포함된 각 항목별 정보 위치를 저장할 변수이다.이 포인터에 전달된 주소는 p_info_memory 의 내부 위치이기 때문에 해제하면 안됩니다.			
			char* p_data = NULL;

			// 실제로 읽은 정보의 크기를 저장할 변수이다.
			UINT data_size = 0;

			struct LANGANDCODEPAGE
			{
				WORD wLanguage;
				WORD wCodePage;
			} *lpTranslate;

			UINT cbTranslate;
			LPBYTE lpBuffer = NULL;
			VerQueryValue(p_info_memory, _T("\\VarFileInfo\\Translation"), (LPVOID*)&lpTranslate, &cbTranslate);

			CString strSub;

			if (cbTranslate != 0)
			{

				strSub.Format(_T("\\\\StringFileInfo\\%04x%04x\\FileVersion"), lpTranslate[0].wLanguage, lpTranslate[0].wCodePage);
			}

			// 버전정보에 포함된 FileVersion 정보를 얻어서 출력한다.
			//if (VerQueryValue(p_info_memory, "\\StringFileInfo\\040903b5\\FileVersion", (void**)&p_data, &data_size)) {
			if (VerQueryValue(p_info_memory, strSub, (void**)&p_data, &data_size))
			{
				strVersion.Format(_T("%s"), (WCHAR*)p_data);
			}

			GlobalUnlock(h_memory);
		}

		GlobalFree(h_memory);
	}

	return strVersion;
}

CString Version_Dialog::GetDateOfChange()
{
	CString strDateOfChange = _T("");

	TCHAR szFilePath[_MAX_PATH];
	::GetModuleFileName(NULL, szFilePath, _MAX_PATH);

	HANDLE h_File = CreateFile(szFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (h_File != INVALID_HANDLE_VALUE)
	{
		FILETIME create_time, access_time, write_time;									// 지정된 파일에서 파일의 생성, 최근 사용 그리고 최근 갱신된 시간을 얻고
		GetFileTime(h_File, &create_time, &access_time, &write_time); 					// create_time:만든날짜,
																						// access_time:마지막 접근 날짜
																						// write_time:마지막 수정날짜
		SYSTEMTIME write_system_time, write_local_time;
		FileTimeToSystemTime(&write_time, &write_system_time);							//filetime을 systemtime으로 변경

																						//systemtime을 그냥 쓰면 안되고 UTC(Universal Time Coordinated) 형식이기 때문에 지역시간으로 변경
		SystemTimeToTzSpecificLocalTime(NULL, &write_system_time, &write_local_time);

		strDateOfChange.Format(_T("%d년%d월%d일%d시%d분%d초"), write_local_time.wYear, write_local_time.wMonth, write_local_time.wDay, write_local_time.wHour, write_local_time.wMinute, write_local_time.wSecond);
		CloseHandle(h_File);
	}

	return strDateOfChange;
}

void Version_Dialog::CreateResourceText()
{
	//CreateDirectory(_T(RESOURCE_FOLDER_TEST), NULL); // Verison Info 저장 폴더 생성

	// Find and load the resource
	HRSRC hResource = FindResource(NULL, MAKEINTRESOURCE(IDR_TEXT1), _T("TEXT")); // 리소스 파일 찾기
	HGLOBAL hFileResource = LoadResource(NULL, hResource); // 리소스를 읽어 메모리 핸들 리턴

	LPVOID lpFile = LockResource(hFileResource); // 리소스가 있는 포인터를 리턴
	DWORD dwSize = SizeofResource(NULL, hResource); // 텍스트 리소스의 길이를 확인

	HANDLE hFile = CreateFile(_T("")_T(RESOURCE_FILE), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL); // 폴더, 경로에 Version 파일 생성(Resource)
	HANDLE hFileMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, dwSize, NULL);
	LPVOID lpAddress = MapViewOfFile(hFileMap, FILE_MAP_WRITE, 0, 0, 0);

	CopyMemory(lpAddress, lpFile, dwSize);

	UnmapViewOfFile(lpAddress);
	CloseHandle(hFileMap);
	CloseHandle(hFile);

	return;
}
