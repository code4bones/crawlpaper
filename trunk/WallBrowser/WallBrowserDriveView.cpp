/*
	WallBrowserDriveView.cpp
	Classe per la vista per l'albero delle directory.
	La struttura basica per lo splitter delle directory e' stata ripresa e modificata dall'esempio presente in "Programmare Windows con MFC" di Prosise.
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
#include <ctype.h>
#include "strings.h"
#include "window.h"
#include "win32api.h"
#include <afxcview.h>
#include "CImageFactory.h"
#include "CWinAppEx.h"
#include "WallBrowserMessages.h"
#include "WallBrowserDoc.h"
#include "WallBrowserDriveView.h"
#include "WallBrowserResource.h"
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

IMPLEMENT_DYNCREATE(CWallBrowserDriveView,CTreeView)

BEGIN_MESSAGE_MAP(CWallBrowserDriveView,CTreeView)
	ON_WM_CREATE()
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_WM_RBUTTONDOWN()
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING,OnItemExpanding)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED,OnSelChanged)
	ON_COMMAND(IDM_DIRPOPUP_DELETE,OnDeleteFile)
END_MESSAGE_MAP()

/*
	CWallBrowserDriveView()
*/
CWallBrowserDriveView::CWallBrowserDriveView()
{
	m_hPrevItem = NULL;
	memset(m_szPathName,'\0',sizeof(m_szPathName));
	m_pMainFrameStatusBar = NULL;
	m_bDeleting = FALSE;
	CWinAppEx* pWinAppEx = (CWinAppEx*)AfxGetApp();
	if(pWinAppEx)
		strcpyn(m_szLastBrowseDir,pWinAppEx->m_Config.GetString(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_LASTBROWSEDIR_KEY),sizeof(m_szLastBrowseDir));
}

/*
	~CWallBrowserDriveView()
*/
CWallBrowserDriveView::~CWallBrowserDriveView()
{
	// elimina la lista delle directory
	m_DriveInfoList.DeleteAll();
	
	// aggiorna la configurazione con la directory corrente
	CWinAppEx* pWinAppEx = (CWinAppEx*)AfxGetApp();
	if(pWinAppEx)
	{
		pWinAppEx->m_Config.UpdateString(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_LASTBROWSEDIR_KEY,m_szLastBrowseDir);
		pWinAppEx->m_Config.SaveKey(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_LASTBROWSEDIR_KEY);
	}
}

/*
	PreCreateWindow()
*/
BOOL CWallBrowserDriveView::PreCreateWindow(CREATESTRUCT& cs)
{
	// classe base
	if(!CTreeView::PreCreateWindow(cs))
		return(FALSE);

	cs.style |= TVS_HASLINES|TVS_LINESATROOT|TVS_HASBUTTONS|TVS_SHOWSELALWAYS;
	
	return(TRUE);
}

/*
	OnCreate()
*/
int CWallBrowserDriveView::OnCreate(LPCREATESTRUCT lpcs)
{
	// classe base, crea la vista
	if(CTreeView::OnCreate(lpcs)==-1)
		return(-1);

	// associa l'array per le immagini dell'albero
//	if(m_ImageList.Create(IDR_DRIVEIMAGES,16,1,RGB(255,0,255)))
//		GetTreeCtrl().SetImageList(&m_ImageList,TVSIL_NORMAL);
	if(m_ImageList.Create(16,16,ILC_COLOR8,0,1))
	{
		m_ImageList.Add((HICON)::LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICON_HARDDISK),IMAGE_ICON,16,16,LR_DEFAULTCOLOR));
		m_ImageList.Add((HICON)::LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICON_NETDRIVE),IMAGE_ICON,16,16,LR_DEFAULTCOLOR));
		m_ImageList.Add((HICON)::LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICON_FLOPPYDISK),IMAGE_ICON,16,16,LR_DEFAULTCOLOR));
		m_ImageList.Add((HICON)::LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICON_CDROM),IMAGE_ICON,16,16,LR_DEFAULTCOLOR));
		m_ImageList.Add((HICON)::LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICON_FOLDERCLOSED),IMAGE_ICON,16,16,LR_DEFAULTCOLOR));
		m_ImageList.Add((HICON)::LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICON_FOLDEROPEN),IMAGE_ICON,16,16,LR_DEFAULTCOLOR));
		GetTreeCtrl().SetImageList(&m_ImageList,TVSIL_NORMAL);
	}
	
	// carica l'albero con le directories
	LoadTree();

	// ricava l'ultima directory visitata
	// notare che l'espansione dell'albero, dato che puo' generare il caricamento dell'immagine se la directory non e' vuota, deve
	// avvenire nella OnInitialUpdate() e non qui, dato che il frame, a questo punto, ancora non e' stato creato
	// LoadTree() deve espandere il drive per permettere a ExpandTree() di effettuare la ricerca, pero' dato che LoadTree() cambia
	// (imposta) la directory corrente all'espandere il drive, deve ricaricare il valore relativo all'ultima directory visitata
	CWinAppEx* pWinAppEx = (CWinAppEx*)AfxGetApp();
	if(pWinAppEx)
		strcpyn(m_szLastBrowseDir,pWinAppEx->m_Config.GetString(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_LASTBROWSEDIR_KEY),sizeof(m_szLastBrowseDir));

	// se viene passato come argomento da linea di comando (-d"..."), sovrascrive il valore relativo all'ultima directory visitata
	char szCommandLine[512];
	strcpyn(szCommandLine,GetCommandLine(),sizeof(szCommandLine));
	char* p = stristr(szCommandLine,"/d");
	if(!p)
		p = stristr(szCommandLine,"-d");
	if(p)
	{
		char delim = ' ';
		p += 2;
		if(*p=='"')
		{
			p++;
			delim = '"';
		}
		int i = 0;
		for(;*p && *p!=delim; i++)
			m_szLastBrowseDir[i] = *p++;
		m_szLastBrowseDir[i] = '\0';
	}

	// lasciare commentato - vedi le note sopra
	//ExpandTree(m_szLastBrowseDir);

	return(0);
}

/*
	OnInitialUpdate()
*/
void CWallBrowserDriveView::OnInitialUpdate(void)
{
	// classe base
	CView::OnInitialUpdate();

	// resetta (tutti) gli indicatori della barra di stato
	CWnd* pWnd = AfxGetMainWnd();
	if(pWnd)
	{
		m_pMainFrameStatusBar = (CStatusBar*)pWnd->SendMessage(WM_GET_STATUSBAR,0,0L);
		if(m_pMainFrameStatusBar)
		{			
			m_pMainFrameStatusBar->SetPaneText(ID_INDICATOR_FILENAME_ID,"");
			m_pMainFrameStatusBar->SetPaneText(ID_INDICATOR_FILES_ID,"");
			m_pMainFrameStatusBar->SetPaneText(ID_INDICATOR_INFO_ID,"");
			m_pMainFrameStatusBar->SetPaneText(ID_INDICATOR_LIBRARY_ID,"");
			m_pMainFrameStatusBar->SetPaneText(ID_INDICATOR_MEM_ID,"");
			m_pMainFrameStatusBar->SetPaneText(ID_INDICATOR_PICTURES_ID,"");
			m_pMainFrameStatusBar->SetPaneText(ID_INDICATOR_RATIO_ID,"");
			m_pMainFrameStatusBar->SetPaneText(ID_INDICATOR_ZOOM_ID,"");
		}
	}

	// espande l'albero posizionando sull'ultima directory visitata
	ExpandTree(m_szLastBrowseDir);
	TRACE("%s(%d): tree expanded\n",__FILE__,__LINE__);
	
	// informa le viste che ha finito di espandere l'albero
	CWallBrowserDoc* pDoc = (CWallBrowserDoc*)GetDocument();
	if(pDoc)
	{
		TRACE("%s(%d): sending ONUPDATE_FLAG_TREEEXPANDED\n",__FILE__,__LINE__);
		TRACE("%s(%d): file:%s,pathname:%s\n",__FILE__,__LINE__,pDoc->GetFileName(),pDoc->GetPathName());
		pDoc->UpdateAllViews(NULL,ONUPDATE_FLAG_TREEEXPANDED);
	}
}

/*
	OnUpdate()
*/
void CWallBrowserDriveView::OnUpdate(CView* pSender,LPARAM lHint,CObject* pHint)
{
	BOOL bHint = FALSE;

	if((lHint & ONUPDATE_FLAG_DIRCHANGED) || (lHint & ONUPDATE_FLAG_DEFAULTLIBRARY))
	{
		TRACE("%s(%d): receiving ONUPDATE_FLAG_DIRCHANGED || ONUPDATE_FLAG_DEFAULTLIBRARY\n",__FILE__,__LINE__);
		
		// resetta gli indicatori della barra di stato
		if(m_pMainFrameStatusBar)
		{
			m_pMainFrameStatusBar->SetPaneText(ID_INDICATOR_FILENAME_ID,"");
			//m_pMainFrameStatusBar->SetPaneText(ID_INDICATOR_FILES_ID,"");
			m_pMainFrameStatusBar->SetPaneText(ID_INDICATOR_INFO_ID,"");
			m_pMainFrameStatusBar->SetPaneText(ID_INDICATOR_LIBRARY_ID,"");
			m_pMainFrameStatusBar->SetPaneText(ID_INDICATOR_MEM_ID,"");
			//m_pMainFrameStatusBar->SetPaneText(ID_INDICATOR_PICTURES_ID,"");
			m_pMainFrameStatusBar->SetPaneText(ID_INDICATOR_RATIO_ID,"");
			m_pMainFrameStatusBar->SetPaneText(ID_INDICATOR_ZOOM_ID,"");
		}

		bHint = TRUE;
	}
	
	// classe base
	if(!bHint)
		CView::OnUpdate(pSender,lHint,pHint);
}

/*
	OnChar()
*/
void CWallBrowserDriveView::OnChar(UINT nChar,UINT nRepCnt,UINT nFlags)
{
	// riflette i tasti per la vista
	switch(nChar)
	{
		case '+':
		{
			CWnd* pWnd = AfxGetMainWnd();
			if(pWnd)
				pWnd->SendMessage(WM_COMMAND,MAKELONG(ID_VIEW_ZOOMIN,0),0L);
			return;
		}
		case '-':
		{
			CWnd* pWnd = AfxGetMainWnd();
			if(pWnd)
				pWnd->SendMessage(WM_COMMAND,MAKELONG(ID_VIEW_ZOOMOUT,0),0L);
			return;
		}
		case '=':
		{
			CWnd* pWnd = AfxGetMainWnd();
			if(pWnd)
				pWnd->SendMessage(WM_COMMAND,MAKELONG(ID_VIEW_ZOOMRESET,0),0L);
			return;
		}
	}
	
	// classe base
	CView::OnChar(nChar,nRepCnt,nFlags);
}

/*
	OnKeyDown()
*/
void CWallBrowserDriveView::OnKeyDown(UINT nChar,UINT nRepCnt,UINT nFlags)
{
	switch(nChar)
	{
		// redo
		// lo riflette per la vista
		case 'y':
		case 'Y':
		{
			if(::GetKeyState(VK_CONTROL) < 0)
			{
				CWnd* pWnd = AfxGetMainWnd();
				if(pWnd)
					pWnd->SendMessage(WM_COMMAND,MAKELONG(ID_EDIT_REDO,0),0L);
				return;
			}
			break;
		}

		// next picture
		// lo riflette per la vista
		case VK_RIGHT:
		{
			if(::GetKeyState(VK_SHIFT) < 0)
			{
				CWnd* pWnd = AfxGetMainWnd();
				if(pWnd)
					pWnd->SendMessage(WM_COMMAND,MAKELONG(ID_VIEW_NEXT_PICTURE,0),0L);
				return;
			}
			break;
		}

		// previous picture
		// lo riflette per la vista
		case VK_LEFT:
		{
			if(::GetKeyState(VK_SHIFT) < 0)
			{
				CWnd* pWnd = AfxGetMainWnd();
				if(pWnd)
					pWnd->SendMessage(WM_COMMAND,MAKELONG(ID_VIEW_PREVIOUS_PICTURE,0),0L);
				return;
			}
			break;
		}

		// eliminazione della directory
		case VK_DELETE:
			if(::GetKeyState(VK_CONTROL) < 0)
			{	
				OnDeleteFile();
				return;
			}
			break;
		
		// passa alla vista seguente
		case VK_TAB:
		{	
			CWallBrowserDoc* pDoc = (CWallBrowserDoc*)GetDocument();
			if(pDoc)
				pDoc->GotoNextView();
			break;
		}
	}

	// classe base
	CTreeView::OnKeyDown(nChar,nRepCnt,nFlags);
}

/*
	OnRButtonDown()
*/
void CWallBrowserDriveView::OnRButtonDown(UINT /*nFlags*/,CPoint point)
{
	UINT uFlags = 0;
	HTREEITEM htItem = GetTreeCtrl().HitTest(point,&uFlags);

	// se e' stato cliccato un elemento, visualizza il menu popup
	if((htItem!=NULL) && (uFlags & TVHT_ONITEM)) 
	{
		// lo seleziona
		m_hPrevItem = htItem;
		GetTreeCtrl().Select(htItem,TVGN_CARET|TVGN_DROPHILITE);
		GetPathFromNode(htItem);

		// menu popup
		ClientToScreen(&point);
		PopupMenu(point);
	}
}

/*
	OnSelChanged()
*/
void CWallBrowserDriveView::OnSelChanged(NMHDR* pnmh,LRESULT* /*pResult*/)
{
	// quando cambia la selezione imposta l'ultima directory cliccata
	NM_TREEVIEW* pnmtv = (NM_TREEVIEW*)pnmh;
	char szPathName[_MAX_PATH+1];

	m_hPrevItem = pnmtv->itemNew.hItem;
	strcpy(szPathName,GetPathFromNode(pnmtv->itemNew.hItem));

	CWinAppEx* pWinAppEx = (CWinAppEx*)AfxGetApp();
	if(pWinAppEx)
		strcpyn(m_szLastBrowseDir,szPathName,sizeof(m_szLastBrowseDir));
	
	OnSelectionChanged(szPathName);
}

/*
	OnSelectionChanged()
*/
void CWallBrowserDriveView::OnSelectionChanged(LPCSTR lpcszPathName)
{
	// e' cambiata la selezione, informa il documento e aggiorna le viste
	CWallBrowserDoc* pDoc = (CWallBrowserDoc*)GetDocument();
	if(pDoc)
	{
		pDoc->SetPathName(lpcszPathName);

		if(!m_bDeleting)
		{
			TRACE("%s(%d): sending ONUPDATE_FLAG_DIRCHANGED\n",__FILE__,__LINE__);
			pDoc->UpdateAllViews(NULL,ONUPDATE_FLAG_DIRCHANGED);
		}
	}
}

/*
	OnItemExpanding()
*/
void CWallBrowserDriveView::OnItemExpanding(NMHDR* pnmh,LRESULT* pResult)
{
	CWaitCursor cursor;

	// ricava l'elemento corrente (cliccato)
	NM_TREEVIEW* pnmtv = (NM_TREEVIEW*)pnmh;
	HTREEITEM hItem = pnmtv->itemNew.hItem;
	char szPathName[_MAX_PATH+1];

	// ricava il pathname relativo
	*pResult = FALSE;
	m_hPrevItem = hItem;
	strcpy(szPathName,GetPathFromNode(hItem));

	// espande
	if(pnmtv->action==TVE_EXPAND)
	{
		DeleteFirstChild(hItem);
		if(AddDirectories(hItem,szPathName)==0)
			*pResult = TRUE;
	}
	// costringe
	else if(pnmtv->action==TVE_COLLAPSE)
	{
		m_bDeleting = TRUE;
		DeleteAllChildren(hItem);
		if(GetTreeCtrl().GetParentItem(hItem)==NULL)
			GetTreeCtrl().InsertItem("",ILI_CLOSED_FOLDER,ILI_CLOSED_FOLDER,hItem);
		else
			SetButtonState(hItem,szPathName);
		m_bDeleting = FALSE;
	}

	// lo seleziona e aggiorna 
	GetTreeCtrl().Select(hItem,TVGN_CARET|TVGN_DROPHILITE);
	strcpy(szPathName,GetPathFromNode(hItem));
	OnSelectionChanged(szPathName);
}

/*
	OnDeleteFile()
*/
void CWallBrowserDriveView::OnDeleteFile(void)
{
	CWallBrowserDoc* pDoc = (CWallBrowserDoc*)GetDocument();
	if(pDoc)
	{
		memset(m_szPathName,'\0',sizeof(m_szPathName));
		strcpyn(m_szPathName,pDoc->GetPathName(),sizeof(m_szPathName));

		if(m_szPathName[0]!='\0')
		{
			::RemoveBackslash(m_szPathName);

			BOOL bDelete = FALSE;
			BOOL bDeleteToRecycleBin = TRUE;
			BOOL bConfirmFileDelete = TRUE;
			CWinAppEx* pWinAppEx = (CWinAppEx*)AfxGetApp();
			if(pWinAppEx)
			{
				bDeleteToRecycleBin = pWinAppEx->m_Config.GetNumber(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_DELETEFILESTORECYCLEBIN_KEY);
				bConfirmFileDelete = pWinAppEx->m_Config.GetNumber(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_CONFIRMFILEDELETE_KEY);
			}

			if(bDeleteToRecycleBin)
			{
				bDelete = ::DeleteFileToRecycleBin(this->m_hWnd,m_szPathName,bConfirmFileDelete);
			}
			else
			{
				if(bConfirmFileDelete)
					bDelete = ::MessageBoxResourceEx(this->m_hWnd,MB_ICONQUESTION|MB_YESNO,WALLBROWSER_PROGRAM_NAME,IDS_QUESTION_DIRDELETE,m_szPathName)==IDYES;
				else
					bDelete = TRUE;
				if(bDelete)
					::DeleteFileToRecycleBin(this->m_hWnd,m_szPathName,FALSE,FALSE);
			}

			if(bDelete)
			{
				GetTreeCtrl().DeleteItem(m_hPrevItem);
				memset(m_szPathName,'\0',sizeof(m_szPathName));
				pDoc->ResetFileName();
			}
		}
	}
}

/*
	LoadTree()
*/
int CWallBrowserDriveView::LoadTree(void)
{
	CWaitCursor cursor;

	// azzera l'albero
	GetTreeCtrl().DeleteAllItems();
	m_DriveInfoList.DeleteAll();

	int nPos = 0;
	int nDrivesAdded = 0;
	CString strDrive = "?:\\";
	DWORD dwDriveList = ::GetLogicalDrives();

	// per ognuno dei drive presenti
	while(dwDriveList)
	{
		if(dwDriveList & 1)
		{
			strDrive.SetAt(0,(char)0x41 + nPos);
			
			// aggiunge all'albero i rami relativi al drive
			if(AddDriveNode(strDrive))
				nDrivesAdded++;
		}
	
		dwDriveList >>= 1;
		nPos++;
	}
	
	return(nDrivesAdded);
}

/*
	ExpandTree()
*/
void CWallBrowserDriveView::ExpandTree(LPCSTR lpcszPathName)
{
	char szPathName[_MAX_PATH+1];
	char szDrive[5];
	int i;

	// ricava il drive dal pathname
	strcpyn(szPathName,lpcszPathName,sizeof(szPathName));
	for(i = 0; szPathName[i] && i < 3; i++)
		szDrive[i] = szPathName[i];
	szDrive[i] = '\0';

	// ricava l'elemento (handle) relativo al drive
	HTREEITEM hItem = NULL;
	DRIVEINFO* driveinfo;
	ITERATOR iter;

	if((iter = m_DriveInfoList.First())!=(ITERATOR)NULL)
		while(iter!=(ITERATOR)NULL)
		{
			driveinfo = (DRIVEINFO*)iter->data;
			if(driveinfo)
				if(strcmp(szDrive,driveinfo->szDrive)==0)
				{
					hItem = driveinfo->hItem;
					break;
				}

			iter = m_DriveInfoList.Next(iter);
		}

	// espande a partire dal drive (handle) specificato
	if(hItem)
	{
		char* pToken;

		for(i = 0,pToken = strtok(szPathName,"\\"); pToken!=NULL && hItem!=NULL; i++)
		{
			if(pToken[1]==':')
			{
				pToken = strtok(NULL,"\\");
				continue;
			}

			if((hItem = FindItem(hItem,pToken))!=NULL)
			{
				GetTreeCtrl().SelectItem(hItem);
				GetTreeCtrl().Expand(hItem,TVE_EXPAND);
			}

			pToken = strtok(NULL,"\\");
		}
	}
}

/*
	AddDriveNode()
*/
BOOL CWallBrowserDriveView::AddDriveNode(CString& strDrive)
{
	static BOOL bFirst = TRUE;
	HTREEITEM hItem = NULL;
	char szItemText[_MAX_PATH+1] = {0};
	UINT nType = ::GetDriveType((LPCTSTR)strDrive);
	UINT nErrorMode = ::SetErrorMode(SEM_FAILCRITICALERRORS);
	char szVolumeName[_MAX_PATH+1] = {0};
	DWORD dwVolumeSN = 0L;
	DWORD dwVolumeFlags = 0L;
	char szVolumeFileSystem[_MAX_PATH+1];
	DWORD dwMaxFN = 0L;
	DRIVEINFO* driveinfo;

	// ricava le info sul drive
	::GetVolumeInformation(	strDrive,
						szVolumeName,
						sizeof(szVolumeName)-1,
						&dwVolumeSN,
						&dwMaxFN,
						&dwVolumeFlags,
						szVolumeFileSystem,
						sizeof(szVolumeFileSystem)-1
						);

	::SetErrorMode(nErrorMode);

	strlwr(szVolumeName);
	szVolumeName[0] = (char)toupper(szVolumeName[0]);

	// inserisce l'icona relativa al drive nell'albero
	switch(nType)
	{
		case DRIVE_REMOVABLE:
			_snprintf(szItemText,sizeof(szItemText)-1,"Floppy (%c:)",strDrive[0]);
			hItem = GetTreeCtrl().InsertItem(szItemText,ILI_FLOPPY,ILI_FLOPPY);
			GetTreeCtrl().InsertItem("",ILI_CLOSED_FOLDER,ILI_CLOSED_FOLDER,hItem);
			break;

		case DRIVE_FIXED:
			_snprintf(szItemText,sizeof(szItemText)-1,"%s (%c:)",szVolumeName,strDrive[0]);
			hItem = GetTreeCtrl().InsertItem(szItemText,ILI_HARD_DISK,ILI_HARD_DISK);
			SetButtonState(hItem,strDrive);
			break;

		case DRIVE_REMOTE:
			_snprintf(szItemText,sizeof(szItemText)-1,"%s (%c:)",szVolumeName,strDrive[0]);
			hItem = GetTreeCtrl().InsertItem(szItemText,ILI_NET_DRIVE,ILI_NET_DRIVE);
			SetButtonState(hItem,strDrive);
			break;

		case DRIVE_CDROM:
			_snprintf(szItemText,sizeof(szItemText)-1,"%s (%c:)",szVolumeName,strDrive[0]);
			hItem = GetTreeCtrl().InsertItem(szItemText,ILI_CD_ROM,ILI_CD_ROM);
			GetTreeCtrl().InsertItem("",ILI_CLOSED_FOLDER,ILI_CLOSED_FOLDER,hItem);
			break;

		case DRIVE_RAMDISK:
			_snprintf(szItemText,sizeof(szItemText)-1,"%s (%c:)",szVolumeName,strDrive[0]);
			hItem = GetTreeCtrl().InsertItem(szItemText,ILI_RAM_DRIVE,ILI_RAM_DRIVE);
			SetButtonState(hItem,strDrive);
			break;

		default:
			return(FALSE);
	}

	// se coincide con il drive dell'ultima directory aperta, lo espande
	if(bFirst && toupper(m_szLastBrowseDir[0])==toupper(strDrive.GetAt(0)))
	{
		GetTreeCtrl().SelectItem(hItem);
		GetTreeCtrl().Expand(hItem,TVE_EXPAND);
		bFirst = FALSE;
	}

	driveinfo = new DRIVEINFO;
	if(driveinfo)
	{
		driveinfo->hItem = hItem;
		strcpyn(driveinfo->szDrive,strDrive,MAX_DRIVE_INFO);
		m_DriveInfoList.Add(driveinfo);
	}

	return(TRUE);
}

/*
	AddDirectories()
*/
int CWallBrowserDriveView::AddDirectories(HTREEITEM hItem,LPCSTR lpcszPathName)
{
	HANDLE hFind;
	WIN32_FIND_DATA fd;
	HTREEITEM hNewItem;

	int nCount = 0;
	CString strPath = lpcszPathName;
	if(strPath.Right(1)!="\\")
		strPath += "\\";
	strPath += "*.*";

	CWallBrowserDoc* pDoc = (CWallBrowserDoc*)GetDocument();
	if(pDoc)
		pDoc->SetTitle("reading directory...");

	// cerca e aggiunge le (sub)directory relative al pathname
	if((hFind = ::FindFirstFile((LPCTSTR)strPath,&fd))==INVALID_HANDLE_VALUE)
	{
		if(GetTreeCtrl().GetParentItem(hItem)==NULL)
			GetTreeCtrl().InsertItem("",ILI_CLOSED_FOLDER,ILI_CLOSED_FOLDER,hItem,TVI_SORT);
		return(0);
	}

	do {
		if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			CString strCmp = (LPCTSTR)&fd.cFileName;
			if((strCmp!=".") && (strCmp!=".."))
			{
				hNewItem = GetTreeCtrl().InsertItem((LPCTSTR)&fd.cFileName,ILI_CLOSED_FOLDER,ILI_OPEN_FOLDER,hItem,TVI_SORT);

				CString strNewPath = lpcszPathName;
				if(strNewPath.Right(1)!="\\")
					strNewPath += "\\";
				strNewPath += (LPCTSTR)&fd.cFileName;
				SetButtonState(hNewItem,strNewPath);
				nCount++;
			}
		}
	} while(::FindNextFile(hFind,&fd));

	::FindClose(hFind);

	if(pDoc)
		pDoc->SetTitle(lpcszPathName);

	return(nCount);
}

/*
	FindItem()
*/
HTREEITEM CWallBrowserDriveView::FindItem(HTREEITEM hParent,LPCSTR lpcszLabel)
{
	HTREEITEM hItem = NULL;
	CString strLabel;

	if((hItem = GetTreeCtrl().GetChildItem(hParent))!=NULL)
	{
		strLabel = GetTreeCtrl().GetItemText(hItem);
		if(strcmp(lpcszLabel,strLabel)==0)
			return(hItem);
	}

	do {
		HTREEITEM hNextItem = GetTreeCtrl().GetNextSiblingItem(hItem);
		strLabel = GetTreeCtrl().GetItemText(hNextItem);
		
		if(strcmp(lpcszLabel,strLabel)==0)
			return(hNextItem);
		
		hItem = hNextItem;
	}
	while(hItem!=NULL);

	return(hItem);
}

/*
	DeleteFirstChild()
*/
void CWallBrowserDriveView::DeleteFirstChild(HTREEITEM hParent)
{
	HTREEITEM hItem;
    
	if((hItem = GetTreeCtrl().GetChildItem(hParent))!=NULL)
		GetTreeCtrl().DeleteItem(hItem);
}

/*
	DeleteAllChildren()
*/
void CWallBrowserDriveView::DeleteAllChildren(HTREEITEM hParent)
{
	HTREEITEM hItem;
	if((hItem = GetTreeCtrl().GetChildItem(hParent))==NULL)
		return;

	do {
		HTREEITEM hNextItem = GetTreeCtrl().GetNextSiblingItem(hItem);
		GetTreeCtrl().DeleteItem(hItem);
		hItem = hNextItem;
	} while(hItem!=NULL);
}

/*
	SetButtonState()
*/
BOOL CWallBrowserDriveView::SetButtonState(HTREEITEM hItem,LPCSTR lpcszPathName)
{
	HANDLE hFind;
	WIN32_FIND_DATA fd;
	BOOL bResult = FALSE;

	CString strPath = lpcszPathName;
	if(strPath.Right(1)!= "\\")
		strPath += "\\";
	strPath += "*.*";

	if((hFind = ::FindFirstFile((LPCTSTR)strPath,&fd))==INVALID_HANDLE_VALUE)
		return(bResult);

	do {
		if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			CString strCmp = (LPCTSTR)&fd.cFileName;
			if((strCmp!=".") && (strCmp!=".."))
			{
				GetTreeCtrl().InsertItem("",ILI_CLOSED_FOLDER,ILI_CLOSED_FOLDER,hItem);
				bResult = TRUE;
				break;
			}
		}
	} while(::FindNextFile(hFind,&fd));

	::FindClose(hFind);

	return(bResult);
}

/*
	GetPathFromNode()
*/
LPCSTR CWallBrowserDriveView::GetPathFromNode(HTREEITEM hItem)
{
/*	char buffer[256];
	TV_ITEM tv_item;
	tv_item.mask = TVIF_HANDLE|TVIF_TEXT;
	tv_item.hItem = hItem;
	tv_item.pszText = buffer;
	tv_item.cchTextMax = sizeof(buffer)-1;
	GetTreeCtrl().GetItem(&tv_item);
*/
	CString strResult = GetTreeCtrl().GetItemText(hItem);

	char driveletter = 0;
	char* p = (char*)strchr(strResult,':');
	if(p)
	{
		driveletter = *(p-1);
		strResult.Format("%c:\\",driveletter);
	}

	HTREEITEM hParent;
	while((hParent = GetTreeCtrl().GetParentItem(hItem))!=NULL)
	{
		CString strPath = GetTreeCtrl().GetItemText(hParent);
		char* p = (char*)strchr(strPath,':');
		if(p)
		{
			driveletter = *(p-1);
			strPath.Format("%c:\\",driveletter);
		}

		if(strPath.Right(1)!="\\")
			strPath += "\\";

		if(strPath=="\\")
			strPath = "";

		strResult = strPath + strResult;
		hItem = hParent;
	}

	strcpy(m_szPathName,strResult);

	CWallBrowserDoc* pDoc = (CWallBrowserDoc*)GetDocument();
	if(pDoc)
	{
		pDoc->SetPathName(m_szPathName);
		pDoc->SetTitle(m_szPathName);
	}

	return(m_szPathName);
}

/*
	PopupMenu()
*/
void CWallBrowserDriveView::PopupMenu(CPoint& point)
{
	CMenu menu;
	if(menu.LoadMenu(IDM_DIRPOPUP))
	{
		CMenu* pSubMenu = menu.GetSubMenu(0);
		if(pSubMenu)
			pSubMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,point.x,point.y,this);
	}
}
