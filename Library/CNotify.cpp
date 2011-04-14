/*
	CNotify.cpp
	Classe per l'invio del messaggio (SDK/MFC).
	Luca Piergentili, 12/12/02
	lpiergentili@yahoo.com
*/
#include "window.h"
#include "CNotify.h"

#include "traceexpr.h"
//#define _TRACE_FLAG	_TRFLAG_TRACEOUTPUT
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
	CNotify()
*/
CNotify::CNotify(HWND hWnd,UINT nMsg)
{
	m_hWnd = hWnd;
	m_nMsg = nMsg;
}

/*
	Send()
*/
void CNotify::Send(WPARAM wParam/*=0*/,LPARAM lParam/*=0L*/)
{
	if(m_hWnd && m_nMsg > 0)
		::SendMessage(m_hWnd,m_nMsg,wParam,lParam);
}

/*
	Post()
*/
void CNotify::Post(WPARAM wParam/*=0*/,LPARAM lParam/*=0L*/)
{
	if(m_hWnd && m_nMsg > 0)
		::PostMessage(m_hWnd,m_nMsg,wParam,lParam);
}
