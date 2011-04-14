/*
	James Spibey, 04/08/1998
	spib@bigfoot.com
	You are free to use, distribute or modify this code as long as this header is not removed or modified.

	-------------------------------------------------------

	Added support for file compression through the GZW API.
	More changes.
	Luca Piergentili, 24/08/00
	lpiergentili@yahoo.com
*/
#ifndef _SELF_EXTRACTOR_H
#define _SELF_EXTRACTOR_H 1

#include "window.h"
#include "CSelfExtractor.h"
#include "CSEFileInfo.h"
#include "CDirDialog.h"
#include "CTextProgressCtrl.h"
#include "resource.h"

class CSelfExtractorDlg : public CDialog
{
public:
	CSelfExtractorDlg(CWnd* pParent = NULL);

	enum { IDD = IDD_SELFEXTRACTOR_DIALOG };

	CString		GetWorkingDir(BOOL withBackSlash = FALSE);
	void			UpdateList();
	static UINT	AddCallBack(LPVOID CallbackData,LPVOID UserData);

	CSelfExtractor	m_Extractor;
	CString		m_strWorkingDir;
	CTextProgressCtrl m_Progress;
	CListCtrl		m_List;
	CString		m_strExePath;
	CString		m_strExtractorPath;
	CString		m_strCurrFile;

protected:
	void			DoDataExchange(CDataExchange* pDX);

	HICON		m_hIcon;

	BOOL			OnInitDialog(void);
	void			OnPaint(void);
	HCURSOR		OnQueryDragIcon(void);

	void			OnBrowseExtractor(void);
	void			OnReadExtractor(void);

	void			OnBrowseExe(void);
	void			OnCreateExe(void);

	void			OnAddFile(void);
	void			OnAddScript(void);
	void			OnClearList(void);

	DECLARE_MESSAGE_MAP()
};

#endif // _SELF_EXTRACTOR_H
