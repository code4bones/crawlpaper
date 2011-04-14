/*
	CWinsock.h
	Classe base per interfaccia Winsock (CRT/SDK/MFC).
	Luca Piergentili, 15/07/96
	lpiergentili@yahoo.com
*/
#ifndef _CWINSOCK_H
#define _CWINSOCK_H 1

#include "macro.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "window.h"
#include "CWinsock.config.h"

#ifdef _DEBUGSOCKET
   #pragma message("\t\t\t"__FILE__"("STR(__LINE__)"): warning: the _DEBUGSOCKET has been defined and all Winsock services will be mapped to local host")
   #include "CDateTime.h"
#endif

/*
	_WINSOCK_2
	definire per usare Winsock2
*/
#if defined(_WINDOWS)
   #define _WINSOCK_2 1
#endif

#ifdef _WINSOCK_2
#ifdef PRAGMA_MESSAGE_VERBOSE
   #pragma message("\t\t\t"__FILE__"("STR(__LINE__)"): all Winsock services will be mapped to Winsock 2 DLL version 2.2.x")
#endif
   #include <winsock2.h>
   #ifndef _DEBUGSOCKET
      #pragma comment(lib,"WS2_32.lib")
#ifdef PRAGMA_MESSAGE_VERBOSE
      #pragma message("\t\t\t"__FILE__"("STR(__LINE__)"): automatically linking with WS2_32.dll")
#endif
   #endif
#else
#ifdef PRAGMA_MESSAGE_VERBOSE
   #pragma message("\t\t\t"__FILE__"("STR(__LINE__)"): all Winsock services will be mapped to Windows Sockets specification version 1.1")
#endif
   #include <winsock.h>
   #ifndef _DEBUGSOCKET
      #pragma comment(lib,"WSock32.lib")
#ifdef PRAGMA_MESSAGE_VERBOSE
      #pragma message("\t\t\t"__FILE__"("STR(__LINE__)"): automatically linking with WSock32.dll")
#endif
   #endif
#endif

#include "lmhosts.h"

/*
	CWinsock
*/
class CWinsock
{
#ifdef _DEBUGSOCKET
	public:
		CWinsock();
		virtual ~CWinsock();

		SOCKET						accept		(SOCKET s,struct sockaddr FAR* addr,int FAR* addrlen) {return(INVALID_SOCKET);}
		int							bind			(SOCKET s,const struct sockaddr FAR* addr,int namelen) {return(SOCKET_ERROR);}
		int PASCAL FAR					connect		(SOCKET,const struct sockaddr FAR*,int);
		int FAR PASCAL					closesocket	(SOCKET);
		struct hostent FAR*				gethostbyaddr	(const char FAR*,int,int);
		struct hostent FAR* PASCAL FAR	gethostbyname	(const char FAR*);
		int PASCAL FAR					gethostname	(char FAR*,int);
		struct servent FAR*				getservbyname	(const char FAR*,const char FAR*);
		struct servent FAR*				getservbyport	(int,const char FAR*);
		int							getsockname	(SOCKET s,struct sockaddr FAR* name,int FAR* len);
		u_short PASCAL FAR				htons		(u_short);
		unsigned long PASCAL FAR			inet_addr		(const char FAR*);
		char FAR*						inet_ntoa		(struct in_addr);
		int							listen		(SOCKET s,int backlog) {return(SOCKET_ERROR);}
		int PASCAL FAR					recv			(SOCKET,char FAR*,int,int);
		int PASCAL FAR					send			(SOCKET,const char FAR*,int,int);
		int							setsockopt	(SOCKET,int,int,const char FAR*,int);
		int							shutdown		(SOCKET,int);
		SOCKET PASCAL FAR				socket		(int,int,int);
		int							WSAAsyncSelect	(SOCKET s,HWND hWnd,u_int wMsg,long lEvent) {return(SOCKET_ERROR);}
		int PASCAL FAR					WSACleanup	(void);
		int PASCAL FAR					WSAGetLastError(void);
		void PASCAL FAR				WSASetLastError(int);
		int PASCAL FAR					WSAStartup	(WORD,LPWSADATA);	
	private:
		int							send_to_host	(const char FAR*,int);
		int							recv_from_host	(char FAR*,int);

		int							iWSALastError;			// ultimo errore WSA
		int							iWSAFlag;				// flag per inizializzazione avvenuta
		HFILE						hFile;				// handle del file utilizzato per il socket
		SOCKET						hSocket;				// handle del socket
		char FAR*						pHostName;			// nome host
		char FAR*						pHostIp;				// indirizzo ip
		int							iService;				// flag per servizio richiesto (SMTP/POP3)
		int							iCommand;				// indice per comandi
		char*						pHostLastSend;			// buffer per dati ricevuti
		char*						pHostLastRecv;			// buffer per dati inviati
		char*						pPop3Buffer;			// puntatore al buffer per lettura file
		BOOL							bRetr;				// flag per invio dati
		int							hMailBox;				// handle del file utilizzato per la mailbox
		char*						pMailBox;				// nome della mailbox (email)
		int							iTotMail;				// totale email
		int*							iDelArray;			// array per email da eliminare
		int							iDelIndex;			// indice per l'array
		CDateTime						date;
#else
	public:
		CWinsock() {}
		virtual ~CWinsock() {}

		SOCKET						accept		(SOCKET s,struct sockaddr FAR *addr,int FAR *addrlen)				{return(::accept(s,addr,addrlen));}
		int							bind			(SOCKET s,const struct sockaddr FAR *addr,int namelen)				{return(::bind(s,addr,namelen));}
		int PASCAL FAR					connect		(SOCKET s,const struct sockaddr FAR *name,int namelen)				{return(::connect(s,name,namelen));}
		int FAR PASCAL					closesocket	(SOCKET s)												{return(::closesocket(s));}
		struct hostent FAR*				gethostbyaddr	(const char FAR *addr,int len,int type)							{return(::gethostbyaddr(addr,len,type));}
		struct hostent FAR* PASCAL FAR	gethostbyname	(const char FAR *name)										{return(::gethostbyname(name));}
		int PASCAL FAR					gethostname	(char FAR *name,int namelen)									{return(::gethostname(name,namelen));}
		struct servent FAR*				getservbyname	(const char FAR *name,const char FAR *proto)						{return(::getservbyname(name,proto));}
		struct servent FAR*				getservbyport	(int port,const char FAR *proto)								{return(::getservbyport(port,proto));}
		int							getsockname	(SOCKET s,struct sockaddr FAR *name,int FAR *len)					{return(::getsockname(s,name,len));}
		u_short PASCAL FAR				htons		(u_short hostshort)											{return(::htons(hostshort));}
		unsigned long PASCAL FAR			inet_addr		(const char FAR *cp)										{return(::inet_addr(cp));}
		char FAR*						inet_ntoa		(struct in_addr in)											{return(::inet_ntoa(in));}
		int							listen		(SOCKET s,int backlog)										{return(::listen(s,backlog));}
		int PASCAL FAR					recv			(SOCKET s,char FAR *buf,int len,int flags)						{return(::recv(s,buf,len,flags));}
		int PASCAL FAR					send			(SOCKET s,const char FAR *buf,int len,int flags)					{return(::send(s,buf,len,flags));}
		int							setsockopt	(SOCKET s,int level,int optname,const char FAR *optval,int optlen)	{return(::setsockopt(s,level,optname,optval,optlen));}
		int							shutdown		(SOCKET s,int how)											{return(::shutdown(s,how));}
		SOCKET PASCAL FAR				socket		(int af,int type,int protocol)								{return(::socket(af,type,protocol));}
		int							WSAAsyncSelect	(SOCKET s,HWND hWnd,u_int wMsg,long lEvent)						{return(::WSAAsyncSelect(s,hWnd,wMsg,lEvent));}
		int PASCAL FAR					WSACleanup	(void)													{return(::WSACleanup());}
		int PASCAL FAR					WSAGetLastError(void)													{return(::WSAGetLastError());}
		void PASCAL FAR				WSASetLastError(int iError)												{::WSASetLastError(iError);}
		int PASCAL FAR					WSAStartup	(WORD wVersionRequested,LPWSADATA lpWSAData)						{return(::WSAStartup(wVersionRequested,lpWSAData));}
#endif
};

#endif // _CWINSOCK_H
