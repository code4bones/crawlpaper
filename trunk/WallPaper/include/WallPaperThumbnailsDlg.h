/*
	WallPaperThumbnailsDlg.h
	Dialogo per la generazione delle miniature.
	Luca Piergentili, 05/10/00
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
#ifndef _WALLPAPERTHUMBNAILSDLG_H
#define _WALLPAPERTHUMBNAILSDLG_H 1

#include "window.h"
#include "CConfig.h"
#include "CDialogEx.h"
#include "CDibCtrl.h"
#include "CListCtrlEx.h"
#include "CNodeList.h"
#include "CProgressBar.h"
#include "CStaticFilespec.h"
#include "CUrl.h"
#include "CWndLayered.h"
#include "WallPaperConfig.h"

#define IDS_DIALOG_THUMBNAILS_TITLE " Thumbnails "

// struttura per l'elemento della lista
struct THUMBNAIL {
	int	thumb_width;
	int	thumb_height;
	int	width;
	int	height;
	int	colors;
	int	bpp;
	char	url[MAX_URL+1];
	char	file[_MAX_FILEPATH+1];
	char	thumbnail[_MAX_FILEPATH+1];
};

// classe per la gestione della lista
class CThumbnailsList : public CNodeList
{
public:
	CThumbnailsList() : CNodeList() {}
	virtual ~CThumbnailsList() {CNodeList::DeleteAll();}

	void* Create(void)
	{
		THUMBNAIL* pData = new THUMBNAIL;
		return(pData);
	}
	
	void* Initialize(void* pVoid)
	{
		THUMBNAIL* pData = (THUMBNAIL*)pVoid;
		if(!pData)
			pData = (THUMBNAIL*)Create();
		if(pData)
			memset(pData,'\0',sizeof(THUMBNAIL));
		return(pData);
	}

	BOOL PreDelete(ITERATOR iter)
	{
		if((THUMBNAIL*)iter->data)
		{
			delete ((THUMBNAIL*)iter->data);
			iter->data = (THUMBNAIL*)NULL;
		}
		return(TRUE);
	}

	int Size(void) {return(sizeof(THUMBNAIL));}

#ifdef _DEBUG
	const char* Signature(void) {return("CThumbnailsList");}
#endif
};

/*
	CWallPaperThumbnailsDlg
*/
class CWallPaperThumbnailsDlg : public CDialogEx
{
	DECLARE_DYNCREATE(CWallPaperThumbnailsDlg)

protected: // provide default constructor only for dynamic creation, private/protected to prevent it from being called from outside the class implementation
	CWallPaperThumbnailsDlg() {::MessageBox(NULL,"PANIC! This shouldn't happen!","CWallPaperThumbnailsDlg()",MB_OK|MB_ICONERROR|MB_TASKMODAL|MB_SETFOREGROUND|MB_TOPMOST);}

public:
	CWallPaperThumbnailsDlg(HWND hWndParent);
	~CWallPaperThumbnailsDlg() {}

private:
	void			DoDataExchange			(CDataExchange* pDX);
	BOOL			OnInitDialog			(void);
	BOOL			OnInitDialogEx			(UINT = -1,LPCSTR = NULL) {return(FALSE);}
	BOOL			OnQueryNewPalette		(void);
	void			OnPaletteChanged		(CWnd* pFocusWnd);
	void			OnSetFocus			(CWnd* pOldWnd);
	void			OnOk					(void);
	void			OnExit				(void);
	BOOL			OnEscapeKey			(void) {ShowWindow(SW_MINIMIZE); return(TRUE);}

	void			OnButtonFrom			(void);
	void			OnRadioButtonFromList	(void);
	void			OnRadioButtonFromDir	(void);
	void			OnButtonInto			(void);
	void			OnRadioButtonIntoList	(void);
	void			OnRadioButtonIntoDir	(void);
	void			OnCheckThumbnailPreview	(void);

	static UINT	Thumbnails			(LPVOID lpVoid);
	UINT			Thumbnails			(void);

	HWND			m_hWndParent;
	CString		m_strFrom;
	CString		m_strPreviousFrom;
	int			m_nRadioFromList;
	int			m_nRadioFromDir;
	int			m_nGenerateFrom;
	CString		m_strInto;
	CString		m_strPreviousInto;
	int			m_nRadioIntoList;
	int			m_nRadioIntoDir;
	int			m_nGenerateInto;
	CProgressBar	m_wndProgressBar;
	UINT			m_nCancel;
	CStaticFilespec m_wndStaticFilespec;
	CListCtrlEx*	m_pwndPictureList;
	BOOL			m_bPicturePreview;
	CDibCtrl		m_wndStaticDib;
	CWallPaperConfig* m_pConfig;
	BOOL			m_bCloseDialog;
	CWndLayered	m_wndLayered;

	DECLARE_MESSAGE_MAP()
};

#endif // _WALLPAPERTHUMBNAILSDLG_H
