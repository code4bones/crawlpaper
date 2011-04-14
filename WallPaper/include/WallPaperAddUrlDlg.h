/*
	WallPaperAddUrlDlg.h
	Dialogo per l'aggiunta dell'url alla lista delle immagini.
	Luca Piergentili, 06/08/00
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
#ifndef _WALLPAPERADDURLDLG_H
#define _WALLPAPERADDURLDLG_H 1

#include "window.h"
#include "CComboBoxExt.h"
#include "CDialogEx.h"
#include "COleDropTargetEx.h"
#include "CWndLayered.h"
#include "WallPaperConfig.h"

/*
	CWallPaperAddUrlDlg
*/
class CWallPaperAddUrlDlg : public CDialogEx
{
	DECLARE_DYNCREATE(CWallPaperAddUrlDlg)

protected: // provide default constructor only for dynamic creation, private/protected to prevent it from being called from outside the class implementation
	CWallPaperAddUrlDlg() {::MessageBox(NULL,"PANIC! This shouldn't happen!","CWallPaperAddUrlDlg()",MB_OK|MB_ICONERROR|MB_TASKMODAL|MB_SETFOREGROUND|MB_TOPMOST);}

public:
	CWallPaperAddUrlDlg(HWND hWndParent,CWallPaperConfig* pConfig);
	~CWallPaperAddUrlDlg() {}

	CString	GetUrl			(void) const {return(m_strUrl);}

private:
	void		DoDataExchange		(CDataExchange* pDX);
	BOOL		OnInitDialog		(void);
	BOOL		OnInitDialogEx		(UINT = -1,LPCSTR = NULL) {return(FALSE);}
	void		OnOK				(void);
	void		OnCancel			(void);
	void		OnSelChangeComboUrl	(void);
	BOOL		OnComboUrl		(void);
	LONG		OnDropOle			(UINT wParam,LONG lParam);
	void		OnDropFiles		(void);

	CWallPaperConfig*	m_pConfig;
	CString			m_strUrl;
	CComboBoxExt		m_wndComboUrl;
	COleDropTargetEx	m_OleDropTarget;
	CWndLayered		m_wndLayered;

	DECLARE_MESSAGE_MAP()
};

#endif // _WALLPAPERADDURLDLG_H
