/*
	WallBrowserDoc.cpp
	Classe per il documento.
	Luca Piergentili, 30/07/01
	lpiergentili@yahoo.com

	WallBrowser - the smart picture browser
	http://www.crawlpaper.com/
	copyright © 1998-2004 Luca Piergentili, all rights reserved
	crawlpaper is a registered name, all rights reserved
	
	This is a free software, released under the terms of the BSD license. Do not
	attempt to use it in any form which violates the license or you will be persecuted
	and charged for this.

	Redistribution and use in source and binary forms, with or without modification,
	are permitted provided that the following conditions are met:

	- Redistributions of source code must retain the above copyright notice, this
	list of conditions and the following disclaimer. 

	- Redistributions in binary form must reproduce the above copyright notice, this
	list of conditions and the following disclaimer in the documentation and/or other
	materials provided with the distribution. 

	- Neither the name of "crawlpaper" nor the names of its contributors may be used
	to endorse or promote products derived from this software without specific prior
	written permission. 

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
	ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
	IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
	INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
	BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
	LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
	OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
	OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include <stdio.h>
#include <stdlib.h>
#include "strings.h"
#include "window.h"
#include "win32api.h"
#include <afxdisp.h>
#include "CImageDialog.h"
#include "CImageFactory.h"
#include "CRegKey.h"
#include "CWinAppEx.h"
#include "WallPaperVersion.h"
#include "WallBrowserVersion.h"
#include "WallBrowserMessages.h"
#include "WallBrowserDoc.h"
#include "resource.h"

#include "traceexpr.h"
//#define _TRACE_FLAG	_TRFLAG_TRACEOUTPUT
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

IMPLEMENT_DYNCREATE(CWallBrowserDoc,CDocument)

BEGIN_MESSAGE_MAP(CWallBrowserDoc,CDocument)
	ON_COMMAND(ID_FILE_PRINT,OnNotImplementedYet)
	ON_COMMAND(ID_FILE_PRINT_SETUP,OnNotImplementedYet)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW,OnNotImplementedYet)
	ON_COMMAND(ID_FILE_SAVE,OnFileSave)
	ON_COMMAND(ID_FILE_SAVE_AS,OnFileSaveAs)
	ON_COMMAND(ID_TOOLS_SETWALLPAPER,OnSetWallPaper)
	ON_COMMAND(ID_TOOLS_RESETWALLPAPER,OnResetWallPaper)
END_MESSAGE_MAP()

/*
	CWallBrowserDoc()
*/
CWallBrowserDoc::CWallBrowserDoc()
{
	m_bPictureFlag = FALSE;

	// crea l'oggetto immagine a seconda della libreria
	memset(m_szLibraryName,'\0',sizeof(m_szLibraryName));
	CWinAppEx* pWinAppEx = (CWinAppEx*)AfxGetApp();
	if(pWinAppEx)
		strcpyn(m_szLibraryName,pWinAppEx->m_Config.GetString(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_CURRENTLIBRARY_KEY),sizeof(m_szLibraryName));
	m_pImage = m_ImageFactory.Create(m_szLibraryName,sizeof(m_szLibraryName));

	memset(m_szFileName,'\0',sizeof(m_szFileName));
	memset(m_szPathName,'\0',sizeof(m_szPathName));
	memset(m_szModified,'\0',sizeof(m_szModified));

	// array per le viste
	m_nCurrentView = 0;
	for(int i = 0; i < ARRAY_SIZE(m_ViewArray); i++)
		m_ViewArray[i] = NULL;
}

/*
	SetTitle()
*/
void CWallBrowserDoc::SetTitle(LPCTSTR lpszTitle)
{
	m_strTitle.Format("%s",lpszTitle);
	
	int nLen = m_strTitle.GetLength()-1;
	if(nLen > 0)
	{
		if(nLen==2 && m_strTitle.GetAt(1)==':')
			;
		else
		{
			if(m_strTitle.GetAt(nLen)=='\\')
				m_strTitle.SetAt(nLen,'\0');
		}
	}

	CWnd* pWnd = AfxGetMainWnd();
	if(pWnd)
	{
		int nDirSplitterSize = pWnd->SendMessage(WM_GET_DIRSPLITTER_SIZE,0,0L);
		if(nDirSplitterSize <= 0)
		{
			char szPathname[_MAX_PATH+1];
			_snprintf(szPathname,sizeof(szPathname)-1," - (%s",GetPathName());
			::RemoveBackslash(szPathname);
			strcat(szPathname,")");
			m_strTitle += szPathname;
		}
		::SetWindowText(pWnd->m_hWnd,m_strTitle);
	}
}

/*
	SaveModified()
*/
BOOL CWallBrowserDoc::SaveModified(void)
{
	if(IsModified())
	{
		SetModifiedFlag(FALSE);
		
		CWnd* pWnd = AfxGetMainWnd();
		if(m_szModified[0]=='\0')
			_snprintf(m_szModified,sizeof(m_szModified)-1,"%s%s",m_szPathName,m_szFileName);
		int nRet = ::MessageBoxResourceEx(pWnd ? pWnd->m_hWnd : NULL,MB_ICONWARNING|MB_YESNO,WALLBROWSER_PROGRAM_NAME,IDS_QUESTION_SAVECHANGES,m_szModified);
		if(nRet==IDYES)
			OnFileSave();
		else if(nRet==IDNO)
			SetModifiedFlag(FALSE);
	}
	
	return(TRUE);
}

/*
	SaveModified()
*/
BOOL CWallBrowserDoc::SaveModified(LPCSTR lpcszFileName)
{
	strcpyn(m_szModified,lpcszFileName,sizeof(m_szModified));
	return(SaveModified());
}

/*
	SetPictureFlag()
*/
void CWallBrowserDoc::SetPictureFlag(BOOL bFlag)
{
	m_bPictureFlag = bFlag;
	CWnd* pWnd = AfxGetMainWnd();
	if(pWnd)
	{
		pWnd->SendMessage(WM_TOOLS_SETWALLPAPER,m_bPictureFlag,0L);
		pWnd->SendMessage(WM_TOOLS_RESETWALLPAPER,m_bPictureFlag,0L);
	}
}

/*
	SetLibrary()
*/
void CWallBrowserDoc::SetLibrary(LPCSTR lpcszLibraryName)
{
	strcpyn(m_szLibraryName,lpcszLibraryName,sizeof(m_szLibraryName));
	CWinAppEx* pWinAppEx = (CWinAppEx*)AfxGetApp();
	if(pWinAppEx)
	{
		pWinAppEx->m_Config.UpdateString(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_CURRENTLIBRARY_KEY,m_szLibraryName);
		pWinAppEx->m_Config.SaveKey(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_CURRENTLIBRARY_KEY);
	}
	m_pImage = m_ImageFactory.Create(m_szLibraryName,sizeof(m_szLibraryName));
	TRACE("%s(%d): sending ONUPDATE_FLAG_DEFAULTLIBRARY\n",__FILE__,__LINE__);
	UpdateAllViews(NULL,ONUPDATE_FLAG_DEFAULTLIBRARY);
}

/*
	SetPathName()
*/
void CWallBrowserDoc::SetPathName(LPCSTR lpcszPathName)
{
	strcpyn(m_szPathName,lpcszPathName,sizeof(m_szPathName));
	::EnsureBackslash(m_szPathName,sizeof(m_szPathName));
}

/*
	OnSaveDocument()
*/
BOOL CWallBrowserDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	BOOL bSaved = FALSE;
	
	if(m_pImage)
	{
		bSaved = m_pImage->Save();
	
		if(!bSaved)
		{
			CWnd* pWnd = AfxGetMainWnd();
			::MessageBoxResourceEx(pWnd ? pWnd->m_hWnd : NULL,MB_ICONWARNING,WALLBROWSER_PROGRAM_NAME,IDS_ERROR_FILESAVE,lpszPathName);
		}

		SetModifiedFlag(FALSE);

		TRACE("%s(%d): sending ONUPDATE_FLAG_EMPTYSTACK\n",__FILE__,__LINE__);
		UpdateAllViews(NULL,ONUPDATE_FLAG_EMPTYSTACK);
	}

	return(bSaved);
}

/*
	OnFileSave()
*/
void CWallBrowserDoc::OnFileSave(void)
{
	BOOL bSaved = FALSE;
	
	if(m_pImage)
	{
		CWnd* pWnd = AfxGetMainWnd();
		if(pWnd)
		{
			CStatusBar* pMainFrameStatusBar = (CStatusBar*)pWnd->SendMessage(WM_GET_STATUSBAR,0,0);
			if(pMainFrameStatusBar)
				pMainFrameStatusBar->SetPaneText(3,"saving...");
		}

		bSaved = m_pImage->Save();
	
		if(!bSaved)
		{
			CWnd* pWnd = AfxGetMainWnd();
			::MessageBoxResourceEx(pWnd ? pWnd->m_hWnd : NULL,MB_ICONWARNING,WALLBROWSER_PROGRAM_NAME,IDS_ERROR_FILESAVE,m_pImage->GetFileName());
		}

		SetModifiedFlag(FALSE);

		if(pWnd)
			pWnd->SendMessage(WM_FILE_SAVE,!bSaved,0L);
		
		TRACE("%s(%d): sending ONUPDATE_FLAG_EMPTYSTACK\n",__FILE__,__LINE__);
		UpdateAllViews(NULL,ONUPDATE_FLAG_EMPTYSTACK);
	}
}

/*
	OnFileSaveAs()
*/
void CWallBrowserDoc::OnFileSaveAs(void)
{
	BOOL bSaved = FALSE;
	CWinAppEx* pWinAppEx = (CWinAppEx*)AfxGetApp();
	
	if(m_pImage)
	{
		CImageSaveAsDialog dlg(	(CWnd*)this,
							m_pImage,
							pWinAppEx ? pWinAppEx->m_Config.GetString(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_LASTSAVEAS_KEY) : ""
							);
		if(dlg.DoModal()==IDOK)
		{
			CWaitCursor cursor;
			
			char szDir[_MAX_PATH+1];
			strcpyn(szDir,dlg.GetPathName(),sizeof(szDir));
			strrev(szDir);
			int i = strchr(szDir,'\\') - szDir;
			strrev(szDir);
			if(i > 0)
				szDir[strlen(szDir)-i-1] = '\0';
			if(strlen(szDir)==2 && szDir[1]==':')
				strcat(szDir,"\\");
			if(pWinAppEx)
			{
				pWinAppEx->m_Config.UpdateString(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_LASTSAVEAS_KEY,szDir);
				pWinAppEx->m_Config.SaveKey(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_LASTSAVEAS_KEY);
			}

			CWnd* pWnd = AfxGetMainWnd();
			if(pWnd)
			{
				CStatusBar* pMainFrameStatusBar = (CStatusBar*)pWnd->SendMessage(WM_GET_STATUSBAR,0,0);
				if(pMainFrameStatusBar)
					pMainFrameStatusBar->SetPaneText(3,"saving...");
			}

			CString strExt;
			strExt.Format(".%s",dlg.GetFileExt());
			bSaved = dlg.SaveAs(dlg.GetPathName(),strExt);

			if(!bSaved)
			{
				CWnd* pWnd = AfxGetMainWnd();
				::MessageBoxResourceEx(pWnd ? pWnd->m_hWnd : NULL,MB_ICONWARNING,WALLBROWSER_PROGRAM_NAME,IDS_ERROR_FILESAVE,dlg.GetPathName());
			}

			SetModifiedFlag(FALSE);

			if(pWnd)
				pWnd->SendMessage(WM_FILE_SAVE,!bSaved,0L);
		}

		COnUpdateInfo OnUpdateInfo(dlg.GetFileName());
		TRACE("%s(%d): sending ONUPDATE_FLAG_EMPTYSTACK|ONUPDATE_FLAG_DIRUPDATED\n",__FILE__,__LINE__);
		UpdateAllViews(NULL,ONUPDATE_FLAG_EMPTYSTACK|ONUPDATE_FLAG_DIRUPDATED,&OnUpdateInfo);
	}
}

/*
	OnNotImplementedYet()
*/
void CWallBrowserDoc::OnNotImplementedYet(void)
{
	::MessageBox(NULL,"Not implemented yet.","Print()",MB_ICONWARNING);
}

/*
	OnSetWallPaper()
*/
void CWallBrowserDoc::OnSetWallPaper(void)
{
	CWaitCursor cursor;

	if(HavePathName() && HaveFileName())
	{
		char szFileName[_MAX_PATH+1];

		_snprintf(szFileName,
				sizeof(szFileName)-1,
				"%s%s",
				GetPathName(),
				GetFileName()
				);

		CImage* pImage;
		CImageFactory ImageFactory;
		pImage = ImageFactory.Create(m_szLibraryName,sizeof(m_szLibraryName));
		if(pImage)
		{
			if(pImage->Load(szFileName))
			{
				CWinAppEx* pWinAppEx = (CWinAppEx*)AfxGetApp();
				if(pWinAppEx)
				{
					_snprintf(szFileName,sizeof(szFileName)-1,"%sWallPaper.bmp",pWinAppEx->m_Config.GetString(WALLBROWSER_INSTALL_KEY,WALLBROWSER_DIR_KEY));
					if(pImage->Save(szFileName,".bmp"))
					{
						SetWallPaperMode(WINDOWS_DRAWMODE_STRETCH);
						SetWallPaper(szFileName);
					}
				}
			}
		}
	}
}

/*
	OnResetWallPaper()
*/
void CWallBrowserDoc::OnResetWallPaper(void)
{
	ResetWallPaper();
}

/*
	SetWallPaperMode()
*/
void CWallBrowserDoc::SetWallPaperMode(UINT nMode)
{
	CRegKey regkey;
	char value[REGKEY_MAX_KEY_VALUE+1];
	BOOL bHaveStyle = FALSE;

	/*
	mentre con W95/NT viene usata solo la chiave TileWallpaper (con valori da 0 a 2), la zoccola di W98 usa anche la chiave WallpaperStyle:
				normal	stretch	tile
	TileWallpaper	0		0		1
	WallpaperStyle	0		2		0
	*/
	if(regkey.Open(HKEY_CURRENT_USER,"Control Panel\\Desktop")==ERROR_SUCCESS)
	{
		DWORD dwMode = nMode==WINDOWS_DRAWMODE_STRETCH ? WINDOWS_DRAWMODE_STRETCH : WINDOWS_DRAWMODE_NORMAL;
		memset(value,'\0',sizeof(value));
		DWORD valuesize = sizeof(value);
		if(regkey.QueryValue(value,"WallpaperStyle",&valuesize)==ERROR_SUCCESS)
		{
			_snprintf(value,sizeof(value)-1,"%d",dwMode);
			regkey.SetValue(value,"WallpaperStyle");
			bHaveStyle = TRUE;
		}
		
		regkey.Close();
	}

	if(regkey.Open(HKEY_CURRENT_USER,"Control Panel\\Desktop")==ERROR_SUCCESS)
	{
		if(bHaveStyle && nMode==WINDOWS_DRAWMODE_STRETCH)
			nMode = WINDOWS_DRAWMODE_NORMAL;

		_snprintf(value,sizeof(value)-1,"%d",nMode);
		regkey.SetValue(value,"TileWallPaper");
		
		regkey.Close();
	}
}

/*
	SetWallPaper()
*/
void CWallBrowserDoc::SetWallPaper(LPCSTR lpcszFileName,UINT nMode/* = 0 */)
{
	CRegKey regkey;
	LONG reg;
	char value[REGKEY_MAX_KEY_VALUE+1];

	if((reg = regkey.Open(HKEY_CURRENT_USER,"Control Panel\\Desktop"))==ERROR_SUCCESS)
	{
		regkey.SetValue(lpcszFileName,"Wallpaper");
		_snprintf(value,sizeof(value)-1,"%d",nMode);
		regkey.SetValue(value,"TileWallPaper");
		regkey.Close();
	}
	
	::SystemParametersInfo(SPI_SETDESKWALLPAPER,NULL,(PVOID)lpcszFileName,SPIF_UPDATEINIFILE|SPIF_SENDCHANGE);
}

/*
	ResetWallPaper()
*/
void CWallBrowserDoc::ResetWallPaper(void)
{
	SetWallPaperMode(WINDOWS_DRAWMODE_STRETCH);
	SetWallPaper("");
	::SystemParametersInfo(SPI_SETDESKWALLPAPER,NULL,"",SPIF_UPDATEINIFILE|SPIF_SENDCHANGE);
}

/*
	GotoNextView()
*/
void CWallBrowserDoc::GotoNextView(void)
{
	CFrameWnd* pMainFrame = (CFrameWnd*)AfxGetApp()->m_pMainWnd;
	if(pMainFrame)
	{
		if(m_ViewArray[0]==NULL)
		{
			POSITION pos = GetFirstViewPosition();
			for(int i = 0; pos; i++)
				m_ViewArray[i] = GetNextView(pos);
			
			m_nCurrentView = 0;
		}

		if(++m_nCurrentView > (ARRAY_SIZE(m_ViewArray)-1))
			m_nCurrentView = 0;

		int nCurrentView = m_nCurrentView;
		
		CView* pView;
		CView* pActiveView = pMainFrame->GetActiveView();
		if(pActiveView)
		{
			POSITION pos = GetFirstViewPosition();
			for(int i = 0; pos; i++)
			{
				pView = GetNextView(pos);
				if(pView==pActiveView)
				{
					nCurrentView = i;
					if(++nCurrentView > (ARRAY_SIZE(m_ViewArray)-1))
						nCurrentView = 0;
					break;
				}
			}
		}

		pMainFrame->SetActiveView(m_ViewArray[nCurrentView],TRUE);
	}
}

/*
	RouteCmdToAllViews()
*/
BOOL CWallBrowserDoc::RouteCmdToAllViews(CView* pView,UINT nID,int nCode,void* pExtra,AFX_CMDHANDLERINFO* pHandlerInfo)
{
	POSITION pos = GetFirstViewPosition();

	while(pos!=NULL)
	{
		CView* pNextView = GetNextView(pos);
		if(pNextView!=pView)
		{
			// cast CView upward to CCmdTarget because OnCmdMsg is
			// erroneously declared protected in CView
			if(((CCmdTarget*)pNextView)->OnCmdMsg(nID,nCode,pExtra,pHandlerInfo))
				return(TRUE);
		}
	}

	return(FALSE);
}
