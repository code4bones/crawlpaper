/*
	WallPaperMP3TagDlg.cpp
	Luca Piergentili, 05/05/04
	Dialogo per l'editor dei tags mp3.
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
#ifndef _WALLPAPERMP3TAGDLG_H
#define _WALLPAPERMP3TAGDLG_H 1

#include <string.h>
#include "strings.h"
#include "window.h"
#include "win32api.h"
#include "CDialogEx.h"
#include "CComboBoxExt.h"
#include "CIconStatic.h"
#include "CWndLayered.h"
#include "CMP3Info.h"
#include "CId3Lib.h"
#include "CFindFile.h"
#include "WallPaperConfig.h"

#define IDS_DIALOG_MP3TAG_TITLE " MP3 Tag Editor "
#define IDS_DIALOG_MP3TAG_TITLE_READ_ONLY " MP3 Tag Editor - (read only) "

/*
	CWallPaperMP3TagDlg
*/
class CWallPaperMP3TagDlg : public CDialogEx
{
	DECLARE_DYNCREATE(CWallPaperMP3TagDlg)

protected: // provide default constructor only for dynamic creation, private/protected to prevent it from being called from outside the class implementation
	CWallPaperMP3TagDlg() {::MessageBox(NULL,"PANIC! This shouldn't happen!","CWallPaperMP3TagDlg()",MB_OK|MB_ICONERROR|MB_TASKMODAL|MB_SETFOREGROUND|MB_TOPMOST);}

public:
	CWallPaperMP3TagDlg(HWND hWndParent);
	~CWallPaperMP3TagDlg() {}

	void				SetFileName		(LPCSTR lpcszFileName) {m_strFilename.Format("%s",lpcszFileName);}

	void				DoDataExchange		(CDataExchange*);
	BOOL				OnInitDialog		(void);
	BOOL				OnInitDialogEx		(UINT = -1,LPCSTR = NULL) {return(FALSE);}
	void				OnOK				(void);
	void				OnCancel			(void);
	void				OnButtonPlayCurrent	(void);
	LRESULT			OnMp3TagEditorExit	(WPARAM,LPARAM);
	BOOL				OnVirtualKey		(WPARAM wVirtualKey,BOOL bShift,BOOL bCtrl,BOOL bAlt);
	LRESULT			OnTooltipCallback	(WPARAM wParam,LPARAM lParam);
	void				OnSetTags			(void);
	void				OnClearTags		(void);
	BOOL				OnEscapeKey		(void) {OnCancel(); return(TRUE);}
	void				OnDropFiles		(void);
	LRESULT			OnLoadItem		(UINT wParam,LONG lParam);
	LRESULT			OnSetParentWindow	(UINT wParam,LONG lParam);
	void				OnButtonBrowse		(void);
	void				OnLoadPrev		(void);
	void				OnLoadNext		(void);
	void				OnCheckAutoSave	(void);
	BOOL				LoadFile			(LPCSTR lpcszFileName);
	int				LoadDirectory		(LPCSTR lpcszDirectory = NULL);

	inline void		OnEnChangeTitle	(void) {OnEnChangeTags();}
	inline void		OnEnChangeArtist	(void) {OnEnChangeTags();}
	inline void		OnEnChangeAlbum	(void) {OnEnChangeTags();}
	inline void		OnEnChangeYear		(void) {OnEnChangeTags();}
	LRESULT			OnEnChangeGenre	(WPARAM /*wParam*/,LPARAM /*lParam*/) {OnEnChangeTags(); return(0L);}
	inline void		OnEnChangeGenre	(void) {OnEnChangeTags();}
	inline void		OnEnChangeTrack	(void) {OnEnChangeTags();}
	inline void		OnEnChangeComment	(void) {OnEnChangeTags();}
	inline void		OnEnChangeTags		(void) {m_bDirty = TRUE;	GetDlgItem(IDOK)->EnableWindow(m_bDirty);}

private:
	HWND				m_hWndParent;
	HWND				m_hParentWindow;
	CWallPaperConfig*	m_pConfig;
	char				m_szFileName[_MAX_FILEPATH+1];
	CString			m_strFilename;
	CString			m_strTitle;
	CString			m_strArtist;
	CString			m_strAlbum;
	CString			m_strYear;
	CString			m_strGenre;
	CString			m_strTrack;
	CString			m_strComment;
	CString			m_strInfo;
	BOOL				m_bReadOnly;
	CComboBoxExt		m_wndComboGenre;
	CWndLayered		m_wndLayered;
	CMP3Info			m_MP3Info;
	CId3Lib			m_Id3Lib;
	CIconStatic		m_ctrlProperties;
	CIconStatic		m_ctrlMP3Info;
	CFindFile			m_FindFile;
	int				m_nFileCount;
	int				m_nCurrentFile;
	BOOL				m_bAutoSaveTags;
	BOOL				m_bDirty;

	DECLARE_MESSAGE_MAP()
};

#endif // _WALLPAPERMP3TAGDLG_H
