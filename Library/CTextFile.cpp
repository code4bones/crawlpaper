/*
	CTextFile.cpp
	Classe derivata per interfaccia file di testo (SDK).
	Luca Piergentili, 06/07/98
	lpiergentili@yahoo.com
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "window.h"
#include "CBinFile.h"
#include "CTextFile.h"

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
	ReadLine()

	Legge una linea dal file di input.
	Elimina la coppia CRLF dal buffer di lettura restituendo la lunghezza della linea,
	restituisce FILE_EOF per fine file.
*/
DWORD CTextFile::ReadLine(LPSTR lpBuffer,DWORD dwToRead)
{
	LONG lCrlf = 0L;
	char* pCrlf;
	DWORD dwOffset = CBinFile::Seek(0L,FILE_CURRENT);

	if((dwToRead = CBinFile::Read(lpBuffer,dwToRead))!=FILE_EOF)
	{
		if(dwToRead > 0L)
		{
			if((pCrlf = (char*)memchr(lpBuffer,'\r',dwToRead))!=(char*)NULL)
			{
				if(*(pCrlf + 1)=='\n')
				{
					lCrlf = 2L;
					dwToRead = (pCrlf-lpBuffer);
					*pCrlf = '\0';
				}
				else
					lpBuffer[dwToRead] = '\0';
			}
			else
			{
#if 0
				lpBuffer[dwToRead] = '\0';
#else
				if((pCrlf = (char*)memchr(lpBuffer,'\n',dwToRead))!=(char*)NULL)
				{
					lCrlf = 1L;
					dwToRead = (pCrlf-lpBuffer);
					*pCrlf = '\0';
				}
				else
					lpBuffer[dwToRead] = '\0';
#endif
			}
			
			CBinFile::Seek((LONG)dwOffset + (LONG)dwToRead + lCrlf,FILE_BEGIN);
		}
		else
			dwToRead = FILE_EOF;
	}

	return(dwToRead);
}

/*
	WriteLine()

	Scrive una linea nel file di input.
	Restituisce il numero di caratteri scritti.
*/
DWORD CTextFile::WriteLine(LPCSTR lpcBuffer,DWORD dwToWrite/* = (DWORD)-1L */)
{
	char* pCrlf;

	if(dwToWrite==(DWORD)-1L)
		dwToWrite = strlen(lpcBuffer);

	if((pCrlf = (char*)memchr(lpcBuffer,'\r',dwToWrite))!=(char*)NULL)
		if(*(pCrlf + 1)=='\n')
			dwToWrite = (pCrlf-lpcBuffer);

	if((dwToWrite = CBinFile::Write(lpcBuffer,dwToWrite))!=FILE_EOF)
		if(CBinFile::Write("\r\n",2L)!=FILE_EOF)
			dwToWrite += 2L;

	return(dwToWrite);
}

/*
	WriteFormattedLine()

	Scrive una linea nel file di input formattandola secondo il formato e aggiungendo la coppia
	CRLF alla fine della linea. Il contenuto della linea, una volta formattata, non puo' superare
	gli 8192 caratteri.
	Restituisce il numero di caratteri scritti.
*/
DWORD CTextFile::WriteFormattedLine(LPSTR pFmt,...)
{
	LPSTR pArgs;
	DWORD dwToWrite;
	char buffer[8192];
	char buf[4096] = {0};

	// ricava gli argomenti
	pArgs = (LPSTR)&pFmt + sizeof(pFmt);
	_vsnprintf(buf,sizeof(buf)-1,pFmt,pArgs);
	dwToWrite = (DWORD)_snprintf(buffer,sizeof(buffer)-1,"%s\r\n",buf);
	
	return(dwToWrite==(DWORD)-1L ? 0 : CBinFile::Write(buffer,dwToWrite));
}
