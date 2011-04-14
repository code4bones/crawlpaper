/*
	CPropertyPageDialog.h
	Classe base per la pagina del dialogo basato sul property sheet (MFC).
	Luca Piergentili, 14/07/99
	lpiergentili@yahoo.com
*/
#ifndef _CPROPERTYPAGEOPTIONS_H
#define _CPROPERTYPAGEOPTIONS_H 1

#include <stdio.h>
#include <stdlib.h>
#include "strings.h"
#include "window.h"
#include "CPropertyPageList.h"
#include "CPropertySheetDialogMessages.h"

//#define HAVE_STATUSBAR 1

// testo per i bottoni dello sheet
#define IDOK_PROMPT				"&Ok"
#define IDCANCEL_CANCEL_PROMPT	"&Cancel"
#define IDCANCEL_EXIT_PROMPT		"E&xit"
#define MAX_BUTTON_PROMPT		32

/*
	CPropertyPageDialog
*/
class CPropertyPageDialog : public CPropertyPage
{
	DECLARE_DYNCREATE(CPropertyPageDialog)

public:
	// costruttore/distruttore
	CPropertyPageDialog();
	CPropertyPageDialog(int nID);
	~CPropertyPageDialog() {}

	// gestori
	virtual BOOL	OnInitDialog		(void);			// inizializzazione dialogo (pagina)
	virtual BOOL	OnSetActive		(void);			// attivazione pagina
	virtual BOOL	OnKillActive		(void);			// disattivazione pagina
	virtual void	OnKillSheet		(void);			// chiusura sheet
	virtual void	OnOk				(void) {}			// click su IDOK dello sheet
	virtual void	OnCancel			(void) {}			// click su IDCANCEL dello sheet

	// membri
	void			Reset					(void);

	inline void	SetTitle					(LPCSTR lpcszTitle) {::SetWindowText(AfxGetMainWnd()->m_hWnd,lpcszTitle);}
	inline void	SetStatusBarId				(int nID) {m_nStatusBarId = nID;}
#ifdef HAVE_STATUSBAR
	BOOL			SetStatusBar				(LPCSTR lpcszText);
	BOOL			GetStatusBar				(LPSTR lpszText,int nSize);
	LPCSTR		GetStatusBar				(void);
#else
	inline BOOL	SetStatusBar				(LPCSTR /*lpcszText*/) {return(FALSE);}
	inline BOOL	GetStatusBar				(LPSTR /*lpszText*/,int /*nSize*/) {return(FALSE);}
	inline LPCSTR	GetStatusBar				(void) {return(NULL);}
#endif
	inline void	SetPageList				(CPropertyPageList* pPropertyPageList) {m_pPropertyPageList = pPropertyPageList;}
	inline void	SetPropertySheetWnd			(CWnd* pWnd) {m_pPropertySheetWnd = pWnd;}

	inline void	ActivatePage				(int nID) {if(m_pPropertySheetWnd) ::SendMessage(m_pPropertySheetWnd->m_hWnd,WM_ACTIVATEPAGE,nID,0);}
	PROPERTYPAGE*	FindPage					(int nID);
	BOOL			EnterPageBusyState			(int nID);
	BOOL			LeavePageBusyState			(int nID);
	BOOL			LookForBusyPages			(int nID);
	BOOL			LookForPageBusy			(int nID);

	inline void	SetPropertyPageOkButton		(LPCSTR lpcszText) {strcpyn(m_szOkButtonText,lpcszText,MAX_BUTTON_PROMPT);}
	inline LPCSTR	GetPropertyPageOkButton		(void) {return(m_szOkButtonText);}

	inline void	SetPropertySheetOkButton		(LPCSTR lpcszText)	{if(m_pPropertySheetWnd) ::SendMessage(m_pPropertySheetWnd->m_hWnd,WM_SETPROPERTYSHEET_IDOKBUTTON,WM_SETPROPERTYSHEET_IDOKBUTTON,(LONG)lpcszText);}
	inline void	GetPropertySheetOkButton		(LPCSTR lpcszText)	{if(m_pPropertySheetWnd) ::SendMessage(m_pPropertySheetWnd->m_hWnd,WM_GETPROPERTYSHEET_IDOKBUTTON,WM_GETPROPERTYSHEET_IDOKBUTTON,(LONG)lpcszText);}
	inline void	SetPropertySheetCancelButton	(LPCSTR lpcszText)	{if(m_pPropertySheetWnd) ::SendMessage(m_pPropertySheetWnd->m_hWnd,WM_SETPROPERTYSHEET_IDCANCELBUTTON,WM_SETPROPERTYSHEET_IDCANCELBUTTON,(LONG)lpcszText);}
	inline void	GetPropertySheetCancelButton	(LPCSTR lpcszText)	{if(m_pPropertySheetWnd) ::SendMessage(m_pPropertySheetWnd->m_hWnd,WM_GETPROPERTYSHEET_IDCANCELBUTTON,WM_GETPROPERTYSHEET_IDCANCELBUTTON,(LONG)lpcszText);}

	inline void	EnablePropertySheetOkButton	(void)			{if(m_pPropertySheetWnd) ::SendMessage(m_pPropertySheetWnd->m_hWnd,WM_ENABLEPROPERTYSHEET_IDOKBUTTON,WM_ENABLEPROPERTYSHEET_IDOKBUTTON,0);}
	inline void	DisablePropertySheetOkButton	(void)			{if(m_pPropertySheetWnd) ::SendMessage(m_pPropertySheetWnd->m_hWnd,WM_DISABLEPROPERTYSHEET_IDOKBUTTON,WM_DISABLEPROPERTYSHEET_IDOKBUTTON,0);}

	inline void	EnablePropertySheetCancelButton(void)			{if(m_pPropertySheetWnd) ::SendMessage(m_pPropertySheetWnd->m_hWnd,WM_ENABLEPROPERTYSHEET_IDCANCELBUTTON,WM_ENABLEPROPERTYSHEET_IDCANCELBUTTON,0);}
	inline void	DisablePropertySheetCancelButton(void)			{if(m_pPropertySheetWnd) ::SendMessage(m_pPropertySheetWnd->m_hWnd,WM_DISABLEPROPERTYSHEET_IDCANCELBUTTON,WM_DISABLEPROPERTYSHEET_IDCANCELBUTTON,0);}

	inline BOOL	Dirty					(void)			{return(m_bDirty);}
	inline BOOL	KillPage					(void)			{return(m_bKillPage);}

private:
	BOOL			m_bDirty;							// flag per pagina visualizzata
	BOOL			m_bKillPage;						// flag per cambio pagina
	char			m_szOkButtonText[MAX_BUTTON_PROMPT+1];	// testo bottone IDOK dello sheet
	int			m_nStatusBarId;					// id del controllo
	CPropertyPageList* m_pPropertyPageList;				// lista per le pagine dello sheet
	CWnd*		m_pPropertySheetWnd;				// puntatore allo sheet

	DECLARE_MESSAGE_MAP()
};

#endif // _CPROPERTYPAGEOPTIONS_H
