/*
	CHyperLink.h
	Classe base per il collegamento ipertestuale (MFC).
	Luca Piergentili, 18/09/01
	lpiergentili@yahoo.com
*/
#ifndef _CHYPERLINK_H
#define _CHYPERLINK_H 1

#include "window.h"

class CMyHyperLink : public CStatic
{
public:
	CMyHyperLink();
	virtual ~CMyHyperLink() {}
	
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	void SetUrl(LCPSTR lpcszUrl) {strcpyn(m_szUrl,lpcszUrl,sizeof(m_szUrl));}
	LPCSTR GetUrl(void) const {return(m_szUrl);}

	void SetText(LCPSTR lpcszText) {strcpyn(m_szText,lpcszText,sizeof(m_szText)); this->SetWindowText(m_szText);}
	LPCSTR GetText(void) const {return(m_szText);}

	void SetToolTip(LPCSTR szToolTip) {m_ctrlToolTip.UpdateTipText(szToolTip,this,1965);}

	void SetColor(COLORREF sLinkColor) {m_crfLinkColor = sLinkColor;}
	void SetHoverColor(COLORREF cHoverColor) {m_crfOnMouseHoverColor = cHoverColor;}
	void SetVisitedColor(COLORREF sVisitedColor) {m_crfVisitedColor = sVisitedColor;}

protected:
	virtual void	PreSubclassWindow	(void);
	void			OnMouseMove		(UINT nFlags,CPoint point);
	BOOL			OnSetCursor		(CWnd* pWnd,UINT nHitTest,UINT message);
	void			OnClicked			(void);
	HBRUSH		CtlColor			(CDC* pDC,UINT nCtlColor);
	
	HCURSOR		m_hHyperLinkCursor;
	BOOL			m_bIsMouseOver;
	BOOL			m_bIsVisited;

	CFont		m_fontText;
	CToolTipCtrl	m_ctrlToolTip;

	char			m_szText[256];
	char			m_szUrl[256];

	COLORREF		m_crfOnMouseHoverColor;
	COLORREF		m_crfLinkColor;
	COLORREF		m_crfVisitedColor;

	DECLARE_MESSAGE_MAP()
};

#endif // _CHYPERLINK_H
