/*
	CUrl.h
	Classe base per la gestione delle URLs.
	Luca Piergentili, 05/10/00
	lpiergentili@yahoo.com
*/
#ifndef _CURL_H
#define _CURL_H 1

#include "typedef.h"
#include "window.h"
#include <stdlib.h>
#include "CNodeList.h"
#include "CHttp.h"
#include "CSock.h"
#include "CFindFile.h"

#define	CURL_FLAG_ENABLE_JAVASCRIPT			0x00000001
#define	CURL_FLAG_EXTRACT_JAVASCRIPT_FROM_HTML	0x00000002
#define	CURL_FLAG_EXTRACT_TEXT_FROM_HTML		0x00000004

#define	INDEX_HTML			"index.html"
#define	DEFAULT_SHORTHTML_EXT	".htm"
#define	DEFAULT_HTML_EXT		".html"
#define	DEFAULT_URL_EXT		".url"
#define	MAX_URL				HTTP_MAX_URL
#define	DEFAULT_MP3_NAME		"default.mp3"
#define	DEFAULT_MP3_STREAM_NAME	"stream.mp3"

/*
	HTMLTAG_TYPE
	tipi per i tag html riconosciuti
*/
enum HTMLTAG_TYPE {
	UNKNOW_TAG,
	
	BASE_TAG,
	A_TAG,
	LINK_TAG,
	IMG_TAG,
	FRAME_TAG,
	EMBED_TAG,
	BLOCKQUOTE_TAG,
	XMLNS_TAG,

	SRC_TAG,
	HREF_TAG,
	DATA_TAG,
	CLASSID_TAG,
	USEMAP_TAG,
	CODEBASE_TAG,
	ARCHIVE_TAG,
	VALUE_TAG,
	
	BEGINSCRIPT_TAG,
	BEGINSCRIPTLANGUAGE_TAG,
	BEGINSCRIPTSRC_TAG,
	ENDSCRIPT_TAG,
	BEGINNOSCRIPT_TAG,
	ENDNOSCRIPT_TAG,
	
	BEGINDIV_TAG,
	ENDDIV_TAG,
	BEGINSPAN_TAG,
	ENDSPAN_TAG,
	BEGINOBJECT_TAG,
	ENDOBJECT_TAG,
	BEGINAPPLET_TAG,
	ENDAPPLET_TAG
};

/*
	HTMLTAG
	struttura per il tag html
*/
#define MAX_HTMLTAG 16

struct HTMLTAG {
	char			name		[MAX_HTMLTAG+1];
	char			specifier	[MAX_HTMLTAG+1];
	int			len;
	HTMLTAG_TYPE	type;
	HTMLTAG_TYPE	refer;
};

/*
	HTMLOBJECTSTAT
	status corrente relativo all'analisi del file
*/
enum HTMLOBJECTSTAT {
	HTMLOBJECTSTAT_VOID			= 0, // lasciare a indice 0 in modo tale che per resettare sia sufficente una memset
	HTMLOBJECTSTAT_BEGINPARSING	= 1,
	HTMLOBJECT_PARSING			= 2,
	HTMLOBJECT_ENDPARSING		= 3
};

/*
	HTMLOBJECT
	struttura utilizzata durante l'analisi del file
*/
struct HTMLOBJECT {
	HTMLOBJECTSTAT stat;			// status (inizio, parsing, fine)
	char		filename[_MAX_PATH+1];	// file
	QWORD	amount;				// bytes analizzati
	QWORD	size;				// dimensione totale
};

/*
	URL_TYPE
	tipi per le url riconosciute
*/
enum URL_TYPE {
	UNKNOW_URL,
	CID_URL,
	CLSID_URL,
	FILE_URL,
	FINGER_URL,
	FTP_URL,
	GOPHER_URL,
	HDL_URL,
	HTTP_URL,
	HTTPS_URL,
	ILU_URL,
	IOR_URL,
	IRC_URL,
	JAVA_URL,
	JAVASCRIPT_URL,
	LIFN_URL,
	MAILTO_URL,
	MID_URL,
	NEWS_URL,
	NNTP_URL,
	PATH_URL,
	PROSPERO_URL,
	RLOGIN_URL,
	SERVICE_URL,
	SHTTP_URL,
	SNEWS_URL,
	STANF_URL,
	TELNET_URL,
	TN3270_URL,
	WAIS_URL,
	ICY_URL,
	WHOIS_URL
};

/*
	URLTYPE
	struttura per i tipi di url
*/
struct URLTYPE {
	char		url[MAX_URL+1];
	URL_TYPE	type;
};

/*
	URL
	struttura per i dati contenuti nell'url
*/
#define MAX_URL_HOST	HOSTNAME_SIZE
#define MAX_URL_DIR		_MAX_PATH
#define MAX_URL_FILE	_MAX_FNAME+_MAX_EXT
#define MAX_URL_DELIM	2
#define MAX_URL_CGI		_MAX_FNAME+_MAX_EXT
#define MAX_URL_QUERY	_MAX_PATH

struct URL {
	char	host	[MAX_URL_HOST+1];
	char	dir	[MAX_URL_DIR+1];
	char	file	[MAX_URL_FILE+1];
	char	delim[MAX_URL_DELIM+1];
	char	cgi	[MAX_URL_CGI+1];
	char	query[MAX_URL_QUERY+1];
	int	port;
};

/*
	HREF
	struttura per la lista delle referenze (links)
*/
struct HREF {
	char	href[MAX_URL+1];
};

/*
	CHRefList()
	classe per la lista delle referenze
*/
class CHRefList : public CNodeList
{
public:
	CHRefList() : CNodeList() {}
	virtual ~CHRefList() {CNodeList::DeleteAll();}	
	void* Create(void)
	{
		return(new HREF);
	}
	void* Initialize(void* pVoid)
	{
		HREF* pData = (HREF*)pVoid;
		if(!pData)
			pData = (HREF*)Create();
		if(pData)
			memset(pData,'\0',sizeof(HREF));
		return(pData);
	}
	BOOL PreDelete(ITERATOR iter)
	{
		if((HREF*)iter->data)
			delete ((HREF*)iter->data),iter->data = (HREF*)NULL;
		return(TRUE);
	}
	int Size(void) {return(sizeof(HREF));}
#ifdef _DEBUG
	const char* Signature(void) {return("CHRefList");}
#endif
};

/*
	FILETYPE
	struttura per i tipi di file da riconoscere
*/
#define MAX_FILETYPE_EXT (_MAX_EXT*2)

struct FILETYPE {
	char	ext[MAX_FILETYPE_EXT+1];
	int	len;
};

/*
	CFileTypeList
	classe per la lista dei tipi di file da riconoscere
*/
class CFileTypeList : public CNodeList
{
public:
	CFileTypeList() : CNodeList() {}
	virtual ~CFileTypeList() {CNodeList::DeleteAll();}
	void* Create(void)
	{
		return(new FILETYPE);
	}
	void* Initialize(void* pVoid)
	{
		FILETYPE* pData = (FILETYPE*)pVoid;
		if(!pData)
			pData = (FILETYPE*)Create();
		if(pData)
			memset(pData,'\0',sizeof(FILETYPE));
		return(pData);
	}
	BOOL PreDelete(ITERATOR iter)
	{
		if((FILETYPE*)iter->data)
			delete ((FILETYPE*)iter->data),iter->data = (FILETYPE*)NULL;
		return(TRUE);
	}
	int Size(void) {return(sizeof(FILETYPE));}
#ifdef _DEBUG
	const char* Signature(void) {return("CFileTypeList");}
#endif
};

/*
	CUrlStatus
	classe per lo status dell'url
*/
class CUrlStatus {
public:
	/*
		URL_STATUS
		status dell'url
		numerare a base 0 e lasciare UNKNOWN come ultimo per il calcolo dell'indice
		mantenere in corrispondenza con l'array relativo in CUrl.cpp
	*/
	enum URL_STATUS {
		URL_STATUS_DONE				= 0,
		URL_STATUS_DOWNLOADED			= 1,
		URL_STATUS_CHECKED				= 2,
		URL_STATUS_MOVED				= 3,
		URL_STATUS_NOT_MODIFIED			= 4,
		URL_STATUS_CANCELLED			= 5,
		URL_STATUS_EXCLUDED_BY_ROBOTSTXT	= 6,
		URL_STATUS_EXCLUDED_BY_SIZE		= 7,
		URL_STATUS_EXCLUDED_BY_WILDCARDS	= 8,
		URL_STATUS_EXTERNAL_DOMAIN		= 9,
		URL_STATUS_PARENT_URL			= 10,
		URL_STATUS_FILE_CREATION_ERROR	= 11,
		URL_STATUS_INVALID_FILE			= 12,
		URL_STATUS_INVALID_URL			= 13,
		URL_STATUS_CGI_URL				= 14,
		URL_STATUS_WINSOCK_ERROR			= 15,
		URL_STATUS_HTTP_ERROR			= 16,
		URL_STATUS_INCOMPLETE			= 17,
		URL_STATUS_UNKNOWN				= 18
	};
	enum URLSTATUSLEN {
		URL_STATUS_LEN = ((int)URL_STATUS::URL_STATUS_UNKNOWN+1)
	};

	CUrlStatus() {}
	virtual ~CUrlStatus() {}

	LPCSTR GetStatus(URL_STATUS& url_status);
};

/*
	CUrl
	classe per la gestione delle url
*/
class CUrl
{
public:
	CUrl();
	virtual ~CUrl();

	// tipo url
	BOOL		IsUrl				(LPCSTR lpcszUrl);
	BOOL		IsUrlType				(LPCSTR lpcszUrl,URL_TYPE url_type);
	URL_TYPE	GetUrlType			(LPCSTR lpcszUrl,int* pLen = NULL);

	BOOL		IsCgiUrl				(LPCSTR lpcszUrl);
	LPCSTR	GetCgiUrl				(LPCSTR lpcszUrl);
	LPSTR	GetCgiScript			(LPCSTR lpcszUrl,LPSTR lpszCgiScript,UINT nCgiScriptSize);
	LPSTR	GetCgiQuery			(LPCSTR lpcszUrl,LPSTR lpszCgiQuery,UINT nCgiQuerySize);

	BOOL		IsParentHttpUrl		(LPCSTR lpcszParentUrl,LPCSTR lpcszUrl);
	BOOL		CompareHttpHost		(LPCSTR lpcszHost,LPCSTR lpcszUrl,BOOL bIncludeSubDomains = TRUE,BOOL bComparePortNumber = FALSE);

	// aggiunta tipi file html/url da considerare durante il parsing
	LPCSTR	EnumHtmlFileTypes		(void);
	BOOL		AddHtmlFile			(LPCSTR lpcszExt);
	BOOL		IsHtmlFile			(LPCSTR lpcszFileName);
	BOOL		AddUrlFile			(LPCSTR lpcszExt);
	BOOL		IsUrlFile				(LPCSTR lpcszFileName);
	BOOL		IsUrlDir				(LPCSTR lpcszUrl);

	// parsing
	LPCSTR	StripUrlType			(LPCSTR lpcszUrl);
	LPCSTR	StripPathFromUrl		(LPCSTR lpcszUrl);
	LPSTR	StripParentFromUrl		(LPSTR lpszUrl,UINT nUrlSize);
	BOOL		SplitUrl				(LPCSTR lpcszUrl,URL* pUrl);
	BOOL		ComposeUrl			(const URL* pUrl,LPSTR lpszUrl,UINT nUrlSize,BOOL bForceDefaultHttpPort = TRUE);
	CHRefList* ExtractLinks			(LPCSTR lpcszFile,LPCSTR lpcszBaseUrl,DWORD dwFlags,FPCALLBACK lpfnCallback = NULL,LPVOID lpVoid = NULL);
	
	// codifica/decodifica
	LPSTR	EncodeToHTML			(LPSTR s,UINT n);
	LPSTR	EncodeUrl				(LPSTR lpszUrl,UINT nSize);
	LPSTR	DecodeUrl				(LPSTR lpszUrl);
	LPSTR	EncodeHex				(LPSTR s,UINT n,LPCSTR pExclude = "",LPCSTR pInclude = "");
	char		DecodeHex				(LPSTR s);
	
	// pathnames
	BOOL		CreatePathNameFromHttpUrl(LPCSTR lpcszUrl,LPSTR lpszPathname,UINT cbPathnameSize,LPCSTR lpcszInitialDir = NULL,BOOL bCreatePathname = TRUE);
	LPSTR	LocalFileToUrl			(LPCSTR lpcszFile,LPSTR lpszUrl,UINT nUrlSize);
	LPSTR	EnsureValidFileName		(LPSTR lpszFile,UINT cbFileSize,BOOL bConvertSlasch = TRUE,LPCSTR lpcszExt = NULL);
	LPSTR	EnsureBackslash		(LPSTR lpszFile,UINT nFileSize);

	// scarica l'url in locale
	UINT		GetHttpUrl			(CHttpConnection*	pHttpConnection,
								LPCSTR			lpcszUrl,
								BOOL*			bFileDirMismatched,
								LPSTR			lpszTranslatedUrl,
								UINT				cbTranslatedUrl,
								LPCSTR			lpcszDownloadPath,
								LPSTR			lpszLocalFile,
								UINT				cbLocalFileSize,
								CONTENTTYPE*		pContentType,
								BOOL				bForceFileName = FALSE,
								FPCALLBACK		lpfnCallback = NULL,
								LPVOID			lpVoid = NULL,
								BOOL				bDownload = TRUE);
	
	inline UINT GetDownloadBufferSize	(void) {return(MAX_DOWNLOAD_BUFFER_SIZE);}

private:
	// parsing
	BOOL		ExtractLinksFromHtmlTag	(LPCSTR lpcszHtmlTag,HTMLTAG_TYPE& htmltag,LPSTR lpszLink,UINT cbLinkSize);
	UINT		ExtractLinksFromBuffer	(LPSTR szBuffer,URL* pBaseUrl,URL* pUrl,DWORD dwFlags);
	BOOL		AddLink				(URL* pBaseUrl,URL* pUrl,LPCSTR szLink,DWORD dwFlags);

	// lista interna
	BOOL		m_bHtmlFileTypeList;
	void		LoadHtmlFileTypeList	(void);

	enum {MAX_DOWNLOAD_BUFFER_SIZE = 8192};

	CHRefList		m_HRefList;
	CFileTypeList	m_HtmlFileTypeList;
	CFileTypeList	m_UrlFileTypeList;
	CFindFile		m_findFile;
};

#endif // _CURL_H
