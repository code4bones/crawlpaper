/*
	mfcapi.c
	Implementazione di quanto omesso dall' API (MFC).
	Luca Piergentili, 13/09/98
	lpiergentili@yahoo.com
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include "window.h"
#include "mfcapi.h"

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
	AppendFileType()

	Aggiunge il tipo file alla stringa utilizzata dalla shell.
*/
void AppendFileType(CString& strFilter,CString& strTypeFilter,LPCSTR lpcszDesc,LPCSTR lpcszExt)
{
	strFilter += lpcszDesc;
	strFilter += '\0';
	strFilter += lpcszExt;
	strFilter += '\0';
	if(!strTypeFilter.IsEmpty())
		strTypeFilter += ';';
	strTypeFilter += lpcszExt;
}
