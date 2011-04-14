/*
	CComboBoxEx.h
	Autocompleting combo-box (like the URL edit box in netscape)
	Written by Chris Maunder (Chris.Maunder@cbr.clw.csiro.au)
	Copyright (c) 1998.

	Modifiche minori.
	Luca Piergentili, 08/09/00
	lpiergentili@yahoo.com
*/
#ifndef _CCOMBOBOXEX_H
#define _CCOMBOBOXEX_H 1

// la zoccola del 6 definisce la stessa classe
#if _MSC_VER < 1200

#include "window.h"

class CComboBoxEx : public CComboBox
{
public:
	CComboBoxEx();
	virtual ~CComboBoxEx() {}
	virtual BOOL	PreTranslateMessage	(MSG* pMsg);

protected:
	void			OnEditUpdate		(void);

private:
	BOOL			m_bAutoComplete;

	DECLARE_MESSAGE_MAP()
};

#endif // _MSC_VER < 1200

#endif // _CCOMBOBOXEX_H
