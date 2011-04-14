/*
	WallPaperDrawSettingsModeDlg.h
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
#ifndef _WALLPAPERDRAWSETTINGSMODEDLG_H
#define _WALLPAPERDRAWSETTINGSMODEDLG_H

#include "window.h"
#include "CImageFactory.h"
#include "CImageParams.h"
#include "CToolTipCtrlEx.h"
#include "WallPaperConfig.h"

#define IDS_DRAWOWNERMODE_ORIGINAL_SIZE					"original size"
#define IDS_DRAWOWNERMODE_FIT_TO_SCREEN					"fit to screen"
#define IDS_DRAWOWNERMODE_STRETCH_TO_SCREEN				"stretch to screen"
#define IDS_DRAWOWNERMODE_STRETCH_TO_SCREEN_IF_DOES_NOT_FIT	"stretch to screen if does not fit"
#define IDS_DRAWOWNERMODE_STRETCH_TO_RECT				"stretch to rect..."
#define IDS_DRAWOWNERMODE_TILE_STRETCH					"tile/stretch..."
#define IDS_DRAWOWNERMODE_TILE_FIT						"tile/fit..."
#define IDS_DRAWOWNERMODE_OVERLAP_STRETCH				"overlap/stretch..."
#define IDS_DRAWOWNERMODE_OVERLAP_FIT					"overlap/fit..."

#define IDS_DRAWOWNERMODE_TILE_STRETCH_TEXT				"tile/stretch if the picture area if less/greater than the % of screen area"
#define IDS_DRAWOWNERMODE_TILE_FIT_TEXT					"tile/fit if the picture area if less/greater than the % of screen area"
#define IDS_DRAWOWNERMODE_OVERLAP_STRETCH_TEXT			"overlap/stretch if the picture area if less/greater than the % of screen area"
#define IDS_DRAWOWNERMODE_OVERLAP_FIT_TEXT				"overlap/fit if the picture area if less/greater than the % of screen area"

#define IDS_DRAWWINDOWSMODE_NORMAL						"normal"
#define IDS_DRAWWINDOWSMODE_TILE						"tile"
#define IDS_DRAWWINDOWSMODE_STRETCH					"stretch"

#define IDS_DRAWEFFECT_NONE							"<none>"
#define IDS_DRAWEFFECT_GRAYSCALE						"grayscale"
#define IDS_DRAWEFFECT_ROTATE90LEFT					"rotate 90 left"
#define IDS_DRAWEFFECT_ROTATE90RIGHT					"rotate 90 right"
#define IDS_DRAWEFFECT_ROTATE180						"rotate 180"

#define IDS_DRAWEFFECT_BLUR							"blur"
#define IDS_DRAWEFFECT_BRIGHTNESS						"brightness"
#define IDS_DRAWEFFECT_CONTRAST						"contrast"
#define IDS_DRAWEFFECT_EDGEENHANCE						"edge enhance"
#define IDS_DRAWEFFECT_EMBOSS							"emboss"
#define IDS_DRAWEFFECT_FINDEDGE						"find edge"
#define IDS_DRAWEFFECT_INVERT							"invert"
#define IDS_DRAWEFFECT_MIRRORH						"mirror (horiz.)"
#define IDS_DRAWEFFECT_MIRRORV						"mirror (vert.)"
#define IDS_DRAWEFFECT_SHARPEN						"sharpen"

#define IDS_DRAWEFFECT_RANDOM							"random"

#define MIN_EFFECT_VALUE 0
#define MAX_EFFECT_VALUE 100

/*
	CWallPaperDrawSettingsModeDlg
*/
class CWallPaperDrawSettingsModeDlg : public CPropertyPage
{
	DECLARE_DYNCREATE(CWallPaperDrawSettingsModeDlg)

public:
	CWallPaperDrawSettingsModeDlg() {}
	~CWallPaperDrawSettingsModeDlg() {}

	BOOL			CreateEx					(CWnd* pParent,UINT nID,CWallPaperConfig* pConfig);

	void			DoDataExchange				(CDataExchange* pDX);
	BOOL			OnInitDialog				(void);
	BOOL			OnSetActive				(void) {return(TRUE);}
	void			OnRadioButtonOwnerMode		(void);
	void			OnRadioButtonWindowsMode		(void);
	void			OnComboOwnerMode			(void);
	void			OnKillFocusComboOwnerMode	(void) {OnComboOwnerMode();}
	void			OnSelChangeComboOwnerMode	(void) {OnComboOwnerMode();}
	void			OnComboWindowsMode			(void);
	void			OnSelChangeComboWindowsMode	(void) {OnComboWindowsMode();}
	void			OnKillFocusComboWindowsMode	(void) {OnComboWindowsMode();}
	void			OnComboEffect				(void);
	void			OnKillFocusComboEffect		(void) {OnComboEffect();}
	void			OnSelChangeComboEffect		(void) {OnComboEffect();}
	void			OnEnChangeEffect			(void);
	void			OnVScroll					(UINT nSBCode,UINT nPos,CScrollBar* pScrollBar);

	CToolTipCtrlEx		m_Tooltip;
	CWallPaperConfig*	m_pConfig;
	char				m_szLibraryName[_MAX_PATH+1];
	CImageFactory		m_ImageFactory;
	CImage*			m_pImage;
	CImageParams		m_ImageParams;

	int				m_nDrawMode;
	int				m_nOwnerMode;
	int				m_nWindowsMode;
	CComboBox			m_wndComboOwnerMode;
	CComboBox			m_wndComboWindowsMode;
	UINT				m_nAreaRatio;
	UINT				m_nWidth;
	UINT				m_nHeight;
	UINT				m_nScreenWidth;
	UINT				m_nScreenHeight;
	UINT				m_nMinWidth;
	UINT				m_nMinHeight;

	int				m_nDrawEffect;
	int				m_nDrawEffectValue;
	CComboBox			m_wndComboEffect;

	DECLARE_MESSAGE_MAP()
};

#endif // _WALLPAPERDRAWSETTINGSMODEDLG_H
