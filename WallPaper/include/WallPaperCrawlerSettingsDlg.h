/*
	WallPaperCrawlerSettingsDlg.h
	Dialogo per le opzioni del crawler.
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
#ifndef _WALLPAPERCRAWLERSETTINGSDLG_H
#define _WALLPAPERCRAWLERSETTINGSDLG_H 1

#include "window.h"
#include "CIconStatic.h"
#include "CWndLayered.h"
#include "WallPaperConfig.h"
#include "WallPaperCrawlerSettingsConnDlg.h"
#include "WallPaperCrawlerSettingsDownDlg.h"
#include "WallPaperCrawlerSettingsFileTypesDlg.h"
#include "WallPaperCrawlerSettingsDomainDlg.h"
#include "WallPaperCrawlerSettingsParentUrlDlg.h"
#include "WallPaperCrawlerSettingsWildcardsDlg.h"
#include "WallPaperCrawlerSettingsProxyDlg.h"
#include "WallPaperCrawlerSettingsReportDlg.h"

#define IDS_DIALOG_CRAWLER_SETTINGS_TITLE " Crawler Settings "

/*
	CWallPaperCrawlerSettingsDlg
*/
class CWallPaperCrawlerSettingsDlg : public CDialog
{
public:
	CWallPaperCrawlerSettingsDlg(CWnd* pParent,CWallPaperConfig* pConfig);
	~CWallPaperCrawlerSettingsDlg() {}

private:
	void		DoDataExchange		(CDataExchange*);
	BOOL		OnInitDialog		(void);
	void		OnOk				(void);
	void		OnCancel			(void);
	void		OnSelchangingTree	(NMHDR* pNMHDR,LRESULT* pResult);
	void		OnSelchangedTree	(NMHDR* pNMHDR,LRESULT* pResult);

	CIconStatic						m_ctrlSettingsArea;
	CTreeCtrl							m_wndSettingsTree;
	HTREEITEM							m_hItemRoot;
	HTREEITEM							m_hItemConnection;
	HTREEITEM							m_hItemDownload;
	HTREEITEM							m_hItemFileTypes;
	HTREEITEM							m_hItemDomainAcceptance;
	HTREEITEM							m_hItemDomainInclude;
	HTREEITEM							m_hItemDomainExclude;
	HTREEITEM							m_hItemParentUrlAcceptance;
	HTREEITEM							m_hItemParentUrlInclude;
	HTREEITEM							m_hItemParentUrlExclude;
	HTREEITEM							m_hItemWildcards;
	HTREEITEM							m_hItemProxy;
	HTREEITEM							m_hItemReport;
	DWORD							m_dwRootData;
	CWallPaperCrawlerSettingsConnDlg		m_dlgTreeConnection;
	CWallPaperCrawlerSettingsDownDlg		m_dlgTreeDownload;
	CWallPaperCrawlerSettingsFileTypesDlg	m_dlgTreeFileTypes;
	CWallPaperCrawlerSettingsDomainDlg		m_dlgTreeDomainInclude;
	CWallPaperCrawlerSettingsDomainDlg		m_dlgTreeDomainExclude;
	CWallPaperCrawlerSettingsParentUrlDlg	m_dlgTreeParentUrlInclude;
	CWallPaperCrawlerSettingsParentUrlDlg	m_dlgTreeParentUrlExclude;
	CWallPaperCrawlerSettingsWildcardsDlg	m_dlgTreeWildcards;
	CWallPaperCrawlerSettingsProxyDlg		m_dlgTreeProxy;
	CWallPaperCrawlerSettingsReportDlg		m_dlgTreeReport;
	CWallPaperConfig*					m_pConfig;
	BOOL								m_bCreated;
	int								m_nDomainAcceptance;
	BOOL								m_bIncludeSubDomains;
	BOOL								m_bDomainUseAlways;
	int								m_nParentUrlAcceptance;
	BOOL								m_bParentUrlUseAlways;
	BOOL								m_bSaveList;
	CWndLayered						m_wndLayered;

	DECLARE_MESSAGE_MAP()
};

#endif // _WALLPAPERCRAWLERSETTINGSDLG_H
