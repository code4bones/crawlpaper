/*
	http.h
	Luca Piergentili, 06/07/98
	l.piergentili@ifies.es

	Interfaccia HTTP.
*/
#ifndef _HTTP_H
#define _HTTP_H 1

/* headers */
#define STRICT 1
#include <windows.h>
#include "winplat.h"
#include "wsocket.h"		/* per dimensioni buffer, email, host, etc. */
#include "lmhosts.h"		/* per LOCAL_HOST */
#include "wtime.h"

/*
	CONTENT
	struttura per le direttive Content-Type
	il nome della direttiva non puo' superare i CONTENT_MAX_NAME caratteri ed il valore
	i CONTENT_MAX_EXT caratteri
*/
#define CONTENT_MIN			16
#define CONTENT_MED			64
#define CONTENT_MAX			128
#define CONTENT_MAX_NAME		32
#define CONTENT_MAX_EXT		64
#define CONTENT_TYPE_LEN		(CONTENT_MAX_NAME+1+CONTENT_MAX_EXT+1)

typedef struct content_t {
	char	szContent[CONTENT_MAX_NAME];		/* nome Content-Type */
	char	szExt[CONTENT_MAX_EXT];			/* estensioni associate */
} CONTENT,*LPCONTENT;

/*
	FORMLITERAL
	struttura per i campi/valori presenti nel FORM inviato via POST
	il nome del campo non puo' superare i FORMLITERAL_MAX_NAME caratteri ed il valore i
	FORMLITERAL_MAX_VALUE caratteri
	la dimensione totale dei campi presenti nel .ini all'interno della sezione non puo'
	superare i FORMLITERAL_MAX_SECTION caratteri
*/
#define FORMLITERAL_SECTION_NAME	"Form Literal"	/* nome della sezione */
#define FORMLITERAL_MAX_SECTION	WSA_BUF_SIZE	/* dimensione max. sezione [Form Literal] del .ini */
#define FORMLITERAL_MAX_NAME		64			/* dimensione max. per nome campo */
#define FORMLITERAL_MAX_VALUE		256			/* dimensione max. per valore campo */

typedef struct formliteral_t {
	char	szName[FORMLITERAL_MAX_NAME];			/* nome del campo */
	char	szValue[FORMLITERAL_MAX_VALUE];		/* valore */
} FORMLITERAL,*LPFORMLITERAL;

/* generiche */
#define HTTP_PORT			80
#define HTTP_LOCAL_HOST		LOCAL_HOST
#define HTTP_HEADER_SIZE		1024
#define HTTP_ARGS_SIZE		1024
#define HTTP_QUERY_SIZE		1024
#define HTTP_COMMAND_SIZE	16
#define HTTP_VERSION_09		"HTTP/0.9"
#define HTTP_VERSION_10		"HTTP/1.0"
#define HTTP_SUCCESS		1965
#define HTTP_FAILURE		1981
#define HTTP_UNKNOWERROR		1995

/*
	HTTPSTATUS
	definisce i codici HTTP standard:
	100 = info
	200 = success
	300 = redirection
	400 = client error
	500 = server error
*/
typedef enum _http_status_t {
	HTTP_CGIREQUEST=101,
	HTTP_OK=200,
	HTTP_CREATED=201,
	HTTP_ACCEPTED=202,
	HTTP_NOCONTENT=204,
	HTTP_MOVEDPERM=301,
	HTTP_MOVEDTEMP=302,
	HTTP_NOTMODIFIED=304,
	HTTP_BADREQUEST=400,
	HTTP_UNAUTHORIZED=401,
	HTTP_FORBIDDEN=403,
	HTTP_NOTFOUND=404,
	HTTP_INTERNALERROR=500,
	HTTP_NOTIMPL=501,
	HTTP_BADGATEWAY=502,
	HTTP_SERVICEUNAV=503
} HTTPSTATUS;

/*
	HTTPCOMMAND
	enum per i comandi
*/
typedef enum http_command_t {
	HEAD=1,								/* usare 0 per inizializzare */
	GET,
	POST,
	POSTHEADER,
	POSTBODY
} HTTPCOMMAND;

/*
	HTTPERROR
	struttura per associare i .html ai codici d'errore HTTP
*/
typedef struct http_error_t {
	HTTPSTATUS eCode;					/* codice HTTP */
	char	szHtml[MAX_FNAME];				/* nome file .html relativo al codice */
} HTTPERROR,*LPHTTPERROR;

/*
	HTTPHDRFIELDS
	struttura per i campi dell'header HTTP
*/
typedef struct http_headerfields_t {
	int	iIndex;						/* indice dell'array */
	char	szField[CONTENT_MAX_NAME];			/* direttiva */
	int	iLen;						/* dimensione della direttiva + 1 */
	int	iSize;						/* dimensione del campo della struttura */
} HTTPHDRFIELDS,*LPHTTPHDRFIELDS;

/*
	HTTPHEADER
	struttura per i campi dell'header HTTP
*/
typedef struct http_header_t {
	/* interni */
	int	iHdrLen;							/* dimensione dell'header */
	HTTPSTATUS http_status;					/* codice HTTP per la risposta */
	char	szFile			[MAX_PATH];		/* oggetto richiesto (utilizzato per copiare il nome originale quando si invia un .html differente) */

	/* RFC 1945 */
	char	szAllow			[CONTENT_MED];		/* comandi supportati */
	char	szAuthorization	[CONTENT_MAX];		/* autenticazione */
	char	szContentEncoding	[CONTENT_TYPE_LEN];	/* formato codifica dell'oggetto */
	DWORD dwContentLength;					/* dimensione dell'oggetto */
	char	szContentType		[CONTENT_TYPE_LEN];	/* tipo dell'oggetto */
	char	szDate			[GMT_DATE_SIZE];	/* data GMT di sistema */
	char	szExpire			[GMT_DATE_SIZE];	/* data GMT di scadenza dell'oggetto */
	char	szFrom			[EMAIL_LEN];		/* identita' mittente (email) */
	char	szIfModifiedSince	[GMT_DATE_SIZE];	/* data GMT ultima modifica per invio */
	char	szLastModified		[GMT_DATE_SIZE];	/* data GMT ultima modifica dell'oggetto */
	char	szLocation		[MAX_FNAME];		/* ubicazione della risorsa */
	char	szPragma			[CONTENT_MAX];		/* implementazioni */
	char	szReferer			[HOSTNAME_SIZE];	/* indirizzo di provenienza */
	char	szServer			[CONTENT_MAX];		/* nome dell'applicazione server */
	char	szUserAgent		[CONTENT_MAX];		/* nome dell'applicazione client */
	char	szWWWAuthenticate	[CONTENT_MAX];		/* autenticazione */

	/* RFC 1945 - estensioni */
	char	szAccept			[CONTENT_MAX];		/* tipi accettati */
	char	szAcceptCharset	[CONTENT_MED];		/* */
	char	szAcceptEncoding	[CONTENT_MED];		/* */
	char	szAcceptLanguage	[CONTENT_MED];		/* */
	char	szContentLanguage	[CONTENT_MED];		/* */
	char	szConnection		[CONTENT_MED];		/* modalita' connessione */
	char	szHost			[HOSTNAME_SIZE];	/* */
	char	szLink			[MAX_FNAME];		/* */
	char	szMimeVersion		[CONTENT_MIN];		/* versione MIME */
	char	szRetryAfter		[CONTENT_MIN];		/* */
	char	szTitle			[MAX_FNAME];		/* */
	char	szUri			[MAX_FNAME];		/* */

	char	szHeader			[HTTP_HEADER_SIZE];	/* buffer per header HTTP */
} HTTPHEADER,*LPHTTPHEADER;

/* prototipi */
LPCSTR Http_Perror(HTTPSTATUS);

#endif /* _HTTP_H */
