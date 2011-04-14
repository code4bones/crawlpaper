/*
	CImageObject.h
	Classe base per la definizione dell'oggetto immagine.
	Luca Piergentili, 01/09/00
	lpiergentili@yahoo.com

	Ad memoriam - Nemo me impune lacessit.
*/
#ifndef _CIMAGEOBJECT_H
#define _CIMAGEOBJECT_H 1

#include <stdio.h>
#include <stdlib.h>
#include "strings.h"
#include "window.h"
#include "CNodeList.h"
#include "CImageParams.h"

//#define NO_ERROR					0			// error.h, winerror.h
//#define GDI_ERROR					0xFFFFFFFFL	// wingdi.h
#define ERROR_NOT_32BPP_FORMAT		1

#define WINDOWSALIGNMENT				4
#define MAXPALETTECOLORS				256
#define PALVERSION					0x300
#define IS_WIN30_DIB(lpbi)			((*(LPDWORD)(lpbi))==sizeof(BITMAPINFOHEADER))

// CeXDib.h
#ifdef WIDTHBYTES
#undef WIDTHBYTES
#endif

#define WIDTHBYTES(bits,alig)			(((bits)+((alig*8)-1))/(alig*8)*alig)

enum ORDERRGB{
	RGB_RED = 0,
	RGB_GREEN = 1,
	RGB_BLUE = 2
};
	
enum PHOTOMETRIC {
	NOPHOTOMETRIC,
	PHOTOMETRICMINISBLACK,
	PHOTOMETRICMINISWHITE,
	PHOTOMETRICPALETTE,
	PHOTOMETRICRGB
};

enum RESOLUTION {
	RESUNITNONE,
	RESUNITINCH,
	RESUNITCENTIMETER
};

#ifndef _HDIB_DECLARED
	DECLARE_HANDLE(HDIB);
	#define _HDIB_DECLARED 1
#endif

struct DIBINFO {
	int nPerspective;
};

// tipo di operazione supportata dal formato
#define IMAGE_READ_FLAG	0x00000001
#define IMAGE_WRITE_FLAG	0x00000002

/*
	IMAGE_TYPE
	formati supportati
*/
enum IMAGE_TYPE {
	AVI_PICTURE	= 0,
	BMP_PICTURE	= 1,
	CUR_PICTURE	= 2,
	DCX_PICTURE	= 3,
	GIF_PICTURE	= 4,
	ICO_PICTURE	= 5,
	JPEG_PICTURE	= 6,
	MPEG_PICTURE	= 7,
	PCD_PICTURE	= 8,
	PCT_PICTURE	= 9,
	PCX_PICTURE	= 10,
	PGM_PICTURE	= 11,
	PICT_PICTURE	= 12,
	PNG_PICTURE	= 13,
	PPM_PICTURE	= 14,
	PSD_PICTURE	= 15,
	TGA_PICTURE	= 16,
	TIFF_PICTURE	= 17,
	WEMF_PICTURE	= 18,
	NULL_PICTURE	= 19
};

struct IMAGE_TYPE_EXT {
	char* ext;
};

/*
	IMAGE_HEADERINFO
	struttura per l'header dell'immagine
*/
struct IMAGE_HEADERINFO {
	IMAGE_TYPE	type;		// formato
	float		xres;		// risoluzione
	float		yres;		// risoluzione
	int			restype;		// tipo risoluzione
	int			compression;	// tiff
	int			quality;		// jpeg 0-100
	unsigned long	filesize;		// dimensione del file su disco
	
	unsigned long	width;
	unsigned long	height;
	int			bpp;
	int			colors;
	unsigned long	memused;
};
typedef IMAGE_HEADERINFO IMAGEHEADERINFO,*LPIMAGEHEADERINFO;
typedef const IMAGEHEADERINFO *LPCIMAGEHEADERINFO;

/*
	IMAGE_FILETYPE
	elemento per la lista dei formati supportati (tipo, estensione, descrizione)
*/
struct IMAGE_FILETYPE {
public:
	IMAGE_FILETYPE()
	{
		Reset();
	}
	IMAGE_FILETYPE(IMAGE_TYPE type,char* ext,char* desc,DWORD flags = 0L)
	{
		this->type = type;
		strcpyn(this->ext,ext,_MAX_EXT+1);
		strcpyn(this->desc,desc,_MAX_PATH+1);
		this->flags = flags;
	}
	
	virtual ~IMAGE_FILETYPE() {}

	void Reset(void)
	{
		type = NULL_PICTURE;
		memset(ext,'\0',_MAX_EXT+1);
		memset(desc,'\0',_MAX_PATH+1);
		flags = 0L;
	}

	IMAGE_TYPE	type;
	char			ext[_MAX_EXT+1];
	char			desc[_MAX_PATH+1];
	DWORD		flags;
};
typedef IMAGE_FILETYPE IMAGETYPE,*LPIMAGETYPE;

/*
	CImageTypeList
	classe per la lista dei formati supportati
*/
class CImageTypeList : public CNodeList
{
public:
	CImageTypeList() : CNodeList() {}
	virtual ~CImageTypeList() {CNodeList::DeleteAll();}
	
	void* Create(void)
	{
		IMAGETYPE* pData = new IMAGETYPE;
		return(pData);
	}
	
	void* Initialize(void* pVoid)
	{
		IMAGETYPE* pData = (IMAGETYPE*)pVoid;
		if(!pData)
			pData = (IMAGETYPE*)Create();
		if(pData)
			pData->Reset();
		return(pData);
	}

	BOOL PreDelete(ITERATOR iter)
	{
		if((IMAGETYPE*)iter->data)
		{
			delete ((IMAGETYPE*)iter->data);
			iter->data = (IMAGETYPE*)NULL;
		}
		return(TRUE);
	}

	int Size(void) {return(-1);}

#ifdef _DEBUG
	const char* Signature(void) {return("CImageTypeList");}
#endif
};

#define ADDFILETYPE(type,ext,desc,flags,ptr) {ptr = new IMAGETYPE(type,ext,desc,flags); if(ptr) m_ImageTypeList.Add(ptr);}

/*
	IMAGE_OPERATION
	elemento per la lista delle operazioni (filtri) supportate
*/
struct IMAGE_OPERATION {
public:
	IMAGE_OPERATION()
	{
		Reset();
	}
	IMAGE_OPERATION(LPCSTR name,BOOL flag)
	{
		strcpyn(this->name,name,_MAX_PATH+1);
		this->flag = flag;
	}
	
	~IMAGE_OPERATION() {}

	void Reset(void)
	{
		memset(name,'\0',_MAX_PATH+1);
		flag = FALSE;
	}

	char name[_MAX_PATH+1];
	BOOL flag;
};
typedef IMAGE_OPERATION IMAGEOPERATION,*LPIMAGEOPERATION;

/*
	CImageOperationList
	classe per la lista delle operazioni (filtri) supportate
*/
class CImageOperationList : public CNodeList
{
public:
	CImageOperationList() : CNodeList() {}
	virtual ~CImageOperationList() {CNodeList::DeleteAll();}
	
	void* Create(void)
	{
		IMAGEOPERATION* pData = new IMAGEOPERATION;
		return(pData);
	}
	
	void* Initialize(void* pVoid)
	{
		IMAGEOPERATION* pData = (IMAGEOPERATION*)pVoid;
		if(!pData)
			pData = (IMAGEOPERATION*)Create();
		if(pData)
			pData->Reset();
		return(pData);
	}

	BOOL PreDelete(ITERATOR iter)
	{
		if((IMAGEOPERATION*)iter->data)
		{
			delete ((IMAGEOPERATION*)iter->data);
			iter->data = (IMAGEOPERATION*)NULL;
		}
		return(TRUE);
	}

	int Size(void) {return(-1);}

#ifdef _DEBUG
	const char* Signature(void) {return("CImageOperationList");}
#endif
};

#define ADDIMAGEOPERATION(name,flag,ptr) {ptr = new IMAGEOPERATION(name,flag); if(ptr) m_ImageOperationList.Add(ptr);}

/*
	IMAGE_TIFFTYPE
	elemento per la lista dei tipi TIFF supportati
*/
struct IMAGE_TIFFTYPE {
public:
	IMAGE_TIFFTYPE()
	{
		Reset();
	}
	
	IMAGE_TIFFTYPE(int type,char* desc)
	{
		this->type = type;
		strcpyn(this->desc,desc,_MAX_PATH+1);
	}
	
	virtual ~IMAGE_TIFFTYPE() {}

	void Reset(void)
	{
		type = NULL_PICTURE;
		memset(desc,'\0',_MAX_PATH+1);
	}

	int	type;
	char	desc[_MAX_PATH+1];
};
typedef IMAGE_TIFFTYPE IMAGETIFFTYPE,*LPIMAGETIFFTYPE;

/*
	CTiffTypeList
	classe per la lista dei tipi TIFF
*/
class CTiffTypeList : public CNodeList
{
public:
	CTiffTypeList() : CNodeList() {}
	virtual ~CTiffTypeList() {CNodeList::DeleteAll();}
	
	void* Create(void)
	{
		IMAGETIFFTYPE* pData = new IMAGETIFFTYPE;
		return(pData);
	}
	
	void* Initialize(void* pVoid)
	{
		IMAGETIFFTYPE* pData = (IMAGETIFFTYPE*)pVoid;
		if(!pData)
			pData = (IMAGETIFFTYPE*)Create();
		if(pData)
			pData->Reset();
		return(pData);
	}

	BOOL PreDelete(ITERATOR iter)
	{
		if((IMAGETIFFTYPE*)iter->data)
		{
			delete ((IMAGETIFFTYPE*)iter->data);
			iter->data = (IMAGETIFFTYPE*)NULL;
		}
		return(TRUE);
	}

	int Size(void) {return(-1);}

#ifdef _DEBUG
	const char* Signature(void) {return("CTiffTypeList");}
#endif
};

#define ADDTIFFTYPE(type,desc,ptr) {ptr = new IMAGETIFFTYPE(type,desc); if(ptr) m_TiffTypeList.Add(ptr);}

/*
	CImageObject
*/
class CImageObject
{
public:
	CImageObject();
	virtual ~CImageObject() {}

	static BOOL			IsImageFile			(LPCSTR lpcszFileName); // formato grafico, non necessariamente supportato
	BOOL					IsSupportedFormat		(LPCSTR lpcszFileName); // formato grafico supportato 
	BOOL					IsSupportedFormatType	(LPCSTR lpcszFileName,IMAGE_TYPE type);
	
	int					CountImageFormats		(int& nTot);
	LPIMAGETYPE			EnumImageFormats		(void);
	LPIMAGETYPE			EnumReadableImageFormats	(void);
	LPIMAGETYPE			EnumWritableImageFormats	(void);
	BOOL					HaveImageOperation		(LPCSTR lpcszOperation);
	LPIMAGEOPERATION		EnumImageOperation		(void);

	CTiffTypeList*			GetTiffType			(void) {return(&m_TiffTypeList);}
	BOOL					HaveTiffType			(void) {return(m_TiffTypeList.Count() > 0);}

	// virtuali pure
	virtual LPCSTR			GetLibraryName			(void) = 0;
	virtual LPCSTR			GetPathName			(void) = 0;
	virtual LPCSTR			GetFileName			(void) = 0;
	virtual LPCSTR			GetFileExt			(void) = 0;
	virtual DWORD			GetFileSize			(void) = 0;

	virtual UINT			GetWidth				(void) = 0;
	virtual UINT			GetHeight				(void) = 0;
	
	virtual float			GetXRes				(void) = 0;
	virtual float			GetYRes				(void) = 0;
	virtual void			SetXRes				(float) = 0;
	virtual void			SetYRes				(float) = 0;
	virtual int			GetURes				(void) = 0;
	virtual void			SetURes				(UINT nRes) = 0;
	virtual void			GetDPI				(float& nXRes,float& nYRes) = 0;

	virtual int			GetCompression			(void) = 0;
	virtual void			SetCompression			(int) = 0;
	virtual int			GetQuality			(void) = 0;
	virtual void			SetQuality			(int) = 0;
	virtual BOOL			GetQualityRange		(int& nMin,int& nMax) = 0;

	virtual int			GetAlignment			(void) = 0;
	virtual UINT			GetBytesWidth			(UINT nWidth,UINT nBitsPerPixel,UINT nAlig) = 0;
	virtual UINT			GetBytesWidth			(void) = 0;
	virtual PHOTOMETRIC		GetPhotometric			(void) = 0;

	virtual int			GetMaxPaletteColors		(void) = 0;
	virtual UINT			GetNumColors			(void) = 0;
	virtual BOOL			CountBWColors			(unsigned int* pColors,unsigned char nNumColors) = 0;
	virtual BOOL			CountRGBColors			(COLORREF* pColors,unsigned int* pCountColors,unsigned char nNumColors) = 0;
	virtual UINT			GetBPP				(void) = 0;
	virtual UINT			ConvertToBPP			(UINT nBitsPerPixel,UINT nFlags,RGBQUAD* pPalette = NULL,UINT nColors = 0) = 0;
	
	virtual COLORREF		GetPixel				(UINT x,UINT y) = 0;
	virtual void			SetPixel				(UINT x,UINT y,COLORREF colorref) = 0;
	virtual void*			GetPixels				(void) = 0;
	virtual LPBITMAPINFO	GetBMI				(void) = 0;
	virtual HBITMAP		GetBitmap				(void) = 0;
	virtual UINT			GetMemUsed			(void) = 0;
	
	virtual HDIB			GetDIB				(DIBINFO* pDibInfo = NULL) = 0;
	virtual BOOL			SetDIB				(HDIB hDib,DIBINFO* pDibInfo = NULL) = 0;
	virtual int			GetDIBOrder			(void) = 0;
	virtual WORD			GetDIBNumColors		(LPSTR lpbi) = 0;
	virtual HPALETTE		CreateDIBPalette		(LPBITMAPINFO lpbmi) = 0;
	virtual BOOL			SetPalette			(UINT nIndex,UINT nColors,RGBQUAD* pPalette) = 0;

	virtual IMAGE_TYPE		GetType				(void) = 0;
	virtual void			GetHeaderInfo			(LPCSTR lpcszFileName,LPIMAGEHEADERINFO pHeaderInfo) = 0;

	virtual BOOL			Create				(BITMAPINFO* pBitmapInfo,void* pData = NULL) = 0;
	virtual BOOL			Load					(LPCSTR lpcszFileName) = 0;
	virtual BOOL			Unload				(void) = 0;
	virtual BOOL			IsLoaded				(void) = 0;
	virtual BOOL			Save					(void) = 0;
	virtual BOOL			Save					(LPCSTR lpcszFileName,LPCSTR lpcszFormat,DWORD dwFlags = 0L) = 0;
	virtual BOOL			Draw					(HDC hDC,const RECT* rcDest,const RECT* rcSrc,double fZoom = 1.0f,BOOL bPrinting = FALSE) = 0;
	virtual BOOL			Stretch				(RECT& rcSize,BOOL bAspectRatio = TRUE) = 0;
	virtual HANDLE			Copy					(RECT& rect) = 0;
	virtual BOOL			Paste				(UINT x,UINT y,HANDLE hDib) = 0;

	virtual BOOL			LockData				(void) = 0;
	virtual BOOL			UnlockData			(BOOL bModified = FALSE) = 0;
	virtual void			ShowErrors			(BOOL bFlag) = 0;

	virtual BOOL			SetImageParamsDefaultValues(CImageParams* pCImageParams) = 0;
	virtual BOOL			SetImageParamsMinMax	(CImageParams* pCImageParams) = 0;

	virtual UINT			Blur					(CImageParams*) = 0;
	virtual UINT			Brightness			(CImageParams*) = 0;
	virtual UINT			Contrast				(CImageParams*) = 0;
	virtual UINT			Deskew				(CImageParams*) = 0;
	virtual UINT			Despeckle				(CImageParams*) = 0;
	virtual UINT			Dilate				(CImageParams*) = 0;
	virtual UINT			EdgeEnhance			(CImageParams*) = 0;
	virtual UINT			Emboss				(CImageParams*) = 0;
	virtual UINT			Equalize				(CImageParams*) = 0;
	virtual UINT			Erosion				(CImageParams*) = 0;
	virtual UINT			FindEdge				(CImageParams*) = 0;
	virtual UINT			GammaCorrection		(CImageParams*) = 0;
	virtual UINT			Grayscale				(CImageParams*) = 0;
	virtual UINT			Halftone				(CImageParams*) = 0;
	virtual UINT			HistoContrast			(CImageParams*) = 0;
	virtual UINT			Hue					(CImageParams*) = 0;
	virtual UINT			Intensity				(CImageParams*) = 0;
	virtual UINT			IntensityDetect		(CImageParams*) = 0;
	virtual UINT			Invert				(CImageParams*) = 0;
	virtual UINT			Median				(CImageParams*) = 0;
	virtual UINT			MirrorHorizontal		(CImageParams*) = 0;
	virtual UINT			MirrorVertical			(CImageParams*) = 0;
	virtual UINT			Mosaic				(CImageParams*) = 0;
	virtual UINT			Negate				(CImageParams*) = 0;
	virtual UINT			Noise				(CImageParams*) = 0;
	virtual UINT			Posterize				(CImageParams*) = 0;
	virtual UINT			Rotate90Left			(CImageParams*) = 0;
	virtual UINT			Rotate90Right			(CImageParams*) = 0;
	virtual UINT			Rotate180				(CImageParams*) = 0;
	virtual UINT			SaturationHorizontal	(CImageParams*) = 0;
	virtual UINT			SaturationVertical		(CImageParams*) = 0;
	virtual UINT			Sharpen				(CImageParams*) = 0;
	virtual UINT			Size					(CImageParams*) = 0;

protected:
	char					m_szFileName[_MAX_PATH+1];	// nome file completo di pathname
	char					m_szFileExt[_MAX_PATH+1];	// estensione
	IMAGEHEADERINFO		m_InfoHeader;				// header immagine
	IMAGE_TYPE			m_ImageType;				// tipo immagine
	CImageTypeList			m_ImageTypeList;			// lista per i formati riconosciuti (deve essere caricata dalla derivata)
	CImageOperationList		m_ImageOperationList;		// lista per le operazioni supportate (deve essere caricata dalla derivata)
	CTiffTypeList			m_TiffTypeList;			// lista per i tipi tiff supportati (deve essere caricata dalla derivata)
	BOOL					m_bShowErrors;				// flag per visualizzazione errori
};

#endif // _CIMAGEOBJECT_H
