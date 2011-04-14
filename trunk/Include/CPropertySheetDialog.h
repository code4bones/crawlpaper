/*
	CPropertySheetDialog.h
	Classe base per il dialogo basato sul property sheet.
	Luca Piergentili, 14/07/99
	lpiergentili@yahoo.com
*/
#ifndef _CPROPERTYSHEETDIALOG_H
#define _CPROPERTYSHEETDIALOG_H 1

#include <string.h>
#include "strings.h"
#include <ctype.h>
#include "window.h"
#include "CTrayIcon.h"
#include "CPropertyPageList.h"
#include "CPropertyPageDialog.h"
#include "CPropertySheetDialogMessages.h"

/*
	CPropertySheetDialog
*/
class CPropertySheetDialog : public CPropertySheet
{
	DECLARE_DYNCREATE(CPropertySheetDialog)

public:
	// costruttore/distruttore
	CPropertySheetDialog(CWnd* = NULL,CPropertyPageList* = NULL);
	virtual ~CPropertySheetDialog();

	// messaggi
	virtual BOOL	PreTranslateMessage	(MSG* msg);
	virtual BOOL	OnReturnKey		(void)											{return(FALSE);}
	virtual BOOL	OnEscapeKey		(void)											{return(TRUE);}
	virtual BOOL	OnFunctionKey		(WPARAM /*wFunctionKey*/,BOOL /*bShift*/, BOOL /*bCtrl*/)	{return(FALSE);}
	virtual BOOL	OnAltFunctionKey	(WPARAM /*wFunctionKey*/)							{return(FALSE);}
	virtual BOOL	OnVirtualKey		(WPARAM /*wVirtualKey*/,BOOL /*bShift*/,BOOL /*bCtrl*/)	{return(FALSE);}

	// gestori
	virtual void	OnSysCommand		(UINT,LPARAM);					// menu di sistema
	virtual LRESULT OnNotifyTrayIcon	(WPARAM /*wParam*/,LPARAM lParam);	// tray icon
	virtual BOOL	OnInitDialog		(void);						// inizializzazione dialogo
	virtual void	OnOK				(void);						// bottone IDOK dello sheet
	virtual void	OnCancel			(void);						// bottone IDCANCEL dello sheet
	LONG			OnActivatePage		(UINT wParam,LONG lParam);		// attiva la pagina

	inline BOOL	Create			(void) {return(m_bCreated);}		// per verificare la corretta inizializzazione dello sheet

private:
	void		SetSheetOkButtonText	(LPCSTR);						// cambia il testo del bottone OK
	LPCSTR	GetSheetOkButtonText	(void);						// ricava il testo del bottone OK
	void		EnableSheetButtons		(LPCSTR = IDCANCEL_EXIT_PROMPT);	// abilita OK e imposta CANCEL
	void		DisableSheetButtons		(LPCSTR = IDCANCEL_CANCEL_PROMPT);	// disabilita OK e imposta CANCEL
	void		EnableSheetButton		(UINT);
	void		DisableSheetButton		(UINT);
	
	LONG		OnSetSheetOkButtonText	(UINT,LONG);					// idem per la chiamata dalle pagine
	LONG		OnGetSheetOkButtonText	(UINT,LONG);
	LONG		OnEnableSheetButtons	(UINT,LONG);
	LONG		OnDisableSheetButtons	(UINT,LONG);
	LONG		OnPropertySheetButton	(UINT,LONG);

	BOOL				m_bCreated;								// flag per esito costruttore
	CPropertyPageList*	m_pPropertyPageList;						// lista per le pagine dello sheet
	CPropertyPage*		m_pActivePageOnButtonClick;					// puntatore alla pagina che ha ricevuto il click su IDOK
//	CStatusBarCtrl		m_StatusBar;								// status bar
	CTrayIcon*		m_pTrayIcon;								// icona nella tray area

	DECLARE_MESSAGE_MAP()
};

#endif // _CPROPERTYSHEETDIALOG_H
