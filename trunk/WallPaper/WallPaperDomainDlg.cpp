/*
	WallPaperDomainDlg.cpp
	Dialogo per la gestione dei domini.
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
#include "win32api.h"
#include "CWndLayered.h"
#include "WallPaperDomainDlg.h"
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

BEGIN_MESSAGE_MAP(CWallPaperDomainDlg,CDialog)
	ON_BN_CLICKED(IDOK,OnOk)
	ON_BN_CLICKED(IDCANCEL,OnCancel)
	ON_CBN_SELCHANGE(IDC_COMBO_ACCEPTANCE,OnSelChangeComboAcceptance)
	ON_CBN_KILLFOCUS(IDC_COMBO_ACCEPTANCE,OnKillFocusComboAcceptance)
END_MESSAGE_MAP()

/*
	DoDataExchange()
*/
void CWallPaperDomainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX,IDC_EDIT_URL,m_strUrl);
	DDX_Control(pDX,IDC_COMBO_ACCEPTANCE,m_wndComboAcceptance);
	DDX_Check(pDX,IDC_CHECK_SHOWDIALOG,m_bDontShowDialog);
}

/*
	CWallPaperDomainDlg()
*/
CWallPaperDomainDlg::CWallPaperDomainDlg(CWnd* pParent,CWallPaperConfig* pConfig,LPCSTR lpcszUrl,DOMAINACCEPTANCE domainacceptance/*=UNDEFINED_DOMAIN_ACCEPTANCE*/) : CDialog(IDD_DIALOG_DOMAIN,pParent)
{
	m_pConfig = pConfig;
	m_strUrl.Format("%s",lpcszUrl);
	m_DomainAcceptance = domainacceptance;
	m_bDontShowDialog = FALSE;
}

/*
	OnInitDialog()
*/
BOOL CWallPaperDomainDlg::OnInitDialog(void)
{
	// classe base
	CDialog::OnInitDialog();

	// icona di default
	HICON hIcon = AfxGetApp()->LoadIcon(IDI_ICON_WALLPAPER);
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	// imposta il combo con i valori relativi
	m_wndComboAcceptance.AddString(IDS_EXCLUDE_THIS_DOMAIN);
	m_wndComboAcceptance.AddString(IDS_EXCLUDE_ALL_DOMAINS);
	m_wndComboAcceptance.AddString(IDS_EXCLUDE_ALL_DOMAINS_EXCEPT_PICTURES);
	m_wndComboAcceptance.AddString(IDS_EXCLUDE_ALL_DOMAINS_EXCEPT_REGFILETYPES);
	m_wndComboAcceptance.AddString(IDS_EXCLUDE_ALL_URLS_FROM_THIS_DOMAIN);
	m_wndComboAcceptance.AddString(IDS_FOLLOW_THIS_DOMAIN);
	m_wndComboAcceptance.AddString(IDS_FOLLOW_ALL_DOMAINS);
	m_wndComboAcceptance.AddString(IDS_FOLLOW_ALL_URLS_FROM_THIS_DOMAIN);
	int nIndex = 0;
	switch(m_DomainAcceptance)
	{
		case EXCLUDE_THIS_DOMAIN:
			nIndex = 0;
			break;
		case EXCLUDE_ALL_DOMAINS:
			nIndex = 1;
			break;
		case EXCLUDE_ALL_DOMAINS_EXCEPT_PICTURES:
			nIndex = 2;
			break;
		case EXCLUDE_ALL_DOMAINS_EXCEPT_REGFILETYPES:
			nIndex = 3;
			break;
		case EXCLUDE_ALL_URLS_FROM_THIS_DOMAIN:
			nIndex = 4;
			break;
		case FOLLOW_THIS_DOMAIN:
			nIndex = 5;
			break;
		case FOLLOW_ALL_DOMAINS:
			nIndex = 6;
			break;
		case FOLLOW_ALL_URLS_FROM_THIS_DOMAIN:
			nIndex = 7;
			break;
		case UNDEFINED_DOMAIN_ACCEPTANCE:
		default:
			nIndex = 0;
			break;
	}
	m_wndComboAcceptance.SetCurSel(nIndex);
	
	UpdateData(FALSE);

	// trasparenza
	if(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAYERED_KEY))
		m_wndLayered.SetLayer(this->m_hWnd,(BYTE)m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAYERED_VALUE_KEY));

	// mette in primo piano
	SetForegroundWindowEx(this->m_hWnd);

	return(TRUE);
}

/*
	OnOk()
*/
void CWallPaperDomainDlg::OnOk(void)
{
	if(UpdateData(TRUE))
		EndDialog(IDOK);
}

/*
	OnCancel()
*/
void CWallPaperDomainDlg::OnCancel(void)
{
	EndDialog(IDCANCEL);
}

/*
	OnComboAcceptance()
*/
void CWallPaperDomainDlg::OnComboAcceptance(void)
{
	// ricava la selezione corrente del combo
	char szComboEntry[_MAX_PATH+1];
	m_wndComboAcceptance.GetWindowText(szComboEntry,sizeof(szComboEntry));

	if(strcmp(szComboEntry,IDS_EXCLUDE_THIS_DOMAIN)==0)
		m_DomainAcceptance = EXCLUDE_THIS_DOMAIN;
	else if(strcmp(szComboEntry,IDS_EXCLUDE_ALL_DOMAINS)==0)
		m_DomainAcceptance = EXCLUDE_ALL_DOMAINS;
	else if(strcmp(szComboEntry,IDS_EXCLUDE_ALL_DOMAINS_EXCEPT_PICTURES)==0)
		m_DomainAcceptance = EXCLUDE_ALL_DOMAINS_EXCEPT_PICTURES;
	else if(strcmp(szComboEntry,IDS_EXCLUDE_ALL_DOMAINS_EXCEPT_REGFILETYPES)==0)
		m_DomainAcceptance = EXCLUDE_ALL_DOMAINS_EXCEPT_REGFILETYPES;
	else if(strcmp(szComboEntry,IDS_EXCLUDE_ALL_URLS_FROM_THIS_DOMAIN)==0)
		m_DomainAcceptance = EXCLUDE_ALL_URLS_FROM_THIS_DOMAIN;
	else if(strcmp(szComboEntry,IDS_FOLLOW_THIS_DOMAIN)==0)
		m_DomainAcceptance = FOLLOW_THIS_DOMAIN;
	else if(strcmp(szComboEntry,IDS_FOLLOW_ALL_DOMAINS)==0)
		m_DomainAcceptance = FOLLOW_ALL_DOMAINS;
	else if(strcmp(szComboEntry,IDS_FOLLOW_ALL_URLS_FROM_THIS_DOMAIN)==0)
		m_DomainAcceptance = FOLLOW_ALL_URLS_FROM_THIS_DOMAIN;
	else
		m_DomainAcceptance = EXCLUDE_THIS_DOMAIN;

	// da mantenere in corrispondenza con il codice del crawler
	switch(m_DomainAcceptance)
	{
		case EXCLUDE_ALL_DOMAINS:
		case EXCLUDE_ALL_DOMAINS_EXCEPT_PICTURES:
		case EXCLUDE_ALL_DOMAINS_EXCEPT_REGFILETYPES:
		case FOLLOW_ALL_DOMAINS:
			m_bDontShowDialog = TRUE;
			break;
		default:
			m_bDontShowDialog = FALSE;
			break;
	}
	
	UpdateData(FALSE);
	GetDlgItem(IDC_CHECK_SHOWDIALOG)->EnableWindow(!m_bDontShowDialog);
}
