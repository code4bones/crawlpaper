/*
	WallPaperPlayerDlg.cpp
	Dialogo per il player audio.
	Luca Piergentili, 09/11/01
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "strings.h"
#include "window.h"
#include "win32api.h"
#include <shlobj.h>
#include <shellapi.h>
#include "CNodeList.h"
#include "CRand.h"
#include "CUrl.h"
#include "CBrowser.h"
#include "CDateTime.h"
#include "CFindFile.h"
#include "CFilenameFactory.h"
#include "CTrayIcon.h"
#include "CTaskbarNotifier.h"
#include "CAudioPlayer.h"
#include "CDialogEx.h"
#include "CDirDialog.h"
#include "CTitleMenu.h"
#include "WallPaperConfig.h"
#include "WallPaperMessages.h"
#include "WallPaperMP3TagDlg.h"
#include "WallPaperMP3TagUIThread.h"
#include "WallPaperPlayerDlg.h"
#include "WallPaperVersion.h"
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

BEGIN_MESSAGE_MAP(CWallPaperPlayerDlg,CDialogEx)
	ON_WM_TIMER()
	ON_WM_SYSCOMMAND()
	
	// gestori per le chiamate provenienti dalla gui (dialogo)
	ON_BN_CLICKED(IDC_BUTTON_PLAY,OnButtonPlay)
	ON_BN_CLICKED(IDC_BUTTON_STOP,OnButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_PAUSE,OnButtonPause)
	ON_BN_CLICKED(IDC_BUTTON_PREVIOUS,OnButtonPrev)
	ON_BN_CLICKED(IDC_BUTTON_NEXT,OnButtonNext)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE,OnButtonRemove)
	ON_BN_CLICKED(IDC_BUTTON_DELETE,OnButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_EXIT,OnExit)

	// gestori per le chiamate provenienti dalla gui (menu popup della tray icon)
	ON_COMMAND(IDM_MENU_WEBSEARCH,OnButtonWebSearch)
	ON_COMMAND(IDM_MENU_INFO,OnButtonInfo)
	ON_COMMAND(IDM_MENU_TITLE,OnButtonPlay)
	ON_COMMAND(IDM_MENU_PLAY,OnButtonPlay)
	ON_COMMAND(IDM_MENU_STOP,OnButtonStop)
	ON_COMMAND(IDM_MENU_PAUSE,OnButtonPause)
	ON_COMMAND(IDM_MENU_PREVIOUS,OnButtonPrev)
	ON_COMMAND(IDM_MENU_NEXT,OnButtonNext)
	ON_COMMAND(IDM_MENU_REMOVE,OnButtonRemove)
	ON_COMMAND(IDM_MENU_DELETE,OnButtonDelete)
	ON_COMMAND(IDM_MENU_MOVETO,OnButtonMoveTo)
	ON_COMMAND(IDM_MENU_COPYTO,OnButtonCopyTo)
	ON_COMMAND(IDM_MENU_EXIT,OnExit)
	ON_COMMAND(IDM_MENU_SOUNDCONTROL,OnOpenSoundControl)
	ON_COMMAND(IDM_MENU_FAVOURITE_AUDIO,OnFavouriteAudio)
	ON_COMMAND(IDM_MENU_MP3TAG,OnMP3Tag)

	ON_MESSAGE(WM_NOTIFYTASKBARICON_AUDIO,OnNotifyTrayIcon)
	ON_MESSAGE(WM_NOTIFYTASKBARPOPUP_AUDIO,OnNotifyTaskbarPopup)
		
	ON_MESSAGE(WM_AUDIOPLAYER,OnPlayer)
	ON_MESSAGE(WM_AUDIOPLAYER_MODE,OnPlayerSetMode)
	ON_MESSAGE(WM_AUDIOPLAYER_SETVOLUME,OnPlayerSetVolume)
	ON_MESSAGE(WM_AUDIOPLAYER_PLAY,OnPlayerPlay)
	ON_MESSAGE(WM_AUDIOPLAYER_STOP,OnPlayerStop)
	ON_MESSAGE(WM_AUDIOPLAYER_PAUSE,OnPlayerPause)
	ON_MESSAGE(WM_AUDIOPLAYER_NEXT,OnPlayerNext)
	ON_MESSAGE(WM_AUDIOPLAYER_PREV,OnPlayerPrev)
	ON_MESSAGE(WM_AUDIOPLAYER_REMOVE,OnPlayerRemove)
	ON_MESSAGE(WM_AUDIOPLAYER_REMOVEALL,OnPlayerRemoveAll)
	ON_MESSAGE(WM_AUDIOPLAYER_EXIT,OnPlayerExit)
	ON_MESSAGE(WM_AUDIOPLAYER_GETSTATUS,OnPlayerGetStatus)
	ON_MESSAGE(WM_AUDIOPLAYER_SETSTATUS,OnPlayerSetStatus)
	ON_MESSAGE(WM_AUDIOPLAYER_POPUPLIST,OnPlayerSetPopupList)
	ON_MESSAGE(WM_AUDIOPLAYER_EXCEPTION,OnPlayerException)
	ON_MESSAGE(WM_AUDIOPLAYER_FAVOURITE,OnFavouriteAudio)
END_MESSAGE_MAP()

IMPLEMENT_DYNCREATE(CWallPaperPlayerDlg,CDialogEx)

/*
	DoDataExchange()
*/
void CWallPaperPlayerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

/*
	CWallPaperPlayerDlg()
*/
CWallPaperPlayerDlg::CWallPaperPlayerDlg(HWND hWndParent)
: CDialogEx(	/*nTemplateID*/		IDD_DIALOG_PLAYER,
			/*nToolbarID*/			0L,
			/*nStatusbarID*/		0L,
			/*nIconID*/			IDI_ICON_PLAYER,
			/*nMenuID*/			0L,
#ifdef _RESIZABLE_DIALOG_STYLE
			/*bAllowResize*/		FALSE,
#endif
			/*bAllowSnap*/			FALSE,
			/*bAllowDragAndDrop*/	FALSE,
			/*lpcszUniqueName*/		IDS_PLAYER_MUTEX_NAME,
			/*bAllowMultipleInstances*/FALSE,
			/*hWndParent*/			NULL)
{
	// per evitare che la zoccola di MFC visualizzi il dialogo quando decide lei
	SetVisible(FALSE);

	m_bFirstCall = TRUE;
	m_hWndParent = hWndParent;
	m_nTimerID = 0;
	m_pTrayIcon = NULL;
	memset(m_szTitleMenuText,'\0',sizeof(m_szTitleMenuText));
	memset(m_szTrayIconTooltipText,'\0',sizeof(m_szTrayIconTooltipText));
	m_pTaskbarNotifier = NULL;
	memset(m_szTaskbarToolTipText,'\0',sizeof(m_szTaskbarToolTipText));
	m_nTaskbarPopupMode = AUDIOPLAYER_MODE_NOPOPUP;
	m_bAutoSkip = TRUE;
	m_bFeedback = TRUE;
	m_pListTaskbarBitmaps = NULL;
	memset(&m_AudioInfo,'\0',sizeof(AUDIOINFO));
	memset(&m_TaskbarPopup,'\0',sizeof(TASKBARPOPUP));
	m_nPopupIndex = 0;
	memset(m_szLastPlayedSong,'\0',sizeof(m_szLastPlayedSong));
	memset(m_szAudioFileName,'\0',sizeof(m_szAudioFileName));
	memset(m_szFavouriteName,'\0',sizeof(m_szFavouriteName));
	strcpy(m_Tags[0].tag,"%t");	// titolo / nome file
	strcpy(m_Tags[1].tag,"%a");	// artista
	strcpy(m_Tags[2].tag,"%b");	// album
	strcpy(m_Tags[3].tag,"%g");	// genere
	strcpy(m_Tags[4].tag,"%y");	// anno
	strcpy(m_Tags[5].tag,"%r");	// traccia / memoria utilizzata
	strcpy(m_Tags[6].tag,"%d");	// durata / dimensione (bytes)
	strcpy(m_Tags[7].tag,"%c");	// commento
	strcpy(m_Tags[8].tag,"%B");	// bitrate / dimensione (pixel)
	strcpy(m_Tags[9].tag,"%F");	// frequenza / colori
	strcpy(m_Tags[10].tag,"%C");	// modalita' / qualita
	strcpy(m_Tags[11].tag,"%T");	// formato / BPP
	strcpy(m_Tags[12].tag,"\\n");	// a capo
	for(int i = 0; i < ARRAY_SIZE(m_Tags); i++)
		m_Tags[i].value = NULL;
	m_nCurrentAudioIndex = 0;
	m_mmInternalMode = mmAudioPmUndefined;
	CDateTime creatorDate("19650826",ANSI_SHORT);
	CDateTime currentDate(ANSI_SHORT);
	currentDate.GetFormattedDate(TRUE);
	unsigned long nSeed = currentDate.GetJulianDateDiff(currentDate,creatorDate);
	m_randomNumberGenerator.Seed(nSeed + ::GetTickCount());
	m_pMP3TagWinThread = NULL;
	m_pFindAudioIcons = NULL;
	m_nCurrentAudioIcon = -1;
	m_nAudioIconsCount = 0;
	m_syncAudioPlay.SetName("AudioPlay");
}

/*
	OnNotifyTrayIcon()

	Gestore dei messaggi inviati dal menu popup dell'icona nella tray area.
*/
LRESULT CWallPaperPlayerDlg::OnNotifyTrayIcon(WPARAM /*wParam*/,LPARAM lParam)
{
	//UINT uID = (UINT)wParam;
	UINT uMouseMsg = (UINT)lParam;

	switch(uMouseMsg)
	{
		// click sinistro, passa al file audio precedente/successivo
		case WM_LBUTTONDOWN:
			if(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_ONE_CLICK_CHANGE_KEY))
			{
				BOOL bShiftPressed = ::GetKeyState(VK_SHIFT) < 0;
				BOOL bCtrlPressed = ::GetKeyState(VK_CONTROL) < 0;
				if(bCtrlPressed)
				{
					OnEd2kQuery();
				}
				else
				{
					if(bShiftPressed)
						PostMessage(WM_AUDIOPLAYER_PREV);
					else
						PostMessage(WM_AUDIOPLAYER_NEXT);
				}
			}
			break;

		// click destro, visualizza il menu popup
		case WM_RBUTTONDOWN:
		{
			// se il nome file in riproduzione inizia con il pathname per i temporanei, assume
			// che il file provenga da un file compresso, per cui elimina le voci incompatibili
			BOOL bComesFromArchiveFile = stristr(m_szAudioFileName,m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_TEMPDIR_KEY))!=NULL;

			// crea il menu
			CTitleMenu popupMenu;
			popupMenu.CreatePopupMenu();
			
			// abilita le voci del menu a seconda dello stato corrente
			if(m_AudioPlayer.GetStatus()==mmAudioPmPlaying)
			{
				// in riproduzione
				if(!bComesFromArchiveFile)
				{
					popupMenu.AppendMenu(MF_STRING,IDM_MENU_COPYTO,"&Copy To...");
					popupMenu.AppendMenu(MF_STRING,IDM_MENU_MOVETO,"&Move To...");
					popupMenu.AppendMenu(MF_STRING,IDM_MENU_DELETE,"&Delete It From Disk");
				}
				popupMenu.AppendMenu(MF_STRING,IDM_MENU_REMOVE,"&Remove It From Playlist");
				popupMenu.AppendMenu(MF_SEPARATOR);
			}
			
			// aggiunge le voci fisse
			popupMenu.AppendMenu(MF_STRING,IDM_MENU_EXIT,"E&xit");
			popupMenu.AppendMenu(MF_STRING,IDM_MENU_SOUNDCONTROL,"&Open Sound Controls...");
			popupMenu.AppendMenu(MF_SEPARATOR);

			// abilita le voci del menu a seconda dello stato corrente
			if(m_AudioPlayer.GetStatus()==mmAudioPmPlaying)
			{
				// in riproduzione
				if(!strnull(m_szFavouriteName))
					popupMenu.AppendMenu(MF_STRING,IDM_MENU_FAVOURITE_AUDIO,"Add to &Favourite Audio");

				if(striright(m_szAudioFileName,MP3_EXTENSION)==0)
					popupMenu.AppendMenu(MF_STRING,IDM_MENU_MP3TAG,"MP&3 Tag Editor...");
				
				popupMenu.AppendMenu(MF_STRING,IDM_MENU_WEBSEARCH,"&Web Search...");
				popupMenu.AppendMenu(MF_STRING,IDM_MENU_INFO,"&Balloon Info...");
				popupMenu.AppendMenu(MF_SEPARATOR);
				popupMenu.AppendMenu(MF_STRING,IDM_MENU_STOP,"&Stop");
				popupMenu.AppendMenu(MF_STRING,IDM_MENU_PAUSE,"P&ause");
				popupMenu.AppendMenu(MF_STRING,IDM_MENU_PLAY,"P&lay"); // abilita comunque per il re-play
				if(m_AudioFilesList.Count() > 0)
				{
					popupMenu.AppendMenu(MF_STRING,IDM_MENU_PREVIOUS,"&Previous");
					popupMenu.AppendMenu(MF_STRING,IDM_MENU_NEXT,"&Next");
				}
			}
			else
			{
				// fermo
				popupMenu.AppendMenu(MF_STRING,IDM_MENU_PLAY,"&Resume");
			}

			// imposta il titolo del menu
			popupMenu.AddTitle(m_szTrayIconTooltipText,IDM_MENU_TITLE);
			popupMenu.SetEdge(TRUE,0,EDGE_SUNKEN);

			// visualizza il menu
			SetForegroundWindow();
			CPoint point;
			GetCursorPos(&point);
			popupMenu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,point.x,point.y,this);
			popupMenu.DestroyMenu();
		}
		break;

		// doppio click, apre il controllo per il volume
		case WM_LBUTTONDBLCLK :
			OpenSoundControl();
			break;
	}

	return(0L);
}

/*
	OnNotifyTaskbarPopup()
	
	Gestore dei messaggi inviati dal popup audio (bitmap).
*/
LRESULT CWallPaperPlayerDlg::OnNotifyTaskbarPopup(WPARAM /*wParam*/,LPARAM lParam)
{
	// click sul bitmap, controlla se sono state specificate le coordinate
	// per il bottone di chiusura e se il click e' avvenuto nell'area relativa
	if(m_TaskbarPopup.nPopupCloseLeft==0 AND m_TaskbarPopup.nPopupCloseTop==0 AND m_TaskbarPopup.nPopupCloseRight==0 AND m_TaskbarPopup.nPopupCloseBottom==0)
		;
	else
	{
		// rimuove il popup se viene cliccato il bottone di chiusura
		CRect rc(m_TaskbarPopup.nPopupCloseLeft,m_TaskbarPopup.nPopupCloseTop,m_TaskbarPopup.nPopupCloseRight,m_TaskbarPopup.nPopupCloseBottom);
		CPoint pt(((CPoint*)lParam)->x,((CPoint*)lParam)->y);
		if(rc.PtInRect(pt))
		{
			if(m_pTaskbarNotifier)
				m_pTaskbarNotifier->UnShow();
			return(0L);
		}
	}

	// se non e' presente il bottone di chiusura o se e' stata cliccato il bitmap, apre
	// il browser sull'url associata (se specificata) o il controllo per il volume
	CUrl url;
	if(url.IsUrlType(m_TaskbarPopup.szUrl,HTTP_URL))
	{
		CBrowser browser(m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_PREFERRED_INTERNET_BROWSER_KEY));
		browser.Browse(m_TaskbarPopup.szUrl);
	}
	else
		OpenSoundControl();

	return(0L);
}

/*
	OnSysCommand()
*/
void CWallPaperPlayerDlg::OnSysCommand(UINT nID,LPARAM lParam)
{
	if(nID==SC_MINIMIZE AND m_pTrayIcon)
		ShowWindow(SW_HIDE);
	else
		CDialogEx::OnSysCommand(nID,lParam);
}

/*
	OnInitDialog()
*/
BOOL CWallPaperPlayerDlg::OnInitDialog(void)
{
	// classe base
	if(!CDialogEx::OnInitDialog())
	{
		EndDialog(IDCANCEL);
		return(FALSE);
	}

	// inizializzazione estesa
	OnInitDialogEx();

	// imposta il titolo della finestra (non modificare dato che viene usato dal dialogo principale)
	SetWindowText(IDS_DIALOG_PLAYER_TITLE);

	// icona per la system tray
	m_pTrayIcon = new CTrayIcon();
	if(m_pTrayIcon)
	{
		// crea l'icona, minimizza e nasconde il dialogo
		if(!m_pTrayIcon->Create(this->m_hWnd,AfxGetApp()->LoadIcon(IDI_ICON_PLAYER),WM_NOTIFYTASKBARICON_AUDIO,IDM_TRAY_MENU_PLAYER,IDI_ICON_BLANK,IDS_DIALOG_PLAYER_TITLE))
		{
			delete m_pTrayIcon;
			m_pTrayIcon = NULL;
		}
		else
		{
			char szString[512];
			::FormatResourceString(szString,sizeof(szString),IDS_TOOLTIP_CSS_STYLE);
			m_pTrayIcon->SetCssStyles(szString);
		}

		// imposta l'icona su idle
		if(m_pTrayIcon)
		{
			ShowWindow(SW_MINIMIZE);
			PostMessage(WM_SYSCOMMAND,MAKELONG(SC_MINIMIZE,0),0L);
			char szToolTipText[TRAYICON_MAX_TOOLTIP_TEXT+1];
			char szString[_MAX_PATH+1];
			::FormatResourceString(szString,sizeof(szString),IDS_MESSAGE_IDLE);
			_snprintf(szToolTipText,sizeof(szToolTipText)-1,"(%s)",szString);
			SetTrayIcon(szToolTipText);
		}
	}

	// ricava la configurazione corrente
	m_pConfig = NULL;
	if(m_hWndParent)
	{
		m_pConfig = (CWallPaperConfig*)::SendMessage(m_hWndParent,WM_GETCONFIGURATION,0L,0L);
	}
	if(!m_pConfig)
	{
		::MessageBoxResource(NULL,MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_RETRIEVE_CONFIGURATION);
		EndDialog(IDCANCEL);
		return(FALSE);
	}

	// imposta il timer per il passaggio al brano seguente
	m_nTimerID = SetTimer(ID_TIMER_AUDIO_PLAYER,1000L,NULL);

	// nome file dell'ultimo brano riprodotto
	strcpyn(m_szLastPlayedSong,m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_PLAYEDSONG_KEY),sizeof(m_szLastPlayedSong));

	return(TRUE);
}

/*
	OnExit()
*/
void CWallPaperPlayerDlg::OnExit(void)
{
	// elimina il timer
	if(m_nTimerID!=0)
		KillTimer(m_nTimerID);
	
	// chiude il dialogo per i tags
	if(m_pMP3TagWinThread)
	{
		CWnd* pWnd = m_pMP3TagWinThread->GetMainWnd();
		if(pWnd)
		{
			HWND hWnd = pWnd->GetSafeHwnd();
			if(hWnd)
			{
				//::PostMessage(hWnd,WM_COMMAND,MAKELONG(IDCANCEL,0),0L);	// nella versione release fa terminare l'applicazione principale
				::PostMessage(hWnd,WM_MP3TAGEDITOR_EXIT,0L,0L);
				
				// aspetta che la finestra venga chiusa
				int n = 0;
				while(::FindWindow(NULL,IDS_DIALOG_MP3TAG_TITLE)!=NULL AND n < 10)
					n++,::Sleep(100L);
				::Sleep(100L);
			}
		}

		delete m_pMP3TagWinThread,m_pMP3TagWinThread = NULL;
	}
	
	// chiude il player
	if(m_AudioPlayer.GetStatus()==mmAudioPmPlaying)
		m_AudioPlayer.Stop();
	if(m_AudioPlayer.GetStatus()!=mmAudioPmClosed)
		m_AudioPlayer.Close();

	// elimina il popup per la taskbar
	if(m_pTaskbarNotifier)
	{
		m_pTaskbarNotifier->UnShow();
		delete m_pTaskbarNotifier,m_pTaskbarNotifier = NULL;
	}

	// elimina l'icona per la system tray
	if(m_pTrayIcon)
	{
		m_pTrayIcon->Balloon();
		delete m_pTrayIcon,m_pTrayIcon = NULL;
	}

	// ripulisce la directory temporanea dai files audio esistenti
	if(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CLEAR_TEMPORARY_FILES_KEY)!=TEMPORARY_FILES_DO_NOT_CLEAR)
	{
		LPSTR pFileName;
		CFindFile findFile;
		while((pFileName = (LPSTR)findFile.FindEx(m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_TEMPDIR_KEY),"*.*",TRUE))!=NULL)
		{
			if(CAudioPlayer::IsSupportedFormat(pFileName))
				::DeleteFile(pFileName);
		}
	}

	// aggiorna il nome dell'ultimo brano riprodotto
	m_pConfig->UpdateString(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_PLAYEDSONG_KEY,m_szLastPlayedSong);
	m_pConfig->SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_PLAYEDSONG_KEY);
	
	// classe base
	CDialogEx::OnExit();
}

/*
	OnTimer()
*/
void CWallPaperPlayerDlg::OnTimer(UINT nIDEvent)
{
	// timer per il passaggio al brano seguente
	if(nIDEvent==ID_TIMER_AUDIO_PLAYER)
	{
		mmAudioPlayerMode status = m_AudioPlayer.GetStatus();
		switch(status)
		{
			case mmAudioPmLoading:
				TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperPlayerDlg::OnTimer(): loading\n"));
				break;

			case mmAudioPmOpened:
				TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperPlayerDlg::OnTimer(): opened\n"));
				break;

			case mmAudioPmReady:
				TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperPlayerDlg::OnTimer(): ready\n"));
				break;

			case mmAudioPmPlaying:
				//TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperPlayerDlg::OnTimer(): playing\n"));
				break;

			case mmAudioPmPaused:
				TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperPlayerDlg::OnTimer(): paused\n"));
				break;
			
			case mmAudioPmStopped:
				TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperPlayerDlg::OnTimer(): stopped\n"));
				break;

			case mmAudioPmClosed:
				TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperPlayerDlg::OnTimer(): closed\n"));
				break;

			case mmAudioPmDone:
				TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperPlayerDlg::OnTimer(): done, posting WM_AUDIOPLAYER_NEXT\n"));
				char szString[_MAX_PATH+1];
				::FormatResourceString(szString,sizeof(szString),IDS_MESSAGE_DONE);
				_snprintf(m_szTrayIconTooltipText,sizeof(m_szTrayIconTooltipText)-1,"(%s)",szString);
				TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperPlayerDlg::OnTimer(): posting WM_AUDIOPLAYER_NEXT\n"));
				PostMessage(WM_AUDIOPLAYER_NEXT);
				break;

			case mmAudioPmUndefined:
				TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperPlayerDlg::OnTimer(): undefined\n"));
				break;
		}
	}
	else
		CDialogEx::OnTimer(nIDEvent);
}

/*
	OpenSoundControl()

	Apre il pannello di controllo audio.
*/
BOOL CWallPaperPlayerDlg::OpenSoundControl(void)
{
	ITEMIDLIST *id;
	char szSoundCtrlExecutable[_MAX_FILEPATH+1] = {0};
	BOOL bHaveSoundCtrlExecutable = FALSE;

	// se non trova l'eseguibile per il controllo (del volume), apre l'applet del pannello di controllo
	if(!bHaveSoundCtrlExecutable)
		if(::SHGetSpecialFolderLocation(NULL,CSIDL_SYSTEM,&id)==NOERROR)
			if(::SHGetPathFromIDList(id,&szSoundCtrlExecutable[0]))
			{
				int nLen = strlen(szSoundCtrlExecutable);
				_snprintf(szSoundCtrlExecutable+nLen,sizeof(szSoundCtrlExecutable)-1-nLen,"\\sndvol32.exe");
				bHaveSoundCtrlExecutable = ::FileExist(szSoundCtrlExecutable);
				TRACEEXPR((_TRACE_FLAG_ERR,__NOFILE__,__NOLINE__,"CWallPaperPlayerDlg::OpenSoundControl(): %s %s\n",szSoundCtrlExecutable,bHaveSoundCtrlExecutable ? "exists" : "does not exist"));
			}

	if(!bHaveSoundCtrlExecutable)
		if(::SHGetSpecialFolderLocation(NULL,CSIDL_WINDOWS,&id)==NOERROR)
			if(::SHGetPathFromIDList(id,&szSoundCtrlExecutable[0]))
			{
				int nLen = strlen(szSoundCtrlExecutable);
				_snprintf(szSoundCtrlExecutable+nLen,sizeof(szSoundCtrlExecutable)-1-nLen,"\\sndvol32.exe");
				bHaveSoundCtrlExecutable = ::FileExist(szSoundCtrlExecutable);
				TRACEEXPR((_TRACE_FLAG_ERR,__NOFILE__,__NOLINE__,"CWallPaperPlayerDlg::OpenSoundControl(): %s %s\n",szSoundCtrlExecutable,bHaveSoundCtrlExecutable ? "exists" : "does not exist"));
			}

	if(bHaveSoundCtrlExecutable)
		bHaveSoundCtrlExecutable = (int)::ShellExecute(this->m_hWnd,"open",szSoundCtrlExecutable,NULL,NULL,SW_SHOW) > 32;
	else
		bHaveSoundCtrlExecutable = (int)::ShellExecute(this->m_hWnd,"open","rundll32","shell32.dll,Control_RunDLL mmsys.cpl",NULL,SW_SHOW) > 32;
	
	return(bHaveSoundCtrlExecutable);
}

/*
	OnFavouriteAudio()
	
	Aggiunge il file alla playlist per i favoriti (audio).
*/
void CWallPaperPlayerDlg::OnFavouriteAudio(void)
{
	if(!strnull(m_szFavouriteName))
		if(m_hWndParent)
			::SendMessage(m_hWndParent,WM_FAVOURITE_ADD,(WPARAM)-1L,(LPARAM)m_szFavouriteName);
}

/*
	OnFavouriteAudio()
	
	Restituisce il nome del brano corrente per l'aggiunta ai favoriti (audio).
*/
LRESULT CWallPaperPlayerDlg::OnFavouriteAudio(WPARAM /*wParam*/,LPARAM /*lParam*/)
{
	return((LRESULT)(!strnull(m_szFavouriteName) ? m_szFavouriteName : NULL));
}

/*
	OnMp3Tag()
	
	Lancia l'istanza del thread ui per il dialogo per l'editor dei tags mp3.
*/
void CWallPaperPlayerDlg::OnMP3Tag(void)
{
	// quando la finestra del dialogo viene chiusa, viene chiuso il dialogo ma il thread rimane attivo
	if(!::FindWindow(NULL,IDS_DIALOG_MP3TAG_TITLE) AND m_pMP3TagWinThread)
		delete m_pMP3TagWinThread,m_pMP3TagWinThread = NULL;

	// lancia il thread per il dialogo (in sospeso per impostare l'autodelete)
	if(!m_pMP3TagWinThread)
		m_pMP3TagWinThread = AfxBeginThread(	RUNTIME_CLASS(CWallPaperMP3TagUIThread),
										m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PRIORITY_THREAD_KEY),
										0L,
										CREATE_SUSPENDED,
										NULL
									);

	if(m_pMP3TagWinThread)
	{
		// imposta l'autodelete e riavvia
		m_pMP3TagWinThread->m_bAutoDelete = FALSE;
		m_pMP3TagWinThread->ResumeThread();
		::Sleep(0L);
		
		// aspetta che la finestra diventi visibile
		while(::FindWindow(NULL,IDS_DIALOG_MP3TAG_TITLE)==NULL)
			::Sleep(100L);
		::Sleep(100L);

		// ricava l'handle del dialogo
		CWnd* pWnd = m_pMP3TagWinThread->GetMainWnd();
		if(pWnd)
		{
			HWND hWnd = pWnd->GetSafeHwnd();
			if(hWnd)
			{
				// invia il messaggio per il caricamento del file
				::SendMessage(hWnd,WM_LOADITEM,0L,(LPARAM)m_szAudioFileName);

				// invia il messaggio impostare l'handle della finestra
				::SendMessage(hWnd,WM_SETPARENTWINDOW,0L,(LPARAM)this->m_hWnd);
			}
		}
	}
}

/*
	OnEd2kQuery()
*/
void CWallPaperPlayerDlg::OnEd2kQuery(void)
{
	if(!strnull(m_szAudioFileName) AND m_AudioPlayer.GetStatus()==mmAudioPmPlaying)
	{
		int i,n;
		char szQueryStr[256] = {0};
		char szFormat[256] = {0};
		char szQuery[1024] = {0};
		char szBuffer[1024] = {0};
		CBrowser browser(m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_PREFERRED_INTERNET_BROWSER_KEY));

		// titolo
		i = n = 0;
		if(!strnull(m_AudioInfo.szTitle))
			i = _snprintf(szQueryStr+n,sizeof(szQueryStr)-1-n,"%s",m_AudioInfo.szTitle),n = (i > 0 ? n+i : -1);
		// artista
		if(n > 0 AND !strnull(m_AudioInfo.szArtist))
			i = _snprintf(szQueryStr+n,sizeof(szQueryStr)-1-n,"%s%s",strnull(m_AudioInfo.szTitle) ? "" : " ",m_AudioInfo.szArtist),n = (i > 0 ? n+i : -1);
		// se non ci sono tag usa il nomefile
		if(n <= 0)
		{
			strcpyn(szQueryStr,::StripPathFromFile(m_szAudioFileName),sizeof(szQueryStr));
			if(striright(szQueryStr,MP3_EXTENSION)==0)
				szQueryStr[strlen(szQueryStr)-strlen(MP3_EXTENSION)] = '\0';
			else if(striright(szQueryStr,WAV_EXTENSION)==0)
				szQueryStr[strlen(szQueryStr)-strlen(WAV_EXTENSION)] = '\0';
			else if(striright(szQueryStr,CDA_EXTENSION)==0)
				szQueryStr[strlen(szQueryStr)-strlen(CDA_EXTENSION)] = '\0';
		}
		
		if(!strnull(szQueryStr))
		{
			char chr[2] = {0};
			char chars[] = {"&.,;:/\"()[]{}-+*_?!@#$%%=<>\0"};
			//char* words[] = {"ft.","feat.",""};
			
			for(i=0; szQueryStr[i]; i++)
			{
				if(szQueryStr[i]=='(')
				{
					do {
						szQueryStr[i++] = ' ';
					} while(szQueryStr[i] && szQueryStr[i]!=')');
					if(szQueryStr[i]==')')
						szQueryStr[i] = ' ';
				}
			}
			
			for(i=0; chars[i]; i++)
			{
				chr[0] = chars[i];
				while(substr(szQueryStr,chr," ",szBuffer,sizeof(szBuffer)))
				{
					strcpyn(szQueryStr,szBuffer,sizeof(szQueryStr));
					memset(szBuffer,'\0',sizeof(szBuffer));
				}
			}

			strltrim(szQueryStr);
			strrtrim(szQueryStr);
			strstrim(szQueryStr);
			
			memset(szBuffer,'\0',sizeof(szBuffer));
			strcpy(szBuffer,"+%2B");
			for(i=0,n=4; szQueryStr[i]; i++,n++)
			{
				if(szQueryStr[i]==' ')
				{
					strcpyn(szBuffer+n,"+%2B",sizeof(szBuffer)-n);
					n+=3;
				}
				else
					szBuffer[n] = szQueryStr[i];
			}
			strcpyn(szQueryStr,szBuffer,sizeof(szQueryStr));

			// The Pirate Bay
			strcpyn(szFormat,m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_ED2KPIRATEBAY_QUERYSTRING_KEY),sizeof(szFormat));
			substr(szFormat,"%%QUERY%%",szQueryStr,szQuery,sizeof(szQuery));
			browser.Browse(szQuery);
			::Sleep(500);

			// FooFind
			strcpyn(szFormat,m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_ED2KFOOFIND_QUERYSTRING_KEY),sizeof(szFormat));
			substr(szFormat,"%%QUERY%%",szQueryStr,szQuery,sizeof(szQuery));
			browser.Browse(szQuery);
			::Sleep(500);

			// Google
			strcpyn(szFormat,m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_ED2KGOOGLE_QUERYSTRING_KEY),sizeof(szFormat));
			substr(szFormat,"%%QUERY%%",szQueryStr,szQuery,sizeof(szQuery));
			browser.Browse(szQuery);
			::Sleep(500);

			// YouTube
			strcpyn(szFormat,m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_YOUTUBE_QUERYSTRING_KEY),sizeof(szFormat));
			substr(szFormat,"%%QUERY%%",szQueryStr,szQuery,sizeof(szQuery));
			browser.Browse(szQuery);
		}
	}
}

/*
	OnButtonWebSearch()
	
	Cerca i links ed2k.
*/
void CWallPaperPlayerDlg::OnButtonWebSearch(void)
{	
	//BOOL bShiftPressed = ::GetKeyState(VK_SHIFT) < 0;
	//BOOL bCtrlPressed = ::GetKeyState(VK_CONTROL) < 0;
	
	OnEd2kQuery();
}

/*
	OnButtonInfo()
	
	Visualizza le info a proposito del brano corrente.
*/
void CWallPaperPlayerDlg::OnButtonInfo(void)
{	
	//BOOL bShiftPressed = ::GetKeyState(VK_SHIFT) < 0;
	//BOOL bCtrlPressed = ::GetKeyState(VK_CONTROL) < 0;
	
	ShowBalloonPopup(TRUE);
}

/*
	OnButtonPlay()
	
	Invia il messaggio per la riproduzione del brano corrente.
*/
void CWallPaperPlayerDlg::OnButtonPlay(void)
{	
	::SendMessage(this->m_hWnd,WM_AUDIOPLAYER_PLAY,0L,0L);
}

/*
	OnButtonStop()
	
	Invia il messaggio per fermare la riproduzione del brano corrente.
*/
void CWallPaperPlayerDlg::OnButtonStop(void)
{
	PostMessage(WM_AUDIOPLAYER_STOP);
}

/*
	OnButtonPause()
	
	Invia il messaggio per mettere in pausa la riproduzione del brano corrente.
*/
void CWallPaperPlayerDlg::OnButtonPause(void)
{
	PostMessage(WM_AUDIOPLAYER_PAUSE);
}

/*
	OnButtonPrev()
	
	Invia il messaggio per il passaggio al brano precedente.
*/
void CWallPaperPlayerDlg::OnButtonPrev(void)
{
	PostMessage(WM_AUDIOPLAYER_PREV);
}

/*
	OnButtonNext()
	
	Invia il messaggio per il passaggio al brano successivo.
*/
void CWallPaperPlayerDlg::OnButtonNext(void)
{
	PostMessage(WM_AUDIOPLAYER_NEXT);
}

/*
	OnButtonRemove()
	
	Invia il messaggio per l'eliminazione dalla playlist del brano corrente.
*/
void CWallPaperPlayerDlg::OnButtonRemove(void)
{
	PostMessage(WM_AUDIOPLAYER_REMOVE,MAKEWPARAM(1,0),0L);
}

/*
	OnButtonDelete()
	
	Invia il messaggio per l'eliminazione dalla playlist e dal disco del brano corrente.
*/
void CWallPaperPlayerDlg::OnButtonDelete(void)
{
	PostMessage(WM_AUDIOPLAYER_REMOVE,MAKEWPARAM(1,1),0L);
}

/*
	OnButtonMoveTo()
	
	Invia il messaggio per lo spostamento del file.
*/
void CWallPaperPlayerDlg::OnButtonMoveTo(void)
{
	::SendMessage(this->m_hWnd,WM_AUDIOPLAYER_REMOVE,MAKEWPARAM(1,2),0L);
}

/*
	OnButtonCopyTo()
	
	Invia il messaggio per la copia del file.
*/
void CWallPaperPlayerDlg::OnButtonCopyTo(void)
{
	::SendMessage(this->m_hWnd,WM_AUDIOPLAYER_REMOVE,MAKEWPARAM(1,3),0L);
}

/*
	OnPlayer()

	In input la modalita' (play|queue) ed il nome (o la lista) del file (audio).
	Da utilizzare solo dall'esterno per comunicare col player.
*/
LRESULT CWallPaperPlayerDlg::OnPlayer(WPARAM wParam,LPARAM lParam)
{
	// riproduzione di un singolo file
	if((int)wParam==AUDIOPLAYER_COMMAND_PLAY_FROM_FILE OR (int)wParam==AUDIOPLAYER_COMMAND_QUEUE_FROM_FILE)
	{
		char* pAudioFileName = (char*)lParam;
		if(pAudioFileName)
		{
			TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperPlayerDlg::OnPlayer(): %s request on %s\n",(int)wParam==AUDIOPLAYER_COMMAND_PLAY_FROM_FILE ? "play" : "queue",pAudioFileName));

			// cerca il file audio nella lista, se non lo trova lo inserisce
			ITERATOR iter;
			AUDIOITEM* audioItem;
			BOOL bFound = FALSE;
			int nIndex = -1;
			if(m_AudioFilesList.Lock(SYNC_5_SECS_TIMEOUT))
			{
				// se trova il nome file richiesto imposta l'indice sul valore relativo
				if((iter = m_AudioFilesList.First())!=(ITERATOR)NULL)
				{
					do
					{
						audioItem = (AUDIOITEM*)iter->data;
						if(audioItem)
						{
							if(stricmp(audioItem->file,pAudioFileName)==0)
							{
								nIndex = audioItem->index;
								TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperPlayerDlg::OnPlayer(): %s found at index %d\n",pAudioFileName,nIndex));
								bFound = TRUE;
								break;
							}
						}

						iter = m_AudioFilesList.Next(iter);
					
					} while(iter!=(ITERATOR)NULL);
				}

				// non ha trovato il nome file richiesto, lo inserisce e imposta l'indice sul valore relativo
				if(!bFound)
				{
					audioItem = new AUDIOITEM;
					if(audioItem)
					{
						strcpyn(audioItem->file,pAudioFileName,_MAX_FILEPATH+1);
						nIndex = audioItem->index = m_AudioFilesList.Count();
						TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperPlayerDlg::OnPlayer(): %s not found, adding it at index %d\n",pAudioFileName,nIndex));
						audioItem->lparam = (LPARAM)-1L;
						m_AudioFilesList.Add(audioItem);
					}
				}

				m_AudioFilesList.Unlock();
			}

			// invia il messaggio per la riproduzione del file audio
			if((int)wParam==AUDIOPLAYER_COMMAND_PLAY_FROM_FILE)
			{
				if(nIndex >= 0)
					m_nCurrentAudioIndex = nIndex;
				TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperPlayerDlg::OnPlayer(): posting WM_AUDIOPLAYER_PLAY\n"));
				PostMessage(WM_AUDIOPLAYER_PLAY);
			}
			else if((int)wParam==AUDIOPLAYER_COMMAND_QUEUE_FROM_FILE)
			{
				// il chiamante deve verificare lo status e se il player non si trova in esecuzione
				// deve aggiungere il file con PLAY non con QUEQUE, altrimenti non inizia a riprodurre
			}
		}
	}
	// riproduzione della lista dei files
	else if((int)wParam==AUDIOPLAYER_COMMAND_PLAY_FROM_LIST OR (int)wParam==AUDIOPLAYER_COMMAND_QUEUE_FROM_LIST)
	{
		CAudioFilesList* pAudioFileList = (CAudioFilesList*)lParam;
		if(pAudioFileList)
		{
			TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperPlayerDlg::OnPlayer(): request %s on <filelist>\n",(int)wParam==AUDIOPLAYER_COMMAND_PLAY_FROM_FILE ? "play" : "queue"));

			ITERATOR iter;
			AUDIOITEM* audioItem;
			if(m_AudioFilesList.Lock(SYNC_5_SECS_TIMEOUT))
			{
				// aggiunge gli elementi della lista ricevuta alla playlist corrente (non controlla i duplicati)
				if((iter = pAudioFileList->First())!=(ITERATOR)NULL)
				{
					do
					{
						audioItem = (AUDIOITEM*)iter->data;
						if(audioItem)
						{
							AUDIOITEM* p = (AUDIOITEM*)m_AudioFilesList.Add();
							if(p)
								memcpy(p,audioItem,sizeof(AUDIOITEM));
						}

						iter = pAudioFileList->Next(iter);
					
					} while(iter!=(ITERATOR)NULL);
				}

				int nIndex = 0;
				
				// rinumera gli elementi della playlist
				if((iter = m_AudioFilesList.First())!=(ITERATOR)NULL)
				{
					do
					{
						audioItem = (AUDIOITEM*)iter->data;
						if(audioItem)
							audioItem->index = nIndex++;

						iter = m_AudioFilesList.Next(iter);
					
					} while(iter!=(ITERATOR)NULL);
				}

				m_AudioFilesList.Unlock();
			}

			// invia il messaggio per la riproduzione del file audio
			if((int)wParam==AUDIOPLAYER_COMMAND_PLAY_FROM_LIST)
			{
				TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperPlayerDlg::OnPlayer(): posting WM_AUDIOPLAYER_PLAY\n"));
				m_nCurrentAudioIndex = 0;
				PostMessage(WM_AUDIOPLAYER_PLAY);
			}
			else if((int)wParam==AUDIOPLAYER_COMMAND_QUEUE_FROM_LIST)
			{
				// il chiamante deve verificare lo status e se il player non si trova in esecuzione
				// deve aggiungere il file con PLAY non con QUEQUE, altrimenti non inizia a riprodurre
			}
		}
	}

	return(0L);
}

/*
	OnPlayerSetMode()
	
	Imposta le modalita' di riproduzione.
*/
LRESULT CWallPaperPlayerDlg::OnPlayerSetMode(WPARAM wParam,LPARAM lParam)
{
	switch((int)wParam)
	{
		case AUDIOPLAYER_MODE_TASKBAR_POPUP:
		case AUDIOPLAYER_MODE_BALLOON_POPUP:
		case AUDIOPLAYER_MODE_NOPOPUP:
		{
			m_nTaskbarPopupMode = (int)wParam;
			switch(m_nTaskbarPopupMode)
			{
				case AUDIOPLAYER_MODE_TASKBAR_POPUP:
					if(m_pTrayIcon)
						m_pTrayIcon->Balloon();
					break;
				
				case AUDIOPLAYER_MODE_BALLOON_POPUP:
					m_nCurrentAudioIcon = -1;
					m_nAudioIconsCount = 0;
					m_pFindAudioIcons = (CFindFile*)lParam;
					if(m_pFindAudioIcons)
						m_nAudioIconsCount = m_pFindAudioIcons->Count();
					if(m_pTaskbarNotifier)
						m_pTaskbarNotifier->UnShow();
					break;
								
				case AUDIOPLAYER_MODE_NOPOPUP:
					if(m_pTaskbarNotifier)
						m_pTaskbarNotifier->UnShow();
					if(m_pTrayIcon)
						m_pTrayIcon->Balloon();
					break;
			}
		}
		break;

		case AUDIOPLAYER_MODE_RELOAD_BALLOON:
		{
			ShowBalloonPopup();
		}
		break;

		case AUDIOPLAYER_MODE_AUTOSKIP:
		{
			m_bAutoSkip = (BOOL)lParam;
		}
		break;
	
		case AUDIOPLAYER_MODE_FEEDBACK:
		{
			m_bFeedback = (BOOL)lParam;
		}
		break;
	}

	return(0L);
}

/*
	OnPlayerSetVolume()
	
	Imposta il volume.
*/
LRESULT CWallPaperPlayerDlg::OnPlayerSetVolume(WPARAM wParam,LPARAM /*lParam*/)
{
	if((int)wParam==AUDIOPLAYER_COMMAND_INCREASE_VOLUME)
		m_AudioPlayer.IncreaseVolume();
	else if((int)wParam==AUDIOPLAYER_COMMAND_DECREASE_VOLUME)
		m_AudioPlayer.DecreaseVolume();

	return(0L);
}

/*
	OnPlayerPlay()

	Riproduce il brano corrente (quello relativo all'indice corrente).
*/
LRESULT CWallPaperPlayerDlg::OnPlayerPlay(WPARAM /*wParam*/,LPARAM /*lParam*/)
{
	BOOL bPlaying = FALSE;

	if(m_syncAudioPlay.Lock())
	{
		// se in pausa, riprende
		if(m_AudioPlayer.GetStatus()==mmAudioPmPaused)
		{			
			bPlaying = m_AudioPlayer.Resume();
			TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperPlayerDlg::OnPlayerPlay(): paused, %s resumed\n",bPlaying ? "now" : "not"));
			
			// imposta l'icona con i dati del brano corrente
			SetTrayIcon();
		}
		else // altrimenti carica il file audio seguente (o il primo se ancora non ha iniziato a riprodurre)
		{
			// chiude il player per riaprirlo sotto
			if(m_AudioPlayer.GetStatus()==mmAudioPmPlaying)
			{
				TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperPlayerDlg::OnPlayerPlay(): stopping\n"));
				m_AudioPlayer.Stop();
			}
			if(m_AudioPlayer.GetStatus()!=mmAudioPmClosed)
			{
				TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperPlayerDlg::OnPlayerPlay(): closing\n"));
				m_AudioPlayer.Close();
			}

			// imposta in modo tale che il chiamante non spari a raffica files da riprodurre se trova il player chiuso
			// quando in effetti il player sta caricando (ad es. estraendo da un archivio compresso) il file gia' specificato
			m_mmInternalMode = mmAudioPmLoading;

			// ricava il nome file relativo all'indice corrente
			BOOL bHaveFile = TRUE;
			if(m_nCurrentAudioIndex < 0 OR m_nCurrentAudioIndex >= m_AudioFilesList.Count())
				m_nCurrentAudioIndex = 0;
			AUDIOITEM* audioItem = NULL;
			ITERATOR iter;
			bHaveFile = FALSE;
			if(m_AudioFilesList.Lock(SYNC_5_SECS_TIMEOUT))
			{
				int nFoundAt = -1;
				
				// ricava (una sola volta) l'ultimo brano riprodotto nella sessione precedente
				if(m_bFirstCall)
				{
					m_bFirstCall = FALSE;
					
					if((iter = m_AudioFilesList.First())!=(ITERATOR)NULL)
					{
						do
						{
							audioItem = (AUDIOITEM*)iter->data;
							if(audioItem)
							{
								if(strcmp(audioItem->file,m_szLastPlayedSong)==0)
								{
									nFoundAt = m_nCurrentAudioIndex = audioItem->index;
									bHaveFile = TRUE;
									break;
								}
							}

							iter = m_AudioFilesList.Next(iter);
							
						} while(iter!=(ITERATOR)NULL);
					}
				}
				
				if(nFoundAt < 0)
					if((iter = m_AudioFilesList.First())!=(ITERATOR)NULL)
					{
						do
						{
							audioItem = (AUDIOITEM*)iter->data;
							if(audioItem)
							{
								if(audioItem->index==m_nCurrentAudioIndex)
								{
									bHaveFile = TRUE;
									break;
								}
							}

							iter = m_AudioFilesList.Next(iter);
							
						} while(iter!=(ITERATOR)NULL);
					}

				m_AudioFilesList.Unlock();
			}
			
			if(!bHaveFile)
			{
				TRACEEXPR((_TRACE_FLAG_ERR,__NOFILE__,__NOLINE__,"CWallPaperPlayerDlg::OnPlayerPlay(): no file! current index %d\n",m_nCurrentAudioIndex));
				audioItem = NULL;
			}
			else
			{
				TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperPlayerDlg::OnPlayerPlay(): current file %s at index %d\n",audioItem->file,audioItem->index));
			}

			if(audioItem)
			{
				strcpyn(m_szLastPlayedSong,audioItem->file,sizeof(m_szLastPlayedSong));
				strcpyn(m_szAudioFileName,audioItem->file,sizeof(m_szAudioFileName));
				strcpyn(m_szFavouriteName,audioItem->file,sizeof(m_szFavouriteName));
				
				// controlla se si tratta di un file compresso
				if(audioItem->lparam!=(LPARAM)-1L)
				{
					char szParentFileName[_MAX_FILEPATH+1] = {0};
					EXTRACTFILEINFO extractFileInfo = {0};
					extractFileInfo.lParam = audioItem->lparam;
					extractFileInfo.lpszInputFileName = audioItem->file;
					extractFileInfo.lpszOutputFileName = m_szAudioFileName;
					extractFileInfo.cbOutputFileName = sizeof(m_szAudioFileName);
					extractFileInfo.lpszParentFileName = szParentFileName;
					extractFileInfo.cbParentFileName = sizeof(szParentFileName);
					memset(m_szFavouriteName,'\0',sizeof(m_szFavouriteName));
					
					// imposta l'icona
					char szString[_MAX_PATH+1];
					::FormatResourceString(szString,sizeof(szString),IDS_MESSAGE_UNCOMPRESSING);
					SetTrayIcon(szString);
					
					bHaveFile = FALSE;
					if(m_hWndParent)
					{
						TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperPlayerDlg::OnPlayerPlay(): extracting %s\n",audioItem->file));
						
						LONG lRet;
						TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperPlayerDlg::OnPlayerPlay(): sending WM_ARCHIVE_EXTRACT_FILE\n"));
						if((lRet = ::SendMessage(m_hWndParent,WM_ARCHIVE_EXTRACT_FILE,0L,(LPARAM)&extractFileInfo))!=0L)
						{
							TRACEEXPR((_TRACE_FLAG_ERR,__NOFILE__,__NOLINE__,"CWallPaperPlayerDlg::OnPlayerPlay(): extraction failed\n"));
							strcpyn(m_szAudioFileName,audioItem->file,sizeof(m_szAudioFileName));
							// se il file compresso non esiste non visualizza il messaggio d'errore
							if(::FileExist(extractFileInfo.lpszParentFileName))
								::MessageBoxResourceEx(this->m_hWnd,MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_EXTRACT_FILE,m_szAudioFileName,audioItem->file,"",lRet);
						}
						else
						{
							TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperPlayerDlg::OnPlayerPlay(): extracted into %s\n",m_szAudioFileName));
							_snprintf(m_szFavouriteName,sizeof(m_szFavouriteName)-1,"%s\\%s",extractFileInfo.lpszParentFileName,extractFileInfo.lpszInputFileName);
							bHaveFile = TRUE;
						}
					}
					
					// imposta l'icona
					SetTrayIcon(IDS_DIALOG_PLAYER_TITLE);
				}
			}

			// ripulisce la directory temporanea dai files audio esistenti (escludendo quello corrente)
			if(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CLEAR_TEMPORARY_FILES_KEY)==TEMPORARY_FILES_CLEAR_AT_RUNTIME)
			{
				LPSTR pFileName;
				CFindFile findFile;
				while((pFileName = (LPSTR)findFile.FindEx(m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_TEMPDIR_KEY),"*.*",TRUE))!=NULL)
				{
					if(CAudioPlayer::IsSupportedFormat(pFileName))
					{
						if(strcmp(pFileName,m_szAudioFileName)!=0)
							::DeleteFile(pFileName);
					}
				}
			}

			// se dispone di un nome file valido
			if(!strnull(m_szAudioFileName) AND bHaveFile)
			{
				// apre il player
				if(m_AudioPlayer.Open(m_szAudioFileName))
				{
					// imposta per il player
					m_AudioPlayer.SetPriority(THREAD_PRIORITY_HIGHEST);

					// carica le info (tags) del brano corrente
					GetAudioInfo();
					
					// imposta l'icona con i dati del brano corrente
					SetTrayIcon();

					// riproduce
					if(m_AudioPlayer.GetStatus()==mmAudioPmPaused)
						bPlaying = m_AudioPlayer.Resume();
					else
						bPlaying = m_AudioPlayer.Play();

					// visualizza la finestra/tooltip popup
					ShowTaskbarPopup();
					ShowBalloonPopup();
				}
				else
				{
					TRACEEXPR((_TRACE_FLAG_ERR,__NOFILE__,__NOLINE__,"CWallPaperPlayerDlg::OnPlayerPlay(): unable to open %s\n",m_szAudioFileName));
				}
			}
			else
			{
				TRACEEXPR((_TRACE_FLAG_ERR,__NOFILE__,__NOLINE__,"CWallPaperPlayerDlg::OnPlayerPlay(): no file!\n"));
			}
			
			// imposta in modo tale che il chiamante non spari a raffica files da riprodurre se trova il player chiuso
			// quando in effetti il player sta caricando (ad es. estraendo da un archivio compresso) il file gia' specificato
			m_mmInternalMode = mmAudioPmUndefined;
		}

		m_syncAudioPlay.Unlock();
	}
	else
	{
		TRACEEXPR((_TRACE_FLAG_ERR,__NOFILE__,__NOLINE__,"CWallPaperPlayerDlg::OnButtonPlay(): lock failed\n"));
	}
	
	// invia WM_AUDIOPLAYER_EXCEPTION per l'apposito gestore...
	if(!bPlaying)
	{
		TRACEEXPR((_TRACE_FLAG_ERR,__NOFILE__,__NOLINE__,"CWallPaperPlayerDlg::OnPlayerPlay(): sending WM_AUDIOPLAYER_EXCEPTION\n"));
		::SendMessage(this->m_hWnd,WM_AUDIOPLAYER_EXCEPTION,(WPARAM)-1L,(LPARAM)m_szAudioFileName);
	}
	else // informa la finestra principale sul brano in riproduzione
	{
		if(m_hWndParent AND m_bFeedback)
		{
			TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperPlayerDlg::OnPlayerPlay(): sending WM_AUDIOPLAYER_EVENT,AUDIOPLAYER_COMMAND_PLAY\n"));
			::SendMessage(m_hWndParent,WM_AUDIOPLAYER_EVENT,(WPARAM)AUDIOPLAYER_COMMAND_PLAY,(LPARAM)m_szAudioFileName);
		}
	}

	return(0L);
}

/*
	OnPlayerStop()

	Ferma la riproduzione del brano corrente.
*/
LRESULT CWallPaperPlayerDlg::OnPlayerStop(WPARAM /*wParam*/,LPARAM /*lParam*/)
{	
	// ferma la riproduzione del brano
	if(m_AudioPlayer.GetStatus()!=mmAudioPmStopped)
	{
		TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperPlayerDlg::OnPlayerStop(): stopping player\n"));
		m_AudioPlayer.Stop();
	}

	// imposta l'icona su stopped
	if(m_pTrayIcon)
	{
		char szToolTipText[TRAYICON_MAX_TOOLTIP_TEXT+1];
		char szString[TRAYICON_MAX_TOOLTIP_TEXT+1];
		::FormatResourceString(szString,sizeof(szString),IDS_MESSAGE_STOPPED);
		int n = _snprintf(szToolTipText,sizeof(szToolTipText)-1,"(%s) ",szString);
		strcpyn(szToolTipText + n,m_pTrayIcon->GetToolTip(),sizeof(szToolTipText) - n);
		SetTrayIcon(szToolTipText,IDI_ICON_PLAYER_STOPPED);
	}
	
	// informa la finestra principale sullo status
	if(m_bFeedback)
		if(m_hWndParent)
		{
			TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperPlayerDlg::OnPlayerStop(): sending WM_AUDIOPLAYER_EVENT,AUDIOPLAYER_COMMAND_STOP\n"));
			::SendMessage(m_hWndParent,WM_AUDIOPLAYER_EVENT,(WPARAM)AUDIOPLAYER_COMMAND_STOP,(LPARAM)m_szAudioFileName);
		}

	return(0L);
}

/*
	OnPlayerPause()

	Mette in pausa la riproduzione del brano corrente.
*/
LRESULT CWallPaperPlayerDlg::OnPlayerPause(WPARAM /*wParam*/,LPARAM /*lParam*/)
{
	// mette in pause la riproduzione del brano
	if(m_AudioPlayer.GetStatus()!=mmAudioPmPaused)
	{
		TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperPlayerDlg::OnPlayerPause(): pausing player\n"));
		m_AudioPlayer.Pause();
	}

	// imposta l'icona su paused
	if(m_pTrayIcon)
	{
		char szToolTipText[TRAYICON_MAX_TOOLTIP_TEXT+1];
		char szString[TRAYICON_MAX_TOOLTIP_TEXT+1];
		::FormatResourceString(szString,sizeof(szString),IDS_MESSAGE_PAUSED);
		int n = _snprintf(szToolTipText,sizeof(szToolTipText)-1,"(%s) ",szString);
		strcpyn(szToolTipText + n,m_pTrayIcon->GetToolTip(),sizeof(szToolTipText) - n);
		SetTrayIcon(szToolTipText,IDI_ICON_PLAYER_PAUSED);
	}

	// informa la finestra principale sullo status
	if(m_bFeedback)
		if(m_hWndParent)
		{
			TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperPlayerDlg::OnPlayerPause(): sending WM_AUDIOPLAYER_EVENT,AUDIOPLAYER_COMMAND_PAUSE\n"));
			::SendMessage(m_hWndParent,WM_AUDIOPLAYER_EVENT,(WPARAM)AUDIOPLAYER_COMMAND_PAUSE,(LPARAM)m_szAudioFileName);
		}
		
	return(0L);
}

/*
	OnPlayerNext()

	Riproduce il brano successivo.
*/
LRESULT CWallPaperPlayerDlg::OnPlayerNext(WPARAM /*wParam*/,LPARAM /*lParam*/)
{
	int nAudioFilesCount = m_AudioFilesList.Count();

	if(nAudioFilesCount > 0)
	{
		// passa al file audio successivo
		int nPlayListMode = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_RANDOMIZE_PLAYLIST_KEY);
		if(nPlayListMode==RANDOMIZE_PLAYLIST_NONE OR nPlayListMode==RANDOMIZE_PLAYLIST_REORDER)
		{
			// sequenziale/riordino
			// il riordino (della playlist) viene effettuato una sola volta (per ora dal chiamante), non per ogni passaggio al brano successivo
			if(++m_nCurrentAudioIndex >= nAudioFilesCount)
				m_nCurrentAudioIndex = 0;
		}
		else if(nPlayListMode==RANDOMIZE_PLAYLIST_SELECTION)
		{
			// pure random
			m_nCurrentAudioIndex = m_randomNumberGenerator.RandRange(0,nAudioFilesCount-1);
		}

		// invia il messaggio per la riproduzione del file audio
		TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperPlayerDlg::OnPlayerNext(): posting WM_AUDIOPLAYER_PLAY\n"));
		PostMessage(WM_AUDIOPLAYER_PLAY);
	}

	return(0L);
}

/*
	OnPlayerPrev()

	Riproduce il brano precedente.
*/
LRESULT CWallPaperPlayerDlg::OnPlayerPrev(WPARAM /*wParam*/,LPARAM /*lParam*/)
{
	int nAudioFilesCount = m_AudioFilesList.Count();

	if(nAudioFilesCount > 0)
	{
		// passa al file audio precedente
		int nPlayListMode = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_RANDOMIZE_PLAYLIST_KEY);
		if(nPlayListMode==RANDOMIZE_PLAYLIST_NONE OR nPlayListMode==RANDOMIZE_PLAYLIST_REORDER)
		{
			// sequenziale/riordino
			// il riordino (della playlist) viene effettuato una sola volta (per ora dal chiamante), non per ogni passaggio al brano successivo
			if(--m_nCurrentAudioIndex < 0)
				m_nCurrentAudioIndex = nAudioFilesCount-1;
		}
		else if(nPlayListMode==RANDOMIZE_PLAYLIST_SELECTION)
		{
			// pure random
			m_nCurrentAudioIndex = m_randomNumberGenerator.RandRange(0,nAudioFilesCount-1);
		}

		// invia il messaggio per la riproduzione del file audio
		TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperPlayerDlg::OnPlayerPrev(): posting WM_AUDIOPLAYER_PLAY\n"));
		PostMessage(WM_AUDIOPLAYER_PLAY);
	}

	return(0L);
}

/*
	OnPlayerRemove()

	Elimina (o sposta o copia) il brano dalla playlist/disco.
*/
LRESULT CWallPaperPlayerDlg::OnPlayerRemove(WPARAM wParam,LPARAM lParam)
{
	BOOL bTerminateIfEmptyList = (BOOL)LOWORD(wParam);
	BOOL bDeleteFile = (BOOL)HIWORD(wParam);
	BOOL bMoveFile = HIWORD(wParam)==2;
	BOOL bCopyFile = HIWORD(wParam)==3;
	BOOL bPlayCurrent = FALSE;
	BOOL bIsTheSameFile = FALSE;
	int nAudioFilesCount = m_AudioFilesList.Count();
	char* pAudioFileName = (char*)lParam;

	// se non viene specificato nessun nome file, assume quello in riproduzione
	if(!pAudioFileName)
		pAudioFileName = m_szAudioFileName;

	// per sapere se il file da eliminare e' quello in riproduzione
	// se il file e' contenuto in un file compresso, il nome che viene passato e' senza pathname mentre
	// quello in riproduzione e' <path tmp>+nome, per cui se trova il path temporaneo assume che provenga
	// da un file compresso ed elimina il pathname per il confronto
	char* pTmpDir = (char*)m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_TEMPDIR_KEY);
	int n = strlen(pTmpDir);
	char* pFileName;
	char* pCurrentFileName;
	
	pFileName = stristr(pAudioFileName,pTmpDir);
	if(pFileName)
		pFileName = *(pFileName+n) ? pFileName+n : NULL;
	if(!pFileName)
		pFileName = pAudioFileName;
	pAudioFileName = pFileName;
	
	pCurrentFileName = stristr(m_szAudioFileName,pTmpDir);
	if(pCurrentFileName)
		pCurrentFileName = *(pCurrentFileName+n) ? pCurrentFileName+n : NULL;
	if(!pCurrentFileName)
		pCurrentFileName = m_szAudioFileName;
	bIsTheSameFile = stricmp(pCurrentFileName,pFileName)==0;
	
	TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperPlayerDlg::OnPlayerRemove(): current %s, to be removed %s, is the same: %s\n",pCurrentFileName,pFileName,bIsTheSameFile ? "yes" : "no"));

	// elimina il file specificato dalla lista
	if(m_AudioFilesList.Count() > 0 AND pAudioFileName)
	{
		ITERATOR iter;
		AUDIOITEM* audioItem;
		if(m_AudioFilesList.Lock(SYNC_5_SECS_TIMEOUT))
		{
			// scorre la playlist alla ricerca del file
			if((iter = m_AudioFilesList.First())!=(ITERATOR)NULL)
			{
				do
				{
					audioItem = (AUDIOITEM*)iter->data;
					if(audioItem)
					{
						// ha trovato il file specificato nella lista
						if(stricmp(pAudioFileName,audioItem->file)==0)
						{
							// solo per eliminazione o spostamento, non per la copia
							if(!bCopyFile)
							{
								TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperPlayerDlg::OnPlayerRemove(): removing %s at index %d\n",audioItem->file,audioItem->index));
								
								// se il file da eliminare e' quello in riproduzione ferma il player
								if(bIsTheSameFile)
								{
									TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperPlayerDlg::OnPlayerRemove(): is the same file, sending WM_AUDIOPLAYER_STOP\n"));
									::SendMessage(this->m_hWnd,WM_AUDIOPLAYER_STOP,0L,0L);
								}

								// aggiorna la posizione corrente
								if(audioItem->index <= m_nCurrentAudioIndex)
								{
									if(--m_nCurrentAudioIndex < 0)
									{
										m_nCurrentAudioIndex = 0;
										bPlayCurrent = TRUE;
									}
								}
							}

							// se il file deve essere spostato o copiato
							if(bMoveFile OR bCopyFile)
								bDeleteFile = FALSE;
							
							// se il file deve essere cancellato disco
							if(bIsTheSameFile AND bDeleteFile)
							{
								TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperPlayerDlg::OnPlayerRemove(): deleting %s\n",pAudioFileName));
								
								BOOL bDelete = FALSE;
								BOOL bConfirmFileDelete = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CONFIRM_FILE_DELETE_KEY);
								
								if(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_DELETE_FILES_TO_RECYCLEBIN_KEY))
								{
									bDelete = ::DeleteFileToRecycleBin(this->m_hWnd,pAudioFileName,bConfirmFileDelete ? TRUE : FALSE);
								}
								else
								{
									if(bConfirmFileDelete)
										bDelete = ::MessageBoxResourceEx(this->m_hWnd,MB_YESNO|MB_ICONQUESTION,WALLPAPER_PROGRAM_NAME,IDS_QUESTION_DELETE_FILE,pAudioFileName)==IDYES;
									else
										bDelete = TRUE;
									if(bDelete)
										::DeleteFile(pAudioFileName);
								}
							}
							// se il file deve essere spostato o copiato
							else if(bIsTheSameFile AND (bMoveFile OR bCopyFile))
							{
								TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperPlayerDlg::OnPlayerRemove(): moving %s\n",pAudioFileName));
								
								// selezione della directory
								CDirDialog dlg(m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_MOVETO_KEY),
											"Select Directory...",
											"Select the output folder:",
											m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_ALWAYSONTOP_KEY)
											);
								
								if(dlg.DoModal(this->m_hWnd)==IDOK)
								{
									char szFolder[_MAX_FILEPATH+1] = {0};
									strcpyn(szFolder,dlg.GetPathName(),sizeof(szFolder));
									m_pConfig->UpdateString(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_MOVETO_KEY,szFolder);
									m_pConfig->SaveKey(WALLPAPER_INSTALL_KEY,WALLPAPER_LAST_MOVETO_KEY);
									
									CWaitCursor cursor;
									_snprintf(szFolder,sizeof(szFolder)-1,"%s\\%s",dlg.GetPathName(),::StripPathFromFile(pAudioFileName));
									if(bMoveFile)
										CopyFileTo(this->m_hWnd,pAudioFileName,szFolder,TRUE);
									else if(bCopyFile)
										CopyFileTo(this->m_hWnd,pAudioFileName,szFolder);
								}
							}
							
							// solo per eliminazione o spostamento, non per la copia
							if(!bCopyFile)
							{
								// informa la finestra principale sulla rimozione dell'elemento
								if(m_bFeedback)
									if(m_hWndParent)
									{
										int i = 0;
										char szFileName[_MAX_FILEPATH+1] = {0};

										if(audioItem->lparam!=(LPARAM)-1L)
										{
											TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperPlayerDlg::OnPlayerRemove(): getting parent for %s\n",audioItem->file));

											char szParentFile[_MAX_FILEPATH+1] = {0};
											EXTRACTFILEINFO extractFileInfo = {0};
											extractFileInfo.lParam = audioItem->lparam;
											extractFileInfo.lpszParentFileName = szParentFile;
											extractFileInfo.cbParentFileName = sizeof(szParentFile);
											TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperPlayerDlg::OnPlayerRemove(): sending WM_ARCHIVE_GET_PARENT\n"));
											if(::SendMessage(m_hWndParent,WM_ARCHIVE_GET_PARENT,0L,(LPARAM)&extractFileInfo)==0L)
												i = _snprintf(szFileName,sizeof(szFileName)-1,"%s\\",extractFileInfo.lpszParentFileName);
										}
									
										TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperPlayerDlg::OnPlayerRemove(): sending WM_AUDIOPLAYER_REMOVEITEM\n"));
										_snprintf(szFileName+i,sizeof(szFileName)-1-i,"%s",pAudioFileName);
										::SendMessage(m_hWndParent,WM_AUDIOPLAYER_REMOVEITEM,0L,(LPARAM)szFileName);
									}

								// elimina il file specificato dalla lista
								m_AudioFilesList.Erase(iter);

								// rinumera tutti gli elementi della lista
								nAudioFilesCount = 0;
								if((iter = m_AudioFilesList.First())!=(ITERATOR)NULL)
								{
									do
									{
										audioItem = (AUDIOITEM*)iter->data;
										if(audioItem)
											audioItem->index = nAudioFilesCount++;
										iter = m_AudioFilesList.Next(iter);
									} while(iter!=(ITERATOR)NULL);
								}
							}

							break;
						}
					}

					iter = m_AudioFilesList.Next(iter);
				
				} while(iter!=(ITERATOR)NULL);
			}

			m_AudioFilesList.Unlock();
		}
	}

	// solo per eliminazione o spostamento, non per la copia
	if(!bCopyFile)
	{
		// se non rimangono piu' files esce
		if(bTerminateIfEmptyList AND m_AudioFilesList.Count() <= 0)
		{
			TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperPlayerDlg::OnPlayerRemove(): no more files, posting WM_AUDIOPLAYER_EXIT\n"));
			PostMessage(WM_AUDIOPLAYER_EXIT);
		}
		else
		{
			// se il file da eliminare era quello in riproduzione manda il esecuzione il successivo
			if(bIsTheSameFile)
				if(m_bAutoSkip)
				{
					if(bPlayCurrent)
					{
						TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperPlayerDlg::OnPlayerRemove(): autoskip: posting WM_AUDIOPLAYER_PLAY\n"));
						PostMessage(WM_AUDIOPLAYER_PLAY);
					}
					else
					{
						TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperPlayerDlg::OnPlayerRemove(): autoskip: posting WM_AUDIOPLAYER_NEXT\n"));
						PostMessage(WM_AUDIOPLAYER_NEXT);
					}
				}
		}
	}

	return(0L);
}

/*
	OnPlayerRemoveAll()

	Elimina tutti i brani presenti nella playlist.
*/
LRESULT CWallPaperPlayerDlg::OnPlayerRemoveAll(WPARAM /*wParam*/,LPARAM /*lParam*/)
{
	// chiude il player
	if(m_AudioPlayer.GetStatus()==mmAudioPmPlaying)
		m_AudioPlayer.Stop();
	if(m_AudioPlayer.GetStatus()!=mmAudioPmClosed)
		m_AudioPlayer.Close();

	// azzera la lista dei files
	m_nCurrentAudioIndex = 0;
	memset(m_szLastPlayedSong,'\0',sizeof(m_szLastPlayedSong));
	memset(m_szAudioFileName,'\0',sizeof(m_szAudioFileName));
	memset(m_szFavouriteName,'\0',sizeof(m_szFavouriteName));
	
	if(m_AudioFilesList.Lock(SYNC_5_SECS_TIMEOUT))
	{
		m_AudioFilesList.EraseAll();
		m_AudioFilesList.Unlock();
	}

	// eliminando tutti i files esce
	TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperPlayerDlg::OnPlayerRemoveAll(): no more files, posting WM_AUDIOPLAYER_EXIT\n"));
	PostMessage(WM_AUDIOPLAYER_EXIT);

	return(0L);
}

/*
	OnPlayerExit()

	Chiude il player (notare che per chiamare la OnExit() invia il comando di menu relativo).
*/
LRESULT CWallPaperPlayerDlg::OnPlayerExit(WPARAM /*wParam*/,LPARAM /*lParam*/)
{
	// informa la finestra principale sullo status
	if(m_hWndParent)
	{
		TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperPlayerDlg::OnPlayerExit(): sending WM_AUDIOPLAYER_EVENT,AUDIOPLAYER_COMMAND_EXIT\n"));
		::SendMessage(m_hWndParent,WM_AUDIOPLAYER_EVENT,(WPARAM)AUDIOPLAYER_COMMAND_EXIT,(LPARAM)0L);
	}

	TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperPlayerDlg::OnPlayerExit(): posting IDM_MENU_EXIT\n"));
	PostMessage(WM_COMMAND,MAKELONG(IDM_MENU_EXIT,0),0L);

	return(0L);
}

/*
	OnPlayerGetStatus()
	
	Restituisce lo status corrente del player.
*/
LRESULT CWallPaperPlayerDlg::OnPlayerGetStatus(WPARAM /*wParam*/,LPARAM /*lParam*/)
{
	LRESULT lStatus = m_AudioPlayer.GetStatus();
	TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperPlayerDlg::OnPlayerGetStatus(): current status: %ld, internal: %ld\n",lStatus,m_mmInternalMode));

	// imposta in modo tale che il chiamante non spari a raffica files da riprodurre se trova il player chiuso
	// quando in effetti il player sta caricando (ad es. estraendo da un archivio compresso) il file gia' specificato
	if(m_mmInternalMode==mmAudioPmLoading)
		lStatus = m_mmInternalMode;

	return(lStatus);
}

/*
	OnPlayerSetStatus()

	Imposta lo status corrente del player (mantenere allineato con le definizioni dei comandi).
	Da usare per comunicare dall'esterno.
*/
LRESULT CWallPaperPlayerDlg::OnPlayerSetStatus(WPARAM wParam,LPARAM /*lParam*/)
{
	mmAudioPlayerMode playerStatus = m_AudioPlayer.GetStatus();

	// usare Send... e non Post... per restituire correttamente lo status corrente
	switch(wParam)
	{
		case AUDIOPLAYER_COMMAND_NULL:
			break;
		case AUDIOPLAYER_COMMAND_EXIT:
			break;
		case AUDIOPLAYER_COMMAND_STOP:
			break;
		case AUDIOPLAYER_COMMAND_PLAY:
			break;
		case AUDIOPLAYER_COMMAND_PAUSE:
			break;
		case AUDIOPLAYER_COMMAND_PLAY_FROM_FILE:
			break;
		case AUDIOPLAYER_COMMAND_QUEUE_FROM_FILE:
			break;
		case AUDIOPLAYER_COMMAND_PLAY_FROM_LIST:
			break;
		case AUDIOPLAYER_COMMAND_QUEUE_FROM_LIST:
			break;
		case AUDIOPLAYER_COMMAND_INCREASE_VOLUME:
			break;
		case AUDIOPLAYER_COMMAND_DECREASE_VOLUME:
			break;
		case AUDIOPLAYER_COMMAND_TOGGLE_PLAYING:
			TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperPlayerDlg::OnPlayerSetStatus(): posting WM_AUDIOPLAYER_PAUSE/WM_AUDIOPLAYER_PLAY\n"));
			PostMessage(playerStatus==mmAudioPmPlaying ? WM_AUDIOPLAYER_PAUSE : WM_AUDIOPLAYER_PLAY);
			break;
	}

	return(m_AudioPlayer.GetStatus());
}

/*
	OnPlayerSetPopupList()

	Imposta la lista per i popups audio.
*/
LRESULT CWallPaperPlayerDlg::OnPlayerSetPopupList(WPARAM wParam,LPARAM lParam)
{
	// azzera l'elemento interno
	memset(&m_TaskbarPopup,'\0',sizeof(TASKBARPOPUP));

	if(wParam==AUDIOPLAYER_MODE_SETPOPUPLIST OR wParam==AUDIOPLAYER_MODE_LOADPOPUPLIST)
	{
		// puntatore alla lista per i bitmap per i popup
		if((CTaskbarPopupList*)lParam)
		{
			m_pListTaskbarBitmaps = (CTaskbarPopupList*)lParam;
			if(m_pListTaskbarBitmaps)
				if(m_pListTaskbarBitmaps->Count() > 0)
				{
					m_nPopupIndex = 0;
					TASKBARPOPUP* pAudioPopupInfoItem = (TASKBARPOPUP*)m_pListTaskbarBitmaps->GetAt(m_nPopupIndex++);
					if(pAudioPopupInfoItem)
						memcpy(&m_TaskbarPopup,pAudioPopupInfoItem,sizeof(TASKBARPOPUP));
				}
		}
	}

	if(wParam==AUDIOPLAYER_MODE_LOADPOPUPLIST)
	{
		GetAudioInfo();
		SetTrayIcon();
		ShowTaskbarPopup();
	}

	if(wParam==AUDIOPLAYER_MODE_LOADBALLOONLIST)
	{
		GetAudioInfo();
		SetTrayIcon();
		ShowBalloonPopup();
	}

	return(0L);
}

/*
	OnPlayerException()

	Gestore (minimo) per le eccezioni.
*/
LRESULT CWallPaperPlayerDlg::OnPlayerException(WPARAM wParam,LPARAM lParam)
{
	if(wParam==(WPARAM)-1L AND m_AudioFilesList.Count() > 0)
	{
		// salva in locale il nome file (se come lParam viene passato il buffer per il nome file corrente)
		char szFileName[_MAX_FILEPATH+1];
		strcpyn(szFileName,(LPCSTR)lParam,sizeof(szFileName));

		// sdraia il popup
		if(m_pTaskbarNotifier)
			m_pTaskbarNotifier->UnShow();
		if(m_pTrayIcon)
			m_pTrayIcon->Balloon();

		// rimuove dalla lista l'elemento che non e' riuscito a riprodurre
		TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperPlayerDlg::OnPlayerException(): sending WM_AUDIOPLAYER_REMOVE\n"));
		::SendMessage(this->m_hWnd,WM_AUDIOPLAYER_REMOVE,MAKEWPARAM(1,0),lParam);
		
		// informa la finestra principale sulla rimozione dell'elemento
		if(m_bFeedback)
			if(m_hWndParent)
			{
				TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperPlayerDlg::OnPlayerException(): sending WM_AUDIOPLAYER_REMOVEITEM\n"));
				::SendMessage(m_hWndParent,WM_AUDIOPLAYER_REMOVEITEM,0L,lParam);
			}

		// visualizza il messaggio d'errore
		//::MessageBoxResourceEx(this->m_hWnd,MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_AUDIOPLAYER_CANNOT_PLAY,szFileName);

		// manda in riproduzione il brano successivo
		if(m_bAutoSkip)
		{
			TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperPlayerDlg::OnPlayerException(): autoskip: posting WM_AUDIOPLAYER_PLAY\n"));
			PostMessage(WM_AUDIOPLAYER_PLAY);
		}
	}

	return(0L);
}

/*
	GetAudioInfo()

	Ricava le info relative al file in riproduzione.
*/
void CWallPaperPlayerDlg::GetAudioInfo(void)
{
	// controlla l'avvenuto caricamento
	if(!strnull(m_szAudioFileName))
	{
		// ricava il nome del file (da usare se mancano i tag)
		char* pAudioFileName = NULL;
		char* pAudioFileExt = NULL;
		pAudioFileName = (char*)::StripPathFromFile(m_szAudioFileName);
		if((pAudioFileExt = strrchr(pAudioFileName,'.'))!=NULL)
			_snprintf(m_AudioInfo.szAudioFileExt,sizeof(m_AudioInfo.szAudioFileExt)-1,"%s",pAudioFileExt);
		else
			memset(m_AudioInfo.szAudioFileExt,'\0',sizeof(m_AudioInfo.szAudioFileExt));
		strcpyn(m_AudioInfo.szAudioFileName,pAudioFileName,sizeof(m_AudioInfo.szAudioFileName));
		pAudioFileName = stristr(m_AudioInfo.szAudioFileName,MP3_EXTENSION);
		if(pAudioFileName)
			*pAudioFileName = '\0';
		else
		{
			pAudioFileName = stristr(m_AudioInfo.szAudioFileName,WAV_EXTENSION);
			if(pAudioFileName)
				*pAudioFileName = '\0';
			else
			{
				pAudioFileName = stristr(m_AudioInfo.szAudioFileName,CDA_EXTENSION);
				if(pAudioFileName)
					*pAudioFileName = '\0';
			}
		}

		// ricava le info sul file (tags)
		strcpyn(m_AudioInfo.szTitle,m_AudioPlayer.GetTitle(),sizeof(m_AudioInfo.szTitle));
		strcpyn(m_AudioInfo.szArtist,m_AudioPlayer.GetArtist(),sizeof(m_AudioInfo.szArtist));
		strcpyn(m_AudioInfo.szAlbum,m_AudioPlayer.GetAlbum(),sizeof(m_AudioInfo.szAlbum));
		strcpyn(m_AudioInfo.szGenre,m_AudioPlayer.GetGenre(),sizeof(m_AudioInfo.szGenre));
		_snprintf(m_AudioInfo.szYear,sizeof(m_AudioInfo.szYear)-1,"%s",m_AudioPlayer.GetYear());
		_snprintf(m_AudioInfo.szTrack,sizeof(m_AudioInfo.szTrack)-1,"%d",m_AudioPlayer.GetTrack());
		m_AudioInfo.qwSize = m_AudioPlayer.GetLength(m_AudioInfo.lMinutes,m_AudioInfo.lSeconds);
		_snprintf(m_AudioInfo.szDuration,sizeof(m_AudioInfo.szDuration)-1,"%02ld:%02ld",m_AudioInfo.lMinutes,m_AudioInfo.lSeconds);
		mmAudioFormat fmt = m_AudioPlayer.GetFormat();
		_snprintf(m_AudioInfo.szBitrate,sizeof(m_AudioInfo.szBitrate)-1,"%d %s",m_AudioPlayer.GetBitRate(),(fmt==mmAudioAfWav ? "bit" : (fmt==mmAudioAfCda ? "" : (fmt==mmAudioAfMp3 ? "kbps" : ""))));
		_snprintf(m_AudioInfo.szFrequency,sizeof(m_AudioInfo.szFrequency)-1,"%ld khz",m_AudioPlayer.GetFrequency());
		strcpyn(m_AudioInfo.szChannelMode,m_AudioPlayer.GetChannelMode(),sizeof(m_AudioInfo.szChannelMode));
		strcpyn(m_AudioInfo.szFormatType,m_AudioPlayer.GetFormatType(),sizeof(m_AudioInfo.szFormatType));
		strcpyn(m_AudioInfo.szComment,m_AudioPlayer.GetComment(),sizeof(m_AudioInfo.szComment));
	}
}

/*
	ShowTaskbarPopup()

	Visualizza il popup audio.
*/
void CWallPaperPlayerDlg::ShowTaskbarPopup(void)
{
	// controlla l'avvenuto caricamento
	if(!strnull(m_szAudioFileName) AND m_AudioPlayer.GetStatus()==mmAudioPmPlaying)
	{
		// controlla se in modalita' popup
		if(m_nTaskbarPopupMode==AUDIOPLAYER_MODE_TASKBAR_POPUP)
		{
			// crea l'oggetto se ancora non esiste
			if(!m_pTaskbarNotifier)
			{
				m_pTaskbarNotifier = new CTaskbarNotifier();
				if(m_pTaskbarNotifier)
				{
					if(m_pTaskbarNotifier->Create(this))
					{
						//m_pTaskbarNotifier->SetMessageWindow(this->GetSafeHwnd());
						m_pTaskbarNotifier->SetOnMouseClick(TRUE);
						m_pTaskbarNotifier->SetOnMouseClickMessage(WM_NOTIFYTASKBARPOPUP_AUDIO);
						m_pTaskbarNotifier->SetIDTimerAppearing(ID_TIMER_POPUP_APPEARING_AUDIO);
						m_pTaskbarNotifier->SetIDTimerDisappearing(ID_TIMER_POPUP_DISAPPEARING_AUDIO);
						m_pTaskbarNotifier->SetIDTimerWaiting(ID_TIMER_POPUP_WAITING_AUDIO);
						m_pTaskbarNotifier->SetIDTimerScrolling(ID_TIMER_POPUP_SCROLLING_AUDIO);
					}
					else
						delete m_pTaskbarNotifier,m_pTaskbarNotifier = NULL;
				}
			}

			// se dispone di un oggetto valido
			if(m_pTaskbarNotifier)
			{
				// ricava l'indice relativo al popup corrente della lista
				if(m_pListTaskbarBitmaps)
				{
					int nPopupCount = m_pListTaskbarBitmaps->Count();
					if(nPopupCount > 0)
					{
						TASKBARPOPUP* pAudioPopupInfoItem = (TASKBARPOPUP*)m_pListTaskbarBitmaps->GetAt(m_nPopupIndex++);
						if(pAudioPopupInfoItem)
							memcpy(&m_TaskbarPopup,pAudioPopupInfoItem,sizeof(TASKBARPOPUP));
						if(m_nPopupIndex >= nPopupCount)
							m_nPopupIndex = 0;
					}
				}

				BOOL bLoaded = FALSE;
				
				// elemento della lista per i popup valido
				if(!strnull(m_TaskbarPopup.szPicture))
				{
					// scarica l'eventuale bitmap corrente
					m_pTaskbarNotifier->UnShow();

					// (ri)carica il bitmap
					if(m_pTaskbarNotifier->SetBitmap(m_TaskbarPopup.szPicture,m_TaskbarPopup.R_Transparent,m_TaskbarPopup.G_Transparent,m_TaskbarPopup.B_Transparent))
					{
						// imposta il testo per tooltip
						int i = 0;
						int n;
						char szSkinInfo[512] = {0};
						
						n = 0;
						i = _snprintf(szSkinInfo+n,sizeof(szSkinInfo)-1-n,"Popup: <%s>",m_TaskbarPopup.szName),n = (i > 0 ? n+i : -1);
						if(n > 0 AND !strnull(m_TaskbarPopup.szAuthor))
							i = _snprintf(szSkinInfo+n,sizeof(szSkinInfo)-1-n," by <%s>",m_TaskbarPopup.szAuthor),n = (i > 0 ? n+i : -1);
						if(n > 0 AND !strnull(m_TaskbarPopup.szCopyright))
							i = _snprintf(szSkinInfo+n,sizeof(szSkinInfo)-1-n," (%s)",m_TaskbarPopup.szCopyright),n = (i > 0 ? n+i : -1);

						n = 0;
						if(!strnull(m_AudioInfo.szTitle))
							i = _snprintf(m_szTaskbarToolTipText+n,sizeof(m_szTaskbarToolTipText)-1-n,"Title: %s\n",m_AudioInfo.szTitle),n = (i > 0 ? n+i : -1);
						if(n > 0 AND !strnull(m_AudioInfo.szAlbum))
							i = _snprintf(m_szTaskbarToolTipText+n,sizeof(m_szTaskbarToolTipText)-1-n,"Album: %s\n",m_AudioInfo.szAlbum),n = (i > 0 ? n+i : -1);
						if(n > 0 AND !strnull(m_AudioInfo.szGenre))
							i = _snprintf(m_szTaskbarToolTipText+n,sizeof(m_szTaskbarToolTipText)-1-n,"Genre: %s\n",m_AudioInfo.szGenre),n = (i > 0 ? n+i : -1);
						if(n > 0 AND !strnull(m_AudioInfo.szArtist))
							i = _snprintf(m_szTaskbarToolTipText+n,sizeof(m_szTaskbarToolTipText)-1-n,"Artist: %s\n",m_AudioInfo.szArtist),n = (i > 0 ? n+i : -1);
						if(n > 0 AND !strnull(m_AudioInfo.szYear) AND atoi(m_AudioInfo.szYear) > 0)
							i = _snprintf(m_szTaskbarToolTipText+n,sizeof(m_szTaskbarToolTipText)-1-n,"Year: %s\n",m_AudioInfo.szYear),n = (i > 0 ? n+i : -1);
						if(n > 0 AND !strnull(m_AudioInfo.szTrack) AND atoi(m_AudioInfo.szTrack) > 0)
							i = _snprintf(m_szTaskbarToolTipText+n,sizeof(m_szTaskbarToolTipText)-1-n,"Track: %s\n",m_AudioInfo.szTrack),n = (i > 0 ? n+i : -1);
						if(n > 0)
							i = _snprintf(m_szTaskbarToolTipText+n,sizeof(m_szTaskbarToolTipText)-1-n,"Duration: %02d:%02d\n",m_AudioInfo.lMinutes,m_AudioInfo.lSeconds),n = (i > 0 ? n+i : -1);
						if(n > 0)
							i = _snprintf(m_szTaskbarToolTipText+n,sizeof(m_szTaskbarToolTipText)-1-n,"Bitrate: %s\n",m_AudioInfo.szBitrate),n = (i > 0 ? n+i : -1);
						if(n > 0)
							i = _snprintf(m_szTaskbarToolTipText+n,sizeof(m_szTaskbarToolTipText)-1-n,"Frequency: %s\n",m_AudioInfo.szFrequency),n = (i > 0 ? n+i : -1);
						if(n > 0)
							i = _snprintf(m_szTaskbarToolTipText+n,sizeof(m_szTaskbarToolTipText)-1-n,"Channel Mode: %s\n",m_AudioInfo.szChannelMode),n = (i > 0 ? n+i : -1);
						if(n > 0)
							i = _snprintf(m_szTaskbarToolTipText+n,sizeof(m_szTaskbarToolTipText)-1-n,"Format: %s\n",m_AudioInfo.szFormatType),n = (i > 0 ? n+i : -1);
						if(n > 0 AND !strnull(m_AudioInfo.szComment))
							i = _snprintf(m_szTaskbarToolTipText+n,sizeof(m_szTaskbarToolTipText)-1-n,"Comment: %s\n",m_AudioInfo.szComment),n = (i > 0 ? n+i : -1);
						if(n > 0)
							i = _snprintf(m_szTaskbarToolTipText+n,sizeof(m_szTaskbarToolTipText)-1-n,"%s",szSkinInfo);

						// sostituisce le macro (utente) all'interno del testo per il popup
						m_Tags[0].value = m_AudioInfo.szTitle;
						m_Tags[1].value = m_AudioInfo.szArtist;
						m_Tags[2].value = m_AudioInfo.szAlbum;
						m_Tags[3].value = m_AudioInfo.szGenre;
						m_Tags[4].value = m_AudioInfo.szYear;
						m_Tags[5].value = m_AudioInfo.szTrack;
						m_Tags[6].value = m_AudioInfo.szDuration;
						m_Tags[7].value = m_AudioInfo.szComment;
						m_Tags[8].value = m_AudioInfo.szBitrate;
						m_Tags[9].value = m_AudioInfo.szFrequency;
						m_Tags[10].value = m_AudioInfo.szChannelMode;
						m_Tags[11].value = m_AudioInfo.szFormatType;
						m_Tags[12].value = m_TaskbarPopup.nPopupScrollText ? " " : "\r\n";
						char szPopupText[512] = {0};
						for(i = 0; i < ARRAY_SIZE(m_Tags); i++)
						{
							// se il valore corrispondente alla macro e' nullo non sostituisce (visualizzando la macro)
							if(!strnull(m_Tags[i].value))
								while(substr(m_TaskbarPopup.szPopupText,m_Tags[i].tag,m_Tags[i].value,szPopupText,sizeof(szPopupText)))
								{
									strcpyn(m_TaskbarPopup.szPopupText,szPopupText,sizeof(m_TaskbarPopup.szPopupText)-1);
									memset(szPopupText,'\0',sizeof(szPopupText));
								}
						}

						// imposta il resto dei parametri per la finestra popup
						m_pTaskbarNotifier->SetRect(CRect(m_TaskbarPopup.nTextAreaLeft,m_TaskbarPopup.nTextAreaTop,m_TaskbarPopup.nTextAreaRight,m_TaskbarPopup.nTextAreaBottom));
						m_pTaskbarNotifier->SetFont(m_TaskbarPopup.szFontName,m_TaskbarPopup.nFontSize,m_TaskbarPopup.nFontStyle,TN_TEXT_NORMAL/*TN_TEXT_UNDERLINE*/);
						char szFiller[256];
						_snprintf(szFiller,sizeof(szFiller)-1," ~ %s ~ ",WALLPAPER_WEB_SITE);
						m_pTaskbarNotifier->SetTextFiller(szFiller);
 						m_pTaskbarNotifier->SetTextColor(RGB(m_TaskbarPopup.R_Text,m_TaskbarPopup.G_Text,m_TaskbarPopup.B_Text),/*RGB(0,0,200)*/RGB(m_TaskbarPopup.R_SelectedText,m_TaskbarPopup.G_SelectedText,m_TaskbarPopup.B_SelectedText));
						m_pTaskbarNotifier->SetTextFormat(m_TaskbarPopup.dwFontAlign);
						m_pTaskbarNotifier->SetTextScroll(m_TaskbarPopup.nPopupScrollText,m_TaskbarPopup.nPopupScrollDirection,m_TaskbarPopup.nPopupScrollSpeed);
						if(m_TaskbarPopup.nPopupTooltip)
							m_pTaskbarNotifier->SetToolTip(m_szTaskbarToolTipText);
						else
							m_pTaskbarNotifier->SetToolTip();
						m_pTaskbarNotifier->SetAlwaysOnTop(m_TaskbarPopup.nPopupAlwaysOnTop);
						m_pTaskbarNotifier->SetLayer(m_TaskbarPopup.nPopupTransparency);
						if(m_TaskbarPopup.dwPopupTimeToStay==(DWORD)-1L)
							;
						else if(m_TaskbarPopup.dwPopupTimeToStay==0L OR m_TaskbarPopup.dwPopupTimeToStay < TASKBARPOPUP_MIN_POPUPTIME OR m_TaskbarPopup.dwPopupTimeToStay > TASKBARPOPUP_MAX_POPUPTIME)
							m_TaskbarPopup.dwPopupTimeToStay = (DWORD)((((m_AudioInfo.lMinutes*60)+m_AudioInfo.lSeconds)*1000) - (m_TaskbarPopup.nPopupTimeToShow + m_TaskbarPopup.nPopupTimeToHide + 100L));
						
						// visualizza la finestra popup
						bLoaded = m_pTaskbarNotifier->Show(m_TaskbarPopup.szPopupText,m_TaskbarPopup.nPopupTimeToShow,m_TaskbarPopup.dwPopupTimeToStay,m_TaskbarPopup.nPopupTimeToHide,m_TaskbarPopup.nPopupXPos,m_TaskbarPopup.nPopupYPos);
					}
					else
						::MessageBoxResourceEx(this->m_hWnd,MB_OK|MB_ICONERROR,IDS_DIALOG_PLAYER_TITLE,IDS_ERROR_INVALID_SKIN_FILENAME,m_TaskbarPopup .szPicture);
				}
			}
		}
	}
}

/*
	ShowBalloonPopup()

	Visualizza il balloon tooltip audio.
*/
void CWallPaperPlayerDlg::ShowBalloonPopup(BOOL bForceBalloon/* = FALSE*/)
{
	// controlla l'avvenuto caricamento
	if(!strnull(m_szAudioFileName) AND m_AudioPlayer.GetStatus()==mmAudioPmPlaying)
	{
		// controlla se in modalita' popup
		if(m_nTaskbarPopupMode==AUDIOPLAYER_MODE_BALLOON_POPUP || bForceBalloon)
		{
			// controlla se l'oggetto esiste
			if(m_pTrayIcon)
			{
				int i = 0;
				int n = 0;
				char szTitle[_MAX_FILEPATH+1] = {0};
				char szText[BALLOON_MAX_TOOLTIP_TEXT+1] = {0};

				// titolo
				i = n = 0;
				if(!strnull(m_AudioInfo.szTitle))
					i = _snprintf(szTitle+n,sizeof(szTitle)-1-n,"%s",m_AudioInfo.szTitle),n = (i > 0 ? n+i : -1);
				// artista
				if(n > 0 AND !strnull(m_AudioInfo.szArtist))
					i = _snprintf(szTitle+n,sizeof(szTitle)-1-n,"%s%s",strnull(m_AudioInfo.szTitle) ? "" : " - ",m_AudioInfo.szArtist),n = (i > 0 ? n+i : -1);
				// se non ci sono tag usa il nomefile
				if(n <= 0)
				{
					strcpyn(szTitle,::StripPathFromFile(m_szAudioFileName),sizeof(szTitle));
					if(striright(szTitle,MP3_EXTENSION)==0)
						szTitle[strlen(szTitle)-strlen(MP3_EXTENSION)] = '\0';
					else if(striright(szTitle,WAV_EXTENSION)==0)
						szTitle[strlen(szTitle)-strlen(WAV_EXTENSION)] = '\0';
					else if(striright(szTitle,CDA_EXTENSION)==0)
						szTitle[strlen(szTitle)-strlen(CDA_EXTENSION)] = '\0';
				}
				
				// testo tooltip
				n = 0;
				if(!strnull(m_AudioInfo.szAlbum) AND !strempty(m_AudioInfo.szAlbum))
					i = _snprintf(szText+n,sizeof(szText)-1-n,"Album: %s\n",m_AudioInfo.szAlbum),n = (i > 0 ? n+i : -1);
				if(n >= 0 AND !strnull(m_AudioInfo.szGenre) AND !strempty(m_AudioInfo.szGenre))
					i = _snprintf(szText+n,sizeof(szText)-1-n,"Genre: %s\n",m_AudioInfo.szGenre),n = (i > 0 ? n+i : -1);
				if(n >= 0 AND !strnull(m_AudioInfo.szYear) AND atoi(m_AudioInfo.szYear) > 0)
					i = _snprintf(szText+n,sizeof(szText)-1-n,"Year: %s\n",m_AudioInfo.szYear),n = (i > 0 ? n+i : -1);
				if(n >= 0 AND !strnull(m_AudioInfo.szTrack) AND atoi(m_AudioInfo.szTrack) > 0)
					i = _snprintf(szText+n,sizeof(szText)-1-n,"Track: %s\n",m_AudioInfo.szTrack),n = (i > 0 ? n+i : -1);
				if(n >= 0)
					i = _snprintf(szText+n,sizeof(szText)-1-n,"Duration: %02d:%02d\n",m_AudioInfo.lMinutes,m_AudioInfo.lSeconds),n = (i > 0 ? n+i : -1);
				if(n >= 0)
					if(m_AudioInfo.qwSize > 0L)
						i = _snprintf(szText+n,sizeof(szText)-1-n,"Size: %s\n",strsize((double)m_AudioInfo.qwSize)),n = (i > 0 ? n+i : -1);
				if(n >= 0)
					i = _snprintf(szText+n,sizeof(szText)-1-n,"Bitrate: %s\n",m_AudioInfo.szBitrate),n = (i > 0 ? n+i : -1);
				if(n >= 0)
					i = _snprintf(szText+n,sizeof(szText)-1-n,"Frequency: %s\n",m_AudioInfo.szFrequency),n = (i > 0 ? n+i : -1);
				if(n >= 0)
					i = _snprintf(szText+n,sizeof(szText)-1-n,"Channel Mode: %s\n",m_AudioInfo.szChannelMode),n = (i > 0 ? n+i : -1);
				if(n >= 0)
					i = _snprintf(szText+n,sizeof(szText)-1-n,"Format: %s",m_AudioInfo.szFormatType),n = (i > 0 ? n+i : -1);
				if(n >= 0 AND !strnull(m_AudioInfo.szComment) AND !strempty(m_AudioInfo.szComment))
					i = _snprintf(szText+n,sizeof(szText)-1-n,"\nComment: %s",m_AudioInfo.szComment),n = (i > 0 ? n+i : -1);
				if(n >= 0)
					i = _snprintf(szText+n,sizeof(szText)-1-n,"%s","\n");

				// plin!
				BOOL bUseExtended = FALSE;
				if(m_nAudioIconsCount > 0)
				{
					if(++m_nCurrentAudioIcon >= m_nAudioIconsCount)
						m_nCurrentAudioIcon = 0;
					
					if(::FileExist(m_pFindAudioIcons->GetFileName(m_nCurrentAudioIcon)))
						bUseExtended = TRUE;
				}
				
				if(bUseExtended)
				{
					int nBiggerIcon = 32;
					FINDFILE* f = m_pFindAudioIcons->GetFindFile(m_nCurrentAudioIcon);
					if(f->wParam <= 0)
						f->wParam = ::GetBiggerIconSize(m_pFindAudioIcons->GetFileName(m_nCurrentAudioIcon),48);
					nBiggerIcon = f->wParam;

					m_pTrayIcon->SetBalloonType(BALLOON_USE_EXTENDED);
					m_pTrayIcon->Balloon(	szTitle,
										szText,
										MB_ICONINFORMATION,
										BALLOON_DEFAULT_TIMEOUT,
										(UINT)-1L,
										m_pFindAudioIcons->GetFileName(m_nCurrentAudioIcon),
										CSize(nBiggerIcon,nBiggerIcon),
										IDI_ICON16X16_CLOSE
										);
				}
				else
				{
					m_pTrayIcon->SetBalloonType(BALLOON_USE_EXTENDED);
					m_pTrayIcon->Balloon(	szTitle,
										szText,
										MB_ICONINFORMATION,
										BALLOON_DEFAULT_TIMEOUT,
										IDI_ICON_PLAYER,
										NULL,
										CSize(32,32),
										IDI_ICON16X16_CLOSE
										);
				}
			}
		}
	}
}

/*
	SetTrayIcon()
	
	Imposta l'icona ed il tooltip per l'icona nella system tray.
*/
void CWallPaperPlayerDlg::SetTrayIcon(LPCSTR lpcszTooltipText/* = NULL*/,UINT nIconID/* = IDI_ICON_PLAYER*/)
{
	if(m_pTrayIcon)
	{
		// imposta l'icona
		m_pTrayIcon->SetIcon(nIconID);

		// imposta il testo con i dati relativi al brano corrente
		if(!lpcszTooltipText)
		{
			int i = 0;
			
			// titolo
			if(!strnull(m_AudioInfo.szTitle))
				i = _snprintf(m_szTrayIconTooltipText,sizeof(m_szTrayIconTooltipText)-1,"%s",m_AudioInfo.szTitle);
			
			// artista
			if(i > 0 AND !strnull(m_AudioInfo.szArtist))
				i += _snprintf(m_szTrayIconTooltipText+i,sizeof(m_szTrayIconTooltipText)-1-i," - %s",m_AudioInfo.szArtist);

			// se non ci sono tag usa il nomefile
			if(i <= 0)
			{
				strcpyn(m_szTrayIconTooltipText,m_AudioInfo.szAudioFileName,sizeof(m_szTrayIconTooltipText));
				i = strlen(m_szTrayIconTooltipText);
			}
			
			// se il nome file sfora, lo accorcia con [...] (8 per la durata)
			if(i >= TRAYICON_MAX_TOOLTIP_TEXT-8)
			{
				CFilenameFactory fn;
				strcpyn(m_szTrayIconTooltipText,fn.Abbreviate(m_szTrayIconTooltipText,TRAYICON_MAX_TOOLTIP_TEXT-8-1),sizeof(m_szTrayIconTooltipText));
				i = strlen(m_szTrayIconTooltipText);
			}
			
			// durata
			_snprintf(m_szTrayIconTooltipText+i,sizeof(m_szTrayIconTooltipText)-1-i," (%02ld:%02ld)",m_AudioInfo.lMinutes,m_AudioInfo.lSeconds);
		}
		else // imposta il testo con quanto ricevuto
			strcpyn(m_szTrayIconTooltipText,lpcszTooltipText,sizeof(m_szTrayIconTooltipText));
		
		// imposta il tooltip
		m_pTrayIcon->SetToolTip(m_szTrayIconTooltipText);
	}
}
