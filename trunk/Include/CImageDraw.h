/*
	CImageDraw.h
	Classe per la visualizzazione dell'immagine (SDK/MFC).
	Luca Piergentili, 20/09/00
	lpiergentili@yahoo.com

	Riadattata e modificata dal codice presente nel progetto paintlib
	(piclook) per l'integrazione con l'oggetto di tipo immagine.
*/
#ifndef _CIMAGEDRAW_H
#define _CIMAGEDRAW_H 1

#include "macro.h"
#include "window.h"
#include "CImageDraw.config.h"
#include "CImage.h"
#include "CImageFactory.h"
// per DrawDib...()
#include <vfw.h>
#ifdef _VFW_DYNAMIC_LOAD
#ifdef PRAGMA_MESSAGE_VERBOSE
  #pragma message("\t\t\t"__FILE__"("STR(__LINE__)"): using msvfw32.dll (dynamic loading)")
#endif
#else
  #pragma comment(lib,"vfw32.lib")
#ifdef PRAGMA_MESSAGE_VERBOSE
  #pragma message("\t\t\t"__FILE__"("STR(__LINE__)"): using vfw32.lib (static linkage)")
  #pragma message("\t\t\t"__FILE__"("STR(__LINE__)"): automatically linking with vfw32.dll")
#endif
#endif
// per TransparentBlt(), in wingdi.h/windows.h
#pragma comment(lib,"MSImg32.lib")
#ifdef PRAGMA_MESSAGE_VERBOSE
#pragma message("\t\t\t"__FILE__"("STR(__LINE__)"): automatically linking with MSImg32.dll")
#endif

// modalita' visualizzazione (GDI/VFW)
#define	DRAW_STRETCHDIBITS	0
#define	DRAW_STRETCHBLT	1
#define	DRAW_TRANSPARENTBLT	2
#define	DRAW_VFWDRAWDIB	3

// macro generiche
#define	WIDTH_BYTES(bits)	(((bits)+31)/32*4)
#define	RECTWIDTH(lpRect)	((lpRect)->right - (lpRect)->left)
#define	RECTHEIGHT(lpRect)	((lpRect)->bottom - (lpRect)->top)
#define	PALVERSION		0x300

// prototipi per le funzioni della dll
#ifdef _VFW_DYNAMIC_LOAD
  typedef HDRAWDIB	(VFWAPI *PFNDRAWDIBOPEN)	(VOID);
  typedef BOOL		(VFWAPI *PFNDRAWDIBDRAW)	(HDRAWDIB hdd,HDC hdc,int xDst,int yDst,int dxDst,int dyDst,LPBITMAPINFOHEADER lpbi,LPVOID lpBits,int xSrc,int ySrc,int dxSrc,int dySrc,UINT wFlags);
  typedef BOOL		(VFWAPI *PFNDRAWDIBCLOSE)(HDRAWDIB hdd);
#endif

/*
	CImageDraw
*/
class CImageDraw
{
public:
	CImageDraw();
	virtual ~CImageDraw();

	// extended draw function
	BOOL			DrawEx(	HDC			hDc,
						const RECT*	rcDst = NULL,
						const RECT*	rcSrc = NULL,
						HPALETTE		hPalette = NULL,
						BOOL			bForeground = TRUE,
						int			nStretchMode = COLORONCOLOR,
						int			nDrawMode = DRAW_STRETCHDIBITS,
						DWORD		dwRopCode = SRCCOPY,
						BOOL			bRebuildPalette = FALSE
						);
    
	// draws the bitmap with top left corner at specified location
	virtual BOOL	Draw(	HDC			hDc,
						int			x,
						int			y,
						HPALETTE		hPalette = NULL,
						BOOL			bForeground = TRUE,
						int			nStretchMode = COLORONCOLOR,
						int			nDrawMode = DRAW_STRETCHDIBITS,
						DWORD		dwRopCode = SRCCOPY,
						BOOL			bRebuildPalette = FALSE
						);
    
	// draws a portion of the bitmap on the given device context
	virtual BOOL	DrawPortion(HDC		hDC,
						POINT		pntDest,
						RECT			rcSrc,
						HPALETTE		hPalette = NULL,
						BOOL			bForeground = TRUE,
						int			nStretchMode = COLORONCOLOR,
						int			nDrawMode = DRAW_STRETCHDIBITS,
						DWORD		dwRopCode = SRCCOPY,
						BOOL			bRebuildPalette = FALSE
						);

	// draws the bitmap on the given device context (scales the bitmap so w is the width and h the height)
	virtual BOOL	DrawStretch(HDC		hDC,
						int			x,
						int			y,
						int			w,
						int			h,
						HPALETTE		hPalette = NULL,
						BOOL			bForeground = TRUE,
						int			nStretchMode = COLORONCOLOR,
						int			nDrawMode = DRAW_STRETCHDIBITS,
						DWORD		dwRopCode = SRCCOPY,
						BOOL			bRebuildPalette = FALSE
						);

	// draws the bitmap on the given device context (scales the bitmap by factor)
	virtual BOOL	DrawStretch(HDC		hDC,
						int			x,
						int			y,
						double		factor,
						HPALETTE		hPalette = NULL,
						BOOL			bForeground = TRUE,
						int			nStretchMode = COLORONCOLOR,
						int			nDrawMode = DRAW_STRETCHDIBITS,
						DWORD		dwRopCode = SRCCOPY,
						BOOL			bRebuildPalette = FALSE
						);

	inline const CImage*	GetImage			(void) const {return(m_pImage);}
	inline void			SetImage			(CImage *pImage) {m_pImage = pImage;}
	
	BOOL					SetBitmap			(void);
	HBITMAP				DIBToBitmap		(BITMAPINFO *bmi,const void *pBits,HPALETTE hPalette);

	HPALETTE				GetLogPalette		(void);
	HPALETTE				BuildLogPalette	(void);
	HPALETTE				CreatePalette		(void);

private:
	HPALETTE				CreateDIBPalette	(LPBITMAPINFO lpBitmapInfo);
	WORD					GetPaletteSize		(LPSTR lpbi);
	WORD					GetDIBNumColors	(LPSTR lpbi);

	HPALETTE				m_hPalette;
	HBITMAP				m_hBitmap;
	CImage*				m_pImage;
	HDRAWDIB				m_hDrawDib;

#ifdef _VFW_DYNAMIC_LOAD
	static int			m_nRefCount;
	static HINSTANCE		m_hVfwDll;
	static PFNDRAWDIBOPEN	m_lpfnDrawDibOpen;
	static PFNDRAWDIBDRAW	m_lpfnDrawDibDraw;
	static PFNDRAWDIBCLOSE	m_lpfnDrawDibClose;
#endif
};

#endif // _CIMAGEDRAW_H
