/*
	WallPaperDlg.h
	Dialogo per l'applicazione principale.
	Luca Piergentili, 06/08/98
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
#ifndef _WALLPAPERDLG_H
#define _WALLPAPERDLG_H 1

#include "window.h"
#include "CNodeList.h"
#include "CRand.h"
#include "CDialogEx.h"
#include "CDibCtrl.h"
#include "CColorStatic.h"
#include "CWndLayered.h"
#include "CProgressCtrlEx.h"
#include "CImageFactory.h"
#include "CListCtrlEx.h"
#include "CSync.h"
#include "COleDropTargetEx.h"
#include "CStaticFilespec.h"
#include "CTrayIcon.h"
#include "CBalloon.h"
#include "CHttp.h"
#include "CUrl.h"
#include "CArchive.h"
#include "WallPaperConfig.h"
#include "WallPaperCmdLine.h"
#include "WallPaperIconIndex.h"
#include "WallPaperThreadList.h"
#include "WallPaperPlayerDlg.h"
#include "WallPaperMessages.h"

// indici per le colonne del controllo
#define CTRLLISTEX_FILENAME_INDEX				0
#define CTRLLISTEX_INFO_INDEX					1
#define CTRLLISTEX_SIZE_INDEX					2
#define CTRLLISTEX_LOCATION_INDEX				3
#define CTRLLISTEX_TYPE_INDEX					4

// titolo a scorrimento
#define WALLPAPER_TITLE_CHARS_IN_TASKBAR_ICON	256
#define WALLPAPER_MAX_TITLE_CHARS				2048
#define WALLPAPER_DEFAULT_TITLE_STRING_PRG		" - "WALLPAPER_PROGRAM_TITLE" ~ "
#define WALLPAPER_DEFAULT_TITLE_STRING_WEB		" - "WALLPAPER_WEB_SITE" ~ The free media crawler - "
#define WALLPAPER_DEFAULT_TITLE_STRING_TAG		" - "WALLPAPER_PROGRAM_NAME_ALIAS" - "WALLPAPER_PROGRAM_TAGLINE" ~ "
#define WALLPAPER_DEFAULT_INFAMOUS_STRING		" - (In)Famous Sentences ~ "
#define WALLPAPER_INFAMOUS_FILE				"infamous.txt"

// timeout per il mutex
#define WALLPAPER_DESKTOP_MUTEX_TIMEOUT			SYNC_1_SECS_TIMEOUT

// passaggio all'immagine precedente/successiva
#define WALLPAPER_TIMER_CHANGE_PICTURE			-6
#define WALLPAPER_GOTO_FIRST_PICTURE			-5
#define WALLPAPER_GOTO_LAST_PICTURE			-4
#define WALLPAPER_GOTO_PREVIOUS_PICTURE			-3
#define WALLPAPER_GOTO_NEXT_PICTURE			-2

// valori min e max per il cambio del desktop (in minuti)
#define WALLPAPER_MIN_VALUE_FOR_CHANGE_TIMEOUT	1
#define WALLPAPER_MAX_VALUE_FOR_CHANGE_TIMEOUT	1440

// status (attivo/in pausa)
#define WALLPAPER_IN_ACTIVE_STATE				1
#define WALLPAPER_IN_PAUSED_STATE				0

// codici id per gli hotkeys
#define HOTKEY_CTRL_ALT_MULTIPLY				1965	// do a random reorder of the current playlist
#define HOTKEY_CTRL_ALT_DIVIDE				1966	// remove the current wallpaper from the desktop
#define HOTKEY_CTRL_ALT_UP					1967	// go to the previous item of the playlist
#define HOTKEY_CTRL_ALT_DOWN					1968	// go to the next item of the playlist
#define HOTKEY_CTRL_ALT_LEFT					1969	// play the previous song
#define HOTKEY_CTRL_ALT_RIGHT					1970	// play the next song
#define HOTKEY_CTRL_ALT_BACKSPACE				1971	// delete the current song
#define HOTKEY_CTRL_ALT_SPACE					1972	// toggle playing
#define HOTKEY_CTRL_ALT_ADD					1973	// increase the volume
#define HOTKEY_CTRL_ALT_SUBTRACT				1974	// decrease the volume

// id per gli elementi del menu di sistema (gli id sono a base 0)
// per le operazioni di sistema non usare gli IDM_ ma le costanti SC_
// per gli id definiti dall'utente (quelli che si inseriscono nel menu di sistema) usare multipli di 16
#define IDM_SYSMENU_EXIT						160
#define IDM_SYSMENU_ABOUT					144
//------------------------------------------------
//#define IDM_SYSMENU_CLOSE					128	// SC_CLOSE
//#define IDM_SYSMENU_MIN					112	// SC_MINIMIZE
//#define IDM_SYSMENU_MAX					96	// SC_RESTORE
//------------------------------------------------
#define IDM_SYSMENU_PAUSE					80
#define IDM_SYSMENU_RESUME					64
#define IDM_SYSMENU_FAVOURITE					48
//------------------------------------------------
#define IDM_SYSMENU_PREVPIC					32
#define IDM_SYSMENU_NEXTPIC					16

// posizioni assolute nel menu di sistema per le entrate relative (a base 0)
#define IDM_SYSMENU_PAUSERESUME_POSITION		12
#define IDM_SYSMENU_FAVOURITE_POSITION			13
#define IDM_SYSMENU_NEXTPICT_POSITION			14
#define IDM_SYSMENU_PREVPICT_POSITION			15

// posizioni assolute nel menu della tray icon per le entrate relative
#define IDM_TRAYMENU_SEPARATOR_POSITION			7

/*
	CLIPBOARDFORMATS
	formati clipboard riconosciuti
*/
enum CLIPBOARDFORMATS {
	CLIPBOARD_UNAVAILABLE,
	CLIPBOARD_PICTURE_FORMAT,
	CLIPBOARD_FILEDROP_FORMAT,
	CLIPBOARD_URL_NETSCAPE_FORMAT,
	CLIPBOARD_URL_IEXPLORE_FORMAT
};

/*
	PICTUREINFO
	struttura per le info sul file
*/
struct PICTUREINFO {
	int		nWidth;
	int		nHeight;
	char		szWidthxHeight[16];
	int		nColors;
	char		szColors[16];
	int		nBPP;
	char		szBPP[8];
	char		szMemUsed[16];
	DWORD	dwMemUsed;
	DWORD	dwFileSize;
	char		szFileSize[16];
	float	nXRes;
	float	nYRes;
	int		nResType;
	int		nQuality;
	char		szQuality[8];
	char		szDrawmode[32];
	char		szEffect[32];
};

/*
	CWallPaperDlg
*/
class CWallPaperDlg : public CDialogEx
{
	DECLARE_DYNCREATE(CWallPaperDlg)

protected: // provide default constructor only for dynamic creation, private/protected to prevent it from being called from outside the class implementation
	CWallPaperDlg() {::MessageBox(NULL,"PANIC! This shouldn't happen!","CWallPaperDlg()",MB_OK|MB_ICONERROR|MB_TASKMODAL|MB_SETFOREGROUND|MB_TOPMOST);}

public:
	CWallPaperDlg(HWND hWndParent,LPCSTR lpcszCmd = NULL);	
	~CWallPaperDlg();

	// dialogo
	void			DoDataExchange						(CDataExchange* pDX);
	BOOL			OnInitDialog						(void);
	BOOL			OnInitDialogEx						(UINT nInstanceCount = (UINT)-1,LPCSTR lpcszCommandLine = NULL);
	BOOL			ApplySettings						(BOOL bCalledFromOnInitDialog);
	BOOL			InitializeApplication				(void);
	BOOL			RegisterApplication					(void);

	// tastiera e mouse
	BOOL			OnReturnKey						(void);
	inline BOOL	OnEscapeKey						(void) {OnMenuFileMinimize(); return(TRUE);}
	BOOL			OnFunctionKey						(WPARAM wFunctionKey,BOOL bShift, BOOL bCtrl);
	BOOL			OnAltFunctionKey					(WPARAM wFunctionKey);
	BOOL			OnVirtualKey						(WPARAM wVirtualKey,BOOL bShift,BOOL bCtrl,BOOL bAlt);
	LRESULT		OnHotKey							(WPARAM wParam, LPARAM lParam);
	void			HotKeyHandler						(UINT nMsg,WPARAM wParam,LPARAM lParam,LPCSTR lpcszThreadName = NULL);
	
	// icona per la tray area
	LRESULT		OnNotifyTrayIcon					(WPARAM wParam,LPARAM lParam);
	LRESULT		OnNotifyTaskbarPopup				(WPARAM /*wParam*/,LPARAM lParam);

	// menu (sistema/dialogo)
	void			OnSysCommand						(UINT nID,LPARAM lParam);
	void			OnInitMenuPopup					(CMenu* pMenu,UINT nIndex,BOOL bSysMenu);

	//
	// File (menu)
	//
	inline void	OnMenuFileMinimize					(void);						// IDM_FILE_MINIMIZE
	inline void	OnMenuFileMaximize					(void);						// IDM_FILE_MAXIMIZE
	inline void	OnMenuFileOpen						(void)	{OnOpen();}			// IDM_FILE_OPEN
	inline void	OnMenuFileSaveAs					(void)	{OnSaveAs();}			// IDM_FILE_SAVEAS
	inline void	OnMenuFileDeleteFile				(void)	{OnDeleteFile();}		// IDM_FILE_DELETE
	inline void	OnMenuFileExit						(void)	{OnExit();}			// IDM_FILE_EXIT
	//
	// Edit (menu)
	//
	inline void	OnMenuEditCopy						(void)	{OnCopy();}			// IDM_EDIT_COPY
	inline void	OnMenuEditPaste					(void)	{OnPaste();}			// IDM_EDIT_PASTE
	inline void	OnMenuEditEmptyClipboard				(void)	{OnEmptyClipboard();}	// IDM_EDIT_EMPTYCLIPBOARD
	//
	// Playlist (menu)
	//
	inline void	OnMenuPlaylistSavePlaylist			(void)	{PlaylistSave();}		// IDM_PLAYLIST_SAVEPLAYLIST
	inline void	OnMenuPlaylistPrevious				(void)	{OnPrevious();}		// IDM_PLAYLIST_PREVIOUS
	inline void	OnMenuPlaylistNext					(void)	{OnNext();}			// IDM_PLAYLIST_NEXT
	inline void	OnMenuPlaylistPause					(void)	{OnPause();}			// IDM_PLAYLIST_PAUSE
	inline void	OnMenuPlaylistResume				(void)	{OnResume();}			// IDM_PLAYLIST_RESUME
	inline void	OnMenuPlaylistAddFile				(void)	{OnAddFile();}			// IDM_PLAYLIST_ADDFILE
	inline void	OnMenuPlaylistAddDir				(void)	{OnAddDir();}			// IDM_PLAYLIST_ADDDIR
	inline void	OnMenuPlaylistAddUrl				(void)	{OnAddUrl();}			// IDM_PLAYLIST_ADDURL
	inline void	OnMenuPlaylistRemove				(void)	{OnRemove();}			// IDM_PLAYLIST_REMOVE
	inline void	OnMenuPlaylistRemoveAll				(void)	{OnRemoveAll();}		// IDM_PLAYLIST_REMOVEALL
	inline void	OnMenuPlaylistUnloadList				(void)	{OnUnloadPlaylistFile();}// IDM_PLAYLIST_UNLOADLIST
	inline void	OnMenuPlaylistNewList				(void)	{OnNewPlaylistFile();}	// IDM_PLAYLIST_NEWLIST
	inline void	OnMenuPlaylistLoadList				(void)	{OnLoadPlaylistFile();}	// IDM_PLAYLIST_LOADLIST
	inline void	OnMenuPlaylistSaveList				(void)	{OnSavePlaylistFile();}	// IDM_PLAYLIST_SAVELIST
	inline void	OnMenuPlaylistFavouritePicture		(void)	{OnFavouritePicture();}	// IDM_PLAYLIST_FAVOURITE_PICTURE
	inline void	OnMenuPlaylistFavouriteAudio			(void)	{OnFavouriteAudio();}	// IDM_PLAYLIST_FAVOURITE_AUDIO
	inline void	OnMenuPlaylistLoadFavouritePicture		(void)	{OnLoadFavouritePicture();}// IDM_PLAYLIST_LOAD_FAVOURITE_PICTURE
	inline void	OnMenuPlaylistLoadFavouriteAudio		(void)	{OnLoadFavouriteAudio();}// IDM_PLAYLIST_LOAD_FAVOURITE_AUDIO
	//
	// Tools (menu)
	//
	inline void	OnMenuToolsUndesktop				(void)	{OnUndesktop();}		// IDM_TOOLS_UNDESKTOP
	inline void	OnMenuToolsDesktopIcons				(void)	{OnDesktopIcons();}		// IDM_TOOLS_DESKTOPICONS
	inline void	OnMenuToolsDisplayProperties			(void)	{OnDisplayProperties();}	// IDM_TOOLS_DISPLAYPROPERTIES
	inline void	OnMenuToolsWallBrowser				(void)	{OnBrowser();}			// IDM_TOOLS_WALLBROWSER
	inline void	OnMenuToolsThumbnails				(void)	{OnThumbnails();}		// IDM_TOOLS_THUMBNAILS
	inline void	OnMenuToolsCrawler					(void)	{OnCrawler();}			// IDM_TOOLS_CRAWLER
	inline void	OnMenuToolsUnCompress				(void)	{OnUnCompress();}		// IDM_TOOLS_UNCOMPRESS
	//
	// Options (menu)
	//
	void			OnMenuOptionsAlwaysOnTop				(void);						// IDM_OPTIONS_ALWAYSONTOP
	void			OnMenuOptionsAutoRun				(void);						// IDM_OPTIONS_AUTORUN
	void			OnMenuOptionsSplashScreen			(void);						// IDM_OPTIONS_SPLASHSCREEN
	void			OnMenuOptionsTrayIcon				(void);						// IDM_OPTIONS_TRAYICON
	void			OnMenuOptionsMinimizeOnClose			(void);						// IDM_OPTIONS_MINIMIZE_ON_CLOSE
	void			OnMenuOptionsAudioNoPopup			(void);						// IDM_OPTIONS_AUDIO_NO_POPUP
	void			OnMenuOptionsAudioBalloonPopup		(void);						// IDM_OPTIONS_AUDIO_BALLOON_POPUP
	void			OnMenuOptionsAudioReloadBalloon		(void);						// IDM_OPTIONS_AUDIO_RELOAD_BALLOON
	void			OnMenuOptionsAudioBalloonDefaultIcon	(void);						// IDM_OPTIONS_AUDIO_BALLOON_DEFAULTICON
	void			OnMenuOptionsAudioBalloonCustomIcon	(void);						// IDM_OPTIONS_AUDIO_BALLOON_CUSTOMICON
	void			OnMenuOptionsAudioTaskbarPopup		(void);						// IDM_OPTIONS_AUDIO_TASKBAR_POPUP
	void			OnMenuOptionsAudioReloadTaskbarBitmapList(void);						// IDM_OPTIONS_AUDIO_RELOAD_TASKBAR_POPUP
	void			OnMenuOptionsPictNoPopup				(void);						// IDM_OPTIONS_PICT_NO_POPUP
	void			OnMenuOptionsPictBalloonPopup			(void);						// IDM_OPTIONS_PICT_BALLOON_POPUP
	inline void	OnMenuOptionsPictReloadBalloon		(void)	{OnBalloon();}			// IDM_OPTIONS_PICT_RELOAD_BALLOON
	void			OnMenuOptionsPictBalloonDefaultIcon	(void);						// IDM_OPTIONS_PICT_BALLOON_DEFAULTICON
	void			OnMenuOptionsPictBalloonCustomIcon		(void);						// IDM_OPTIONS_PICT_BALLOON_CUSTOMICON
	void			OnMenuOptionsPictTaskbarPopup			(void);						// IDM_OPTIONS_PICT_TASKBAR_POPUP
	void			OnMenuOptionsPictReloadTaskbarBitmapList(void);						// IDM_OPTIONS_PICT_RELOAD_TASKBAR_POPUP
	void			OnMenuOptionsGridlines				(void);						// IDM_OPTIONS_GRIDLINES
	void			OnMenuOptionsGridlinesFgColor			(void);						// IDM_OPTIONS_GRIDLINES_FOREGROUND_COLOR
	void			OnMenuOptionsGridlinesBgColor			(void);						// IDM_OPTIONS_GRIDLINES_BACKGROUND_COLOR
	void			OnMenuOptionsGridReset				(void);						// IDM_OPTIONS_GRID_RESET
	void			OnMenuOptionsClearDoNotAskMore		(void);						// IDM_OPTIONS_CLEAR_DONOTASKMORE
	void			OnMenuOptionsOneClickChange			(void);						// IDM_OPTIONS_ONE_CLICK_CHANGE
	void			OnMenuOptionsNervousTitle			(void);						// IDM_OPTIONS_NERVOUS_TITLE
	void			OnMenuOptionsInfamousSentences		(void);						// IDM_OPTIONS_INFAMOUS_SENTENCES
	void			OnMenuOptionsContextMenu				(void);						// IDM_OPTIONS_CONTEXT_MENU
	void			OnMenuOptionsPlaylistLoadingAdd		(void);						// IDM_OPTIONS_PLAYLIST_LOADING_ADD
	void			OnMenuOptionsPlaylistLoadingPut		(void);						// IDM_OPTIONS_PLAYLIST_LOADING_PUT
	void			OnMenuOptionsRandomizePlaylistSelection	(void);						// IDM_OPTIONS_RANDOMIZE_PLAYLIST_SELECTION
	void			OnMenuOptionsRandomizePlaylistReorder	(void);						// IDM_OPTIONS_RANDOMIZE_PLAYLIST_REORDER
	void			OnMenuOptionsRandomizePlaylistNone		(void);						// IDM_OPTIONS_RANDOMIZE_PLAYLIST_NONE
	void			OnMenuOptionsRecurseSubdir			(void);						// IDM_OPTIONS_RECURSE_SUBDIR
	void			OnMenuOptionsConfirmFileDelete		(void);						// IDM_OPTIONS_CONFIRM_FILE_DELETE
	void			OnMenuOptionsDeleteFilesToRecycleBin	(void);						// IDM_OPTIONS_DELETE_FILES_TO_RECYCLEBIN
	void			OnMenuOptionsTempFilesClearAtExit		(void);						// IDM_OPTIONS_TEMPORARY_FILES_CLEAR_AT_EXIT
	void			OnMenuOptionsTempFilesClearAtRuntime	(void);						// IDM_OPTIONS_TEMPORARY_FILES_CLEAR_AT_RUNTIME
	void			OnMenuOptionsTempFilesDoNotClear		(void);						// IDM_OPTIONS_TEMPORARY_FILES_DO_NOT_CLEAR
	void			OnMenuOptionsProcessPriorityRealTime	(void);						// IDM_OPTIONS_PRIORITY_PROCESS_REALTIME
	void			OnMenuOptionsProcessPriorityHigh		(void);						// IDM_OPTIONS_PRIORITY_PROCESS_HIGH
	void			OnMenuOptionsProcessPriorityNormal		(void);						// IDM_OPTIONS_PRIORITY_PROCESS_NORMAL
	void			OnMenuOptionsProcessPriorityIdle		(void);						// IDM_OPTIONS_PRIORITY_PROCESS_IDLE
	void			OnMenuOptionsThreadPriorityTimeCritical	(void);						// IDM_OPTIONS_PRIORITY_THREAD_TIME_CRITICAL
	void			OnMenuOptionsThreadPriorityHighest		(void);						// IDM_OPTIONS_PRIORITY_THREAD_HIGHEST
	void			OnMenuOptionsThreadPriorityAboveNormal	(void);						// IDM_OPTIONS_PRIORITY_THREAD_ABOVE_NORMAL
	void			OnMenuOptionsThreadPriorityNormal		(void);						// IDM_OPTIONS_PRIORITY_THREAD_NORMAL
	void			OnMenuOptionsThreadPriorityBelowNormal	(void);						// IDM_OPTIONS_PRIORITY_THREAD_BELOW_NORMAL
	void			OnMenuOptionsThreadPriorityLowest		(void);						// IDM_OPTIONS_PRIORITY_THREAD_LOWEST
	void			OnMenuOptionsThreadPriorityIdle		(void);						// IDM_OPTIONS_PRIORITY_THREAD_IDLE
	void			OnMenuOptionsDrawSettings			(void);						// IDM_OPTIONS_DRAW_SETTINGS
	void			OnMenuOptionsThumbnailsSettings		(void);						// IDM_OPTIONS_THUMBNAILS_SETTINGS
	void			OnMenuOptionsCrawlerSettings			(void);						// IDM_OPTIONS_CRAWLER_SETTINGS
	void			OnMenuOptionsSaveConfiguration		(void);						// IDM_OPTIONS_SAVE_CONFIG
	void			OnMenuOptionsLoadConfiguration		(void);						// IDM_OPTIONS_LOAD_CONFIG
	//
	// Help (menu)
	//
	inline void	OnMenuHelpHotKeys					(void)	{OnHotKeys();}			// IDM_HELP_HOTKEYS
	inline void	OnMenuHelpHomePage					(void)	{OnHomePage();}		// IDM_HELP_HOMEPAGE
	inline void	OnMenuHelpLatestVersion				(void)	{OnLatestVersion(FALSE,0L);}// IDM_HELP_LATESTVERSION
	inline void	OnMenuHelpLatestVersionAtStartup		(void);						// IDM_HELP_LATESTVERSION_AT_STARTUP
	inline void	OnMenuHelpLatestVersionEveryNDays		(void);						// IDM_HELP_LATESTVERSION_EVERY_N_DAYS
	inline void	OnMenuHelpLatestVersionNever			(void);						// IDM_HELP_LATESTVERSION_NEVER
	inline void	OnMenuHelpAbout					(void)	{OnAbout();}			// IDM_HELP_ABOUT
	
	// menu popup
	inline void	OnPopupMenuCopy					(void)	{OnCopy();}			// IDM_POPUP_COPY
	inline void	OnPopupMenuPaste					(void)	{OnPaste();}			// IDM_POPUP_PASTE
	inline void	OnPopupMenuOpen					(void)	{OnOpen();}			// IDM_POPUP_OPEN
	inline void	OnPopupMenuUnCompress				(void)	{OnUnCompress();}		// IDM_POPUP_UNCOMPRESS
	
	void			OnPopupMenuLoad					(void);						// IDM_POPUP_LOADPICT
	void			OnPopupMenuMoveUp					(void);						// IDM_POPUP_MOVEUP
	void			OnPopupMenuMoveDown					(void);						// IDM_POPUP_MOVEDOWN

	inline void	OnPopupMenuAddFile					(void)	{OnAddFile();}			// IDM_POPUP_ADDFILE
	inline void	OnPopupMenuAddDir					(void)	{OnAddDir();}			// IDM_POPUP_ADDDIR
	inline void	OnPopupMenuAddUrl					(void)	{OnAddUrl();}			// IDM_POPUP_ADDURL
	inline void	OnPopupMenuRemove					(void)	{OnRemove();}			// IDM_POPUP_REMOVE
	inline void	OnPopupMenuRemoveAll				(void)	{OnRemoveAll();}		// IDM_POPUP_REMOVEALL
	
	inline void	OnPopupMenuUnloadList				(void)	{OnUnloadPlaylistFile();}// IDM_POPUP_UNLOADLIST
	inline void	OnPopupMenuNewList					(void)	{OnNewPlaylistFile();}	// IDM_POPUP_NEWLIST
	inline void	OnPopupMenuLoadList					(void)	{OnLoadPlaylistFile();}	// IDM_POPUP_LOADLIST
	inline void	OnPopupMenuSaveList					(void)	{OnSavePlaylistFile();}	// IDM_POPUP_SAVELIST
	inline void	OnPopupMenuFavouritePicture			(void)	{OnFavouritePicture();}	// IDM_POPUP_FAVOURITE_PICTURE
	inline void	OnPopupMenuFavouriteAudio			(void)	{OnFavouriteAudio();}	// IDM_POPUP_FAVOURITE_AUDIO
	inline void	OnPopupMenuLoadFavouritePicture		(void)	{OnLoadFavouritePicture();}// IDM_POPUP_LOAD_FAVOURITE_PICTURE
	inline void	OnPopupMenuLoadFavouriteAudio			(void)	{OnLoadFavouriteAudio();}// IDM_POPUP_LOAD_FAVOURITE_AUDIO

	// toolbar
	inline void	OnButtonAddFile					(void)	{OnAddFile();}			// IDC_BUTTON_ADDFILE
	inline void	OnButtonAddDir						(void)	{OnAddDir();}			// IDC_BUTTON_ADDDIR
	inline void	OnButtonRemove						(void)	{OnRemove();}			// IDC_BUTTON_REMOVE
	inline void	OnButtonRemoveAll					(void)	{OnRemoveAll();}		// IDC_BUTTON_REMOVEALL
	inline void	OnButtonDeleteFile					(void)	{OnDeleteFile();}		// IDC_BUTTON_DELETEFILE
	inline void	OnButtonUndesktop					(void)	{OnUndesktop();}		// IDC_BUTTON_UNDESKTOP
	inline void	OnButtonWallBrowser					(void)	{OnBrowser();}			// IDC_BUTTON_WALLBROWSER
	inline void	OnButtonCrawler					(void)	{OnCrawler();}			// IDC_BUTTON_CRAWLER

	LRESULT		OnThreadParams						(WPARAM wParam,LPARAM lParam);

	// File
	//
	void			OnOpen					(void);
	void			OnSaveAs					(void);
	void			OnDeleteFile				(void);
	void			OnExit					(void);
	LRESULT		OnQueryEndSession			(WPARAM wParam,LPARAM lParam);
	//
	// Edit
	//
	void			OnCopy					(void);
	void			OnPaste					(void);
	void			OnEmptyClipboard			(void);
	BOOL			IsCopyAvailable			(BOOL* bIsPicture = NULL);
	BOOL			IsPasteAvailable			(void);
	//
	// Playlist
	//
	inline void	OnPrevious				(void) {SendWallPaperMessage(WALLPAPER_GOTO_PREVIOUS_PICTURE);}
	inline void	OnNext					(void) {SendWallPaperMessage(WALLPAPER_GOTO_NEXT_PICTURE);}
	LRESULT		OnPause					(WPARAM = 0/*wParam*/,LPARAM = 0/*lParam*/);
	void			OnResume					(void);
	void			OnAddFile					(void);
	LRESULT		OnAddFile					(WPARAM wParam,LPARAM lParam);
	void			OnAddDir					(void);
	LRESULT		OnAddDir					(WPARAM wParam,LPARAM lParam);
	void			OnAddUrl					(void);
	LRESULT		OnRemove					(WPARAM wParam,LPARAM lParam);
	void			OnRemove					(void);
	void			OnRemoveAll				(void);
	void			OnUnloadPlaylistFile		(void);
	void			OnNewPlaylistFile			(void);
	void			OnLoadPlaylistFile			(void);
	void			OnSavePlaylistFile			(void);
	LRESULT		OnFavourite				(WPARAM wParam,LPARAM lParam);
	void			OnFavouritePicture			(void);
	void			OnFavouriteAudio			(void);
	void			OnLoadFavouritePicture		(void);
	void			OnLoadFavouriteAudio		(void);
	//
	// Tools
	//
	inline void	OnUndesktop				(void) {DisableDesktopWallPaper();}
	void			OnDesktopIcons				(void);
	void			OnDisplayProperties			(void);
	void			OnBrowser					(void);
	void			OnThumbnails				(void);
	LRESULT		OnThumbnailsDone			(WPARAM wParam,LPARAM lParam);
	LRESULT		OnCrawler					(WPARAM wParam = NULL,LPARAM lParam = NULL);
	LRESULT		OnCrawlerDone				(WPARAM wParam,LPARAM lParam);
	void			OnPlayer					(LPCSTR lpcszAudioFileName,CAudioFilesList* pAudioFilesList,int nPlayMode,BOOL bForcePlayingIfIdle = FALSE);
	LRESULT		OnAudioPlayerEvent			(WPARAM wParam,LPARAM lParam);
	LRESULT		OnDownloadProgress			(WPARAM wParam,LPARAM lParam);
	BOOL			LoadTaskbarPopupList		(CTaskbarPopupList* pTaskbarList,int nType/* 0=audio, 1=pict */);
	void			OnUnCompress				(void);
	//
	// Options
	//
	void			OnNervousTitle				(BOOL bFlag);
	void			OnContextMenu				(void);
	//
	// About
	//
	void			OnHotKeys					(void);
	void			OnHomePage				(void);
	LRESULT		OnLatestVersion			(WPARAM wParam,LPARAM lParam);
	void			OnAbout					(void);

	// interfaccia
	void			PlaylistSave				(void);
	inline void	PlaylistSetModified			(void) {m_bIsPlaylistModified = TRUE;}
	inline void	PlaylistResetModified		(void) {m_bIsPlaylistModified = FALSE;}
	inline BOOL	PlaylistIsModified			(void) {return(m_bIsPlaylistModified);}
	void			OnUpdateGUI				(void);
	LRESULT		OnEnableGUI				(WPARAM bEnable,LPARAM lPauseResume);

	// playlist
	LRESULT		OnPlaylistPopupMenu			(WPARAM wParam,LPARAM lParam);
	LRESULT		OnPlaylistButtonDown		(WPARAM wParam,LPARAM /*lParam*/);
	LRESULT		OnPlaylistButtonDoubleClick	(WPARAM /*wParam*/,LPARAM /*lParam*/);
	LRESULT		OnPlaylistColumnClick		(WPARAM /*wParam*/,LPARAM /*lParam*/);
	LRESULT		OnPlaylistReorder			(WPARAM /*wParam*/,LPARAM /*lParam*/);
	LRESULT		OnLoadPlaylist				(WPARAM /*wParam*/,LPARAM /*lParam*/);
	static UINT	LoadPlaylist				(LPVOID);
	int			LoadPlaylist				(void);
	BOOL			CreatePlaylistCtrl			(LPCSTR = NULL);
	LPCSTR		FormatPlaylistItem			(LPCSTR,LPCSTR = NULL,LPCSTR = NULL,QWORD = (QWORD)-1L);
	int			GetItem					(LPSTR lpszItemText,int cbItemText,int nItem = -1);
	int			AddFile					(LPCSTR lpcszFileName,int nPlayMode = 0);
	int			AddDir					(LPCSTR,BOOL);
	BOOL			AddUrl					(LPCSTR);
	BOOL			GetUrl					(LPSTR lpszUrl,UINT cbUrl,LPSTR lpszLocalFile,UINT nLocalFileSize);

	// anteprima
	LRESULT		OnCreatePreviewWindow		(WPARAM /*wParam*/,LPARAM /*lParam*/);
	LRESULT		OnClosePreviewWindow		(WPARAM /*wParam*/,LPARAM /*lParam*/);
	void			OnCheckPreviewMode			(void);
	void			OnPreviewMode				(BOOL bForceFocus);

	// popup e tooltip
	void			OnBalloon					(void);
	void			OnTaskbarPopup				(void);
	LRESULT		OnTooltipCallback			(WPARAM wParam,LPARAM lParam);

	// drag and drop
	void			OnBeginDrag				(NMHDR* pNMHDR,LRESULT* pResult) ;
	void			OnDropFiles				(void);
	LRESULT		OnDropOle					(WPARAM wParam,LPARAM lParam);

	// timer x cambio sfondo
	void			OnTimer					(UINT);
	void			OnTimerChange				(void);
	void			OnVScroll					(UINT,UINT,CScrollBar*);
	BOOL			Timerize					(void);

	// titolo finestra
	void			SetWindowTitleString		(LPCSTR,BOOL bCheckForInfamousSentencesUsage = TRUE,BOOL bIsAnInternalString = FALSE);
	void			SetWindowTitleID			(UINT,LPCSTR = NULL);

	// infamous sentences
	void			NervousTitle				(void);
	LPSTR		GetInfamous				(void);
	BOOL			CreateInfamous				(void);

	// barre di stato
	void			SetStatusBarPicturePanelCount	(int nItemCount = -1);		// 0
	void			SetStatusBarPicturePanelMessage(UINT nID = (UINT)-1L);		// 0
	void			SetStatusBarDplPanel		(LPCSTR = NULL);			// 1
	void			SetStatusBarInfoPane		(LPCSTR);					// 2
	void			SetStatusBarInfoPane		(UINT);					// 2
	void			SetStatusBarInfoPane		(UINT,LPCSTR);				// 2
	void			SetStatusBarInfoPane		(UINT,UINT);				// 2

	// desktop
	inline void	SendWallPaperMessage		(int i = -1) {SendMessage(WM_SETWALLPAPER,(WPARAM)i,0L);}
	inline void	PostWallPaperMessage		(int i = -1) {PostMessage(WM_SETWALLPAPER,(WPARAM)i,0L);}
	LRESULT		OnSetWallpaper				(WPARAM wParam,LPARAM lParam);
	BOOL			SetDesktopWallPaper			(LPCSTR lpcszFileName,int nItem,BOOL bCalledFromCommandLine);
	void			EnableDesktopWallPaper		(LPCSTR lpcszFileName,UINT nMode);
	void			DisableDesktopWallPaper		(void);
	void			SetWallPaperMode			(UINT nMode);
	int			NormalizeEffectValue		(int nValue,int nLibraryMin,int nLibraryMax);

	// affinche' i threads possano ricavare i puntatori ai membri interni
	LRESULT		OnGetCurrentItem			(WPARAM wParam,LPARAM lParam);
	LRESULT		OnGetPlaylist				(WPARAM wParam,LPARAM lParam);
	LRESULT		OnGetConfiguration			(WPARAM wParam,LPARAM lParam);
	
	// gestione threads
	BOOL			IsUIThreadRunning			(LPCSTR lpcszThreadName,CWinThread** pWinThread,BOOL bCheckForExistingWindow = FALSE,LPCSTR lpcszWindowTitle = NULL,HWND* phWnd = NULL,BOOL* pbIsAnExternalInstance = NULL);
	BOOL			IsInternalThreadRunning		(LPCSTR lpcszThreadName,CThread** pThread);

	// gestione files compressi (.gzw, .rar, .zip, etc.)
	BOOL			AddArchiveContentToPlaylist	(LPARAM lParam,LPCSTR lpcszZippedFile);	
	LRESULT		OnArchiveExtractFile		(WPARAM wParam,LPARAM lParam);
	LRESULT		OnArchiveGetParent			(WPARAM wParam,LPARAM lParam);

	// variabili
	CCmdLineOptions*		m_pCmdLine;								// opzioni da linea di comando
	char					m_szCmdLine[MAX_CMDLINE+1];					// linea di comando
	BOOL					m_bMustTerminate;							// istanze multiple

	CString				m_strTitleBar;								// titolo del dialogo
	char					m_szNervousTitle[WALLPAPER_MAX_TITLE_CHARS+1];	// buffer per titolo a scorrimento
	UINT					m_nNervousTitleTimerId;						// id timer per titolo a scorrimento

	UINT					m_nDesktopTimerId;							// id per timer (cambio sfondo)
	UINT					m_nDesktopTimeout;							// valore per timeout (cambio sfondo)
	CSyncProcesses			m_mutexDesktop;							// mutex per cambio sfondo
	BOOL					m_bDesktopIcons;							// visualizzazione icone desktop

	CWndLayered			m_wndLayered;								// trasparenza
	
	CProgressCtrlEx		m_ProgressCtrl;							// progress bar per la status bar
	CTrayIcon*			m_pTrayIcon;								// icona per la system tray
	CBalloon*				m_pBalloon;								// per il (tooltip) balloon esteso
	CWnd*				m_pWndTaskbarParent;						// oggetto per la finestra padre del popup (per non sucare la minchia dato che passando la finestra dell'applicazione, quando questa viene minimizzata, verrebbe minimizzato anche il popup)
	CTaskbarNotifier*		m_pTaskbarNotifier;							// oggetto per la finestra popup
	TASKBARPOPUP			m_TaskbarPopup;							// elemento corrente nella lista dei popup
	int					m_nPopupIndex;
	char					m_szTaskbarToolTipText[1024];					// testo per il tooltip per il popup

	CLIPBOARDFORMATS		m_enumClipboardFormat;						// formato dati clipboard
	UINT					m_nNetscapeClipboardFormat;					// formato dati clipboard per Netscape
	UINT					m_nExplorerClipboardFormat;					// formato dati clipboard per IExplore
	COleDropTargetEx		m_oleDropTarget;							// drag and drop via OLE

	CArchiveFile			m_Archive;								// per gestione file compressi

	CImageFactory			m_ImageFactory;							// interfaccia x immagini
	CImage*				m_pImage;									// oggetto immagine
	PICTUREINFO			m_PictureInfo;								// per le info sull'immagine corrente
	CImageFactory			m_ImageFactoryPreview;						// interfaccia x immagini (anteprima)
	CImage*				m_pImagePreview;							// oggetto immagine (anteprima)
	
	CRect				m_rcDrawRect;								// dimensione rettangolo per il bitmap per lo sfondo

	CIconIndexList			m_listPlaylistIcons;						// lista icone per la playlist
	CListCtrlEx			m_wndPlaylist;								// lista per la playlist

	CSyncThreads			m_syncAudioPlaylist;						// per la sincronizzazione sulla lista per il player audio
	CTaskbarPopupList		m_listAudioTaskbarBitmaps;					// lista per i popup audio
	CTaskbarPopupList		m_listPictTaskbarBitmaps;					// lista per i popup per le immagini

	BOOL					m_bPreviewMode;							// modalita' anteprima
	CRect				m_rcPlaylistArea;							// spazio occupato dal riquadro per l'anteprima
	HFONT				m_hFilePreviewFont;							// font per il controllo di cui sopra

	CUrl					m_Url;									// gestione urls

	CWallPaperThreadList	m_listRunningUIThreads;						// lista per i threads attivi (UI)
	CWallPaperThreadList	m_listRunningInternalThreads;					// lista per i threads attivi (interni)
	HANDLE				m_hTerminateEvent;							// per la sincronizzazione con i threads interni

	CRand				m_RandomNumberGenerator;						// per l'ordinamento (casuale) della playlist

	CFindFile				m_findPictIcons;							// per le icone per il balloon (immagini e audio)
	int					m_nCurrentPictIcon;
	int					m_nPictIconsCount;
	CFindFile				m_findAudioIcons;
	int					m_nCurrentAudioIcon;
	int					m_nAudioIconsCount;

	CWallPaperConfig		m_Config;									// configurazione
	BOOL					m_bIsPlaylistModified;

	DECLARE_MESSAGE_MAP()
};

#endif // _WALLPAPERDLG_H
