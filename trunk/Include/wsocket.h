/*
	wsocket.h
	Interfaccia Winsock.
	Luca Piergentili, 06/07/96
	l.piergentili@ifies.es
*/
#ifndef _WSOCKET_H
#define _WSOCKET_H 1

/* headers */
#define STRICT 1
#include <windows.h>
#include <winsock.h>
#if defined(_WIN32)
  #pragma comment(lib,"WSOCK32")			/* crea la referenza alla libreria per il linker */
  #pragma PRAGMA_MESSAGE(__FILE__": including WSOCK32.LIB")
#endif
#include <lmcons.h>						/* interfaccia LAN manager, per macro ??LEN */
#include "lmhosts.h"					/* indirizzo ip host locale */
#include "macro.h"						/* STR() */

/* interfaccia C++ */
#ifdef  __cplusplus
  extern "C" {
#endif

/* messaggio per WSAAsyncSelect() */
#define WM_WSAASYNC			990
#define WM_WSAASYNCREJECT	991

/* codici WSA */
#define WSA_SUCCESS			WSABASEERR-1
#define WSA_FAILURE			WSABASEERR-2
#define WSA_GETLASTERROR		WSABASEERR-3	/* WSAPerror() */
#define WSA_UNKNOWERROR		WSABASEERR-4

/* WSAStartup() */
#ifndef MAKEWORD
  #define MAKEWORD(l,h)		((WORD)(((BYTE)(l))|(((WORD)(BYTE)(h))<<8)))
#endif
#define WSA_MAJOR_VERSION	1
#define WSA_MINOR_VERSION	1
#define WSA_VERSION			MAKEWORD(WSA_MAJOR_VERSION,WSA_MINOR_VERSION)

/* dimensione buffer I/O */
#define WSA_BUF_SIZE		8192
#pragma PRAGMA_MESSAGE(__FILE__": WSA buffer size set to "STR(WSA_BUF_SIZE)" bytes")

/* dimensioni campi */
#define USER_MAX_LEN		UNLEN+1		/* dim. max. nome utente */
#define PSW_MAX_LEN			PWLEN+1		/* dim. max. password */
#define EMAIL_LEN			128			/* dim. max per indirizzo email */
#define IPADDR_SIZE			16			/* dim. max per stringa indirizzo ip */
#define HOSTNAME_SIZE		128			/* dim. max per nome host */

/*
	SOCKETSTATUS
	tipo per lo status delle operazioni di I/O sul socket
*/
typedef enum socket_status_t {
	WAITING,
	PENDING,
	DONE
} SOCKETSTATUS;

/*
	SOCKET_EXT
	struttura per il socket
*/
typedef struct socket_ext_t {
	SOCKET socket;						/* handle del socket */
	int error;						/* codice d'errore */
} SOCKET_EXT,*LPSOCKET_EXT;

/*
	SOCKET_EXT_LEN
	dimensione della struttura per il socket
*/
#define SOCKET_EXT_LEN sizeof(SOCKET_EXT)

/*
	SOCKADDR_LEN
	dimensione della struttura per l'ip
*/
#define SOCKADDR_LEN sizeof(struct sockaddr)

/*
	FPWSAPERROR
	tipo per il puntatore a funzione richiesto da WSAPerror()
*/
typedef void (* FPWSAPERROR)(LPCSTR);

/*
	HOSTENT
	tipo per la struttura per il collegamento all'host
*/
typedef struct hostent HOSTENT,*LPHOSTENT;

/* prototipi */
int		WSA_Perror			(int);
#define	WSAPERROR(n)			WSA_Perror(n)
LPCSTR	GetWSAErrorString		(int);
BOOL		InitializeSocket		(FPWSAPERROR);
void		ShutdownSocket			(void);
SOCKET	CreateSocket			(SOCKET_EXT *);
SOCKET	CreateSocketExt		(SOCKET_EXT *,int,int,int);
SOCKET	CreateListenSocket		(SOCKET_EXT *,HWND,UINT,UINT);
SOCKET	ConnectSocket			(SOCKET_EXT *,LPCSTR,UINT);
LPHOSTENT GetHostByName			(LPCSTR);
SOCKET	AcceptConnectingSocket	(SOCKET_EXT *,HWND,UINT);
void		SetSocketBufferSize		(SOCKET,UINT);
int		SocketSend			(SOCKET_EXT *,LPVOID,int);
int		SocketReceive			(SOCKET_EXT *,LPVOID,int);
int		CloseConnection		(SOCKET_EXT *,LPSTR,int,HWND);
#define	CloseSocket(s)			CloseConnection(s,(LPSTR)NULL,0,(HWND)NULL)

/* interfaccia C++ */
#ifdef  __cplusplus
  }
#endif

#endif /* _WSOCKET_H */
