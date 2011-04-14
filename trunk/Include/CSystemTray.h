/*
	CSystemTray.h
	Classe per l'icona nella System Tray.
	Riadattata dal codice originale (vedi sotto il copyright originale).
	Luca Piergentili, 27/04/00
	l.pierge@teleline.es
*/

// Written by Chris Maunder (chrismaunder@codeguru.com)
// Copyright (c) 1998.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included. If 
// the source code in  this file is used in any commercial application 
// then acknowledgement must be made to the author of this file 
// (in whatever form you wish).
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage to your
// computer, causes your pet cat to fall ill, increases baldness or
// makes you car start emitting strange noises when you start it up.
//
// Expect bugs.
// 
// Please use and enjoy. Please let me know of any bugs/mods/improvements 
// that you have found/implemented and I will fix/incorporate them into this
// file. 
#ifndef _INCLUDED_SYSTEMTRAY_H_
#define _INCLUDED_SYSTEMTRAY_H_

#include <afxwin.h>		// MFC core and standard components
#include <afxext.h>		// MFC extensions
#include <afxdisp.h>	// MFC OLE automation classes
#include <afxtempl.h>
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>		// MFC support for Windows Common Controls
#endif

/*-------------------------------------------------------------------------------------------------------------

	CSystemTrayDialog

-------------------------------------------------------------------------------------------------------------*/
class CSystemTrayDialog : public CDialog
{
public:
	CSystemTrayDialog(UINT);
	virtual ~CSystemTrayDialog()	{}
	
	int	GetDialogStatus(void)	{return(m_iStatus);}
	void	SetDialogStatus(int i);

private:
	int m_iStatus;
};

/*-------------------------------------------------------------------------------------------------------------

	CSystemTray

-------------------------------------------------------------------------------------------------------------*/
class CSystemTray : public CWnd
{
public:
	CSystemTray();
	CSystemTray(CWnd*,UINT,LPCTSTR,HICON,UINT);
	virtual ~CSystemTray();

	BOOL Enabled(void)	{return(m_bEnabled);}
	BOOL Visible(void)	{return(!m_bHidden);}

	// Create the tray icon
	Create(CWnd*,UINT,LPCTSTR,HICON,UINT,CSystemTrayDialog*);

	// Change or retrieve the Tooltip text
	BOOL		SetTooltipText(LPCTSTR);
	BOOL		SetTooltipText(UINT);
	CString	GetTooltipText(void) const;

	// Change or retrieve the icon displayed
	BOOL		SetIcon(HICON);
	BOOL		SetIcon(LPCTSTR);
	BOOL		SetIcon(UINT);
	BOOL		SetStandardIcon(LPCTSTR);
	BOOL		SetStandardIcon(UINT);
	HICON	GetIcon(void) const;
	void		HideIcon(void);
	void		ShowIcon(void);
	void		RemoveIcon(void);
	void		MoveToRight(void);

	// Change or retrieve the window to send notification messages to
	BOOL		SetNotificationWnd(CWnd*);
	CWnd*	GetNotificationWnd(void) const;

	// Default handler for tray notification message
	virtual LRESULT OnTrayNotification(WPARAM,LPARAM);

protected:
	virtual LRESULT WindowProc(UINT,WPARAM,LPARAM);
	void Initialise(void);

private:
	CSystemTrayDialog*	m_pCDialog;	// puntatore al dialogo relativo all'icona della system tray
	BOOL				m_bEnabled;	// does O/S support tray icon?
	BOOL				m_bHidden;	// Has the icon been hidden?
public:
	NOTIFYICONDATA		m_tnd;		// to process taskbar status area messages
};

#endif
