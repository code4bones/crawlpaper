/*
	CTrayIcon.h
	Classe per la gestione dell'icona nel system tray (MFC).
	Luca Piergentili, 22/08/01
	lpiergentili@yahoo.com
*/
#ifndef _CTRAYICON_H
#define _CTRAYICON_H 1

#include <shellapi.h>
#include "window.h"
#include "CWindowsVersion.h"
#include "CTrayIconPosition.h"
#include "CPPTooltip.h"

// Windows e' un po' cazzone e piu' di <n> caratteri non li accetta
#define TRAYICON_MAX_TOOLTIP_TEXT		63											// testo tooltip per icona (tray), 63 o 127 per comctl32.dll >= 5.0
#define BALLOON_MAX_TOOLTIP_TITLE		63											// titolo balloon
#define BALLOON_MAX_TOOLTIP_TEXT		1024											// testo balloon
#define BALLOON_DEFAULT_TIMEOUT		10											// timeout (in sec.) di default per la rimozione
#define BALLOON_USE_NATIVE			0											// usa il balloon nativo		
#define BALLOON_USE_EXTENDED			1											// usa il balloon esteso
#define _MAX_MENU_STRING_LENGTH		TRAYICON_MAX_TOOLTIP_TEXT						// max len per voce menu

/*
	CTrayIcon
*/
class CTrayIcon : public CWnd
{
public:
	CTrayIcon();
	virtual ~CTrayIcon();

	// trayicon
	BOOL			Create			(												// crea l'icona nella tray area
								HWND		hWnd,									// finestra del chiamante
								HICON	hIcon,									// handle per l'icona
								UINT		nMsg,									// messaggio da inviare
								UINT		nMenuId,									// id risorsa del menu popup
								UINT		nBlankIconId = (UINT)-1L,					// id risorsa per icona per il tracking nella tray area
								LPCSTR	lpcszTooltip = NULL							// testo tooltip
								);
	BOOL			Destroy			(void);											// elimina l'icona dalla tray area

	BOOL			SetIcon			(HICON hIcon);										// imposta l'icona in base all'handle
	BOOL			SetIcon			(UINT nIconId);									// imposta l'icona in base all'id risorsa
	inline HICON	GetIcon			(void) const {return(m_hIcon);}						// restituisce l'handle dell'icona

	BOOL			SetToolTip		(LPCSTR lpcszTooltip = NULL);							// imposta il tooltip per l'icona
	inline LPCSTR	GetToolTip		(void) const {return(m_szToolTipText);}					// restituisce il tooltip per l'icona
	
	// balloon
	inline void	SetCssStyles		(LPCSTR lpcszCssStyles) {strcpyn(m_szCssStyles,lpcszCssStyles,sizeof(m_szCssStyles));}
	inline void	SetBalloonType		(int nType) {m_nBalloonType = nType;}					// imposta il tipo di balloon (nativo/esteso)
	inline void	SetBalloonCloseIcon	(int nID) {m_nCloseIconID = nID;}
	void			Balloon			(												// visualizza il (tooltip) balloon nativo, chiamare senza parametri per rimuoverlo
								LPCSTR	lpcszTitle = NULL,							// titolo
								LPCSTR	lpcszText = NULL,							// testo
								UINT		nIconType = MB_ICONINFORMATION,				// icona (nativo: info, warning, errore - esteso: custom)
								UINT		nTimeout = BALLOON_DEFAULT_TIMEOUT,			// timeout per rimozione
								UINT		nIconID = (UINT)-1L,						// handle x icona custom (esclude il nome file), solo per esteso
								LPCSTR	lpcszIconFileName = NULL,					// nome file x icona custom (esclude l'handle), solo per esteso
								CSize	iconSize = CSize(16,16),						// dimensione icona custom, solo per esteso
								UINT		nCloseIconID = (UINT)-1L						// id risorsa per icona chiusura, solo per esteso
								);
	static VOID CALLBACK BalloonTimerProc(HWND hWnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTime);	// callback per il timer per la rimozione del balloon

protected:
	void			OnDestroy		(void);

	BOOL				m_bIsValid;												// flag avvenuta creazione
	NOTIFYICONDATA		m_NotifyIconData;											// struttura dati per l'icona della rtay area
	HWND				m_hWndParent;												// handle della finestra a cui inviare i messaggi
	HICON			m_hIcon;													// handle dell'icona
	HICON			m_hBalloonIcon;											// handle dell'icona per il balloon (esteso)
	BOOL				m_bSharedIcon;												// per sapere quando rilasciare l'handle di cui sopra
	UINT				m_nMenuId;												// id risorsa per il menu popup
	UINT				m_nBlankIconId;											// id risorsa per l'icona da usare per il tracking nella tray area
	char				m_szToolTipText[TRAYICON_MAX_TOOLTIP_TEXT+1];					// testo per il tooltip per l'icona
	UINT				m_nTimeout;												// timeout per il balloon
	UINT				m_nTimerId;												// timer per il balloon
	CTrayIconPosition	m_TrayPosition;											// per determinare le coordinate dell'icona nella tray area
	CPPToolTip		m_TrayTooltip;												// per il (tooltip) balloon esteso
	CWindowsVersion	m_winVer;													// per determinare il tipo di balloon
	int				m_nBalloonType;											// tipo balloon in uso (0=nativo, 1=esteso)
	UINT				m_nCloseIconID;
	char				m_szCssStyles[512];											// stili css per l'html utilizzato nel balloon (esteso)

	DECLARE_MESSAGE_MAP()
};

#endif // _CTRAYICON_H
