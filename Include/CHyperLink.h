/*
	CHyperLink.cpp
	Classe per il collegamento ipertestuale (MFC).
	Luca Piergentili, 21/07/03
	lpiergentili@yahoo.com

	Codice iniziale ripreso da:
	HyperLink static control.
	Copyright Chris Maunder, 1997, 1998
	Feel free to use and distribute. May not be sold for profit. 
*/
#ifndef _CHYPERLINK_H
#define _CHYPERLINK_H 1

#include "window.h"
#include "CToolTipCtrlEx.h"

/*
	CHyperLink
*/
class CHyperLink : public CStatic
{
public:
	CHyperLink();
	virtual ~CHyperLink();

	void			SetUrl			(LPCSTR lpcszUrl);
	LPCSTR		GetUrl			(void) const {return(LPCSTR(m_strUrl));}
	BOOL			OpenUrl			(LPCSTR lpcszUrl = NULL);

	inline void	SetLinkColor		(COLORREF crLinkColour) {m_crLinkColour = crLinkColour;}
	inline void	SetVisitedColor	(COLORREF crVisitedColour) {m_crVisitedColour = crVisitedColour;}
	inline void	SetHoverColor		(COLORREF crHoverColour) {m_crHoverColour = crHoverColour;}
	inline COLORREF GetLinkColour		(void) const {return(m_crLinkColour);}
	inline COLORREF GetVisitedColour	(void) const {return(m_crVisitedColour);}
	inline COLORREF GetHoverColour	(void) const {return(m_crHoverColour);}

	void			SetVisited		(BOOL bVisited);
	inline BOOL	GetVisited		(void) const {return(m_bVisited);}

	void			SetCursor			(HCURSOR hCursor);
	inline HCURSOR	GetCursor			(void) const {return(m_hCursor);}

	void			SetUnderline		(BOOL bUnderline);
	inline BOOL	GetUnderline		(void) const {return(m_bUnderline);}

	void			SetAutoSize		(BOOL bAutoSize);
	inline BOOL	GetAutoSize		(void) const {return(m_bAdjustToFit);}

	inline void	SetAutoText		(BOOL bAutoText) {m_bAutoText = bAutoText;}
	inline BOOL	GetAutoText		(void) const {return(m_bAutoText);}
	inline void	SetText			(LPCSTR lpcszString) {m_strText.Format("%s",lpcszString);}
	inline LPCSTR	GetText			(void) {return(LPCSTR(m_strText));}

	inline void	SetToolTip		(LPCSTR lpcszText) {m_strToolTipText.Format("%s",lpcszText);}
	inline LPCSTR	GetToolTip		(void) {return(LPCSTR(m_strToolTipText));}

protected:
	virtual void	PreSubclassWindow	(void);

	void			PositionWindow		(void);
	void			SetDefaultCursor	(void);

	HBRUSH		CtlColor			(CDC* pDC,UINT nCtlColor);
	BOOL			OnSetCursor		(CWnd* pWnd,UINT nHitTest,UINT nMsg);
	void			OnMouseMove		(UINT nFlags,CPoint point);
	void			OnClicked			(void);

private:	
	HCURSOR		m_hCursor;
	COLORREF		m_crLinkColour;
	COLORREF		m_crVisitedColour;
	COLORREF		m_crHoverColour;
	BOOL			m_bOverControl;
	BOOL			m_bVisited;
	BOOL			m_bUnderline;
	BOOL			m_bAdjustToFit;
	CString		m_strUrl;
	BOOL			m_bAutoText;
	CString		m_strText;
	CString		m_strToolTipText;
	CFont		m_Font;
	CToolTipCtrlEx m_wndToolTip;

	DECLARE_MESSAGE_MAP()
};

#endif // _CHYPERLINK_H
