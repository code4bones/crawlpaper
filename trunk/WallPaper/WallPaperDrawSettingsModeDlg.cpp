/*
	WallPaperDrawSettingsModeDlg.cpp
	Dialogo per la pagina relativa alle opzioni per le miniature.
	Luca Piergentili, 20/09/01
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
#include "CToolTipCtrlEx.h"
#include "WallPaperConfig.h"
#include "WallPaperDrawSettingsModeDlg.h"
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

BEGIN_MESSAGE_MAP(CWallPaperDrawSettingsModeDlg,CPropertyPage)
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDC_RADIO_OWNERMODE,OnRadioButtonOwnerMode)
	ON_BN_CLICKED(IDC_RADIO_WINDOWSMODE,OnRadioButtonWindowsMode)
	ON_CBN_KILLFOCUS(IDC_COMBO_OWNERMODE,OnKillFocusComboOwnerMode)
	ON_CBN_SELCHANGE(IDC_COMBO_OWNERMODE,OnSelChangeComboOwnerMode)
	ON_CBN_KILLFOCUS(IDC_COMBO_WINDOWSMODE,OnKillFocusComboWindowsMode)
	ON_CBN_SELCHANGE(IDC_COMBO_WINDOWSMODE,OnSelChangeComboWindowsMode)
	ON_CBN_KILLFOCUS(IDC_COMBO_EFFECT,OnKillFocusComboEffect)
	ON_CBN_SELCHANGE(IDC_COMBO_EFFECT,OnSelChangeComboEffect)
	ON_EN_CHANGE(IDC_EDIT_EFFECT,OnEnChangeEffect)
END_MESSAGE_MAP()

IMPLEMENT_DYNCREATE(CWallPaperDrawSettingsModeDlg,CPropertyPage)

/*
	DoDataExchange()
*/
void CWallPaperDrawSettingsModeDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Radio(pDX,IDC_RADIO_OWNERMODE,m_nOwnerMode);
	DDX_Radio(pDX,IDC_RADIO_WINDOWSMODE,m_nWindowsMode);
	DDX_Control(pDX,IDC_COMBO_OWNERMODE,m_wndComboOwnerMode);
	DDX_Control(pDX,IDC_COMBO_WINDOWSMODE,m_wndComboWindowsMode);
	DDX_Text(pDX,IDC_EDIT_AREA,m_nAreaRatio);
	DDX_Text(pDX,IDC_EDIT_WIDTH,m_nWidth);
	DDX_Text(pDX,IDC_EDIT_HEIGHT,m_nHeight);
	DDX_Text(pDX,IDC_EDIT_EFFECT,m_nDrawEffectValue);
	DDX_Control(pDX,IDC_COMBO_EFFECT,m_wndComboEffect);
}

/*
	CreateEx()
*/
BOOL CWallPaperDrawSettingsModeDlg::CreateEx(CWnd* pParent,UINT nID,CWallPaperConfig* pConfig)
{
	// configurazione corrente
	m_pConfig = pConfig;

	// crea l'oggetto immagine per l'elenco dei formati supportati
	strcpyn(m_szLibraryName,m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_CURRENTLIBRARY_KEY),sizeof(m_szLibraryName));
	m_pImage = m_ImageFactory.Create(m_szLibraryName,sizeof(m_szLibraryName));
	m_ImageParams.Reset();
	m_pImage->SetImageParamsDefaultValues(&m_ImageParams);
	m_pImage->SetImageParamsMinMax(&m_ImageParams);

	// imposta con i valori della configurazione
	m_nOwnerMode       = m_nWindowsMode = 0;
	m_nDrawMode        = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_DRAWMODE_KEY);
	m_nAreaRatio       = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_DRAWAREARATIO_KEY);
	m_nWidth           = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_DRAWRECTX_KEY);
	m_nHeight          = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_DRAWRECTY_KEY);
	m_nMinWidth        = DEFAULT_DRAWRECTX_MIN;
	m_nMinHeight       = DEFAULT_DRAWRECTY_MIN;
	m_nScreenWidth     = ::GetSystemMetrics(SM_CXSCREEN);
	m_nScreenHeight    = ::GetSystemMetrics(SM_CYSCREEN);
	m_nAreaRatio       = m_nAreaRatio < 1 || m_nAreaRatio > 100 ? 50 : m_nAreaRatio;
	m_nWidth           = m_nWidth < m_nMinWidth || m_nWidth > m_nScreenWidth ? m_nMinWidth : m_nWidth;
	m_nHeight          = m_nHeight < m_nMinHeight || m_nHeight > m_nScreenHeight ? m_nMinHeight : m_nHeight;
	m_nDrawEffect      = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_DRAWEFFECT_KEY);
	m_nDrawEffectValue = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_DRAWEFFECTVALUE_KEY);

	// crea il dialogo
	return(Create(nID,pParent));
}

/*
	OnInitDialog()
*/
BOOL CWallPaperDrawSettingsModeDlg::OnInitDialog(void)
{
	// classe base
	CPropertyPage::OnInitDialog();

	// imposta i radio buttons
	switch(m_nDrawMode)
	{
		case DRAWMODE_ORIGINAL_SIZE:
		case DRAWMODE_FIT_TO_SCREEN:
		case DRAWMODE_STRETCH_TO_SCREEN:
		case DRAWMODE_STRETCH_TO_RECT:
		case DRAWMODE_STRETCH_IF_DOES_NOT_FIT:
		case DRAWMODE_TILE_STRETCH:
		case DRAWMODE_TILE_FIT:
		case DRAWMODE_OVERLAP_STRETCH:
		case DRAWMODE_OVERLAP_FIT:
			m_nOwnerMode = 0;
			m_nWindowsMode = 1;
			break;
		case DRAWMODE_WINDOWS_NORMAL:
		case DRAWMODE_WINDOWS_TILE:
		case DRAWMODE_WINDOWS_STRETCH:
			m_nOwnerMode = 1;
			m_nWindowsMode = 0;
			break;
	}

	switch(m_nDrawMode)
	{
		case DRAWMODE_TILE_STRETCH:
			GetDlgItem(IDC_STATIC_AREARATIO)->SetWindowText(IDS_DRAWOWNERMODE_TILE_STRETCH_TEXT);
			break;
		case DRAWMODE_TILE_FIT:
			GetDlgItem(IDC_STATIC_AREARATIO)->SetWindowText(IDS_DRAWOWNERMODE_TILE_FIT_TEXT);
			break;
		case DRAWMODE_OVERLAP_STRETCH:
			GetDlgItem(IDC_STATIC_AREARATIO)->SetWindowText(IDS_DRAWOWNERMODE_OVERLAP_STRETCH_TEXT);
			break;
		case DRAWMODE_OVERLAP_FIT:
			GetDlgItem(IDC_STATIC_AREARATIO)->SetWindowText(IDS_DRAWOWNERMODE_OVERLAP_FIT_TEXT);
			break;
	}

	// abilita i controlli
	GetDlgItem(IDC_EDIT_AREA)->EnableWindow(FALSE);
	GetDlgItem(IDC_SPIN_AREA)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_WIDTH)->EnableWindow(FALSE);
	GetDlgItem(IDC_SPIN_WIDTH)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_HEIGHT)->EnableWindow(FALSE);
	GetDlgItem(IDC_SPIN_HEIGHT)->EnableWindow(FALSE);
	
	// carica il combo con le modalita' proprie
	m_wndComboOwnerMode.AddString(IDS_DRAWOWNERMODE_ORIGINAL_SIZE);
	m_wndComboOwnerMode.AddString(IDS_DRAWOWNERMODE_FIT_TO_SCREEN);
	m_wndComboOwnerMode.AddString(IDS_DRAWOWNERMODE_STRETCH_TO_SCREEN);
	m_wndComboOwnerMode.AddString(IDS_DRAWOWNERMODE_STRETCH_TO_SCREEN_IF_DOES_NOT_FIT);
	m_wndComboOwnerMode.AddString(IDS_DRAWOWNERMODE_STRETCH_TO_RECT);
	m_wndComboOwnerMode.AddString(IDS_DRAWOWNERMODE_TILE_STRETCH);
	m_wndComboOwnerMode.AddString(IDS_DRAWOWNERMODE_TILE_FIT);
	m_wndComboOwnerMode.AddString(IDS_DRAWOWNERMODE_OVERLAP_STRETCH);
	m_wndComboOwnerMode.AddString(IDS_DRAWOWNERMODE_OVERLAP_FIT);
	int nIndex = -1;
	switch(m_nDrawMode)
	{
		case DRAWMODE_ORIGINAL_SIZE:
			nIndex = 0;
			break;
		case DRAWMODE_FIT_TO_SCREEN:
			nIndex = 1;
			break;
		case DRAWMODE_STRETCH_TO_SCREEN:
			nIndex = 2;
			break;
		case DRAWMODE_STRETCH_IF_DOES_NOT_FIT:
			nIndex = 3;
			break;
		case DRAWMODE_STRETCH_TO_RECT:
			nIndex = 4;
			GetDlgItem(IDC_EDIT_WIDTH)->EnableWindow(TRUE);
			GetDlgItem(IDC_SPIN_WIDTH)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_HEIGHT)->EnableWindow(TRUE);
			GetDlgItem(IDC_SPIN_HEIGHT)->EnableWindow(TRUE);
			break;
		case DRAWMODE_TILE_STRETCH:
			nIndex = 5;
			GetDlgItem(IDC_EDIT_AREA)->EnableWindow(TRUE);
			GetDlgItem(IDC_SPIN_AREA)->EnableWindow(TRUE);
			break;
		case DRAWMODE_TILE_FIT:
			nIndex = 6;
			GetDlgItem(IDC_EDIT_AREA)->EnableWindow(TRUE);
			GetDlgItem(IDC_SPIN_AREA)->EnableWindow(TRUE);
			break;
		case DRAWMODE_OVERLAP_STRETCH:
			nIndex = 7;
			GetDlgItem(IDC_EDIT_AREA)->EnableWindow(TRUE);
			GetDlgItem(IDC_SPIN_AREA)->EnableWindow(TRUE);
			break;
		case DRAWMODE_OVERLAP_FIT:
			nIndex = 8;
			GetDlgItem(IDC_EDIT_AREA)->EnableWindow(TRUE);
			GetDlgItem(IDC_SPIN_AREA)->EnableWindow(TRUE);
			break;
	}
	m_wndComboOwnerMode.SetCurSel(nIndex >= 0 ? nIndex : 0);
	GetDlgItem(IDC_COMBO_OWNERMODE)->EnableWindow(nIndex >= 0 ? TRUE : FALSE);

	// carica il combo con le modalita' previste dal sistema
	m_wndComboWindowsMode.AddString(IDS_DRAWWINDOWSMODE_NORMAL);
	m_wndComboWindowsMode.AddString(IDS_DRAWWINDOWSMODE_TILE);
	m_wndComboWindowsMode.AddString(IDS_DRAWWINDOWSMODE_STRETCH);
	nIndex = -1;
	switch(m_nDrawMode)
	{
		case DRAWMODE_WINDOWS_NORMAL:
			nIndex = 0;
			break;
		case DRAWMODE_WINDOWS_TILE:
			nIndex = 1;
			break;
		case DRAWMODE_WINDOWS_STRETCH:
			nIndex = 2;
			break;
	}
	m_wndComboWindowsMode.SetCurSel(nIndex >= 0 ? nIndex : 0);
	GetDlgItem(IDC_COMBO_WINDOWSMODE)->EnableWindow(nIndex >= 0 ? TRUE : FALSE);

	// imposta gli spin buttons per le dimensioni del rettangolo
	CSpinButtonCtrl* pSpin;

	pSpin = (CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_AREA);
	pSpin->SetRange(1,100);
	pSpin->SetPos(m_nAreaRatio);
	pSpin->SetBuddy(GetDlgItem(IDC_EDIT_AREA));

	pSpin = (CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_WIDTH);
	pSpin->SetRange((short)m_nMinWidth,(short)m_nScreenWidth);
	pSpin->SetPos(m_nWidth);
	pSpin->SetBuddy(GetDlgItem(IDC_EDIT_WIDTH));

	pSpin = (CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_HEIGHT);
	pSpin->SetRange((short)m_nMinHeight,(short)m_nScreenHeight);
	pSpin->SetPos(m_nHeight);
	pSpin->SetBuddy(GetDlgItem(IDC_EDIT_HEIGHT));

	// carica il combo con gli effetti
	m_wndComboEffect.AddString(IDS_DRAWEFFECT_NONE);
	m_wndComboEffect.AddString(IDS_DRAWEFFECT_GRAYSCALE);
	m_wndComboEffect.AddString(IDS_DRAWEFFECT_ROTATE90LEFT);
	m_wndComboEffect.AddString(IDS_DRAWEFFECT_ROTATE90RIGHT);
	m_wndComboEffect.AddString(IDS_DRAWEFFECT_ROTATE180);
	m_wndComboEffect.AddString(IDS_DRAWEFFECT_BLUR);
	m_wndComboEffect.AddString(IDS_DRAWEFFECT_BRIGHTNESS);
	m_wndComboEffect.AddString(IDS_DRAWEFFECT_CONTRAST);
	m_wndComboEffect.AddString(IDS_DRAWEFFECT_EDGEENHANCE);
	m_wndComboEffect.AddString(IDS_DRAWEFFECT_EMBOSS);
	m_wndComboEffect.AddString(IDS_DRAWEFFECT_FINDEDGE);
	m_wndComboEffect.AddString(IDS_DRAWEFFECT_INVERT);
	m_wndComboEffect.AddString(IDS_DRAWEFFECT_MIRRORH);
	m_wndComboEffect.AddString(IDS_DRAWEFFECT_MIRRORV);
	m_wndComboEffect.AddString(IDS_DRAWEFFECT_SHARPEN);
	nIndex = -1;
	BOOL bEnableEffectValue = FALSE;
	switch(m_nDrawEffect)
	{
		case DRAWEFFECT_NONE:
			nIndex = 0;
			break;
		case DRAWEFFECT_GRAYSCALE:
			nIndex = 1;
			break;
		case DRAWEFFECT_ROTATE90LEFT:
			nIndex = 2;
			break;
		case DRAWEFFECT_ROTATE90RIGHT:
			nIndex = 3;
			break;
		case DRAWEFFECT_ROTATE180:
			nIndex = 4;
			break;
		case DRAWEFFECT_BLUR:
			bEnableEffectValue = m_ImageParams.GetBlurMin()!=0 || m_ImageParams.GetBlurMax()!=0;
			nIndex = 5;
			break;
		case DRAWEFFECT_BRIGHTNESS:
			bEnableEffectValue = m_ImageParams.GetBrightnessMin()!=0 || m_ImageParams.GetBrightnessMax()!=0;
			nIndex = 6;
			break;
		case DRAWEFFECT_CONTRAST:
			bEnableEffectValue = m_ImageParams.GetContrastMin()!=0 || m_ImageParams.GetContrastMax()!=0;
			nIndex = 7;
			break;
		case DRAWEFFECT_EDGEENHANCE:
			nIndex = 8;
			break;
		case DRAWEFFECT_EMBOSS:
			bEnableEffectValue = m_ImageParams.GetEmbossMin()!=0 || m_ImageParams.GetEmbossMax()!=0;
			nIndex = 9;
			break;
		case DRAWEFFECT_FINDEDGE:
			bEnableEffectValue = m_ImageParams.GetFindEdgeMin()!=0 || m_ImageParams.GetFindEdgeMax()!=0;
			nIndex = 10;
			break;
		case DRAWEFFECT_INVERT:
			nIndex = 11;
			break;
		case DRAWEFFECT_MIRRORH:
			nIndex = 12;
			break;
		case DRAWEFFECT_MIRRORV:
			nIndex = 13;
			break;
		case DRAWEFFECT_SHARPEN:
			bEnableEffectValue = m_ImageParams.GetSharpenMin()!=0 || m_ImageParams.GetSharpenMax()!=0;
			nIndex = 14;
			break;
		case DRAWEFFECT_RANDOM:
			nIndex = 15;
			break;
	}
	m_wndComboEffect.SetCurSel(nIndex >= 0 ? nIndex : 0);
	GetDlgItem(IDC_COMBO_EFFECT)->EnableWindow(nIndex >= 0 ? TRUE : FALSE);

	// imposta lo spin button per il valore
	pSpin = (CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_EFFECT);
	pSpin->SetRange(MIN_EFFECT_VALUE,MAX_EFFECT_VALUE);
	pSpin->SetPos(m_nDrawEffectValue);
	pSpin->SetBuddy(GetDlgItem(IDC_EDIT_EFFECT));

	// abilita i controlli
	GetDlgItem(IDC_EDIT_EFFECT)->EnableWindow(bEnableEffectValue);
	GetDlgItem(IDC_SPIN_EFFECT)->EnableWindow(bEnableEffectValue);

	// aggiunge i tooltips
	if(m_Tooltip.Create(this,TTS_ALWAYSTIP))
	{
		m_Tooltip.SetWidth(TOOLTIP_REASONABLE_WIDTH);
		m_Tooltip.SetDelay(TOOLTIP_REASONABLE_DELAYTIME);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_RADIO_WINDOWSMODE),IDC_RADIO_WINDOWSMODE,IDS_TOOLTIP_OPTIONS_DRAW_MODE);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_COMBO_WINDOWSMODE),IDC_COMBO_WINDOWSMODE,IDS_TOOLTIP_OPTIONS_DRAW_MODE);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_RADIO_OWNERMODE),IDC_RADIO_OWNERMODE,IDS_TOOLTIP_OPTIONS_DRAW_OWNER);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_COMBO_OWNERMODE),IDC_COMBO_OWNERMODE,IDS_TOOLTIP_OPTIONS_DRAW_OWNER);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_EDIT_AREA),IDC_EDIT_AREA,IDS_TOOLTIP_OPTIONS_DRAW_AREARATIO);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_SPIN_AREA),IDC_SPIN_AREA,IDS_TOOLTIP_OPTIONS_DRAW_AREARATIO);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_EDIT_WIDTH),IDC_EDIT_WIDTH,IDS_TOOLTIP_OPTIONS_DRAW_RECTWIDTH);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_SPIN_WIDTH),IDC_SPIN_WIDTH,IDS_TOOLTIP_OPTIONS_DRAW_RECTWIDTH);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_EDIT_HEIGHT),IDC_EDIT_HEIGHT,IDS_TOOLTIP_OPTIONS_DRAW_RECTHEIGHT);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_SPIN_HEIGHT),IDC_SPIN_HEIGHT,IDS_TOOLTIP_OPTIONS_DRAW_RECTHEIGHT);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_COMBO_EFFECT),IDC_COMBO_EFFECT,IDS_TOOLTIP_OPTIONS_DRAW_EFFECT);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_EDIT_EFFECT),IDC_EDIT_EFFECT,IDS_TOOLTIP_OPTIONS_DRAW_EFFECTVALUE);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_SPIN_EFFECT),IDC_SPIN_EFFECT,IDS_TOOLTIP_OPTIONS_DRAW_EFFECTVALUE);
	}

	UpdateData(FALSE);

	return(FALSE);
}

/*
	OnRadioButtonOwnerMode()
*/
void CWallPaperDrawSettingsModeDlg::OnRadioButtonOwnerMode(void)
{
	UpdateData(TRUE);

	m_nOwnerMode = 0;
	m_nWindowsMode = 1;

	GetDlgItem(IDC_COMBO_OWNERMODE)->EnableWindow(TRUE);
	GetDlgItem(IDC_COMBO_WINDOWSMODE)->EnableWindow(FALSE);

	OnComboOwnerMode();

	UpdateData(FALSE);
}

/*
	OnRadioButtonWindowsMode()
*/
void CWallPaperDrawSettingsModeDlg::OnRadioButtonWindowsMode(void)
{
	UpdateData(TRUE);

	m_nOwnerMode = 1;
	m_nWindowsMode = 0;

	GetDlgItem(IDC_COMBO_OWNERMODE)->EnableWindow(FALSE);
	GetDlgItem(IDC_COMBO_WINDOWSMODE)->EnableWindow(TRUE);

	OnComboWindowsMode();

	UpdateData(FALSE);
}

/*
	OnComboOwnerMode()
*/
void CWallPaperDrawSettingsModeDlg::OnComboOwnerMode(void)
{
	BOOL bEnableRect = FALSE;
	BOOL bEnableAreaRatio = FALSE;
	char szComboEntry[_MAX_PATH+1];
	m_wndComboOwnerMode.GetWindowText(szComboEntry,sizeof(szComboEntry));
	
	if(strcmp(szComboEntry,IDS_DRAWOWNERMODE_ORIGINAL_SIZE)==0)
		m_nDrawMode = DRAWMODE_ORIGINAL_SIZE;
	else if(strcmp(szComboEntry,IDS_DRAWOWNERMODE_FIT_TO_SCREEN)==0)
		m_nDrawMode = DRAWMODE_FIT_TO_SCREEN;
	else if(strcmp(szComboEntry,IDS_DRAWOWNERMODE_STRETCH_TO_SCREEN)==0)
		m_nDrawMode = DRAWMODE_STRETCH_TO_SCREEN;
	else if(strcmp(szComboEntry,IDS_DRAWOWNERMODE_STRETCH_TO_SCREEN_IF_DOES_NOT_FIT)==0)
		m_nDrawMode = DRAWMODE_STRETCH_IF_DOES_NOT_FIT;
	else if(strcmp(szComboEntry,IDS_DRAWOWNERMODE_STRETCH_TO_RECT)==0)
	{
		m_nDrawMode = DRAWMODE_STRETCH_TO_RECT;
		bEnableRect = TRUE;
	}
	else if(strcmp(szComboEntry,IDS_DRAWOWNERMODE_TILE_STRETCH)==0)
	{
		GetDlgItem(IDC_STATIC_AREARATIO)->SetWindowText(IDS_DRAWOWNERMODE_TILE_STRETCH_TEXT);
		m_nDrawMode = DRAWMODE_TILE_STRETCH;
		bEnableAreaRatio = TRUE;
	}
	else if(strcmp(szComboEntry,IDS_DRAWOWNERMODE_TILE_FIT)==0)
	{
		GetDlgItem(IDC_STATIC_AREARATIO)->SetWindowText(IDS_DRAWOWNERMODE_TILE_FIT_TEXT);
		m_nDrawMode = DRAWMODE_TILE_FIT;
		bEnableAreaRatio = TRUE;
	}
	else if(strcmp(szComboEntry,IDS_DRAWOWNERMODE_OVERLAP_STRETCH)==0)
	{
		GetDlgItem(IDC_STATIC_AREARATIO)->SetWindowText(IDS_DRAWOWNERMODE_OVERLAP_STRETCH_TEXT);
		m_nDrawMode = DRAWMODE_OVERLAP_STRETCH;
		bEnableAreaRatio = TRUE;
	}
	else if(strcmp(szComboEntry,IDS_DRAWOWNERMODE_OVERLAP_FIT)==0)
	{
		GetDlgItem(IDC_STATIC_AREARATIO)->SetWindowText(IDS_DRAWOWNERMODE_OVERLAP_FIT_TEXT);
		m_nDrawMode = DRAWMODE_OVERLAP_FIT;
		bEnableAreaRatio = TRUE;
	}

	GetDlgItem(IDC_EDIT_AREA)->EnableWindow(bEnableAreaRatio);
	GetDlgItem(IDC_SPIN_AREA)->EnableWindow(bEnableAreaRatio);
	GetDlgItem(IDC_EDIT_WIDTH)->EnableWindow(bEnableRect);
	GetDlgItem(IDC_SPIN_WIDTH)->EnableWindow(bEnableRect);
	GetDlgItem(IDC_EDIT_HEIGHT)->EnableWindow(bEnableRect);
	GetDlgItem(IDC_SPIN_HEIGHT)->EnableWindow(bEnableRect);
}

/*
	OnComboWindowsMode()
*/
void CWallPaperDrawSettingsModeDlg::OnComboWindowsMode(void)
{
	char szComboEntry[_MAX_PATH+1];
	m_wndComboWindowsMode.GetWindowText(szComboEntry,sizeof(szComboEntry));
	
	if(strcmp(szComboEntry,IDS_DRAWWINDOWSMODE_NORMAL)==0)
		m_nDrawMode = DRAWMODE_WINDOWS_NORMAL;
	else if(strcmp(szComboEntry,IDS_DRAWWINDOWSMODE_TILE)==0)
		m_nDrawMode = DRAWMODE_WINDOWS_TILE;
	else if(strcmp(szComboEntry,IDS_DRAWWINDOWSMODE_STRETCH)==0)
		m_nDrawMode = DRAWMODE_WINDOWS_STRETCH;

	GetDlgItem(IDC_EDIT_AREA)->EnableWindow(FALSE);
	GetDlgItem(IDC_SPIN_AREA)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_WIDTH)->EnableWindow(FALSE);
	GetDlgItem(IDC_SPIN_WIDTH)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_HEIGHT)->EnableWindow(FALSE);
	GetDlgItem(IDC_SPIN_HEIGHT)->EnableWindow(FALSE);
}

/*
	OnComboEffect()
*/
void CWallPaperDrawSettingsModeDlg::OnComboEffect(void)
{
	BOOL bEnableEffectValue = FALSE;
	char szComboEntry[_MAX_PATH+1];
	m_wndComboEffect.GetWindowText(szComboEntry,sizeof(szComboEntry));
	
	if(strcmp(szComboEntry,IDS_DRAWEFFECT_NONE)==0)
		m_nDrawEffect = DRAWEFFECT_NONE;
	else if(strcmp(szComboEntry,IDS_DRAWEFFECT_GRAYSCALE)==0)
		m_nDrawEffect = DRAWEFFECT_GRAYSCALE;
	else if(strcmp(szComboEntry,IDS_DRAWEFFECT_ROTATE90LEFT)==0)
		m_nDrawEffect = DRAWEFFECT_ROTATE90LEFT;
	else if(strcmp(szComboEntry,IDS_DRAWEFFECT_ROTATE90RIGHT)==0)
		m_nDrawEffect = DRAWEFFECT_ROTATE90RIGHT;
	else if(strcmp(szComboEntry,IDS_DRAWEFFECT_ROTATE180)==0)
		m_nDrawEffect = DRAWEFFECT_ROTATE180;
	else if(strcmp(szComboEntry,IDS_DRAWEFFECT_BLUR)==0)
	{	m_nDrawEffect = DRAWEFFECT_BLUR;
		bEnableEffectValue = m_ImageParams.GetBlurMin()!=0 || m_ImageParams.GetBlurMax()!=0;
	}
	else if(strcmp(szComboEntry,IDS_DRAWEFFECT_BRIGHTNESS)==0)
	{
		m_nDrawEffect = DRAWEFFECT_BRIGHTNESS;
		bEnableEffectValue = m_ImageParams.GetBrightnessMin()!=0 || m_ImageParams.GetBrightnessMax()!=0;
	}
	else if(strcmp(szComboEntry,IDS_DRAWEFFECT_CONTRAST)==0)
	{
		m_nDrawEffect = DRAWEFFECT_CONTRAST;
		bEnableEffectValue = m_ImageParams.GetContrastMin()!=0 || m_ImageParams.GetContrastMax()!=0;
	}
	else if(strcmp(szComboEntry,IDS_DRAWEFFECT_EDGEENHANCE)==0)
		m_nDrawEffect = DRAWEFFECT_EDGEENHANCE;
	else if(strcmp(szComboEntry,IDS_DRAWEFFECT_EMBOSS)==0)
	{
		m_nDrawEffect = DRAWEFFECT_EMBOSS;
		bEnableEffectValue = m_ImageParams.GetEmbossMin()!=0 || m_ImageParams.GetEmbossMax()!=0;
	}
	else if(strcmp(szComboEntry,IDS_DRAWEFFECT_FINDEDGE)==0)
	{
		m_nDrawEffect = DRAWEFFECT_FINDEDGE;
		bEnableEffectValue = m_ImageParams.GetFindEdgeMin()!=0 || m_ImageParams.GetFindEdgeMax()!=0;
	}
	else if(strcmp(szComboEntry,IDS_DRAWEFFECT_INVERT)==0)
		m_nDrawEffect = DRAWEFFECT_INVERT;
	else if(strcmp(szComboEntry,IDS_DRAWEFFECT_MIRRORH)==0)
		m_nDrawEffect = DRAWEFFECT_MIRRORH;
	else if(strcmp(szComboEntry,IDS_DRAWEFFECT_MIRRORV)==0)
		m_nDrawEffect = DRAWEFFECT_MIRRORV;
	else if(strcmp(szComboEntry,IDS_DRAWEFFECT_SHARPEN)==0)
	{
		m_nDrawEffect = DRAWEFFECT_SHARPEN;
		bEnableEffectValue = m_ImageParams.GetSharpenMin()!=0 || m_ImageParams.GetSharpenMax()!=0;
	}
	else if(strcmp(szComboEntry,IDS_DRAWEFFECT_RANDOM)==0)
		m_nDrawEffect = DRAWEFFECT_RANDOM;
	else
		m_nDrawEffect = DRAWEFFECT_NONE;

	// abilita i controlli
	GetDlgItem(IDC_EDIT_EFFECT)->EnableWindow(bEnableEffectValue);
	GetDlgItem(IDC_SPIN_EFFECT)->EnableWindow(bEnableEffectValue);
}

/*
	OnEnChangeEffect()
*/
void CWallPaperDrawSettingsModeDlg::OnEnChangeEffect(void)
{
	CString strValue;
	GetDlgItemText(IDC_EDIT_EFFECT,strValue);
	int nValue = atoi(strValue);

	if(nValue < MIN_EFFECT_VALUE || nValue > MAX_EFFECT_VALUE)
	{
		strValue.Format("%d",MIN_EFFECT_VALUE);
		m_nDrawEffectValue = MIN_EFFECT_VALUE;
		UpdateData(FALSE);
	}
}

/*
	OnVScroll()
*/
void CWallPaperDrawSettingsModeDlg::OnVScroll(UINT nSBCode,UINT nPos,CScrollBar* pScrollBar)
{
	if(nSBCode!=SB_ENDSCROLL)
	{
		CString strValue;
		strValue.Format("%d",(int)nPos);
		((CSpinButtonCtrl*)pScrollBar)->GetBuddy()->SetWindowText(strValue);
		((CSpinButtonCtrl*)pScrollBar)->SetPos((int)nPos);
		UpdateData(TRUE);
	}
}
