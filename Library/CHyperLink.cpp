/*
	CHyperLink.cpp
	Classe per il collegamento ipertestuale (MFC).
	Luca Piergentili, 21/07/03
	lpiergentili@yahoo.com

	Codice iniziale ripreso da:
	HyperLink static control.
	Copyright Chris Maunder, 1997, 1998
	Feel free to use and distribute. May not be sold for profit. 
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include "window.h"
#include "CBrowser.h"
#include "CToolTipCtrlEx.h"
#include "CHyperLink.h"

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

// id per il tooltip
#define ID_TOOLTIP_CONTROL 1965

BEGIN_MESSAGE_MAP(CHyperLink,CStatic)
    ON_CONTROL_REFLECT(STN_CLICKED,OnClicked)
    ON_WM_CTLCOLOR_REFLECT()
    ON_WM_SETCURSOR()
    ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

/*
	CHyperLink()
*/
CHyperLink::CHyperLink()
{
	m_hCursor = NULL;
	m_crLinkColour = RGB(0,0,255);
	m_crVisitedColour = RGB(128,0,128);
	m_crHoverColour = ::GetSysColor(COLOR_HIGHLIGHT);
	m_bOverControl = FALSE;
	m_bVisited = FALSE;
	m_bUnderline = TRUE;
	m_bAdjustToFit = FALSE;
	m_strUrl = "";
	m_bAutoText = TRUE;
	m_strText = "";
	m_strToolTipText = "";
}

/*
	~CHyperLink()
*/
CHyperLink::~CHyperLink()
{
	m_Font.DeleteObject();
}

/*
	SetUrl()
*/
void CHyperLink::SetUrl(LPCSTR lpcszUrl)
{
	m_strUrl.Format("%s",lpcszUrl);

	if(m_bAdjustToFit)
		if(::IsWindow(GetSafeHwnd()))
			PositionWindow();
}

/*
	OpenUrl()
*/
BOOL CHyperLink::OpenUrl(LPCSTR lpcszUrl/*=NULL*/)
{
	BOOL bRes = FALSE;
	if(lpcszUrl)
		m_strUrl.Format("%s",lpcszUrl);
		
	if(!m_strUrl.IsEmpty())
	{
		CBrowser browser;
		bRes = browser.Browse(LPCSTR(m_strUrl));
	}

	return(bRes);
}

/*
	SetVisited()
*/
void CHyperLink::SetVisited(BOOL bVisited)
{ 
	m_bVisited = bVisited;
	if(::IsWindow(GetSafeHwnd()))
		Invalidate(); 
}

/*
	SetCursor()
*/
void CHyperLink::SetCursor(HCURSOR hCursor)
{ 
	m_hCursor = hCursor;
	if(!m_hCursor)
		SetDefaultCursor();
}

/*
	SetUnderline()
*/
void CHyperLink::SetUnderline(BOOL bUnderline)
{
	m_bUnderline = bUnderline;

	if(::IsWindow(GetSafeHwnd()))
	{
		LOGFONT lf;
		GetFont()->GetLogFont(&lf);
		lf.lfUnderline = (BYTE)m_bUnderline;

		m_Font.DeleteObject();
		m_Font.CreateFontIndirect(&lf);
		SetFont(&m_Font);

		Invalidate(); 
	}
}

/*
	SetAutoSize()
*/
void CHyperLink::SetAutoSize(BOOL bAutoSize)
{
	m_bAdjustToFit = bAutoSize;

	if(m_bAdjustToFit)
		if(::IsWindow(GetSafeHwnd()))
			PositionWindow();
}

/*
	PreSubclassWindow()
*/
void CHyperLink::PreSubclassWindow(void)
{
	// we want to get mouse clicks via STN_CLICKED
	DWORD dwStyle = GetStyle();
	::SetWindowLong(GetSafeHwnd(),GWL_STYLE,dwStyle|SS_NOTIFY);

	// check that the window text isn't empty. If it is, set it as the URL.
	if(m_strText.IsEmpty() && m_bAutoText)
		m_strText = m_strUrl;

	SetWindowText(m_strText);

	// create the font
	LOGFONT lf;
	GetFont()->GetLogFont(&lf);
	lf.lfUnderline = (BYTE)m_bUnderline;
	m_Font.CreateFontIndirect(&lf);
	SetFont(&m_Font);

	// adjust size of window to fit URL if necessary
	if(m_bAdjustToFit)
		PositionWindow();
	
	// try and load up a "hand" cursor
	SetDefaultCursor();

	// crea il tooltip
	m_wndToolTip.Create(this,TTS_ALWAYSTIP);
	m_wndToolTip.SetWidth(TOOLTIP_XXL_WIDTH);
	m_wndToolTip.SetDelay(TOOLTIP_REASONABLE_DELAYTIME);
	CRect rect(0,0,0,0);
	m_wndToolTip.AddRectTool(this,"hyperlink",&rect,ID_TOOLTIP_CONTROL);
	m_wndToolTip.UpdateTipText(m_strToolTipText,this,ID_TOOLTIP_CONTROL);
	GetClientRect(rect);
	m_wndToolTip.SetToolRect(this,ID_TOOLTIP_CONTROL,&rect);
	m_wndToolTip.Activate(TRUE);

	CStatic::PreSubclassWindow();
}

/*
	PositionWindow()
	
	Move and resize the window so that the window is the same size as the hyperlink text. This stops
	the hyperlink cursor being active when it is not directly over the text. If the text is left
	justified then the window is merely shrunk, but if it is centred or right justified then the window
	will have to be moved as well.
	Suggested by Pål K. Tønder 
*/
void CHyperLink::PositionWindow(void)
{
	if(!::IsWindow(GetSafeHwnd()) || !m_bAdjustToFit) 
		return;

	// get the current window position
	CRect rect;
	GetWindowRect(rect);

	CWnd* pParent = GetParent();
	if(pParent)
		pParent->ScreenToClient(rect);

	// get the size of the window text
	CDC* pDC = GetDC();
	CFont* pOldFont = pDC->SelectObject(&m_Font);
	CSize Extent = pDC->GetTextExtent(m_strText);
	pDC->SelectObject(pOldFont);
	ReleaseDC(pDC);

	// get the text justification via the window style
	DWORD dwStyle = GetStyle();

	// recalc the window size and position based on the text justification
	if(dwStyle & SS_CENTERIMAGE)
		rect.DeflateRect(0,(rect.Height() - Extent.cy) / 2);
	else
		rect.bottom = rect.top + Extent.cy;

	if(dwStyle & SS_CENTER)
		rect.DeflateRect((rect.Width() - Extent.cx) / 2,0);
	else if(dwStyle & SS_RIGHT) 
		rect.left  = rect.right - Extent.cx;
	else // SS_LEFT = 0, so we can't test for it explicitly 
		rect.right = rect.left + Extent.cx;

	// move the window
	SetWindowPos(NULL,rect.left,rect.top,rect.Width(),rect.Height(),SWP_NOZORDER);
}

/*
	SetDefaultCursor()
*/
void CHyperLink::SetDefaultCursor(void)
{
	if(!m_hCursor)
		m_hCursor = ::LoadCursor(NULL,MAKEINTRESOURCE(32649));
}

/*
	CtlColor()
*/
HBRUSH CHyperLink::CtlColor(CDC* pDC,UINT nCtlColor) 
{
	ASSERT(nCtlColor==CTLCOLOR_STATIC);

	if(m_bOverControl)
		pDC->SetTextColor(m_crHoverColour);
	else
	{
		if(m_bVisited)
			pDC->SetTextColor(m_crVisitedColour);
		else
			pDC->SetTextColor(m_crLinkColour);
	}

	// transparent text
	pDC->SetBkMode(TRANSPARENT);
	return((HBRUSH)GetStockObject(NULL_BRUSH));
}

/*
	OnSetCursor()
*/
BOOL CHyperLink::OnSetCursor(CWnd* /*pWnd*/,UINT /*nHitTest*/,UINT /*nMsg*/) 
{
	if(m_hCursor)
	{
		::SetCursor(m_hCursor);
		return(TRUE);
	}

	return(FALSE);
}

/*
	OnMouseMove()
*/
void CHyperLink::OnMouseMove(UINT nFlags,CPoint point) 
{
	CStatic::OnMouseMove(nFlags,point);

	if(m_bOverControl) // cursor is currently over control
	{
		CRect rect;
		GetClientRect(rect);

		if(!rect.PtInRect(point))
		{
			m_bOverControl = FALSE;
			ReleaseCapture();
			RedrawWindow();
			return;
		}
	}
	else // cursor has just moved over control
	{
		m_bOverControl = TRUE;
		RedrawWindow();
		SetCapture();
	}
}

/*
	OnClicked()
*/
void CHyperLink::OnClicked(void)
{
	if(OpenUrl())
		SetVisited(TRUE);
}
