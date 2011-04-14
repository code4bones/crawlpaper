/*
	CBalloon.cpp
	Classe per la visualizzazione del balloon tooltip (MFC).
	Luca Piergentili, 25/05/04
	lpiergentili@yahoo.com
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include <string.h>
#include "strings.h"
#include "window.h"
#include <afxtempl.h>
#include <afxdisp.h>
#include "CTrayIcon.h"
#include "CTrayIconPosition.h"
#include "CPPTooltip.h"
#include "CWindowsVersion.h"
#include "CBalloon.h"

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
	CBalloon()
*/
CBalloon::CBalloon()
{
	m_hWndParent = AfxGetMainWnd()->GetSafeHwnd();
	m_nTimeout = m_nTimerId = 0L;
	memset(m_szCssStyles,'\0',sizeof(m_szCssStyles));
	m_hIconInformation = m_hIconWarning = m_hIconError = NULL;

	// passando l'handle se la finestra principale non e' minimizzata il clic sul popup per chiuderlo le
	// da il focus portandola in primo piano, passando NULL invece quando si clicca sul popup per chiuderlo
	// la finestra principale non prende il focus
	m_pTooltipEx.Create(NULL);
//	m_pTooltipEx.Create(m_hWndParent);
	
	m_pTooltipEx.SetSize(CPPToolTip::PPTTSZ_MARGIN_CX,4);
	m_pTooltipEx.SetSize(CPPToolTip::PPTTSZ_MARGIN_CY,4);
	CWindowsVersion winVer;
	m_nBalloonType = (winVer.GetCommonControlsVer() >= PACKVERSION(5,0)) ? BALLOON_USE_NATIVE : BALLOON_USE_EXTENDED;
	m_nCloseIconID = 0;
}

/*
	~CBalloon()
*/
CBalloon::~CBalloon()
{
	if(m_hIconInformation)
		::DestroyIcon(m_hIconInformation),m_hIconInformation = NULL;
	if(m_hIconWarning)
		::DestroyIcon(m_hIconWarning),m_hIconWarning = NULL;
	if(m_hIconError)
		::DestroyIcon(m_hIconError),m_hIconError = NULL;
	
	if(m_nTimerId!=0L)
	{
		::KillTimer(m_hWndParent,m_nTimerId);
		m_nTimerId = 0L;
	}
}

/*
	Balloon()
*/
void CBalloon::Balloon(	LPCSTR	lpcszTitle/* = NULL*/,				// titolo
					LPCSTR	lpcszText/* = NULL*/,				// testo
					UINT		nIconType/* = MB_ICONINFORMATION*/,	// icona
					UINT		nTimeout/* = BALLOON_DEFAULT_TIMEOUT*/,	// timeout per rimozione
					UINT		nIconID/* = (UINT)-1L*/,				// handle x icona custom (esclude il nome file)
					LPCSTR	lpcszIconFileName/* = NULL*/,			// nome file x icona custom (esclude l'handle)
					CSize	iconSize/* = CSize(16,16)*/,			// dimensione icona custom
					UINT		nCloseIconID/* = (UINT)-1L*/,			// id risorsa per icona chiusura
					CPoint	ptPos/* = CPoint(-1,-1)*/			// coordinate per la visualizzazione
					)
{
	// per default visualizza il balloon nell'angolo superiore sinistro della tray area
	CPoint ptPosition;
	ptPosition.x = ptPos.x,ptPosition.y = ptPos.y;
	if(ptPosition.x==-1 || ptPosition.y==-1)
	{
		CTrayIconPosition TrayIconPosition;
		TrayIconPosition.GetPosition(ptPosition);
	}
	
	// controlla se deve visualizzare o rimuovere il balloon
	BOOL bRemoveBalloon = !lpcszTitle || !lpcszText;
	
	// ricava il timeout per la rimozione del balloon
	if(!bRemoveBalloon)
	{
		nTimeout = (nTimeout >= 1 && nTimeout <= 30) ? nTimeout : BALLOON_DEFAULT_TIMEOUT;
		nTimeout *= 1000L;
	}
	else
		nTimeout = 0;

	m_nTimeout = nTimeout;

	if(!bRemoveBalloon)
	{
		// imposta il timer per la rimozione
		if(nTimeout!=0)
		{
			if(m_nTimerId!=0L)
			{
				::KillTimer(m_hWndParent,m_nTimerId);
				m_nTimerId = 0L;
			}
			
			m_nTimerId = ::SetTimer(m_hWndParent,(UINT)this,m_nTimeout,(TIMERPROC)BalloonTimerProc);
		}

		// imposta lo stile
		if(m_szCssStyles[0]!='\0')
			m_pTooltipEx.SetCssStyles(m_szCssStyles);
		
		// formatta il testo
		char szText[1024];
		if(nCloseIconID!=(UINT)-1L)
			m_nCloseIconID = nCloseIconID;
		_snprintf(szText,
				sizeof(szText)-1,
				"<table>"
				"<tr>"
				"<td align=left><h4>%s</h4></td>"
				"<td align=right><a><icon idres=%d width=16 height=16 style=g hotstyle></a></td>"
				"</tr>"
				"</table><hr><br>"
				"%s",
				lpcszTitle,
				m_nCloseIconID,
				lpcszText);

		// visualizza il tooltip
		if(nIconID!=(UINT)-1L)
		{
			// carica l'icona dall'id
			//m_pTooltipEx.SetNotify(FALSE);
			m_pTooltipEx.SetEffectBk(CPPDrawManager::EFFECT_SOLID,0);
			m_pTooltipEx.SetDelayTime(PPTOOLTIP_TIME_AUTOPOP,1000);
			m_pTooltipEx.SetBehaviour(PPTOOLTIP_CLOSE_LEAVEWND);
			m_pTooltipEx.ShowHelpTooltip(&ptPosition,szText,nIconID,iconSize);
		}
		else
		{
			HICON hIcon = NULL;
			
			// carica l'icona dal file
			if(strnull(lpcszIconFileName))
			{
				switch(nIconType)
				{
					case MB_ICONINFORMATION:
						if(!m_hIconInformation)
							m_hIconInformation = ::LoadIcon(NULL,IDI_INFORMATION);
						hIcon = m_hIconInformation;
						break;
					case MB_ICONWARNING:
						if(!m_hIconWarning)
							m_hIconWarning = ::LoadIcon(NULL,IDI_WARNING);
						hIcon = m_hIconWarning;
						break;
					case MB_ICONERROR:
						if(!m_hIconError)
							m_hIconError = ::LoadIcon(NULL,IDI_ERROR);
						hIcon = m_hIconError;
						break;
				}
			}
			else // carica l'icona predefinita
			{
				hIcon = (HICON)::LoadImage(NULL,lpcszIconFileName,IMAGE_ICON,iconSize.cx,iconSize.cy,LR_DEFAULTCOLOR|LR_LOADFROMFILE);
			}
			
			if(hIcon)
			{
				//m_pTooltipEx.SetNotify(FALSE);
				m_pTooltipEx.SetEffectBk(CPPDrawManager::EFFECT_SOLID,0);
				m_pTooltipEx.SetDelayTime(PPTOOLTIP_TIME_AUTOPOP,1000);
				m_pTooltipEx.SetBehaviour(PPTOOLTIP_CLOSE_LEAVEWND);
				m_pTooltipEx.ShowHelpTooltip(&ptPosition,szText,hIcon);
			}
		}
	}
}

/*
	BalloonTimerProc()
*/
VOID CALLBACK CBalloon::BalloonTimerProc(HWND /*hWnd*/,UINT /*uMsg*/,UINT_PTR idEvent,DWORD /*dwTime*/)
{
	CBalloon* pBalloon = (CBalloon*)idEvent;	
	if(pBalloon)
	{
		// rimuove il timer
		if(pBalloon->m_nTimerId!=0L)
		{
			::KillTimer(pBalloon->m_hWndParent,pBalloon->m_nTimerId);
			pBalloon->m_nTimerId = 0L;
		}

		// rimuove il tooltip
		pBalloon->m_pTooltipEx.HideTooltip();
	}
}
