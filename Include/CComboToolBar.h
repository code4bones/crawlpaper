/*
	CComboToolBar.h
	Classe per il combo nella toolbar (MFC).
	Nel file delle risorse bisogna definire l'id relativo al combo (IDC_COMBO_ON_TOOLBAR),
	in caso contrario produce un errore in compilazione.
	Luca Piergentili, 16/09/00
	lpiergentili@yahoo.com
*/
#ifndef _CCOMBOTOOLBAR_H
#define _CCOMBOTOOLBAR_H 1

#include "window.h"
#include "resource.h"
#ifndef IDC_COMBO_ON_TOOLBAR
  #error IDC_COMBO_ON_TOOLBAR not defined
#endif

/*
	CComboToolBar
*/
class CComboToolBar : public CToolBar
{
public:
	CComboToolBar();
	virtual ~CComboToolBar() {}

	inline void	SetMessage		(HWND hWnd,UINT nMessage) {m_hWnd = hWnd; m_nMessage = nMessage;}
	inline void	SetCommand		(HWND hWnd,UINT nCommand) {m_hWnd = hWnd; m_nCommand = nCommand;}

	void			OnComboSelEndOk	(void);
	void			OnComboSelChange	(void);

	CComboBox		m_wndCombo;

private:
	int			m_nComboIndex;
	char			m_szComboText[256];
	HWND			m_hWnd;
	UINT			m_nMessage;
	UINT			m_nCommand;

	DECLARE_MESSAGE_MAP()
};

#endif // _CCOMBOTOOLBAR_H
