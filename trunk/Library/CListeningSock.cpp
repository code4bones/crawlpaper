/*
	CListeningSock.cpp
	Classi per la gestione delle connessioni TCP/IP (lato server) - versione bloccante.
	Luca Piergentili, 20/01/00
	lpiergentili@yahoo.com
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include "window.h"
#include <stdlib.h>
#include "CSync.h"
#include "CSock.h"
#include "CListeningSock.h"

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
	CListeningSocket()
*/
CListeningSocket::CListeningSocket(UINT nPortNumber) : CSyncThreads("CListeningSocket")
{
	m_nPortNumber = nPortNumber;
}

/*
	~CListeningSocket()
*/
CListeningSocket::~CListeningSocket()
{
}

/*
	StartListen()
*/
BOOL CListeningSocket::StartListen(void)
{
	// mette in ascolto il socket sulla porta specificata
	return(m_ListeningSocket.Listen(m_nPortNumber)!=INVALID_SOCKET);
}

/*
	StopListen()
*/
BOOL CListeningSocket::StopListen(void)
{
	// chiude la connessione
	return(m_ListeningSocket.Close());
}

/*
	GetSocket()
*/
CSock* CListeningSocket::GetSocket(void)
{
	return(&m_ListeningSocket);
}
