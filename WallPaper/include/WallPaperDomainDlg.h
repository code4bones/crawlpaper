/*
	WallPaperDomainDlg.h
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
#ifndef _WALLPAPERDOMAINDLG_H
#define _WALLPAPERDOMAINDLG_H 1

#include "window.h"
#include "CWndLayered.h"
#include "WallPaperConfig.h"

enum DOMAINACCEPTANCE {
	UNDEFINED_DOMAIN_ACCEPTANCE,
	EXCLUDE_THIS_DOMAIN,
	EXCLUDE_ALL_DOMAINS,
	EXCLUDE_ALL_DOMAINS_EXCEPT_PICTURES,
	EXCLUDE_ALL_DOMAINS_EXCEPT_REGFILETYPES,
	EXCLUDE_ALL_URLS_FROM_THIS_DOMAIN,
	FOLLOW_THIS_DOMAIN,
	FOLLOW_ALL_DOMAINS,
	FOLLOW_ALL_URLS_FROM_THIS_DOMAIN
};

#define IDS_EXCLUDE_THIS_DOMAIN					"exclude this url"
#define IDS_EXCLUDE_ALL_DOMAINS					"exclude all url(s) outside the current domain"
#define IDS_EXCLUDE_ALL_DOMAINS_EXCEPT_PICTURES		"exclude all url(s) outside except pictures"
#define IDS_EXCLUDE_ALL_DOMAINS_EXCEPT_REGFILETYPES	"exclude all url(s) outside except registered file types"
#define IDS_EXCLUDE_ALL_URLS_FROM_THIS_DOMAIN		"exclude all url(s) from this domain"
#define IDS_FOLLOW_THIS_DOMAIN					"follow this url"
#define IDS_FOLLOW_ALL_DOMAINS					"follow all url(s) outside the current domain"
#define IDS_FOLLOW_ALL_URLS_FROM_THIS_DOMAIN			"follow all url(s) from this domain"

/*
	CWallPaperDomainDlg
*/
class CWallPaperDomainDlg : public CDialog
{
public:
	CWallPaperDomainDlg(CWnd* pParent,CWallPaperConfig* pConfig,LPCSTR lpcszUrl,DOMAINACCEPTANCE domainacceptance = UNDEFINED_DOMAIN_ACCEPTANCE);
	~CWallPaperDomainDlg() {}

	DOMAINACCEPTANCE	GetDomainAcceptance			(void) const {return(m_DomainAcceptance);}
	BOOL				AskForDomainAcceptance		(void) const {return(!m_bDontShowDialog);}

private:
	void				DoDataExchange				(CDataExchange* pDX);
	BOOL				OnInitDialog				(void);
	void				OnOk						(void);
	void				OnCancel					(void);
	void				OnSelChangeComboAcceptance	(void) {OnComboAcceptance();}
	void				OnKillFocusComboAcceptance	(void) {OnComboAcceptance();}
	void				OnComboAcceptance			(void);

	CWallPaperConfig*	m_pConfig;
	CString			m_strUrl;
	CComboBox			m_wndComboAcceptance;
	DOMAINACCEPTANCE	m_DomainAcceptance;
	BOOL				m_bDontShowDialog;
	CWndLayered		m_wndLayered;

	DECLARE_MESSAGE_MAP()
};

#endif // _WALLPAPERDOMAINDLG_H
