/*
	CLicenseProtocol.h
	Implementazione delle funzioni per il servizio delle licenze.
	Luca Piergentili, 02/06/00
	lpiergentili@yahoo.com
*/
#ifndef _CLICENSEPROTOCOL_H
#define _CLICENSEPROTOCOL_H 1

#include "typedef.h"
#include <stdlib.h>
#include "strcpyn.h"
#include "CPop3.h"
#include "CListenSock.h"
#include "CConnectSock.h"
#include "CLicenseService.h"

// 0x8000 through 0xBFFF
#define WM_LICENSEPROTOCOL WM_USER + 0x8000 + 1

struct SOCKETINFO {
	SOCKET	socket;
	char		ip[32];
};

class CListeningLicenseProtocolSocket;

/*
	CLicenseListeningWnd

	Definisce la finestra a cui indirizzare i messaggi wsa ed il codice per l'elaborazione dei messaggi.
	I socket in collegamento (che generano i messaggi wsa) vengono inseriti in una lista	ed associati ad
	un thread che si occupa dell'elaborazione (il thread si occupa di attivare il codice del protocollo
	delle licenze definito sotto da CLicenseProtocol).
*/
class CLicenseListeningWnd : public CFrameWnd
{
	DECLARE_DYNCREATE(CLicenseListeningWnd)

public:
	CLicenseListeningWnd(CListeningLicenseProtocolSocket* pSocket = NULL,CWnd* pMainWnd = NULL,UINT nWsa = (UINT)-1);
	virtual ~CLicenseListeningWnd() {}

	CWnd*	GetListeningWnd	(void) {return(this);}

private:
	LONG		OnLicense			(UINT wParam,LONG lParam);

	HICON	m_hIcon;
	CWnd*	m_pMainWnd;
	UINT		m_nWsa;
	SOCKETINFO m_SocketInfo;
	
	DECLARE_MESSAGE_MAP()
};

/*
	CLicenseProtocol

	Deriva da CPop3 per la gestione del protocollo (POP3) e da CLicenseService per la gestione
	dei dati relativi al servizio delle licenze (accesso al database delle licenze).
	Implementa il servizio delle licenze derivandolo da quello relativo al protocollo (POP3) e
	duplicando le funzioni basiche per intercettare i comandi non gestiti dal protocollo originale
	(POP3) ed implementarli in proprio (accede ai dati derivando dalla classe per l'accesso al database).
*/
class CLicenseProtocol : public CPop3, public CLicenseService
{
public:
	CLicenseProtocol(const char* pDomain,const char* pMailpath,const char* pDatapath,const char* pLogpath)
	: CPop3(pDomain,pMailpath,pDatapath,pLogpath), CLicenseService(pDatapath)
	{}

	virtual ~CLicenseProtocol() {}

	// servizio	
	virtual const char*	StartService	(CSock* pSocket);
	virtual void		ResetService	(CSock* pSocket = NULL);
	virtual const char*	ParseCommand	(POP3STATE& pop3state,const char* pCommand,int nLen);
	
	// utenti
	BOOLEAN			CreateUser	(const char* pCommand);
	BOOLEAN			DeleteUser	(const char* pCommand);
	BOOLEAN			ValidateUser	(const char* pCommand);
	BOOLEAN			ChangeUserPass	(const char* pCommand);
	
	// etichette
	const char*		GetLabel		(const char* pCommand);
	BOOLEAN			PutLabel		(const char* pCommand);
	BOOLEAN			RemoveLabel	(const char* pCommand);
};

/*
	CConnectedLicenseProtocolSocket

	Classe per il socket in collegamento.
	La classe definisce l'oggetto usato per i socket che si collegano al socket in ascolto sul lato
	server (implementato tramite la CListeningLicenseProtocolSocket).
	Allo scopo deriva da CConnectSock per il socket in collegamento e da CLicenseProtocol per le
	operazioni che vengono svolte dal socket in collegamento.
*/
class CConnectedLicenseProtocolSocket : public CConnectSock, public CLicenseProtocol
{
public:
	CConnectedLicenseProtocolSocket(const char* pDomain,const char* pMailpath,const char* pDatapath,const char* pLogpath)
	: CLicenseProtocol(pDomain,pMailpath,pDatapath,pLogpath)
	{}

	virtual ~CConnectedLicenseProtocolSocket() {}
};

/*
	CListeningLicenseProtocolSocket

	Classe per la gestione della lista dei socket in collegamento.
	La classe viene usata per creare l'oggetto in grado di gestire le connessioni TCP/IP sul lato server.
	Allo scopo deriva da CLicenseListeningWnd per la finestra/gestore dei messaggi wsa e da CListenSock
	per il socket in ascolto.
*/
class CListeningLicenseProtocolSocket : public CLicenseListeningWnd, public CListenSock
{
public:
	CListeningLicenseProtocolSocket(CWnd* pWnd,UINT nWsa,UINT nPort,const char* pDomain,const char* pMailpath,const char* pDatapath,const char* pLogpath)
	: CLicenseListeningWnd(this,pWnd,nWsa), CListenSock(CLicenseListeningWnd::GetListeningWnd()/*pWnd*/,WM_LICENSEPROTOCOL/*nWsa*/,nPort)
	{
		strcpyn(domain,pDomain,sizeof(domain));
		strcpyn(mailpath,pMailpath,sizeof(mailpath));
		strcpyn(datapath,pDatapath,sizeof(datapath));
		if(pLogpath && *pLogpath)
			strcpyn(logpath,pLogpath,sizeof(logpath));
		else
			memset(logpath,'\0',sizeof(logpath));
	}
	
	virtual ~CListeningLicenseProtocolSocket();

	// gestione degli elementi della lista
	CConnectedLicenseProtocolSocket* Insert(WORD);
	CConnectedLicenseProtocolSocket* FindSocket(SOCKET);
	CConnectedLicenseProtocolSocket* FindThread(DWORD);
	void Remove(SOCKET);
	BOOL PreDelete(ITERATOR iter)
	{
		delete ((CConnectedLicenseProtocolSocket*)iter->data);
		return(TRUE);
	}
	const char* Signature(void) {return("CListeningLicenseProtocolSocket");}
	int Size(void) {return(-1);}

private:
	char			domain[(HOSTNAME_SIZE*2) + 1];	// dominio POP3
	char			mailpath[_MAX_PATH + 1];			// pathname x mail
	char			datapath[_MAX_PATH + 1];			// pathname x base dati
	char			logpath[_MAX_PATH + 1];			// pathname x log
	unsigned long	flags;						// flags POP3
};

#endif // _CLICENSEPROTOCOL_H
