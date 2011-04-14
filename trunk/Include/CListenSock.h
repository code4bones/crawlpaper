/*
	CListenSock.h
	Classi per la gestione delle connessioni TCP/IP (lato server) - versione asincrona.
	Luca Piergentili, 20/01/00
	lpiergentili@yahoo.com
*/
#ifndef _CLISTENSOCK_H
#define _CLISTENSOCK_H 1

#include <afxwin.h>
#define STRICT 1
#include <windows.h>
#include "typedef.h"
#include "strcpyn.h"
#include "CNodeList.h"
#include "CMutexEx.h"
#include "CSock.h"

/*
	CListenSock

	Classe base per la gestione delle connessioni TCP/IP (lato server).
	Crea un nuovo socket che viene messo in ascolto sulla porta specificata per la gestione delle connessioni.
	Le connessioni (ossia gli oggetti di tipo CConnectSock) devono essere gestiti (attraverso una lista)
	dalla classe derivata.
	La classe derivata per la gestione della lista dovra' definire i membri per l'inserimento, la ricerca e
	l'eliminazione degli oggetti.
	La gestione della lista non puo' avvenire nella classe base perche' al momento del rilascio degli oggetti
	in essa presenti cio' che verrebbe rilasciato sarebbe un oggetto di tipo CConnectSock e non il tipo
	derivato, effettivamente presente nella lista.
*/
class CListenSock : public CNodeList
{
public:
	CListenSock(CWnd* pCallerWnd,UINT nWsaMessage,UINT nPortNumber);
	virtual ~CListenSock();

	// mutex
	BOOL		LockListenSock			(UINT timeout = MUTEX_LOCK_TIMEOUT);
	BOOL		UnlockListenSock		(void);

protected:
	CWnd*	m_pCallerWnd;			// puntatore alla finestra del chiamante (per l'invio dei messaggi WSA)
	UINT		m_nWsaMessage;			// messaggio (WSA) da inviare alla finestra del chiamante
	UINT		m_nPortNumber;			// numero porta per connessione
	CSock*	m_pListeningSocket;		// socket per ascolto
	CMutexEx*	m_Mutex;				// mutex (anonimo) per sincronizzare l'accesso alla lista
};

#endif // _CLISTENSOCK_H
