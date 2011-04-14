/*
	WallPaperThumbSettingsDlg.h
	Dialogo per le opzioni relative alla miniature.
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
#ifndef _WALLPAPERTHUMBSETTINGSDLG_H
#define _WALLPAPERTHUMBSETTINGSDLG_H 1

#include "window.h"
#include "CIconStatic.h"
#include "CWndLayered.h"
#include "WallPaperConfig.h"
#include "WallPaperThumbSettingsFormatDlg.h"
#include "WallPaperThumbSettingsHtmlOutputDlg.h"
#include "WallPaperThumbSettingsHtmlTableDlg.h"

#define IDS_DIALOG_THUMBNAILS_SETTINGS_TITLE " Thumbnails Settings "

/*
	CWallPaperThumbSettingsDlg
*/
class CWallPaperThumbSettingsDlg : public CDialog
{
public:
	CWallPaperThumbSettingsDlg(CWnd* pParent,CWallPaperConfig* pConfig);
	~CWallPaperThumbSettingsDlg() {}

private:
	void			DoDataExchange		(CDataExchange*);
	BOOL			OnInitDialog		(void);
	void			OnOk				(void);
	void			OnCancel			(void);
	void			OnSelchangingTree	(NMHDR* pNMHDR,LRESULT* pResult);
	void			OnSelchangedTree	(NMHDR* pNMHDR,LRESULT* pResult);

	CIconStatic					m_ctrlSettingsArea;
	CTreeCtrl						m_wndSettingsTree;
	HTREEITEM						m_hItemRoot;
	HTREEITEM						m_hItemFormat;
	HTREEITEM						m_hItemHtmlOutput;
	HTREEITEM						m_hItemHtmlTable;
	DWORD						m_dwRootData;
	CWallPaperThumbSettingsFormatDlg	m_dlgTreeFormat;
	CWallPaperThumbSettingsHtmlOutputDlg m_dlgTreeHtmlOutput;
	CWallPaperThumbSettingsHtmlTableDlg m_dlgTreeHtmlTable;
	CWallPaperConfig*				m_pConfig;
	BOOL							m_bGenerateHtml;
	BOOL							m_bCreated;
	CWndLayered					m_wndLayered;

	DECLARE_MESSAGE_MAP()
};

#endif // _WALLPAPERTHUMBSETTINGSDLG_H
