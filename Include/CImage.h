/*
	CImage.h
	Classe base per l'interfaccia con le librerie.
	Luca Piergentili, 01/09/00
	lpiergentili@yahoo.com

	Ad memoriam - Nemo me impune lacessit.
*/
#ifndef _CIMAGE_H
#define _CIMAGE_H 1

#include "pragma.h"
#include "window.h"
#include "CImageObject.h"
#include "CImageParams.h"

/*
	CImage
*/
class CImage : public CImageObject
{
public:
	CImage();
	virtual ~CImage() {}

	virtual LPCSTR			GetLibraryName			(void) {return("");}
	virtual LPCSTR			GetPathName			(void) {return(m_szFileName);}
	virtual LPCSTR			GetFileName			(void);
	virtual LPCSTR			GetFileExt			(void) {return(m_szFileExt);}
	virtual DWORD			GetFileSize			(void);

	virtual UINT			GetWidth				(void) {return(0);}
	virtual UINT			GetHeight				(void) {return(0);}
	
	virtual float			GetXRes				(void);
	virtual float			GetYRes				(void);
	virtual void			SetXRes				(float nXRes) {m_InfoHeader.xres = nXRes;}
	virtual void			SetYRes				(float nYRes) {m_InfoHeader.yres = nYRes;}
	virtual int			GetURes				(void);
	virtual void			SetURes				(UINT nRes);
	virtual void			GetDPI				(float& nXRes,float& nYRes);

	virtual int			GetCompression			(void);
	virtual void			SetCompression			(int nCompression) {m_InfoHeader.compression = nCompression;}
	virtual int			GetQuality			(void);
	virtual void			SetQuality			(int nQuality) {m_InfoHeader.quality = nQuality;}
	virtual BOOL			GetQualityRange		(int& nMin,int& nMax) {nMin = nMax = -1; return(FALSE);}

	virtual int			GetAlignment			(void) {return(WINDOWSALIGNMENT);}
	virtual UINT			GetBytesWidth			(UINT nWidth,UINT nBitsPerPixel,UINT nAlig) {return(WIDTHBYTES((nWidth * nBitsPerPixel),nAlig));}
	virtual UINT			GetBytesWidth			(void) {return(GetBytesWidth(GetWidth(),GetBPP(),GetAlignment()));}
	virtual PHOTOMETRIC		GetPhotometric			(void);

	virtual int			GetMaxPaletteColors		(void) {return(MAXPALETTECOLORS);}
	virtual UINT			GetNumColors			(void);
	virtual BOOL			CountBWColors			(unsigned int* pColors,unsigned char nNumColors);
	virtual BOOL			CountRGBColors			(COLORREF* pColors,unsigned int* pCountColors,unsigned char nNumColors);
	virtual UINT			GetBPP				(void) {return(0);}
	virtual UINT			ConvertToBPP			(UINT nBitsPerPixel,UINT nFlags,RGBQUAD *pPalette = NULL,UINT nColors = 0);

	virtual COLORREF		GetPixel				(UINT /*x*/,UINT /*y*/) {return(RGB(255,255,255));}
	virtual void			SetPixel				(UINT /*x*/,UINT /*y*/,COLORREF /*colorref*/) {}
	virtual void*			GetPixels				(void) {return(NULL);}
	virtual LPBITMAPINFO	GetBMI				(void) {return(NULL);}
	virtual HBITMAP		GetBitmap				(void) {return(NULL);}
	virtual UINT			GetMemUsed			(void) {return(0);}

	virtual HDIB			GetDIB				(DIBINFO* /*pDibInfo*/ = NULL) {return(NULL);}
	virtual BOOL			SetDIB				(HDIB /*hDib*/,DIBINFO* /*pDibInfo*/ = NULL) {return(FALSE);}
	virtual int			GetDIBOrder			(void) {return(-1);}
	virtual WORD			GetDIBNumColors		(LPSTR lpbi);
	virtual HPALETTE		CreateDIBPalette		(LPBITMAPINFO lpbmi);
	virtual BOOL			SetPalette			(UINT /*nIndex*/,UINT /*nColors*/,RGBQUAD* /*pPalette*/) {return(FALSE);}
	
	virtual IMAGE_TYPE		GetType				(void);
	virtual void			GetHeaderInfo			(LPCSTR lpcszFileName,LPIMAGEHEADERINFO pHeaderInfo);
	virtual LPCIMAGEHEADERINFO GetHeaderInfo		(void) {return(&m_InfoHeader);}
	
	virtual BOOL			Create				(BITMAPINFO* /*pBitmapInfo*/,void* /*pData*/ = NULL) {return(FALSE);}
	virtual BOOL			Load					(LPCSTR lpcszFileName);
	virtual BOOL			Unload				(void) {return(FALSE);}
	virtual BOOL			IsLoaded				(void) {return(FALSE);}
	virtual BOOL			Save					(void) {return(FALSE);}
	virtual BOOL			Save					(LPCSTR /*lpcszFileName*/,LPCSTR /*lpcszFormat*/,DWORD /*dwFlags*/ = 0L) {return(FALSE);}
	virtual BOOL			Draw					(HDC /*hDC*/,const RECT* /*rcDest*/,const RECT* /*rcSrc*/,double /*fZoom*/ = 1.0f,BOOL /*bPrinting*/ = FALSE) {return(FALSE);}
	virtual BOOL			Stretch				(RECT& /*rcSize*/,BOOL /*bAspectRatio*/ = TRUE) {return(FALSE);}
	virtual HANDLE			Copy					(RECT& rect);
	virtual BOOL			Paste				(UINT /*x*/,UINT /*y*/,HANDLE /*hDib*/) {return(FALSE);}

	virtual BOOL			LockData				(void) {return(TRUE);}
	virtual BOOL			UnlockData			(BOOL /*bModified*/ = FALSE) {return(TRUE);}
	virtual void			ShowErrors			(BOOL bFlag) {m_bShowErrors = bFlag;}

	virtual BOOL			SetImageParamsDefaultValues(CImageParams* pCImageParams) {pCImageParams->Reset(); return(FALSE);}
	virtual BOOL			SetImageParamsMinMax	(CImageParams* pCImageParams) {pCImageParams->Reset(); return(FALSE);}

	virtual UINT			Blur					(CImageParams* /*pImageParams*/) {return(NotImplemented("Blur"));}
	virtual UINT			Brightness			(CImageParams* pImageParams);
	virtual UINT			Contrast				(CImageParams* pImageParams);
	virtual UINT			Deskew				(CImageParams* /*pImageParams*/) {return(NotImplemented("Deskew"));}
	virtual UINT			Despeckle				(CImageParams* /*pImageParams*/) {return(NotImplemented("Despeckle"));}
	virtual UINT			Dilate				(CImageParams* /*pImageParams*/) {return(NotImplemented("Dilate"));}
	virtual UINT			EdgeEnhance			(CImageParams* /*pImageParams*/) {return(NotImplemented("EdgeEnhance"));}
	virtual UINT			Emboss				(CImageParams* /*pImageParams*/) {return(NotImplemented("Emboss"));}
	virtual UINT			Equalize				(CImageParams* /*pImageParams*/) {return(NotImplemented("Equalize"));}
	virtual UINT			Erosion				(CImageParams* /*pImageParams*/) {return(NotImplemented("Erosion"));}
	virtual UINT			FindEdge				(CImageParams* /*pImageParams*/) {return(NotImplemented("FindEdge"));}
	virtual UINT			GammaCorrection		(CImageParams* pImageParams);
	virtual UINT			Grayscale				(CImageParams* /*pImageParams*/) {return(NotImplemented("Grayscale"));}
	virtual UINT			Halftone				(CImageParams* pImageParams);
	virtual UINT			HistoContrast			(CImageParams* /*pImageParams*/) {return(NotImplemented("HistoContrast"));}
	virtual UINT			Hue					(CImageParams* pImageParams);
	virtual UINT			Intensity				(CImageParams* /*pImageParams*/) {return(NotImplemented("Intensity"));}
	virtual UINT			IntensityDetect		(CImageParams* /*pImageParams*/) {return(NotImplemented("IntensityDetect"));}
	virtual UINT			Invert				(CImageParams* /*pImageParams*/) {return(NotImplemented("Invert"));}
	virtual UINT			Median				(CImageParams* /*pImageParams*/) {return(NotImplemented("Median"));}
	virtual UINT			MirrorHorizontal		(CImageParams* /*pImageParams*/) {return(Mirror(0));}
	virtual UINT			MirrorVertical			(CImageParams* /*pImageParams*/) {return(Mirror(1));}
	virtual UINT			Mosaic				(CImageParams* /*pImageParams*/) {return(NotImplemented("Mosaic"));}
	virtual UINT			Negate				(CImageParams* pImageParams);
	virtual UINT			Noise				(CImageParams* /*pImageParams*/) {return(NotImplemented("Noise"));}
	virtual UINT			Posterize				(CImageParams* /*pImageParams*/) {return(NotImplemented("Posterize"));}
	virtual UINT			Rotate90Left			(CImageParams* /*pImageParams*/) {return(NotImplemented("Rotate90Left"));}
	virtual UINT			Rotate90Right			(CImageParams* /*pImageParams*/) {return(NotImplemented("Rotate90Right"));}
	virtual UINT			Rotate180				(CImageParams* /*pImageParams*/) {return(NotImplemented("Rotate180"));}
	virtual UINT			SaturationHorizontal	(CImageParams* pImageParams);
	virtual UINT			SaturationVertical		(CImageParams* pImageParams);
	virtual UINT			Sharpen				(CImageParams* /*pImageParams*/) {return(NotImplemented("Sharpen"));}
	virtual UINT			Size					(CImageParams* /*pImageParams*/) {return(NotImplemented("Size"));}

	UINT					Text					(LPCSTR lpcszText,CHOOSEFONT* cf,COLORREF foregroundColor,COLORREF backgroundColor,int nBackgroundMode,SIZE* size);

	HDIB					WindowToDIB			(CWnd *pWnd, CRect* pScreenRect);
	HDIB					BitmapToDIB			(HBITMAP hBitmap, HPALETTE hPal);
	HBITMAP				DIBToBitmap			(HDIB hDib, HPALETTE hPal);
	WORD					PaletteSize			(LPSTR lpbi);
	LPSTR				FindDIBBits			(LPSTR lpbi);
	BOOL					CreateDIBPalette		(HDIB hDIB, CPalette* pPal);
	WORD					DIBNumColors			(LPSTR lpbi);

protected:
	void					RGBtoHSL				(COLORREF rgb,double* H,double* S,double* L);
	COLORREF				HLStoRGB				(const double& H,const double& L,const double& S);
	double				HuetoRGB				(double m1,double m2,double h);

	UINT					NotImplemented			(LPCSTR pMethod) {char buffer[256]; _snprintf(buffer,sizeof(buffer)-1,"The %s() method is not implemented.",pMethod); ::MessageBox(NULL,buffer,"CImage()",MB_OK|MB_ICONWARNING|MB_TASKMODAL|MB_SETFOREGROUND|MB_TOPMOST); return(GDI_ERROR);}

private:
	UINT					Mirror				(UINT nDirection/*0=horiz, 1=vert*/);
	UINT					Saturation			(int nFactor);
};

#endif // _CIMAGE_H
