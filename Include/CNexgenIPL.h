/*
	CNexgenIPL.h
	Classe derivata per interfaccia con NexgenIPL (v.2.9.6).
	Luca Piergentili, 01/09/00
	lpiergentili@yahoo.com

	Ad memoriam - Nemo me impune lacessit.
*/
#ifndef _CNEXGENIPL_H
#define _CNEXGENIPL_H 1

#include "window.h"
#include "ImageConfig.h"

#ifdef HAVE_NEXGENIPL_LIBRARY

#include "ImageLibraryName.h"
#include "CImage.h"
#include "CImageParams.h"
#include "NexgenIPL.h"

/*
	CNexgenIPL
*/
class CNexgenIPL : public CImage 
{
public:
	CNexgenIPL();
	virtual ~CNexgenIPL() {}

	LPCSTR		GetLibraryName		(void) {return(NEXGENIPL_LIB_NAME);}
	
	UINT			GetWidth			(void) {return(m_pImage ? m_pImage->GetWidth() : 0);}
	UINT			GetHeight			(void) {return(m_pImage ? m_pImage->GetHeight() : 0);}
	
	BOOL			GetQualityRange	(int& nMin,int& nMax);

	UINT			GetNumColors		(void) {return(m_pImage ? m_pImage->GetNumColorEntries(GetBPP()) : 0);}
	UINT			GetBPP			(void) {return(m_pImage ? m_pImage->GetBitsPerPixel() : 0);}
	UINT			ConvertToBPP		(UINT nBitsPerPixel,UINT nFlags,RGBQUAD* pPalette,UINT nColors);

	COLORREF		GetPixel			(UINT x,UINT y) {return(m_pImage ? m_pImage->GetColorFromPixel(x,y) : RGB(255,255,255));}
	void			SetPixel			(UINT x,UINT y,COLORREF colorref) {if(m_pImage) m_pImage->SetColorForPixel(x,y,colorref);}
	void*		GetPixels			(void) {return(m_pImage ? m_pImage->GetBits() : NULL);}
	LPBITMAPINFO	GetBMI			(void) {return(m_pImage ? m_pImage->GetBitmapInfo() : NULL);}
	HBITMAP		GetBitmap			(void) {return(m_pImage ? m_pImage->GetBitmap() : NULL);}
	UINT			GetMemUsed		(void) {return(m_pImage ? m_pImage->GetImageSize(m_pImage->GetWidth(),m_pImage->GetHeight(),m_pImage->GetBitsPerPixel()) : 0);}

	HDIB			GetDIB			(DIBINFO* = NULL);
	BOOL			SetDIB			(HDIB hDib,DIBINFO* = NULL);
	int			GetDIBOrder		(void) {return(-1);}
	BOOL			SetPalette		(UINT nIndex,UINT nColors,RGBQUAD* pPalette);

	BOOL			Create			(BITMAPINFO* pBitmapInfo,void *pData = NULL);
	BOOL			Load				(LPCSTR lpcszFileName);
	BOOL			Unload			(void);
	BOOL			IsLoaded			(void) {return(m_szFileName[0]!='\0');}
	BOOL			Save				(void);
	BOOL			Save				 (LPCSTR lpcszFileName,LPCSTR lpcszFormat,DWORD dwFlags);
	BOOL			Draw				(HDC /*hDC*/,const RECT* /*rcDest*/,const RECT* /*rcSrc*/,double /*fZoom*/ = 1.0f,BOOL /*bPrinting*/ = FALSE) {return(FALSE);}
	BOOL			Stretch			(RECT& drawRect,BOOL bAspectRatio = TRUE);
	BOOL			Paste			(UINT x,UINT y,HANDLE hDIB);

	BOOL			SetImageParamsDefaultValues(CImageParams*);
	BOOL			SetImageParamsMinMax(CImageParams*);

	UINT			Blur				(CImageParams*);
	UINT			Brightness		(CImageParams*);
	UINT			Contrast			(CImageParams*);
	UINT			EdgeEnhance		(CImageParams*);
	UINT			Emboss			(CImageParams*);
	UINT			FindEdge			(CImageParams*);
	UINT			GammaCorrection	(CImageParams*);
	UINT			Grayscale			(CImageParams*);
	UINT			Halftone			(CImageParams*);
	UINT			Hue				(CImageParams*);
	UINT			Invert			(CImageParams*) {return(m_pImage ? m_pImage->Negate() : GDI_ERROR);}
	UINT			Median			(CImageParams* pCImageParams) {return(m_pImage ? m_pImage->Median(pCImageParams->GetMedian()) : GDI_ERROR);}
	UINT			MirrorHorizontal	(CImageParams*) {return(m_pImage ? m_pImage->Flip() : GDI_ERROR);}
	UINT			MirrorVertical		(CImageParams*);
	UINT			Negate			(CImageParams*) {return(m_pImage ? CImage::Negate(NULL) : GDI_ERROR);}
	UINT			Posterize			(CImageParams*);
	UINT			Rotate90Left		(CImageParams*) {return(m_pImage ? m_pImage->Rotate90(BTCImageData::Left) : GDI_ERROR);}
	UINT			Rotate90Right		(CImageParams*) {return(m_pImage ? m_pImage->Rotate90(BTCImageData::Right) : GDI_ERROR);}
	UINT			Rotate180			(CImageParams*);
	UINT			SaturationHorizontal(CImageParams*);
	UINT			SaturationVertical	(CImageParams*);
	UINT			Sharpen			(CImageParams*);
	UINT			Size				(CImageParams*);

protected:
	BOOL			Rotate			(int nDegree);
	static HBITMAP GetBitmap			(BTCImageData &ImageData);
	BOOL			SaveTIFF			(LPCTSTR lpcszInputFile);
	
	BTCImageData*	m_pImage;
	BTCImageObject	m_NexgenObject;
};

#endif // HAVE_NEXGENIPL_LIBRARY

#endif // _CPNEXGENIPL__H
