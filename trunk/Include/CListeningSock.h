/*
	CListeningSock.h
	Classi per la gestione delle connessioni TCP/IP (lato server) - versione bloccante.
	Luca Piergentili, 20/01/00
	lpiergentili@yahoo.com
*/
#ifndef _CLISTENINGSOCK_H
#define _CLISTENINGSOCK_H 1

#include "window.h"
#include "CNodeList.h"
#include "CSync.h"
#include "CSock.h"

/*
	CListeningSocket

	Classe base per la gestione delle connessioni TCP/IP (lato server).
	Crea un nuovo socket che viene messo in ascolto sulla porta specificata per la gestione delle connessioni.
	Le connessioni (ossia gli oggetti di tipo CConnectedSocket) devono essere gestiti (attraverso una lista)
	dalla classe derivata.
	La classe derivata per la gestione della lista dovra' definire i membri per l'inserimento, la ricerca e
	l'eliminazione degli oggetti.
	La gestione della lista non puo' avvenire nella classe base perche' al momento del rilascio degli oggetti
	in essa presenti cio' che verrebbe rilasciato sarebbe un oggetto di tipo CConnectedSocket e non il tipo
	derivato, effettivamente presente nella lista.
*/
class CListeningSocket : public CNodeList, public CSyncThreads
{
public:
	CListeningSocket(UINT nPortNumber);
	virtual ~CListeningSocket();

	// servizio
	BOOL		StartListen			(void);
	BOOL		StopListen			(void);

	// socket
	CSock*	GetSocket				(void);
	UINT		GetPortNumber			(void) {return(m_nPortNumber);}

private:
	UINT		m_nPortNumber;			// porta per connessioni
	CSock	m_ListeningSocket;		// socket per ascolto
};

#endif // _CLISTENINGSOCK_H
