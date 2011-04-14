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

#include "env.h"
#include "pragma.h"
#include "macro.h"
#include "window.h"
#include "Resizable.h"
#include "ResizableMinMax.h"

#include "traceexpr.h"
//#define _TRACE_FLAG	_TRFLAG_TRACEOUTPUT
//#define _TRACE_FLAG	_TRFLAG_NOTRACE
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

/*
	CResizableMinMax()
*/
CResizableMinMax::CResizableMinMax()
{
	m_bUseMinTrack = FALSE;
	m_bUseMaxTrack = FALSE;
	m_bUseMaxRect = FALSE;
}

/*
	MinMaxInfo()
*/
void CResizableMinMax::MinMaxInfo(LPMINMAXINFO lpMMI)
{
	if(m_bUseMinTrack)
		lpMMI->ptMinTrackSize = m_ptMinTrackSize;

	if(m_bUseMaxTrack)
		lpMMI->ptMaxTrackSize = m_ptMaxTrackSize;

	if(m_bUseMaxRect)
	{
		lpMMI->ptMaxPosition = m_ptMaxPos;
		lpMMI->ptMaxSize = m_ptMaxSize;
	}
}

/*
	SetMaximizedRect()
*/
void CResizableMinMax::SetMaximizedRect(const CRect& rc)
{
	m_bUseMaxRect = TRUE;

	m_ptMaxPos = rc.TopLeft();
	m_ptMaxSize.x = rc.Width();
	m_ptMaxSize.y = rc.Height();
}

/*
	ResetMaximizedRect()
*/
void CResizableMinMax::ResetMaximizedRect(void)
{
	m_bUseMaxRect = FALSE;
}

/*
	SetMinTrackSize()
*/
void CResizableMinMax::SetMinTrackSize(const CSize& size)
{
	m_bUseMinTrack = TRUE;

	m_ptMinTrackSize.x = size.cx;
	m_ptMinTrackSize.y = size.cy;
}

/*
	ResetMinTrackSize()
*/
void CResizableMinMax::ResetMinTrackSize()
{
	m_bUseMinTrack = FALSE;
}

/*
	SetMaxTrackSize()
*/
void CResizableMinMax::SetMaxTrackSize(const CSize& size)
{
	m_bUseMaxTrack = TRUE;

	m_ptMaxTrackSize.x = size.cx;
	m_ptMaxTrackSize.y = size.cy;
}

/*
	ResetMaxTrackSize()
*/
void CResizableMinMax::ResetMaxTrackSize()
{
	m_bUseMaxTrack = FALSE;
}
