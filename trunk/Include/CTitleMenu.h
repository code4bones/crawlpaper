/*
	CTitleMenu.h
	Based on the code of Per Fikse(1999/06/16) on codeguru.earthweb.com
	Author: Arthur Westerman
	Bug reports by : Brian Pearson 

	Luca Piergentili, 09/02/04 (modifiche minori)
*/
#ifndef _CTITLEMENU_H
#define _CTITLEMENU_H 1

#include "window.h"

/*
	CTitleMenu
*/
class CTitleMenu : public CMenu
{
	typedef UINT (CALLBACK* LPFNDLLFUNC1)(HDC,CONST PTRIVERTEX,DWORD,CONST PVOID,DWORD,DWORD);

public:
	CTitleMenu();
	virtual ~CTitleMenu();
	
	void			AddTitle			(LPCSTR lpcszTitle,UINT nID = (UINT)-1L);
	
	inline void	SetColor			(long cl)							{m_lLeft = cl;};
	inline void	SetGradientColor	(long cl)							{m_lRight = cl;};
	inline void	SetTextColor		(long cl)							{m_lText = cl;};
	inline void	SetEdge			(bool bShown,UINT nRemove = 0,UINT nAdd = 0) {m_bDrawEdge = bShown; (m_nEdgeFlags^=nRemove) |= nAdd;};// see ::DrawEdge for flag values

	inline long	GetColor			(void) const						{return(m_lLeft);};
	inline long	GetGradientColor	(void) const						{return(m_lRight);};
	inline long	GetTextColor		(void) const						{return(m_lText);};
	inline long	GetEdge			(void) const						{return(m_nEdgeFlags);};

	virtual void	MeasureItem		(LPMEASUREITEMSTRUCT lpMIS);
	virtual void	DrawItem			(LPDRAWITEMSTRUCT lpDIS);

protected:
	CFont		m_Font;
	long			m_lRight;
	long			m_lLeft;
	long			m_lText;
	bool			m_bDrawEdge;
	UINT			m_nEdgeFlags;
	char			m_szTitle[1024];
	BOOL			m_bCanDoGradientFill;
	HINSTANCE		m_hMSImg32;
	LPFNDLLFUNC1	m_lpfnGradientFill;
	
	HFONT		CreatePopupMenuTitleFont	(void);
	BOOL			GradientFill			(HDC hdc,
									CONST PTRIVERTEX pVertex,
									DWORD dwNumVertex,
									CONST PVOID pMesh,
									DWORD dwNumMesh,
									DWORD dwMode);
};

#endif // _CTITLEMENU_H
