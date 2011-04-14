/*
	WallPaperSplashScreenDlg.cpp
	Dialogo per la schermata iniziale.
	Luca Piergentili, 09/06/03
	lpiergentili@yahoo.com

	WallPaper (alias crawlpaper) - the hardcore of Windows desktop
	http://www.crawlpaper.com/
	copyright © 1998-2004 Luca Piergentili, all rights reserved
	crawlpaper is a registered name, all rights reserved

	This is a free software, released under the terms of the BSD license. Do not
	attempt to use it in any form which violates the license or you will be persecuted
	and charged for this.

	Redistribution and use in source and binary forms, with or without modification,
	are permitted provided that the following conditions are met:

	- Redistributions of source code must retain the above copyright notice, this
	list of conditions and the following disclaimer. 

	- Redistributions in binary form must reproduce the above copyright notice, this
	list of conditions and the following disclaimer in the documentation and/or other
	materials provided with the distribution. 

	- Neither the name of "crawlpaper" nor the names of its contributors may be used
	to endorse or promote products derived from this software without specific prior
	written permission. 

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
	ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
	IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
	INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
	BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
	LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
	OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
	OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include "window.h"
#include "CWndLayered.h"
#include "CHyperLink.h"
#include "CColorStatic.h"
#include "WallPaperVersion.h"
#include "WallPaperMessages.h"
#include "WallPaperSplashScreenDlg.h"
#include "resource.h"

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

BEGIN_MESSAGE_MAP(CWallPaperSplashScreenDlg,CDialog)
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_TIMER()
END_MESSAGE_MAP()

/*
	CWallPaperSplashScreenDlg()
*/
CWallPaperSplashScreenDlg::CWallPaperSplashScreenDlg(CWnd* /*pParent*/) : CDialog(/*nTemplateID*/IDD_DIALOG_SPLASHSCREEN,/*pWndParent*/NULL)
{
	m_bVisible = FALSE;
	m_hFont = NULL;
	m_nTimerId = 0L;
	m_nLayerValue = LWA_ALPHA_OPAQUE-1;
}

/*
	~CWallPaperSplashScreenDlg()
*/
CWallPaperSplashScreenDlg::~CWallPaperSplashScreenDlg()
{
	if(m_hFont)
		::DeleteObject(m_hFont);
}

/*
	OnWindowPosChanging()
*/
void CWallPaperSplashScreenDlg::OnWindowPosChanging(WINDOWPOS FAR* lpWndPos) 
{
    if(!m_bVisible)
        lpWndPos->flags &= ~SWP_SHOWWINDOW;

    CDialog::OnWindowPosChanging(lpWndPos);
}

/*
	OnInitDialog()
*/
BOOL CWallPaperSplashScreenDlg::OnInitDialog(void)
{
	char szTooltip[256];

	// classe base
	CDialog::OnInitDialog();

	// tooltip per l'hyperlink
	_snprintf(szTooltip,
			sizeof(szTooltip)-1,
			"Click here to open your browser on the %s website (%s).",
			WALLPAPER_PROGRAM_NAME,
			WALLPAPER_WEB_SITE
			);

	// link al sito web
	m_wndWebSite.SetAutoSize(FALSE);
	m_wndWebSite.SetAutoText(FALSE);
	m_wndWebSite.SetUrl(WALLPAPER_WEB_SITE);
//	m_wndWebSite.SetText(WALLPAPER_WEB_SITE);
	m_wndWebSite.SetToolTip(szTooltip);
	m_wndWebSite.SubclassDlgItem(IDC_STATIC_WEBSITE,this);

	// box per il copyright
	m_wndCopyright.SubclassDlgItem(IDC_STATIC_ABOUT,this);
	
	LOGFONT lf = {0};
	lf.lfHeight = -11;
	lf.lfWidth = 0;
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	lf.lfWeight = 400;
	lf.lfItalic = 0;
	lf.lfUnderline = 0;
	lf.lfStrikeOut = 0;
	lf.lfCharSet = 0;
	lf.lfOutPrecision = 3;
	lf.lfClipPrecision = 2;
	lf.lfQuality = 1;
	lf.lfPitchAndFamily = 34;
	lstrcpy(lf.lfFaceName,"Tahoma");
	m_hFont = ::CreateFontIndirect(&lf);  
	CFont* pFont = CFont::FromHandle(m_hFont);
	m_wndCopyright.SetFont(pFont,FALSE);
	
	m_wndCopyright.SetTextColor(RGB(0,0,0));
	m_wndCopyright.SetBkColor(RGB(255,255,255));

	CString strCopyright;
	strCopyright.Format("   %s v.%s\n   %s\n   Copyright © 1998-2004 by Luca Piergentili\n\n   %s",
					WALLPAPER_PROGRAM_NAME_ALIAS,
					WALLPAPER_VERSION,
					WALLPAPER_PROGRAM_DESCRIPTION,
					WALLPAPER_WEB_SITE
					);
	m_wndCopyright.SetWindowText(strCopyright);

	// imposta l'effetto a dissolvenza, se non riesce chiude il dialogo
	m_nTimerId = SetTimer(ID_TIMER_SPLASH_SCREEN,85L,NULL);
	if(m_nTimerId > 0L)
	{
		ShowWindow(SW_HIDE);
		m_wndLayered.SetLayer(this->m_hWnd,(BYTE)m_nLayerValue);
	}
	else
		::PostMessage(this->m_hWnd,WM_COMMAND,MAKELONG(IDCANCEL,0),0L);
	
	::PostMessage(this->m_hWnd,WM_TIMER,(WPARAM)m_nTimerId,(LPARAM)NULL);

	return(TRUE);
}

/*
	OnTimer()
*/
void CWallPaperSplashScreenDlg::OnTimer(UINT nID)
{
	if(nID==ID_TIMER_SPLASH_SCREEN)
	{
		// ancora non invisibile
		if(m_nLayerValue >= LWA_ALPHA_INVISIBLE)
		{
			// usa il flag per evitare lo sfarfallio/fondo nero iniziali
			if(!m_bVisible)
			{
				m_bVisible = TRUE;
				ShowWindow(SW_SHOW);
				SetForegroundWindow();
				CenterWindow();
				this->SetWindowPos(&CWnd::wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
			}
			
			// imposta la trasparenza
			m_wndLayered.SetLayer(this->m_hWnd,(BYTE)m_nLayerValue);
			m_nLayerValue -= 5;
		}
		else
		{
			// dialogo invisibile, termina
			KillTimer(m_nTimerId);
			::PostMessage(this->m_hWnd,WM_COMMAND,MAKELONG(IDCANCEL,0),0L);
		}
	}
	
	CDialog::OnTimer(nID);
}

/*
	OnOK()
*/
void CWallPaperSplashScreenDlg::OnOK(void)
{
	if(m_nTimerId > 0L)
		KillTimer(m_nTimerId),m_nTimerId = 0L;
	CDialog::OnOK();
}

/*
	OnCancel()
*/
void CWallPaperSplashScreenDlg::OnCancel(void)
{
	if(m_nTimerId > 0L)
		KillTimer(m_nTimerId),m_nTimerId = 0L;
	CDialog::OnCancel();
}
