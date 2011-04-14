/*
	CBrowser.cpp
	Classe per accesso al browser.
	Luca Piergentili, 08/09/00
	lpiergentili@yahoo.com

	La parte relativa alla comunicazione DDE e' stata ripresa dal codice della
	"VWCL - The Virtual Windows Class Library" di Todd Osborne (http://www.vwcl.org).
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "strings.h"
#include "window.h"
#include <ddeml.h>
#include "CBrowser.h"
#include "CRegKey.h"
#include "CRegistry.h"
#include "CUrl.h"

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

/*
	CBrowser()
*/
CBrowser::CBrowser(LPCSTR lpcszBrowserName/* = NULL */)
{
	m_browserType = IEXPLORE;
	
	if(lpcszBrowserName)
		SetPreferredBrowser(lpcszBrowserName);
}

/*
	SetPreferredBrowser()
*/
BROWSER_TYPE CBrowser::SetPreferredBrowser(LPCSTR lpcszBrowserName)
{
	if(stricmp(lpcszBrowserName,NETSCAPE_SERVICE_NAME)==0)
		m_browserType = NETSCAPE;
	else if(stricmp(lpcszBrowserName,IEXPLORER_SERVICE_NAME)==0)
		m_browserType = IEXPLORE;
	else if(stricmp(lpcszBrowserName,MOZILLA_SERVICE_NAME)==0)
		m_browserType = MOZILLA;
	else if(stricmp(lpcszBrowserName,OPERA_SERVICE_NAME)==0)
		m_browserType = OPERA;
	else if(stricmp(lpcszBrowserName,CHROME_SERVICE_NAME)==0)
		m_browserType = CHROME;
	
	return(m_browserType);
}

/*
	GetDefaultBrowser()

	Restituisce il pathname per l'eseguibile relativo al browser di default.
	Il nome del programma viene restituito eliminando gli eventuali parametri/opzioni presenti nel registro.
*/
BOOL	CBrowser::GetDefaultBrowser(LPSTR lpszProgram,UINT nSize)
{
	CRegistry registry;
	return(registry.GetProgramForRegisteredFileType(DEFAULT_HTML_EXT,lpszProgram,nSize));
}

/*
	GetBrowserProgramFilename()

	Restituisce il pathname per l'eseguibile relativo al browser specificato.
	Il nome del programma viene restituito eliminando gli eventuali parametri/opzioni presenti nel registro.
*/
BOOL CBrowser::GetBrowserProgramFilename(BROWSER_TYPE type,LPSTR lpszProgram,UINT nSize)
{
	CRegKey regkey;
	LONG lRet = -1L;
	
	memset(lpszProgram,'\0',nSize);

	regkey.Attach(HKEY_LOCAL_MACHINE);
	
	switch(type)
	{
		case NETSCAPE:
			lRet = regkey.Open(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\Netscape.exe");
			break;
		case IEXPLORE:
			lRet = regkey.Open(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\IEXPLORE.EXE");
			break;
		case MOZILLA:
			lRet = regkey.Open(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\firefox.exe");
			break;
		case OPERA:
			lRet = regkey.Open(HKEY_CLASSES_ROOT,"Applications\\Opera.exe\\shell\\open\\command");
			if(lRet!=ERROR_SUCCESS)
				lRet = regkey.Open(HKEY_LOCAL_MACHINE,"SOFTWARE\\Classes\\Applications\\Opera.exe\\shell\\open\\command");
			if(lRet!=ERROR_SUCCESS)
				lRet = regkey.Open(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\Opera.exe");
			break;
		case CHROME:
			lRet = -1L;
			break;
		case DEFAULT_INTERNET_BROWSER:
		default:
			if(GetDefaultBrowser(lpszProgram,nSize))
				lRet = ERROR_SUCCESS;
			break;
	}

	if(lRet==ERROR_SUCCESS)
	{
		if(lpszProgram[0]=='\0')
		{
			DWORD dwSize = nSize;
			lRet = regkey.QueryValue(lpszProgram,"",&dwSize);
			regkey.Close();
		}
	}
	
	regkey.Detach();

	return(lRet==ERROR_SUCCESS);
}

/*
	Browse()

	Apre l'url nel browser specificato/di default.
	Prova a passare l'url da aprire via DDE, se la connessione col server fallisce lancia il browser
	specificato/di default.
	Nel caso in cui si specifichi un id per il browser, passare il nome del servizio DDE relativo.

	Ripresa dal codice presente in:
	vwebhyperlink.hpp - Todd Osborne (todd@vwcl.org)
	VWCL - The Virtual Windows Class Library.
	Copyright (C) 1996-2000 The VWCL Alliance
*/
BOOL CBrowser::Browse(LPCSTR lpcszUrl,BOOL bForceNewInstance/* = TRUE*/)
{
	UINT nResult = (UINT)-1;
	DWORD dwDDE = 0L;
	LPSTR lpszServiceName = {""};

	CUrl url;
	char szUrl[MAX_URL+1];
	
	if(!url.IsUrl(lpcszUrl))
		url.LocalFileToUrl(lpcszUrl,szUrl,sizeof(szUrl));
	else
		strcpyn(szUrl,lpcszUrl,sizeof(szUrl));

	switch(m_browserType)
	{
		case NETSCAPE:
			lpszServiceName = NETSCAPE_SERVICE_NAME;
			break;
		case IEXPLORE:
			lpszServiceName = IEXPLORER_SERVICE_NAME;
			break;
		case MOZILLA:
			lpszServiceName = MOZILLA_SERVICE_NAME;
			break;
		case OPERA:
			lpszServiceName = OPERA_SERVICE_NAME;
			break;
		case CHROME:
			lpszServiceName = CHROME_SERVICE_NAME;
			break;
		case DEFAULT_INTERNET_BROWSER:
		default:
		{
			CRegistry registry;
			return(registry.OpenFileType(szUrl));
		}
	}

	if(bForceNewInstance)
	{
		char szProgram[_MAX_PATH+1];
		if(GetBrowserProgramFilename(m_browserType,szProgram,sizeof(szProgram)))
		{
			char cmd[(_MAX_PATH*2)+1] = {0};
			if(strstr(szProgram,"\"%1\""))
			{
				substr(szProgram,"\"%1\"","",cmd,sizeof(cmd)-1);
				strcpyn(szProgram,cmd,sizeof(szProgram));
			}
			if(strstr(szProgram,"%1"))
			{
				substr(szProgram,"%1","",cmd,sizeof(cmd)-1);
				strcpyn(szProgram,cmd,sizeof(szProgram));
			}
			if(szProgram[0]!='"')
			{
				_snprintf(cmd,sizeof(cmd)-1,"\"%s\"",szProgram);
				strcpyn(szProgram,cmd,sizeof(szProgram));
			}
			if(szUrl[0]!='"')
			{
				_snprintf(cmd,sizeof(cmd)-1,"\"%s\"",szUrl);
				strcpyn(szUrl,cmd,sizeof(szProgram));
			}
			_snprintf(cmd,
					sizeof(cmd)-1,
					"%s %s",
					szProgram,
					szUrl
					);
			
			STARTUPINFO si = {0};
			si.cb = sizeof(STARTUPINFO);
			PROCESS_INFORMATION pi = {0};
			if(::CreateProcess(NULL,cmd,NULL,NULL,FALSE,0L,NULL,NULL,&si,&pi))
			{
				::CloseHandle(pi.hProcess);
				nResult = 0;
			}
		}
	}
	else // prova a comunicarsi con l'eventuale istanza gia' in esecuzione
	{
		if(::DdeInitialize(&dwDDE,NULL,APPCMD_CLIENTONLY|CBF_SKIP_ALLNOTIFICATIONS,0)!=DMLERR_NO_ERROR)
			dwDDE = 0L;

		if(dwDDE!=0L)
		{
			HSZ hszServName = ::DdeCreateStringHandle(dwDDE,lpszServiceName,CP_WINANSI);
			
			if(hszServName!=0L)
			{
				HSZ hszTopic = ::DdeCreateStringHandle(dwDDE,"WWW_OpenURL",CP_WINANSI);
				
				if(hszTopic!=0L)
				{
					// TODO: ::DdeConnect() SOMETIMES fails if a DDE server
					// that responds to WWW_OpenURL is not already running
					HCONV hConv = ::DdeConnect(dwDDE,hszServName,hszTopic,NULL);
					
					if(hConv)
					{
						CString s;
						s.Format("\"%s\",,-1,,,,",szUrl);

						HSZ hszItem = ::DdeCreateStringHandle(dwDDE,s,CP_WINANSI);
						
						if(hszItem!=0L)
						{
							// tell client to go to the URL (assume success)
							HDDEDATA hDdeData = ::DdeClientTransaction(NULL,0,hConv,hszItem,CF_TEXT,XTYP_REQUEST,60000,NULL);
							::DdeFreeStringHandle(dwDDE,hszItem);
							nResult = (hDdeData==0 ? (UINT)-1 : 0);
						}

						::DdeDisconnect(hConv);
					}

					::DdeFreeStringHandle(dwDDE,hszTopic);
				}

				::DdeFreeStringHandle(dwDDE,hszServName);
			}
		}

		if(dwDDE!=0L)
			::DdeUninitialize(dwDDE);
	}

	return(nResult==0);
}
