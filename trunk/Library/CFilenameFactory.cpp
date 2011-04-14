/*
	CFilenameFactory.cpp
	Classe base per la gestione dei nomi files (SDK/MFC).
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
#include "CFilenameFactory.h"

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
	GetFollowing()
*/
LPCSTR CFilenameFactory::GetFollowing(LPCSTR pFilename,LPCSTR pPathname/* = NULL*/)
{
	if(!pPathname || strcmp(pPathname,".")==0 || strcmp(pPathname,"")==0)
		_snprintf(m_szFileName,sizeof(m_szFileName)-1,"%s",pFilename);
	else
		_snprintf(m_szFileName,sizeof(m_szFileName)-1,"%s%s%s",pPathname,pPathname[strlen(pPathname)-1]=='\\' ? "" : "\\",pFilename);
	
	if(::FileExist(m_szFileName))
	{
		int n;
		int i=0;
		char* p;
		char szFileName[_MAX_FILEPATH+1];
		char szExt[_MAX_FILEPATH+1] = {""};
		
		do
		{
			strcpyn(szFileName,m_szFileName,sizeof(szFileName));
			p = strrchr(szFileName,'.');
			if(p)
			{
				strcpyn(szExt,p,sizeof(szExt));
				*p = '\0';
			}
			n = strlen(szFileName);
			_snprintf(szFileName+n,sizeof(szFileName)-1-n,"-%d%s",++i,szExt);
		} while(::FileExist(szFileName));

		strcpyn(m_szFileName,szFileName,sizeof(m_szFileName));
	}

	return(m_szFileName);
}

/*
	Abbreviate()
*/
LPCSTR CFilenameFactory::Abbreviate(	LPCSTR	pFilename,
								int		nMaxLength/* = ABBREVIATE_MAX_FNAME */,
								BOOL		bSaveExt/* = FALSE*/,
								BOOL		bStripPath/* = TRUE*/,
								LPCSTR	pDefaultString/* = NULL*/)
{
	char* p;
	char* P;
	char szFileName[_MAX_FILEPATH+1] = {0};
	char szExt[_MAX_FILEPATH+1] = {0};

	// elimina il path dal nome file
	if(bStripPath)
	{
		p = (char*)strrchr(pFilename,'\\');
		if(p)
			p++;
		if(!p)
			p = (char*)pFilename;
			
		P = (char*)strrchr(pFilename,'/');
		if(P)
			p = P++;
			
		strcpyn(szFileName,p,sizeof(szFileName));
	}
	else
		strcpyn(szFileName,pFilename,sizeof(szFileName));

	// se <= 0 non accorcia ma elimina solo il pathname
	if(nMaxLength > 0)
	{
		// ricava (e fa fuori dal nome) l'estensione del file
		if(bSaveExt)
		{
			p = strrchr(szFileName,'.');
			if(p)
			{
				strcpyn(szExt,p,sizeof(szExt));
				*p = '\0';
			}
		}

		// se la lunghezza totale (nome+ext) sfora il limite
		if((int)(strlen(szFileName) + strlen(szExt)) > nMaxLength)
		{
			// imposta la stringa per l'abbreviazione
			if(!pDefaultString)
				pDefaultString = ABBREVIATE_DEFAULT_STRING;
			int n = strlen(pDefaultString);
			
			// occhio, con un nome file lungo se c'e' piu' di un '.' nel nome ed il nome viene passato
			// senza estensione impostando il flag per salvare l'estensione puo' fare il botto, aggiustare
			memcpy(szFileName+(nMaxLength-(n+strlen(szExt))),pDefaultString,n);
			szFileName[nMaxLength-strlen(szExt)] = '\0';
			n = strlen(szFileName);
			if(!strnull(szExt))
				_snprintf(szFileName+n,sizeof(szFileName)-1-n,"%s",szExt);
		}
		else // nessuno sforamento, riattacca l'estensione
		{
			if(!strnull(szExt))
				strcat(szFileName,szExt);
		}
	}

	strcpyn(m_szFileName,szFileName,sizeof(m_szFileName));

	return(m_szFileName);
}
