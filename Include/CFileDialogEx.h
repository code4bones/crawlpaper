/*
	CFileDialogEx.h
	Classi base per i dialoghi "File Open|Save as" (MFC).
	Luca Piergentili, 14/02/00
	lpiergentili@yahoo.com
*/
#ifndef _CFILEDIALOGEX_H
#define _CFILEDIALOGEX_H 1

#include <stdlib.h>
#include <string.h>
#include "strings.h"
#include "window.h"

/*
	CFileDialogEx

	Classe base per i dialoghi "File Open|Save as".
	Rimappa la CFileDialog aggiungendo il membro per l'aggiunta dinamica dei tipi di files.
*/
class CFileDialogEx : public CFileDialog
{
public:
	CFileDialogEx(	BOOL		bOpenFileDialog,
				LPCSTR	lpszDefExt = NULL,
				LPCSTR	lpszFileName = NULL,
				DWORD	dwFlags = OFN_HIDEREADONLY|OFN_FORCESHOWHIDDEN|OFN_OVERWRITEPROMPT,
				LPCSTR	lpszFilter = NULL,
				CWnd*	pWndParent = NULL)
				:
	CFileDialog(	bOpenFileDialog,
				lpszDefExt,
				lpszFileName,
				dwFlags,
				lpszFilter,
				pWndParent) {}
	
	virtual ~CFileDialogEx() {}

	int		DoModal		(void);

protected:
	CString	m_strFilter;
	CString	m_strDefaultFilter;
	char		m_szFileName[_MAX_PATH+1];
	LPSTR	m_lpszTitle;
	LPSTR	m_lpszInitialDir;
	LPSTR	m_lpszFilter;
};

/*
	CFileOpenDialog

	Classe derivata per il dialogo "File Open".
*/
class CFileOpenDialog : public CFileDialogEx
{
public:
	CFileOpenDialog(	LPCSTR	lpszTitle = "Open",
					LPCSTR	lpszInitialDir = ".\\",
					LPCSTR	lpszDefExt = "*.*",
					LPCSTR	lpszFilter = "All files (*.*)|*.*||",
					CWnd*	pWndParent = AfxGetMainWnd())
					:
	CFileDialogEx(		TRUE,
					lpszDefExt,
					NULL,
					OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|OFN_FORCESHOWHIDDEN,
					lpszFilter,
					pWndParent)
					{
						CFileDialogEx::m_lpszTitle      = (LPSTR)lpszTitle;
						CFileDialogEx::m_lpszInitialDir = (LPSTR)lpszInitialDir;
						CFileDialogEx::m_lpszFilter     = (LPSTR)lpszFilter;
						memset(CFileDialogEx::m_szFileName,'\0',sizeof(CFileDialogEx::m_szFileName));
					}

	virtual ~CFileOpenDialog() {}
};

/*
	CFileSaveAsDialog

	Classe derivata per il dialogo "Save As".
*/
class CFileSaveAsDialog : public CFileDialogEx
{
public:
	CFileSaveAsDialog(	LPCSTR	lpszTitle = "Save As...",
					LPCSTR	lpszInitialDir = ".\\",
					LPCSTR	lpszDefExt = "*.*",
					LPCSTR	lpszFileName = NULL,
					LPCSTR	lpszFilter = "All files (*.*)|*.*||",
					CWnd*	pWndParent = AfxGetMainWnd())
					:
	CFileDialogEx(		FALSE,
					lpszDefExt,
					lpszFileName,
					OFN_OVERWRITEPROMPT|OFN_HIDEREADONLY|OFN_FORCESHOWHIDDEN|OFN_NOREADONLYRETURN,
					lpszFilter,
					pWndParent)
					{
						CFileDialogEx::m_lpszTitle      = (LPSTR)lpszTitle;
						CFileDialogEx::m_lpszInitialDir = (LPSTR)lpszInitialDir;
						CFileDialogEx::m_lpszFilter     = (LPSTR)lpszFilter;
						if(lpszFileName)
							strcpyn(CFileDialogEx::m_szFileName,lpszFileName,sizeof(CFileDialogEx::m_szFileName));
						else
							memset(CFileDialogEx::m_szFileName,'\0',sizeof(CFileDialogEx::m_szFileName));
					}

	virtual ~CFileSaveAsDialog() {}
};

#endif // _CFILEDIALOGEX_H
