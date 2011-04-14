/*
	CColorStatic.h
	Classe per il controllo statico con controllo dei colori di primo piano/fondo (MFC).
	Luca Piergentili, 06/06/03
	lpiergentili@yahoo.com

	Tratta da "Programmare Windows 95 con MFC" di Jeff Prosise (modifiche minori).
*/
#ifndef _CCOLORSTATIC_H
#define _CCOLORSTATIC_H 1

#include "window.h"

/*
	CColorStatic
*/
class CColorStatic : public CStatic
{
public:
	CColorStatic();
	virtual ~CColorStatic() {}

	void		SetTextColor	(COLORREF crColor);			// imposta il colore di primo piano
	void		SetBkColor	(COLORREF crColor);			// imposta il colore di fondo

protected:
	HBRUSH	CtlColor		(CDC* pDC,UINT nCtlColor);	// gestore per il cambio colore

private:
	COLORREF	m_crTextColor;							// colore di primo piano
	COLORREF	m_crBkColor;							// colore di sfondo
	CBrush	m_brBkgnd;							// pennello per i colori

	DECLARE_MESSAGE_MAP();
};

#endif // _CCOLORSTATIC_H
