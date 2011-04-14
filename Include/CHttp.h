/*
	CHttp.h
	Classi per la gestione del protocollo http.
	Luca Piergentili, 05/10/00
	lpiergentili@yahoo.com
*/
#ifndef _CHTTP_H
#define _CHTTP_H 1

#include "typedef.h"
#include "window.h"
#include "win32api.h"
#include <stdio.h>
#include <stdlib.h>
#include "strings.h"
#include "CNodeList.h"
#include "CSync.h"
#include "CThread.h"
#include "CDateTime.h"
#include "CSock.h"
#include "CListeningSock.h"
#include "CConnectedSock.h"

/*
	HOSTNAME
	struttura per i dati dell'host
*/
struct HOSTNAME {
	char name[HOSTNAME_SIZE+1];
};

/*
	CHostNameList
	classe per la lista degli host
*/
class CHostNameList : public CNodeList
{
public:
	CHostNameList() : CNodeList() {}
	virtual ~CHostNameList() {CNodeList::DeleteAll();}
	void* Create(void)
	{
		return(new HOSTNAME);
	}
	void* Initialize(void* pVoid)
	{
		HOSTNAME* pData = (HOSTNAME*)pVoid;
		if(!pData)
			pData = (HOSTNAME*)Create();
		if(pData)
			memset(pData,'\0',sizeof(HOSTNAME));
		return(pData);
	}
	BOOL PreDelete(ITERATOR iter)
	{
		if((HOSTNAME*)iter->data)
			delete ((HOSTNAME*)iter->data),iter->data = (HOSTNAME*)NULL;
		return(TRUE);
	}
	int Size(void) {return(sizeof(HOSTNAME));}
#ifdef _DEBUG
	const char* Signature(void) {return("CHostNameList");}
#endif
};

/*
	PROXY
	struttura per i dati del proxy (ip/nome host + numero porta)
*/
struct PROXY {
	char	address	[HOSTNAME_SIZE+1];
	UINT	port;
	char user		[MAX_USER_LEN+1];
	char password	[MAX_PASS_LEN+1];
	char auth		[MAX_USER_LEN+1+MAX_PASS_LEN+1];
	CHostNameList exclude;
};

/*
	COOKIE
	struttura per i dati del cookie
*/
#define	COOKIE_NAME_LEN		_MAX_PATH
#define	COOKIE_VALUE_LEN		1020
#define	COOKIE_EXPIRES_LEN		MAX_DATE_STRING
#define	COOKIE_DOMAIN_LEN		HOSTNAME_SIZE
#define	COOKIE_PATH_LEN		_MAX_PATH
#define	COOKIE_MAX_LEN			(COOKIE_NAME_LEN + COOKIE_VALUE_LEN + COOKIE_EXPIRES_LEN + COOKIE_DOMAIN_LEN + COOKIE_PATH_LEN)
//#define	COOKIE_REALLY_MAX_LEN	4096 // Netscape

struct COOKIE {
	char	name		[COOKIE_NAME_LEN+1];
	char	value	[COOKIE_VALUE_LEN+1];
	char	expires	[COOKIE_EXPIRES_LEN+1];
	char	path		[COOKIE_PATH_LEN+1];
	char	domain	[COOKIE_DOMAIN_LEN+1];
	int	secure;
};

/*
	CCookiesList
	classe per la lista dei cookies
*/
class CCookiesList : public CNodeList
{
public:
	CCookiesList() : CNodeList() {}
	virtual ~CCookiesList() {CNodeList::DeleteAll();}
	void* Create(void)
	{
		return(new COOKIE);
	}
	void* Initialize(void* pVoid)
	{
		COOKIE* pData = (COOKIE*)pVoid;
		if(!pData)
			pData = (COOKIE*)Create();
		if(pData)
			memset(pData,'\0',sizeof(COOKIE));
		return(pData);
	}
	BOOL PreDelete(ITERATOR iter)
	{
		if((COOKIE*)iter->data)
			delete ((COOKIE*)iter->data),iter->data = (COOKIE*)NULL;
		return(TRUE);
	}
	int Size(void) {return(sizeof(COOKIE));}
#ifdef _DEBUG
	const char* Signature(void) {return("CCookiesList");}
#endif
};

// directory logica per i files di sistema (da mappare con una fisica)
#define SYSDIR	"sys"

/*
	HTTP
	definizioni relative al protocollo
	vedi: http://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html#sec10
*/
#define HTTP_DEFAULT_PORT					80
#define HTTP_SUPPORTED_VERSION				"1.0"
#define MIME_SUPPORTED_VERSION				"1.0"

#define HTTP_CODE_VOID						0
#define HTTP_CODE_SUCCESSFUL					299
// successful 2xx 
#define HTTP_CODE_OK						200
#define HTTP_CODE_CREATED					201
#define HTTP_CODE_ACCEPTED					202
#define HTTP_CODE_NON_AUTH_INFO				203
#define HTTP_CODE_NO_CONTENT					204
#define HTTP_CODE_RESET_CONTENT				205
#define HTTP_CODE_PARTIAL_CONTENTS				206
#define HTTP_CODE_REDIRECTION					399
// redirection 3xx
#define HTTP_CODE_MULTIPLE_CHOICES				300
#define HTTP_CODE_MOVED_PERMANENTLY			301
#define HTTP_CODE_MOVED_TEMPORARILY			302
#define HTTP_CODE_SEE_OTHER					303
#define HTTP_CODE_NOT_MODIFIED				304
#define HTTP_CODE_USE_PROXY					305
#define HTTP_CODE_TEMPORARY_REDIRECT			307
#define HTTP_CODE_CLIENT_ERROR				499
// client error 4xx
#define HTTP_CODE_BAD_REQUEST					400
#define HTTP_CODE_UNAUTHORIZED				401
#define HTTP_CODE_PAYMENT_REQUIRED				402
#define HTTP_CODE_FORBIDDEN					403
#define HTTP_CODE_NOT_FOUND					404
#define HTTP_CODE_METHOD_NOT_ALLOWED			405
#define HTTP_CODE_NOT_ACCEPTABLE				406
#define HTTP_CODE_PROXY_AUTH_REQUIRED			407
#define HTTP_CODE_REQUEST_TIMEOUT				408
#define HTTP_CODE_CONFLICT					409
#define HTTP_CODE_GONE						410
#define HTTP_CODE_LENGTH_REQUIRED				411
#define HTTP_CODE_PRECONDITION_FAILED			412
#define HTTP_CODE_REQUEST_TOO_LARGE			413
#define HTTP_CODE_REQUEST_TOO_LONG				414
#define HTTP_CODE_UNSUPPORTED_MEDIA_TYPE		415
#define HTTP_CODE_RANGE_NOT_SATISFIABLE			416
#define HTTP_CODE_EXPECTATION_FAILED			417
#define HTTP_CODE_SERVER_ERROR				599
// server error 5xx
#define HTTP_CODE_INTERNAL					500
#define HTTP_CODE_NOT_IMPLEMENTED				501
#define HTTP_CODE_BAD_GATEWAY					502	
#define HTTP_CODE_SERVICE_UNAVAILABLE			503
#define HTTP_CODE_GATEWAY_TIMEOUT				504
#define HTTP_CODE_VERSION_NOT_SUPPORTED			505
#define HTTP_CODE_PERMISSION_DENIED			550
#define HTTP_CODE_SERVER_INTERNAL_ERROR			899
// interni
#define HTTP_CODE_BELOW_MIN_SIZE				900
#define HTTP_CODE_ABOVE_MAX_SIZE				901
#define HTTP_CODE_UNABLE_TO_CREATE_LOCAL_FILE	902
#define HTTP_CODE_EMPTY_DIRECTORY				903
#define HTTP_CODE_UNKNOW						1001

/*
	HTTP_STATUS
	codici di ritorno http (RFC1945 + utente) - lasciare UNKNOWN come ultimo per il calcolo dell'indice
	mantenere allineato con quanto presente in CHttp.cpp
*/
enum HTTP_STATUS {
	HTTP_STATUS_VOID						= HTTP_CODE_VOID,

	HTTP_STATUS_SUCCESSFUL					= HTTP_CODE_SUCCESSFUL,
	// successful 2xx 
	HTTP_STATUS_OK							= HTTP_CODE_OK,
	HTTP_STATUS_CREATED						= HTTP_CODE_CREATED,
	HTTP_STATUS_ACCEPTED					= HTTP_CODE_ACCEPTED,
	HTTP_STATUS_NON_AUTH_INFO				= HTTP_CODE_NON_AUTH_INFO,
	HTTP_STATUS_NO_CONTENT					= HTTP_CODE_NO_CONTENT,
	HTTP_STATUS_RESET_CONTENT				= HTTP_CODE_RESET_CONTENT,
	HTTP_STATUS_PARTIAL_CONTENTS				= HTTP_CODE_PARTIAL_CONTENTS,

	HTTP_STATUS_REDIRECTION					= HTTP_CODE_REDIRECTION,
	// redirection 3xx
	HTTP_STATUS_MULTIPLE_CHOICES				= HTTP_CODE_MULTIPLE_CHOICES,
	HTTP_STATUS_MOVED_PERMANENTLY				= HTTP_CODE_MOVED_PERMANENTLY,
	HTTP_STATUS_MOVED_TEMPORARILY				= HTTP_CODE_MOVED_TEMPORARILY,
	HTTP_STATUS_SEE_OTHER					= HTTP_CODE_SEE_OTHER,
	HTTP_STATUS_NOT_MODIFIED					= HTTP_CODE_NOT_MODIFIED,
	HTTP_STATUS_USE_PROXY					= HTTP_CODE_USE_PROXY,
	HTTP_STATUS_TEMPORARY_REDIRECT			= HTTP_CODE_TEMPORARY_REDIRECT,

	HTTP_STATUS_CLIENT_ERROR					= HTTP_CODE_CLIENT_ERROR,
	// client error 4xx
	HTTP_STATUS_BAD_REQUEST					= HTTP_CODE_BAD_REQUEST,
	HTTP_STATUS_UNAUTHORIZED					= HTTP_CODE_UNAUTHORIZED,
	HTTP_STATUS_PAYMENT_REQUIRED				= HTTP_CODE_PAYMENT_REQUIRED,
	HTTP_STATUS_FORBIDDEN					= HTTP_CODE_FORBIDDEN,
	HTTP_STATUS_NOT_FOUND					= HTTP_CODE_NOT_FOUND,
	HTTP_STATUS_METHOD_NOT_ALLOWED			= HTTP_CODE_METHOD_NOT_ALLOWED,
	HTTP_STATUS_NOT_ACCEPTABLE				= HTTP_CODE_NOT_ACCEPTABLE,
	HTTP_STATUS_PROXY_AUTH_REQUIRED			= HTTP_CODE_PROXY_AUTH_REQUIRED,
	HTTP_STATUS_REQUEST_TIMEOUT				= HTTP_CODE_REQUEST_TIMEOUT,
	HTTP_STATUS_CONFLICT					= HTTP_CODE_CONFLICT,
	HTTP_STATUS_GONE						= HTTP_CODE_GONE,
	HTTP_STATUS_LENGTH_REQUIRED				= HTTP_CODE_LENGTH_REQUIRED,
	HTTP_STATUS_PRECONDITION_FAILED			= HTTP_CODE_PRECONDITION_FAILED,
	HTTP_STATUS_REQUEST_TOO_LARGE				= HTTP_CODE_REQUEST_TOO_LARGE,
	HTTP_STATUS_REQUEST_TOO_LONG				= HTTP_CODE_REQUEST_TOO_LONG,
	HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE			= HTTP_CODE_UNSUPPORTED_MEDIA_TYPE,
	HTTP_STATUS_RANGE_NOT_SATISFIABLE			= HTTP_CODE_RANGE_NOT_SATISFIABLE,
	HTTP_STATUS_EXPECTATION_FAILED			= HTTP_CODE_EXPECTATION_FAILED,

	HTTP_STATUS_SERVER_ERROR					= HTTP_CODE_SERVER_ERROR,
	// server error 5xx
	HTTP_STATUS_INTERNAL					= HTTP_CODE_INTERNAL,
	HTTP_STATUS_NOT_IMPLEMENTED				= HTTP_CODE_NOT_IMPLEMENTED,
	HTTP_STATUS_BAD_GATEWAY					= HTTP_CODE_BAD_GATEWAY,
	HTTP_STATUS_SERVICE_UNAVAILABLE			= HTTP_CODE_SERVICE_UNAVAILABLE,
	HTTP_STATUS_GATEWAY_TIMEOUT				= HTTP_CODE_GATEWAY_TIMEOUT,
	HTTP_STATUS_VERSION_NOT_SUPPORTED			= HTTP_CODE_VERSION_NOT_SUPPORTED,
	HTTP_STATUS_PERMISSION_DENIED				= HTTP_CODE_PERMISSION_DENIED,

	HTTP_STATUS_SERVER_INTERNAL_ERROR			= HTTP_CODE_SERVER_INTERNAL_ERROR,
	// interni
	HTTP_STATUS_BELOW_MIN_SIZE				= HTTP_CODE_BELOW_MIN_SIZE,
	HTTP_STATUS_ABOVE_MAX_SIZE				= HTTP_CODE_ABOVE_MAX_SIZE,
	HTTP_STATUS_UNABLE_TO_CREATE_LOCAL_FILE		= HTTP_CODE_UNABLE_TO_CREATE_LOCAL_FILE,
	HTTP_STATUS_EMPTY_DIRECTORY				= HTTP_CODE_EMPTY_DIRECTORY,
	HTTP_STATUS_UNKNOW						= HTTP_CODE_UNKNOW
};

enum HTTPSTATUSLEN {
	HTTP_STATUS_LEN = ((int)HTTP_STATUS_UNKNOW+1)
};

#define IS_HTTP_CODE(n)			(((n) >= HTTP_STATUS_OK) && ((n) <= HTTP_STATUS_SERVICE_UNAVAILABLE))
#define IS_HTTP_ERROR(n)			((n) > HTTP_STATUS_NOT_MODIFIED)
#define IS_HTTP_INTERNAL_ERROR(n)	((n) > HTTP_STATUS_SERVER_INTERNAL_ERROR)

/*
	HTTPREQUEST
	struttura per la richiesta http
*/
#define	HTTP_MAX_REQUEST		1024
#define	HTTP_MAX_COMMAND		32
#define	HTTP_MAX_FILENAME		_MAX_FILEPATH
#define	HTTP_MAX_URL			512
#define	HTTP_MAX_CONTENT		128
#define	HTTP_MAX_CONTENT_EXT	32
#define	HTTP_MAX_USERAGENT		128

struct HTTPREQUEST {
	DWORD		start;							// GetTickCount() inizio elaborazione
	DWORD		end;								// GetTickCount() fine elaborazione
	char			received		[HTTP_MAX_CONTENT+1];	// data/ora richiesta
	char			request		[HTTP_MAX_REQUEST+1];	// dati ricevuti
	int			requestlength;						// lunghezza dati ricevuti
	char			command		[HTTP_MAX_COMMAND+1];	// comando http
	char			url			[HTTP_MAX_URL+1];		// url richiesta
	char			version		[HTTP_MAX_CONTENT+1];	// versione http
	char			referer		[HTTP_MAX_URL+1];		// url di provenienza
	char			useragent		[HTTP_MAX_USERAGENT+1];	// nome client
	char			filename		[HTTP_MAX_FILENAME+1];	// nome file relativo all'url
	char			ifmodifiedsince[HTTP_MAX_CONTENT+1];	// data/ora ultimo accesso al file richiesto
	QWORD		ifmodifiedlength;					// dimensione file durante ultimo accesso
	char			lastmodified	[HTTP_MAX_CONTENT+1];	// data/ora ultimo accesso al file richiesto
	char			contenttype	[HTTP_MAX_CONTENT+1];	// tipo mime
	QWORD		contentlen;						// dimensione file richiesto
	char			cookiename	[COOKIE_NAME_LEN+1];	// nome cookie
	char			cookievalue	[COOKIE_VALUE_LEN+1];	// valore cookie
	char			response		[HTTP_MAX_REQUEST+1];	// header risposta
	int			code;							// codice http per la risposta
	char			htmlfile		[HTTP_MAX_FILENAME+1];	// nome file html usato per risposta (ad es. per listato dir)
	SOCKETINFO	socketinfo;						// info sul socket in collegamento
	CCookiesList	cookies;							// lista per i cookies
};

/*
	CONTENTTYPE
	struttura per il tipo mime relativo al file
	
	http://en.wikipedia.org/wiki/MIME (email content type system)
	http://en.wikipedia.org/wiki/Internet_media_type (World Wide Web content type system)
	http://www.iana.org/assignments/media-types/index.html (IANA MIME Media Types)
	
	Content Types list:
	application
	audio
	example
	image
	message
	model
	multipart
	text
	video
*/
struct CONTENTTYPE {
	char	ext[HTTP_MAX_CONTENT_EXT+1];
	char content[HTTP_MAX_CONTENT+1];
};

/*
	CContentTypeList
	classe per la lista dei tipi mime relativi ai file
*/
class CContentTypeList : public CNodeList
{
public:
	CContentTypeList() : CNodeList() {}
	virtual ~CContentTypeList() {CNodeList::DeleteAll();}
	void* Create(void)
	{
		return(new CONTENTTYPE);
	}
	void* Initialize(void* pVoid)
	{
		CONTENTTYPE* pData = (CONTENTTYPE*)pVoid;
		if(!pData)
			pData = (CONTENTTYPE*)Create();
		if(pData)
			memset(pData,'\0',sizeof(CONTENTTYPE));
		return(pData);
	}
	BOOL PreDelete(ITERATOR iter)
	{
		if((CONTENTTYPE*)iter->data)
			delete ((CONTENTTYPE*)iter->data),iter->data = (CONTENTTYPE*)NULL;
		return(TRUE);
	}
	int Size(void) {return(sizeof(CONTENTTYPE));}
#ifdef _DEBUG
	const char* Signature(void) {return("CContentTypeList");}
#endif
};

/*
	HTTPALIASES
	struttura per l'aliasing delle directory
*/
#define	HTTPALIASES_URL_LEN		HTTP_MAX_URL
#define	HTTPALIASES_PATH_LEN	_MAX_PATH

struct HTTPALIASES {
	char url [HTTPALIASES_URL_LEN+1];
	char path[HTTPALIASES_PATH_LEN+1];
};

/*
	CHttpAliasesList
	classe per la lista per l'aliasing delle directory
*/
class CHttpAliasesList : public CNodeList
{
public:
	CHttpAliasesList() : CNodeList() {}
	virtual ~CHttpAliasesList() {CNodeList::DeleteAll();}
	void* Create(void)
	{
		return(new HTTPALIASES);
	}
	void* Initialize(void* pVoid)
	{
		HTTPALIASES* pData = (HTTPALIASES*)pVoid;
		if(!pData)
			pData = (HTTPALIASES*)Create();
		if(pData)
			memset(pData,'\0',sizeof(HTTPALIASES));
		return(pData);
	}
	BOOL PreDelete(ITERATOR iter)
	{
		if((HTTPALIASES*)iter->data)
			delete ((HTTPALIASES*)iter->data),iter->data = (HTTPALIASES*)NULL;
		return(TRUE);
	}
	int Size(void) {return(sizeof(HTTPALIASES));}
#ifdef _DEBUG
	const char* Signature(void) {return("CHttpAliasesList");}
#endif
};

/*
	HTTPSERVICE
	struttura per i campi relativi al servizio http
*/
class CHttpExtension;
struct HTTPSERVICE {
	int				port;					// porta di default
	char				domain	[HOSTNAME_SIZE+1];	// indirizzo dominio (127.0.0.1)
	char				server	[HOSTNAME_SIZE+1];	// nome server (SAMBAR 4.4)
	char				rootdir	[_MAX_PATH+1];		// pathname per la root directory (*non* terminare con \)
	char				uploaddir	[_MAX_PATH+1];		// pathname per la directory per gli upload (POST) (*non* terminare con \)
	char				sysdir	[_MAX_PATH+1];		// pathname per la directory di sistema (*non* terminare con \)
	char				logdir	[_MAX_PATH+1];		// pathname per la directory di log (*non* terminare con \)
	char				listdir	[_MAX_PATH+1];		// pathname per la directory per i listati (*non* terminare con \)
	char				defaulthtml[_MAX_PATH+1];	// nome file .html di default (se non viene abilitato il listato delle directories)
	DWORD			flags;					// flags per opzioni
	CContentTypeList	listContentType;			// lista per i tipi mime relativi ai file
	CHttpAliasesList	listHttpAliases;			// lista per l'aliasing delle directory
	CCookiesList		listCookies;				// lista per i cookies
	CHttpExtension*	pHttpExtension;			// puntatore alla classe per l'estensione del servizio http
};

/*
	CHttpExtension
	
	Classe per l'estensione del servizio http (lato server).
	Non utilizzare direttamente ma derivare ed implementare le funzioni necessarie.
*/
class CHttpExtension
{
public:
	CHttpExtension() {}
	virtual ~CHttpExtension() {}

	virtual BOOL CreateDirectoryListing(HTTPSERVICE* /*pHttpService*/,HTTPREQUEST* /*pHttpRequest*/,int* /*pHttpCode*/) {return(TRUE);}
	virtual UINT ParseHeader(HTTPSERVICE* /*pHttpService*/,HTTPREQUEST* /*pHttpRequest*/) {return(HTTP_STATUS_OK);}
};

#define HTTPSERVICE_FLAG_DELETE_LISTING		0x00000001
#define HTTPSERVICE_FLAG_REQUEST_LOG		0x00000002
#define HTTPSERVICE_FLAG_ACCESS_LOG		0x00000004
#define HTTPSERVICE_FLAG_PARENT_DIR		0x00000008
#define HTTPSERVICE_FLAG_DIRECTORY_LISTING	0x00000010
/*
#define HTTPSERVICE_FLAG_			0x00000010
#define HTTPSERVICE_FLAG_			0x00000020
#define HTTPSERVICE_FLAG_			0x00000040
#define HTTPSERVICE_FLAG_			0x00000080
*/

/*
	HTTPOBJECTSTAT
	enum per gli stati dell'oggetto HTTP
*/
enum HTTPOBJECTSTAT {
	HTTPOBJECTSTAT_VOID = 0,				// lasciare a 0 in modo tale che per resettare HTTPOBJECT sia sufficente una memset
	HTTPOBJECTSTAT_BEGIN_RECEIVING = 1,	// IDOK, IDCANCEL
	HTTPOBJECTSTAT_RECEIVING = 2,			// IDOK, IDCANCEL
	HTTPOBJECTSTAT_END_RECEIVING = 3,		// IDOK, IDCANCEL
	HTTPOBJECTSTAT_SET_COOKIE = 4,		// IDYES/IDOK, IDNO, IDCANCEL
	HTTPOBJECTSTAT_GET_COOKIE = 5,		// IDYES/IDOK, IDNO, IDCANCEL
	HTTPOBJECTSTAT_LOCAL_FILENAME = 6,		// IDOK, IDCANCEL
	HTTPOBJECTSTAT_DEFAULT_HTMLFNAME = 7,	// IDOK, IDCANCEL
	HTTPOBJECTSTAT_DEFAULT_MP3FNAME = 8,	// IDOK, IDCANCEL
	HTTPOBJECTSTAT_SHOUTCAST_STREAM = 9,	// IDOK, IDCANCEL
	HTTPOBJECTSTAT_ICECAST_STREAM = 10		// IDOK, IDCANCEL
};

/*
	HTTPOBJECT
	struttura per l'oggetto http in collegamento
*/
struct HTTPOBJECT {
	HTTPOBJECTSTAT stat;				// status
	char		url[HTTP_MAX_URL+1];		// url completa
	char		filename[_MAX_FILEPATH+1];	// nome del file locale
	char		defaultfname[_MAX_FNAME+1];	// nome di default per il file locale
	char		type[HTTP_MAX_CONTENT+1];	// content-type
	QWORD	amount;					// bytes ricevuti
	QWORD	size;					// dimensione totale
	DWORD	starttime;				// inizio download (in millisecondi)
	DWORD	endtime;					// fine download (in millisecondi)
	COOKIE	cookie;					// cookie
	WPARAM	wParam;					// per porcherie varie
	LPARAM	lParam;					// come sopra
};

/*
	HTTPHEADER
	struttura per i campi dell'header http
*/
#define	HTTPHEADER_NAME_LEN		32
#define	HTTPHEADER_VALUE_LEN	_MAX_PATH

struct HTTPHEADER {
	char name	[HTTPHEADER_NAME_LEN+1];
	char value[HTTPHEADER_VALUE_LEN+1];
};

/*
	HTTP_THREAD_PARAMS
	struttura per il passaggio dei parametri
*/
class CListeningHttpSocket;
struct HTTP_THREAD_PARAMS {
	CListeningHttpSocket* pListenSocket;
	SOCKET hConnectedSocket;
	HANDLE hEvent;
};

/*
	CHttpHeaderList
	classe per la lista dei campi dell'header http
*/
class CHttpHeaderList : public CNodeList
{
public:
	CHttpHeaderList() : CNodeList() {}
	virtual ~CHttpHeaderList() {CNodeList::DeleteAll();}
	void* Create(void)
	{
		return(new HTTPHEADER);
	}
	void* Initialize(void* pVoid)
	{
		HTTPHEADER* pData = (HTTPHEADER*)pVoid;
		if(!pData)
			pData = (HTTPHEADER*)Create();
		if(pData)
			memset(pData,'\0',sizeof(HTTPHEADER));
		return(pData);
	}
	BOOL PreDelete(ITERATOR iter)
	{
		if((HTTPHEADER*)iter->data)
			delete ((HTTPHEADER*)iter->data),iter->data = (HTTPHEADER*)NULL;
		return(TRUE);
	}
	int Size(void) {return(sizeof(HTTPHEADER));}
#ifdef _DEBUG
	const char* Signature(void) {return("CHttpHeaderList");}
#endif
};

/*
	CConnectedHttpSocket

	Classe per la gestione del colloquio con i socket in collegamento (lato server).
	La classe definisce le funzioni necessarie per la gestione del colloquio http con i socket che si collegano
	al socket in ascolto sul lato server (implementato tramite la CListeningHttpSocket).
*/
class CConnectedHttpSocket : public CConnectedSocket
{
public:
	CConnectedHttpSocket(HTTPSERVICE* pHttpService);
	virtual ~CConnectedHttpSocket() {}

	void			ResetHttpRequest		(void);
	HTTPREQUEST*	GetHttpRequest			(void) {return(&m_HttpRequest);}
	BOOL			DoHttpConnection		(HANDLE hEvent);

private:
	UINT			ParseHeader			(void);
	UINT			ExtractUploadedFile		(LPCSTR szUploadFile);
	UINT			CreateDirectoryListing	(void);
	UINT			SetFileContents		(void);
	void			PrepareResponse		(UINT nHttpCode);
	void			SetCookies			(int& nOffset);
	BOOL			SendResponse			(void);
	BOOL			WriteLog				(void);

	HTTPSERVICE*	m_pHttpService;
	HTTPREQUEST	m_HttpRequest;
	HANDLE		m_hEvent;
};

/*
	CListeningHttpSocket

	Classe per la gestione della lista dei socket in collegamento (lato server).
	La classe viene usata per gestire le connessioni TCP/IP sul lato server.
*/
class CListeningHttpSocket : public CListeningSocket
{
public:
	CListeningHttpSocket(	UINT			nPort,
						const char*	pDomain,
						const char*	pServerName,
						const char*	pRootDir,
						const char*	pSysDir,
						const char*	pUploadDir,
						const char*	pLogDir,
						const char*	pListDir,
						const char*	pDefaultHtmlFile,
						DWORD		dwFlags,
						CHttpExtension* pHttpExtension);
	virtual ~CListeningHttpSocket();

	// servizio
	BOOL			Start			(void);
	BOOL			Stop				(void);
	HTTPSERVICE*	GetHttpService		(void) {return(&m_HttpService);}

	// gestione degli elementi della lista
	CConnectedHttpSocket* Insert(CSock* pSock);
	CConnectedHttpSocket* FindSocket(SOCKET socket);
	void Remove(SOCKET socket);

	void* Create(void)
	{
		return(new CConnectedHttpSocket(GetHttpService()));
	}
	void* Initialize(void* pVoid)
	{
		CConnectedHttpSocket* pData = (CConnectedHttpSocket*)pVoid;
		if(!pData)
			pData = (CConnectedHttpSocket*)Create();
		if(pData)
			((CConnectedHttpSocket*)pData)->Reset();
		return(pData);
	}
	BOOL PreDelete(ITERATOR iter)
	{
		if((CConnectedHttpSocket*)iter->data)
			delete ((CConnectedHttpSocket*)iter->data),iter->data = (CConnectedHttpSocket*)NULL;
		return(TRUE);
	}
	int Size(void) {return(-1);}
#ifdef _DEBUG
	const char* Signature(void) {return("CListeningHttpSocket");}
#endif

private:
	void			LoadMappings		(void);
	void			LoadCookies		(void);

	HANDLE		m_hEvent;
	CThread*		m_pListenThread;
	HTTPSERVICE	m_HttpService;
	HTTP_THREAD_PARAMS m_HttpParams;
};

/*
	CHttpConnection
	
	Classe per il collegamento http (lato client).
	La classe viene usata per gestire la connessione TCP/IP al server http.
	Non condividere tra threads diversi perche' l'utilizzo delle strutture dati interne non prevede un meccanismo di sincronizzazione.
*/
class CHttpConnection : public CSock
{
public:
	CHttpConnection();
	virtual ~CHttpConnection();

	//inizializza solo gli interni, i modificabili vengono inizializzati dal costruttore
	void				Reset			(void);

	// chiude la connessione
	BOOL				Close			(void);
	
	// opzioni
	inline BOOL		GetSkipExisting	(void)					{return(m_bDoNotSkipExisting ? FALSE : (m_bSkipExisting ? m_bSkipExisting : m_bForceSkipExisting));}
	inline void		SetSkipExisting	(BOOL bSkipExisting)		{m_bSkipExisting = bSkipExisting;}
	inline BOOL		GetForceSkipExisting(void)					{return(m_bForceSkipExisting);}
	inline void		SetForceSkipExisting(BOOL bFlag)				{m_bForceSkipExisting = bFlag;}
	inline BOOL		GetDoNotSkipExisting(void)					{return(m_bDoNotSkipExisting);}
	inline void		SetDoNotSkipExisting(BOOL bFlag)				{m_bDoNotSkipExisting = bFlag;}
	inline UINT		GetConnectionRetry	(void)					{return(m_nConnectionRetry);}
	inline void		SetConnectionRetry	(UINT nRetry)				{m_nConnectionRetry = nRetry;}
	inline UINT		GetConnectionTimeout(void)					{return(m_nConnectionTimeout);}
	inline void		SetConnectionTimeout(UINT nTimeout)			{m_nConnectionTimeout = nTimeout;}

	// nome del server http
	inline LPCSTR		GetServerName		(void)					{return(m_szServerName);}
	inline void		SetServerName		(LPCSTR lpcszServerName)		{strcpyn(m_szServerName,lpcszServerName,sizeof(m_szServerName));}

	// nome host
	inline LPCSTR		GetHostName		(void)					{return(m_szHostName);}
	inline void		SetHostName		(LPCSTR lpcszHostName)		{strcpyn(m_szHostName,lpcszHostName,sizeof(m_szHostName));}
	
	// indirizzo ip dell'host
	inline LPCSTR		GetHostAddr		(void)					{return(m_szHostAddr);}
	inline void		SetHostAddr		(LPCSTR lpcszHostAddr)		{strcpyn(m_szHostAddr,lpcszHostAddr,sizeof(m_szHostAddr));}
	
	// numero porta
	inline UINT		GetPortNumber		(void)					{return(m_nPortNumber);}
	inline void		SetPortNumber		(UINT nPortNumber)			{m_nPortNumber = nPortNumber;}

	// proxy
	inline LPCSTR		GetProxyAddress	(void)					{return(m_Proxy.address[0]!='\0' ? m_Proxy.address : NULL);}
	inline void		SetProxyAddress	(LPCSTR lpcszProxyAddress)	{strcpyn(m_Proxy.address,lpcszProxyAddress,HOSTNAME_SIZE+1);}
	inline UINT		GetProxyPortNumber	(void)					{return(m_Proxy.port);}
	inline void		SetProxyPortNumber	(UINT nProxyPortNumber)		{m_Proxy.port = nProxyPortNumber;}
	inline LPCSTR		GetProxyUser		(void)					{return(m_Proxy.user);}
	inline void		SetProxyUser		(LPCSTR lpcszUser)			{strcpyn(m_Proxy.user,lpcszUser,sizeof(m_Proxy.user));}
	inline LPCSTR		GetProxyPassword	(void)					{return(m_Proxy.password);}
	inline void		SetProxyPassword	(LPCSTR lpcszPassword)		{strcpyn(m_Proxy.password,lpcszPassword,sizeof(m_Proxy.password));}
	inline LPCSTR		GetProxyAuth		(void)					{return(m_Proxy.auth);}
	inline void		SetProxyAuth		(LPCSTR lpcszAuth)			{strcpyn(m_Proxy.auth,lpcszAuth,sizeof(m_Proxy.auth));}
	inline CHostNameList* GetProxyExcludeList(void)					{return(&(m_Proxy.exclude));}

	// cookies
	inline BOOL		CookiesAllowed		(void) const				{return(m_bAreCookiesAllowed);}
	inline void		AllowCookies		(BOOL bAllow)				{m_bAreCookiesAllowed = bAllow;}
	void				SetCookieFileName	(LPCSTR lpcszCookieFileName);
	inline LPCSTR		GetCookieFileName	(void) const				{return(m_szCookieFileName);}
	void				ParseCookie		(LPCSTR lpcszCookie,COOKIE* cookie);
	void				ParseInternalCookie	(LPCSTR lpcszCookie,COOKIE* cookie);
	void				SetCookie			(COOKIE* cookie);
	int				GetCookies		(LPCSTR lpcszHost,LPCSTR lpcszPath);
	BOOL				UpdateCookies		(void);
	BOOL				IsCookieExpired	(COOKIE* cookie);
	HTTPHEADER*		EnumCookies		(void);

	// accesso all'oggetto http (HTTPOBJECT)
	inline HTTPOBJECT*	GetObject			(void)					{return(&m_HttpObject);}
	inline void		ResetObject		(void)					{memset(&m_HttpObject,'\0',sizeof(HTTPOBJECT));}

	// header http
	HTTPHEADER*		EnumHeaders		(void);
	BOOL				AddHeader			(LPCSTR lpcszName,LPCSTR lpcszValue,BOOL bAllowDuplicates = TRUE);
	void				DeleteHeader		(LPCSTR lpcszName);
	void				DeleteAllHeaders	(void);
	inline CHttpHeaderList* GetHeadersList	(void)					{return(&m_listHttpHeaders);}

	// limiti download
	inline void		SetMinSize		(UINT nSize)				{m_nMinSize = nSize;}
	inline void		SetMaxSize		(UINT nSize)				{m_nMaxSize = nSize;}
	inline UINT		GetMinSize		(void)					{return(m_nMinSize);}
	inline UINT		GetMaxSize		(void)					{return(m_nMaxSize);}
	
	// gestione errori
	// i codici relativi al socket vengono impostati ricavando i valori della classe base (CSock)
	// mentre i codici http vengono impostati da chi usa la classe per effettuare la connessione
	// notare che i codici winsock vengono impostati solo se il valore corrente e' a 0, ossia se non
	// e' gia' presente un altro codice d'errore, in modo tale che il chiamante possa recuperare il
	// codice reale (in caso contrario errori successivi o la chiusura del socket cambierebbero il
	// valore corrente)
	LPCSTR			GetErrorString		(void);
	inline int		GetHttpError		(void)					{return(m_nHttpCode);}
	inline int		GetSocketError		(void)					{return(m_nSocketCode);}
	inline BOOL		IsSocketError		(void)					{return(m_nSocketCode!=0);}
	inline BOOL		IsHttpError		(void)					{return(m_nHttpCode > (int)HTTP_STATUS_SUCCESSFUL);}
	void				ResetSocketError	(void);
	void				ResetHttpError		(void);
	void				SetSocketError		(void);
	inline void		SetHttpErrorCode	(int nCode)				{m_nHttpCode = nCode;}
	inline void		SetHttpErrorString	(LPCSTR lpcszDesc)			{strcpyn(m_szHttpCode,lpcszDesc,sizeof(m_szHttpCode));}
	static LPCSTR		GetHttpErrorString	(int nCode);

private:
	int				m_nSocketCode;
	char				m_szSocketCode[_MAX_PATH+1];
	
	int				m_nHttpCode;
	char				m_szHttpCode[_MAX_PATH+1];
	HTTPOBJECT		m_HttpObject;
	CHttpHeaderList	m_listHttpHeaders;

	char				m_szServerName[_MAX_PATH+1];
	char				m_szHostName[_MAX_PATH+1];
	char				m_szHostAddr[_MAX_PATH+1];
	UINT				m_nPortNumber;
	PROXY			m_Proxy;
	
	BOOL				m_bSkipExisting;
	BOOL				m_bForceSkipExisting;
	BOOL				m_bDoNotSkipExisting;
	UINT				m_nConnectionRetry;
	UINT				m_nConnectionTimeout;
	BOOL				m_bAreCookiesAllowed;
	char				m_szCookieFileName[_MAX_PATH+1];
	CCookiesList		m_listCookies;
	CSyncProcesses*	m_pmutexCookiesFile;
	UINT				m_nMinSize;
	UINT				m_nMaxSize;
};

#endif // _CHTTP_H
