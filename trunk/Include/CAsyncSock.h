/*
	CAsyncSock.h
	Classe per interfaccia Winsock (modalita' asincrona).
	Luca Piergentili, 04/09/98
	lpiergentili@yahoo.com
*/
#ifndef _CASYNCSOCK_H
#define _CASYNCSOCK_H 1

#include "window.h"
#include "CSock.h"
#include "CAsyncSockMessages.h"

class CSockWnd;
class CAsyncSock;

/*
	CSockWnd

	Classe interna per CAsyncSock, non utilizzare direttamente.
	Crea la finestra a cui vengono indirizzati i messaggi delle WSAAsync...().

	Ognuna delle funzioni Async...() di CAsyncSock chiama la corrispondente di CSockWnd.
	Le Async...() di CSockWnd chiamano le WSAAsync...() impostando come callback le proprie
	On...().
	Le On...() di CSockWnd formattano i dati ricevuti e chiamano le callback di CAsyncSock
	che a loro volta si occupano di inviare il messaggio relativo con i dati all'applicazione
	principale:
	App			CAsyncSock		CSockWnd
	x()		->	a()		->		a()
								|
								wsa...(wnd handle,msg)
								|
	on_x()	<-	a_callback()	<-	on_a()
*/
class CSockWnd : public CWnd
{
	// per poter accedere ai membri privati/protetti di CSock/CAsyncSock
	friend class CAsyncSock;

public:
	CSockWnd(CAsyncSock* pAsyncSock = NULL,CWnd* pParent = NULL);
	virtual ~CSockWnd(void);

private:
	/*
		AsyncGetHostByName()
		hostname -> ip
	*/
	int AsyncGetHostByName(LPCSTR,LPSTR,UINT);

	HANDLE	m_hAsyncGetHostByName;
	LPSTR	m_lpszAsyncGetHostByName;
	UINT		m_nAsyncGetHostByName;
	char		m_szIPAddr[IP_ADDRESS_SIZE];

	/*		
		AsyncGetHostByAddr()
		ip -> hostname
	*/
	int AsyncGetHostByAddr(LPCSTR,LPSTR,UINT);

	HANDLE	m_hAsyncGetHostByAddr;
	LPSTR	m_lpszAsyncGetHostByAddr;
	UINT		m_nAsyncGetHostByAddr;
	char		m_szHostName[HOSTNAME_SIZE];
	
	/*
		AsyncGetServByName()
		servicename -> port
	*/
	int AsyncGetServByName(LPCSTR,LPCSTR,LPSTR,UINT);

	HANDLE	m_hAsyncGetServByName;
	LPSTR	m_lpszAsyncGetServByName;
	UINT		m_nAsyncGetServByName;
	UINT		m_nPort;
	
	/*
		AsyncGetServByPort()
		port -> servicename
	*/
	int AsyncGetServByPort(UINT,LPCSTR,LPSTR,UINT);

	HANDLE	m_hAsyncGetServByPort;
	LPSTR	m_lpszAsyncGetServByPort;
	UINT		m_nAsyncGetServByPort;
	char		m_szService[32];

	// ptr a CAsyncSock per chiamare le callback relative
	CAsyncSock* m_pAsyncSock;

protected:
	// callback per WSAAsyncSelect() per gli eventi asincroni del socket
	LONG OnAsyncSelect(UINT,LONG);				// WM_ASYNCSELECT
	
	// callback per le WSAAsyncGetXByY(), ricevono i dati non formattati
	LONG OnAsyncGetHostByName(UINT,LONG);			// WM_ASYNCGETHOSTBYNAME
	LONG OnAsyncGetHostByAddr(UINT,LONG);			// WM_ASYNCGETHOSTBYADDR
	LONG OnAsyncGetServByName(UINT,LONG);			// WM_ASYNCGETSERVBYNAME
	LONG OnAsyncGetServByPort(UINT,LONG);			// WM_ASYNCGETSERVBYPORT

	DECLARE_MESSAGE_MAP()
};

/*
	CAsyncSock
*/
class CAsyncSock : public CSock
{
	// per poter accedere ai membri privati/protetti di CSockWnd
	friend class CSockWnd;

public:
	CAsyncSock(CWnd* = NULL);
	~CAsyncSock(void);

	// interfaccia
	inline int	AsyncClose		(void) {return(CAsyncSock::Close());}	// chiude il socket asincrono
	BOOL			AsyncConnect		(LPCSTR,UINT);						// collega all'host/porta
	BOOL			AsyncGetHostByName	(LPCSTR);							// ricava l'ip a partire dal nome
	BOOL			AsyncGetHostByAddr	(LPCSTR);							// ricava il nome a partire dall'ip
	BOOL			AsyncGetServByName	(LPCSTR,LPCSTR = "tcp");				// ricava il # porta corrispondente al servizio
	BOOL			AsyncGetServByPort	(UINT,LPCSTR = "tcp");				// ricava il servizio corrispondente al # porta

	// ridefinisce le virtual di CSock per la gestione degli eventi asincroni
	virtual SOCKET	Create			(void);
	virtual int	Close			(LPSTR = NULL,int = 0);

	CSockWnd*		m_pSockWnd;										// ptr alla finestra interna per chiamare le funzioni relative

protected:
	// callback per CSockWnd::OnAsyncSelect() per gli eventi asincroni del socket
	LONG			AsyncConnectCallBack		(UINT,LONG);
	LONG			AsyncWriteCallBack			(UINT,LONG);
	LONG			AsyncReadCallBack			(UINT,LONG);
	LONG			AsyncCloseCallBack			(UINT,LONG);
	
	// callback per le CSockWnd::AsyncGetXByY(), ricevono i dati formattati
	LONG			AsyncGetHostByNameCallBack	(UINT,LONG);
	LONG			AsyncGetHostByAddrCallBack	(UINT,LONG);
	LONG			AsyncGetServByNameCallBack	(UINT,LONG);
	LONG			AsyncGetServByPortCallBack	(UINT,LONG);
	
	// On...() per le classi derivate:
	// l'applicazione che istanzia la classe CAsyncSock riceve i messaggi
	// le classi derivate da CAsyncSock accedono ai dati non attraverso i messaggi ma ridefinendo
	// le seguenti funzioni
	virtual BOOL	OnAsyncGetHostByName		(LPCSTR)	{return(TRUE);}
	virtual BOOL	OnAsyncGetHostByAddr		(LPCSTR)	{return(TRUE);}
	virtual BOOL	OnAsyncGetServByName		(UINT)	{return(TRUE);}
	virtual BOOL	OnAsyncGetServByPort		(LPCSTR)	{return(TRUE);}

private:
	char			m_szAsyncGetHostByName[MAXGETHOSTSTRUCT];				// buffer utilizzati dalle AsyncGetXByY()
	char			m_szAsyncGetHostByAddr[MAXGETHOSTSTRUCT];
	char			m_szAsyncGetServByName[MAXGETHOSTSTRUCT];
	char			m_szAsyncGetServByPort[MAXGETHOSTSTRUCT];
};

#endif // _CASYNCSOCK_H
