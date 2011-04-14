/*
	WallPaperParentUrlDlg.h
	Dialogo per la gestione delle directories padre.
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
#ifndef _WALLPAPERPARENTURLDLG_H
#define _WALLPAPERPARENTURLDLG_H 1

#include "window.h"
#include "CWndLayered.h"
#include "WallPaperConfig.h"

enum PARENTURLACCEPTANCE {
	UNDEFINED_PARENT_ACCEPTANCE,
	EXCLUDE_THIS_PARENTURL,
	EXCLUDE_ALL_PARENTURLS,
	EXCLUDE_ALL_PARENTURLS_EXCEPT_PICTURES,
	EXCLUDE_ALL_PARENTURLS_EXCEPT_REGFILETYPES,
	EXCLUDE_ALL_PARENTURLS_FROM_THIS_PATHNAME,
	FOLLOW_THIS_PARENTURL,
	FOLLOW_ALL_PARENTURLS,
	FOLLOW_ALL_PARENTURLS_FROM_THIS_PATHNAME
};

#define IDS_EXCLUDE_THIS_PARENTURL					"exclude this url"
#define IDS_EXCLUDE_ALL_PARENTURLS					"exclude all parent url(s)"
#define IDS_EXCLUDE_ALL_PARENTURLS_EXCEPT_PICTURES	"exclude all parent url(s) except pictures"
#define IDS_EXCLUDE_ALL_PARENTURLS_EXCEPT_REGFILETYPES	"exclude all parent url(s) except registered file types"
#define IDS_EXCLUDE_ALL_PARENTURLS_FROM_THIS_PATHNAME	"exclude all parent url(s) from this pathname"
#define IDS_FOLLOW_THIS_PARENTURL					"follow this url"
#define IDS_FOLLOW_ALL_PARENTURLS					"follow all parent url(s)"
#define IDS_FOLLOW_ALL_PARENTURLS_FROM_THIS_PATHNAME	"follow all parent url(s) from this pathname"

/*
	CWallPaperParentUrlDlg
*/
class CWallPaperParentUrlDlg : public CDialog
{
public:
	CWallPaperParentUrlDlg(CWnd* pParent,CWallPaperConfig* pConfig,LPCSTR lpcszUrl,PARENTURLACCEPTANCE parenturlacceptance = UNDEFINED_PARENT_ACCEPTANCE);
	~CWallPaperParentUrlDlg() {}

	PARENTURLACCEPTANCE	GetParentAcceptance			(void) const {return(m_ParentUrlAcceptance);}
	BOOL				AskForParentAcceptance		(void) const {return(!m_bDontShowDialog);}

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
	PARENTURLACCEPTANCE	m_ParentUrlAcceptance;
	BOOL				m_bDontShowDialog;
	CWndLayered		m_wndLayered;

	DECLARE_MESSAGE_MAP()
};

#endif // _WALLPAPERPARENTURLDLG_H
