/*
	CUrl.cpp
	Classe base per la gestione delle URLs.
	Luca Piergentili, 05/10/00
	lpiergentili@yahoo.com
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include "typedef.h"
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include "strings.h"
#include "window.h"
#include "win32api.h"
#include "CNodeList.h"
#include "CBinFile.h"
#include "CTextFile.h"
#include "CFindFile.h"
#include "CRegistry.h"
#include "CImage.h"
#include "CHttp.h"
#include "CUrl.h"

#include "traceexpr.h"
//#define _TRACE_FLAG	_TRFLAG_TRACEOUTPUT
//#define _TRACE_FLAG	_TRFLAG_NOTRACE
#define _TRACE_FLAG		_TRFLAG_TRACEOUTPUT
#define _TRACE_FLAG_INFO	_TRFLAG_TRACEOUTPUT
#define _TRACE_FLAG_WARN	_TRFLAG_TRACEOUTPUT
#define _TRACE_FLAG_ERR	_TRFLAG_TRACEOUTPUT

#if (defined(_DEBUG) && defined(_WINDOWS)) && (defined(_AFX) || defined(_AFXDLL))
#ifdef PRAGMA_MESSAGE_VERBOSE
  #pragma message("\t\t\t"__FILE__"("STR(__LINE__)"): using DEBUG_NEW macro")
#endif
  #define new DEBUG_NEW
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif

/*
	array per i tipi di url riconosciuti
	usare UNKNOW_URL per chiudere l'array
*/
static const URLTYPE urltype[] = {
	{"http:"		, HTTP_URL},
	{"https:"		, HTTPS_URL},
	{"ftp:"		, FTP_URL},
	{"file:"		, FILE_URL},
	{"java:"		, JAVA_URL},
	{"javascript:"	, JAVASCRIPT_URL},
	{"mailto:"	, MAILTO_URL},
	{"finger:"	, FINGER_URL},
	{"gopher:"	, GOPHER_URL},
	{"news:"		, NEWS_URL},
	{"nntp:"		, NNTP_URL},
	{"cid:"		, CID_URL},
	{"clsid:"		, CLSID_URL},
	{"hdl:"		, HDL_URL},
	{"ilu:"		, ILU_URL},
	{"ior:"		, IOR_URL},
	{"irc:"		, IRC_URL},
	{"lifn:"		, LIFN_URL},
	{"mid:"		, MID_URL},
	{"path:"		, PATH_URL},
	{"prospero:"	, PROSPERO_URL},
	{"rlogin:"	, RLOGIN_URL},
	{"service:"	, SERVICE_URL},
	{"shttp:"		, SHTTP_URL},
	{"snews:"		, SNEWS_URL},
	{"stanf:"		, STANF_URL},
	{"telnet:"	, TELNET_URL},
	{"tn3270:"	, TN3270_URL},
	{"wais:"		, WAIS_URL},
	{"whois:"		, WHOIS_URL},
	{"icy:"		, ICY_URL},
	{""			, UNKNOW_URL},
};

/*
	array per i tipi di file considerati come html
	aggiungere qui i nuovi tipi, dato che la funzione che controlla se il file
	e' di tipo html carica la lista interna con il contenuto dell'array
	usare 0 per chiudere l'array
*/
static const FILETYPE htmlfiletype[] = {
	{".htm"  , 4},
	{".html" , 5},
	{".shtml", 6},
	{".jhtml", 6},
	{".asp"  , 4},
	{".cfm"  , 4},
	{".php"  , 4},
	{".php3" , 5},
	{".php4" , 5},
//	{".js"   , 3},
	{".pl"   , 3},
	{".cgi"  , 4},
	{".xml"  , 4},
//	{".css"  , 4},
//	{".rss"  , 4},
	{""      , 0},
};

/*
	array per i tag html, inclusi i complessi (block-level elements)
	non inserire solo lo specificatore (src o href) ma il tag completo (img src o a href) perche'
	quando si tratta di tag complessi (come <div>) possono essere presenti varie referenze (src o href)
	inserendo solo lo specificatore verrebbe estratto solo il primo link come se si trattasse di un
	tag semplice
	impostare la lunghezza a -1 per i tag fine (come </script>) dato che, non contenendo nessun link,
	devono essere esclusi
	il contenuto dei tag complessi deve essere analizzato in modo tale da estrarre tutti i links (possono
	contenere vari src/href)
	usare 0 per chiudere l'array dato che i valori negativi vengono usati per i tag di chiusura
*/
static const HTMLTAG htmltagFullRef [] = {
	// tag semplici
	{"a "        , " href"      , 5, A_TAG                   , UNKNOW_TAG},
	{"area "     , " href"      , 5, A_TAG                   , UNKNOW_TAG},
	{"base "     , " href"      , 5, BASE_TAG                , UNKNOW_TAG},
	{"blockquote", " cite"      , 5, BLOCKQUOTE_TAG          , UNKNOW_TAG},
	{"body "     , " background",11, IMG_TAG                 , UNKNOW_TAG},
	{"embed "    , " src"       , 4, EMBED_TAG               , UNKNOW_TAG},
	{"form"      , " action"    , 7, FRAME_TAG               , UNKNOW_TAG},
	{"frame "    , " src"       , 4, FRAME_TAG               , UNKNOW_TAG},
	{"iframe "   , " src"       , 4, FRAME_TAG               , UNKNOW_TAG},
	{"input "    , " src"       , 4, FRAME_TAG               , UNKNOW_TAG},
	{"img "      , " src"       , 4, IMG_TAG                 , UNKNOW_TAG},
	{"link "     , " href"      , 5, LINK_TAG                , UNKNOW_TAG},
 
	{" xmlns"    , ""           , 6, XMLNS_TAG               , UNKNOW_TAG},

	// tag complessi (script)
	{"noscript" , ""            , 8, BEGINNOSCRIPT_TAG      , ENDNOSCRIPT_TAG},
	{"/noscript", ""            ,-1, ENDNOSCRIPT_TAG        , UNKNOW_TAG},
	{"script "  , " language"   , 9, BEGINSCRIPTLANGUAGE_TAG, ENDSCRIPT_TAG},
	{"script "  , " src"        , 4, BEGINSCRIPTSRC_TAG     , ENDSCRIPT_TAG},
	{"script"   , ""            , 6, BEGINSCRIPT_TAG        , ENDSCRIPT_TAG},
	{"/script"  , ""            ,-1, ENDSCRIPT_TAG          , UNKNOW_TAG},

	// tag complessi (composti)
	{"div "     , ""            , 4, BEGINDIV_TAG           , ENDDIV_TAG},
	{"div"      , ""            , 3, BEGINDIV_TAG           , ENDDIV_TAG},
	{"/div"     , ""            ,-1, ENDDIV_TAG             , UNKNOW_TAG},
	{"span "    , ""            , 5, BEGINSPAN_TAG          , ENDSPAN_TAG},
	{"span"     , ""            , 4, BEGINSPAN_TAG          , ENDSPAN_TAG},
	{"/span"    , ""            ,-1, ENDSPAN_TAG            , UNKNOW_TAG},
	{"object "  , ""            , 7, BEGINOBJECT_TAG        , ENDOBJECT_TAG},
	{"object"   , ""            , 6, BEGINOBJECT_TAG        , ENDOBJECT_TAG},
	{"/object"  , ""            ,-1, ENDOBJECT_TAG          , UNKNOW_TAG},
	{"applet "  , ""            , 7, BEGINAPPLET_TAG        , ENDAPPLET_TAG},
	{"applet"   , ""            , 6, BEGINAPPLET_TAG        , ENDAPPLET_TAG},
	{"/applet"  , ""            ,-1, ENDAPPLET_TAG          , UNKNOW_TAG},

	{""        , ""             , 0, UNKNOW_TAG             , UNKNOW_TAG}
};

/*
	array per i tag html, include solo i semplici (inline elements)
	inserire solo lo specificatore, dato che deve essere usato per estrarre tutti i link presenti in
	un tag complesso, fa lo stesso il tipo di tag
	usare 0 per chiudere l'array dato che i valori negativi vengono usati per i tag di chiusura
*/
static const HTMLTAG htmltagShortRef [] = {
	{" src"     , "", 4, SRC_TAG     , UNKNOW_TAG},
	{" href"    , "", 5, HREF_TAG    , UNKNOW_TAG},
	{" data"    , "", 5, DATA_TAG    , UNKNOW_TAG},
	{" classid" , "", 8, CLASSID_TAG , UNKNOW_TAG},
	{" usemap"  , "", 7, USEMAP_TAG  , UNKNOW_TAG},
	{" codebase", "", 9, CODEBASE_TAG, UNKNOW_TAG},
	{" archive" , "", 8, ARCHIVE_TAG , UNKNOW_TAG},
	{" value"   , "", 6, VALUE_TAG   , UNKNOW_TAG},
	{" xmlns"   , "", 6, XMLNS_TAG   , UNKNOW_TAG},
	{""         , "", 0, UNKNOW_TAG  , UNKNOW_TAG}
};

/*
	array per lo status dell'url
	mantenere in corrispondenza con le definizioni in CUrl.h
*/
struct URL_STAT {
	CUrlStatus::URL_STATUS stat;
	char* status;
};
static const URL_STAT url_stat [] = {
	CUrlStatus::URL_STATUS_DONE,				"done",
	CUrlStatus::URL_STATUS_DOWNLOADED,			"downloaded",
	CUrlStatus::URL_STATUS_CHECKED,			"checked",
	CUrlStatus::URL_STATUS_MOVED,				"moved",
	CUrlStatus::URL_STATUS_NOT_MODIFIED,		"not modified",
	CUrlStatus::URL_STATUS_CANCELLED,			"cancelled",
	CUrlStatus::URL_STATUS_EXCLUDED_BY_ROBOTSTXT,"excluded by robots.txt",
	CUrlStatus::URL_STATUS_EXCLUDED_BY_SIZE,	"excluded by size",
	CUrlStatus::URL_STATUS_EXCLUDED_BY_WILDCARDS,"excluded by wildcards",
	CUrlStatus::URL_STATUS_EXTERNAL_DOMAIN,		"external domain",
	CUrlStatus::URL_STATUS_PARENT_URL,			"parent url",
	CUrlStatus::URL_STATUS_FILE_CREATION_ERROR,	"file creation error",
	CUrlStatus::URL_STATUS_INVALID_FILE,		"invalid file type",
	CUrlStatus::URL_STATUS_INVALID_URL,		"invalid url type",
	CUrlStatus::URL_STATUS_CGI_URL,			"cgi url",
	CUrlStatus::URL_STATUS_WINSOCK_ERROR,		"winsock error",
	CUrlStatus::URL_STATUS_HTTP_ERROR,			"http error",
	CUrlStatus::URL_STATUS_INCOMPLETE,			"incomplete",
	CUrlStatus::URL_STATUS_UNKNOWN,			"unknown"
};

/*	
	GetStatus()
*/
LPCSTR CUrlStatus::GetStatus(URL_STATUS& url_status)
{
	int nStatus = (int)url_status;
	LPSTR pStatus = NULL;
	
	if(nStatus >= 0 && nStatus < URL_STATUS_LEN)
	{
		pStatus = url_stat[nStatus].status;
	}
	else
	{
		static char szBuffer[64];
		memset(szBuffer,'\0',sizeof(szBuffer));
		if(nStatus <= (int)HTTP_STATUS_UNKNOW)
		{
			switch((HTTP_STATUS)nStatus)
			{
				case HTTP_STATUS_BELOW_MIN_SIZE:
					strcpyn(szBuffer,"below min size",sizeof(szBuffer));
					url_status = URL_STATUS_EXCLUDED_BY_SIZE;
					break;
				case HTTP_STATUS_ABOVE_MAX_SIZE:
					strcpyn(szBuffer,"above max size",sizeof(szBuffer));
					url_status = URL_STATUS_EXCLUDED_BY_SIZE;
					break;
				case HTTP_STATUS_UNABLE_TO_CREATE_LOCAL_FILE:
					strcpyn(szBuffer,"unable to create local file",sizeof(szBuffer));
					url_status = URL_STATUS_FILE_CREATION_ERROR;
					break;
				default:
					_snprintf(szBuffer,sizeof(szBuffer)-1,"http error (%ld)",nStatus);
					url_status = URL_STATUS_HTTP_ERROR;
					break;
			}
		}
		else
		{
			_snprintf(szBuffer,sizeof(szBuffer)-1,"winsock error (%ld)",nStatus);
			url_status = URL_STATUS_WINSOCK_ERROR;
		}
		
		pStatus = szBuffer;
	}

	return(pStatus);
}

/*
	CUrl()
*/
CUrl::CUrl()
{
	// la lista per tipi file html verra' caricata solo quando necessario
	m_bHtmlFileTypeList = FALSE;
}

/*
	~CUrl()
*/
CUrl::~CUrl()
{
}

/*
	IsUrl()

	Controlla se si tratta di un url valida (deve iniziare con uno degli identificativi previsti).
*/
BOOL CUrl::IsUrl(LPCSTR lpcszUrl)
{
	return(GetUrlType(lpcszUrl)!=UNKNOW_URL);
}

/*
	IsUrlType()

	Controlla se l'url e' del tipo indicato (deve iniziare con l'identificativo specificato).
*/
BOOL CUrl::IsUrlType(LPCSTR lpcszUrl,URL_TYPE url_type)
{
	return(GetUrlType(lpcszUrl)==url_type);
}

/*
	GetUrlType()

	Restituisce il tipo di url.
*/
URL_TYPE CUrl::GetUrlType(LPCSTR lpcszUrl,int* pLen/*=NULL*/)
{
	URL_TYPE url_type = UNKNOW_URL;

	if(pLen)
		*pLen = 0;

	for(int i = 0; urltype[i].type!=UNKNOW_URL; i++)
		if(memicmp(urltype[i].url,lpcszUrl,strlen(urltype[i].url))==0)
		{
			url_type = urltype[i].type;
			if(pLen)
				*pLen = strlen(urltype[i].url);
			break;
		}

	return(url_type);
}

/*
	IsCgiUrl()

	Controlla se l'url referenzia uno script cgi.
*/
BOOL CUrl::IsCgiUrl(LPCSTR lpcszUrl)
{
	return(!(strchr(lpcszUrl,'?')==NULL && strchr(lpcszUrl,'&')==NULL));
}

/*
	GetCgiUrl()

	Restituisce il puntatore all'inizio della stringa cgi contenuta nell'url.
*/
LPCSTR CUrl::GetCgiUrl(LPCSTR lpcszUrl)
{
	char* p = NULL;
	
	if(!p)
		p = (char*)strchr(lpcszUrl,'?');
	if(!p)
		p = (char*)strchr(lpcszUrl,'&');
	
	if(p)
	{
		for(int i = p - lpcszUrl; i >= 0; i--)
		{
			p--;
			if(*p=='/')
			{
				p++;
				break;
			}
		}
	}

	return(p);
}

/*
	GetCgiScript()

	Estrae dall'url il nome dello script cgi.
*/
LPSTR CUrl::GetCgiScript(LPCSTR lpcszUrl,LPSTR lpszCgiScript,UINT nCgiScriptSize)
{
	char* p = NULL;
	char szUrl[MAX_URL+1];

	memset(lpszCgiScript,'\0',nCgiScriptSize);
	strcpyn(szUrl,lpcszUrl,sizeof(szUrl));

	if(!p)
		p = strchr(szUrl,'?');
	if(!p)
		p = strchr(szUrl,'&');
	
	if(p)
	{
		*p = '\0';
		p = strrchr(szUrl,'/');
		if(p)
		{
			p++;
			if(p)
				strcpyn(lpszCgiScript,p,nCgiScriptSize);
		}
	}

	return(p);
}

/*
	GetCgiQuery()

	Estrae dall'url il contenuto della query per lo script cgi.
*/
LPSTR CUrl::GetCgiQuery(LPCSTR lpcszUrl,LPSTR lpszCgiQuery,UINT nCgiQuerySize)
{
	char* p = NULL;

	memset(lpszCgiQuery,'\0',nCgiQuerySize);

	if(!p)
		p = (char*)strchr(lpcszUrl,'?');
	if(!p)
		p = (char*)strchr(lpcszUrl,'&');
	
	if(p)
	{
		p++;
		if(p)
			strcpyn(lpszCgiQuery,p,nCgiQuerySize);
	}

	return(p);
}

/*
	IsParentHttpUrl()

	Controlla se la prima url e' padre della seconda, restituendo TRUE in caso affermativo, FALSE
	altrimenti.
	Passare come primo parametro la url suppostamente padre e come secondo la url suppostamente
	figlia.
	es. 1) http://127.0.0.1/pierge/, 2) http://127.0.0.1/pierge/bdt/ la prima e' padre della seconda
	    1) http://127.0.0.1/pierge/bdt/, 2) http://127.0.0.1/pierge/ la prima non e' padre della seconda
*/
BOOL CUrl::IsParentHttpUrl(LPCSTR lpcszParentUrl,LPCSTR lpcszUrl)
{
	URL Url;
	URL ParentUrl;

	if(strstr(lpcszParentUrl,lpcszUrl))
	{
		return(FALSE);
	}
	
	// divide l'url nei componenti
	SplitUrl(lpcszUrl,&Url);
	SplitUrl(lpcszParentUrl,&ParentUrl);

	// host diversi
	if(stricmp(Url.host,ParentUrl.host)!=0)
		return(FALSE);

	// porte diverse
	if((Url.port==0 || Url.port==HTTP_DEFAULT_PORT) && (ParentUrl.port==0 || ParentUrl.port==HTTP_DEFAULT_PORT))
		;
	else
	{
		if(Url.port!=ParentUrl.port)
			return(FALSE);
	}

	// elimina lo '/' finale
	int i;
	i = strlen(ParentUrl.dir);
	if(ParentUrl.dir[i-1]=='/')
		ParentUrl.dir[i-1] = '\0';
	i = strlen(Url.dir);
	if(Url.dir[i-1]=='/')
		Url.dir[i-1] = '\0';

	// codifica le url
	EncodeUrl(ParentUrl.dir,MAX_URL_DIR+1);
	EncodeUrl(Url.dir,MAX_URL_DIR+1);

	TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CUrl::IsParentHttpUrl(): parent: %s, url: %s\n",ParentUrl.dir,Url.dir));

	return(!stristr(ParentUrl.dir,Url.dir));
}

/*
	CompareHttpHost()

	Confronta i due host restituendo TRUE se sono uguali, FALSE altrimenti.
	Si limita a confrontare le stringhe, senza effettuare la risoluzione dei nomi.
*/
BOOL CUrl::CompareHttpHost(LPCSTR lpcszHost,LPCSTR lpcszUrl,BOOL bIncludeSubDomains/* = TRUE*/,BOOL bComparePortNumber/* =FALSE */)
{
	URL Url;
	URL HostUrl;
	BOOL bIsSameDomain = FALSE;

	// non e' un url http, errore
	if(!IsUrlType(lpcszUrl,HTTP_URL))
		return(FALSE);

	// divide l'url nei componenti
	SplitUrl(lpcszUrl,&Url);
	SplitUrl(lpcszHost,&HostUrl);
	
	if(Url.host[0]=='\0')
		return(TRUE);

	// confronta i nomi
	if(strlen(HostUrl.host)==strlen(Url.host))
		bIsSameDomain = (stricmp(HostUrl.host,Url.host)==0 && (bComparePortNumber ? HostUrl.port==Url.port : 1));
	else
		bIsSameDomain = FALSE;

	if(!bIsSameDomain && bIncludeSubDomains)
	{
		// www.somedomain.com, subdomain.somedomain.com
		char* pAddressFromDomain1;
		if(stristr(HostUrl.host,"www."))
		{
			pAddressFromDomain1 = strchr(HostUrl.host,'.');
			if(pAddressFromDomain1)
				pAddressFromDomain1++;
		}
		else // somedomain.com, subdomain.somedomain.com
		{
			pAddressFromDomain1 = stristr(HostUrl.host,"://");
			if(pAddressFromDomain1)
				pAddressFromDomain1 += 3;
		}

		char* pAddressFromDomain2 = strchr(Url.host,'.');
		if(pAddressFromDomain2)
			pAddressFromDomain2++;
		
		if(pAddressFromDomain1 && pAddressFromDomain2)
			bIsSameDomain = (stricmp(pAddressFromDomain1,pAddressFromDomain2)==0 && (bComparePortNumber ? HostUrl.port==Url.port : 1));
	}

	return(bIsSameDomain);
}

/*
	AddHtmlFile()

	Aggiunge il tipo alla lista interna.
*/
BOOL CUrl::AddHtmlFile(LPCSTR lpcszExt)
{
	BOOL bFound = FALSE;
	ITERATOR iter;
	FILETYPE* h;

	// carica la lista se necessario
	LoadHtmlFileTypeList();
	
	// cerca l'estensione nella lista interna
	if((iter = m_HtmlFileTypeList.First())!=(ITERATOR)NULL)
	{
		do
		{
			if((h = (FILETYPE*)iter->data)!=(FILETYPE*)NULL)
				if(memicmp(lpcszExt,h->ext,h->len)==0)
				{
					bFound = TRUE;
					break;
				}
		
			iter = m_HtmlFileTypeList.Next(iter);
		
		} while(iter!=(ITERATOR)NULL);
	}

	// non trovata, aggiunge il tipo
	if(!bFound)
	{
		h = (FILETYPE*)m_HtmlFileTypeList.Add();
		if(h)
		{
			strcpyn(h->ext,lpcszExt,MAX_FILETYPE_EXT+1);
			h->len = strlen(lpcszExt);
			bFound = TRUE;
		}
	}

	return(bFound);
}

/*
	IsHtmlFile()

	Controlla, tramite l'estensione, se si tratta di un file html.
*/
BOOL CUrl::IsHtmlFile(LPCSTR lpcszFileName)
{
	BOOL bIsHtmlFile = FALSE;
	int nLen = strlen(lpcszFileName);
	ITERATOR iter;
	FILETYPE* h;

	// carica la lista se necessario
	LoadHtmlFileTypeList();

	// cerca l'estensione nella lista interna
	if((iter = m_HtmlFileTypeList.First())!=(ITERATOR)NULL)
	{
		do
		{
			if((h = (FILETYPE*)iter->data)!=(FILETYPE*)NULL)
				if(memicmp(lpcszFileName + (nLen - h->len),h->ext,h->len)==0)
				{
					bIsHtmlFile = TRUE;
					break;
				}
		
			iter = m_HtmlFileTypeList.Next(iter);
		
		} while(iter!=(ITERATOR)NULL);
	}

	// controllare qui i files di servizio
	if(!bIsHtmlFile)
	{
		bIsHtmlFile = striright(lpcszFileName,"robots.txt")==0;
	}

	// metodo bestia
	if(!bIsHtmlFile)
	{
		CBinFile f;
		if(f.OpenExistingReadOnly(lpcszFileName))
		{
			char szBuffer[2048] = {0};
			if(f.Read(szBuffer,sizeof(szBuffer)-1))
			{
				bIsHtmlFile = stristr(szBuffer,"<head>")!=NULL;
			}
			f.Close();
		}
	}
	
	return(bIsHtmlFile);
}

/*
	AddUrlFile()

	Aggiunge il tipo alla lista interna.
*/
BOOL CUrl::AddUrlFile(LPCSTR lpcszExt)
{
	BOOL bFound = FALSE;
	ITERATOR iter;
	FILETYPE* u;

	// cerca l'estensione nella lista interna
	if((iter = m_UrlFileTypeList.First())!=(ITERATOR)NULL)
	{
		do
		{
			if((u = (FILETYPE*)iter->data)!=(FILETYPE*)NULL)
				if(memicmp(lpcszExt,u->ext,u->len)==0)
				{
					bFound = TRUE;
					break;
				}
		
			iter = m_UrlFileTypeList.Next(iter);
		
		} while(iter!=(ITERATOR)NULL);
	}

	// non trovata, aggiunge il tipo
	if(!bFound)
	{
		u = (FILETYPE*)m_UrlFileTypeList.Add();
		if(u)
		{
			strcpyn(u->ext,lpcszExt,MAX_FILETYPE_EXT+1);
			u->len = strlen(lpcszExt);
			bFound = TRUE;
		}
	}

	return(bFound);
}

/*
	IsUrlFile()

	Controlla, tramite l'estensione, se si tratta di uno dei file da riconoscere.
*/
BOOL CUrl::IsUrlFile(LPCSTR lpcszFileName)
{
	BOOL bIsUrlFile = FALSE;
	int nLen = strlen(lpcszFileName);
	ITERATOR iter;
	FILETYPE* u;

	// cerca l'estensione nella lista interna
	if((iter = m_UrlFileTypeList.First())!=(ITERATOR)NULL)
	{
		do
		{
			if((u = (FILETYPE*)iter->data)!=(FILETYPE*)NULL)
				if(memicmp(lpcszFileName + (nLen - u->len),u->ext,u->len)==0)
				{
					bIsUrlFile = TRUE;
					break;
				}
		
			iter = m_UrlFileTypeList.Next(iter);
		
		} while(iter!=(ITERATOR)NULL);
	}

	return(bIsUrlFile);
}

/*
	IsUrlDir()

	Controlla se l'url specifica una directory (non effettua lo split ma controlla al volo).
*/
BOOL CUrl::IsUrlDir(LPCSTR lpcszUrl)
{
	BOOL bIsUrlDir = FALSE;

	bIsUrlDir = IsUrl(lpcszUrl) && lpcszUrl[strlen(lpcszUrl)-1]=='/';

	return(bIsUrlDir);
}

/*
	StripUrlType()

	Restituisce il puntatore all'url senza l'identificativo.
	Il puntatore fa riferimento all'offset a cui termina l'url nel buffer.
	es. http://127.0.0.1/... -> 127.0.0.1/...
*/
LPCSTR CUrl::StripUrlType(LPCSTR lpcszUrl)
{
	URL_TYPE url_type = GetUrlType(lpcszUrl);
	LPSTR pUrl = (LPSTR)lpcszUrl;

	for(int i = 0; urltype[i].type!=UNKNOW_URL; i++)
		if(memicmp(urltype[i].url,lpcszUrl,strlen(urltype[i].url))==0)
		{
			url_type = urltype[i].type;
			pUrl += strlen(urltype[i].url);
			break;
		}

	if(url_type!=UNKNOW_URL)
		while(*pUrl && *pUrl=='/')
			pUrl++;

	return(pUrl);
}

/*
	StripPathFromUrl()

	Restituisce il puntatore all'url eliminando quanto presente prima dell'url.
	Il puntatore fa riferimento all'offset a cui inizia l'url nel buffer.
	es. http://127.0.0.1/.../file.html -> file.html
*/
LPCSTR CUrl::StripPathFromUrl(LPCSTR lpcszUrl)
{
	LPSTR pUrl = (LPSTR)strrchr(lpcszUrl,'/');
	pUrl = (pUrl && *(pUrl+1)) ? pUrl+1 : (LPSTR)lpcszUrl;
	return(pUrl);
}

/*
	StripParentFromUrl()

	Risolve le referenze alla directory padre (../) all'interno dell'url, eliminando la directory relativa.
	Prima di risolvere le referenze si occupa di convertire i "./" (usati ad es. nei javascript) in "../".
*/
LPSTR CUrl::StripParentFromUrl(LPSTR lpszUrl,UINT nUrlSize)
{
	char* p;

	// elimina lo spazio inutilizzato alla fine della stringa
	int i = strlen(lpszUrl);
	if((nUrlSize - i) > 0)
		memset(lpszUrl+i,'\0',nUrlSize-i);

	// sostituisce i ./ in ../
	p = strstr(lpszUrl,"./");
	while(p)
	{
		if(p-lpszUrl > 0)
			if(*(p-1)!='.')
				if(strlen(lpszUrl) < nUrlSize-1)
				{
					memmove(p+1,p,strlen(p));
					*p = '.';
				}

		p = strstr(p+2,"./");
	}

	// risolve le referenze a ../ eliminando la directory relativa
	p = strstr(lpszUrl,"../");
	if(p)
	{
		int nStart;
		char* pPrev;
		char szBegin[MAX_URL+1];
		char szEnd[MAX_URL+1];
		
		while(p)
		{
			// copia quanto (eventualmente) presente a partire da "../"
			memset(szEnd,'\0',sizeof(szEnd));
			if(*(p+3))
				strcpyn(szEnd,p+3,sizeof(szEnd));
			
			// il "../" trovato sopra deve essere preceduto da un altra barra ed un carattere: "<chr>/"
			// quindi retrocede di 2 per posizionarsi sul primo carattere prima della barra
			p -= 2;

			// controlla che, retrocedendo, non abbia superato l'inizio della stringa
			if((nStart = p - lpszUrl) > 0)
			{
				// e' stato specificato un "../" di seguito all'identificativo dell'url, ossia il "../"
				// non e' preceduto da una directory - "<chr>/", ritorna
				if(*p=='/')
					return(lpszUrl);

				// retrocede fino al prossimo "/"
				pPrev = p;
				while(nStart-- >= 0 && *pPrev!='/')
					pPrev--;
			}
			else
				return(lpszUrl);

			// copia quanto precede il "../" trovato sopra, includendo l'ultima "/"
			memset(szBegin,'\0',sizeof(szBegin));
			memcpy(szBegin,lpszUrl,(pPrev-lpszUrl)+1);

			// ricostruisce l'url (per ogni "../" trovato elimina una directory)
			_snprintf(lpszUrl,nUrlSize-1,"%s%s",szBegin,szEnd);

			// cerca il prossimo "../"
			p = strstr(lpszUrl,"../");
		}
	}

	return(lpszUrl);
}

/*
	SplitUrl()

	Divide l'url nei vari componenti (host, dir, file, cgi, numero porta, etc.).
*/
BOOL CUrl::SplitUrl(LPCSTR lpcszUrl,URL* pUrl)
{
	char* p;
	BOOL bIsHttpUrl = FALSE;
	char szUrl[MAX_URL+1];

	strcpyn(szUrl,lpcszUrl,sizeof(szUrl));
	memset(pUrl,'\0',sizeof(URL));
	
	// estrae i dati relativi all'eventuale cgi
	if(IsCgiUrl(szUrl))
/*
se 'icon' e' una directory il codice sotto #if 0 lo considera come nome file facendo sballare il controllo per parent url
{
	char* p1={"http://iconfactory.com:80/freeware/icon?page=2"};
	char* p2={"http://iconfactory.com/freeware/icon"};
	CUrl u;
	BOOL bIs = u.IsParentHttpUrl(p1,p2);
	if(bIs)
		bIs = !bIs;
}
*/
#if 0
	{
		p = (char*)GetCgiUrl(szUrl);
		GetCgiScript(szUrl,pUrl->cgi,MAX_URL_CGI+1);
		GetCgiQuery(szUrl,pUrl->query,MAX_URL_QUERY+1);
		*p = '\0';
		p = (char*)strchr(lpcszUrl,'?');
		if(!p)
			p = (char*)strchr(lpcszUrl,'&');
		memset(pUrl->delim,'\0',MAX_URL_DELIM+1);
		if(p)
			pUrl->delim[0] = *p;
		else
			pUrl->delim[0] = '?';
	}
#else
	{
		p = (char*)GetCgiUrl(szUrl);
		
		char* P = p;
		BOOL bHasCgiFilename = FALSE;
		while(*P && *P!='?')
		{
			if(*P++=='.')
			{
				bHasCgiFilename = TRUE;
				break;
			}
		}
		
		if(bHasCgiFilename)
			GetCgiScript(szUrl,pUrl->cgi,MAX_URL_CGI+1);

		GetCgiQuery(szUrl,pUrl->query,MAX_URL_QUERY+1);
		
		if(!bHasCgiFilename)
		{
			while(*p)
			{
				if(*p=='?')
					break;
				p++;
			}
		}
		*p = '\0';
		
		p = (char*)strchr(lpcszUrl,'?');
		if(!p)
			p = (char*)strchr(lpcszUrl,'&');
		memset(pUrl->delim,'\0',MAX_URL_DELIM+1);
		if(p)
			pUrl->delim[0] = *p;
		else
			pUrl->delim[0] = '?';
	}
#endif

	// e' un url http ?
	bIsHttpUrl = IsUrlType(szUrl,HTTP_URL);

	// url con identificativo (qualsiasi, non solo http://)
	if(IsUrl(szUrl))
	{
		// imposta l'host
		strcpyn(pUrl->host,szUrl,MAX_URL_HOST+1);
		p = strchr(pUrl->host+7,'/');
		if(p)
		{
			// imposta la directory
			// controllare, nel caso di un cgi, se elimina correttamente il nome del file script
			strcpyn(pUrl->dir,p,MAX_URL_DIR+1);
			*p = '\0';

#if 1
			int dir_len = strlen(pUrl->dir);
			if(	(pUrl->dir[0]=='/' && dir_len==1) ||
				(dir_len > 1 && pUrl->dir[dir_len-1]=='/')
				)
			{
				strcpy(pUrl->file,"");
			}
			else
			{
				p = strrchr(pUrl->dir,'/');
				strcpyn(pUrl->file,p+1,MAX_URL_FILE+1);
				*(p+1)='\0';
			}
#else
/*
			p = strrchr(pUrl->dir+1,'/');
			if(p)
			{
				char* ext = strchr(p+1,'.');
				if(ext)
				{
					strcpyn(pUrl->file,p+1,MAX_URL_FILE+1);
					*(p+1)='\0';
				}
			}
			else
			{
				char* ext = strchr(pUrl->dir,'.');
				if(ext)
				{
					strcpyn(pUrl->file,pUrl->dir+1,MAX_URL_FILE+1);
					*(pUrl->dir+1)='\0';
				}
			}

			if(pUrl->dir[strlen(pUrl->dir)-1]!='/')
				// per non aggiungere lo slasch finale se non viene specificato un nome file, come per le query
				if(pUrl->file[0]!='\0')
					strcatn(pUrl->dir,"/",MAX_URL_DIR+1);
*/
#endif
		}
	}
	else // url senza identificativo
	{
		// si tratta di un file, senza nome host/directory
		if(strchr(szUrl,'/')==NULL)
		{
			strcpyn(pUrl->file,szUrl,MAX_URL_FILE+1);
			return(FALSE);
		}

		// se l'url inizia con '/' la considera una directory, altrimenti considera
		// quanto presente (fino al prossimo '/') come l'identificativo dell'host
		if(szUrl[0]!='/')
		{
			int i;
			for(i = 0; szUrl[i] && szUrl[i]!='/' && i < MAX_URL_HOST; i++)
				pUrl->host[i] = pUrl->host[i];
			pUrl->host[i] = '\0';
			
			// controlla che l'host non specifichi un qualche tipo di url
			if(IsUrl(pUrl->host))
				bIsHttpUrl = FALSE;

			p = (char*)szUrl+i+1;
		}
		else
			p = (char*)szUrl;

		// imposta la directory
		strcpyn(pUrl->dir,p,MAX_URL_DIR+1);

		p = strrchr(pUrl->dir,'/');
		if(p)
		{
			char* ext = strchr(p+1,'.');
			if(ext)
			{
				strcpyn(pUrl->file,p+1,MAX_URL_FILE+1);
				*(p+1)='\0';
			}
		}
		else
		{
			char* ext = strchr(pUrl->dir,'.');
			if(ext)
			{
				strcpyn(pUrl->file,pUrl->dir,MAX_URL_FILE+1);
				*pUrl->dir='\0';
			}
		}

		int i = strlen(pUrl->dir);
		if(i > 0)
			if(pUrl->dir[i-1]!='/')
				// per non aggiungere lo slasch finale se non viene specificato un nome file, come per le query
				if(pUrl->file[0]!='\0')
					strcatn(pUrl->dir,"/",MAX_URL_DIR+1);
	}

	// se l'url contiene solo l'host assume come directory la root (/)
	if(pUrl->dir[0]=='\0')
		if(pUrl->file[0]=='\0')
			pUrl->dir[0] = '/';


	// ricava il numero della porta se presente
	// la sintassi corretta e' http://127.0.0.1:8080/..., pero', siccome ci sono molti cazzoni in giro,
	// ammette il numero di porta alla fine del nome del file (ossia a fine url)
	// occhio che se non viene cercata l'ultima occorrenza del separatore (':') si beccano i due punti
	// dell'url
	int nPort = 0;
	int nUrlLen = 0;
	GetUrlType(pUrl->host,&nUrlLen);
	
	p = strrchr(pUrl->host+nUrlLen,':'); // forma canonica
	if(!p)
		p = strrchr(pUrl->file,':'); // per i cazzoni
	if(p)
	{
		int i = 0;
		char szPort[8] = {0};
		
		// ricava il numero della porta (stringa)
		p++;
		while(*p && isdigit(*p) && i < sizeof(szPort)-1)
			szPort[i++] = *p++;
		
		// ricava il numero della porta (numerico)
		*(p-i-1) = '\0';
		szPort[i] = '\0';
		nPort = atoi(szPort);
		
		if(nPort <= 0)
			nPort = bIsHttpUrl ? HTTP_DEFAULT_PORT : 0;
	}
	
	// imposta il numero della porta, con il default o con la specificata
	pUrl->port = nPort;

	return(bIsHttpUrl);
}

/*
	ComposeUrl()

	Ricompone l'url a partire dai componenti.
*/
BOOL CUrl::ComposeUrl(const URL* pUrl,LPSTR lpszUrl,UINT nUrlSize,BOOL bForceDefaultHttpPort/* = TRUE*/)
{
	// forza il numero porta se l'url non la specifica gia'
	char szPort[10] = {""};
	int nPort = pUrl->port;
	if(nPort <= 0 && bForceDefaultHttpPort)
		nPort = IsUrlType(pUrl->host,HTTP_URL) ? HTTP_DEFAULT_PORT : 0;
	if(nPort > 0)
		_snprintf(szPort,sizeof(szPort)-1,":%ld",nPort);

	_snprintf(lpszUrl,
			nUrlSize-1,
			"%s%s%s%s%s%s%s",
			pUrl->host,
			szPort,
			pUrl->dir,
			pUrl->file,
			pUrl->cgi,
			pUrl->delim,
			pUrl->query
			);

	return(TRUE);
}

/*
	ExtractLinks()

	Analizza il file html restituendo il puntatore alla lista contenente i links trovati.
	Non considera il numero della porta.
*/
CHRefList* CUrl::ExtractLinks(LPCSTR lpcszFile,LPCSTR lpcszBaseUrl,DWORD dwFlags,FPCALLBACK lpfnCallback,LPVOID lpVoid)
{
	URL			BaseUrl;
	URL			Url;
	char			szBaseUrl[MAX_URL+1];
	char			szLink[MAX_URL+1];
	char			szHtmlTag[2048];
	HTMLOBJECT	htmlobject;
	HANDLE		hFile,hFileMap;
	LPVOID		lpFileView;
	LPSTR		pData;
	QWORD		qwFileSize = 0L;
	LARGE_INTEGER	li = {0};
	int			nOffset = 0;
	HTMLTAG_TYPE	htmltagtype;
	FILETIME		filetime;
	CBinFile		textFile;
	char			szTextFile[_MAX_FILEPATH+1];
	char			szTextBuffer[2048];
	int			nText = 0;
	CBinFile		scriptFile;
	char			szScriptFile[_MAX_FILEPATH+1];
	int			nScript = 0;
	int			nCallBackCalls = 0;

	memset(&BaseUrl,'\0',sizeof(URL));

	// azzera la lista per i links
	m_HRefList.RemoveAll();

	// verifica che si tratti di un file HTML
	if(IsHtmlFile(lpcszFile))
	{
		// normalizza l'url
		strcpyn(szBaseUrl,lpcszBaseUrl,sizeof(szBaseUrl));
		EncodeUrl(szBaseUrl,sizeof(szBaseUrl));

		// divide l'url nei tre componenti
		SplitUrl(szBaseUrl,&Url);

		// apre il file HTML ricavando data e ora
		if((hFile = ::CreateFile(lpcszFile,GENERIC_WRITE|GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL))!=INVALID_HANDLE_VALUE)
			::GetFileTime(hFile,NULL,NULL,&filetime);
		else
			goto parse_done;

		// dimensione del file HTML
		if(::GetFileSizeExt(hFile,&li))
			qwFileSize = li.QuadPart;

		// crea la mappa per il file HTML
		if((hFileMap = ::CreateFileMapping(hFile,NULL,PAGE_READWRITE,li.HighPart,li.LowPart+sizeof(char),NULL))==NULL)
		{
			::CloseHandle(hFile);
			goto parse_done;
		}

		// mappa il file HTML
		if((lpFileView = ::MapViewOfFile(hFileMap,FILE_MAP_WRITE,0,0,0))==NULL)
		{
			::CloseHandle(hFileMap);
			::CloseHandle(hFile);
			goto parse_done;
		}

		// chiama la callback (inizio)
		if(lpfnCallback)
		{
			memset(&htmlobject,'\0',sizeof(HTMLOBJECT));
			htmlobject.stat = HTMLOBJECTSTAT_BEGINPARSING;
			strcpyn(htmlobject.filename,lpcszFile,sizeof(htmlobject.filename));
			htmlobject.amount = 0L;
			htmlobject.size = qwFileSize;
			if(lpfnCallback(&htmlobject,lpVoid)!=0)
				goto done;
		}

		// inserisce uno zero alla fine del file per analizzarne il contenuto come se fosse una stringa
		pData = (LPSTR)lpFileView;
		pData[qwFileSize] = '\0';

		// crea il file per il contenuto (testo) del file HTML
		if(dwFlags & CURL_FLAG_EXTRACT_TEXT_FROM_HTML)
		{
			memset(szTextBuffer,'\0',sizeof(szTextBuffer));
			_snprintf(szTextFile,sizeof(szTextFile)-1,"%s.txt",lpcszFile);
			textFile.Create(szTextFile);
		}

		// ciclo principale, legge il file HTML per caratteri, fino ad incontrare lo '\0' finale impostato
		// sopra, come si si trattasse di una unica stringa
		nOffset = 0;
		while(pData[nOffset])
		{
			// salta il contenuto fino ad incontrare l'inizio di un tag
			while(pData[nOffset] && pData[nOffset]!='<')
			{
				// controlla se deve estrarre il testo del file HTML
				if(dwFlags & CURL_FLAG_EXTRACT_TEXT_FROM_HTML)
				{
					if(pData[nOffset]!='>')
					{
						if(nText >= sizeof(szTextBuffer)-1)
						{
							textFile.Write(szTextBuffer,nText+1);
							memset(szTextBuffer,'\0',sizeof(szTextBuffer));
							nText = 0;
						}
						else
							szTextBuffer[nText++] = *(pData+nOffset);
					}
				}

				nOffset++;
			}

			// chiama la callback (analisi)
			if(lpfnCallback)
			{
				if(++nCallBackCalls > 5)
				{
					nCallBackCalls = 0;
					htmlobject.stat = HTMLOBJECT_PARSING;
					htmlobject.amount = nOffset;
					if(lpfnCallback(&htmlobject,lpVoid)!=0)
						goto done;
				}
			}

			// posiziona all'inizio del tag ('<')
			while(pData[nOffset] && pData[nOffset]=='<')
			{
				nOffset++;
				
				// copia il contenuto del tag (fino a raggiungere la fine del tag '>')
				int i = 0;
				memset(szHtmlTag,'\0',sizeof(szHtmlTag));
				while(pData[nOffset] && pData[nOffset]!='>' && i < sizeof(szHtmlTag)+1)
				{
					if(pData[nOffset]=='\r' || pData[nOffset]=='\n')
						szHtmlTag[i++] = ' ';
					else
						szHtmlTag[i++] = pData[nOffset];
					nOffset++;
				}

				TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CUrl::ExtractLinks(): tag: [%s]\n",szHtmlTag));

				// estrae i link presenti nel tag
				// i tag semplici, come IMG SRC, A HREF, etc., contengono un solo link che viene estratto da ExtractLinksFromHtmlTag(),
				// i tag complessi, come SCRIPT, DIV, etc., possono contenere svariati link, per cui l'estrazione avviene analizzando i
				// dati compresi tra l'inizio e la fine del tag, con ExtractLinksFromBuffer()
				// in generale, se il tag puo' contenere altri tag, deve essere usato il ciclo sottostante, altrimenti l'elaborazione
				// basata sul riconoscimento dei caratteri '<>' fallirebbe, dato che non gestisce '<>' annidati
				// un caso a parte sono gli <script> strutturati come segue:
				//
				//	<script type="text/javascript" language="Javascript">
				//	<!--
				//		if(document.images) {
				//		}
				//	//-->
				//	</script>
				//
				// con questa sintassi, le due linee <script...> e </script> vengono analizzate correttamente ma il resto viene ignorato,
				// sia per il commento iniziale (<!--), sia perche' la ExtractLinksFromHtmlTag() non gestisce piu' link come testo, in
				// tal caso (ossia se la ExtractLinksFromHtmlTag() sottostante restituisce false) prova ad analizzare il contenuto del
				// buffer con ExtractLinksFromBuffer() per vedere se esce fuori qualcosa
				memset(szLink,'\0',sizeof(szLink));
				if(ExtractLinksFromHtmlTag(szHtmlTag,htmltagtype,szLink,sizeof(szLink)))
				{
					TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CUrl::ExtractLinks(): link(type=%d): %s\n",(int)htmltagtype,szLink));

					// estrae il contenuto dello script (<SCRIPT>...</SCRIPT>)
					switch(htmltagtype)
					{
						case BEGINSCRIPTLANGUAGE_TAG:
						case BEGINSCRIPTSRC_TAG:
						case BEGINSCRIPT_TAG:
						case BEGINNOSCRIPT_TAG:
						{
							// posiziona all'inizio dello script (escludendo <SCRIPT>)
							// salta il '>' del ciclo anteriore
							nOffset++;
							
							// salta gli spazi
							while(pData[nOffset] && ISSPACE(pData[nOffset]))
								nOffset++;

							// cerca la fine dello script
							char endtag[16] = {0};
							int htmltagindex = 0;

							while(htmltagFullRef[htmltagindex].len!=0)
							{
								if(htmltagtype==htmltagFullRef[htmltagindex].type)
								{
									int htmltagrefer = 0;
									while(strcmp(htmltagFullRef[htmltagrefer].name,"")!=0)
									{
										if(htmltagFullRef[htmltagindex].refer==htmltagFullRef[htmltagrefer].type)
										{
											strcpyn(endtag,htmltagFullRef[htmltagrefer].name,sizeof(endtag));
											// break???
											break;
										}
										htmltagrefer++;
									}
								}
								htmltagindex++;
							}

							LPSTR pDataBegin = pData+nOffset;
							LPSTR pDataEnd = stristr(pDataBegin,endtag);
							if(pDataEnd)
							{
								// posiziona alla fine dello script (escludendo </SCRIPT>)
								while(*pDataEnd!='<')
									pDataEnd--;
								
								// salta il '<' del ciclo anteriore
								pDataEnd--;
								
								// salta gli spazi
								while(ISSPACE(*pDataEnd))
									pDataEnd--;

								// calcola la dimensione dello script (puo' essere 0 per <SCRIPT SRC="..."></SCRIPT>)
								int nSize = (pDataEnd - pDataBegin)+1;
								if(nSize > 0)
								{
									BOOL bParseScript = FALSE;

									switch(htmltagtype)
									{
										case BEGINSCRIPTLANGUAGE_TAG:
										case BEGINSCRIPTSRC_TAG:
										case BEGINSCRIPT_TAG:
											bParseScript = (dwFlags & CURL_FLAG_ENABLE_JAVASCRIPT);
											break;
										case BEGINNOSCRIPT_TAG:
											bParseScript = !(dwFlags & CURL_FLAG_ENABLE_JAVASCRIPT);
											break;
									}
									if(bParseScript)
									{
										// estrae i link dallo script
										char c = pDataBegin[nSize];
										pDataBegin[nSize] = '\0';
										ExtractLinksFromBuffer(pDataBegin,&BaseUrl,&Url,dwFlags);
										pDataBegin[nSize] = c;

										// crea il file con il contenuto (codice) dello script
										if(dwFlags & CURL_FLAG_EXTRACT_JAVASCRIPT_FROM_HTML)
										{
											_snprintf(szScriptFile,sizeof(szScriptFile)-1,"%s.%d.js",lpcszFile,nScript++);
											if(scriptFile.Create(szScriptFile))
											{
												scriptFile.Write(pDataBegin,nSize);
												scriptFile.Close();
											}
										}
									}

									// posiziona alla fine dello script (</SCRIPT> incluso)
									nOffset += nSize;
									while(pData[nOffset] && pData[nOffset]!='<')
										nOffset++;
									
									// salta il '<' del ciclo anteriore
									nOffset++;
									while(pData[nOffset] && pData[nOffset]!='>')
										nOffset++;
									
									// salta il '>' del ciclo anteriore
									nOffset++;
								}
							}
							
							break;
						}
						
						case BEGINDIV_TAG:
						case BEGINSPAN_TAG:
						case BEGINOBJECT_TAG:
						case BEGINAPPLET_TAG:
						{
							// posiziona all'inizio del contenuto del tag (escludendo "<DIV ")
							// salta il '>' del ciclo anteriore
							nOffset++;
							
							// salta gli spazi
							while(pData[nOffset] && ISSPACE(pData[nOffset]))
								nOffset++;

							// cerca la fine del tag
							char endtag[16] = {0};
							int htmltagindex = 0;

							while(htmltagFullRef[htmltagindex].len!=0)
							{
								if(htmltagtype==htmltagFullRef[htmltagindex].type)
								{
									int htmltagrefer = 0;
									while(strcmp(htmltagFullRef[htmltagrefer].name,"")!=0)
									{
										if(htmltagFullRef[htmltagindex].refer==htmltagFullRef[htmltagrefer].type)
										{
											strcpyn(endtag,htmltagFullRef[htmltagrefer].name,sizeof(endtag));
											// break???
											break;
										}
										htmltagrefer++;
									}
								}
								htmltagindex++;
							}

							LPSTR pDataBegin = pData+nOffset;
							LPSTR pDataEnd = stristr(pDataBegin,endtag);
							if(pDataEnd)
							{
								// posiziona alla fine del contenuto del tag (escludendo </DIV>)
								while(*pDataEnd!='<')
									pDataEnd--;
								
								// salta il '<' del ciclo anteriore
								pDataEnd--;
								
								// salta gli spazi
								while(ISSPACE(*pDataEnd))
									pDataEnd--;

								// calcola la dimensione del contenuto del tag
								int nSize = (pDataEnd - pDataBegin)+1;
								if(nSize > 0)
								{
									// estrae i links dallo script
									char c = pDataBegin[nSize];
									pDataBegin[nSize] = '\0';
									ExtractLinksFromBuffer(pDataBegin,&BaseUrl,&Url,dwFlags);
									pDataBegin[nSize] = c;

									// posiziona alla fine del contenuto del tag (</DIV> incluso)
									nOffset += nSize;
									while(pData[nOffset] && pData[nOffset]!='<')
										nOffset++;
									
									// salta il '<' del ciclo anteriore
									nOffset++;
									while(pData[nOffset] && pData[nOffset]!='>')
										nOffset++;
									
									// salta il '>' del ciclo anteriore
									nOffset++;
								}
							}
						
							break;
						}

						case BASE_TAG:
						{
							SplitUrl(szLink,&BaseUrl);
							break;
						}

						default:
						{
							if(IsUrlType(szLink,JAVASCRIPT_URL))
								ExtractLinksFromBuffer(szHtmlTag,&BaseUrl,&Url,dwFlags);
							else
								AddLink(&BaseUrl,&Url,szLink,dwFlags);

							break;
						}
					}
				}
				else // vedi le note sopra sull'altro ramo della if
				{
					// prima di analizzare a rotta di collo, controlla se il buffer 'sembra' contenere links
					// non dimenticare che l'espediente (ossia la chiamata a ExtractLinksFromBuffer() se quella a
					// ExtractLinksFromHtmlTag() ha restituito false) per beccare i links contenuti negli script
					// con la sintassi di cui sopra, comporterebbe l'analisi di un buffer che per la maggior parte
					// dei casi contiene un semplice tag senza specificatore
//					if(strpbrk(szHtmlTag,"\"'.\\/="))
					if(strpbrk(szHtmlTag,".\\"))
						ExtractLinksFromBuffer(szHtmlTag,&BaseUrl,&Url,dwFlags);
				}
			}
		}
		
		// chiama la callback (fine)
		if(lpfnCallback)
		{
			htmlobject.stat = HTMLOBJECT_ENDPARSING;
			if(lpfnCallback(&htmlobject,lpVoid)!=0)
				goto done;
		}
done:
		// chiude la mappa
		::UnmapViewOfFile(lpFileView);
		::CloseHandle(hFileMap);

		// chiude il file, eliminando lo zero finale
		LONG lHigh = li.HighPart;
		::SetFilePointer(hFile,li.LowPart,&lHigh,FILE_BEGIN);
		::SetEndOfFile(hFile);
		::SetFileTime(hFile,&filetime,&filetime,&filetime);
		::CloseHandle(hFile);

		if(dwFlags & CURL_FLAG_EXTRACT_TEXT_FROM_HTML)
		{
			if(nText > 0)
				textFile.Write(szTextBuffer,nText+1);
			textFile.Close();
		}
	}

parse_done:

	return(&m_HRefList);
}

/*
	EncodeToHTML()

	Converte, all'interno della stringa, i caratteri speciali in sequenze HTML (&...).
*/
LPSTR CUrl::EncodeToHTML(LPSTR s,UINT n)
{
	// azzera lo spazio inutilizzato alla fine della stringa
	int i = strlen(s);
	if((n - i) > 0)
		memset(s+i,'\0',n-i);

	for(i = 0; i < (int)n && s[i]; i++)
	{
		// sposta il contenuto della stringa per fare spazio alla direttiva
		if(s[i]==' ')
		{
			if((int)strlen(s) + 6 < (int)n-1)
			{
				memmove(s+i+6,s+i+1,strlen(s+i+1));
				memcpy(s+i,"&nbsp;",6);
				i += 6;
			}
		}
	}

	return(s);
}

/*
	EncodeUrl()

	Trasforma, all'interno dell'url, i caratteri speciali in sequenze esadecimali.
*/
LPSTR CUrl::EncodeUrl(LPSTR lpszUrl,UINT nSize)
{
	// il carattere '%' non viene incluso nella stringa delle esclusioni per permettere la
	// codifica corretta nel caso in cui l'url contenga una sequenza hex letterale, ad es.
	// con un url come http://localhost/pictures/mil/marine corp/Sunset%20Landing.jpg
	// (dove il file sul server *contiene* i caratteri '%20'
	return(EncodeHex(lpszUrl,nSize,"%|/?&=:.-_"));
}

/*
	DecodeUrl()

	Trasforma, all'interno dell'url, le sequenze esadecimali nei caratteri (ascii) corrispondenti.
*/
LPSTR CUrl::DecodeUrl(LPSTR lpszUrl)
{
	int i,n;

	for(i = 0,n = 0; lpszUrl[n]; ++i,++n)
	{
		lpszUrl[i] = lpszUrl[n];
		if(lpszUrl[i]=='%')
		{
			lpszUrl[i] = DecodeHex(&lpszUrl[n+1]);
			n += 2;
		}
		else if(lpszUrl[i]=='+')
			lpszUrl[i] = ' ';
	}
	
	lpszUrl[i] = '\0';

	return(lpszUrl);
}

/*
	EncodeHex()

	Converte i caratteri ascii in sequenze esadecimali.
*/
LPSTR CUrl::EncodeHex(LPSTR s,UINT n,LPCSTR pExclude/* = "" */,LPCSTR pInclude/* = "" */)
{
	char hex[5];
	int i,chr;

	// azzera lo spazio inutilizzato alla fine della stringa	
	i = strlen(s);
	if((n - i) > 0)
		memset(s+i,'\0',n-i);

	for(i = 0; i < (int)n && s[i]; i++)
	{
		chr = (int)s[i];
		
		// nessuna conversione
		if(((chr >= 65 && chr <= 90) || (chr >= 97 && chr <= 122) || (chr >= 48 && chr <= 57) || strchr(pExclude,chr)) && !strchr(pInclude,chr))
		{
			;
		}
		else // sposta il contenuto della stringa inserendo il carattere codificato
		{
			_snprintf(hex,sizeof(hex)-1,"%02X",(int)s[i]);
			if(strlen(s) + 4 < n-1)
			{
				memmove(s+i+3,s+i+1,strlen(s+i+1));
				s[i++] = '%';

				// trasforma CRLF in SPACE
				s[i++] = (hex[0]=='0' && hex[1]=='A') ? '2' : hex[0];
				s[i]   = (hex[0]=='0' && hex[1]=='D') ? '0' : hex[1];
			}
			else
				break;
		}
	}

	return(s);
}

/*
	DecodeHex()

	Converte le sequenze esadecimali in caratteri ascii.
*/
char CUrl::DecodeHex(LPSTR s)
{
	char chr;
	chr = (char)(s[0] >= 'A' ? ((s[0] & 0xdf) - 'A') + 10 : (s[0] - '0'));
	chr *= 16;
	chr += (char)(s[1] >= 'A' ? ((s[1] & 0xdf) - 'A') + 10 : (s[1] - '0'));
	return(chr);
}

/*
	ExtractLinksFromHtmlTag()

	Estrae il link contenuto nel tag html, restituendo TRUE se ha estratto il link da un tag riconosciuto,
	FALSE altrimenti.
	Il link estratto viene copiato nel buffer di input, il chiamante deve occuparsi di inserirlo nella lista
	interna.
	Deve ricevere in input il contenuto del tag senza i delimitatori ('<>').
	Da usare per estrarre il link dai tag semplici, ossia che contengono un solo link.
	Non considera il numero della porta.
*/
BOOL CUrl::ExtractLinksFromHtmlTag(LPCSTR lpcszHtmlTag,HTMLTAG_TYPE& htmltagtype,LPSTR lpszLink,UINT cbLinkSize)
{
	BOOL bTagFound = FALSE;
	LPSTR pTag = NULL;
	
	htmltagtype = UNKNOW_TAG;
	memset(lpszLink,'\0',cbLinkSize);

	// tag semplici e complessi
	int htmltagindex = 0;
	
	// salta i commenti
	if(lpcszHtmlTag[0]!='!')
		while(htmltagFullRef[htmltagindex].len!=0 && !bTagFound)
		{
			bTagFound = FALSE;

			if(!bTagFound)
			{
				int i;
				char szTag[MAX_HTMLTAG+1] = {0};
				
				for(i = 0; lpcszHtmlTag[i] && (ISALPHA(lpcszHtmlTag[i]) || lpcszHtmlTag[i]==' ') && i < sizeof(szTag)+1; i++)
				{
					szTag[i] = lpcszHtmlTag[i];
					if(lpcszHtmlTag[i]==' ')
						break;
				}
				bTagFound = stricmp(szTag,htmltagFullRef[htmltagindex].name)==0;
				if(bTagFound)
					pTag = (char*)lpcszHtmlTag + i;
				else
					pTag = NULL;

				if(bTagFound)
				{
					if(strcmp(htmltagFullRef[htmltagindex].specifier,"")!=0)
					{
						bTagFound = (pTag = stristr(pTag,htmltagFullRef[htmltagindex].specifier))!=NULL;
						if(bTagFound)
						{
							if(htmltagFullRef[htmltagindex].len > 0)
								pTag += htmltagFullRef[htmltagindex].len;
							else
								pTag = NULL;
							htmltagtype = htmltagFullRef[htmltagindex].type;
						}
					}
					else
					{
						if(htmltagFullRef[htmltagindex].len > 0)
							pTag += htmltagFullRef[htmltagindex].len;
						else
							pTag = NULL;
						htmltagtype = htmltagFullRef[htmltagindex].type;
					}
				}
			}

			htmltagindex++;
		}

	// estrae il link contenuto nel tag
	// se si tratta di un tag complesso non effettua nessuna estrazione (il chiamante analizzera' il contenuto)
	BOOL bBlockLevelElement = FALSE;
	switch(htmltagtype)
	{
		case BEGINSCRIPT_TAG:
		case BEGINSCRIPTLANGUAGE_TAG:
		case BEGINSCRIPTSRC_TAG:
		case BEGINNOSCRIPT_TAG:
		case BEGINDIV_TAG:
		case BEGINSPAN_TAG:
		case BEGINOBJECT_TAG:
		case BEGINAPPLET_TAG:
			bBlockLevelElement = TRUE;
			break;
	}
	if(bTagFound && pTag && !bBlockLevelElement)
	{
		// elimina quanto presente tra il tag e l'uguale
		while(*pTag && *pTag!='=')
			pTag++;

		// elimina l'uguale
		pTag++;
		
		// elimina gli spazi dopo l'uguale
		while(*pTag && (ISSPACE(*pTag) || *pTag=='\r' || *pTag=='\n'))
			pTag++;

		// ricava il delimitatore utilizzato, gestisce le forme seguenti:
		// href="/dir/file.htm"
		// href='/dir/file.htm'
		// href=/dir/file.htm
		char delim = ' ';
		if(*pTag=='\"' || *pTag=='\'')
		{
			delim = *pTag;

			// elimina il delimitatore
			while(*pTag && *pTag==delim)
				pTag++;

			// elimina gli spazi tra il delimitatore ed il contenuto
			while(*pTag && (ISSPACE(*pTag) || *pTag=='\r' || *pTag=='\n'))
				pTag++;
		}

		// copia il contenuto del tag fino ad incontrare il delimitatore finale
		int n = 0;
		while(*pTag && *pTag!=delim && n < (int)cbLinkSize-1)
			lpszLink[n++] = *pTag++;
	}

	// per non considerare costrutti come il seguente (ossia senza link):
	// <span id=AdBanner4>
	if(lpszLink[0]=='\0' || strcmp(lpszLink,"")==0)
		bTagFound = FALSE;

	return(bTagFound && pTag);
}

/*
	ExtractLinksFromBuffer()

	Estrae i links presenti nel buffer, restituendo il numero di links estratti/inseriti nella lista.
	Il link estratto viene copiato inserito automaticamente nella lista interna.
	Deve ricevere in input il contenuto del tag senza i delimitatori ('<>').
	Da usare per estrarre i links da porzioni di codice HTML che costituiscono il contenuto di tag complessi,
	come <SCRIPT> o <DIV> (ossia di tag che al loro interno possono contenere altri tag).
	Non considera il numero della porta.
*/
UINT CUrl::ExtractLinksFromBuffer(LPSTR szBuffer,URL* pBaseUrl,URL* pUrl,DWORD dwFlags)
{
	char* pBuffer;
	char szLink[MAX_URL+1];
	BOOL bTagFound = FALSE;
	char* pTag = NULL;
	UINT nLinks = 0;

	// il controllo sulle stringhe da escludere viene fatto da AddLink(), qui viene inserito perche' l'url
	// potrebbe essere del tipo <a href="javascript:void(null)" OnMouseOver('file.htm') ...> in tal caso,
	// dato che ad AddLink() verrebbe passato solo file.htm, non funzionerebbe l'esclusione dei javascript
	// il problema si presenta se il buffer contiene altri tag con link che non siano javascript (al momento
	// esclude l'intero contenuto)
	if(!(dwFlags & CURL_FLAG_ENABLE_JAVASCRIPT))
	{
		if(stristr(szBuffer,"javascript"))
			return(nLinks);
	}

	// carica la lista se necessario
	LoadHtmlFileTypeList();

	// per ognuno dei files riconosciuti come HTML cerca l'estensione relativa e
	// se la trova estrae quanto contenuto tra apici considerandolo come una url
	ITERATOR iter;
	FILETYPE* h;
	if((iter = m_HtmlFileTypeList.First())!=(ITERATOR)NULL)
	{
		do
		{
			if((h = (FILETYPE*)iter->data)!=(FILETYPE*)NULL)
			{
				char* p;
				pBuffer = szBuffer;
				while((p = stristr(pBuffer,h->ext))!=NULL)
				{
					int i = 0;
					int len = p - pBuffer;
					pBuffer = p + h->len;
					while(*p && *p!='\'' && *p!='\"' && *p!='>' && *p!='<' && *p!=' ' && *p!='\r' && *p!='\n' && len > 0)
					{
						p--;
						len--;
					}
					
					char delim = *p;
					memset(szLink,'\0',sizeof(szLink));
					
					if(len > 0 && *p)
					{
						p++;
						while(*p && *p!=delim && *p!='\'' && *p!='\"' && *p!='<' && *p!='>' && *p!=' ' && *p!='\r' && *p!='\n' && i < sizeof(szLink)-1)
							szLink[i++] = *p++;
					}
					
					// inserisce il link nella lista interna
					if(szLink[0]!='\0')
					{
						TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CUrl::ExtractLinks(): link(complex tag 1): %s\n",szLink));
					
						if(AddLink(pBaseUrl,pUrl,szLink,dwFlags))
							nLinks++;
					}
				}
			}
			
			iter = m_HtmlFileTypeList.Next(iter);
		
		} while(iter!=(ITERATOR)NULL);
	}
	// idem come sopra per i file da riconoscere (definiti dal chiamante)
	FILETYPE* u;
	if((iter = m_UrlFileTypeList.First())!=(ITERATOR)NULL)
	{
		do
		{
			if((u = (FILETYPE*)iter->data)!=(FILETYPE*)NULL)
			{
				char* p;
				pBuffer = szBuffer;
				while((p = stristr(pBuffer,u->ext))!=NULL)
				{
					int i = 0;
					int len = p - pBuffer;
					pBuffer = p + u->len;
					while(*p && *p!='\'' && *p!='\"' && *p!='>' && *p!='<' && *p!=' ' && *p!='\r' && *p!='\n' && len > 0)
					{
						p--;
						len--;
					}
					
					char delim = *p;
					memset(szLink,'\0',sizeof(szLink));
					
					if(len > 0 && *p)
					{
						p++;
						while(*p && *p!=delim && *p!='\'' && *p!='\"' && *p!='<' && *p!='>' && *p!=' ' && *p!='\r' && *p!='\n' && i < sizeof(szLink)-1)
							szLink[i++] = *p++;
					}
					
					// inserisce il link nella lista interna
					if(szLink[0]!='\0')
					{
						TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CUrl::ExtractLinks(): link(complex tag 2): %s\n",szLink));

						if(AddLink(pBaseUrl,pUrl,szLink,dwFlags))
							nLinks++;
					}
				}
			}
			
			iter = m_UrlFileTypeList.Next(iter);
		
		} while(iter!=(ITERATOR)NULL);
	}

	// solo tag semplici
	int htmltagindex = 0;

	while(htmltagShortRef[htmltagindex].len!=0)
	{
		// cerca gli eventuali tag annidati estraendo i link in essi contenuti
		pBuffer = szBuffer;
		do {
			bTagFound = FALSE;
			memset(szLink,'\0',sizeof(szLink));

			if(!bTagFound)
			{
				bTagFound = (pTag = stristr(pBuffer,htmltagShortRef[htmltagindex].name))!=NULL;
				if(bTagFound)
				{
					if(strcmp(htmltagShortRef[htmltagindex].specifier,"")!=0)
					{
						bTagFound = (pTag = stristr(pTag,htmltagShortRef[htmltagindex].specifier))!=NULL;
						if(bTagFound)
							pTag += htmltagShortRef[htmltagindex].len;
					}
					else
					{
						pTag += htmltagShortRef[htmltagindex].len;
					}
				}
			}

			if(bTagFound)
			{
				// elimina quanto presente tra il tag e l'uguale
				while(*pTag && *pTag!='=')
					pTag++;

				// elimina l'uguale
				if(*pTag)
					pTag++;
				
				// elimina gli spazi dopo l'uguale
				while(*pTag && (ISSPACE(*pTag) || *pTag=='\r' || *pTag=='\n'))
					pTag++;

				// ricava il delimitatore utilizzato, gestisce le forme seguenti:
				// href="/dir/file.htm"
				// href='/dir/file.htm'
				// href=/dir/file.htm
				char delim = ' ';
				if(*pTag && (*pTag=='"' || *pTag=='\''))
				{
					delim = *pTag;

					// elimina il delimitatore
					while(*pTag && *pTag==delim)
						pTag++;

					// elimina gli spazi tra il delimitatore ed il contenuto
					while(*pTag && (ISSPACE(*pTag) || *pTag=='\r' || *pTag=='\n'))
						pTag++;
				}

				// copia il contenuto del tag fino ad incontrare il delimitatore finale
				int n = 0;
				while(*pTag && *pTag!=delim && n < sizeof(szLink)-1)
					szLink[n++] = *pTag++;

				// se si tratta di un link javascript che viene composto dinamicamente, l'url potrebbe non
				// essere completa, come in document.write('<img src="http://<...>?') dove manca il \" finale
				// dato che l'url che referenzia il cgi viene costruita dinamicamente
				// in tal caso deve eliminare quanto presente dopo l'ultimo delimitatore
				if(szLink[0]!='\0')
				{	
					char* p = strchr(szLink,'\"');
					if(!p)
						p = strchr(szLink,'\'');
					if(p)
						*p = '\0';

					int len = strlen(szLink);
					int pos = strcspn(szLink,";=");

					TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CUrl::ExtractLinks(): link(complex tag 3): %s,%d\n",szLink,pos));
					TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CUrl::ExtractLinks(): len,pos: %d,%d\n",len,pos));

					// inserisce il link nella lista interna
					if(len > 5 && pos >= len)
					if(AddLink(pBaseUrl,pUrl,szLink,dwFlags))
						nLinks++;
				}

				pBuffer = pTag;
			}
		} while(bTagFound);

		htmltagindex++;
	}

	return(nLinks);
}

/*
	AddLink()

	Aggiunge il link alla lista interna.
	Si occupa di controllare il pattern per le esclusioni e di normalizzare l'url.
	Non considera il numero della porta.
*/
BOOL CUrl::AddLink(URL* pBaseUrl,URL* pUrl,LPCSTR lpcszLink,DWORD dwFlags)
{
	BOOL bAdded = FALSE;
	BOOL bIsValidUrl = TRUE;
	char szUrl[MAX_URL+1];

	// lavora sulla copia locale
	strcpyn(szUrl,lpcszLink,sizeof(szUrl));

	// tronca le url composte (#)
	char* p = strchr(szUrl,'#');
	if(p)
		*p = '\0';

	// converte gli eventuali caratteri a cazzo di cane
	while((p = strchr(szUrl,'\\'))!=NULL)
		*p = '/';
	while((p = strchr(szUrl,'\r'))!=NULL)
		*p = ' ';
	while((p = strchr(szUrl,'\n'))!=NULL)
		*p = ' ';

	// alcuni cazzoni referenziano i files con il punto iniziale, come in ./html/top.html
	if(szUrl[0]=='.')
	{
		int i = strlen(szUrl)-1;
		memmove(szUrl,szUrl+1,i);
		szUrl[i] = '\0';
	}

	// verifica se l'url contiene una delle stringhe da escludere
	if(!(dwFlags & CURL_FLAG_ENABLE_JAVASCRIPT))
	{
		if(stristr(szUrl,"javascript"))
			bIsValidUrl = FALSE;
	}

	// elimina le url che contengono solo un estensione (.ext)
	if(bIsValidUrl)
	{
		if(szUrl[0]=='.' && szUrl[1]!='.' && szUrl[1]!='/')
			bIsValidUrl = FALSE;
	}
	
	if(bIsValidUrl)
	{
		// url
		if(IsUrl(szUrl))
		{
			// elimina le url javascript, non il codice ma l'url (come in <a href=""javascript:void(null)">)
			if(IsUrlType(szUrl,JAVASCRIPT_URL))
				bIsValidUrl = FALSE;
		}
		else // directory(?) o qualsiasi altra cosa (cgi, etc.)
		{
			char szTempUrl[MAX_URL+1] = {0};
			
			// se il link inizia con '/' lo copia tale e quale, altrimenti recupera la directory dell'url base
			_snprintf(szTempUrl,
					sizeof(szTempUrl)-1,
					"%s%s%s%s",
					pBaseUrl->host[0]!='\0' ? pBaseUrl->host : pUrl->host,
					szUrl[0]=='/' ? "" : (pBaseUrl->dir[0]!='\0' ? pBaseUrl->dir : pUrl->dir),
					szUrl[0]=='/' ? "" : (pBaseUrl->dir[0]!='\0' ? (pBaseUrl->dir[strlen(pBaseUrl->dir)-1]=='/' ? "" : "/") : (pUrl->dir[strlen(pUrl->dir)-1]=='/' ? "" : "/")),
					szUrl
					);

			strcpyn(szUrl,szTempUrl,sizeof(szUrl));
		}
	}

	if(bIsValidUrl)
	{
		// risolve i riferimenti ./ e ../
		StripParentFromUrl(szUrl,sizeof(szUrl));

		// inserisce il link nella lista (controllando che non esista gia')
		ITERATOR iter;
		HREF* href;
		BOOL bFound = FALSE;
		if((iter = m_HRefList.First())!=(ITERATOR)NULL)
		{
			do
			{
				if((href = (HREF*)iter->data)!=(HREF*)NULL)
					if(strcmp(szUrl,href->href)==0)
					{
						bFound = TRUE;
						break;
					}
				iter = m_HRefList.Next(iter);
			} while(iter!=(ITERATOR)NULL);
		}
		if(!bFound)
		{
			href = (HREF*)m_HRefList.Add();
			if(href)
			{
				strcpyn(href->href,szUrl,MAX_URL+1);
				bAdded = TRUE;
			}
		}
	}

	return(bAdded);
}

/*
	CreatePathNameFromHttpUrl()

	Compone il pathname per l'url specificata
	Notare che la creazione su disco del pathname non e' automatica, ma dipende del valore del parametro relativo.
	La directory iniziale, se passata, deve essere terminata con '\'.
	Considera il numero della porta.
*/
BOOL CUrl::CreatePathNameFromHttpUrl(LPCSTR lpcszUrl,LPSTR lpszPathname,UINT cbPathnameSize,LPCSTR lpcszInitialDir/*=NULL*/,BOOL bCreatePathname/* = TRUE*/)
{
	BOOL bExist = TRUE;
	int nLen;
	char szUrl[MAX_URL+1];
	URL Url;
	
	// divide l'url nei tre componenti
	strcpyn(szUrl,lpcszUrl,sizeof(szUrl));
	DecodeUrl(szUrl);
	SplitUrl(szUrl,&Url);

	// imposta la directory di base (deve terminare con '\')
	if(lpcszInitialDir)
	{
		strcpyn(lpszPathname,lpcszInitialDir,cbPathnameSize);
		::EnsureBackslash(lpszPathname,cbPathnameSize);
	}
	else
		strcpy(lpszPathname,".\\");

	// imposta il nome host + numero porta
	nLen = strlen(lpszPathname);
	int nUrlLen = 0;
	GetUrlType(Url.host,&nUrlLen);
	if(*(Url.host+nUrlLen)=='/')
		while(*(Url.host+nUrlLen) && *(Url.host+nUrlLen)=='/')
			nUrlLen++;
	_snprintf(
			lpszPathname+nLen,
			cbPathnameSize-nLen-1,
			"%s_%ld",
			Url.host+nUrlLen,
			Url.port <= 0 ? HTTP_DEFAULT_PORT : Url.port
			);
	
	// copia la directory
	nLen = strlen(lpszPathname);
	strcpyn(lpszPathname+nLen,Url.dir,cbPathnameSize-nLen);

	// effettua un controllo minimo sul nome della directory da creare
	char* p = (lpszPathname[1]==':' && lpszPathname[2]=='\\') ? lpszPathname+2 : lpszPathname;
	if(strstr(p,"..") || strstr(p,"\\\\") || strstr(p,":\\"))
	{
		bExist = FALSE;
	}
	else
	{
		// trasforma gli slash ed i caratteri speciali
		EnsureValidFileName(lpszPathname,cbPathnameSize);

		// la directory deve terminare con '\'
		::EnsureBackslash(lpszPathname,cbPathnameSize);

		// se deve creare il pathname su disco
		if(bCreatePathname)
		{
			// controlla che la directory non esista gia'
			if(::GetFileAttributes(lpszPathname)==0xFFFFFFFF)
			{
				bExist = m_findFile.CreatePathName(lpszPathname,cbPathnameSize);
			}
		}
		else
			bExist = TRUE;
	}

	return(bExist);
}

/*
	LocalFileToUrl()

	Converte il nome del file locale in un url.
	C:\WallPaper\Download\127.0.0.1\pierge\index.html -> file://C|/WallPaper/Download/127.0.0.1/pierge/index.html
	index.html -> file://<current drive>|/<current directory>/index.html
*/
LPSTR CUrl::LocalFileToUrl(LPCSTR lpcszFile,LPSTR lpszUrl,UINT nUrlSize)
{
	char szFileName[_MAX_FILEPATH+1];

	if(lpcszFile[1]!=':' || strchr(lpcszFile,'\\')==NULL)
	{
		memset(szFileName,'\0',sizeof(szFileName));
		::GetCurrentDirectory(sizeof(szFileName),szFileName);
		int i = strlen(szFileName);
		_snprintf(szFileName+i,
				sizeof(szFileName)-(i+1),
				"\\%s",
				lpcszFile);
	}
	else
	{
		strcpyn(szFileName,lpcszFile,sizeof(szFileName));
	}

	// IExplorer: "file://", Netscape: "file:///" (riconosce anche la precedente)
	// IExplorer: "file://C:", Netscape: "file:///C|"
	memset(lpszUrl,'\0',nUrlSize);
	_snprintf(lpszUrl,
			nUrlSize-1,
//			"file://%c|%s",
			"file://%c:%s",
			szFileName[0],
			szFileName+2);
	
	for(int i = 0; lpszUrl[i]; i++)
		if(lpszUrl[i]=='\\')
			lpszUrl[i] = '/';

	// occhio che la zoccola di IExplorer non riconosce l'url se gli spazi vengono convertiti in '%20'
	//EncodeUrl(lpszUrl,nUrlSize);
		
	return(lpszUrl);
}

/*
	EnsureValidFileName()

	Converte / in \ cosi' come i caratteri non consentiti in _.
	Non considera il carattere : perche' prende in considerazione i patnames.
*/
LPSTR CUrl::EnsureValidFileName(LPSTR lpszFile,UINT cbFileSize,BOOL bConvertSlasch/* = TRUE*/,LPCSTR lpcszExt/* = NULL*/)
{
	int i = 0;

	for(i=0; lpszFile[i]; i++)
	{	
		if(lpszFile[i]=='/')
		{
			lpszFile[i] = bConvertSlasch ? '\\' : '_';
		}
		else if(lpszFile[i]=='*' || lpszFile[i]=='?' || lpszFile[i]=='&' || lpszFile[i]=='=')
		{
			lpszFile[i] = '_';
		}
	}

	if(lpcszExt && i > 0)
	{
		if(lpszFile[i-1]=='_' || lpszFile[i-1]=='.')
			lpszFile[i-1] = '\0';

		if(strlen(lpszFile)+strlen(lpcszExt) < cbFileSize-1)
			strcat(lpszFile,lpcszExt);
	}

	return(lpszFile);
}

/*
	EnsureBackslash()

	Si assicura che il pathname termini con '\'.
*/
LPSTR CUrl::EnsureBackslash(LPSTR lpszFile,UINT nFileSize)
{
	int i = strlen(lpszFile);
	if(lpszFile[i-1]!='\\')
		if(i < (int)nFileSize-1)
			strcatn(lpszFile,"\\",nFileSize);
		else
			lpszFile[i-1] = '\\';

	return(lpszFile);
}

/*
	GetHttpUrl()

	Si collega all'host scaricando l'url in locale (l'url deve iniziare con http://).
	Restituisce 1 per operazione completata, 0 se viene interrotto (quando la callback restituisce
	un valore diverso da 0).
	Per sapere se si sono verificati errori NON usare il codice di ritorno ma controllare i campi
	della struttura HTTP_CONNECTION.
	Considera il numero della porta.
*/
UINT CUrl::GetHttpUrl(	CHttpConnection*	pHttpConnection,
					LPCSTR			lpcszUrl,
					BOOL*			bFileDirMismatched,
					LPSTR			lpszTranslatedUrl,
					UINT				cbTranslatedUrl,
					LPCSTR			lpcszDownloadPath,
					LPSTR			lpszLocalFile,
					UINT				cbLocalFileSize,
					CONTENTTYPE*		pCtype,
					BOOL				bForceFileName/* = FALSE */,
					FPCALLBACK		lpfnCallback/* = NULL */,
					LPVOID			lpVoid/* = NULL */,
					BOOL				bDownload/* = TRUE */)
{
	BOOL bHtmlFileWithoutExt = FALSE;
	UINT nTranslated = 1;
	URL Url = {0};
	char szUrl[MAX_URL+1] = {0};
	char szHost[MAX_URL+1] = {0};
	char szBuffer[MAX_DOWNLOAD_BUFFER_SIZE+1] = {0};
	char szLocalPath[_MAX_FILEPATH+1] = {0};
	char szDefaultHtml[_MAX_FNAME+1] = {0};
	BOOL bConnected = FALSE;
	int nRetry = 0;
	BOOL bMimeType = FALSE;
	CRegistry registry;
	char szLastModified[128] = {0};
	QWORD qwLength = 0L;
	int nWSAError = 0;

	if(bFileDirMismatched)
		*bFileDirMismatched = FALSE;

	// imposta l'oggetto per la connessione
	// (resettare solo l'oggetto e non la connessione perche' il chiamante potrebbe aver impostato i campi)
	HTTPOBJECT* pHttpObject = pHttpConnection->GetObject();
	pHttpConnection->ResetObject();
	pHttpConnection->ResetSocketError();
	pHttpConnection->ResetHttpError();

	// per la Content-Type
	memset(pCtype,'\0',sizeof(CONTENTTYPE));
	
	// lavora sulla copia locale dell'url
	strcpyn(szUrl,lpcszUrl,sizeof(szUrl));

	// chiama la callback per impostare il nome di default per il file locale (.html) nel caso in cui il server non lo specifichi
	pHttpObject->stat = HTTPOBJECTSTAT_DEFAULT_HTMLFNAME;
	strcpy(pHttpObject->defaultfname,INDEX_HTML);
	if(lpfnCallback)
	{
		if(lpfnCallback(pHttpConnection,lpVoid)==IDCANCEL)
		{
			nTranslated = 0;
			TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CUrl::GetHttpUrl(): callback breaks! (%d)\n",__LINE__));
			goto done;
		}
	}
	strcpyn(szDefaultHtml,pHttpObject->defaultfname,sizeof(szDefaultHtml));

parse_url:

	// divide l'url nei suoi componenti
	SplitUrl(szUrl,&Url);
	if(Url.port <= 0)
		Url.port = pHttpConnection->GetPortNumber();

//parsed:

	// fino a che non viene risolto il nome host con il collegamento, imposta su quanto ricevuto
//	pHttpConnection->SetHostAddr(Url.host);
//	pHttpConnection->SetHostName(Url.host);

	// normalizza il nome dell'host, eliminando http:// se presente
	strcpyn(szHost,Url.host,sizeof(szHost));
	int nHostLen = strlen(szHost);
	if(IsUrlType(szHost,HTTP_URL))
	{
		memmove(szHost,szHost+7,nHostLen-7);
		szHost[nHostLen-7] = '\0';
	}

	// imposta il pathname per il file locale (deve terminare con '\')
	if(lpcszDownloadPath)
		strcpyn(szLocalPath,lpcszDownloadPath,sizeof(szLocalPath));
	else
		strcpy(szLocalPath,".\\");
	::EnsureBackslash(szLocalPath,sizeof(szLocalPath));

	// fix anti-cazzone: elimina dal pathname (locale) il nome del file interpretato come directory
	if(bHtmlFileWithoutExt)
	{
		char* p = strstr(szLocalPath,Url.file);
		if(p)
			*p = '\0';
	}
	
	// aggiunge il nome del file al pathname di cui sopra
	// se il parametro relativo viene viene passato a FALSE usa il nome di default (ricavato dall'url),
	// con TRUE usa il nome che e' stato passato come parametro
	if(bForceFileName)
	{
		strcatn(szLocalPath,lpszLocalFile,sizeof(szLocalPath));
	}
	else
	{
		// se non e' presente un nome file (directory, cgi, etc.) imposta il nome del file locale con quello di default
		if(Url.file[0]=='\0')
		{
			// se si tratta di un cgi il nome file e' vuoto, imposta quindi il nome del file locale
			// con la stringa utilizzata per la query
			// non mettere index.html perche' il contenuto potrebbe essere qualsiasi cosa (img, etc.)
			//strcatn(szLocalPath,Url.query[0]!='\0' ? Url.query : INDEX_HTML,sizeof(szLocalPath));
			if(IsCgiUrl(lpcszUrl))
				strcatn(szLocalPath,Url.query[0]!='\0' ? Url.query : "untitled",sizeof(szLocalPath));
			else
				strcatn(szLocalPath,szDefaultHtml,sizeof(szLocalPath));

			// trasforma tutto in _
			EnsureValidFileName(szLocalPath,sizeof(szLocalPath),FALSE);
			
			// trasforma tutto in %..
			//EncodeHex(szLocalPath,sizeof(szLocalPath),"\\:.","/=?&");
		}
		else
		{
			strcatn(szLocalPath,Url.file,sizeof(szLocalPath));
		}
	}
	strcpyn(lpszLocalFile,szLocalPath,cbLocalFileSize);

	// si assicura che il nome file sia valido
	EnsureValidFileName(lpszLocalFile,cbLocalFileSize);

	// fix anti-cazzone: aggiunge al file interpretato come directory l'estensione di default
	// non e' detto che il file sia in formato html, ma ci prova comunque
	if(bHtmlFileWithoutExt)
		strcatn(lpszLocalFile,DEFAULT_HTML_EXT,cbLocalFileSize);

	// se deve scaricare l'url (e non esplorarla)
	if(bDownload)
	{
		// controlla se il file gia' esiste
		// il download condizionale viene effettuato solo se viene impostato il flag relativo
		// per i cgi viene controllata solo l'esistenza su disco (il file locale risultante potrebbe avere un
		// estensione differente a quella dell'url a seconda della content-type), per i files rimanenti, se il
		// file esiste, utilizza l'header if-modified-since
		memset(szLastModified,'\0',sizeof(szLastModified));
		if(pHttpConnection->GetSkipExisting())
		{
			// se il nome del file locale coincide con quello di default deve scaricare comunque il file perche'
			// il contenuto potrebbe essere differente (ad es. se il server restituisce il contenuto della directory)
			// in caso contrario esegue il controllo su data/ora, sempre che non venga forzato lo skip per esistenza
			int nLen = strlen(lpszLocalFile);
			int nDefaultLen = strlen(szDefaultHtml);
			if(memcmp(lpszLocalFile+(nLen-nDefaultLen),szDefaultHtml,nDefaultLen)!=0)
			{
				// verifica l'esistenza del file, se si tratta di un cgi aggiunge *.* perche' in tal caso
				// l'estensione del file locale puo' variare a seconda del contenuto della Content-Type
				BOOL bExist;
				char szFileName[_MAX_FILEPATH+1];
				strcpyn(szFileName,lpszLocalFile,sizeof(szFileName));
				if(IsCgiUrl(szUrl))
				{
					strcatn(szFileName,"*.*",sizeof(szFileName));
					bExist = m_findFile.ExistEx(szFileName);
				}
				else
					bExist = m_findFile.Exist(szFileName);

				// per i cgi o se viene forzato lo skip per esistenza
				//if(bExist && pHttpConnection->GetForceSkipExisting())
				if((bExist && IsCgiUrl(szUrl)) || (bExist && pHttpConnection->GetForceSkipExisting()))
				{
					if(lpszTranslatedUrl)
					{
						_snprintf(lpszTranslatedUrl,
								cbTranslatedUrl-1,
								"%s:%ld%s%s%s%s%s%s",
								Url.host,
								Url.port,
								Url.dir,
								Url.dir[strlen(Url.dir)-1]=='/' ? "" : "/",
								(Url.file[0]=='\0' && Url.cgi[0]=='\0' && Url.delim=='\0' && Url.query=='\0') ? szDefaultHtml : Url.file,
								Url.cgi,
								Url.delim,
								Url.query);
						
						DecodeUrl(lpszTranslatedUrl);
					}

					pHttpConnection->SetHttpErrorCode(HTTP_STATUS_NOT_MODIFIED);

					return(nTranslated);
				}
				else if(bExist)
				{
					HANDLE hHandle;
					if((hHandle = ::CreateFile(szFileName,GENERIC_READ,0,NULL,OPEN_EXISTING,0,NULL))!=INVALID_HANDLE_VALUE)
					{
						FILETIME filetime;
						SYSTEMTIME systemtime;

						// ricava la data/ora UTC del file (ossia assolute, GMT) e la dimensione
						::GetFileTime(hHandle,NULL,NULL,&filetime);
						qwLength = ::GetFileSizeExt(hHandle);
						::CloseHandle(hHandle);

						// converte in formato di sistema - non convere in locale perche' il campo
						// last-modified viene inviato al server in formato in formato UTC
						::FileTimeToSystemTime(&filetime,&systemtime);
						
						// formatta la data/ora del file nel formato GMT (Day, dd Mon yyyy hh:mm:ss GMT)
						CDateTime datetime(GMT_SHORT);
						datetime.SetYear(systemtime.wYear);
						datetime.SetMonth(systemtime.wMonth);
						datetime.SetDay(systemtime.wDay);
						datetime.SetHour(systemtime.wHour);
						datetime.SetMin(systemtime.wMinute);
						datetime.SetSec(systemtime.wSecond);
						strcpyn(szLastModified,datetime.GetFormattedDate(FALSE),sizeof(szLastModified));
					}
				}
			}
		}
	}

	// ricompone l'url per la richiesta http (senza host/porta)
	// per url come http://wickedweasel.com/es/contributor_galleries/2448-lacey aggiungendo un / alla fine il 
	// server restituisce moved permanently, verificare se l'eliminazione del / finale comporta controindicazioni
	_snprintf(szUrl,
			sizeof(szUrl)-1,
			"%s%s%s%s%s%s",
			Url.dir,
			//Url.dir[strlen(Url.dir)-1]=='/' ? "" : "",
			Url.dir[strlen(Url.dir)-1]=='/' ? "" : "/",
			Url.file,
			Url.cgi,
			Url.delim,
			Url.query);

	EncodeUrl(szUrl,sizeof(szUrl));

	CHostNameList* pProxyExcludeList = pHttpConnection->GetProxyExcludeList();
	BOOL bUseProxyForThisDomain = TRUE;
	if(pProxyExcludeList->Count() > 0)
	{
		ITERATOR iter;
		HOSTNAME* n;
		if((iter = pProxyExcludeList->First())!=(ITERATOR)NULL)
		{
			do
			{
				if((n = (HOSTNAME*)iter->data)!=(HOSTNAME*)NULL)
					if(stricmp(szHost,n->name)==0)
					{
						bUseProxyForThisDomain = FALSE;
						break;
					}
				iter = pProxyExcludeList->Next(iter);
			} while(iter!=(ITERATOR)NULL);
		}
	}

	// costruisce la GET (se si collega tramite proxy deve specificare url completa + numero porta + autenticazione)
	int nGetLen = 0;
	memset(szBuffer,'\0',sizeof(szBuffer));
	if(pHttpConnection->GetProxyAddress() && pHttpConnection->GetProxyPortNumber() > 0 && bUseProxyForThisDomain)
	{
		nGetLen += _snprintf(szBuffer+nGetLen,
						sizeof(szBuffer) - (1 + nGetLen),
						"%s http://%s:%ld%s HTTP/1.0\r\n"
						"Host: %s:%ld\r\n",
						bDownload ? "GET" : "HEAD",
						szHost,
						pHttpConnection->GetPortNumber(),
						szUrl,
						szHost,
						pHttpConnection->GetPortNumber()
						);
		
		if(strcmp(pHttpConnection->GetProxyAuth(),"")!=0 && nGetLen > 0)
		{
			nGetLen += _snprintf(	szBuffer+nGetLen,
								sizeof(szBuffer) - (1 + nGetLen),
								"Proxy-Authorization: Basic %s\r\n",
								pHttpConnection->GetProxyAuth()
								);
		}
	}
	else
	{
		char szHttpPortNumber[16] = {0};
		if(pHttpConnection->GetPortNumber()!=HTTP_DEFAULT_PORT)
			_snprintf(szHttpPortNumber,sizeof(szHttpPortNumber)-1,":%ld",pHttpConnection->GetPortNumber());
		
		nGetLen += _snprintf(szBuffer+nGetLen,
						sizeof(szBuffer) - (1 + nGetLen),
						"%s %s HTTP/1.0\r\n"
						"Host: %s%s\r\n",
						bDownload ? "GET" : "HEAD",
						szUrl,
						szHost,
						szHttpPortNumber
						);
	}

	// inserisce gli headers di default
	nGetLen += _snprintf(szBuffer+nGetLen,
					sizeof(szBuffer) - (1 + nGetLen),
					"Accept: *.*, */*\r\n"
					);

	// se deve scaricare l'url (e non esplorarla)
	if(bDownload)
	{
		// inserisce l'header per il download condizionale in base alla data/ora dell'ultimo accesso/dimensione file
		if(szLastModified[0]!='\0')
		{
			nGetLen += _snprintf(szBuffer+nGetLen,
							sizeof(szBuffer) - (1 + nGetLen),
							"If-Modified-Since: %s; length=%I64u\r\n",
							szLastModified,
							qwLength
							);
			memset(szLastModified,'\0',sizeof(szLastModified));
		}
	}

	// inserisce gli headers definiti dal chiamante
	HTTPHEADER* httpheader;
	while((httpheader = pHttpConnection->EnumHeaders())!=(HTTPHEADER*)NULL)
	{
		nGetLen += _snprintf(szBuffer+nGetLen,
						sizeof(szBuffer) - (1 + nGetLen),
						"%s: %s\r\n",
						httpheader->name,
						httpheader->value
						);
	}

	// aggiunge gli eventuali cookies
	if(pHttpConnection->CookiesAllowed())
	{
		// carica i cookies relativi al dominio/pathname
		if(pHttpConnection->GetCookies(Url.host,Url.dir) > 0)
		{
			HTTPHEADER* httpheader;
			char szCookie[COOKIE_MAX_LEN/*COOKIE_REALLY_MAX_LEN*/+1] = {0};
			int nCookieLen = 0;
			BOOL bHaveCookies = FALSE;
			BOOL bAllowCookie = TRUE;

			// scorre i cookies caricati sopra verificando se devono essere inseriti nell'header http della richiesta
			while((httpheader = pHttpConnection->EnumCookies())!=(HTTPHEADER*)NULL)
			{
				bAllowCookie = TRUE;

				// chiama la callback (cookie)
				// IDOK,IDYES = invia il cookie, IDNO = non lo invia, IDCANCEL = termina
				pHttpObject->stat = HTTPOBJECTSTAT_GET_COOKIE;
				if(lpfnCallback)
				{
					strcpyn(pHttpObject->cookie.value,httpheader->value,COOKIE_VALUE_LEN+1);
					UINT nRet = lpfnCallback(pHttpConnection,lpVoid);
					if(nRet==IDYES || nRet==IDOK)
						bAllowCookie = TRUE;
					else if(nRet==IDNO)
						bAllowCookie = FALSE;
					else if(nRet==IDCANCEL)
					{
						bAllowCookie = FALSE;
						nTranslated = 0;
						goto done;
					}
				}

				if(bAllowCookie)
				{
					nCookieLen += _snprintf(	szCookie+nCookieLen,
										sizeof(szCookie) - (1 + nCookieLen),
										" %s;",
										httpheader->value
										);
					bHaveCookies = TRUE;
				}
			}
			
			if(bHaveCookies)
			{
				nGetLen += _snprintf(szBuffer+nGetLen,
								sizeof(szBuffer) - (1 + 2 + nGetLen),
								"Cookie:%s\r\n",
								szCookie
								);
			}
		}
	}

	// termina la richesta
	strcatn(szBuffer,"\r\n",sizeof(szBuffer));

	// apre il socket collegandosi all'host/porta
	if(pHttpConnection->Open())
	{
		// numero di tentativi
		if(nRetry==0)
		{
			nRetry = pHttpConnection->GetConnectionRetry();
			nRetry = nRetry <= 0 ? 1 : nRetry;
		}
		while(--nRetry >= 0)
		{
			// controlla se deve usare il proxy o collegarsi direttamente
			if(pHttpConnection->GetProxyAddress() && pHttpConnection->GetProxyPortNumber() > 0 && bUseProxyForThisDomain)
				bConnected = pHttpConnection->Connect(pHttpConnection->GetProxyAddress(),pHttpConnection->GetProxyPortNumber());
			else
				bConnected = pHttpConnection->Connect(szHost,pHttpConnection->GetPortNumber());
			
			if(!bConnected)
				::Sleep(500L);
			else
				break;
		}

		if(bConnected)
		{
			int nRecv;
			char szHttpCode[HTTP_MAX_CONTENT+1];
			char szHttpDesc[HTTP_MAX_CONTENT+1];

			// una volta collegato risolve il nome host
			char szHostAddr[HOSTNAME_SIZE+1];
			strcpyn(szHostAddr,pHttpConnection->GetProxyAddress() && pHttpConnection->GetProxyPortNumber() > 0 ? pHttpConnection->GetProxyAddress() : szHost,sizeof(szHostAddr));
			if(isdigit(szHostAddr[0]))
			{
				pHttpConnection->SetHostAddr(szHostAddr);
				pHttpConnection->SetHostName(pHttpConnection->GetHostByAddr(szHostAddr));
			}
			else
			{
				pHttpConnection->SetHostName(szHostAddr);
				pHttpConnection->SetHostAddr(pHttpConnection->GetHostByName(szHostAddr));
			}

			// resetta il codice d'errore per il test alla fine del download
			pHttpConnection->ResetSocketError();

			// verifica se deve usare il timeout winsock
			if(pHttpConnection->GetConnectionTimeout()!=0)
				pHttpConnection->SetTimeout(pHttpConnection->GetConnectionTimeout());

			TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CUrl::GetHttpUrl(): Send():\n[%s]\n",szBuffer));

			// invio dati
			if(pHttpConnection->Send(szBuffer,strlen(szBuffer))!=SOCKET_ERROR)
			{
				// Send() e Receive() a raffica scasinano, sopratutto con ICY
				::Sleep((DWORD)pHttpConnection->GetDelay());
				
				// ricezione dati
				memset(szBuffer,'\0',sizeof(szBuffer));
				if((nRecv = pHttpConnection->Receive(szBuffer,sizeof(szBuffer)-1)) > 0)
				{
					TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CUrl::GetHttpUrl(): Receive():\n[%s]\n",szBuffer));
					
					// estrae la risposta dell'host
					pHttpConnection->ResetHttpError();
					int i = 0;
					char* p = szBuffer;
					for(; *p && *p!=' ';)
						p++;
					
					// codice numerico
					p++;
					for(i = 0; *p && isdigit(*p) && *p!=' ' && i < (sizeof(szHttpCode)-1); i++)
						szHttpCode[i] = *p++;
					szHttpCode[i] = '\0';
					pHttpConnection->SetHttpErrorCode(atoi(szHttpCode));

#if 0
					// per url come: /~cazzonegrande/file_html
					// dove file_html e' un file .html senza estensione
					if(atoi(szHttpCode)==HTTP_STATUS_NOT_FOUND && strcmp(Url.file,"")==0)
					{
						char* p = strrchr(Url.dir,'/');
						if(p)
						{
							p++;
							if(p && strcmp(p,"")!=0)
							{
								// imposta il nome file
								// notare che qui non viene aggiunta l'estensione dato che sul server il file non ne ha
								// verra' invece aggiunta al nome file scaricato in locale e nel buffer per l'url translata
								strcpyn(Url.file,p,sizeof(Url.file));
								*p = '\0';
								
								// elimina dall'url ricevuta in input il nome file, lasciando solo la directory vera
								p = strstr(lpszUrl,Url.file);
								if(p)
									*p = '\0';
								
								// deve correggere anche la directory locale su cui viene salvata l'url
								p = strstr(lpszDownloadPath,Url.file);
								if(p)
									*p = '\0';

								// chiude la connessione, il goto salta prima dell'apertura
								pHttpConnection->Close();

								// imposta i flags anti cazzone
								bHtmlFileWithoutExt = TRUE;
								if(bFileDirMismatched)
									*bFileDirMismatched = TRUE;
								
								// ci riprova
								goto parsed;
							}
						}
					}
#endif

					// descrizione
					i = _snprintf(szHttpDesc,sizeof(szHttpDesc)-1,"HTTP %s - ",szHttpCode);
					p++;
					for(/*i=0*/; *p && *p!='\r' && *p!='\n' && i < (sizeof(szHttpDesc)-1); i++)
						szHttpDesc[i] = *p++;
					szHttpDesc[i] = '\0';
					pHttpConnection->SetHttpErrorString(szHttpDesc);

					// nessun errore http
					if(!pHttpConnection->IsHttpError())
					{
						// (inizio) controlli per il nome file locale (nomi incompleti o nomi diversi per url cgi)

						// fix anti-cazzone: nomi immagini senza estensione
						// un url come http://server/filenamewithoutext verrebbe tradotta in http://server/filenamewithoutext/index.html
						// quindi fa riferimento alla content-type inviata dal server verificando che l'estensione del file di output
						// corrisponda a quella del tipo mime, in caso contrario effettua la trasformazione
						char* pContentType = stristr(szBuffer,"Content-Type:");
						if(pContentType)
							pContentType += 13;
						if(pContentType)
							while(*pContentType && *pContentType==' ')
								pContentType++;
						if(pContentType)
						{
							// ricava dalla content-type inviata dal server il tipo (l'estensione) dell'oggetto ricevuto
							char* p;
							char szContentType[64] = {0};
							char szType[32] = {0};
							char szExt[32] = {0};
							strcpyn(szContentType,pContentType,sizeof(szContentType));
							if((p = strchr(szContentType,'\r'))!=NULL)
								*p = '\0';
							if((p = strchr(szContentType,'\n'))!=NULL)
								*p = '\0';
							strcpyn(szType,szContentType,sizeof(szType));
							if((p = strchr(szType,'/'))!=NULL)
								*p = '\0';
							if((p = strchr(szContentType,'/'))!=NULL)
							{
								szExt[0] = '.';
								strcpyn(szExt+1,p+1,sizeof(szExt)-1);
							}
							
							// per il chiamante
							strcpyn(pCtype->content,szContentType,HTTP_MAX_CONTENT+1);
							strcpyn(pCtype->ext,szExt,HTTP_MAX_CONTENT_EXT+1);
							
							// da testare...
							if(stristr(szContentType,"image/"))
							{
								// il nome file di output impostato in precedenza non
								// coincide con l'estensione specificata dalla content-type
								if(striright(lpszLocalFile,szExt)!=0)
								{
									BOOL bFixIt = TRUE;

									// per evitare il cambio (non necessario) in casi in cui l'estensione
									// specifica un sottoinsieme della content-type e viceversa
									if(bFixIt)
									{
										char* pExt = strrchr(lpszLocalFile,'.');
										if(pExt)
										{
											if(stristr(pExt,szExt) || stristr(pExt,szExt))
												bFixIt = FALSE;
										}
									}
									// per evitare il cambio (non necessario) in casi in cui l'estensione
									// e' diversa ma fa sempre riferimento allo stesso tipo (es. .jpg e .jpeg)
									if(bFixIt)
									{
										if(CImageObject::IsImageFile(lpszLocalFile))
											bFixIt = FALSE;
									}

									if(bFixIt)
									{
										// rimuove \index.html e aggiunge l'estensione specificata dalla content-type
										p = strrchr(lpszLocalFile,'\\');
										if(p)
										{
											*p = '\0';
											strcatn(lpszLocalFile,szExt,cbLocalFileSize);
										}
									}
								}
							}
							else if(stristr(szContentType,"audio/"))
							{
								// "audio/mpeg" o "audio/mpg" sta per .mp3
								if(stricmp(szExt,".mpeg")==0 || stricmp(szExt,".mpg")==0)
									strcpy(szExt,".mp3");
								
								// chiama la callback per informare che si tratta di uno stream audio
								char* pIcy = stristr(szBuffer,"icy-name:");
								char* pIce = stristr(szBuffer,"ice-audio-info:");
								if(pIcy || pIce)
								{
									pHttpObject->stat = pIce ? HTTPOBJECTSTAT_ICECAST_STREAM : HTTPOBJECTSTAT_SHOUTCAST_STREAM;
									if(lpfnCallback)
									{
										if(lpfnCallback(pHttpConnection,lpVoid)==IDCANCEL)
										{
											nTranslated = 0;
											TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CUrl::GetHttpUrl(): callback breaks! (%d)\n",__LINE__));
											goto done;
										}
									}
								}
								
								// il nome file di output impostato in precedenza non coincide con l'estensione prevista (mp3)
								if(striright(lpszLocalFile,szExt)!=0 && stricmp(szExt,".mp3")==0)
								{
									// cambia il nome usando l'estensione specificata dalla content
									p = strrchr(lpszLocalFile,'\\');
									if(p)
									{
										*(p+1) = '\0';
										int n = strlen(lpszLocalFile);

										// chiama la callback per impostare il nome di default per il file locale (.mp3) nel caso in cui il server non lo specifichi
										pHttpObject->stat = HTTPOBJECTSTAT_DEFAULT_MP3FNAME;
										strcpy(pHttpObject->defaultfname,DEFAULT_MP3_NAME);
										if(lpfnCallback)
										{
											if(lpfnCallback(pHttpConnection,lpVoid)==IDCANCEL)
											{
												nTranslated = 0;
												TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CUrl::GetHttpUrl(): callback breaks! (%d)\n",__LINE__));
												goto done;
											}
										}

										_snprintf(lpszLocalFile+n,cbLocalFileSize-n-1,pHttpObject->defaultfname);
									}
								}
							}
						}
						
						// fix cgi: nomi file diverso da quello presente nella query cgi
						// un url come http://saho.vis.ne.jp/dlrankw/dlranklog.cgi?dl=pasta-w genera una risposta che contiene un nome file
						// di output specifico, indicato con la seguente direttiva: Content-Disposition: attachment; filename="pastaXP.zip" 
						// in tal caso deve correggere il nome file locale
						pContentType = stristr(szBuffer,"Content-Disposition:");
						if(pContentType)
						{
							char szDisposition[_MAX_FILEPATH+1];
							char szAttachment[_MAX_FILEPATH+1] = {0};
							strcpyn(szDisposition,pContentType,sizeof(szDisposition));
							char* p = strchr(szDisposition,'\r');
							if(p)
								*p = '\0';
							p = strchr(szDisposition,'\n');
							if(p)
								*p = '\0';
							p = stristr(szDisposition,"filename=");
							if(p)
							{
								p += 9;
								if(*p)
								{
									while(*p && (*p=='"' || *p==' '))
										p++;
									int i = 0;
									while(*p && *p!='"' && *p!=' ' && i < sizeof(szAttachment))
										szAttachment[i++] = *p++;
								}
								if(!strnull(szAttachment))
								{
									// rimuove il nome originale (quello della query cgi) e aggiunge quello specificato dalla content
									p = strrchr(lpszLocalFile,'\\');
									if(p && *(p+1))
									{
										*(p+1) = '\0';
										strcatn(lpszLocalFile,szAttachment,cbLocalFileSize);
									}
								}
							}
						}
						
						// (fine) controlli per il nome file locale (nomi incompleti o nomi diversi per url cgi)

						// crea il file locale con i dati ricevuti dall'host
						CBinFile file;

						// chiama la callback (nomefile)
						pHttpObject->stat = HTTPOBJECTSTAT_LOCAL_FILENAME;
						_snprintf(pHttpObject->url,
								HTTP_MAX_URL,
								"%s:%ld%s%s%s%s%s%s",
								Url.host,
								Url.port,
								Url.dir,
								Url.dir[strlen(Url.dir)-1]=='/' ? "" : "/",
								Url.file,
								Url.cgi,
								Url.delim,
								Url.query);
						strcpyn(pHttpObject->filename,lpszLocalFile,_MAX_FILEPATH+1);
						if(lpfnCallback)
						{
							if(lpfnCallback(pHttpConnection,lpVoid)==IDCANCEL)
							{
								nTranslated = 0;
								file.Close();
								TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CUrl::GetHttpUrl(): callback breaks! (%d)\n",__LINE__));
								goto done;
							}
							else
							{
								strcpyn(lpszLocalFile,pHttpObject->filename,cbLocalFileSize);
							}
						}
						
						// se deve scaricare l'url (e non esplorarla)
						BOOL bFileCreated = TRUE;
						if(bDownload)
							bFileCreated = file.Create(pHttpObject->filename);
						
						if(bFileCreated)
						{
							// deve eliminare l'header http per posizionarsi sui dati ricevuti, notare che
							// alcuni host possono inviare una coppia LF+LF, per cui cambia l'offset dei dati
							int nOfs = 4;
							p = strstr(szBuffer,"\r\n\r\n");
							if(!p)
							{
								p = strstr(szBuffer,"\n\n");
								nOfs = 2;
							}

							// analizza i campi dell'header http
							if(p)
							{
								// Last-Modified (Day, dd Mon yyyy hh:mm:ss GMT)
								// occhio che alcuni host inviano la data separata dal trattino: dd-Mon-yyyy
								char* lastmodified = stristr(szBuffer,"Last-Modified: ");
								if(lastmodified)
								{
									strcpyn(szLastModified,lastmodified+15,sizeof(szLastModified));
									lastmodified = strchr(szLastModified,'\r');
									if(!lastmodified)
										lastmodified = strchr(szLastModified,'\n');
									if(lastmodified)
										*lastmodified = '\0';
								}

								// Content-Length
								char* content = stristr(szBuffer,"Content-Length: ");
								if(content)
								{
									int i = 0;
									char size[32] = {0};
									for(i=0; i < sizeof(size)-1 && isdigit(*(content+16+i)); i++)	
										size[i] = *(content+16+i);
									pHttpObject->size = _atoi64(size);
								}
 								else
									pHttpObject->size = 0L;
								
								// controlla i limiti sulle dimensioni (min e max), sempre che non si tratti di un file html
								if(!IsUrlDir(lpcszUrl) && !IsHtmlFile(lpcszUrl))
								{
									if(pHttpObject->size > 0L && pHttpConnection->GetMinSize() > 0)
									{
										if(pHttpObject->size < pHttpConnection->GetMinSize())// * 1024)
										{
											pHttpConnection->SetHttpErrorCode(HTTP_STATUS_BELOW_MIN_SIZE);
											pHttpConnection->SetHttpErrorString("below min size");
											file.Close();
											::DeleteFile(pHttpObject->filename);
											goto terminate;
										}
									}
									if(pHttpObject->size > 0L && pHttpConnection->GetMaxSize() > 0)
									{
										if(pHttpObject->size > pHttpConnection->GetMaxSize())// * 1024)
										{
											pHttpConnection->SetHttpErrorCode(HTTP_STATUS_ABOVE_MAX_SIZE);
											pHttpConnection->SetHttpErrorString("above max size");
											file.Close();
											::DeleteFile(pHttpObject->filename);
											goto terminate;
										}
									}
								}

								// Content-Type
								content = stristr(szBuffer,"Content-Type: ");
								if(content)
								{
									// ricava il tipo MIME (eliminando quanto eventualmente specificato in aggiunta)
									strcpyn(pHttpObject->type,content+14,HTTP_MAX_CONTENT+1);
									for(int i = 0; pHttpObject->type[i]; i++)
										if(pHttpObject->type[i]=='\r' || pHttpObject->type[i]=='\n' || pHttpObject->type[i]==' ' || pHttpObject->type[i]==';')
										{
											pHttpObject->type[i] = '\0';
											break;
										}
									
									// se l'url referenzia un cgi aggiunge l'estensione relativa al tipo MIME
									if(!IsHtmlFile(pHttpObject->filename) && IsCgiUrl(lpcszUrl))
									{
										char szExt[_MAX_EXT+1];
										if(registry.GetContentTypeExtension(pHttpObject->type,szExt,sizeof(szExt)))
										{
											strcatn(pHttpObject->filename,szExt,cbLocalFileSize);
											bMimeType = TRUE;
										}
									}
								}
								
								// Server
								char* server = stristr(szBuffer,"Server: ");
								if(server)
								{
									char szServerName[HTTP_MAX_USERAGENT+1] = {0};
									int i = 0;
									server += 8;
									while(*server && *server!='\r' && *server!='\n' && i < sizeof(szServerName)-1)
										szServerName[i++] = *server++;
									pHttpConnection->SetServerName(szServerName);
								}
								
								// Set-Cookie
								if(pHttpConnection->CookiesAllowed())
								{
									char* cookie = stristr(szBuffer,"Set-Cookie: ");
									if(cookie)
									{
										do
										{
											char szCookie[COOKIE_MAX_LEN/*COOKIE_REALLY_MAX_LEN*/+1] = {0};
											int i = 0;
											cookie += 12;
											while(*cookie && *cookie!='\r' && *cookie!='\n' && i < sizeof(szCookie)-1)
												szCookie[i++] = *cookie++;
											
											// chiama la callback (cookie)
											// IDOK,IDYES = registra il cookie, IDNO = lo ignora, IDCANCEL = termina
											pHttpObject->stat = HTTPOBJECTSTAT_SET_COOKIE;
											pHttpConnection->ParseCookie(szCookie,&(pHttpObject->cookie));
											if(lpfnCallback)
											{
												UINT nRet = lpfnCallback(pHttpConnection,lpVoid);
												if(nRet==IDYES || nRet==IDOK)
													pHttpConnection->SetCookie(&(pHttpObject->cookie));
												else if(nRet==IDNO)
													;
												else if(nRet==IDCANCEL)
												{
													nTranslated = 0;
													file.Close();
													goto done;
												}
											}
										}
										while((cookie = stristr(cookie,"Set-Cookie: "))!=NULL);
									}
								}
								
								// salta l'header http
								p += nOfs;
								i = p-szBuffer;
								nRecv -= i;
							}
							else
								p = szBuffer;

							// chiama la callback (inizio)
							pHttpObject->stat = HTTPOBJECTSTAT_BEGIN_RECEIVING;
							_snprintf(pHttpObject->url,
									HTTP_MAX_URL,
									"%s:%ld%s%s%s%s%s%s",
									Url.host,
									Url.port,
									Url.dir,
									Url.dir[strlen(Url.dir)-1]=='/' ? "" : "/",
									Url.file,
									Url.cgi,
									Url.delim,
									Url.query);
//							strcpyn(pHttpObject->filename,lpszLocalFile,_MAX_FILEPATH+1);
							pHttpObject->starttime = ::GetTickCount();
							pHttpObject->endtime = 0L;
							if(lpfnCallback)
								if(lpfnCallback(pHttpConnection,lpVoid)==IDCANCEL)
								{
									nTranslated = 0;
									file.Close();
									TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CUrl::GetHttpUrl(): callback breaks! (%d)\n",__LINE__));
									goto done;
								}

							// scrive il primo blocco di dati ricevuti (senza l'header http)
							// se deve scaricare l'url (e non esplorarla)
							if(bDownload)
								file.Write(p,nRecv);

							// chiama la callback (ricezione)
							pHttpObject->stat = HTTPOBJECTSTAT_RECEIVING;
							pHttpObject->amount += nRecv;
							pHttpObject->endtime = ::GetTickCount();
							if(lpfnCallback)
								if(lpfnCallback(pHttpConnection,lpVoid)==IDCANCEL)
								{
									nTranslated = 0;
									file.Close();
									TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CUrl::GetHttpUrl(): callback breaks! (%d)\n",__LINE__));
									goto done;
								}

							// se deve scaricare l'url (e non esplorarla)
							if(bDownload)
							{
								// riceve e scrive il resto
								while((pHttpObject->size > 0L ? pHttpObject->amount < pHttpObject->size : 1) && (nRecv = pHttpConnection->Receive(szBuffer,sizeof(szBuffer))) > 0)
								{
									file.Write(szBuffer,nRecv);

									// chiama la callback (ricezione)
									pHttpObject->stat = HTTPOBJECTSTAT_RECEIVING;
									pHttpObject->amount += nRecv;
									pHttpObject->endtime = ::GetTickCount();
									if(lpfnCallback)
										if(lpfnCallback(pHttpConnection,lpVoid)==IDCANCEL)
										{
											nTranslated = 0;
											file.Close();
											TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CUrl::GetHttpUrl(): callback breaks! (%d)\n",__LINE__));
											goto done;
										}
								}

								// alcuni host vanno in timeout alla fine del download nonostante il file venga scaricato correttamente
								if(pHttpObject->amount==pHttpObject->size)
								{
									pHttpConnection->SetWSALastError(0);
									pHttpConnection->ResetSocketError();
								}
							}

							// chiama la callback (fine)
							pHttpObject->stat = HTTPOBJECTSTAT_END_RECEIVING;
							pHttpObject->endtime = ::GetTickCount();
							if(lpfnCallback)
								if(lpfnCallback(pHttpConnection,lpVoid)==IDCANCEL)
								{
									nTranslated = 0;
									file.Close();
									TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CUrl::GetHttpUrl(): callback breaks! (%d)\n",__LINE__));
									goto done;
								}

							// se deve scaricare l'url (e non esplorarla)
							if(bDownload)
								file.Close();
						}
						else
						{
							// errore nella creazione del file locale
							pHttpConnection->SetHttpErrorCode(HTTP_STATUS_UNABLE_TO_CREATE_LOCAL_FILE);
							char szErrorString[256];
							_snprintf(szErrorString,sizeof(szErrorString)-1,"Unable to create the local file - %s",file.GetLastErrorString());
							pHttpConnection->SetHttpErrorString(szErrorString);
							goto terminate;
						}
					}
					else // errore http
					{
						// 300 Multiple Choices
						// 301 Moved Permanently
						// 302 Moved Temporarily
						// 304 Not Modified - If-Modified-Since
						pHttpConnection->SetHttpErrorCode(atoi(szHttpCode));

						// spostato rispetto alla locazione originale
						if(atoi(szHttpCode)==HTTP_CODE_MOVED_PERMANENTLY || atoi(szHttpCode)==HTTP_CODE_MOVED_TEMPORARILY)
						{
							if((p = strstr(szBuffer,"\r\n\r\n"))!=(char*)NULL)
							{
								char* location = stristr(szBuffer,"Location: ");
								if(location)
									location += 10;
								if(location)
								{
									URL LocationUrl;
									SplitUrl(lpcszUrl,&LocationUrl);

									// imposta l'url per il download con quanto presente nel campo 'Location:'
									memset(szUrl,'\0',sizeof(szUrl));
									for(i = 0; *location && (*location!='\r' && *location!='\n') && *location!=',' && i < sizeof(szUrl)-1; i++)
										szUrl[i] = *location++;

									// 'Location:' dovrebbe contenere il pathname assoluto (completo), ma molti
									// se ne fregano ed indicano solo il nome file se si tratta dello stesso host
#if 1
									if(!IsUrlType(szUrl,HTTP_URL))
									{
										char szLocationUrl[MAX_URL+1];
										_snprintf(szLocationUrl,sizeof(szLocationUrl),"%s%s%s",LocationUrl.host,szUrl[0]=='/' ? "" : "/",szUrl);
										strcpyn(szUrl,szLocationUrl,sizeof(szUrl));
									}
#else
									URL LocationUrl;
									char szLocationUrl[MAX_URL+1];
									strcpyn(szLocationUrl,szUrl,sizeof(szLocationUrl));
									SplitUrl(szLocationUrl,&LocationUrl);
									if(LocationUrl.host[0]=='\0')
									{
										strcpyn(LocationUrl.host,Url.host,sizeof(LocationUrl.host));
										if(LocationUrl.dir[0]=='\0')
											strcpyn(LocationUrl.dir,Url.dir,sizeof(LocationUrl.dir));
										if(LocationUrl.file[0]=='\0')
											strcpyn(LocationUrl.file,Url.file,sizeof(LocationUrl.file));
										if(LocationUrl.delim[0]=='\0')
											strcpyn(LocationUrl.delim,Url.delim,sizeof(LocationUrl.delim));
										if(LocationUrl.cgi[0]=='\0')
											strcpyn(LocationUrl.cgi,Url.cgi,sizeof(LocationUrl.cgi));
										if(LocationUrl.query[0]=='\0')
											strcpyn(LocationUrl.query,Url.query,sizeof(LocationUrl.query));
										LocationUrl.port = Url.port;
										ComposeUrl(&LocationUrl,szUrl,sizeof(szUrl));
									}
#endif
									// per evitare il loop
									if(strcmp(lpcszUrl,szUrl)==0)
										goto done;
										
									pHttpConnection->Close();
									goto parse_url;
								}
							}
						}
						// non modificato rispetto alla data/ora specificate
						else if(atoi(szHttpCode)==HTTP_CODE_NOT_MODIFIED)
						{
							TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CUrl::GetHttpUrl(): filename: %s, not modified\n",pHttpObject->filename));
						}

						// chiama la callback (inizio)
						pHttpObject->stat = HTTPOBJECTSTAT_BEGIN_RECEIVING;
						_snprintf(pHttpObject->url,
								HTTP_MAX_URL,
								"%s:%ld%s%s%s%s%s%s",
								Url.host,
								Url.port,
								Url.dir,
								Url.dir[strlen(Url.dir)-1]=='/' ? "" : "/",
								Url.file,
								Url.cgi,
								Url.delim,
								Url.query);
						strcpyn(pHttpObject->filename,lpszLocalFile,_MAX_FILEPATH+1);
						pHttpObject->starttime = ::GetTickCount();
						pHttpObject->endtime = 0L;
						if(lpfnCallback)
							if(lpfnCallback(pHttpConnection,lpVoid)==IDCANCEL)
							{
								nTranslated = 0;
								TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CUrl::GetHttpUrl(): callback breaks! (%d)\n",__LINE__));
								goto done;
							}

						// chiama la callback (fine)
						pHttpObject->stat = HTTPOBJECTSTAT_END_RECEIVING;
						pHttpObject->endtime = ::GetTickCount();
						if(lpfnCallback)
							if(lpfnCallback(pHttpConnection,lpVoid)==IDCANCEL)
							{
								nTranslated = 0;
								TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CUrl::GetHttpUrl(): callback breaks! (%d)\n",__LINE__));
								goto done;
							}
					}
				}
			}
		}
		else
			pHttpConnection->SetSocketError();
done:

		if((nWSAError = pHttpConnection->GetWSALastError())!=0)
		{
			strcpy(pHttpObject->filename,"");
			strcpy(pHttpObject->type,"");
			pHttpObject->amount = 0L;
			pHttpObject->size = 0L;
			pHttpObject->starttime = 0L;
			pHttpObject->endtime = 0L;
			memset(&(pHttpObject->cookie),'\0',sizeof(COOKIE));
			pHttpConnection->SetSocketError();
			pHttpConnection->SetHttpErrorCode(0);
			pHttpConnection->SetHttpErrorString("");

			// se deve scaricare l'url (e non esplorarla)
			if(bDownload)
				::DeleteFile(lpszLocalFile);

			/*
			if(nWSAError==WSAETIMEDOUT)
			{
				//http://codeguru.developer.com/images/rule.gif
				static int nRetryTimeout = 0;
				if(nRetryTimeout==0)
				{
					pHttpConnection->GetConnectionRetry();
					nRetryTimeout = nRetryTimeout <= 0 ? 1 : nRetryTimeout;
				}
				if(--nRetryTimeout >= 0)
				{
					bConnected = FALSE;
					pHttpConnection->Reset();
					pHttpConnection->SetWSALastError(WSAETIMEDOUT);
					memset(pHttpObject,'\0',sizeof(HTTPOBJECT));
					pHttpConnection->ResetSocketError();
					pHttpConnection->ResetHttpError();
					strcpyn(szUrl,lpszUrl,sizeof(szUrl));
					goto parse_url;
				}
				else
					goto terminate;
			}
			else
			*/	goto terminate;
		}

		// se si trattava di un cgi ha aggiunto l'estensione relativa al tipo MIME al nome del file
		// locale, quindi dato che il file e' stato aperto prima del test deve rinominarlo
		// se deve scaricare l'url (e non esplorarla)
		if(bDownload)
		{
			if(bMimeType)
			{
				char szFileName[_MAX_FILEPATH+1] = {0};
				char* p = strrchr(lpszLocalFile,'.');
				if(p)
				{
					memcpy(szFileName,lpszLocalFile,p - lpszLocalFile);
					::MoveFile(szFileName,lpszLocalFile);
				}
			}
		}

		// se deve scaricare l'url (e non esplorarla)
		if(bDownload)
		{
			// imposta la data/ora del file locale con quanto specificato dal server
			// notare che il server restituisce la data/ora del file in formato UTC, per
			// cui quando si crea il file la data/ora deve essere converita in locale
			if(szLastModified[0]!='\0')
			{
				FILETIME filetime;
				SYSTEMTIME systemtime;
				CDateTime datetime(szLastModified,GMT_SHORT);
				systemtime.wYear = (WORD)datetime.GetYear();
				systemtime.wMonth = (WORD)datetime.GetMonth();
				systemtime.wDayOfWeek = 0;
				systemtime.wDay = (WORD)datetime.GetDay();
				systemtime.wHour = (WORD)datetime.GetHour();
				systemtime.wMinute = (WORD)datetime.GetMin();
				systemtime.wSecond = (WORD)datetime.GetSec();
				systemtime.wMilliseconds = 0;
				
				// converte da formato di sistema a a formato file
				::SystemTimeToFileTime(&systemtime,&filetime);
			
				// imposta la data/ora
				HANDLE hHandle;
				if((hHandle = ::CreateFile(lpszLocalFile,GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_ARCHIVE,NULL))!=INVALID_HANDLE_VALUE)
				{
					// converte automaticamente da UTC (=GMT) a locale
					::SetFileTime(hHandle,&filetime,&filetime,&filetime);
					::CloseHandle(hHandle);
				}
			}
		}

		// se e' stata passata un url senza nome file aggiunge quello di default
		// (usato per salvare il contenuto del listato della directory)
		// se si trattava di un url alla cazzone, aggiunge l'estensione al nome file
		if(lpszTranslatedUrl)
		{
			_snprintf(lpszTranslatedUrl,
					cbTranslatedUrl-1,
					"%s:%ld%s%s%s%s%s%s%s",
					Url.host,
					Url.port,
					Url.dir,
					Url.dir[strlen(Url.dir)-1]=='/' ? "" : "/",
					(Url.file[0]=='\0' && Url.cgi[0]=='\0' && Url.delim=='\0' && Url.query=='\0') ? szDefaultHtml : Url.file,
					bHtmlFileWithoutExt ? DEFAULT_HTML_EXT : "",
					Url.cgi,
					Url.delim,
					Url.query);

			DecodeUrl(lpszTranslatedUrl);
		}
	}
	else
		pHttpConnection->SetSocketError();

terminate:

	// chiude la connessione
	if(nWSAError==0)
		pHttpConnection->Close();
	else
		pHttpConnection->Abort();

	// il codice di ritorno e' sempre 1, a meno che la callback non interrompa il download per cui viene
	// restituito 0, motivo per cui per ricavare l'esito del download bisogna controllare i campi relativi
	// della struttura
	return(nTranslated);
}

/*
	LoadHtmlFileTypeList()

	Per caricare la lista interna dei tipi file html solo quando necessario.
*/
void CUrl::LoadHtmlFileTypeList(void)
{
	if(!m_bHtmlFileTypeList)
	{
		// carica la lista interna con i tipi di file html riconosciuti
		for(int i = 0; htmlfiletype[i].len!=0; i++)
		{
			FILETYPE* h = (FILETYPE*)m_HtmlFileTypeList.Add();
			if(h)
			{
				strcpyn(h->ext,htmlfiletype[i].ext,MAX_FILETYPE_EXT+1);
				h->len = htmlfiletype[i].len;
			}
		}

		m_bHtmlFileTypeList = TRUE;
	}
}

/*
	EnumHtmlFileTypes()

	Enumera quanto contenuto nella lista interna dei tipi file html,
	chiamare in un ciclo (senza break) fino a che non restituisce NULL.
*/
LPCSTR CUrl::EnumHtmlFileTypes(void)
{
	static int i = -1;
	LPCSTR p = NULL;
	ITERATOR iter;

	// carica la lista se necessario
	LoadHtmlFileTypeList();

	if(++i >= m_HtmlFileTypeList.Count())
	{
		i = -1;
		p = NULL;
	}
	else
	{
		if((iter = m_HtmlFileTypeList.FindAt(i))!=(ITERATOR)NULL)
			p = ((FILETYPE*)iter->data)->ext;
	}

	return(p);
}

