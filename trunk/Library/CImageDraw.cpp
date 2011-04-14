/*
	CImageDraw.cpp
	Classe per la visualizzazione dell'immagine (SDK/MFC).
	Luca Piergentili, 20/09/00
	lpiergentili@yahoo.com

	Riadattata e modificata dal codice presente nel progetto paintlib
	(piclook) per l'integrazione con l'oggetto di tipo immagine.
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include "window.h"
#include "CImageDraw.h"

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

// statiche per la classe (comuni a tutte le istanze della classe)
// il contatore per le referenze viene usato per sapere quando caricare/scaricare la dll
#ifdef _VFW_DYNAMIC_LOAD
  int			CImageDraw::m_nRefCount = 0;
  HINSTANCE		CImageDraw::m_hVfwDll = NULL;
  PFNDRAWDIBOPEN	CImageDraw::m_lpfnDrawDibOpen = NULL;
  PFNDRAWDIBDRAW	CImageDraw::m_lpfnDrawDibDraw = NULL;
  PFNDRAWDIBCLOSE	CImageDraw::m_lpfnDrawDibClose = NULL;
#endif

/*
	CImageDraw()
*/
CImageDraw::CImageDraw()
{
	m_hPalette = 0;
	m_hBitmap = 0;
	m_pImage = NULL;
	m_hDrawDib = 0;

#ifdef _VFW_DYNAMIC_LOAD
	// carica la dll solo alla prima chiamata (l'handle viene condiviso da tutte le istanze della classe)
	if(m_nRefCount++==0)
	{
		if((m_hVfwDll = ::LoadLibrary("MsVfw32.dll"))!=(HINSTANCE)NULL)
		{
			m_lpfnDrawDibOpen  = (PFNDRAWDIBOPEN)::GetProcAddress(m_hVfwDll,"DrawDibOpen");
			m_lpfnDrawDibDraw  = (PFNDRAWDIBDRAW)::GetProcAddress(m_hVfwDll,"DrawDibDraw");
			m_lpfnDrawDibClose = (PFNDRAWDIBCLOSE)::GetProcAddress(m_hVfwDll,"DrawDibClose");
		}
	}
#endif
}

/*
	~CImageDraw()
*/
VIRTUAL CImageDraw::~CImageDraw()
{
	if(m_hPalette)
		::DeleteObject(m_hPalette);

#ifdef _VFW_DYNAMIC_LOAD
	if(m_lpfnDrawDibClose && m_hDrawDib)
		m_lpfnDrawDibClose(m_hDrawDib);

	// scarica la dll quando non esistono piu' istanze della classe
	if(--m_nRefCount==0)
	{
		if(m_hVfwDll)
		{
			::FreeLibrary((HMODULE)m_hVfwDll),m_hVfwDll = NULL;
			m_lpfnDrawDibOpen = NULL;
			m_lpfnDrawDibDraw = NULL;
			m_lpfnDrawDibClose = NULL;
		}
	}
#else
	if(m_hDrawDib)
		::DrawDibClose(m_hDrawDib);
#endif
}

/*
	DrawEx()

	Visualizza il contenuto del DIB nel dispositivo di contesto del chiamante.

	HDC			hDc				handle del dispositivo	
	const RECT*	rcDst			area sorgente, se NULL usa la dimensione del bitmap
	const RECT*	rcSrc			area destinazione, se NULL usa l'area sorgente
	HPALETTE		hPalette			se NULL usa la paletta dell'immagine
	BOOL			bForeground		realizza in primo piano (TRUE) se si gestiscono i messaggi relativi alla paletta bisogna
								usare FALSE dato che la paletta dovra' realizzarsi nella gestione di WM_QUERYNEWPALETTE
	int			nStretchMode		modalita' per lo stretch
	int			nDrawMode			modalita' per visualizzazione (DRAW_STRETCHDIBITS, DRAW_STRETCHBLT, DRAW_TRANSPARENTBLT, DRAW_VFWDRAWDIB)
	DWORD		dwRopCode			modalita' rasterizzazione/colore per trasparenza
	BOOL			bRebuildPalette	flag per ricalcolo paletta
*/
BOOL CImageDraw::DrawEx(HDC hDc,const RECT* rcDst/*=NULL*/,const RECT* rcSrc/*=NULL*/,HPALETTE hPalette/*=NULL*/,BOOL bForeground/*=TRUE*/,int nStretchMode/*=COLORONCOLOR*/,int nDrawMode/*=DRAW_STRETCHDIBITS*/,DWORD dwRopCode/*=SRCCOPY*/,BOOL bRebuildPalette/*=FALSE*/)
{
	BOOL bRet = FALSE;

	// controlla che l'oggetto immagine sia valido (deve venir impostato prima di qualsiasi operazione)
	if(!m_pImage)
		return(bRet);

	// paletta
	HPALETTE hOldPalette;
	if(!hPalette)
		hPalette = bRebuildPalette ? BuildLogPalette() : GetLogPalette();
	hOldPalette = ::SelectPalette(hDc,hPalette,!bForeground);
	::RealizePalette(hDc);

	// controlla i rettangoli
	if(!rcSrc)
	{
		RECT rc;
		rc.left = rc.top = 0;
		rc.right = m_pImage->GetWidth();
		rc.bottom = m_pImage->GetHeight();
		rcSrc = &rc;
	}
	if(!rcDst)
		rcDst = rcSrc;

gdi_vfw:

	// VFW
	if(nDrawMode==DRAW_VFWDRAWDIB)
	{
		if(!m_hDrawDib)
		{
#ifdef _VFW_DYNAMIC_LOAD
			if(!m_hVfwDll || !m_lpfnDrawDibOpen || !m_lpfnDrawDibDraw || !m_lpfnDrawDibClose)
			{
				nDrawMode = DRAW_STRETCHDIBITS;
				goto gdi_vfw;
			}
			
			m_hDrawDib = m_lpfnDrawDibOpen();
#else
			m_hDrawDib = ::DrawDibOpen();
#endif
		}

		if(m_pImage->LockData())
		{
			BITMAPINFO* pBitmapInfo = m_pImage->GetBMI();
			BITMAPINFOHEADER* pBitmapInfoHeader = &pBitmapInfo->bmiHeader;
			BOOL bInverted = FALSE;

			// la zoccola di VFW non accetta i bitmap invertiti
			if(pBitmapInfoHeader->biHeight < 0)
			{
				pBitmapInfoHeader->biHeight *= m_pImage->GetDIBOrder();
				m_pImage->MirrorHorizontal(NULL);
				bInverted = TRUE;
			}

#ifdef _VFW_DYNAMIC_LOAD
			bRet = m_lpfnDrawDibDraw(
#else
			bRet = ::DrawDibDraw(
#endif
							m_hDrawDib,
							hDc,
							rcDst->left,
							rcDst->top,
							rcDst->right - rcDst->left,
							rcDst->bottom - rcDst->top,
							pBitmapInfoHeader,
							m_pImage->GetPixels(),
							rcSrc->left,
							rcSrc->top,
							rcSrc->right - rcSrc->left,
							rcSrc->bottom - rcSrc->top,
							0 //DDF_SAME_HDC
							//bForeground ? 0 : DDF_BACKGROUNDPAL
							);

			if(bInverted)
			{
				pBitmapInfoHeader->biHeight *= m_pImage->GetDIBOrder();
				m_pImage->MirrorHorizontal(NULL);
			}

			m_pImage->UnlockData();
		}
	}
	// GDI
	else
	{
		// try to use best looking GDI mode
		if(nStretchMode < 0)
			nStretchMode = HALFTONE;

		int nPreviousStretchMode = 0;
		if(nStretchMode > 0)
			nPreviousStretchMode = ::SetStretchBltMode(hDc,nStretchMode);
	
		// stretchdibits (slow?)
		if(nDrawMode==DRAW_STRETCHDIBITS)
		{
			if((bRet = m_pImage->Draw(hDc,(RECT*)rcDst,(RECT*)rcSrc))==FALSE)
			{
				if(m_pImage->LockData())
				{
					bRet = GDI_ERROR!=::StretchDIBits(	hDc,
												rcDst->left,
												rcDst->top,
												rcDst->right - rcDst->left,
												rcDst->bottom - rcDst->top,
												rcSrc->left,
												m_pImage->GetHeight()-rcSrc->bottom,
												(rcSrc->right - rcSrc->left),
												(rcSrc->bottom - rcSrc->top),
												m_pImage->GetPixels(),
												(BITMAPINFO*)m_pImage->GetBMI(),
												DIB_RGB_COLORS,
												dwRopCode
												);
					m_pImage->UnlockData();
				}
			}
		}
		// stretchblt (fast?)
		else if(nDrawMode==DRAW_STRETCHBLT || nDrawMode==DRAW_TRANSPARENTBLT)
		{
			if((bRet = m_pImage->Draw(hDc,(RECT*)rcDst,(RECT*)rcSrc))==FALSE)
			{
				HDC hSrcDC = ::CreateCompatibleDC(hDc);
				HGDIOBJ hOldBmp = ::SelectObject(hSrcDC,m_pImage->GetBitmap());

				// uses associated colormap (paintlib):
				// if(m_pClrTab)
				//	(void)::SetDIBColorTable(hSrcDC,0,256,(RGBQUAD *)m_pClrTab);
				if(GetDIBNumColors((LPSTR)m_pImage->GetBMI()))
				{
					RGBQUAD* pClrTab = ((LPBITMAPINFO)m_pImage->GetBMI())->bmiColors;
					(void)::SetDIBColorTable(hSrcDC,0,256,(RGBQUAD*)pClrTab);
				}
				
				if(nDrawMode==DRAW_STRETCHBLT)
					bRet = ::StretchBlt(hDc,
									rcDst->left,
									rcDst->top,
									rcDst->right  - rcDst->left,
									rcDst->bottom - rcDst->top,
									hSrcDC,
									rcSrc->left,
									rcSrc->top,
									rcSrc->right  - rcSrc->left,
									rcSrc->bottom - rcSrc->top,
									dwRopCode	// modalita'
									);
				else if(nDrawMode==DRAW_TRANSPARENTBLT)
					bRet = ::TransparentBlt(hDc,
									rcDst->left,
									rcDst->top,
									rcDst->right  - rcDst->left,
									rcDst->bottom - rcDst->top,
									hSrcDC,
									rcSrc->left,
									rcSrc->top,
									rcSrc->right  - rcSrc->left,
									rcSrc->bottom - rcSrc->top,
									dwRopCode	// trasparenza
									);

				::SelectObject(hSrcDC,hOldBmp);
				::DeleteDC(hSrcDC);
			}
		}

		// restore GDI stretch-mode
		if(nPreviousStretchMode) 
			(void)::SetStretchBltMode(hDc,nPreviousStretchMode);
	}

	// ripristina la paletta originale del DC
	if(hOldPalette)
		::SelectPalette(hDc,hOldPalette,TRUE);

	return(bRet);
}

/*
	Draw()
	
	Draw DIB on DC
*/
VIRTUAL BOOL CImageDraw::Draw(HDC hDc,int x,int y,HPALETTE hPalette/*=NULL*/,BOOL bForeground/*=TRUE*/,int nStretchMode/*=COLORONCOLOR*/,int nDrawMode/*=DRAW_STRETCHDIBITS*/,DWORD dwRopCode/*=SRCCOPY*/,BOOL bRebuildPalette/*=FALSE*/)
{
	BOOL bRet = FALSE;

	if(m_pImage)
	{
		RECT rcDst;
		rcDst.left   = x;
		rcDst.top    = y;
		rcDst.right  = x + m_pImage->GetWidth();
		rcDst.bottom = y + m_pImage->GetHeight();
	    
		bRet = DrawEx(hDc,&rcDst,NULL,hPalette,bForeground,nStretchMode,nDrawMode,dwRopCode,bRebuildPalette);
	}

	return(bRet);
}

/*
	DrawPortion()

	Draw part of the DIB on a DC
*/
VIRTUAL BOOL CImageDraw::DrawPortion(HDC hDc,POINT pntDest,RECT rcSrc,HPALETTE hPalette/*=NULL*/,BOOL bForeground/*=TRUE*/,int nStretchMode/*=COLORONCOLOR*/,int nDrawMode/*=DRAW_STRETCHDIBITS*/,DWORD dwRopCode/*=SRCCOPY*/,BOOL bRebuildPalette/*=FALSE*/)
{
	BOOL bRet = FALSE;

	if(m_pImage)
	{
		RECT rcDst;
		rcDst.left   = pntDest.x;
		rcDst.top    = pntDest.y;
		rcDst.right  = pntDest.x + abs(rcSrc.right  - rcSrc.left);
		rcDst.bottom = pntDest.y + abs(rcSrc.bottom - rcSrc.top );
		
		bRet = DrawEx(hDc,&rcDst,&rcSrc,hPalette,bForeground,nStretchMode,nDrawMode,dwRopCode,bRebuildPalette);
	}

	return(bRet);
}

/*
	DrawStretch()

	Draws the bitmap on the given device context.
	Scales the bitmap so w is the width and h the height.
*/
VIRTUAL BOOL CImageDraw::DrawStretch(HDC hDC,int x,int y,int w,int h,HPALETTE hPalette/*=NULL*/,BOOL bForeground/*=TRUE*/,int nStretchMode/*=COLORONCOLOR*/,int nDrawMode/*=DRAW_STRETCHDIBITS*/,DWORD dwRopCode/*=SRCCOPY*/,BOOL bRebuildPalette/*=FALSE*/)
{
	BOOL bRet = FALSE;

	if(m_pImage)
	{
		RECT rcSrc,rcDst;
		rcSrc.left   = 0;
		rcSrc.top    = 0;
		rcSrc.right  = m_pImage->GetWidth();
		rcSrc.bottom = m_pImage->GetHeight();
		rcDst.left   = x;
		rcDst.top    = y;
		rcDst.right  = x + w;
		rcDst.bottom = y + h;
		
		bRet = DrawEx(hDC,&rcDst,&rcSrc,hPalette,bForeground,nStretchMode,nDrawMode,dwRopCode,bRebuildPalette);
	}

	return(bRet);
}

/*
	DrawStretch()

	Draws the bitmap on the given device context.
	Scales the bitmap by factor.
*/
VIRTUAL BOOL CImageDraw::DrawStretch(HDC hDc,int x,int y,double factor,HPALETTE hPalette/*=NULL*/,BOOL bForeground/*=TRUE*/,int nStretchMode/*=COLORONCOLOR*/,int nDrawMode/*=DRAW_STRETCHDIBITS*/,DWORD dwRopCode/*=SRCCOPY*/,BOOL bRebuildPalette/*=FALSE*/)
{
	BOOL bRet = FALSE;

	if(m_pImage)
	{
		RECT rcSrc,rcDst;
		rcSrc.left   = 0;
		rcSrc.top    = 0;
		rcSrc.right  = m_pImage->GetWidth();
		rcSrc.bottom = m_pImage->GetHeight();
		rcDst.left   = x;
		rcDst.top    = y;
		rcDst.right  = x + int(factor * m_pImage->GetWidth());
		rcDst.bottom = y + int(factor * m_pImage->GetHeight());

		bRet = DrawEx(hDc,&rcDst,&rcSrc,hPalette,bForeground,nStretchMode,nDrawMode,dwRopCode,bRebuildPalette);
	}

	return(bRet);
}

/*
	SetBitmap()
*/
BOOL CImageDraw::SetBitmap(void)
{
	BOOL bRet = FALSE;

	if(m_pImage)
		if(m_pImage->LockData())
		{
			m_hBitmap = DIBToBitmap(m_pImage->GetBMI(),m_pImage->GetPixels(),GetLogPalette());
			m_pImage->UnlockData();
			bRet = m_hBitmap!=(HBITMAP)NULL;
		}

	return(bRet);
}

/*
	DIBToBitmap()
*/
HBITMAP CImageDraw::DIBToBitmap(BITMAPINFO *pBitmapInfo,const void *pBits,HPALETTE hPalette)
{
	HBITMAP hBitmap = NULL;	// handle to device-dependent bitmap
	HDC hDC = ::GetDC(NULL);	// handle to DC

	if(pBitmapInfo && pBits && hDC)
	{
		// select and realize palette
		HPALETTE hOldPalette = NULL;
		if(hPalette)
			hOldPalette = ::SelectPalette(hDC,hPalette,FALSE);
		::RealizePalette(hDC);

		// create bitmap from DIB info and bits
		hBitmap = ::CreateDIBitmap(hDC,(LPBITMAPINFOHEADER)pBitmapInfo,CBM_INIT,pBits,(LPBITMAPINFO)pBitmapInfo,DIB_RGB_COLORS);

		// restore previous palette
		if(hOldPalette)
			::SelectPalette(hDC,hOldPalette,FALSE);

		// clean up
		::ReleaseDC(NULL,hDC);
	}

	return(hBitmap);
}

/*
	GetLogPalette()
*/
HPALETTE CImageDraw::GetLogPalette(void)
{
	if(!m_hPalette)
		m_hPalette = CreatePalette();

	return(m_hPalette);
}

/*
	BuildLogPalette()
*/
HPALETTE CImageDraw::BuildLogPalette(void)
{
	if(m_hPalette)
	{
		::DeleteObject(m_hPalette);
		m_hPalette = 0;
	}

	m_hPalette = CreatePalette();

	return(m_hPalette);
}

/*
	CreatePalette()
*/
HPALETTE CImageDraw::CreatePalette(void)
{
	HPALETTE hRet = NULL;

	// should not already have palette
	if(!m_hPalette)
	{
		hRet = HPALETTE(0);
		if((hRet = CreateDIBPalette((LPBITMAPINFO)m_pImage->GetBMI()))==NULL)
		{
			// Create the palette. Use halftone palette for hi-color bitmaps.
			// If the file itself did not contain a colormap, we will use a default palette and rely on DrawDib's halftoning.
			// When displaying higher-color images on 8bpp hardware, this is suboptimal though. Ideally we should build the
			// palette according to an histogram of the actual RGB values.
			// "SeeDib" from MS Source Code Samples does that nicely
			HDC hDcScreen = ::GetWindowDC(NULL);
			hRet = ::CreateHalftonePalette(hDcScreen);
			::ReleaseDC(NULL,hDcScreen);
		}
	}

	return(hRet);
}

/*
	CreateDIBPalette()

	Creates a palette from a DIB by allocating memory for the logical palette, reading and storing the colors from the
	DIB's color table into the logical palette, creating a palette from this logical palette, and then returning the
	palette's handle. This allows the DIB to be displayed using the best possible colors (important for DIBs with 256
	or more colors).
*/
HPALETTE CImageDraw::CreateDIBPalette(LPBITMAPINFO lpBitmapInfo)
{
	LPLOGPALETTE lpLogPalette;		// pointer to a logical palette
	HANDLE hLogPalette;				// handle to a logical palette
	HPALETTE hPalette = NULL;		// handle to a palette
	WORD nNumColors;				// number of colors in color table
	LPBITMAPCOREINFO lpBitmapCoreInfo;	// pointer to BITMAPCOREINFO structure (old)
	BOOL bWinStyleDIB;				// flag which signifies whether this is a Win3.0 DIB
	int i;

	if(!lpBitmapInfo)
		return(FALSE);

	// get pointer to BITMAPCOREINFO (old 1.x)
	lpBitmapCoreInfo = (LPBITMAPCOREINFO)lpBitmapInfo;

	// get the number of colors in the DIB
	nNumColors = GetDIBNumColors((LPSTR)lpBitmapInfo);

	if(nNumColors!=0)
	{
		// allocate memory block for logical palette
		hLogPalette = ::GlobalAlloc(GHND,sizeof(LOGPALETTE) + sizeof(PALETTEENTRY) * nNumColors);
		if(!hLogPalette)
			return(FALSE);

		lpLogPalette = (LPLOGPALETTE)::GlobalLock((HGLOBAL)hLogPalette);

		// set version and number of palette entries
		lpLogPalette->palVersion = PALVERSION;
		lpLogPalette->palNumEntries = (WORD)nNumColors;

		// is this a Win 3.0 DIB?
		bWinStyleDIB = IS_WIN30_DIB((LPSTR)lpBitmapInfo);
		for(i = 0; i < (int)nNumColors; i++)
		{
			if(bWinStyleDIB)
			{
				lpLogPalette->palPalEntry[i].peRed   = lpBitmapInfo->bmiColors[i].rgbRed;
				lpLogPalette->palPalEntry[i].peGreen = lpBitmapInfo->bmiColors[i].rgbGreen;
				lpLogPalette->palPalEntry[i].peBlue  = lpBitmapInfo->bmiColors[i].rgbBlue;
				lpLogPalette->palPalEntry[i].peFlags = 0;
			}
			else
			{
				lpLogPalette->palPalEntry[i].peRed   = lpBitmapCoreInfo->bmciColors[i].rgbtRed;
				lpLogPalette->palPalEntry[i].peGreen = lpBitmapCoreInfo->bmciColors[i].rgbtGreen;
				lpLogPalette->palPalEntry[i].peBlue  = lpBitmapCoreInfo->bmciColors[i].rgbtBlue;
				lpLogPalette->palPalEntry[i].peFlags = 0;
			}
		}

		// create the palette and get handle to it
		hPalette = ::CreatePalette(lpLogPalette);
		::GlobalUnlock((HGLOBAL)hLogPalette);
		::GlobalFree((HGLOBAL)hLogPalette);
	}

	return(hPalette);
}

/*
	GetPaletteSize()

	Gets the size required to store the DIB's palette by multiplying the number of colors by the size of an RGBQUAD
	(for a Windows 3.0-style DIB) or by the size of an RGBTRIPLE (for an other-style DIB).
*/
WORD CImageDraw::GetPaletteSize(LPSTR lpbi)
{
	// calculate the size required by the palette
	if (IS_WIN30_DIB (lpbi))
		return((WORD)(GetDIBNumColors(lpbi) * sizeof(RGBQUAD)));
	else
		return((WORD)(GetDIBNumColors(lpbi) * sizeof(RGBTRIPLE)));
}

/*
	GetDIBNumColors()

	Calculates the number of colors in the DIB's color table by finding the bits per pixel for the DIB (whether Win3.0
	or other-style DIB). If bits per pixel is 1: colors=2, if 4: colors=16, if 8: colors=256, if 24, no colors in color table.
*/
WORD CImageDraw::GetDIBNumColors(LPSTR lpbi)
{
	WORD nBitCount;

	// If this is a Windows-style DIB, the number of colors in the color table can be less than the number of bits per pixel
	// allows for (i.e. lpbi->biClrUsed can be set to some value). If this is the case, return the appropriate value.
	if(IS_WIN30_DIB(lpbi))
	{
		DWORD dwClrUsed = ((LPBITMAPINFOHEADER)lpbi)->biClrUsed;
		if(dwClrUsed!=0)
			return((WORD)dwClrUsed);
	}

	// Calculate the number of colors in the color table based on the number of bits per pixel for the DIB.
	if(IS_WIN30_DIB(lpbi))
		nBitCount = ((LPBITMAPINFOHEADER)lpbi)->biBitCount;
	else
		nBitCount = ((LPBITMAPCOREHEADER)lpbi)->bcBitCount;

	// return number of colors based on bits per pixel
	switch(nBitCount)
	{
		case 1:
			return 2;
		case 4:
			return 16;
		case 8:
			return 256;
		default:
			return 0;
	}
}
