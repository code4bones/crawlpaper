#if 0
/*
/--------------------------------------------------------------------
|
|      $Id: WinBmpEx.h,v 1.2 2000/03/30 21:47:41 Ulrich von Zadow Exp $
|	     PaintLib windows DrawDib bitmap class implementation
|
|      See Paul DiLascia's excellent january and march ´97 MSJ
|      articles. This version optionally draws using the 'video for
|      windows' DrawDib API. This is supposedly faster, and offers
|      dithering, too.
|
|      Copyright (c) 1998-2000 Bernard Delmée
|
\--------------------------------------------------------------------
*/
#include "ImageConfig.h"
#ifdef HAVE_PAINTLIB_LIBRARY

#ifndef INCL_DRAWDIB
#define INCL_DRAWDIB

#pragma message("including "__FILE__)

#include "dibsect.h"

#ifndef _WITHOUT_VFW_
#include "vfw.h"
#endif

class CWinBmpEx: public CDIBSection
{
public:
    CWinBmpEx();
    virtual ~CWinBmpEx();

    //! Extended Draw function; can use DrawDib or not.
    //! Using any stretch-mode (but the default) forces GDI usage
    BOOL DrawEx(HDC dc, const RECT* rcDst=NULL, const RECT* rcSrc=NULL,
                HPALETTE hPal=NULL, BOOL bForeground=TRUE,
		int StretchMode=-1, DWORD rop=SRCCOPY);
    
    //! Draws the bitmap with top left corner at specified location.
    virtual void Draw( HDC dc, int x, int y, DWORD rop = -1 );

    //! Draws the bitmap on the given device context.
    //! Scales the bitmap by Factor.
    virtual void StretchDraw (HDC hDC, int x, int y, 
	double Factor, DWORD rop = -1);

    //! Draws the bitmap on the given device context.
    //! Scales the bitmap so w is the width and h the height.
    virtual void StretchDraw (HDC hDC, int x, int y, 
	int w, int h, DWORD rop = -1);
    
    //! Draws a portion of the bitmap on the given device context
    virtual BOOL DrawExtract( HDC hDC, POINT pntDest, RECT rcSrc );

    //! Call this whenever recycling the bitmap
    void BuildLogPalette();

    //! Retrieve windows logical palette.
    HPALETTE GetLogPalette();

private:
    HPALETTE m_pal;    // logical palette

    HPALETTE CreatePalette();

// if we ever wanted to build without pulling in vfw 
#ifndef _WITHOUT_VFW_
    HDRAWDIB m_hdd;    // for DrawDib API
#endif
};

#endif // INCL_DRAWDIB

/*
/--------------------------------------------------------------------
|
|      $Log: WinBmpEx.h,v $
|      Revision 1.2  2000/03/30 21:47:41  Ulrich von Zadow
|      Added zoom-in mode, CWinBmpEx, conditional use of DrawDIB
|      and some other nice stuff by Bernard Delmée.
|
|
|
\--------------------------------------------------------------------
*/
#endif // HAVE_PAINTLIB_LIBRARY
#endif
