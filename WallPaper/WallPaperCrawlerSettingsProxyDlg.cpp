/*
	WallPaperCrawlerSettingsProxyDlg.cpp
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
#include "window.h"
#include "CToolTipCtrlEx.h"
#include "WallPaperConfig.h"
#include "WallPaperCrawlerSettingsProxyDlg.h"
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

BEGIN_MESSAGE_MAP(CWallPaperCrawlerSettingsProxyDlg,CPropertyPage)
	ON_BN_CLICKED(IDC_CHECK_PROXY,OnCheckUseProxy)
END_MESSAGE_MAP()

IMPLEMENT_DYNCREATE(CWallPaperCrawlerSettingsProxyDlg,CPropertyPage)

/*
	DoDataExchange()
*/
void CWallPaperCrawlerSettingsProxyDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Text(pDX,IDC_EDIT_PROXY_ADDRESS,m_strProxyAddress);
	DDX_Text(pDX,IDC_EDIT_PROXY_PORT,m_nProxyPort);
	DDX_Text(pDX,IDC_EDIT_PROXY_USER,m_strProxyUser);
	DDX_Text(pDX,IDC_EDIT_PROXY_PASSWORD,m_strProxyPassword);
	DDX_Text(pDX,IDC_EDIT_PROXY_EXCLUDEDOMAINS,m_strProxyExcludeDomains);
	DDX_Check(pDX,IDC_CHECK_PROXY,m_bUseProxy);
}

/*
	CreateEx()
*/
BOOL CWallPaperCrawlerSettingsProxyDlg::CreateEx(CWnd* pParent,UINT nID,CWallPaperConfig* pConfig)
{
	// configurazione corrente
	m_pConfig = pConfig;

	// imposta con i valori della configurazione
	m_strProxyAddress.Format("%s",m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_PROXY_ADDRESS_KEY));
	m_nProxyPort = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PROXY_PORTNUMBER_KEY);
	m_strProxyUser.Format("%s",m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_PROXY_USER_KEY));
	m_strProxyPassword.Format("%s",m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_PROXY_PASSWORD_KEY));
	m_strProxyExcludeDomains.Format("%s",m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_PROXY_EXCLUDEDOMAINS_KEY));
	m_bUseProxy = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_USEPROXY_KEY);

	// crea il dialogo
	return(Create(nID,pParent));
}

/*
	OnInitDialog()
*/
BOOL CWallPaperCrawlerSettingsProxyDlg::OnInitDialog(void)
{
	// classe base
	CPropertyPage::OnInitDialog();

	// abilita i controlli
	GetDlgItem(IDC_EDIT_PROXY_ADDRESS)->EnableWindow(m_bUseProxy);
	GetDlgItem(IDC_EDIT_PROXY_PORT)->EnableWindow(m_bUseProxy);
	GetDlgItem(IDC_EDIT_PROXY_USER)->EnableWindow(m_bUseProxy);
	GetDlgItem(IDC_EDIT_PROXY_PASSWORD)->EnableWindow(m_bUseProxy);
	GetDlgItem(IDC_EDIT_PROXY_EXCLUDEDOMAINS)->EnableWindow(m_bUseProxy);

	// aggiunge i tooltips
	if(m_Tooltip.Create(this,TTS_ALWAYSTIP))
	{
		m_Tooltip.SetWidth(TOOLTIP_REASONABLE_WIDTH);
		m_Tooltip.SetDelay(TOOLTIP_REASONABLE_DELAYTIME);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_EDIT_PROXY_ADDRESS),IDC_EDIT_PROXY_ADDRESS,IDS_TOOLTIP_OPTIONS_PROXY_ADDRESS);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_EDIT_PROXY_PORT),IDC_EDIT_PROXY_PORT,IDS_TOOLTIP_OPTIONS_PROXY_PORT);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_EDIT_PROXY_USER),IDC_EDIT_PROXY_USER,IDS_TOOLTIP_OPTIONS_PROXY_AUTH);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_EDIT_PROXY_PASSWORD),IDC_EDIT_PROXY_PASSWORD,IDS_TOOLTIP_OPTIONS_PROXY_AUTH);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_EDIT_PROXY_EXCLUDEDOMAINS),IDC_EDIT_PROXY_EXCLUDEDOMAINS,IDS_TOOLTIP_OPTIONS_PROXY_LOCAL_ADDR);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_CHECK_PROXY),IDC_CHECK_PROXY,IDS_TOOLTIP_OPTIONS_PROXY_ENABLE);
	}

	UpdateData(FALSE);

	return(FALSE);
}

/*
	OnCheckUseProxy()
*/
void CWallPaperCrawlerSettingsProxyDlg::OnCheckUseProxy(void)
{
	m_bUseProxy = !m_bUseProxy;

	GetDlgItem(IDC_EDIT_PROXY_ADDRESS)->EnableWindow(m_bUseProxy);
	GetDlgItem(IDC_EDIT_PROXY_PORT)->EnableWindow(m_bUseProxy);
	GetDlgItem(IDC_EDIT_PROXY_USER)->EnableWindow(m_bUseProxy);
	GetDlgItem(IDC_EDIT_PROXY_PASSWORD)->EnableWindow(m_bUseProxy);
	GetDlgItem(IDC_EDIT_PROXY_EXCLUDEDOMAINS)->EnableWindow(m_bUseProxy);
}
