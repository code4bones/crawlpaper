/*
	CId3Lib.cpp
	Classe d'interfaccia con la libreria id3lib (http://www.id3lib.org/) per info/tags sui files .mp3
	Luca Piergentili, 21/06/03
	lpiergentili@yahoo.com
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include "strings.h"
#include <string.h>
#include "window.h"
#include "win32api.h"
#include "mmaudio.h"
#include "CId3v1Tag.h"
#include "CId3Lib.h"

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
	CId3Lib()
*/
CId3Lib::CId3Lib(LPCSTR lpcszFileName /*= NULL*/)
{
	memset(m_szYear,'\0',sizeof(m_szYear));

	if(lpcszFileName)
		Link(lpcszFileName);
}

/*
	Link()
*/
BOOL CId3Lib::Link(LPCSTR lpcszFileName)
{
	BOOL bRet = FALSE;
	
	if(::FileExist(lpcszFileName) && striright(lpcszFileName,MP3_EXTENSION)==0)
		bRet = CId3v1Tag::Link(lpcszFileName);

	return(bRet);
}
