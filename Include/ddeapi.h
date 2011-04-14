/*
	ddeapi.h
	Luca Piergentili, 06/09/98
	l.piergentili@ifies.es

	Interfaccia DDE.
*/
#ifndef _DDEAPI_H
#define _DDEAPI_H 1

/* headers */
#define STRICT 1
#include <windows.h>
#include <dde.h>
#include "macro.h"

/* blocchi */
#define DDE_BLOCK_SIGN		"LPI"					/* signature del blocco */
#define DDE_BLOCK_SIGN_LEN	sizeofmacro(DDE_BLOCK_SIGN)	/* dimensione signature */
#define DDE_USED_BLOCK		1						/* flag per blocco in uso */
#define DDE_UNUSED_BLOCK		0						/* flag per blocco inutilizzato */

/* dimensioni campi x interfaccia DDE */
#define DDE_MAX_APP			256						/* nome applicazione */
#define DDE_MAX_TOPIC		256						/* argomento */
#define DDE_MAX_ITEM		256						/* item */
#define DDE_MAX_VALUE		4096						/* valore */

/* timeout per risposta DDE (30.000msec.=30sec.) */
#define DDE_TIMEOUT			30000

/* nome classe/finestra */
#define DDE_CLASS_NAME		"DDEClientClass"
#define DDE_WINDOW_NAME		"DDEClientWindow"

/* macro per test risposta server */
#define DDE_ACK(w)			((w) & 0x8000)
#define DDE_NACK(w)			(!((w) & 0x8000))

/* codici d'errore */
#define DDE_OK				0
#define DDE_ERROR			1
#define DDE_NACK_ERROR		2
#define DDE_TIMEOUT_ERROR	3

/* formato dati */
#define DDE_CF_FORMAT		CF_TEXT

/*
	PENDINGACK
	enum per i possibili valori di stato
*/
typedef enum pending_ack_t {
	UNDEFINED,
	ADVISE,
	UNADVISE,
	POKE,
	REQUEST,
	EXECUTE,
	TERMINATE
} PENDINGACK;

/*
	DDECONV
	struttura per il colloquio DDE
*/
typedef struct dde_conv_t {
	char	szSign[DDE_BLOCK_SIGN_LEN];	/* signature del blocco */
	int	iStatus;					/* status del blocco (in uso/disponibile)*/
	UINT	uTimerId;					/* id per il timer */
	PENDINGACK ePendingAck;			/* status della conversazione DDE (vedi PENDINGACK) */
	HWND	hWndClient;				/* handle client della conversazione */
	HWND	hWndServer;				/* handle server della conversazione */
	UINT uError;					/* codice d'errore */
	char	szApplication[DDE_MAX_APP+1];	/* nome applicazione (server DDE) */
	char	szTopic[DDE_MAX_TOPIC+1];	/* argomento conversazione */
	char	szDdeValue[DDE_MAX_VALUE];	/* valore */
	struct dde_conv_t *next;			/* ptr. all'elemento successivo */
} DDECONV,*LPDDECONV;

/*
	CONV
	struttura interna per la gestione (sincrona) dei colloqui DDE
*/
typedef struct conv_t {
	BOOL bInit;					/* flag per WM_DDE_INITIATE */
	BOOL bRequest;					/* flag per WM_DDE_REQUEST */
	BOOL bPoke;					/* flag per WM_DDE_POKE */
	HINSTANCE hInstance;			/* handle istanza del chiamante */
	HWND hWnd;					/* handle finestra del chiamante */
	LPDDECONV	lpConvFirst;			/* ptr. al primo elemento dell'array per le conv. */
	LPDDECONV lpConvLast;			/* ptr. all'ultimo elemento dell'array per le conv. */
} CONV,*LPCONV;

/* pubbliche */
BOOL					Dde_Init			(HINSTANCE,HWND);
BOOL					Dde_End			(void);
HWND					Dde_Connect		(LPCSTR,LPCSTR);
BOOL					Dde_Disconnect		(HWND);
BOOL					Dde_Request		(HWND,LPCSTR);
BOOL					Dde_Poke			(HWND,LPCSTR,LPCSTR);
LPSTR				Dde_Data			(HWND);
void					Dde_ClearData		(HWND);
UINT					Dde_LastError		(HWND);
LRESULT CALLBACK EXPORT	Dde_WndProc		(HWND,UINT,WPARAM,LPARAM);

/* private */
#ifdef _DDEAPI_C
static BOOL			WaitForEnter		(BOOL *,UINT);
static BOOL			WaitForLeave		(HWND,BOOL *,UINT);
static void			AcknowledgeConv	(HWND,HWND,LONG,BOOL);
static void			ReceiveConvData	(HWND,HWND,LONG);
static void			TerminateConv		(HWND,HWND);
static HWND			SendInitiate		(LPCSTR,LPCSTR);
static void			SendTerminate		(HWND,HWND);
static void			SendRequest		(HWND,HWND,LPCSTR);
static void			SendPoke			(HWND,HWND,LPCSTR,LPCSTR);
static void			SetConvError		(HWND,UINT);
static UINT			GetConvError		(HWND);
static void			SetConvTimer		(HWND);
static void			KillConvTimer		(HWND);
static LPDDECONV		CreateConv		(HWND,HWND,LPCSTR,LPCSTR);
static void			InitializeConv		(LPDDECONV,HWND,HWND,LPCSTR,LPCSTR,int);
static void			InsertConv		(LPDDECONV);
static BOOL			CheckConv			(LPDDECONV);
static LPDDECONV		FindConvByHandle	(HWND);
static LPDDECONV		FindConvByHandles	(HWND,HWND);
static LPDDECONV		FindUnusedConv		(void);
static BOOL			ReleaseConv		(LPDDECONV);
static int			ReleaseConvList	(void);
static void			SetConvPendingAck	(HWND,PENDINGACK);
static PENDINGACK		GetConvPendingAck	(HWND);
static HWND			GetDdeServerHandle	(HWND);
#endif

#endif /* _DDEAPI_H */
