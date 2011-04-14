/*
	CConnectedSock.h
	Classi per la gestione delle connessioni TCP/IP (lato client) - versione bloccante (SDK/MFC).
	Luca Piergentili, 20/01/00
	lpiergentili@yahoo.com
*/
#ifndef _CCONNECTEDSOCK_H
#define _CCONNECTEDSOCK_H 1

#include "window.h"
#include "CThread.h"
#include "CSock.h"

/*
	CConnectedSocket

	Classe base per la gestione delle connessioni TCP/IP (lato client).
	Crea l'oggetto basico per il socket in connessione.
	Gli oggetti di tale tipo devono essere gestiti (attraverso una lista) da una classe che deve essere
	fatta derivare da CListeningSocket.
*/
class CConnectedSocket
{
public:
	CConnectedSocket();
	virtual ~CConnectedSocket();

	void				Reset		(void);

	// socket
	inline void		SetSocket		(CSock* pSocket)	{m_pSocket = pSocket;}
	inline CSock*		GetSocket		(void) const		{return(m_pSocket);}

	// thread
	inline void		SetThread		(CThread* pThread)	{m_pThread = pThread;}
	inline CThread*	GetThread		(void) const		{return(m_pThread);}

private:
	CSock*			m_pSocket;	// socket in connessione
	CThread*			m_pThread;	// thread per le operazioni di I/O sul socket in connessione
};

#endif // _CCONNECTEDSOCK_H
