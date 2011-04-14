/*
	CImage.cpp
	Classe base per l'interfaccia con le librerie.
	Luca Piergentili, 01/09/00
	lpiergentili@yahoo.com

	Ad memoriam - Nemo me impune lacessit.
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include <string.h>
#include <math.h>
#include "window.h"
#include "CImage.h"
#include "CNodeList.h"
#include "libjpeg.h"
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
	CImage()
*/
CImage::CImage()
{
	memset(m_szFileName,'\0',sizeof(m_szFileName));
	memset(m_szFileExt,'\0',sizeof(m_szFileExt));
	m_InfoHeader.type = NULL_PICTURE;
	m_InfoHeader.xres = -1;
	m_InfoHeader.yres = -1;
	m_InfoHeader.restype = -1;
	m_InfoHeader.compression = -1;
	m_InfoHeader.quality = -1;
	m_InfoHeader.filesize = (unsigned long)-1L;
	m_InfoHeader.width = (unsigned long)-1L;
	m_InfoHeader.height = (unsigned long)-1L;
	m_ImageType = NULL_PICTURE;
	m_ImageTypeList.DeleteAll();
	m_ImageOperationList.DeleteAll();
	m_TiffTypeList.DeleteAll();
	m_bShowErrors = TRUE;
}

/*
	GetFileName()
*/
LPCSTR CImage::GetFileName(void)
{
	char *p = strrchr(m_szFileName,'\\');
	if(p)
		p++;
	if(!p)
		p = m_szFileName;
	return(p);
}

/*
	GetFileSize()
*/
DWORD CImage::GetFileSize(void)
{
	if(m_InfoHeader.filesize==(unsigned long)-1L)
		GetHeaderInfo(m_szFileName,&m_InfoHeader);

	return(m_InfoHeader.filesize);
}

/*
	GetXRes()
*/
float CImage::GetXRes(void)
{
	if(m_InfoHeader.xres==-1)
		GetHeaderInfo(m_szFileName,&m_InfoHeader);

	return(m_InfoHeader.xres);
}

/*
	GetYRes()
*/
float CImage::GetYRes(void)
{
	if(m_InfoHeader.yres==-1)
		GetHeaderInfo(m_szFileName,&m_InfoHeader);

	return(m_InfoHeader.yres);
}

/*
	GetURes()
*/
int CImage::GetURes(void)
{
	if(m_InfoHeader.restype==-1)
		GetHeaderInfo(m_szFileName,&m_InfoHeader);

	return(m_InfoHeader.restype);
}

/*
	SetURes()
*/
void CImage::SetURes(UINT nURes)
{
	if(m_InfoHeader.restype!=(int)nURes && m_InfoHeader.restype!=RESUNITNONE && nURes!=RESUNITNONE && m_InfoHeader.restype!=-1)
	{
		switch(nURes)
		{
			case RESUNITINCH:
			{
				// centimetri
				float nXRes = GetXRes() / 2.54f;
				float nYRes = GetYRes() / 2.54f;
				SetXRes(nXRes);
				SetYRes(nYRes);
				break;
			}

			case RESUNITCENTIMETER:
			{
				// inch
				float nXRes = GetXRes() * 2.54f;
				float nYRes = GetYRes() * 2.54f;
				SetXRes(nXRes);
				SetYRes(nYRes);
				break;
			}

			default:
				return;
		}
	}

	m_InfoHeader.restype = nURes;
}

/*
	GetDPI()
*/
void CImage::GetDPI(float& nXRes,float& nYRes)
{
	if(m_InfoHeader.xres==-1 || m_InfoHeader.yres==-1)
		GetHeaderInfo(m_szFileName,&m_InfoHeader);

	nXRes = m_InfoHeader.xres;
	nYRes = m_InfoHeader.yres;
}

/*
	GetCompression()
*/
int CImage::GetCompression(void)
{
	if(m_InfoHeader.compression==-1)
		GetHeaderInfo(m_szFileName,&m_InfoHeader);
	
	return(m_InfoHeader.compression);
}

/*
	GetQuality()
*/
int CImage::GetQuality(void)
{
	if(m_InfoHeader.quality==-1)
		GetHeaderInfo(m_szFileName,&m_InfoHeader);
	
	return(m_InfoHeader.quality);
}

/*
	GetPhotometric()
*/
PHOTOMETRIC CImage::GetPhotometric(void)
{
	BITMAPINFO* pBmi = GetBMI();
	BITMAPINFOHEADER* pBitmapInfoHeader = (BITMAPINFOHEADER*)pBmi;
	PHOTOMETRIC photometric = NOPHOTOMETRIC;
		
	if(pBitmapInfoHeader)
	{
		switch(pBitmapInfoHeader->biBitCount)
		{
			case 1:
			{
				if(pBmi->bmiColors[0].rgbRed==0 && pBmi->bmiColors[0].rgbGreen==0 && pBmi->bmiColors[0].rgbBlue==0)
					photometric = PHOTOMETRICMINISBLACK;
				else
					photometric = PHOTOMETRICMINISWHITE;
				break;
			}

			case 4:
			case 8:
				photometric = PHOTOMETRICPALETTE;
				break;
			
			default:
				photometric = PHOTOMETRICRGB;
				break;
		}
	}

	return(photometric);
}

/*
	GetNumColors()
*/
UINT CImage::GetNumColors(void)
{
	UINT nNumColors = 0;

	switch(GetBPP())
	{
		case 1:
			nNumColors = 2;
			break;
		case 4:
			nNumColors = 16;
			break;
		case 8:
			nNumColors = 256;
			break;
	}

	return(nNumColors);
}

/*
	CountBWColors()
*/
BOOL CImage::CountBWColors(unsigned int* pColors,unsigned char nNumColors)
{
	BOOL bCount = FALSE;
	unsigned int  nBitsPerPixel  = GetBPP();
	unsigned int  nHeight        = GetHeight();
	unsigned int  nWidth         = GetWidth();
	unsigned int  nColBytesReal  = (nWidth * nBitsPerPixel) / 8; 
	unsigned int  nBitsEnd       = (nWidth * nBitsPerPixel) % 8;
	unsigned int  nColBytes      = ((((nWidth * nBitsPerPixel) + 31) & ~31) >> 3);
	unsigned int  nRestBytesAlig = nColBytes - nColBytesReal - (nBitsEnd ? 1 : 0);
	
	if(LockData())
	{
		unsigned char* pData = (unsigned char*)GetPixels();
		
		if(pData)
		{
			memset(pColors,'\0',sizeof(unsigned int) * nNumColors);

			switch(nBitsPerPixel)
			{
				case 1:
				case 4:
				case 8:
				{
					unsigned char mask = 0xff;
					unsigned int nBitsByte = 8 / nBitsPerPixel;
					mask >>= (8 - nBitsPerPixel);

					for(unsigned int i = 0; i < nHeight ; i++)
					{
						for(unsigned int j = 0; j < nColBytesReal; j++)
						{
							unsigned char uByte = *pData++;
							for(unsigned int k = 0; k < nBitsByte; k++)
							{
								unsigned char uColor = (unsigned char)(uByte & mask);
								if(uColor < nNumColors)
									pColors[uColor]++;
								uByte >>= nBitsPerPixel;
							}
						}
						if(nBitsEnd)
						{
							unsigned char uByte = *pData++;
							for(unsigned int k = 0; k < nBitsEnd; k++)
							{
								unsigned char uColor = (unsigned char)(uByte & mask);
								if(uColor < nNumColors)
									pColors[uColor]++;
								uByte >>= nBitsPerPixel;

							}
						}
						pData += nRestBytesAlig;
					}
					
					bCount = TRUE;
				}
				break;

				case 16:
				case 24:
				case 32:
					break;

				default:
					break;
			}
		}

		UnlockData();
	}
	
	return(bCount);
}

/*
	CountRGBColors()
*/
BOOL CImage::CountRGBColors(COLORREF* pColors,unsigned int* pCountColors,unsigned char nNumColors)
{
	BOOL bCount = FALSE;
	unsigned int  nBitsPerPixel  = GetBPP();
	unsigned int  iBytesPerPixel = nBitsPerPixel / 8;
	unsigned int  nHeight        = GetHeight();
	unsigned int  nWidth         = GetWidth();
	unsigned int  nColBytesReal  = (nWidth* nBitsPerPixel) / 8; 
	unsigned int  nBitsEnd       = (nWidth* nBitsPerPixel) % 8;
	unsigned int  nColBytes      = ((((nWidth * nBitsPerPixel) + 31) & ~31) >> 3);
	unsigned int  nRestBytesAlig = nColBytes - nColBytesReal - (nBitsEnd ? 1 : 0);
	
	if(LockData())
	{
		unsigned char* pData = (unsigned char*)GetPixels();
		
		if(pData)
		{
			memset(pCountColors,'\0',sizeof(unsigned int) * nNumColors);

			switch(nBitsPerPixel)
			{
				case 1:
				case 4:
				case 8:
				case 16:
					break;

				case 24:
				case 32:
					{
						for(unsigned int i = 0; i < nHeight ; i++)
						{
							for(unsigned int j = 0; j < nWidth; j++)
							{
								for(int k = 0; k < nNumColors; k++)
								{
									if(GetRValue(pColors[k])==pData[RGB_RED] && GetGValue(pColors[k])==pData[RGB_GREEN] && GetBValue(pColors[k])==pData[RGB_BLUE])
									{
										pCountColors[k]++;
										break;
									}
								}
								
								pData += iBytesPerPixel;
							}

							pData += nRestBytesAlig;
						}
						
						bCount = TRUE;
					}
					break;

				default:
					break;
			}
		}

		UnlockData();
	}
	
	return(bCount);
}

/*
	ConvertToBPP()
*/
UINT CImage::ConvertToBPP(UINT nBitsPerPixel,UINT nFlags,RGBQUAD *pPalette/*=NULL*/,UINT nColors/*=0*/)
{
	UINT nRet = GDI_ERROR;	
	BOOL bLocked = FALSE;

	void* pVoid = NULL;
	HDC hDC = NULL;
	BITMAPINFO* pBitmapInfoSrc = NULL;
	BITMAPINFO* pBitmapInfo = NULL;
	HBITMAP hBitmap = NULL;
	HBITMAP hOldBitmap = NULL;
	HPALETTE hPal = NULL;
	HPALETTE hOldPal = NULL;
	UINT nColorData = 0;

	if(LockData())
	{
		bLocked = TRUE;

		if(nBitsPerPixel <= 8)
		{
			nColorData = 1 << nBitsPerPixel;
			if(!pPalette)
				goto done;
		}

		hDC = ::CreateCompatibleDC(NULL);
		if(!hDC)
			goto done;

		pBitmapInfoSrc = GetBMI();
		if(!pBitmapInfoSrc)
			goto done;

		pBitmapInfo = (BITMAPINFO*)new char[sizeof(BITMAPINFOHEADER)+(nColorData*sizeof(RGBQUAD))];
		if(!pBitmapInfo)
			goto done;

		memset(pBitmapInfo,'\0',sizeof(BITMAPINFOHEADER)+(nColorData*sizeof(RGBQUAD)));
		pBitmapInfo->bmiHeader.biBitCount    = (unsigned short)nBitsPerPixel;
		pBitmapInfo->bmiHeader.biHeight      = GetHeight();
		pBitmapInfo->bmiHeader.biWidth       = GetWidth();
		pBitmapInfo->bmiHeader.biPlanes      = 1;
		pBitmapInfo->bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
		pBitmapInfo->bmiHeader.biCompression = BI_RGB;
		
		if(pPalette && nColorData)
			memcpy(pBitmapInfo->bmiColors,pPalette,min(nColorData,nColors));

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
		
		int nCurrentMode = HALFTONE;
		if((nFlags & 0xffff0000)==QUANTIZE_NODITHERING)
		{
			switch(GetPhotometric())
			{
				case PHOTOMETRICMINISBLACK:
					nCurrentMode = WHITEONBLACK;
					break;
				case PHOTOMETRICMINISWHITE:
					nCurrentMode = BLACKONWHITE;
					break;
				default:
					nCurrentMode = COLORONCOLOR;
					break;
			}
		}
		
		int nMode = ::SetStretchBltMode(hDC,nCurrentMode);
		nRet = ::StretchDIBits(	hDC,
							0,
							0,
							pBitmapInfo->bmiHeader.biWidth,
							pBitmapInfo->bmiHeader.biHeight,
							0,
							0,
							pBitmapInfoSrc->bmiHeader.biWidth,
							pBitmapInfo->bmiHeader.biHeight,
							(LPSTR)GetPixels(),
							(BITMAPINFO*)pBitmapInfoSrc,
							DIB_RGB_COLORS,
							SRCCOPY
							);
		::SetStretchBltMode(hDC,nMode);
		
		::SelectObject(hDC,hOldBitmap);
		
		bLocked = !UnlockData();

		if(nRet!=GDI_ERROR)
			nRet = NO_ERROR;

		if(nRet==NO_ERROR)
		{
			nRet = GDI_ERROR;
			if(Create(pBitmapInfo,NULL) && LockData())
			{
				nRet = ::GetDIBits(hDC,hBitmap,0,GetHeight(),GetPixels(),GetBMI(),DIB_RGB_COLORS) ? NO_ERROR : GDI_ERROR;
				bLocked = !UnlockData(TRUE);
			}
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
	GetDIBNumColors()
*/
WORD CImage::GetDIBNumColors(LPSTR lpbi)
{
	WORD wNumColors = 0;
	WORD wBitCount;

	if(IS_WIN30_DIB(lpbi))
	{
		DWORD dwClrUsed = ((LPBITMAPINFOHEADER)lpbi)->biClrUsed;
		if(dwClrUsed!=0)
			return((WORD)dwClrUsed);
	}

	if(IS_WIN30_DIB(lpbi))
		wBitCount = ((LPBITMAPINFOHEADER)lpbi)->biBitCount;
	else
		wBitCount = ((LPBITMAPCOREHEADER)lpbi)->bcBitCount;

	switch(wBitCount)
	{
		case 1:
			wNumColors = 2;
			break;
		case 4:
			wNumColors = 16;
			break;
		case 8:
			wNumColors = 256;
			break;
	}

	return(wNumColors);
}

/*
	CreateDIBPalette()
*/
HPALETTE CImage::CreateDIBPalette(LPBITMAPINFO lpbmi)
{
	LPLOGPALETTE lpPal;
	HANDLE hLogPal;
	HPALETTE hPal = NULL;
	WORD wNumColors;
	LPBITMAPCOREINFO lpbmc;
	BOOL bWinStyleDIB;

	if(lpbmi)
	{
		lpbmc = (LPBITMAPCOREINFO)lpbmi;

		if((wNumColors = GetDIBNumColors((LPSTR)lpbmi))!=0)
		{
			if((hLogPal = ::GlobalAlloc(GHND,sizeof(LOGPALETTE) + sizeof(PALETTEENTRY) * wNumColors))!=(HGLOBAL)NULL)
			{
				lpPal = (LPLOGPALETTE)::GlobalLock((HGLOBAL)hLogPal);

				lpPal->palVersion = PALVERSION;
				lpPal->palNumEntries = (WORD)wNumColors;

				bWinStyleDIB = IS_WIN30_DIB((LPSTR)lpbmi);
				for(int i = 0; i < (int)wNumColors; i++)
				{
					if(bWinStyleDIB)
					{
						lpPal->palPalEntry[i].peRed = lpbmi->bmiColors[i].rgbRed;
						lpPal->palPalEntry[i].peGreen = lpbmi->bmiColors[i].rgbGreen;
						lpPal->palPalEntry[i].peBlue = lpbmi->bmiColors[i].rgbBlue;
						lpPal->palPalEntry[i].peFlags = 0;
					}
					else
					{
						lpPal->palPalEntry[i].peRed = lpbmc->bmciColors[i].rgbtRed;
						lpPal->palPalEntry[i].peGreen = lpbmc->bmciColors[i].rgbtGreen;
						lpPal->palPalEntry[i].peBlue = lpbmc->bmciColors[i].rgbtBlue;
						lpPal->palPalEntry[i].peFlags = 0;
					}
				}

				hPal = ::CreatePalette(lpPal);
				::GlobalUnlock((HGLOBAL)hLogPal);
				::GlobalFree((HGLOBAL)hLogPal);
			}
		}
	}

	return(hPal);
}

/*
	GetType()
*/
IMAGE_TYPE CImage::GetType(void)
{
	if(m_InfoHeader.type==NULL_PICTURE)
		GetHeaderInfo(m_szFileName,&m_InfoHeader);
	
	return(m_InfoHeader.type);
}

/*
	GetHeaderInfo()
*/
void CImage::GetHeaderInfo(LPCSTR lpcszFileName,LPIMAGEHEADERINFO pHeaderInfo)
{
	pHeaderInfo->type = NULL_PICTURE;
	pHeaderInfo->xres = 0;
	pHeaderInfo->yres = 0;
	pHeaderInfo->restype = RESUNITNONE;
	pHeaderInfo->compression = 0;
	pHeaderInfo->quality = 0;
	HANDLE hHandle = INVALID_HANDLE_VALUE;
	HANDLE hMap = INVALID_HANDLE_VALUE;
	LPVOID pMap = NULL;

	if((hHandle = ::CreateFile(lpcszFileName,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL))!=INVALID_HANDLE_VALUE)
	{
		DWORD dwSize = pHeaderInfo->filesize = ::GetFileSize(hHandle,NULL);
		DWORD dwRead = dwSize;

		if((hMap = ::CreateFileMapping(hHandle,NULL,PAGE_READONLY,0,0,NULL))!=INVALID_HANDLE_VALUE)
		{
			if((pMap = ::MapViewOfFile(hMap,FILE_MAP_READ,0,0,0))!=NULL)
			{
				BYTE* pData = (BYTE*)pMap;

				// formato

				// BMP
				if(pHeaderInfo->type==NULL_PICTURE)
				{
					BITMAPFILEHEADER* pBi = (BITMAPFILEHEADER*)pData;
					if(pBi->bfType==0x4d42) // 0x4d42=win, 0x424d=altri
						pHeaderInfo->type = BMP_PICTURE;
				}

				// JPEG/JFIF
				if(pHeaderInfo->type==NULL_PICTURE)
				{
					if((*pData==0xFF) && (*(pData+1)==0xD8) && (*(pData+2)==0xFF))
						pHeaderInfo->type = JPEG_PICTURE;
				}

				// GIF
				if(pHeaderInfo->type==NULL_PICTURE)
				{
					ULONG GIFSig = *((ULONG*)pData);
					if(GIFSig==0x38464947)
						pHeaderInfo->type = GIF_PICTURE;
				}

				// TIFF
				if(pHeaderInfo->type==NULL_PICTURE)
				{
					ULONG TIFFSig = *((ULONG *)pData);
					if(TIFFSig==0x002A4949 || TIFFSig==0x2A004D4D)
						pHeaderInfo->type = TIFF_PICTURE;
				}

				// PNG
				if(pHeaderInfo->type==NULL_PICTURE)
				{
					if((*pData==0x89) && (*(pData+1)==0x50) && (*(pData+2)==0x4E) && (*(pData+3)==0x47))
						pHeaderInfo->type = PNG_PICTURE;
				}

				// PGM
				if(pHeaderInfo->type==NULL_PICTURE)
				{
					if(pData[0]==0x50 && ((pData[1]==0x32)||(pData[1]==0x35)))
						pHeaderInfo->type = PGM_PICTURE;
				}

				// PCX
				if(pHeaderInfo->type==NULL_PICTURE)
				{
					if(pData[0]==0x0A && pData[2]==0x01)
						pHeaderInfo->type = PCX_PICTURE;
				}

				// PICT
				if(pHeaderInfo->type==NULL_PICTURE)
				{
					if(dwRead > 540)
					{
						BYTE* pPictSig = (BYTE*)(pData+0x20a);
						if((pPictSig[0]==0x00 && pPictSig[1]==0x11 && pPictSig[2]==0x02 && pPictSig[3]==0xFF) || (pPictSig[0]==0x00 && pPictSig[1]==0x11 && pPictSig[2]==0x01) || (pPictSig[0]==0x11 && pPictSig[1]==0x01 && pPictSig[2]==0x01 && pPictSig[3]==0x00))
							pHeaderInfo->type = PICT_PICTURE;
					}
				}

				// TGA (senza signature)
				if(pHeaderInfo->type==NULL_PICTURE)
				{
					BOOL bCouldBeTGA = TRUE;
					if(*(pData+1) > 1)
						bCouldBeTGA = FALSE;
					BYTE TGAImgType = *(pData+2);
					if((TGAImgType > 11) || (TGAImgType > 3 && TGAImgType < 9))
						bCouldBeTGA = FALSE;
					BYTE TGAColMapDepth = *(pData+7);
					if(TGAColMapDepth != 8 && TGAColMapDepth != 15 &&
						TGAColMapDepth != 16 && TGAColMapDepth != 24 && TGAColMapDepth != 32 && TGAColMapDepth != 0)
					bCouldBeTGA = FALSE;
					BYTE TGAPixDepth = *(pData+16);
					if(TGAPixDepth != 8 && TGAPixDepth != 15 && TGAPixDepth != 16 && TGAPixDepth != 24 && TGAPixDepth != 32)
						bCouldBeTGA = FALSE;
					if(bCouldBeTGA)
						pHeaderInfo->type = TGA_PICTURE;
				}
		
				// header
				
				if(pHeaderInfo->type==JPEG_PICTURE)
				{
					jpeg_decompress_struct cinfo;
					jpeg_error_mgr jerr;
					cinfo.err = jpeg_std_error (&jerr);
					jerr.error_exit = jpeg_error_exit;
					
					jpeg_create_decompress(&cinfo);
					jpeg_mem_src(&cinfo,pData,dwSize,(void*)pData);
					jpeg_read_header(&cinfo,0);
				
					//???
					int nQuality = 0;
					if(cinfo.quant_tbl_ptrs!=NULL)
					{
						nQuality = cinfo.quant_tbl_ptrs[0]->quantval[61];
					
						int nMin,nMax;
						GetQualityRange(nMin,nMax);
						if(nMax==255) // leadtools
							nQuality = 100;
						else //paintlib, nexgen
						{
							if(nQuality==255)
								nQuality = 1;
							else 
							{
								if(nQuality==1)
									nQuality = 100;
								else
								{
									if(nQuality > 100) 
										nQuality = 5000 / nQuality;
									else
										nQuality = (200 - nQuality) / 2;
								}
							}
						}

						pHeaderInfo->quality = nQuality;
					}
					//???

					jpeg_destroy_decompress(&cinfo);
					
					int nDPI;
					nDPI = cinfo.X_density;
					if(nDPI <= 1)
						nDPI = 72;
					pHeaderInfo->xres = (float)nDPI;
					nDPI = cinfo.Y_density;
					if(nDPI <= 1)
						nDPI = 72;
					pHeaderInfo->yres = (float)nDPI;
					switch(cinfo.density_unit)
					{
						// no meaningful units
						case 0:
							pHeaderInfo->restype = RESUNITNONE;
							break;
						// english
						case 1:
							pHeaderInfo->restype = RESUNITINCH;
							break;
						// metric
						case 2:
							pHeaderInfo->restype = RESUNITCENTIMETER;
							break;
	
						}
				}
				else if(pHeaderInfo->type==TIFF_PICTURE)
				{
					TIFFSetWarningHandler(NULL);
					TIFF* tif = TIFFOpenMem(pData,dwSize,NULL);
					if(tif)
					{
						float nRes;
						
						nRes = 0.0;
						TIFFGetField(tif,TIFFTAG_XRESOLUTION,&nRes);
						if(nRes <= 1.0)
							nRes = 300.0;
						pHeaderInfo->xres = nRes;

						nRes = 0.0;
						TIFFGetField(tif,TIFFTAG_YRESOLUTION,&nRes);
						if(nRes <= 1.0)
							nRes = 300.0;
						pHeaderInfo->yres = nRes;

						TIFFGetFieldDefaulted(tif,TIFFTAG_RESOLUTIONUNIT,&pHeaderInfo->restype);
						switch(pHeaderInfo->restype)
						{
							// no meaningful units
							case RESUNIT_NONE:
								pHeaderInfo->restype = RESUNITNONE;
								break;
							// english
							case RESUNIT_INCH:
								pHeaderInfo->restype = RESUNITINCH;
								break;
							// metric
							case RESUNIT_CENTIMETER:
								pHeaderInfo->restype = RESUNITCENTIMETER;
								break;
							default:
								pHeaderInfo->restype = RESUNITNONE;
								break;
						}

						unsigned short int nComp;
						if(TIFFGetField(tif,TIFFTAG_COMPRESSION,&nComp))
							pHeaderInfo->compression = nComp;

						TIFFClose(tif);
					}
				}
				else if(pHeaderInfo->type==BMP_PICTURE)
				{
					BITMAPINFO* bi = (BITMAPINFO*)(pData+sizeof(BITMAPFILEHEADER));

					float nDPI;
					
					nDPI = 0;
					if(bi->bmiHeader.biXPelsPerMeter > 0)
						nDPI = ((float)bi->bmiHeader.biXPelsPerMeter / 39.37f);
					if(nDPI <= 1)
						nDPI = 150;
					pHeaderInfo->xres = nDPI;

					nDPI = 0;
					if(bi->bmiHeader.biYPelsPerMeter > 0)
						nDPI = ((float)bi->bmiHeader.biYPelsPerMeter / 39.37f);
					if(nDPI <= 1)
						nDPI = 150;
					pHeaderInfo->yres = nDPI;
					pHeaderInfo->restype = RESUNITINCH;
				}
				
				::UnmapViewOfFile(pMap);
			}
			
			::CloseHandle(hMap);
		}
		
		::CloseHandle(hHandle);
	}
}

/*
	Load()
*/
BOOL CImage::Load(LPCSTR)
{
	m_InfoHeader.type = NULL_PICTURE;
	m_InfoHeader.xres = -1;
	m_InfoHeader.yres = -1;
	m_InfoHeader.restype = -1;
	m_InfoHeader.compression = -1;
	m_InfoHeader.quality = -1;
	m_InfoHeader.filesize = (unsigned long)-1L;
	m_InfoHeader.width = (unsigned long)-1L;
	m_InfoHeader.height = (unsigned long)-1L;
	m_ImageType = NULL_PICTURE;

	return(FALSE);
}

/*
	Copy()
*/
HANDLE CImage::Copy(RECT& rect)
{
	BOOL bRet = FALSE;
	BOOL bLocked = FALSE;
	
	HDC hDC = NULL;
	BITMAPINFO* pBitmapInfoSrc = NULL;
	BITMAPINFO* pBitmapInfo = NULL;
	HDIB hDIB = (HDIB)NULL;
	HBITMAP hBitmap = NULL;
	HBITMAP hOldBitmap = NULL;
	HPALETTE hOldPal = NULL;
	HPALETTE hPal = NULL;

	if(LockData())
	{
		bLocked = TRUE;
		
		hDC = ::CreateCompatibleDC(NULL);
		if(!hDC)
			goto done;

		pBitmapInfoSrc = GetBMI();
		if(!pBitmapInfoSrc)
			goto done;

		int nColorBytes = GetNumColors() * sizeof(RGBQUAD);
		int nTotalBytes = sizeof(BITMAPINFOHEADER) + nColorBytes + (GetBytesWidth(rect.right,GetBPP(),GetAlignment())*rect.bottom);
		
		hDIB = (HDIB)::GlobalAlloc(GHND,nTotalBytes);
		if(!hDIB)
			goto done;

		pBitmapInfo = (BITMAPINFO*)::GlobalLock(hDIB);
		if(!pBitmapInfo)
			goto done;

		memcpy(pBitmapInfo,pBitmapInfoSrc,sizeof(BITMAPINFOHEADER)+nColorBytes);
		pBitmapInfo->bmiHeader.biHeight    = rect.bottom;
		pBitmapInfo->bmiHeader.biWidth     = rect.right;
		pBitmapInfo->bmiHeader.biSizeImage = 0;

		void* pVoid = NULL;
		hBitmap = ::CreateDIBSection(hDC,(LPBITMAPINFO)pBitmapInfo,DIB_RGB_COLORS,&pVoid,0,0);
		if(!hBitmap)
			goto done;

		hOldBitmap = (HBITMAP)::SelectObject(hDC,hBitmap);

		hOldPal = NULL;
		hPal = CreateDIBPalette((BITMAPINFO*)pBitmapInfoSrc);
		if(hPal)
		{
			hOldPal = ::SelectPalette(hDC,hPal,FALSE);
			::RealizePalette(hDC);
		}
		
		bRet = ::StretchDIBits(	hDC,
							0,
							0,
							pBitmapInfo->bmiHeader.biWidth,
							pBitmapInfo->bmiHeader.biHeight,
							rect.left,
							GetHeight() - rect.top-rect.bottom,
							pBitmapInfo->bmiHeader.biWidth,
							pBitmapInfo->bmiHeader.biHeight,
							(LPSTR)GetPixels(),
							(BITMAPINFO*)pBitmapInfoSrc,
							DIB_RGB_COLORS,
							SRCCOPY)!=GDI_ERROR;
				
		::SelectObject(hDC,hOldBitmap);
		
		if(bRet)
		{
			pBitmapInfo->bmiHeader.biSizeImage = nTotalBytes - sizeof(BITMAPINFOHEADER) - nColorBytes;
			bRet = ::GetDIBits(hDC,hBitmap,0,pBitmapInfo->bmiHeader.biHeight,(LPSTR)pBitmapInfo+sizeof(BITMAPINFOHEADER)+nColorBytes,pBitmapInfo,DIB_RGB_COLORS)!=0;
		}

		if(hOldPal)
			::SelectPalette(hDC,hOldPal,TRUE);

		::GlobalUnlock(hDIB);
		
		if(!bRet)
		{
			::GlobalFree(hDIB);
			hDIB = NULL;
		}
	}

done:

	if(hDC)
		::DeleteDC(hDC);
	if(hBitmap)
		::DeleteObject(hBitmap);				
	if(hPal)
		::DeleteObject(hPal);

	if(bLocked)
		UnlockData();

	return(hDIB);
}

/*
	Brightness()
*/
UINT CImage::Brightness(CImageParams* pCImageParams)
{
	UINT nRet = GDI_ERROR;
	
	int nFactor = (int)pCImageParams->GetBrightness();
	if(nFactor >= 0)
	{	
		RECT r;
		r.top = r.left = 0;
		r.right = GetWidth();
		r.bottom = GetHeight();
  			
		for(int y = r.top; y < r.bottom; y++)
		{
			for(int x = r.left; x < r.right; x++)
			{
				COLORREF c = GetPixel(x,y);
				
				int r = min(GetRValue(c) * nFactor / 100,255);
				int g = min(GetGValue(c) * nFactor / 100,255);
				int b = min(GetBValue(c) * nFactor / 100,255);
				
				r = max(r,0);
				g = max(g,0);
				b = max(b,0);
//				c = RGB(r,g,b);
				c = RGB(b,g,r);
				
				SetPixel(x,y,c);
			}
		}

		nRet = NO_ERROR;
	}

	return(nRet);
}

/*
	Contrast()
*/
UINT CImage::Contrast(CImageParams* pCImageParams)
{
	UINT nRet = GDI_ERROR;

	int nFactor = (int)pCImageParams->GetContrast();
	if(nFactor >= 0)
	{
		RECT r;
		r.top = r.left = 0;
		r.right = GetWidth();
		r.bottom = GetHeight();
  			
		for(int y = r.top; y < r.bottom; y++)
		{
			for(int x = r.left; x < r.right; x++)
			{
				COLORREF c = GetPixel(x,y);
				
				int r = min(128 + ((GetRValue(c) - 128) * nFactor / 100),255);
				int g = min(128 + ((GetGValue(c) - 128) * nFactor / 100),255);
				int b = min(128 + ((GetBValue(c) - 128) * nFactor / 100),255);
				
				r = max(r,0);
				g = max(g,0);
				b = max(b,0);
//				c = RGB(r,g,b);
     			c = RGB(b,g,r);
				
				SetPixel(x,y,c);
			}
		}
	
		nRet = NO_ERROR;
	}

	return(nRet);
}

/*
	GammaCorrection()
*/
UINT CImage::GammaCorrection(CImageParams* pCImageParams)
{
	UINT nRet = GDI_ERROR;

	double Value = pCImageParams->GetGamma();
	double MaxRange = pow((double)255,Value) / (double)255;
	if(MaxRange >= 0)
	{
		RECT r;
		r.top = r.left = 0;
		r.right = GetWidth();
		r.bottom = GetHeight();
  		
		for(int y = r.top; y < r.bottom; y++)
		{
			for(int x = r.left; x < r.right; x++)
			{
				COLORREF c = GetPixel(x,y);
				double dblR = pow((double)GetRValue(c),Value) / (double)MaxRange;
				double dblG = pow((double)GetGValue(c),Value) / (double)MaxRange;
				double dblB = pow((double)GetBValue(c),Value) / (double)MaxRange;

				int r = min((int)(dblR + 0.5),255);
				int g = min((int)(dblG + 0.5),255);
				int b = min((int)(dblB + 0.5),255);
				
				r = max(r,0);
				g = max(g,0);
				b = max(b,0);
//				c = RGB(r,g,b);
				c = RGB(b,g,r);
				
				SetPixel(x,y,c);
			}
		}

		nRet = NO_ERROR;
	}

	return(nRet);
}

/*
	Halftone()
*/
UINT CImage::Halftone(CImageParams*)
{
	UINT nRet = GDI_ERROR;
	BOOL bLocked = FALSE;

	HDC hDC = NULL;
	BITMAPINFO* pBitmapInfoSrc = NULL;
	BITMAPINFO* pBitmapInfo = NULL;
	HBITMAP hBitmap = NULL;
	HBITMAP hOldBitmap = NULL;
	HPALETTE hPal = NULL;
	HPALETTE hOldPal = NULL;

	if(GetBPP()!=1)
	{
		if(LockData())
		{
			bLocked = TRUE;

			hDC = ::CreateCompatibleDC(NULL);
			if(!hDC)
				goto done;

			pBitmapInfoSrc = GetBMI();
			if(!pBitmapInfoSrc)
				goto done;
			
			pBitmapInfo = (BITMAPINFO*)new char[sizeof(BITMAPINFOHEADER)+(2*sizeof(RGBQUAD))];
			if(!pBitmapInfo)
				goto done;

			memset(pBitmapInfo,'\0',sizeof(BITMAPINFOHEADER)+(2*sizeof(RGBQUAD)));
			pBitmapInfo->bmiHeader.biBitCount    = 1;
			pBitmapInfo->bmiHeader.biHeight      = GetHeight();
			pBitmapInfo->bmiHeader.biWidth       = GetWidth();
			pBitmapInfo->bmiHeader.biPlanes      = 1;
			pBitmapInfo->bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
			pBitmapInfo->bmiHeader.biCompression = BI_RGB;
			pBitmapInfo->bmiColors[0].rgbBlue    = 0;
			pBitmapInfo->bmiColors[0].rgbRed     = 0;
			pBitmapInfo->bmiColors[0].rgbGreen   = 0;
			pBitmapInfo->bmiColors[1].rgbBlue    = 255;
			pBitmapInfo->bmiColors[1].rgbRed     = 255;
			pBitmapInfo->bmiColors[1].rgbGreen   = 255;
 			
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
			
			int nMode = ::SetStretchBltMode(hDC,HALFTONE);
			nRet = ::StretchDIBits(	hDC,
								0,
								0,
								pBitmapInfo->bmiHeader.biWidth,
								pBitmapInfo->bmiHeader.biHeight,
								0,
								0,
								pBitmapInfoSrc->bmiHeader.biWidth,
								pBitmapInfo->bmiHeader.biHeight,
								(LPSTR)GetPixels(),
								(BITMAPINFO*)pBitmapInfoSrc,
								DIB_RGB_COLORS,SRCCOPY)!=GDI_ERROR ? NO_ERROR : GDI_ERROR;
			::SetStretchBltMode(hDC,nMode);
			
			::SelectObject(hDC,hOldBitmap);

			if(nRet==GDI_ERROR)
				goto done;
			else
				nRet = NO_ERROR;
			
			bLocked = !UnlockData();

			if(Create(pBitmapInfo,0) && LockData())
			{
				nRet = ::GetDIBits(hDC,hBitmap,0,GetHeight(),GetPixels(),GetBMI(),DIB_RGB_COLORS) ? NO_ERROR : GDI_ERROR;
				bLocked = !UnlockData(TRUE);
			}
			
			if(hOldPal)
				::SelectPalette(hDC,hOldPal,TRUE);			
		}
	}
	else
		nRet = NO_ERROR;

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
	Hue()
*/
UINT CImage::Hue(CImageParams* pCImageParams)
{
	UINT nRet = GDI_ERROR;
	
	int nFactor = (int)pCImageParams->GetHue();
	if(nFactor >= 0)
	{
		RECT r;
		r.top = r.left = 0;
		r.right = GetWidth();
		r.bottom = GetHeight();
  			
		for(int y = r.top; y < r.bottom; y++)
		{
			for(int x = r.left; x < r.right; x++)
			{
				COLORREF c = GetPixel(x,y);
				double H,S,L;
				RGBtoHSL(c,&H,&S,&L);
				H = (double)(H * nFactor / 100.0);
				SetPixel(x,y,HLStoRGB(H,L,S));
			}
		}
		
		nRet = NO_ERROR;
	}

	return(nRet);
}

/*
	Mirror()
*/
UINT CImage::Mirror(UINT nDirection/*0=horiz, 1=vert*/)
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
		
		if(nDirection==0)
		{
			nRet = ::StretchDIBits(	hDC,
								0,
								0,
								nWidth,
								nHeight,
								0,
								nHeight-1,
								nWidth,
								(-1)*nHeight,
								(LPSTR)GetPixels(),
								(BITMAPINFO*)pBitmapInfoSrc,
								DIB_RGB_COLORS,
								SRCCOPY);
		}
		else if(nDirection==1)
		{
			nRet = ::StretchDIBits(	hDC,
								0,
								0,
								nWidth,
								nHeight,
								nWidth-1,
								0,
								(-1)*nWidth,
								nHeight,
								(LPSTR)GetPixels(),
								(BITMAPINFO*)pBitmapInfoSrc,
								DIB_RGB_COLORS,
								SRCCOPY);
		}
		else
			nRet = GDI_ERROR;

		::SelectObject(hDC,hOldBitmap);
		
		if(nRet==GDI_ERROR)
			goto done;
		else
			nRet = NO_ERROR;

		bLocked = !UnlockData();

		if(Create(pBitmapInfo,0) && LockData())
		{
			nRet = ::GetDIBits(hDC,hBitmap,0,nHeight,GetPixels(),GetBMI(),DIB_RGB_COLORS)!=0 ? NO_ERROR : GDI_ERROR;
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
	Text()
*/
UINT CImage::Text(LPCSTR lpcszText,CHOOSEFONT* cf,COLORREF foregroundColor,COLORREF backgroundColor,int nBackgroundMode,SIZE* size)
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
		
		DWORD rgbCurrent;
		HFONT hfont,hfontPrev;

		hfont = CreateFontIndirect(cf->lpLogFont);
		hfontPrev = (HFONT)SelectObject(hDC,hfont);
		rgbCurrent= cf->rgbColors;

		//rgbPrev = SetTextColor(hDC,rgbCurrent);
		SetTextColor(hDC,RGB(GetRValue(foregroundColor),GetGValue(foregroundColor),GetBValue(foregroundColor)));

		SetBkColor(hDC,RGB(GetRValue(backgroundColor),GetGValue(backgroundColor),GetBValue(backgroundColor)));
		SetBkMode(hDC,nBackgroundMode);

		TextOut(hDC,size->cx,size->cy,lpcszText,strlen(lpcszText));
		SIZE Size;
		GetTextExtentPoint32(hDC,lpcszText,strlen(lpcszText),&Size);
		size->cy += Size.cy;
	
		DeleteObject(hfont);
		
		::SelectObject(hDC,hOldBitmap);
		
		if(nRet==GDI_ERROR)
			goto done;
		else
			nRet = NO_ERROR;

		bLocked = !UnlockData();

		if(Create(pBitmapInfo) && LockData())
		{
			nRet = ::GetDIBits(hDC,hBitmap,0,nHeight,GetPixels(),GetBMI(),DIB_RGB_COLORS)!=0 ? NO_ERROR : GDI_ERROR;
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
	Negate()
*/
UINT	CImage::Negate(CImageParams*)
{
	UINT nRet = GDI_ERROR;
	UINT nBitsPerPixel = GetBPP();
	
	switch(nBitsPerPixel)
	{
		case 1:
			if(LockData())
			{
				unsigned char* pData = (unsigned char*)GetPixels();
				if(pData)
				{
					UINT nHeight = GetHeight();
					UINT nBytesWidth = GetBytesWidth();
					UINT nTot = nHeight * nBytesWidth;				
					
					for(register int i = nTot; i > 0; i--,pData++)
						(*pData) = ~(*pData);
					
					nRet = NO_ERROR;
				}
				
				UnlockData(TRUE);
			}
			break;
		
		default:
			break;
	}

	return(nRet);	
}

/*
	SaturationHorizontal()
*/
UINT CImage::SaturationHorizontal(CImageParams* pCImageParams)
{
	UINT nRet = GDI_ERROR;
	
	int nFactor = (int)pCImageParams->GetSaturation();
	if(nFactor < 0)
		return(nRet);
	
	return(Saturation(nFactor));
}

/*
	SaturationVertical()
*/
UINT CImage::SaturationVertical(CImageParams* pCImageParams)
{
	UINT nRet = GDI_ERROR;
	
	int nFactor = (int)pCImageParams->GetSaturation();
	if(nFactor < 0)
		return(nRet);
	
	return(Saturation(nFactor));
}

/*
	Saturation()
*/
UINT CImage::Saturation(int nFactor)
{
	UINT nRet = NO_ERROR;

	RECT rc;
	rc.top = rc.left = 0;
	rc.right  = GetWidth();
	rc.bottom = GetHeight();

	for(int y = rc.top; y < rc.bottom; y++)
	{
		for(int x = rc.left; x < rc.right; x++)
		{
			COLORREF c = GetPixel(x,y);
			double H,S,L;
			RGBtoHSL(c,&H,&S,&L);
			S = (double)(S*(nFactor)/100);
			SetPixel(x,y,HLStoRGB(H,L,S));  
		}
	}

	return(nRet);
}

/*
	RGBtoHSL()
*/
void CImage::RGBtoHSL(COLORREF rgb,double* H,double* S,double* L)
{
	double delta;
	double r = (double)GetRValue(rgb) / 255;
	double g = (double)GetGValue(rgb) / 255;
	double b = (double)GetBValue(rgb) / 255;
	double cmax = max(r,max(g,b));
	double cmin = min(r,min(g,b));
	*L = (cmax + cmin) / 2.0f;
	
	if(cmax==cmin) 
	{
		*S = 0;
		*H = 0; // it's really undefined
	} 
	else 
	{
		if(*L < 0.5f) 
			*S = (cmax - cmin) / (cmax + cmin);
		else
			*S = (cmax - cmin) / (2.0f - cmax - cmin);
		
		delta = cmax - cmin;
		
		if(r==cmax)
			*H = (g - b) / delta;
		else if(g==cmax)
			*H = 2.0f + (b - r) / delta;
		else
			*H = 4.0f + (r - g) / delta;
		
		*H /= 6.0f;
		
		if(*H < 0.0f)
			*H += 1;
	}
}

/*
	HLStoRGB()
*/
COLORREF CImage::HLStoRGB(const double& H,const double& L,const double& S)
{
	double r,g,b;
	double m1,m2;

	if(S==0)
	{
		r = g = b = L;
	} 
	else 
	{
		if(L <= 0.5f)
			m2 = L * (1.0f + S);
		else
			m2 = L + S - L * S;
		
		m1 = 2.0f * L - m2;
		
		r = HuetoRGB(m1,m2,H + 1.0f / 3.0f);
		g = HuetoRGB(m1,m2,H);
		b = HuetoRGB(m1,m2,H - 1.0f / 3.0f);
	}

//	return(RGB((BYTE)(r*255),(BYTE)(g*255),(BYTE)(b*255)));
	return(RGB((BYTE)(b*255),(BYTE)(g*255),(BYTE)(r*255)));
}

/*
	HuetoRGB()
*/
double CImage::HuetoRGB(double m1,double m2,double h)
{
	if(h < 0)
		h += 1.0;
	if(h > 1)
		h -= 1.0;
	if((6.0f * h) < 1)
		return(m1 + (m2 - m1) * h * 6.0f);
	if((2.0f * h) < 1)
		return(m2);
	if((3.0f * h) < 2.0f)
		return(m1 + (m2 - m1) * ((2.0f / 3.0f) - h) * 6.0f);

	return(m1);
}

#define WIDTH_BYTES(bits)    (((bits) + 31) / 32 * 4)

#ifndef _HDIB_DECLARED
   DECLARE_HANDLE(HDIB);
   #define _HDIB_DECLARED 1
#endif

DECLARE_HANDLE(HMEMBMPFILE);

/* DIB constants */
#define PALVERSION      0x300
#define MAXPALCOLORS    256

/* DIB Macros*/
#define RECTWIDTH(lpRect)     ((lpRect)->right - (lpRect)->left)
#define RECTHEIGHT(lpRect)    ((lpRect)->bottom - (lpRect)->top)

/*
	WindowToDIB()
	From dibapi.cpp, part of the Microsoft Foundation Classes C++ library.
*/
HDIB CImage::WindowToDIB(CWnd *pWnd, CRect* pScreenRect)
{
   CBitmap 	   bitmap;
   CWindowDC	dc(pWnd);
   CDC 		   memDC;
   CRect		   rect;
   
   memDC.CreateCompatibleDC(&dc); 
   
   if ( pScreenRect == NULL )
      pWnd->GetWindowRect(rect);
   else
      rect = *pScreenRect;
   
   bitmap.CreateCompatibleBitmap(&dc, rect.Width(),rect.Height() );
   
   CBitmap* pOldBitmap = memDC.SelectObject(&bitmap);
   memDC.BitBlt(0, 0, rect.Width(),rect.Height(), &dc, rect.left, rect.top, SRCCOPY); 
   
   // Create logical palette if device support a palette
   CPalette pal;
   if ( dc.GetDeviceCaps(RASTERCAPS) & RC_PALETTE )
   {
      UINT nSize = sizeof(LOGPALETTE) + (sizeof(PALETTEENTRY) * MAXPALCOLORS);
      LOGPALETTE *pLP = (LOGPALETTE *) new BYTE[nSize];
      pLP->palVersion = PALVERSION;
      
      pLP->palNumEntries = (USHORT) GetSystemPaletteEntries( dc, 0, MAXPALCOLORS-1, pLP->palPalEntry );
      
      // Create the palette
      pal.CreatePalette( pLP );
      
      delete[] pLP;
   }
   
   memDC.SelectObject(pOldBitmap);
   
   HDIB hDib = BitmapToDIB( bitmap, pal );
   
   return hDib;
}
/*************************************************************************
*
* BitmapToDIB()
*
* Parameter:
*
* HBITMAP - handle to a Bitmap that the DIB is to be 
*            created from
* HPALETTE - palette to use in creation of DIB
*
* Return Value:
*
* HDIB - handle of the DIB created from the Bitmap, NULL
*        on error
*
* Description:
*
* Returns an HDIB created from an HBITMAP.
* This function creates a DIB from a bitmap using the 
* specified palette.
************************************************************************/

HDIB CImage::BitmapToDIB(HBITMAP hBitmap, HPALETTE hPal)
{
   ASSERT(hBitmap);
   
   BITMAP bm;               // bitmap structure
   BITMAPINFOHEADER bi;     // bitmap header
   LPBITMAPINFOHEADER lpbi; // pointer to BITMAPINFOHEADER
   DWORD dwLen;             // size of memory block
   HDIB hDib, h;            // handle to DIB, temp handle
   HDC hDC;                 // handle to DC
   WORD biBits;             // bits per pixel
   UINT wLineLen;
   DWORD dwSize;
   DWORD wColSize;
   
   // check if bitmap handle is valid
   if (!hBitmap)
   {
      return NULL;
   }
   
   // fill in BITMAP structure, return NULL if it didn't work
   if (!::GetObject(hBitmap, sizeof(bm), &bm))
   {
      return NULL;
   }
   
   // if no palette is specified, use default palette
   if (hPal == NULL)
      hPal = (HPALETTE)::GetStockObject(DEFAULT_PALETTE);
   
   // calculate bits per pixel
   biBits = (WORD) (bm.bmPlanes * bm.bmBitsPixel);
   
   wLineLen = ( bm.bmWidth * biBits + 31 ) / 32 * 4;
   wColSize = sizeof(RGBQUAD) * (( biBits <= 8 ) ? 
      1 << biBits : 0 );
   dwSize = sizeof( BITMAPINFOHEADER ) + wColSize +
      (DWORD)(UINT)wLineLen * (DWORD)(UINT)bm.bmHeight;
   
   // make sure bits per pixel is valid
   if (biBits <= 1)
      biBits = 1;
   else if (biBits <= 4)
      biBits = 4;
   else if (biBits <= 8)
      biBits = 8;
   else // if greater than 8-bit, force to 24-bit
      biBits = 24;
   
   // initialize BITMAPINFOHEADER
   bi.biSize = sizeof(BITMAPINFOHEADER);
   bi.biWidth = bm.bmWidth;
   bi.biHeight = bm.bmHeight;
   bi.biPlanes = 1;
   bi.biBitCount = biBits;
   bi.biCompression = BI_RGB;
   bi.biSizeImage = dwSize - sizeof(BITMAPINFOHEADER) - wColSize;
   bi.biXPelsPerMeter = 0;
   bi.biYPelsPerMeter = 0;
   bi.biClrUsed = ( biBits <= 8 ) ? 1 << biBits : 0;	
   bi.biClrImportant = 0;
   
   // calculate size of memory block required to store BITMAPINFO
   dwLen = bi.biSize + PaletteSize((LPSTR) &bi);
   
   // get a DC
   hDC = ::GetDC(NULL);
   
   // select and realize our palette
   hPal = ::SelectPalette(hDC, hPal, FALSE);
   ::RealizePalette(hDC);
   
   // alloc memory block to store our bitmap
   hDib = (HDIB)::GlobalAlloc(GHND, dwLen);
   
   // if we couldn't get memory block
   if (!hDib)
   {
      // clean up and return NULL
      ::SelectPalette(hDC, hPal, TRUE);
      ::RealizePalette(hDC);
      ::ReleaseDC(NULL, hDC);
      
      return NULL;
   }
   
   // lock memory and get pointer to it
   lpbi = (LPBITMAPINFOHEADER)::GlobalLock((HGLOBAL)hDib);
   if (!lpbi)
   {
      // clean up and return NULL
      ::SelectPalette(hDC, hPal, TRUE);
      ::RealizePalette(hDC);
      ::ReleaseDC(NULL, hDC);
      
      return NULL;
   }
   
   // use our bitmap info. to fill BITMAPINFOHEADER
   *lpbi = bi;
   
   // call GetDIBits with a NULL lpBits param, so it will 
   // calculate the biSizeImage field for us
   ::GetDIBits(hDC, hBitmap, 0, (WORD)bi.biHeight, NULL, 
      (LPBITMAPINFO)lpbi, DIB_RGB_COLORS);
   
   // get the info. returned by GetDIBits and unlock 
   // memory block
   bi = *lpbi;
   bi.biClrUsed = ( biBits <= 8 ) ? 1 << biBits : 0;
   ::GlobalUnlock(hDib);
   
   // if the driver did not fill in the biSizeImage field, 
   // make one up
   if (bi.biSizeImage == 0)
      bi.biSizeImage = WIDTH_BYTES((DWORD)bm.bmWidth * biBits) * bm.bmHeight;
   
   
   // realloc the buffer big enough to hold all the bits
   dwLen = bi.biSize + PaletteSize((LPSTR) &bi) + 
      bi.biSizeImage;
   h = (HDIB)::GlobalReAlloc(hDib, dwLen, 0);
   if ( h )
   {
      hDib = h;
   }
   else
   {
      // clean up and return NULL
      ::GlobalFree(hDib);
      hDib = NULL;
      
      ::SelectPalette(hDC, hPal, TRUE);
      ::RealizePalette(hDC);
      ::ReleaseDC(NULL, hDC);
      
      return NULL;
   }
   
   // lock memory block and get pointer to it
   lpbi = (LPBITMAPINFOHEADER)::GlobalLock((HGLOBAL)hDib);
   if (!lpbi)
   {
      // clean up and return NULL
      ::GlobalFree(hDib);
      hDib = NULL;
      
      ::SelectPalette(hDC, hPal, TRUE);
      ::RealizePalette(hDC);
      ::ReleaseDC(NULL, hDC);
      
      return NULL;
   }
   
   // call GetDIBits with a NON-NULL lpBits param, and 
   // actualy get the bits this time
   if (::GetDIBits(hDC, hBitmap, 0, (WORD)bi.biHeight, 
      (LPSTR)lpbi + (WORD)lpbi->biSize + 
      PaletteSize((LPSTR) lpbi), (LPBITMAPINFO)lpbi, 
      DIB_RGB_COLORS) == 0)
   {
      // clean up and return NULL
      ::GlobalUnlock(hDib);
      hDib = NULL;
      
      ::SelectPalette(hDC, hPal, TRUE);
      ::RealizePalette(hDC);
      ::ReleaseDC(NULL, hDC);
      
      return NULL;
   }
   
   bi = *lpbi;
   
   // clean up
   ::GlobalUnlock(hDib);
   ::SelectPalette(hDC, hPal, TRUE);
   ::RealizePalette(hDC);
   ::ReleaseDC(NULL, hDC);
   
   // return handle to the DIB
   return hDib;
}

/*************************************************************************
*
* DIBToBitmap()
*
* Parameters:
* HDIB - handle to a DIB that the Bitmap (DDB) is to be 
*        created from
*
* HPALETTE - palette to use in creation of bitmap (DDB)
*
* Return Value:
*
* HBITMAP - handle of the Bitmap created from the DIB,
*           NULL on error
* Description:
*
* Returns an HBITMAP created from an HDIB.
* This function creates a bitmap from a DIB using the 
* specified palette. If no palette is specified, one is 
* created, used for the conversion, and then deleted.
*
* The bitmap returned from this funciton is always a 
* bitmap compatible with the screen (e.g. same 
* bits/pixel and color planes) rather than a bitmap 
* with the same attributes as the DIB.
* This behavior is by design, and occurs because this 
* function calls CreateDIBitmap to do its work, and 
* CreateDIBitmap always creates a bitmap compatible with 
* the hDC parameter passed in (because it in turn calls 
* CreateCompatibleBitmap).
*
* So for instance, if your DIB is a monochrome DIB and 
* you call this function, you will not get back a 
* monochrome HBITMAP -- you will get an HBITMAP 
* compatible with the screen DC, but with only 2 colors 
* used in the bitmap.
*
* if your application requires a monochrome HBITMAP 
* returned for a monochrome DIB, use the function SetDIBits().
************************************************************************/

HBITMAP CImage::DIBToBitmap(HDIB hDib, HPALETTE hPal)
{
   ASSERT(hDib);
   
   LPVOID lpDIBHdr = NULL;  // pointer to DIB header
   LPVOID lpDIBBits = NULL; // pointer to DIB bits
   HBITMAP hBitmap = NULL;  // handle to DDB
   HDC hDC = NULL;          // handle to DC
   HPALETTE hOldPal = NULL; // handle to a palette
   BOOL bPalCreated = FALSE;
   
   // if invalid handle, return NULL
   if (!hDib)
   {
      return NULL;
   }
   
   // lock memory block and get a pointer to it
   lpDIBHdr = ::GlobalLock((HGLOBAL)hDib);
   if (!lpDIBHdr)
   {
      return NULL;
   }
   
   // get a pointer to the DIB bits
   lpDIBBits = FindDIBBits((LPSTR) lpDIBHdr);
   if (!lpDIBBits)
      return NULL;
   
   // get a DC
   hDC = ::GetDC(NULL);
   if (!hDC)
   {
      // clean up and return NULL
      ::GlobalUnlock(hDib);
      
      return NULL;
   }
   
   // select and realize palette
   if (!hPal)
   {
      CPalette    DibPal; 
      CreateDIBPalette(hDib,&DibPal);
      hPal = (HPALETTE) DibPal.Detach();
      bPalCreated = TRUE;
   }
   hOldPal = ::SelectPalette(hDC, hPal, FALSE);
   ::RealizePalette(hDC);
   
   // create bitmap from DIB info. and bits
   hBitmap = ::CreateDIBitmap(hDC, (LPBITMAPINFOHEADER)lpDIBHdr, 
      CBM_INIT, lpDIBBits, (LPBITMAPINFO)lpDIBHdr, 
      DIB_RGB_COLORS);
   if (!hBitmap)
   {
      if (hOldPal)
         ::SelectPalette(hDC, hOldPal, FALSE);
      ::DeleteObject(hPal);
      return NULL;
   }
   
   // restore previous palette
   if (hOldPal)
      ::SelectPalette(hDC, hOldPal, FALSE);
   
   // if we created the palette then we clean it up
   if (bPalCreated && hPal)
      ::DeleteObject(hPal);
   
   // clean up
   ::ReleaseDC(NULL, hDC);
   ::GlobalUnlock(hDib);
   
   // return handle to the bitmap
   return hBitmap;
}

/*************************************************************************
*
* PaletteSize()
*
* Parameter:
*
* LPSTR lpbi       - pointer to packed-DIB memory block
*
* Return Value:
*
* WORD             - size of the color palette of the DIB
*
* Description:
*
* This function gets the size required to store the DIB's palette by
* multiplying the number of colors by the size of an RGBQUAD (for a
* Windows 3.0-style DIB) or by the size of an RGBTRIPLE (for an other-
* style DIB).
*
************************************************************************/


WORD CImage::PaletteSize(LPSTR lpbi)
{
   /* calculate the size required by the palette */
   if (IS_WIN30_DIB (lpbi))
      return (WORD)(DIBNumColors(lpbi) * sizeof(RGBQUAD));
   else
      return (WORD)(DIBNumColors(lpbi) * sizeof(RGBTRIPLE));
}
/*************************************************************************
*
* FindDIBBits()
*
* Parameter:
*
* LPSTR lpbi       - pointer to packed-DIB memory block
*
* Return Value:
*
* LPSTR            - pointer to the DIB bits
*
* Description:
*
* This function calculates the address of the DIB's bits and returns a
* pointer to the DIB bits.
*
************************************************************************/


LPSTR CImage::FindDIBBits(LPSTR lpbi)
{
   return (lpbi + *(LPDWORD)lpbi + PaletteSize(lpbi));
}

/*************************************************************************
*
* CreateDIBPalette()
*
* Parameter:
*
* HDIB hDIB        - specifies the DIB
*
* Return Value:
*
* HPALETTE         - specifies the palette
*
* Description:
*
* This function creates a palette from a DIB by allocating memory for the
* logical palette, reading and storing the colors from the DIB's color table
* into the logical palette, creating a palette from this logical palette,
* and then returning the palette's handle. This allows the DIB to be
* displayed using the best possible colors (important for DIBs with 256 or
* more colors).
*
************************************************************************/
                     
                    
BOOL CImage::CreateDIBPalette(HDIB hDIB, CPalette* pPal)
{
   LPLOGPALETTE lpPal;      // pointer to a logical palette
   HANDLE hLogPal;          // handle to a logical palette
   int i;                   // loop index
   WORD wNumColors;         // number of colors in color table
   LPSTR lpbi;              // pointer to packed-DIB
   LPBITMAPINFO lpbmi;      // pointer to BITMAPINFO structure (Win3.0)
   LPBITMAPCOREINFO lpbmc;  // pointer to BITMAPCOREINFO structure (old)
   BOOL bWinStyleDIB;       // flag which signifies whether this is a Win3.0 DIB
   BOOL bResult = FALSE;
   
   /* if handle to DIB is invalid, return FALSE */
   
   if (hDIB == NULL)
      return FALSE;
   
   lpbi = (LPSTR) ::GlobalLock((HGLOBAL) hDIB);
   
   /* get pointer to BITMAPINFO (Win 3.0) */
   lpbmi = (LPBITMAPINFO)lpbi;
   
   /* get pointer to BITMAPCOREINFO (old 1.x) */
   lpbmc = (LPBITMAPCOREINFO)lpbi;
   
   /* get the number of colors in the DIB */
   wNumColors = DIBNumColors(lpbi);
   
   if (wNumColors != 0)
   {
      /* allocate memory block for logical palette */
      hLogPal = ::GlobalAlloc(GHND, sizeof(LOGPALETTE)
         + sizeof(PALETTEENTRY)
         * wNumColors);
      
      /* if not enough memory, clean up and return NULL */
      if (hLogPal == 0)
      {
         ::GlobalUnlock((HGLOBAL) hDIB);
         return FALSE;
      }
      
      lpPal = (LPLOGPALETTE) ::GlobalLock((HGLOBAL) hLogPal);
      
      /* set version and number of palette entries */
      lpPal->palVersion = PALVERSION;
      lpPal->palNumEntries = (WORD)wNumColors;
      
      /* is this a Win 3.0 DIB? */
      bWinStyleDIB = IS_WIN30_DIB(lpbi);
      for (i = 0; i < (int)wNumColors; i++)
      {
         if (bWinStyleDIB)
         {
            lpPal->palPalEntry[i].peRed = lpbmi->bmiColors[i].rgbRed;
            lpPal->palPalEntry[i].peGreen = lpbmi->bmiColors[i].rgbGreen;
            lpPal->palPalEntry[i].peBlue = lpbmi->bmiColors[i].rgbBlue;
            lpPal->palPalEntry[i].peFlags = 0;
         }
         else
         {
            lpPal->palPalEntry[i].peRed = lpbmc->bmciColors[i].rgbtRed;
            lpPal->palPalEntry[i].peGreen = lpbmc->bmciColors[i].rgbtGreen;
            lpPal->palPalEntry[i].peBlue = lpbmc->bmciColors[i].rgbtBlue;
            lpPal->palPalEntry[i].peFlags = 0;
         }
      }
      
      /* create the palette and get handle to it */
      bResult = pPal->CreatePalette(lpPal);
      ::GlobalUnlock((HGLOBAL) hLogPal);
      ::GlobalFree((HGLOBAL) hLogPal);
   }
   else
   {
      CWindowDC dcScreen(NULL);
      
      if ( dcScreen.GetDeviceCaps(RASTERCAPS) & RC_PALETTE )
      {
         /* create the palette and get handle to it */
         bResult = pPal->CreateHalftonePalette(&dcScreen);
      }
      else
      {
         pPal->DeleteObject();
         bResult = TRUE;
      }
   }
   
   ::GlobalUnlock((HGLOBAL) hDIB);
   
   return bResult;
}
/*************************************************************************
*
* DIBNumColors()
*
* Parameter:
*
* LPSTR lpbi       - pointer to packed-DIB memory block
*
* Return Value:
*
* WORD             - number of colors in the color table
*
* Description:
*
* This function calculates the number of colors in the DIB's color table
* by finding the bits per pixel for the DIB (whether Win3.0 or other-style
* DIB). If bits per pixel is 1: colors=2, if 4: colors=16, if 8: colors=256,
* if 24, no colors in color table.
*
************************************************************************/


WORD CImage::DIBNumColors(LPSTR lpbi)
{
   WORD wBitCount;  // DIB bit count
   
                    /*  If this is a Windows-style DIB, the number of colors in the
                    *  color table can be less than the number of bits per pixel
                    *  allows for (i.e. lpbi->biClrUsed can be set to some value).
                    *  If this is the case, return the appropriate value.
   */
   
   if (IS_WIN30_DIB(lpbi))
   {
      DWORD dwClrUsed;
      
      dwClrUsed = ((LPBITMAPINFOHEADER)lpbi)->biClrUsed;
      if (dwClrUsed != 0)
         return (WORD)dwClrUsed;
   }
   
   /*  Calculate the number of colors in the color table based on
   *  the number of bits per pixel for the DIB.
   */
   if (IS_WIN30_DIB(lpbi))
      wBitCount = ((LPBITMAPINFOHEADER)lpbi)->biBitCount;
   else
      wBitCount = ((LPBITMAPCOREHEADER)lpbi)->bcBitCount;
   
   /* return number of colors based on bits per pixel */
   switch (wBitCount)
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
