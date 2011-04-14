/*
	WallPaperCrawlerSettingsFileTypesDlg.h
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
#ifndef _WALLPAPERCRAWLERSETTINGSFILETYPESDLG_H
#define _WALLPAPERCRAWLERSETTINGSFILETYPESDLG_H 1

#include "window.h"
#include "CListCtrlEx.h"
#include "CToolTipCtrlEx.h"
#include "CImageFactory.h"
#include "WallPaperConfig.h"

/*
	CWallPaperCrawlerSettingsFileTypesDlg
*/
class CWallPaperCrawlerSettingsFileTypesDlg : public CPropertyPage
{
	DECLARE_DYNCREATE(CWallPaperCrawlerSettingsFileTypesDlg)

public:
	CWallPaperCrawlerSettingsFileTypesDlg() {}
	~CWallPaperCrawlerSettingsFileTypesDlg() {}

	BOOL		CreateEx			(CWnd* pParent,UINT nID,CWallPaperConfig* pConfig);

	void		DoDataExchange		(CDataExchange* pDX);
	BOOL		OnInitDialog		(void);
	BOOL		OnSetActive		(void) {return(TRUE);}
	void		OnButtonAdd		(void);
	void		OnButtonRemove		(void);
	void		OnButtonDefault	(void);
	LONG		OnListDoubleClick	(UINT wParam,LONG lParam);
	void		OnCheckAskFileTypes	(void);
	void		OnCheckMultiThread	(void);
	void		OnDefaultFileTypes	(void);
	UINT		GetIconID			(LPCSTR lpcszFileType);

	CToolTipCtrlEx		m_Tooltip;
	CWallPaperConfig*	m_pConfig;
	BOOL				m_bAskFileTypes;
	BOOL				m_bMultiThread;
	CString			m_strValue;
	CListCtrlEx		m_wndFileTypesList;
	CImage*			m_pImage;
	CImageFactory		m_ImageFactory;
	char				m_szLibraryName[_MAX_PATH+1];

	DECLARE_MESSAGE_MAP()
};

#endif // _WALLPAPERCRAWLERSETTINGSFILETYPESDLG_H
