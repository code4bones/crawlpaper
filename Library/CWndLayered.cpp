/*
	CWndLayered.cpp
	Classe per la trasparenza della finestra (SDK/MFC).
	Luca Piergentili, 26/09/01
	lpiergentili@yahoo.com
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include "window.h"
#include "CWndLayered.h"

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

// statiche per la classe (comuni a tutte le istanze della classe)
// il contatore per le referenze viene usato per sapere quando caricare/scaricare la dll
int CWndLayered::m_nRefCount = 0;
HMODULE CWndLayered::m_hUser32Dll = NULL;
PFNSETLAYEREDWINDOWATTRIBUTES CWndLayered::m_pfnSetLayeredWindowAttributes = NULL;

/*
	CWndLayered()
*/
CWndLayered::CWndLayered(HWND hWnd/*=NULL*/)
{
	m_hWnd = hWnd;
	m_lWndStyle = 0L;
	
	// carica la dll solo alla prima chiamata (l'handle viene condiviso da tutte le istanze della classe)
	if(m_nRefCount++==0)
		if((m_hUser32Dll = ::GetModuleHandle("User32.dll"))!=(HMODULE)NULL)
			m_pfnSetLayeredWindowAttributes = (PFNSETLAYEREDWINDOWATTRIBUTES)::GetProcAddress(m_hUser32Dll,"SetLayeredWindowAttributes");
}

/*
	~CWndLayered()
*/
CWndLayered::~CWndLayered()
{
	// scarica la dll quando non esistono piu' istanze della classe
	if(--m_nRefCount==0)
		if(m_hUser32Dll)
			::FreeLibrary(m_hUser32Dll),m_hUser32Dll = NULL;
}

/*
	Reset()
*/
BOOL CWndLayered::Reset(void)
{
	BOOL bResetted = FALSE;

	if(m_hWnd)
		if(m_lWndStyle!=0L)
			bResetted = ::SetWindowLong(m_hWnd,GWL_EXSTYLE,m_lWndStyle)!=0L;
	
	return(bResetted);
}

/*
	Layer()
*/
BOOL CWndLayered::Layer(BYTE bAlpha/* = (LWA_ALPHA_MAX * LWA_ALPHA_DEFAULT / 100)*/,COLORREF crKey/* = 0*/,DWORD dwFlags/* = LWA_ALPHA*/)
{
	BOOL bLayered = FALSE;

	if(m_hWnd && m_hUser32Dll && m_pfnSetLayeredWindowAttributes && (bAlpha >= LWA_ALPHA_MIN && bAlpha <= LWA_ALPHA_MAX))
	{
		LONG lStyle = ::GetWindowLong(m_hWnd,GWL_EXSTYLE);
		
/*		if(!(lStyle & WS_EX_LAYERED))
			if((m_lWndStyle = ::SetWindowLong(m_hWnd,GWL_EXSTYLE,lStyle|WS_EX_LAYERED))!=0L)
				bLayered = m_pfnSetLayeredWindowAttributes(m_hWnd,crKey,bAlpha,dwFlags);
*/		if(!(lStyle & WS_EX_LAYERED))
			m_lWndStyle = ::SetWindowLong(m_hWnd,GWL_EXSTYLE,lStyle|WS_EX_LAYERED);
		bLayered = m_pfnSetLayeredWindowAttributes(m_hWnd,crKey,bAlpha,dwFlags);
	}

	return(bLayered);
}

/*
	Transparent()
*/
BOOL CWndLayered::Transparent(void)
{
	BOOL bTransparent = FALSE;

	if(m_hWnd)
	{
		LONG lStyle = ::GetWindowLong(m_hWnd,GWL_EXSTYLE);
		
		if(!(lStyle & WS_EX_TRANSPARENT))
			if((m_lWndStyle = ::SetWindowLong(m_hWnd,GWL_EXSTYLE,lStyle|WS_EX_TRANSPARENT))!=0L)
				bTransparent = TRUE;
	}

	return(bTransparent);
}
