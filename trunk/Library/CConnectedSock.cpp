/*
	CConnectedSock.cpp
	Classi per la gestione delle connessioni TCP/IP (lato client) - versione bloccante (SDK/MFC).
	Luca Piergentili, 20/01/00
	lpiergentili@yahoo.com
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include "window.h"
#include <stdlib.h>
#include "CThread.h"
#include "CSock.h"
#include "CConnectedSock.h"

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
	CConnectedSocket()
*/
CConnectedSocket::CConnectedSocket()
{
	m_pSocket = (CSock*)NULL;
	m_pThread = (CThread*)NULL;
}

/*
	~CConnectedSocket()
*/
CConnectedSocket::~CConnectedSocket()
{
	Reset();
}

/*
	Reset()
*/
void CConnectedSocket::Reset(void)
{
	// socket
	if(m_pSocket)
	{
		m_pSocket->Close();
		delete m_pSocket,m_pSocket = (CSock*)NULL;
	}

	// thread
	m_pThread = (CThread*)NULL;
}
