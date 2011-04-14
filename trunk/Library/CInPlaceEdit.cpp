/*
	CInPlaceEdit.cpp
	Classe derivata (CListView) per l'aggiornamento del (sub)elemento della lista (MFC).
	La lista deve essere in modalita' report.
	La classe derivata per la lista deve definire il gestore per il messaggio LVN_ENDLABELEDIT,
	inviato al termine dell'editing del campo.
	Modifiche minori al codice originale: Editable subitems - Zafir Anjum (1998/08/06)
	Luca Piergentili, 12/12/02
	lpiergentili@yahoo.com
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include "window.h"
#include "strings.h"
#include "CInPlaceEdit.h"

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

BEGIN_MESSAGE_MAP(CInPlaceEdit,CEdit)
	ON_WM_CREATE()
	ON_WM_CHAR()
	ON_WM_KILLFOCUS()
	ON_WM_NCDESTROY()
END_MESSAGE_MAP()

/*
	CInPlaceEdit()
*/
CInPlaceEdit::CInPlaceEdit(int nItem,int nSubItem,CString strInitText) : m_strInitText(strInitText)
{
	// lasciare a TRUE affinche' l'oggetto si elimini automaticamente al termine dell'editing
	m_bAutoDelete = TRUE;
	
	// riga,colonna dell'elemento della lista da editare
	m_nItem = nItem;
	m_nSubItem = nSubItem;
	
	// flag per uscita dall'editing con il tasto ESC
	m_bEsc = FALSE;

	// picture per il campo di editing, se non impostata accetta qualsiasi carattere
	memset(m_szPicture,'\0',sizeof(m_szPicture));
}

/*
	PreTranslateMessage()
*/
VIRTUAL BOOL CInPlaceEdit::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message==WM_KEYDOWN)
	{
		if(pMsg->wParam==VK_RETURN || pMsg->wParam==VK_DELETE || pMsg->wParam==VK_ESCAPE || GetKeyState(VK_CONTROL))
		{
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);
			return(TRUE);
		}
	}

	return(CEdit::PreTranslateMessage(pMsg));
}

/*
	OnCreate()
*/
int CInPlaceEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CEdit::OnCreate(lpCreateStruct)==-1)
		return(-1);

	// Set the proper font
	CFont* pFont = GetParent()->GetFont();
	SetFont(pFont);

	// imposta il campo con il valore iniziale (quanto contenuto nell'elemento della lista)
	SetWindowText(m_strInitText);
	SetFocus();
	SetSel(0,-1);
	
	return(0);
}

/*
	OnChar()
*/
void CInPlaceEdit::OnChar(UINT nChar,UINT nRepCnt,UINT nFlags)
{
	// termina l'editing del campo
	if(nChar==VK_ESCAPE || nChar==VK_RETURN)
	{
		if(nChar==VK_ESCAPE)
			m_bEsc = TRUE;
		GetParent()->SetFocus();
	}
	else
	{
		// controlla la picture (se specificata il carattere, per essere accettato, deve essere compreso nella picture)
		// il tasto backspace, a differenza degli altri tasti di spostamento (home, etc.), genera un WM_CHAR
		BOOL bIsValidChar = TRUE;

		if(m_szPicture[0]!='\0')
			bIsValidChar = (nChar==VK_BACK || strchr(m_szPicture,nChar)) ? TRUE : FALSE;
		
		if(bIsValidChar)
		{
			CEdit::OnChar(nChar,nRepCnt,nFlags);

			// Resize edit control if needed

			// Get text extent
			CString str;
			GetWindowText(str);
			CWindowDC dc(this);
			CFont *pFont = GetParent()->GetFont();
			CFont *pFontDC = dc.SelectObject(pFont);
			CSize size = dc.GetTextExtent(str);
			dc.SelectObject(pFontDC);
			size.cx += 5; // add some extra buffer

			// Get client rect
			CRect rect,parentrect;
			GetClientRect(&rect);
			GetParent()->GetClientRect(&parentrect);

			// Transform rect to parent coordinates
			ClientToScreen(&rect);
			GetParent()->ScreenToClient(&rect);

			// Check whether control needs to be resized and whether there is space to grow
			if(size.cx > rect.Width())
			{
				if(size.cx + rect.left < parentrect.right)
					rect.right = rect.left + size.cx;
				else
					rect.right = parentrect.right;
				
				MoveWindow(&rect);
			}
		}
	}
}

/*
	OnKillFocus()
*/
void CInPlaceEdit::OnKillFocus(CWnd* pNewWnd)
{
	CEdit::OnKillFocus(pNewWnd);

	CString str;
	GetWindowText(str);

	// verifica se il campo ha subito modifiche
	BOOL bModified = strcmp((LPCTSTR)m_strInitText,(LPCTSTR)str)!=0;

	// Send Notification to parent of ListView ctrl
	LV_DISPINFO lvdispinfo = {0};
	lvdispinfo.hdr.hwndFrom    = GetParent()->m_hWnd;
	lvdispinfo.hdr.idFrom      = GetDlgCtrlID();
	lvdispinfo.hdr.code        = LVN_ENDLABELEDIT;
	lvdispinfo.item.mask       = LVIF_TEXT;
	lvdispinfo.item.iItem      = m_nItem;
	lvdispinfo.item.iSubItem   = m_nSubItem;
	lvdispinfo.item.pszText    = m_bEsc ? LPTSTR((LPCTSTR)m_strInitText) : LPTSTR((LPCTSTR)str);
	lvdispinfo.item.cchTextMax = m_bEsc ? m_strInitText.GetLength() : str.GetLength();
	lvdispinfo.item.lParam     = MAKELPARAM(bModified,m_bEsc);

	// invia il messaggio, da gestire nella classe derivata da CListView con: ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT,...)
	// nel gestore associato al messaggio LVN_ENDLABELEDIT deve essere aggiornato il campo della lista
	GetParent()->GetParent()->SendMessage(WM_NOTIFY,GetParent()->GetDlgCtrlID(),(LPARAM)&lvdispinfo);

	DestroyWindow();
}

/*
	OnNcDestroy()
*/
void CInPlaceEdit::OnNcDestroy(void)
{
	CEdit::OnNcDestroy();
	if(m_bAutoDelete)
		delete this;
}
