/*
	CFileDialogEx.cpp
	Classi base per i dialoghi "File Open|Save as" (MFC).
	Luca Piergentili, 14/02/00
	lpiergentili@yahoo.com
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include "window.h"
#include "mfcapi.h"
#include "CFileDialogEx.h"

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
	DoModal()

	Mappa la classe base.
*/
int CFileDialogEx::DoModal(void)
{
	if(!m_lpszFilter)
	{
		// filtro di default
		::AppendFileType(m_strFilter,m_strDefaultFilter,"All files","*.*");
		CFileDialog::m_ofn.nMaxCustFilter++;
		CFileDialog::m_ofn.nFilterIndex = CFileDialog::m_ofn.nMaxCustFilter;
		CFileDialog::m_ofn.lpstrFilter = m_strFilter;
	}

	// directory iniziale
	CFileDialog::m_ofn.lpstrInitialDir = m_lpszInitialDir;

	// titolo
	CFileDialog::m_ofn.lpstrTitle = m_lpszTitle;
	
	// buffer per file selezionato
	CFileDialog::m_ofn.lpstrFile = m_szFileName;

	// lancia il dialogo
	return(CFileDialog::DoModal());
}
