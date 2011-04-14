/*
	WallBrowserFileView.cpp
	Classe per la vista per la lista dei files.
	La struttura basica per lo splitter della lista dei files e' stata ripresa e modificata dall'esempio presente in "Programmare Windows con MFC" di Prosise.
	Luca Piergentili, 02/08/01
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
#include "CDirDialog.h"
#include "CListViewEx.h"
#include "CImageFactory.h"
#include "CWinAppEx.h"
#include "CRegistry.h"
#include "WallPaperVersion.h"
#include "WallBrowserVersion.h"
#include "WallBrowserMessages.h"
#include "WallBrowserDoc.h"
#include "WallBrowserFileView.h"
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

IMPLEMENT_DYNCREATE(CWallBrowserFileView,CListViewEx)

BEGIN_MESSAGE_MAP(CWallBrowserFileView,CListViewEx)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()

	ON_NOTIFY_REFLECT(LVN_GETDISPINFO,OnGetDispInfo)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK,OnColumnClick)

	ON_COMMAND(ID_FILE_MOVE,OnMoveFile)
	ON_COMMAND(ID_FILE_DELETE,OnDeleteFile)
	ON_COMMAND(ID_VIEW_PREVIOUS_PICTURE,OnPrevious)
	ON_COMMAND(ID_VIEW_NEXT_PICTURE,OnNext)

	ON_COMMAND(IDM_FILEPOPUP_MOVE,OnMoveFile)
	ON_COMMAND(IDM_FILEPOPUP_DELETE,OnDeleteFile)
	ON_COMMAND(IDM_FILEPOPUP_COPY,OnCopy)
	ON_COMMAND(IDM_FILEPOPUP_PASTE,OnPaste)
END_MESSAGE_MAP()

static int g_bFileNameAscending = TRUE;
static int g_bSizeAscending = FALSE;
static int g_bModifiedAscending = FALSE;
static int g_nSortOrder = 0;

/*
	CWallBrowserFileView()
*/
CWallBrowserFileView::CWallBrowserFileView()
{
	m_nFiles = 0;
	m_nPictures = 0;
	memset(m_szFileName,'\0',sizeof(m_szFileName));
	m_pMainFrameStatusBar = NULL;
}

/*
	~CWallBrowserFileView()
*/
CWallBrowserFileView::~CWallBrowserFileView()
{
}

/*
	OnCreate()
*/
int CWallBrowserFileView::OnCreate(LPCREATESTRUCT lpcs)
{
	if(CListViewEx::OnCreate(lpcs)==-1)
		return(-1);

	if(m_ImageList.Create(IDR_SMALLDOC,16,1,RGB(255,0,255)))
	{
		CWallBrowserDoc* pDoc = (CWallBrowserDoc*)GetDocument();
		if(pDoc)
		{
			CImage* pImage = pDoc->GetImage();
			if(pImage)
			{
				LPIMAGETYPE p;
				ICONINDEX* iconindex;
				CRegistry registry;
				
				for(int i = 0; (p = pImage->EnumReadableImageFormats())!=(LPIMAGETYPE)NULL; i++)
				{
					iconindex = new ICONINDEX;
					if(iconindex)
					{
						strcpyn(iconindex->ext,p->ext,_MAX_EXT+1);
						iconindex->icon = registry.GetSafeIconForRegisteredFileType(iconindex->ext);
						iconindex->index = m_ImageList.Add(iconindex->icon);
						m_IconList.Add(iconindex);
					}
				}
			}
		}
		
		GetListCtrl().SetImageList(&m_ImageList,LVSIL_SMALL);
	}
	else
		return(-1);

	GetListCtrl().InsertColumn(0,"Name",LVCFMT_LEFT,200);
//	AddHeaderToolTip(0,szName);
	GetListCtrl().InsertColumn(1,"Size",LVCFMT_RIGHT,50);
//	AddHeaderToolTip(1,"File Size");
	GetListCtrl().InsertColumn(2,"Modified",LVCFMT_LEFT,150);
//	AddHeaderToolTip(2,"Last Access Time");

	return(0);
}

/*
	OnDestroy()
*/
void CWallBrowserFileView::OnDestroy(void)
{
	// deve eliminare gli elementi della lista
    FreeItemMemory();

    CListViewEx::OnDestroy();
}

/*
	OnInitialUpdate()
*/
void CWallBrowserFileView::OnInitialUpdate(void)
{
	CWnd* pWnd = AfxGetMainWnd();
	if(pWnd)
	{
		m_pMainFrameStatusBar = (CStatusBar*)pWnd->SendMessage(WM_GET_STATUSBAR,0,0L);
		pWnd->SendMessage(WM_VIEW_PREVIOUS_PICTURE,m_nPictures > 0 ? TRUE : FALSE,0L);
		pWnd->SendMessage(WM_VIEW_NEXT_PICTURE,m_nPictures > 0 ? TRUE : FALSE,0L);
	}

	CListViewEx::OnInitialUpdate();
}

/*
	OnUpdate()
*/
void CWallBrowserFileView::OnUpdate(CView* pView,LPARAM lHint,CObject* pHint)
{
	BOOL bHint = FALSE;
	static BOOL bTreeExpanded = FALSE;

	if(lHint & ONUPDATE_FLAG_TREEEXPANDED)
	{
		TRACE("%s(%d): receiving ONUPDATE_FLAG_TREEEXPANDED\n",__FILE__,__LINE__);
		bTreeExpanded = TRUE;

		CWallBrowserDoc* pDoc = (CWallBrowserDoc*)GetDocument();
		if(pDoc)
		{
			TRACE("%s(%d): sending ONUPDATE_FLAG_TREEEXPANDEDEX\n",__FILE__,__LINE__);
			pDoc->UpdateAllViews(NULL,ONUPDATE_FLAG_TREEEXPANDEDEX);
		}
	}

	if(!bTreeExpanded)
		goto done;

	if(lHint & ONUPDATE_FLAG_DIRSELECTED)
	{
		TRACE("%s(%d): receiving ONUPDATE_FLAG_DIRSELECTED\n",__FILE__,__LINE__);
		
		// ri-selezionando la directory, riposiziona sul primo file della lista
		if(m_nPictures > 0)
			OnSelectItem(0);
		
		bHint = TRUE;
	}
	if((lHint & ONUPDATE_FLAG_TREEEXPANDED) || (lHint & ONUPDATE_FLAG_DIRCHANGED) || (lHint & ONUPDATE_FLAG_DIRUPDATED))
	{
		TRACE("%s(%d): receiving ONUPDATE_FLAG_TREEEXPANDED || ONUPDATE_FLAG_DIRCHANGED || ONUPDATE_FLAG_DIRUPDATED\n",__FILE__,__LINE__);
		
		// salva l'elemento corrente della lista per il confronto nel caso in cui venga aggiornata la directory
		char szItem[_MAX_PATH+1];
		int nItem = GetCurrentItem(szItem,sizeof(szItem));

		SetRedraw(FALSE);
		FreeItemMemory();
		GetListCtrl().DeleteAllItems();
		SetCurrentItem(0);
		SetRedraw(TRUE);

		CWallBrowserDoc* pDoc = (CWallBrowserDoc*)GetDocument();
		if(pDoc)
		{
			pDoc->SetPictureFlag(FALSE);
			pDoc->ResetFileName();
			
			if(m_pMainFrameStatusBar)
				m_pMainFrameStatusBar->SetPaneText(ID_INDICATOR_FILENAME_ID,"reading directory...");

			LoadList(pDoc->GetPathName());
			
			if(m_pMainFrameStatusBar)
				m_pMainFrameStatusBar->SetPaneText(ID_INDICATOR_FILENAME_ID,"");
		}

		CWnd* pWnd = AfxGetMainWnd();
		if(pWnd)
		{
			pWnd->SendMessage(WM_VIEW_PREVIOUS_PICTURE,m_nPictures > 0 ? TRUE : FALSE,0L);
			pWnd->SendMessage(WM_VIEW_NEXT_PICTURE,m_nPictures > 0 ? TRUE : FALSE,0L);
		}
		
		if(m_nPictures > 0)
		{
			if(lHint & ONUPDATE_FLAG_DIRUPDATED)
			{
				COnUpdateInfo* pOnUpdateInfo = (COnUpdateInfo*)pHint;
				strcpyn(szItem,pOnUpdateInfo->GetInfo(),sizeof(szItem));

				LVFINDINFO lv = {0};
				lv.flags = LVFI_STRING;
				lv.psz = szItem;
				if((nItem = GetListCtrl().FindItem(&lv,-1)) < 0)
					nItem = 0;
			}
			else
				nItem = 0;

			OnSelectItem(nItem);
		}
		else
			memset(m_szFileName,'\0',sizeof(m_szFileName));

		bHint = TRUE;
	}
	if(lHint & ONUPDATE_FLAG_DEFAULTLIBRARY)
	{
		TRACE("%s(%d): receiving ONUPDATE_FLAG_DEFAULTLIBRARY\n",__FILE__,__LINE__);
		
		char szItem[_MAX_PATH+1];
		int nItem = GetCurrentItem(szItem,sizeof(szItem));
		int nTotItems = GetListCtrl().GetItemCount();

		SetRedraw(FALSE);
		FreeItemMemory();
		GetListCtrl().DeleteAllItems();
		SetCurrentItem(0);
		SetRedraw(TRUE);

		CWallBrowserDoc* pDoc = (CWallBrowserDoc*)GetDocument();
		if(pDoc)
		{
			if(m_pMainFrameStatusBar)
				m_pMainFrameStatusBar->SetPaneText(ID_INDICATOR_FILENAME_ID,"reading directory...");

			LoadList(pDoc->GetPathName());
			
			if(m_pMainFrameStatusBar)
				m_pMainFrameStatusBar->SetPaneText(ID_INDICATOR_FILENAME_ID,"");
		}

		// per gestire il cambio di libreria senza ridisegnare l'immagine se rimane la stessa
		BOOL bChanged = FALSE;

		if(nTotItems==GetListCtrl().GetItemCount())
		{
			strcpyn(m_szFileName,szItem,sizeof(m_szFileName));
			{
				LV_ITEM lvitem;
				lvitem.mask = LVIF_TEXT;
				lvitem.iItem = nItem;
				lvitem.iSubItem = 0;
				lvitem.pszText = szItem;
				lvitem.cchTextMax = sizeof(szItem)-1;
				GetListCtrl().GetItem(&lvitem);
			}
			if(strcmp(m_szFileName,szItem)!=0)
			{
				memset(m_szFileName,'\0',sizeof(m_szFileName));
				bChanged = TRUE;
			}

			OnSelectItem(nItem);
		}
		else
		{
			bChanged = TRUE;

			nTotItems = GetListCtrl().GetItemCount();
			
			if(nItem > nTotItems-1)
				nItem = nTotItems-1;
			if(nItem >= 0)
				OnSelectItem(nItem);
		}

		CWnd* pWnd = AfxGetMainWnd();
		if(pWnd)
		{
			pWnd->SendMessage(WM_VIEW_PREVIOUS_PICTURE,nTotItems > 0 ? TRUE : FALSE,0L);
			pWnd->SendMessage(WM_VIEW_NEXT_PICTURE,nTotItems > 0 ? TRUE : FALSE,0L);
		}
		
		bHint = TRUE;
	}

	if(m_pMainFrameStatusBar)
	{
		char szStatus[_MAX_PATH+1];
		_snprintf(szStatus,
				sizeof(szStatus)-1,
				"%d file(s)",
				m_nFiles
				);
		m_pMainFrameStatusBar->SetPaneText(ID_INDICATOR_FILES_ID,szStatus);

		_snprintf(szStatus,
				sizeof(szStatus)-1,
				"%d picture(s)",
				m_nPictures
				);
		m_pMainFrameStatusBar->SetPaneText(ID_INDICATOR_PICTURES_ID,szStatus);
	}

done:
	
	if(!bHint)
		CListViewEx::OnUpdate(pView,lHint,pHint);
}

/*
	OnChar()
*/
void CWallBrowserFileView::OnChar(UINT nChar,UINT nRepCnt,UINT nFlags)
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
void CWallBrowserFileView::OnKeyDown(UINT nChar,UINT nRepCnt,UINT nFlags)
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

		// spostamento del file
		case 'm':
		case 'M':
		{
			if(::GetKeyState(VK_CONTROL) < 0)
			{
				OnMoveFile();
				return;
			}
			break;
		}

		// eliminazione del file
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
	CListViewEx::OnKeyDown(nChar,nRepCnt,nFlags);
}

/*
	OnLButtonDown()
*/
void CWallBrowserFileView::OnLButtonDown(UINT nFlags,CPoint point)
{
	CListViewEx::OnLButtonDown(nFlags,point);
	OnSelectItem(point);
}

/*
	OnRButtonDown()
*/
void CWallBrowserFileView::OnRButtonDown(UINT nFlags,CPoint point)
{
	CListViewEx::OnRButtonDown(nFlags,point);
	if(OnSelectItem(point) >= 0)
		PopUpMenu(point);
}

/*
	OnGetDispInfo()
*/
void CWallBrowserFileView::OnGetDispInfo(NMHDR* pnmh,LRESULT* /*pResult*/)
{
	LV_DISPINFO* plvdi = (LV_DISPINFO*)pnmh;

	if(plvdi->item.mask & LVIF_TEXT)
	{
		ITEMINFO* pItem = (ITEMINFO*)plvdi->item.lParam;

		switch(plvdi->item.iSubItem)
		{
			// Name
			case 0:
				strcpyn(plvdi->item.pszText,(LPCTSTR)pItem->strFileName,plvdi->item.cchTextMax-1);
				break;

			// Size
			case 1:
				_snprintf(plvdi->item.pszText,
						plvdi->item.cchTextMax-1,
						"%u",
						pItem->nFileSizeLow
						);
				break;

			// Modified
			case 2:
				BOOL pm = FALSE;
				CTime time(pItem->ftLastWriteTime);
				int nHour = time.GetHour();
				if(nHour==0)
					nHour = 12;
				else if(nHour==12)
					pm = TRUE;
				else if(nHour > 12)
				{
					nHour -= 12;
					pm = TRUE;
				}
				
				_snprintf(plvdi->item.pszText,
						plvdi->item.cchTextMax-1,
						"%0.2d/%0.2d/%0.2d %0.2d:%0.2d %s",
						time.GetDay(),
						time.GetMonth(),
						time.GetYear() % 100,
						nHour,
						time.GetMinute(),
						pm ? "pm" : "am"
						);
				break;
		}
	}
}

/*
	OnColumnClick()
*/
void CWallBrowserFileView::OnColumnClick(NMHDR* pnmh,LRESULT* /*pResult*/)
{
	NM_LISTVIEW* pnmlv = (NM_LISTVIEW*)pnmh;
	
	switch(pnmlv->iSubItem)
	{
		case 0:
			g_bFileNameAscending = !g_bFileNameAscending;
			break;
		case 1:
			g_bSizeAscending = !g_bSizeAscending;
			break;
		case 2:
			g_bModifiedAscending = !g_bModifiedAscending;
			break;
	}

	g_nSortOrder = pnmlv->iSubItem;
	GetListCtrl().SortItems(CompareFunc,pnmlv->iSubItem);
}

/*
	OnSelectItem()
*/
int CWallBrowserFileView::OnSelectItem(int nItem)
{
	// controlla che l'elemento specificato sia valido
	if(nItem >= 0 && nItem < GetListCtrl().GetItemCount())
	{
		// ricava il contenuto dell'elemento corrente (nomefile)
		char szItem[_MAX_PATH+1] = {0};
		LV_ITEM lvitem;
		lvitem.mask = LVIF_TEXT;
		lvitem.iItem = nItem;
		lvitem.iSubItem = 0;
		lvitem.pszText = szItem;
		lvitem.cchTextMax = sizeof(szItem)-1;
		GetListCtrl().GetItem(&lvitem);

		// lo imposta come attivo
		SelectItem(nItem);

		CWallBrowserDoc* pDoc = (CWallBrowserDoc*)GetDocument();
		if(pDoc)
		{
			// bug: espandendo un ramo chiama piu' volte il gestore per cui all'ultimo passaggio
			// i nomi coincidono per cui non visualizza niente anche se la directory contiene delle
			// immagini
/*			if(strcmp(szItem,m_szFileName)==0)
			{
				// e' stato ri-selezionato lo stesso file
				TRACE("%s(%d): sending ONUPDATE_FLAG_FILESELECTED\n",__FILE__,__LINE__);
				pDoc->UpdateAllViews(NULL,ONUPDATE_FLAG_FILESELECTED);
			}
			else
*/			{
				// e' stato selezionato un nuovo file
				strcpyn(m_szFileName,szItem,sizeof(m_szFileName));
				pDoc->SetFileName(m_szFileName);
				TRACE("%s(%d): sending ONUPDATE_FLAG_FILECHANGED\n",__FILE__,__LINE__);
				pDoc->UpdateAllViews(NULL,ONUPDATE_FLAG_FILECHANGED);
			}
		}
	}

	return(nItem);
}

/*
	OnSelectItem()
*/
int CWallBrowserFileView::OnSelectItem(CPoint point)
{
	// ricava l'elemento corrente
	int nItem = GetListCtrl().HitTest(point);
	
	// controlla che sia valido
	if(nItem >= 0 && nItem < GetListCtrl().GetItemCount())
	{
		// ricava il contenuto dell'elemento corrente (nomefile)
		char szItem[_MAX_PATH+1] = {0};
		LV_ITEM lvitem;
		lvitem.mask = LVIF_TEXT;
		lvitem.iItem = nItem;
		lvitem.iSubItem = 0;
		lvitem.pszText = szItem;
		lvitem.cchTextMax = sizeof(szItem)-1;
		GetListCtrl().GetItem(&lvitem);

		// lo imposta come attivo
		SelectItem(nItem);

		CWallBrowserDoc* pDoc = (CWallBrowserDoc*)GetDocument();
		if(pDoc)
		{
			// bug: espandendo un ramo chiama piu' volte il gestore per cui all'ultimo passaggio
			// i nomi coincidono per cui non visualizza niente anche se la directory contiene delle
			// immagini
/*			if(strcmp(szItem,m_szFileName)==0)
			{
				// e' stato ri-selezionato lo stesso file
				TRACE("%s(%d): sending ONUPDATE_FLAG_FILESELECTED\n",__FILE__,__LINE__);
				pDoc->UpdateAllViews(NULL,ONUPDATE_FLAG_FILESELECTED);
			}
			else
*/			{
				// e' stato selezionato un nuovo file
				strcpyn(m_szFileName,szItem,sizeof(m_szFileName));
				pDoc->SetFileName(m_szFileName);
				TRACE("%s(%d): sending ONUPDATE_FLAG_FILECHANGED\n",__FILE__,__LINE__);
				pDoc->UpdateAllViews(NULL,ONUPDATE_FLAG_FILECHANGED);
			}
		}
	}

	return(nItem);
}

/*
	OnDeleteItem()
*/
int CWallBrowserFileView::OnDeleteItem(void)
{
	// ricava l'elemento corrente
	int nItem = GetCurrentItem();

	// controlla che sia valido
	if(nItem >= 0 && nItem < GetListCtrl().GetItemCount())
	{
		// 
		LV_ITEM lvitem = {0};
		lvitem.mask = LVIF_PARAM;
		lvitem.iItem = nItem;
		lvitem.iSubItem = 0;
		GetListCtrl().GetItem(&lvitem);
		ITEMINFO* pItem = (ITEMINFO*)lvitem.lParam;
		if(pItem)
			delete pItem;

		// lo elimina dalla lista
		GetListCtrl().DeleteItem(nItem);
		
		// imposta il nuovo elemento corrente
		if(nItem >= GetListCtrl().GetItemCount())
			nItem--;
		if(nItem < 0)
			nItem = 0;
		nItem = SelectItem(nItem);

		CWallBrowserDoc* pDoc = (CWallBrowserDoc*)GetDocument();
		if(pDoc)
		{
			memset(m_szFileName,'\0',sizeof(m_szFileName));
			pDoc->SetPictureFlag(FALSE);
			pDoc->ResetFileName();
			TRACE("%s(%d): sending ONUPDATE_FLAG_FILEDELETED\n",__FILE__,__LINE__);
 			pDoc->UpdateAllViews(NULL,ONUPDATE_FLAG_FILEDELETED);
		}
	}

	return(nItem);
}

/*
	OnMoveFile()
*/
void CWallBrowserFileView::OnMoveFile(void)
{
	CWinAppEx* pWinAppEx = (CWinAppEx*)AfxGetApp();
	CWallBrowserDoc* pDoc = (CWallBrowserDoc*)GetDocument();
	
	if(pWinAppEx && pDoc)
	{
		if(pDoc->HavePathName() && pDoc->HaveFileName())
		{
			char szFolder[_MAX_PATH+1] = {0};
			char szDestinationFile[_MAX_PATH+1] = {0};

			// selezione della directory
			CDirDialog dlg(pWinAppEx->m_Config.GetString(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_LASTMOVETODIR_KEY),
						"Move To...",
						"Select a folder:"
						);
select_dir:
			if(dlg.DoModal()==IDOK)
			{
				strcpyn(szFolder,dlg.GetPathName(),sizeof(szFolder));
				pWinAppEx->m_Config.UpdateString(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_LASTMOVETODIR_KEY,szFolder);
				pWinAppEx->m_Config.SaveKey(WALLBROWSER_INSTALL_KEY,WALLBROWSER_LASTMOVETODIR_KEY);
				
				_snprintf(m_szFileName,
						sizeof(m_szFileName)-1,
						"%s%s",
						pDoc->GetPathName(),
						pDoc->GetFileName()
						);
				
				_snprintf(szDestinationFile,sizeof(szDestinationFile)-1,"%s\\%s",szFolder,pDoc->GetFileName());
				
				if(strcmp(m_szFileName,szDestinationFile)==0)
				{
					dlg.SetWindowTitle("Move To...");
					dlg.SetTitle("Select a folder (the destination folder cannot be the same as the source folder):");
					goto select_dir;
				}

				BOOL bMoved = FALSE;
				if(m_szFileName[1]==':' && szDestinationFile[1]==':' && (m_szFileName[0]!=szDestinationFile[0]))
				{
					bMoved = ::CopyFile(m_szFileName,szDestinationFile,FALSE);
					::DeleteFile(m_szFileName);
				}
				else
				{
					::DeleteFile(szDestinationFile);
					bMoved = ::MoveFile(m_szFileName,szDestinationFile);
				}

				if(bMoved)
				{
					int nItem = OnDeleteItem();
					OnSelectItem(nItem);

					int nTotItems = GetListCtrl().GetItemCount();
					CWnd* pWnd = AfxGetMainWnd();
					if(pWnd)
					{
						pWnd->SendMessage(WM_VIEW_PREVIOUS_PICTURE,nTotItems > 0 ? TRUE : FALSE,0L);
						pWnd->SendMessage(WM_VIEW_NEXT_PICTURE,nTotItems > 0 ? TRUE : FALSE,0L);
					}

					if(m_pMainFrameStatusBar)
					{
						char szStatus[_MAX_PATH+1];

						_snprintf(szStatus,
								sizeof(szStatus)-1,
								"%d file(s)",
								--m_nFiles
								);
						m_pMainFrameStatusBar->SetPaneText(ID_INDICATOR_FILES_ID,szStatus);

						_snprintf(szStatus,
								sizeof(szStatus)-1,
								"%d picture(s)",
								--m_nPictures
								);
						m_pMainFrameStatusBar->SetPaneText(ID_INDICATOR_PICTURES_ID,szStatus);
					}
				}
			}
		}
	}
}

/*
	OnDeleteFile()
*/
void CWallBrowserFileView::OnDeleteFile(void)
{
	CWallBrowserDoc* pDoc = (CWallBrowserDoc*)GetDocument();
	
	if(pDoc)
	{
		if(pDoc->HavePathName() && pDoc->HaveFileName())
		{
			_snprintf(m_szFileName,
					sizeof(m_szFileName)-1,
					"%s%s",
					pDoc->GetPathName(),
					pDoc->GetFileName()
					);
			
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
				bDelete = ::DeleteFileToRecycleBin(this->m_hWnd,m_szFileName,bConfirmFileDelete);
			}
			else
			{
				if(bConfirmFileDelete)
					bDelete = ::MessageBoxResourceEx(this->m_hWnd,MB_ICONQUESTION|MB_YESNO,WALLBROWSER_PROGRAM_NAME,IDS_QUESTION_FILEDELETE,m_szFileName)==IDYES;
				else
					bDelete = TRUE;
				if(bDelete)
					::DeleteFile(m_szFileName);
			}

			if(bDelete)
			{
				int nItem = OnDeleteItem();
				OnSelectItem(nItem);

				int nTotItems = GetListCtrl().GetItemCount();
				CWnd* pWnd = AfxGetMainWnd();
				if(pWnd)
				{
					pWnd->SendMessage(WM_VIEW_PREVIOUS_PICTURE,nTotItems > 0 ? TRUE : FALSE,0L);
					pWnd->SendMessage(WM_VIEW_NEXT_PICTURE,nTotItems > 0 ? TRUE : FALSE,0L);
				}

				if(m_pMainFrameStatusBar)
				{
					char szStatus[_MAX_PATH+1];

					_snprintf(szStatus,
							sizeof(szStatus)-1,
							"%d file(s)",
							--m_nFiles
							);
					m_pMainFrameStatusBar->SetPaneText(ID_INDICATOR_FILES_ID,szStatus);

					_snprintf(szStatus,
							sizeof(szStatus)-1,
							"%d picture(s)",
							--m_nPictures
							);
					m_pMainFrameStatusBar->SetPaneText(ID_INDICATOR_PICTURES_ID,szStatus);
				}
			}
		}
	}
}

/*
	OnCopy()
*/
void CWallBrowserFileView::OnCopy(void)
{
	CWnd* pWnd = AfxGetMainWnd();
	if(pWnd)
		pWnd->SendMessage(WM_COMMAND,MAKELONG(ID_EDIT_COPY,0),0L);
}

/*
	OnPaste()
*/
void CWallBrowserFileView::OnPaste(void)
{
	CWnd* pWnd = AfxGetMainWnd();
	if(pWnd)
		pWnd->SendMessage(WM_COMMAND,MAKELONG(ID_EDIT_PASTE,0),0L);
}

/*
	OnPrevious()
*/
void CWallBrowserFileView::OnPrevious(void)
{
	if(GetListCtrl().GetItemCount() > 0)
	{
		int nItem = GetCurrentItem();
		
		if(--nItem < 0)
			nItem = GetListCtrl().GetItemCount()-1;

		OnSelectItem(nItem);
	}
}

/*
	OnNext()
*/
void CWallBrowserFileView::OnNext(void)
{
	if(GetListCtrl().GetItemCount() > 0)
	{
		int nItem = GetCurrentItem();
		
		if(++nItem >= GetListCtrl().GetItemCount())
			nItem = 0;

		OnSelectItem(nItem);
	}
}

/*
	PopUpMenu()
*/
void CWallBrowserFileView::PopUpMenu(const CPoint& point)
{
	CRect rect;
	CMenu menu;
	CMenu* pSubMenu = NULL;

	// coordinate dell'area client (CWnd, base 0,0)
	CWnd::GetClientRect(&rect);
	
	// trasforma le coordinate in assolute (schermo)
	// da aggiungere alle coordinate del click del mouse per ottenere le coordinate a cui visualizzare il menu a comparsa
	CWnd::ClientToScreen(&rect);	
	
	// click su un elemento del controllo
	if(GetListCtrl().HitTest(point) >= 0)
	{
		if(menu.LoadMenu(IDM_FILEPOPUP))
			pSubMenu = menu.GetSubMenu(0);
	}

	// visualizza il menu popup
	if(pSubMenu)
	{
		pSubMenu->EnableMenuItem(IDM_FILEPOPUP_PASTE,::IsClipboardFormatAvailable(CF_DIB) ? MF_ENABLED : MF_GRAYED);
		pSubMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,point.x+rect.left,point.y+rect.top,this);
	}
}

/*
	LoadList()
*/
int CWallBrowserFileView::LoadList(LPCSTR lpcszPathName)
{
	CWaitCursor cursor;

	CWallBrowserDoc* pDoc = (CWallBrowserDoc*)GetDocument();
	if(!pDoc)
		return(0);

	CImage* pImage = pDoc->GetImage();
	if(!pImage)
		return(0);
	
	CString strPath = lpcszPathName;
	if(strPath.Right(1)!="\\")
		strPath += "\\";
	strPath += "*.*";

	HANDLE hFind;
	WIN32_FIND_DATA fd;

	m_nFiles = m_nPictures = 0;

	SetRedraw(FALSE);

	if((hFind = ::FindFirstFile((LPCTSTR)strPath,&fd))!=INVALID_HANDLE_VALUE)
	{
		if(!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			m_nFiles++;

			if(pImage->IsImageFile(fd.cFileName))
			{
				if(AddItem(m_nPictures,&fd))
					m_nPictures++;
			}
		}

		while(::FindNextFile(hFind,&fd))
		{
			if(!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				m_nFiles++;

				if(pImage->IsImageFile(fd.cFileName))
				{
					if(AddItem(m_nPictures,&fd))
						m_nPictures++;
					else
						break;
				}
			}
		}
	
		::FindClose(hFind);
	}

	if(m_nPictures > 0)
		GetListCtrl().SortItems(CompareFunc,g_nSortOrder);

	BOOL bAutoSizeCols = TRUE;
	CWinAppEx* pWinAppEx = (CWinAppEx*)AfxGetApp();
	if(pWinAppEx)
		bAutoSizeCols = pWinAppEx->m_Config.GetNumber(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_AUTOSIZECOLUMNS_KEY);
	if(bAutoSizeCols)
		AutoSizeColumns();
	
	SetRedraw(TRUE);

	pDoc->SetTitle(lpcszPathName);

	return(m_nPictures);
}

/*
	AddItem()
*/
BOOL CWallBrowserFileView::AddItem(int nIndex,WIN32_FIND_DATA* pfd)
{
	BOOL bAdded = FALSE;
	ITEMINFO* pItem = new ITEMINFO;
	
	if(pItem)
	{
		int nImageIndex = 0;
		char* p = (char*)strrchr(pItem->strFileName,'.');
		if(p)
		{
			char szExt[_MAX_EXT+1];
			strcpyn(szExt,p,sizeof(szExt));

			ITERATOR iter;
			ICONINDEX* iconindex;

			if((iter = m_IconList.First())!=(ITERATOR)NULL)
			{
				while(iter!=(ITERATOR)NULL)
				{
					iconindex = (ICONINDEX*)iter->data;
					if(iconindex)
					{
						if(stricmp(iconindex->ext,szExt)==0)
						{
							nImageIndex = iconindex->index;
							break;
						}
					}

					iter = m_IconList.Next(iter);
				}
			}
		}

		pItem->strFileName = pfd->cFileName;
		pItem->nFileSizeLow = pfd->nFileSizeLow;
		pItem->ftLastWriteTime = pfd->ftLastWriteTime;

		LV_ITEM lvi;
		lvi.mask = LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM; 
		lvi.iItem = nIndex; 
		lvi.iSubItem = 0; 
		lvi.iImage = nImageIndex;
		lvi.pszText = LPSTR_TEXTCALLBACK; 
		lvi.lParam = (LPARAM)pItem;

		bAdded = GetListCtrl().InsertItem(&lvi)!=-1;
	}

	return(bAdded);
}

/*
	FreeItemMemory()
*/
void CWallBrowserFileView::FreeItemMemory(void)
{
	int nCount = GetListCtrl().GetItemCount();
	if(nCount > 0)
	{
		for(int i = 0; i < nCount; i++)
			delete (ITEMINFO*)GetListCtrl().GetItemData(i);
	}
}

/*
	CompareFunc()
*/
int CALLBACK CWallBrowserFileView::CompareFunc(LPARAM lParam1,LPARAM lParam2,LPARAM lParamSort)
{
	ITEMINFO* pItem1 = (ITEMINFO*)lParam1;
	ITEMINFO* pItem2 = (ITEMINFO*)lParam2;
	int nResult = 0;

	switch(lParamSort)
	{
		// Name
		case 0:
			nResult = pItem1->strFileName.CompareNoCase(pItem2->strFileName);
			if(!g_bFileNameAscending)
				if(nResult!=0)
					nResult = nResult > 0 ? -1 : +1;
			break;

		// Size
		case 1:
			nResult = pItem1->nFileSizeLow - pItem2->nFileSizeLow;
			if(!g_bSizeAscending)
				if(nResult!=0)
					nResult = nResult > 0 ? -1 : +1;
			break;

		// Modified
		case 2:
			nResult = ::CompareFileTime(&pItem1->ftLastWriteTime,&pItem2->ftLastWriteTime);
			if(!g_bModifiedAscending)
				if(nResult!=0)
					nResult = nResult > 0 ? -1 : +1;
			break;
	}

	return(nResult);
}
