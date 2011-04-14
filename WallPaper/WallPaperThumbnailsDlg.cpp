/*
	WallPaperThumbnailsDlg.cpp
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
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include <string.h>
#include "strings.h"
#include "window.h"
#include "win32api.h"
#include "CBrowser.h"
#include "CConfig.h"
#include "CDialogEx.h"
#include "CDibCtrl.h"
#include "CDirDialog.h"
#include "CFindFile.h"
#include "CHtml.h"
#include "CImageFactory.h"
#include "CListCtrlEx.h"
#include "CNodeList.h"
#include "CProgressBar.h"
#include "CStaticFilespec.h"
#include "CUrl.h"
#include "CWndLayered.h"
#include "CWindowsVersion.h"
#include "WallPaperConfig.h"
#include "WallPaperMessages.h"
#include "WallPaperThumbnailsDlg.h"
#include "resource.h"

#include "traceexpr.h"
//#define _TRACE_FLAG	_TRFLAG_TRACEOUTPUT
//#define _TRACE_FLAG	_TRFLAG_NOTRACE
#define _TRACE_FLAG		_TRFLAG_NOTRACE
#define _TRACE_FLAG_INFO	_TRFLAG_NOTRACE
#define _TRACE_FLAG_WARN	_TRFLAG_NOTRACE
#define _TRACE_FLAG_ERR	_TRFLAG_NOTRACE

#if (defined(_DEBUG) && defined(_WINDOWS)) && (defined(_AFX) || defined(_AFXDLL))
#ifdef PRAGMA_MESSAGE_VERBOSE
  #pragma message("\t\t\t"__FILE__"("STR(__LINE__)"): using DEBUG_NEW macro")
#endif
  #define new DEBUG_NEW
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CWallPaperThumbnailsDlg,CDialogEx)
	ON_WM_QUERYNEWPALETTE()
	ON_WM_PALETTECHANGED()
	ON_WM_SETFOCUS()
	ON_BN_CLICKED(IDC_BUTTON_THUMBNAILS_FROM,OnButtonFrom)
	ON_BN_CLICKED(IDC_RADIO_THUMBNAILS_FROMLIST,OnRadioButtonFromList)
	ON_BN_CLICKED(IDC_RADIO_THUMBNAILS_FROMDIR,OnRadioButtonFromDir)
	ON_BN_CLICKED(IDC_BUTTON_THUMBNAILS_INTO,OnButtonInto)
	ON_BN_CLICKED(IDC_RADIO_THUMBNAILS_INTOLIST,OnRadioButtonIntoList)
	ON_BN_CLICKED(IDC_RADIO_THUMBNAILS_INTODIR,OnRadioButtonIntoDir)
	ON_BN_CLICKED(IDC_CHECK_THUMBNAILS_PREVIEW,OnCheckThumbnailPreview)
	ON_BN_CLICKED(IDOK,OnOk)
END_MESSAGE_MAP()

IMPLEMENT_DYNCREATE(CWallPaperThumbnailsDlg,CDialogEx)

/*
	DoDataExchange()
*/
void CWallPaperThumbnailsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX,IDC_EDIT_THUMBNAILS_FROM,m_strFrom);
	DDX_Radio(pDX,IDC_RADIO_THUMBNAILS_FROMLIST,m_nRadioFromList);
	DDX_Radio(pDX,IDC_RADIO_THUMBNAILS_FROMDIR,m_nRadioFromDir);
	DDX_Text(pDX,IDC_EDIT_THUMBNAILS_INTO,m_strInto);
	DDX_Radio(pDX,IDC_RADIO_THUMBNAILS_INTOLIST,m_nRadioIntoList);
	DDX_Radio(pDX,IDC_RADIO_THUMBNAILS_INTODIR,m_nRadioIntoDir);
	DDX_Check(pDX,IDC_CHECK_THUMBNAILS_PREVIEW,m_bPicturePreview);
}

/*
	CWallPaperThumbnailsDlg()
*/
CWallPaperThumbnailsDlg::CWallPaperThumbnailsDlg(HWND hWndParent)
: CDialogEx(	/*nTemplateID*/		IDD_DIALOG_THUMBNAILS,
			/*nToolbarID*/			0L,
			/*nStatusbarID*/		0L,
			/*nIconID*/			IDI_ICON_WALLPAPER,
			/*nMenuID*/			0L,
#ifdef _RESIZABLE_DIALOG_STYLE
			/*bAllowResize*/		FALSE,
#endif
			/*bAllowSnap*/			TRUE,
			/*bAllowDragAndDrop*/	FALSE,
			/*lpcszUniqueName*/		IDS_DIALOG_THUMBNAILS_TITLE,
			/*bAllowMultipleInstances*/TRUE,
			/*hWndParent*/			NULL)
{
	m_hWndParent      = hWndParent;
	m_strFrom         = "";
	m_strPreviousFrom = "";
	m_nRadioFromList  = 0;
	m_nRadioFromDir   = 1;
	m_nGenerateFrom   = 0;
	m_strInto         = "";
	m_strPreviousInto = "";
	m_nRadioIntoList  = 0;
	m_nRadioIntoDir   = 1;
	m_nGenerateInto   = 0;
	m_nCancel         = 0;
	m_bPicturePreview = TRUE;
	m_pConfig         = NULL;
}

/*
	OnInitDialog()
*/
BOOL CWallPaperThumbnailsDlg::OnInitDialog(void)
{
	// classe base
	if(!CDialogEx::OnInitDialog())
	{
		EndDialog(IDCANCEL);
		return(FALSE);
	}

	// modifica il menu di sistema
	CMenu* pSysmenu = GetSystemMenu(FALSE);
	if(pSysmenu)
	{
		pSysmenu->RemoveMenu(0,MF_BYPOSITION); // restore
		//pSysmenu->RemoveMenu(0,MF_BYPOSITION); // move
		pSysmenu->RemoveMenu(1,MF_BYPOSITION); // size
		pSysmenu->RemoveMenu(1,MF_BYPOSITION); // min
		pSysmenu->RemoveMenu(1,MF_BYPOSITION); // max
		pSysmenu->RemoveMenu(1,MF_BYPOSITION); // ---
		pSysmenu->RemoveMenu(1,MF_BYPOSITION); // close
		pSysmenu->RemoveMenu(1,MF_BYPOSITION); // ---

		CWindowsVersion winver;
		char* p = winver.IsRunningOnCartoons() ? "   " : "";
		char szMenuItem[32];
		_snprintf(szMenuItem,sizeof(szMenuItem)-1,"%s&Move",p);
		pSysmenu->ModifyMenu(0,MF_BYPOSITION|MF_STRING,SC_MOVE,szMenuItem);
		_snprintf(szMenuItem,sizeof(szMenuItem)-1,"%s&Close\tAlt+F4",p);
		pSysmenu->AppendMenu(MF_STRING,SC_CLOSE,szMenuItem);
		_snprintf(szMenuItem,sizeof(szMenuItem)-1,"%sMinimi&ze",p);
		pSysmenu->AppendMenu(MF_STRING,SC_MINIMIZE,szMenuItem);
		_snprintf(szMenuItem,sizeof(szMenuItem)-1,"%sMa&ximize",p);
		pSysmenu->AppendMenu(MF_STRING,SC_RESTORE,szMenuItem);

		DrawMenuBar();
	}

	// tooltip
	AddToolTip(IDC_EDIT_THUMBNAILS_FROM,IDS_TOOLTIP_THUMBNAILS_FROM);
	AddToolTip(IDC_BUTTON_THUMBNAILS_FROM,IDS_TOOLTIP_THUMBNAILS_SELECT_FROM);
	AddToolTip(IDC_RADIO_THUMBNAILS_FROMLIST,IDS_TOOLTIP_THUMBNAILS_FROM_LIST);
	AddToolTip(IDC_RADIO_THUMBNAILS_FROMDIR,IDS_TOOLTIP_THUMBNAILS_FROM_DIR);
	AddToolTip(IDC_EDIT_THUMBNAILS_INTO,IDS_TOOLTIP_THUMBNAILS_INTO);
	AddToolTip(IDC_BUTTON_THUMBNAILS_INTO,IDS_TOOLTIP_THUMBNAILS_SELECT_INTO);
	AddToolTip(IDC_RADIO_THUMBNAILS_INTOLIST,IDS_TOOLTIP_THUMBNAILS_INTO_LIST);
	AddToolTip(IDC_RADIO_THUMBNAILS_INTODIR,IDS_TOOLTIP_THUMBNAILS_INTO_DIR);
	AddToolTip(IDOK,IDS_TOOLTIP_THUMB);
	AddToolTip(IDC_CHECK_THUMBNAILS_PREVIEW,IDS_TOOLTIP_PREVIEW);
	AddToolTip(IDCANCEL,IDS_TOOLTIP_CLOSE);

	// ricava i puntatori dall'applicazione principale
	m_pConfig = NULL;
	m_pwndPictureList = NULL;
	if(m_hWndParent)
	{
		m_pwndPictureList = (CListCtrlEx*)::SendMessage(m_hWndParent,WM_GETPLAYLIST,0L,0L);
		m_pConfig = (CWallPaperConfig*)::SendMessage(m_hWndParent,WM_GETCONFIGURATION,0L,0L);
	}
	if(!m_pConfig || !m_pwndPictureList)
	{
		::MessageBoxResource(NULL,MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_RETRIEVE_CONFIGURATION);
		if(m_hWndParent)
			::PostMessage(m_hWndParent,WM_THUMBNAILS_DONE,0,0);
		EndDialog(IDCANCEL);
		return(FALSE);
	}

	// inizializza con i valori della configurazione
	m_strFrom.Format("%s",m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_THUMBFROM_KEY));
	m_strPreviousFrom.Format("%s",m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_THUMBFROM_KEY));
	m_nGenerateFrom = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_GENERATEFROM_KEY);
	m_strInto.Format("%s",m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_THUMBINTO_KEY));
	m_strPreviousInto.Format("%s",m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_THUMBINTO_KEY));
	m_nGenerateInto = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_GENERATEINTO_KEY);

	// status bar
	m_wndStaticFilespec.SubclassDlgItem(IDC_STATIC_THUMBNAILS_STATUS,this);
	m_wndStaticFilespec.SetPath(TRUE);

	// progress bar
	m_wndProgressBar.Attach(this->m_hWnd,IDC_THUMBNAILS_PROGRESSBAR);

	// preview
	m_wndStaticDib.SubclassDlgItem(IDC_THUMBNAILS_PICTURE_PREVIEW,this);

	// imposta con i defaults
	if(m_nGenerateFrom)
		OnRadioButtonFromDir();
	else
		OnRadioButtonFromList();
	
	if(m_nGenerateInto)
		OnRadioButtonIntoDir();
	else
		OnRadioButtonIntoList();

	// interfaccia utente
	SetWindowText(IDS_DIALOG_THUMBNAILS_TITLE);
	SetDlgItemText(IDC_STATIC_THUMBNAILS_STATUS,"");
	SetDlgItemText(IDC_STATIC_THUMBNAILS_GENERATION_TITLE,"");
	SetDlgItemText(IDC_STATIC_THUMBNAILS_GENERATION,"");
	m_wndProgressBar.SetPos(0);
	m_wndProgressBar.Hide();
	SetDlgItemText(IDC_TEXT_INFO,"");
	GetDlgItem(IDC_CHECK_THUMBNAILS_PREVIEW)->EnableWindow(TRUE);
	SetDlgItemText(IDCANCEL,"&Close");

	// inizializza il dialogo
	OnInitDialogEx();

	UpdateData(FALSE);

	// trasparenza
	if(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAYERED_KEY))
		m_wndLayered.SetLayer(this->m_hWnd,(BYTE)m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAYERED_VALUE_KEY));

	// mette in primo piano
	SetForegroundWindowEx(this->m_hWnd);

	return(TRUE);
}

/*
	OnQueryNewPalette()
*/
BOOL CWallPaperThumbnailsDlg::OnQueryNewPalette(void)
{
	m_wndStaticDib.SendMessage(WM_QUERYNEWPALETTE);
	return(CDialog::OnQueryNewPalette());
}

/*
	OnPaletteChanged()
*/
void CWallPaperThumbnailsDlg::OnPaletteChanged(CWnd* pFocusWnd)
{
	CDialog::OnPaletteChanged(pFocusWnd);
	m_wndStaticDib.SendMessage(WM_PALETTECHANGED,(WPARAM)pFocusWnd->GetSafeHwnd());
}

/*
	OnSetFocus()
*/
void CWallPaperThumbnailsDlg::OnSetFocus(CWnd* pOldWnd)
{
	CDialog::OnSetFocus(pOldWnd);
	m_wndStaticDib.SendMessage(WM_QUERYNEWPALETTE);
}

/*
	OnOk()
*/
void CWallPaperThumbnailsDlg::OnOk(void)
{
	BOOL bUpdated = TRUE;

	if(!UpdateData(TRUE))
		bUpdated = FALSE;

	// controlli sui campi di input
	if(bUpdated)
	{
		if(m_strFrom.IsEmpty())
		{
			::MessageBoxResourceEx(this->m_hWnd,MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_INVALID_DIRECTORY,"[none]");
			bUpdated = FALSE;
		}

		if(m_nGenerateFrom==1 && ::GetFileAttributes(m_strFrom)==0xFFFFFFFF)
		{
			::MessageBoxResourceEx(this->m_hWnd,MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_INVALID_DIRECTORY,m_strFrom);
			bUpdated = FALSE;
		}
	}

	if(bUpdated)
	{
		if(m_strInto.IsEmpty())
		{
			::MessageBoxResourceEx(this->m_hWnd,MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_INVALID_DIRECTORY,"[none]");
			bUpdated = FALSE;
		}

		if(m_nGenerateFrom==1 && m_strInto=="<original location>")
			m_strInto = m_strFrom;

		if(m_nGenerateFrom==1 && ::GetFileAttributes(m_strInto)==0xFFFFFFFF)
		{
			::MessageBoxResourceEx(this->m_hWnd,MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_INVALID_DIRECTORY,m_strInto);
			bUpdated = FALSE;
		}
	}

	// lancia il thread UI per i thumbnails
	if(bUpdated)
	{
		GetDlgItem(IDOK)->EnableWindow(FALSE);
		SetDlgItemText(IDCANCEL,"&Cancel");
		AfxBeginThread(Thumbnails,this,m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PRIORITY_THREAD_KEY));
	}
}

/*
	OnExit()
*/
void CWallPaperThumbnailsDlg::OnExit(void)
{
	UpdateData(TRUE);

	// aggiorna i valori dei campi nella cofigurazione
	if(strcmp(m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_THUMBFROM_KEY),m_strFrom)!=0 && m_strFrom[0]!='<')
		m_pConfig->UpdateString(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_THUMBFROM_KEY,m_strFrom);
	if(strcmp(m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_THUMBINTO_KEY),m_strInto)!=0 && m_strInto[0]!='<')
		m_pConfig->UpdateString(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_THUMBINTO_KEY,m_strInto);
	if(m_nGenerateFrom!=(int)m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_GENERATEFROM_KEY))
		m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_GENERATEFROM_KEY,m_nGenerateFrom);
	if(m_nGenerateInto!=(int)m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_GENERATEINTO_KEY))
		m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_GENERATEINTO_KEY,m_nGenerateInto);

	// imposta il flag e cambia il prompt del bottone
	m_nCancel = 1;
	CString cText;
	GetDlgItemText(IDCANCEL,cText);
	if(cText=="&Close")
	{
		// termina inviando il messaggio alla finestra principale
		if(m_hWndParent)
			::PostMessage(m_hWndParent,WM_THUMBNAILS_DONE,0,0);
		CDialogEx::OnExit();
	}
}

/*
	OnButtonFrom()
*/
void CWallPaperThumbnailsDlg::OnButtonFrom(void)
{
	LPCSTR lpcszDir;
	
	UpdateData(TRUE);
	
	if(m_strFrom.IsEmpty() || m_strFrom[0]=='<')
		lpcszDir = m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_THUMBFROM_KEY);
	else
		lpcszDir = m_strFrom;

	CDirDialog dlg(lpcszDir,
				"Open Directory...",
				"Select source folder for pictures:"
				);

	if(dlg.DoModal()==IDOK)
	{
		m_strFrom.Format("%s",dlg.GetPathName());
		m_strPreviousFrom.Format("%s",dlg.GetPathName());
		UpdateData(FALSE);
	}
}

/*
	OnRadioButtonFromList()
*/
void CWallPaperThumbnailsDlg::OnRadioButtonFromList(void)
{
	m_strFrom = "<current picture list>";
	GetDlgItem(IDC_EDIT_THUMBNAILS_FROM)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_THUMBNAILS_FROM)->EnableWindow(FALSE);
	m_nRadioFromList = 0;
	m_nRadioFromDir = 1;
	m_nGenerateFrom = 0;
	UpdateData(FALSE);
}

/*
	OnRadioButtonFromDir()
*/
void CWallPaperThumbnailsDlg::OnRadioButtonFromDir(void)
{
	m_strFrom = m_strPreviousFrom.IsEmpty() ? m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_THUMBFROM_KEY) : m_strPreviousFrom;
	GetDlgItem(IDC_EDIT_THUMBNAILS_FROM)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_THUMBNAILS_FROM)->EnableWindow(TRUE);
	m_nRadioFromList = 1;
	m_nRadioFromDir = 0;
	m_nGenerateFrom = 1;
	UpdateData(FALSE);
}

/*
	OnButtonInto()
*/
void CWallPaperThumbnailsDlg::OnButtonInto(void)
{
	LPCSTR lpcszDir;

	UpdateData(TRUE);

	if(m_strInto.IsEmpty() || m_strInto[0]=='<')
		lpcszDir = m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_THUMBINTO_KEY);
	else
		lpcszDir = m_strInto;

	CDirDialog dlg(lpcszDir,
				"Open Directory...",
				"Select destination folder for thumbnails:"
				);

	if(dlg.DoModal()==IDOK)
	{
		m_strInto.Format("%s",dlg.GetPathName());
		m_strPreviousInto.Format("%s",dlg.GetPathName());
		UpdateData(FALSE);
	}
}

/*
	OnRadioButtonIntoList()
*/
void CWallPaperThumbnailsDlg::OnRadioButtonIntoList(void)
{
	m_strInto = "<original location>";
	GetDlgItem(IDC_EDIT_THUMBNAILS_INTO)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_THUMBNAILS_INTO)->EnableWindow(FALSE);
	m_nRadioIntoList = 0;
	m_nRadioIntoDir = 1;
	m_nGenerateInto = 0;
	UpdateData(FALSE);
}

/*
	OnRadioButtonIntoDir()
*/
void CWallPaperThumbnailsDlg::OnRadioButtonIntoDir(void)
{
	if(m_strInto == "<original location>")
		m_strInto = ".\\";
	m_strInto = m_strPreviousInto.IsEmpty() ? m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_THUMBINTO_KEY) : m_strPreviousInto;
	GetDlgItem(IDC_EDIT_THUMBNAILS_INTO)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_THUMBNAILS_INTO)->EnableWindow(TRUE);
	m_nRadioIntoList = 1;
	m_nRadioIntoDir = 0;
	m_nGenerateInto = 1;
	UpdateData(FALSE);
}

/*
	OnCheckThumbnailPreview()
*/
void CWallPaperThumbnailsDlg::OnCheckThumbnailPreview(void)
{
	m_bPicturePreview = !m_bPicturePreview;
	if(!m_bPicturePreview)
		m_wndStaticDib.Unload();
}

/*
	Thumbnails()

	Wrapper per il thread.
*/
UINT CWallPaperThumbnailsDlg::Thumbnails(LPVOID lpVoid)
{
	UINT nRet = 0;
	CWallPaperThumbnailsDlg* This = (CWallPaperThumbnailsDlg*)lpVoid;
	
	if(This)
		nRet = This->Thumbnails();
	
	return(nRet);
}

/*
	Thumbnails()

	Thread per la generazione dei thumbnails.
*/
UINT CWallPaperThumbnailsDlg::Thumbnails(void)
{
	CUrl url;
	char szItem[_MAX_PATH+1];
	char szThumbnail[_MAX_PATH+1];
	char szFileName[_MAX_PATH+1];
	char szPathName[_MAX_PATH+1];
	char szExt[_MAX_EXT+1];
	char szBuffer[1024];
	CFindFile findfile;

	char szLibraryName[_MAX_PATH+1];
	strcpyn(szLibraryName,m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_CURRENTLIBRARY_KEY),sizeof(szLibraryName));
	CImageFactory ImageFactory;
	CImage* pImage = ImageFactory.Create(szLibraryName,sizeof(szLibraryName));

	CRect thumbnailsRect(0,0,m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_X_KEY),m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_Y_KEY));
	char* p;
	int nTotItem;
	THUMBNAIL* thumbnail;
	CThumbnailsList thumblist;
	ITERATOR iter;
	int nThumbnailProg = 0;
	BOOL bThumbnail = FALSE;
	
	// interfaccia utente
	SetWindowText(IDS_DIALOG_THUMBNAILS_TITLE);
	SetDlgItemText(IDC_STATIC_THUMBNAILS_STATUS,"");
	SetDlgItemText(IDC_STATIC_THUMBNAILS_GENERATION_TITLE,"");
	SetDlgItemText(IDC_STATIC_THUMBNAILS_GENERATION,"");
	GetDlgItem(IDC_CHECK_THUMBNAILS_PREVIEW)->EnableWindow(FALSE);
	m_wndProgressBar.SetPos(0);
	m_wndProgressBar.Hide();

	// riempie la lista con i nomi file per le immagini da trattare

	// seleziona le immagini dalla lista corrente
	if(m_nRadioFromList==0)
	{
		// interfaccia utente
		SetDlgItemText(IDC_STATIC_THUMBNAILS_STATUS,"Loading files names from current picture's list...");
		
		// scorre la lista
		if((nTotItem = m_pwndPictureList->GetItemCount()) > 0)
			for(int nItem = 0; nItem < nTotItem; nItem++)
			{
				// ricava il pathname completo per l'immagine
				memset(szItem,'\0',sizeof(szItem));
				m_pwndPictureList->GetItemText(nItem,3,szItem,sizeof(szItem));
				int n = strlen(szItem);
				*(szItem+n) = url.IsUrl(szItem) ? '/' : '\\';
				strcpyn(szPathName,szItem,sizeof(szPathName));
				strrtrim(szPathName);
				m_pwndPictureList->GetItemText(nItem,0,szItem+n+1,sizeof(szItem)-n);
				strrtrim(szItem);

				// controlla che non si tratti di un url
				if(!url.IsUrl(szItem))
				{
					// controlla che il file esista
					if(findfile.Exist(szItem))
					{
						thumbnail = new THUMBNAIL;
						if(thumbnail)
						{
							memset(thumbnail,'\0',sizeof(THUMBNAIL));
							strcpyn(thumbnail->file,szItem,_MAX_FILEPATH+1);
							thumblist.Add(thumbnail);
						}
					}
				}
				else
				{
					::MessageBoxResourceEx(this->m_hWnd,MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_INVALID_THUMBNAIL_LOCATION,szItem);
				}
			}
	}
	// seleziona le immagini dalla directory
	else
	{
		// interfaccia utente
		SetDlgItemText(IDC_STATIC_THUMBNAILS_STATUS,"Loading files names from directory...");

		// normalizza la directory di input
		if(m_strFrom.IsEmpty())
			m_strFrom = ".";
		strcpyn(szPathName,m_strFrom,sizeof(szPathName));
		::EnsureBackslash(szPathName,sizeof(szPathName));

		// ricerca nella directory specificata i tipi attualmente supportati
		int nTot;		
		IMAGETYPE* picttype;
		while((picttype = pImage->EnumReadableImageFormats())!=(LPIMAGETYPE)NULL)
		{
			_snprintf(szBuffer,sizeof(szBuffer)-1,"Looking for <%s> files...",picttype->ext);
			SetDlgItemText(IDC_STATIC_THUMBNAILS_STATUS,szBuffer);
			nTot = 0;
			_snprintf(szExt,sizeof(szExt)-1,"*%s",picttype->ext);
			while((p = (LPSTR)findfile.FindEx(szPathName,szExt,m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_RECURSE_SUBDIR_KEY)))!=NULL)
			{
				_snprintf(szBuffer,sizeof(szBuffer)-1,"Looking for <%s> files... (%d found)",picttype->ext,++nTot);
				SetDlgItemText(IDC_STATIC_THUMBNAILS_STATUS,szBuffer);

				thumbnail = new THUMBNAIL;
				if(thumbnail)
				{
					memset(thumbnail,'\0',sizeof(THUMBNAIL));
					strcpyn(thumbnail->file,p,_MAX_FILEPATH+1);
					thumblist.Add(thumbnail);
				}
			}
		}
	}

	CString strInfo;
	int nTotFiles = thumblist.Count();
	int nCurrentFile = 0;

	// per ognuno dei files presenti nella lista
	if((iter = thumblist.First())!=(ITERATOR)NULL)
	{
		m_wndProgressBar.SetRange(0,100);
		m_wndProgressBar.SetStep(1);
		m_wndProgressBar.SetPos(0);
		m_wndProgressBar.Show();

		do
		{
			::PeekAndPump();
			if(m_nCancel)
				goto done;

			thumbnail = (THUMBNAIL*)iter->data;
			if(thumbnail)
			{
				p = strrchr(thumbnail->file,'\\');
				strcpyn(szFileName,p ? p+1 : thumbnail->file,sizeof(szFileName));
				
				// interfaccia utente
				strInfo.Format(" %s (%d of %d)",szFileName,nCurrentFile+1,nTotFiles);
				SetWindowText(strInfo);
				strInfo.Format("Loading:\n%s",thumbnail->file);
				SetDlgItemText(IDC_STATIC_THUMBNAILS_STATUS,strInfo);
				SetDlgItemText(IDC_STATIC_THUMBNAILS_GENERATION_TITLE,"Thumbnails generation:");
				strInfo.Format("%d of %d (%d%%)",nCurrentFile+1,nTotFiles,(100 * (nCurrentFile+1))/nTotFiles);
				SetDlgItemText(IDC_STATIC_THUMBNAILS_GENERATION,strInfo);

				// generazione thumbnails nella stessa directory delle immagini
				if(m_nRadioIntoList==0)
				{
					strcpyn(szPathName,thumbnail->file,sizeof(szPathName));
					p = strrchr(szPathName,'\\');
					if(p)
						*p = '\0';
					else
						strcpyn(szPathName,".",sizeof(szPathName));
				}
				// generazione miniature nella directory selezionata
				else
				{
					if(m_strInto.IsEmpty())
						strcpyn(szPathName,".",sizeof(szPathName));
					else
						strcpyn(szPathName,m_strInto,sizeof(szPathName));
				
					::RemoveBackslash(szPathName);
				}

				// compone il nome della miniatura a seconda della regola corrente
				switch(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_NAMING_KEY))
				{
					// prefisso di default
					case 0:
					default:
						_snprintf(szThumbnail,sizeof(szThumbnail)-1,"%s\\thumbnail.%s",szPathName,szFileName);
						break;

					// progressivo numerico
					case 1:
						p = strrchr(szFileName,'.');
						if(p)
							strcpyn(szExt,p,sizeof(szExt));
						else
							strcpyn(szExt,"",sizeof(szExt));
						_snprintf(szThumbnail,sizeof(szThumbnail)-1,"%s\\%05d%s",szPathName,++nThumbnailProg,szExt);
						break;

					// prefisso
					case 2:
						_snprintf(szThumbnail,sizeof(szThumbnail)-1,"%s\\%s%s",szPathName,m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_NAME_KEY),szFileName);
						break;

					// suffisso
					case 3:
						p = strrchr(szFileName,'.');
						if(p)
						{
							strcpyn(szExt,p,sizeof(szExt));
							*p = '\0';
						}
						else
							strcpyn(szExt,"",sizeof(szExt));
						_snprintf(szThumbnail,sizeof(szThumbnail)-1,"%s\\%s%s%s",szPathName,szFileName,m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_NAME_KEY),szExt);
						break;
				}

				// interfaccia utente
				nCurrentFile++;
				m_wndProgressBar.SetPos((100 * nCurrentFile)/nTotFiles);

				bThumbnail = FALSE;

				// carica l'immagine e genera la miniatura
				if(pImage->Load(thumbnail->file))
				{
					char* pExt = (char*)m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_FORMAT_KEY);
					if(strcmp(pExt,"*")==0)
					{
						if((p = strrchr(szThumbnail,'.'))!=NULL)
							strcpyn(szExt,p,sizeof(szExt));
						else
							memset(szExt,'\0',sizeof(szExt));
					}
					else
					{
						_snprintf(szExt,sizeof(szExt)-1,".%s",pExt);
						if((p = strrchr(szThumbnail,'.'))!=NULL)
							if(strcmp(p,szExt)!=0)
							{
								*p = '\0';
								strcat(szThumbnail,szExt);
							}
					}

					BOOL bWritable = FALSE;
					LPIMAGETYPE imagetype;
					while((imagetype = pImage->EnumWritableImageFormats())!=(LPIMAGETYPE)NULL)
					{
						if(stricmp(imagetype->ext,szExt)==0)
							bWritable = TRUE;
					}
					if(!bWritable)
					{
						::MessageBoxResourceEx(this->m_hWnd,MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_INVALID_GRAPHICS_FORMAT,szExt,pImage->GetLibraryName());
						goto done;
					}

					if(m_bPicturePreview)
					{
						if(m_wndStaticDib.Load(pImage))
						{
							char* p = strrchr(thumbnail->file,'\\');
							char szImageInfo[_MAX_PATH+1];
							
							char szMemUsed[32];
							strsize(szMemUsed,sizeof(szMemUsed),pImage->GetMemUsed());

							int nColors = pImage->GetNumColors();

							_snprintf(szImageInfo,
									sizeof(szImageInfo)-1,
									"%s\n\n%d x %d pixels\n%d%s colors x %d bpp\n%s required",
									(p && p+1) ? p+1 : thumbnail->file,
									pImage->GetWidth(),
									pImage->GetHeight(),
									(nColors > 256 || nColors==0) ? 16 : nColors,
									(nColors > 256 || nColors==0) ? "M" : "",
									pImage->GetBPP(),
									szMemUsed
									);

							SetDlgItemText(IDC_TEXT_INFO,szImageInfo);
						}
						else
						{
							m_wndStaticDib.Unload();
							SetDlgItemText(IDC_TEXT_INFO,"");
						}
					}
					
					strInfo.Format("Generating:\n%s",szThumbnail);
					SetDlgItemText(IDC_STATIC_THUMBNAILS_STATUS,strInfo);

					thumbnail->width = thumbnail->thumb_width = pImage->GetWidth();
					thumbnail->height = thumbnail->thumb_height = pImage->GetHeight();
					thumbnail->colors = pImage->GetNumColors();
					thumbnail->bpp = pImage->GetBPP();
					
					if(thumbnailsRect.right > 0 && thumbnailsRect.bottom > 0)
						if(thumbnail->width > thumbnailsRect.right || thumbnail->height > thumbnailsRect.bottom)
						{
							pImage->Stretch(thumbnailsRect);
							thumbnail->thumb_width  = pImage->GetWidth();
							thumbnail->thumb_height = pImage->GetHeight();
						}

					bThumbnail = pImage->Save(szThumbnail,szExt);
				}

				// aggiorna l'elemento della lista con il nome del thumbnail
				strcpyn(thumbnail->thumbnail,bThumbnail ? szThumbnail : "",_MAX_FILEPATH+1);
			}

			iter = thumblist.Next(iter);
		
		} while(iter!=(ITERATOR)NULL);

		if(m_bPicturePreview)
			m_wndStaticDib.Unload();

		//
		if(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_GENERATETABLE_KEY))
		{
			char szHtmlReport[_MAX_PATH+1];
			_snprintf(szHtmlReport,sizeof(szHtmlReport)-1,"%s",m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_HTMLFILE_KEY));

			// interfaccia utente
			strInfo.Format("Generating HTML table:\n%s",szHtmlReport);
			SetDlgItemText(IDC_STATIC_THUMBNAILS_STATUS,strInfo);

			// imposta il layout per il report
			HTMLLAYOUT htmllayout;
			htmllayout.font.type  = "verdana";
			htmllayout.font.size  = 1;
			htmllayout.font.color = NULL;
			htmllayout.align      = HTMLALIGN_CENTER;
			htmllayout.style      = HTMLSTYLE_NONE;
			htmllayout.border     = 0;
			htmllayout.width      = 0;

			HTMLTABLELAYOUT htmltablelayout;
			memcpy(&(htmltablelayout.htmllayout),&htmllayout,sizeof(HTMLLAYOUT));
			htmltablelayout.title       = (char*)m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_TABLETITLE_KEY);
			htmltablelayout.border      = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_TABLEBORDER_KEY);
			htmltablelayout.cellpadding = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_TABLEPADDING_KEY);
			htmltablelayout.cellspacing = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_TABLESPACING_KEY);
			htmltablelayout.width       = m_pConfig->GetNumber (WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_TABLEWIDTH_KEY);

			CHtml html;
			
			// crea il file HTML per il report
			if(!html.Open(szHtmlReport))
			{
				::MessageBoxResourceEx(this->m_hWnd,MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_OPENING_HTML_REPORT,szHtmlReport);
				goto done;
			}
			
			// header HTML
			char szMeta[512];
			_snprintf(szMeta,sizeof(szMeta)-1,"<meta name=\"generator\" content=\"%s, %s\">",WALLPAPER_PROGRAM_TITLE,m_pConfig->GetString(WALLPAPER_INSTALL_KEY,WALLPAPER_WEBSITE_KEY));
			html.Header(m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_HTMLTITLE_KEY),"<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 3.2//EN\">",szMeta);

			// header della tabella
			html.TableOpen(&htmltablelayout);

			int nTotCols = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_TABLECOLS_KEY);
			int nCurrentCol = 0;

			char* pImageUrl = (char*)m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_IMAGEURL_KEY);
			char* pThumbUrl = (char*)m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_THUMBURL_KEY);
			int nTotRows = 0;

			if((iter = thumblist.First())!=(ITERATOR)NULL)
			{
				do
				{
					pImageUrl = (char*)m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_IMAGEURL_KEY);
					pThumbUrl = (char*)m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_THUMBURL_KEY);

					thumbnail = (THUMBNAIL*)iter->data;
					if(thumbnail)
					{
						if(strcmp(thumbnail->thumbnail,"")!=0)
						{
							char* p = strrchr(thumbnail->file,'\\');
							if(p)
							{
								p++;
								if(!*p)
									p = NULL;
							}
							if(!p)
								p = thumbnail->file;

							// base url
							if(strcmp(pImageUrl,"")==0)
								pImageUrl = url.LocalFileToUrl(thumbnail->file,szFileName,sizeof(szFileName));
							else
							{
								char* p = strrchr(thumbnail->file,'\\');
								if(p)
									p++;
								else
									p = thumbnail->file;
								_snprintf(szFileName,sizeof(szFileName)-1,"%s%s",pImageUrl,p);
								pImageUrl = szFileName;
							}
							if(strcmp(pThumbUrl,"")==0)
								pThumbUrl = url.LocalFileToUrl(thumbnail->thumbnail,szThumbnail,sizeof(szThumbnail));
							else
							{
								char* p = strrchr(thumbnail->thumbnail,'\\');
								if(p)
									p++;
								else
									p = thumbnail->thumbnail;
								_snprintf(szThumbnail,sizeof(szThumbnail)-1,"%s%s",pThumbUrl,p);
								pThumbUrl = szThumbnail;
							}
							
							char szFileSize[32];
							strsize(szFileSize,sizeof(szFileSize),(double)::GetFileSizeExt(thumbnail->file));

							_snprintf(szBuffer,
									sizeof(szBuffer)-1,
									"<a href=\"%s\"><img border=0 width=%d height=%d alt=\"%s\" src=\"%s\"></a><br><br>%s<br>%d x %d x %d%s colors<br>%s",
									pImageUrl,
									thumbnail->thumb_width,
									thumbnail->thumb_height,
									thumbnail->file,
									pThumbUrl,
									p,
									thumbnail->width,
									thumbnail->height,
									(thumbnail->colors > 256 || thumbnail->colors <= 0) ? 16 : thumbnail->colors,
									(thumbnail->colors > 256 || thumbnail->colors <= 0) ? "M" : "",
									szFileSize
									);
						}
						else
						{
							_snprintf(szBuffer,
									sizeof(szBuffer)-1,
									"<a href=\"%s\">%s</a><br>(invalid file type)",
									thumbnail->file,
									thumbnail->file
									);
						}

						if(nCurrentCol >= nTotCols)
						{
							html.TableCloseRow();
							nCurrentCol = 0;
						}

						if(nCurrentCol==0)
						{
							html.TableOpenRow();
							nTotRows++;
						}
						
						html.TablePutCol(szBuffer,&htmllayout);

						nCurrentCol++;
					}
					
					iter = thumblist.Next(iter);
				
				} while(iter!=(ITERATOR)NULL);
			
				if(nCurrentCol < nTotCols)
				{
					if(nTotRows > 1)
						while(nCurrentCol++ < nTotCols)
							html.TablePutCol("&nbsp;",&htmllayout);
					html.TableCloseRow();
				}
			}

			html.TableClose();

			// footer
			_snprintf(szBuffer,sizeof(szBuffer)-1,WALLPAPER_AUTHOR_HTML_COPYRIGHT,m_pConfig->GetString(WALLPAPER_INSTALL_KEY,WALLPAPER_AUTHOREMAIL_KEY));
			html.NewLine();
			html.FormattedLine(	"<br>"
							"<hr>"
							"<font face=\"verdana\" size=\"1\">"
							"<a href=\"%s\">%s</a>"
							"<br>"
							"%s"
							"</font>",
							m_pConfig->GetString(WALLPAPER_INSTALL_KEY,WALLPAPER_WEBSITE_KEY),
							WALLPAPER_PROGRAM_TITLE,
							szBuffer
							);
			html.NewLine();
			html.Footer();
			html.Close();

			if(m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_OPENREPORT_KEY))
			{
				CBrowser browser(m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_PREFERRED_INTERNET_BROWSER_KEY));
				browser.Browse(szHtmlReport);
			}
		}
	}
	else
		::MessageBoxResource(this->m_hWnd,MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_NOPICTURES);
done:

	if(m_bPicturePreview)
		m_wndStaticDib.Unload();

	// interfaccia utente
	SetWindowText(m_nCancel!=0 ? " cancelled" : " done");
	SetDlgItemText(IDC_STATIC_THUMBNAILS_STATUS,"");
	SetDlgItemText(IDC_STATIC_THUMBNAILS_GENERATION_TITLE,"");
	SetDlgItemText(IDC_STATIC_THUMBNAILS_GENERATION,"");
	m_wndProgressBar.SetPos(0);
	m_wndProgressBar.Hide();
	SetDlgItemText(IDC_TEXT_INFO,"");
	GetDlgItem(IDC_CHECK_THUMBNAILS_PREVIEW)->EnableWindow(TRUE);

	GetDlgItem(IDOK)->EnableWindow(TRUE);
	SetDlgItemText(IDCANCEL,"&Close");

	m_nCancel = 0;
	
	return(0);
}
