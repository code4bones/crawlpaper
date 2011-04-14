/*
	CCaption.cpp
	Classe per la finestra popup sulla taskbar (MFC).
	Ripresa e modificata dall'originale di Bart Gysens (Iconizer DLL).
	Luca Piergentili, 11/09/03
	lpiergentili@yahoo.com
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include "window.h"
#include "CWindowsVersion.h"
#include "CWindowsXPTheme.h"
#include "CCaption.h"

#include "traceexpr.h"
//#define _TRACE_FLAG _TRFLAG_TRACEOUTPUT
//#define _TRACE_FLAG _TRFLAG_NOTRACE
#define _TRACE_FLAG_INFO _TRFLAG_NOTRACE
#define _TRACE_FLAG_WARN _TRFLAG_NOTRACE
#define _TRACE_FLAG_ERR _TRFLAG_NOTRACE

#if (defined(_DEBUG) && defined(_WINDOWS)) && (defined(_AFX) || defined(_AFXDLL))
  #pragma message("\t\t\t"__FILE__"("STR(__LINE__)"): using DEBUG_NEW macro")
  #define new DEBUG_NEW
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif

// lista per le finestre subclassate
static CWndCaptionList m_listWndCaption;

// messaggio per l'unsubclassing della finestra
const UINT WM_NCUNSUBCLASSWND = ::RegisterWindowMessage("NcUnSubclassWnd");

/*
	SubClassProcedure()
*/
static LRESULT CALLBACK SubClassProcedure(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	LONG lResult = 0;
	ITERATOR iter;
	WNDCAPTION* wnd = NULL;
	CCaption* pCaption = NULL;

	//static BOOL bRestored = FALSE;
	
	// cerca l'handle della finestra nella lista interna, se non lo trova errore
	if((iter = m_listWndCaption.First())!=(ITERATOR)NULL)
	{
		while(iter!=(ITERATOR)NULL)
		{
			wnd = (WNDCAPTION*)iter->data;
			if(wnd)
			{
				if(hWnd==wnd->hWnd)
				{
					pCaption = (CCaption*)wnd->pThis;
					break;
				}
			}

			iter = m_listWndCaption.Next(iter);
		}
	}
	if(!pCaption)
		return(lResult);

	// controlla il messaggio ricevuto dalla finestra subclassata
	//
	// elimina il subclassing
	if(uMsg==WM_NCUNSUBCLASSWND)
	{
		::SetWindowLong(hWnd,GWL_WNDPROC,(LONG)wnd->lWndProc);
		lResult = 1965;
	}
	// la finestra sta' per essere eliminata
	else if(uMsg==WM_NCDESTROY)
	{
		::SetWindowLong(hWnd,GWL_WNDPROC,(LONG)wnd->lWndProc);
		lResult = ::CallWindowProc((WNDPROC)wnd->lWndProc,hWnd,uMsg,wParam,lParam);			
	}
	// cambio dell'area del titolo (per attivazione/disattivazione finestra)
	else if(uMsg==WM_NCACTIVATE)
	{
		lResult = pCaption->OnNcActivate(hWnd,uMsg,wParam,lParam,wnd);
	}
	// doppio click sulla barra del titolo, esegue il gestore originale (massimizza/ripristina)
	// solo se non e' stato fatto doppio click sul bottone per l'icona usata per il rollup 
	else if(uMsg==WM_NCLBUTTONDBLCLK)
	{
		if((lResult = pCaption->OnNcLButtonDblClk(hWnd,uMsg,wParam,lParam,wnd))!=(LONG)-1L)
		{
			lResult = pCaption->OnDefault(hWnd,uMsg,wParam,lParam,wnd);
			CRect rc;
			::GetWindowRect(hWnd,&rc);
			//???
		}
	}
	// click sull'icona per il rollup
	else if(uMsg==WM_NCLBUTTONDOWN)
	{
		lResult = pCaption->OnNcLButtonDown(hWnd,uMsg,wParam,lParam,wnd);
	}
	else if(uMsg==WM_SYSCOMMAND)
	{
		if(/*wParam==SC_MAXIMIZE || */wParam==SC_RESTORE)
		{
			wnd->bRestored = TRUE;
			//bRestored = TRUE;
		}

		lResult = pCaption->OnDefault(hWnd,uMsg,wParam,lParam,wnd);
	}
	// rilascio del mouse dopo il click sull'icona per il rollup, controlla anche WM_LBUTTONUP
	// perche' cattura il mouse per intercettare i rilasci fuori dall'area in questione
	else if(uMsg==WM_NCLBUTTONUP || uMsg==WM_LBUTTONUP)
	{
		lResult = pCaption->OnNcLButtonUp(hWnd,uMsg,wParam,lParam,wnd);
	}
	// passaggio del cursore del mouse sulla barra del titolo
	else if(uMsg==WM_NCMOUSEMOVE)
	{
		lResult = pCaption->OnNcMouseMove(hWnd,uMsg,wParam,lParam,wnd);
	}
	// dimensionamento della finestra, se non si trova tra il click ed il rilascio sull'icona
	// per il rollup, salva la nuova altezza della finestra dopo il dimensionamento per il
	// rollup successivo
	else if(uMsg==WM_SIZE)// || uMsg==WM_SYSCOMMAND)
	{
		lResult = pCaption->OnDefault(hWnd,uMsg,wParam,lParam,wnd);
		
		if(!wnd->bInNcHandler)
		{
			CRect rcWnd(0,0,0,0);
			::GetWindowRect(hWnd,&rcWnd);
			if(rcWnd.bottom < 0) // viene dal click sul bottone per iconizzare
				;
			else // salvare il nuovo valore solo se non ritorna da un iconizzazione (restore) ???
			{
				if(wnd->bRestored)
				//if(bRestored)
					wnd->bRestored = FALSE;
					//bRestored = FALSE;
				else
					wnd->nBottom = rcWnd.bottom;
			}
		}
	}
	// messaggi non gestiti, disegna comunque l'icona (disegnare solo per WM_NCPAINT???)
	else
	{
		//WM_MOVE
		//WM_SIZE
		//WM_NCPAINT
		lResult = pCaption->OnDefault(hWnd,uMsg,wParam,lParam,wnd);
		pCaption->OnDrawButton(hWnd,uMsg,wParam,lParam,wnd);
	}

	return(lResult);
}

/*
	CCaption()
*/
CCaption::CCaption()
{
	// per disegnare il puntino nell'icona
	m_hPen = ::CreatePen(PS_SOLID,2,RGB(0,0,0));
}

/*
	~CCaption()
*/
CCaption::~CCaption()
{
	if(m_hPen)
		::DeleteObject(m_hPen);
}

/*
	Subclass()
*/
BOOL CCaption::Subclass(HWND hWnd)
{
	BOOL bSubClassed = FALSE;
	ITERATOR iter;
	WNDCAPTION* wnd = NULL;
	CCaption* pCaption = NULL;

	// al momento gestisce solo lo stile classico
	BOOL bIsWindowsClassicStyle = TRUE;
	CWindowsXPTheme* pXPTheme = (CWindowsXPTheme*)m_winVer.GetWindowsXPTheme();
	if(pXPTheme)
		bIsWindowsClassicStyle = !(pXPTheme->IsThemeActive() && pXPTheme->IsAppThemed());
	if(!bIsWindowsClassicStyle)
		return(bSubClassed);

	// subclassa solo se non ha gia' subclassato
	if((iter = m_listWndCaption.First())!=(ITERATOR)NULL)
	{
		while(iter!=(ITERATOR)NULL)
		{
			wnd = (WNDCAPTION*)iter->data;
			if(wnd)
			{
				if(hWnd==wnd->hWnd)
				{
					pCaption = (CCaption*)wnd->pThis;
					break;
				}
			}

			iter = m_listWndCaption.Next(iter);
		}
	}
	if(pCaption)
		if(pCaption==this)
			return(bSubClassed);

	// subclassing
	wnd = (WNDCAPTION*)m_listWndCaption.Add();
	if(wnd)
	{
		wnd->hWnd = hWnd;
		bSubClassed = (wnd->lWndProc = (WNDPROC)::SetWindowLong(hWnd,GWL_WNDPROC,(LONG)SubClassProcedure))!=0;
		wnd->bInNcHandler = FALSE;
		wnd->bLButtonDown = 0;
		wnd->bIconized = FALSE;
		wnd->bRestored = FALSE;
		wnd->nHeigth = -1;
		wnd->nBottom = -1;
		wnd->pThis = (LPVOID)this;
	}

	return(bSubClassed);
}

/*
	UnSubclass()
*/
BOOL CCaption::UnSubclass(HWND hWnd)
{
	BOOL bUnSubclassed = FALSE;
	ITERATOR iter;
	WNDCAPTION* wnd = NULL;
	CCaption* pCaption = NULL;

	// elimina il subclassing solo se la finestra e' stata subclassata
	if((iter = m_listWndCaption.First())!=(ITERATOR)NULL)
	{
		while(iter!=(ITERATOR)NULL)
		{
			wnd = (WNDCAPTION*)iter->data;
			if(wnd)
			{
				if(hWnd==wnd->hWnd)
				{
					pCaption = (CCaption*)wnd->pThis;
					break;
				}
			}

			iter = m_listWndCaption.Next(iter);
		}
	}
	if(!pCaption || pCaption!=this)
		return(bUnSubclassed);

	// elimina il subclassing
	if(::SendMessage(wnd->hWnd,WM_NCUNSUBCLASSWND,0L,0L)==1965)
	{
		m_listWndCaption.Remove(iter);
		// bug, bug, bug: non funge...
		::SendMessage(hWnd,WM_NCPAINT,1L,0L);
		bUnSubclassed = TRUE;
	}

	return(bUnSubclassed);
}

/*
	IsSubclassed()
*/
BOOL CCaption::IsSubclassed(HWND hWnd)
{
	BOOL bIsSubclassed = FALSE;
	ITERATOR iter;
	WNDCAPTION* wnd = NULL;
	CCaption* pCaption = NULL;
	
	if((iter = m_listWndCaption.First())!=(ITERATOR)NULL)
	{
		while(iter!=(ITERATOR)NULL)
		{
			wnd = (WNDCAPTION*)iter->data;
			if(wnd)
			{
				if(hWnd==wnd->hWnd)
				{
					pCaption = (CCaption*)wnd->pThis;
					break;
				}
			}

			iter = m_listWndCaption.Next(iter);
		}
	}

	if(!pCaption || pCaption!=this)
		bIsSubclassed = FALSE;
	else
		bIsSubclassed = TRUE;

	return(bIsSubclassed);
}

/*
	CalcCaptionRect()
*/
void CCaption::CalcCaptionRect(HWND hWnd,RECT& rcCaption)
{	
	DWORD dwStyle;
	SIZE sizeFrame;
	int nIconSize;	

	// get frame size of window
	dwStyle = ::GetWindowLong(hWnd,GWL_STYLE);
	sizeFrame.cx = ::GetSystemMetrics((dwStyle & WS_THICKFRAME) ? SM_CXSIZEFRAME : SM_CXFIXEDFRAME);
	sizeFrame.cy = ::GetSystemMetrics((dwStyle & WS_THICKFRAME) ? SM_CYSIZEFRAME : SM_CYFIXEDFRAME);

	// get width of icon/button in caption
	nIconSize = ::GetSystemMetrics(SM_CXSIZE);

	// calculate rectangle dimensions
	::GetWindowRect(hWnd,&rcCaption);
	rcCaption.bottom -= rcCaption.top;
	rcCaption.right  -= rcCaption.left;
	rcCaption.top     = 0;
	rcCaption.left    = 0;

	rcCaption.left   += sizeFrame.cx;
	rcCaption.right  -= sizeFrame.cx;
	rcCaption.top    += sizeFrame.cy;
	rcCaption.bottom  = rcCaption.top + ::GetSystemMetrics(SM_CYCAPTION) - ::GetSystemMetrics(SM_CYBORDER);
}

/*
	DrawCaptionButton()
*/
void CCaption::DrawCaptionButton(HDC hDc,int x,int y,int off)
{
	HPEN	hOldPen;
	RECT	rcCaptionIcon;	

	// select new pen
	hOldPen = (HPEN)::SelectObject(hDc,m_hPen);

	// calculate dimensions
	m_d1 = 2 + (::GetSystemMetrics(SM_CXSIZE) - 5) * 7 / 16;
	m_d2 = 2 + (::GetSystemMetrics(SM_CYSIZE) - 5) * 9 / 16;

/*	originale, con puntino in basso:

	rcCaptionIcon.top    = y + m_d1 + 2;
	rcCaptionIcon.left   = x + m_d1 + off;
	rcCaptionIcon.bottom = y + m_d2 + 2;
	rcCaptionIcon.right  = x + m_d2 + off;
*/
/*	con puntino al centro:

	rcCaptionIcon.top    = y + m_d1;
	rcCaptionIcon.left   = x + m_d1 + off;
	rcCaptionIcon.bottom = y + m_d2;
	rcCaptionIcon.right  = x + m_d2 + off;
*/
	// con linetta in alto a sx
	rcCaptionIcon.top    = y + m_d1 - 4;
	rcCaptionIcon.left   = x + m_d1 + off - 3;
	rcCaptionIcon.bottom = y + m_d2 - 5;
	rcCaptionIcon.right  = x + m_d2 + off + 2;
	
	// disegna il bottone con il puntino
	::Rectangle(hDc,rcCaptionIcon.left,rcCaptionIcon.top,rcCaptionIcon.right,rcCaptionIcon.bottom);

	// restore old pen
	::SelectObject(hDc,hOldPen);
}

/*
	OnDefault()
*/
LRESULT CCaption::OnDefault(HWND hWnd,UINT nMsg,WPARAM wParam,LPARAM lParam,WNDCAPTION* pWndCaption)
{
	// call default window procedure and redraw caption buttons
	return(::CallWindowProc(pWndCaption->lWndProc,hWnd,nMsg,wParam,lParam));
}

/*
	OnNcActivate()
*/
LRESULT CCaption::OnNcActivate(HWND hWnd,UINT nMsg,WPARAM wParam,LPARAM lParam,WNDCAPTION* pWndCaption)
{
	// call default window procedure and send WM_NCPAINT message
	LRESULT lResult = ::CallWindowProc(pWndCaption->lWndProc,hWnd,nMsg,wParam,lParam);
	::SendMessage(hWnd,WM_NCPAINT,1L,0L);
	return(lResult);
}

/*
	OnNcLButtonDown()
*/
LRESULT CCaption::OnNcLButtonDown(HWND hWnd,UINT nMsg,WPARAM wParam,LPARAM lParam,WNDCAPTION* pWndCaption)
{
	HDC hDc;
	POINT point;
	RECT	rcPos;
	RECT	rcWnd;
	LRESULT lResult = 0L;

	// redraw caption buttons
	if((hDc = ::GetWindowDC(hWnd))!=NULL)
	{
		// convert mouse postion relative to caption rectangle
		::GetWindowRect(hWnd,&rcWnd);
		point.x = MAKEPOINTS(lParam).x - rcWnd.left;
		point.y = MAKEPOINTS(lParam).y - rcWnd.top;

		// calculate rectangle position of iconize button
		CCaption::CalcCaptionRect(hWnd,rcPos);
		rcPos.top    += 2;	
		rcPos.bottom -= 2;
		rcPos.right  -= ((::GetSystemMetrics(SM_CXSIZE) * 3) - 2);
		rcPos.left    = rcPos.right - (::GetSystemMetrics(SM_CXSIZE) - 2);

		// check if mouse position is in rectangle
		if(::PtInRect(&rcPos,point))
		{
			// cattura il mouse per sentire il rilascio se viene rilasciato fuori dall'area di competenza
			::SetCapture(hWnd);
			if(wParam==HTCAPTION)
				pWndCaption->bInNcHandler = TRUE;
			
			pWndCaption->bLButtonDown = TRUE;

			// disegna il bottone
			DrawFrameControl(hDc,&rcPos,DFC_BUTTON,DFCS_BUTTONPUSH | DFCS_PUSHED);
			DrawCaptionButton(hDc,rcPos.left,rcPos.top,1);
		}
		else
			lResult = ::CallWindowProc(pWndCaption->lWndProc,hWnd,nMsg,wParam,lParam);
		
		::ReleaseDC(hWnd,hDc);
	}

	return(lResult);
}

/*
	OnNcLButtonDblClk()
*/
LRESULT CCaption::OnNcLButtonDblClk(HWND hWnd,UINT nMsg,WPARAM wParam,LPARAM lParam,WNDCAPTION* pWndCaption)
{
	HDC hDc;
	POINT point;
	RECT rcPos;
	RECT rcWnd;
	LRESULT lResult = 0L;

	// redraw caption buttons
	if((hDc = ::GetWindowDC(hWnd))!=NULL)
	{
		// convert mouse postion relative to caption rectangle
		::GetWindowRect(hWnd,&rcWnd);
		point.x = MAKEPOINTS(lParam).x - rcWnd.left;
		point.y = MAKEPOINTS(lParam).y - rcWnd.top;

		// calculate rectangle position of iconize button
		CCaption::CalcCaptionRect(hWnd,rcPos);
		rcPos.top    += 2;	
		rcPos.bottom -= 2;
		rcPos.right  -= ((::GetSystemMetrics(SM_CXSIZE) * 3) - 2);
		rcPos.left    = rcPos.right - (::GetSystemMetrics(SM_CXSIZE) - 2);

		// check if mouse position is in rectangle
		if(::PtInRect(&rcPos,point))
			lResult = (LONG)-1L;
	}

	return(lResult);
}

/*
	OnNcLButtonUp()
*/
LRESULT CCaption::OnNcLButtonUp(HWND hWnd,UINT nMsg,WPARAM wParam,LPARAM lParam,WNDCAPTION* pWndCaption)
{
	HDC hDc;
	POINT point;
	RECT rcPos;
	RECT rcWnd;
	LRESULT lResult = 0L;

	// per terminare di ricevere i messaggi con il mouse fuori dell'area client
	if(::GetCapture()==hWnd)
		::ReleaseCapture();

	// redraw caption buttons
	if((hDc = ::GetWindowDC(hWnd))!=NULL)
	{
		// convert mouse postion relative to caption rectangle
		::GetWindowRect(hWnd,&rcWnd);
		point.x = MAKEPOINTS(lParam).x - rcWnd.left;
		point.y = MAKEPOINTS(lParam).y - rcWnd.top;

		// calculate rectangle position of iconize button
		CCaption::CalcCaptionRect(hWnd,rcPos);
		rcPos.top    += 2;
		rcPos.bottom -= 2;
		rcPos.right  -= ((::GetSystemMetrics(SM_CXSIZE) * 3) - 2);
		rcPos.left    = rcPos.right - (::GetSystemMetrics(SM_CXSIZE) - 2);

		// check if mouse position is in rectangle
		if(pWndCaption->bLButtonDown)
		{
			pWndCaption->bIconized = TRUE;

			// ricava la dimensione della finestra
			CRect rc;
			::GetWindowRect(hWnd,&rc);

			// prima chiamata, salva l'altezza corrente della finestra
			if(pWndCaption->nHeigth==-1)
			{
				pWndCaption->nHeigth = 0;
				pWndCaption->nBottom = rc.bottom;
			}

			//int nMaximizedX = ::GetSystemMetrics(SM_CXMAXIMIZED);
			//int nMaximizedY = ::GetSystemMetrics(SM_CYMAXIMIZED);
			int nFullScreenX = ::GetSystemMetrics(SM_CXFULLSCREEN);
			//int nFullScreenY = ::GetSystemMetrics(SM_CYFULLSCREEN);

			// finestra aperta, rolla
			if(pWndCaption->nHeigth==0)
			{
				pWndCaption->nHeigth = 1;
				pWndCaption->nBottom = rc.bottom;
				rc.bottom = 0;
			}
			else if(pWndCaption->nHeigth==1) // rollato, ripristina
			{
				pWndCaption->nHeigth = 0;
				rc.bottom = pWndCaption->nBottom-rc.top;
			}

			// rolla o ripristina
			::MoveWindow(	hWnd,
						rc.left < 0 ? 0 : rc.left,
						rc.top < 0 ? 0 : rc.top,
						rc.right-rc.left > nFullScreenX ? nFullScreenX : rc.right-rc.left,
						rc.bottom,
						TRUE);
		}
		else
			lResult = ::CallWindowProc(pWndCaption->lWndProc,hWnd,nMsg,wParam,lParam);

		// disegna il bottone
		::DrawFrameControl(hDc,&rcPos,DFC_BUTTON,DFCS_BUTTONPUSH);
		DrawCaptionButton(hDc,rcPos.left,rcPos.top,0);

		pWndCaption->bLButtonDown = FALSE;

		::ReleaseDC(hWnd,hDc);
	}

	// per la (s)cattura del mouse
	pWndCaption->bInNcHandler = FALSE;

	return(lResult);
}

/*
	OnNcMouseMove()
*/
LRESULT CCaption::OnNcMouseMove(HWND hWnd,UINT nMsg,WPARAM wParam,LPARAM lParam,WNDCAPTION* pWndCaption)
{
	HDC hDc;
	POINT point;	
	RECT rcPos;
	RECT rcWnd;
	LRESULT lResult = 0L;

	// redraw caption buttons
	if((hDc = ::GetWindowDC(hWnd))!=NULL)
	{
		// convert mouse postion relative to caption rectangle
		::GetWindowRect(hWnd,&rcWnd);
		point.x = MAKEPOINTS(lParam).x - rcWnd.left;
		point.y = MAKEPOINTS(lParam).y - rcWnd.top;

		// calculate rectangle position of iconize button
		CCaption::CalcCaptionRect(hWnd,rcPos);
		rcPos.top    += 2;
		rcPos.bottom -= 2;
		rcPos.right  -= ((::GetSystemMetrics(SM_CXSIZE) * 3) - 2);
		rcPos.left    = rcPos.right - (::GetSystemMetrics(SM_CXSIZE) - 2);

		// check if mouse position is in rectangle
		if(::PtInRect(&rcPos,point))
		{
			::DrawFrameControl(hDc,&rcPos,DFC_BUTTON,pWndCaption->bLButtonDown ? DFCS_BUTTONPUSH | DFCS_PUSHED : DFCS_BUTTONPUSH);
			DrawCaptionButton(hDc,rcPos.left,rcPos.top,pWndCaption->bLButtonDown ? 1 : 0);
		}
		else
		{
			lResult = ::CallWindowProc(pWndCaption->lWndProc,hWnd,nMsg,wParam,lParam);
			if(pWndCaption->bLButtonDown)
			{				
				::DrawFrameControl(hDc,&rcPos,DFC_BUTTON,DFCS_BUTTONPUSH);
				DrawCaptionButton(hDc,rcPos.left,rcPos.top,0);
			}
		}
	
		::ReleaseDC(hWnd,hDc);
	}

	return(lResult);
}

/*
	OnDrawButton()
*/
LRESULT CCaption::OnDrawButton(HWND hWnd,UINT nMsg,WPARAM wParam,LPARAM lParam,WNDCAPTION* pWndCaption)
{
	HDC hDc;
	RECT rcPos;
	RECT rcWnd;
	LRESULT lResult = 0L;

	// redraw caption buttons
	if((hDc = ::GetWindowDC(hWnd))!=NULL)
	{
		// convert mouse postion relative to caption rectangle
		::GetWindowRect(hWnd,&rcWnd);

		// calculate rectangle position of iconize button
		CCaption::CalcCaptionRect(hWnd,rcPos);
		rcPos.top    += 2;	
		rcPos.bottom -= 2;
		rcPos.right  -= ((::GetSystemMetrics(SM_CXSIZE) * 3) - 2);
		rcPos.left    = rcPos.right - (::GetSystemMetrics(SM_CXSIZE) - 2);
		
		::DrawFrameControl(hDc,&rcPos,DFC_BUTTON,pWndCaption->bLButtonDown ? DFCS_BUTTONPUSH | DFCS_PUSHED : DFCS_BUTTONPUSH);
		DrawCaptionButton(hDc,rcPos.left,rcPos.top,pWndCaption->bLButtonDown ? 1 : 0);
	
		::ReleaseDC(hWnd,hDc);
	}

	return(lResult);
}
