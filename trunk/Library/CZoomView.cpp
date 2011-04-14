/*
	CZoomView.h
	Classe per la vista scroll/zoom (MFC).
	Ripresa e modificata dall'originale:
	Written by Brad Pirtle, CS:72450,1156, Internet:pirtle@qlogic.com
	Copyright 1994, QuickLogic Corp., all rights reserved.
	Luca Piergentili, 05/09/00
	lpiergentili@yahoo.com
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include "window.h"
#include "CZoomView.h"
#include "limits.h"

#include "traceexpr.h"
//#define _TRACE_FLAG	_TRFLAG_TRACEOUTPUT
#define _TRACE_FLAG		_TRFLAG_NOTRACE
#define _TRACE_FLAG_INFO	_TRFLAG_NOTRACE
#define _TRACE_FLAG_WARN	_TRFLAG_NOTRACE
#define _TRACE_FLAG_ERR	_TRFLAG_NOTRACE

#if (defined(_DEBUG) && defined(_WINDOWS)) && (defined(_AFX) || defined(_AFXDLL))
#ifdef PRAGMA_MESSAGE_VERBOSE
  #pragma message("\t\t\t"__FILE__"("STR(__LINE__)"): using DEBUG_NEW macro")
#endif
  #define new DEBUG_NEW
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif

// screen pixels apart for region zoom
#define PICKMARGIN 10

IMPLEMENT_DYNCREATE(CZoomView,CScrollView)

BEGIN_MESSAGE_MAP(CZoomView,CScrollView)
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_RBUTTONDOWN()
    ON_WM_SETCURSOR()
END_MESSAGE_MAP()

/*
	CZoomView()
*/
CZoomView::CZoomView() : CScrollView()
{
	m_zoomMode = MODE_ZOOMOFF;
	m_cursorZoomMode = CURSOR_MODE_NONE;
	m_bCaptured = FALSE;
	m_zoomScale = 1.0;
	m_ptDragRect.SetRectEmpty();
	m_bCenter = FALSE;
	m_nMapMode = MM_ANISOTROPIC;
	m_hZoomCursor = NULL;
}

/*
	~CZoomView()
*/
VIRTUAL CZoomView::~CZoomView()
{
	if(m_hZoomCursor)
		::DestroyCursor(m_hZoomCursor),m_hZoomCursor = NULL;
}

/*
	AssertValid()
*/
#ifdef _DEBUG
VIRTUAL void CZoomView::AssertValid(void) const
{
	// deve saltare la AssertValid() di CScrollView per evitare il controllo sulla
	// modalita' MM_ANISOTROPIC che non essendo supportata genererebbe un asserzione
	CView::AssertValid();
}
#endif //_DEBUG

/*
	CenterOnLogicalPoint()
   
	Same as CScrollView::CenterOnPoint(), but for logical coordinates.
*/
void CZoomView::CenterOnLogicalPoint(CPoint pt)
{
	// convert the point to device coordinates
	ViewLPtoDP(&pt);

	// account for scroll bar position
	ClientToDevice(pt);
	
	// use CScrollView's function for device coordinates
	CScrollView::CenterOnPoint(pt);
}

/*
	GetLogicalCenterPoint()

	Get the center of screen in logical coordinates.
*/
CPoint CZoomView::GetLogicalCenterPoint(void)
{
	// point in logical units
	CPoint pt;
	CRect rect;
	
	// get the center of screen
	GetClientRect(&rect);
	pt.x = (rect.Width()  / 2);
	pt.y = (rect.Height() / 2);

	// convert the point to logical coordinates
	ViewDPtoLP(&pt);
	return(pt);
}

/*
	SetZoomSizes()

	Set up the CZoomView class with the logical page size, and scrolling page/line units.
	This replaces CScrollView::SetScrollSizes().
*/
void CZoomView::SetZoomSizes(SIZE sizeTotal,const SIZE& sizePage,const SIZE& sizeLine,BOOL bInvalidate)
{
	// set up the defaults
	ASSERT(sizeTotal.cx >= 0 && sizeTotal.cy >= 0);
	
	// need for arbitrary scaling
	// m_nMapMode = MAPMODE;

	m_totalLog = sizeTotal;

	// setup default Viewport extent to be conversion of Window extent into device units
	// block for DC
	{
		CWindowDC dc(NULL);
		dc.SetMapMode(m_nMapMode);

		// total size
		m_totalDev = m_totalLog;
		dc.LPtoDP((LPPOINT)&m_totalDev);
	}

	// save the original Viewport Extent
	m_origTotalDev = m_totalDev;
	
	// save the original scrollbar info - for CalcBars
	m_origPageDev = sizePage;
	m_origLineDev = sizeLine;
	
	// figure out scroll bar info
	CalcBars(bInvalidate);
	
	// notify the class that the zoom scale was set
	OnNotifyZoom();
}

/*
	SetZoomMode()
	
	Put the view into the specified zoom mode.
*/
void CZoomView::SetZoomMode(ZOOMMODE zoomMode)
{
	if(zoomMode!=m_zoomMode)
	{
		m_zoomMode = zoomMode;
		// force cursor change now
		OnSetCursor(NULL,HTCLIENT,0);
	}
}

/*
	SetZoomCursor()

	Carica il cursore specificato.
*/
BOOL CZoomView::SetZoomCursor(UINT nID)
{
	m_hZoomCursor = ::LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(nID));
	return(m_hZoomCursor!=NULL);
}

/*
	DoZoomIn()

	Zoom the view in on a rect (in logical coordinates).
*/
void CZoomView::DoZoomIn(CRect& rect)
{
	// make sure that the rect is normalized
	CRect rcZoom = rect;
	NormalizeRect(rcZoom);

	// get the center of rect
	CPoint ptCenter;
	ptCenter.x = ((rcZoom.left + rcZoom.right)  / 2);
	ptCenter.y = ((rcZoom.top  + rcZoom.bottom) / 2);

	// see if the rect is small enough for a point zoom (device coordinates)
	CRect rectDP = rcZoom;
	ViewLPtoDP((LPPOINT)&rectDP,2);
	BOOL bPointZoom = max(rectDP.Width(),rectDP.Height()) < PICKMARGIN;
	
	// just do normal point zoom
	if(bPointZoom)
	{
		DoZoomIn(&ptCenter);
		return;
	}

	CRect rcClient;
	GetClientRect(&rcClient);

	// calculate the new zoom scale
	double scaleH = (double)(rcClient.right  + 1) / (double)rcZoom.Width();
	double scaleV = (double)(rcClient.bottom + 1) / (double)rcZoom.Height();

	// keep the scale isotropic
	double Temp = m_zoomScale;
	m_zoomScale = min(scaleH,scaleV);
	if(m_zoomScale > 28.42)
		m_zoomScale = Temp;

	// modify the Viewport extent
	m_totalDev.cx = (int)((double)m_origTotalDev.cx * m_zoomScale);
	m_totalDev.cy = (int)((double)m_origTotalDev.cy * m_zoomScale);
	CalcBars();

	// set the current center point
	CenterOnLogicalPoint(ptCenter);

	// notify the class that a new zoom scale was done
	OnNotifyZoom();
}

/*
	DoZoomIn()

	Zoom the view in on a point (logical coordinates) by the specified scale factor.
*/
void CZoomView::DoZoomIn(CPoint *point,double delta)
{
	CPoint ptCenter;

	// save the current center point
	if(!point)
		ptCenter = GetLogicalCenterPoint();
	else
		ptCenter = *point;

	double Temp = m_zoomScale;

	// increase the zoom scale
	m_zoomScale *= delta;
	if(m_zoomScale > 28.42)
	{
		TRACE("m_zoomScale > %.2f\n",m_zoomScale);
		m_zoomScale = Temp;
		TRACE("m_zoomScale = %.2f\n",m_zoomScale);
	}

	// modify the Viewport extent
	m_totalDev.cx = (int)((double)m_origTotalDev.cx * m_zoomScale);
	m_totalDev.cy = (int)((double)m_origTotalDev.cy * m_zoomScale);
	CalcBars();

	// Set the current center point
	CenterOnLogicalPoint(ptCenter);

	// notify the class that a new zoom scale was done
	OnNotifyZoom();
}

/*
	DoZoomIn()

	Zoom the view in by the specified factor.
*/
void CZoomView::DoZoomIn(double factor)
{
	// save the current center point
	CPoint ptCenter = GetLogicalCenterPoint();

	// set the zoom factor
	m_zoomScale = factor;
	//m_zoomScale = min(m_zoomScale,GetZoomMax());

	// modify the viewport extent
	m_totalDev.cx = (int)((double)m_origTotalDev.cx * m_zoomScale);
	m_totalDev.cy = (int)((double)m_origTotalDev.cy * m_zoomScale);
	CalcBars();

	// set the current center point
	CenterOnLogicalPoint(ptCenter);

	// notify the class that a new zoom scale was done
	OnNotifyZoom();
}

/*
	DoZoomOut()

	Zoom the view out by factor.
*/
void CZoomView::DoZoomOut(double factor)
{
	// save the current center point
	CPoint ptCenter = GetLogicalCenterPoint();

	// set the zoom factor
	m_zoomScale = factor;

	// modify the Viewport extent
	m_totalDev.cx = (int)((double)m_origTotalDev.cx * m_zoomScale);
	m_totalDev.cy = (int)((double)m_origTotalDev.cy * m_zoomScale);
	CalcBars();

	// set the current center point (logical coordinates)
	CenterOnLogicalPoint(ptCenter);

	// notify the class that a new zoom scale was done
	OnNotifyZoom();
}

/*
	DoZoomOut()
	
	Zoom the view out on a point (logical coordinates) by one scale factor.
*/
void CZoomView::DoZoomOut(CPoint *point,double delta)
{
	CPoint ptCenter;

	// save the current center point
	if(!point)
		ptCenter = GetLogicalCenterPoint();
	else
		ptCenter = *point;

	// decrease the zoom scale
	m_zoomScale /= delta;

	// modify the Viewport extent
	m_totalDev.cx = (int)((double)m_origTotalDev.cx * m_zoomScale);
	m_totalDev.cy = (int)((double)m_origTotalDev.cy * m_zoomScale);
	CalcBars();
	
	// set the current center point (logical coordinates
	CenterOnLogicalPoint(ptCenter);

	// notify the class that a new zoom scale was done
	OnNotifyZoom();
}

/*
	DoZoomOut()

	Zoom the view out on a rect.
*/
void CZoomView::DoZoomOut(CRect &rect)
{
	CRect rcZoom = rect;
	NormalizeRect(rcZoom);

	CPoint ptCenter;
	ptCenter.x = ((rcZoom.left + rcZoom.right)  / 2);
	ptCenter.y = ((rcZoom.top  + rcZoom.bottom) / 2);

	CRect rcClient;
	GetClientRect(&rcClient);

	// calculate the new zoom scale
	double scaleH = (double)rcZoom.Width()  / (double)(rcClient.right  + 1);
	double scaleV = (double)rcZoom.Height() / (double)(rcClient.bottom + 1);

	// keep the scale Isotropic
	m_zoomScale = max(scaleH,scaleV);

	// modify the viewport extent
	m_totalDev.cx = (int)((double) m_origTotalDev.cx * m_zoomScale);
	m_totalDev.cy = (int)((double) m_origTotalDev.cy * m_zoomScale);
	CalcBars();

	// set the current center point (logical coordinates)
	CenterOnLogicalPoint(ptCenter);
	
	// notify the class that a new zoom scale was done
	OnNotifyZoom();
}

/*
	DoZoomFull()

	Zoom the view to full state.
*/
void CZoomView::DoZoomFull(void)
{
	CRect  rc;
	CPoint pt;
	CSize  sizeSb;
	
	// just set Viewport Extent to Client size for full fit
	GetTrueClientSize(m_totalDev,sizeSb);
	
	// maintain original ratio
	PersistRatio(m_totalLog,m_totalDev,pt);
	
	// set the new zoom scale (could use cx or cy)
	m_zoomScale = (double)m_totalDev.cx / (double)m_origTotalDev.cx;
	
	// remove the scrollbars
	UpdateBars();

	// complete redraw
	Invalidate(TRUE);

	// notify the class that a new zoom scale was done
	OnNotifyZoom();
}

/*
	ViewDPtoLP()

	Same as DPtoLP, but gets the Client DC for the view.
*/
void CZoomView::ViewDPtoLP(LPPOINT lpPoints,int nCount)
{
    // convert to logical units
    // called from View when no DC is available
    ASSERT(m_nMapMode > 0); // must be set
    CWindowDC dc(this);
    OnPrepareDC(&dc);
    dc.DPtoLP(lpPoints,nCount);
}

/*
	ViewLPtoDP()

	Same as LPtoDP, but gets the Client DC for the view.
*/
void CZoomView::ViewLPtoDP(LPPOINT lpPoints,int nCount)
{
    // convert to logical units
    // called from View when no DC is available
    ASSERT(m_nMapMode > 0); // must be set
    CWindowDC dc(this);
    OnPrepareDC(&dc);
    dc.LPtoDP(lpPoints,nCount);
}

/*
	ViewLPtoDst()
*/
void CZoomView::ViewLPtoDst(LPRECT rcDst)
{
	// convert to logical units
	// called from View when no DC is available
	ASSERT(rcDst!=NULL);
	ASSERT(m_nMapMode > 0); // must be set
	ASSERT(m_nMapMode==MM_ANISOTROPIC || m_nMapMode==MM_TEXT);
	switch(m_nMapMode)
	{
		case MM_ANISOTROPIC:
		{
			InflateRect(rcDst,1,1);
			CRect rcDoc(0,0,m_totalLog.cx,m_totalLog.cy);
			rcDoc &= *rcDst;
			*rcDst = rcDoc;
			break;
		}

		case MM_TEXT:		
		{
			CRect rcDoc(0,0,m_totalDev.cx,m_totalDev.cy);
			rcDoc &= *rcDst;
			*rcDst = rcDoc;
			break;
		}
	}
}

/*
	ViewLPtoSrc()
*/
void CZoomView::ViewLPtoSrc(LPRECT rcSrc)
{
	// convert to logical units
	// called from View when no DC is available
	ASSERT(rcSrc!=NULL);
	ASSERT(m_nMapMode > 0); // must be set
	ASSERT(m_nMapMode==MM_ANISOTROPIC || m_nMapMode==MM_TEXT);
	switch(m_nMapMode)
	{
		case MM_ANISOTROPIC:
		{
			InflateRect(rcSrc,1,1);
			CRect rcDoc(0,0,m_origTotalDev.cx,m_origTotalDev.cy);
			rcDoc &= *rcSrc;
			*rcSrc = rcDoc;
			break;
		}

		case MM_TEXT:
		{
			rcSrc->left   /= (LONG)m_zoomScale;
			rcSrc->right  /= (LONG)m_zoomScale;
			rcSrc->top    /= (LONG)m_zoomScale;
			rcSrc->bottom /= (LONG)m_zoomScale;
			CRect rcDoc(0,0,m_origTotalDev.cx,m_origTotalDev.cy);
			rcDoc &= *rcSrc;
			*rcSrc = rcDoc;
			break;
		}
	}
}

/*
	ViewLPtoSrc()
*/
void CZoomView::ViewLPtoSrc(LPPOINT lpPoint,int nCount)
{
    // convert to logical units
    // called from View when no DC is available
	ASSERT(lpPoint!=NULL);
	ASSERT(m_nMapMode > 0); // must be set
	ASSERT(m_nMapMode==MM_ANISOTROPIC || m_nMapMode==MM_TEXT);
	switch(m_nMapMode)
	{
		case MM_ANISOTROPIC:
			break;
		
		case MM_TEXT:
			for(register int i=0; i < nCount; i++)
			{
				lpPoint[i].x /= m_zoomScale;
				lpPoint[i].y /= m_zoomScale;
				
				if(lpPoint[i].x > m_origTotalDev.cx)
					lpPoint[i].x = m_origTotalDev.cx;
				
				if(lpPoint[i].y > m_origTotalDev.cy)
					lpPoint[i].y = m_origTotalDev.cy;
			}
			break;
	}
}

/*
	ClientToDevice()
	
	Convert from Client coordinates to relative Device coordinates.
*/
void CZoomView::ClientToDevice(CPoint &point)
{
    // need to account for scrollbar position
    CPoint scrollPt = CScrollView::GetDeviceScrollPosition();
    point.x += scrollPt.x;
    point.y += scrollPt.y;
}

/*
	NormalizeRect()

	Normalize the rectangle.
*/
void CZoomView::NormalizeRect(CRect &rect)
{
	if(rect.left > rect.right)
	{
		int r = rect.right;
		rect.right = rect.left;
		rect.left = r;
	}
	
	if(rect.top > rect.bottom)
	{
		int b = rect.bottom;
		rect.bottom = rect.top;
		rect.top = b;
	}
}

/*
	DrawBox()

	Draw a box - XOR to erase.
*/
void CZoomView::DrawBox(CDC &dc,CRect &rect,BOOL xor)
{
	CPen pen;

	// save the device context
	dc.SaveDC();
	if(xor)
	{
		dc.SetROP2(R2_NOTXORPEN);
		pen.CreatePen(PS_DASH,0,RGB(0,0,0)); // 0 width = 1 device unit
	}
	else
	{
		pen.CreatePen(PS_SOLID,0,RGB(0,0,0)); // 0 width = 1 device unit
	}

	dc.SelectObject(&pen);
	
	// draw the rect with lines (eliminate rect middle fill)
	dc.MoveTo(rect.left, rect.top);
	dc.LineTo(rect.right,rect.top);
	dc.LineTo(rect.right,rect.bottom);
	dc.LineTo(rect.left, rect.bottom);
	dc.LineTo(rect.left, rect.top);

	// clean up
	dc.RestoreDC(-1);
}

/*
	DrawLine()

	Draw a line - XOR to erase (logical units).
*/
void CZoomView::DrawLine(CDC &dc,const int &x1,const int &y1,const int &x2,const int &y2,BOOL xor)
{
	CPen pen;
	
	// save the device context
	dc.SaveDC();
	if(xor)
	{
		dc.SetROP2(R2_NOTXORPEN);
		pen.CreatePen(PS_DASH,0,RGB(0,0,0)); // 0 width = 1 device unit
	}
	else
	{
		pen.CreatePen(PS_SOLID,0,RGB(0,0,0)); // 0 width = 1 device unit
	}

	dc.SelectObject(&pen);

	// draw the line
	dc.MoveTo(x1,y1);
	dc.LineTo(x2,y2);
	
	// clean up
	dc.RestoreDC(-1);
}

/*
	OnLButtonDown()

	Handle the left mouse click.
*/
void CZoomView::OnLButtonDown(UINT nFlags,CPoint point)
{
	// pass the message along
	CScrollView::OnLButtonDown(nFlags, point);

	if(m_cursorZoomMode==CURSOR_MODE_BYCLICK)
	{
		// capture the mouse for zooming in
		m_bCaptured = TRUE;
		SetCapture();

		// save the mouse down point for XOR rect
		ViewDPtoLP(&point);
		m_ptDragRect.SetRect(point.x,point.y,point.x,point.y);

		// set the cursor to the cross hair
		::SetCursor(::LoadCursor(NULL,MAKEINTRESOURCE(IDC_CROSS)));
	}
	else if(m_cursorZoomMode==CURSOR_MODE_BYMODE)
	{
		switch(m_zoomMode)
		{
			case MODE_ZOOMIN:
				// capture the mouse for zooming in
				m_bCaptured = TRUE;
				SetCapture();

				// save the mouse down point for XOR rect
				ViewDPtoLP(&point);
				m_ptDragRect.SetRect(point.x,point.y,point.x,point.y);

				// set the cursor to the cross hair
				::SetCursor(::LoadCursor(NULL,MAKEINTRESOURCE(IDC_CROSS)));
				break;

			default:
				break;
		}
	}
}

/*
	OnLButtonUp()

	Handle the left mouse release.
*/
void CZoomView::OnLButtonUp(UINT nFlags,CPoint point)
{
    // Pass the message along
    CScrollView::OnLButtonUp(nFlags,point);

	if(m_cursorZoomMode==CURSOR_MODE_BYCLICK)
	{
		// uncapture the mouse ?
		if(m_bCaptured)
		{
			m_bCaptured = FALSE;
			ReleaseCapture();
			
			// set back the cross cursor to the Z
			if(!m_hZoomCursor)
				m_hZoomCursor = ::LoadCursor(NULL,IDC_ARROW);
			::SetCursor(m_hZoomCursor);
			
			// get the Device Context
			CClientDC dc(this);
			OnPrepareDC(&dc);
			
			// erase the bounding box
			DrawBox(dc,m_ptDragRect);
			
			// now zoom in on logical rectangle
			DoZoomIn(m_ptDragRect);
		}
	}
	else if(m_cursorZoomMode==CURSOR_MODE_BYMODE)
	{
		switch(m_zoomMode)
		{
			case MODE_ZOOMIN:
				// uncapture the mouse ?
				if(m_bCaptured)
				{
					m_bCaptured = FALSE;
					ReleaseCapture();

					// set back the cross cursor to the Z
					if(!m_hZoomCursor)
						m_hZoomCursor = ::LoadCursor(NULL,IDC_ARROW);
					::SetCursor(m_hZoomCursor);
					
					// get the device context
					CClientDC dc(this);
					OnPrepareDC(&dc);

					// erase the bounding box
					DrawBox(dc,m_ptDragRect);
					
					// now zoom in on logical rectangle
					DoZoomIn(m_ptDragRect);
				}
				break;

			case MODE_ZOOMOUT:
				ViewDPtoLP(&point);
				DoZoomOut(&point);
				break;

			default:
				break;
		}
	}
}

/*
	OnRButtonDown()

	Handle the right mouse click - CANCELS CURRENT ZOOM MODE OR DRAG.
*/
void CZoomView::OnRButtonDown(UINT nFlags,CPoint point)
{
	CScrollView::OnRButtonDown(nFlags,point);

	// see if currently captured
	if(m_bCaptured)
	{
		// naintain current mode, just stop current drag
		m_bCaptured = FALSE;
		ReleaseCapture();

		// get the device context
		CClientDC dc(this);
		OnPrepareDC(&dc);

		if(m_cursorZoomMode!=CURSOR_MODE_BYCLICK)
		{
			switch(m_zoomMode)
			{
				case MODE_ZOOMIN:
					// erase last rect
					DrawBox(dc,m_ptDragRect);
					break;

				default:
					break;
			}
		}
	}
	else
	{
		// cancel current mode
		if(m_cursorZoomMode==CURSOR_MODE_BYCLICK)
		{
			ViewDPtoLP(&point);
			DoZoomOut(&point);
		}
		else if(m_cursorZoomMode==CURSOR_MODE_BYMODE)
			m_zoomMode = MODE_ZOOMOFF;
	}
}

/*
	OnMouseMove()

	Handle the mouse movement.
*/
void CZoomView::OnMouseMove(UINT nFlags,CPoint point)
{
    // pass the message along
    CScrollView::OnMouseMove(nFlags,point);

	if(m_bCaptured)
	{
		// get the Device Context
		CClientDC dc(this);
		OnPrepareDC(&dc);

		if(m_cursorZoomMode==CURSOR_MODE_BYCLICK)
		{
			// draw the drag-rect/erase last rect
			DrawBox(dc,m_ptDragRect);
			
			// draw new rect
			dc.DPtoLP(&point);
			m_ptDragRect.BottomRight() = point;
			DrawBox(dc,m_ptDragRect);
		}
		else if(m_cursorZoomMode==CURSOR_MODE_BYMODE)
		{
			switch(m_zoomMode)
			{
				case MODE_ZOOMIN:
					// draw the drag-rect/erase last rect
					DrawBox(dc,m_ptDragRect);
					
					// draw new rect
					dc.DPtoLP(&point);
					m_ptDragRect.BottomRight() = point;
					DrawBox(dc,m_ptDragRect);
					break;

				default:
					break;
			}
		}
	}
}

/*
	OnSetCursor()

	Set the cursor depending on the zoom mode.
*/
BOOL CZoomView::OnSetCursor(CWnd* pWnd,UINT nHitTest,UINT message)
{
	if(nHitTest!=HTCLIENT)
		return(CScrollView::OnSetCursor(pWnd,nHitTest,message));

	switch(m_zoomMode)
	{
		case MODE_ZOOMOFF:
			::SetCursor(::LoadCursor(NULL, IDC_ARROW));
			break;
		default:
			if(!m_hZoomCursor)
				m_hZoomCursor = ::LoadCursor(NULL,IDC_ARROW);
			::SetCursor(m_hZoomCursor);
			break;
	}

	return(TRUE);
}

/*
	OnPrepareDC()

	Override of CScrollView for MAPMODE zoom mode.
*/
VIRTUAL void CZoomView::OnPrepareDC(CDC* pDC,CPrintInfo* pInfo)
{
//	ASSERT_VALID(pDC);
//	ASSERT(m_totalLog.cx >= 0 && m_totalLog.cy >= 0);
//	ASSERT(m_totalDev.cx >= 0 && m_totalDev.cy >= 0);

	if(m_totalLog.cx > 0 && m_totalLog.cy > 0 && m_totalDev.cx > 0 && m_totalDev.cy > 0)
	{
		// set the Mapping mode, and the window and viewport extents
		// valido solo se MM_ANISOTROPIC o MM_ISOTROPIC
		pDC->SetMapMode(m_nMapMode);

		// in logical coordinates
		pDC->SetWindowExt(m_totalLog);
		CPoint ptVpOrg;

		if(!pDC->IsPrinting())
		{
			// valido solo se MM_ANISOTROPIC o MM_ISOTROPIC
			pDC->SetViewportExt(m_totalDev); // in device coordinates

			// by default shift viewport origin in negative direction of scroll
			ASSERT(pDC->GetWindowOrg()==CPoint(0,0));
			ptVpOrg = -CScrollView::GetDeviceScrollPosition();

			// center full fit
			if(m_bCenter)
			{
				CRect rect;
				GetClientRect(&rect);

				// if client area is larger than total device size,
				// override scroll positions to place origin such that
				// output is centered in the window
				if(m_totalDev.cx < rect.Width())
					ptVpOrg.x = (rect.Width() - m_totalDev.cx) / 2;
				if(m_totalDev.cy < rect.Height())
					ptVpOrg.y = (rect.Height() - m_totalDev.cy) / 2;
			}
		}
		else
		{
			// special case for printing
			CSize printSize;
			printSize.cx = pDC->GetDeviceCaps(HORZRES);
			printSize.cy = pDC->GetDeviceCaps(VERTRES);
			
			// maintain the original ratio, setup origin shift
			PersistRatio(m_totalLog,printSize,ptVpOrg);
			
			// zoom completely out
			pDC->SetViewportExt(printSize);
	    }

		// set the new origin
		pDC->SetViewportOrg(ptVpOrg);
	}

	// for default printing behavior
	CScrollView::OnPrepareDC(pDC,pInfo);
}

/*
	PersistRatio()

	Make a CSize maintain the given ratio (by shrinking if nescessary).
*/
void CZoomView::PersistRatio(const CSize &orig,CSize &dest,CPoint &remainder)
{
	int newSize;
	double ratio1 = (double) orig.cx / orig.cy;
	double ratio2 = (double) dest.cx / dest.cy;

	// do nothing if they are the same
	if(ratio1 > ratio2)
	{
		// shrink hieght
		newSize = (int)(dest.cx / ratio1);
		remainder.x = 0;
		remainder.y = dest.cy - newSize;
		dest.cy = newSize;
	}
	else if(ratio2 > ratio1)
	{
		// shrink width
		newSize = (int)(dest.cy * ratio1);
		remainder.x = dest.cx - newSize;
		remainder.y = 0;
		dest.cx = newSize;
	}
}

/*
	CalcBars()

	Update the scrollbars - uses logical units.
	Call when the Viewport changes size.
*/
void CZoomView::CalcBars(BOOL bInvalidate/*=TRUE*/)
{
	// block for DC
	{
		CWindowDC dc(NULL);
		dc.SetMapMode(m_nMapMode);

		switch(m_nMapMode)
		{
			case MM_TEXT:
				m_totalLog = m_totalDev;
				break;
			default:
				break;
		}

		// calculate new device units for scrollbar
		// start with original logical units from SetScrollPos
		m_pageDev = m_origPageDev;
		dc.LPtoDP((LPPOINT)&m_pageDev);
		m_lineDev = m_origLineDev;
		dc.LPtoDP((LPPOINT)&m_lineDev);
	}

	// make sure of the range
	if(m_totalDev.cy < 0) m_totalDev.cy = -m_totalDev.cy;
	if(m_pageDev.cy  < 0) m_pageDev.cy  = -m_pageDev.cy;
	if(m_lineDev.cy  < 0) m_lineDev.cy  = -m_lineDev.cy;

	// if none specified - use one tenth
	ASSERT(m_totalDev.cx >= 0 && m_totalDev.cy >= 0);
	if(m_pageDev.cx==0) m_pageDev.cx = m_totalDev.cx / 10;
	if(m_pageDev.cy==0) m_pageDev.cy = m_totalDev.cy / 10;
	if(m_lineDev.cx==0) m_lineDev.cx = m_pageDev.cx  / 10;
	if(m_lineDev.cy==0) m_lineDev.cy = m_pageDev.cy  / 10;

	// now update the scrollbars
	if(m_hWnd)
	{
		UpdateBars();
		if(bInvalidate)
			Invalidate(TRUE); // zoom scale changed, redraw all
	}
}
