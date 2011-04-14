/*
	WallPaperAboutDlg.cpp
	Dialogo per la finestra dei crediti.
	Luca Piergentili, 06/08/98
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
#include <stdio.h>
#include <string.h>
#include "window.h"
#include "win32api.h"
#include "CDialogEx.h"
#include "CWindowsVersion.h"
#include "CWndLayered.h"
#include "CHyperLink.h"
#include "CColorStatic.h"
#include "CAudioWav.h"
#include "WallPaperConfig.h"
#include "WallPaperVersion.h"
#include "WallPaperMessages.h"
#include "WallPaperAboutDlg.h"
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

BEGIN_MESSAGE_MAP(CWallPaperAboutDlg,CDialogEx)
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_TIMER()
	ON_MESSAGE(WM_PLAY_WAVE,OnPlayWave)
END_MESSAGE_MAP()

IMPLEMENT_DYNCREATE(CWallPaperAboutDlg,CDialogEx)

/*
	CWallPaperAboutDlg()
*/
CWallPaperAboutDlg::CWallPaperAboutDlg(HWND hWndParent)
: CDialogEx(	/*nTemplateID*/		IDD_DIALOG_ABOUT,
			/*nToolbarID*/			0L,
			/*nStatusbarID*/		0L,
			/*nIconID*/			IDI_ICON_WALLPAPER,
			/*nMenuID*/			0L,
#ifdef _RESIZABLE_DIALOG_STYLE
			/*bAllowResize*/		FALSE,
#endif
			/*bAllowSnap*/			FALSE,
			/*bAllowDragAndDrop*/	FALSE,
			/*lpcszUniqueName*/		IDS_DIALOG_ABOUT_TITLE,
			/*bAllowMultipleInstances*/FALSE,
			/*hWndParent*/			NULL)
{
	// per evitare che la zoccola di MFC visualizzi il dialogo quando decide lei
	SetVisible(FALSE);

	m_hWndParent = hWndParent;
	m_hFont = NULL;
	m_strAbout = "";
	m_nTimerId = 0L;
	m_nLayerValue = LWA_ALPHA_OPAQUE-1;
	m_nScrolled = 0;
	m_nTimerCalls = -4;
	m_nScrollPos = 0;
	m_pConfig = NULL;
}

/*
	Clean()
*/
void CWallPaperAboutDlg::Clean(void)
{
	//PlaySound(NULL,NULL,SND_PURGE);
	PlaySound(NULL,0,0);

	if(m_nTimerId > 0L)
		KillTimer(m_nTimerId),m_nTimerId = 0L;

	if(m_hFont)
		::DeleteObject(m_hFont);
}

/*
	OnInitDialog()
*/
BOOL CWallPaperAboutDlg::OnInitDialog(void)
{
	char szBuffer[2048] = {0};
	char szTooltip[256] = {0};
	CWindowsVersion winVer;

	// classe base
	if(!CDialogEx::OnInitDialog())
	{
		EndDialog(IDCANCEL);
		return(FALSE);
	}

	// ricava i puntatori dall'applicazione principale
	m_pConfig = NULL;
	if(m_hWndParent)
		m_pConfig = (CWallPaperConfig*)::SendMessage(m_hWndParent,WM_GETCONFIGURATION,0L,0L);
	if(!m_pConfig)
	{
		::MessageBoxResource(NULL,MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_RETRIEVE_CONFIGURATION);
		EndDialog(IDCANCEL);
		return(FALSE);
	}
	
	// tooltip
	_snprintf(szTooltip,
			sizeof(szTooltip)-1,
			"Click here to open your browser on the %s website (%s) or press any key to close this dialog.",
			WALLPAPER_PROGRAM_NAME,
			WALLPAPER_WEB_SITE
			);

	// link al sito web
	m_wndWebSite.SetAutoSize(FALSE);
	m_wndWebSite.SetAutoText(FALSE);
	m_wndWebSite.SetUrl(WALLPAPER_WEB_SITE);
	m_wndWebSite.SetToolTip(szTooltip);
	m_wndWebSite.SubclassDlgItem(IDC_STATIC_WEBSITE,this);

	// info sul sistema operativo
	winVer.GetPlatformInfo(szBuffer,sizeof(szBuffer));

	// about
	m_strAbout.Format(	"%s"
					"crashing on:\r\n\r\n%s",
					g_lpcszCredits,
					szBuffer
					);

	// subclassa il controllo per le info sull'immagine (imposta font e colore del testo)
	m_wndAbout.SubclassDlgItem(IDC_EDIT_ABOUT,this);
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
	m_wndAbout.SetFont(pFont,FALSE);
	m_wndAbout.SetTextColor(RGB(0,0,0));
	m_wndAbout.SetBkColor(RGB(255,255,255));

	GetDlgItem(IDC_EDIT_ABOUT)->SetWindowText(m_strAbout);

	// inizializzazione estesa
	OnInitDialogEx();

	// imposta l'effetto a dissolvenza, se non riesce chiude il dialogo
	m_nTimerId = SetTimer(ID_TIMER_SPLASH_SCREEN,300L,NULL);
	if(m_nTimerId > 0L)
	{
		ShowWindow(SW_HIDE);
		m_wndLayered.SetLayer(this->m_hWnd,(BYTE)m_nLayerValue);
	}
	else
		::PostMessage(this->m_hWnd,WM_COMMAND,MAKELONG(IDCANCEL,0),0L);
	
	::PostMessage(this->m_hWnd,WM_TIMER,(WPARAM)m_nTimerId,(LPARAM)NULL);

	::PostMessage(this->m_hWnd,WM_PLAY_WAVE,(WPARAM)0,(LPARAM)NULL);

	return(TRUE);
}

/*
	OnTimer()

	Chiamata per ogni tick del timer.
*/
void CWallPaperAboutDlg::OnTimer(UINT nID)
{
	if(nID==ID_TIMER_SPLASH_SCREEN)
	{
		// ancora non invisibile
		if(m_nLayerValue >= LWA_ALPHA_INVISIBLE_THRESHOLD/*LWA_ALPHA_INVISIBLE*/)
		{
			// usa il flag per evitare lo sfarfallio/fondo nero iniziali
			if(!IsVisible())
			{
				CenterWindow();
				::SetForegroundWindowEx(this->m_hWnd);
				SetVisible(TRUE);
				this->SetWindowPos(&CWnd::wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
				GetDlgItem(IDC_STATIC_WEBSITE)->SetFocus();
			}
			else
			{
				if(++m_nTimerCalls > 0)
				{
					if(!m_nScrolled)
					{
						CWnd* pWnd = GetDlgItem(IDC_EDIT_ABOUT);
						if(pWnd)
						{
							SCROLLINFO si = {0};
							si.cbSize = sizeof(si);
							si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE | SIF_TRACKPOS;
							pWnd->GetScrollInfo(SB_VERT,&si,SIF_ALL);
							
							if(si.nPos > m_nScrollPos || (si.nPos==m_nScrollPos && m_nScrollPos==0))
							{
								m_nScrollPos = si.nPos;
								pWnd->SendMessage(WM_VSCROLL,SB_LINEDOWN);
								m_nScrolled = 1;
							}
						}
					}
					else
						m_nScrolled = 0;
				}

				// imposta la trasparenza
				m_wndLayered.SetLayer(this->m_hWnd,(BYTE)m_nLayerValue);
				int nFactor = 1;
				if(m_nLayerValue >= LWA_ALPHA_OPAQUE/2)
					nFactor = 1;
				else if(m_nLayerValue >= LWA_ALPHA_OPAQUE/3)
					nFactor = 2;
				else if(m_nLayerValue >= LWA_ALPHA_OPAQUE/4)
					nFactor = 4;//3;
				else
					nFactor = 7;//5;
				m_nLayerValue -= nFactor;
			}
		}
		else
		{
			// dialogo invisibile, termina
			KillTimer(m_nTimerId),m_nTimerId = 0L;
			::PostMessage(this->m_hWnd,WM_COMMAND,MAKELONG(IDCANCEL,0),0L);
		}
	}

	CDialogEx::OnTimer(nID);
}

/*
	OnPlayWave()
*/
LRESULT CWallPaperAboutDlg::OnPlayWave(WPARAM /*wParam*/,LPARAM /*lParam*/)
{
	// .wav
	char szWave[_MAX_FILEPATH+1] = {0};
	_snprintf(szWave,sizeof(szWave)-1,"%sabout.wav",m_pConfig->GetString(WALLPAPER_INSTALL_KEY,WALLPAPER_DIR_KEY));
	if(!FileExist(szWave))
		::ExtractResource(IDR_WAVE_ABOUT,"WAVE",szWave);

	//PlaySound(MAKEINTRESOURCE(IDR_WAVE_ABOUT),::GetModuleHandle(NULL),SND_RESOURCE|SND_ASYNC);
	PlaySound(szWave,NULL,SND_FILENAME|SND_ASYNC);

	return(0L);
}
