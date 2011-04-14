/*
	CProgressCtrlEx.h
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
#ifndef _CPROGRESSCTRLEX_H
#define _CPROGRESSCTRLEX_H 1

#include "window.h"
#include "CStatusBarCtrlEx.h"

class CProgressCtrlEx : public CProgressCtrl
{
	DECLARE_DYNCREATE(CProgressCtrlEx)

public:
	CProgressCtrlEx();
	CProgressCtrlEx(LPCTSTR	strMessage,
				int		nSize = 100,
				int		MaxValue = 100,
				BOOL		bSmooth = FALSE,
				int		nPane = 0,
				CStatusBarCtrlEx* pBar = NULL);
	
	~CProgressCtrlEx();
	
	BOOL Create(	LPCTSTR	strMessage,
				int		nSize = 100,
				int		MaxValue = 100,
				BOOL		bSmooth = FALSE,
				int		nPane = 0);

public:
	BOOL		SetRange		(int nLower,int nUpper,int nStep = 1);
	BOOL		SetText		(LPCTSTR strMessage);
	BOOL		SetSize		(int nSize);
	COLORREF	SetBarColour	(COLORREF clrBar);
	COLORREF	SetBkColour	(COLORREF clrBk);
	int		SetPos		(int nPos);
	int		OffsetPos		(int nPos);
	int		SetStep		(int nStep);
	int		StepIt		(void);
	void		Clear		(void);

protected:
	BOOL		OnEraseBkgnd	(CDC* pDC);
	BOOL		Resize		(void);
	CStatusBarCtrlEx *GetStatusBar(void);
	
	int		m_nSize;					// Percentage size of control
	int		m_nPane;					// ID of status bar pane progress bar is to appear in
	CString	m_strMessage;				// Message to display to left of control
	CString	m_strPrevText;				// Previous text in status bar
	CRect	m_Rect;					// Dimensions of the whole thing
	CStatusBarCtrlEx* const m_pStatusBar;	// Set in the ctor to explicitly state which status bar to use.

	DECLARE_MESSAGE_MAP()
};

#endif // _CPROGRESSCTRLEX_H
