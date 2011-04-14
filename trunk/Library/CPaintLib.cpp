/*
	CPaintLib.cpp
	Classe derivata per interfaccia con paintlib.
	Luca Piergentili, 01/09/00
	lpiergentili@yahoo.com

	Ad memoriam - Nemo me impune lacessit.
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include "window.h"
#include "ImageConfig.h"

#ifdef HAVE_PAINTLIB_LIBRARY

#include "strings.h"
#include "CImage.h"
#include "CImageParams.h"
#include "CPaintLib.h"
#include "tiff.h"

#include "traceexpr.h"
//#define _TRACE_FLAG _TRFLAG_TRACEOUTPUT
//#define _TRACE_FLAG _TRFLAG_NOTRACE
#define _TRACE_FLAG_INFO _TRFLAG_NOTRACE
#define _TRACE_FLAG_WARN _TRFLAG_NOTRACE
#define _TRACE_FLAG_ERR _TRFLAG_NOTRACE

#if (defined(_DEBUG) && defined(_WINDOWS)) && (defined(_AFX) || defined(_AFXDLL))
  #pragma message("\t\t\t"__FILE__"("STR(__LINE__)"): using DEBUG_NEW macro")
  #define new DEBUG_NEW
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif

/*
	CPaintLib()
*/
CPaintLib::CPaintLib()
{
	memset(m_szFileName,'\0',sizeof(m_szFileName));
	memset(m_szFileExt,'\0',sizeof(m_szFileExt));
	m_ImageType = (IMAGE_TYPE)-1;

	LPIMAGETYPE p;
#ifdef SUPPORT_BMP
	ADDFILETYPE(BMP_PICTURE,		".bmp",	"BMP file ",						IMAGE_READ_FLAG|IMAGE_WRITE_FLAG,p)
	ADDFILETYPE(BMP_PICTURE,		".dib",	"DIB file ",						IMAGE_READ_FLAG|IMAGE_WRITE_FLAG,p)
	ADDFILETYPE(BMP_PICTURE,		".rle",	"RLE file ",						IMAGE_READ_FLAG|IMAGE_WRITE_FLAG,p)
#endif
#ifdef SUPPORT_JPEG
	ADDFILETYPE(JPEG_PICTURE,	".jpg",	"JPEG file ",						IMAGE_READ_FLAG|IMAGE_WRITE_FLAG,p)
	ADDFILETYPE(JPEG_PICTURE,	".jpeg",	"JPEG file ",						IMAGE_READ_FLAG|IMAGE_WRITE_FLAG,p)
	ADDFILETYPE(JPEG_PICTURE,	".jif",	"JPEG file ",						IMAGE_READ_FLAG|IMAGE_WRITE_FLAG,p)
	ADDFILETYPE(JPEG_PICTURE,	".jpe",	"JPEG file ",						IMAGE_READ_FLAG|IMAGE_WRITE_FLAG,p)
#endif
#ifdef SUPPORT_PCX
	ADDFILETYPE(PCX_PICTURE,		".pcx",	"PCX file ",						IMAGE_READ_FLAG,p)
#endif
#ifdef SUPPORT_PGM
	ADDFILETYPE(PGM_PICTURE,		".pgm",	"Portable Graymap File ",			IMAGE_READ_FLAG,p)
#endif
#ifdef SUPPORT_PICT
	ADDFILETYPE(PCT_PICTURE,		".pct",	"Macintosh Pict Format ",			IMAGE_READ_FLAG,p)
#endif
#ifdef SUPPORT_PNG
	ADDFILETYPE(PNG_PICTURE,		".png",	"Portable Network Graphics Format ",	IMAGE_READ_FLAG|IMAGE_WRITE_FLAG,p)
#endif
#ifdef SUPPORT_TGA
	ADDFILETYPE(TGA_PICTURE,		".tga",	"TARGA file ",						IMAGE_READ_FLAG,p)
#endif
#ifdef SUPPORT_TIFF
	ADDFILETYPE(TIFF_PICTURE,	".tif",	"TIFF file ",						IMAGE_READ_FLAG|IMAGE_WRITE_FLAG,p)
	ADDFILETYPE(TIFF_PICTURE,	".tiff",	"TIFF file ",						IMAGE_READ_FLAG|IMAGE_WRITE_FLAG,p)
#endif

	LPIMAGEOPERATION o;
	ADDIMAGEOPERATION(	"Brightness",			TRUE,	o)
	ADDIMAGEOPERATION(	"Contrast",			TRUE,	o)
	ADDIMAGEOPERATION(	"GammaCorrection",		TRUE,	o)
	ADDIMAGEOPERATION(	"Equalize",			FALSE,	o)
	ADDIMAGEOPERATION(	"Grayscale",			TRUE,	o)
	ADDIMAGEOPERATION(	"Halftone",			TRUE,	o)
	ADDIMAGEOPERATION(	"Hue",				TRUE,	o)
	ADDIMAGEOPERATION(	"HistoContrast",		FALSE,	o)
	ADDIMAGEOPERATION(	"Intensity",			FALSE,	o)
	ADDIMAGEOPERATION(	"IntensityDetect",		FALSE,	o)
	ADDIMAGEOPERATION(	"Invert",				TRUE,	o)
	ADDIMAGEOPERATION(	"SaturationHorizontal",	TRUE,	o)
	ADDIMAGEOPERATION(	"SaturationVertical",	TRUE,	o)
	ADDIMAGEOPERATION(	"Noise",				FALSE,	o)
	ADDIMAGEOPERATION(	"Blur",				FALSE,	o)
	ADDIMAGEOPERATION(	"Despeckle",			FALSE,	o)
	ADDIMAGEOPERATION(	"Deskew",				FALSE,	o)
	ADDIMAGEOPERATION(	"Dilate",				FALSE,	o)
	ADDIMAGEOPERATION(	"EdgeEnhance",			FALSE,	o)
	ADDIMAGEOPERATION(	"Emboss",				FALSE,	o)
	ADDIMAGEOPERATION(	"Erosion",			FALSE,	o)
	ADDIMAGEOPERATION(	"FindEdge",			FALSE,	o)
	ADDIMAGEOPERATION(	"Mosaic",				FALSE,	o)
	ADDIMAGEOPERATION(	"Median",				FALSE,	o)
	ADDIMAGEOPERATION(	"MirrorHorizontal",		TRUE,	o)
	ADDIMAGEOPERATION(	"MirrorVertical",		TRUE,	o)
	ADDIMAGEOPERATION(	"Posterize",			FALSE,	o)
	ADDIMAGEOPERATION(	"Rotate90Left",		TRUE,	o)
	ADDIMAGEOPERATION(	"Rotate90Right",		TRUE,	o)
	ADDIMAGEOPERATION(	"Rotate180",			TRUE,	o)
	ADDIMAGEOPERATION(	"Sharpen",			FALSE,	o)
	ADDIMAGEOPERATION(	"Size",				TRUE,	o)

	LPIMAGETIFFTYPE t;
	ADDTIFFTYPE(COMPRESSION_NONE,		"COMPRESSION_NONE (dump mode)",							t)
	ADDTIFFTYPE(COMPRESSION_CCITTRLE,	"COMPRESSION_CCITTRLE (CCITT modified Huffman RLE)",			t)
	ADDTIFFTYPE(COMPRESSION_CCITTRLEW,	"COMPRESSION_CCITTRLEW (#1 w/ word alignment)",				t)
	ADDTIFFTYPE(COMPRESSION_CCITTFAX3,	"COMPRESSION_CCITTFAX3 (CCITT Group 3 fax encoding)",			t)
	ADDTIFFTYPE(COMPRESSION_CCITTFAX4,	"COMPRESSION_CCITTFAX4 (CCITT Group 4 fax encoding)",			t)
	ADDTIFFTYPE(COMPRESSION_PACKBITS,	"COMPRESSION_PACKBITS (Macintosh RLE)",						t)
	ADDTIFFTYPE(COMPRESSION_LZW,		"COMPRESSION_LZW (Lempel-Ziv & Welch, requires Unisys patent)",	t)
}

/*
	GetLibraryName()
*/
LPCSTR CPaintLib::GetLibraryName(void)
{
	return(PAINTLIB_LIB_NAME);
}

/*
	GetWidth()
*/
UINT CPaintLib::GetWidth(void)
{
	return(m_Image.GetWidth());
}

/*
	GetHeight()
*/
UINT CPaintLib::GetHeight(void)
{
	return(m_Image.GetHeight());
}

/*
	GetXRes()
*/
float CPaintLib::GetXRes(void)
{
	return(m_Image.GetXRes());
}

/*
	GetYRes()
*/
float CPaintLib::GetYRes(void)
{
	return(m_Image.GetYRes());
}

/*
	SetXRes()
*/
void CPaintLib::SetXRes(float nXRes)
{
	m_Image.SetXRes(nXRes);
	CImage::SetXRes(nXRes);
}

/*
	SetYRes()
*/
void CPaintLib::SetYRes(float nYRes)
{
	m_Image.SetYRes(nYRes);
	CImage::SetYRes(nYRes);
}

/*
	GetURes()
*/
int CPaintLib::GetURes(void)
{
	return(m_Image.GetURes());
}

/*
	SetURes()
*/
void CPaintLib::SetURes(UINT nURes)
{
	m_Image.SetURes(nURes);
	CImage::SetURes(nURes);
}

/*
	GetDPI()
*/
void CPaintLib::GetDPI(float& nXRes,float& nYRes)
{
	nXRes = m_Image.GetXRes();
	nYRes = m_Image.GetYRes();
}

/*
	GetCompression()
*/
int CPaintLib::GetCompression(void)
{
	return(m_Image.GetCompression());
}

/*
	SetCompression()
*/
void CPaintLib::SetCompression(int nCompression)
{
	m_Image.SetCompression(nCompression);
	CImage::SetCompression(nCompression);
}

/*
	GetNumColors()
*/
UINT CPaintLib::GetNumColors(void)
{
	return(m_Image.GetNumColors() > 256 ? 0 : m_Image.GetNumColors());
}

/*
	GetBPP()
*/
UINT CPaintLib::GetBPP(void)
{
	return(m_Image.GetBitsPerPixel());
}

/*
	ConvertToBPP()
*/
UINT CPaintLib::ConvertToBPP(UINT nBitsPerPixel,UINT nFlags,RGBQUAD* pPalette,UINT nColors)
{
	UINT nRet = GDI_ERROR;
	
	switch(GetBPP())
	{
		case 1:
		case 8:
		case 32:
			if(nBitsPerPixel!=GetBPP())
				nRet = ConvertToBPP(nBitsPerPixel) ? NO_ERROR : nRet;
			break;
		
		default:
			nRet = CImage::ConvertToBPP(nBitsPerPixel,nFlags,pPalette,nColors);
			break;
	}

	return(nRet);
}

/*
	ConvertToBPP()
*/
BOOL CPaintLib::ConvertToBPP(UINT nBPP)
{
	BOOL bConverted = TRUE;
	CWinBmp bppCopy;
	
	if(bConverted)
		if(!bppCopy.CreateCopy((CBmp&)m_Image,nBPP))
			bConverted = FALSE;
	if(bConverted)
		if(!m_Image.CreateCopy((CBmp&)bppCopy,nBPP))
			bConverted = FALSE;

	if(!bConverted && m_bShowErrors)
	{
		char buffer[256];
		_snprintf(buffer,
				sizeof(buffer)-1,
				"Unable to convert the image from %d to %d bpp.",
				m_Image.GetBitsPerPixel(),
				nBPP
				);
		::MessageBox(NULL,buffer,"CPaintLib::ConvertToBPP()",MB_ICONERROR|MB_TASKMODAL|MB_SETFOREGROUND|MB_TOPMOST);
	}

	return(bConverted);
}

/*
	GetPixel()
*/
COLORREF CPaintLib::GetPixel(UINT x,UINT y)
{
	return(m_Image.GetPixel(x,y));
}

/*
	SetPixel()
*/
void CPaintLib::SetPixel(UINT x,UINT y,COLORREF colorref)
{
	m_Image.SetPixel(x,y,RGB(GetBValue(colorref),GetGValue(colorref),GetRValue(colorref)));
}

/*
	GetPixels()
*/
void* CPaintLib::GetPixels(void)
{
	return(m_Image.GetBits());
}

/*
	GetBMI()
*/
LPBITMAPINFO CPaintLib::GetBMI(void)
{
	return((LPBITMAPINFO)m_Image.GetBMI());
}

/*
	GetBitmap()
*/
HBITMAP CPaintLib::GetBitmap(void)
{
	HBITMAP hBitmap = (HBITMAP)NULL;
	static CDIBSection Dib;
	Dib = m_Image;
	hBitmap = (HBITMAP)Dib.GetHandle();
	return(hBitmap);
}

/*
	GetMemUsed()
*/
UINT CPaintLib::GetMemUsed(void)
{
	return(m_Image.GetMemNeeded((LONG)m_Image.GetWidth(),(LONG)m_Image.GetHeight(),(WORD)m_Image.GetBitsPerPixel()));
}

/*
	GetDIB()
*/
HDIB CPaintLib::GetDIB(DIBINFO* /*pDibInfo=NULL*/)
{
	CImageParams ImageParams;
	ImageParams.Reset();
	if(GetDIBOrder() < 0)
		MirrorHorizontal(&ImageParams);

	HDIB hDib = (HDIB)NULL;
	BITMAPINFOHEADER* pBitmapInfoHdr = (BITMAPINFOHEADER*)GetBMI();

	if(pBitmapInfoHdr)
	{
		if(LockData())
		{
			LPSTR pData = (LPSTR)GetPixels();
			UINT nBitmapSize = (pBitmapInfoHdr->biHeight * GetDIBOrder()) * GetBytesWidth();
			UINT nColorBytes = sizeof(RGBQUAD) * GetNumColors();
			UINT nTotalBytes = sizeof(BITMAPINFOHEADER) + nColorBytes + nBitmapSize;
			
			hDib = (HDIB)::GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE|GMEM_ZEROINIT,nTotalBytes);
			if(hDib)
			{
				BITMAPINFOHEADER* pBitmapInfoHeader = (BITMAPINFOHEADER*)::GlobalLock((HGLOBAL)hDib);
				if(pBitmapInfoHeader)
				{
					memcpy(pBitmapInfoHeader,pBitmapInfoHdr,sizeof(BITMAPINFOHEADER)+nColorBytes);
					memcpy((LPSTR)pBitmapInfoHeader+sizeof(BITMAPINFOHEADER)+nColorBytes,pData,nBitmapSize);
					pBitmapInfoHeader->biHeight = GetHeight();
					pBitmapInfoHeader->biSizeImage = nBitmapSize;

					::GlobalUnlock((HGLOBAL)hDib);
				}
			}

			UnlockData();
		}
	}
	
	if(GetDIBOrder() < 0)
		MirrorHorizontal(&ImageParams);

	return(hDib);
}

/*
	SetDIB()
*/
BOOL CPaintLib::SetDIB(HDIB hDib,DIBINFO* /* pDibInfo = NULL */)
{
	BOOL bSet = FALSE;
	BITMAPINFOHEADER* pBitmapInfoHeader = (BITMAPINFOHEADER*)::GlobalLock((HGLOBAL)hDib);
	
	if(pBitmapInfoHeader)
	{
		LPSTR pData = NULL;
		UINT nNumColors = 0;
		if(pBitmapInfoHeader->biBitCount <= 8)
			nNumColors = 1 << pBitmapInfoHeader->biBitCount;
		
		pData = ((LPSTR)pBitmapInfoHeader) + (sizeof(BITMAPINFOHEADER) + (sizeof(RGBQUAD)*nNumColors));
		
		if(Create((BITMAPINFO*)pBitmapInfoHeader,pData))
		{
			if(GetDIBOrder() < 0)
			{
				CImageParams ImageParams;
				ImageParams.Reset();
				MirrorHorizontal(&ImageParams);
			}

			bSet = TRUE;
		}
		
		::GlobalUnlock((HGLOBAL)hDib);
	}
	
	return(bSet);
}

/*
	GetDIBOrder()
*/
int CPaintLib::GetDIBOrder(void)
{
	return(1);
}

/*
	SetPalette()
*/
BOOL CPaintLib::SetPalette(UINT nIndex,UINT nColors,RGBQUAD* pPalette)
{
	UINT nColorData = GetNumColors();
	
	if(!nColorData || nIndex < 0 || nIndex >= nColorData || pPalette==NULL || nColors <= 0 || (nColors+nIndex) > nColorData)
		return(FALSE);
	
	for(int i = 0; i < (int)nColors; i++)
		m_Image.SetPaletteEntry(i+nIndex,pPalette[i+nIndex].rgbRed,pPalette[i+nIndex].rgbGreen,pPalette[i+nIndex].rgbBlue,255);
	
	return(TRUE);
}

/*
	Create()
*/
BOOL CPaintLib::Create(BITMAPINFO* pBitmapInfo,void *pData/* = NULL */)
{
	BOOL bCreated = FALSE;

	if(pBitmapInfo)
	{
		BITMAPINFOHEADER* pBitmapInfoHeader = (BITMAPINFOHEADER*)pBitmapInfo;
		RGBQUAD* pPalette = pBitmapInfo->bmiColors;

		if(pBitmapInfoHeader && pPalette)
		{
			m_Image.Create(pBitmapInfoHeader->biWidth,abs(pBitmapInfoHeader->biHeight),pBitmapInfoHeader->biBitCount,FALSE);

			int nColorData = 0;
			if(pBitmapInfoHeader->biBitCount <= 8)
				nColorData = 1 << pBitmapInfoHeader->biBitCount;
			if(nColorData)
			{
				RGBQUAD* pPaletteDst = (RGBQUAD*)m_Image.GetPalette();
				if(pPaletteDst)
					memcpy(pPaletteDst,pPalette,nColorData*sizeof(RGBQUAD));
			}

			unsigned char *pDataDst = m_Image.GetBits();
			int WidthEnBytes = WIDTHBYTES(pBitmapInfoHeader->biWidth * pBitmapInfoHeader->biBitCount,GetAlignment());
			int nTotalBytes = abs(pBitmapInfoHeader->biHeight) * WidthEnBytes;
			if(!pData)
				memset(pDataDst,'\0',nTotalBytes);
			else 
				memcpy(pDataDst,pData,nTotalBytes);

			bCreated = TRUE;
		}
	}

	return(bCreated);
}

/*
	Load()
*/
BOOL CPaintLib::Load(LPCSTR lpcszFileName)
{
	BOOL bLoaded = TRUE;

	__try {
		bLoaded = _Load(lpcszFileName);
	}
	__except(EXCEPTION_EXECUTE_HANDLER) {
		if(m_bShowErrors)
		{
			char buffer[512];
			_snprintf(buffer,
					sizeof(buffer)-1,
					"An exception has occurred into the graphics engine loading: %s\n\nThe current library (paintlib) is unable to load the specified file.",
					lpcszFileName
					);
			::MessageBox(NULL,buffer,"CPaintLib::Load()",MB_ICONERROR|MB_TASKMODAL|MB_SETFOREGROUND|MB_TOPMOST);
		}

		bLoaded = FALSE;
	}

	if(bLoaded)
	{
		CImage::SetXRes(m_Image.GetXRes());
		CImage::SetYRes(m_Image.GetYRes());

		CImage::SetQuality(m_Image.GetQuality());

		strcpyn(m_szFileName,lpcszFileName,sizeof(m_szFileName));
		char* p = strrchr(m_szFileName,'.');
		if(p)
			strcpyn(m_szFileExt,p,sizeof(m_szFileExt));
		
		LPIMAGETYPE pImagetype;
		while((pImagetype = EnumImageFormats())!=NULL)
			if(stricmp(m_szFileExt,pImagetype->ext)==0)
				m_ImageType = pImagetype->type;
	}
	else
	{
		memset(m_szFileName,'\0',sizeof(m_szFileName));
		memset(m_szFileExt,'\0',sizeof(m_szFileExt));
		m_ImageType = (IMAGE_TYPE)-1;
	}

	return(bLoaded);
}

/*
	_Load()
*/
BOOL CPaintLib::_Load(LPCSTR lpcszFileName)
{
	BOOL bLoaded = TRUE;

	try
	{
		CAnyPicDecoder Decoder;
		Decoder.MakeBmpFromFile(lpcszFileName,&m_Image,0);
	}
	catch(CTextException e)
	{
		bLoaded = FALSE;
	}

	return(bLoaded);
}

/*
	Save()
*/
BOOL CPaintLib::Save(LPCSTR lpcszFileName,LPCSTR lpcszFormat,DWORD dwFlags)
{
	BOOL bSaved = FALSE;

	__try {
		bSaved = _Save(lpcszFileName,lpcszFormat,dwFlags);
	}
	__except(EXCEPTION_EXECUTE_HANDLER) {
		if(m_bShowErrors)
		{
			char buffer[512];
			_snprintf(buffer,
					sizeof(buffer)-1,
					"An exception has occurred into the graphics engine saving: %s\n\nThe current library (paintlib) is unable to save the specified file.",
					lpcszFileName
					);
			::MessageBox(NULL,buffer,"CPaintLib::Save()",MB_ICONERROR|MB_TASKMODAL|MB_SETFOREGROUND|MB_TOPMOST);
		}

		bSaved = FALSE;
	}

	return(bSaved);
}

/*
	Save()
*/
BOOL CPaintLib::Save(void)
{
	BOOL bSaved = FALSE;

	if(m_szFileName[0]!='\0' && m_szFileExt[0]!='\0' && m_ImageType!=(IMAGE_TYPE)-1)
		bSaved = Save(m_szFileName,m_szFileExt,0L);

	return(bSaved);
}

/*
	_Save()
*/
BOOL CPaintLib::_Save(LPCSTR lpcszFileName,LPCSTR lpcszFormat,DWORD /*dwFlags*/)
{
	BOOL bSaved = TRUE;
	LPIMAGETYPE imagetype;
	
	while((imagetype = EnumImageFormats())!=NULL)
	{
		if(stricmp(lpcszFormat,imagetype->ext)==0)
		{
			switch(imagetype->type)
			{
				case BMP_PICTURE:
				{
					try
					{
						//CBmpEncoder Encoder;
						//Encoder.MakeFileFromBmp(lpcszFileName,(CBmp*)&m_Image);
						m_Image.SaveAsBmp(lpcszFileName);
					}
					catch(CTextException e)
					{
						bSaved = FALSE;
					}
					break;
				}

				case JPEG_PICTURE:
				{
					CJPEGEncoder encoder;
					try
					{
						BOOL bConverted = TRUE;
						if(m_Image.GetBitsPerPixel()!=32)
							bConverted = ConvertToBPP(32);
						if(bConverted)
						{
							int nQuality = CImage::GetQuality();
							nQuality = nQuality <= 0 || nQuality > 100 ? 100 : nQuality;
							m_Image.SetQuality(nQuality);
							encoder.MakeFileFromBmp(lpcszFileName,(CBmp*)&m_Image);
						}
						else
							bSaved = FALSE;
					}
					catch(CTextException e)
					{
						bSaved = FALSE;
					}
					break;
				}

				case PNG_PICTURE:
				{
					CPNGEncoder encoder;
					try
					{
						encoder.MakeFileFromBmp(lpcszFileName,(CBmp*)&m_Image);
					}
					catch(CTextException e)
					{
						bSaved = FALSE;
					}
					break;
				}

				case TIFF_PICTURE:
				{
/*
#if 0
					CTIFFEncoder encoder;
					try
					{
						encoder.MakeFileFromBmp(lpcszFileName,(CBmp*)&m_Image);
					}
#else
					#define HOWMANY(x,y) ((((UINT)(x))+(((UINT)(y))-1))/((UINT)(y)))

					CTIFFEncoderEx encoder;
					try
					{
						UINT nBufSize = m_Image.GetMemUsed();
						nBufSize = nBufSize < 20000 ? nBufSize + 4096 : int(1.2 * nBufSize);
						CFileSink FileSink;
						if(!FileSink.Open( lpcszFileName, nBufSize ))
						{
							encoder.Associate(&FileSink);
							encoder.SetBaseTags((CBmp*)&m_Image);
							
							UINT nLineBytes = HOWMANY(GetWidth() * GetBPP(),8);
							UINT nRowsPerStrips = (8*1024)/nLineBytes;
							encoder.SetField(TIFFTAG_ROWSPERSTRIP,nRowsPerStrips==0 ? 1L : nRowsPerStrips);
							switch(dwFlags)
							{
								case COMPRESSION_CCITTFAX3:
								case COMPRESSION_CCITTFAX4:
									encoder.SetField(TIFFTAG_ROWSPERSTRIP,-1L);
									break;
								default:
									break;
							}
							
							encoder.SetField(TIFFTAG_COMPRESSION,dwFlags);
							encoder.SaveBmp((CBmp*)&m_Image,&FileSink);
							encoder.Dissociate();
							FileSink.Close();
						}
					}
#endif
*/

						#define HOWMANY(x,y) ((((UINT)(x))+(((UINT)(y))-1))/((UINT)(y)))

						CTIFFEncoderEx encoder;
						try
						{
// file mappato
#if 0
							UINT nBufSize = ((CBmp*)m_pImage)->GetMemUsed();
							nBufSize = nBufSize < 20000 ? nBufSize + 8192 : int(1.2 * nBufSize);
							CFileSink FileSink;
							if(!FileSink.Open( lpcszFileName, nBufSize ))
							{
								encoder.Associate(&FileSink);
								encoder.SetBaseTags((CBmp*)m_pImage);
								
								UINT nLineBytes = HOWMANY(GetWidth() * GetBPP(),8);
								UINT nRowsPerStrips = (8*1024)/nLineBytes;
								encoder.SetField(TIFFTAG_ROWSPERSTRIP,nRowsPerStrips==0 ? 1L : nRowsPerStrips);
								
								int nCompression = GetCompression();
								switch(nCompression)
								{
									case COMPRESSION_CCITTFAX3:
									case COMPRESSION_CCITTFAX4:
										encoder.SetField(TIFFTAG_ROWSPERSTRIP,-1L);
										break;
									default:
										break;
								}
								
								encoder.SetField(TIFFTAG_COMPRESSION,nCompression);
								encoder.SaveBmp((CBmp*)m_pImage,&FileSink);
								encoder.Dissociate();
								FileSink.Close();
							}
// senza mappatura del file
#else
							encoder.Associate(lpcszFileName,"wb");
							encoder.SetBaseTags((CBmp*)&m_Image);
							
							UINT nLineBytes = HOWMANY(GetWidth() * GetBPP(),8);
							UINT nRowsPerStrips = (8*1024)/nLineBytes;
							encoder.SetField(TIFFTAG_ROWSPERSTRIP,nRowsPerStrips==0 ? 1L : nRowsPerStrips);
							
							int nCompression = GetCompression();
							switch(nCompression)
							{
								case COMPRESSION_CCITTFAX3:
								case COMPRESSION_CCITTFAX4:
									encoder.SetField(TIFFTAG_ROWSPERSTRIP,-1L);
									break;
								default:
									break;
							}
							
							encoder.SetField(TIFFTAG_COMPRESSION,nCompression);
							encoder.SaveBmp((CBmp*)&m_Image,NULL);
							encoder.Dissociate();
#endif
						}
					catch(CTextException e)
					{
						bSaved = FALSE;
					}
					break;
				}
			}
		}
	}

	return(bSaved);
}

/*
	Stretch()
*/
BOOL CPaintLib::Stretch(RECT& drawRect,BOOL bAspectRatio/*=TRUE*/)
{
	// adatta la dimensione dell'immagine al rettangolo (mantenendo le proporzioni originali)
	float nFactor = 0.0;
	float nWidth = (float)GetWidth();
	float nHeight = (float)GetHeight();

	if(bAspectRatio)
	{
		// l'immagine e' piu' piccola del rettangolo, l'allarga
		if(nHeight < (float)drawRect.bottom)
		{
			nFactor = (float)drawRect.bottom/nHeight;
			if(nFactor > 0.0)
			{
				nHeight *= nFactor;
				nWidth *= nFactor;
			}
		}
		if(nWidth < (float)drawRect.right)
		{
			nFactor = (float)drawRect.right/nWidth;
			if(nFactor > 0.0)
			{
				nHeight *= nFactor;
				nWidth *= nFactor;
			}
		}
	}
	else
	{
		nWidth = (float)drawRect.right;
		nHeight = (float)drawRect.bottom;
	}

	// l'immagine e' piu' grande del rettangolo, la riduce
	// la normalizzazione e' necessaria perche' se l'immagine e' stata allargata sopra una delle
	// dimensioni potrebbe eccedere il rettangolo
	if(nHeight > (float)drawRect.bottom)
	{
		nFactor = nHeight/(float)drawRect.bottom;
		if(nFactor > 0.0)
		{
			nHeight /= nFactor;
			nWidth /= nFactor;
		}
	}
	if(nWidth > (float)drawRect.right)
	{
		nFactor = nWidth/(float)drawRect.right;
		if(nFactor > 0.0)
		{
			nHeight /= nFactor;
			nWidth /= nFactor;
		}
	}

	// ridimensiona l'immagine
//	if(nWidth > (float)GetWidth() || nHeight > (float)GetHeight())
	{
		/*
		void ResizeBilinear(int NewXSize,int NewYSize);

		//!
		void ResizeBox(int NewXSize,int NewYSize,double NewRadius);

		//!
		void ResizeGaussian(int NewXSize,int NewYSize,double NewRadius);

		//!
		void ResizeHamming(int NewXSize,int NewYSize,double NewRadius);
		*/

		BOOL bConverted = TRUE;
		if(m_Image.GetBitsPerPixel()==8)
			bConverted = ConvertToBPP(32);
		
		//sfocato
		//m_Image.ResizeBox(nWidth,nHeight,1.0);
		//sfocato
		//m_Image.ResizeBilinear(nWidth,nHeight);
		//sfocato
		//m_Image.ResizeGaussian(nWidth,nHeight,1.0);
		if(bConverted)
			m_Image.ResizeHamming(nWidth,nHeight,1.0);
	}
/*	else
	{
		if(m_Image.GetBPP()==8)
			ConvertToBPP(32);

		if((int)nHeight <= 0)
			nHeight = 1;
		if((int)nWidth <= 0)
			nWidth = 1;

		HBITMAP hShrinkedBitmap = ShrinkBitmap(GetBitmap(),nWidth,nHeight);
		m_Image.CreateFromHBitmap(hShrinkedBitmap);
	}
*/	
	return(TRUE);
}

/*
	SetImageParamsDefaultValues()
*/
BOOL CPaintLib::SetImageParamsDefaultValues(CImageParams* pCImageParams)
{
	pCImageParams->SetBlur(0);
	pCImageParams->SetBrightness(100);
	pCImageParams->SetContrast(100);
	pCImageParams->SetDilate(0);
	pCImageParams->SetEmboss(0);
	pCImageParams->SetEmbossDirection(0);
	pCImageParams->SetErosion(0);
	pCImageParams->SetFindEdge(0);
	pCImageParams->SetGamma(1);
	pCImageParams->SetHalftoneAngle(0);
	pCImageParams->SetHalftoneType(0);
	pCImageParams->SetHeight(0);
	pCImageParams->SetHistoContrast(0);
	pCImageParams->SetHue(100);
	pCImageParams->SetIntensityDetectMax(0);
	pCImageParams->SetIntensityDetectMin(0);
	pCImageParams->SetInterpolation(0);
	pCImageParams->SetMedian(0);
	pCImageParams->SetMosaic(0);
	pCImageParams->SetNoiseChannel(0);
	pCImageParams->SetNoiseRange(0);
	pCImageParams->SetNoiseType(0);
	pCImageParams->SetPosterize(0);
	pCImageParams->SetRadius(1);
	pCImageParams->SetSaturation(0);
	pCImageParams->SetSharpen(0);
	pCImageParams->SetSizeFilter(0);
	pCImageParams->SetSizeQualityControl(0);
	pCImageParams->SetWidth(0);
	return(TRUE);
}

/*
	SetImageParamsMinMax()
*/
BOOL CPaintLib::SetImageParamsMinMax(CImageParams* pCImageParams)
{
	pCImageParams->SetBlurMinMax(0,0);
	pCImageParams->SetBrightnessMinMax(0,500);
	pCImageParams->SetContrastMinMax(0,500);
	pCImageParams->SetDilateMinMax(0,0);
	pCImageParams->SetEmbossMinMax(0,0);
	pCImageParams->SetEmbossDirectionMinMax(0,0);
	pCImageParams->SetErosionMinMax(0,0);
	pCImageParams->SetFindEdgeMinMax(0,0);
	pCImageParams->SetGammaMinMax(0,10);
	pCImageParams->SetHalftoneAngleMinMax(0,0);
	pCImageParams->SetHalftoneTypeMinMax(0,0);
	pCImageParams->SetHeightMinMax(0,10000);
	pCImageParams->SetHistoContrastMinMax(0,0);
	pCImageParams->SetHueMinMax(0,500);
	pCImageParams->SetIntensityDetectMin_MinMax(0,0);
	pCImageParams->SetIntensityDetectMax_MinMax(0,0);
	pCImageParams->SetInterpolationMinMax(0,0);
	pCImageParams->SetMedianMinMax(0,0);
	pCImageParams->SetMosaicMinMax(0,0);
	pCImageParams->SetNoiseChannelMinMax(0,0);
	pCImageParams->SetNoiseRangeMinMax(0,0);
	pCImageParams->SetNoiseTypeMinMax(0,0);
	pCImageParams->SetPosterizeMinMax(0,0);
	pCImageParams->SetRadiusMinMax(1,50);
	pCImageParams->SetSaturationMinMax(0,100);
	pCImageParams->SetSharpenMinMax(0,0);
	pCImageParams->SetSizeFilterMinMax(0,3); // box,bilinear,gaussian,hamming
	pCImageParams->SetSizeQualityControlMinMax(0,0);
	pCImageParams->SetWidthMinMax(0,10000);
	return(TRUE);
}

/*
	Grayscale()
*/
UINT CPaintLib::Grayscale(CImageParams* /*pCImageParams*/)
{
	UINT nRet = NO_ERROR;

	if(m_Image.GetBitsPerPixel()==32)
		m_Image.MakeGrayscale(); 
	else
		nRet = ERROR_NOT_32BPP_FORMAT;

	return(nRet);
}

/*
	Halftone()
*/
UINT CPaintLib::Halftone(CImageParams*)
{
	UINT nRet = GDI_ERROR;

	UINT nBitsPerPixel = GetBPP();

	switch(nBitsPerPixel)
	{
		case 1:
		{
			// controlla se la paletta e' B/N
			RGBQUAD *pPalette = (RGBQUAD*)m_Image.GetPalette();
			
			if(pPalette[0].rgbBlue==pPalette[0].rgbGreen && pPalette[0].rgbGreen==pPalette[0].rgbRed && pPalette[1].rgbBlue==pPalette[1].rgbGreen && pPalette[1].rgbGreen==pPalette[1].rgbRed)
			{
				if((pPalette[0].rgbBlue==0 && pPalette[1].rgbBlue==255) || (pPalette[0].rgbBlue==255 && pPalette[1].rgbBlue==0))
					break; // paletta B/N

			}
			
			// paletta B/N MINISWHITE
			m_Image.SetPaletteEntry(0,255,255,255,255);
			m_Image.SetPaletteEntry(1,0,0,0,255);
			
			nRet = NO_ERROR;

			break;
		}
#if 0
		case 4:
		case 16:
		case 24:
			break;
		
		case 8:
		case 32:
			ConvertToBPP(1);
			nRet = NO_ERROR;
			break;
#endif
		default:
			return(CImage::Halftone(NULL));
	}

	return(nRet);
}

/*
	Invert()
*/
UINT CPaintLib::Invert(CImageParams* /*pCImageParams*/)
{
	UINT nRet = NO_ERROR;

#if 0
	if(m_Image.GetBPP()==32)
		m_Image.Invert();
	else
		nRet = ERROR_NOT_32BPP_FORMAT;
#else
	UINT nBitsPerPixel = m_Image.GetBitsPerPixel();

	switch (nBitsPerPixel)
	{
		case 1:
			CImage::Negate(NULL);
			break;

		case 32:
			m_Image.Invert();
			break;

		default:
			nRet = ERROR_NOT_32BPP_FORMAT;
	}
#endif

	return(nRet);
}

/*
	Rotate90Left()
*/
UINT CPaintLib::Rotate90Left(CImageParams* /*pCImageParams*/)
{
	UINT nRet = NO_ERROR;

	Rotate((double)-90.0);

	return(nRet);
}

/*
	Rotate90Right()
*/
UINT CPaintLib::Rotate90Right(CImageParams* /*pCImageParams*/)
{
	UINT nRet = NO_ERROR;

	Rotate((double)90.0);

	return(nRet);
}

/*
	Rotate180()
*/
UINT CPaintLib::Rotate180(CImageParams* /*pCImageParams*/)
{
	UINT nRet = NO_ERROR;

	Rotate((double)180.0);

	return(nRet);
}

/*
	Rotate()
*/
void CPaintLib::Rotate(double grados)
{
	const double PI = 3.14159265358979323846;  
	
	// RGB = 0=fondo immagine nero, 255=fondo immagine bianco
//	m_Image.Rotate((-1)*grados*PI/180.0,CPixel32(255,255,255,0));
	m_Image.Rotate((-1)*grados*PI/180.0,RGB(255,255,255));
}

/*
	Size()
*/
UINT CPaintLib::Size(CImageParams* pCImageParams)
{
	UINT nRet = NO_ERROR;

	if(m_Image.GetBitsPerPixel()==32)
	{
		switch(pCImageParams->GetSizeFilter())
		{
			case RESIZE_BOX:		
				m_Image.ResizeBox(pCImageParams->GetWidth(),pCImageParams->GetHeight(),pCImageParams->GetRadius());
				break;
			
			case RESIZE_GAUSSIAN:
				m_Image.ResizeGaussian(pCImageParams->GetWidth(),pCImageParams->GetHeight(),pCImageParams->GetRadius());
				break;
			
			case RESIZE_HAMMING:
				m_Image.ResizeHamming(pCImageParams->GetWidth(),pCImageParams->GetHeight(),pCImageParams->GetRadius());
				break;

			case RESIZE_BILINEAR:
			default:
				m_Image.ResizeBilinear(pCImageParams->GetWidth(),pCImageParams->GetHeight());
				break;
		 }
	}
	else
		nRet = ERROR_NOT_32BPP_FORMAT;

	return(nRet);
}

#endif // HAVE_PAINTLIB_LIBRARY
