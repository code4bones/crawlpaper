/*
	CIcy.cpp
	Classe base per il protocollo ICY (SDK/MFC).
	Luca Piergentili, 02/09/03
	lpiergentili@yahoo.com
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "strings.h"
#include "window.h"
#include "CUrl.h"
#include "CHttp.h"
#include "CSock.h"
#include "CIcy.h"

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
	CIcy()
*/
CIcy::CIcy(LPCSTR pUrl /*= NULL*/,int nPort /*= HTTP_DEFAULT_PORT*/)
{
	Reset();

	m_nPort = nPort;
	if(pUrl)
	{
		URL url;
		SplitUrl(pUrl,&url);
		strcpyn(m_szHost,StripUrlType(url.host),sizeof(m_szHost));
		if(!strnull(url.dir))
			strcpyn(m_szDir,url.dir,sizeof(m_szDir));
		if(!strnull(url.file))
			strcpyn(m_szFile,url.file,sizeof(m_szFile));
		m_nPort = url.port;
	}
	if(m_nPort <= 0)
		m_nPort = HTTP_DEFAULT_PORT;
}
	
/*
	~CIcy()
*/
CIcy::~CIcy()
{
	if(m_Socket!=INVALID_SOCKET)
		Close();
}

/*
	Reset()
*/
void CIcy::Reset(void)
{
	m_Socket = INVALID_SOCKET;
	memset(m_szFirstChunk,'\0',sizeof(m_szFirstChunk));
	m_nFirstChunkLen = 0;
	memset(m_szUrl,'\0',sizeof(m_szUrl));
	memset(m_szHost,'\0',sizeof(m_szHost));
	strcpy(m_szDir,"/");
	memset(m_szFile,'\0',sizeof(m_szFile));
	m_nPort = 0;
	memset(&m_Proxy,'\0',sizeof(PROXY));
	memset(m_szUserAgent,'\0',sizeof(m_szUserAgent));
	memset(m_szLocation,'\0',sizeof(m_szLocation));
	m_nResponse = 0;
	m_dwLastError = 0L;
	memset(m_szLastError,'\0',sizeof(m_szLastError));
	m_listICYHeaders.DeleteAll();
}

/*
	Open()
*/
SOCKET CIcy::Open(LPCSTR pUrl/* = NULL*/,int nPort/* = HTTP_DEFAULT_PORT*/,int icydata/* = 1*/)
{
	// per evitare la ricorsione infinita sui codici 301/302
	static int nRecurse = 0;
	
	// chiude il socket
	Close();
	
	// resetta i codice d'errore
	m_nResponse = 0;
	m_dwLastError = 0L;
	memset(m_szLastError,'\0',sizeof(m_szLastError));
	CSock::SetWSALastError(0);

	// resetta e normalizza l'url
	memset(m_szUrl,'\0',sizeof(m_szUrl));
	memset(m_szHost,'\0',sizeof(m_szHost));
	strcpy(m_szDir,"/");
	memset(m_szFile,'\0',sizeof(m_szFile));
	m_nPort = nPort;
	if(pUrl)
	{
		URL url;
		SplitUrl(pUrl,&url);
		strcpyn(m_szHost,StripUrlType(url.host),sizeof(m_szHost));
		if(strcmp("",url.dir)!=0)
			strcpyn(m_szDir,url.dir,sizeof(m_szDir));
		if(strcmp("",url.file)!=0)
			strcpyn(m_szFile,url.file,sizeof(m_szFile));
		m_nPort = url.port;
	}
	if(m_nPort <= 0)
		m_nPort = HTTP_DEFAULT_PORT;
	
	// apre il socket
	if(CSock::Open(AF_INET,SOCK_STREAM))
	{
		// si collega all'host
		BOOL bConnected = FALSE;
		
		if(GetProxyAddress() && GetProxyPortNumber() > 0)
			bConnected = CSock::Connect(GetProxyAddress(),GetProxyPortNumber());
		else
			bConnected = CSock::Connect(m_szHost,m_nPort);

		if(bConnected)
		{
			// costruisce la richiesta
			char szBuffer[4096];
			int n = 0;
			
			if(GetProxyAddress() && GetProxyPortNumber() > 0)
				n += _snprintf(szBuffer,sizeof(szBuffer)-1,"GET http://%s:%ld%s%s HTTP/1.0\r\n",m_szHost,m_nPort,m_szDir,m_szFile);
			else
				n += _snprintf(szBuffer,sizeof(szBuffer)-1,"GET %s%s HTTP/1.0\r\n",m_szDir,m_szFile);
			
			if(strcmp(GetProxyAuth(),"")!=0)
			{
				n += _snprintf(szBuffer+n,
							sizeof(szBuffer) - (1 + n),
							"Proxy-Authorization: Basic %s\r\n",
							GetProxyAuth()
							);
			}

			if(m_szUserAgent[0]!='\0' && n > 0)
				n += _snprintf(szBuffer+n,sizeof(szBuffer)-1-n,"Host: %s:%ld\r\nAccept: *.*, */*\r\nUser-Agent: %s\r\n",m_szHost,m_nPort,m_szUserAgent);
			if(icydata==1 && n > 0)
				n += _snprintf(szBuffer+n,sizeof(szBuffer)-1-n,"Icy-Metadata: 1\r\n");
			if(n > 0)
				n = _snprintf(szBuffer+n,sizeof(szBuffer)-1-n,"\r\n");
			
			// invia
			TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CIcy::Open(): Send():\n[%s]\n",szBuffer));
			if((n = CSock::Send(szBuffer,strlen(szBuffer)))!=SOCKET_ERROR)
			{
				// Send() e Receive() a raffica scasinano, sopratutto con ICY
				::Sleep((DWORD)CSock::GetDelay());
				
				// riceve
				if((n = CSock::Receive(szBuffer,sizeof(szBuffer)-1))!=SOCKET_ERROR)
				{
					TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CIcy::Open(): Receive():\n[%s]\n",szBuffer));
					
					// carica i campi dell'header
					if((m_nResponse = ParseHeader(szBuffer))==ICY_CODE_OK)
					{
						m_Socket = CSock::GetHandle();
						
						char* p = strstr(szBuffer,"\r\n\r\n");
						if(p)
						{
							// il primo blocco di dati arriva insieme all'header
							p+=4;
							int tot = n - (p-szBuffer);
							memcpy(m_szFirstChunk,p,tot);
							m_nFirstChunkLen = tot;
							
							/*CBinFile binFile;
							if(binFile.Create("chunk.dat"))
							{
								binFile.Write(m_szFirstChunk,tot);
								binFile.Close();
							}*/
						}
					}
					else
					{
						switch(m_nResponse)
						{
							case HTTP_CODE_MOVED_PERMANENTLY:
							case HTTP_CODE_MOVED_TEMPORARILY:
								if(++nRecurse > 2)
								{
									nRecurse = 0;
									break;
								}
								else
									return(CIcy::Open(m_szLocation));
							default:
								break;
						}
					}
				}
			}
		}
	}

	return(m_Socket);
}

/*
	Close()
*/
void CIcy::Close(void)
{
	CSock::Close();
	m_Socket = INVALID_SOCKET;
}

/*
	GetIcyData()
*/
void CIcy::GetIcyData(ICYDATA* pIcyData)
{
	memset(pIcyData,'\0',sizeof(ICYDATA));
	if(m_nResponse==ICY_CODE_OK)
	{
		strcpyn(pIcyData->station,GetStationName(),ICY_MAX_STATION+1);
		strcpyn(pIcyData->genre,GetGenre(),ICY_MAX_GENRE+1);
		strcpyn(pIcyData->url,GetUrl(),ICY_MAX_URL+1);
		strcpyn(pIcyData->contenttype,GetContentType(),ICY_MAX_CONTENT+1);
		pIcyData->metaint = GetMetaInterval();
		pIcyData->bitrate = GetBitRate();
		strcpyn(pIcyData->notice,GetNotice(),ICY_MAX_NOTICE+1);
		strcpyn(pIcyData->noticeinfo,GetNoticeInfo(),ICY_MAX_NOTICE+1);
		pIcyData->code = m_nResponse;
	}
}

/*
	GetLastError()
*/
DWORD CIcy::GetLastError(void)
{
	if(m_nResponse==0)
	{
		m_dwLastError = CSock::GetWSALastError();
		strcpyn(m_szLastError,CSock::GetWSAErrorString(),sizeof(m_szLastError));
	}
	else if(IS_HTTP_CODE(m_nResponse))
	{
		m_dwLastError = m_nResponse;
		
		// descrizione estesa per 400 e 403, i piu' comuni per ICY
		switch(m_dwLastError)
		{
			case ICY_CODE_SERVER_FULL:
				strcpyn(m_szLastError,"Server Full: This server has reached its user limit.",sizeof(m_szLastError));
				break;
			case ICY_CODE_FORBIDDEN:
				strcpyn(m_szLastError,"Service Forbidden: The resource requested is forbidden.",sizeof(m_szLastError));
				break;
			default:
				strcpyn(m_szLastError,CHttpConnection::GetHttpErrorString(m_dwLastError),sizeof(m_szLastError));
				break;
		}
	}

	return(m_dwLastError);
}

/*
	GetLastErrorString()
*/
LPCSTR CIcy::GetLastErrorString(void)
{
	if(strnull(m_szLastError))
		GetLastError();
		
	return(m_szLastError);
}

/*
	ParseHeader()

	Analizza l'header ICY, caricando la lista interna con i valori dei campi.
*/
int CIcy::ParseHeader(LPCSTR pIcyHeaderData)
{
	/*
	header icy:

	ICY 200 OK
	icy-notice1:<BR>This stream requires <a href="http://www.winamp.com/">Winamp</a><BR>
	icy-notice2:SHOUTcast Distributed Network Audio Server/SolarisSparc v1.9.2<BR>
	icy-name:Secret Agent: The soundtrack for your stylish, mysterious, dangerous life. For Spys and P.I.'s too! [SomaFM]
	icy-genre:Downtempo Lounge Spy
	icy-url:http://www.somafm.com
	Content-Type:audio/mpeg
	icy-pub:1
	icy-metaint:24576
	icy-br:128

	errori:

	ICY 400 Server Full
	icy-notice1:<BR>SHOUTcast Distributed Network Audio Server/win32 v1.9.2<BR>
	icy-notice2:This server has reached its user limit<BR>

	ICY 403 Service Forbidden
	icy-notice1:<BR>SHOUTcast Distributed Network Audio Server/Linux v1.8.9<BR>
	icy-notice2:The resource requested is forbidden<BR>
	
	streamripper/live365:
	
	http://streamripper.sourceforge.net/dc.php
	http://www.dslreports.com/shownews/3472
	*/
	if(strnicmp(pIcyHeaderData,"ICY",3)!=0 && strnicmp(pIcyHeaderData,"HTTP",4)!=0)
		return(HTTP_STATUS_BAD_REQUEST);
	
	int nCode = 0;
	char szCode[8];
	int i = 0;
	char* p = (char*)pIcyHeaderData;
	// salta per arrivare al codice d'errore dato che con http la risposta inizia con (ad es.): HTTP/1.1 301 [...]
	if(strnicmp(pIcyHeaderData,"HTTP",4)==0)
		p+=8;
	while(*p && !isdigit(*p))
		p++;
	for(i=0; *p && isdigit(*p) && i < sizeof(szCode);)
		szCode[i++] = *p++;
	nCode = atoi(szCode);
	switch(nCode)
	{
		case HTTP_CODE_MOVED_PERMANENTLY:
		case HTTP_CODE_MOVED_TEMPORARILY:
			GetHeaderValue(pIcyHeaderData,"Location",m_szLocation,sizeof(m_szLocation));
			return(nCode);
		default:
			break;
	}

	char* pIcyHeaders[] = {
		"Content-Type:",	// Content-Type: application/octet-stream, Content-Type: audio/mpeg, etc.
		"ice-audio-info",	// ice-audio-info: bitrate=128;samplerate=44100;channels=2
		"icy-br",
		"icy-description",
		"icy-genre",
		"icy-metaint",
		"icy-name",
		"icy-notice1",
		"icy-notice2",
		"icy-pub",
		"icy-url",
		"Server:",		// Server: Icecast 2.3.2 oppure icy-notice2: SHOUTcast Distributed Network Audio Server/win32 v1.9.5<BR>
		"Location:",
		NULL,
	};
	
	m_listICYHeaders.DeleteAll();
	
	for(i=0; pIcyHeaders[i]!=NULL; i++)
	{
		HTTPHEADER* h = (HTTPHEADER*)m_listICYHeaders.Add();
		if(h)
		{
			strcpyn(h->name,pIcyHeaders[i],HTTPHEADER_NAME_LEN+1);
			if(GetHeaderValue(pIcyHeaderData,h->name,h->value,HTTPHEADER_VALUE_LEN)!=0)
			{
				strcpy(h->value,"");
			}
			else
			{
				char buffer[HTTPHEADER_VALUE_LEN+1];
				while(substr(h->value,"<BR>","",buffer,sizeof(buffer))!=0)
					strcpyn(h->value,buffer,HTTPHEADER_VALUE_LEN);
				while(substr(h->value,"<br>","",buffer,sizeof(buffer))!=0)
					strcpyn(h->value,buffer,HTTPHEADER_VALUE_LEN);
			}
			
			TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CIcy::ParseHeader(): [%s]->[%s]\n",h->name,h->value));

		}
	}

	if(strnull(GetHeaderValue("Server:")) && stristr(GetHeaderValue("icy-notice2"),"shoutcast") && stristr(GetHeaderValue("icy-notice2"),"server"))
	{
		ITERATOR iter;
		HTTPHEADER* h;

		if((iter = m_listICYHeaders.First())!=(ITERATOR)NULL)
		{
			do
			{
				if((h = (HTTPHEADER*)iter->data)!=(HTTPHEADER*)NULL)
				{
					if(stricmp(h->name,"Server:")==0)
					{
						strcpyn(h->value,GetHeaderValue("icy-notice2"),HTTPHEADER_VALUE_LEN);
						break;
					}
				}

				iter = m_listICYHeaders.Next(iter);
			
			} while(iter!=(ITERATOR)NULL);
		}
	}

	TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"\nCIcy::GetHeaderValue(): Content-Type [%s]\n",GetHeaderValue("Content-Type:")));
	TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CIcy::GetHeaderValue(): ice-audio-info [%s]\n",GetHeaderValue("ice-audio-info")));
	TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CIcy::GetHeaderValue(): icy-br [%s]\n",GetHeaderValue("icy-br")));
	TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CIcy::GetHeaderValue(): icy-description [%s]\n",GetHeaderValue("icy-description")));
	TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CIcy::GetHeaderValue(): icy-genre [%s]\n",GetHeaderValue("icy-genre")));
	TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CIcy::GetHeaderValue(): icy-metaint [%s]\n",GetHeaderValue("icy-metaint")));
	TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CIcy::GetHeaderValue(): icy-name [%s]\n",GetHeaderValue("icy-name")));
	TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CIcy::GetHeaderValue(): icy-notice1 [%s]\n",GetHeaderValue("icy-notice1")));
	TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CIcy::GetHeaderValue(): icy-notice2 [%s]\n",GetHeaderValue("icy-notice2")));
	TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CIcy::GetHeaderValue(): icy-pub [%s]\n",GetHeaderValue("icy-pub")));
	TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CIcy::GetHeaderValue(): icy-url [%s]\n",GetHeaderValue("icy-url")));
	TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CIcy::GetHeaderValue(): Server [%s]\n",GetHeaderValue("Server:")));
	TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CIcy::GetHeaderValue(): Location [%s]\n\n",GetHeaderValue("Location:")));

	return(nCode);
}

/*
	GetHeaderValue()
*/
LPCSTR CIcy::GetHeaderValue(LPCSTR lpcszFieldName)
{
	ITERATOR iter;
	HTTPHEADER* h;

	if((iter = m_listICYHeaders.First())!=(ITERATOR)NULL)
	{
		do
		{
			if((h = (HTTPHEADER*)iter->data)!=(HTTPHEADER*)NULL)
			{
				if(stricmp(lpcszFieldName,h->name)==0)
					return(h->value);
			}

			iter = m_listICYHeaders.Next(iter);
		
		} while(iter!=(ITERATOR)NULL);
	}
	
	return("");
}

/*
	GetHeaderValue()
*/
int CIcy::GetHeaderValue(LPCSTR pIcyHeaderData,LPCSTR lpcszFieldName,LPSTR szValue,int cbValue)
{
	memset(szValue,'\0',cbValue);
	
	char* p = stristr(pIcyHeaderData,lpcszFieldName);
	
	if(p)
	{
		p += strlen(lpcszFieldName);
		if(p)
		{
			while(*p && (isspace(*p) || *p==':'))
				p++;
			int i;
			for(i=0; *p && *p!='\r' && *p!='\n' && i < cbValue-1;)
				szValue[i++] = *p++;

			return(0);
		}
		else
			return(1);
	}
	else
		return(1);
}
