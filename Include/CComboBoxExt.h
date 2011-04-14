/*
	CComboBoxExt.h
	Classe per il combo con auto-completamento.
	Luca Piergentili, 08/09/00
	lpiergentili@yahoo.com

	Tratta da: Autocompleting combo-box, like the URL edit box in netscape,
	Copyright (c) 1998 by Chris Maunder (modifiche minori).
*/
#ifndef _CCOMBOBOXEXT_H
#define _CCOMBOBOXEXT_H 1

#include "window.h"

/*
	CComboBoxExt
*/
class CComboBoxExt : public CComboBox
{
public:
	CComboBoxExt();
	virtual ~CComboBoxExt() {}
	
	virtual BOOL	PreTranslateMessage	(MSG* pMsg);
	inline void	SetNotify			(HWND hWndParent,UINT nMsg) {m_hWndParent = hWndParent; m_nMsg = nMsg;}

protected:
	void			OnEditUpdate		(void);

private:
	HWND			m_hWndParent;
	UINT			m_nMsg;
	BOOL			m_bAutoComplete;

	DECLARE_MESSAGE_MAP()
};

#endif // _CCOMBOBOXEXT_H
