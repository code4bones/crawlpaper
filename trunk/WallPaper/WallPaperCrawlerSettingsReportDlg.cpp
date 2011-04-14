/*
	WallPaperCrawlerSettingsReportDlg.cpp
	Dialogo per la pagina relativa alle opzioni per il crawler.
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
#include <stdio.h>
#include "strings.h"
#include "window.h"
#include "CToolTipCtrlEx.h"
#include "WallPaperConfig.h"
#include "WallPaperCrawlerSettingsReportDlg.h"
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

BEGIN_MESSAGE_MAP(CWallPaperCrawlerSettingsReportDlg,CPropertyPage)
	ON_CBN_KILLFOCUS(IDC_COMBO_SORTEDBY,OnKillFocusComboSortedBy)
	ON_BN_CLICKED(IDC_CHECK_GENERATEASCII,OnCheckGenerateAscii)
	ON_BN_CLICKED(IDC_CHECK_GENERATEHTML,OnCheckGenerateHtml)
	ON_BN_CLICKED(IDC_CHECK_GENERATETHUMBNAILS,OnCheckGenerateThumbnails)
	ON_BN_CLICKED(IDC_CHECK_DUMPDATABASE,OnCheckDumpDatabase)
	ON_BN_CLICKED(IDC_CHECK_EXTRACTTEXT,OnCheckExtractText)
	ON_BN_CLICKED(IDC_CHECK_EXTRACTJAVASCRIPT,OnCheckExtractJavascript)
END_MESSAGE_MAP()

IMPLEMENT_DYNCREATE(CWallPaperCrawlerSettingsReportDlg,CPropertyPage)

/*
	DoDataExchange()
*/
void CWallPaperCrawlerSettingsReportDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Check(pDX,IDC_CHECK_GENERATEASCII,m_bGenerateAscii);
	DDX_Check(pDX,IDC_CHECK_GENERATEHTML,m_bGenerateHtml);
	DDX_Check(pDX,IDC_CHECK_GENERATETHUMBNAILS,m_bGenerateThumbnails);
	DDX_Check(pDX,IDC_CHECK_DUMPDATABASE,m_bDumpDatabase);
	DDX_Check(pDX,IDC_CHECK_EXTRACTTEXT,m_bExtractText);
	DDX_Check(pDX,IDC_CHECK_EXTRACTJAVASCRIPT,m_bExtractJavascript);
	DDX_Control(pDX,IDC_COMBO_SORTEDBY,m_wndComboSortedBy);
}

/*
	CreateEx()
*/
BOOL CWallPaperCrawlerSettingsReportDlg::CreateEx(CWnd* pParent,UINT nID,CWallPaperConfig* pConfig)
{
	// configurazione corrente
	m_pConfig = pConfig;

	// imposta con i valori della configurazione
	m_nSortedBy = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_REPORTORDER_KEY);
	m_bGenerateAscii = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_GENERATEASCII_KEY);
	m_bGenerateHtml = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_GENERATEREPORT_KEY);
	m_bGenerateThumbnails = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_GENERATETHUMBNAILS_KEY);
	m_bDumpDatabase = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_DUMPDATABASE_KEY);
	m_bExtractText = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_EXTRACTTEXT_KEY);
	m_bExtractJavascript = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_EXTRACTJAVASCRIPT_KEY);

	// crea il dialogo
	return(Create(nID,pParent));
}

/*
	OnInitDialog()
*/
BOOL CWallPaperCrawlerSettingsReportDlg::OnInitDialog(void)
{
	// classe base
	CPropertyPage::OnInitDialog();

	// abilita i controlli
	GetDlgItem(IDC_COMBO_SORTEDBY)->EnableWindow(m_bGenerateHtml);
	GetDlgItem(IDC_CHECK_GENERATETHUMBNAILS)->EnableWindow(m_bGenerateHtml);
	GetDlgItem(IDC_CHECK_DUMPDATABASE)->EnableWindow(m_bGenerateHtml);

	// carica il combo con i valori per l'ordinamento del report
	m_wndComboSortedBy.AddString(IDS_SORTEDBY_ID);
	m_wndComboSortedBy.AddString(IDS_SORTEDBY_PARENTURL);
	m_wndComboSortedBy.AddString(IDS_SORTEDBY_URL);
	m_wndComboSortedBy.AddString(IDS_SORTEDBY_STATUS);
	int nIndex = 0;
	switch(m_nSortedBy)
	{
		case REPORT_BY_ID:
			nIndex = 0;
			break;
		case REPORT_BY_PARENT_URL:
			nIndex = 1;
			break;
		case REPORT_BY_URL:
			nIndex = 2;
			break;
		case REPORT_BY_STAT:
			nIndex = 3;
			break;
	}
	m_wndComboSortedBy.SetCurSel(nIndex);

	// aggiunge i tooltips
	if(m_Tooltip.Create(this,TTS_ALWAYSTIP))
	{
		m_Tooltip.SetWidth(TOOLTIP_REASONABLE_WIDTH);
		m_Tooltip.SetDelay(TOOLTIP_REASONABLE_DELAYTIME);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_CHECK_GENERATEASCII),IDC_CHECK_GENERATEASCII,IDS_TOOLTIP_OPTIONS_REPORT_ASCII);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_CHECK_GENERATEHTML),IDC_CHECK_GENERATEHTML,IDS_TOOLTIP_OPTIONS_REPORT_HTML);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_COMBO_SORTEDBY),IDC_COMBO_SORTEDBY,IDS_TOOLTIP_OPTIONS_REPORT_SORT);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_CHECK_GENERATETHUMBNAILS),IDC_CHECK_GENERATETHUMBNAILS,IDS_TOOLTIP_OPTIONS_REPORT_THUMB);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_CHECK_DUMPDATABASE),IDC_CHECK_DUMPDATABASE,IDS_TOOLTIP_OPTIONS_REPORT_DUMP);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_CHECK_EXTRACTTEXT),IDC_CHECK_EXTRACTTEXT,IDS_TOOLTIP_OPTIONS_REPORT_TEXT);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_CHECK_EXTRACTJAVASCRIPT),IDC_CHECK_EXTRACTJAVASCRIPT,IDS_TOOLTIP_OPTIONS_REPORT_JAVASCRIPT);
	}

	UpdateData(FALSE);

	return(FALSE);
}

/*
	OnSetActive()
*/
BOOL CWallPaperCrawlerSettingsReportDlg::OnSetActive(void)
{
	return(TRUE);
}

/*
	OnCheckGenerateAscii()
*/
void CWallPaperCrawlerSettingsReportDlg::OnCheckGenerateAscii(void)
{
	m_bGenerateAscii = !m_bGenerateAscii;
}

/*
	OnCheckGenerateHtml()
*/
void CWallPaperCrawlerSettingsReportDlg::OnCheckGenerateHtml(void)
{
	m_bGenerateHtml = !m_bGenerateHtml;

	GetDlgItem(IDC_COMBO_SORTEDBY)->EnableWindow(m_bGenerateHtml);
	GetDlgItem(IDC_CHECK_GENERATETHUMBNAILS)->EnableWindow(m_bGenerateHtml);
	GetDlgItem(IDC_CHECK_DUMPDATABASE)->EnableWindow(m_bGenerateHtml);
}

/*
	OnCheckGenerateThumbnails()
*/
void CWallPaperCrawlerSettingsReportDlg::OnCheckGenerateThumbnails(void)
{
	m_bGenerateThumbnails = !m_bGenerateThumbnails;
}

/*
	OnCheckDumpDatabase()
*/
void CWallPaperCrawlerSettingsReportDlg::OnCheckDumpDatabase(void)
{
	m_bDumpDatabase = !m_bDumpDatabase;
}

/*
	OnCheckExtractText()
*/
void CWallPaperCrawlerSettingsReportDlg::OnCheckExtractText(void)
{
	m_bExtractText = !m_bExtractText;
}

/*
	OnCheckExtractJavascript()
*/
void CWallPaperCrawlerSettingsReportDlg::OnCheckExtractJavascript(void)
{
	m_bExtractJavascript = !m_bExtractJavascript;
}

/*
	OnKillFocusComboSortedBy()
*/
void CWallPaperCrawlerSettingsReportDlg::OnKillFocusComboSortedBy(void)
{
	// ricava la selezione corrente del combo
	char szComboEntry[_MAX_PATH+1];
	m_wndComboSortedBy.GetWindowText(szComboEntry,sizeof(szComboEntry));

	if(strcmp(szComboEntry,IDS_SORTEDBY_ID)==0)
		m_nSortedBy = REPORT_BY_ID;
	else if(strcmp(szComboEntry,IDS_SORTEDBY_PARENTURL)==0)
		m_nSortedBy = REPORT_BY_PARENT_URL;
	else if(strcmp(szComboEntry,IDS_SORTEDBY_URL)==0)
		m_nSortedBy = REPORT_BY_URL;
	else if(strcmp(szComboEntry,IDS_SORTEDBY_STATUS)==0)
		m_nSortedBy = REPORT_BY_STAT;
	else
		m_nSortedBy = REPORT_BY_ID;
}
