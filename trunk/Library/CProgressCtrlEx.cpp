/*
	CProgressCtrlEx.cpp
	Classe per la progress bar dentro la status bar del dialogo (MFC).
	Luca Piergentili, 10/04/11
	lpiergentili@yahoo.com

	Codice originale da:
	ProgressBar.h
	Drop-in status bar progress control
	Written by Chris Maunder (chris@codeproject.com)
	Copyright (c) 1998.
	This code may be used in compiled form in any way you desire. This
	file may be redistributed unmodified by any means PROVIDING it is 
	not sold for profit without the authors written consent, and 
	providing that this notice and the authors name is included. If 
	the source code in this file is used in any commercial application 
	then an email to me would be nice.
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include "window.h"
#include "CDialogEx.h"
#include "CProgressCtrlEx.h"

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

BEGIN_MESSAGE_MAP(CProgressCtrlEx,CProgressCtrl)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

IMPLEMENT_DYNCREATE(CProgressCtrlEx,CProgressCtrl)

CProgressCtrlEx::CProgressCtrlEx() : m_pStatusBar(NULL)
{
	m_Rect.SetRect(0,0,0,0);
}

CProgressCtrlEx::CProgressCtrlEx(	LPCTSTR	strMessage,
							int		nSize /*=100*/, 
							int		MaxValue /*=100*/,
							BOOL		bSmooth /*=FALSE*/,
							int		nPane /*=0*/,
							CStatusBarCtrlEx* pBar /*=NULL*/) : m_pStatusBar(pBar)
{
	Create(strMessage,nSize,MaxValue,bSmooth,nPane);
}

CProgressCtrlEx::~CProgressCtrlEx()
{
	Clear();
}

CStatusBarCtrlEx* CProgressCtrlEx::GetStatusBar(void)
{
	if(m_pStatusBar)
	{
		return(m_pStatusBar);
	}
	else
	{
		CWnd *pMainWnd = AfxGetMainWnd();
		if(!pMainWnd)
			return(NULL);

		if(pMainWnd->IsKindOf(RUNTIME_CLASS(CDialogEx)))
		{
			CWnd* pMessageBar = ((CDialogEx*)pMainWnd)->GetMessageBar();
			return(DYNAMIC_DOWNCAST(CStatusBarCtrlEx,pMessageBar));
		}
/*		else if(pMainWnd->IsKindOf(RUNTIME_CLASS(CFrameWnd)))
		{
			CWnd* pMessageBar = ((CFrameWnd*)pMainWnd)->GetMessageBar();
			return(DYNAMIC_DOWNCAST(CStatusBarCtrlEx,pMessageBar));
		}
		else
*/			return(DYNAMIC_DOWNCAST(CStatusBarCtrlEx,pMainWnd->GetDescendantWindow(AFX_IDW_STATUS_BAR)));
	}
}

BOOL CProgressCtrlEx::Create(	LPCTSTR	strMessage,
						int		nSize /*=100*/,
						int		MaxValue /*=100*/,
						BOOL		bSmooth /*=FALSE*/,
						int		nPane /*=0*/)
{
	BOOL bSuccess = FALSE;
	
	CStatusBarCtrlEx *pStatusBar = GetStatusBar();

	if(!pStatusBar)
		return(FALSE);

	DWORD dwStyle = WS_CHILD|WS_VISIBLE;
#ifdef PBS_SMOOTH
	if(bSmooth)
		dwStyle |= PBS_SMOOTH;
#endif

	// Until m_nPane is initialized, Resize() must not be called. But it can be called (which 
	// happens in multi-threaded programs) in CProgressCtrlEx::OnEraseBkgnd after the control is 
	// created in CProgressCtrlEx::Create.
	m_strMessage = strMessage;	
	m_nSize = nSize;	
	m_nPane = nPane;	
	m_strPrevText = pStatusBar->GetText(m_nPane);

	// Create the progress bar
	CRect PaneRect(0,0,0,0);
	bSuccess = CProgressCtrl::Create(dwStyle,PaneRect,pStatusBar,1);
	if(!bSuccess)
		return(FALSE);

	SetRange(0,MaxValue);
	SetStep(1);

	m_strMessage = strMessage;
	m_nSize = nSize;
	m_nPane = nPane;
	m_strPrevText = pStatusBar->GetText(m_nPane);

	// Resize the control to its desired width
	Resize();

	return(TRUE);
}

void CProgressCtrlEx::Clear(void)
{
	if(!IsWindow(GetSafeHwnd()))
		return;

	// Hide the window. This is necessary so that a cleared window is not redrawn if "Resize" is called
	ModifyStyle(WS_VISIBLE,0);

	CString str;
	if(m_nPane==0)
		str.LoadString(AFX_IDS_IDLEMESSAGE);   // Get the IDLE_MESSAGE
	else
		str = m_strPrevText;                   // Restore previous text

	// Place the IDLE_MESSAGE in the status bar 
	CStatusBarCtrlEx *pStatusBar = GetStatusBar();
	if(pStatusBar)
	{
		pStatusBar->SetPanel(str,m_nPane);
		pStatusBar->UpdateWindow();
	}
}

BOOL CProgressCtrlEx::SetText(LPCTSTR strMessage)
{ 
	m_strMessage = strMessage; 
	return(Resize());
}

BOOL CProgressCtrlEx::SetSize(int nSize)
{
	m_nSize = nSize; 
	return(Resize());
}

COLORREF CProgressCtrlEx::SetBarColour(COLORREF clrBar)
{
#ifdef PBM_SETBKCOLOR
	if(!IsWindow(GetSafeHwnd()))
		return CLR_DEFAULT;

	return SendMessage(PBM_SETBARCOLOR,0,(LPARAM)clrBar);
#else
	UNUSED(clrBar);
	return CLR_DEFAULT;
#endif
}

COLORREF CProgressCtrlEx::SetBkColour(COLORREF clrBk)
{
#ifdef PBM_SETBKCOLOR
	if(!IsWindow(GetSafeHwnd()))
		return CLR_DEFAULT;

	return SendMessage(PBM_SETBKCOLOR,0,(LPARAM)clrBk);
#else
	UNUSED(clrBk);
	return CLR_DEFAULT;
#endif
}

BOOL CProgressCtrlEx::SetRange(int nLower,int nUpper,int nStep/* = 1 */)	
{	 
	if(!IsWindow(GetSafeHwnd()))
		return FALSE;

	// To take advantage of the Extended Range Values we use the PBM_SETRANGE32
	// message intead of calling CProgressCtrl::SetRange directly. If this is
	// being compiled under something less than VC 5.0, the necessary defines
	// may not be available.

#ifdef PBM_SETRANGE32
	ASSERT(-0x7FFFFFFF <= nLower && nLower <= 0x7FFFFFFF);
	ASSERT(-0x7FFFFFFF <= nUpper && nUpper <= 0x7FFFFFFF);
	SendMessage(PBM_SETRANGE32,(WPARAM)nLower,(LPARAM)nUpper);
#else
	ASSERT(0 <= nLower && nLower <= 65535);
	ASSERT(0 <= nUpper && nUpper <= 65535);
	CProgressCtrl::SetRange(nLower, nUpper);
#endif
	
	CProgressCtrl::SetStep(nStep);
	return TRUE;
}

int CProgressCtrlEx::SetPos(int nPos)	   
{
	if(!IsWindow(GetSafeHwnd()))
		return 0;

#ifdef PBM_SETRANGE32
	ASSERT(-0x7FFFFFFF <= nPos && nPos <= 0x7FFFFFFF);
#else
	ASSERT(0 <= nPos && nPos <= 65535);
#endif

	ModifyStyle(0,WS_VISIBLE);
	return CProgressCtrl::SetPos(nPos);
}

int CProgressCtrlEx::OffsetPos(int nPos) 
{ 
	if(!IsWindow(GetSafeHwnd()))
		return 0;

	ModifyStyle(0,WS_VISIBLE);
	return CProgressCtrl::OffsetPos(nPos);
}

int CProgressCtrlEx::SetStep(int nStep)
{ 
	if(!IsWindow(GetSafeHwnd()))
		return 0;

	ModifyStyle(0,WS_VISIBLE);
	return CProgressCtrl::SetStep(nStep);	 
}

int CProgressCtrlEx::StepIt() 			
{ 
	if(!IsWindow(GetSafeHwnd()))
		return 0;

	ModifyStyle(0,WS_VISIBLE);
	return CProgressCtrl::StepIt();    
}

BOOL CProgressCtrlEx::Resize() 
{
	if(!IsWindow(GetSafeHwnd()))
		return FALSE;

	CStatusBarCtrlEx *pStatusBar = GetStatusBar();
	if (!pStatusBar)
		return FALSE;

	// Redraw the window text
	if(IsWindowVisible())
	{
		pStatusBar->SetPanel(m_strMessage,m_nPane);
		pStatusBar->UpdateWindow();
	}

	// Calculate how much space the text takes up
	CClientDC dc(pStatusBar);
	CFont *pOldFont = dc.SelectObject(pStatusBar->GetFont());
	CSize size = dc.GetTextExtent(m_strMessage);		// Length of text
	int margin = dc.GetTextExtent(_T(" ")).cx * 2;	// Text margin
	dc.SelectObject(pOldFont);

	// Now calculate the rectangle in which we will draw the progress bar
	CRect rc;
	pStatusBar->GetRect(m_nPane,rc);

	// Position left of progress bar after text and right of progress bar
	// to requested percentage of status bar pane
	if(!m_strMessage.IsEmpty())
		rc.left += (size.cx + 2*margin);
	rc.right -= (rc.right - rc.left) * (100 - m_nSize) / 100;

	if(rc.right < rc.left)
		rc.right = rc.left;
	
	// Leave a litle vertical margin (10%) between the top and bottom of the bar
	int Height = rc.bottom - rc.top;
	rc.bottom -= Height/10;
	rc.top += Height/10;

	// If the window size has changed, resize the window
	if(rc != m_Rect)
	{
		MoveWindow(&rc);
		m_Rect = rc;
	}

	return TRUE;
}

BOOL CProgressCtrlEx::OnEraseBkgnd(CDC* pDC) 
{
	Resize();
	return CProgressCtrl::OnEraseBkgnd(pDC);
}
