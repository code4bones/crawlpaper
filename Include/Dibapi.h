// dibapi.h
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-1997 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.
#if 0

#ifndef _INC_DIBAPI
#define _INC_DIBAPI

//LPI
#include "window.h"
//$LPI

#ifndef _HDIB_DECLARED
   DECLARE_HANDLE(HDIB);
   #define _HDIB_DECLARED 1
#endif

DECLARE_HANDLE(HMEMBMPFILE);

//LPI

/* DIB constants */
#define PALVERSION      0x300
#define MAXPALCOLORS    256

/* DIB Macros*/
#define RECTWIDTH(lpRect)     ((lpRect)->right - (lpRect)->left)
#define RECTHEIGHT(lpRect)    ((lpRect)->bottom - (lpRect)->top)
//$LPI

BOOL        WINAPI  PaintDIB			(HDC,LPRECT,HDIB,LPRECT,CPalette* pPal,BOOL bStretch);
BOOL        WINAPI  DrawDIB			(HDC,LPRECT,HDIB,LPRECT,CPalette* pPal);
BOOL        WINAPI  CreateDIBPalette	(HDIB hDIB,CPalette* cPal);
LPSTR       WINAPI  FindDIBBits		(LPSTR lpbi);
DWORD       WINAPI  DIBWidth			(LPSTR lpDIB);
DWORD       WINAPI  DIBHeight			(LPSTR lpDIB);
WORD        WINAPI  PaletteSize		(LPSTR lpbi);
WORD        WINAPI  DIBNumColors		(LPSTR lpbi);
HGLOBAL     WINAPI  CopyHandle		(HGLOBAL h);
LOGPALETTE* WINAPI	CreateLogPalette	(HPALETTE hPal);
void        WINAPI  DestroyLogPalette	(LOGPALETTE* pLP);
HDIB        WINAPI  BitmapToDIB		(HBITMAP hBitmap,HPALETTE hPal);
HBITMAP     WINAPI  DIBToBitmap		(HDIB hDib,HPALETTE hPal);
BOOL        WINAPI  SaveDIBFile		(HDIB hDib,CFile& file);
HDIB        WINAPI  ReadDIBFile		(CFile& file);
HDIB        WINAPI	MemBmpFileToDIB	(HMEMBMPFILE hMemBmpFile);
HMEMBMPFILE WINAPI	DIBToMemBmpFile	(HDIB hDib); 
HDIB        WINAPI  WindowToDIB		(CWnd *pWnd,CRect* pScreenRect );

#endif // _INC_DIBAPI
#endif
