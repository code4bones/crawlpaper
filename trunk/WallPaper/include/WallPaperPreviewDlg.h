/*
	WallPaperPreviewDlg.h
	Dialogo per il preview.
	Luca Piergentili, 08/09/03
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
#ifndef _WALLPAPERPREVIEWDLG_H
#define _WALLPAPERPREVIEWDLG_H 1

#include "window.h"
#include "CDialogEx.h"
#include "CDibCtrl.h"
#include "CWndLayered.h"
#include "CDialogHeader.h"
#include "WallPaperConfig.h"

#define IDS_DIALOG_PREVIEW_TITLE " Wallpaper preview "

/*
	CWallPaperPreviewDlg
*/
class CWallPaperPreviewDlg : public CDialogEx
{
	DECLARE_DYNCREATE(CWallPaperPreviewDlg)

protected: // provide default constructor only for dynamic creation, private/protected to prevent it from being called from outside the class implementation
	CWallPaperPreviewDlg() {::MessageBox(NULL,"PANIC! This shouldn't happen!","CWallPaperPreviewDlg()",MB_OK|MB_ICONERROR|MB_TASKMODAL|MB_SETFOREGROUND|MB_TOPMOST);}

public:
	CWallPaperPreviewDlg(HWND hWndParent);
	~CWallPaperPreviewDlg() {}

private:
	void				DoDataExchange		(CDataExchange*);
	BOOL				OnInitDialog		(void);
	BOOL				OnInitDialogEx		(UINT = -1,LPCSTR = NULL) {return(FALSE);}
	void				OnSysCommand		(UINT nID,LPARAM lParam);
	BOOL				OnQueryNewPalette	(void);
	void				OnPaletteChanged	(CWnd* pFocusWnd);
	void				OnSetFocus		(CWnd* pOldWnd);
	void				OnExit			(void) {CDialogEx::OnExit();}

	LONG				OnSetConfig		(UINT wParam,LONG lParam);
	LONG				OnPreviewMinmaximize(UINT wParam,LONG lParam);
	LONG				OnPreviewForceFocus	(UINT wParam,LONG lParam);
	LONG				OnPreviewEnabled	(UINT wParam,LONG lParam);
	LONG				OnPreviewDisabled	(UINT wParam,LONG lParam);

	HWND				m_hWndParent;
	BOOL				m_bForceFocus;
	CWallPaperConfig*	m_pConfig;
	CDialogHeader		m_wndHeader;
	char				m_szItem[_MAX_PATH+1];
	CDibCtrl			m_wndStaticDib;
	CWndLayered		m_wndLayered;

	DECLARE_MESSAGE_MAP()
};

#endif // _WALLPAPERPREVIEWDLG_H
