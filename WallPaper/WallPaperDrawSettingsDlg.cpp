/*
	WallPaperDrawSettingsDlg.cpp
	Dialogo per le opzioni relative alla modalita' di visualizzazione.
	Luca Piergentili, 21/10/00
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
#include "CIconStatic.h"
#include "CImageFactory.h"
#include "CWndLayered.h"
#include "WallPaperConfig.h"
#include "WallPaperMessages.h"
#include "WallPaperDrawSettingsDlg.h"
#include "WallPaperDrawSettingsModeDlg.h"
#include "WallPaperDrawSettingsAlphaDlg.h"
#include "WallPaperDrawSettingsTextDlg.h"
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

BEGIN_MESSAGE_MAP(CWallPaperDrawSettingsDlg,CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK,OnOk)
	ON_BN_CLICKED(IDCANCEL,OnCancel)
	ON_NOTIFY(TVN_SELCHANGING,IDC_TREE_SETTINGS,OnSelchangingTree)
	ON_NOTIFY(TVN_SELCHANGED,IDC_TREE_SETTINGS,OnSelchangedTree)
END_MESSAGE_MAP()

/*
	DoDataExchange()
*/
void CWallPaperDrawSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_STATIC_SETTINGS_AREA,m_ctrlSettingsArea);
	DDX_Control(pDX,IDC_TREE_SETTINGS,m_wndSettingsTree);
}

/*
	CWallPaperDrawSettingsDlg()
*/
CWallPaperDrawSettingsDlg::CWallPaperDrawSettingsDlg(CWnd* pParent,CWallPaperConfig* pConfig) : CDialog(IDD_DIALOG_SETTINGS,pParent)
{
	m_pConfig = pConfig;
	m_bCreated = FALSE;
	m_dwRootData = 0L;
}

/*
	OnInitDialog()
*/
BOOL CWallPaperDrawSettingsDlg::OnInitDialog(void)
{
	// classe base
	CDialog::OnInitDialog();

	// titolo
	SetWindowText(IDS_DIALOG_DRAW_SETTINGS_TITLE);

	// cornice interna
	m_ctrlSettingsArea.SetIconSize(16);
	m_ctrlSettingsArea.SetIcon(IDI_ICON_SETTINGS);
	m_ctrlSettingsArea.SetText(IDS_DIALOG_DRAW_SETTINGS_TITLE);	
	m_ctrlSettingsArea.ShowWindow(SW_SHOW);

	// icona di default
	SetIcon((HICON)::LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICON_WALLPAPER),IMAGE_ICON,16,16,LR_DEFAULTCOLOR),FALSE);
	SetIcon((HICON)::LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICON_WALLPAPER),IMAGE_ICON,32,32,LR_DEFAULTCOLOR),TRUE);

	// crea l'albero per le opzioni
	m_hItemRoot = m_wndSettingsTree.InsertItem("Draw",TVI_ROOT,TVI_LAST);
	m_wndSettingsTree.SetItemData(m_hItemRoot,m_dwRootData);
	
	// Mode
	m_hItemMode = m_wndSettingsTree.InsertItem("Mode",m_hItemRoot,TVI_LAST);
	m_wndSettingsTree.SetItemData(m_hItemMode,(DWORD)&m_dlgTreeMode);

	// Alpha
	m_hItemAlpha = m_wndSettingsTree.InsertItem("Transparency",m_hItemRoot,TVI_LAST);
	m_wndSettingsTree.SetItemData(m_hItemAlpha,(DWORD)&m_dlgTreeAlpha);
	
	// Text
	m_hItemText = m_wndSettingsTree.InsertItem("Text",m_hItemRoot,TVI_LAST);
	m_wndSettingsTree.SetItemData(m_hItemText,(DWORD)&m_dlgTreeText);

	// posiziona sulla radice dell'albero	
	m_wndSettingsTree.Expand(m_hItemRoot,TVE_EXPAND);
	m_wndSettingsTree.SelectItem(m_hItemRoot);

	// crea i dialoghi relativi
	m_dlgTreeMode.CreateEx(this,IDD_DIALOG_DRAW_SETTINGS_MODE,m_pConfig);
	m_dlgTreeAlpha.CreateEx(this,IDD_DIALOG_DRAW_SETTINGS_TRANSPARENCY,m_pConfig);
	m_dlgTreeText.CreateEx(this,IDD_DIALOG_DRAW_SETTINGS_TEXT,m_pConfig);

	// posiziona i dialoghi nel controllo
	CRect rcDlg;
	GetDlgItem(IDC_STATIC_SETTINGS_AREA)->GetWindowRect(rcDlg);
	ScreenToClient(rcDlg);
	m_dlgTreeMode.MoveWindow(rcDlg);
	m_dlgTreeAlpha.MoveWindow(rcDlg);
	m_dlgTreeText.MoveWindow(rcDlg);

	m_bCreated = TRUE;

	// trasparenza
	if(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAYERED_KEY))
		m_wndLayered.SetLayer(this->m_hWnd,(BYTE)m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAYERED_VALUE_KEY));

	return(TRUE);
}

/*
	OnOk()
*/
void CWallPaperDrawSettingsDlg::OnOk(void)
{
	// Mode
	if(!m_dlgTreeMode.UpdateData(TRUE))
	{
		m_wndSettingsTree.SelectItem(m_hItemMode);
		m_dlgTreeMode.OnSetActive();
		return;
	}

	if(m_dlgTreeMode.m_nAreaRatio < 1 || m_dlgTreeMode.m_nAreaRatio > 100)
	{
		m_wndSettingsTree.SelectItem(m_hItemMode);
		m_dlgTreeMode.OnSetActive();
		::MessageBoxResource(this->m_hWnd,MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_INVALID_AREARATIO);
		return;
	}
	
	if(m_dlgTreeMode.m_nWidth < m_dlgTreeMode.m_nMinWidth || m_dlgTreeMode.m_nWidth > m_dlgTreeMode.m_nScreenWidth)
	{
		m_wndSettingsTree.SelectItem(m_hItemMode);
		m_dlgTreeMode.OnSetActive();
		::MessageBoxResourceEx(this->m_hWnd,MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_INVALID_WIDTH,m_dlgTreeMode.m_nMinWidth,m_dlgTreeMode.m_nScreenWidth);
		return;
	}

	if(m_dlgTreeMode.m_nHeight < m_dlgTreeMode.m_nMinHeight || m_dlgTreeMode.m_nHeight > m_dlgTreeMode.m_nScreenHeight)
	{
		m_wndSettingsTree.SelectItem(m_hItemMode);
		m_dlgTreeMode.OnSetActive();
		::MessageBoxResourceEx(this->m_hWnd,MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_INVALID_HEIGHT,m_dlgTreeMode.m_nMinHeight,m_dlgTreeMode.m_nScreenHeight);
		return;
	}

	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_DRAWMODE_KEY,m_dlgTreeMode.m_nDrawMode);
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_DRAWAREARATIO_KEY,m_dlgTreeMode.m_nAreaRatio);
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_DRAWRECTX_KEY,m_dlgTreeMode.m_nWidth);
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_DRAWRECTY_KEY,m_dlgTreeMode.m_nHeight);
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_DRAWEFFECT_KEY,m_dlgTreeMode.m_nDrawEffect);
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_DRAWEFFECTVALUE_KEY,m_dlgTreeMode.m_nDrawEffectValue);

	// Alpha
	if(!m_dlgTreeAlpha.UpdateData(TRUE))
	{
		m_wndSettingsTree.SelectItem(m_hItemAlpha);
		m_dlgTreeAlpha.OnSetActive();
		return;
	}

	// trasparenza
	if(m_dlgTreeAlpha.m_bLayered)
	{
		if(m_dlgTreeAlpha.m_nLayered >= LWA_ALPHA_INVISIBLE && m_dlgTreeAlpha.m_nLayered <= LWA_ALPHA_OPAQUE)
		{
			if(m_dlgTreeAlpha.m_nLayered < LWA_ALPHA_INVISIBLE_THRESHOLD)
				m_dlgTreeAlpha.m_bLayered = ::MessageBoxResourceEx(this->m_hWnd,MB_YESNO|MB_ICONQUESTION,WALLPAPER_PROGRAM_NAME,IDS_QUESTION_LAYERED,m_dlgTreeAlpha.m_nLayered,LWA_ALPHA_INVISIBLE,LWA_ALPHA_OPAQUE)==IDYES;
		}
		else if(m_dlgTreeAlpha.m_nLayered==LWA_ALPHA_INVISIBLE || m_dlgTreeAlpha.m_nLayered==LWA_ALPHA_OPAQUE)
		{
			m_dlgTreeAlpha.m_bLayered = FALSE;
			m_dlgTreeAlpha.m_nLayered = 0;
			::MessageBoxResourceEx(this->m_hWnd,MB_OK|MB_ICONWARNING,WALLPAPER_PROGRAM_NAME,IDS_MESSAGE_LAYERED_WARNING,LWA_ALPHA_INVISIBLE,LWA_ALPHA_OPAQUE);
		}
	}

	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAYERED_KEY,m_dlgTreeAlpha.m_bLayered);
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAYERED_VALUE_KEY,m_dlgTreeAlpha.m_nLayered);

	// Text
	if(!m_dlgTreeText.UpdateData(TRUE))
	{
		m_wndSettingsTree.SelectItem(m_hItemText);
		m_dlgTreeText.OnSetActive();
		return;
	}

	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_DRAWTEXTMODE_KEY,m_dlgTreeText.m_nDrawTextMode);
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_DRAWTEXTPOSITION_KEY,m_dlgTreeText.m_nDrawTextPosition);
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_CF_iPointSize,m_dlgTreeText.m_cf.iPointSize);
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_CF_rgbColors,m_dlgTreeText.m_cf.rgbColors);
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_CF_nFontType,m_dlgTreeText.m_cf.nFontType);
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfHeight,m_dlgTreeText.m_lf.lfHeight);
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfWidth,m_dlgTreeText.m_lf.lfWidth);
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfEscapement,m_dlgTreeText.m_lf.lfEscapement);
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfOrientation,m_dlgTreeText.m_lf.lfOrientation);
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfWeight,m_dlgTreeText.m_lf.lfWeight);
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfItalic,m_dlgTreeText.m_lf.lfItalic);
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfUnderline,m_dlgTreeText.m_lf.lfUnderline);
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfStrikeOut,m_dlgTreeText.m_lf.lfStrikeOut);
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfCharSet,m_dlgTreeText.m_lf.lfCharSet);
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfOutPrecision,m_dlgTreeText.m_lf.lfOutPrecision);
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfClipPrecision,m_dlgTreeText.m_lf.lfClipPrecision);
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfQuality,m_dlgTreeText.m_lf.lfQuality);
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfPitchAndFamily,m_dlgTreeText.m_lf.lfPitchAndFamily);
	m_pConfig->UpdateString(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfFaceName,m_dlgTreeText.m_lf.lfFaceName);
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_TEXTCOLOR,m_dlgTreeText.m_crFontTextColor);
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_BKCOLOR,m_dlgTreeText.m_crFontBkColor);
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_BKCOLORTRANSPARENT,m_dlgTreeText.m_bFontBkColorTransparent);

	EndDialog(IDOK);
}

/*
	OnCancel()
*/
void CWallPaperDrawSettingsDlg::OnCancel(void)
{
	EndDialog(IDCANCEL);
}

/*
	OnSelchangingTree()
*/
void CWallPaperDrawSettingsDlg::OnSelchangingTree(NMHDR* /*pNMHDR*/,LRESULT* pResult)
{
	if(m_bCreated)
	{
		HTREEITEM hCurrent = m_wndSettingsTree.GetSelectedItem();
		if(hCurrent)
		{
			CPropertyPage* pPropertyPage = (CPropertyPage*)m_wndSettingsTree.GetItemData(hCurrent);
			if(pPropertyPage)
				pPropertyPage->ShowWindow(SW_HIDE);
		}
	}

	*pResult = 0;
}

/*
	OnSelchangedTree()
*/
void CWallPaperDrawSettingsDlg::OnSelchangedTree(NMHDR* /*pNMHDR*/,LRESULT* pResult)
{
	if(m_bCreated)
	{
		HTREEITEM hCurrent = m_wndSettingsTree.GetSelectedItem();
		if(hCurrent)
		{
			CPropertyPage* pPropertyPage = (CPropertyPage*)m_wndSettingsTree.GetItemData(hCurrent);
			if(pPropertyPage)
			{
				m_ctrlSettingsArea.SetText("");	
				m_ctrlSettingsArea.ShowWindow(SW_HIDE);

				pPropertyPage->ShowWindow(SW_SHOW);
				pPropertyPage->OnSetActive();
			}
			else
			{
				m_ctrlSettingsArea.SetText(IDS_DIALOG_DRAW_SETTINGS_TITLE);	
				m_ctrlSettingsArea.ShowWindow(SW_SHOW);
			}
		}
	}

	*pResult = 0;
}
