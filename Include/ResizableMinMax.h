/////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2000-2002 by Paolo Messina
// (http://www.geocities.com/ppescher - ppescher@yahoo.com)
//
// The contents of this file are subject to the Artistic License (the "License").
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.opensource.org/licenses/artistic-license.html
//
// If you find this code useful, credits would be nice!
//
/////////////////////////////////////////////////////////////////////////////

// LPI 11/09/03: modificato il codice originale.

#ifndef _RESIZABLEMINMAX_H
#define _RESIZABLEMINMAX_H 1

/*
	CResizableMinMax()
*/
class CResizableMinMax
{
public:
	CResizableMinMax();
	virtual ~CResizableMinMax() {}

	inline BOOL IsMinTrackSizeEnabled(void) const {return(m_bUseMinTrack);}
	inline BOOL IsMaxTrackSizeEnabled(void) const {return(m_bUseMaxTrack);}

protected:
	void		MinMaxInfo		(LPMINMAXINFO lpMMI);
	void		SetMaximizedRect	(const CRect& rc);		// set window rect when maximized
	void		ResetMaximizedRect	(void);				// reset to default maximized rect
	void		SetMinTrackSize	(const CSize& size);	// set minimum tracking size
	void		ResetMinTrackSize	(void);				// reset to default minimum tracking size
	void		SetMaxTrackSize	(const CSize& size);	// set maximum tracking size
	void		ResetMaxTrackSize	(void);				// reset to default maximum tracking size

private:
	BOOL		m_bUseMaxTrack;
	BOOL		m_bUseMinTrack;
	BOOL		m_bUseMaxRect;

	POINT	m_ptMinTrackSize;		// min tracking size
	POINT	m_ptMaxTrackSize;		// max tracking size
	POINT	m_ptMaxPos;			// maximized position
	POINT	m_ptMaxSize;			// maximized size
};

#endif // _RESIZABLEMINMAX_H
