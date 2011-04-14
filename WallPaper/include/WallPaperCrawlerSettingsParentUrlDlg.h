/*
	WallPaperCrawlerSettingsParentUrlDlg.h
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
#ifndef _WALLPAPERCRAWLERSETTINGSPARENTURLDLG_H
#define _WALLPAPERCRAWLERSETTINGSPARENTURLDLG_H 1

#include "window.h"
#include "CListCtrlEx.h"
#include "CToolTipCtrlEx.h"
#include "WallPaperConfig.h"

#define PARENTURL_FLAG_INCLUDE 0x00000001
#define PARENTURL_FLAG_EXCLUDE 0x00000002

#define IDS_PARENTACCEPTANCE_ASK								"ask (same as exclude all except pics when in page content only)"
#define IDS_PARENTACCEPTANCE_EXCLUDE_ALL_OUTSIDE					"no parents"
#define IDS_PARENTACCEPTANCE_EXCLUDE_ALL_OUTSIDE_EXCEPT_PICTURES		"no parents except pictures"
#define IDS_PARENTACCEPTANCE_EXCLUDE_ALL_OUTSIDE_EXCEPT_REGFILETYPES	"no parents except registered file types"
#define IDS_PARENTACCEPTANCE_FOLLOW_ALL_OUTSIDE					"include parents"

/*
	CWallPaperCrawlerSettingsParentUrlDlg
*/
class CWallPaperCrawlerSettingsParentUrlDlg : public CPropertyPage
{
	DECLARE_DYNCREATE(CWallPaperCrawlerSettingsParentUrlDlg)

public:
	CWallPaperCrawlerSettingsParentUrlDlg() {}
	~CWallPaperCrawlerSettingsParentUrlDlg() {}

	BOOL		CreateEx				(CWnd* pParent,UINT nID,CWallPaperConfig* pConfig);

	void		DoDataExchange			(CDataExchange* pDX);
	BOOL		OnInitDialog			(void);
	BOOL		OnSetActive			(void);
	void		OnButtonAdd			(void);
	void		OnButtonRemove			(void);
	LONG		OnListDoubleClick		(UINT wParam,LONG lParam);
	void		OnSelChangeComboParentUrl(void);
	void		OnCheckParentUrlUseAlways(void);
	void		OnCheckSaveList		(void);

	void		SetFlags				(DWORD dwFlags)			{m_dwFlags = dwFlags;}
	void		SetParentUrlAcceptance	(int nParentUrlAcceptance)	{m_nParentUrlAcceptance = nParentUrlAcceptance;}
	int		GetParentUrlAcceptance	(void) const				{return(m_nParentUrlAcceptance);}
	void		SetParentUrlUseAlways	(BOOL bParentUrlUseAlways)	{m_bParentUrlUseAlways = bParentUrlUseAlways;}
	BOOL		GetParentUrlUseAlways	(void) const				{return(m_bParentUrlUseAlways);}
	void		SetSaveList			(BOOL bSaveList)			{m_bSaveList = bSaveList;}
	BOOL		GetSaveList			(void) const				{return(m_bSaveList);}

	CToolTipCtrlEx		m_Tooltip;
	CToolTipList		m_TooltipList;
	CWallPaperConfig*	m_pConfig;
	CString			m_strValue;
	CListCtrlEx		m_wndParentUrlList;
	int				m_nParentUrlAcceptance;
	BOOL				m_bParentUrlUseAlways;
	BOOL				m_bSaveList;
	CComboBox			m_wndComboParentUrl;
	DWORD			m_dwFlags;
	char				m_szIncludeExcludeKey[REGKEY_MAX_KEY_NAME+1];
	char				m_szIncludeExcludeListKey[REGKEY_MAX_KEY_NAME+1];

	DECLARE_MESSAGE_MAP()
};

#endif // _WALLPAPERCRAWLERSETTINGSPARENTURLDLG_H
