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
	strCurrUser.Format(_T("  �� Version : [ %s ], Date of change [ %s ], Mode [ Debug ]  ��"), GetVersion(), GetDateOfChange());
#endif

#ifdef NDEBUG
	strCurrUser.Format(_T("  �� Current User : [ %s ] as [ %s ], Version : [ %s ], Date of change [ %s ], Mode [ Release ]  ��"), theApp.m_vecUserInfo->at(theApp.m_iCurUserIdx).m_strId, strAuthority, GetVersion(), GetDateOfChange());
#endif

	((CFrameWnd*)pWnd)->SetWindowText(strCurrUser);
}

CString Version_Dialog::GetVersion()
{
#pragma comment (lib, "Version.lib")

	CString strVersion;

	// ���� ����� ���α׷��� ��θ� ������ �����̴�.
	TCHAR temp_path[MAX_PATH] = { 0, };

	// ���� ����� ���α׷��� ��θ� ��´�.
	::GetModuleFileName(AfxGetInstanceHandle(), temp_path, sizeof(temp_path));

	// ���� ������ ��� ���� ����� �ڵ鰪�� �����ϴ� �����̴�.
	DWORD h_version_handle;

	// ���������� �׸��� ����ڰ� �߰�/���� �Ҽ� �ֱ� ������ ������ ũ�Ⱑ �ƴϴ�.
	// ���� ���� ���α׷��� ���������� ���� ũ�⸦ �� �� ũ�⿡ �´� �޸𸮸� �Ҵ��ϰ� �۾��ؾ��Ѵ�.
	DWORD version_info_size = GetFileVersionInfoSize(temp_path, &h_version_handle);

	// ���������� �����ϱ� ���� �ý��� �޸𸮸� �����Ѵ�. ( �ڵ� �������� ���� )
	HANDLE h_memory = GlobalAlloc(GMEM_MOVEABLE, version_info_size);

	if (h_memory != NULL) {
		// �ڵ� ������ �޸𸮸� ����ϱ� ���ؼ� �ش� �ڵ鿡 �����Ҽ� �ִ� �ּҸ� ��´�.
		LPVOID p_info_memory = ::GlobalLock(h_memory);

		if (p_info_memory != NULL) {
			// ���� ���α׷��� ���� ������ �����´�.
			::GetFileVersionInfo(temp_path, h_version_handle, version_info_size, p_info_memory);

			// ���� ������ ���Ե� �� �׸� ���� ��ġ�� ������ �����̴�.�� �����Ϳ� ���޵� �ּҴ� p_info_memory �� ���� ��ġ�̱� ������ �����ϸ� �ȵ˴ϴ�.			
			char* p_data = NULL;

			// ������ ���� ������ ũ�⸦ ������ �����̴�.
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

			// ���������� ���Ե� FileVersion ������ �� ����Ѵ�.
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
		FILETIME create_time, access_time, write_time;									// ������ ���Ͽ��� ������ ����, �ֱ� ��� �׸��� �ֱ� ���ŵ� �ð��� ���
		GetFileTime(h_File, &create_time, &access_time, &write_time); 					// create_time:���糯¥,
																						// access_time:������ ���� ��¥
																						// write_time:������ ������¥
		SYSTEMTIME write_system_time, write_local_time;
		FileTimeToSystemTime(&write_time, &write_system_time);							//filetime�� systemtime���� ����

																						//systemtime�� �׳� ���� �ȵǰ� UTC(Universal Time Coordinated) �����̱� ������ �����ð����� ����
		SystemTimeToTzSpecificLocalTime(NULL, &write_system_time, &write_local_time);

		strDateOfChange.Format(_T("%d��%d��%d��%d��%d��%d��"), write_local_time.wYear, write_local_time.wMonth, write_local_time.wDay, write_local_time.wHour, write_local_time.wMinute, write_local_time.wSecond);
		CloseHandle(h_File);
	}

	return strDateOfChange;
}

void Version_Dialog::CreateResourceText()
{
	//CreateDirectory(_T(RESOURCE_FOLDER_TEST), NULL); // Verison Info ���� ���� ����

	// Find and load the resource
	HRSRC hResource = FindResource(NULL, MAKEINTRESOURCE(IDR_TEXT1), _T("TEXT")); // ���ҽ� ���� ã��
	HGLOBAL hFileResource = LoadResource(NULL, hResource); // ���ҽ��� �о� �޸� �ڵ� ����

	LPVOID lpFile = LockResource(hFileResource); // ���ҽ��� �ִ� �����͸� ����
	DWORD dwSize = SizeofResource(NULL, hResource); // �ؽ�Ʈ ���ҽ��� ���̸� Ȯ��

	HANDLE hFile = CreateFile(_T("")_T(RESOURCE_FILE), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL); // ����, ��ο� Version ���� ����(Resource)
	HANDLE hFileMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, dwSize, NULL);
	LPVOID lpAddress = MapViewOfFile(hFileMap, FILE_MAP_WRITE, 0, 0, 0);

	CopyMemory(lpAddress, lpFile, dwSize);

	UnmapViewOfFile(lpAddress);
	CloseHandle(hFileMap);
	CloseHandle(hFile);

	return;
}
