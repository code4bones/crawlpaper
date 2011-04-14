/*
	WallPaperDrawSettingsTextDlg.h
	Dialogo per la pagina relativa alle opzioni per le miniature.
	Luca Piergentili, 10/06/03
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
#ifndef _WALLPAPERDRAWSETTINGSTEXTDLG_H
#define _WALLPAPERDRAWSETTINGSTEXTDLG_H 1

#include "window.h"
#include "CColorStatic.h"
#include "CToolTipCtrlEx.h"
#include "WallPaperConfig.h"

/*
	CWallPaperDrawSettingsTextDlg
*/
class CWallPaperDrawSettingsTextDlg : public CPropertyPage
{
	DECLARE_DYNCREATE(CWallPaperDrawSettingsTextDlg)

public:
	CWallPaperDrawSettingsTextDlg() {}
	~CWallPaperDrawSettingsTextDlg() 
	{
		if(m_hFont)
			::DeleteObject(m_hFont);
	}

	BOOL				CreateEx				(CWnd* pParent,UINT nID,CWallPaperConfig* pConfig);

	void				DoDataExchange			(CDataExchange* pDX);
	BOOL				OnInitDialog			(void);
	BOOL				OnSetActive			(void) {return(TRUE);}
	void				OnKillFocusComboText	(void);
	void				OnSelChangeComboText	(void);
	void				OnKillFocusComboTextPosition(void);
	void				OnSelChangeComboTextPosition(void);
	void				OnButtonFont			(void);
	void				OnButtonFgColor		(void);
	void				OnButtonBkColor		(void);
	void				OnCheckBkColorTransparent(void);

	CToolTipCtrlEx		m_Tooltip;
	CWallPaperConfig*	m_pConfig;
	int				m_nDrawTextMode;
	CComboBox			m_wndComboText;
	int				m_nDrawTextPosition;
	CComboBox			m_wndComboTextPosition;
	HFONT			m_hFont;
	CHOOSEFONT		m_cf;
	LOGFONT			m_lf;
	CString			m_strFontName;
	COLORREF			m_crFontTextColor;
	COLORREF			m_crFontBkColor;
	BOOL				m_bFontBkColorTransparent;
	CColorStatic		m_wndSampleText;
	CColorStatic		m_wndSampleTextColor;
	CColorStatic		m_wndSampleBkColor;

	DECLARE_MESSAGE_MAP()
};

#endif // _WALLPAPERDRAWSETTINGSTEXTDLG_H
