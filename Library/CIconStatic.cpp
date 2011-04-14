//this file is part of eMule
//Copyright (C)2002 Merkur ( merkur-@users.sourceforge.net / http://www.emule-project.net )
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

// Modificato il codice originale (vedi sopra).
// Luca Piergentili, 02/05/04

#include "env.h"
#include "pragma.h"
#include "macro.h"
#include "window.h"
#include "win32api.h"
#include <atlbase.h> // USES_CONVERSION
//#include "CWindowsXPTheme.h"
#include "CIconStatic.h"

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
	CTempIconLoader()
*/
CTempIconLoader::CTempIconLoader(UINT nIconID,int cx/* = 16|32 default=16*/,int cy/* = 16|32 default=16*/,UINT uFlags/* = LR_DEFAULTCOLOR*/)
{
	m_hIcon = (HICON)::LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(nIconID),IMAGE_ICON,cx,cy,uFlags);
}

/*
	~CTempIconLoader()
*/
CTempIconLoader::~CTempIconLoader()
{
	if(m_hIcon)
		::DestroyIcon(m_hIcon);
}

BEGIN_MESSAGE_MAP(CIconStatic,CStatic)
	ON_WM_SYSCOLORCHANGE()
END_MESSAGE_MAP()

/*
	CIconStatic()
*/
CIconStatic::CIconStatic()
{
	m_nIconSize = 16;
	m_nIconID = (UINT)-1L;
	m_strText = "";
}

/*
	~CIconStatic()
*/
CIconStatic::~CIconStatic()
{
	if(m_bmpIcon.m_hObject)
		m_bmpIcon.DeleteObject();
}

/*
	Init()
*/
BOOL CIconStatic::Init(UINT nIconID)
{
	if(m_nIconSize!=16 && m_nIconSize!=32)
		m_nIconSize = 16;
	
	m_nIconID = nIconID;

	CString strText;
	GetWindowText(strText);
	SetWindowText("");
	if(strText!="")
		m_strText = strText;

	CRect rcClient;
	GetClientRect(rcClient);

	CDC* pDC = GetDC();
	CDC memDC;
	CBitmap* pOldBitmap;
	
	if(!memDC.CreateCompatibleDC(pDC))
		return(FALSE);

	CFont *pOldFont = memDC.SelectObject(GetFont());

	CRect rcCaption(0,0,0,0);
	memDC.DrawText(m_strText,rcCaption,DT_CALCRECT);
	if(rcCaption.Height() < (int)m_nIconSize)
		rcCaption.bottom = rcCaption.top + m_nIconSize;
	rcCaption.right += m_nIconSize+3;
	if(rcCaption.Width() > rcClient.Width() - (int)m_nIconSize)
		rcCaption.right = rcCaption.left + rcClient.Width() - m_nIconSize;

	if(m_bmpIcon.m_hObject)
		m_bmpIcon.DeleteObject();
	if(!m_bmpIcon.CreateCompatibleBitmap(pDC,rcCaption.Width(),rcCaption.Height()))
		return(FALSE);
	pOldBitmap = memDC.SelectObject(&m_bmpIcon);
	memDC.FillSolidRect(rcCaption,::GetSysColor(COLOR_BTNFACE));
	::DrawState(memDC.m_hDC,NULL,NULL,(LPARAM)(HICON)CTempIconLoader(m_nIconID,m_nIconSize,m_nIconSize),NULL,3,0,m_nIconSize,m_nIconSize,DST_ICON|DSS_NORMAL);

	// clear all alpha channel data
	BITMAP bmMem;
	if(m_bmpIcon.GetObject(sizeof(bmMem),&bmMem) >= sizeof(bmMem) && bmMem.bmBitsPixel==32)
	{
		DWORD dwSize = m_bmpIcon.GetBitmapBits(0,NULL);
		if(dwSize)
		{
			LPBYTE pPixels = (LPBYTE)malloc(dwSize);
			if(pPixels)
			{
				if(m_bmpIcon.GetBitmapBits(dwSize,pPixels)==dwSize)
				{
					LPBYTE pLine = pPixels;
					int iLines = bmMem.bmHeight;
					while(iLines-- > 0)
					{
						LPDWORD pdwPixel = (LPDWORD)pLine;
						for(int x = 0; x < bmMem.bmWidth; x++)
							*pdwPixel++ &= 0x00FFFFFF;
						pLine += bmMem.bmWidthBytes;
					}
					m_bmpIcon.SetBitmapBits(dwSize,pPixels);
				}

				free(pPixels);
			}
		}
	}

	rcCaption.left += m_nIconSize+3;

#if defined(_HAVE_XP_THEME_TYPES_FROM_MINGW) || defined(_HAVE_XP_THEME_TYPES_FROM_MS)
	CWindowsXPTheme winTheme;
	if(winTheme.IsThemingSupported() && winTheme.IsAppThemed())
	{
		HTHEME hTheme = winTheme.OpenThemeData(NULL,L"BUTTON"); 
		if(hTheme)
		{
			USES_CONVERSION;
			LPOLESTR oleText = T2OLE(m_strText);
			winTheme.DrawThemeText(hTheme,memDC.m_hDC,BP_GROUPBOX,GBS_NORMAL,oleText,ocslen(oleText),DT_WORDBREAK|DT_CENTER|DT_WORD_ELLIPSIS,NULL,&rcCaption);
			winTheme.CloseThemeData(hTheme);
		}
	}
	else
#endif
	{
		memDC.SetTextColor(pDC->GetTextColor());
		memDC.DrawText(m_strText,rcCaption,DT_SINGLELINE|DT_CENTER|DT_WORD_ELLIPSIS);
	}

	ReleaseDC(pDC);

	memDC.SelectObject(pOldBitmap);
	memDC.SelectObject(pOldFont);
	
	if(!m_wndPicture.m_hWnd)
		m_wndPicture.Create(NULL,WS_CHILD|WS_VISIBLE|SS_BITMAP,CRect(0,0,0,0),this);
	m_wndPicture.SetWindowPos(NULL,rcClient.left+8,rcClient.top,rcCaption.Width(),rcCaption.Height(),SWP_SHOWWINDOW);
	m_wndPicture.SetBitmap(m_bmpIcon);

	CWnd* pParent = GetParent();
	if(!pParent)
		pParent = GetDesktopWindow();
	
	CRect r;
	GetWindowRect(r);
	r.bottom = r.top + 20;
	GetParent()->ScreenToClient(&r);
	GetParent()->RedrawWindow(r);

	return(TRUE);
}

/*
	SetText()
*/
BOOL CIconStatic::SetText(LPCSTR lpcszText)
{
	m_strText.Format("%s",lpcszText);
	return(Init(m_nIconID));
}

/*
	SetText()
*/
BOOL CIconStatic::SetText(CString& strText)
{
	m_strText.Format("%s",strText);
	return(Init(m_nIconID));
}

/*
	SetIcon()
*/
BOOL CIconStatic::SetIcon(UINT nIconID)
{
	return(Init(nIconID));
}

/*
	OnSysColorChange()
*/
void CIconStatic::OnSysColorChange(void)
{
	CStatic::OnSysColorChange();
	if(m_nIconID!=(UINT)-1L)
		Init(m_nIconID);
}
