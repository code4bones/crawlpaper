/*
	CToolTipCtrlEx.cpp
	Classe derivata per gestire il bug presente nella principale (MFC).
	Luca Piergentili, 17/08/00
	lpiergentili@yahoo.com

	Tratta da "Programmare Windows 95 con MFC" di Jeff Prosise (modifiche minori).
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include "strings.h"
#include "window.h"
#include "CNodeList.h"
#include "CToolTipCtrlEx.h"

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
	AddTooltip()
*/
BOOL CToolTipCtrlEx::AddTooltip(CWnd* pWnd,UINT nCtrlID,UINT nStringID)
{
	ITERATOR iter;
	TOOLTIPITEM* t;

	if((iter = m_TooltipList.First())!=(ITERATOR)NULL)
	{
		do
		{
			t = (TOOLTIPITEM*)iter->data;
			
			if(t)
				if(t->nStringID==nStringID)
					return(AddWindowTool(pWnd,t->pszText));

			iter = m_TooltipList.Next(iter);
		
		} while(iter!=(ITERATOR)NULL);
	}

	if((t = (TOOLTIPITEM*)m_TooltipList.Add())!=(TOOLTIPITEM*)NULL)
	{
		t->nCtrlID	= nCtrlID;
		t->nStringID	= nStringID;
		t->pszText	= NULL;
		t->bExtended	= FALSE;
		char szText[2048] = {0};
		int nLen = ::LoadString(AfxGetInstanceHandle(),t->nStringID,szText,sizeof(szText)-1);
		if(nLen > 0)
		{
			t->pszText = new char[nLen+1];
			if(t->pszText)
			{
				strcpyn(t->pszText,szText,nLen+1);
				return(AddWindowTool(pWnd,t->pszText));
			}
		}
	}

	return(FALSE);
}

/*
	AddWindowTool()
*/
BOOL CToolTipCtrlEx::AddWindowTool(CWnd* pWnd,LPCSTR pszText,UINT /*nIDTool*/ /* = 0 non utilizzato */)
{
	TOOLINFO ti;
	ti.cbSize = sizeof(TOOLINFO);
	ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS; // TTF_CENTERTIP;
	ti.hwnd = pWnd->GetParent()->GetSafeHwnd();
	ti.uId = (UINT)pWnd->GetSafeHwnd();
	ti.hinst = AfxGetInstanceHandle();
	ti.lpszText = (LPSTR)pszText;
	return((BOOL)SendMessage(TTM_ADDTOOL,0,(LPARAM)&ti));
}

/*
	AddRectTool()
*/
BOOL CToolTipCtrlEx::AddRectTool(CWnd* pWnd,LPCSTR pszText,LPCRECT lpRect,UINT nIDTool)
{
	TOOLINFO ti;
	ti.cbSize = sizeof(TOOLINFO);
	ti.uFlags = TTF_SUBCLASS;
	ti.hwnd = pWnd->GetSafeHwnd();
	ti.uId = nIDTool;
	ti.hinst = AfxGetInstanceHandle();
	ti.lpszText = (LPSTR)pszText;
	::CopyRect(&ti.rect,lpRect);
	return((BOOL)SendMessage(TTM_ADDTOOL,0,(LPARAM)&ti));
}

/*
	SetWidth()
*/
void CToolTipCtrlEx::SetWidth(int nWidth)
{
	// imposta la dimensione massima della finestra per il tooltip, non quella del testo
#if _MSC_VER >= 1200
	SetMaxTipWidth(nWidth);
#else
	SendMessage(TTM_SETMAXTIPWIDTH,0,(LPARAM)nWidth);
#endif
}
