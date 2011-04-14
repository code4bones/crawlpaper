/*
	CWinBmpEx.cpp
	Classe derivata per gestione BMP (MFC).
	Luca Piergentili, 07/08/00
	l.pierge@terra.es

	Tratto da:
	WinBmpEx.h,v 1.2 2000/03/30 21:47:41 Ulrich von Zadow
	PaintLib windows DrawDib bitmap class implementation.
	See Paul DiLascia's excellent january and march ´97 MSJ
	articles. This version optionally draws using the 'video for
	windows' DrawDib API. This is supposedly faster, and offers
	dithering, too.
	Copyright (c) 1998-2000 Bernard Delmée
*/
#ifndef _CWINBMPEX_H
#define _CWINBMPEX_H 1

#include "window.h"
#ifndef _WITHOUT_VFW_
#include <vfw.h>
#pragma comment(lib,"vfw32.lib")
#endif
//#include "dibsect.h"
#include "paintlib.h"

class CWinBmpEx: public CDIBSection
{
public:
	CWinBmpEx();
	virtual ~CWinBmpEx();

	// extended Draw function - can use DrawDib or not
	// using any stretch-mode (but the default) forces GDI usage
	BOOL DrawEx(HDC,const RECT* = NULL,const RECT* = NULL, HPALETTE = NULL,BOOL = TRUE,int = -1,DWORD = SRCCOPY);

	// draws the bitmap with top left corner at specified location
	virtual void Draw(HDC,int,int,DWORD = -1);

	// draws the bitmap on the given device context
	// scales the bitmap by factor
	virtual void StretchDraw(HDC,int,int,double,DWORD = -1);

	// draws the bitmap on the given device context
	// scales the bitmap so w is the width and h the height
	virtual void StretchDraw(HDC,int,int,int,int,DWORD = -1);

	// draws a portion of the bitmap on the given device context
	virtual BOOL DrawExtract(HDC,POINT,RECT);

	// call this whenever recycling the bitmap
	void BuildLogPalette(void);

	// retrieve windows logical palette.
	HPALETTE GetLogPalette(void);

private:
	HPALETTE	CreatePalette(void);

	HPALETTE	m_pal;			// logical palette
#ifndef _WITHOUT_VFW_
	HDRAWDIB	m_hdd;			// for DrawDib API
#endif
};

#endif // _CWINBMPEX_H
