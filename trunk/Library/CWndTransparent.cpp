/*
	CWndTransparent.cpp
	Classe per la trasparenza della finestra (SDK/MFC).
	Luca Piergentili, 03/10/03
	lpiergentili@yahoo.com
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include "window.h"
#include "CWndTransparent.h"

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

// statiche per la classe (comuni a tutte le istanze della classe)
// il contatore per le referenze viene usato per sapere quando caricare/scaricare la dll
int CWndTransparent::m_nRefCount = 0;
HMODULE CWndTransparent::m_hUser32Dll = NULL;
PFNSETLAYEREDWINDOWATTRIBUTES CWndTransparent::m_pfnSetLayeredWindowAttributes = NULL;

/*
	CWndTransparent()
*/
CWndTransparent::CWndTransparent(HWND hWnd/*=NULL*/)
{
	m_hWnd = hWnd;
	m_lWndStyle = 0L;
	
	// carica la dll solo alla prima chiamata (l'handle viene condiviso da tutte le istanze della classe)
	if(m_nRefCount++==0)
		if((m_hUser32Dll = ::GetModuleHandle("User32.dll"))!=(HMODULE)NULL)
			m_pfnSetLayeredWindowAttributes = (PFNSETLAYEREDWINDOWATTRIBUTES)::GetProcAddress(m_hUser32Dll,"SetLayeredWindowAttributes");
}

/*
	~CWndTransparent()
*/
CWndTransparent::~CWndTransparent()
{
	// scarica la dll quando non esistono piu' istanze della classe
	if(--m_nRefCount==0)
		if(m_hUser32Dll)
			::FreeLibrary(m_hUser32Dll),m_hUser32Dll = NULL;
}

/*
	Reset()
*/
BOOL CWndTransparent::Reset(void)
{
	BOOL bResetted = FALSE;

	if(m_hWnd)
		if(m_lWndStyle!=0L)
			bResetted = ::SetWindowLong(m_hWnd,GWL_EXSTYLE,m_lWndStyle)!=0L;
	
	return(bResetted);
}

/*
	Transparent()
*/
BOOL CWndTransparent::Transparent(void)
{
	BOOL bLayered = FALSE;

	if(m_hWnd && m_hUser32Dll && m_pfnSetLayeredWindowAttributes)
	{
		LONG lStyle = ::GetWindowLong(m_hWnd,GWL_EXSTYLE);
		
		if(!(lStyle & WS_EX_TRANSPARENT))
			m_lWndStyle = ::SetWindowLong(m_hWnd,GWL_EXSTYLE,lStyle|WS_EX_TRANSPARENT);
		
//		if(m_lWndStyle!=0L)
//			bLayered = m_pfnSetLayeredWindowAttributes(m_hWnd,crKey,bAlpha,dwFlags);
	}

	return(bLayered);
}
