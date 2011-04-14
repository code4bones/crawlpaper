/*
	WallPaperCrawlerSettingsConnDlg.cpp
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
#include "WallPaperConfig.h"
#include "WallPaperCrawlerSettingsConnDlg.h"
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

BEGIN_MESSAGE_MAP(CWallPaperCrawlerSettingsConnDlg,CPropertyPage)
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDC_CHECK_RETRY,OnCheckRetry)
	ON_BN_CLICKED(IDC_CHECK_TIMEOUT,OnCheckTimeout)
	ON_BN_CLICKED(IDC_CHECK_WINSOCKTIMEOUT,OnCheckWinsockTimeout)
	ON_BN_CLICKED(IDC_CHECK_COOKIES_ALLOWED,OnCheckAllowCookies)
	ON_BN_CLICKED(IDC_CHECK_WARNBEFOREACCEPTCOOKIE,OnCheckWarnBeforeAcceptCookie)
	ON_BN_CLICKED(IDC_CHECK_WARNBEFORESENDCOOKIE,OnCheckWarnBeforeSendCookie)
END_MESSAGE_MAP()

IMPLEMENT_DYNCREATE(CWallPaperCrawlerSettingsConnDlg,CPropertyPage)

/*
	DoDataExchange()
*/
void CWallPaperCrawlerSettingsConnDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Text(pDX,IDC_EDIT_RETRY,m_nRetry);
	DDX_Text(pDX,IDC_EDIT_TIMEOUT,m_nTimeout);
	DDX_Check(pDX,IDC_CHECK_RETRY,m_bRetry);
	DDX_Check(pDX,IDC_CHECK_TIMEOUT,m_bTimeout);
	DDX_Check(pDX,IDC_CHECK_WINSOCKTIMEOUT,m_bWinsockTimeout);
	DDX_Check(pDX,IDC_CHECK_COOKIES_ALLOWED,m_bAllowCookies);
	DDX_Check(pDX,IDC_CHECK_WARNBEFOREACCEPTCOOKIE,m_bWarnBeforeAcceptCookie);
	DDX_Check(pDX,IDC_CHECK_WARNBEFORESENDCOOKIE,m_bWarnBeforeSendCookie);
}

/*
	CreateEx()
*/
BOOL CWallPaperCrawlerSettingsConnDlg::CreateEx(CWnd* pParent,UINT nID,CWallPaperConfig* pConfig)
{
	// configurazione corrente
	m_pConfig = pConfig;

	// imposta con i valori della configurazione
	m_nRetry = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_CONNECTIONRETRY_KEY);
	m_bRetry = m_nRetry > 0;
	m_nTimeout = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_CONNECTIONTIMEOUT_KEY);
	m_bTimeout = m_nTimeout > 0;
	m_bWinsockTimeout = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_USEWINSOCKTIMEOUT_KEY);
	m_bWinsockTimeout = m_bTimeout ? m_bWinsockTimeout : FALSE;
	m_bAllowCookies = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_COOKIES_ALLOWED_KEY);
	m_bWarnBeforeAcceptCookie = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_COOKIES_WARNINGONACCEPT_KEY);
	m_bWarnBeforeSendCookie = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_COOKIES_WARNINGONSEND_KEY);

	// crea il dialogo
	return(Create(nID,pParent));
}

/*
	OnInitDialog()
*/
BOOL CWallPaperCrawlerSettingsConnDlg::OnInitDialog(void)
{
	// classe base
	CPropertyPage::OnInitDialog();

	// imposta gli spin buttons
	CSpinButtonCtrl* pSpin;

	pSpin = (CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_RETRY);
	pSpin->SetRange(MIN_RETRY_VALUE,MAX_RETRY_VALUE);
	pSpin->SetPos(m_nRetry);
	pSpin->SetBuddy(GetDlgItem(IDC_EDIT_RETRY));

	pSpin = (CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_TIMEOUT);
	pSpin->SetRange(MIN_TIMEOUT_VALUE,MAX_TIMEOUT_VALUE);
	pSpin->SetPos(m_nTimeout);
	pSpin->SetBuddy(GetDlgItem(IDC_EDIT_TIMEOUT));

	// abilita i controlli
	GetDlgItem(IDC_EDIT_RETRY)->EnableWindow(m_bRetry);
	GetDlgItem(IDC_SPIN_RETRY)->EnableWindow(m_bRetry);
	GetDlgItem(IDC_EDIT_TIMEOUT)->EnableWindow(m_bTimeout);
	GetDlgItem(IDC_SPIN_TIMEOUT)->EnableWindow(m_bTimeout);
	GetDlgItem(IDC_CHECK_WARNBEFOREACCEPTCOOKIE)->EnableWindow(m_bAllowCookies);
	GetDlgItem(IDC_CHECK_WARNBEFORESENDCOOKIE)->EnableWindow(m_bAllowCookies);

	// aggiunge i tooltips
	if(m_Tooltip.Create(this,TTS_ALWAYSTIP))
	{
		m_Tooltip.SetWidth(TOOLTIP_REASONABLE_WIDTH);
		m_Tooltip.SetDelay(TOOLTIP_REASONABLE_DELAYTIME);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_CHECK_RETRY),IDC_CHECK_RETRY,IDS_TOOLTIP_OPTIONS_CONN_RETRY);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_EDIT_RETRY),IDC_EDIT_RETRY,IDS_TOOLTIP_OPTIONS_CONN_RETRY);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_SPIN_RETRY),IDC_SPIN_RETRY,IDS_TOOLTIP_OPTIONS_CONN_RETRY);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_CHECK_TIMEOUT),IDC_CHECK_TIMEOUT,IDS_TOOLTIP_OPTIONS_CONN_ABORT_TIMEOUT);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_EDIT_TIMEOUT),IDC_EDIT_TIMEOUT,IDS_TOOLTIP_OPTIONS_CONN_ABORT_TIMEOUT);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_SPIN_TIMEOUT),IDC_SPIN_TIMEOUT,IDS_TOOLTIP_OPTIONS_CONN_ABORT_TIMEOUT);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_CHECK_WINSOCKTIMEOUT),IDC_CHECK_WINSOCKTIMEOUT,IDS_TOOLTIP_OPTIONS_CONN_WINSOCK_TIMEOUT);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_CHECK_COOKIES_ALLOWED),IDC_CHECK_COOKIES_ALLOWED,IDS_TOOLTIP_OPTIONS_CONN_COOKIES);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_CHECK_WARNBEFOREACCEPTCOOKIE),IDC_CHECK_WARNBEFOREACCEPTCOOKIE,IDS_TOOLTIP_OPTIONS_CONN_COOKIES);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_CHECK_WARNBEFORESENDCOOKIE),IDC_CHECK_WARNBEFORESENDCOOKIE,IDS_TOOLTIP_OPTIONS_CONN_COOKIES);
	}

	UpdateData(FALSE);

	return(FALSE);
}

/*
	OnCheckRetry()
*/
void CWallPaperCrawlerSettingsConnDlg::OnCheckRetry(void)
{
	m_bRetry = !m_bRetry;

	if(!m_bRetry)
	{
		m_nRetry = 0;
	}
	else
	{
		if(m_nRetry==0)
			m_nRetry = DEFAULT_CRAWLER_CONNECTIONRETRY;
	}
	
	GetDlgItem(IDC_EDIT_RETRY)->EnableWindow(m_bRetry);
	GetDlgItem(IDC_SPIN_RETRY)->EnableWindow(m_bRetry);

	UpdateData(FALSE);
}

/*
	OnCheckTimeout()
*/
void CWallPaperCrawlerSettingsConnDlg::OnCheckTimeout(void)
{
	m_bTimeout = !m_bTimeout;

	if(!m_bTimeout)
	{
		m_nTimeout = 0;
		m_bWinsockTimeout = FALSE;
		GetDlgItem(IDC_CHECK_WINSOCKTIMEOUT)->EnableWindow(FALSE);
	}
	else
	{
		if(m_nTimeout==0)
			m_nTimeout = DEFAULT_CRAWLER_CONNECTIONTIMEOUT;
		m_bWinsockTimeout = TRUE;
		GetDlgItem(IDC_CHECK_WINSOCKTIMEOUT)->EnableWindow(TRUE);
	}

	GetDlgItem(IDC_EDIT_TIMEOUT)->EnableWindow(m_bTimeout);
	GetDlgItem(IDC_SPIN_TIMEOUT)->EnableWindow(m_bTimeout);

	UpdateData(FALSE);
}

/*
	OnCheckWinsockTimeout()
*/
void CWallPaperCrawlerSettingsConnDlg::OnCheckWinsockTimeout(void)
{
	m_bWinsockTimeout = !m_bWinsockTimeout;
}

/*
	OnCheckAllowCookies()
*/
void CWallPaperCrawlerSettingsConnDlg::OnCheckAllowCookies(void)
{
	m_bAllowCookies = !m_bAllowCookies;
	
	GetDlgItem(IDC_CHECK_WARNBEFOREACCEPTCOOKIE)->EnableWindow(m_bAllowCookies);
	GetDlgItem(IDC_CHECK_WARNBEFORESENDCOOKIE)->EnableWindow(m_bAllowCookies);
}

/*
	OnCheckWarnBeforeAcceptCookie()
*/
void CWallPaperCrawlerSettingsConnDlg::OnCheckWarnBeforeAcceptCookie(void)
{
	m_bWarnBeforeAcceptCookie = !m_bWarnBeforeAcceptCookie;
}

/*
	OnCheckWarnBeforeSendCookie()
*/
void CWallPaperCrawlerSettingsConnDlg::OnCheckWarnBeforeSendCookie(void)
{
	m_bWarnBeforeSendCookie = !m_bWarnBeforeSendCookie;
}

/*
	OnVScroll()
*/
void CWallPaperCrawlerSettingsConnDlg::OnVScroll(UINT nSBCode,UINT nPos,CScrollBar* pScrollBar)
{
	if(nSBCode!=SB_ENDSCROLL)
	{
		CString strValue;
		strValue.Format("%d",(int)nPos);
		((CSpinButtonCtrl*)pScrollBar)->GetBuddy()->SetWindowText(strValue);
		((CSpinButtonCtrl*)pScrollBar)->SetPos((int)nPos);
	}
}
