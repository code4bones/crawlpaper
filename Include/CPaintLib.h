/*
	CPaintLib.h
	Classe derivata per interfaccia con paintlib.
	Luca Piergentili, 01/09/00
	lpiergentili@yahoo.com

	Ad memoriam - Nemo me impune lacessit.
*/
#ifndef _CPAINTLIB_H
#define _CPAINTLIB_H 1

#include "window.h"
#include "ImageConfig.h"

#ifdef HAVE_PAINTLIB_LIBRARY

#include "ImageLibraryName.h"
#include "CImage.h"
#include "CImageParams.h"
#include "paintlib.h"

/*
	CPaintLib
*/
class CPaintLib : public CImage
{
public:
	CPaintLib();
	virtual ~CPaintLib() {}

	LPCSTR		GetLibraryName		(void);
	
	UINT			GetWidth			(void);
	UINT			GetHeight			(void);
	
	float		GetXRes			(void);
	float		GetYRes			(void);
	void			SetXRes			(float nXRes);
	void			SetYRes			(float nYRes);
	int			GetURes			(void);
	void			SetURes			(UINT);
	void			GetDPI			(float&,float&);

	int			GetCompression		(void);
	void			SetCompression		(int);

	UINT			GetNumColors		(void);
	UINT			GetBPP			(void);
	UINT			ConvertToBPP		(UINT nBitsPerPixel,UINT nFlags,RGBQUAD *pPalette,UINT nColors);

	COLORREF		GetPixel			(UINT x,UINT y);
	void			SetPixel			(UINT x,UINT y,COLORREF colorref);
	void*		GetPixels			(void);
	LPBITMAPINFO	GetBMI			(void);
	HBITMAP		GetBitmap			(void);
	UINT			GetMemUsed		(void);

	HDIB			GetDIB			(DIBINFO* pDibInfo = NULL);
	BOOL			SetDIB			(HDIB hDib,DIBINFO* pDibInfo = NULL);
	int			GetDIBOrder		(void);
	BOOL			SetPalette		(UINT nIndex,UINT nColors,RGBQUAD* pPalette);

	BOOL			Create			(BITMAPINFO* pBitmapInfo,void *pData = NULL);
	BOOL			Load				(LPCSTR lpcszFileName);
	BOOL			Save				(void);
	BOOL			Save				(LPCSTR lpcszFileName,LPCSTR lpcszFormat,DWORD dwFlags);
	BOOL			Stretch			(RECT& drawRect,BOOL bAspectRatio = TRUE);

	BOOL			SetImageParamsDefaultValues(CImageParams*);
	BOOL			SetImageParamsMinMax(CImageParams*);

	UINT			Grayscale			(CImageParams*);
	UINT			Halftone			(CImageParams*);
	UINT			Invert			(CImageParams*);
	UINT			Rotate90Left		(CImageParams*);
	UINT			Rotate90Right		(CImageParams*);
	UINT			Rotate180			(CImageParams*);
	UINT			Size				(CImageParams*);

protected:
	BOOL			_Load			(LPCSTR lpcszFileName);
	BOOL			_Save			(LPCSTR lpcszFileName,LPCSTR lpcszFormat,DWORD dwFlags);
	void			Rotate			(double);
	BOOL			ConvertToBPP		(UINT nBPP);

	CWinBmp		m_Image;
};

#endif // HAVE_PAINTLIB_LIBRARY
#endif // _CPAINTLIB_H
