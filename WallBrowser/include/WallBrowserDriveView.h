/*
	WallBrowserDriveView.h
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
#ifndef _CWALLBROWSERDRIVEVIEW_H
#define _CWALLBROWSERDRIVEVIEW_H 1

#include <stdlib.h>
#include "window.h"
#include "CNodeList.h"
#include <afxcview.h>
#include "WallBrowserDoc.h"

// indici delle immagini presenti nel bitmap (IDR_DRIVEIMAGES)
#define ILI_HARD_DISK       0
#define ILI_RAM_DRIVE       0
#define ILI_NET_DRIVE       1
#define ILI_FLOPPY          2
#define ILI_CD_ROM          3
#define ILI_CLOSED_FOLDER   4
#define ILI_OPEN_FOLDER     5

#define MAX_DRIVE_INFO 16

/*
	DRIVEINFO
	struttura per le info sui drive
*/
struct DRIVEINFO {
	HTREEITEM hItem;
	char szDrive[MAX_DRIVE_INFO];
};

/*
	CDriveInfoList
	classe per la lista dei drive
*/
class CDriveInfoList : public CNodeList
{
public:
	CDriveInfoList() : CNodeList() {}
	virtual ~CDriveInfoList() {CNodeList::DeleteAll();}
	
	void* Create(void)
	{
		DRIVEINFO* pData = new DRIVEINFO;
		return(pData);
	}
	
	void* Initialize(void* pVoid)
	{
		DRIVEINFO* pData = (DRIVEINFO*)pVoid;
		if(!pData)
			pData = (DRIVEINFO*)Create();
		if(pData)
			memset(pData,'\0',sizeof(DRIVEINFO));
		return(pData);
	}

	BOOL PreDelete(ITERATOR iter)
	{
		if((DRIVEINFO*)iter->data)
		{
			delete ((DRIVEINFO*)iter->data);
			iter->data = (DRIVEINFO*)NULL;
		}
		return(TRUE);
	}

	int Size(void) {return(-1);}

#ifdef _DEBUG
	const char* Signature(void) {return("CDriveInfoList");}
#endif
};

/*
	CWallBrowserDriveView
*/
class CWallBrowserDriveView : public CTreeView
{
	DECLARE_DYNCREATE (CWallBrowserDriveView)

public:
	CWallBrowserDriveView();
	~CWallBrowserDriveView();

	BOOL			PreCreateWindow	(CREATESTRUCT& cs);
	int			OnCreate			(LPCREATESTRUCT lpcs);
	
	void			OnInitialUpdate	(void);
	void			OnUpdate			(CView* pSender,LPARAM lHint,CObject* pHint);
	
	void			OnChar			(UINT nChar,UINT nRepCnt,UINT nFlags);
	void			OnKeyDown			(UINT nChar,UINT nRepCnt,UINT nFlags);
	void			OnRButtonDown		(UINT nFlags,CPoint point);

	void			OnSelChanged		(NMHDR* pnmh,LRESULT* pResult);
	void			OnSelectionChanged	(LPCSTR lpcszPathName);
	void			OnItemExpanding	(NMHDR* pnmh,LRESULT* pResult);

	void			OnDeleteFile		(void);

	inline const CWallBrowserDoc* GetDocument(void) const {return((CWallBrowserDoc*)m_pDocument);}

private:
	int			LoadTree			(void);
	void			ExpandTree		(LPCSTR lpcszPathName);
	BOOL			AddDriveNode		(CString& strDrive);
	int			AddDirectories		(HTREEITEM hItem,LPCSTR lpcszPathName);
	HTREEITEM		FindItem			(HTREEITEM hParent,LPCSTR lpcszLabel);
	void			DeleteFirstChild	(HTREEITEM hItem);
	void			DeleteAllChildren	(HTREEITEM hItem);
	BOOL			SetButtonState		(HTREEITEM hItem,LPCSTR lpcszPathName);
	LPCSTR		GetPathFromNode	(HTREEITEM hItem);
	void			PopupMenu			(CPoint& point);

	HTREEITEM		m_hPrevItem;
	char			m_szPathName[_MAX_PATH+1];
	CImageList	m_ImageList;
	CStatusBar*	m_pMainFrameStatusBar;
	BOOL			m_bDeleting;
	CDriveInfoList	m_DriveInfoList;
	char			m_szLastBrowseDir[_MAX_PATH+1];

	DECLARE_MESSAGE_MAP();
};

#endif // _CWALLBROWSERDRIVEVIEW_H
