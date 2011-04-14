/*
	CColorStatic.cpp
	Classe per il controllo statico con controllo dei colori di primo piano/fondo (MFC).
	Luca Piergentili, 06/06/03
	lpiergentili@yahoo.com

	Tratta da "Programmare Windows 95 con MFC" di Jeff Prosise (modifiche minori).
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include "window.h"
#include "CColorStatic.h"

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

BEGIN_MESSAGE_MAP(CColorStatic,CStatic)
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()

/*
	CColorStatic()
*/
CColorStatic::CColorStatic()
{
	m_crTextColor = RGB(0,0,0);
	m_crBkColor = ::GetSysColor(COLOR_3DFACE);
	m_brBkgnd.CreateSolidBrush(m_crBkColor);
}

/*
	SetTextColor()
*/
void CColorStatic::SetTextColor(COLORREF crColor)
{
	m_crTextColor = crColor;
	Invalidate();
}

/*
	SetBkColor()
*/
void CColorStatic::SetBkColor(COLORREF crColor)
{
	m_crBkColor = crColor;
	m_brBkgnd.DeleteObject();
	m_brBkgnd.CreateSolidBrush(crColor);
	Invalidate();
}

/*
	CtlColor()
*/
HBRUSH CColorStatic::CtlColor(CDC* pDC,UINT /*nCtlColor*/)
{
	pDC->SetTextColor(m_crTextColor);
	pDC->SetBkColor(m_crBkColor);
	return((HBRUSH)m_brBkgnd);
}
