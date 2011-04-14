/*
	CImageObject.cpp
	Classe base per la definizione dell'oggetto immagine.
	Luca Piergentili, 01/09/00
	lpiergentili@yahoo.com

	Ad memoriam - Nemo me impune lacessit.
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include <stdio.h>
#include <stdlib.h>
#include "strings.h"
#include "window.h"
#include "CNodeList.h"
#include "CImageParams.h"
#include "CImageObject.h"

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

static const IMAGE_TYPE_EXT pImageFormatsArray[] = {
	".avi",
	".bmp",
	".dib",
	".rle",
	".cur",
	".dcx",
	".gif",
	".ico",
	".iif",
	".eps",
	".dxf",
	".drw",
	".cgm",
	".cmx",
	".cdr",
	".flm",
	".fpx",
	".jpg",
	".jpeg",
	".jif",
	".jpe",
	".lbm",
	".img",
	".kdc",
	".mpg",
	".mpeg",
	".msp",
	".psp",
	".pbm",
	".pcd",
	".mac",
	".pct",
	".pcx",
	".pdf",
	".pxr",
	".pgm",
	".pic",
	".pict",
	".png",
	".ppm",
	".psd",
	".pdd",
	".sct",
	".raw",
	".tga",
	".vda",
	".icb",
	".vst",
	".tif",
	".tiff",
	".wmf",
	NULL
};

/*
	CImageObject()
*/
CImageObject::CImageObject()
{
	memset(m_szFileName,'\0',sizeof(m_szFileName));
	memset(m_szFileExt,'\0',sizeof(m_szFileExt));
	memset(&m_InfoHeader,'\0',sizeof(m_InfoHeader));
	m_ImageType = NULL_PICTURE;
	m_ImageTypeList.DeleteAll();
	m_ImageOperationList.DeleteAll();
	m_TiffTypeList.DeleteAll();
	m_bShowErrors = TRUE;
}

/*
	IsImageFile()

	Controlla (in base all'estensione) se il nome file fa riferimento ad un tipo valido.
*/
BOOL CImageObject::IsImageFile(LPCSTR lpcszFileName)
{
	// formato grafico, non necessariamente supportato
	for(register int i=0; pImageFormatsArray[i].ext!=NULL; i++)
		if(striright(lpcszFileName,pImageFormatsArray[i].ext)==0)
			return(TRUE);

	return(FALSE);
}

/*
	IsSupportedFormat()

	Controlla (in base all'estensione) se il nome file fa riferimento ad un tipo supportato.
*/
BOOL CImageObject::IsSupportedFormat(LPCSTR lpcszFileName)
{
	// formato grafico supportato 
	BOOL bIsImageFile = FALSE;
	char* pExt = (char*)strrchr(lpcszFileName,'.');

	if(pExt)
	{
		ITERATOR iter;
		LPIMAGETYPE p;

		if((iter = m_ImageTypeList.First())!=(ITERATOR)NULL)
		{
			do
			{
				p = (LPIMAGETYPE)iter->data;
				
				if(p)
					if(stricmp(pExt,p->ext)==0)
					{
						bIsImageFile = TRUE;
						break;
					}

				iter = m_ImageTypeList.Next(iter);
			
			} while(iter!=(ITERATOR)NULL);
		}
	}

	return(bIsImageFile);
}

/*
	IsSupportedFormatType()

	Controlla (in base all'estensione ed al tipo) se il nome file fa riferimento ad un tipo supportato.
*/
BOOL CImageObject::IsSupportedFormatType(LPCSTR lpcszFileName,IMAGE_TYPE type)
{
	// formato grafico supportato 
	BOOL bIsImageFile = FALSE;
	char* pExt = (char*)strrchr(lpcszFileName,'.');

	if(pExt)
	{
		ITERATOR iter;
		LPIMAGETYPE p;

		if((iter = m_ImageTypeList.First())!=(ITERATOR)NULL)
		{
			do
			{
				p = (LPIMAGETYPE)iter->data;

				if(p)
					if(stricmp(pExt,p->ext)==0)
						if(type==p->type)
						{
							bIsImageFile = TRUE;
							break;
						}

				iter = m_ImageTypeList.Next(iter);
			
			} while(iter!=(ITERATOR)NULL);
		}
	}

	return(bIsImageFile);
}

/*
	CountImageFormats()

	Restituisce il numero di formati supportati.
*/
int CImageObject::CountImageFormats(int& nTot)
{
	ITERATOR iter;
	nTot = 0;

	if((iter = m_ImageTypeList.First())!=(ITERATOR)NULL)
	{
		do
		{
			nTot++;
			iter = m_ImageTypeList.Next(iter);
		
		} while(iter!=(ITERATOR)NULL);
	}
	
	return(nTot);
}

/*
	EnumImageFormats()

	Enumera i formati supporati, da chiamare in un ciclo fino a che non restituisca
	NULL (non interrompere il ciclo con un break perche' sfaserebbe il fine ciclo interno).
*/
LPIMAGETYPE CImageObject::EnumImageFormats(void)
{
	static int i = -1;
	LPIMAGETYPE p = NULL;
	ITERATOR iter;

	if(++i >= m_ImageTypeList.Count())
	{
		i = -1;
		p = NULL;
	}
	else
	{
		if((iter = m_ImageTypeList.FindAt(i))!=(ITERATOR)NULL)
			p = (LPIMAGETYPE)iter->data;
	}

	return(p);
}

/*
	EnumReadableImageFormats()

	Enumera i formati supporati in lettura, da chiamare in un ciclo fino a che non restituisca
	NULL (non interrompere il ciclo con un break perche' sfaserebbe il fine ciclo interno).
*/
LPIMAGETYPE CImageObject::EnumReadableImageFormats(void)
{
	static int i = -1;
	LPIMAGETYPE p = NULL;
	ITERATOR iter;

next:
	if(++i >= m_ImageTypeList.Count())
	{
		i = -1;
		p = NULL;
	}
	else
	{
		if((iter = m_ImageTypeList.FindAt(i))!=(ITERATOR)NULL)
		{
			p = (LPIMAGETYPE)iter->data;
			
			if(p)
				if(!(p->flags & IMAGE_READ_FLAG))
				{
					p = NULL;
					goto next;
				}
		}
	}

	return(p);
}

/*
	EnumWritableImageFormats()

	Enumera i formati supporati in scrittura, da chiamare in un ciclo fino a che non restituisca
	NULL (non interrompere il ciclo con un break perche' sfaserebbe il fine ciclo interno).
*/
LPIMAGETYPE CImageObject::EnumWritableImageFormats(void)
{
	static int i = -1;
	LPIMAGETYPE p = NULL;
	ITERATOR iter;

next:
	if(++i >= m_ImageTypeList.Count())
	{
		i = -1;
		p = NULL;
	}
	else
	{
		if((iter = m_ImageTypeList.FindAt(i))!=(ITERATOR)NULL)
		{
			p = (LPIMAGETYPE)iter->data;
			
			if(p)
				if(!(p->flags & IMAGE_WRITE_FLAG))
				{
					p = NULL;
					goto next;
				}
		}
	}

	return(p);
}

/*
	HaveImageOperation()

	Controlla se l'operazione specificata e' tra quelle supportate.
*/
BOOL CImageObject::HaveImageOperation(LPCSTR lpcszOperation)
{
	BOOL bFound = FALSE;
	ITERATOR iter;
	LPIMAGEOPERATION p;

	if((iter = m_ImageOperationList.First())!=(ITERATOR)NULL)
	{
		do
		{
			p = (LPIMAGEOPERATION)iter->data;

			if(p)
				if(stricmp(lpcszOperation,p->name)==0)
				{
					bFound = TRUE;
					break;
				}

			iter = m_ImageOperationList.Next(iter);
		
		} while(iter!=(ITERATOR)NULL);
	}
	
	return(bFound);
}

/*
	EnumImageOperation()
	
	Enumera le operazioni supportate, da chiamare in un ciclo fino a che non restituisca
	NULL (non interrompere il ciclo con un break perche' sfaserebbe il fine ciclo interno).
*/
LPIMAGEOPERATION CImageObject::EnumImageOperation(void)
{
	static int i = -1;
	LPIMAGEOPERATION p = NULL;
	ITERATOR iter;

	if(++i >= m_ImageOperationList.Count())
	{
		i = -1;
		p = NULL;
	}
	else
	{
		if((iter = m_ImageOperationList.FindAt(i))!=(ITERATOR)NULL)
			p = (LPIMAGEOPERATION)iter->data;
	}

	return(p);
}
