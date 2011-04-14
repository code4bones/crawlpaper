/*
	CSock.cpp
	Classe derivata per interfaccia Winsock (CRT/SDK/MFC).
	Luca Piergentili, 06/07/98
	lpiergentili@yahoo.com
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "strings.h"
#include <ctype.h>
#include "typedef.h"
#if defined(_WINDOWS)
  #include "window.h"
  #include "CTextFile.h"
#endif
#include "CNodeList.h"
#include "lmhosts.h"
#include "CWinsock.h"
#include "CSock.h"

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

// codici di ritorno
static const int wsa_errorcode_num[] = {
	// propri
	WSA_SUCCESS,WSA_FAILURE,
	// wsa
	WSAEINTR,
	WSAEBADF,
	WSAEACCES,
	WSAEFAULT,
	WSAEINVAL,
	WSAEMFILE,
	WSAEWOULDBLOCK,
	WSAEINPROGRESS,
	WSAEALREADY,
	WSAENOTSOCK,
	WSAEDESTADDRREQ,
	WSAEMSGSIZE,
	WSAEPROTOTYPE,
	WSAENOPROTOOPT,
	WSAEPROTONOSUPPORT,
	WSAESOCKTNOSUPPORT,
	WSAEOPNOTSUPP,
	WSAEPFNOSUPPORT,
	WSAEAFNOSUPPORT,
	WSAEADDRINUSE,
	WSAEADDRNOTAVAIL,
	WSAENETDOWN,
	WSAENETUNREACH,
	WSAENETRESET,
	WSAECONNABORTED,
	WSAECONNRESET,
	WSAENOBUFS,
	WSAEISCONN,
	WSAENOTCONN,
	WSAESHUTDOWN,
	WSAETOOMANYREFS,
	WSAETIMEDOUT,
	WSAECONNREFUSED,
	WSAEDISCON,
	WSAELOOP,
	WSAENAMETOOLONG,
	WSAEHOSTDOWN,
	WSAEHOSTUNREACH,
	WSAENOTEMPTY,
	WSAEPROCLIM,
	WSAEUSERS,
	WSAEDQUOT,
	WSAESTALE,
	WSAEREMOTE,
	WSASYSNOTREADY,
	WSAVERNOTSUPPORTED,
	WSANOTINITIALISED,
	WSAHOST_NOT_FOUND,
	WSATRY_AGAIN,
	WSANO_RECOVERY,
	WSANO_DATA,
	WSANO_ADDRESS,
	// lasciare per ultimo
	WSA_UNKNOWERROR
};

// messaggi relativi ai codici di ritorno
static const char* wsa_errorcode_str[] = {
	// propri
	"WSA_SUCCESS",
	"WSA_FAILURE",
	// wsa
	"WSAEINTR: the call was cancelled via WSACancelBlockingCall()",
	"WSAEBADF: bad file number",
	"WSAEACCES: the address specified is a broadcast address, but the socket doesn't support sending broadcast packets",
	"WSAEFAULT: the address length supplied is too small",
	"WSAEINVAL: this function shouldn't be called at this time because listen() wasnt called before accept()",
	"WSAEMFILE: the queue is empty and no descriptors are available",
	"WSAEWOULDBLOCK: this is a non-blocking socket and no connections are present",
	"WSAEINPROGRESS: a blocking call is now in progress",
	"WSAEALREADY: the non-blocking routine has already finished",
	"WSAENOTSOCK: the socket number used isn't a socket",
	"WSAEDESTADDRREQ: a destination address wasn't given",
	"WSAEMSGSIZE: the datagram was too large for the buffer and was truncated",
	"WSAEPROTOTYPE: the socket can't use the protocol specified",
	"WSAENOPROTOOPT: the option is unknown or unsupported",
	"WSAEPROTONOSUPPORT: this protocol isn't supported",
	"WSAESOCKTNOSUPPORT: this address family doesn't support the socket type specified",
	"WSAEOPNOTSUPP: the socket referenced isn't open for connection oriented services",
	"WSAEPFNOSUPPORT: the address family specified isn't supported by this protocol",
	"WSAEAFNOSUPPORT: the address family specified isn't supported by this protocol",
	"WSAEADDRINUSE: the address given is already in use",
	"WSAEADDRNOTAVAIL: the address isn't available from this machine",
	"WSAENETDOWN: Winsock has reported that the network subsystem has failed",
	"WSAENETUNREACH: the network can't be reached",
	"WSAENETRESET: Winsock dropped the connection",
	"WSAECONNABORTED: the connection was aborted due to a network failure",
	"WSAECONNRESET: the connection was reset by the remote host",
	"WSAENOBUFS: no buffer space is available",
	"WSAEISCONN: the socket is already connected",
	"WSAENOTCONN: the socket isn't connected",
	"WSAESHUTDOWN: the socket has been shut down",
	"WSAETOOMANYREFS",
	"WSAETIMEDOUT: a connection attempt timed out; no connection was established",
	"WSAECONNREFUSED: the connection was refused",
	"WSAEDISCON: the message terminated gracefully",
	"WSAELOOP",
	"WSAENAMETOOLONG",
	"WSAEHOSTDOWN",
	"WSAEHOSTUNREACH",
	"WSAENOTEMPTY",
	"WSAEPROCLIM",
	"WSAEUSERS",
	"WSAEDQUOT",
	"WSAESTALE",
	"WSAEREMOTE",
	"WSASYSNOTREADY: the network subsystem isn't ready for communications",
	"WSAVERNOTSUPPORTED: the Winsock API version requested isn't supported",
	"WSANOTINITIALISED: WSAStartup() must be called successfully before you can use this function",
	"WSAHOST_NOT_FOUND: the authoritative answer host isn't available",
	"WSATRY_AGAIN: the non-authoritative host isn't available",
	"WSANO_RECOVERY: this is a non-recoverable error or SERVERFAIL",
	"WSANO_DATA: valid name but no data record of that type",
	"WSANO_ADDRESS",
	// lasciare per ultimo
	"unknown Winsock error"
};

// dimensione dell'array
#define WSAERRORCODE_ARRAY_SIZE ARRAY_SIZE(wsa_errorcode_str)

/*
	CSock()
*/
#ifdef WIN32_MFC
CSock::CSock(CWnd* pParent/*=NULL*/)
#else
#ifdef WIN32_SDK
CSock::CSock(HWND hWnd/*=NULL*/)
#else
CSock::CSock()
#endif
#endif
 : CWinsock()
{
	// ptr/handle finestra applicazione principale
#ifdef WIN32_MFC
	m_pParent = pParent;
	if(pParent)
		m_hWnd = pParent->m_hWnd;
	else
		m_hWnd = (HWND)NULL;
#else
	#ifdef WIN32_SDK
		m_hWnd = hWnd;
	#endif
#endif

	// flag per inizializzazione
	m_bInitialized = FALSE;

	// flag per visualizzazione errori
#if defined(_WINDOWS)
	m_bShowErrors = FALSE;
#endif

	// inizializza
	Reset();

	// delay per send()/receive() consecutive
	m_nDelay = WSA_WINSOCK_DELAY;

	// local host
	memset(m_szLocalHostIP,'\0',sizeof(m_szLocalHostIP));
	memset(m_szLocalHostName,'\0',sizeof(m_szLocalHostName));

	// inizializza la DLL
	if((m_WsaData.error = CWinsock::WSAStartup(WSA_VERSION,&(m_WsaData.data)))!=0) 
	{
		GetWSALastError(m_WsaData.error);
		return;
	}
 
	// controlla la versione (1.1)
	if(LOBYTE(m_WsaData.data.wVersion)!=WSA_MINOR_VERSION || HIBYTE(m_WsaData.data.wVersion)!=WSA_MAJOR_VERSION)
	{
		SetWSALastError(WSAEINVAL);
		CWinsock::WSACleanup();
		return;
	}

	// ricava il nome e l'ip dell'host locale
	GetHostLocalAddr();
	GetHostLocalName();

	m_bInitialized = TRUE;
}

/*
	~CSock()
*/
CSock::~CSock(void)
{
	// chiude il socket
	Close();

	// resetta la DLL
	CWinsock::WSACleanup();
}

/*
	Init()

	Inizializzazione dei dati relativi al socket.
*/
void CSock::Init(SOCKET socket/*=INVALID_SOCKET*/,SOCKADDR_IN* sockaddr_in/*=NULL*/,SOCKETSTATE state/*=UNDEFINED*/)
{
	m_Socket.socket = socket;
	m_Socket.status = state;
	m_Socket.error  = 0;
	m_Socket.format = PF_INET;
	m_Socket.type   = SOCK_STREAM;
	m_Socket.proto  = 0;

	SOCKADDR_IN saddr_in;
	memcpy(&saddr_in,sockaddr_in,sizeof(SOCKADDR_IN));
	strncpy(m_szLocalHostIP,CWinsock::inet_ntoa(saddr_in.sin_addr),HOSTNAME_SIZE);
	strncpy(m_szLocalHostName,m_szLocalHostIP,HOSTNAME_SIZE);
	
	memset(&m_WsaData,'\0',sizeof(WSADATA_EXT));
	ResetData();
	SetWSALastError(0);
}

/*
	Reset()

	Resetta i dati del socket.
*/
void CSock::Reset(void)
{
	m_Socket.socket = INVALID_SOCKET;
	m_Socket.status = UNDEFINED;
	m_Socket.error  = 0;
	m_Socket.format = PF_INET;
	m_Socket.type   = SOCK_STREAM;
	m_Socket.proto  = 0;
	memset(&m_WsaData,'\0',sizeof(WSADATA_EXT));
	ResetData();
	SetWSALastError(0);
}

/*
	SetFormat()

	Imposta il formato per il socket (da chiamare prima di creare il socket).
*/
int CSock::SetFormat(int nFormat/*=PF_INET*/)
{
	int nWsaError = WSANOTINITIALISED;

	// controlla che il costruttore abbia terminato correttamente
	if(IsValid())
	{
		if(m_Socket.socket==INVALID_SOCKET && m_Socket.status==UNDEFINED)
		{
			m_Socket.format = nFormat;
			nWsaError = 0;
		}
	}
	else
		SetWSALastError(nWsaError);
	
	return(nWsaError);
}

/*
	SetType()

	Imposta il tipo di socket (da chiamare prima di creare il socket).
*/
int CSock::SetType(int nType/*=SOCK_STREAM*/)
{
	int nWsaError = WSANOTINITIALISED;

	// controlla che il costruttore abbia terminato correttamente
	if(IsValid())
	{
		if(m_Socket.socket==INVALID_SOCKET && m_Socket.status==UNDEFINED)
		{
			m_Socket.type = nType;
			nWsaError = 0;
		}
	}
	else
		SetWSALastError(nWsaError);
	
	return(nWsaError);
}

/*
	SetProto()

	Imposta il protocollo per il socket (da chiamare prima di creare il socket).
*/
int CSock::SetProto(int nProto/*=0*/)
{
	int nWsaError = WSANOTINITIALISED;

	// controlla che il costruttore abbia terminato correttamente
	if(IsValid())
	{
		if(m_Socket.socket==INVALID_SOCKET && m_Socket.status==UNDEFINED)
		{
			m_Socket.proto = nProto;
			nWsaError = 0;
		}
	}
	else
		SetWSALastError(nWsaError);

	return(nWsaError);
}

/*
	SetBufferSize()

	Imposta le dimensioni dei buffer di I/O del socket.
*/
int CSock::SetBufferSize(UINT nSize/*=WSA_BUF_SIZE*/)
{
	int nWsaError;

	if(nSize < 512 || nSize > WSA_BUF_SIZE)
		nSize = WSA_BUF_SIZE;

	// dimensione del buffer di ricezione
	if(CWinsock::setsockopt(m_Socket.socket,SOL_SOCKET,SO_RCVBUF,(const char FAR *)&nSize,sizeof(int))==SOCKET_ERROR)
		nWsaError = GetWSALastError();
	else
		nWsaError = 0;

	// dimensione del buffer di invio
	if(CWinsock::setsockopt(m_Socket.socket,SOL_SOCKET,SO_SNDBUF,(const char FAR *)&nSize,sizeof(int))==SOCKET_ERROR)
		nWsaError = GetWSALastError();
	else
		nWsaError = 0;

	return(nWsaError);
}

/*
	SetTimeout()

	Imposta il tempo limite (in secondi) per il timeout sul socket.
*/
int CSock::SetTimeout(UINT nSecs/* = 15 */)
{
	int nWsaError;

	if(nSecs <= 0)
		nSecs = 15;
	nSecs *= 1000;

	// timeout (millisecondi) in ricezione
	if(CWinsock::setsockopt(m_Socket.socket,SOL_SOCKET,SO_RCVTIMEO,(const char FAR *)&nSecs,sizeof(int))==SOCKET_ERROR)
		nWsaError = GetWSALastError();
	else
		nWsaError = 0;

	// timeout (millisecondi) in invio
	if(CWinsock::setsockopt(m_Socket.socket,SOL_SOCKET,SO_SNDTIMEO,(const char FAR *)&nSecs,sizeof(int))==SOCKET_ERROR)
		nWsaError = GetWSALastError();
	else
		nWsaError = 0;

	return(nWsaError);
}

/*
	Open()

	Apre il socket.
*/
BOOL CSock::Open(int nFormat/*=PF_INET*/,int nType/*=SOCK_STREAM*/,int nProto/*=0*/)
{
	int nWsaError = WSANOTINITIALISED;

	// controlla che il costruttore abbia terminato correttamente
	if(IsValid())
	{
		// controlla che il socket non sia gia' aperto
		if(m_Socket.socket==INVALID_SOCKET && m_Socket.status==UNDEFINED)
		{
			if(Create()!=INVALID_SOCKET)
			{
				m_Socket.format = nFormat;
				m_Socket.type   = nType;
				m_Socket.proto  = nProto;
				m_Socket.status = OPEN;
				nWsaError = 0;
			}
		}
	}
	else
		SetWSALastError(WSANOTINITIALISED);
	
	return(nWsaError==0);
}

/*
	Create()

	Crea un nuovo socket.
*/
SOCKET CSock::Create(void)
{
	if((m_Socket.socket = CWinsock::socket(m_Socket.format,m_Socket.type,m_Socket.proto))==INVALID_SOCKET)
		GetWSALastError();
	else
		SetBufferSize();

	return(m_Socket.socket);
}

/*
	Connect()

	Collega il socket all'host/porta specificato.
*/
BOOL CSock::Connect(LPCSTR lpcszHost,UINT nPort)
{
	LPHOSTENT pHostEnt = (LPHOSTENT)NULL;
	SOCKADDR_IN sockaddr_in;
	int nWsaError = WSANOTINITIALISED;

	// controlla che il costruttore abbia terminato correttamente
	if(IsValid())
	{
		// ricava i dati relativi all'host
		if((pHostEnt = GetHostInfo(lpcszHost))!=(LPHOSTENT)NULL)
		{
			// imposta il socket (tipo, porta, indirizzo)
			sockaddr_in.sin_family = PF_INET;
			sockaddr_in.sin_port   = CWinsock::htons((u_short)nPort);
			sockaddr_in.sin_addr   = *((LPIN_ADDR)*pHostEnt->h_addr_list);

			// collega il socket
			if(CWinsock::connect(m_Socket.socket,(SOCKADDR *)&sockaddr_in,SOCKADDR_IN_LEN)==SOCKET_ERROR)
				GetWSALastError();
			else
				nWsaError = 0;
		}
	}
	else
		SetWSALastError(WSANOTINITIALISED);

	return(nWsaError==0);
}

/*
	Abort()

	Taglia le gambe socket.
*/
BOOL CSock::Abort(void)
{
	int nWsaError = WSANOTINITIALISED;

	// controlla che il costruttore abbia terminato correttamente
	if(IsValid())
	{
		// resetta e chiude il socket
		if(m_Socket.socket!=INVALID_SOCKET && m_Socket.status!=UNDEFINED)
		{
			LINGER stLinger;
			stLinger.l_onoff = TRUE;
			stLinger.l_linger = 1;
			CWinsock::setsockopt(m_Socket.socket,SOL_SOCKET,SO_LINGER,(const char FAR *)&stLinger,sizeof(LINGER));
			CWinsock::closesocket(m_Socket.socket);
			Reset();
			nWsaError = WSAECONNABORTED;
		}
	}

	// imposta il codice d'errore
	SetWSALastError(nWsaError);

	return(nWsaError==0);
}

/*
	Close()

	Chiude il socket.
*/
BOOL CSock::Close(void)
{
	int nWsaError = WSANOTINITIALISED;

	// controlla che il costruttore abbia terminato correttamente
	if(IsValid())
	{
		// chiude e resetta il socket
		if(m_Socket.socket!=INVALID_SOCKET && m_Socket.status!=UNDEFINED)
		{
			nWsaError = Shutdown();
			Reset();
		}
	}

	// imposta il codice d'errore
	SetWSALastError(nWsaError);

	return(nWsaError==0);
}

/*
	Shutdown()

	Sdraia il socket.
*/
int CSock::Shutdown(LPSTR lpszDiscard/*=NULL*/,int nSize/*=0*/)
{
	int nWsaError = 0;
	int remain = 1;
	int offset = 0;

	// disabilita il socket (solo in invio) per una chiusura morbida
	if(CWinsock::shutdown(m_Socket.socket,1)==SOCKET_ERROR)
		nWsaError = GetWSALastError();

	// legge gli eventuali dati rimanenti
	while(remain!=0 && remain!=SOCKET_ERROR)
	{
		// se non viene passato nessun buffer, scarta i dati pendenti
		if(lpszDiscard)
		{
			remain = CWinsock::recv(m_Socket.socket,&lpszDiscard[offset],nSize,0);
			if(remain!=0 && remain!=SOCKET_ERROR)
			{
				nSize -= remain;
				offset += remain;
			}
		}
		else
		{
			remain = SOCKET_ERROR;
		}
	}

	// resetta il codice d'errore
	SetWSALastError(0);

	// chiude il socket
	if(CWinsock::closesocket(m_Socket.socket)==SOCKET_ERROR)
		nWsaError = GetWSALastError();

	return(nWsaError);
}

/*
	GetIPAddress()

	Ricava l'ip relativo al socket.
*/
LPCSTR CSock::GetIPAddress(SOCKET socket/*=INVALID_SOCKET*/)
{
	unsigned char* ip;
	static char ip_addr[IP_ADDRESS_SIZE+1];
	SOCKADDR_IN sockaddr_in;
	int sockaddr_size = sizeof(SOCKADDR_IN);

	strcpyn(ip_addr,NULL_HOST,sizeof(ip_addr));

	// controlla che il costruttore abbia terminato correttamente
	if(IsValid())
	{
		// ricava l'ip del socket
		if(socket==INVALID_SOCKET)
			socket = m_Socket.socket;

		if(CWinsock::getsockname(socket,(SOCKADDR*)&sockaddr_in,&sockaddr_size)==0)
		{
			ip = (unsigned char*)&(sockaddr_in.sin_addr);
			_snprintf(ip_addr,sizeof(ip_addr)-1,"%i.%i.%i.%i",ip[0],ip[1],ip[2],ip[3]);
		}
		else
			GetWSALastError();
	}
	else
		SetWSALastError(WSANOTINITIALISED);

	return(ip_addr);
}

/*
	Listen()

	Crea e mette in ascolto il socket sulla porta specificata (versione bloccante).
*/
SOCKET CSock::Listen(UINT nPort)
{
	int nWsaError = WSANOTINITIALISED;
	SOCKADDR_IN sockaddr_in;
	int sockaddr_size = sizeof(SOCKADDR_IN);

	// controlla che il costruttore abbia terminato correttamente
	if(IsValid())
	{
		// controlla che il socket non sia gia' aperto
		if(m_Socket.socket==INVALID_SOCKET && m_Socket.status==UNDEFINED)
		{
			if(Create()!=INVALID_SOCKET)
			{
				// arpa/tcp/protocollo di default
				m_Socket.format = PF_INET;
				m_Socket.type   = SOCK_STREAM;
				m_Socket.proto  = 0;
				m_Socket.status = OPEN;

				// famiglia=ARPA, porta=[input], indirizzo=qualunque (INADDR_ANY, per cui bind() utilizza l'indirizzo TCP/IP del computer)
				sockaddr_in.sin_family           = PF_INET;
				sockaddr_in.sin_port             = (u_short)CWinsock::htons((u_short)nPort);
				sockaddr_in.sin_addr.S_un.S_addr = INADDR_ANY;

				// da' il nome al socket
				if(CWinsock::bind(m_Socket.socket,(SOCKADDR *)&sockaddr_in,sockaddr_size)==0)
				{
					// mette il socket in ascolto
					if(CWinsock::listen(m_Socket.socket,SOMAXCONN)==0)
						nWsaError = 0;
				}

				if(nWsaError!=0)
					GetWSALastError();
			}
		}
	}
	else
		SetWSALastError(WSANOTINITIALISED);
	
	return(nWsaError==0 ? m_Socket.socket : INVALID_SOCKET);
}

/*
	Accept()

	Accetta le connessioni sul socket in ascolto (versione bloccante).
*/
CSock* CSock::Accept(void)
{
	SOCKET socket;
	SOCKADDR_IN sockaddr_in;
	int sockaddr_size = sizeof(SOCKADDR_IN);
	CSock* pSocket = NULL;

	// controlla che il costruttore abbia terminato correttamente
	if(IsValid())
	{
		// accetta la connessione sul socket in ascolto
		if((socket = CWinsock::accept(m_Socket.socket,(SOCKADDR*)&sockaddr_in,&sockaddr_size))!=INVALID_SOCKET)
		{
			pSocket = new CSock();
			if(pSocket)
				pSocket->Init(socket,&sockaddr_in,OPEN);
		}
		else
		{
			GetWSALastError();
			pSocket = NULL;
		}
	}
	else
		SetWSALastError(WSANOTINITIALISED);

	return(pSocket);
}

/*
	CreateListenSocket()

	Crea e mette in ascolto il socket sulla porta specificata (versione asincrona/messaggi).
	Notare che la funzione crea un socket di tipo PF_INET(arpa)/SOCK_STREAM(tcp)/protocollo di default.
*/
#if defined(_WINDOWS)
SOCKET CSock::CreateListenSocket(HWND hWnd,UINT uMsg,UINT nPort)
{
	int nWsaError = WSANOTINITIALISED;
	SOCKADDR_IN sockaddr_in;
	int sockaddr_size = sizeof(SOCKADDR_IN);

	// controlla che il costruttore abbia terminato correttamente
	if(IsValid())
	{
		// controlla che il socket non sia gia' aperto
		if(m_Socket.socket==INVALID_SOCKET && m_Socket.status==UNDEFINED)
		{
			if(Create()!=INVALID_SOCKET)
			{
				// arpa/tcp/protocollo di default
				m_Socket.format = PF_INET;
				m_Socket.type   = SOCK_STREAM;
				m_Socket.proto  = 0;
				m_Socket.status = OPEN;

				// famiglia=ARPA, porta=[input], indirizzo=qualunque (INADDR_ANY, per cui bind() utilizza l'indirizzo TCP/IP del computer)
				sockaddr_in.sin_family           = PF_INET;
				sockaddr_in.sin_port             = (u_short)CWinsock::htons((u_short)nPort);
				sockaddr_in.sin_addr.S_un.S_addr = INADDR_ANY;

				// configura il socket per la modalita' asincrona e registra gli eventi da monitorare
				if(CWinsock::WSAAsyncSelect(m_Socket.socket,hWnd,uMsg,FD_CONNECT|FD_ACCEPT|FD_READ|FD_WRITE|FD_CLOSE)==0)
				{
					// da' il nome al socket
					if(CWinsock::bind(m_Socket.socket,(SOCKADDR *)&sockaddr_in,sockaddr_size)==0)
					{
						// mette il socket in ascolto
						if(CWinsock::listen(m_Socket.socket,SOMAXCONN)==0)
							nWsaError = 0;
					}
				}

				if(nWsaError!=0)
					GetWSALastError();
			}
		}
	}
	else
		SetWSALastError(WSANOTINITIALISED);
	
	return(nWsaError==0 ? INVALID_SOCKET : m_Socket.socket);
}
#endif

/*
	AcceptConnectingSocket()

	Accetta le connessioni sul socket in ascolto (versione asincrona/messaggi).
*/
#if defined(_WINDOWS)
CSock* CSock::AcceptConnectingSocket(HWND hWnd,UINT uMsg)
{
	SOCKET socket;
	SOCKADDR_IN sockaddr_in;
	int sockaddr_size = sizeof(SOCKADDR_IN);
	CSock* pSocket = NULL;

	// controlla che il costruttore abbia terminato correttamente
	if(IsValid())
	{
		// accetta la connessione sul socket in ascolto
		if((socket = CWinsock::accept(m_Socket.socket,(SOCKADDR*)&sockaddr_in,&sockaddr_size))!=INVALID_SOCKET)
		{
			// imposta gli eventi da monitorare
			CWinsock::WSAAsyncSelect(socket,hWnd,uMsg,FD_CONNECT|FD_ACCEPT|FD_READ|FD_WRITE|FD_CLOSE);
			
			pSocket = new CSock();
			if(pSocket)
				pSocket->Init(socket,&sockaddr_in,OPEN);
		}
		else
			GetWSALastError();
	}
	else
		SetWSALastError(WSANOTINITIALISED);

	return(pSocket);
}
#endif

/*
	Send()

	Invia i dati presenti nel buffer (interno) di output. Se non viene specificato,
	calcola automaticamente il numero di bytes da inviare (la dimensione del buffer).
*/
int CSock::Send(int nLen/*=-1*/)
{
	int nSent = SOCKET_ERROR;

	// controlla che il costruttore abbia terminato correttamente
	if(IsValid())
	{
		// calcola la dimensione del buffer
		// (se non viene passato il numero di bytes da inviare, il buffer deve essere terminato con \0)
		nLen = (nLen <= 0 ? strlen(m_szSendBuffer) : nLen);
		if(nLen > 0 && nLen <= WSA_BUF_SIZE)
		{
			// invia i dati
			if((nSent = CWinsock::send(m_Socket.socket,(const char FAR *)m_szSendBuffer,nLen,0))==SOCKET_ERROR)
				GetWSALastError();
		}
		else
			SetWSALastError(WSAENOBUFS);
	}
	else
		SetWSALastError(WSANOTINITIALISED);

	return(nSent);
}

/*
	Send()

	Invia il numero di bytes specificati presenti nel buffer.
*/
int CSock::Send(LPCSTR pData,int nLen)
{
	int nSent = SOCKET_ERROR;

	// controlla che il costruttore abbia terminato correttamente
	if(IsValid())
	{
		// invia i dati
		if((nSent = CWinsock::send(m_Socket.socket,(const char FAR *)pData,nLen,0))==SOCKET_ERROR)
			GetWSALastError();
	}
	else
		SetWSALastError(WSANOTINITIALISED);

	return(nSent);
}

/*
	WSASend()

	Invia il numero di bytes specificati presenti nel buffer.
	Non mappa le chiamate all'API tramite CWinsock.
*/
#if defined(_WINDOWS)
BOOL CSock::WSASend(LPBYTE pBuf,DWORD dwBufSize,HANDLE hExitHandle)
{
	WSABUF		wsabuf;
	WSAOVERLAPPED	over;
	DWORD		dwRecv;
	DWORD		dwFlags;
	DWORD		dwRet;
	HANDLE		hEvents[2];
	BOOL			fPending;
	int			nRet;

	// setup the WSABUF and WSAOVERLAPPED structures
	wsabuf.buf  = (char FAR*)pBuf;
	wsabuf.len  = dwBufSize;
	over.hEvent = ::WSACreateEvent();

	fPending = FALSE;
	nRet = ::WSASend(
				m_Socket.socket,	// socket
				&wsabuf,			// WSABUF
				1,				// number of buffers
				&dwRecv,			// bytes received
				0,				// flags
				&over,			// WSAOVERLAPPED
				NULL				// completion function
				);
	
	if(nRet!=0)
	{
		if(::WSAGetLastError()==WSA_IO_PENDING)
			fPending = TRUE;
		else
		{
			::CloseHandle(over.hEvent);
			return(FALSE);
		}
	}

	// if the I/O isn't finished...
	if(fPending)
	{
		// wait for the request to complete or the exit event to be signaled
		hEvents[0]  = over.hEvent;
		hEvents[1]  = hExitHandle;
		dwRet = ::WaitForMultipleObjects(
									2,
									hEvents,
									FALSE,
									INFINITE
									);

		// was the recv event signaled?
		if(dwRet!=0)
		{
			::CloseHandle(over.hEvent);
			return(FALSE);
		}

		// get I/O result
		if(!::WSAGetOverlappedResult(m_Socket.socket,&over,&dwRecv,FALSE,&dwFlags))
		{
			::CloseHandle(over.hEvent);
			return(FALSE);
		}
	}

	::CloseHandle(over.hEvent);
	
	return(TRUE);
}
#endif

/*
	Receive()

	Riceve il numero di bytes specificati nel buffer. Se non viene specificato, assume
	WSA_BUF_SIZE come numero di bytes da ricevere.
	La ricezione avviene nel buffer (interno) di input di cui restituisce il puntatore.
*/
const char* CSock::Receive(int* nRecv/*=NULL*/,int nLen/*=-1*/)
{
	int nReceived = SOCKET_ERROR;
	int nWsaError = WSANOTINITIALISED;

	// controlla che il costruttore abbia terminato correttamente
	if(IsValid())
	{
		// calcola il numero di bytes da ricevere
		// (se non viene passato il numero di bytes da ricevere, assume WSA_BUF_SIZE)
		nLen = (nLen <= 0 ? WSA_BUF_SIZE : nLen);
		if(nLen > 0 && nLen <= WSA_BUF_SIZE)
		{
			// riceve i dati
			if((nReceived = CWinsock::recv(m_Socket.socket,(char FAR *)m_szRecvBuffer,nLen,0))!=SOCKET_ERROR)
			{
				nWsaError = 0;
				m_szRecvBuffer[nReceived] = '\0';
			}
			else
			{
				nWsaError = GetWSALastError();
			}
		}
		else
			nWsaError = WSAENOBUFS;
	}
	else
		nWsaError = WSANOTINITIALISED;

	if(nWsaError!=0)
		SetWSALastError(nWsaError);

	if(nRecv)
		*nRecv = nReceived;

	return(nReceived!=SOCKET_ERROR ? m_szRecvBuffer : NULL);
}

/*
	Receive()

	Riceve il numero di bytes specificati nel buffer.
*/
int CSock::Receive(LPSTR pBuffer,int nLen)
{
	int nReceived = SOCKET_ERROR;

	// controlla che il costruttore abbia terminato correttamente
	if(IsValid())
	{
		// riceve i dati
		if((nReceived = CWinsock::recv(m_Socket.socket,(char FAR *)pBuffer,nLen,0))==SOCKET_ERROR)
			GetWSALastError();
		else
			pBuffer[nReceived] = '\0';
	}
	else
		SetWSALastError(WSANOTINITIALISED);

	return(nReceived);
}

/*
	GetLocalHostName()

	Restituisce il nome dell'host locale.
*/
LPCSTR CSock::GetLocalHostName(void)
{
	// controlla che il costruttore abbia terminato correttamente
	if(!IsValid())
	{
		SetWSALastError(WSANOTINITIALISED);
		return(NULL);
	}
	else
		return(m_szLocalHostName);
}

/*
	GetLocalHostAddr()

	Restituisce l'indirizzo ip dell'host locale.
*/
LPCSTR CSock::GetLocalHostAddr(void)
{
	// controlla che il costruttore abbia terminato correttamente
	if(!IsValid())
	{
		SetWSALastError(WSANOTINITIALISED);
		return(NULL);
	}
	else
		return(m_szLocalHostIP);
}

/*
	GetHostLocalName()

	Ricava il nome dell'host locale.
*/
LPCSTR CSock::GetHostLocalName(void)
{
	char* p = m_szLocalHostName;
	
	//  ricava il nome
	if(CWinsock::gethostname(m_szLocalHostName,sizeof(m_szLocalHostName))==SOCKET_ERROR)
	{
		GetWSALastError();
		memset(m_szLocalHostName,'\0',sizeof(m_szLocalHostName));
		p = NULL;
	}

	return(p);
}

/*
	GetHostLocalAddr()

	Ricava l'ip dell'host locale.
*/
LPCSTR CSock::GetHostLocalAddr(void)
{
	LPHOSTENT pHostEnt;
	SOCKADDR_IN sockaddr_in_local;
	SOCKADDR_IN sockaddr_in_remote;
	int sockaddr_size = sizeof(SOCKADDR);
	char* p = NULL;

	// inizializza l'indirizzo locale
	strcpy(m_szLocalHostIP,NULL_HOST);
	sockaddr_in_local.sin_addr.s_addr = INADDR_ANY;

	// ricava il nome
	if(CWinsock::gethostname(m_szLocalHostName,sizeof(m_szLocalHostName))!=SOCKET_ERROR)
	{
		// ricava le informazioni sull'host relative al nome
		if((pHostEnt = CWinsock::gethostbyname(m_szLocalHostName))!=(LPHOSTENT)NULL)
		{
			sockaddr_in_local.sin_addr.s_addr = *((u_long FAR*)(pHostEnt->h_addr));
			strncpy(m_szLocalHostIP,CWinsock::inet_ntoa(sockaddr_in_local.sin_addr),HOSTNAME_SIZE);
			p = m_szLocalHostIP;
		}
	} 

	// errore, prova con un socket fittizzio
	if(sockaddr_in_local.sin_addr.s_addr==INADDR_ANY)
	{
		SOCKET socket;

		// prova con un socket UDP
		if((socket = CWinsock::socket(AF_INET,SOCK_DGRAM,0))!=INVALID_SOCKET)
		{
			// collega ad un indirizzo fittizzio (non usare il loopback) */
			sockaddr_in_remote.sin_family		= AF_INET;
			sockaddr_in_remote.sin_port		= CWinsock::htons(IPPORT_ECHO);
			sockaddr_in_remote.sin_addr.s_addr	= CWinsock::inet_addr(DUMMY_HOST);

			if(CWinsock::connect(socket,(LPSOCKADDR)&sockaddr_in_remote,sizeof(SOCKADDR))!=SOCKET_ERROR)
			{
				// ricava l'indirizzo locale
				if(CWinsock::getsockname(socket,(LPSOCKADDR)&sockaddr_in_local,(int FAR*)&sockaddr_size)!=SOCKET_ERROR)
				{
					strncpy(m_szLocalHostIP,CWinsock::inet_ntoa(sockaddr_in_local.sin_addr),HOSTNAME_SIZE);
					p = m_szLocalHostIP;
				}
			}
			
			CWinsock::closesocket(socket);
		}
	}

	return(p);
}

/*
	GetHostByName()

	Ricava l'ip dell'host a partire del nome.
*/
LPCSTR CSock::GetHostByName(LPCSTR lpcszHost)
{
	LPHOSTENT pHostEnt;
	char* pHostAddr = NULL;
	
	if((pHostEnt = GetHostInfo(lpcszHost))!=(LPHOSTENT)NULL)
	{
		struct in_addr st_addr;
		st_addr.s_addr = *((u_long FAR*)(pHostEnt->h_addr));
		pHostAddr = CWinsock::inet_ntoa(st_addr);
	}

	return(pHostAddr);
}

/*
	GetHostByAddr()

	Ricava il nome dell' host partire dell'indirizzo ip.
*/
const char* CSock::GetHostByAddr(const char* lpcszHostAddr)
{
	LPHOSTENT pHostEnt;
	char* pHostName = NULL;
	
	if((pHostEnt = GetHostInfo(lpcszHostAddr))!=(LPHOSTENT)NULL)
		pHostName = pHostEnt->h_name;

	return(pHostName);
}

/*
	GetHostInfo()

	Ricava le informazioni relative all'host specificato, distinguendo se si sta' referenziando
	l'host locale (ad es. MiPC.medusa.es o 127.0.0.1) o meno.
	Notare che considera come host locale il nome del computer (MiPC.medusa.es) o quanto indicato
	dalla macro LOCAL_HOST (vedi lmhosts.h).
	L' host puo' essere specificato nei due formati: stringa (MiPC.medusa.es) o n.n.n.n (127.0.0.1).
	Il formato utilizzato per il socket e' PF_INET.
*/
LPHOSTENT CSock::GetHostInfo(LPCSTR lpcszHost)
{
	// valori per la struct HOSTENT statica
	static char* host_alias[] = {NULL,NULL};
	static struct in_addr st_addr;
	st_addr.S_un.S_addr = LOCAL_HOST_NUM;
	static char* host_names[] = {(char FAR *)&st_addr,NULL};
	static HOSTENT host_ent = {0};
	LPHOSTENT pHostEnt = (LPHOSTENT)NULL;

	// controlla che il costruttore abbia terminato correttamente
	if(!IsValid())
	{
		SetWSALastError(WSANOTINITIALISED);
		return((LPHOSTENT)NULL);
	}

	// se viene referenziato l'host locale collega sulla struttura interna
	if(stricmp(lpcszHost,m_szLocalHostName)==0 || stricmp(lpcszHost,host_names[0])==0 || stricmp(lpcszHost,LOCAL_HOST)==0)
	{
		host_ent.h_name      = m_szLocalHostName;
		host_ent.h_aliases   = host_alias;
		host_ent.h_addrtype  = PF_INET;
		host_ent.h_length    = 4;
		host_ent.h_addr_list = host_names;
		pHostEnt             = &host_ent;
	}
	else
	{
		unsigned long lAddr;

		/* "n.n.n.n" -> long */
		if((lAddr=CWinsock::inet_addr(lpcszHost))!=INADDR_NONE)
			pHostEnt = CWinsock::gethostbyaddr((const char FAR *)&lAddr,4,PF_INET); // n.n.n.n (long)
		else
			pHostEnt = CWinsock::gethostbyname((const char FAR *)lpcszHost); // "host.domain"
	}

	if(pHostEnt==(LPHOSTENT)NULL)
		GetWSALastError();

	return(pHostEnt);
}

/*
	GetServiceByName()

	Ricava il # della porta corrispondente al servizio.
	Prima di chiamare winsock controlla nel database (locale).
*/
int CSock::GetServiceByName(LPCSTR lpcszService,LPCSTR lpcszProto/*="tcp"*/,BOOL bUseLocalDabatase/*=TRUE*/)
{
	struct servent *se;
	int port = -1;

#if defined(_WINDOWS)
	// cerca il servizio nell'array
	if(bUseLocalDabatase)
	{
		if(m_pServicesList.Count() <= 0)
			LoadServices();

		ITERATOR iter;
		SERVICES* s;
		if((iter = m_pServicesList.First())!=(ITERATOR)NULL)
		{
			do
			{
				s = (SERVICES*)iter->data;
				
				if(stricmp(lpcszService,s->service)==0)
					if(stricmp(lpcszProto,s->proto)==0)
					{
						port = s->port;
						break;
					}

				iter = m_pServicesList.Next(iter);
			
			} while(iter!=(ITERATOR)NULL);
		}
	}
#endif

	// se non trova il servizio chiama winsock
	if(port < 0)
		if((se = CWinsock::getservbyname(lpcszService,lpcszProto))!=(struct servent *)NULL)
			port = (int)se->s_port;

	return(port);
}

/*
	GetServiceByPort()

	Ricava il nome del servizio corrispondente al # porta.
	Prima di chiamare winsock controlla nel database (locale).
*/
LPCSTR CSock::GetServiceByPort(UINT port,LPCSTR lpcszProto/*="tcp"*/,BOOL bUseLocalDabatase/*=TRUE*/)
{
	struct servent *se;
	char* p;
	static char name[SERVICE_NAME + 1];
	
	p = NULL;
	memset(name,'\0',sizeof(name));

#if defined(_WINDOWS)
	// cerca il servizio nell'array
	if(bUseLocalDabatase)
	{
		if(m_pServicesList.Count() <= 0)
			LoadServices();

		ITERATOR iter;
		SERVICES* s;
		if((iter = m_pServicesList.First())!=(ITERATOR)NULL)
		{
			do
			{
				s = (SERVICES*)iter->data;
				
				if(port==s->port)
					if(stricmp(lpcszProto,s->proto)==0)
					{
						strncpy(name,s->service,SERVICE_NAME+1);
						p = name;
						break;
					}

				iter = m_pServicesList.Next(iter);
			
			} while(iter!=(ITERATOR)NULL);
		}
	}
#endif

	// se non trova il servizio chiama winsock
	if(!p)
	{
		if((se=CWinsock::getservbyport(port,lpcszProto))!=(struct servent *)NULL)
		{
			strncpy(name,se->s_name,SERVICE_NAME);
			p = name;
		}
		else
			p = "unknown";
	}

	return(p);
}

/*
	LoadServices()

	Carica la lista con i servizi presenti nel database.
	Il formato del database e' il seguente:
	<service> <port>/<protocol> [aliases...] [#comment]
	Il carattere #, all'interno del file, viene usato come commento.
*/
#if defined(_WINDOWS)
int CSock::LoadServices(LPCSTR filename/*=NULL*/)
{
	int i;
	int read = 0;
	CTextFile services_file;
	char *token;
	char buffer[SERVICE_NAME+PORT_NAME+PROTOCOL_NAME+COMMENT_NAME+1];
	char service[SERVICE_NAME+1];
	char port_number[PORT_NAME+1];
	char protocol[PROTOCOL_NAME+1];
	char comment[COMMENT_NAME+1];
	char database[(_MAX_PATH*2)+1];
	SERVICES* s;
	ITERATOR iter;

	if(!filename)
	{
		DWORD dwVersion = ::GetVersion();
		DWORD dwWindowsMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
		char szWindowsDir[_MAX_PATH+1];
		::GetWindowsDirectory(szWindowsDir,sizeof(szWindowsDir)-1);

		// Windows NT
		if(dwVersion < 0x80000000)
			_snprintf(database,sizeof(database)-1,"%s\\system32\\drivers\\etc\\services",szWindowsDir);
		// Win32s
		else if(dwWindowsMajorVersion < 4)
			strcpy(database,"services");
		// Windows 95
		else
			_snprintf(database,sizeof(database)-1,"%s\\services",szWindowsDir);
	}
	else
	{
		strcpyn(database,filename,sizeof(database));
	}
 
	// apre il file dei servizi caricandolo nell'array
 	if(services_file.Open(database))
	{
		while((read = services_file.ReadLine(buffer,sizeof(buffer)-1))!=FILE_EOF)
		{
			// salta i commenti e le linee vuote
			if(buffer[0]=='#' || isspace(buffer[0]) || buffer[0]=='\r' || read==0)
				continue;

			memset(service,'\0',sizeof(service));
			memset(port_number,'\0',sizeof(port_number));
			memset(protocol,'\0',sizeof(protocol));
			memset(comment,'\0',sizeof(comment));

			if((token = strchr(buffer,'#'))!=NULL)
			{
				token++;

				while(*token && isspace(*token))
					token++;

				for(i = 0; i < sizeof(comment) && *token && !isspace(*token); i++)
					comment[i] = *token++;
			}

			for(i = 0,token = strtok(buffer," \t/"); token!=NULL; i++)
			{
				switch(i)
				{
					case 0:
						strcpyn(service,token,sizeof(service));
						break;
					case 1:
						strcpyn(port_number,token,sizeof(port_number));
						break;
					case 2:
						strcpyn(protocol,token,sizeof(protocol));
						break;
					default:
						break;
				}

				token = strtok(NULL," \t/");
			}
			
			BOOL found = FALSE;

			// controlla che il servizio non esista gia'
			if((iter = m_pServicesList.First())!=(ITERATOR)NULL)
			{
				do
				{
					s = (SERVICES*)iter->data;
					if(stricmp(s->service,service)==0)
					{
						found = TRUE;
						break;
					}

					iter = m_pServicesList.Next(iter);
				} while(iter!=(ITERATOR)NULL);
			}
			
			// carica il servizio
			if(!found)
			{
				s = new SERVICES(service,atoi(port_number),protocol,comment[0]!='\0' ? comment : " ");
				if(s)
					m_pServicesList.Add(s);
			}
		}

		services_file.Close();
	}

	return(m_pServicesList.Count());
}
#endif

/*
	ParseIPRange()

	Restituisce gli indirizzi ip compresi nell'intervallo.
*/
LPCSTR CSock::ParseIPRange(LPCSTR start_addr,LPCSTR end_addr)
{
	char* pHostAddr = NULL;
	static BOOL first_call = TRUE;
	static IP_RANGE ip_range = {0};

	if(first_call)
	{
		unsigned int a,b,c,d;
		char ip_start_addr[IP_ADDRESS_SIZE+1],ip_end_addr[IP_ADDRESS_SIZE+1];

		if(ValidateIPAddr(start_addr))
			strncpy(ip_start_addr,start_addr,IP_ADDRESS_SIZE);
		else
		{
			if((pHostAddr = (char*)GetHostByName(start_addr))!=NULL)
				strncpy(ip_start_addr,pHostAddr,IP_ADDRESS_SIZE);
			else
				goto done;
		}

		if(ValidateIPAddr(end_addr))
			strncpy(ip_end_addr,end_addr,IP_ADDRESS_SIZE);
		else
		{
			if((pHostAddr = (char*)GetHostByName(end_addr))!=NULL)
				strncpy(ip_end_addr,pHostAddr,IP_ADDRESS_SIZE);
			else
				goto done;
		}
		
		strcpy(ip_range.ip_start,ip_start_addr);
		strcpy(ip_range.ip_end,ip_end_addr);
		
		TokenizeIPAddr(ip_start_addr,a,b,c,d);
		ip_range.a_start = a;
		ip_range.b_start = b;
		ip_range.c_start = c;
		ip_range.d_start = d;
		
		TokenizeIPAddr(ip_end_addr,a,b,c,d);
		ip_range.a_end = a;
		ip_range.b_end = b;
		ip_range.c_end = c;
		ip_range.d_end = d;

		first_call = FALSE;
	}

	pHostAddr = (char*)GetIPFromRange(&ip_range);

	if(!pHostAddr)
	{
		first_call = TRUE;
		memset(&ip_range,'\0',sizeof(IP_RANGE));
	}

done:

	return(pHostAddr);
}

/*
	ValidateIPAddr()

	Controlla la validita' (formale) dell'indirizzo ip.
*/
BOOL CSock::ValidateIPAddr(const char* ip_address)
{
	char* token;
	char* ip;
	int ip_number,ip_len;
	char ip_addr[IP_ADDRESS_SIZE + 1];
	int i = -1;

	strncpy(ip_addr,ip_address,IP_ADDRESS_SIZE);
		
	for(token = strtok(ip_addr,"."); token!=NULL; token = strtok(NULL,"."))
	{
		ip = token;
		ip_len = strlen(ip);

		for(i = 0; i < ip_len; i++)
		{
			if(!isdigit(ip[i]))
			{
				i = -1;
				break;
			}
		}

		if(i < 0)
			break;
		
		ip_number = atoi(ip);

		if(ip_number < 0 || ip_number > 255)
		{
			i = -1;
			break;
		}
	}

	return(i >= 0);
}

/*
	TokenizeIPAddr()

	Scompone l'indirizzo ip ("127.0.0.1") ricavando i valori numerici relativi (a=127, b=0, c=0, d=1).
*/
void CSock::TokenizeIPAddr(const char* ip_address,unsigned int& a,unsigned int& b,unsigned int& c,unsigned int& d)
{
	int i;
	char* token;
	char ip_addr[IP_ADDRESS_SIZE + 1];

	strncpy(ip_addr,ip_address,IP_ADDRESS_SIZE);

	token = strtok(ip_addr,".");
	
	for(i = 0; token!=NULL; i++)
	{
		switch(i)
		{
			case 0:
				a = atoi(token);
				break;
			case 1:
				b = atoi(token);
				break;
			case 2:
				c = atoi(token);
				break;
			case 3:
				d = atoi(token);
				break;
		}
		
		token = strtok(NULL,".");
	}
}

/*
	GetIPFromRange()

	Restituisce l'indirizzo ip successivo (relativamente all'intervallo specificato).
*/
const char* CSock::GetIPFromRange(IP_RANGE *ip)
{
	#define CHECK_IP(f,a,b,c,d) {if(a==b) f = (c < d); else f = (c < (int)256);}
	#define CHECK_CLASS(a) {if(a > (int)255) a = 0;}
	
	BOOL next_class = FALSE;
	static char ip_addr[IP_ADDRESS_SIZE + 1];
	static BOOL first_call = TRUE;

	if(first_call)
	{
		first_call = FALSE;

		CHECK_CLASS(ip->a_start);
		CHECK_CLASS(ip->b_start);
		CHECK_CLASS(ip->c_start);
		CHECK_CLASS(ip->d_start);
		
		_snprintf(ip_addr,sizeof(ip_addr)-1,"%d.%d.%d.%d",ip->a_start,ip->b_start,ip->c_start,ip->d_start);
		
		return(ip_addr);
	}

d_1:

	CHECK_IP(next_class,ip->c_start,ip->c_end,ip->d_start,ip->d_end);

	if(next_class)
	{
		ip->d_start++;

		CHECK_CLASS(ip->a_start);
		CHECK_CLASS(ip->b_start);
		CHECK_CLASS(ip->c_start);
		CHECK_CLASS(ip->d_start);
		
		_snprintf(ip_addr,sizeof(ip_addr)-1,"%d.%d.%d.%d",ip->a_start,ip->b_start,ip->c_start,ip->d_start);
		
		return(ip_addr);
	}

// c_1:

	CHECK_IP(next_class,ip->b_start,ip->b_end,ip->c_start,ip->c_end);

	if(next_class)
	{
		ip->d_start = -1;
		ip->c_start++;
		goto d_1;
	}

// b_1:

	CHECK_IP(next_class,ip->a_start,ip->a_end,ip->b_start,ip->b_end);

	if(next_class)
	{
		ip->d_start = -1;
		ip->c_start = 0;
		ip->b_start++;
		goto d_1;
	}

// a_1:

	if(ip->a_start < ip->a_end)
	{
		ip->d_start = -1;
		ip->c_start = 0;
		ip->b_start = 0;
		ip->a_start++;
		goto d_1;
	}

	first_call = TRUE;

	return(NULL);
}

/*
	IsWSAError()

	Controlla se il codice fa riferimento ad un errore.
*/
BOOL CSock::IsWSAError(int iWSAError)
{
	BOOL bFlag = FALSE;
	register int i;

	// cerca il codice d'errore nell'array
	for(i = 0; i < WSAERRORCODE_ARRAY_SIZE ;i++)
		if(wsa_errorcode_num[i]==iWSAError)
		{
			bFlag = TRUE;
			break;
		}

	return(bFlag);
}

/*
	ShowErrors()

	Imposta il flag per la visualizzazione degli errori.
*/
#if defined(_WINDOWS)
void CSock::ShowErrors(BOOL bFlag)
{
	// controlla che il costruttore abbia terminato correttamente
	if(IsValid())
		m_bShowErrors = bFlag;
	else
		SetWSALastError(WSANOTINITIALISED);
}
#endif

/*
	GetWSAErrorNumber()

	Restituisce l'ultimo codice d'errore numerico WSA.
*/
const int CSock::GetWSAErrorNumber(void)
{
	return(m_WsaData.error);
}

/*
	GetWSAErrorString()

	Restituisce la descrizione relativa all'ultimo codice d'errore WSA.
*/
const char* CSock::GetWSAErrorString(void)
{
	return(m_WsaData.errorstr);
}

/*
	GetWSALastError()

	Ricava l'ultimo errore winsock, impostando di conseguenza i campi relativi della struttura
	del socket e dei dati wsa.
*/
int CSock::GetWSALastError(int iWSAError)
{
	register int i;

	// ricava il codice d'errore
	if(iWSAError==WSA_GETLASTERROR)
	{
		if((iWSAError = CWinsock::WSAGetLastError())==0)
			iWSAError = WSA_SUCCESS;
	}

	if(iWSAError!=WSA_SUCCESS)
	{
		// imposta il codice d'errore (numerico)
		m_Socket.error = m_WsaData.error = iWSAError;

		/* cerca il codice d'errore nell'array */
		for(i=0; i < WSAERRORCODE_ARRAY_SIZE ;i++)
			if(wsa_errorcode_num[i]==m_WsaData.error)
				break;

		// ricava il codice d'errore (stringa)
		_snprintf(m_WsaData.errorstr,sizeof(m_WsaData.errorstr)-1,"%s (%d)",wsa_errorcode_str[(i >= WSAERRORCODE_ARRAY_SIZE ? WSAERRORCODE_ARRAY_SIZE-1 : i)],m_WsaData.error);

#if defined(_WINDOWS)
		// visualizza l'errore
		if(m_bShowErrors)
#ifdef WIN32_MFC
			::MessageBox(m_pParent ? m_pParent->m_hWnd : NULL,m_WsaData.errorstr,"CSock::GetWSALastError()",MB_OK|MB_ICONERROR|(m_pParent ? MB_APPLMODAL : MB_TASKMODAL)|MB_SETFOREGROUND|MB_TOPMOST);
#else
			::MessageBox(m_hWnd,m_WsaData.errorstr,"CSock::GetWSALastError()",MB_OK|MB_ICONERROR|MB_APPLMODAL|MB_SETFOREGROUND|MB_TOPMOST);
#endif
#endif
	}
	else
	{
		m_Socket.error = m_WsaData.error = iWSAError = 0;
		strcpy(m_WsaData.errorstr,"");
	}

	return(iWSAError);
}

/*
	SetWSALastError()

	Imposta il codice d'errore winsock, impostando di conseguenza i campi relativi della struttura
	del socket e dei dati wsa.
*/
int CSock::SetWSALastError(int iWSAError)
{
	// imposta il codice d'errore (numerico)
	m_Socket.error = m_WsaData.error = iWSAError;

	if(iWSAError > 0)
	{
		register int i;

		// cerca il codice d'errore nell'array
		for(i = 0; i < WSAERRORCODE_ARRAY_SIZE ;i++)
			if(wsa_errorcode_num[i]==m_WsaData.error)
				break;

		// imposta il codice d'errore (stringa)
		_snprintf(m_WsaData.errorstr,sizeof(m_WsaData.errorstr)-1,"%s (%d)",wsa_errorcode_str[(i >= WSAERRORCODE_ARRAY_SIZE ? WSAERRORCODE_ARRAY_SIZE-1 : i)],m_WsaData.error);
	}
	else
		strcpy(m_WsaData.errorstr,"");
	
	CWinsock::WSASetLastError(iWSAError);

	return(iWSAError);
}
