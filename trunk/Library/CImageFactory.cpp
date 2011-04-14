/*
	CImageFactory.cpp
	Classe fattoria per l'oggetto immagine.
	Luca Piergentili, 10/05/00
	lpiergentili@yahoo.com

	Ad memoriam - Nemo me impune lacessit.
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include <string.h>
#include "strings.h"
#include "window.h"
#include "ImageLibraryName.h"
#include "ImageConfig.h"
#ifdef HAVE_NEXGENIPL_LIBRARY
  #include "CNexgenIPL.h"
#endif
#ifdef HAVE_FREEIMAGE_LIBRARY
  #include "CFreeImage.h"
#endif
#ifdef HAVE_PAINTLIB_LIBRARY
  #include "CPaintLib.h"
#endif
#ifdef HAVE_LEADTOOLS_LIBRARY
  #include "CLeadTools.h"
#endif
#include "CImageFactory.h"
#include "CImage.h"

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

#ifdef IMAGE_DLL
/*
	DllMain()
*/
BOOL APIENTRY DllMain(HANDLE hModule,DWORD dwReason,LPVOID lpReserved)
{
	switch(dwReason)
	{
		case DLL_PROCESS_ATTACH:
#ifdef HAVE_LEADTOOLS_LIBRARY
			HWND hWnd;
			hWnd = ::FindWindow(NULL,"LEADTOOLS 10.0 Express Notice");
			if(hWnd)
				::SendMessage(hWnd,WM_CLOSE,0,0);
				//::ShowWindow(hWnd,SW_HIDE);
#endif
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			break;
	}

	return(TRUE);
}
#endif

/*
	CImageFactory()
*/
CImageFactory::CImageFactory()
{
	SUPPORTED_LIBRARIES* library;

	m_bCreated = FALSE;
	m_pImage = NULL;

	// aggiunge alla lista interna le librerie supportate
#ifdef HAVE_NEXGENIPL_LIBRARY
	library = new SUPPORTED_LIBRARIES;
	if(library)
	{
		strcpyn(library->name,NEXGENIPL_LIB_NAME,_MAX_PATH+1);
		m_LibrariesList.Add(library);
	}
#endif
#ifdef HAVE_FREEIMAGE_LIBRARY
	library = new SUPPORTED_LIBRARIES;
	if(library)
	{
		strcpyn(library->name,FREEIMAGE_LIB_NAME,_MAX_PATH+1);
		m_LibrariesList.Add(library);
	}
#endif
#ifdef HAVE_PAINTLIB_LIBRARY
	library = new SUPPORTED_LIBRARIES;
	if(library)
	{
		strcpyn(library->name,PAINTLIB_LIB_NAME,_MAX_PATH+1);
		m_LibrariesList.Add(library);
	}
#endif
#ifdef HAVE_LEADTOOLS_LIBRARY
	library = new SUPPORTED_LIBRARIES;
	if(library)
	{
		strcpyn(library->name,LEADTOOLS_LIB_NAME,_MAX_PATH+1);
		m_LibrariesList.Add(library);
	}
#endif
}

/*
	~CImageFactory()
*/
CImageFactory::~CImageFactory()
{
	Delete();
}

/*
	Create()

	Crea l'oggetto di tipo CImage a seconda della libreria specificata.
*/
CImage* CImageFactory::Create(LPSTR lpszLibraryName/*= NULL*/,UINT nSize/*= (UINT)-1*/)
{
	char* pLibraryName = lpszLibraryName ? lpszLibraryName : "";

	if(m_pImage)
		delete m_pImage;
	
	m_pImage = NULL;

#ifdef HAVE_NEXGENIPL_LIBRARY
	if(!m_pImage)
		if(strcmp(pLibraryName,NEXGENIPL_LIB_NAME)==0)
			m_pImage = new CNexgenIPL();
#endif
#ifdef HAVE_FREEIMAGE_LIBRARY
	if(!m_pImage)
		if(strcmp(pLibraryName,FREEIMAGE_LIB_NAME)==0)
			m_pImage = new CFreeImage();
#endif
#ifdef HAVE_PAINTLIB_LIBRARY
	if(!m_pImage)
		if(strcmp(pLibraryName,PAINTLIB_LIB_NAME)==0)
			m_pImage = new CPaintLib();
#endif
#ifdef HAVE_LEADTOOLS_LIBRARY
	if(!m_pImage)
		if(strcmp(pLibraryName,LEADTOOLS_LIB_NAME)==0)
			m_pImage = new CLeadTools();
#endif

#ifdef IMAGE_DEFAULT_LIBRARY
	if(!m_pImage)
	{
#ifdef HAVE_NEXGENIPL_LIBRARY
		if(strcmp(IMAGE_DEFAULT_LIBRARY,NEXGENIPL_LIB_NAME)==0)
			m_pImage = new CNexgenIPL();
#endif
#ifdef HAVE_PAINTLIB_LIBRARY
		if(strcmp(IMAGE_DEFAULT_LIBRARY,PAINTLIB_LIB_NAME)==0)
			m_pImage = new CPaintLib();
#endif
#ifdef HAVE_FREEIMAGE_LIBRARY
		if(strcmp(IMAGE_DEFAULT_LIBRARY,FREEIMAGE_LIB_NAME)==0)
			m_pImage = new CFreeImage();
#endif
#ifdef HAVE_LEADTOOLS_LIBRARY
		if(strcmp(IMAGE_DEFAULT_LIBRARY,LEADTOOLS_LIB_NAME)==0)
			m_pImage = new CLeadTools();
#endif
	}
#endif

	if(m_pImage && pLibraryName && nSize!=(UINT)-1)
		strcpyn(pLibraryName,m_pImage->GetLibraryName(),nSize);

	if(m_pImage)
		m_bCreated = TRUE;

	return(m_pImage);
}

/*
	Delete()
*/
void CImageFactory::Delete(void)
{
	if(m_bCreated)
		if(m_pImage)
			delete m_pImage,m_pImage = NULL;
}

/*
	EnumLibraryNames()

	Enumera le librerie supporate, da chiamare in un ciclo fino a che non restituisca
	NULL (non interrompere il ciclo con un break perche' sfaserebbe il fine ciclo interno).
*/
LPCSTR CImageFactory::EnumLibraryNames(void)
{
	static int i = -1;
	char* p = NULL;
	SUPPORTED_LIBRARIES* library;
	ITERATOR iter;

	if(++i >= m_LibrariesList.Count())
	{
		i = -1;
		p = NULL;
	}
	else
	{
		if((iter = m_LibrariesList.FindAt(i))!=(ITERATOR)NULL)
		{
			library = (SUPPORTED_LIBRARIES*)iter->data;
			if(library)
				p = library->name;
		}
	}

	return(p);
}
