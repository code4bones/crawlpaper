/*
	CDibCtrl.cpp
	Classe per controllo DIB statico (MFC).
	Luca Piergentili, 07/08/00
	lpiergentili@yahoo.com

	Riadattata, modificata ed ampliata a partire dal codice di:
	Copyright (C) 1998 by Jorge Lodos
	All rights reserved
	Distribute and use freely, except:
	1. Don't alter or remove this notice.
	2. Mark the changes you made
	Send bug reports, bug fixes, enhancements, requests, etc. to: lodos@cigb.edu.cu	
	Adapted from Mr.Lodos's code in order to use PaintLib's
	windows bitmap class and multi-format decoding
	B.Delmée 1998
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include "window.h"
#include "strings.h"
#include "CImageFactory.h"
#include "CImageDraw.h"
#include "CDibCtrl.h"

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

BEGIN_MESSAGE_MAP(CDibCtrl,CStatic)
    ON_WM_CTLCOLOR_REFLECT()
    ON_WM_QUERYNEWPALETTE()
    ON_WM_PALETTECHANGED()
END_MESSAGE_MAP()

/*
	CDibCtrl()
*/
CDibCtrl::CDibCtrl()
{
	memset(m_szLibraryName,'\0',sizeof(m_szLibraryName));
	m_pImage = NULL;
	m_bDrawTransparent = FALSE;
	m_cr = RGB(255,0,255);
	m_bValidDib = FALSE;
}

/*
	Load()

	Carica l'immagine dalla risorsa, creando l'oggetto immagine con la libreria specificata.
*/
BOOL CDibCtrl::Load(UINT nID,LPCSTR lpcszLibraryName,BOOL bRebuildPalette/*=TRUE*/,BOOL bClearBeforeDraw/*=TRUE*/)
{
	CWaitCursor cursor;
	m_bValidDib = FALSE;

	// crea l'oggetto immagine
	if(!m_pImage)
	{
		char szLibraryName[_MAX_PATH+1];
		strcpyn(szLibraryName,lpcszLibraryName,sizeof(szLibraryName));
		m_pImage = m_ImageFactory.Create(szLibraryName,sizeof(szLibraryName));
	}
	
	// carica l'immagine nel controllo
	if(m_pImage)
	{
		HBITMAP hBmp = (HBITMAP)::LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(nID),IMAGE_BITMAP,0,0,LR_DEFAULTCOLOR);
		HDIB hDib = m_pImage->BitmapToDIB(hBmp,NULL);
		if(m_pImage->SetDIB(hDib))
		{
			m_bValidDib = TRUE;

			// usa lo stesso oggetto per ogni immagine, per cui occhio con il parametro per ricalcolare la paletta
			m_ImageDraw.SetImage(m_pImage);
			
			if(bRebuildPalette)
			{
				m_ImageDraw.BuildLogPalette();
				RealizePalette(FALSE);
			}

			Paint(bClearBeforeDraw);
		}
	}

	if(!m_bValidDib)
		Clear();

	return(m_bValidDib);
}

/*
	Load()

	Carica l'immagine dal file nel controllo, creando l'oggetto immagine con la libreria specificata.
*/
BOOL CDibCtrl::Load(LPCSTR lpszFileName,LPCSTR lpcszLibraryName,BOOL bRebuildPalette/*=TRUE*/,BOOL bClearBeforeDraw/*=TRUE*/)
{
	CWaitCursor cursor;
	m_bValidDib = FALSE;

	// controlla che sia un file
	if(!(::GetFileAttributes(lpszFileName) & FILE_ATTRIBUTE_DIRECTORY))
	{
		// verifica se e' stata specificata una libreria differente
		if(m_szLibraryName[0]=='\0')
			strcpyn(m_szLibraryName,lpcszLibraryName,sizeof(m_szLibraryName));
		if(stricmp(m_szLibraryName,lpcszLibraryName)!=0)
		{
			m_ImageFactory.Delete();
			m_pImage = NULL;
			strcpyn(m_szLibraryName,lpcszLibraryName,sizeof(m_szLibraryName));
		}

		// crea l'oggetto immagine
		if(!m_pImage)
			m_pImage = m_ImageFactory.Create(m_szLibraryName,sizeof(m_szLibraryName));
		
		// carica l'immagine nel controllo
		if(m_pImage)
		{
			if(m_pImage->IsSupportedFormat(lpszFileName))
				if(m_pImage->Load(lpszFileName))
				{
					m_bValidDib = TRUE;

					// usa lo stesso oggetto per ogni immagine, per cui occhio con il parametro per ricalcolare la paletta
					m_ImageDraw.SetImage(m_pImage);
					
					if(bRebuildPalette)
					{
						m_ImageDraw.BuildLogPalette();
						RealizePalette(FALSE);
					}

					Paint(bClearBeforeDraw);
				}
		}
	}

	if(!m_bValidDib)
		Clear();

	return(m_bValidDib);
}

/*
	Load()

	Carica l'oggetto immagine nel controllo.
*/
BOOL CDibCtrl::Load(CImage* pImage,BOOL bRebuildPalette/*=TRUE*/,BOOL bClearBeforeDraw/*=TRUE*/)
{
	CWaitCursor cursor;
	m_bValidDib = FALSE;
	
	// verifica se e' stata chiamata la Load() da file
	if(!strnull(m_szLibraryName))
	{
		m_ImageFactory.Delete();
		m_pImage = NULL;
		memset(m_szLibraryName,'\0',sizeof(m_szLibraryName));
	}

	// se e' stato passato un oggetto valido
	if(pImage)
	{
		m_bValidDib = TRUE;

		// non crea nessun oggetto nuovo ma usa quello ricevuto in input
		m_pImage = pImage;
		
		// usa lo stesso oggetto per ogni immagine, per cui occhio con il parametro per ricalcolare la paletta
		m_ImageDraw.SetImage(m_pImage);
		
		if(bRebuildPalette)
		{
			m_ImageDraw.BuildLogPalette();
			RealizePalette(FALSE);
		}
		
		Paint(bClearBeforeDraw);
	}

	if(!m_bValidDib)
		Clear();

	return(m_bValidDib);
}

/*
	Unload()
	
	Scarica l'oggetto immagine dal controllo.
*/
void CDibCtrl::Unload(void)
{
	m_bValidDib = FALSE;
	Clear();
}

/*
	Clear()
	
	Pulisce l'area utilizzata dall'immagine.
*/
void CDibCtrl::Clear(void)
{
	if(this->m_hWnd)
	{
		CClientDC dc(this);
		CRect rcPaint;
		GetClientRect(&rcPaint);
		rcPaint.InflateRect(-1,-1);
		//dc.FillSolidRect(&rcPaint, m_bValidDib ? ::GetSysColor(COLOR_3DFACE) : ::GetSysColor(COLOR_3DSHADOW));
		dc.FillSolidRect(&rcPaint,::GetSysColor(COLOR_3DFACE));
	}
}

/*
	Paint()

	Disegna l'immagine nel controllo.
*/
void CDibCtrl::Paint(BOOL bInvalidate/*=TRUE*/)
{
	if(bInvalidate)
		Clear();

	if(this->m_hWnd && m_bValidDib)
	{
		CRect rcPaint;
		GetClientRect(&rcPaint);
		rcPaint.InflateRect(-1,-1);

		CClientDC dc(this);
		double nStretchFactor = 1.0;
		
		if((int)m_pImage->GetWidth() > rcPaint.Width() || (int)m_pImage->GetHeight() > rcPaint.Height())
		{
			// the bitmap doesn't fit, scale to fit
			double rx = (double)m_pImage->GetWidth() / (double)rcPaint.Width();
			double ry = (double)m_pImage->GetHeight() / (double)rcPaint.Height();
			nStretchFactor = 1.0 / max(rx,ry);
		}

		int w,h,nDestX,nDestY;
		
		w = rcPaint.right - rcPaint.left;
		h = rcPaint.bottom - rcPaint.top;
		nDestX = rcPaint.left + (w - int(nStretchFactor * m_pImage->GetWidth())) / 2;
		nDestY = rcPaint.top  + (h - int(nStretchFactor * m_pImage->GetHeight())) / 2;

		//ok
		//m_ImageDraw.DrawStretch(dc.GetSafeHdc(),nDestX,nDestY,nStretchFactor,NULL,TRUE,COLORONCOLOR,/*DRAW_STRETCHBLT*/DRAW_STRETCHDIBITS,SRCCOPY,TRUE);
		
		// non funziona con la GetBitmap() (derivata) di nexgen
		//m_ImageDraw.DrawStretch(dc.GetSafeHdc(),nDestX,nDestY,nStretchFactor,NULL,TRUE,COLORONCOLOR,DRAW_STRETCHBLT/*DRAW_STRETCHDIBITS*/,SRCCOPY,TRUE);

		m_ImageDraw.DrawStretch(	dc.GetSafeHdc(),
							nDestX,
							nDestY,
							nStretchFactor,
							NULL,
							TRUE,
							COLORONCOLOR,
							m_bDrawTransparent ? DRAW_TRANSPARENTBLT : /*DRAW_STRETCHBLT*/DRAW_STRETCHDIBITS,
							m_bDrawTransparent ? m_cr : SRCCOPY,
							TRUE
							);
	}
}

/*
	CtlColor()
*/
HBRUSH CDibCtrl::CtlColor(CDC* /*pDC*/,UINT /*nCtlColor*/)
{
	Paint();

	// return a non-NULL brush if the parent's handler should not be called
	return((HBRUSH)::GetStockObject(NULL_BRUSH));
}

/*
	OnQueryNewPalette()
*/
BOOL CDibCtrl::OnQueryNewPalette(void)
{
	// we actually never get to see the palette negotiation messages, which are not forwarded to us by the MFC
	// mainframe, so we just take our chances in Load()
	return(RealizePalette(FALSE));
}

/*
	OnPaletteChanged()
*/
void CDibCtrl::OnPaletteChanged(CWnd* /*pFocusWnd*/)
{
	RealizePalette(TRUE);
}

/*
	RealizePalette()
*/
BOOL CDibCtrl::RealizePalette(BOOL bForceBackGround)
{
	if(this->m_hWnd && m_bValidDib)
	{
		CPalette* pPal = CPalette::FromHandle(m_ImageDraw.GetLogPalette());
		if(!pPal)
			return(FALSE);

		CClientDC dc(this);
		CPalette* pOldPalette = dc.SelectPalette(pPal,bForceBackGround);
		UINT nChanged = dc.RealizePalette();
		dc.SelectPalette(pOldPalette,TRUE);

		if(nChanged==0) // no change to our mapping
			return(FALSE);

		// some changes have been made; invalidate
		Paint();
	}

	return(TRUE);
}
