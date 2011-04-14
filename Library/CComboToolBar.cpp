/*
	CComboToolBar.cpp
	Classe per il combo nella toolbar (MFC).
	Nel file delle risorse bisogna definire l'id relativo al combo (IDC_COMBO_ON_TOOLBAR),
	in caso contrario produce un errore in compilazione.
	Luca Piergentili, 16/09/00
	lpiergentili@yahoo.com
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include <string.h>
#include "window.h"
#include "CComboToolBar.h"
#include "resource.h"

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

BEGIN_MESSAGE_MAP(CComboToolBar,CToolBar)
	ON_CBN_SELENDOK(IDC_COMBO_ON_TOOLBAR,OnComboSelEndOk)
	ON_CBN_SELCHANGE(IDC_COMBO_ON_TOOLBAR,OnComboSelChange)
END_MESSAGE_MAP()

/*
	CComboToolBar()
*/
CComboToolBar::CComboToolBar() : CToolBar()
{
	m_nComboIndex = -1;
	memset(m_szComboText,'\0',sizeof(m_szComboText));
	m_nMessage = m_nCommand = (UINT)-1;
}

/*
	OnComboSelEndOk()
*/
void CComboToolBar::OnComboSelEndOk()
{
	m_nComboIndex = m_wndCombo.GetCurSel();
	m_wndCombo.GetLBText(m_nComboIndex,m_szComboText);
	AfxGetMainWnd()->SetFocus();
}

/*
	OnComboSelChange()
*/
void CComboToolBar::OnComboSelChange()
{
	m_nComboIndex = m_wndCombo.GetCurSel();
	m_wndCombo.GetLBText(m_nComboIndex,m_szComboText);
	AfxGetMainWnd()->SetFocus();

	if(m_nMessage!=(UINT)-1)
		/*AfxGetMainWnd()->*/::SendMessage(m_hWnd,m_nMessage,(WPARAM)m_nComboIndex,(LPARAM)m_szComboText);

	if(m_nCommand!=(UINT)-1)
		/*AfxGetMainWnd()->*/::SendMessage(m_hWnd,WM_COMMAND,MAKELONG(m_nCommand,0),0L);
}
