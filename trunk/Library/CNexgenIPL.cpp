/*
	CNexgenIPL.cpp
	Classe derivata per interfaccia con NexgenIPL (v.2.9.6).
	Luca Piergentili, 01/09/00
	lpiergentili@yahoo.com

	Ad memoriam - Nemo me impune lacessit.
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include "window.h"
#include "ImageConfig.h"

#ifdef HAVE_NEXGENIPL_LIBRARY

#include <math.h>
#include <stdio.h>
#include "strings.h"
#include "CImage.h"
#include "CImageParams.h"
#include "CNexgenIPL.h"
#include "libtiff.h"

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

/*
	CNexgenIPL()
*/
CNexgenIPL::CNexgenIPL()
{
	memset(m_szFileName,'\0',sizeof(m_szFileName));
	memset(m_szFileExt,'\0',sizeof(m_szFileExt));
	m_ImageType = (IMAGE_TYPE)-1;

	// puntatore all'oggetto immagine
	m_pImage = NULL;

	LPIMAGETYPE p;
	ADDFILETYPE(BMP_PICTURE,		".bmp",	"BMP file ",						IMAGE_READ_FLAG|IMAGE_WRITE_FLAG,p)
	ADDFILETYPE(BMP_PICTURE,		".dib",	"DIB file ",						IMAGE_READ_FLAG|IMAGE_WRITE_FLAG,p)
	ADDFILETYPE(BMP_PICTURE,		".rle",	"RLE file ",						IMAGE_READ_FLAG|IMAGE_WRITE_FLAG,p)
	ADDFILETYPE(GIF_PICTURE,		".gif",	"GIF file ",						IMAGE_READ_FLAG|IMAGE_WRITE_FLAG,p)
	ADDFILETYPE(JPEG_PICTURE,	".jpg",	"JPEG file ",						IMAGE_READ_FLAG|IMAGE_WRITE_FLAG,p)
	ADDFILETYPE(JPEG_PICTURE,	".jpeg",	"JPEG file ",						IMAGE_READ_FLAG|IMAGE_WRITE_FLAG,p)
	ADDFILETYPE(JPEG_PICTURE,	".jif",	"JPEG file ",						IMAGE_READ_FLAG|IMAGE_WRITE_FLAG,p)
	ADDFILETYPE(JPEG_PICTURE,	".jpe",	"JPEG file ",						IMAGE_READ_FLAG|IMAGE_WRITE_FLAG,p)
	ADDFILETYPE(PCX_PICTURE,		".pcx",	"PCX file ",						IMAGE_READ_FLAG|IMAGE_WRITE_FLAG,p)
	ADDFILETYPE(PGM_PICTURE,		".pgm",	"Portable Graymap File ",			IMAGE_READ_FLAG|IMAGE_WRITE_FLAG,p)
	ADDFILETYPE(PNG_PICTURE,		".png",	"Portable Network Graphics Format ",	IMAGE_READ_FLAG|IMAGE_WRITE_FLAG,p)
	ADDFILETYPE(PPM_PICTURE,		".ppm",	"PPM file ",						IMAGE_READ_FLAG|IMAGE_WRITE_FLAG,p)
	ADDFILETYPE(TGA_PICTURE,		".tga",	"TARGA file ",						IMAGE_READ_FLAG|IMAGE_WRITE_FLAG,p)
	ADDFILETYPE(TIFF_PICTURE,	".tif",	"TIFF file ",						IMAGE_READ_FLAG|IMAGE_WRITE_FLAG,p)
	ADDFILETYPE(TIFF_PICTURE,	".tiff",	"TIFF file ",						IMAGE_READ_FLAG|IMAGE_WRITE_FLAG,p)

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
	ADDIMAGEOPERATION(	"Blur",				TRUE,	o)
	ADDIMAGEOPERATION(	"Despeckle",			FALSE,	o)
	ADDIMAGEOPERATION(	"Deskew",				FALSE,	o)
	ADDIMAGEOPERATION(	"Dilate",				FALSE,	o)
	ADDIMAGEOPERATION(	"EdgeEnhance",			TRUE,	o)
	ADDIMAGEOPERATION(	"Emboss",				TRUE,	o)
	ADDIMAGEOPERATION(	"Erosion",			FALSE,	o)
	ADDIMAGEOPERATION(	"FindEdge",			TRUE,	o)
	ADDIMAGEOPERATION(	"Mosaic",				FALSE,	o)
	ADDIMAGEOPERATION(	"Median",				TRUE,	o)
	ADDIMAGEOPERATION(	"MirrorHorizontal",		TRUE,	o)
	ADDIMAGEOPERATION(	"MirrorVertical",		TRUE,	o)
	ADDIMAGEOPERATION(	"Posterize",			TRUE,	o)
	ADDIMAGEOPERATION(	"Rotate90Left",		TRUE,	o)
	ADDIMAGEOPERATION(	"Rotate90Right",		TRUE,	o)
	ADDIMAGEOPERATION(	"Rotate180",			TRUE,	o)
	ADDIMAGEOPERATION(	"Sharpen",			TRUE,	o)
	ADDIMAGEOPERATION(	"Size",				TRUE,	o)

	LPIMAGETIFFTYPE t;
	ADDTIFFTYPE(COMPRESSION_NONE,		"COMPRESSION_NONE (dump mode)",					t)
	ADDTIFFTYPE(COMPRESSION_CCITTRLE,	"COMPRESSION_CCITTRLE (CCITT modified Huffman RLE)",	t)
	ADDTIFFTYPE(COMPRESSION_CCITTFAX3,	"COMPRESSION_CCITTFAX3 (CCITT Group 3 fax encoding)",	t)
	ADDTIFFTYPE(COMPRESSION_CCITTFAX4,	"COMPRESSION_CCITTFAX4 (CCITT Group 4 fax encoding)",	t)
	ADDTIFFTYPE(COMPRESSION_PACKBITS,	"COMPRESSION_PACKBITS (Macintosh RLE)",				t)
}

/*
	GetQualityRange()
*/
BOOL CNexgenIPL::GetQualityRange(int& nMin,int& nMax)
{
	if(m_pImage)
	{
		nMin = 1;
		nMax = 100;
		return(TRUE);
	}
	else
	{
		nMin = nMax = -1;
		return(FALSE);
	}
}

/*
	ConvertToBPP()
*/
UINT CNexgenIPL::ConvertToBPP(UINT nBitsPerPixel,UINT /*nFlags*/,RGBQUAD* /*pPalette*/,UINT /*nColors*/)
{
	UINT nRet = GDI_ERROR;

	if(m_pImage)
	{
		switch(nBitsPerPixel)
		{
			case 1:
			case 4:
			case 8:
			{
				if(nBitsPerPixel==GetBPP())
					return(NO_ERROR);
				else
					return(m_pImage->Quantize(nBitsPerPixel) ? NO_ERROR : GDI_ERROR);
			}

			case 24:
				return(m_pImage->ConvertTo24BPP());
			
			case 32:
			default:
				return(m_pImage->ConvertTo32BPP());
		}
	}
	
	return(nRet);
}

/*
	GetBitmap()
*/
HBITMAP CNexgenIPL::GetBitmap(BTCImageData &ImageData)
{
	static HBITMAP hBitmap = NULL;
	BITMAPINFOHEADER* pBitmapInfoHeader = (BITMAPINFOHEADER*)ImageData.GetBitmapInfo();
	unsigned char* pData = ImageData.GetBits();
	
	if(pBitmapInfoHeader && pData)
	{
		HDC hDC = ::CreateCompatibleDC(NULL);
 		if(hDC)
		{
			void* pVoid;
			HBITMAP hBitmap = ::CreateDIBSection(hDC,(LPBITMAPINFO)pBitmapInfoHeader,DIB_RGB_COLORS,&pVoid,0,0);
			if(hBitmap)
				::SetDIBits(hDC,hBitmap,0,pBitmapInfoHeader->biHeight * (-1),pData,(LPBITMAPINFO)pBitmapInfoHeader,DIB_RGB_COLORS);
			::DeleteDC(hDC);
		}
	}
	
	return(hBitmap);
}

/*
	GetDIB()
*/
HDIB CNexgenIPL::GetDIB(DIBINFO* /*pDibInfo = NULL*/)
{
	HDIB hDib = (HDIB)NULL;
	
	if(m_pImage)
	{
		CImageParams ImageParams;
		ImageParams.Reset();

		if(GetDIBOrder() < 0)
			MirrorHorizontal(&ImageParams);

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
	}

	return(hDib);
}

/*
	SetDIB()
*/
BOOL CNexgenIPL::SetDIB(HDIB hDib,DIBINFO* /*pDibInfo = NULL*/)
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
		else
		{
			bSet = m_pImage->FromDIB(hDib); // per incompatibilita' tra la 2.7 e la 2.9 con la Create()
		}
				
		::GlobalUnlock((HGLOBAL)hDib);
	}

	return(bSet);
}

/*
	SetPalette()
*/
BOOL CNexgenIPL::SetPalette(UINT nIndex,UINT nColors,RGBQUAD* pPalette)
{
	BOOL bPalette = FALSE;
	
	if(m_pImage)
	{
		UINT nColorData = GetNumColors();
		
		if(!nColorData || nIndex!=0 || pPalette==NULL || nColors <= 0 || (nColors+nIndex) > nColorData)
			;
		else
		{
			m_pImage->FillColorTable(pPalette,nColors);
			bPalette = TRUE;
		}
	}

	return(bPalette);
}

/*
	Create()
*/
BOOL CNexgenIPL::Create(BITMAPINFO *pBitmapInfo,void *pData/* = NULL */)
{
	BOOL bCreated = FALSE;

	if(pBitmapInfo)
	{
		m_NexgenObject.GetObjectData().DeleteObject();

		BITMAPINFOHEADER* pBitmapInfoHeader = (BITMAPINFOHEADER*)pBitmapInfo;
		RGBQUAD* pPalette = pBitmapInfo->bmiColors;
		
		if(pBitmapInfoHeader && pPalette)
		{
			// per incompatibilita' tra la 2.7 e la 2.9 (pDataDst e' a null)
			if(m_NexgenObject.GetObjectData().Create(pBitmapInfoHeader->biWidth,abs(pBitmapInfoHeader->biHeight),pBitmapInfoHeader->biBitCount))
			{
				if((m_pImage = m_NexgenObject.GetObjectDataPtr())!=(BTCImageData*)NULL)
				{
					int nColorData = 0;
					if(pBitmapInfoHeader->biBitCount <= 8)
						nColorData = 1 << pBitmapInfoHeader->biBitCount;
					if(nColorData)
						m_NexgenObject.GetObjectData().FillColorTable(pPalette,nColorData);
					
					unsigned char* pDataDst = m_NexgenObject.GetObjectDataPtr()->GetBits(0,0);
					if(pDataDst)
					{
						int nWidthEnBytes = WIDTHBYTES(pBitmapInfoHeader->biWidth * pBitmapInfoHeader->biBitCount,GetAlignment());
						int nTotalBytes = abs(pBitmapInfoHeader->biHeight) * nWidthEnBytes;
						if(pData)
							memcpy(pDataDst,pData,nTotalBytes);
						bCreated = TRUE;
					}
				}
			}
		}
	}

	return(bCreated);
}

/*
	Load()
*/
BOOL CNexgenIPL::Load(LPCSTR lpcszFileName)
{
	Unload(); // propria

	__try {
		CImage::Load(lpcszFileName); // classe base

		if(m_NexgenObject.Load(lpcszFileName)!=-1)
			m_pImage = m_NexgenObject.GetObjectDataPtr();
		else
			m_pImage = NULL;
	}
	__except(EXCEPTION_EXECUTE_HANDLER) {
		m_pImage = NULL;
	}

	if(m_pImage)
	{
		strcpyn(m_szFileName,lpcszFileName,sizeof(m_szFileName));
		char* p = strrchr(m_szFileName,'.');
		if(p)
			strcpyn(m_szFileExt,p,sizeof(m_szFileExt));
		
		LPIMAGETYPE pImagetype;
		while((pImagetype = EnumImageFormats())!=NULL)
			if(stricmp(m_szFileExt,pImagetype->ext)==0)
				m_ImageType = pImagetype->type;
		
		m_InfoHeader.width = m_pImage->GetWidth();
		m_InfoHeader.height = m_pImage->GetHeight();
		m_InfoHeader.bpp = m_pImage->GetBitsPerPixel();
		m_InfoHeader.colors = m_pImage->GetNumColorEntries(m_pImage->GetBitsPerPixel());
		m_InfoHeader.memused = m_pImage->GetImageSize(m_pImage->GetWidth(),m_pImage->GetHeight(),m_pImage->GetBitsPerPixel());
	}
	else
	{
		memset(m_szFileName,'\0',sizeof(m_szFileName));
		memset(m_szFileExt,'\0',sizeof(m_szFileExt));
		m_ImageType = (IMAGE_TYPE)-1;
	}

	return(m_pImage!=NULL);
}

/*
	Unload()
*/
BOOL CNexgenIPL::Unload(void)
{
	CImage::Unload(); // classe base

	m_NexgenObject.GetObjectDataPtr()->DeleteObject();

	m_pImage = NULL;
	memset(m_szFileName,'\0',sizeof(m_szFileName));
	memset(m_szFileExt,'\0',sizeof(m_szFileExt));
	m_ImageType = (IMAGE_TYPE)-1;

	return(m_pImage==NULL);
}

/*
	Save()
*/
BOOL CNexgenIPL::Save(LPCSTR lpcszFileName,LPCSTR lpcszFormat,DWORD)
{
	BOOL bSaved = FALSE;

	if(m_pImage)
	{
		long lCodecId = m_NexgenObject.GetCodecIdFromExtension(lpcszFormat[0]=='.' ? lpcszFormat+1 : lpcszFormat,BTCODECTYPE_ENCODER);
		if(lCodecId!=-1L)
		{
			BTCString bctStrFileName = lpcszFileName;
			bSaved = m_NexgenObject.Save(bctStrFileName,lCodecId)==BT_S_OK;
		}
	}

	return(bSaved);
}

/*
	Save()
*/
BOOL CNexgenIPL::Save(void)
{
	BOOL bSaved = FALSE;

	if(m_pImage && m_szFileName[0]!='\0' && m_szFileExt[0]!='\0' && m_ImageType!=(IMAGE_TYPE)-1)
		bSaved = Save(m_szFileName,m_szFileExt,0L);

	return(bSaved);
}

/*
	SaveTIFF()
*/
BOOL CNexgenIPL::SaveTIFF(LPCTSTR lpcszInputFile)
{
	BOOL bRet = FALSE;

	TIFF* tiff = TIFFOpen(lpcszInputFile,"wb");
	if(tiff)
	{
		CImage* pImage = this;
		UINT iBitsPerPixel = pImage->GetBPP();
		UINT nWidth = pImage->GetWidth();
		UINT nHeight = pImage->GetHeight();

		TIFFSetField(tiff,TIFFTAG_BITSPERSAMPLE,iBitsPerPixel);
		TIFFSetField(tiff,TIFFTAG_SAMPLESPERPIXEL,1);
		TIFFSetField(tiff,TIFFTAG_COMPRESSION,pImage->GetCompression());           

		TIFFSetField(tiff,TIFFTAG_IMAGEWIDTH,nWidth);
		TIFFSetField(tiff,TIFFTAG_IMAGELENGTH,nHeight);

		TIFFSetField(tiff,TIFFTAG_XRESOLUTION,(float)pImage->GetXRes());
		TIFFSetField(tiff,TIFFTAG_YRESOLUTION,(float)pImage->GetYRes());
		
		int nURes = pImage->GetURes();
		switch(nURes)
		{
			case RESUNITINCH:
				nURes = RESUNIT_INCH;
				break;
			case RESUNITCENTIMETER:
				nURes = RESUNIT_CENTIMETER;
				break;
			//RESUNITNONE:
			default:
				nURes = RESUNIT_NONE;
				break;
		}
		
		TIFFSetField(tiff,TIFFTAG_RESOLUTIONUNIT,(unsigned short)nURes);
		TIFFSetField(tiff,TIFFTAG_PLANARCONFIG,PLANARCONFIG_CONTIG);
		TIFFSetField(tiff,TIFFTAG_ORIENTATION,ORIENTATION_TOPLEFT);

		// numero pagina, descrizione e colori (mancano)
		#if 0
		TIFFSetField(tiff,TIFFTAG_PAGENAME,???);
		TIFFSetField(tiff,TIFFTAG_IMAGEDESCRIPTION,???);
		TIFFSetField(tiff,TIFFTAG_PRIMARYCHROMATICITIES,???);
		TIFFSetField(tiff,TIFFTAG_WHITEPOINT,???);
		#endif

		unsigned short nPhotometric = (unsigned short)pImage->GetPhotometric();
		switch(nPhotometric)
		{
			case PHOTOMETRICMINISBLACK:
				nPhotometric = PHOTOMETRIC_MINISBLACK;
				break;
			case PHOTOMETRICMINISWHITE:
				nPhotometric = PHOTOMETRIC_MINISWHITE;
				break;
			case PHOTOMETRICPALETTE:
				nPhotometric = PHOTOMETRIC_PALETTE;
				break;
			case PHOTOMETRICRGB:
				nPhotometric = PHOTOMETRIC_RGB;
				break;
		}
		
		TIFFSetField(tiff,TIFFTAG_PHOTOMETRIC,nPhotometric);
		
		UINT nColorData = 0;
		BITMAPINFO* pBitmapInfo = pImage->GetBMI();
		
		if(pBitmapInfo && pBitmapInfo->bmiHeader.biBitCount <= 8)
			nColorData = 1 << pBitmapInfo->bmiHeader.biBitCount;

		if(nPhotometric==PHOTOMETRIC_PALETTE && nColorData)
		{
			unsigned short* red = new unsigned short[nColorData];;
			unsigned short* green = new unsigned short[nColorData];
			unsigned short* blue = new unsigned short[nColorData];

			if(red && green && blue)
			{
				register int i;

				for(i = 0; i < (int)nColorData; i++)
					red[i] = pBitmapInfo->bmiColors[i].rgbRed << 8;
				for(i = 0; i < (int)nColorData; i++)
					green[i] = pBitmapInfo->bmiColors[i].rgbGreen << 8;
				for(i = 0; i < (int)nColorData; i++)
					blue[i] = pBitmapInfo->bmiColors[i].rgbBlue << 8;

				TIFFSetField(tiff,TIFFTAG_COLORMAP,red,green,blue);
				delete [] red;
				delete [] green;
				delete [] blue;
			}
		}

		if(pImage->LockData())
		{
			unsigned char* pData = (unsigned char*)pImage->GetPixels();
			register unsigned int i;
			UINT nWidthBytes = pImage->GetBytesWidth();
			
			switch(iBitsPerPixel)
			{
				case 24:
				case 32:
				{
					#define RGBA_RED 2
					#define RGBA_GREEN 1
					#define RGBA_BLUE 0
					#define RGBA_ALPHA 3

					// todo: check whether (r,g,b) components come in the correct order here...
					int iRGBAPixel = iBitsPerPixel/8;
					BYTE* pBuf = new BYTE[iRGBAPixel*nWidth];
					if(pBuf)
					{
						BYTE* pBufAux = (pImage->GetDIBOrder()==-1) ? pData : pData+((nHeight-1)*nWidthBytes); // top-left
						BOOL bAlpha = iRGBAPixel==4 ? TRUE : FALSE;
					
						if(pImage->GetDIBOrder()==-1)
						{
							for(register int l=0; l < (int)nHeight; l++)
							{
								for(register int c=0; c < (int)nWidth; c++)
								{
									pBuf[c * iRGBAPixel + 0] = pBufAux[c * iRGBAPixel + RGBA_RED];
									pBuf[c * iRGBAPixel + 1] = pBufAux[c * iRGBAPixel + RGBA_GREEN];
									pBuf[c * iRGBAPixel + 2] = pBufAux[c * iRGBAPixel + RGBA_BLUE];
									if(bAlpha)
										pBuf[c * iRGBAPixel + 3] = pBufAux[c * iRGBAPixel + RGBA_ALPHA];
								}
								
								TIFFWriteScanline(tiff,pBuf,l,0);
								
								pBufAux += nWidthBytes;
							}
						}
						else
						{
							for(register int l=0; l < (int)nHeight; l++)
							{
								for(register int c=0; c < (int)nWidth; c++)
								{
									pBuf[c * iRGBAPixel + 0] = pBufAux[c * iRGBAPixel + RGBA_RED];
									pBuf[c * iRGBAPixel + 1] = pBufAux[c * iRGBAPixel + RGBA_GREEN];
									pBuf[c * iRGBAPixel + 2] = pBufAux[c * iRGBAPixel + RGBA_BLUE];
									if(bAlpha)
										pBuf[c * iRGBAPixel + 3] = pBufAux[c * iRGBAPixel + RGBA_ALPHA];
								}

								TIFFWriteScanline(tiff,pBuf,l,0);
								pBufAux -= nWidthBytes;
							}
						}

						delete [] pBuf;
					}
				}
				break;

				default:
				{
					if(pImage->GetDIBOrder()==-1) // top-left
					{	
						for(i=0; i < nHeight; i++)
						{
							TIFFWriteScanline(tiff,pData,i,0);
							pData += nWidthBytes;;
						}
					}
					else
					{
						unsigned char* pBuf = pData+((nHeight-1)*nWidthBytes);
						for(i=0; i < nHeight; i++)
						{
							TIFFWriteScanline(tiff,pBuf,i,0);
							pBuf -= nWidthBytes;
						}
					}
				}
				break;

			}

			TIFFWriteDirectory(tiff);
			pImage->UnlockData();
			bRet = TRUE;
		}

		TIFFClose(tiff);
	}

	return(bRet);
}

/*
	Stretch()
*/
BOOL CNexgenIPL::Stretch(RECT& drawRect,BOOL bAspectRatio/*=TRUE*/)
{
	BOOL bStretched = FALSE;

	if(m_pImage)
	{
		if(m_pImage->GetBitsPerPixel()!=32)
			m_pImage->ConvertTo32BPP();

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

		if((int)nHeight <= 0)
			nHeight = 1;
		if((int)nWidth <= 0)
			nWidth = 1;

		BTCImageData::BTResizeFilter Filter;

		//case RESIZE_BOX:
//		Filter = BTCImageData::Box;		//	ok, cacca in resize

		//case RESIZE_BILINEAR:
		Filter = BTCImageData::Bilinear;	//	ok (default)

		//case RESIZE_GAUSSIAN:
//		Filter = BTCImageData::Gaussian;	//	cacca totale (blur)

		//case RESIZE_HAMMING:
//		Filter = BTCImageData::Hamming;	//	ok, cacca in resize

		//case RESIZE_BLACKMAN:
//		Filter = BTCImageData::Blackman;	//	ok, cacca in resize

		bStretched = m_pImage->Resize((int)nWidth,(int)nHeight,Filter);
	}

	return(bStretched);
}

/*
	Paste()
*/
BOOL CNexgenIPL::Paste(UINT x,UINT y,HANDLE hDIB)
{
	UINT nRet = GDI_ERROR;
	BOOL bLocked = FALSE;
	
	HDC hDC = NULL;
	BITMAPINFO *pBitmapInfoSrc = NULL;
	BITMAPINFO *pBitmapInfo = NULL;
	HBITMAP hBitmap = NULL;
	HBITMAP hOldBitmap = NULL;
	HPALETTE hPal = NULL;
	HPALETTE hOldPal = NULL;
	UINT nWidth;
	UINT nHeight;

	if(LockData())
	{
		bLocked = TRUE;

		hDC = ::CreateCompatibleDC(NULL);
		if(!hDC)
			goto done;

		pBitmapInfoSrc = GetBMI();
		unsigned char *pDataSrc = (unsigned char*)GetPixels();
		if(!pBitmapInfoSrc || !pDataSrc)
			goto done;

		pBitmapInfo = (BITMAPINFO*)new char[sizeof(BITMAPINFOHEADER) + (GetMaxPaletteColors() * sizeof(RGBQUAD))];
		if(!pBitmapInfo)
			goto done;

		memset(pBitmapInfo,'\0',sizeof(BITMAPINFOHEADER) + (GetMaxPaletteColors() * sizeof(RGBQUAD)));
		memcpy(pBitmapInfo,pBitmapInfoSrc,sizeof(BITMAPINFOHEADER) + (GetNumColors() * sizeof(RGBQUAD)));

		void* pVoid;		
		hBitmap = ::CreateDIBSection(hDC,(LPBITMAPINFO)pBitmapInfo,DIB_RGB_COLORS,&pVoid,0,0);
		if(!hBitmap)
			goto done;

		hOldBitmap = (HBITMAP)::SelectObject(hDC,hBitmap);

		hPal = CreateDIBPalette((BITMAPINFO*)pBitmapInfoSrc);
   		if(hPal)
		{
			hOldPal = ::SelectPalette(hDC,hPal,FALSE);
			::RealizePalette(hDC);
		}
		
		nWidth = GetWidth();
		nHeight = GetHeight();
	
		nRet = ::StretchDIBits(	hDC,
							0,
							0,
							nWidth,
							nHeight,
							0,
							0,
							nWidth,
							nHeight,
							(LPSTR)GetPixels(),
							(BITMAPINFO*)pBitmapInfoSrc,
							DIB_RGB_COLORS,
							SRCCOPY);
		
		BITMAPINFOHEADER* pBitmapInfoHeader = (BITMAPINFOHEADER*)GlobalLock(hDIB);
		int nColorData = 0;
		if(pBitmapInfoHeader->biBitCount <= 8)
			nColorData = 1 << pBitmapInfoHeader->biBitCount;
		nRet = ::SetDIBitsToDevice(	hDC,
								x,
								y,
								pBitmapInfoHeader->biWidth,
								pBitmapInfoHeader->biHeight,
								0,
								0,
								0,
								pBitmapInfoHeader->biHeight,
								(LPSTR)pBitmapInfoHeader + sizeof(BITMAPINFOHEADER) + (nColorData * sizeof(RGBQUAD)),
								(BITMAPINFO*)pBitmapInfoHeader,
								DIB_RGB_COLORS
								)==0 ? GDI_ERROR : nRet;
		
		::SelectObject(hDC,hOldBitmap);

		bLocked = !UnlockData();

		if(Create(pBitmapInfo) && LockData())
		{
			nRet = ::GetDIBits(hDC,hBitmap,0,nHeight,GetPixels(),GetBMI(),DIB_RGB_COLORS)==0 ? GDI_ERROR : nRet;
			bLocked = !UnlockData(TRUE);
		}
			
		if(hOldPal)
			::SelectPalette(hDC,hOldPal,TRUE);			
	}

done:

	if(hDC)
		::DeleteDC(hDC);
	if(pBitmapInfo)
		delete [] pBitmapInfo;
	if(hBitmap)
		 ::DeleteObject(hBitmap);
	if(hPal)
		 ::DeleteObject(hPal);

	if(bLocked)
		UnlockData();
	
	return(nRet);
}

/*
	SetImageParamsDefaultValues()
*/
BOOL CNexgenIPL::SetImageParamsDefaultValues(CImageParams* pCImageParams)
{
	pCImageParams->SetBlur(0);
	pCImageParams->SetBrightness(0);
	pCImageParams->SetContrast(0);
	pCImageParams->SetDilate(0);
	pCImageParams->SetEmboss(0);
	pCImageParams->SetEmbossDirection(0);
	pCImageParams->SetErosion(0);
	pCImageParams->SetFindEdge(60);
	pCImageParams->SetGamma(1);
	pCImageParams->SetHalftoneAngle(0);  
	pCImageParams->SetHalftoneType(0);
	pCImageParams->SetHeight(0);
	pCImageParams->SetHistoContrast(0);
	pCImageParams->SetHue(0);
	pCImageParams->SetIntensityDetectMax(0);
	pCImageParams->SetIntensityDetectMin(0);
	pCImageParams->SetInterpolation(1);
	pCImageParams->SetMedian(1);
	pCImageParams->SetMosaic(0);
	pCImageParams->SetNoiseChannel(0);
	pCImageParams->SetNoiseRange(0);
	pCImageParams->SetNoiseType(0);
	pCImageParams->SetPosterize(7);
	pCImageParams->SetRadius(0);
	pCImageParams->SetSaturation(0);
	pCImageParams->SetSharpen(86);
	pCImageParams->SetSizeFilter(0);
	pCImageParams->SetSizeQualityControl(0);
	pCImageParams->SetWidth(0);
	return(TRUE);
}

/*
	SetImageParamsMinMax()
*/
BOOL CNexgenIPL::SetImageParamsMinMax(CImageParams* pCImageParams)
{
	pCImageParams->SetBlurMinMax(0,0);
	pCImageParams->SetBrightnessMinMax(-100,100);
	pCImageParams->SetContrastMinMax(-100,100);
	pCImageParams->SetDilateMinMax(0,0);
	pCImageParams->SetEmbossMinMax(0,0);
	pCImageParams->SetEmbossDirectionMinMax(0,0);
	pCImageParams->SetErosionMinMax(0,0);
	pCImageParams->SetFindEdgeMinMax(0,100);
	pCImageParams->SetGammaMinMax(0,5);
	pCImageParams->SetHalftoneAngleMinMax(0,0);
	pCImageParams->SetHalftoneTypeMinMax(0,0);
	pCImageParams->SetHeightMinMax(0,5000);		
	pCImageParams->SetHistoContrastMinMax(0,0);
	pCImageParams->SetHueMinMax(-180,180);
	pCImageParams->SetIntensityDetectMin_MinMax(0,0);
	pCImageParams->SetIntensityDetectMax_MinMax(0,0);
	pCImageParams->SetInterpolationMinMax(0,0);
	pCImageParams->SetMedianMinMax(1,30);
	pCImageParams->SetMosaicMinMax(0,0);
	pCImageParams->SetNoiseChannelMinMax(0,0);
	pCImageParams->SetNoiseRangeMinMax(0,0);
	pCImageParams->SetNoiseTypeMinMax(0,0);
	pCImageParams->SetPosterizeMinMax(2,255);
	pCImageParams->SetRadiusMinMax(0,0);
	pCImageParams->SetSaturationMinMax(-100,100);
	pCImageParams->SetSharpenMinMax(0,100);
	pCImageParams->SetSizeFilterMinMax(0,4); //box,bilinear,gaussian,hamming,blackman
	pCImageParams->SetSizeQualityControlMinMax(0,0);
	pCImageParams->SetWidthMinMax(0,5000);
	return(TRUE);
}

/*
	Blur()
*/
UINT CNexgenIPL::Blur(CImageParams* /*pCImageParams*/)
{
	UINT nRet = NO_ERROR;

	if(m_pImage)
	{
		if(m_pImage->GetBitsPerPixel()==32)
			m_pImage->Blur();
		else
			m_pImage->Smooth();
	}
	else
		nRet = GDI_ERROR;

	return(nRet);
}

/*
	Brightness()
*/
UINT CNexgenIPL::Brightness(CImageParams* pCImageParams)
{
	UINT nRet = NO_ERROR;

	if(m_pImage)
	{
		if(m_pImage->GetBitsPerPixel()!=32)
			m_pImage->ConvertTo32BPP();
		
		m_pImage->AdjustBrightness((int)pCImageParams->GetBrightness());
	}
	else
		nRet = GDI_ERROR;

	return(nRet);
}

/*
	Contrast()
*/
UINT CNexgenIPL::Contrast(CImageParams* pCImageParams)
{
	UINT nRet = NO_ERROR;

	if(m_pImage)
	{
		if(m_pImage->GetBitsPerPixel()!=32)
			m_pImage->ConvertTo32BPP();
		
		m_pImage->AdjustContrast(pCImageParams->GetContrast());
	}
	else
		nRet = GDI_ERROR;

	return(nRet);
}

/*
	EdgeEnhance()
*/
UINT CNexgenIPL::EdgeEnhance(CImageParams* /*pCImageParams*/)
{
	UINT nRet = NO_ERROR;

	if(m_pImage)
		m_pImage->EdgeEnhance();
	else
		nRet = GDI_ERROR;

	return(nRet);
}

/*
	Emboss()
*/
UINT CNexgenIPL::Emboss(CImageParams* /*pCImageParams*/)
{
	UINT nRet = NO_ERROR;

	if(m_pImage)
		m_pImage->Emboss();
	else
		nRet = GDI_ERROR;

	return(nRet);
}

/*
	FindEdge()
*/
UINT CNexgenIPL::FindEdge(CImageParams* pCImageParams)
{
	UINT nRet = NO_ERROR;

	if(m_pImage)
		m_pImage->FindEdge(pCImageParams->GetFindEdge());
	else
		nRet = GDI_ERROR;

	return(nRet);
}

/*
	GammaCorrection()
*/
UINT CNexgenIPL::GammaCorrection(CImageParams* pCImageParams)
{
	UINT nRet = NO_ERROR;

	if(m_pImage)
		m_pImage->AdjustGamma(pCImageParams->GetGamma(),pCImageParams->GetGamma(),pCImageParams->GetGamma());
	else
		nRet = GDI_ERROR;

	return(nRet);
}

/*
	Grayscale()
*/
UINT CNexgenIPL::Grayscale(CImageParams* /*pCImageParams*/)
{
	UINT nRet = NO_ERROR;

	if(m_pImage)
		m_pImage->Grayscale();
	else
		nRet = GDI_ERROR;

	return(nRet);
}

/*
	Halftone()
*/
UINT CNexgenIPL::Halftone(CImageParams* /*pCImageParams*/)
{
	UINT nRet = GDI_ERROR;
	
	if(m_pImage)
	{
		if(GetBPP()==1)
		{
			// cambio della paletta a B/N
			RGBQUAD Palette[2];
			RGBQUAD* pPalette = m_pImage->GetColorTable();

			if(pPalette[0].rgbBlue==pPalette[0].rgbGreen && pPalette[0].rgbGreen==pPalette[0].rgbRed && pPalette[1].rgbBlue==pPalette[1].rgbGreen && pPalette[1].rgbGreen==pPalette[1].rgbRed)
			{
				if((pPalette[0].rgbBlue==0 && pPalette[1].rgbBlue==255) || (pPalette[0].rgbBlue==255 && pPalette[1].rgbBlue==0))
					return(NO_ERROR); // e' gia' B/N
			}
			
			// paletta B/N MINISWHITE
			Palette[0].rgbBlue = Palette[0].rgbGreen = Palette[0].rgbRed = 255;
			Palette[1].rgbBlue = Palette[1].rgbGreen = Palette[1].rgbRed = 0;
			m_pImage->FillColorTable(Palette,2);

			return(NO_ERROR);
		}
		
		nRet = CImage::Halftone(NULL);
	}

	return(nRet);
}

/*
	Hue()
*/
UINT CNexgenIPL::Hue(CImageParams* pCImageParams)
{
	UINT nRet = NO_ERROR;

	if(m_pImage)
	{
		if(m_pImage->GetBitsPerPixel()!=32)
			m_pImage->ConvertTo32BPP();
		
		m_pImage->AdjustHue((int)pCImageParams->GetHue());
	}
	else
		nRet = GDI_ERROR;

	return(nRet);
}

/*
	MirrorVertical()
*/
UINT CNexgenIPL::MirrorVertical(CImageParams* /*pCImageParams*/) 
{
	UINT nRet = NO_ERROR;

	if(m_pImage)
	{
		if(m_pImage->GetBitsPerPixel()==32)
			m_pImage->Mirror();
		else
		{
			m_pImage->Flip();
			m_pImage->Rotate90(BTCImageData::Right);
			m_pImage->Rotate90(BTCImageData::Right);
		}
	}
	else
		nRet = GDI_ERROR;

	return(nRet);
}

/*
	Posterize()
*/
UINT CNexgenIPL::Posterize(CImageParams* pCImageParams)
{
	UINT nRet = NO_ERROR;

	if(m_pImage)
	{
		int n = pCImageParams->GetPosterize();
		if(!m_pImage->Posterize(n))
			nRet = GDI_ERROR;
	}
	else
		nRet = GDI_ERROR;

	return(nRet);
}

/*
	Rotate180()
*/
UINT CNexgenIPL::Rotate180(CImageParams* /*pCImageParams*/)
{
	UINT nRet = NO_ERROR;

	if(m_pImage)
	{
		m_pImage->Rotate90(BTCImageData::Right);
		m_pImage->Rotate90(BTCImageData::Right);
	}
	else
		nRet = GDI_ERROR;

	return(nRet);
}

/*
	Rotate()
*/
BOOL CNexgenIPL::Rotate(int nDegree)
{
	BOOL bRotate = FALSE;
	
	if(m_pImage)
	{
		bRotate= TRUE;
		enum BTCImageData::BTDirection Direction;

		switch(nDegree)
		{
			case -90:
			case 270:
				Direction = BTCImageData::Left;
				m_pImage->Rotate90(Direction);
				break;
			
			case 90:
			case -270:
				Direction = BTCImageData::Right;
				m_pImage->Rotate90(Direction);
				break;
			
			case 180:
				Direction = BTCImageData::Right;
				m_pImage->Rotate90(Direction);
				m_pImage->Rotate90(Direction);
				break;
			
			default: 
				bRotate = FALSE;
		}
	}
	
	return(bRotate);
}

/*
	SaturationHorizontal()
*/
UINT CNexgenIPL::SaturationHorizontal(CImageParams* pCImageParams)
{
	UINT nRet = NO_ERROR;

	if(m_pImage)
	{
		if(m_pImage->GetBitsPerPixel()!=32)
			m_pImage->ConvertTo32BPP();
		
		m_pImage->AdjustSaturation((int)pCImageParams->GetSaturation());
	}
	else
		nRet = GDI_ERROR;

	return(nRet);
}

/*
	SaturationVertical()
*/
UINT CNexgenIPL::SaturationVertical(CImageParams* pCImageParams)
{
	UINT nRet = NO_ERROR;

	if(m_pImage)
	{
		if(m_pImage->GetBitsPerPixel()!=32)
			m_pImage->ConvertTo32BPP();
		
		m_pImage->AdjustSaturation((int)pCImageParams->GetSaturation());
	}
	else
		nRet = GDI_ERROR;

	return(nRet);
}

/*
	Sharpen()
*/
UINT CNexgenIPL::Sharpen(CImageParams* /*pCImageParams*/)
{
	UINT nRet = NO_ERROR;

	if(m_pImage)
		m_pImage->Sharpen();
	else
		nRet = GDI_ERROR;

	return(nRet);
}

/*
	Size()
*/
UINT CNexgenIPL::Size(CImageParams* pCImageParams)
{
	UINT nRet = NO_ERROR;

	if(m_pImage)
	{
		if(m_pImage->GetBitsPerPixel()!=32)
			m_pImage->ConvertTo32BPP();

		BTCImageData::BTResizeFilter Filter;
		
		switch(pCImageParams->GetSizeFilter())
		{
			case RESIZE_BILINEAR:
				Filter = BTCImageData::Bilinear;
				break;
		
			case RESIZE_GAUSSIAN:
				Filter = BTCImageData::Gaussian;
				break;
		
			case RESIZE_HAMMING:
				Filter = BTCImageData::Hamming;
				break;
		
			case RESIZE_BLACKMAN:
				Filter = BTCImageData::Blackman;
				break;
		
			case RESIZE_BOX:
			default:
				Filter = BTCImageData::Box;
				break;
		}
		
		m_pImage->Resize(pCImageParams->GetWidth(),pCImageParams->GetHeight(),Filter);
	}
	else
		nRet = GDI_ERROR;

	return(nRet);
}

#endif // HAVE_NEXGENIPL_LIBRARY
