//
//:  Interface to the Version_Dialog class.

#ifndef VERSION_DIALOG_H_

#define VERSION_DIALOG_H_
#define RESOURCE_FILE            "Version.txt"

class Version_Dialog : public CDialog
{
public:
	Version_Dialog(const char* title = 0, int width = 100, int height = 100, int position_x = 0, int position_y = 0);
	virtual ~Version_Dialog();

	void ViewVersion(CWnd* pWnd);
	BOOL Create(CWnd* pWndParent, bool bChild);
	// Use this macro to declare the message map for this class.
protected:
	virtual BOOL OnInitDialog();
	// Generated message map functions
	CEdit *m_pEdit;
	unsigned char* d_template_buffer;

	CString        d_title;
	CString        d_font_name;
	WORD           d_font_size;

	DLGTEMPLATE    d_dialog_template;
	static bool Build_template_buffer(const DLGTEMPLATE& dialog_template, const char* dialog_caption, const char* dialog_font_name,	const WORD dialog_font_size, unsigned char** buffer);

	CString			GetVersion();
	CString			GetDateOfChange();
	void			CreateResourceText();
	DECLARE_MESSAGE_MAP()
};

#endif   // #ifndef VERSION_DIALOG_H_
