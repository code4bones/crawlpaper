/*
	CSEFileInfo.cpp
	James Spibey, 04/08/1998
	spib@bigfoot.com
	Specification of the CSEFileInfo class.
	This class holds data regarding each file in an archive
	You are free to use, distribute or modify this code as long as this header is not removed or modified.

	Rimossi i riferimenti a MFC (solo SDK) e cambi vari.
	Luca Piergentili, 24/08/00
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
#include "CSEFileInfo.h"

#include "traceexpr.h"
//#define _TRACE_FLAG	_TRFLAG_TRACEOUTPUT
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
	SetData()
*/
BOOL CSEFileInfo::SetData(LPCSTR lpcszFileName)
{
	HANDLE hHandle;
	BOOL bFlag = FALSE;

	Reset();

	// apre il file
	if((hHandle = ::CreateFile(lpcszFileName,GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL))!=INVALID_HANDLE_VALUE)
	{
		// dimensione del file
		m_dwSize = ::GetFileSize(hHandle,NULL);
		::CloseHandle(hHandle);

		// imposta il pathname
		strcpyn(m_szPathName,lpcszFileName,sizeof(m_szPathName));
	
		// imposta il nome file
		strcpyn(m_szFileName,lpcszFileName,sizeof(m_szFileName));
		strrev(m_szFileName);
		char* p;
		if((p = strchr(m_szFileName,'\\'))!=(char*)NULL)
			*p = '\0';
		strrev(m_szFileName);

		bFlag = TRUE;
	}

	return(bFlag);
}

/*
	Reset()
*/
void CSEFileInfo::Reset()
{
	memset(m_szPathName,'\0',sizeof(m_szPathName));
	memset(m_szFileName,'\0',sizeof(m_szFileName));
	m_dwSize = m_dwOffset = 0L;
}
