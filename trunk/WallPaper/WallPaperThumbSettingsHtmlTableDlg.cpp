/*
	WallPaperThumbSettingsHtmlTableDlg.cpp
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
#include "window.h"
#include "CToolTipCtrlEx.h"
#include "WallPaperConfig.h"
#include "WallPaperThumbSettingsHtmlTableDlg.h"
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

BEGIN_MESSAGE_MAP(CWallPaperThumbSettingsHtmlTableDlg,CPropertyPage)
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDC_CHECK_GENERATEHTML,OnCheckGenerateHtml)
END_MESSAGE_MAP()

IMPLEMENT_DYNCREATE(CWallPaperThumbSettingsHtmlTableDlg,CPropertyPage)

/*
	DoDataExchange()
*/
void CWallPaperThumbSettingsHtmlTableDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Text(pDX,IDC_EDIT_TABLETITLE,m_strTitle);
	DDX_Text(pDX,IDC_EDIT_TABLEWIDTH,m_nWidth);
	DDX_Text(pDX,IDC_EDIT_TABLECOLS,m_nCols);
	DDX_Text(pDX,IDC_EDIT_TABLEBORDER,m_nBorder);
	DDX_Text(pDX,IDC_EDIT_TABLESPACING,m_nSpacing);
	DDX_Text(pDX,IDC_EDIT_TABLEPADDING,m_nPadding);
	DDX_Check(pDX,IDC_CHECK_GENERATEHTML,m_bGenerateHtml);
}

/*
	CreateEx()
*/
BOOL CWallPaperThumbSettingsHtmlTableDlg::CreateEx(CWnd* pParent,UINT nID,CWallPaperConfig* pConfig)
{
	// configurazione corrente
	m_pConfig = pConfig;

	// imposta i campi del dialogo con i valori della configurazione
	m_strTitle = m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_TABLETITLE_KEY);
	m_nWidth   = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_TABLEWIDTH_KEY);
	m_nCols    = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_TABLECOLS_KEY);
	m_nBorder  = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_TABLEBORDER_KEY);
	m_nSpacing = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_TABLESPACING_KEY);
	m_nPadding = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_TABLEPADDING_KEY);

	// crea il dialogo
	return(Create(nID,pParent));
}

/*
	OnInitDialog()
*/
BOOL CWallPaperThumbSettingsHtmlTableDlg::OnInitDialog(void)
{
	// classe base
	CPropertyPage::OnInitDialog();

	// imposta gli spin per le dimensioni
	CSpinButtonCtrl* pSpin;

	pSpin = (CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_TABLEWIDTH);
	pSpin->SetRange(MIN_THUMBNAILS_TABLE_WIDTH,MAX_THUMBNAILS_TABLE_WIDTH);
	pSpin->SetPos(m_nWidth);
	pSpin->SetBuddy(GetDlgItem(IDC_EDIT_TABLEWIDTH));
	
	pSpin = (CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_TABLECOLS);
	pSpin->SetRange(MIN_THUMBNAILS_COLS,MAX_THUMBNAILS_COLS);
	pSpin->SetPos(m_nCols);
	pSpin->SetBuddy(GetDlgItem(IDC_EDIT_TABLECOLS));
	
	pSpin = (CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_TABLEBORDER);
	pSpin->SetRange(0,100);
	pSpin->SetPos(m_nBorder);
	pSpin->SetBuddy(GetDlgItem(IDC_EDIT_TABLEBORDER));
	
	pSpin = (CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_TABLESPACING);
	pSpin->SetRange(0,100);
	pSpin->SetPos(m_nSpacing);
	pSpin->SetBuddy(GetDlgItem(IDC_EDIT_TABLESPACING));
	
	pSpin = (CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_TABLEPADDING);
	pSpin->SetRange(0,100);
	pSpin->SetPos(m_nPadding);
	pSpin->SetBuddy(GetDlgItem(IDC_EDIT_TABLEPADDING));

	// aggiorna i campi del dialogo
	OnSetActive();
	
	// aggiunge i tooltips
	if(m_Tooltip.Create(this,TTS_ALWAYSTIP))
	{
		m_Tooltip.SetWidth(TOOLTIP_REASONABLE_WIDTH);
		m_Tooltip.SetDelay(TOOLTIP_REASONABLE_DELAYTIME);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_EDIT_TABLETITLE),IDC_EDIT_TABLETITLE,IDS_TOOLTIP_OPTIONS_THUMB_HTMLTITLE);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_EDIT_TABLEWIDTH),IDC_EDIT_TABLEWIDTH,IDS_TOOLTIP_OPTIONS_THUMB_HTMLWIDTH);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_SPIN_TABLEWIDTH),IDC_SPIN_TABLEWIDTH,IDS_TOOLTIP_OPTIONS_THUMB_HTMLWIDTH);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_EDIT_TABLECOLS),IDC_EDIT_TABLECOLS,IDS_TOOLTIP_OPTIONS_THUMB_HTMLCOLS);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_SPIN_TABLECOLS),IDC_SPIN_TABLECOLS,IDS_TOOLTIP_OPTIONS_THUMB_HTMLCOLS);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_EDIT_TABLEBORDER),IDC_EDIT_TABLEBORDER,IDS_TOOLTIP_OPTIONS_THUMB_PIXBORDER);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_SPIN_TABLEBORDER),IDC_SPIN_TABLEBORDER,IDS_TOOLTIP_OPTIONS_THUMB_PIXBORDER);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_EDIT_TABLEPADDING),IDC_EDIT_TABLEPADDING,IDS_TOOLTIP_OPTIONS_THUMB_PIXPADDING);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_SPIN_TABLEPADDING),IDC_SPIN_TABLEPADDING,IDS_TOOLTIP_OPTIONS_THUMB_PIXPADDING);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_EDIT_TABLESPACING),IDC_EDIT_TABLESPACING,IDS_TOOLTIP_OPTIONS_THUMB_PIXSPACING);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_SPIN_TABLESPACING),IDC_SPIN_TABLESPACING,IDS_TOOLTIP_OPTIONS_THUMB_PIXSPACING);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_CHECK_GENERATEHTML),IDC_CHECK_GENERATEHTML,IDS_TOOLTIP_OPTIONS_THUMB_GENERATEHTML);
	}
	
	return(FALSE);
}

/*
	OnSetActive()
*/
BOOL CWallPaperThumbSettingsHtmlTableDlg::OnSetActive(void)
{
	// abilit i controlli
	GetDlgItem(IDC_EDIT_TABLETITLE)->EnableWindow(m_bGenerateHtml);
	GetDlgItem(IDC_EDIT_TABLEWIDTH)->EnableWindow(m_bGenerateHtml);
	GetDlgItem(IDC_EDIT_TABLECOLS)->EnableWindow(m_bGenerateHtml);
	GetDlgItem(IDC_EDIT_TABLEBORDER)->EnableWindow(m_bGenerateHtml);
	GetDlgItem(IDC_EDIT_TABLESPACING)->EnableWindow(m_bGenerateHtml);
	GetDlgItem(IDC_EDIT_TABLEPADDING)->EnableWindow(m_bGenerateHtml);
	UpdateData(FALSE);

	return(TRUE);
}

/*
	OnVScroll()
*/
void CWallPaperThumbSettingsHtmlTableDlg::OnVScroll(UINT nSBCode,UINT nPos,CScrollBar* pScrollBar)
{
	// per aggiornare il valore del campo quando cambia la posizione dello spin
	if(nSBCode!=SB_ENDSCROLL)
	{
		CString strValue;
		strValue.Format("%d",(int)nPos);
		((CSpinButtonCtrl*)pScrollBar)->GetBuddy()->SetWindowText(strValue);
		((CSpinButtonCtrl*)pScrollBar)->SetPos((int)nPos);
	}
}

/*
	OnCheckGenerateHtml()
*/
void CWallPaperThumbSettingsHtmlTableDlg::OnCheckGenerateHtml(void)
{
	m_bGenerateHtml = !m_bGenerateHtml;

	OnSetActive();
}
