/*
	WallBrowserStretchViewDlg.cpp
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
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include <stdio.h>
#include <stdlib.h>
#include "strings.h"
#include "macro.h"
#include "window.h"
#include "win32api.h"
#include "CImageFactory.h"
#include "CImageDraw.h"
#include "CZoomView.h"
#include "CWinAppEx.h"
#include "WallPaperVersion.h"
#include "WallBrowserVersion.h"
#include "WallBrowserMessages.h"
#include "WallBrowserDoc.h"
#include "WallBrowserStretchView.h"
#include "WallBrowserImageOperation.h"
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

IMPLEMENT_DYNCREATE(CWallBrowserStretchView,CZoomView)

BEGIN_MESSAGE_MAP(CWallBrowserStretchView,CZoomView)
	ON_WM_SIZE()

	ON_WM_ERASEBKGND()
	ON_WM_KEYDOWN()
	ON_WM_CHAR()

	ON_COMMAND(ID_EDIT_COPY,OnEditCopy)
	ON_COMMAND(ID_EDIT_PASTE,OnEditPaste)
	ON_COMMAND(ID_EDIT_UNDO,OnEditUndo)
	ON_COMMAND(ID_EDIT_REDO,OnEditRedo)

	ON_COMMAND(ID_FILE_PRINT,CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT,CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW,CView::OnFilePrintPreview)

	ON_COMMAND_EX(ID_IMAGE_MIRROR_H,OnImageOperation)
	ON_COMMAND_EX(ID_IMAGE_MIRROR_V,OnImageOperation)
	ON_COMMAND_EX(ID_IMAGE_ROTATE_90_LEFT,OnImageOperation)
	ON_COMMAND_EX(ID_IMAGE_ROTATE_90_RIGHT,OnImageOperation)
	ON_COMMAND_EX(ID_IMAGE_ROTATE_180,OnImageOperation)
	ON_COMMAND_EX(ID_IMAGE_DESKEW,OnImageOperation)
	ON_COMMAND_EX(ID_IMAGE_SIZE,OnImageOperation)
	ON_COMMAND_EX(ID_IMAGE_POSTERIZE,OnImageOperation)
	ON_COMMAND_EX(ID_IMAGE_MOSAIC,OnImageOperation)
	ON_COMMAND_EX(ID_IMAGE_BLUR,OnImageOperation)
	ON_COMMAND_EX(ID_IMAGE_MEDIAN,OnImageOperation)
	ON_COMMAND_EX(ID_IMAGE_SHARPEN,OnImageOperation)
	ON_COMMAND_EX(ID_IMAGE_DESPECKLE,OnImageOperation)
	ON_COMMAND_EX(ID_IMAGE_NOISE,OnImageOperation)
	ON_COMMAND_EX(ID_IMAGE_EMBOSS,OnImageOperation)
	ON_COMMAND_EX(ID_IMAGE_EDGE_ENHANCE,OnImageOperation)
	ON_COMMAND_EX(ID_IMAGE_EROSION,OnImageOperation)
	ON_COMMAND_EX(ID_IMAGE_FIND_EDGE,OnImageOperation)
	ON_COMMAND_EX(ID_IMAGE_DILATE,OnImageOperation)

	ON_COMMAND_EX(ID_COLOR_HALFTONE,OnImageOperation)
	ON_COMMAND_EX(ID_COLOR_GRAYSCALE,OnImageOperation)
	ON_COMMAND_EX(ID_COLOR_INVERT,OnImageOperation)
	ON_COMMAND_EX(ID_COLOR_BRIGHTNESS,OnImageOperation)
	ON_COMMAND_EX(ID_COLOR_CONTRAST,OnImageOperation)
	ON_COMMAND_EX(ID_COLOR_HISTOGRAM_CONTRAST,OnImageOperation)
	ON_COMMAND_EX(ID_COLOR_HUE,OnImageOperation)
	ON_COMMAND_EX(ID_COLOR_SATURATION_H,OnImageOperation)
	ON_COMMAND_EX(ID_COLOR_SATURATION_V,OnImageOperation)
	ON_COMMAND_EX(ID_COLOR_EQUALIZE,OnImageOperation)
	ON_COMMAND_EX(ID_COLOR_INTENSITY,OnImageOperation)
	ON_COMMAND_EX(ID_COLOR_INTENSITY_DETECT,OnImageOperation)
	ON_COMMAND_EX(ID_COLOR_GAMMA_CORRECTION,OnImageOperation)
END_MESSAGE_MAP()

/*
	CWallBrowserStretchView()
*/
CWallBrowserStretchView::CWallBrowserStretchView()
{
	m_pWinAppEx = (CWinAppEx*)AfxGetApp();
	m_bLoaded = FALSE;
	m_bHavePicture = FALSE;
	memset(m_szFileName,'\0',sizeof(m_szFileName));
	m_nViewType = m_pWinAppEx->m_Config.GetNumber(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_VIEWTYPE_KEY);
	if(m_nViewType==VIEWTYPE_SCROLL)
	{
		// modalita' visualizzazione (centrato/isotropic)
		SetCenterMode(TRUE);
		SetMapMode(MM_ANISOTROPIC);
		SetCursorZoomMode(CURSOR_MODE_BYCLICK);
	}
	else if(m_nViewType==VIEWTYPE_STRETCH)
	{
		// modalita' visualizzazione (centrato/text)
		SetCenterMode(FALSE);
		SetMapMode(MM_TEXT);
		SetCursorZoomMode(CURSOR_MODE_NONE);
	}
	m_nDrawMode = m_pWinAppEx->m_Config.GetNumber(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_DRAWMODE_KEY);
	m_rcInvalid.SetRect(-1,-1,-1,-1);
	m_rcClient.SetRect(-1,-1,-1,-1);
	m_ImageOperation.SetImage(NULL);
	m_ImageDraw.SetImage(NULL);
	m_bRebuildPalette = FALSE; // mettere a TRUE con le operazioni che cambiano l'immagine (ad es. grayscale)
	memset(m_szStatus,'\0',sizeof(m_szStatus));
	m_pMainFrameStatusBar = NULL;
	m_nUndoLevel = 0;
	m_nRedoLevel = 0;
	m_nLastOperation = -1;
	m_nRedo = -1;
}

/*
	~CWallBrowserStretchView()
*/
CWallBrowserStretchView::~CWallBrowserStretchView()
{
	UndoEmpty();
	RedoEmpty();
}

/*
	NotifyRanges()

	Posiziona l'immagine nella vista.
*/
void CWallBrowserStretchView::NotifyRanges(BOOL bResetZoom/*=TRUE*/)
{
	CImage *pImage = GetDocument()->GetImage();
	if(pImage)
	{
		CSize size(0,0);

		if(bResetZoom)
		{
			size.cx = pImage->GetWidth();
			size.cy = pImage->GetHeight();
		}
		else
		{
			size.cx = (int)((double)pImage->GetWidth() * GetZoomRatio());
			size.cy = (int)((double)pImage->GetHeight() * GetZoomRatio());
		}

		SetZoomSizes(size);
		
		if(bResetZoom)
		{
			SetZoomMode(MODE_ZOOMOFF);
			SetZoomRatio(1.0);
		}
	}
}

/*
	OnInitialUpdate()
*/
void CWallBrowserStretchView::OnInitialUpdate(void)
{
	// imposta il puntatore all'immagine per le classi di visualizzazione/manipolazione
	CImage* pImage = NULL;
	CWallBrowserDoc* pDoc = (CWallBrowserDoc*)GetDocument();
	if(pDoc)
		pImage = pDoc->GetImage();
	
	m_ImageDraw.SetImage(pImage);
	m_ImageOperation.SetImage(pImage);

	// aggiorna la GUI
	CWnd* pWnd = AfxGetMainWnd();
	if(pWnd)
	{
		m_pMainFrameStatusBar = (CStatusBar*)pWnd->SendMessage(WM_GET_STATUSBAR,0,0L);
		if(m_pMainFrameStatusBar)
			EnableMenu();

		pWnd->SendMessage(WM_FILE_SAVE,pDoc ? pDoc->IsModified() : FALSE,0L);
		pWnd->SendMessage(WM_FILE_SAVE_AS,m_bLoaded,0L);
		pWnd->SendMessage(WM_FILE_MOVE,m_bHavePicture,0L);
		pWnd->SendMessage(WM_FILE_DELETE,m_bHavePicture,0L);
		pWnd->SendMessage(WM_EDIT_COPY,m_bLoaded,0L);
		pWnd->SendMessage(WM_EDIT_PASTE,m_bLoaded ? ::IsClipboardFormatAvailable(CF_DIB) : FALSE,0L);
		pWnd->SendMessage(WM_EDIT_UNDO,UndoEnabled(),0L);
		pWnd->PostMessage(WM_EDIT_REDO,RedoEnabled(),0L);
	}

	// classe base
	CZoomView::OnInitialUpdate();
}

/*
	OnUpdate()
*/
void CWallBrowserStretchView::OnUpdate(CView* pSender,LPARAM lHint,CObject* pHint)
{
	CWaitCursor cursor;
	BOOL bHint = FALSE;
	static BOOL bTreeExpandedEx = FALSE;

	if(lHint & ONUPDATE_FLAG_TREEEXPANDEDEX)
	{
		TRACE("%s(%d): receiving ONUPDATE_FLAG_TREEEXPANDEDEX\n",__FILE__,__LINE__);
		bTreeExpandedEx = TRUE;
	}

	if(!bTreeExpandedEx)
		goto done;

	// cambio immagine/libreria correnti
	if((lHint & ONUPDATE_FLAG_FILECHANGED) || (lHint & ONUPDATE_FLAG_DEFAULTLIBRARY))
	{
		TRACE("%s(%d): receiving ONUPDATE_FLAG_FILECHANGED || ONUPDATE_FLAG_DEFAULTLIBRARY\n",__FILE__,__LINE__);

		m_bLoaded = FALSE;
		CWallBrowserDoc* pDoc = (CWallBrowserDoc*)GetDocument();
		CImage* pImage = NULL;

		memset(m_szStatus,'\0',sizeof(m_szStatus));

		if(pDoc)
		{
			// salva l'immagine se e' stata modificata
			if(pDoc->IsModified())
				pDoc->SaveModified(m_szFileName);

			pImage = pDoc->GetImage();
			if(pImage)
			{
				if(pDoc->HaveFileName())
				{
					// ricava il nome del file da caricare
					_snprintf(m_szFileName,
							sizeof(m_szFileName)-1,
							"%s%s",
							pDoc->GetPathName(),
							pDoc->GetFileName()
							);

					strcpyn(m_szStatus,"loading...",sizeof(m_szStatus));
					if(m_pMainFrameStatusBar)
						m_pMainFrameStatusBar->SetPaneText(ID_INDICATOR_FILENAME_ID,m_szStatus);
					pDoc->SetTitle(m_szStatus);
					
					// carica l'immagine
					m_bHavePicture = TRUE;

					if(pImage->Load(m_szFileName))
					{
						m_bLoaded = TRUE;
						strcpyn(m_szStatus,pDoc->GetFileName(),sizeof(m_szStatus));
					}
					else
					{
						m_bLoaded = FALSE;
						_snprintf(m_szStatus,sizeof(m_szStatus)-1,"load failed - %s",pDoc->GetFileName());
					}

					pDoc->SetTitle(m_szStatus);
					pDoc->SetPictureFlag(m_bLoaded);
					if(!m_bLoaded)
						pDoc->ResetFileName();
				}
			}
		}
		
		// caricamento dell'immagine riuscito
		if(m_bLoaded)
		{
			if(m_pMainFrameStatusBar)
			{
				EnableMenu();
				m_pMainFrameStatusBar->SetPaneText(ID_INDICATOR_FILENAME_ID,m_szStatus);
			}
		}
		else // caricamento fallito
		{
			if(m_pMainFrameStatusBar)
			{
				DisableMenu();
				m_pMainFrameStatusBar->SetPaneText(ID_INDICATOR_FILENAME_ID,"");
				//m_pMainFrameStatusBar->SetPaneText(ID_INDICATOR_FILES_ID,"");
				m_pMainFrameStatusBar->SetPaneText(ID_INDICATOR_INFO_ID,"");
				m_pMainFrameStatusBar->SetPaneText(ID_INDICATOR_LIBRARY_ID,"");
				m_pMainFrameStatusBar->SetPaneText(ID_INDICATOR_MEM_ID,"");
				//m_pMainFrameStatusBar->SetPaneText(ID_INDICATOR_PICTURES_ID,"");
				m_pMainFrameStatusBar->SetPaneText(ID_INDICATOR_RATIO_ID,"");
				m_pMainFrameStatusBar->SetPaneText(ID_INDICATOR_ZOOM_ID,"");
			}
		}
		
		// aggiorna la GUI
		CWnd* pWnd = AfxGetMainWnd();
		if(pWnd)
		{
			pWnd->SendMessage(WM_FILE_SAVE,FALSE,0L);
			pWnd->SendMessage(WM_FILE_SAVE_AS,m_bLoaded,0L);
			pWnd->SendMessage(WM_FILE_MOVE,m_bHavePicture,0L);
			pWnd->SendMessage(WM_FILE_DELETE,m_bHavePicture,0L);
			pWnd->SendMessage(WM_EDIT_COPY,m_bLoaded,0L);
			pWnd->SendMessage(WM_EDIT_PASTE,m_bLoaded ? ::IsClipboardFormatAvailable(CF_DIB) : FALSE,0L);
			pWnd->SendMessage(WM_EDIT_UNDO,FALSE,0L);
			pWnd->PostMessage(WM_EDIT_REDO,FALSE,0L);
			UndoEmpty();
			RedoEmpty();
		}

		// per evitare di cancellare il fondo se la nuova immagine e' piu' grande della precedente
		BOOL bEraseOnInvalidate = TRUE;

		// modalita' stretch, calcola la dimensione dello stretch
		if(m_nViewType==VIEWTYPE_STRETCH)
		{
			// modalita' visualizzazione (centrato/text)
			SetCenterMode(FALSE);
			SetMapMode(MM_TEXT);
			SetCursorZoomMode(CURSOR_MODE_NONE);

			// elimina le barre dello scroll
			CSize size(0,0);
			SetZoomSizes(size);

			bEraseOnInvalidate = m_bLoaded ? ((lHint & ONUPDATE_FLAG_DEFAULTLIBRARY) ? FALSE : TRUE) : TRUE;

			if(bEraseOnInvalidate && m_rcInvalid.right!=-1 && m_rcInvalid.bottom!=-1 && pImage)
			{
				double nRemains = 0.0;
				double nWidth   = (double)pImage->GetWidth();
				double nHeight  = (double)pImage->GetHeight();

				GetClientRect(&m_rcClient);

				if(nHeight > (double)m_rcClient.bottom)
				{
					nRemains = FDIV(nHeight,(double)m_rcClient.bottom);
					if(nRemains > 0.0)
					{
						nHeight = FDIV(nHeight,nRemains);
						nWidth  = FDIV(nWidth,nRemains);
					}
				}
				if(nWidth > (double)m_rcClient.right)
				{
					nRemains = FDIV(nWidth,(double)m_rcClient.right);
					if(nRemains > 0.0)
					{
						nHeight = FDIV(nHeight,nRemains);
						nWidth  = FDIV(nWidth,nRemains);
					}
				}

				if((int)nWidth >= m_rcInvalid.right && (int)nHeight >= m_rcInvalid.bottom)
					bEraseOnInvalidate = FALSE;
			}
		}
		// modalita' scroll
		else if(m_nViewType==VIEWTYPE_SCROLL)
		{
			// deve cancellare lo sfondo e riposizionare le barre dello scroll a 0,0
			bEraseOnInvalidate = TRUE;
			POINT point = {0,0};
			ScrollToPosition(point);
			NotifyRanges();
		}

		// (ri)disegna l'immagine
		Invalidate(bEraseOnInvalidate);

		bHint = TRUE;
	}
	// click sull'immagine gia' selezionata (ma non attiva)
	if(lHint & ONUPDATE_FLAG_FILESELECTED)
	{
		TRACE("%s(%d): receiving ONUPDATE_FLAG_FILESELECTED\n",__FILE__,__LINE__);

		CWallBrowserDoc* pDoc = (CWallBrowserDoc*)GetDocument();
		if(pDoc)
		{
			CImage* pImage = pDoc->GetImage();
			if(pImage && pImage->GetWidth() > 0 && pImage->GetHeight() > 0)
			{
				// aggiorna la GUI
				CWnd* pWnd = AfxGetMainWnd();
				if(pWnd)
				{
					m_bLoaded = pDoc->GetPictureFlag();
					pWnd->SendMessage(WM_FILE_SAVE,pDoc->IsModified(),0L);
					pWnd->SendMessage(WM_FILE_SAVE_AS,m_bLoaded,0L);
					pWnd->SendMessage(WM_FILE_MOVE,m_bHavePicture,0L);
					pWnd->SendMessage(WM_FILE_DELETE,m_bHavePicture,0L);
					pWnd->SendMessage(WM_EDIT_COPY,m_bLoaded,0L);
					pWnd->SendMessage(WM_EDIT_PASTE,m_bLoaded ? ::IsClipboardFormatAvailable(CF_DIB) : FALSE,0L);
					pWnd->SendMessage(WM_EDIT_UNDO,UndoEnabled(),0L);
					pWnd->PostMessage(WM_EDIT_REDO,RedoEnabled(),0L);
				}
			}
		}

		//Invalidate(FALSE);

		bHint = TRUE;
	}
	// eliminazione dell'immagine corrente
	if(lHint & ONUPDATE_FLAG_FILEDELETED)
	{
		TRACE("%s(%d): receiving ONUPDATE_FLAG_FILEDELETED\n",__FILE__,__LINE__);

		// se l'immagine e' stata modificata resetta il flag
		CWallBrowserDoc* pDoc = (CWallBrowserDoc*)GetDocument();
		if(pDoc->IsModified())
			pDoc->SetModifiedFlag(FALSE);

		// aggiorna la GUI
		CWnd* pWnd = AfxGetMainWnd();
		if(pWnd)
		{
			pWnd->SendMessage(WM_FILE_SAVE,FALSE,0L);
			pWnd->SendMessage(WM_FILE_SAVE_AS,FALSE,0L);
			pWnd->SendMessage(WM_FILE_MOVE,FALSE,0L);
			pWnd->SendMessage(WM_FILE_DELETE,FALSE,0L);
			pWnd->SendMessage(WM_EDIT_COPY,FALSE,0L);
			pWnd->SendMessage(WM_EDIT_PASTE,FALSE,0L);
			pWnd->SendMessage(WM_EDIT_UNDO,FALSE,0L);
			pWnd->PostMessage(WM_EDIT_REDO,FALSE,0L);
			UndoEmpty();
			RedoEmpty();
		}

		// deve disegnare il contenuto della nuova immagine o ripulire l'area client se era l'unica immagine della directory
		Invalidate(TRUE);

		// se il file era l'unico nella directory
		pDoc->SetTitle(pDoc->GetPathName());
		
		bHint = TRUE;
	}
	// cambio della directory corrente
	if(lHint & ONUPDATE_FLAG_DIRCHANGED || lHint & ONUPDATE_FLAG_DIRSELECTED)
	{
		TRACE("%s(%d): receiving ONUPDATE_FLAG_DIRCHANGED || ONUPDATE_FLAG_DIRSELECTED\n",__FILE__,__LINE__);

		m_bLoaded = FALSE;

		// salva l'immagine se e' stata modificata
		CWallBrowserDoc* pDoc = (CWallBrowserDoc*)GetDocument();
		if(pDoc)
		{
			if(pDoc->IsModified())
				pDoc->SaveModified(m_szFileName);
			
			m_bLoaded = pDoc->GetPictureFlag();
		}

		// aggiorna la GUI
		CWnd* pWnd = AfxGetMainWnd();
		if(pWnd)
		{
			pWnd->SendMessage(WM_FILE_SAVE,FALSE,0L);
			pWnd->SendMessage(WM_FILE_SAVE_AS,m_bLoaded,0L);
			pWnd->SendMessage(WM_FILE_MOVE,m_bHavePicture,0L);
			pWnd->SendMessage(WM_FILE_DELETE,m_bHavePicture,0L);
			pWnd->SendMessage(WM_EDIT_COPY,m_bLoaded,0L);
			pWnd->SendMessage(WM_EDIT_PASTE,m_bLoaded ? ::IsClipboardFormatAvailable(CF_DIB) : FALSE,0L);
			pWnd->SendMessage(WM_EDIT_UNDO,FALSE,0L);
			pWnd->PostMessage(WM_EDIT_REDO,FALSE,0L);
			UndoEmpty();
			RedoEmpty();
		}

		// deve ripulire l'area client
		if(m_nViewType==VIEWTYPE_SCROLL)
			NotifyRanges();

		Invalidate(TRUE);
		
		bHint = TRUE;
	}
	// e' stato svuotato lo stack per l'undo/redo
	if(lHint & ONUPDATE_FLAG_EMPTYSTACK)
	{
		TRACE("%s(%d): receiving ONUPDATE_FLAG_EMPTYSTACK\n",__FILE__,__LINE__);

		CWnd* pWnd = AfxGetMainWnd();
		if(pWnd)
		{
			pWnd->SendMessage(WM_EDIT_UNDO,FALSE,0L);
			pWnd->PostMessage(WM_EDIT_REDO,FALSE,0L);
		}

		UndoEmpty();
		RedoEmpty();
		
		bHint = TRUE;
	}
	// e' stata cambiata la modalita' di visualizzazione (stretch/scroll)
	if(lHint & ONUPDATE_FLAG_VIEWTYPE)
	{
		TRACE("%s(%d): receiving ONUPDATE_FLAG_VIEWTYPE\n",__FILE__,__LINE__);

		m_nViewType = m_pWinAppEx->m_Config.GetNumber(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_VIEWTYPE_KEY);
		
		if(m_nViewType==VIEWTYPE_SCROLL)
		{
			// modalita' visualizzazione (centrato/isotropic)
			SetCenterMode(TRUE);
			SetMapMode(MM_ANISOTROPIC);
			SetCursorZoomMode(CURSOR_MODE_BYCLICK);

			// risposiziona le barre dello scroll a 0,0
			POINT point = {0,0};
			ScrollToPosition(point);
			
			SetZoomMode(MODE_ZOOMOFF);
			NotifyRanges();
		}
		else if(m_nViewType==VIEWTYPE_STRETCH)
		{
			// modalita' visualizzazione (centrato/text)
			SetCenterMode(FALSE);
			SetMapMode(MM_TEXT);
			SetCursorZoomMode(CURSOR_MODE_NONE);

			// elimina le barre dello scroll
			CSize size(0,0);
			SetZoomSizes(size);
		}

		// deve ridisegnare l'immagine ripulendo l'area client (dato che passa da una modalita' all'altra)
		Invalidate(TRUE);

		bHint = TRUE;
	}
	// e' stata cambiata la modalita' di disegno
	if((lHint & ONUPDATE_FLAG_DRAWSTRETCHDIBITS) || (lHint & ONUPDATE_FLAG_DRAWSTRETCHBLT) || (lHint & ONUPDATE_FLAG_DRAWVFWDRAWDIB))
	{
		TRACE("%s(%d): receiving ONUPDATE_FLAG_DRAWSTRETCHDIBITS || ONUPDATE_FLAG_DRAWSTRETCHBLT || ONUPDATE_FLAG_DRAWVFWDRAWDIB\n",__FILE__,__LINE__);

		if(lHint & ONUPDATE_FLAG_DRAWSTRETCHDIBITS)
			m_nDrawMode = DRAW_STRETCHDIBITS;
		else if(lHint & ONUPDATE_FLAG_DRAWSTRETCHBLT)
			m_nDrawMode = DRAW_STRETCHBLT;
		else if (lHint & ONUPDATE_FLAG_DRAWVFWDRAWDIB)
			m_nDrawMode = DRAW_VFWDRAWDIB;
		else
			m_nDrawMode = DEFAULT_DRAWMODE;

		// deve ridisegnare l'immagine senza ripulire l'area client (cambia solo la modalita' di disegno, non quella di visualizzazione)
		Invalidate(FALSE);

		bHint = TRUE;
	}
	// e' stato cambiato il fattore di zoom
	if((lHint & ONUPDATE_FLAG_ZOOMIN) || (lHint & ONUPDATE_FLAG_ZOOMOUT) || (lHint & ONUPDATE_FLAG_ZOOMRESET))
	{
		TRACE("%s(%d): receiving ONUPDATE_FLAG_ZOOMIN || ONUPDATE_FLAG_ZOOMOUT || ONUPDATE_FLAG_ZOOMRESET\n",__FILE__,__LINE__);

		if(lHint & ONUPDATE_FLAG_ZOOMIN)
		{
			SetZoomMode(MODE_ZOOMIN);
			TRACE("zoom ratio: %.2f\n",GetZoomRatio());
			if(GetZoomRatio() <= 28.42)
				DoZoomIn();
		}
		else if(lHint & ONUPDATE_FLAG_ZOOMOUT)
		{
			SetZoomMode(MODE_ZOOMOUT);
			TRACE("zoom ratio: %.2f\n",GetZoomRatio());
			if(GetZoomRatio() >= 0.04)
				DoZoomOut();
		}
		else if(lHint & ONUPDATE_FLAG_ZOOMRESET)
		{
			SetZoomMode(MODE_ZOOMOFF);
			NotifyRanges();
			Invalidate(TRUE);
		}

		bHint = TRUE;
	}

done:

	// classe base
	if(!bHint)
		CZoomView::OnUpdate(pSender,lHint,pHint);
}

/*
	OnSize()
*/
void CWallBrowserStretchView::OnSize(UINT nType,int cx,int cy) 
{
	CZoomView::OnSize(nType,cx,cy);
	Invalidate(TRUE);
}

/*
	OnEraseBkgnd()
*/
BOOL CWallBrowserStretchView::OnEraseBkgnd(CDC* pDC)
{
	if((GetStyle() & WS_VSCROLL) && (GetStyle() & WS_HSCROLL))
	{
		BOOL bNeedErase = FALSE;

		// documento
		CWallBrowserDoc* pDoc = (CWallBrowserDoc*)GetDocument();
		if(!pDoc)
			bNeedErase = TRUE;

		// nome file
		if(!bNeedErase)
		{
			memset(m_szFileName,'\0',sizeof(m_szFileName));
			strcpyn(m_szFileName,pDoc->GetFileName(),sizeof(m_szFileName));
			if(m_szFileName[0]=='\0')
				bNeedErase = TRUE;
		}

		// immagine
		if(!bNeedErase)
		{
			CImage *pImage = pDoc->GetImage();
			if(pImage && pImage->GetWidth() > 0 && pImage->GetHeight() > 0)
				;
			else
				bNeedErase = TRUE;
		}

		// immagine valida
		if(!bNeedErase)
		{
			if(!pDoc->GetPictureFlag())
				bNeedErase = TRUE;
		}
				
		if(!bNeedErase)
			return(TRUE);
	}

	return(CZoomView::OnEraseBkgnd(pDC));
}

/*
	OnDraw()
*/
void CWallBrowserStretchView::OnDraw(CDC* pDC)
{
	// documento
	CWallBrowserDoc* pDoc = (CWallBrowserDoc*)GetDocument();
	if(!pDoc)
		return;

	// nome file
	memset(m_szFileName,'\0',sizeof(m_szFileName));
	strcpyn(m_szFileName,pDoc->GetFileName(),sizeof(m_szFileName));
	if(m_szFileName[0]=='\0')
		return;

	// immagine
	CImage *pImage = pDoc->GetImage();
	if(pImage && pImage->GetWidth() > 0 && pImage->GetHeight() > 0)
		m_ImageDraw.SetImage(pImage);
	else
		return;

	// immagine valida
	if(!pDoc->GetPictureFlag())
		return;

	// dimensione corrente della vista
	GetClientRect(&m_rcClient);

	// adatta l'immagine alla dimensione corrente della vista
	double nRemains = 0.0;
	double nWidth   = (double)pImage->GetWidth();
	double nHeight  = (double)pImage->GetHeight();

	if(nHeight > (double)m_rcClient.bottom)
	{
		nRemains = FDIV(nHeight,(double)m_rcClient.bottom);
		if(nRemains > 0.0)
		{
			nHeight = FDIV(nHeight,nRemains);
			nWidth  = FDIV(nWidth,nRemains);
		}
	}
	if(nWidth > (double)m_rcClient.right)
	{
		nRemains = FDIV(nWidth,(double)m_rcClient.right);
		if(nRemains > 0.0)
		{
			nHeight = FDIV(nHeight,nRemains);
			nWidth  = FDIV(nWidth,nRemains);
		}
	}

	m_rcInvalid.SetRect(0,0,(int)nWidth,(int)nHeight);

	// nome del file
	if(m_pMainFrameStatusBar)
	{
		strcpyn(m_szStatus,pDoc->GetFileName(),sizeof(m_szStatus));
		m_pMainFrameStatusBar->SetPaneText(ID_INDICATOR_FILENAME_ID,m_szStatus);
	}

	double nFactor = 0.0;
	char szFactor[8] = {0};
	if(m_nViewType==VIEWTYPE_SCROLL)
	{
		nFactor = GetZoomRatio();
		if(nFactor >= 1.0)
			sprintf(szFactor,"%.2f:1",nFactor);
		else
			sprintf(szFactor,"1:%.2f",1/nFactor);
	}

	// fattore di zoom e % di visualizzazione dell'immagine nella vista
	int nRatio = (int)((nWidth * 100.0)/pImage->GetWidth());
	if(m_pMainFrameStatusBar)
	{
		m_pMainFrameStatusBar->SetPaneText(ID_INDICATOR_ZOOM_ID,szFactor);
		
		if(m_nViewType==VIEWTYPE_STRETCH)
			_snprintf(m_szStatus,sizeof(m_szStatus)-1,"%d%% (stretch)",nRatio);
		else
			_snprintf(m_szStatus,sizeof(m_szStatus)-1,"%.1f%% (scroll)",nFactor * 100.0);
		
		m_pMainFrameStatusBar->SetPaneText(ID_INDICATOR_RATIO_ID,m_szStatus);
	}
	
	// area della vista (client rect)
	m_rcClient.right  = (int)nWidth;
	m_rcClient.bottom = (int)nHeight;

	// ricava le informazioni per il titolo
	char szTitle[_MAX_PATH+1] = {0};
	int nColors = pImage->GetNumColors();
	if(m_nViewType==VIEWTYPE_STRETCH)
		_snprintf(szFactor,sizeof(szFactor)-1,"%d%%",nRatio);
	_snprintf(szTitle,
			sizeof(szTitle)-1,
			"%s (%s) - %d x %d x %d%s",
			pDoc->GetFileName(),
			szFactor,
			pImage->GetWidth(),
			pImage->GetHeight(),
			(nColors > 256 || nColors==0) ? 16 : nColors,
			(nColors > 256 || nColors==0) ? "M" : ""
			);
	pDoc->SetTitle(szTitle);

	// ricava le informazioni per la status bar
	if(m_pMainFrameStatusBar)
	{
		int nColors = pImage->GetNumColors();
		_snprintf(m_szStatus,
				sizeof(m_szStatus)-1,
				"%d x %d x %d%s colors, %d bpp",
				pImage->GetWidth(),
				pImage->GetHeight(),
				(nColors > 256 || nColors==0) ? 16 : nColors,
				(nColors > 256 || nColors==0) ? "M" : "",
				pImage->GetBPP()
				);
		m_pMainFrameStatusBar->SetPaneText(ID_INDICATOR_INFO_ID,m_szStatus);

		UINT nMemUsed = pImage->GetMemUsed();
		if(nMemUsed < 1024L)
			_snprintf(m_szStatus,sizeof(m_szStatus)-1,"%ld bytes",nMemUsed);
		else if(nMemUsed < 1048576L)
			_snprintf(m_szStatus,sizeof(m_szStatus)-1,"%0.1f KB",FDIV(((float)nMemUsed),1024.0f));
		else
			_snprintf(m_szStatus,sizeof(m_szStatus)-1,"%0.2f MB",FDIV(((float)nMemUsed),1048576.0f));
		m_pMainFrameStatusBar->SetPaneText(ID_INDICATOR_MEM_ID,m_szStatus);

		strcpyn(m_szStatus,pImage->GetLibraryName(),sizeof(m_szStatus));
		m_pMainFrameStatusBar->SetPaneText(ID_INDICATOR_LIBRARY_ID,m_szStatus);
	}
	
	// area dell'immagine
	CRect rcImg(0,0,pImage->GetWidth(),pImage->GetHeight());

	// se per la vista deve usare lo scroll, imposta le aree in modo tale che vengano visualizzate le barre di scorrimento
	if(m_nViewType==VIEWTYPE_SCROLL)
		m_rcClient = rcImg;

	// visualizza l'immagine scalata rispetto alla dimensione corrente della vista
	m_ImageDraw.DrawEx(pDC->GetSafeHdc(),
					&m_rcClient,
					&rcImg,
					NULL,
					FALSE,
					COLORONCOLOR,
					m_nDrawMode,
					SRCCOPY,
					m_bRebuildPalette
					);

	m_bRebuildPalette = FALSE;
}

/*
	OnChar()
*/
void CWallBrowserStretchView::OnChar(UINT nChar,UINT nRepCnt,UINT nFlags)
{
	switch(nChar)
	{
		case '+':
		{
			if(m_nViewType==VIEWTYPE_SCROLL)
			{
				SetZoomMode(MODE_ZOOMIN);
				TRACE("zoom ratio: %.2f\n",GetZoomRatio());
				if(GetZoomRatio() <= 28.42)
					DoZoomIn();
				return;
			}
		}
		case '-':
		{
			if(m_nViewType==VIEWTYPE_SCROLL)
			{
				SetZoomMode(MODE_ZOOMOUT);
				TRACE("zoom ratio: %.2f\n",GetZoomRatio());
				if(GetZoomRatio() >= 0.04)
					DoZoomOut();
				return;
			}
		}
		case '=':
		{
			if(m_nViewType==VIEWTYPE_SCROLL)
			{
				SetZoomMode(MODE_ZOOMOFF);
				NotifyRanges();
				Invalidate(TRUE);
				return;
			}
		}
	}
	
	CZoomView::OnChar(nChar,nRepCnt,nFlags);
}

/*
	OnKeyDown()
*/
void CWallBrowserStretchView::OnKeyDown(UINT nChar,UINT nRepCnt,UINT nFlags)
{
	switch(nChar)
	{
		// redo
		case 'y':
		case 'Y':
		{
			if(::GetKeyState(VK_CONTROL) < 0)
			{
				OnEditRedo();
				return;
			}
			break;
		}

		// ctrl inizio-fine su barra vert, senza inizio-fine su barra oriz
		case VK_HOME:
			SendMessage(::GetAsyncKeyState(VK_CONTROL) ? WM_VSCROLL : WM_HSCROLL,SB_TOP);
//			SendMessage(WM_VSCROLL,SB_TOP);
//			SendMessage(WM_HSCROLL,SB_TOP);
			return;
		case VK_END:
			SendMessage(::GetAsyncKeyState(VK_CONTROL) ? WM_VSCROLL : WM_HSCROLL,SB_BOTTOM);
//			SendMessage(WM_VSCROLL,SB_BOTTOM);
//			SendMessage(WM_HSCROLL,SB_BOTTOM);
			return;
		case VK_PRIOR:
			SendMessage(WM_VSCROLL,SB_PAGEUP);
			return;
		case VK_NEXT:
			SendMessage(WM_VSCROLL,SB_PAGEDOWN);
			return;
		case VK_UP:
			SendMessage(WM_VSCROLL,SB_LINEUP);
			return;
		case VK_DOWN:
			SendMessage(WM_VSCROLL,SB_LINEDOWN);
			return;

		// next picture
		case VK_RIGHT:
		{
			if(::GetKeyState(VK_SHIFT) < 0)
			{
				CWnd* pWnd = AfxGetMainWnd();
				if(pWnd)
					pWnd->SendMessage(WM_COMMAND,MAKELONG(ID_VIEW_NEXT_PICTURE,0),0L);
				return;
			}
			else
			{
				SendMessage(WM_HSCROLL,SB_LINEDOWN);
				return;
			}
			break;
		}

		// previous picture
		case VK_LEFT:
		{
			if(::GetKeyState(VK_SHIFT) < 0)
			{
				CWnd* pWnd = AfxGetMainWnd();
				if(pWnd)
					pWnd->SendMessage(WM_COMMAND,MAKELONG(ID_VIEW_PREVIOUS_PICTURE,0),0L);
				return;
			}
			else
			{
				SendMessage(WM_HSCROLL,SB_LINEUP);
				return;
			}
			break;
		}
		
		// passa alla vista seguente
		case VK_TAB:
		{	
			CWallBrowserDoc* pDoc = (CWallBrowserDoc*)GetDocument();
			if(pDoc)
				pDoc->GotoNextView();
			break;
		}
	}
	
	CZoomView::OnKeyDown(nChar,nRepCnt,nFlags);
}

/*
	OnEditCopy()
*/
void CWallBrowserStretchView::OnEditCopy(void)
{
	CWaitCursor cursor;
	BOOL bCopied = FALSE;

	// copia il contenuto dell'immagine nella clipboard
	if(::OpenClipboard(NULL))
	{
		if(::EmptyClipboard())
		{
			CWallBrowserDoc* pDoc = (CWallBrowserDoc*)GetDocument();
			if(pDoc)
			{
				CImage *pImage = pDoc->GetImage();
				if(pImage && pImage->GetWidth() > 0 && pImage->GetHeight() > 0)
					bCopied = ::SetClipboardData(CF_DIB,pImage->GetDIB())!=(HANDLE)NULL;
			}
		}

		::CloseClipboard();
	}

	// imposta il redo con l'ultima operazione effettuata
	if(bCopied)
		RedoPush(ID_EDIT_COPY);
}

/*
	OnEditPaste()
*/
void CWallBrowserStretchView::OnEditPaste(void)
{
	CWaitCursor cursor;
	BOOL bPasted = FALSE;

	// salva il contenuto corrente per l'undo
	if(!UndoPush(ID_EDIT_PASTE))
		if(::MessageBoxResource(this->m_hWnd,MB_ICONWARNING|MB_YESNO,WALLBROWSER_PROGRAM_NAME,IDS_ERROR_UNDOSTACKFULL)==IDNO)
			return;
	
	// incolla il contenuto della clipboard nell'immagine
	if(::OpenClipboard(NULL))
	{
		HDIB hDib = (HDIB)::GetClipboardData(CF_DIB);
		::CloseClipboard();

		if(hDib)
		{
			CWallBrowserDoc* pDoc = (CWallBrowserDoc*)GetDocument();
			if(pDoc)
			{
				CImage *pImage = pDoc->GetImage();
				if(pImage && pImage->GetWidth() > 0 && pImage->GetHeight() > 0)
					if(pImage->SetDIB(hDib))
					{
						m_bHavePicture = TRUE;
						pDoc->SetModifiedFlag(TRUE);
						CWnd* pWnd = AfxGetMainWnd();
						if(pWnd)
						{
							pWnd->SendMessage(WM_FILE_SAVE_AS,TRUE,0L);
							pWnd->SendMessage(WM_FILE_SAVE,TRUE,0L);
							pWnd->SendMessage(WM_FILE_MOVE,TRUE,0L);
							pWnd->SendMessage(WM_FILE_DELETE,TRUE,0L);
						}
						Invalidate(TRUE);
						bPasted = TRUE;
					}
			}
		}
	}

	// se l'incolla non riesce ripristina il contenuto originale
	if(!bPasted)
	{
		UndoPop();
		Invalidate(TRUE);
	}

	// imposta il redo con l'ultima operazione effettuata
	if(bPasted)
		RedoPush(ID_EDIT_PASTE);
}

/*
	OnEditUndo()
*/
void CWallBrowserStretchView::OnEditUndo(void)
{
	// ripristina il contenuto dell'immagine
	if(UndoPop())
	{
		Invalidate(TRUE);

		CWallBrowserDoc* pDoc = (CWallBrowserDoc*)GetDocument();
		if(pDoc)
		{
			// fino a che non raggiunge la fine dello stack dell'undo non resetta il flag per documento modificato
			BOOL bModified = UndoEnabled();
			pDoc->SetModifiedFlag(bModified);
			CWnd* pWnd = AfxGetMainWnd();
			if(pWnd)
			{
				pWnd->SendMessage(WM_FILE_SAVE_AS,bModified,0L);
				pWnd->SendMessage(WM_FILE_SAVE,bModified,0L);
				pWnd->SendMessage(WM_FILE_MOVE,bModified,0L);
				pWnd->SendMessage(WM_FILE_DELETE,bModified,0L);
			}
		}
	}
}

/*
	UndoPush()
*/
BOOL CWallBrowserStretchView::UndoPush(int nID)
{
	BOOL bUndo = FALSE;

	if(++m_nUndoLevel <= MAX_STACK_SIZE)
	{
		// salva il contenuto dell'immagine nello stack per l'undo
		CWallBrowserDoc* pDoc = (CWallBrowserDoc*)GetDocument();
		if(pDoc)
		{
			CImage *pImage = pDoc->GetImage();
			if(pImage && pImage->GetWidth() > 0 && pImage->GetHeight() > 0)
			{
				CImage* pImg;
				CImageFactory* pImgFactory = new CImageFactory();
				char szLibraryName[_MAX_PATH+1];
				strcpy(szLibraryName,pImage->GetLibraryName());
				pImg = pImgFactory->Create(szLibraryName,sizeof(szLibraryName));
				DIBINFO DibInfo;
				DibInfo.nPerspective = 0;

				HDIB hDib = pImage->GetDIB(&DibInfo);
				if(hDib)
				{
					pImg->SetDIB(hDib,&DibInfo);
					::GlobalFree(hDib);
					
					UNDOINFO* undoinfo = new UNDOINFO;
					undoinfo->pImageFactory = pImgFactory;
					undoinfo->DibInfo.nPerspective = DibInfo.nPerspective;
					undoinfo->nRedo = nID;
					int nLevel = m_UndoInfo.Push(undoinfo);
					if(nLevel!=-1)
						bUndo = TRUE;
				}
				else
				{
					delete pImgFactory;
				}
			}
		}
	}
	else
		m_nUndoLevel--;

	CWnd* pWnd = AfxGetMainWnd();
	if(pWnd)
	{
		pWnd->SendMessage(WM_EDIT_UNDO,UndoEnabled(),0L);
		pWnd->SendMessage(WM_EDIT_REDO,FALSE,0L);
	}

	return(bUndo);
}

/*
	UndoPop()
*/
BOOL CWallBrowserStretchView::UndoPop(void)
{
	BOOL bUndo = FALSE;

	if(--m_nUndoLevel >= 0)
	{
		CWallBrowserDoc* pDoc = (CWallBrowserDoc*)GetDocument();
		if(pDoc)
		{
			CImage *pImage = pDoc->GetImage();
			if(pImage && pImage->GetWidth() > 0 && pImage->GetHeight() > 0)
			{
				int index;
				CImage* pImg;

				UNDOINFO* undoinfo = m_UndoInfo.Pop(index);
				if(index >= 0 && undoinfo)
				{
					m_nRedo = undoinfo->nRedo;

					CImageFactory* pImgFactory = undoinfo->pImageFactory;
					pImg = pImgFactory->GetImage();
					HDIB hDib = pImg->GetDIB(&(undoinfo->DibInfo));
					
					if(hDib)
					{
						pImage->SetDIB(hDib,&(undoinfo->DibInfo));
						::GlobalFree(hDib);
						delete (CImageFactory*)undoinfo->pImageFactory;
						delete undoinfo;
						bUndo = TRUE;
					}
					else
					{
						delete (CImageFactory*)undoinfo->pImageFactory;
						delete undoinfo;
					}
				}
				else
					::MessageBeep(MB_ICONEXCLAMATION);
			}
		}
	}
	else
		m_nUndoLevel++;

	CWnd* pWnd = AfxGetMainWnd();
	if(pWnd)
	{
		pWnd->SendMessage(WM_EDIT_UNDO,UndoEnabled(),0L);
		pWnd->SendMessage(WM_EDIT_REDO,RedoEnabled(),0L);
	}

	return(bUndo);
}

/*
	UndoEmpty()
*/
void CWallBrowserStretchView::UndoEmpty(void)
{
	int index = 1;
	UNDOINFO* undoinfo;
	
	while((undoinfo = m_UndoInfo.Pop(index))!=NULL && index >= 0)
	{
		delete (CImageFactory*)undoinfo->pImageFactory;
		delete undoinfo;
	}
}

/*
	UndoEnabled()
*/
BOOL CWallBrowserStretchView::UndoEnabled(void)
{
	return(m_UndoInfo.Deep() > 0);
}

/*
	OnEditRedo()
*/
void CWallBrowserStretchView::OnEditRedo(void)
{
	if(RedoPop())
	{
		CWallBrowserDoc* pDoc = (CWallBrowserDoc*)GetDocument();
		if(pDoc)
		{
			m_bHavePicture = TRUE;
			pDoc->SetModifiedFlag(TRUE);
			CWnd* pWnd = AfxGetMainWnd();
			if(pWnd)
			{
				pWnd->SendMessage(WM_FILE_SAVE_AS,TRUE,0L);
				pWnd->SendMessage(WM_FILE_SAVE,TRUE,0L);
				pWnd->SendMessage(WM_FILE_MOVE,TRUE,0L);
				pWnd->SendMessage(WM_FILE_DELETE,TRUE,0L);
			}
		}
	}
}

/*
	RedoPush()
*/
BOOL CWallBrowserStretchView::RedoPush(int nRedo)
{
	m_nRedo = nRedo;
	return(TRUE);
}

/*
	RedoPop()
*/
BOOL CWallBrowserStretchView::RedoPop(void)
{
	BOOL bRedo = FALSE;

	if(m_nRedo!=-1)
	{
		CWallBrowserDoc* pDoc = (CWallBrowserDoc*)GetDocument();
		if(pDoc)
		{
			CImage *pImage = pDoc->GetImage();
			if(pImage && pImage->GetWidth() > 0 && pImage->GetHeight() > 0)
			{
				bRedo = DoImageOperation(m_nRedo)==NO_ERROR;
				m_nRedo = -1;
			}
		}
	}

	CWnd* pWnd = AfxGetMainWnd();
	if(pWnd)
		pWnd->PostMessage(WM_EDIT_REDO,RedoEnabled(),0L);

	return(bRedo);
}

/*
	RedoEmpty()
*/
void CWallBrowserStretchView::RedoEmpty(void)
{
	m_nRedo = -1;
}

/*
	RedoEnabled()
*/
BOOL CWallBrowserStretchView::RedoEnabled(void)
{
	return(m_nRedo!=-1);
}

/*
	OnPreparePrinting()
*/
BOOL CWallBrowserStretchView::OnPreparePrinting(CPrintInfo* pInfo)
{
	return(DoPreparePrinting(pInfo));
}

/*
	OnBeginPrinting()
*/
void CWallBrowserStretchView::OnBeginPrinting(CDC* /*pDC*/,CPrintInfo* /*pInfo*/)
{
}

/*
	OnEndPrinting
*/
void CWallBrowserStretchView::OnEndPrinting(CDC* /*pDC*/,CPrintInfo* /*pInfo*/)
{
}

/*
	OnImageOperation()
*/
BOOL CWallBrowserStretchView::OnImageOperation(UINT nID)
{
	RedoPush(nID);
	DoImageOperation(nID);
	return(TRUE);
}

/*
	DoImageOperation()
*/
UINT CWallBrowserStretchView::DoImageOperation(UINT nID)
{
	CWaitCursor cursor;

	UINT nRet = GDI_ERROR;

	m_nLastOperation = nID;

	CImage* pImage = NULL;
	CWallBrowserDoc* pDoc = (CWallBrowserDoc*)GetDocument();
	if(pDoc)
		pImage = pDoc->GetImage();
	
	if(pImage && pImage->GetWidth() > 0 && pImage->GetHeight() > 0)
	{
		BOOL bModified = FALSE;
		
		if(!UndoPush(nID))
			if(::MessageBoxResource(this->m_hWnd,MB_ICONWARNING|MB_YESNO,WALLBROWSER_PROGRAM_NAME,IDS_ERROR_UNDOSTACKFULL)==IDNO)
				return(TRUE);

		m_ImageOperation.SetImage(pImage);	

		BOOL bEraseOnInvalidate = FALSE;

		switch(nID)
		{
			// Image->Mirror
			case ID_IMAGE_MIRROR_H:
				nRet = m_ImageOperation.MirrorHorizontal();
				break;
			case ID_IMAGE_MIRROR_V:
				nRet = m_ImageOperation.MirrorVertical();
				break;
			
			// Image->Rotate
			case ID_IMAGE_ROTATE_90_LEFT:
				nRet = m_ImageOperation.Rotate90Left();
				bEraseOnInvalidate = TRUE;
				break;
			case ID_IMAGE_ROTATE_90_RIGHT:
				nRet = m_ImageOperation.Rotate90Right();
				bEraseOnInvalidate = TRUE;
				break;
			case ID_IMAGE_ROTATE_180:
				nRet = m_ImageOperation.Rotate180();
				bEraseOnInvalidate = TRUE;
				break;
			
			// Image->Deskew
			case ID_IMAGE_DESKEW:
				nRet = m_ImageOperation.Deskew();
				break;
			
			// Image->Size
			case ID_IMAGE_SIZE:
				nRet = m_ImageOperation.Size();
				bEraseOnInvalidate = TRUE;
				break;
			
			// Image->Effects
			case ID_IMAGE_POSTERIZE:
				nRet = m_ImageOperation.Posterize();
				break;
			case ID_IMAGE_MOSAIC:
				nRet = m_ImageOperation.Mosaic();
				break;
			case ID_IMAGE_BLUR:
				nRet = m_ImageOperation.Blur();
				break;
			case ID_IMAGE_MEDIAN:
				nRet = m_ImageOperation.Median();
				break;
			case ID_IMAGE_SHARPEN:
				nRet = m_ImageOperation.Sharpen();
				break;
			case ID_IMAGE_DESPECKLE:
				nRet = m_ImageOperation.Despeckle();
				break;
			case ID_IMAGE_NOISE:
				nRet = m_ImageOperation.Noise();
				break;
			case ID_IMAGE_EMBOSS:
				nRet = m_ImageOperation.Emboss();
				break;
			case ID_IMAGE_EDGE_ENHANCE:
				nRet = m_ImageOperation.EdgeEnhance();
				break;
			case ID_IMAGE_FIND_EDGE:
				nRet = m_ImageOperation.FindEdge();
				break;
			case ID_IMAGE_EROSION:
				nRet = m_ImageOperation.Erosion();
				break;
			case ID_IMAGE_DILATE:
				nRet = m_ImageOperation.Dilate();
				break;

			case ID_COLOR_HALFTONE:
				nRet = m_ImageOperation.Halftone();
				m_bRebuildPalette = TRUE;
				break;
			case ID_COLOR_GRAYSCALE:
				nRet = m_ImageOperation.Grayscale();
				m_bRebuildPalette = TRUE;
				break;
			case ID_COLOR_INVERT:
				nRet = m_ImageOperation.Invert();
				m_bRebuildPalette = TRUE;
				break;
			case ID_COLOR_BRIGHTNESS:
				nRet = m_ImageOperation.Brightness();
				m_bRebuildPalette = TRUE;
				break;
			case ID_COLOR_CONTRAST:
				nRet = m_ImageOperation.Contrast();
				m_bRebuildPalette = TRUE;
				break;
			case ID_COLOR_HISTOGRAM_CONTRAST:
				nRet = m_ImageOperation.HistoContrast();
				m_bRebuildPalette = TRUE;
				break;
			case ID_COLOR_HUE:
				nRet = m_ImageOperation.Hue();
				m_bRebuildPalette = TRUE;
				break;
			case ID_COLOR_SATURATION_H:
				nRet = m_ImageOperation.SaturationHorizontal();
				m_bRebuildPalette = TRUE;
				break;
			case ID_COLOR_SATURATION_V:
				nRet = m_ImageOperation.SaturationVertical();
				m_bRebuildPalette = TRUE;
				break;
			case ID_COLOR_EQUALIZE:
				nRet = m_ImageOperation.Equalize();
				m_bRebuildPalette = TRUE;
				break;
			case ID_COLOR_INTENSITY:
				nRet = m_ImageOperation.Intensity();
				m_bRebuildPalette = TRUE;
				break;
			case ID_COLOR_INTENSITY_DETECT:
				nRet = m_ImageOperation.IntensityDetect();
				m_bRebuildPalette = TRUE;
				break;
			case ID_COLOR_GAMMA_CORRECTION:
				nRet = m_ImageOperation.GammaCorrection();
				m_bRebuildPalette = TRUE;
				break;
		}

		if(nRet==NO_ERROR)
		{
			bModified = TRUE;
			
			if(m_nViewType==VIEWTYPE_SCROLL)
			{
				POINT point = {0,0};
				ScrollToPosition(point);
				NotifyRanges();
			}

			Invalidate(bEraseOnInvalidate);
			
			((CWallBrowserDoc*)GetDocument())->SetModifiedFlag(TRUE);

			CWnd* pWnd = AfxGetMainWnd();
			if(pWnd)
			{
				pWnd->SendMessage(WM_FILE_SAVE_AS,TRUE,0L);
				pWnd->SendMessage(WM_FILE_SAVE,TRUE,0L);
				pWnd->SendMessage(WM_FILE_MOVE,TRUE,0L);
				pWnd->SendMessage(WM_FILE_DELETE,TRUE,0L);
			}
		}

		if(!bModified)
			UndoPop();
	}

	return(nRet);
}

/*
	EnableMenu()
*/
void CWallBrowserStretchView::EnableMenu(void)
{
	CWnd* pWnd = AfxGetMainWnd();

	CImage* pImage = GetDocument()->GetImage();

	LPIMAGEOPERATION pImageOperation;
	while((pImageOperation = pImage->EnumImageOperation())!=(LPIMAGEOPERATION)NULL)
	{
		if(strcmp(pImageOperation->name,"MirrorHorizontal")==0)
			pWnd->SendMessage(WM_IMAGE_MIRROR_H,pImageOperation->flag,0L);
		else if(strcmp(pImageOperation->name,"MirrorVertical")==0)
			pWnd->SendMessage(WM_IMAGE_MIRROR_V,pImageOperation->flag,0L);
		else if(strcmp(pImageOperation->name,"Rotate90Left")==0)
			pWnd->SendMessage(WM_IMAGE_ROTATE_90_LEFT,pImageOperation->flag,0L);
		else if(strcmp(pImageOperation->name,"Rotate90Right")==0)
			pWnd->SendMessage(WM_IMAGE_ROTATE_90_RIGHT,pImageOperation->flag,0L);
		else if(strcmp(pImageOperation->name,"Rotate180")==0)
			pWnd->SendMessage(WM_IMAGE_ROTATE_180,pImageOperation->flag,0L);
		else if(strcmp(pImageOperation->name,"Deskew")==0)
			pWnd->SendMessage(WM_IMAGE_DESKEW,pImageOperation->flag,0L);
		else if(strcmp(pImageOperation->name,"Size")==0)
			pWnd->SendMessage(WM_IMAGE_SIZE,pImageOperation->flag,0L);
		else if(strcmp(pImageOperation->name,"Posterize")==0)
			pWnd->SendMessage(WM_IMAGE_POSTERIZE,pImageOperation->flag,0L);
		else if(strcmp(pImageOperation->name,"Mosaic")==0)
			pWnd->SendMessage(WM_IMAGE_MOSAIC,pImageOperation->flag,0L);
		else if(strcmp(pImageOperation->name,"Blur")==0)
			pWnd->SendMessage(WM_IMAGE_BLUR,pImageOperation->flag,0L);
		else if(strcmp(pImageOperation->name,"Median")==0)
			pWnd->SendMessage(WM_IMAGE_MEDIAN,pImageOperation->flag,0L);
		else if(strcmp(pImageOperation->name,"Sharpen")==0)
			pWnd->SendMessage(WM_IMAGE_SHARPEN,pImageOperation->flag,0L);
		else if(strcmp(pImageOperation->name,"Despeckle")==0)
			pWnd->SendMessage(WM_IMAGE_DESPECKLE,pImageOperation->flag,0L);
		else if(strcmp(pImageOperation->name,"Noise")==0) 
			pWnd->SendMessage(WM_IMAGE_NOISE,pImageOperation->flag,0L);
		else if(strcmp(pImageOperation->name,"Emboss")==0)
			pWnd->SendMessage(WM_IMAGE_EMBOSS,pImageOperation->flag,0L);
		else if(strcmp(pImageOperation->name,"EdgeEnhance")==0)
			pWnd->SendMessage(WM_IMAGE_EDGEENHANCE,pImageOperation->flag,0L);
		else if(strcmp(pImageOperation->name,"FindEdge")==0)
			pWnd->SendMessage(WM_IMAGE_FINDEDGE,pImageOperation->flag,0L);
		else if(strcmp(pImageOperation->name,"Erosion")==0)
			pWnd->SendMessage(WM_IMAGE_EROSION,pImageOperation->flag,0L);
		else if(strcmp(pImageOperation->name,"Dilate")==0)
			pWnd->SendMessage(WM_IMAGE_DILATE,pImageOperation->flag,0L);

		else if(strcmp(pImageOperation->name,"Halftone")==0)
			pWnd->SendMessage(WM_COLOR_HALFTONE,pImageOperation->flag,0L);
		else if(strcmp(pImageOperation->name,"Grayscale")==0)
			pWnd->SendMessage(WM_COLOR_GRAYSCALE,pImageOperation->flag,0L);
		else if(strcmp(pImageOperation->name,"Invert")==0)
			pWnd->SendMessage(WM_COLOR_INVERT,pImageOperation->flag,0L);
		else if(strcmp(pImageOperation->name,"Brightness")==0)
			pWnd->SendMessage(WM_COLOR_BRIGHTNESS,pImageOperation->flag,0L);
		else if(strcmp(pImageOperation->name,"Contrast")==0)
			pWnd->SendMessage(WM_COLOR_CONTRAST,pImageOperation->flag,0L);
		else if(strcmp(pImageOperation->name,"HistoContrast")==0) 
			pWnd->SendMessage(WM_COLOR_HISTOGRAM_CONTRAST,pImageOperation->flag,0L);
		else if(strcmp(pImageOperation->name,"Hue")==0)
			pWnd->SendMessage(WM_COLOR_HUE,pImageOperation->flag,0L);
		else if(strcmp(pImageOperation->name,"SaturationHorizontal")==0)
			pWnd->SendMessage(WM_COLOR_SATURATION_H,pImageOperation->flag,0L);
		else if(strcmp(pImageOperation->name,"SaturationVertical")==0)
			pWnd->SendMessage(WM_COLOR_SATURATION_V,pImageOperation->flag,0L);
		else if(strcmp(pImageOperation->name,"Equalize")==0)
			pWnd->SendMessage(WM_COLOR_EQUALIZE,pImageOperation->flag,0L);
		else if(strcmp(pImageOperation->name,"Intensity")==0)
			pWnd->SendMessage(WM_COLOR_INTENSITY,pImageOperation->flag,0L);
		else if(strcmp(pImageOperation->name,"IntensityDetect")==0)
			pWnd->SendMessage(WM_COLOR_INTENSITY_DETECT,pImageOperation->flag,0L);
		else if(strcmp(pImageOperation->name,"GammaCorrection")==0)
			pWnd->SendMessage(WM_COLOR_GAMMA_CORRECTION,pImageOperation->flag,0L);
	}
}

/*
	DisableMenu()
*/
void CWallBrowserStretchView::DisableMenu(void)
{
	CWnd* pWnd = AfxGetMainWnd();

	pWnd->SendMessage(WM_IMAGE_MIRROR_H,FALSE,0L);
	pWnd->SendMessage(WM_IMAGE_MIRROR_V,FALSE,0L);
	pWnd->SendMessage(WM_IMAGE_ROTATE_90_LEFT,FALSE,0L);
	pWnd->SendMessage(WM_IMAGE_ROTATE_90_RIGHT,FALSE,0L);
	pWnd->SendMessage(WM_IMAGE_ROTATE_180,FALSE,0L);
	pWnd->SendMessage(WM_IMAGE_DESKEW,FALSE,0L);
	pWnd->SendMessage(WM_IMAGE_SIZE,FALSE,0L);
	pWnd->SendMessage(WM_IMAGE_POSTERIZE,FALSE,0L);
	pWnd->SendMessage(WM_IMAGE_MOSAIC,FALSE,0L);
	pWnd->SendMessage(WM_IMAGE_BLUR,FALSE,0L);
	pWnd->SendMessage(WM_IMAGE_MEDIAN,FALSE,0L);
	pWnd->SendMessage(WM_IMAGE_SHARPEN,FALSE,0L);
	pWnd->SendMessage(WM_IMAGE_DESPECKLE,FALSE,0L);
	pWnd->SendMessage(WM_IMAGE_NOISE,FALSE,0L);
	pWnd->SendMessage(WM_IMAGE_EMBOSS,FALSE,0L);
	pWnd->SendMessage(WM_IMAGE_EDGEENHANCE,FALSE,0L);
	pWnd->SendMessage(WM_IMAGE_FINDEDGE,FALSE,0L);
	pWnd->SendMessage(WM_IMAGE_EROSION,FALSE,0L);
	pWnd->SendMessage(WM_IMAGE_DILATE,FALSE,0L);

	pWnd->SendMessage(WM_COLOR_HALFTONE,FALSE,0L);
	pWnd->SendMessage(WM_COLOR_GRAYSCALE,FALSE,0L);
	pWnd->SendMessage(WM_COLOR_INVERT,FALSE,0L);
	pWnd->SendMessage(WM_COLOR_BRIGHTNESS,FALSE,0L);
	pWnd->SendMessage(WM_COLOR_CONTRAST,FALSE,0L);
	pWnd->SendMessage(WM_COLOR_HISTOGRAM_CONTRAST,FALSE,0L);
	pWnd->SendMessage(WM_COLOR_HUE,FALSE,0L);
	pWnd->SendMessage(WM_COLOR_SATURATION_H,FALSE,0L);
	pWnd->SendMessage(WM_COLOR_SATURATION_V,FALSE,0L);
	pWnd->SendMessage(WM_COLOR_EQUALIZE,FALSE,0L);
	pWnd->SendMessage(WM_COLOR_INTENSITY,FALSE,0L);
	pWnd->SendMessage(WM_COLOR_INTENSITY_DETECT,FALSE,0L);
	pWnd->SendMessage(WM_COLOR_GAMMA_CORRECTION,FALSE,0L);
}
