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
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include "window.h"
#include "win32api.h"
#include <string.h>
#include "strings.h"
#include "CArchive.h"
#include "CDialogEx.h"
#include "CComboBoxExt.h"
#include "CIconStatic.h"
#include "CWndLayered.h"
#include "CWindowsVersion.h"
#include "mmaudio.h"
#include "CMP3Info.h"
#include "CId3Lib.h"
#include "CDirDialog.h"
#include "CFileDialogEx.h"
#include "WallPaperMessages.h"
#include "WallPaperConfig.h"
#include "WallPaperPlayerDlg.h"
#include "WallPaperMP3TagDlg.h"
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

BEGIN_MESSAGE_MAP(CWallPaperMP3TagDlg,CDialogEx)
	ON_BN_CLICKED(IDOK,OnOK)
	ON_BN_CLICKED(IDCANCEL,OnCancel)
	ON_BN_CLICKED(IDC_BUTTON_PLAYCURRENT,OnButtonPlayCurrent)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR,OnClearTags)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE,OnButtonBrowse)
	ON_BN_CLICKED(IDC_BUTTON_PREV,OnLoadPrev)
	ON_BN_CLICKED(IDC_BUTTON_NEXT,OnLoadNext)
	ON_BN_CLICKED(IDC_CHECK_AUTOSAVE,OnCheckAutoSave)
	
	ON_EN_CHANGE(IDC_EDIT_MP3TAG_TITLE,OnEnChangeTitle)
	ON_EN_CHANGE(IDC_EDIT_MP3TAG_ARTIST,OnEnChangeArtist)
	ON_EN_CHANGE(IDC_EDIT_MP3TAG_ALBUM,OnEnChangeAlbum)
	ON_EN_CHANGE(IDC_EDIT_MP3TAG_YEAR,OnEnChangeYear)
	ON_CBN_SELCHANGE(IDC_COMBO_MP3TAG_GENRE,OnEnChangeGenre)
	ON_MESSAGE(WM_NOTIFY_CTRL,OnEnChangeGenre)
	ON_EN_CHANGE(IDC_EDIT_MP3TAG_TRACK,OnEnChangeTrack)
	ON_EN_CHANGE(IDC_EDIT_MP3TAG_COMMENT,OnEnChangeComment)

	ON_MESSAGE(WM_TOOLTIP_CALLBACK,OnTooltipCallback)
	ON_MESSAGE(WM_LOADITEM,OnLoadItem)
	ON_MESSAGE(WM_SETPARENTWINDOW,OnSetParentWindow)

	ON_MESSAGE(WM_MP3TAGEDITOR_EXIT,OnMp3TagEditorExit)
END_MESSAGE_MAP()

IMPLEMENT_DYNCREATE(CWallPaperMP3TagDlg,CDialogEx)

/*
	DoDataExchange()
*/
void CWallPaperMP3TagDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_STATIC_PROPERTIES,m_ctrlProperties);
	DDX_Control(pDX,IDC_STATIC_MP3INFO,m_ctrlMP3Info);
	DDX_Text(pDX,IDC_EDIT_MP3TAG_FILENAME,m_strFilename);

	DDX_Text(pDX,IDC_EDIT_MP3TAG_TITLE,m_strTitle);
	DDV_MaxChars(pDX,m_strTitle,ID3V1_TITLE_SIZE);

	DDX_Text(pDX,IDC_EDIT_MP3TAG_ARTIST,m_strArtist);
	DDV_MaxChars(pDX,m_strArtist,ID3V1_ARTIST_SIZE);

	DDX_Text(pDX,IDC_EDIT_MP3TAG_ALBUM,m_strAlbum);
	DDV_MaxChars(pDX,m_strAlbum,ID3V1_ALBUM_SIZE);

	DDX_Text(pDX,IDC_EDIT_MP3TAG_YEAR,m_strYear);
	DDV_MaxChars(pDX,m_strYear,ID3V1_YEAR_SIZE);

	DDX_Control(pDX,IDC_COMBO_MP3TAG_GENRE,m_wndComboGenre);

	DDX_Text(pDX,IDC_EDIT_MP3TAG_TRACK,m_strTrack);
	DDV_MaxChars(pDX,m_strTrack,ID3V1_TRACK_SIZE);

	DDX_Text(pDX,IDC_EDIT_MP3TAG_COMMENT,m_strComment);
	DDV_MaxChars(pDX,m_strComment,ID3V1_COMMENT_SIZE);

	DDX_Text(pDX,IDC_EDIT_MP3TAG_INFO,m_strInfo);

	DDX_Check(pDX,IDC_CHECK_AUTOSAVE,m_bAutoSaveTags);
}

/*
	CWallPaperMP3TagDlg()
*/
CWallPaperMP3TagDlg::CWallPaperMP3TagDlg(HWND hWndParent)
: CDialogEx(	/*nTemplateID*/		IDD_DIALOG_MP3TAG,
			/*nToolbarID*/			0L,
			/*nStatusbarID*/		0L,
			/*nIconID*/			IDI_ICON_MP3TAG,
			/*nMenuID*/			0L,
#ifdef _RESIZABLE_DIALOG_STYLE
			/*bAllowResize*/		FALSE,
#endif
			/*bAllowSnap*/			TRUE,
			/*bAllowDragAndDrop*/	TRUE,
			/*lpcszUniqueName*/		IDS_DIALOG_MP3TAG_TITLE,
			/*bAllowMultipleInstances*/FALSE,
			/*hWndParent*/			NULL)
{
	m_hWndParent		= hWndParent;
	m_hParentWindow	= NULL;
	m_pConfig			= NULL;
	memset(m_szFileName,'\0',sizeof(m_szFileName));
	m_strFilename		= "";
	m_strTitle		= "";
	m_strArtist		= "";
	m_strAlbum		= "";
	m_strYear			= "";
	m_strGenre		= "";
	m_strTrack		= "";
	m_strComment		= "";
	m_strInfo			= "";
	m_bReadOnly		= FALSE;
	m_nFileCount		= 0;
	m_nCurrentFile		= 0;
	m_bAutoSaveTags	= TRUE;
	m_bDirty			= FALSE;
}

/*
	OnInitDialog()
*/
BOOL CWallPaperMP3TagDlg::OnInitDialog(void)
{
	// classe base
	if(!CDialogEx::OnInitDialog())
	{
		EndDialog(IDCANCEL);
		return(FALSE);
	}

	// ricava i puntatori dall'applicazione principale
	m_pConfig = NULL;
	if(m_hWndParent)
		m_pConfig = (CWallPaperConfig*)::SendMessage(m_hWndParent,WM_GETCONFIGURATION,0L,0L);
	if(!m_pConfig)
	{
		::MessageBoxResource(NULL,MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_RETRIEVE_CONFIGURATION);
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
	if(m_pConfig->GetNumber(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_TOOLTIP_MP3TAG_KEY))
	{
		char szTooltipText[1024];
		::FormatResourceStringEx(szTooltipText,
							sizeof(szTooltipText)-1,
							IDS_TOOLTIP_EX_MP3TAG,
							WALLPAPER_WEB_SITE,
							IDI_ICON_MP3TAG,
							WALLPAPER_WEB_SITE,
							WALLPAPER_WEB_SITE,
							IDI_ICON16X16_CLOSE
							);
		char szCss[512];
		::FormatResourceString(szCss,sizeof(szCss),IDS_TOOLTIP_CSS_STYLE);
		// la zoccola dell'ultima ver. per i tooltips non funge con IDC_STATIC_BACKGROUND
		//AddExtendedToolTip(IDC_STATIC_BACKGROUND,szTooltipText,szCss);
		AddExtendedToolTip(IDC_EDIT_MP3TAG_FILENAME,szTooltipText,szCss);
		CPPToolTip* pTooltip = GetExtendedToolTipCtrl();
		pTooltip->SetCallbackHyperlink(this->GetSafeHwnd(),WM_TOOLTIP_CALLBACK);
		//AddToolTip(IDC_EDIT_MP3TAG_FILENAME,IDS_TOOLTIP_MP3TAG_FILENAME);
	}
	else
		AddToolTip(IDC_EDIT_MP3TAG_FILENAME,IDS_TOOLTIP_MP3TAG_DLG);
		
	AddToolTip(IDC_CHECK_AUTOSAVE,IDS_TOOLTIP_MP3TAG_AUTOSAVE);
	AddToolTip(IDC_EDIT_MP3TAG_TITLE,IDS_TOOLTIP_MP3TAG_TITLE);
	AddToolTip(IDC_EDIT_MP3TAG_ARTIST,IDS_TOOLTIP_MP3TAG_ARTIST);
	AddToolTip(IDC_EDIT_MP3TAG_ALBUM,IDS_TOOLTIP_MP3TAG_ALBUM);
	AddToolTip(IDC_EDIT_MP3TAG_YEAR,IDS_TOOLTIP_MP3TAG_YEAR);
	AddToolTip(IDC_COMBO_MP3TAG_GENRE,IDS_TOOLTIP_MP3TAG_GENRE);
	AddToolTip(IDC_EDIT_MP3TAG_TRACK,IDS_TOOLTIP_MP3TAG_TRACK);
	AddToolTip(IDC_EDIT_MP3TAG_COMMENT,IDS_TOOLTIP_MP3TAG_COMMENT);
	AddToolTip(IDC_EDIT_MP3TAG_INFO,IDS_TOOLTIP_MP3TAG_INFO);
	AddToolTip(IDC_EDIT_MP3TAG_COMPOSER,IDS_TOOLTIP_MP3TAG_COMPOSER);
	AddToolTip(IDC_EDIT_MP3TAG_ORIGINALARTIST,IDS_TOOLTIP_MP3TAG_ORIGINALARTIST);
	AddToolTip(IDC_EDIT_MP3TAG_COPYRIGHT,IDS_TOOLTIP_MP3TAG_COPYRIGHT);
	AddToolTip(IDC_EDIT_MP3TAG_URL,IDS_TOOLTIP_MP3TAG_URL);
	AddToolTip(IDC_EDIT_MP3TAG_ENCODEDBY,IDS_TOOLTIP_MP3TAG_ENCODEDBY);
	AddToolTip(IDC_EDIT_MP3TAG_LYRICS,IDS_TOOLTIP_MP3TAG_LYRICS);
	AddToolTip(IDOK,IDS_TOOLTIP_MP3TAG_SAVE);
	AddToolTip(IDC_BUTTON_CLEAR,IDS_TOOLTIP_MP3TAG_CLEAR);
	AddToolTip(IDCANCEL,IDS_TOOLTIP_CLOSE);
	AddToolTip(IDC_BUTTON_BROWSE,IDS_TOOLTIP_MP3TAG_BROWSE);
	AddToolTip(IDC_BUTTON_PREV,IDS_TOOLTIP_MP3TAG_PREV);
	AddToolTip(IDC_BUTTON_NEXT,IDS_TOOLTIP_MP3TAG_NEXT);

	// acceleratori in combinazione con Alt
	SetAltAcceleratorKeys("TABYGKMROPUEL");

	// per le notifiche sulla modifica
	m_wndComboGenre.SetNotify(this->GetSafeHwnd(),WM_NOTIFY_CTRL);
		
	// imposta con quanto presente in configurazione
	m_bAutoSaveTags = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_MP3TAG_SAVEONSKIP_KEY);

	// interfaccia utente
	m_ctrlProperties.SetIconSize(32);
	m_ctrlProperties.SetIcon(IDI_ICON_MP3);
	CString strText(" MP3 Tags (ID3v1) ");
	m_ctrlProperties.SetText(strText);	
	m_ctrlMP3Info.SetIconSize(32);
	m_ctrlMP3Info.SetIcon(IDI_ICON_MPEG);
	strText = " MPEG Format ";
	m_ctrlMP3Info.SetText(strText);
	SetWindowText(IDS_DIALOG_MP3TAG_TITLE);

	// inizializzazione estesa
	OnInitDialogEx();
	
	// carica (e visualizza) i tags dal file
	LoadFile(m_strFilename);
	
	// carica (nella lista) il totale dei files presenti nella directory relativa
	BOOL bHaveMp3Files = LoadDirectory() > 1;
	GetDlgItem(IDC_BUTTON_PREV)->EnableWindow(bHaveMp3Files);
	GetDlgItem(IDC_BUTTON_NEXT)->EnableWindow(bHaveMp3Files);

	GetDlgItem(IDOK)->EnableWindow(m_bDirty);

	// trasparenza
	if(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAYERED_KEY))
		m_wndLayered.SetLayer(this->m_hWnd,(BYTE)m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAYERED_VALUE_KEY));

	// mette in primo piano
	CenterWindow();
	::SetForegroundWindowEx(this->m_hWnd);

	return(TRUE);
}

/*
	OnOK()
*/
void CWallPaperMP3TagDlg::OnOK(void)
{
	// imposta i tags
	OnSetTags();
	
	// salva i tags nel file
	m_Id3Lib.Update();
	m_bDirty = FALSE;
	GetDlgItem(IDOK)->EnableWindow(m_bDirty);
}

/*
	OnMp3TagEditorExit()
*/
LRESULT CWallPaperMP3TagDlg::OnMp3TagEditorExit(WPARAM,LPARAM)
{
	CDialogEx::OnExit(IDCANCEL);
	return(0L);
}

/*
	OnCancel()
*/
void CWallPaperMP3TagDlg::OnCancel(void)
{
	CDialogEx::OnExit(IDCANCEL);
}

/*
	OnButtonPlayCurrent()
*/
void CWallPaperMP3TagDlg::OnButtonPlayCurrent(void)
{
	if(m_hParentWindow)
		::SendMessage(m_hParentWindow,WM_AUDIOPLAYER,(WPARAM)AUDIOPLAYER_COMMAND_PLAY_FROM_FILE,(LPARAM)m_szFileName);
}

/*
	OnVirtualKey()

	Intercetta la pressione dei tasti.
*/
BOOL CWallPaperMP3TagDlg::OnVirtualKey(WPARAM wVirtualKey,BOOL bShiftPressed,BOOL bCtrlPressed,BOOL bAltPressed)
{
	BOOL bTranslated = FALSE;
	
	// abilita i campi solo se puo' aggiornare il file
	if(stristr(m_szFileName,m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_TEMPDIR_KEY))!=NULL)
	{
		m_bReadOnly = TRUE;
	}
	// se proviene da un file compresso
	else if(stristr(m_szFileName,GZW_EXTENSION) || stristr(m_szFileName,RAR_EXTENSION) || stristr(m_szFileName,ZIP_EXTENSION))
	{
		m_bReadOnly = TRUE;
	}

	// acceleratori (Alt + lettera evidenziata)
	// utilizzare solo per pulire i campi (i tags)
	if(m_bReadOnly)
		::MessageBeep(MB_ICONEXCLAMATION);
	else
	{
		UpdateData(TRUE);
		
		switch(wVirtualKey)
		{
			// Alt + T (azzera il titolo)
			case 'T':
				if(bAltPressed)
				{
					m_strTitle = "";
					m_Id3Lib.SetTitle(m_strTitle);
					bTranslated = TRUE;
				}
				break;
			// Alt + A (azzera l'artista)
			case 'A':
				if(bAltPressed)
				{
					m_strArtist = "";
					m_Id3Lib.SetArtist(m_strArtist);
					bTranslated = TRUE;
				}
				break;
			// Alt + B (azzera l'album)
			case 'B':
				if(bAltPressed)
				{
					m_strAlbum = "";
					m_Id3Lib.SetAlbum(m_strAlbum);
					bTranslated = TRUE;
				}
				break;
			// Alt + Y (azzera l'anno)
			case 'Y':
				if(bAltPressed)
				{
					m_strYear = "";
					m_Id3Lib.SetYear(0);
					bTranslated = TRUE;
				}
				break;
			// Alt + G (azzera il genere)
			case 'G':
				if(bAltPressed)
				{
					m_strGenre = "";
					int n = 0;
					if((n = m_wndComboGenre.FindStringExact(-1,m_strGenre))==CB_ERR)
						n = m_wndComboGenre.AddString(m_strGenre);
					m_wndComboGenre.SetCurSel(n);
					m_Id3Lib.SetGenre(m_strGenre);
					bTranslated = TRUE;
				}
				break;
			// Alt + K (azzera il numero traccia)
			case 'K':
				if(bAltPressed)
				{
					m_strTrack = "";
					m_Id3Lib.SetTrack(0);
					bTranslated = TRUE;
				}
				break;
			// Alt + M (azzera il commento)
			case 'M':
				if(bAltPressed)
				{
					m_strComment = "";
					m_Id3Lib.SetComment(m_strComment);
					bTranslated = TRUE;
				}
				break;
		}
	}
	
	// se e' passato per una combinazione Alt + lettera aggiorna il dialogo
	if(bTranslated)
	{
		UpdateData(FALSE);
		OnEnChangeTags();
	}

	// se la combinazione premuta non era un acceleratore
	if(!bTranslated)
		switch(wVirtualKey)
		{
			// Ctrl + Pagina/Freccia su (elemento precedente)
			case VK_PRIOR:
			case VK_UP:
			//case VK_LEFT:
				if(bCtrlPressed)
				{
					OnLoadPrev();
					bTranslated = TRUE;
				}
				break;
			
			// Ctrl + Pagina/Freccia giu' (elemento successivo)
			case VK_NEXT:
			case VK_DOWN:
			//case VK_RIGHT:
				if(bCtrlPressed)
				{
					OnLoadNext();
					bTranslated = TRUE;
				}
				break;
			
			// Ctrl + Ins (cambio file), Shift + Ins (cambio direcory)
			case VK_INSERT:
				if(bCtrlPressed || bShiftPressed)
				{
					OnButtonBrowse();
					bTranslated = TRUE;
				}
				break;
		}

	return(bTranslated);
}

/*
	OnTooltipCallback()
*/
LRESULT CWallPaperMP3TagDlg::OnTooltipCallback(WPARAM wParam,LPARAM /*lParam*/)
{
	LPSTR pMsg = (LPSTR)wParam;
	if(pMsg)
	{
		if(stricmp(pMsg,"MP3Tag")==0)
		{
			m_pConfig->UpdateNumber(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_TOOLTIP_MP3TAG_KEY,0);
			RemoveToolTip(IDC_EDIT_MP3TAG_FILENAME);
			AddToolTip(IDC_EDIT_MP3TAG_FILENAME,IDS_TOOLTIP_MP3TAG_DLG);
		}
		m_pConfig->SaveSection(WALLPAPER_DONOTASKMORE_KEY);
	}
	
	return(0L);
}

/*
	OnSetTags()
*/
void CWallPaperMP3TagDlg::OnSetTags(void)
{
	// imposta i tags
	UpdateData(TRUE);
	
	m_Id3Lib.SetTitle(m_strTitle);
	m_Id3Lib.SetArtist(m_strArtist);
	m_Id3Lib.SetAlbum(m_strAlbum);
	m_Id3Lib.SetYear(atoi(m_strYear));
	m_Id3Lib.SetTrack(atoi(m_strTrack));
	CString strGenre("");
	m_wndComboGenre.GetWindowText(strGenre);
	int n = 0;
	if((n = m_wndComboGenre.FindStringExact(-1,strGenre))==CB_ERR)
		n = m_wndComboGenre.AddString(strGenre);
	m_strGenre.Format("%s",strGenre);
	m_Id3Lib.SetGenre(m_strGenre);
	m_Id3Lib.SetComment(m_strComment);
}

/*
	OnClearTags()
*/
void CWallPaperMP3TagDlg::OnClearTags(void)
{
	// resetta i tags
	m_strTitle		= "";
	m_strArtist		= "";
	m_strAlbum		= "";
	m_strYear			= "";
	m_strGenre		= "";
	m_strTrack		= "";
	m_strComment		= "";
	m_strInfo			= "";

	int n = 0;
	if((n = m_wndComboGenre.FindStringExact(-1,m_strGenre))==CB_ERR)
		n = m_wndComboGenre.AddString(m_strGenre);
	m_wndComboGenre.SetCurSel(n);

	UpdateData(FALSE);
}

/*
	OnDropFiles()

	Carica il file ricevuto via drag and drop dalla shell.
*/
void CWallPaperMP3TagDlg::OnDropFiles(void)
{
	char szFileName[_MAX_PATH+1];
	DWORD dwAttribute;

	// mette in primo piano quando riceve via drag & drop
	SetForegroundWindowEx(this->m_hWnd);

	// considera solo il primo file ricevuto
	if(CDialogEx::GetDroppedFile(szFileName,sizeof(szFileName),&dwAttribute))
	{
		// file .mp3
		if(striright(szFileName,MP3_EXTENSION)==0)
		{
			// carica il file
			LoadFile(szFileName);
			
			// carica (nella lista) il totale dei files presenti nella directory relativa
			BOOL bHaveMp3Files = LoadDirectory() > 1;
			GetDlgItem(IDC_BUTTON_PREV)->EnableWindow(bHaveMp3Files);
			GetDlgItem(IDC_BUTTON_NEXT)->EnableWindow(bHaveMp3Files);
		}
		// file sconosciuto
		else
		{
			::MessageBoxResourceEx(this->m_hWnd,MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_DROPPED_FILE,szFileName);
		}
	}
}

/*
	OnLoadItem()
*/
LRESULT CWallPaperMP3TagDlg::OnLoadItem(WPARAM /*wParam*/,LPARAM lParam)
{
	// mette in primo piano quando riceve il file da caricare via messaggio
	SetForegroundWindowEx(this->m_hWnd);

	// carica il file specificato
	LoadFile((LPCSTR)lParam);
	
	// carica (nella lista) il totale dei files presenti nella directory relativa
	BOOL bHaveMp3Files = LoadDirectory() > 1;
	GetDlgItem(IDC_BUTTON_PREV)->EnableWindow(bHaveMp3Files);
	GetDlgItem(IDC_BUTTON_NEXT)->EnableWindow(bHaveMp3Files);
	
	return(0L);
}

/*
	OnSetParentWindow()
*/
LRESULT CWallPaperMP3TagDlg::OnSetParentWindow(WPARAM /*wParam*/,LPARAM lParam)
{
	m_hParentWindow = (HWND)lParam;
	return(0L);
}

/*
	OnButtonBrowse()
*/
void CWallPaperMP3TagDlg::OnButtonBrowse(void)
{
	char szDir[_MAX_FILEPATH+1] = {0};
	char szFile[_MAX_FILEPATH+1] = {0};
	BOOL bShiftPressed = ::GetKeyState(VK_SHIFT) < 0;

	// se viene premuto shift seleziona la directory, altrimenti il file
	if(bShiftPressed)
	{
		// lancia il dialogo per la selezione della directory
		CDirDialog dlg(m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_ADDDIR_KEY),
					"Open MP3 Directory...",
					"Select a folder:",
					m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_ALWAYSONTOP_KEY)
					);

		if(dlg.DoModal(this->m_hWnd)==IDOK)
			strcpyn(szDir,dlg.GetPathName(),sizeof(szDir));
	}
	else
	{
		// lancia il dialogo per la selezione del file da caricare
		CFileOpenDialog dlg("Open MP3 File...",
						m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_ADDFILE_KEY),
						"*.mp3",
						"MP3 files (*.mp3)|*.mp3||"
						);

		if(dlg.DoModal()==IDOK)
		{
			// imposta il file
			strcpyn(szFile,dlg.GetPathName(),sizeof(szFile));
			
			// imposta la directory
			strcpyn(szDir,dlg.GetPathName(),sizeof(szDir));
			strrev(szDir);
			int i = strchr(szDir,'\\') - szDir;
			strrev(szDir);
			if(i > 0)
				szDir[strlen(szDir)-i-1] = '\0';
		}
	}

	if(!strnull(szDir))
	{
		// aggiorna il valore relativo all'ultima directory utilizzata
		m_pConfig->UpdateString(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_ADDDIR_KEY,szDir);
		m_pConfig->SaveKey(WALLPAPER_INSTALL_KEY,WALLPAPER_LAST_ADDDIR_KEY);
		
		// aggiorna il valore relativo all'ultimo file
		if(!strnull(szFile))
		{
			m_pConfig->UpdateString(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_ADDFILE_KEY,szFile);
			m_pConfig->SaveKey(WALLPAPER_INSTALL_KEY,WALLPAPER_LAST_ADDFILE_KEY);
		}

		// carica (nella lista) il totale dei files presenti nella directory relativa
		BOOL bHaveMp3Files = LoadDirectory(szDir) > 1;
		GetDlgItem(IDC_BUTTON_PREV)->EnableWindow(bHaveMp3Files);
		GetDlgItem(IDC_BUTTON_NEXT)->EnableWindow(bHaveMp3Files);

		// carica il file
		if(szFile[0]=='\0')
			if(bHaveMp3Files)
				strcpyn(szFile,m_FindFile.GetFileName(0),sizeof(szFile));
		LoadFile(szFile);
	}
}

/*
	OnLoadPrev()
*/
void CWallPaperMP3TagDlg::OnLoadPrev(void)
{
	// se esistono altri file oltre a quello caricato
	if(m_nFileCount > 1)
	{
		// imposta l'indice
		if(--m_nCurrentFile < 0)
		{
			m_nCurrentFile = 0;
			::MessageBeep(MB_ICONEXCLAMATION);
		}

		// carica il file
		LoadFile(m_FindFile.GetFileName(m_nCurrentFile));
	}
}

/*
	OnLoadNext()
*/
void CWallPaperMP3TagDlg::OnLoadNext(void)
{
	// se esistono altri file oltre a quello caricato
	if(m_nFileCount > 1)
	{
		// imposta l'indice
		if(++m_nCurrentFile >= m_nFileCount)
		{
			m_nCurrentFile = m_nFileCount-1;
			::MessageBeep(MB_ICONEXCLAMATION);
		}

		// carica il file
		LoadFile(m_FindFile.GetFileName(m_nCurrentFile));
	}
}

/*
	OnCheckAutoSave()
*/
void CWallPaperMP3TagDlg::OnCheckAutoSave(void)
{
	m_bAutoSaveTags = !m_bAutoSaveTags;
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_MP3TAG_SAVEONSKIP_KEY,m_bAutoSaveTags);
	m_pConfig->SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_MP3TAG_SAVEONSKIP_KEY);
}

/*
	LoadFile()
*/
BOOL CWallPaperMP3TagDlg::LoadFile(LPCSTR lpcszFileName)
{
	// dopo il lancio, fino a che non riceve il messaggio relativo, il nome file interno e' vuoto
	if(strnull(lpcszFileName))
		return(FALSE);
		
	// salva i cambi pendenti
	if(m_bDirty && m_bAutoSaveTags && !strnull(m_szFileName))
	{
		OnSetTags();
		m_Id3Lib.Update();
	}

	// azzera i campi
	OnClearTags();
	
	// imposta i nomi file interni con quello ricevuto
	strcpyn(m_szFileName,lpcszFileName,sizeof(m_szFileName));
	m_strFilename.Format("%s",lpcszFileName);

	// carica i tags relativi al file
	if(m_Id3Lib.Link(m_szFileName))
	{
		m_MP3Info.Load(m_szFileName);
		
		m_strTitle.Format("%s",m_Id3Lib.GetTitle());
		m_strArtist.Format("%s",m_Id3Lib.GetArtist());
		m_strAlbum.Format("%s",m_Id3Lib.GetAlbum());
		m_strYear.Format("%s",strcmp(m_Id3Lib.GetYear(),"0")==0 ? "" : m_Id3Lib.GetYear());
		char* p = (char*)m_Id3Lib.GetGenre();
		m_strGenre.Format("%s",p ? p : "");
		m_strTrack = "";
		int nTrack = m_Id3Lib.GetTrack();
		if(nTrack > 0)
			m_strTrack.Format("%d",nTrack);
		m_strComment.Format("%s",m_Id3Lib.GetComment());
		long lMinutes = 0L;
		long lSeconds = 0L;
		QWORD qwSize = m_MP3Info.GetLength(lMinutes,lSeconds);
		m_strInfo.Format(	" File size: %s (%I64u bytes)\r\n"
						" Duration: %02d:%02d\r\n"
						" Bitrate: %d kbps\r\n"
						" Frequency: %ld khz\r\n"
						" Channel mode: %s\r\n"
						" MPEG layer %s, version %.1f",
						strsize((double)qwSize),
						qwSize,
						lMinutes,lSeconds,
						m_MP3Info.GetBitRate(),
						m_MP3Info.GetFrequency(),
						m_MP3Info.GetChannelMode(),
						m_MP3Info.GetLayer(),
						m_MP3Info.GetVersion()
						);
	}
		
	// imposta il genere
	int i = 0;
	int n = -1;
	char* p = NULL;
	m_wndComboGenre.ResetContent();
	do {
		p = (char*)CId3v1Tag::GetGenre(i++);
		if(p)
		{
			m_wndComboGenre.AddString(p);
			if(stricmp(p,m_strGenre)==0)
				n = i-1;
		}
	} while(p);
	if(n < 0)
		n = m_wndComboGenre.AddString(m_strGenre);
	m_wndComboGenre.SetCurSel(n);

	// abilita i campi solo se puo' aggiornare il file
	if(stristr(m_szFileName,m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_TEMPDIR_KEY))!=NULL)
	{
		m_bReadOnly = TRUE;
	}
	// se proviene da un file compresso
	else if(stristr(m_szFileName,GZW_EXTENSION) || stristr(m_szFileName,RAR_EXTENSION) || stristr(m_szFileName,ZIP_EXTENSION))
	{
		OnClearTags();
		m_bReadOnly = TRUE;
	}
	else
		m_bReadOnly = FALSE;

	GetDlgItem(IDC_CHECK_AUTOSAVE)->EnableWindow(!m_bReadOnly);
	GetDlgItem(IDC_EDIT_MP3TAG_TITLE)->EnableWindow(!m_bReadOnly);
	GetDlgItem(IDC_EDIT_MP3TAG_ARTIST)->EnableWindow(!m_bReadOnly);
	GetDlgItem(IDC_EDIT_MP3TAG_ALBUM)->EnableWindow(!m_bReadOnly);
	GetDlgItem(IDC_EDIT_MP3TAG_YEAR)->EnableWindow(!m_bReadOnly);
	GetDlgItem(IDC_COMBO_MP3TAG_GENRE)->EnableWindow(!m_bReadOnly);
	GetDlgItem(IDC_EDIT_MP3TAG_TRACK)->EnableWindow(!m_bReadOnly);
	GetDlgItem(IDC_EDIT_MP3TAG_COMMENT)->EnableWindow(!m_bReadOnly);
	GetDlgItem(IDC_BUTTON_CLEAR)->EnableWindow(!m_bReadOnly);
	SetWindowText(m_bReadOnly ? IDS_DIALOG_MP3TAG_TITLE_READ_ONLY : IDS_DIALOG_MP3TAG_TITLE);
	
	m_bDirty = FALSE;
	GetDlgItem(IDOK)->EnableWindow(m_bDirty);

	UpdateData(FALSE);

	return(TRUE);
}

/*
	LoadDirectory()
*/
int CWallPaperMP3TagDlg::LoadDirectory(LPCSTR lpcszDirectory/* = NULL*/)
{
	// carica (nella lista) il totale dei files presenti nella directory specificata
	char szDir[_MAX_FILEPATH+1];
	if(!lpcszDirectory)
	{
		if(strnull(m_szFileName))
			strcpyn(szDir,".\\",sizeof(szDir));
		else
		{
			strcpyn(szDir,m_szFileName,sizeof(szDir));
			char* p  = strrchr(szDir,'\\');
			if(p && *(p+1))
				*(p+1) = '\0';
		}
	}
	else
		strcpyn(szDir,lpcszDirectory,sizeof(szDir));
	
	::EnsureBackslash(szDir,sizeof(szDir));
	m_nFileCount = m_FindFile.FindFile(szDir,"*.mp3",FALSE);
	m_nCurrentFile = 0;

	// modifica il testo del tooltip del bottone per lo skip aggiungendo il numero di files trovati
	char szString[_MAX_FILEPATH];	
	::FormatResourceStringEx(szString,sizeof(szString),IDS_TOOLTIP_MP3TAG_PREV,m_nFileCount);
	ModifyToolTip(IDC_BUTTON_PREV,szString);
	::FormatResourceStringEx(szString,sizeof(szString),IDS_TOOLTIP_MP3TAG_NEXT,m_nFileCount);
	ModifyToolTip(IDC_BUTTON_NEXT,szString);

	return(m_nFileCount);
}
