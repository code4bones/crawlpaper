/*
	WallPaperThumbSettingsHtmlTableDlg.h
	Dialogo per la pagina relativa alle opzioni per le miniature.
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
#ifndef _WALLPAPERTHUMBSETTINGSHTMLTABLEDLG_H
#define _WALLPAPERTHUMBSETTINGSHTMLTABLEDLG_H

#include "window.h"
#include "CToolTipCtrlEx.h"
#include "WallPaperConfig.h"

#define MIN_THUMBNAILS_TABLE_WIDTH	0
#define MAX_THUMBNAILS_TABLE_WIDTH	100
#define MIN_THUMBNAILS_COLS		0
#define MAX_THUMBNAILS_COLS		100

/*
	CWallPaperThumbSettingsHtmlTableDlg
*/
class CWallPaperThumbSettingsHtmlTableDlg : public CPropertyPage
{
	DECLARE_DYNCREATE(CWallPaperThumbSettingsHtmlTableDlg)

public:
	CWallPaperThumbSettingsHtmlTableDlg() {}
	~CWallPaperThumbSettingsHtmlTableDlg() {}

	BOOL		CreateEx				(CWnd* pParent,UINT nID,CWallPaperConfig* pConfig);

	void		DoDataExchange			(CDataExchange* pDX);
	BOOL		OnInitDialog			(void);
	BOOL		OnSetActive			(void);
	void		OnVScroll				(UINT nSBCode,UINT nPos,CScrollBar* pScrollBar);
	void		OnCheckGenerateHtml		(void);

	void		SetGenerateHtmlFlag		(BOOL bGenerateHtml) {m_bGenerateHtml = bGenerateHtml;}
	BOOL		GetGenerateHtmlFlag		(void) const {return(m_bGenerateHtml);}

	CToolTipCtrlEx		m_Tooltip;
	CWallPaperConfig*	m_pConfig;
	CString			m_strTitle;
	UINT				m_nWidth;
	UINT				m_nCols;
	UINT				m_nBorder;
	UINT				m_nSpacing;
	UINT				m_nPadding;
	BOOL				m_bGenerateHtml;

	DECLARE_MESSAGE_MAP()
};

#endif // _WALLPAPERTHUMBSETTINGSHTMLTABLEDLG_H
