/*
	WallBrowserFileView.h
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
#ifndef _CWALLBROWSERFILEVIEW_H
#define _CWALLBROWSERFILEVIEW_H 1

#include "window.h"
#include "CNodeList.h"
#include "CListViewEx.h"
#include "WallBrowserDoc.h"

/*
	MINCOLWIDTH
	dimensione mimina per la colonna
*/
#define MINCOLWIDTH 10

/*
	ITEMINFO
	struttura per le info sul file
*/
typedef struct tagITEMINFO {
	CString	strFileName; 
	DWORD	nFileSizeLow; 
	FILETIME	ftLastWriteTime; 
} ITEMINFO;

/*
	CWallBrowserFileView
*/
class CWallBrowserFileView : public CListViewEx
{
	DECLARE_DYNCREATE(CWallBrowserFileView)

public:
	CWallBrowserFileView();
	virtual ~CWallBrowserFileView();

	int		OnCreate			(LPCREATESTRUCT lpcs);
	void		OnDestroy			(void);
	void		OnInitialUpdate	(void);
	void		OnUpdate			(CView* pView,LPARAM lHint,CObject* pHint);

	void		OnChar			(UINT nChar,UINT nRepCnt,UINT nFlags);
	void		OnKeyDown			(UINT nChar,UINT nRepCnt,UINT nFlags);
	void		OnLButtonDown		(UINT nFlags,CPoint point);
	void		OnRButtonDown		(UINT nFlags,CPoint point);
	
	void		OnGetDispInfo		(NMHDR* pnmh,LRESULT* pResult);
	void		OnColumnClick		(NMHDR* pnmh,LRESULT* pResult);
	int		OnSelectItem		(int nItem);
	int		OnSelectItem		(CPoint point);
	int		OnDeleteItem		(void);

	void		OnMoveFile		(void);
	void		OnDeleteFile		(void);
	void		OnCopy			(void);
	void		OnPaste			(void);
	void		OnPrevious		(void);
	void		OnNext			(void);

	inline const CWallBrowserDoc* GetDocument (void) const {return((CWallBrowserDoc*)m_pDocument);}

	static int CALLBACK CompareFunc(LPARAM lParam1,LPARAM lParam2,LPARAM lParamSort);

private:
	void		PopUpMenu			(const CPoint& point);
	
	int		LoadList			(LPCSTR lpcszPathName);
	BOOL		AddItem			(int,WIN32_FIND_DATA*);
	void		FreeItemMemory		(void);

	int			m_nFiles;
	int			m_nPictures;
	char			m_szFileName[_MAX_PATH+1];
	CImageList	m_ImageList;
	CStatusBar*	m_pMainFrameStatusBar;
	CIconIndexList	m_IconList;

	DECLARE_MESSAGE_MAP();
};

#endif // _CWALLBROWSERFILEVIEW_H
