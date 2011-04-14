/*
	WallPaperCrawlerSettingsDownDlg.cpp
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
#include "window.h"
#include "CToolTipCtrlEx.h"
#include "CUrl.h"
#include "CUrlDatabaseService.h"
#include "CUrlHistoryService.h"
#include "WallPaperConfig.h"
#include "WallPaperCrawlerSettingsDownDlg.h"
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

BEGIN_MESSAGE_MAP(CWallPaperCrawlerSettingsDownDlg,CPropertyPage)
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDC_CHECK_ADDDOWNLOADS,OnCheckAddDownloads)
	ON_BN_CLICKED(IDC_CHECK_PAGECONTENT,OnCheckPageContent)
	ON_BN_CLICKED(IDC_CHECK_MINSIZE,OnCheckMinSize)
	ON_BN_CLICKED(IDC_CHECK_MAXSIZE,OnCheckMaxSize)
	ON_BN_CLICKED(IDC_CHECK_SKIPEXISTING,OnCheckSkipExisting)
	ON_BN_CLICKED(IDC_CHECK_SKIPEXISTING_FORCE,OnCheckForceSkipExisting)
	ON_BN_CLICKED(IDC_CHECK_SKIPEXISTING_DONT,OnCheckDoNotSkipExisting)
	ON_BN_CLICKED(IDC_CHECK_BEQUIETINSCRIPTMODE,OnCheckBeQuiet)
	ON_BN_CLICKED(IDC_CHECK_FOLLOWCGILINKS,OnCheckFollowCgi)
	ON_BN_CLICKED(IDC_CHECK_FOLLOWJAVASCRIPTLINKS,OnCheckFollowJavascript)
	ON_BN_CLICKED(IDC_CHECK_ROBOTS_TXT,OnCheckUseRobotsTxt)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR_HISTORY,OnButtonClearHistory)
	ON_EN_CHANGE(IDC_EDIT_MINSIZE,OnEnChangeMinSize)
	ON_EN_CHANGE(IDC_EDIT_MAXSIZE,OnEnChangeMaxSize)
	ON_CBN_SELCHANGE(IDC_COMBO_MINSIZE,OnSelChangeComboMinSize)
	ON_CBN_SELCHANGE(IDC_COMBO_MAXSIZE,OnSelChangeComboMaxSize)
END_MESSAGE_MAP()

IMPLEMENT_DYNCREATE(CWallPaperCrawlerSettingsDownDlg,CPropertyPage)

/*
	DoDataExchange()
*/
void CWallPaperCrawlerSettingsDownDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Text(pDX,IDC_EDIT_MINSIZE,m_nMinSize);
	DDX_Text(pDX,IDC_EDIT_MAXSIZE,m_nMaxSize);
	DDX_Check(pDX,IDC_CHECK_ADDDOWNLOADS,m_bAddDownloads);
	DDX_Check(pDX,IDC_CHECK_PAGECONTENT,m_bDownloadPageContent);
	DDX_Check(pDX,IDC_CHECK_MINSIZE,m_bMinSize);
	DDX_Check(pDX,IDC_CHECK_MAXSIZE,m_bMaxSize);
	DDX_Check(pDX,IDC_CHECK_SKIPEXISTING,m_bSkipExisting);
	DDX_Check(pDX,IDC_CHECK_SKIPEXISTING_FORCE,m_bForceSkipExisting);
	DDX_Check(pDX,IDC_CHECK_SKIPEXISTING_DONT,m_bDoNotSkipExisting);
	DDX_Check(pDX,IDC_CHECK_BEQUIETINSCRIPTMODE,m_bBeQuiet);
	DDX_Check(pDX,IDC_CHECK_FOLLOWCGILINKS,m_bFollowCgi);
	DDX_Check(pDX,IDC_CHECK_FOLLOWJAVASCRIPTLINKS,m_bFollowJavascript);
	DDX_Check(pDX,IDC_CHECK_ROBOTS_TXT,m_bUseRobotsTxt);
	DDX_Control(pDX,IDC_COMBO_MINSIZE,m_wndComboMinSize);
	DDX_Control(pDX,IDC_COMBO_MAXSIZE,m_wndComboMaxSize);
}

/*
	CreateEx()
*/
BOOL CWallPaperCrawlerSettingsDownDlg::CreateEx(CWnd* pParent,UINT nID,CWallPaperConfig* pConfig)
{
	// configurazione corrente
	m_pConfig = pConfig;

	// imposta con i valori della configurazione
	m_bAddDownloads = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_ADDPICTURES_KEY);
	m_bDownloadPageContent = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_ONLYCONTENT_KEY);
	m_nMinSizeType = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_MINSIZETYPE_KEY);
	m_nMinSize = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_MINSIZE_KEY);
	m_nMaxSizeType = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_MAXSIZETYPE_KEY);
	m_nMaxSize = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_MAXSIZE_KEY);
	m_bMinSize = m_nMinSize > 0;
	m_bMaxSize = m_nMaxSize > 0;
	m_bSkipExisting = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_SKIPEXISTING_KEY);
	m_bForceSkipExisting = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_SKIPEXISTING_FORCE_KEY);
	m_bDoNotSkipExisting = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_SKIPEXISTING_DONT_KEY);
	m_bBeQuiet = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_QUIETINSCRIPTMODE_KEY);
	m_bFollowCgi = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_ENABLECGI_KEY);
	m_bFollowJavascript = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_ENABLEJAVASCRIPT_KEY);
	m_bUseRobotsTxt = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_ENABLEROBOTSTXT_KEY);

	// crea il dialogo
	return(Create(nID,pParent));
}

/*
	OnInitDialog()
*/
BOOL CWallPaperCrawlerSettingsDownDlg::OnInitDialog(void)
{
	// classe base
	CPropertyPage::OnInitDialog();

	// imposta gli spin buttons
	CSpinButtonCtrl* pSpin;

	pSpin = (CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_MINSIZE);
	pSpin->SetRange(0,UD_MAXVAL);
	pSpin->SetPos(m_nMinSize);
	pSpin->SetBuddy(GetDlgItem(IDC_EDIT_MINSIZE));

	pSpin = (CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_MAXSIZE);
	pSpin->SetRange(0,UD_MAXVAL);
	pSpin->SetPos(m_nMaxSize);
	pSpin->SetBuddy(GetDlgItem(IDC_EDIT_MAXSIZE));

	// crea i combo
	m_wndComboMinSize.AddString("Bytes");
	m_wndComboMinSize.AddString("KBytes");
	m_wndComboMinSize.AddString("MBytes");

	m_wndComboMaxSize.AddString("Bytes");
	m_wndComboMaxSize.AddString("KBytes");
	m_wndComboMaxSize.AddString("MBytes");

	int nIndex = 0;
	switch(m_nMinSizeType)
	{
		case SIZETYPE_BYTES:
			nIndex = 0;
			break;
		case SIZETYPE_KBYTES:
			nIndex = 1;
			break;
		case SIZETYPE_MBYTES:
			nIndex = 2;
			break;
	}
	m_wndComboMinSize.SetCurSel(nIndex);

	nIndex = 0;
	switch(m_nMaxSizeType)
	{
		case SIZETYPE_BYTES:
			nIndex = 0;
			break;
		case SIZETYPE_KBYTES:
			nIndex = 1;
			break;
		case SIZETYPE_MBYTES:
			nIndex = 2;
			break;
	}
	m_wndComboMaxSize.SetCurSel(nIndex);

	OnSelChangeComboMinSize();
	OnSelChangeComboMaxSize();

	// abilita i controlli
	GetDlgItem(IDC_EDIT_MINSIZE)->EnableWindow(m_bMinSize);
	GetDlgItem(IDC_SPIN_MINSIZE)->EnableWindow(m_bMinSize);
	GetDlgItem(IDC_COMBO_MINSIZE)->EnableWindow(m_bMinSize);
	GetDlgItem(IDC_EDIT_MAXSIZE)->EnableWindow(m_bMaxSize);
	GetDlgItem(IDC_SPIN_MAXSIZE)->EnableWindow(m_bMaxSize);
	GetDlgItem(IDC_COMBO_MAXSIZE)->EnableWindow(m_bMaxSize);

	// aggiunge i tooltips
	if(m_Tooltip.Create(this,TTS_ALWAYSTIP))
	{
		m_Tooltip.SetWidth(TOOLTIP_REASONABLE_WIDTH);
		m_Tooltip.SetDelay(TOOLTIP_REASONABLE_DELAYTIME);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_CHECK_ADDDOWNLOADS),IDC_CHECK_ADDDOWNLOADS,IDS_TOOLTIP_OPTIONS_DOWN_ADD_DOWNLOAD);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_CHECK_PAGECONTENT),IDC_CHECK_PAGECONTENT,IDS_TOOLTIP_OPTIONS_DOWN_PAGE_CONTENT);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_CHECK_MINSIZE),IDC_CHECK_MINSIZE,IDS_TOOLTIP_OPTIONS_DOWN_MIN_SIZE);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_EDIT_MINSIZE),IDC_EDIT_MINSIZE,IDS_TOOLTIP_OPTIONS_DOWN_MIN_SIZE);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_SPIN_MINSIZE),IDC_SPIN_MINSIZE,IDS_TOOLTIP_OPTIONS_DOWN_MIN_SIZE);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_COMBO_MINSIZE),IDC_COMBO_MINSIZE,IDS_TOOLTIP_OPTIONS_DOWN_MIN_SIZE);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_CHECK_MAXSIZE),IDC_CHECK_MAXSIZE,IDS_TOOLTIP_OPTIONS_DOWN_MAX_SIZE);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_EDIT_MAXSIZE),IDC_EDIT_MAXSIZE,IDS_TOOLTIP_OPTIONS_DOWN_MAX_SIZE);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_SPIN_MAXSIZE),IDC_SPIN_MAXSIZE,IDS_TOOLTIP_OPTIONS_DOWN_MAX_SIZE);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_COMBO_MAXSIZE),IDC_COMBO_MAXSIZE,IDS_TOOLTIP_OPTIONS_DOWN_MAX_SIZE);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_CHECK_SKIPEXISTING),IDC_CHECK_SKIPEXISTING,IDS_TOOLTIP_OPTIONS_DOWN_SKIP_EXISTING);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_CHECK_SKIPEXISTING_FORCE),IDC_CHECK_SKIPEXISTING_FORCE,IDS_TOOLTIP_OPTIONS_DOWN_FORCE_SKIP_EXISTING);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_CHECK_SKIPEXISTING_DONT),IDC_CHECK_SKIPEXISTING_DONT,IDS_TOOLTIP_OPTIONS_DOWN_DONT_SKIP_EXISTING);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_CHECK_BEQUIETINSCRIPTMODE),IDC_CHECK_BEQUIETINSCRIPTMODE,IDS_TOOLTIP_OPTIONS_DOWN_BE_QUIET);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_CHECK_FOLLOWCGILINKS),IDC_CHECK_FOLLOWCGILINKS,IDS_TOOLTIP_OPTIONS_DOWN_CGI);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_CHECK_FOLLOWJAVASCRIPTLINKS),IDC_CHECK_FOLLOWJAVASCRIPTLINKS,IDS_TOOLTIP_OPTIONS_DOWN_JAVASCRIPT);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_CHECK_ROBOTS_TXT),IDC_CHECK_ROBOTS_TXT,IDS_TOOLTIP_OPTIONS_DOWN_ROBOT_TXT);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_BUTTON_CLEAR_HISTORY),IDC_BUTTON_CLEAR_HISTORY,IDS_TOOLTIP_OPTIONS_CLEAR_HISTORY);
	}
	
	UpdateData(FALSE);

	return(FALSE);
}

/*
	OnCheckAddDownloads()
*/
void CWallPaperCrawlerSettingsDownDlg::OnCheckAddDownloads(void)
{
	m_bAddDownloads = !m_bAddDownloads;
}

/*
	OnCheckPageContent()
*/
void CWallPaperCrawlerSettingsDownDlg::OnCheckPageContent(void)
{
	m_bDownloadPageContent = !m_bDownloadPageContent;
}

/*
	OnCheckMinSize()
*/
void CWallPaperCrawlerSettingsDownDlg::OnCheckMinSize(void)
{
	m_bMinSize = !m_bMinSize;

	if(!m_bMinSize)
	{
		m_nMinSize = 0;
		UpdateData(FALSE);
	}

	GetDlgItem(IDC_EDIT_MINSIZE)->EnableWindow(m_bMinSize);
	GetDlgItem(IDC_SPIN_MINSIZE)->EnableWindow(m_bMinSize);
	GetDlgItem(IDC_COMBO_MINSIZE)->EnableWindow(m_bMinSize);
	OnSelChangeComboMinSize();
	OnSelChangeComboMaxSize();
}

/*
	OnCheckMaxSize()
*/
void CWallPaperCrawlerSettingsDownDlg::OnCheckMaxSize(void)
{
	m_bMaxSize = !m_bMaxSize;

	if(!m_bMaxSize)
	{
		m_nMaxSize = 0;
		UpdateData(FALSE);
	}
	
	GetDlgItem(IDC_EDIT_MAXSIZE)->EnableWindow(m_bMaxSize);
	GetDlgItem(IDC_SPIN_MAXSIZE)->EnableWindow(m_bMaxSize);
	GetDlgItem(IDC_COMBO_MAXSIZE)->EnableWindow(m_bMinSize);
	OnSelChangeComboMinSize();
	OnSelChangeComboMaxSize();
}

/*
	OnCheckSkipExisting()
*/
void CWallPaperCrawlerSettingsDownDlg::OnCheckSkipExisting(void)
{
	m_bSkipExisting = !m_bSkipExisting;
	if(m_bSkipExisting)
		m_bDoNotSkipExisting = m_bForceSkipExisting = FALSE;
	else
		m_bDoNotSkipExisting = TRUE,m_bForceSkipExisting = FALSE;
	UpdateData(FALSE);
}

/*
	OnCheckForceSkipExisting()
*/
void CWallPaperCrawlerSettingsDownDlg::OnCheckForceSkipExisting(void)
{
	m_bForceSkipExisting = !m_bForceSkipExisting;
	if(m_bForceSkipExisting)
		m_bDoNotSkipExisting = m_bSkipExisting = FALSE;
	else
		m_bDoNotSkipExisting = TRUE,m_bSkipExisting = FALSE;
	UpdateData(FALSE);
}

/*
	OnCheckDoNotSkipExisting()
*/
void CWallPaperCrawlerSettingsDownDlg::OnCheckDoNotSkipExisting(void)
{
	m_bDoNotSkipExisting = !m_bDoNotSkipExisting;
	if(m_bDoNotSkipExisting)
		m_bSkipExisting = m_bForceSkipExisting = FALSE;
	else
		m_bSkipExisting = TRUE,m_bForceSkipExisting = FALSE;
	UpdateData(FALSE);
}

/*
	OnCheckBeQuiet()
*/
void CWallPaperCrawlerSettingsDownDlg::OnCheckBeQuiet(void)
{
	m_bBeQuiet = !m_bBeQuiet;
}

/*
	OnCheckFollowCgi()
*/
void CWallPaperCrawlerSettingsDownDlg::OnCheckFollowCgi(void)
{
	m_bFollowCgi = !m_bFollowCgi;
}

/*
	OnCheckFollowJavascript()
*/
void CWallPaperCrawlerSettingsDownDlg::OnCheckFollowJavascript(void)
{
	m_bFollowJavascript = !m_bFollowJavascript;
}

/*
	OnCheckUseRobotsTxt()
*/
void CWallPaperCrawlerSettingsDownDlg::OnCheckUseRobotsTxt(void)
{
	m_bUseRobotsTxt = !m_bUseRobotsTxt;
}

/*
	OnButtonClearHistory()
*/
void CWallPaperCrawlerSettingsDownDlg::OnButtonClearHistory(void)
{
	// azzera il database
	CUrlHistoryService UrlHistoryService(URL_HISTORY_TABLE,m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_DATADIR_KEY));
	if(UrlHistoryService.IsValid())
	{
		CUrlHistoryTable* pUrlHistory = UrlHistoryService.GetTable();
		if(pUrlHistory)
			if(pUrlHistory->Lock())
			{
				pUrlHistory->Zap();
				pUrlHistory->Unlock();
			}
	}

	// azzera il campo per l'ultima url
	m_pConfig->UpdateString(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_ADDURL_KEY,"");
	m_pConfig->SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_ADDURL_KEY);
}

/*
	OnEnChangeMinSize()
*/
void CWallPaperCrawlerSettingsDownDlg::OnEnChangeMinSize(void)
{
	CString strBytes;
	CString strValue;
	GetDlgItemText(IDC_EDIT_MINSIZE,strValue);
	
	if(atol(strValue) > UD_MAXVAL)
	{
		strValue.Format("%ld",UD_MAXVAL);
		m_nMinSize = UD_MAXVAL;
		UpdateData(FALSE);
	}

	if(m_nMinSizeType==SIZETYPE_BYTES)
		strBytes.Format("(%0.2f KB)",atof(strValue) / 1024.0f);
	else if(m_nMinSizeType==SIZETYPE_KBYTES)
		strBytes.Format("(%0.2f MB)",atof(strValue) / 1024.0f);
	else if(m_nMinSizeType==SIZETYPE_MBYTES)
		strBytes.Format("(%0.2f GB)",atof(strValue) / 1024.0f);
	else
		strBytes = "";

	SetDlgItemText(IDC_STATIC_MINSIZE,strBytes);
}

/*
	OnEnChangeMaxSize()
*/
void CWallPaperCrawlerSettingsDownDlg::OnEnChangeMaxSize(void)
{
	CString strBytes;
	CString strValue;
	GetDlgItemText(IDC_EDIT_MAXSIZE,strValue);
	
	if(atol(strValue) > UD_MAXVAL)
	{
		strValue.Format("%ld",UD_MAXVAL);
		m_nMaxSize = UD_MAXVAL;
		UpdateData(FALSE);
	}
	
	if(m_nMaxSizeType==SIZETYPE_BYTES)
		strBytes.Format("(%0.2f KB)",atof(strValue) / 1024.0f);
	else if(m_nMaxSizeType==SIZETYPE_KBYTES)
		strBytes.Format("(%0.2f MB)",atof(strValue) / 1024.0f);
	else if(m_nMaxSizeType==SIZETYPE_MBYTES)
		strBytes.Format("(%0.2f GB)",atof(strValue) / 1024.0f);
	else
		strBytes = "";

	SetDlgItemText(IDC_STATIC_MAXSIZE,strBytes);
}

/*
	OnSelChangeComboMinSize()
*/
void CWallPaperCrawlerSettingsDownDlg::OnSelChangeComboMinSize(void)
{
	// ricava la selezione corrente
	CString strValue;
	m_wndComboMinSize.GetLBText(m_wndComboMinSize.GetCurSel(),strValue);
	
	if(stricmp(strValue,"Bytes")==0)
		m_nMinSizeType = SIZETYPE_BYTES;
	else if(stricmp(strValue,"KBytes")==0)
		m_nMinSizeType = SIZETYPE_KBYTES;
	else if(stricmp(strValue,"MBytes")==0)
		m_nMinSizeType = SIZETYPE_MBYTES;
	else
		m_nMinSizeType = SIZETYPE_KBYTES;

	OnEnChangeMinSize();
}

/*
	OnSelChangeComboMaxSize()
*/
void CWallPaperCrawlerSettingsDownDlg::OnSelChangeComboMaxSize(void)
{
	// ricava la selezione corrente
	CString strValue;
	m_wndComboMaxSize.GetLBText(m_wndComboMaxSize.GetCurSel(),strValue);
	
	if(stricmp(strValue,"Bytes")==0)
		m_nMaxSizeType = SIZETYPE_BYTES;
	else if(stricmp(strValue,"KBytes")==0)
		m_nMaxSizeType = SIZETYPE_KBYTES;
	else if(stricmp(strValue,"MBytes")==0)
		m_nMaxSizeType = SIZETYPE_MBYTES;
	else
		m_nMaxSizeType = SIZETYPE_KBYTES;

	OnEnChangeMaxSize();
}

/*
	OnVScroll()
*/
void CWallPaperCrawlerSettingsDownDlg::OnVScroll(UINT nSBCode,UINT nPos,CScrollBar* pScrollBar)
{
	if(nSBCode!=SB_ENDSCROLL)
	{
		CString strValue;
		strValue.Format("%d",(int)nPos);
		((CSpinButtonCtrl*)pScrollBar)->GetBuddy()->SetWindowText(strValue);
		((CSpinButtonCtrl*)pScrollBar)->SetPos((int)nPos);
	}
}
