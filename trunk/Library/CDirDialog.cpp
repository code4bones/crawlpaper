/*
	CDirDialog.cpp
	Classe base per la selezione della directory (SDK/MFC).
	Riadattata e modificata dall'originale di James Spibey per essere usata senza MFC.
	Luca Piergentili, 14/07/00
	lpiergentili@yahoo.com
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "strings.h"
#include "window.h"
#include "win32api.h"
#include <shlobj.h>
#include "CDirDialog.h"

#include "traceexpr.h"
//#define _TRACE_FLAG	_TRFLAG_TRACEOUTPUT
//#define _TRACE_FLAG	_TRFLAG_NOTRACE
#define _TRACE_FLAG		_TRFLAG_NOTRACE
#define _TRACE_FLAG_INFO	_TRFLAG_NOTRACE
#define _TRACE_FLAG_WARN	_TRFLAG_NOTRACE
#define _TRACE_FLAG_ERR	_TRFLAG_NOTRACE

#if (defined(_DEBUG) && defined(_WINDOWS)) && (defined(_AFX) || defined(_AFXDLL))
#ifdef PRAGMA_MESSAGE_VERBOSE
  #pragma message("\t\t\t"__FILE__"("STR(__LINE__)"): using DEBUG_NEW macro")
#endif
  #define new DEBUG_NEW
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif

// per l'always on top
HWND m_hWnd = NULL;
static BOOL m_bAlwaysOnTop = FALSE;

/*
	CDirDialog()
*/
CDirDialog::CDirDialog(LPCSTR lpcszStartFolder/*=NULL*/,LPCSTR lpcszWindowTitle/*=NULL*/,LPCSTR lpcszTitle/*=NULL*/,BOOL bAlwaysOnTop/*=FALSE*/)
{
	if(!lpcszStartFolder)
		memset(m_szSelDir,'\0',sizeof(m_szSelDir));
	else
		strcpyn(m_szSelDir,lpcszStartFolder,sizeof(m_szSelDir));
	
	memset(m_szInitDir,'\0',sizeof(m_szInitDir));

	if(!lpcszWindowTitle)
		memset(m_szWindowTitle,'\0',sizeof(m_szWindowTitle));
	else
		strcpyn(m_szWindowTitle,lpcszWindowTitle,sizeof(m_szWindowTitle));
	
	if(!lpcszTitle)
		memset(m_szTitle,'\0',sizeof(m_szTitle));
	else
		strcpyn(m_szTitle,lpcszTitle,sizeof(m_szTitle));
	
	memset(m_szPathName,'\0',sizeof(m_szPathName));

	m_bStatus = FALSE;
	m_bAlwaysOnTop = bAlwaysOnTop;
	m_nImageIndex = 0;
}

/*
	DoModal()
*/
int CDirDialog::DoModal(HWND hWnd/*=NULL*/)
{
	if(!strempty(m_szSelDir))
	{
		strrtrim(m_szSelDir);
		::RemoveBackslash(m_szSelDir);
	}

	LPMALLOC pMalloc;
	if(::SHGetMalloc(&pMalloc)!=NOERROR)
		return(IDCANCEL);

	BROWSEINFO bInfo;
	LPITEMIDLIST pidl;
	::ZeroMemory((PVOID)&bInfo,sizeof(BROWSEINFO));

	if(!strempty(m_szInitDir))
	{
		OLECHAR olePath[_MAX_PATH];
		ULONG chEaten;
		ULONG dwAttributes;
		HRESULT hr;
		LPSHELLFOLDER pDesktopFolder;

		// get a pointer to the Desktop's IShellFolder interface
		if(SUCCEEDED(::SHGetDesktopFolder(&pDesktopFolder)))
		{
			// IShellFolder::ParseDisplayName requires the file name be in Unicode
			MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,m_szInitDir,-1,olePath,_MAX_PATH);

			// convert the path to an ITEMIDLIST
			hr = pDesktopFolder->ParseDisplayName(NULL,NULL,olePath,&chEaten,&pidl,&dwAttributes);

			if(FAILED(hr))
			{
				pMalloc->Free(pidl);
				pMalloc->Release();
				return(IDCANCEL);
			}

			bInfo.pidlRoot = pidl;
		}
	}
	
	bInfo.hwndOwner = m_hWnd = hWnd;
	bInfo.pszDisplayName	= m_szPathName;
	bInfo.lpszTitle		= m_szTitle;
	bInfo.ulFlags			= BIF_USENEWUI | BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS | (m_bStatus ? BIF_STATUSTEXT : 0);
	bInfo.lpfn			= &BrowseCtrlCallback;
	bInfo.lParam			= (LPARAM)this;

	if((pidl = ::SHBrowseForFolder(&bInfo))==NULL)
		return(IDCANCEL);
	
	m_nImageIndex = bInfo.iImage;

	if(::SHGetPathFromIDList(pidl,m_szPathName)==FALSE)
	{
		pMalloc->Free(pidl);
		pMalloc->Release();
		return(IDCANCEL);
	}

	pMalloc->Free(pidl);
	pMalloc->Release();
	return(IDOK);
}

/*
	BrowseCtrlCallback()
*/
int __stdcall CDirDialog::BrowseCtrlCallback(HWND hWnd,UINT uMsg,LPARAM lParam,LPARAM lpData)
{
	CDirDialog* pCDirDialog = (CDirDialog*)lpData;

	if(uMsg==BFFM_INITIALIZED)
	{
		if(!strempty(pCDirDialog->m_szSelDir))
			::SendMessage(hWnd,BFFM_SETSELECTION,TRUE,(LPARAM)pCDirDialog->m_szSelDir);
		if(!strempty(pCDirDialog->m_szWindowTitle))
			::SetWindowText(hWnd,pCDirDialog->m_szWindowTitle);
		if(m_bAlwaysOnTop)
			::SetWindowPos(hWnd,/*HWND_TOPMOST|*/m_hWnd,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	}
	else if(uMsg==BFFM_SELCHANGED)
	{
		LPITEMIDLIST pidl = (LPITEMIDLIST)lParam;
		char selection[_MAX_PATH];
		
		if(!::SHGetPathFromIDList(pidl,selection))
			selection[0] = '\0';

		LPSTR lpszStatusText = NULL;
		BOOL bOk = pCDirDialog->SelChanged(selection,lpszStatusText);

		if(pCDirDialog->m_bStatus)
			::SendMessage(hWnd,BFFM_SETSTATUSTEXT,0,(LPARAM)(LPCSTR)lpszStatusText);

		::SendMessage(hWnd,BFFM_ENABLEOK,0,bOk);
	}

	return(0);
}
