/*
	CConnectSock.h
	Classi per la gestione delle connessioni TCP/IP (lato client) - versione asincrona.
	Luca Piergentili, 20/01/00
	lpiergentili@yahoo.com
*/
#ifndef _CCONNECTSOCK_H
#define _CCONNECTSOCK_H 1

#include <afxwin.h>
#define STRICT 1
#include <windows.h>
#include "typedef.h"
#include "strcpyn.h"
#include "CNodeList.h"
#include "CMutexEx.h"
#include "CSock.h"

/*
	CConnectSock

	Classe base per la gestione delle connessioni TCP/IP (lato client).
	Crea l'oggetto basico per il socket in connessione.
	Gli oggetti di tale tipo devono essere gestiti (attraverso una lista) da una classe che deve essere
	fatta derivare da CListenSock.
*/
class CConnectSock
{
public:
	CConnectSock();
	virtual ~CConnectSock();

	// recupera/imposta i buffer di input/output
	inline const char*	GetData			(void)			{return(m_pSocket ? m_pSocket ->GetData() : NULL);}
	inline void		SetData			(const char* data)	{if(m_pSocket ) m_pSocket ->SetData(data);}

	// riceve/invia dai buffer di input/output
	int				ReceiveData		(void);
	int				SendData			(void);
	
	// socket
	inline void		SetSocketObject	(CSock* s)		{m_pSocket  = s;}
	inline CSock*		GetSocketObject	(void)			{return(m_pSocket);}
	inline SOCKET		GetSocketHandle	(void)			{return(m_pSocket  ? m_pSocket ->GetHandle() : INVALID_SOCKET);}
	inline void		SetSocketState		(WORD f)			{socket_state = f;}
	inline WORD		GetSocketState		(void)			{return(socket_state);}

	// thread
	inline void		SetThread			(CWinThread* t)	{m_pThread = t;}
	inline CWinThread*	GetThread			(void)			{return(m_pThread);}
	inline DWORD		GetThreadId		(void)			{return(m_pThread ? m_pThread->m_nThreadID : (DWORD)-1);}
	inline void		SetThreadState		(THREAD_STATE ts)	{thread_state = ts;}
	inline THREAD_STATE	GetThreadState		(void)			{return(thread_state);}

	// mutex
	BOOL				LockConnectSock	(UINT timeout = MUTEX_LOCK_TIMEOUT);
	BOOL				UnlockConnectSock	(void);

private:
	CSock*			m_pSocket;		// socket in connessione
	WORD				socket_state;		// status del socket (messaggio WSA corrente)
	CWinThread*		m_pThread;		// thread per le operazioni di I/O sul socket in connessione
	THREAD_STATE		thread_state;		// status del thread
	CMutexEx*			m_pMutex;			// mutex (anonimo) per sincornizzare l'accesso al socket
};

#endif // _CCONNECTSOCK_H
