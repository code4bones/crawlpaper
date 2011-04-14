/*
	WallBrowserStretchViewDlg.h
	Classe per la vista per la visualizzazione dell'immagine.
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
#ifndef _WALLBROWSERSTRETCHVIEW_H
#define _WALLBROWSERSTRETCHVIEW_H 1

#include "window.h"
#include "CWinAppEx.h"
#include "CImageDraw.h"
#include "CZoomView.h"
#include "TStack.h"
#include "WallBrowserDoc.h"
#include "WallBrowserImageOperation.h"

/*
	UNDOINFO
	struttura per l'elemento per lo stack dell'undo/redo
*/
struct UNDOINFO {
	CImageFactory* pImageFactory;
	DIBINFO DibInfo;
	int nRedo;
};

/*
	CWallBrowserStretchView
*/
class CWallBrowserStretchView : public CZoomView
{
	DECLARE_DYNCREATE(CWallBrowserStretchView)

public:
	CWallBrowserStretchView();
	virtual ~CWallBrowserStretchView();

	void		NotifyRanges		(BOOL bResetZoom = TRUE);

	void		OnInitialUpdate	(void);
	void		OnUpdate			(CView* pSender,LPARAM lHint,CObject* pHint);

	void		OnSize			(UINT nType,int cx,int cy);
	BOOL		OnEraseBkgnd		(CDC* pDC);
	void		OnDraw			(CDC* pDC);

	void		OnChar			(UINT nChar,UINT nRepCnt,UINT nFlags);
	void		OnKeyDown			(UINT nChar,UINT nRepCnt,UINT nFlags);

	void		OnEditCopy		(void);
	void		OnEditPaste		(void);
	void		OnEditUndo		(void);
	BOOL		UndoPush			(int);
	BOOL		UndoPop			(void);
	void		UndoEmpty			(void);
	BOOL		UndoEnabled		(void);
	void		OnEditRedo		(void);
	BOOL		RedoPush			(int);
	BOOL		RedoPop			(void);
	void		RedoEmpty			(void);
	BOOL		RedoEnabled		(void);

	BOOL		OnPreparePrinting	(CPrintInfo* pInfo);
	void		OnBeginPrinting	(CDC* pDC,CPrintInfo* pInfo);
	void		OnEndPrinting		(CDC* pDC,CPrintInfo* pInfo);

	BOOL		OnImageOperation	(UINT nID);
	UINT		DoImageOperation	(UINT nID);
	void		EnableMenu		(void);
	void		DisableMenu		(void);

	inline const CWallBrowserDoc* GetDocument(void) const {return((CWallBrowserDoc*)m_pDocument);}

private:
	CWinAppEx*				m_pWinAppEx;
	BOOL						m_bLoaded;
	BOOL						m_bHavePicture;
	char						m_szFileName[_MAX_PATH+1];
	int						m_nViewType;
	int						m_nDrawMode;
	CRect					m_rcInvalid;
	CRect					m_rcClient;
	CWallBrowserImageOperation	m_ImageOperation;
	CImageDraw				m_ImageDraw;
	BOOL						m_bRebuildPalette;
	char						m_szStatus[_MAX_PATH+1];
	CStatusBar*				m_pMainFrameStatusBar;
	int						m_nUndoLevel;
	TStack<UNDOINFO*>			m_UndoInfo;
	TStack<int>				m_RedoInfo;
	int						m_nRedoLevel;
	int						m_nLastOperation;
	int						m_nRedo;

	DECLARE_MESSAGE_MAP()
};

#endif // _WALLBROWSERSTRETCHVIEW_H
