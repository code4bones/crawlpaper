/*
	CComboBoxExt.cpp
	Classe per il combo con auto-completamento.
	Luca Piergentili, 08/09/00
	lpiergentili@yahoo.com

	Tratta da: Autocompleting combo-box, like the URL edit box in netscape,
	Copyright (c) 1998 by Chris Maunder (modifiche minori).
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include "window.h"
#include "CComboBoxExt.h"

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

BEGIN_MESSAGE_MAP(CComboBoxExt,CComboBox)
	ON_CONTROL_REFLECT(CBN_EDITUPDATE,OnEditUpdate)
END_MESSAGE_MAP()

/*
	CComboBoxExt()
*/
CComboBoxExt::CComboBoxExt()
{
	m_hWndParent = NULL;
	m_nMsg = (UINT)-1L;
	m_bAutoComplete = TRUE;
}

/*
	PreTranslateMessage()

	Need to check for backspace/delete. These will modify the text in
	the edit box, causing the auto complete to just add back the text
	the user has just tried to delete. 
*/
BOOL CComboBoxExt::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message==WM_KEYDOWN)
	{
		m_bAutoComplete = TRUE;

		int nVirtKey = (int)pMsg->wParam;
		
		if(nVirtKey==VK_DELETE || nVirtKey==VK_BACK)
			m_bAutoComplete = FALSE;
	}

	return(CComboBox::PreTranslateMessage(pMsg));
}

/*
	OnEditUpdate()

	Chiamata per le modifiche al campo.
*/
void CComboBoxExt::OnEditUpdate(void) 
{
	// if we are not to auto update the text, get outta here
	if(m_bAutoComplete) 
	{
		// Get the text in the edit box
		CString str;
		GetWindowText(str);
		int nLength = str.GetLength();

		// Currently selected range
		DWORD dwCurSel = GetEditSel();
		WORD dStart = LOWORD(dwCurSel);
		WORD dEnd = HIWORD(dwCurSel);

		// Search for, and select in, and string in the combo box that is prefixed
		// by the text in the edit box
		if(SelectString(-1,str)==CB_ERR)
		{
			SetWindowText(str); // No text selected, so restore what was there before
			if(dwCurSel!=CB_ERR)
				SetEditSel(dStart,dEnd);	//restore cursor postion
		}

		// Set the text selection as the additional text that we have added
		if(dEnd < nLength && dwCurSel!=CB_ERR)
			SetEditSel(dStart,dEnd);
		else
			SetEditSel(nLength,-1);

		if(m_hWndParent && m_nMsg!=(UINT)-1L)
			::SendMessage(m_hWndParent,m_nMsg,0L,0L);
	}
}
