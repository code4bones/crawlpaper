/*
	CBalloon.h
	Classe per la visualizzazione del balloon tooltip (MFC).
	Luca Piergentili, 25/05/04
	lpiergentili@yahoo.com
*/
#ifndef _CBALLOON_H
#define _CBALLOON_H 1

#include "strings.h"
#include "window.h"
#include "CTrayIcon.h"
#include "CPPTooltip.h"

/*
	CBalloon
*/
class CBalloon
{
public:
	CBalloon();
	virtual ~CBalloon();

	void				Balloon(	LPCSTR	lpcszTitle = NULL,				// titolo
							LPCSTR	lpcszText = NULL,				// testo
							UINT		nIconType = MB_ICONINFORMATION,	// icona
							UINT		nTimeout = BALLOON_DEFAULT_TIMEOUT,// timeout per rimozione
							UINT		nIconID = (UINT)-1L,			// handle x icona custom (esclude il nome file)
							LPCSTR	lpcszIconFileName = NULL,		// nome file x icona custom (esclude l'handle)
							CSize	iconSize = CSize(16,16),			// dimensione icona custom
							UINT		nCloseIconID = (UINT)-1L,		// id risorsa per icona chiusura
							CPoint	ptPos = CPoint(-1,-1)			// coordinate per la visualizzazione
							);

	inline void		SetCssStyles		(LPCSTR lpcszCssStyles)	{strcpyn(m_szCssStyles,lpcszCssStyles,sizeof(m_szCssStyles));}
	inline void		SetBalloonType		(int nType)			{m_nBalloonType = nType;}
	inline void		SetBalloonCloseIcon	(int nID)				{m_nCloseIconID = nID;}

	inline void		SetIconInformation	(HICON hIcon)			{m_hIconInformation = hIcon;}
	inline void		SetIconWarning		(HICON hIcon)			{m_hIconWarning = hIcon;}
	inline void		SetIconError		(HICON hIcon)			{m_hIconError= hIcon;}

	static VOID CALLBACK BalloonTimerProc(HWND hWnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTime); // callback per il timer per la rimozione del balloon

protected:
	HWND				m_hWndParent;									// handle della finestra principale
	UINT				m_nTimeout;									// timeout per il balloon
	UINT				m_nTimerId;									// timer per il balloon
	char				m_szCssStyles[512];								// stili css per l'html utilizzato nel balloon (esteso)
	HICON			m_hIconInformation;								// handle per l'icona predefinita
	HICON			m_hIconWarning;								// handle per l'icona predefinita
	HICON			m_hIconError;									// handle per l'icona predefinita
	CPPToolTip		m_pTooltipEx;									// per il balloon
	int				m_nBalloonType;								// tipo balloon in uso (0=nativo, 1=esteso)
	UINT				m_nCloseIconID;
};

#endif // _CBALLOON_H
