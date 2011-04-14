/*
	WallPaperDrawSettingsTextDlg.cpp
	Dialogo per la pagina relativa alle opzioni per la modalita' di visualizzazione.
	Luca Piergentili, 10/06/03
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
#include "strings.h"
#include "window.h"
#include "CColorStatic.h"
#include "CToolTipCtrlEx.h"
#include "WallPaperConfig.h"
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

BEGIN_MESSAGE_MAP(CWallPaperDrawSettingsTextDlg,CPropertyPage)
	ON_CBN_KILLFOCUS(IDC_COMBO_TEXT,OnKillFocusComboText)
	ON_CBN_SELCHANGE(IDC_COMBO_TEXT,OnSelChangeComboText)
	ON_CBN_KILLFOCUS(IDC_COMBO_TEXTPOSITION,OnKillFocusComboTextPosition)
	ON_CBN_SELCHANGE(IDC_COMBO_TEXTPOSITION,OnSelChangeComboTextPosition)
	ON_BN_CLICKED(IDC_BUTTON_FONT,OnButtonFont)
	ON_BN_CLICKED(IDC_BUTTON_FGCOLOR,OnButtonFgColor)
	ON_BN_CLICKED(IDC_BUTTON_BKCOLOR,OnButtonBkColor)
	ON_BN_CLICKED(IDC_CHECK_TRANSPARENT,OnCheckBkColorTransparent)
END_MESSAGE_MAP()

IMPLEMENT_DYNCREATE(CWallPaperDrawSettingsTextDlg,CPropertyPage)

/*
	DoDataExchange()
*/
void CWallPaperDrawSettingsTextDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_COMBO_TEXT,m_wndComboText);
	DDX_Control(pDX,IDC_COMBO_TEXTPOSITION,m_wndComboTextPosition);
	DDX_Check(pDX,IDC_CHECK_TRANSPARENT,m_bFontBkColorTransparent);
}

/*
	CreateEx()
*/
BOOL CWallPaperDrawSettingsTextDlg::CreateEx(CWnd* pParent,UINT nID,CWallPaperConfig* pConfig)
{
	// configurazione corrente
	m_pConfig = pConfig;

	// crea il dialogo
	return(Create(nID,pParent));
}

/*
	OnInitDialog()
*/
BOOL CWallPaperDrawSettingsTextDlg::OnInitDialog(void)
{
	// classe base
	CPropertyPage::OnInitDialog();

	// combo per selezione tipo stringa (testo)
	m_nDrawTextMode = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_DRAWTEXTMODE_KEY);
	m_wndComboText.SetCurSel(m_nDrawTextMode);

	// combo per selezione posizione stringa: 0=left/up, 1=right/down
	m_nDrawTextPosition = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_DRAWTEXTPOSITION_KEY);
	m_wndComboTextPosition.SetCurSel(m_nDrawTextPosition);

	GetDlgItem(IDC_COMBO_TEXTPOSITION)->EnableWindow(m_nDrawTextMode!=0);
	GetDlgItem(IDC_BUTTON_FONT)->EnableWindow(m_nDrawTextMode!=0);
	GetDlgItem(IDC_BUTTON_FGCOLOR)->EnableWindow(m_nDrawTextMode!=0);
	GetDlgItem(IDC_BUTTON_BKCOLOR)->EnableWindow(m_nDrawTextMode!=0);
	GetDlgItem(IDC_CHECK_TRANSPARENT)->EnableWindow(m_nDrawTextMode!=0);

	m_hFont = NULL;

	// strutture per il font
	ZeroMemory(&m_cf,sizeof(CHOOSEFONT));
	m_cf.lStructSize = sizeof(CHOOSEFONT);
	m_cf.hwndOwner = this->m_hWnd;
	m_cf.lpLogFont = &m_lf;
	m_cf.Flags = CF_SCREENFONTS | CF_EFFECTS | CF_INITTOLOGFONTSTRUCT;
	
	ZeroMemory(&m_lf,sizeof(LOGFONT));
	m_cf.iPointSize = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_CF_iPointSize);
	m_cf.rgbColors = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_CF_rgbColors);
	m_cf.nFontType = (WORD)m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_CF_nFontType);
	m_lf.lfHeight = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfHeight);
	m_lf.lfWidth = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfWidth);
	m_lf.lfEscapement = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfEscapement);
	m_lf.lfOrientation = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfOrientation);
	m_lf.lfWeight = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfWeight);
	m_lf.lfItalic = (BYTE)m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfItalic);
	m_lf.lfUnderline = (BYTE)m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfUnderline);
	m_lf.lfStrikeOut = (BYTE)m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfStrikeOut);
	m_lf.lfCharSet = (BYTE)m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfCharSet);
	m_lf.lfOutPrecision = (BYTE)m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfOutPrecision);
	m_lf.lfClipPrecision = (BYTE)m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfClipPrecision);
	m_lf.lfQuality = (BYTE)m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfQuality);
	m_lf.lfPitchAndFamily = (BYTE)m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfPitchAndFamily);
	m_strFontName.Format("%s",m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfFaceName));
	lstrcpy(m_lf.lfFaceName,m_strFontName);

	// sample text, sample fg/bg colors
	m_wndSampleText.SubclassDlgItem(IDC_STATIC_SAMPLE,this);
	m_wndSampleTextColor.SubclassDlgItem(IDC_STATIC_TEXTCOLOR,this);
	m_wndSampleBkColor.SubclassDlgItem(IDC_STATIC_BKCOLOR,this);
	
	m_crFontTextColor = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_TEXTCOLOR);
	m_crFontBkColor = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_BKCOLOR);
	m_bFontBkColorTransparent = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_BKCOLORTRANSPARENT);
	GetDlgItem(IDC_BUTTON_BKCOLOR)->EnableWindow(!m_bFontBkColorTransparent && m_nDrawTextMode!=0);

	m_wndSampleText.SetWindowText(WALLPAPER_WEB_SITE);
	m_wndSampleText.SetTextColor(m_crFontTextColor);
	m_wndSampleText.SetBkColor(m_bFontBkColorTransparent ? ::GetSysColor(COLOR_3DFACE) : m_crFontBkColor);
	if(m_hFont)
		::DeleteObject(m_hFont);
	m_hFont = ::CreateFontIndirect(&m_lf);  
	CFont* pFont = CFont::FromHandle(m_hFont);
	m_wndSampleText.SetFont(pFont,TRUE);
	
	m_wndSampleTextColor.SetBkColor(m_crFontTextColor);
	m_wndSampleBkColor.SetBkColor(m_bFontBkColorTransparent ? ::GetSysColor(COLOR_3DFACE) : m_crFontBkColor);

	// aggiunge i tooltips
	if(m_Tooltip.Create(this,TTS_ALWAYSTIP))
	{
		m_Tooltip.SetWidth(TOOLTIP_REASONABLE_WIDTH);
		m_Tooltip.SetDelay(TOOLTIP_REASONABLE_DELAYTIME);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_COMBO_TEXT),IDC_COMBO_TEXT,IDS_TOOLTIP_OPTIONS_DRAW_TEXTMODE);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_COMBO_TEXTPOSITION),IDC_COMBO_TEXTPOSITION,IDS_TOOLTIP_OPTIONS_DRAW_TEXTPOS);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_STATIC_SAMPLE),IDC_STATIC_SAMPLE,IDS_TOOLTIP_OPTIONS_DRAW_TEXTPREVIEW);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_BUTTON_FONT),IDC_BUTTON_FONT,IDS_TOOLTIP_OPTIONS_DRAW_TEXTFONT);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_BUTTON_FGCOLOR),IDC_BUTTON_FGCOLOR,IDS_TOOLTIP_OPTIONS_DRAW_TEXTFONTFG);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_STATIC_TEXTCOLOR),IDC_STATIC_TEXTCOLOR,IDS_TOOLTIP_OPTIONS_DRAW_TEXTFONTFG);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_BUTTON_BKCOLOR),IDC_BUTTON_BKCOLOR,IDS_TOOLTIP_OPTIONS_DRAW_TEXTFONTBG);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_STATIC_BKCOLOR),IDC_STATIC_BKCOLOR,IDS_TOOLTIP_OPTIONS_DRAW_TEXTFONTBG);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_CHECK_TRANSPARENT),IDC_CHECK_TRANSPARENT,IDS_TOOLTIP_OPTIONS_DRAW_TEXTTRANSPBG);
	}

	UpdateData(FALSE);

	return(FALSE);
}

/*
	OnKillFocusComboText()
*/
void CWallPaperDrawSettingsTextDlg::OnKillFocusComboText(void)
{
	char szText[256] = {0};
	m_wndComboText.GetWindowText(szText,sizeof(szText)-1);
	if(strcmp(szText,"<none>")==0)
		m_nDrawTextMode = DRAWTEXT_NONE;
	else if(strcmp(szText,"current date/time")==0)
		m_nDrawTextMode = DRAWTEXT_DATETIME;
	else if(strcmp(szText,"picture filename")==0)
		m_nDrawTextMode = DRAWTEXT_FILENAME;
	else if(strcmp(szText,"picture pathname")==0)
		m_nDrawTextMode = DRAWTEXT_PATHNAME;
	else if(strcmp(szText,"quotes")==0)
		m_nDrawTextMode = DRAWTEXT_QUOTES;

	GetDlgItem(IDC_COMBO_TEXTPOSITION)->EnableWindow(m_nDrawTextMode!=0);
	GetDlgItem(IDC_BUTTON_FONT)->EnableWindow(m_nDrawTextMode!=0);
	GetDlgItem(IDC_BUTTON_FGCOLOR)->EnableWindow(m_nDrawTextMode!=0);
	GetDlgItem(IDC_BUTTON_BKCOLOR)->EnableWindow(!m_bFontBkColorTransparent && m_nDrawTextMode!=0);
	GetDlgItem(IDC_CHECK_TRANSPARENT)->EnableWindow(m_nDrawTextMode!=0);
}

/*
	OnSelChangeComboText()
*/
void CWallPaperDrawSettingsTextDlg::OnSelChangeComboText(void)
{
	char szText[256] = {0};
	m_wndComboText.GetLBText(m_wndComboText.GetCurSel(),szText);
	if(strcmp(szText,"<none>")==0)
		m_nDrawTextMode = DRAWTEXT_NONE;
	else if(strcmp(szText,"current date/time")==0)
		m_nDrawTextMode = DRAWTEXT_DATETIME;
	else if(strcmp(szText,"picture filename")==0)
		m_nDrawTextMode = DRAWTEXT_FILENAME;
	else if(strcmp(szText,"picture pathname")==0)
		m_nDrawTextMode = DRAWTEXT_PATHNAME;
	else if(strcmp(szText,"quotes")==0)
		m_nDrawTextMode = DRAWTEXT_QUOTES;

	GetDlgItem(IDC_COMBO_TEXTPOSITION)->EnableWindow(m_nDrawTextMode!=0);
	GetDlgItem(IDC_BUTTON_FONT)->EnableWindow(m_nDrawTextMode!=0);
	GetDlgItem(IDC_BUTTON_FGCOLOR)->EnableWindow(m_nDrawTextMode!=0);
	GetDlgItem(IDC_BUTTON_BKCOLOR)->EnableWindow(!m_bFontBkColorTransparent && m_nDrawTextMode!=0);
	GetDlgItem(IDC_CHECK_TRANSPARENT)->EnableWindow(m_nDrawTextMode!=0);
}

/*
	OnKillFocusComboTextPosition()
*/
void CWallPaperDrawSettingsTextDlg::OnKillFocusComboTextPosition(void)
{
	char szText[256] = {0};
	m_wndComboTextPosition.GetWindowText(szText,sizeof(szText)-1);
	if(strcmp(szText,"left/up")==0)
		m_nDrawTextPosition = 0;
	else if(strcmp(szText,"right/down")==0)
		m_nDrawTextPosition = 1;
}

/*
	OnSelChangeComboTextPosition()
*/
void CWallPaperDrawSettingsTextDlg::OnSelChangeComboTextPosition(void)
{
	char szText[256] = {0};
	m_wndComboTextPosition.GetWindowText(szText,sizeof(szText)-1);
	if(strcmp(szText,"left/up")==0)
		m_nDrawTextPosition = DRAWTEXT_POS_LEFTUP;
	else if(strcmp(szText,"right/down")==0)
		m_nDrawTextPosition = DRAWTEXT_POS_RIGHTDOWN;

	else if(strcmp(szText,"left/down")==0)
		m_nDrawTextPosition = DRAWTEXT_POS_LEFTDOWN;
	else if(strcmp(szText,"right/up")==0)
		m_nDrawTextPosition = DRAWTEXT_POS_RIGHTUP;
}

/*
	OnButtonFont()
*/
void CWallPaperDrawSettingsTextDlg::OnButtonFont(void)
{
	m_cf.rgbColors = m_crFontTextColor;

	if(::ChooseFont(&m_cf))
	{
		if(m_hFont)
			::DeleteObject(m_hFont);

		m_hFont = ::CreateFontIndirect(&m_lf);  
		CFont* pFont = CFont::FromHandle(m_hFont);
		
		m_crFontTextColor = m_cf.rgbColors;
		m_wndSampleText.SetTextColor(m_cf.rgbColors);
		m_wndSampleText.SetBkColor(m_bFontBkColorTransparent ? ::GetSysColor(COLOR_3DFACE) : m_crFontBkColor);
		m_wndSampleText.SetFont(pFont,TRUE);
		
		m_wndSampleTextColor.SetBkColor(m_cf.rgbColors);
	}
}

/*
	OnButtonFgColor()
*/
void CWallPaperDrawSettingsTextDlg::OnButtonFgColor(void)
{
	CColorDialog dlg;
	dlg.m_cc.Flags |= CC_RGBINIT;
	dlg.m_cc.Flags |= CC_FULLOPEN;
	dlg.m_cc.Flags |= CC_ANYCOLOR;
	dlg.m_cc.rgbResult = m_crFontTextColor;
	if(dlg.DoModal())
	{
		m_crFontTextColor = m_cf.rgbColors = dlg.GetColor();
		m_wndSampleText.SetTextColor(m_cf.rgbColors);
		m_wndSampleTextColor.SetBkColor(m_crFontTextColor);
	}
}

/*
	OnButtonBkColor()
*/
void CWallPaperDrawSettingsTextDlg::OnButtonBkColor(void)
{
	CColorDialog dlg;
	dlg.m_cc.Flags |= CC_RGBINIT;
	dlg.m_cc.Flags |= CC_FULLOPEN;
	dlg.m_cc.Flags |= CC_ANYCOLOR;
	dlg.m_cc.rgbResult = m_crFontBkColor;
	if(dlg.DoModal())
	{
		m_crFontBkColor = dlg.GetColor();
		m_wndSampleText.SetBkColor(m_crFontBkColor);
		m_wndSampleBkColor.SetBkColor(m_crFontBkColor);
	}
}

/*
	OnCheckBkColorTransparent()
*/
void CWallPaperDrawSettingsTextDlg::OnCheckBkColorTransparent(void)
{
	m_bFontBkColorTransparent = !m_bFontBkColorTransparent;
	if(m_bFontBkColorTransparent)
	{
		m_wndSampleText.SetBkColor(::GetSysColor(COLOR_3DFACE));
		m_wndSampleBkColor.SetBkColor(::GetSysColor(COLOR_3DFACE));
	}
	GetDlgItem(IDC_BUTTON_BKCOLOR)->EnableWindow(!m_bFontBkColorTransparent && m_nDrawTextMode!=0);
}
