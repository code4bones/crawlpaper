/*
	WallPaperPreviewDlg.cpp
	Dialogo per il preview.
	Luca Piergentili, 08/09/03
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
#include <string.h>
#include "strings.h"
#include "CDialogEx.h"
#include "CWndLayered.h"
#include "CWindowsVersion.h"
#include "CDialogHeader.h"
#include "WallPaperConfig.h"
#include "WallPaperMessages.h"
#include "WallPaperPreviewDlg.h"
#include "WallPaperThreadList.h"
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

BEGIN_MESSAGE_MAP(CWallPaperPreviewDlg,CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_QUERYNEWPALETTE()
	ON_WM_PALETTECHANGED()
	ON_WM_SETFOCUS()
	ON_MESSAGE(WM_PREVIEWMINMAXIMIZE,OnPreviewMinmaximize)
	ON_MESSAGE(WM_PREVIEWFORCEFOCUS,OnPreviewForceFocus)
	ON_MESSAGE(WM_PREVIEWENABLED,OnPreviewEnabled)
	ON_MESSAGE(WM_PREVIEWDISABLED,OnPreviewDisabled)
	ON_MESSAGE(WM_SETCONFIGURATION,OnSetConfig)
	ON_COMMAND(IDM_MENU_EXIT,OnExit)
END_MESSAGE_MAP()

IMPLEMENT_DYNCREATE(CWallPaperPreviewDlg,CDialogEx)

/*
	DoDataExchange()
*/
void CWallPaperPreviewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

/*
	CWallPaperPreviewDlg()
*/
CWallPaperPreviewDlg::CWallPaperPreviewDlg(HWND hWndParent)
: CDialogEx(	/*nTemplateID*/		IDD_DIALOG_PREVIEW,
			/*nToolbarID*/			0L,
			/*nStatusbarID*/		0L,
			/*nIconID*/			IDI_ICON_PREVIEW,
			/*nMenuID*/			0L,
#ifdef _RESIZABLE_DIALOG_STYLE
			/*bAllowResize*/		FALSE,
#endif
			/*bAllowSnap*/			TRUE,
			/*bAllowDragAndDrop*/	FALSE,
			/*lpcszUniqueName*/		IDS_DIALOG_PREVIEW_TITLE,
			/*bAllowMultipleInstances*/FALSE,
			/*hWndParent*/			NULL)
{
	m_hWndParent = hWndParent;
	m_bForceFocus = FALSE;
	m_pConfig = NULL;
	memset(m_szItem,'\0',sizeof(m_szItem));
	SetVisible(FALSE);
}

/*
	OnInitDialog()
*/
BOOL CWallPaperPreviewDlg::OnInitDialog(void)
{
	// classe base
	if(!CDialogEx::OnInitDialog())
	{
		EndDialog(IDCANCEL);
		return(FALSE);
	}

	// inizializzazione estesa
	OnInitDialogEx();

	// preview
	m_wndStaticDib.SubclassDlgItem(IDC_PICTURE_PREVIEW,this);

	// titolo
	CString strTitle;
	strTitle.Format("%s",IDS_DIALOG_PREVIEW_TITLE);
	SetWindowText(strTitle);

	// header
	m_wndHeader.SetTitleText("Title");
	m_wndHeader.SetDescText("Description");
	m_wndHeader.SetIconHandle(NULL);
	m_wndHeader.SetIconOffset(0);
	m_wndHeader.Init(this);
	m_wndHeader.MoveCtrls(this);

	// posiziona la finestra
	SetWindowPos(&CWnd::wndTop,0,0,0,0,SWP_NOSIZE|SWP_SHOWWINDOW);

	return(TRUE);
}

/*
	OnSysCommand()

	Gestore del menu di sistema per l'icona del programma (l'icona della taskbar, non la tray).
*/
void CWallPaperPreviewDlg::OnSysCommand(UINT nID,LPARAM lParam)
{
	//if(nID==SC_MINIMIZE || nID==SC_CLOSE)
	if(nID==SC_CLOSE)
	{
		if(m_hWndParent)
			::PostMessage(m_hWndParent,WM_PREVIEW_CLOSED,0L,0L);
			
		ShowWindow(SW_HIDE);
	}
	else
		CDialogEx::OnSysCommand(nID,lParam);
}

/*
	OnQueryNewPalette()
*/
BOOL CWallPaperPreviewDlg::OnQueryNewPalette(void)
{
	m_wndStaticDib.SendMessage(WM_QUERYNEWPALETTE);
	return(CDialog::OnQueryNewPalette());
}

/*
	OnPaletteChanged()
*/
void CWallPaperPreviewDlg::OnPaletteChanged(CWnd* pFocusWnd)
{
	CDialog::OnPaletteChanged(pFocusWnd);
	m_wndStaticDib.SendMessage(WM_PALETTECHANGED,(WPARAM)pFocusWnd->GetSafeHwnd());
}

/*
	OnSetFocus()
*/
void CWallPaperPreviewDlg::OnSetFocus(CWnd* pOldWnd)
{
	CDialog::OnSetFocus(pOldWnd);
	m_wndStaticDib.SendMessage(WM_QUERYNEWPALETTE);
}

/*
	OnSetConfig()
*/
LONG CWallPaperPreviewDlg::OnSetConfig(UINT wParam,LONG lParam)
{
	m_hWndParent = (HWND)wParam;
	m_pConfig = (CWallPaperConfig*)lParam;
	return(0L);
}

/*
	OnPreviewMinmaximize()
*/
LONG CWallPaperPreviewDlg::OnPreviewMinmaximize(UINT wParam,LONG /*lParam*/)
{
	switch(wParam)
	{	
		case SC_MINIMIZE:
			ShowWindow(SW_MINIMIZE);
			break;
		case SC_MAXIMIZE:
			//SW_RESTORE
			ShowWindow(SW_SHOW);
			ShowWindow(SW_SHOWNOACTIVATE);
			SetWindowPos(&CWnd::wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
			SetWindowPos(&CWnd::wndNoTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
			break;
	}
	return(0L);
}

/*
	OnPreviewForceFocus()
*/
LONG CWallPaperPreviewDlg::OnPreviewForceFocus(UINT wParam,LONG /*lParam*/)
{
	m_bForceFocus = (BOOL)wParam;
	return(0L);
}

/*
	OnPreviewEnabled()
*/
LONG CWallPaperPreviewDlg::OnPreviewEnabled(UINT wParam,LONG lParam)
{
	if((const char*)wParam)
		strcpyn(m_szItem,(const char*)wParam,sizeof(m_szItem));

	if(strcmp(m_szItem,"")!=0 && m_pConfig)
	{
		CImage* pImage = (CImage*)lParam;
		if(pImage)
			m_wndStaticDib.Load(pImage);
		else
			m_wndStaticDib.Load(m_szItem,m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_CURRENTLIBRARY_KEY));
		
		pImage = (CImage*)m_wndStaticDib.GetImage();
		if(pImage)
		{
			char szProperties[256] = {0};
			int nColors = pImage->GetNumColors();
			char szFileSize[32];
			strsize(szFileSize,sizeof(szFileSize),pImage->GetFileSize());
			char szMemUsed[32];
			strsize(szMemUsed,sizeof(szMemUsed),pImage->GetMemUsed());
			_snprintf(szProperties,
					sizeof(szProperties)-1,
					"%d x %d pixels, %d%s colors, %d bpp\n%s required (%s file size)",
					pImage->GetWidth(),
					pImage->GetHeight(),
					(nColors > 256 || nColors==0) ? 16 : nColors,
					(nColors > 256 || nColors==0) ? "M" : "",
					pImage->GetBPP(),
					szMemUsed,
					szFileSize
					);
			m_wndHeader.SetTitleText(pImage->GetFileName());
			m_wndHeader.SetDescText(szProperties);
		}
		else
		{
			m_wndStaticDib.Unload();
			ShowWindow(SW_HIDE);
			return(0L);
		}

		// trasparenza
		if(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAYERED_KEY))
			m_wndLayered.SetLayer(this->m_hWnd,(BYTE)m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAYERED_VALUE_KEY));
		else
			m_wndLayered.Reset();

		SetVisible(TRUE);
		TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperPreviewDlg::OnPreviewEnabled(): %s\n",m_bForceFocus ? "force focus" : "DO NOT force focus"));
		if(m_bForceFocus)
		{
			ShowWindow(SW_SHOW);
			ShowWindow(SW_SHOWNOACTIVATE);
			SetWindowPos(&CWnd::wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
			SetWindowPos(&CWnd::wndNoTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
		}
		else
		{
			if(!IsIconic())
				ShowWindow(SW_SHOWNOACTIVATE);
		}
	}

	return(0L);
}

/*
	OnPreviewDisabled()
*/
LONG CWallPaperPreviewDlg::OnPreviewDisabled(UINT /*wParam*/,LONG /*lParam*/)
{
	m_wndStaticDib.Unload();
	ShowWindow(SW_HIDE);
	return(0L);
}
