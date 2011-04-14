/*
	CZoomView.h
	Classe per la vista scroll/zoom (MFC).
	Ripresa e modificata dall'originale:
	Written by Brad Pirtle, CS:72450,1156, Internet:pirtle@qlogic.com
	Copyright 1994, QuickLogic Corp., all rights reserved.
	Luca Piergentili, 05/09/00
	lpiergentili@yahoo.com
*/
#ifndef _CZOOMVIEW_H
#define _CZOOMVIEW_H 1

#include "window.h"

/*
	CZoomView
*/
class CZoomView : public CScrollView
{
	DECLARE_DYNCREATE(CZoomView)

protected:
	// protected constructor used by dynamic creation
	CZoomView();
	virtual ~CZoomView();

#ifdef _DEBUG
	// deve evitare il controllo sulla modalita' MM_ANISOTROPIC
	virtual void	AssertValid			(void) const;
#endif

public:
	// mapping mode
	inline void	SetMapMode			(int nMode)	{m_nMapMode = nMode;}
	inline int	GetMapMode			(void) const	{return(m_nMapMode);}
	inline void	SetCenterMode			(BOOL bCenter)	{m_bCenter = bCenter;}
	inline BOOL	GetCenterMode			(void) const	{return(m_bCenter);}

	// zooming functions
	typedef enum {
		MODE_ZOOMOFF,
		MODE_ZOOMIN,
		MODE_ZOOMOUT
	} ZOOMMODE;
	
	typedef enum {
		CURSOR_MODE_BYCLICK,
		CURSOR_MODE_BYMODE,
		CURSOR_MODE_NONE
	} CURSORZOOMMODE;

	// overridden CScrollView member functions
	void			CenterOnLogicalPoint	(CPoint ptCenter);
	CPoint		GetLogicalCenterPoint	(void);
	void			SetZoomSizes			(SIZE sizeTotal,const SIZE& sizePage = sizeDefault,const SIZE& sizeLine = sizeDefault,BOOL bInvalidate = TRUE);

	// zooming mode
	void			SetZoomMode			(ZOOMMODE zoomMode);
	inline ZOOMMODE GetZoomMode			(void) const {return(m_zoomMode);}

	// related cursors
	BOOL			SetZoomCursor			(UINT);
	inline void	SetCursorZoomMode		(CURSORZOOMMODE mode)	{m_cursorZoomMode = mode;}
	inline CURSORZOOMMODE GetCursorZoomMode	(void) const			{return(m_cursorZoomMode);}

	// to implement zooming functionality
	void			DoZoomIn				(CRect &rect);
	void			DoZoomIn				(CPoint *point = NULL,double delta = 1.25);
	void			DoZoomIn				(double factor);
	void			DoZoomOut				(double factor);
	void			DoZoomOut				(CPoint *point = NULL,double delta = 1.25);
	void			DoZoomOut				(CRect &rect);
	void			DoZoomFull			(void);
	
	// bdelmee code change
	inline double	GetZoomRatio			(void) const {return(m_zoomScale);}
	inline void	SetZoomRatio			(double r) {m_zoomScale = r;}

	// override this to get notified of zoom scale change
	virtual void	OnNotifyZoom			(void) {};

	// utility functions
	void			ViewDPtoLP			(LPPOINT lpPoints,int nCount = 1);
	void			ViewLPtoDP			(LPPOINT lpPoints,int nCount = 1);
	void			ViewLPtoDst			(LPRECT rcDst);
	void			ViewLPtoSrc			(LPRECT rcSrc);
	void			ViewLPtoSrc			(LPPOINT lpPoint,int nCount=1);
	void			ClientToDevice			(CPoint &point);
	void			NormalizeRect			(CRect &rect);
	void			DrawBox				(CDC &dc,CRect &rect,BOOL xor = TRUE);
	void			DrawLine				(CDC &dc,const int &x1,const int &y1,const int &x2,const int &y2,BOOL xor = TRUE);
	
	// handlers
	void			OnLButtonDown			(UINT nFlags,CPoint point);
	void			OnLButtonUp			(UINT nFlags,CPoint point);
	void			OnRButtonDown			(UINT nFlags,CPoint point);
	void			OnMouseMove			(UINT nFlags,CPoint point);
	BOOL			OnSetCursor			(CWnd* pWnd,UINT nHitTest,UINT message);
    
protected:
	virtual void	OnDraw				(CDC* pDC) {/*CZoomView::OnDraw(pDC);*/}
	virtual void	OnPrepareDC			(CDC* pDC,CPrintInfo* pInfo = NULL);

private:
	void			PersistRatio			(const CSize &orig,CSize &dest,CPoint &remainder);
	void			CalcBars				(BOOL bInvalidate = TRUE);

	ZOOMMODE		m_zoomMode;
	CURSORZOOMMODE	m_cursorZoomMode;
	BOOL			m_bCaptured;
	CRect		m_ptDragRect;
	CSize		m_origTotalDev;// original total size in device units
	CSize		m_origPageDev;	// original per page scroll size in device units
	CSize		m_origLineDev;	// original per line scroll size in device units
	double		m_zoomScale;
	HCURSOR		m_hZoomCursor;

	DECLARE_MESSAGE_MAP()
};

#endif // _CZOOMVIEW_H
