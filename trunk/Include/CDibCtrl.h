/*
	CDibCtrl.h
	Classe per controllo DIB statico (MFC).
	Riadattata e modificata da:
	Luca Piergentili, 07/08/00
	lpiergentili@yahoo.com

	Riadattata, modificata ed ampliata a partire dal codice di:
	Copyright (C) 1998 by Jorge Lodos
	All rights reserved
	Distribute and use freely, except:
	1. Don't alter or remove this notice.
	2. Mark the changes you made
	Send bug reports, bug fixes, enhancements, requests, etc. to: lodos@cigb.edu.cu	
	Adapted from Mr.Lodos's code in order to use PaintLib's
	windows bitmap class and multi-format decoding
	B.Delmée 1998
*/
#ifndef _CDIBCTRL_H
#define _CDIBCTRL_H 1

#include "window.h"
#include "CImageFactory.h"
#include "CImageDraw.h"

/*
	CDibCtrl
*/
class CDibCtrl : public CStatic
{
public:
	CDibCtrl();
	virtual ~CDibCtrl() {}

BOOL Load(UINT nID,LPCSTR lpcszLibraryName,BOOL bRebuildPalette = TRUE,BOOL bClearBeforeDraw = TRUE);

	// carica l'immagine dal file
	BOOL			Load				(
								LPCSTR	lpszFileName,
								LPCSTR	lpcszLibraryName,
								BOOL		bRebuildPalette = TRUE,
								BOOL		bClearBeforeDraw = TRUE
								);
	
	// carica l'immagine dall'oggetto
	BOOL			Load				(
								CImage*	pImage,
								BOOL		bRebuildPalette = TRUE,
								BOOL		bClearBeforeDraw = TRUE
								);

	// scarica l'immagine
	void			Unload			(void);
	
	// puntatore all'oggetto immagine
	const CImage*	GetImage			(void) const {return(m_pImage);}

	// per disegnare in trasparenza
	inline void	SetTransparent		(BOOL bDrawTransparent,COLORREF cr) {m_bDrawTransparent = bDrawTransparent; m_cr = cr;}
	inline void	GetTransparent		(BOOL& bDrawTransparent,COLORREF& cr) {bDrawTransparent = m_bDrawTransparent; cr = m_cr;}

	void			Clear			(void);
	void			Paint			(BOOL bInvalidate = TRUE);

protected:
	BOOL			RealizePalette		(BOOL);
	HBRUSH		CtlColor			(CDC*,UINT);
	BOOL			OnQueryNewPalette	(void);
	void			OnPaletteChanged	(CWnd*);

private:
	char			m_szLibraryName[_MAX_PATH+1];
	CImageFactory	m_ImageFactory;
	CImage*		m_pImage;
	CImageDraw	m_ImageDraw;
	BOOL			m_bDrawTransparent;
	COLORREF		m_cr;
	BOOL			m_bValidDib;

	DECLARE_MESSAGE_MAP()
};

#endif // _CDIBCTRL_H
