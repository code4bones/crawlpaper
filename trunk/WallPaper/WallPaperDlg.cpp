/*
	WallPaperDlg.cpp
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
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include "strings.h"
#include <limits.h>
#include "window.h"
#include "win32api.h"
#include "mfcapi.h"
#include "CNodeList.h"
#include "CRand.h"
#include "CBinFile.h"
#include "CFilenameFactory.h"
#include "CBrowser.h"
#include "CDialogEx.h"
#include "CTitleMenu.h"
#include "CDibCtrl.h"
#include "CDirDialog.h"
#include "CFileNameOpenDialog.h"
#include "CTextFile.h"
#include "CFindFile.h"
#include "CArchive.h"
#include "CImageDialog.h"
#include "CImageFactory.h"
#include "CImageParams.h"
#include "CListCtrlEx.h"
#include "CSync.h"
#include "COleDropTargetEx.h"
#include "CRegistry.h"
#include "CColorStatic.h"
#include "CStaticFilespec.h"
#include "CTrayIcon.h"
#include "CBalloon.h"
#include "CTaskbarNotifier.h"
#include "CUrl.h"
#include "CAudioPlayer.h"
#include "CAudioInfo.h"
#include "mmaudio.h"
#include "CWndLayered.h"
#include "CProgressCtrlEx.h"
#include "MessageBoxExt.h"
#include "WallBrowserVersion.h"
#include "WallPaperAboutDlg.h"
#include "WallPaperAddUrlDlg.h"
#include "WallPaperConfig.h"
#include "WallPaperCmdLine.h"
#include "WallPaperCrawlerDlg.h"
#include "WallPaperCrawlerSettingsDlg.h"
#include "WallPaperCrawlerUIThread.h"
#include "WallPaperDlg.h"
#include "WallPaperDrawSettingsDlg.h"
#include "WallPaperIconIndex.h"
#include "WallPaperMessages.h"
#include "WallPaperThumbnailsDlg.h"
#include "WallPaperThumbSettingsDlg.h"
#include "WallPaperThumbnailsUIThread.h"
#include "WallPaperMP3TagDlg.h"
#include "WallPaperMP3TagUIThread.h"
#include "WallPaperPlayerDlg.h"
#include "WallPaperPlayerUIThread.h"
#include "WallPaperPreviewDlg.h"
#include "WallPaperPreviewUIThread.h"
#include "WallPaperThreadList.h"
#include "WallPaperUniVersion.h"
#include "WallPaperVersion.h"
#include "WallPaperDownloadDlg.h"
#include "WallPaperSplashScreenDlg.h"
#include "WallPaperSplashUIThread.h"
#include "WallPaperDoNotAskMore.h"
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

#ifdef _DEBUG
void DoTest(void);
#endif

BEGIN_MESSAGE_MAP(CWallPaperDlg,CDialogEx)
	// drag and drop
	ON_NOTIFY(LVN_BEGINDRAG,IDC_LIST_PLAYLIST,OnBeginDrag)
	ON_MESSAGE(WM_DROPOLE,OnDropOle)
	
	// tray icon
	ON_MESSAGE(WM_NOTIFYTASKBARICON,OnNotifyTrayIcon)
	ON_MESSAGE(WM_NOTIFYTASKBARPOPUP,OnNotifyTaskbarPopup)
	
	// tooltip
	ON_MESSAGE(WM_TOOLTIP_CALLBACK,OnTooltipCallback)

	// sistema
	ON_WM_SYSCOMMAND()

	// per i check del menu opzioni
	ON_WM_INITMENUPOPUP()

	// menu principale
	// File
	ON_COMMAND(IDM_FILE_MINIMIZE,OnMenuFileMinimize)
	ON_COMMAND(IDM_FILE_MAXIMIZE,OnMenuFileMaximize)
	ON_COMMAND(IDM_FILE_OPEN,OnMenuFileOpen)
	ON_COMMAND(IDM_FILE_SAVEAS,OnMenuFileSaveAs)
	ON_COMMAND(IDM_FILE_DELETE,OnMenuFileDeleteFile)
	ON_COMMAND(IDM_FILE_EXIT,OnMenuFileExit)
	// Edit
	ON_COMMAND(IDM_EDIT_COPY,OnMenuEditCopy)
	ON_COMMAND(IDM_EDIT_PASTE,OnMenuEditPaste)
	ON_COMMAND(IDM_EDIT_EMPTYCLIPBOARD,OnMenuEditEmptyClipboard)
	// Playlist
	ON_COMMAND(IDM_PLAYLIST_SAVEPLAYLIST,OnMenuPlaylistSavePlaylist)
	ON_COMMAND(IDM_PLAYLIST_PREVIOUS,OnMenuPlaylistPrevious)
	ON_COMMAND(IDM_PLAYLIST_NEXT,OnMenuPlaylistNext)
	ON_COMMAND(IDM_PLAYLIST_PAUSE,OnMenuPlaylistPause)
	ON_COMMAND(IDM_PLAYLIST_RESUME,OnMenuPlaylistResume)
	ON_COMMAND(IDM_PLAYLIST_ADDFILE,OnMenuPlaylistAddFile)
	ON_COMMAND(IDM_PLAYLIST_ADDDIR,OnMenuPlaylistAddDir)
	ON_COMMAND(IDM_PLAYLIST_ADDURL,OnMenuPlaylistAddUrl)
	ON_COMMAND(IDM_PLAYLIST_REMOVE,OnMenuPlaylistRemove)
	ON_COMMAND(IDM_PLAYLIST_REMOVEALL,OnMenuPlaylistRemoveAll)
	ON_COMMAND(IDM_PLAYLIST_UNLOADLIST,OnMenuPlaylistUnloadList)
	ON_COMMAND(IDM_PLAYLIST_NEWLIST,OnMenuPlaylistNewList)
	ON_COMMAND(IDM_PLAYLIST_LOADLIST,OnMenuPlaylistLoadList)
	ON_COMMAND(IDM_PLAYLIST_SAVELIST,OnMenuPlaylistSaveList)
	ON_COMMAND(IDM_PLAYLIST_FAVOURITE_PICTURE,OnMenuPlaylistFavouritePicture)
	ON_COMMAND(IDM_PLAYLIST_FAVOURITE_AUDIO,OnMenuPlaylistFavouriteAudio)
	ON_COMMAND(IDM_PLAYLIST_LOAD_FAVOURITE_PICTURE,OnMenuPlaylistLoadFavouritePicture)
	ON_COMMAND(IDM_PLAYLIST_LOAD_FAVOURITE_AUDIO,OnMenuPlaylistLoadFavouriteAudio)
	// Tools
	ON_COMMAND(IDM_TOOLS_UNDESKTOP,OnMenuToolsUndesktop)
	ON_COMMAND(IDM_TOOLS_DESKTOPICONS,OnMenuToolsDesktopIcons)
	ON_COMMAND(IDM_TOOLS_DISPLAYPROPERTIES,OnMenuToolsDisplayProperties)
	ON_COMMAND(IDM_TOOLS_WALLBROWSER,OnMenuToolsWallBrowser)
	ON_COMMAND(IDM_TOOLS_THUMBNAILS,OnMenuToolsThumbnails)
	ON_COMMAND(IDM_TOOLS_CRAWLER,OnMenuToolsCrawler)
	ON_COMMAND(IDM_TOOLS_UNCOMPRESS,OnMenuToolsUnCompress)
	// Options
	ON_COMMAND(IDM_OPTIONS_ALWAYSONTOP,OnMenuOptionsAlwaysOnTop)
	ON_COMMAND(IDM_OPTIONS_AUTORUN,OnMenuOptionsAutoRun)
	ON_COMMAND(IDM_OPTIONS_SPLASHSCREEN,OnMenuOptionsSplashScreen)
	ON_COMMAND(IDM_OPTIONS_TRAYICON,OnMenuOptionsTrayIcon)
	ON_COMMAND(IDM_OPTIONS_MINIMIZE_ON_CLOSE,OnMenuOptionsMinimizeOnClose)
	
	ON_COMMAND(IDM_OPTIONS_AUDIO_NO_POPUP,OnMenuOptionsAudioNoPopup)
	ON_COMMAND(IDM_OPTIONS_AUDIO_BALLOON_POPUP,OnMenuOptionsAudioBalloonPopup)
	ON_COMMAND(IDM_OPTIONS_AUDIO_RELOAD_BALLOON,OnMenuOptionsAudioReloadBalloon)
	ON_COMMAND(IDM_OPTIONS_AUDIO_BALLOON_DEFAULTICON,OnMenuOptionsAudioBalloonDefaultIcon)
	ON_COMMAND(IDM_OPTIONS_AUDIO_BALLOON_CUSTOMICON,OnMenuOptionsAudioBalloonCustomIcon)
	ON_COMMAND(IDM_OPTIONS_AUDIO_TASKBAR_POPUP,OnMenuOptionsAudioTaskbarPopup)
	ON_COMMAND(IDM_OPTIONS_AUDIO_RELOAD_TASKBAR_POPUP,OnMenuOptionsAudioReloadTaskbarBitmapList)

	ON_COMMAND(IDM_OPTIONS_PICT_NO_POPUP,OnMenuOptionsPictNoPopup)
	ON_COMMAND(IDM_OPTIONS_PICT_BALLOON_POPUP,OnMenuOptionsPictBalloonPopup)
	ON_COMMAND(IDM_OPTIONS_PICT_RELOAD_BALLOON,OnMenuOptionsPictReloadBalloon)
	ON_COMMAND(IDM_OPTIONS_PICT_BALLOON_DEFAULTICON,OnMenuOptionsPictBalloonDefaultIcon)
	ON_COMMAND(IDM_OPTIONS_PICT_BALLOON_CUSTOMICON,OnMenuOptionsPictBalloonCustomIcon)
	ON_COMMAND(IDM_OPTIONS_PICT_TASKBAR_POPUP,OnMenuOptionsPictTaskbarPopup)
	ON_COMMAND(IDM_OPTIONS_PICT_RELOAD_TASKBAR_POPUP,OnMenuOptionsPictReloadTaskbarBitmapList)

	ON_COMMAND(IDM_OPTIONS_GRIDLINES,OnMenuOptionsGridlines)
	ON_COMMAND(IDM_OPTIONS_GRIDLINES_FOREGROUND_COLOR,OnMenuOptionsGridlinesFgColor)
	ON_COMMAND(IDM_OPTIONS_GRIDLINES_BACKGROUND_COLOR,OnMenuOptionsGridlinesBgColor)
	ON_COMMAND(IDM_OPTIONS_GRID_RESET,OnMenuOptionsGridReset)
	ON_COMMAND(IDM_OPTIONS_CLEAR_DONOTASKMORE,OnMenuOptionsClearDoNotAskMore)
	ON_COMMAND(IDM_OPTIONS_ONE_CLICK_CHANGE,OnMenuOptionsOneClickChange)
	ON_COMMAND(IDM_OPTIONS_NERVOUS_TITLE,OnMenuOptionsNervousTitle)
	ON_COMMAND(IDM_OPTIONS_CONTEXT_MENU,OnMenuOptionsContextMenu)
	ON_COMMAND(IDM_OPTIONS_PLAYLIST_LOADING_ADD,OnMenuOptionsPlaylistLoadingAdd)
	ON_COMMAND(IDM_OPTIONS_PLAYLIST_LOADING_PUT,OnMenuOptionsPlaylistLoadingPut)
	ON_COMMAND(IDM_OPTIONS_RANDOMIZE_PLAYLIST_SELECTION,OnMenuOptionsRandomizePlaylistSelection)
	ON_COMMAND(IDM_OPTIONS_RANDOMIZE_PLAYLIST_REORDER,OnMenuOptionsRandomizePlaylistReorder)
	ON_COMMAND(IDM_OPTIONS_RANDOMIZE_PLAYLIST_NONE,OnMenuOptionsRandomizePlaylistNone)	
	ON_COMMAND(IDM_OPTIONS_INFAMOUS_SENTENCES,OnMenuOptionsInfamousSentences)
	ON_COMMAND(IDM_OPTIONS_RECURSE_SUBDIR,OnMenuOptionsRecurseSubdir)
	ON_COMMAND(IDM_OPTIONS_CONFIRM_FILE_DELETE,OnMenuOptionsConfirmFileDelete)
	ON_COMMAND(IDM_OPTIONS_DELETE_FILES_TO_RECYCLEBIN,OnMenuOptionsDeleteFilesToRecycleBin)
	ON_COMMAND(IDM_OPTIONS_TEMPORARY_FILES_CLEAR_AT_EXIT,OnMenuOptionsTempFilesClearAtExit)
	ON_COMMAND(IDM_OPTIONS_TEMPORARY_FILES_CLEAR_AT_RUNTIME,OnMenuOptionsTempFilesClearAtRuntime)
	ON_COMMAND(IDM_OPTIONS_TEMPORARY_FILES_DO_NOT_CLEAR,OnMenuOptionsTempFilesDoNotClear)
	ON_COMMAND(IDM_OPTIONS_PRIORITY_PROCESS_REALTIME,OnMenuOptionsProcessPriorityRealTime)
	ON_COMMAND(IDM_OPTIONS_PRIORITY_PROCESS_HIGH,OnMenuOptionsProcessPriorityHigh)
	ON_COMMAND(IDM_OPTIONS_PRIORITY_PROCESS_NORMAL,OnMenuOptionsProcessPriorityNormal)
	ON_COMMAND(IDM_OPTIONS_PRIORITY_PROCESS_IDLE,OnMenuOptionsProcessPriorityIdle)
	ON_COMMAND(IDM_OPTIONS_PRIORITY_THREAD_TIME_CRITICAL,OnMenuOptionsThreadPriorityTimeCritical)
	ON_COMMAND(IDM_OPTIONS_PRIORITY_THREAD_HIGHEST,OnMenuOptionsThreadPriorityHighest)
	ON_COMMAND(IDM_OPTIONS_PRIORITY_THREAD_ABOVE_NORMAL,OnMenuOptionsThreadPriorityAboveNormal)
	ON_COMMAND(IDM_OPTIONS_PRIORITY_THREAD_NORMAL,OnMenuOptionsThreadPriorityNormal)
	ON_COMMAND(IDM_OPTIONS_PRIORITY_THREAD_BELOW_NORMAL,OnMenuOptionsThreadPriorityBelowNormal)
	ON_COMMAND(IDM_OPTIONS_PRIORITY_THREAD_LOWEST,OnMenuOptionsThreadPriorityLowest)
	ON_COMMAND(IDM_OPTIONS_PRIORITY_THREAD_IDLE,OnMenuOptionsThreadPriorityIdle)
	ON_COMMAND(IDM_OPTIONS_DRAW_SETTINGS,OnMenuOptionsDrawSettings)
	ON_COMMAND(IDM_OPTIONS_THUMBNAILS_SETTINGS,OnMenuOptionsThumbnailsSettings)
	ON_COMMAND(IDM_OPTIONS_CRAWLER_SETTINGS,OnMenuOptionsCrawlerSettings)
	ON_COMMAND(IDM_OPTIONS_SAVE_CONFIG,OnMenuOptionsSaveConfiguration)
	ON_COMMAND(IDM_OPTIONS_LOAD_CONFIG,OnMenuOptionsLoadConfiguration)
	// Help
	ON_COMMAND(IDM_HELP_HOTKEYS,OnMenuHelpHotKeys)
	ON_COMMAND(IDM_HELP_HOMEPAGE,OnMenuHelpHomePage)
	ON_COMMAND(IDM_HELP_LATESTVERSION,OnMenuHelpLatestVersion)
	ON_COMMAND(IDM_HELP_LATESTVERSION_AT_STARTUP,OnMenuHelpLatestVersionAtStartup)
	ON_COMMAND(IDM_HELP_LATESTVERSION_EVERY_N_DAYS,OnMenuHelpLatestVersionEveryNDays)
	ON_COMMAND(IDM_HELP_LATESTVERSION_NEVER,OnMenuHelpLatestVersionNever)
	ON_COMMAND(IDM_HELP_ABOUT,OnMenuHelpAbout)
	
	// menu popup
	ON_COMMAND(IDM_POPUP_COPY,OnPopupMenuCopy)
	ON_COMMAND(IDM_POPUP_PASTE,OnPopupMenuPaste)
	ON_COMMAND(IDM_POPUP_OPEN,OnPopupMenuOpen)
	ON_COMMAND(IDM_POPUP_UNCOMPRESS,OnPopupMenuUnCompress)
	ON_COMMAND(IDM_POPUP_LOADPICT,OnPopupMenuLoad)
	ON_COMMAND(IDM_POPUP_MOVEUP,OnPopupMenuMoveUp)
	ON_COMMAND(IDM_POPUP_MOVEDOWN,OnPopupMenuMoveDown)
	ON_COMMAND(IDM_POPUP_ADDFILE,OnPopupMenuAddFile)
	ON_COMMAND(IDM_POPUP_ADDDIR,OnPopupMenuAddDir)
	ON_COMMAND(IDM_POPUP_ADDURL,OnPopupMenuAddUrl)
	ON_COMMAND(IDM_POPUP_REMOVE,OnPopupMenuRemove)
	ON_COMMAND(IDM_POPUP_REMOVEALL,OnPopupMenuRemoveAll)
	ON_COMMAND(IDM_POPUP_UNLOADLIST,OnPopupMenuUnloadList)
	ON_COMMAND(IDM_POPUP_NEWLIST,OnPopupMenuNewList)
	ON_COMMAND(IDM_POPUP_LOADLIST,OnPopupMenuLoadList)
	ON_COMMAND(IDM_POPUP_SAVELIST,OnPopupMenuSaveList)
	ON_COMMAND(IDM_POPUP_FAVOURITE_PICTURE,OnPopupMenuFavouritePicture)
	ON_COMMAND(IDM_POPUP_FAVOURITE_AUDIO,OnPopupMenuFavouriteAudio)
	ON_COMMAND(IDM_POPUP_LOAD_FAVOURITE_PICTURE,OnPopupMenuLoadFavouritePicture)
	ON_COMMAND(IDM_POPUP_LOAD_FAVOURITE_AUDIO,OnPopupMenuLoadFavouriteAudio)

	// toolbar
	ON_BN_CLICKED(IDC_BUTTON_ADDFILE,OnButtonAddFile)
	ON_BN_CLICKED(IDC_BUTTON_ADDDIR,OnButtonAddDir)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE,OnButtonRemove)
	ON_BN_CLICKED(IDC_BUTTON_REMOVEALL,OnButtonRemoveAll)
	ON_BN_CLICKED(IDC_BUTTON_DELETEFILE,OnButtonDeleteFile)
	ON_BN_CLICKED(IDC_BUTTON_UNDESKTOP,OnButtonUndesktop)
	ON_BN_CLICKED(IDC_BUTTON_WALLBROWSER,OnButtonWallBrowser)
	ON_BN_CLICKED(IDC_BUTTON_CRAWLER,OnButtonCrawler)

	// interfaccia
	ON_MESSAGE(WM_ENABLEGUI,OnEnableGUI)
	ON_MESSAGE(WM_DISABLEGUI,OnEnableGUI)
	ON_MESSAGE(WM_LOADPLAYLIST,OnLoadPlaylist)
	ON_MESSAGE(WM_FAVOURITE_ADD,OnFavourite)

	// lista
	ON_MESSAGE(WM_ONPOPUPMENU,OnPlaylistPopupMenu)
	ON_MESSAGE(WM_ONLBUTTONDOWN,OnPlaylistButtonDown)
	ON_MESSAGE(WM_ONLBUTTONDBLCLK,OnPlaylistButtonDoubleClick)
	ON_MESSAGE(WM_ONCOLUMNCLICK,OnPlaylistColumnClick)

	// per permettere ai threads di ricavare i puntatori
	ON_MESSAGE(WM_GETCURRENTPLAYLISTITEM,OnGetCurrentItem)
	ON_MESSAGE(WM_GETPLAYLIST,OnGetPlaylist)
	ON_MESSAGE(WM_GETCONFIGURATION,OnGetConfiguration)

	// threads UI
	ON_MESSAGE(WM_THREAD_PARAMS,OnThreadParams)
	ON_MESSAGE(WM_CRAWLER_DONE,OnCrawlerDone)
	ON_MESSAGE(WM_THUMBNAILS_DONE,OnThumbnailsDone)
	ON_MESSAGE(WM_AUDIOPLAYER_EVENT,OnAudioPlayerEvent)
	ON_MESSAGE(WM_AUDIOPLAYER_REMOVEITEM,OnRemove)

	ON_MESSAGE(WM_DOWNLOAD_PROGRESS,OnDownloadProgress)
	
	// cambio sfondo
	ON_MESSAGE(WM_SETWALLPAPER,OnSetWallpaper)

	// hotkeys globali
	ON_MESSAGE(WM_HOTKEY,OnHotKey)

	// aggiunta elementi alla lista
	ON_MESSAGE(WM_PLAYLIST_ADDFILE,OnAddFile)
	ON_MESSAGE(WM_PLAYLIST_ADDDIR,OnAddDir)

	// anteprima
	ON_MESSAGE(WM_CREATEPREVIEWWINDOW,OnCreatePreviewWindow)
	ON_MESSAGE(WM_PREVIEW_CLOSED,OnClosePreviewWindow)

	// estrazione dal file compresso
	ON_MESSAGE(WM_ARCHIVE_EXTRACT_FILE,OnArchiveExtractFile)
	ON_MESSAGE(WM_ARCHIVE_GET_PARENT,OnArchiveGetParent)

	// per assicurarsi di eseguire il codice previsto per la terminazione
	ON_MESSAGE(WM_QUERYENDSESSION,OnQueryEndSession)

	// controlli della finestra principale
	ON_WM_VSCROLL()
	ON_EN_CHANGE(IDC_EDIT_TIMEOUT,OnTimerChange)
	ON_BN_CLICKED(IDC_CHECK_PICTURE_PREVIEW,OnCheckPreviewMode)

	// timer
	ON_WM_TIMER()
END_MESSAGE_MAP()

IMPLEMENT_DYNCREATE(CWallPaperDlg,CDialogEx)

/*
	DoDataExchange()
*/
void CWallPaperDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_LIST_PLAYLIST,m_wndPlaylist);
	DDX_Text(pDX,IDC_EDIT_TIMEOUT,m_nDesktopTimeout);
	// non usare il controllo min max perche' per ogni cambio deve reimpostare il timer
	//DDV_MinMaxUInt(pDX,m_m_nDesktopTimeout,WALLPAPER_MIN_VALUE_FOR_CHANGE_TIMEOUT,WALLPAPER_MAX_VALUE_FOR_CHANGE_TIMEOUT);
	DDX_Check(pDX,IDC_CHECK_PICTURE_PREVIEW,m_bPreviewMode);
}

/*
	CWallPaperDlg()
*/
CWallPaperDlg::CWallPaperDlg(HWND /*hWndParent*/,LPCSTR lpcszCmd/*=NULL*/)
: CDialogEx(	/*nTemplateID*/IDD_DIALOG_WALLPAPER,
			/*nToolbarID*/IDC_TOOL_BAR,
			/*nStatusbarID*/IDC_STATUS_BAR,
			/*nIconID*/IDI_ICON_WALLPAPER,
			/*nMenuID*/IDM_MENU,
#ifdef _RESIZABLE_DIALOG_STYLE
			/*bAllowResize*/TRUE,
#endif
			/*bAllowSnap*/TRUE,
			/*bAllowDragAndDrop*/TRUE,
			/*lpcszUniqueName*/WALLPAPER_INTERNAL_NAME,
			/*bAllowMultipleInstances*/TRUE,
			/*pWndParent*/NULL)
{
	// per evitare che la zoccola di MFC visualizzi il dialogo quando decide lei
	SetVisible(FALSE);

	// per abilitare/disabilitare il limite al ridimensionamento (per default impostato sulle dimensioni originali del dialogo)
#ifdef _RESIZABLE_DIALOG_STYLE
	AllowMinTrackSize(FALSE);
#endif

	// linea di comando
	if(lpcszCmd)
		strcpyn(m_szCmdLine,lpcszCmd,sizeof(m_szCmdLine));
	else
		memset(m_szCmdLine,'\0',sizeof(m_szCmdLine));
	m_pCmdLine = new CCmdLineOptions(m_szCmdLine);

	// per le istanze multiple, impostato per quei parametri da linea di comando
	// che richiedono il termine del programma una volta terminata l'elaborazione
	m_bMustTerminate = FALSE;

	// titolo
	m_strTitleBar.Format("%s v.%s",WALLPAPER_PROGRAM_NAME_ALIAS,WALLPAPER_VERSION);
	memset(m_szNervousTitle,'\0',sizeof(m_szNervousTitle));
	m_nNervousTitleTimerId = 0;

	// timer
	m_nDesktopTimerId = 0;
	m_nDesktopTimeout = m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_TIMEOUT_KEY);
	
	// mutex per il cambio dello sfondo (NON mettere il numero di versione nel nome)
	m_mutexDesktop.SetName(WALLPAPER_PROGRAM_NAME""WALLPAPER_PROGRAM_DESCRIPTION);
	m_mutexDesktop.SetTimeout(WALLPAPER_DESKTOP_MUTEX_TIMEOUT);

	// flag per visualizzazione icone del desktop
	m_bDesktopIcons = TRUE;

	// progress bar, icona per la system tray, balloon e popup
	m_ProgressCtrl.Create("",100,100,TRUE,1);
	m_pTrayIcon = NULL;
	m_pBalloon = NULL;
	m_pWndTaskbarParent = NULL;
	m_pTaskbarNotifier = NULL;
	memset(&m_TaskbarPopup,'\0',sizeof(TASKBARPOPUP));
	m_nPopupIndex = 0;
	memset(m_szTaskbarToolTipText,'\0',sizeof(m_szTaskbarToolTipText));

	// formati clipboard
	m_enumClipboardFormat = CLIPBOARD_UNAVAILABLE;
	m_nNetscapeClipboardFormat = m_nExplorerClipboardFormat = 0L;

	// libreria di default per le immagini
	char szLibraryName[_MAX_PATH+1];
	strcpyn(szLibraryName,m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_CURRENTLIBRARY_KEY),sizeof(szLibraryName));
	m_pImage = m_ImageFactory.Create(szLibraryName,sizeof(szLibraryName));
	memset(&m_PictureInfo,'\0',sizeof(PICTUREINFO));
	m_pImagePreview = m_ImageFactoryPreview.Create(szLibraryName,sizeof(szLibraryName));

	// flags interni	
	m_rcDrawRect.SetRect(0,0,DEFAULT_DRAWRECTX,DEFAULT_DRAWRECTY);

	// per la sincronizzazione sulla lista per il player audio
	m_syncAudioPlaylist.SetName("AudioPlaylist");

	// modalita' anteprima
	m_bPreviewMode = m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PICTUREPREVIEW_KEY);

	// per la sincronizzazione con i threads interni
	m_hTerminateEvent = NULL;

	// per l'ordinamento (casuale) della playlist
	m_RandomNumberGenerator.Seed(::GetTickCount());

	// per le icone per il balloon
	// immagini
	m_findPictIcons.Reset();
	m_nCurrentPictIcon = -1;
	m_nPictIconsCount = 0;
	// audio
	m_findAudioIcons.Reset();
	m_nCurrentAudioIcon = -1;
	m_nAudioIconsCount = 0;

	// flags interni	
	m_bIsPlaylistModified = FALSE;
}

/*
	~CWallPaperDlg()
*/
CWallPaperDlg::~CWallPaperDlg()
{
	if(m_pCmdLine)
		delete m_pCmdLine,m_pCmdLine = NULL;

	if(m_pTrayIcon)
		delete m_pTrayIcon,m_pTrayIcon = NULL;
	if(m_pBalloon)
		delete m_pBalloon,m_pBalloon = NULL;
	if(m_pTaskbarNotifier)
		delete m_pTaskbarNotifier,m_pTaskbarNotifier = NULL;
	if(m_pWndTaskbarParent)
		delete m_pWndTaskbarParent,m_pWndTaskbarParent = NULL;

	if(m_hTerminateEvent)
	{
		::CloseHandle(m_hTerminateEvent);
		m_hTerminateEvent = NULL;
	}

	if(m_hFilePreviewFont)
		::DeleteObject(m_hFilePreviewFont);
}

/*
	OnInitDialog()

	Abilita le istanze multiple (vedi il parametro passato al ctor) solo per ricevere e caricare i parametri della linea di comando
	(puo' essere chiamato dalla shell), per cui una volta eseguito il meccanismo della classe base per il passaggio di parametri
	tra istanze, controlla in proprio se gia' esiste un altra istanza terminando in caso affermativo.
*/
BOOL CWallPaperDlg::OnInitDialog(void)
{
#ifdef _DEBUG
	DoTest();
#endif

	// classe base
	if(!CDialogEx::OnInitDialog())
	{
		EndDialog(IDCANCEL);
		return(FALSE);
	}

	// visualizza la schermata iniziale (se vengono passati parametri si suppone che sia una
	// chiamata della shell, per cui non ri-visualizza lo splash screen da menu contestuale)
	if(m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_SPLASHSCREEN_KEY) && !m_pCmdLine->HaveCommandLine())
	{
		CWallPaperSplashScreenDlg dlg(this);
		dlg.DoModal();
	}

	// modifica il menu di sistema
	CMenu* pSysmenu = GetSystemMenu(FALSE);
	if(pSysmenu)
	{
		// se si aggiunge o toglie un entrata, modificare di conseguenza gli indici (IDM_SYSMENU_..._POSITION)
		// dei menu in modo che OnPause() e OnResume() possano aggiornare le entrate di competenza, notare che
		// gli indici sono a base 0
		char szMenuItem[_MAX_MENU_STRING_LENGTH];
		pSysmenu->AppendMenu(MF_SEPARATOR);
		strcpyn(szMenuItem,"E&xit",sizeof(szMenuItem));
		pSysmenu->AppendMenu(MF_STRING,IDM_SYSMENU_EXIT,szMenuItem);
		pSysmenu->AppendMenu(MF_SEPARATOR);
		strcpyn(szMenuItem,"&About...",sizeof(szMenuItem));
		pSysmenu->AppendMenu(MF_STRING,IDM_SYSMENU_ABOUT,szMenuItem);
		pSysmenu->AppendMenu(MF_SEPARATOR);
		strcpyn(szMenuItem,"Pa&use",sizeof(szMenuItem));
		pSysmenu->AppendMenu(MF_STRING,IDM_SYSMENU_PAUSE,szMenuItem);
		strcpyn(szMenuItem,"Add to &Favourite",sizeof(szMenuItem));
		pSysmenu->AppendMenu(MF_STRING,IDM_SYSMENU_FAVOURITE,szMenuItem);
		strcpyn(szMenuItem,"&Previous",sizeof(szMenuItem));
		pSysmenu->AppendMenu(MF_STRING,IDM_SYSMENU_PREVPIC,szMenuItem);
		strcpyn(szMenuItem,"&Next",sizeof(szMenuItem));
		pSysmenu->AppendMenu(MF_STRING,IDM_SYSMENU_NEXTPIC,szMenuItem);
		
		// forza il ridisegno
		DrawMenuBar();
	}

	// menu principale
	CMenu* pMenu = NULL;
	pMenu = GetMenu();
	if(pMenu)
		pMenu->EnableMenuItem(IDM_PLAYLIST_RESUME,MF_GRAYED);

	// toolbar
	CSize sizeButton(18,18);
	CSize sizeBitmap(17,17);	
	AddToolBarSeparator();
	AddToolBarButton(IDC_BUTTON_ADDFILE,		IDS_TOOLTIP_ADDFILE,		IDS_TOOLBAR_TEXT_ADDFILE);
	AddToolBarButton(IDC_BUTTON_ADDDIR,		IDS_TOOLTIP_ADDDIR,			IDS_TOOLBAR_TEXT_ADDDIR);
	AddToolBarButton(IDC_BUTTON_REMOVE,		IDS_TOOLTIP_REMOVEFILE,		IDS_TOOLBAR_TEXT_REMOVE);
	AddToolBarButton(IDC_BUTTON_REMOVEALL,		IDS_TOOLTIP_REMOVEALLFILES,	IDS_TOOLBAR_TEXT_REMOVEALL);
	AddToolBarButton(IDC_BUTTON_DELETEFILE,		IDS_TOOLTIP_DELETEFILE,		IDS_TOOLBAR_TEXT_DELETE);
	AddToolBarSeparator();
	AddToolBarButton(IDC_BUTTON_UNDESKTOP,		IDS_TOOLTIP_UNDESKTOP,		IDS_TOOLBAR_TEXT_UNDESKTOP);
	AddToolBarSeparator();
	AddToolBarButton(IDC_BUTTON_WALLBROWSER,	IDS_TOOLTIP_WALLBROWSER,		IDS_TOOLBAR_TEXT_BROWSER);
	AddToolBarButton(IDC_BUTTON_CRAWLER,		IDS_TOOLTIP_CRAWLER,		IDS_TOOLBAR_TEXT_CRAWLER);
	AttachToolBar(sizeButton,sizeBitmap,(UINT)-1L,IDB_TOOLBAR_COLD,IDB_TOOLBAR_HOT);

	// crea il controllo per la playlist, il caricamento avviene piu' sotto tramite un thread
	if(!CreatePlaylistCtrl(m_pCmdLine->HaveDplFile() ? m_pCmdLine->GetDplFile() : NULL))
	{
		::MessageBoxResource(this->GetSafeHwnd(),MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_CREATELIST);
		EndDialog(IDCANCEL);
		return(FALSE);
	}

	// barra di stato (3 riquadri)
	AddStatusBarPanel(100);
	AddStatusBarPanel(250);
	AddStatusBarPanel(-1);
	AttachStatusBar();

	// tooltip
	if(m_Config.GetNumber(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_TOOLTIP_MAIN_KEY))
	{
		char szTooltipText[1024];
		::FormatResourceStringEx(szTooltipText,
							sizeof(szTooltipText)-1,
							IDS_TOOLTIP_EX_PLAYLIST,
							WALLPAPER_WEB_SITE,
							IDI_ICON_WALLPAPER,
							WALLPAPER_WEB_SITE,
							WALLPAPER_WEB_SITE,
							IDI_ICON16X16_CLOSE
							);
		char szCss[512];
		::FormatResourceString(szCss,sizeof(szCss),IDS_TOOLTIP_CSS_STYLE);
		AddExtendedToolTip(IDC_LIST_PLAYLIST,szTooltipText,szCss);
		CPPToolTip* pTooltip = GetExtendedToolTipCtrl();
		pTooltip->SetCallbackHyperlink(this->GetSafeHwnd(),WM_TOOLTIP_CALLBACK);
	}
	else
		AddToolTip(IDC_LIST_PLAYLIST,IDS_TOOLTIP_PLAYLIST);
	
	AddToolTip(IDC_CHECK_PICTURE_PREVIEW,IDS_TOOLTIP_PREVIEW);
	AddToolTip(IDC_EDIT_TIMEOUT,IDS_TOOLTIP_TIMEOUT);
	AddToolTip(IDC_SPIN_TIMEOUT,IDS_TOOLTIP_TIMEOUT);

#ifdef _RESIZABLE_DIALOG_STYLE
	// aggiunge le ancore per il riposizionamento degli oggetti durante il cambio di dimensione
	// la toolbar e la statusbar devono gia' essere stare create affinche la AddAnchor() riceva
	// un identifiatore valido per la finestra (dato che sia la toolbar che la statusbar vengono
	// create dinamicamente a partire dall'id specificato)
	if(IsDialogResizeEnabled())
	{
		AddAnchor(IDC_TOOL_BAR,TOP_LEFT,TOP_RIGHT);
		AddAnchor(IDC_LIST_PLAYLIST,TOP_LEFT,BOTTOM_RIGHT);
		AddAnchor(IDC_CHECK_PICTURE_PREVIEW,BOTTOM_LEFT);
		AddAnchor(IDC_SPIN_TIMEOUT,BOTTOM_RIGHT);
		AddAnchor(IDC_EDIT_TIMEOUT,BOTTOM_RIGHT);
		AddAnchor(IDC_STATIC_TIMEOUT,BOTTOM_RIGHT);
		AddAnchor(IDC_STATUS_BAR,BOTTOM_LEFT,BOTTOM_RIGHT);
		// la toolbar sfora durante il ridimensionamento, quindi aggiunge gli id dei controlli da
		// misurare per sapere quando nasconderla/visualizzarla
		// in realta' e' necessario specificarli solo se il ridimensionamento non prevede un minimo
		AddId(IDC_EDIT_TIMEOUT);
		AddId(-3);
	}
#endif

	// imposta gli hotkeys
	if(GetInstanceCount()==1)
	{
		DWORD dwErr = 0L;
		if(!RegisterHotKey(this->m_hWnd,HOTKEY_CTRL_ALT_MULTIPLY,MOD_CONTROL|MOD_ALT,VK_MULTIPLY))
			dwErr = GetLastError();
		if(!RegisterHotKey(this->m_hWnd,HOTKEY_CTRL_ALT_DIVIDE,MOD_CONTROL|MOD_ALT,VK_DIVIDE))
			dwErr = GetLastError();
		if(!RegisterHotKey(this->m_hWnd,HOTKEY_CTRL_ALT_UP,MOD_CONTROL|MOD_ALT,VK_UP))
			dwErr = GetLastError();
		if(!RegisterHotKey(this->m_hWnd,HOTKEY_CTRL_ALT_DOWN,MOD_CONTROL|MOD_ALT,VK_DOWN))
			dwErr = GetLastError();
		if(!RegisterHotKey(this->m_hWnd,HOTKEY_CTRL_ALT_LEFT,MOD_CONTROL|MOD_ALT,VK_LEFT))
			dwErr = GetLastError();
		if(!RegisterHotKey(this->m_hWnd,HOTKEY_CTRL_ALT_RIGHT,MOD_CONTROL|MOD_ALT,VK_RIGHT))
			dwErr = GetLastError();
		if(!RegisterHotKey(this->m_hWnd,HOTKEY_CTRL_ALT_BACKSPACE,MOD_CONTROL|MOD_ALT,VK_BACK))
			dwErr = GetLastError();
		if(!RegisterHotKey(this->m_hWnd,HOTKEY_CTRL_ALT_SPACE,MOD_CONTROL|MOD_ALT,VK_SPACE))
			dwErr = GetLastError();
		if(!RegisterHotKey(this->m_hWnd,HOTKEY_CTRL_ALT_ADD,MOD_CONTROL|MOD_ALT,VK_ADD))
			dwErr = GetLastError();
		if(!RegisterHotKey(this->m_hWnd,HOTKEY_CTRL_ALT_SUBTRACT,MOD_CONTROL|MOD_ALT,VK_SUBTRACT))
			dwErr = GetLastError();
		if(dwErr!=0L)
			::MessageBoxResourceEx(this->GetSafeHwnd(),MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_HOTKEYS,dwErr,(char*)::GetLastErrorString(dwErr));
	}
		
	// acceleratori in combinazione con Alt
	SetAltAcceleratorKeys("SLRMVDUBCK");

	// abilita il drag and drop via OLE, la classe base (CDialogEx) gestisce solo il drag and drop tramite la shell (files)
	if(m_oleDropTarget.Attach(this,WM_DROPOLE))
	{
		m_nNetscapeClipboardFormat = m_oleDropTarget.AddClipboardFormat(NETSCAPE_CLIPBOARDFORMAT_NAME);
		m_nExplorerClipboardFormat = m_oleDropTarget.AddClipboardFormat(IEXPLORER_CLIPBOARDFORMAT_NAME);
	}

	// inizializzazione estesa
	OnInitDialogEx();

	// abilita le istanze multiple solo per la gestione dei parametri della linea di comando (chiamato dalla shell)
	if(GetInstanceCount()!=1 || m_bMustTerminate)
	{
		EndDialog(IDCANCEL);
		return(FALSE);
	}

	// inizializza e registra l'applicazione
	if(!InitializeApplication())
	{
		::MessageBoxResource(this->GetSafeHwnd(),MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_REGISTER_APPLICATION);
		EndDialog(IDCANCEL);
		return(FALSE);
	}

	ApplySettings(TRUE);

	return(TRUE);
}

/*
	OnInitDialogEx()

	Inizializzazione estesa del dialogo.
	Viene chiamata esplicitamente dalla OnInitDialog() e dal gestore della classe base se sono permesse piu' istanze
	(vedi il parametro da passare nel ctor).
	Se non vengono passati parametri si tratta di una chiamata interna, altrimenti e' stato il gestore della classe
	base, per cui in input si ritrova la linea di comando della nuova istanza.
	Restituire TRUE per permettere l'esecuzione della nuova istanza, FALSE per impedirla. In questo caso abilita la
	gestione delle istanze multiple solo per farsi carico dei parametri della nuova istanza (le chiamate della shell),
	una volta caricati i parametri restituisce sempre FALSE (solo un istanza alla volta).
*/
BOOL CWallPaperDlg::OnInitDialogEx(UINT nInstanceCount/*=(UINT)-1*/,LPCSTR lpcszCommandLine/*=NULL*/)
{
	// se TRUE la chiamata proviene da una nuova istanza
	// se FALSE la chiamata e' interna (e' l'unica istanza in corso)
	BOOL bCalledFromNewInstance = (nInstanceCount!=(UINT)-1);
	BOOL bHaveOneInstance = GetInstanceCount()==1;

	// puo' venire sovrascritto durante la gestione dei parametri
	char szCurrentDplFile[_MAX_FILEPATH+1] = {0};
	if(m_pCmdLine->HaveDplFile())
		strcpyn(szCurrentDplFile,m_pCmdLine->GetDplFile(),sizeof(szCurrentDplFile));

//-- linea di comando/parametri ---------------------------------------------------------------------------------------------------------------

	// ricarica la linea di comando se presente (potrebbe essere stata passata dalla nuova istanza)
	if(lpcszCommandLine)
	{
		m_pCmdLine->Reset();
		m_pCmdLine->GetCommandLineOptions(lpcszCommandLine);
	}

	// /i<pathname completo directory di installazione\nome eseguibile completo di estensione>
	// imposta i valori relativi all'installazione
	// - deve essere l'unica istanza in esecuzione
	// - terminata la procedura deve continuare normalmente (viene chiamato con tale parametro dall'installer al primo lancio)
	if(m_pCmdLine->HaveInstall())
	{
		// per eseguire l'installazione non devono esistere altre istanze
		
		// eseguito dall'istanza gia' in esecuzione
		if(bCalledFromNewInstance)
		{
			::MessageBoxResource(this->GetSafeHwnd(),MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_INST_INSTANCE);
			return(FALSE);
		}
		// eseguito dalla nuova istanza
		if(!bHaveOneInstance)
		{
			m_bMustTerminate = TRUE;
			return(FALSE);
		}

		// aggiorna la data dell'ultimo aggiornamento
		CDateTime dateTime;
		dateTime.SetDateFormat(GMT_SHORT);
		m_Config.UpdateString(WALLPAPER_INSTALL_KEY,WALLPAPER_LATESTVERSIONCHECK_KEY,dateTime.GetFormattedDate(TRUE));
		m_Config.SaveKey(WALLPAPER_INSTALL_KEY,WALLPAPER_LATESTVERSIONCHECK_KEY);

		// aggiorna i valori relativi al sito web/email
		m_Config.UpdateString(WALLPAPER_INSTALL_KEY,WALLPAPER_WEBSITE_KEY,DEFAULT_WEBSITE);
		m_Config.SaveKey(WALLPAPER_INSTALL_KEY,WALLPAPER_WEBSITE_KEY);
		m_Config.UpdateString(WALLPAPER_INSTALL_KEY,WALLPAPER_EMAILADDRESS_KEY,DEFAULT_EMAILADDRESS);
		m_Config.SaveKey(WALLPAPER_INSTALL_KEY,WALLPAPER_EMAILADDRESS_KEY);
		m_Config.UpdateString(WALLPAPER_INSTALL_KEY,WALLPAPER_AUTHOREMAIL_KEY,DEFAULT_AUTHOREMAIL);
		m_Config.SaveKey(WALLPAPER_INSTALL_KEY,WALLPAPER_AUTHOREMAIL_KEY);

		// aggiorna il nome del programma (completo di pathname)
		m_Config.UpdateString(WALLPAPER_INSTALL_KEY,WALLPAPER_PROGRAM_KEY,m_pCmdLine->GetInstallDir());
		m_Config.SaveKey(WALLPAPER_INSTALL_KEY,WALLPAPER_PROGRAM_KEY);
		
		// per i file che devono essere estratti (aggiornati) comunque in fase di installazione
		char szFileName[_MAX_FILEPATH+1];
		_snprintf(szFileName,sizeof(szFileName)-1,"%s%s",m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_AUDIOPOPUPDIR_KEY),"audiopopupmp3.png");
		::DeleteFile(szFileName);
		_snprintf(szFileName,sizeof(szFileName)-1,"%s%s",m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_AUDIOPOPUPDIR_KEY),"audiopopupmp3.ini");
		::DeleteFile(szFileName);
		_snprintf(szFileName,sizeof(szFileName)-1,"%s%s",m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_AUDIOPOPUPDIR_KEY),"audiopopupwav.png");
		::DeleteFile(szFileName);
		_snprintf(szFileName,sizeof(szFileName)-1,"%s%s",m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_AUDIOPOPUPDIR_KEY),"audiopopupwav.ini");
		::DeleteFile(szFileName);
		_snprintf(szFileName,sizeof(szFileName)-1,"%s%s",m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_AUDIOPOPUPDIR_KEY),"audiopopupcda.png");
		::DeleteFile(szFileName);
		_snprintf(szFileName,sizeof(szFileName)-1,"%s%s",m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_AUDIOPOPUPDIR_KEY),"audiopopupcda.ini");
		::DeleteFile(szFileName);
		_snprintf(szFileName,sizeof(szFileName)-1,"%s%s",m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_PICTPOPUPDIR_KEY),"pcpopup.png");
		::DeleteFile(szFileName);
		_snprintf(szFileName,sizeof(szFileName)-1,"%s%s",m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_PICTPOPUPDIR_KEY),"pcpopup.ini");
		::DeleteFile(szFileName);
		_snprintf(szFileName,sizeof(szFileName)-1,"%s%s",m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_PICTPOPUPDIR_KEY),"macpopup.png");
		::DeleteFile(szFileName);
		_snprintf(szFileName,sizeof(szFileName)-1,"%s%s",m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_PICTPOPUPDIR_KEY),"macpopup.ini");
		::DeleteFile(szFileName);
		
		// aggiorna la directory base (con '\' finale)
		char szPathName[_MAX_FILEPATH+1];
		strcpyn(szPathName,m_pCmdLine->GetInstallDir(),sizeof(szPathName));
		char* p = strrchr(szPathName,'\\');
		if(p && *(p+1))
			*(p+1) = '\0';
		m_Config.UpdateString(WALLPAPER_INSTALL_KEY,WALLPAPER_DIR_KEY,szPathName);
		m_Config.SaveKey(WALLPAPER_INSTALL_KEY,WALLPAPER_DIR_KEY);

		// aggiorna le directories rimanenti
		_snprintf(szPathName,sizeof(szPathName)-1,"%s%s\\",m_Config.GetString(WALLPAPER_INSTALL_KEY,WALLPAPER_DIR_KEY),DEFAULT_BITMAPSDIR_NAME);
		m_Config.UpdateString(WALLPAPER_OPTIONS_KEY,WALLPAPER_BITMAPSDIR_KEY,szPathName);
		_snprintf(szPathName,sizeof(szPathName)-1,"%s%s\\",m_Config.GetString(WALLPAPER_INSTALL_KEY,WALLPAPER_DIR_KEY),DEFAULT_AUDIOPOPUPDIR_NAME);
		m_Config.UpdateString(WALLPAPER_OPTIONS_KEY,WALLPAPER_AUDIOPOPUPDIR_KEY,szPathName);
		_snprintf(szPathName,sizeof(szPathName)-1,"%s%s\\",m_Config.GetString(WALLPAPER_INSTALL_KEY,WALLPAPER_DIR_KEY),DEFAULT_PICTPOPUPDIR_NAME);
		m_Config.UpdateString(WALLPAPER_OPTIONS_KEY,WALLPAPER_PICTPOPUPDIR_KEY,szPathName);
		_snprintf(szPathName,sizeof(szPathName)-1,"%s%s\\",m_Config.GetString(WALLPAPER_INSTALL_KEY,WALLPAPER_DIR_KEY),DEFAULT_DATADIR_NAME);
		m_Config.UpdateString(WALLPAPER_OPTIONS_KEY,WALLPAPER_DATADIR_KEY,szPathName);
		_snprintf(szPathName,sizeof(szPathName)-1,"%s%s\\",m_Config.GetString(WALLPAPER_INSTALL_KEY,WALLPAPER_DIR_KEY),DEFAULT_DOWNLOADDIR_NAME);
		m_Config.UpdateString(WALLPAPER_OPTIONS_KEY,WALLPAPER_DOWNLOADDIR_KEY,szPathName);
		_snprintf(szPathName,sizeof(szPathName)-1,"%s%s\\",m_Config.GetString(WALLPAPER_INSTALL_KEY,WALLPAPER_DIR_KEY),DEFAULT_REPORTSDIR_NAME);
		m_Config.UpdateString(WALLPAPER_OPTIONS_KEY,WALLPAPER_REPORTSDIR_KEY,szPathName);
		_snprintf(szPathName,sizeof(szPathName)-1,"%s%s\\",m_Config.GetString(WALLPAPER_INSTALL_KEY,WALLPAPER_DIR_KEY),DEFAULT_TEMPDIR_NAME);
		m_Config.UpdateString(WALLPAPER_OPTIONS_KEY,WALLPAPER_TEMPDIR_KEY,szPathName);
		_snprintf(szPathName,sizeof(szPathName)-1,"%s%s\\",m_Config.GetString(WALLPAPER_INSTALL_KEY,WALLPAPER_DIR_KEY),DEFAULT_THUMBNAILSDIR_NAME);
		m_Config.UpdateString(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILSDIR_KEY,szPathName);

		// imposta i nomi files di default
		_snprintf(szPathName,sizeof(szPathName)-1,"%s%s\\%s",m_Config.GetString(WALLPAPER_INSTALL_KEY,WALLPAPER_DIR_KEY),DEFAULT_DOWNLOADDIR_NAME,DEFAULT_COOKIES_FILE_NAME);
		m_Config.UpdateString(WALLPAPER_OPTIONS_KEY,WALLPAPER_COOKIES_FILE_KEY,szPathName);
		_snprintf(szPathName,sizeof(szPathName)-1,"%s%s\\%s",m_Config.GetString(WALLPAPER_INSTALL_KEY,WALLPAPER_DIR_KEY),DEFAULT_THUMBNAILSDIR_NAME,DEFAULT_THUMBNAILS_HTMLFILE_NAME);
		m_Config.UpdateString(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_HTMLFILE_KEY,szPathName);
		m_Config.SaveSection(WALLPAPER_OPTIONS_KEY);

		BOOL bSuccess = FALSE;
		char szDesktop[_MAX_FILEPATH+1] = {0};
		char szPrograms[_MAX_FILEPATH+1] = {0};
		char szBuffer[1024];
		ITEMIDLIST* id = (ITEMIDLIST*)NULL;

		strcpyn(szPathName,m_Config.GetString(WALLPAPER_INSTALL_KEY,WALLPAPER_DIR_KEY),sizeof(szPathName));
		::RemoveBackslash(szPathName);

		// shortcuts
		bSuccess = FALSE;
		if(!bSuccess)
			if(::SHGetSpecialFolderLocation(NULL,CSIDL_DESKTOPDIRECTORY,&id)==NOERROR)
				bSuccess = TRUE;
		if(!bSuccess)
			if(::SHGetSpecialFolderLocation(NULL,CSIDL_DESKTOP,&id)==NOERROR)
				bSuccess = TRUE;
		if(bSuccess)
		{
			bSuccess = FALSE;

			if(::SHGetPathFromIDList(id,&szDesktop[0]))
			{
				char szLink[_MAX_FILEPATH+1];
				_snprintf(szLink,sizeof(szLink)-1,"%s\\%s.lnk",szDesktop,WALLPAPER_PROGRAM_NAME);
				if(!::FileExist(szLink))
				{
					if(::MessageBoxResource(this->GetSafeHwnd(),MB_YESNO|MB_ICONQUESTION,WALLPAPER_PROGRAM_NAME,IDS_QUESTION_CREATE_SHORTCUTS)==IDYES)
					{
						bSuccess = FALSE;
						
						if(::CreateShortcut(m_pCmdLine->GetInstallDir(),NULL,WALLPAPER_PROGRAM_NAME,szDesktop,szPathName,0))
							bSuccess = TRUE;
						
						/*char szInstallDir[_MAX_FILEPATH+1];
						strcpyn(szInstallDir,m_pCmdLine->GetInstallDir(),sizeof(szInstallDir));
						char* p = strrchr(szInstallDir,'\\');
						if(p && *(p+1))
						{
							*(p+1) = '\0';
							strcatn(szInstallDir,WALLBROWSER_PROGRAM_NAME,sizeof(szInstallDir));
							strcatn(szInstallDir,".exe",sizeof(szInstallDir));
							bSuccess = FALSE;
							if(::CreateShortcut(szInstallDir,NULL,WALLBROWSER_PROGRAM_NAME,szDesktop,szPathName,0))
								bSuccess = TRUE;
						}*/
					}
					else
						bSuccess = TRUE;
				}
				else
					bSuccess = TRUE;
			}
		}
		
		if(!bSuccess)
			::MessageBoxResource(this->GetSafeHwnd(),MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_DESKTOP_SHORTCUT);

		// start menu
		bSuccess = FALSE;
		if(!bSuccess)
			if(::SHGetSpecialFolderLocation(NULL,CSIDL_COMMON_PROGRAMS,&id)==NOERROR)
				if(::SHGetPathFromIDList(id,&szPrograms[0]))
				{
					bSuccess = TRUE;
					_snprintf(szBuffer,sizeof(szBuffer)-1,"%s\\%s - %s",szPrograms,WALLPAPER_PROGRAM_NAME,WALLPAPER_PROGRAM_DESCRIPTION);
					strcpyn(szPrograms,szBuffer,sizeof(szPrograms));
				}
		if(!bSuccess)
			if(::SHGetSpecialFolderLocation(NULL,CSIDL_PROGRAMS,&id)==NOERROR)
				if(::SHGetPathFromIDList(id,&szPrograms[0]))
				{
					bSuccess = TRUE;
					_snprintf(szBuffer,sizeof(szBuffer)-1,"%s\\%s - %s",szPrograms,WALLPAPER_PROGRAM_NAME,WALLPAPER_PROGRAM_DESCRIPTION);
					strcpyn(szPrograms,szBuffer,sizeof(szPrograms));
				}

		if(bSuccess)
		{
			bSuccess = FALSE;
			
			BOOL bExist = TRUE;
			char szLink[_MAX_FILEPATH+1];
			_snprintf(szLink,sizeof(szLink)-1,"%s\\%s.lnk",szPrograms,WALLPAPER_PROGRAM_NAME);
			if(!::FileExist(szLink))
				bExist = FALSE;
			/*_snprintf(szLink,sizeof(szLink)-1,"%s\\%s.lnk",szPrograms,WALLBROWSER_PROGRAM_NAME);
			if(!::FileExist(szLink))
				bExist = FALSE;*/

			if(!bExist)
			{
				bSuccess = TRUE;
				if(::MessageBoxResource(this->GetSafeHwnd(),MB_YESNO|MB_ICONQUESTION,WALLPAPER_PROGRAM_NAME,IDS_QUESTION_CREATE_MENUENTRIES)==IDYES)
				{
					::CreateDirectory(szBuffer,NULL);

					// WallPaper
					_snprintf(szBuffer,sizeof(szBuffer)-1,"%s\\%s.exe",szPathName,WALLPAPER_PROGRAM_NAME);
					if(!::CreateShortcut(szBuffer,NULL,WALLPAPER_PROGRAM_NAME,szPrograms,szPathName,0))
						::MessageBoxResource(this->GetSafeHwnd(),MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_MENU_SHORTCUT);

					// WallBrowser
					/*_snprintf(szBuffer,sizeof(szBuffer)-1,"%s\\%s.exe",szPathName,WALLBROWSER_PROGRAM_NAME);
					if(!::CreateShortcut(szBuffer,NULL,WALLBROWSER_PROGRAM_NAME,szPrograms,szPathName,0))
						::MessageBoxResource(this->GetSafeHwnd(),MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_MENU_SHORTCUT);*/
				}
			}
			else
				bSuccess = TRUE;
		}

		if(!bSuccess)
			::MessageBoxResource(this->GetSafeHwnd(),MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_MENU_ENTRY);

		// copia l'eseguibile per la disinstallazione nella directory di Windows
		char szUninstallProgram[_MAX_FILEPATH+1];
		char szUninstallPath[_MAX_FILEPATH+1];
		_snprintf(szUninstallProgram,sizeof(szUninstallProgram)-1,"%s\\WallPaperUni.exe",szPathName);
		::GetWindowsDirectory(szUninstallPath,sizeof(szUninstallPath));
		::EnsureBackslash(szUninstallPath,sizeof(szUninstallPath));
		strcatn(szUninstallPath,"WallPaperUni.exe",sizeof(szUninstallPath));
		::CopyFile(szUninstallProgram,szUninstallPath,FALSE);

		// crea le chiavi per il programma di disinstallazione (entrata in Add/Remove Program)
		char key[REGKEY_MAX_KEY_NAME+1];
		char value[REGKEY_MAX_KEY_VALUE+1];
		CRegKey regkey;
		LONG reg;
		memset(key,'\0',sizeof(key));
		regkey.Attach(HKEY_LOCAL_MACHINE);
		_snprintf(key,sizeof(key)-1,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\%s",WALLPAPER_PROGRAM_NAME);
		if((reg = regkey.Open(HKEY_LOCAL_MACHINE,key))!=ERROR_SUCCESS)
			reg = regkey.Create(HKEY_LOCAL_MACHINE,key);
		if(reg==ERROR_SUCCESS)
		{
			// descrizione/programma
			_snprintf(value,sizeof(value)-1,"%s,0",m_pCmdLine->GetInstallDir());
			regkey.SetValue(value,"DisplayIcon");
			regkey.SetValue(WALLPAPER_PROGRAM_NAME" - "WALLPAPER_PROGRAM_DESCRIPTION,"DisplayName");
			_snprintf(value,sizeof(value)-1,"%s /u",m_pCmdLine->GetInstallDir());
			regkey.SetValue(value,"UninstallString");
			regkey.Close();
		}
		regkey.Detach();
		
		// per evitare il passaggio di piu' parametri alla volta
		return(FALSE);
	}

	// /u
	// rimuove il programma (disinstallazione)
	// - deve essere l'unica istanza in esecuzione
	// - terminata la procedura deve uscire
	if(m_pCmdLine->HaveUninstall())
	{
		// per eseguire la disistallazione non devono esistere altre istanze

		// eseguito dall'istanza gia' in esecuzione
		if(bCalledFromNewInstance)
		{
			::MessageBoxResource(this->GetSafeHwnd(),MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_UNINST_INSTANCE);
			return(FALSE);
		}
		// eseguito dalla nuova istanza
		if(!bHaveOneInstance)
		{
			m_bMustTerminate = TRUE;
			return(FALSE);
		}

		// chiede conferma
		if(::MessageBoxResource(this->GetSafeHwnd(),MB_YESNO|MB_ICONQUESTION,WALLPAPER_PROGRAM_NAME,IDS_QUESTION_UNINSTALL)==IDYES)
		{
			// elimina il lancio automatico del programma
			m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_AUTORUN_KEY,FALSE);
			m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_AUTORUN_KEY);
			m_Config.RegistryDeleteValue("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",WALLPAPER_PROGRAM_NAME);

			// elimina l'entrata nel menu contestuale della shell
			m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CONTEXT_MENU_KEY,FALSE);
			m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_CONTEXT_MENU_KEY);
			OnContextMenu();

			// elimina le registrazioni relative alle estensioni gestite
			CRegistry registry;
			registry.UnregisterFileType(GZW_EXTENSION);
			registry.UnregisterFileType(DPL_EXTENSION);

			// elimina la chiave del programma dal registro
			CRegKey regkey;
			regkey.Attach(HKEY_CURRENT_USER);
			if(regkey.Open(HKEY_CURRENT_USER,"SOFTWARE")==ERROR_SUCCESS)
			{
				regkey.DeleteKey(WALLPAPER_PROGRAM_NAME);
				regkey.DeleteKey(WALLBROWSER_PROGRAM_NAME);
				regkey.Close();
			}
			regkey.Detach();

			// chiavi per uninstall
			regkey.Attach(HKEY_LOCAL_MACHINE);
			if(regkey.Open(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall")==ERROR_SUCCESS)
			{
				regkey.DeleteKey(WALLPAPER_PROGRAM_NAME);
				regkey.Close();
			}
			regkey.Detach();

			char szDesktop[_MAX_FILEPATH+1];
			char szPrograms[_MAX_FILEPATH+1];
			char szPath[_MAX_FILEPATH+1];
			ITEMIDLIST *id;

			// elimina lo shortcut sul desktop (WallPaper)
			::SHGetSpecialFolderLocation(NULL,CSIDL_DESKTOPDIRECTORY,&id); 
			::SHGetPathFromIDList(id,&szDesktop[0]);
			_snprintf(szPath,sizeof(szPath)-1,"%s\\%s.lnk",szDesktop,WALLPAPER_PROGRAM_NAME);
			::DeleteFile(szPath);
			
			// con CSIDL_DESKTOPDIRECTORY fallisce su Win95
			::SHGetSpecialFolderLocation(NULL,CSIDL_DESKTOP,&id); 
			::SHGetPathFromIDList(id,&szDesktop[0]);
			_snprintf(szPath,sizeof(szPath)-1,"%s\\%s.lnk",szDesktop,WALLPAPER_PROGRAM_NAME);
			::DeleteFile(szPath);

			// elimina lo shortcut sul desktop (WallBrowser)
			/*::SHGetSpecialFolderLocation(NULL,CSIDL_DESKTOPDIRECTORY,&id); 
			::SHGetPathFromIDList(id,&szDesktop[0]);
			_snprintf(szPath,sizeof(szPath)-1,"%s\\%s.lnk",szDesktop,WALLBROWSER_PROGRAM_NAME);
			::DeleteFile(szPath);*/

			// con CSIDL_DESKTOPDIRECTORY fallisce su Win95
			/*::SHGetSpecialFolderLocation(NULL,CSIDL_DESKTOP,&id); 
			::SHGetPathFromIDList(id,&szDesktop[0]);
			_snprintf(szPath,sizeof(szPath)-1,"%s\\%s.lnk",szDesktop,WALLBROWSER_PROGRAM_NAME);
			::DeleteFile(szPath);*/

			// elimina le entrate dal menu di sistema
			::SHGetSpecialFolderLocation(NULL,CSIDL_COMMON_PROGRAMS,&id);
			::SHGetPathFromIDList(id,&szPrograms[0]);
			_snprintf(szPath,sizeof(szPath)-1,"%s\\%s - %s",szPrograms,WALLPAPER_PROGRAM_NAME,WALLPAPER_PROGRAM_DESCRIPTION);
			strcpyn(szPrograms,szPath,sizeof(szPrograms));
			/*_snprintf(szPath,sizeof(szPath)-1,"%s\\%s.lnk",szPrograms,WALLBROWSER_PROGRAM_NAME);
			::DeleteFile(szPath);*/
			_snprintf(szPath,sizeof(szPath)-1,"%s\\%s.lnk",szPrograms,WALLPAPER_PROGRAM_NAME);
			::DeleteFile(szPath);
			/*_snprintf(szPath,sizeof(szPath)-1,"%s\\%s.lnk",szPrograms,"WebSite");
			::DeleteFile(szPath);*/
			::RemoveDirectory(szPrograms);
			
			// con CSIDL_COMMON_PROGRAMS fallisce su Win95
			::SHGetSpecialFolderLocation(NULL,CSIDL_PROGRAMS,&id);
			::SHGetPathFromIDList(id,&szPrograms[0]);
			_snprintf(szPath,sizeof(szPath)-1,"%s\\%s - %s",szPrograms,WALLPAPER_PROGRAM_NAME,WALLPAPER_PROGRAM_DESCRIPTION);
			strcpyn(szPrograms,szPath,sizeof(szPrograms));
			/*_snprintf(szPath,sizeof(szPath)-1,"%s\\%s.lnk",szPrograms,WALLBROWSER_PROGRAM_NAME);
			::DeleteFile(szPath);*/
			_snprintf(szPath,sizeof(szPath)-1,"%s\\%s.lnk",szPrograms,WALLPAPER_PROGRAM_NAME);
			::DeleteFile(szPath);
			/*_snprintf(szPath,sizeof(szPath)-1,"%s\\%s.lnk",szPrograms,"WebSite");
			::DeleteFile(szPath);*/
			::RemoveDirectory(szPrograms);

			// lancia l'uninstaller per eliminare la directory del programma
			char szInstallDir[_MAX_FILEPATH+1];
			strcpyn(szInstallDir,m_Config.GetString(WALLPAPER_INSTALL_KEY,WALLPAPER_DIR_KEY),sizeof(szInstallDir));
			::RemoveBackslash(szInstallDir);
			
			char szWindowsDir[_MAX_FILEPATH+1];
			::GetWindowsDirectory(szWindowsDir,sizeof(szWindowsDir));
			::RemoveBackslash(szWindowsDir);

			char szCmd[_MAX_FILEPATH+1];
			_snprintf(szCmd,sizeof(szCmd)-1,"%s\\%s.exe %s",szWindowsDir,WALLPAPERUNI_PROGRAM_NAME,szInstallDir);
			STARTUPINFO si = {0};
			si.cb = sizeof(STARTUPINFO);
			PROCESS_INFORMATION pi = {0};
			if(::CreateProcess(NULL,szCmd,NULL,NULL,FALSE,0L,NULL,NULL,&si,&pi))
				::CloseHandle(pi.hProcess);

			_snprintf(szCmd,sizeof(szCmd)-1,"%s\\%s.exe",szWindowsDir,WALLPAPERUNI_PROGRAM_NAME);
			::MoveFileEx(szCmd,NULL,MOVEFILE_DELAY_UNTIL_REBOOT);
		}

		// una volta terminata la disinstallazione deve terminare
		m_bMustTerminate = TRUE;
			
		// per evitare il passaggio di piu' parametri alla volta
		return(FALSE);
	}

	// /w<picture file>
	// carica l'immagine specificata come sfondo (chiamato dalla shell)
	// specificare sempre il pathname completo, anche se il file si trova nella directory corrente
	// - se e' l'unica istanza in corso (chiamata dalla shell), una volta impostato lo sfondo deve terminare
	// - se invece arriva qui per la ricezione dei parametri della nuova istanza, una volta impostato lo sfondo deve continuare
	if(m_pCmdLine->HavePictureFile())
	{
		if(m_pImage->IsSupportedFormat(m_pCmdLine->GetPictureFile()))
		{
			// eseguito dall'istanza gia' in esecuzione
			if(bCalledFromNewInstance)
			{
				// cambia lo sfondo con l'immagine specificata
				SetDesktopWallPaper(m_pCmdLine->GetPictureFile(),0,TRUE);
				m_pCmdLine->ResetPictureFile();

				// non deve eseguire il codice sottostante
				return(FALSE);
			}

			// eseguito dalla nuova istanza
			if(bHaveOneInstance)
			{
				// cambia lo sfondo con l'immagine specificata
				SetDesktopWallPaper(m_pCmdLine->GetPictureFile(),0,TRUE);
				m_pCmdLine->ResetPictureFile();
			}
			
			// una volta caricato lo sfondo deve terminare
			m_bMustTerminate = TRUE;
		}

		// per evitare il passaggio di piu' parametri alla volta
		return(FALSE);
	}
	
	// /a<audio file>
	// carica il file audio specificato (chiamato dalla shell)
	// specificare sempre il pathname completo, anche se il file si trova nella directory corrente
	if(m_pCmdLine->HaveAudioFile())
	{
		if(szCurrentDplFile[0]!='\0')
			m_pCmdLine->SetDplFile(szCurrentDplFile);

		if(CAudioPlayer::IsSupportedFormat(m_pCmdLine->GetAudioFile()))
		{
			// eseguito dall'istanza gia' in esecuzione
			if(bCalledFromNewInstance)
			{
				// riproduce il file audio
				::SendMessage(this->GetSafeHwnd(),WM_PLAYLIST_ADDFILE,AUDIOPLAYER_COMMAND_PLAY_FROM_FILE,(LONG)m_pCmdLine->GetAudioFile());

				// non deve eseguire il codice sottostante
				return(FALSE);
			}

			// eseguito dalla nuova istanza
			if(bHaveOneInstance)
			{
				// riproduce il file audio
				::SendMessage(this->GetSafeHwnd(),WM_PLAYLIST_ADDFILE,AUDIOPLAYER_COMMAND_PLAY_FROM_FILE,(LONG)m_pCmdLine->GetAudioFile());
			}

			// non deve terminare per poter portare a termine la riproduzione
			m_bMustTerminate = FALSE;
		}

		// per evitare il passaggio di piu' parametri alla volta
		return(FALSE);
	}
	
	// /A<audio file>
	// carica il file audio specificato (chiamato dalla shell)
	// specificare sempre il pathname completo, anche se il file si trova nella directory corrente
	if(m_pCmdLine->HaveQueuedAudioFile())
	{
		if(szCurrentDplFile[0]!='\0')
			m_pCmdLine->SetDplFile(szCurrentDplFile);

		if(CAudioPlayer::IsSupportedFormat(m_pCmdLine->GetQueuedAudioFile()))
		{
			// eseguito dall'istanza gia' in esecuzione
			if(bCalledFromNewInstance)
			{
				// riproduce il file audio
				::SendMessage(this->GetSafeHwnd(),WM_PLAYLIST_ADDFILE,AUDIOPLAYER_COMMAND_QUEUE_FROM_FILE,(LONG)m_pCmdLine->GetQueuedAudioFile());

				// non deve eseguire il codice sottostante
				return(FALSE);
			}

			// eseguito dalla nuova istanza
			if(bHaveOneInstance)
			{
				// riproduce il file audio
				::SendMessage(this->GetSafeHwnd(),WM_PLAYLIST_ADDFILE,AUDIOPLAYER_COMMAND_QUEUE_FROM_FILE,(LONG)m_pCmdLine->GetQueuedAudioFile());
			}

			// non deve terminare per poter portare a termine la riproduzione
			m_bMustTerminate = FALSE;
		}

		// per evitare il passaggio di piu' parametri alla volta
		return(FALSE);
	}

	// se non e' stato passato nessun .dpl ricava l'ultima modalita' attiva per sapere se caricare
	// la lista dal registro o dal file .dpl (i controlli per caricare la lista dal file impostano
	// la struttura come se il file fosse stato passato da linea di comando)
	if(!m_pCmdLine->HaveDplFile())
	{
		if(m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_LOADLISTFROM_KEY))
			m_pCmdLine->SetDplFile(m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_LISTNAME_KEY));
	}

	// /l<filename.dpl>
	// carica la lista (chiamato dalla shell con doppio click sul nome del file, tramite il drag and
	// drop del file o in base a quanto presente nella configurazione)
	// specificare sempre il pathname completo, anche se il file si trova nella directory corrente
	if(m_pCmdLine->HaveDplFile())
	{
		// controlla l'esistenza del file
		if(!::FileExist(m_pCmdLine->GetDplFile()))
		{
			::MessageBoxResourceEx(this->GetSafeHwnd(),MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_OPEN_FILE,m_pCmdLine->GetDplFile());
			m_pCmdLine->ResetDplFile();
			m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_LOADLISTFROM_KEY,FALSE);
			m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_LOADLISTFROM_KEY);
			m_Config.UpdateString(WALLPAPER_OPTIONS_KEY,WALLPAPER_LISTNAME_KEY,DEFAULT_LISTNAME);
			m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_LISTNAME_KEY);
		}

		// eseguito dall'istanza gia' in esecuzione
		if(bCalledFromNewInstance)
		{
			// carica la playlist
			::PostMessage(this->GetSafeHwnd(),WM_LOADPLAYLIST,0L,0L);
			PlaylistSetModified();

			// non deve eseguire il codice sottostante
			return(FALSE);
		}

		// eseguito dalla nuova istanza
		if(!bHaveOneInstance)
			m_bMustTerminate = TRUE;

		// per evitare il passaggio di piu' parametri alla volta
		return(FALSE);
	}

//-- fine linea di comando/parametri ----------------------------------------------------------------------------------------------------------

	return(FALSE);
}

/*
	ApplySettings()
*/
BOOL CWallPaperDlg::ApplySettings(BOOL bCalledFromOnInitDialog)
{
	// imposta il timer per il cambio dello sfondo
	if(!bCalledFromOnInitDialog)
		m_nDesktopTimeout = m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_TIMEOUT_KEY);
	
	// imposta il range dello spin button per il timeout
	CSpinButtonCtrl* pSpin = (CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_TIMEOUT);
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_TIMEOUT);
	if(pSpin && pEdit)
	{
		char szValue[8];
		sprintf(szValue,"%d",m_nDesktopTimeout);
		pEdit->SetWindowText(szValue);
		pSpin->SetBase(10);
		pSpin->SetRange(WALLPAPER_MIN_VALUE_FOR_CHANGE_TIMEOUT,WALLPAPER_MAX_VALUE_FOR_CHANGE_TIMEOUT);
		pSpin->SetPos(m_nDesktopTimeout);
		pSpin->SetBuddy((CWnd*)pEdit);
	}

	// imposta la modalita' visualizzazione per lo sfondo
	m_rcDrawRect.SetRect(0,0,m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_DRAWRECTX_KEY),m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_DRAWRECTY_KEY));

	// imposta il primo piano
	SetWindowPos(m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_ALWAYSONTOP_KEY) ? &wndTopMost : &wndNoTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);

	// configura il titolo a scorrimento
	OnNervousTitle(m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_INFAMOUS_SENTENCES_KEY));

	// imposta/rimuove il menu contestuale della shell per i tipi di files gestiti
	OnContextMenu();

	// imposta il livello di trasparenza per il dialogo
	BYTE nLayered = LWA_ALPHA_OPAQUE;
	if(m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAYERED_KEY))
	{
		nLayered = (BYTE)m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAYERED_VALUE_KEY);
		if(nLayered >= LWA_ALPHA_INVISIBLE && nLayered <= LWA_ALPHA_OPAQUE)
		{
			BOOL bMustSetLayer = FALSE;
			if(nLayered < LWA_ALPHA_INVISIBLE_THRESHOLD)
				bMustSetLayer = ::MessageBoxResourceEx(this->GetSafeHwnd(),MB_YESNO|MB_ICONQUESTION,WALLPAPER_PROGRAM_NAME,IDS_QUESTION_LAYERED,nLayered,LWA_ALPHA_INVISIBLE,LWA_ALPHA_OPAQUE)==IDYES;
			else
				bMustSetLayer = TRUE;
			if(bMustSetLayer)
				m_wndLayered.SetLayer(this->GetSafeHwnd(),nLayered);
		}
	}
	else
		m_wndLayered.Reset();
	
	// modalita' anteprima
	m_bPreviewMode = m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PICTUREPREVIEW_KEY);
	OnPreviewMode(FALSE);

	// icona per la system tray
	if(!bCalledFromOnInitDialog)
	{
		if(m_pTrayIcon)
		{
			m_pTrayIcon->Destroy();
			delete m_pTrayIcon,m_pTrayIcon = NULL;
		}
	}
	if(m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_TRAYICON_KEY))
	{
		if(m_pTrayIcon)
			delete m_pTrayIcon,m_pTrayIcon = NULL;
		
		m_pTrayIcon = new CTrayIcon();
		if(m_pTrayIcon)
		{
			if(m_pTrayIcon->Create(this->GetSafeHwnd(),(HICON)::LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICON_WALLPAPER),IMAGE_ICON,16,16,LR_DEFAULTCOLOR),WM_NOTIFYTASKBARICON,IDM_TRAY_MENU,IDI_ICON_BLANK,WALLPAPER_PROGRAM_TITLE))
			{
				char szString[512];
				::FormatResourceString(szString,sizeof(szString),IDS_TOOLTIP_CSS_STYLE);
				m_pTrayIcon->SetCssStyles(szString);
				
				m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_NERVOUS_TITLE_KEY,FALSE);
				m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_INFAMOUS_SENTENCES_KEY,FALSE);
				
				OnNervousTitle(FALSE);
			}
			else
				delete m_pTrayIcon,m_pTrayIcon = NULL;
		}
	}

	// popup sulla taskbar
	if(m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PICTPOPUP_KEY)==PICTPOPUP_TASKBAR)
		LoadTaskbarPopupList(&m_listPictTaskbarBitmaps,1);


	// sdraia la concorrenza se attiva
	BOOL bParam = FALSE;
	::SystemParametersInfo(SPI_GETSCREENSAVEACTIVE,0L,&bParam,TRUE);
	if(bParam)
		if(DoNotAskMoreMessageBox(this->GetSafeHwnd(),IDS_QUESTION_SCREENSAVER,DEFAULT_DONOTASKMORE_TIMEOUT,&m_Config,WALLPAPER_DONOTASKMORE_SCREENSAVER_KEY,WALLPAPER_DONOTASKMORE_SCREENSAVER_VALUE_KEY)==IDYES)
		{
			bParam = FALSE;
			::SystemParametersInfo(SPI_SETSCREENSAVEACTIVE,0L,&bParam,TRUE);
		}

	// forza la visualizzazione delle icone del desktop
	if(bCalledFromOnInitDialog)
		OnDesktopIcons();

	// imposta la dimensione/posizione della finestra
	RECT rect = {	m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_WINDOW_POS_LEFT),
				m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_WINDOW_POS_TOP),
				m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_WINDOW_POS_RIGHT),
				m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_WINDOW_POS_BOTTOM)
				};

	WINDOWPLACEMENT wp = {0};
	wp.length = sizeof(WINDOWPLACEMENT);
	wp.rcNormalPosition.left = rect.left;
	wp.rcNormalPosition.top = rect.top;
	wp.rcNormalPosition.right = rect.right;
	wp.rcNormalPosition.bottom = rect.bottom;
	SetWindowPlacement(&wp);

	// imposta lo stato iniziale della finestra principale
	if(bCalledFromOnInitDialog)
	{
		if(!m_pTrayIcon)
			SetVisible(TRUE);
		if(m_pTrayIcon)
			ShowWindow(SW_MINIMIZE);
		if(m_pTrayIcon)
			ShowWindow(SW_HIDE);
		if(!m_pTrayIcon)
			SetForegroundWindowEx(this->GetSafeHwnd());
	}
	else
	{
		SetVisible(TRUE);
		SetForegroundWindowEx(this->GetSafeHwnd());
	}

	// per le icone per il balloon
	// immagini
	m_findPictIcons.Reset();
	m_nCurrentPictIcon = -1;
	m_nPictIconsCount = 0;
	if(m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_BALLOON_PICT_ICON_TYPE_KEY)==BALLOON_ICON_CUSTOM)
	{
		char szFolder[_MAX_FILEPATH+1] = {0};
		strcpyn(szFolder,m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_BALLOON_PICT_ICON_DIR_KEY),sizeof(szFolder));
		if(!strnull(szFolder))
		{
			::EnsureBackslash(szFolder,sizeof(szFolder));
			m_nPictIconsCount = m_findPictIcons.FindFile(szFolder,"*.ico",m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_RECURSE_SUBDIR_KEY));
		}
	}
	// audio
	m_findAudioIcons.Reset();
	m_nCurrentAudioIcon = -1;
	m_nAudioIconsCount = 0;
	if(m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_BALLOON_AUDIO_ICON_TYPE_KEY)==BALLOON_ICON_CUSTOM)
	{
		char szFolder[_MAX_FILEPATH+1] = {0};
		strcpyn(szFolder,m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_BALLOON_AUDIO_ICON_DIR_KEY),sizeof(szFolder));
		if(!strnull(szFolder))
		{
			::EnsureBackslash(szFolder,sizeof(szFolder));
			m_nAudioIconsCount = m_findAudioIcons.FindFile(szFolder,"*.ico",m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_RECURSE_SUBDIR_KEY));
		}
	}

	// carica la playlist
	// il gestore del messaggio lancia il thread che a sua volta si occupa di lanciare il thread per il dialogo per l'anteprima
	::PostMessage(this->GetSafeHwnd(),WM_LOADPLAYLIST,0L,0L);
	
	return(TRUE);
}

/*
	InitializeApplication()
*/
BOOL CWallPaperDlg::InitializeApplication(void)
{
	// ripulisce la directory per i temporanei (utilizzata per quanto estratto dal file compresso)
	if(m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CLEAR_TEMPORARY_FILES_KEY)!=TEMPORARY_FILES_DO_NOT_CLEAR)
	{
		char* pFileName;
		CFindFile findFile;
		char szTempFiles[_MAX_FILEPATH+1];
		strcpyn(szTempFiles,m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_TEMPDIR_KEY),sizeof(szTempFiles));
		while((pFileName = (LPSTR)findFile.FindEx(szTempFiles,"*.*",TRUE))!=NULL)
			::DeleteFile(pFileName);
	}

	// controllo versione
	char szCurrentVersion[16] = {0};
	strcpyn(szCurrentVersion,m_Config.GetString(WALLPAPER_INSTALL_KEY,WALLPAPER_CURRENTVERSION_KEY),sizeof(szCurrentVersion));
	BOOL bExistPreviousVersion = strlen(szCurrentVersion) > 0;

	// si assicura che la voce del registro per il file utilizzato come sfondo contenga il nome di default utilizzato dal programma
	char szWallPaperDir[_MAX_FILEPATH+1];
	char szWallPaper[_MAX_FILEPATH+1];
	strcpyn(szWallPaperDir,m_Config.GetString(WALLPAPER_INSTALL_KEY,WALLPAPER_DIR_KEY),sizeof(szWallPaperDir));
	::EnsureBackslash(szWallPaperDir,sizeof(szWallPaperDir));
	_snprintf(szWallPaper,sizeof(szWallPaper)-1,"%s%s.bmp",szWallPaperDir,WALLPAPER_PROGRAM_NAME);
	EnableDesktopWallPaper(szWallPaper,WINDOWS_DRAWMODE_NORMAL);

	// si assicura che i tipi di file gestiti siano registrati
	RegisterApplication();

	// si assicura che esistano le directories necessarie
	CFindFile findFile;
	findFile.CreatePathName((char*)m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_BITMAPSDIR_KEY));
	findFile.CreatePathName((char*)m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_AUDIOPOPUPDIR_KEY));
	findFile.CreatePathName((char*)m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_PICTPOPUPDIR_KEY));
	findFile.CreatePathName((char*)m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_DATADIR_KEY));
	findFile.CreatePathName((char*)m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_DOWNLOADDIR_KEY));
	findFile.CreatePathName((char*)m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_REPORTSDIR_KEY));
	findFile.CreatePathName((char*)m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_TEMPDIR_KEY));
	findFile.CreatePathName((char*)m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILSDIR_KEY));

	// risorse da estrarre
	{
		int i;
		char szResource[_MAX_FILEPATH+1];
		char* gif[] = {
			"checked.gif",
			"file.gif",
			"filter.gif",
			"folder.gif",
			"invalidfile.gif",
			"invalidurl.gif",
			"netserver.gif",
			"network.gif",
			"picture.gif",
			"url.gif",
			"urldir.gif",
			"urlnotfound.gif",
			"wallpaper.gif",
			NULL
			};
		int gifid[] = {
			IDR_CHECKED_GIF,
			IDR_FILE_GIF,
			IDR_FILTER_GIF,
			IDR_FOLDER_GIF,
			IDR_INVALIDFILE_GIF,
			IDR_INVALIDURL_GIF,
			IDR_NETSERVER_GIF,
			IDR_NETWORK_GIF,
			IDR_PICTURE_GIF,
			IDR_URL_GIF,
			IDR_URLDIR_GIF,
			IDR_URLNOTFOUND_GIF,
			IDR_WALLPAPER_GIF,
			-1
			};
		char* audio_popup[] = {
			"audiopopupcda",
			"audiopopupmp3",
			"audiopopupwav",
			NULL
			};
		int audio_popupid[] = {
			IDR_AUDIOPOPUPCDA_PNG,
			IDR_AUDIOPOPUPMP3_PNG,
			IDR_AUDIOPOPUPWAV_PNG,
			-1
			};
		int audio_popupiniid[] = {
			IDR_AUDIOPOPUPCDA_INI,
			IDR_AUDIOPOPUPMP3_INI,
			IDR_AUDIOPOPUPWAV_INI,
			-1
			};
		char* pict_popup[] = {
			"pcpopup",
			"macpopup",
			NULL
			};
		int pict_popupid[] = {
			IDR_PICTPCPOPUP_PNG,
			IDR_PICTMACPOPUP_PNG,
			-1
			};
		int pict_popupiniid[] = {
			IDR_PICTPCPOPUP_INI,
			IDR_PICTMACPOPUP_INI,
			-1
			};

		// estrae le icone utilizzate dal crawler per la generazione del report html
		for(i = 0; gif[i]!=NULL && gifid[i] > 0; i++)
		{
			_snprintf(szResource,sizeof(szResource)-1,"%s%s",m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_BITMAPSDIR_KEY),gif[i]);
			if(bExistPreviousVersion ? !findFile.Exist(szResource) : TRUE)
				::ExtractResource(gifid[i],"GIF",szResource);
		}
		// estrae le risorse utilizzate dal crawler per la generazione del report html
		_snprintf(szResource,sizeof(szResource)-1,"%s%s",m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_REPORTSDIR_KEY),"pattern.gif");
		if(!findFile.Exist(szResource))
			::ExtractResource(IDR_PATTERN_GIF,"GIF",szResource);
		_snprintf(szResource,sizeof(szResource)-1,"%s%s",m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_REPORTSDIR_KEY),"style.css");
		if(!findFile.Exist(szResource))
			::ExtractResource(IDR_STYLE_REPORT_CSS,"TXT",szResource);
						
		// estrae i bitmaps/.ini per i popups audio se non esistono o se in installazione
		for(i = 0; audio_popup[i]!=NULL && audio_popupid[i] > 0; i++)
		{
			_snprintf(szResource,sizeof(szResource)-1,"%s%s.png",m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_AUDIOPOPUPDIR_KEY),audio_popup[i]);
			if(bExistPreviousVersion ? !findFile.Exist(szResource) : TRUE)
				::ExtractResource(audio_popupid[i],"GIF",szResource);
			_snprintf(szResource,sizeof(szResource)-1,"%s%s.ini",m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_AUDIOPOPUPDIR_KEY),audio_popup[i]);
			if(bExistPreviousVersion ? !findFile.Exist(szResource) : TRUE)
				::ExtractResource(audio_popupiniid[i],"TXT",szResource);
		}
						
		// estrae i bitmaps/.ini per i popups per le immagini se non esistono o se in installazione
		for(i = 0; pict_popup[i]!=NULL && pict_popupid[i] > 0; i++)
		{
			_snprintf(szResource,sizeof(szResource)-1,"%s%s.png",m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_PICTPOPUPDIR_KEY),pict_popup[i]);
			if(bExistPreviousVersion ? !findFile.Exist(szResource) : TRUE)
				::ExtractResource(pict_popupid[i],"GIF",szResource);
			_snprintf(szResource,sizeof(szResource)-1,"%s%s.ini",m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_PICTPOPUPDIR_KEY),pict_popup[i]);
			if(bExistPreviousVersion ? !findFile.Exist(szResource) : TRUE)
				::ExtractResource(pict_popupiniid[i],"TXT",szResource);
		}
	}

	// controllo versione
	if(bExistPreviousVersion)
	{		
		// per aggiornamento su versioni precedenti alla 3.8.3
		char szPath[_MAX_FILEPATH+1];
		strcpyn(szPath,m_Config.GetString(WALLPAPER_INSTALL_KEY,WALLPAPER_PROGRAM_KEY),sizeof(szPath));
		if(findFile.Exist(szPath) && strlen(szCurrentVersion) > 0 && szCurrentVersion[0]-'0' <= 3 && szCurrentVersion[2]-'0' <= 8 && szCurrentVersion[4]-'0' <= 3)
		{
			::MessageBoxResource(this->GetSafeHwnd(),MB_OK|MB_ICONINFORMATION,WALLPAPER_PROGRAM_NAME,IDS_MESSAGE_VERSION_UPDATE);
			
			char* pFileName;
			strcpyn(szPath,m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_DATADIR_KEY),sizeof(szPath));
			::EnsureBackslash(szPath,sizeof(szPath));
			while((pFileName = (char*)findFile.FindEx(szPath,"*.def",FALSE))!=NULL)
				::DeleteFileToRecycleBin(this->GetSafeHwnd(),pFileName,FALSE,TRUE);
			while((pFileName = (char*)findFile.FindEx(szPath,"*.db",FALSE))!=NULL)
				::DeleteFileToRecycleBin(this->GetSafeHwnd(),pFileName,FALSE,TRUE);
			while((pFileName = (char*)findFile.FindEx(szPath,"*.idx",FALSE))!=NULL)
				::DeleteFileToRecycleBin(this->GetSafeHwnd(),pFileName,FALSE,TRUE);
		}
	}

	m_Config.UpdateString(WALLPAPER_INSTALL_KEY,WALLPAPER_CURRENTVERSION_KEY,WALLPAPER_VERSION_NUMBER);
	m_Config.SaveKey(WALLPAPER_INSTALL_KEY,WALLPAPER_CURRENTVERSION_KEY);
	
	// controllo ultima versione
	{
		// 1965 = mai, 0 = allo startup, >0 = ogni n giorni
		DWORD dwLatestVersionCheckType = m_Config.GetNumber(WALLPAPER_INSTALL_KEY,WALLPAPER_LATESTVERSIONCHECKTYPE_KEY);
		if(dwLatestVersionCheckType==1965L)
		{
			;
		}
		else if(dwLatestVersionCheckType==0L)
		{
			// controllo versione via thread in auto...
			OnLatestVersion(FALSE,0L);
		}
		else if(dwLatestVersionCheckType > 0L)
		{
			CDateTime dateTime;
			dateTime.GetFormattedDate(TRUE);
			if(strnull(m_Config.GetString(WALLPAPER_INSTALL_KEY,WALLPAPER_LATESTVERSIONCHECK_KEY)))
			{
				dateTime.SetDateFormat(GMT_SHORT);
				m_Config.UpdateString(WALLPAPER_INSTALL_KEY,WALLPAPER_LATESTVERSIONCHECK_KEY,dateTime.GetFormattedDate(FALSE));
				m_Config.SaveKey(WALLPAPER_INSTALL_KEY,WALLPAPER_LATESTVERSIONCHECK_KEY);
			}
			if(!strnull(m_Config.GetString(WALLPAPER_INSTALL_KEY,WALLPAPER_LATESTVERSIONCHECK_KEY)))
			{
				CDateTime checkDateTime(m_Config.GetString(WALLPAPER_INSTALL_KEY,WALLPAPER_LATESTVERSIONCHECK_KEY),GMT_SHORT);
				int nDiff = dateTime.GetJulianDateDiff(dateTime,checkDateTime);
				if(nDiff >= (int)dwLatestVersionCheckType)
				{	
					// chiede conferma per controllare se e' disponibile una nuova versione
					if(DoNotAskMoreMessageBox(this->GetSafeHwnd(),IDS_QUESTION_LATESTVERSIONCHECK,DEFAULT_DONOTASKMORE_TIMEOUT,&m_Config,WALLPAPER_DONOTASKMORE_LATESTVERSION_KEY,WALLPAPER_DONOTASKMORE_LATESTVERSION_VALUE_KEY,1)==IDYES)
						OnLatestVersion(FALSE,0L);

					// aggiorna la data dell'ultimo aggiornamento
					dateTime.SetDateFormat(GMT_SHORT);
					m_Config.UpdateString(WALLPAPER_INSTALL_KEY,WALLPAPER_LATESTVERSIONCHECK_KEY,dateTime.GetFormattedDate(FALSE));
					m_Config.SaveKey(WALLPAPER_INSTALL_KEY,WALLPAPER_LATESTVERSIONCHECK_KEY);
				}
			}
		}
	}

	return(TRUE);
}

/*
	RegisterApplication()
*/
BOOL CWallPaperDlg::RegisterApplication(void)
{
	CRegistry registry;

	// registra il tipo per i .gzw (solo l'icona, non associa il tipo a nessun eseguibile)
	//if(registry.GetIconForRegisteredFileType(GZW_EXTENSION)==(HICON)NULL)
	{
		REGISTERFILETYPE gzwfiletype = {
			GZW_EXTENSION,					// estensione
			GZW_TYPENAME,					// nome del tipo
			GZW_TYPEDESC,					// descrizione del tipo
			"",							// shell
			"",							// argomenti per la shell
			2,							// icona di default per il tipo
			0,							// handle per l'icona di default
			GZW_CONTENTTYPE				// tipo mime
		};
		strcpyn(gzwfiletype.shell,m_Config.GetString(WALLPAPER_INSTALL_KEY,WALLPAPER_PROGRAM_KEY),sizeof(gzwfiletype.shell));
		registry.SetIconForRegisteredFileType(&gzwfiletype);
	}

	// registra il tipo per i .dpl
	REGISTERFILETYPE dplfiletype = {
		DPL_EXTENSION,		// estensione per il tipo file (.dpl)
		DPL_TYPENAME,		// nome del tipo (dplfile)
		DPL_TYPEDESC,		// descrizione del tipo (WallPaper Desktop Picture List)
		"",				// shell (C:\WallPaper\WallPaper.exe)
		DPL_SHELLARGS,		// argomenti per la shell (/l%1)
		3,				// icona di default per il tipo file (indice base 0 dell icona presente nell'eseguibile per la shell)
		0,				// handle per l'icona di default
		DPL_CONTENTTYPE	// tipo mime (application/x-desktop-picture-list)
	};
	strcpyn(dplfiletype.shell,m_Config.GetString(WALLPAPER_INSTALL_KEY,WALLPAPER_PROGRAM_KEY),sizeof(dplfiletype.shell));
	
	return(registry.RegisterFileType(&dplfiletype));
}

/*
	HotKeyHandler()

	Gestore per gli hotkeys, richiamato dai wrappers di cui sotto a loro volta chiamati (come callback)
	dall'oggetto relativo.
*/
void CWallPaperDlg::HotKeyHandler(UINT nMsg,WPARAM wParam,LPARAM lParam,LPCSTR lpcszThreadName/* = NULL*/)
{
	// il messaggio va inviato al thread ui
	if(lpcszThreadName)
	{
		// controlla se l'istanza del thread si trova in esecuzione
		CWinThread* pWinThread = NULL;
		BOOL bHaveThread = IsUIThreadRunning(lpcszThreadName,&pWinThread);
		if(bHaveThread && pWinThread)
		{
			// invia il messaggio al thread
			CWnd* pWnd = pWinThread->GetMainWnd();
			if(pWnd)
			{
				HWND hWnd = pWnd->GetSafeHwnd();
				if(hWnd)
					::PostMessage(hWnd,nMsg,wParam,lParam);
			}
		}
	}
	else // messaggio per l'applicazione principale
		::PostMessage(this->GetSafeHwnd(),nMsg,wParam,lParam);
}

/*
	OnReturnKey()

	Intercetta la pressione del tasto Enter (carica l'elemento corrente).
*/
BOOL CWallPaperDlg::OnReturnKey(void)
{
	int nItem = m_wndPlaylist.GetCurrentItem();
	if(nItem >= 0)
		SendWallPaperMessage(nItem);

	return(TRUE);
}

/*
	OnHotKey()
*/
LRESULT CWallPaperDlg::OnHotKey(WPARAM wParam,LPARAM /*lParam*/)
{
	if(wParam==HOTKEY_CTRL_ALT_MULTIPLY)
		HotKeyHandler(WM_COMMAND,MAKELONG(IDM_OPTIONS_RANDOMIZE_PLAYLIST_REORDER,0),0L);
	else if(wParam==HOTKEY_CTRL_ALT_DIVIDE)
		HotKeyHandler(WM_COMMAND,MAKELONG(IDM_TOOLS_UNDESKTOP,0),0L);
	else if(wParam==HOTKEY_CTRL_ALT_LEFT)
		HotKeyHandler(WM_AUDIOPLAYER_PREV,0L,0L,"CWallPaperPlayerUIThread");
	else if(wParam==HOTKEY_CTRL_ALT_RIGHT)
		HotKeyHandler(WM_AUDIOPLAYER_NEXT,0L,0L,"CWallPaperPlayerUIThread");	
	else if(wParam==HOTKEY_CTRL_ALT_BACKSPACE)
		HotKeyHandler(WM_AUDIOPLAYER_REMOVE,MAKEWPARAM(1,1),0L,"CWallPaperPlayerUIThread");	
	else if(wParam==HOTKEY_CTRL_ALT_UP)
		HotKeyHandler(WM_COMMAND,MAKELONG(IDM_PLAYLIST_PREVIOUS,0),0L);
	else if(wParam==HOTKEY_CTRL_ALT_DOWN)
		HotKeyHandler(WM_COMMAND,MAKELONG(IDM_PLAYLIST_NEXT,0),0L);
	else if(wParam==HOTKEY_CTRL_ALT_SPACE)
		HotKeyHandler(WM_AUDIOPLAYER_SETSTATUS,AUDIOPLAYER_COMMAND_TOGGLE_PLAYING,0L,"CWallPaperPlayerUIThread");
	else if(wParam==HOTKEY_CTRL_ALT_ADD)
		HotKeyHandler(WM_AUDIOPLAYER_SETVOLUME,AUDIOPLAYER_COMMAND_INCREASE_VOLUME,0L,"CWallPaperPlayerUIThread");
	else if(wParam==HOTKEY_CTRL_ALT_SUBTRACT)
		HotKeyHandler(WM_AUDIOPLAYER_SETVOLUME,AUDIOPLAYER_COMMAND_DECREASE_VOLUME,0L,"CWallPaperPlayerUIThread");
	return(0L);
}

/*
	OnFunctionKey()

	Intercetta la pressione dei tasti funzione.
*/
BOOL CWallPaperDlg::OnFunctionKey(WPARAM wFunctionKey,BOOL bShiftPressed,BOOL bCtrlPressed)
{
	BOOL bTranslated = FALSE;
	BOOL bPaused = m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_STATE_KEY)==WALLPAPER_IN_PAUSED_STATE;
	int nItemCount = m_wndPlaylist.GetItemCount();
	int nItem = 0;
	char szItem[MAX_ITEM_SIZE+1] = {0};
	char szLocation[MAX_ITEM_SIZE+1] = {0};
	BOOL bComesFromArchiveFile = FALSE;
	
	// ricava le info relative all'elemento corrente
	if(nItemCount > 0)
	{
		nItem = GetItem(szItem,sizeof(szItem));
		if(nItem >= 0)
		{
			m_wndPlaylist.GetItemText(nItem,CTRLLISTEX_LOCATION_INDEX,szLocation,sizeof(szLocation)-1);
			bComesFromArchiveFile = m_Archive.IsSupportedFormat(szLocation);
		}
	}

	switch(wFunctionKey)
	{
		// help
		case VK_F1:
			if(bShiftPressed)
				OnHomePage();
			bTranslated = TRUE;
			break;

		// salva
		case VK_F4:
			if(!bShiftPressed && !bCtrlPressed)
			{
				if(PlaylistIsModified())
					PlaylistSave();
				bTranslated = TRUE;
			}
			break;
		
		// aggiunta file
		case VK_F5:
			if(!bPaused)
				if(!bShiftPressed && !bCtrlPressed)
				{
					OnAddFile();
					bTranslated = TRUE;
				}
			break;
		
		// aggiunta directory
		case VK_F6:
			if(!bPaused)
				if(!bShiftPressed && !bCtrlPressed)
				{
					OnAddDir();
					bTranslated = TRUE;
				}
			break;
		
		// elimina
		case VK_F7:
			if(!bPaused)
				if(!bShiftPressed && !bCtrlPressed)
				{
					if(nItemCount > 0)
						OnRemove();
					bTranslated = TRUE;
				}
			break;
		
		// elimina tutto
		// shift: scarica (file) playlist
		// ctrl: decomprime
		case VK_F8:
			if(!bPaused)
			{
				if(!bShiftPressed && !bCtrlPressed)
				{
					if(nItemCount > 0)
						OnRemoveAll();
					bTranslated = TRUE;
				}
				if(bShiftPressed && !bCtrlPressed)
				{
					if(m_pCmdLine->HaveDplFile())
						OnUnloadPlaylistFile();
					bTranslated = TRUE;
				}
				if(!bShiftPressed && bCtrlPressed)
				{
					if(bComesFromArchiveFile)
						OnUnCompress();
					bTranslated = TRUE;
				}
			}
			break;

		// shift: nuovo (file) playlist
		// ctrl: visualizzatore
		case VK_F9:
			if(!bPaused)
			{	if(bShiftPressed && !bCtrlPressed)
				{
					OnNewPlaylistFile();
					bTranslated = TRUE;
				}
				if(!bShiftPressed && bCtrlPressed)
				{
					OnBrowser();
					bTranslated = TRUE;
				}
			}
			break;
		
		// shift: apre (file) playlist
		// ctrl: miniature
		case VK_F10:
			if(!bPaused)
				if(bShiftPressed && !bCtrlPressed)
				{
					OnLoadPlaylistFile();
					bTranslated = TRUE;
				}
			if(!bShiftPressed && bCtrlPressed)
			{
				OnThumbnails();
				bTranslated = TRUE;
			}
			break;
		
		// shift: salva (file) playlist
		// ctrl: crawler
		case VK_F11:
			if(!bPaused)
				if(bShiftPressed && !bCtrlPressed)
				{
					OnSavePlaylistFile();
					bTranslated = TRUE;
				}
			if(!bShiftPressed && bCtrlPressed)
			{
				OnCrawler();
				bTranslated = TRUE;
			}
			break;

		default:
			bTranslated = FALSE;
			break;
	}

	return(bTranslated);
}

/*
	OnAltFunctionKey()

	Intercetta la pressione dei tasti Alt+F....
*/
BOOL CWallPaperDlg::OnAltFunctionKey(WPARAM /*wFunctionKey*/)
{
	/*BOOL bTranslated = TRUE;
	
	switch(wFunctionKey)
	{
		default:
			bTranslated = FALSE;
			break;
	}

	return(bTranslated);*/
	
	return(FALSE);
}

/*
	OnVirtualKey()

	Intercetta la pressione dei tasti.
*/
BOOL CWallPaperDlg::OnVirtualKey(WPARAM wVirtualKey,BOOL /*bShiftPressed*/,BOOL bCtrlPressed,BOOL bAltPressed)
{
	BOOL bTranslated = FALSE;
	BOOL bPaused = m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_STATE_KEY)==WALLPAPER_IN_PAUSED_STATE;

	if(!bPaused)
	{
		BOOL bItemSelected = FALSE;
		int nItem = m_wndPlaylist.GetCurrentItem();
		int nItemCount = m_wndPlaylist.GetItemCount(); 
		int nItemPerPage = m_wndPlaylist.GetCountPerPage();

		// acceleratori per la toolbar (Alt + lettera evidenziata nel testo del bottone)
		switch(wVirtualKey)
		{
			// Alt + S (salva)
			case 'S':
				if(bAltPressed)
				{
					if(PlaylistIsModified())
						PlaylistSave();
					bTranslated = TRUE;
				}
				break;
			// Alt + L (aggiunta file)
			case 'L':
				if(bAltPressed)
				{
					if(!bPaused)
						OnAddFile();
					bTranslated = TRUE;
				}
				break;
			// Alt + R (aggiunta directory)
			case 'R':
				if(bAltPressed)
				{
					if(!bPaused)
						OnAddDir();
					bTranslated = TRUE;
				}
				break;
			// Alt + M (rimuove)
			case 'M':
				if(bAltPressed)
				{
					if(!bPaused && nItemCount > 0)
						OnRemove();
					bTranslated = TRUE;
				}
				break;
			// Alt + V (rimuove tutto)
			case 'V':
				if(bAltPressed)
				{
					if(!bPaused && nItemCount > 0)
						OnRemoveAll();
					bTranslated = TRUE;
				}
				break;
			// Alt + D (elimina file)
			case 'D':
				if(bAltPressed)
				{
					if(!bPaused && nItemCount > 0)
						OnDeleteFile();
					bTranslated = TRUE;
				}
				break;
			// Alt + U (scarica il desktop corrente)
			case 'U':
				if(bAltPressed)
				{
					if(!bPaused)
						OnUndesktop();
					bTranslated = TRUE;
				}
				break;
			// Alt + B (apre il browser)
			case 'B':
				if(bAltPressed)
				{
					OnBrowser();
					bTranslated = TRUE;
				}
				break;
			// Alt + C (lancia il crawler)
			case 'C':
				if(bAltPressed)
				{
					OnCrawler();
					bTranslated = TRUE;
				}
				break;
		}

		// se la combinazione premuta non era un acceleratore per la toolbar
		if(!bTranslated)
			switch(wVirtualKey)
			{
				// Ctrl + C (copia)
				case 'C':
					if(bCtrlPressed)
					{
						OnCopy();
						bTranslated = TRUE;
					}
					break;

				// Ctrl + V (incolla)
				case 'V':
					if(bCtrlPressed)
					{
						OnPaste();
						bTranslated = TRUE;
					}
					break;

				// Ctrl + E (svuota la clipboard)
				case 'E':
					if(bCtrlPressed)
					{
						if(::CountClipboardFormats() > 0)
							OnEmptyClipboard();
						bTranslated = TRUE;
					}
					break;

				// Home (primo elemento)
				case VK_HOME:
					if(nItemCount > 0)
					{
						bItemSelected = TRUE;
						nItem = 0;
						m_wndPlaylist.SelectItem(nItem);
					}
					bTranslated = TRUE;
					break;

				// End (ultimo elemento)
				case VK_END:
					if(nItemCount > 0)
					{
						bItemSelected = TRUE;
						nItem = nItemCount-1;
						m_wndPlaylist.SelectItem(nItem);
					}
					bTranslated = TRUE;
					break;

				// Pagina/Freccia su (pagina/elemento precedente)
				case VK_PRIOR:
					if(nItemCount > 0)
					{
						bItemSelected = TRUE;
						nItem -= nItemPerPage;
						if(nItem < 0)
							nItem = 0;
						m_wndPlaylist.SelectItem(nItem);
					}
					bTranslated = TRUE;
					break;
				case VK_UP:
					if(nItemCount > 0)
					{
						bItemSelected = TRUE;
						if(--nItem < 0)
							nItem = 0;
						m_wndPlaylist.SelectItem(nItem);
					}
					bTranslated = TRUE;
					break;
				
				// Pagina/Freccia giu' (pagina/elemento successivo)
				case VK_NEXT:
					if(nItemCount > 0)
					{
						bItemSelected = TRUE;
						nItem += nItemPerPage;
						if(nItem >= nItemCount)
							nItem = nItemCount-1;
						m_wndPlaylist.SelectItem(nItem);
					}
					bTranslated = TRUE;
					break;
				case VK_DOWN:
					if(nItemCount > 0)
					{
						bItemSelected = TRUE;
						if(++nItem >= nItemCount)
							nItem = nItemCount-1;
						m_wndPlaylist.SelectItem(nItem);
					}
					bTranslated = TRUE;
					break;
				
				// Ins (aggiunta file), Shift+Ins (aggiunta directory)
				case VK_INSERT:
					if(::GetKeyState(VK_SHIFT) >= 0)
						OnAddFile();
					else
						OnAddDir();
					bTranslated = TRUE;
					break;
				
				// Del (eliminazione elemento), Shift + Del (eliminazione di tutti gli elementi), Ctrl + Del (cancella il file)
				case VK_DELETE:
					if(nItemCount > 0)
					{
						bItemSelected = TRUE;
						if(::GetKeyState(VK_SHIFT) < 0)
							OnRemoveAll();
						else if(::GetKeyState(VK_LCONTROL) < 0 || ::GetKeyState(VK_RCONTROL) < 0)
							OnDeleteFile();
						else
							OnRemove();
					}
					bTranslated = TRUE;
					break;
				
				// barra spaziatrice (stoppa/riprende)
				case VK_SPACE:
					HotKeyHandler(WM_AUDIOPLAYER_SETSTATUS,AUDIOPLAYER_COMMAND_TOGGLE_PLAYING,0L,"CWallPaperPlayerUIThread");
					break;
			}

		if(bItemSelected)
			OnPreviewMode(TRUE);
	}

	return(bTranslated);
}

/*
	OnNotifyTrayIcon()

	Gestore per il click del mouse sull'icona nella tray area.
*/
LRESULT CWallPaperDlg::OnNotifyTrayIcon(WPARAM /*wParam*/,LPARAM lParam)
{
	//UINT uID = (UINT)wParam;
	UINT uMouseMsg = (UINT)lParam;

	CRect client;
	GetClientRect(&client);

	switch(uMouseMsg)
	{
		// click destro
		case WM_RBUTTONDOWN:
		{
			// visualizza il menu
			CTitleMenu popupMenu;
			popupMenu.CreatePopupMenu();
			
			// aggiunge le voci del menu a seconda dello status
			popupMenu.AppendMenu(MF_STRING,IDM_FILE_EXIT,"E&xit");
			popupMenu.AppendMenu(MF_STRING,IDM_FILE_MINIMIZE,"M&inimize");
			popupMenu.AppendMenu(MF_STRING,IDM_FILE_MAXIMIZE,"M&aximize");
			popupMenu.AppendMenu(MF_SEPARATOR);
			popupMenu.AppendMenu(MF_STRING,IDM_HELP_ABOUT,"Abou&t...");
			popupMenu.AppendMenu(MF_SEPARATOR);

			BOOL bActive = m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_STATE_KEY)==WALLPAPER_IN_ACTIVE_STATE;
			if(bActive)
			{
				int nItemCount = m_wndPlaylist.GetItemCount() > 0;
				popupMenu.AppendMenu(MF_STRING,IDM_PLAYLIST_PAUSE,"Pa&use");
				popupMenu.EnableMenuItem(IDM_PLAYLIST_PAUSE,nItemCount > 0 ? MF_ENABLED : MF_GRAYED);
				
				char szItem[MAX_ITEM_SIZE+1];
				int nItem = GetItem(szItem,sizeof(szItem));
				if(nItem >= 0)
				{
					if(m_pImage->IsSupportedFormat(szItem))
					{
						m_wndPlaylist.GetItemText(nItem,CTRLLISTEX_FILENAME_INDEX,szItem,sizeof(szItem)-1);

						// sostituisce & con && affinche il menu non interpreti il carattere come un acceleratore
						char szBuffer[_MAX_FILEPATH+1];
						char szFileName[_MAX_FILEPATH+1];
						strcpyn(szFileName,szItem,sizeof(szFileName));
						substr(szFileName,"&","&&",szBuffer,sizeof(szBuffer));
						strcpyn(szFileName,szBuffer,sizeof(szFileName));

						// se il nome file sfora, lo accorcia con [...]
						CFilenameFactory fn;
						char szMenuEntry[_MAX_FILEPATH+1];
						_snprintf(szMenuEntry,sizeof(szMenuEntry)-1,"Add to &Favourite Pictures (%s)",fn.Abbreviate(szFileName,_MAX_MENU_STRING_LENGTH-28-1));
						popupMenu.AppendMenu(MF_STRING,IDM_PLAYLIST_FAVOURITE_PICTURE,szMenuEntry);
					}
				}
				
				popupMenu.AppendMenu(MF_STRING,IDM_PLAYLIST_PREVIOUS,"&Previous");
				popupMenu.EnableMenuItem(IDM_PLAYLIST_PREVIOUS,nItemCount > 0 ? MF_ENABLED : MF_GRAYED);
				popupMenu.AppendMenu(MF_STRING,IDM_PLAYLIST_NEXT,"&Next");
				popupMenu.EnableMenuItem(IDM_PLAYLIST_NEXT,nItemCount > 0 ? MF_ENABLED : MF_GRAYED);
			}
			else
				popupMenu.AppendMenu(MF_BYCOMMAND|MF_STRING,IDM_PLAYLIST_RESUME,"&Resume");
		
			char szMenuTitle[256];
			_snprintf(szMenuTitle,sizeof(szMenuTitle)-1,"%s v.%s",WALLPAPER_PROGRAM_NAME,WALLPAPER_VERSION);
			popupMenu.AddTitle(szMenuTitle,IDM_HELP_LATESTVERSION);
			popupMenu.SetEdge(TRUE,0,EDGE_SUNKEN);

			SetForegroundWindow();
			CPoint point;
			GetCursorPos(&point);
			popupMenu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,point.x,point.y,this);
			popupMenu.DestroyMenu();
			break;
		}

		// doppio click sinistro
		case WM_LBUTTONDBLCLK:
		{
			// ripristina o minimizza
			if(IsIconic() || !IsWindowVisible())
				::PostMessage(this->GetSafeHwnd(),WM_COMMAND,MAKELONG(IDM_FILE_MAXIMIZE,0),0L);
			else
				::PostMessage(this->GetSafeHwnd(),WM_COMMAND,MAKELONG(IDM_FILE_MINIMIZE,0),0L);
			break;
		}

		// click sinistro
		case WM_LBUTTONDOWN:
		{
			// passa all'elemento seguente
			if(m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_STATE_KEY)==WALLPAPER_IN_ACTIVE_STATE)
				if(m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_ONE_CLICK_CHANGE_KEY))
				{
					BOOL bShiftPressed = ::GetKeyState(VK_SHIFT) < 0;
					if(bShiftPressed)
						OnPrevious();
					else
						OnNext();
				}
			break;
		}
	}

	return(0L);
}

/*
	OnNotifyTaskbarPopup()
	
	Gestore dei messaggi inviati dal popup (bitmap).
*/
LRESULT CWallPaperDlg::OnNotifyTaskbarPopup(WPARAM /*wParam*/,LPARAM lParam)
{
	// click sul bitmap, controlla se sono state specificate le coordinate
	// per il bottone di chiusura e se il click e' avvenuto nell'area relativa
	if(m_TaskbarPopup.nPopupCloseLeft==0 && m_TaskbarPopup.nPopupCloseTop==0 && m_TaskbarPopup.nPopupCloseRight==0 && m_TaskbarPopup.nPopupCloseBottom==0)
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
	// il browser sull'url associata (se specificata)
	CUrl url;
	if(url.IsUrlType(m_TaskbarPopup.szUrl,HTTP_URL))
	{
		CBrowser browser(m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_PREFERRED_INTERNET_BROWSER_KEY));
		browser.Browse(m_TaskbarPopup.szUrl);
	}

	return(0L);
}

/*
	OnSysCommand()

	Gestore del menu di sistema per l'icona del programma (l'icona della taskbar, non della tray area).
*/
void CWallPaperDlg::OnSysCommand(UINT nID,LPARAM lParam)
{
	// esegue l'azione relativa alla voce selezionata dal menu
	if((nID & 0xfff0)==IDM_SYSMENU_NEXTPIC)
	{
		SendWallPaperMessage(WALLPAPER_GOTO_NEXT_PICTURE);
	}
	else if((nID & 0xfff0)==IDM_SYSMENU_PREVPIC)
	{
		SendWallPaperMessage(WALLPAPER_GOTO_PREVIOUS_PICTURE);
	}
	else if((nID & 0xfff0)==IDM_SYSMENU_FAVOURITE)
	{
		int nItem;
		if((nItem = m_wndPlaylist.GetCurrentItem()) >= 0)
			// wParam: -1=lParam contiene il nome file, 0=picture, 1=audio, 2=ricava in proprio
			::PostMessage(this->GetSafeHwnd(),WM_FAVOURITE_ADD,(WPARAM)2,(LPARAM)nItem);
	}
	else if((nID & 0xfff0)==IDM_SYSMENU_RESUME)
	{
		OnResume();
	}
	else if((nID & 0xfff0)==IDM_SYSMENU_PAUSE)
	{
		OnPause();
	}
	else if((nID & 0xfff0)==IDM_SYSMENU_ABOUT)
	{
		OnAbout();
	}
	else if((nID & 0xfff0)==IDM_SYSMENU_EXIT)
	{
		OnExit();
	}
	else // voci del menu di sistema
	{
		// idem per IDM_FILE_MINIMIZE
		if(nID==SC_MINIMIZE)
		{
			// minimizza il dialogo per l'anteprima
			CWinThread* pWinThread = NULL;
			BOOL bHavePlayer = IsUIThreadRunning("CWallPaperPreviewUIThread",&pWinThread);
			if(bHavePlayer && pWinThread)
			{
				CWnd* pWnd = pWinThread->GetMainWnd();
				if(pWnd)
				{
					HWND hWnd = pWnd->GetSafeHwnd();
					if(hWnd)
						if(pWnd->IsWindowVisible())
							::SendMessage(hWnd,WM_PREVIEWMINMAXIMIZE,(WPARAM)SC_MINIMIZE,(LPARAM)0L);
				}
			}

			ShowWindow(m_pTrayIcon ? SW_HIDE : SW_MINIMIZE);
		}
		// idem per IDM_FILE_MAXIMIZE
		else if(nID==SC_MAXIMIZE || nID==SC_RESTORE)
		{
			// ripristina il dialogo per l'anteprima
			CWinThread* pWinThread = NULL;
			BOOL bHavePlayer = IsUIThreadRunning("CWallPaperPreviewUIThread",&pWinThread);
			if(bHavePlayer && pWinThread)
			{
				CWnd* pWnd = pWinThread->GetMainWnd();
				if(pWnd)
				{
					HWND hWnd = pWnd->GetSafeHwnd();
					if(hWnd)
						if(pWnd->IsWindowVisible())
							::SendMessage(hWnd,WM_PREVIEWMINMAXIMIZE,(WPARAM)SC_MAXIMIZE,(LPARAM)0L);
				}
			}
			
			CDialogEx::OnSysCommand(nID,lParam);
		}
		else if(nID==SC_CLOSE)
		{
			if(m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_MINIMIZE_ON_CLOSE_KEY))
				OnMenuFileMinimize();
			else
				CDialogEx::OnSysCommand(nID,lParam);
		}
		else // classe base
			CDialogEx::OnSysCommand(nID,lParam);
	}
}

/*
	OnInitMenuPopup()

	Modifica le voci dei menu prima della visualizzazione.
*/
void CWallPaperDlg::OnInitMenuPopup(CMenu* pMenu,UINT nIndex,BOOL bSysMenu)
{
	// classe base
	CDialogEx::OnInitMenuPopup(pMenu,nIndex,bSysMenu);

	// ricava il puntatore al menu
	CMenu* pDialogMenu = NULL;
	pDialogMenu = pMenu;

	if(!bSysMenu)
	{
		int nItem;
		char szItem[MAX_ITEM_SIZE+1];
		nItem = GetItem(szItem,sizeof(szItem));

		BOOL bEnable = m_wndPlaylist.GetItemCount() > 0;
		BOOL bActive = m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_STATE_KEY)==WALLPAPER_IN_ACTIVE_STATE;
		BOOL bIsPicture = m_pImage->IsSupportedFormat(szItem);

		BOOL bComesFromArchiveFile = FALSE;
		char szLocation[MAX_ITEM_SIZE+1];
		m_wndPlaylist.GetItemText(nItem,CTRLLISTEX_LOCATION_INDEX,szLocation,sizeof(szLocation)-1);
		bComesFromArchiveFile = m_Archive.IsSupportedFormat(szLocation);

		switch(nIndex)
		{ 
			// File
			case 0:
			{
				pDialogMenu->EnableMenuItem(IDM_FILE_OPEN,bEnable && bActive && !bComesFromArchiveFile ? MF_ENABLED : MF_GRAYED);
				pDialogMenu->EnableMenuItem(IDM_FILE_SAVEAS,bEnable && bActive && !bComesFromArchiveFile ? (bIsPicture ? MF_ENABLED : MF_GRAYED) : MF_GRAYED);
				pDialogMenu->EnableMenuItem(IDM_FILE_DELETE,bEnable && bActive && !bComesFromArchiveFile ? MF_ENABLED : MF_GRAYED);
				break;
			}

			// Edit
			case 1:
			{
				// Copy (Ctrl+C)
				BOOL bIsCopyAvailable = bEnable && bActive;
				pDialogMenu->EnableMenuItem(IDM_EDIT_COPY,bIsCopyAvailable ? MF_ENABLED : MF_GRAYED);
				
				// Paste (Ctrl+V)
				char szPaste[_MAX_MENU_STRING_LENGTH] = {0};
				BOOL bIsPasteAvailable = IsPasteAvailable();
				switch(m_enumClipboardFormat)
				{
					case CLIPBOARD_PICTURE_FORMAT:
						strcpyn(szPaste,"&Paste (picture)\tCtrl+V",sizeof(szPaste));
						break;
					case CLIPBOARD_FILEDROP_FORMAT:
						strcpyn(szPaste,"&Paste (filenames)\tCtrl+V",sizeof(szPaste));
						break;
					case CLIPBOARD_URL_NETSCAPE_FORMAT:
					case CLIPBOARD_URL_IEXPLORE_FORMAT:
						strcpyn(szPaste,"&Paste (url)\tCtrl+V",sizeof(szPaste));
						break;
					default:
						strcpyn(szPaste,"&Paste\tCtrl+V",sizeof(szPaste));
						break;
				}
				pDialogMenu->ModifyMenu(IDM_EDIT_PASTE,MF_BYCOMMAND|MF_STRING,IDM_EDIT_PASTE,szPaste);
				pDialogMenu->EnableMenuItem(IDM_EDIT_PASTE,bActive && bIsPasteAvailable ? MF_ENABLED : MF_GRAYED);
				pDialogMenu->EnableMenuItem(IDM_EDIT_EMPTYCLIPBOARD,IsPasteAvailable() ? MF_ENABLED : MF_GRAYED);
				
				break;
			}

			// Picture
			case 2:
			{
				pDialogMenu->EnableMenuItem(IDM_PLAYLIST_SAVEPLAYLIST,PlaylistIsModified() ? MF_ENABLED : MF_GRAYED);
				pDialogMenu->EnableMenuItem(IDM_PLAYLIST_PREVIOUS,bEnable && bActive ? MF_ENABLED : MF_GRAYED);
				pDialogMenu->EnableMenuItem(IDM_PLAYLIST_NEXT,bEnable && bActive ? MF_ENABLED : MF_GRAYED);
				pDialogMenu->EnableMenuItem(IDM_PLAYLIST_PAUSE,bActive ? MF_ENABLED : MF_GRAYED);
				if(bActive)
					if(!bEnable)
						pDialogMenu->EnableMenuItem(IDM_PLAYLIST_PAUSE,MF_GRAYED);
				pDialogMenu->EnableMenuItem(IDM_PLAYLIST_RESUME,!bActive ? MF_ENABLED : MF_GRAYED);

				pDialogMenu->EnableMenuItem(IDM_PLAYLIST_ADDFILE,bActive ? MF_ENABLED : MF_GRAYED);
				pDialogMenu->EnableMenuItem(IDM_PLAYLIST_ADDDIR,bActive ? MF_ENABLED : MF_GRAYED);
				pDialogMenu->EnableMenuItem(IDM_PLAYLIST_ADDURL,bActive ? MF_ENABLED : MF_GRAYED);

				pDialogMenu->EnableMenuItem(IDM_PLAYLIST_REMOVE,bEnable && bActive ? MF_ENABLED : MF_GRAYED);
				pDialogMenu->EnableMenuItem(IDM_PLAYLIST_REMOVEALL,bEnable && bActive ? MF_ENABLED : MF_GRAYED);

				pDialogMenu->EnableMenuItem(IDM_PLAYLIST_UNLOADLIST,(bActive && m_pCmdLine->HaveDplFile()) ? MF_ENABLED : MF_GRAYED);
				pDialogMenu->EnableMenuItem(IDM_PLAYLIST_NEWLIST,bActive ? MF_ENABLED : MF_GRAYED);
				pDialogMenu->EnableMenuItem(IDM_PLAYLIST_LOADLIST,bActive ? MF_ENABLED : MF_GRAYED);
				pDialogMenu->EnableMenuItem(IDM_PLAYLIST_SAVELIST,bEnable && bActive ? MF_ENABLED : MF_GRAYED);
				pDialogMenu->EnableMenuItem(IDM_PLAYLIST_FAVOURITE_PICTURE,bEnable && bIsPicture ? MF_ENABLED : MF_GRAYED);
				pDialogMenu->EnableMenuItem(IDM_PLAYLIST_FAVOURITE_AUDIO,bEnable && !bIsPicture ? MF_ENABLED : MF_GRAYED);
				pDialogMenu->EnableMenuItem(IDM_PLAYLIST_LOAD_FAVOURITE_PICTURE,bActive ? MF_ENABLED : MF_GRAYED);
				pDialogMenu->EnableMenuItem(IDM_PLAYLIST_LOAD_FAVOURITE_AUDIO,bActive ? MF_ENABLED : MF_GRAYED);
				
				break;
			}

			// Tools
			case 3:
			{
				char szDesktopIcons[_MAX_MENU_STRING_LENGTH] = {0};
				::FormatResourceStringEx(szDesktopIcons,sizeof(szDesktopIcons),IDS_MESSAGE_DESKTOPICONS,m_bDesktopIcons ? "Show" : "Hide");
				
				pDialogMenu->ModifyMenu(IDM_TOOLS_DESKTOPICONS,MF_BYCOMMAND|MF_STRING,IDM_TOOLS_DESKTOPICONS,szDesktopIcons);
				pDialogMenu->EnableMenuItem(IDM_TOOLS_UNCOMPRESS,bEnable && bActive ? (bComesFromArchiveFile ? MF_ENABLED : MF_GRAYED) : MF_GRAYED);

				break;
			}

			// Options
			case 4:
			{
				pDialogMenu->CheckMenuItem(IDM_OPTIONS_ALWAYSONTOP,m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_ALWAYSONTOP_KEY) ? MF_CHECKED : MF_UNCHECKED);
				pDialogMenu->CheckMenuItem(IDM_OPTIONS_AUTORUN,m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_AUTORUN_KEY) ? MF_CHECKED : MF_UNCHECKED);
				pDialogMenu->CheckMenuItem(IDM_OPTIONS_TRAYICON,m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_TRAYICON_KEY) ? MF_CHECKED : MF_UNCHECKED);
				pDialogMenu->CheckMenuItem(IDM_OPTIONS_MINIMIZE_ON_CLOSE,m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_MINIMIZE_ON_CLOSE_KEY) ? MF_CHECKED : MF_UNCHECKED);
				pDialogMenu->CheckMenuItem(IDM_OPTIONS_SPLASHSCREEN,m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_SPLASHSCREEN_KEY) ? MF_CHECKED : MF_UNCHECKED);
				
				pDialogMenu->CheckMenuItem(IDM_OPTIONS_AUDIO_NO_POPUP,m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_AUDIOPOPUP_KEY)==AUDIOPOPUP_NONE ? MF_CHECKED : MF_UNCHECKED);
				pDialogMenu->CheckMenuItem(IDM_OPTIONS_AUDIO_BALLOON_POPUP,m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_AUDIOPOPUP_KEY)==AUDIOPOPUP_BALLOON ? MF_CHECKED : MF_UNCHECKED);
				pDialogMenu->EnableMenuItem(IDM_OPTIONS_AUDIO_RELOAD_BALLOON,m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_AUDIOPOPUP_KEY)==AUDIOPOPUP_BALLOON ? MF_ENABLED : MF_GRAYED);
				pDialogMenu->CheckMenuItem(IDM_OPTIONS_AUDIO_TASKBAR_POPUP,m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_AUDIOPOPUP_KEY)==AUDIOPOPUP_TASKBAR ? MF_CHECKED : MF_UNCHECKED);
				pDialogMenu->EnableMenuItem(IDM_OPTIONS_AUDIO_RELOAD_TASKBAR_POPUP,m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_AUDIOPOPUP_KEY)==AUDIOPOPUP_TASKBAR ? MF_ENABLED : MF_GRAYED);
				pDialogMenu->CheckMenuItem(IDM_OPTIONS_AUDIO_BALLOON_DEFAULTICON,m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_BALLOON_AUDIO_ICON_TYPE_KEY)==BALLOON_ICON_DEFAULT ? MF_CHECKED : MF_UNCHECKED);
				pDialogMenu->EnableMenuItem(IDM_OPTIONS_AUDIO_BALLOON_DEFAULTICON,m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_AUDIOPOPUP_KEY)==AUDIOPOPUP_BALLOON ? MF_ENABLED : MF_GRAYED);
				pDialogMenu->CheckMenuItem(IDM_OPTIONS_AUDIO_BALLOON_CUSTOMICON,m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_BALLOON_AUDIO_ICON_TYPE_KEY)==BALLOON_ICON_CUSTOM ? MF_CHECKED : MF_UNCHECKED);
				pDialogMenu->EnableMenuItem(IDM_OPTIONS_AUDIO_BALLOON_CUSTOMICON,m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_AUDIOPOPUP_KEY)==AUDIOPOPUP_BALLOON ? MF_ENABLED : MF_GRAYED);

				pDialogMenu->CheckMenuItem(IDM_OPTIONS_PICT_NO_POPUP,m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PICTPOPUP_KEY)==PICTPOPUP_NONE ? MF_CHECKED : MF_UNCHECKED);
				pDialogMenu->CheckMenuItem(IDM_OPTIONS_PICT_BALLOON_POPUP,m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PICTPOPUP_KEY)==PICTPOPUP_BALLOON ? MF_CHECKED : MF_UNCHECKED);
				pDialogMenu->EnableMenuItem(IDM_OPTIONS_PICT_RELOAD_BALLOON,m_pImage->IsLoaded() && m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PICTPOPUP_KEY)==PICTPOPUP_BALLOON ? MF_ENABLED : MF_GRAYED);
				pDialogMenu->CheckMenuItem(IDM_OPTIONS_PICT_TASKBAR_POPUP,m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PICTPOPUP_KEY)==PICTPOPUP_TASKBAR ? MF_CHECKED : MF_UNCHECKED);
				pDialogMenu->EnableMenuItem(IDM_OPTIONS_PICT_RELOAD_TASKBAR_POPUP,m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PICTPOPUP_KEY)==PICTPOPUP_TASKBAR ? MF_ENABLED : MF_GRAYED);
				pDialogMenu->CheckMenuItem(IDM_OPTIONS_PICT_BALLOON_DEFAULTICON,m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_BALLOON_PICT_ICON_TYPE_KEY)==BALLOON_ICON_DEFAULT ? MF_CHECKED : MF_UNCHECKED);
				pDialogMenu->EnableMenuItem(IDM_OPTIONS_PICT_BALLOON_DEFAULTICON,m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PICTPOPUP_KEY)==PICTPOPUP_BALLOON ? MF_ENABLED : MF_GRAYED);
				pDialogMenu->CheckMenuItem(IDM_OPTIONS_PICT_BALLOON_CUSTOMICON,m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_BALLOON_PICT_ICON_TYPE_KEY)==BALLOON_ICON_CUSTOM ? MF_CHECKED : MF_UNCHECKED);
				pDialogMenu->EnableMenuItem(IDM_OPTIONS_PICT_BALLOON_CUSTOMICON,m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PICTPOPUP_KEY)==PICTPOPUP_BALLOON ? MF_ENABLED : MF_GRAYED);

				pDialogMenu->CheckMenuItem(IDM_OPTIONS_GRIDLINES,m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_GRID_GRIDLINES_KEY) ? MF_CHECKED : MF_UNCHECKED);
				pDialogMenu->CheckMenuItem(IDM_OPTIONS_ONE_CLICK_CHANGE,m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_ONE_CLICK_CHANGE_KEY) ? MF_CHECKED : MF_UNCHECKED);
				pDialogMenu->EnableMenuItem(IDM_OPTIONS_ONE_CLICK_CHANGE,m_pTrayIcon ? MF_ENABLED : MF_GRAYED);
				pDialogMenu->CheckMenuItem(IDM_OPTIONS_NERVOUS_TITLE,m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_NERVOUS_TITLE_KEY) ? MF_CHECKED : MF_UNCHECKED);
				pDialogMenu->EnableMenuItem(IDM_OPTIONS_NERVOUS_TITLE,m_pTrayIcon ? MF_GRAYED : MF_ENABLED);
				pDialogMenu->CheckMenuItem(IDM_OPTIONS_CONTEXT_MENU,m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CONTEXT_MENU_KEY) ? MF_CHECKED : MF_UNCHECKED);
				pDialogMenu->CheckMenuItem(IDM_OPTIONS_PLAYLIST_LOADING_ADD,m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PLAYLIST_LOADING_KEY)==PLAYLIST_LOADING_ADD ? MF_CHECKED : MF_UNCHECKED);
				pDialogMenu->CheckMenuItem(IDM_OPTIONS_PLAYLIST_LOADING_PUT,m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PLAYLIST_LOADING_KEY)==PLAYLIST_LOADING_PUT ? MF_CHECKED : MF_UNCHECKED);
				pDialogMenu->CheckMenuItem(IDM_OPTIONS_RANDOMIZE_PLAYLIST_SELECTION,m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_RANDOMIZE_PLAYLIST_KEY)==RANDOMIZE_PLAYLIST_SELECTION ? MF_CHECKED : MF_UNCHECKED);
				pDialogMenu->CheckMenuItem(IDM_OPTIONS_RANDOMIZE_PLAYLIST_REORDER,m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_RANDOMIZE_PLAYLIST_KEY)==RANDOMIZE_PLAYLIST_REORDER ? MF_CHECKED : MF_UNCHECKED);
				pDialogMenu->CheckMenuItem(IDM_OPTIONS_RANDOMIZE_PLAYLIST_NONE,m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_RANDOMIZE_PLAYLIST_KEY)==RANDOMIZE_PLAYLIST_NONE ? MF_CHECKED : MF_UNCHECKED);				
				pDialogMenu->CheckMenuItem(IDM_OPTIONS_INFAMOUS_SENTENCES,m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_INFAMOUS_SENTENCES_KEY) ? MF_CHECKED : MF_UNCHECKED);
				pDialogMenu->EnableMenuItem(IDM_OPTIONS_INFAMOUS_SENTENCES,m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_NERVOUS_TITLE_KEY) ? MF_ENABLED : MF_GRAYED);
				pDialogMenu->CheckMenuItem(IDM_OPTIONS_RECURSE_SUBDIR,m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_RECURSE_SUBDIR_KEY) ? MF_CHECKED : MF_UNCHECKED);
				pDialogMenu->CheckMenuItem(IDM_OPTIONS_CONFIRM_FILE_DELETE,m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CONFIRM_FILE_DELETE_KEY) ? MF_CHECKED : MF_UNCHECKED);
				pDialogMenu->CheckMenuItem(IDM_OPTIONS_DELETE_FILES_TO_RECYCLEBIN,m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_DELETE_FILES_TO_RECYCLEBIN_KEY) ? MF_CHECKED : MF_UNCHECKED);
				pDialogMenu->CheckMenuItem(IDM_OPTIONS_TEMPORARY_FILES_CLEAR_AT_EXIT,m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CLEAR_TEMPORARY_FILES_KEY)==TEMPORARY_FILES_CLEAR_AT_EXIT ? MF_CHECKED : MF_UNCHECKED);
				pDialogMenu->CheckMenuItem(IDM_OPTIONS_TEMPORARY_FILES_CLEAR_AT_RUNTIME,m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CLEAR_TEMPORARY_FILES_KEY)==TEMPORARY_FILES_CLEAR_AT_RUNTIME ? MF_CHECKED : MF_UNCHECKED);
				pDialogMenu->CheckMenuItem(IDM_OPTIONS_TEMPORARY_FILES_DO_NOT_CLEAR,m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CLEAR_TEMPORARY_FILES_KEY)==TEMPORARY_FILES_DO_NOT_CLEAR ? MF_CHECKED : MF_UNCHECKED);
				pDialogMenu->CheckMenuItem(IDM_OPTIONS_PRIORITY_PROCESS_REALTIME,m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PRIORITY_PROCESS_KEY)==REALTIME_PRIORITY_CLASS ? MF_CHECKED : MF_UNCHECKED);
				pDialogMenu->CheckMenuItem(IDM_OPTIONS_PRIORITY_PROCESS_HIGH,m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PRIORITY_PROCESS_KEY)==HIGH_PRIORITY_CLASS ? MF_CHECKED : MF_UNCHECKED);
				pDialogMenu->CheckMenuItem(IDM_OPTIONS_PRIORITY_PROCESS_NORMAL,m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PRIORITY_PROCESS_KEY)==NORMAL_PRIORITY_CLASS ? MF_CHECKED : MF_UNCHECKED);
				pDialogMenu->CheckMenuItem(IDM_OPTIONS_PRIORITY_PROCESS_IDLE,m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PRIORITY_PROCESS_KEY)==IDLE_PRIORITY_CLASS ? MF_CHECKED : MF_UNCHECKED);
				pDialogMenu->CheckMenuItem(IDM_OPTIONS_PRIORITY_THREAD_TIME_CRITICAL,m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PRIORITY_THREAD_KEY)==THREAD_PRIORITY_TIME_CRITICAL ? MF_CHECKED : MF_UNCHECKED);
				pDialogMenu->CheckMenuItem(IDM_OPTIONS_PRIORITY_THREAD_HIGHEST,m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PRIORITY_THREAD_KEY)==THREAD_PRIORITY_HIGHEST ? MF_CHECKED : MF_UNCHECKED);
				pDialogMenu->CheckMenuItem(IDM_OPTIONS_PRIORITY_THREAD_ABOVE_NORMAL,m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PRIORITY_THREAD_KEY)==THREAD_PRIORITY_ABOVE_NORMAL ? MF_CHECKED : MF_UNCHECKED);
				pDialogMenu->CheckMenuItem(IDM_OPTIONS_PRIORITY_THREAD_NORMAL,m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PRIORITY_THREAD_KEY)==THREAD_PRIORITY_NORMAL ? MF_CHECKED : MF_UNCHECKED);
				pDialogMenu->CheckMenuItem(IDM_OPTIONS_PRIORITY_THREAD_BELOW_NORMAL,m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PRIORITY_THREAD_KEY)==THREAD_PRIORITY_BELOW_NORMAL ? MF_CHECKED : MF_UNCHECKED);
				pDialogMenu->CheckMenuItem(IDM_OPTIONS_PRIORITY_THREAD_LOWEST,m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PRIORITY_THREAD_KEY)==THREAD_PRIORITY_LOWEST ? MF_CHECKED : MF_UNCHECKED);
				pDialogMenu->CheckMenuItem(IDM_OPTIONS_PRIORITY_THREAD_IDLE,m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PRIORITY_THREAD_KEY)==THREAD_PRIORITY_IDLE ? MF_CHECKED : MF_UNCHECKED);
				break;
			}

			// Help
			case 5:
			{
				char szMenuEntry[32];
				DWORD dwLatestVersionCheckType = m_Config.GetNumber(WALLPAPER_INSTALL_KEY,WALLPAPER_LATESTVERSIONCHECKTYPE_KEY);
				_snprintf(szMenuEntry,
						sizeof(szMenuEntry)-1,
						"&Every %d Day(s)",
						dwLatestVersionCheckType==1965L || dwLatestVersionCheckType==0L ? DEFAULT_DAYS_FOR_LATESTVERSIONCHECK : dwLatestVersionCheckType
						);
				pDialogMenu->ModifyMenu(IDM_HELP_LATESTVERSION_EVERY_N_DAYS,MF_BYCOMMAND|MF_STRING,IDM_HELP_LATESTVERSION_EVERY_N_DAYS,szMenuEntry);
				pDialogMenu->CheckMenuItem(IDM_HELP_LATESTVERSION_AT_STARTUP,dwLatestVersionCheckType==0L ? MF_CHECKED : MF_UNCHECKED);
				pDialogMenu->CheckMenuItem(IDM_HELP_LATESTVERSION_EVERY_N_DAYS,dwLatestVersionCheckType!=1965L && dwLatestVersionCheckType!=0L ? MF_CHECKED : MF_UNCHECKED);
				pDialogMenu->CheckMenuItem(IDM_HELP_LATESTVERSION_NEVER,dwLatestVersionCheckType==1965L ? MF_CHECKED : MF_UNCHECKED);
			}
		}
	}
}

/*
	OnMenuFileMinimize()

	Minimizza il dialogo.
*/
void CWallPaperDlg::OnMenuFileMinimize(void)
{
	// minimizza il dialogo per l'anteprima
	// idem per SC_MINIMIZE
	CWinThread* pWinThread = NULL;
	BOOL bHavePlayer = IsUIThreadRunning("CWallPaperPreviewUIThread",&pWinThread);
	if(bHavePlayer && pWinThread)
	{
		CWnd* pWnd = pWinThread->GetMainWnd();
		if(pWnd)
		{
			HWND hWnd = pWnd->GetSafeHwnd();
			if(hWnd)
				if(pWnd->IsWindowVisible())
					::SendMessage(hWnd,WM_PREVIEWMINMAXIMIZE,(WPARAM)SC_MINIMIZE,(LPARAM)0L);
		}
	}

	ShowWindow(SW_MINIMIZE);
	if(m_pTrayIcon)
		ShowWindow(SW_HIDE);
}

/*
	OnMenuFileMaximize()

	Ripristina il dialogo.
*/
void CWallPaperDlg::OnMenuFileMaximize(void)
{
	// ripristina il dialogo per l'anteprima
	// idem per SC_MAXIMIZE
	CWinThread* pWinThread = NULL;
	BOOL bHavePlayer = IsUIThreadRunning("CWallPaperPreviewUIThread",&pWinThread);
	if(bHavePlayer && pWinThread)
	{
		CWnd* pWnd = pWinThread->GetMainWnd();
		if(pWnd)
		{
			HWND hWnd = pWnd->GetSafeHwnd();
			if(hWnd)
				if(pWnd->IsWindowVisible())
					::SendMessage(hWnd,WM_PREVIEWMINMAXIMIZE,(WPARAM)SC_MAXIMIZE,(LPARAM)0L);
		}
	}

	if(!IsVisible())
		SetVisible(TRUE);
		
	::ShowWindow(this->GetSafeHwnd(),SW_RESTORE);
	::SetForegroundWindow(this->GetSafeHwnd());
	::SetFocus(this->GetSafeHwnd());
}

/*
	OnMenuOptionsAlwaysOnTop()

	Aggiorna il flag per la messa in primo piano.
*/
void CWallPaperDlg::OnMenuOptionsAlwaysOnTop(void)
{
	m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_ALWAYSONTOP_KEY,!m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_ALWAYSONTOP_KEY));
	m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_ALWAYSONTOP_KEY);
	SetWindowPos(m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_ALWAYSONTOP_KEY) ? &wndTopMost : &wndNoTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
}

/*
	OnMenuOptionsAutoRun()

	Aggiorna il flag per il lancio automatico.
*/
void CWallPaperDlg::OnMenuOptionsAutoRun(void)
{
	m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_AUTORUN_KEY,!m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_AUTORUN_KEY));
	m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_AUTORUN_KEY);
	if(m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_AUTORUN_KEY))
		m_Config.RegistryCreateValue("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",WALLPAPER_PROGRAM_NAME,m_Config.GetString(WALLPAPER_INSTALL_KEY,WALLPAPER_PROGRAM_KEY));
	else
		m_Config.RegistryDeleteValue("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",WALLPAPER_PROGRAM_NAME);
}

/*
	OnMenuOptionsSplashScreen()

	Aggiorna il flag per la visualizzazione della schermata iniziale.
*/
void CWallPaperDlg::OnMenuOptionsSplashScreen(void)
{
	m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_SPLASHSCREEN_KEY,!m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_SPLASHSCREEN_KEY));
	m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_SPLASHSCREEN_KEY);
}

/*
	OnMenuOptionsTrayIcon()

	Aggiorna il flag per l'utilizzo dell'icona nella tray area.
*/
void CWallPaperDlg::OnMenuOptionsTrayIcon(void)
{
	m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_TRAYICON_KEY,!m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_TRAYICON_KEY));
	m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_TRAYICON_KEY);
	
	if(m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_TRAYICON_KEY))
	{
		if(!m_pTrayIcon)
		{
			m_pTrayIcon = new CTrayIcon();
			if(m_pTrayIcon)
			{
				if(m_pTrayIcon->Create(this->GetSafeHwnd(),(HICON)::LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICON_WALLPAPER),IMAGE_ICON,16,16,LR_DEFAULTCOLOR),WM_NOTIFYTASKBARICON,IDM_TRAY_MENU,IDI_ICON_BLANK,WALLPAPER_PROGRAM_TITLE))
				{
					char szString[512];
					::FormatResourceString(szString,sizeof(szString),IDS_TOOLTIP_CSS_STYLE);
					m_pTrayIcon->SetCssStyles(szString);

					m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_NERVOUS_TITLE_KEY,FALSE);
					m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_NERVOUS_TITLE_KEY);
					m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_INFAMOUS_SENTENCES_KEY,FALSE);
					m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_INFAMOUS_SENTENCES_KEY);

					OnNervousTitle(FALSE);
				}
				else
					delete m_pTrayIcon,m_pTrayIcon = NULL;
			}
		}
	}
	else
	{
		if(m_pTrayIcon)
			delete m_pTrayIcon,m_pTrayIcon = NULL;
	}

	DoNotAskMoreMessageBox(this->GetSafeHwnd(),IDS_MESSAGE_TRAY_ICON,0,&m_Config,WALLPAPER_DONOTASKMORE_TRAYICON_KEY,WALLPAPER_DONOTASKMORE_TRAYICON_VALUE_KEY,-1,MB_ICONINFORMATION);
}

/*
	OnMenuOptionsMinimizeOnClose()

	Aggiorna il flag per l'iconizzazione.
*/
void CWallPaperDlg::OnMenuOptionsMinimizeOnClose(void)
{
	m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_MINIMIZE_ON_CLOSE_KEY,!m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_MINIMIZE_ON_CLOSE_KEY));
	m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_MINIMIZE_ON_CLOSE_KEY);
}

/*
	OnMenuOptionsAudioNoPopup()

	Aggiorna il flag per il popup audio.
*/
void CWallPaperDlg::OnMenuOptionsAudioNoPopup(void)
{
	m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_AUDIOPOPUP_KEY,AUDIOPOPUP_NONE);
	m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_AUDIOPOPUP_KEY);

	// controlla se l'istanza del player si trova in esecuzione
	CWinThread* pWinThread = NULL;
	BOOL bHavePlayer = IsUIThreadRunning("CWallPaperPlayerUIThread",&pWinThread);
	if(bHavePlayer && pWinThread)
	{
		// notifica al player
		CWnd* pWnd = pWinThread->GetMainWnd();
		if(pWnd)
		{
			HWND hWnd = pWnd->GetSafeHwnd();
			if(hWnd)
				::SendMessage(hWnd,WM_AUDIOPLAYER_MODE,(WPARAM)AUDIOPLAYER_MODE_NOPOPUP,0L);
		}
	}
}

/*
	OnMenuOptionsAudioBalloonPopup()

	Aggiorna il flag per il balloon audio.
*/
void CWallPaperDlg::OnMenuOptionsAudioBalloonPopup(void)
{
	m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_AUDIOPOPUP_KEY,AUDIOPOPUP_BALLOON);
	m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_AUDIOPOPUP_KEY);

	// controlla se l'istanza del player si trova in esecuzione
	CWinThread* pWinThread = NULL;
	BOOL bHavePlayer = IsUIThreadRunning("CWallPaperPlayerUIThread",&pWinThread);
	if(bHavePlayer && pWinThread)
	{
		// notifica al player
		CWnd* pWnd = pWinThread->GetMainWnd();
		if(pWnd)
		{
			HWND hWnd = pWnd->GetSafeHwnd();
			if(hWnd)
			{
				::SendMessage(hWnd,WM_AUDIOPLAYER_MODE,(WPARAM)AUDIOPLAYER_MODE_BALLOON_POPUP,(LPARAM)&m_findAudioIcons);
				::SendMessage(hWnd,WM_AUDIOPLAYER_POPUPLIST,(WPARAM)AUDIOPLAYER_MODE_LOADBALLOONLIST,(LPARAM)NULL);
			}
		}
	}
}

/*
	OnMenuOptionsAudioReloadBalloon()

	Ricarica il balloon per il file audio.
*/
void CWallPaperDlg::OnMenuOptionsAudioReloadBalloon(void)
{
	// controlla se l'istanza del player si trova in esecuzione
	CWinThread* pWinThread = NULL;
	BOOL bHavePlayer = IsUIThreadRunning("CWallPaperPlayerUIThread",&pWinThread);
	if(bHavePlayer && pWinThread)
	{
		// notifica al player
		CWnd* pWnd = pWinThread->GetMainWnd();
		if(pWnd)
		{
			HWND hWnd = pWnd->GetSafeHwnd();
			if(hWnd)
				::SendMessage(hWnd,WM_AUDIOPLAYER_MODE,(WPARAM)AUDIOPLAYER_MODE_RELOAD_BALLOON,0L);
		}
	}
}

/*
	OnMenuOptionsAudioBalloonDefaultIcon()

	Imposta l'icona di default per il balloon popup.
*/
void CWallPaperDlg::OnMenuOptionsAudioBalloonDefaultIcon(void)
{
	// aggiorna il flag
	m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_BALLOON_AUDIO_ICON_TYPE_KEY,BALLOON_ICON_DEFAULT);
	m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_BALLOON_AUDIO_ICON_TYPE_KEY);
	
	// ripulisce la lista per le icone
	m_findAudioIcons.Reset();
	m_nCurrentAudioIcon = -1;
	m_nAudioIconsCount = 0;
}

/*
	OnMenuOptionsAudioBalloonCustomIcon()

	Imposta la directory da cui caricare le icone per il balloon popup.
*/
void CWallPaperDlg::OnMenuOptionsAudioBalloonCustomIcon(void)
{
	char szFolder[_MAX_FILEPATH+1] = {0};
	CDirDialog dlg(m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_BALLOON_AUDIO_ICON_DIR_KEY),
				"Select Directory...",
				"Select a folder containing icons:",
				m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_ALWAYSONTOP_KEY)
				);
select:

	// selezione della directory contenente le icone
	if(dlg.DoModal(this->GetSafeHwnd())==IDOK)
	{
		strcpyn(szFolder,dlg.GetPathName(),sizeof(szFolder));
		::EnsureBackslash(szFolder,sizeof(szFolder));
		m_nCurrentAudioIcon = -1;
		
		// cerca le icone nella directory specificata
		m_nAudioIconsCount = m_findAudioIcons.FindFile(szFolder,"*.ico",m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_RECURSE_SUBDIR_KEY));
		if(m_nAudioIconsCount > 0)
		{
			// aggiorna la configurazione
			m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_BALLOON_AUDIO_ICON_TYPE_KEY,BALLOON_ICON_CUSTOM);
			m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_BALLOON_AUDIO_ICON_TYPE_KEY);
			m_Config.UpdateString(WALLPAPER_OPTIONS_KEY,WALLPAPER_BALLOON_AUDIO_ICON_DIR_KEY,szFolder);
			m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_BALLOON_AUDIO_ICON_DIR_KEY);

			// controlla se l'istanza del player si trova in esecuzione
			CWinThread* pWinThread = NULL;
			BOOL bHavePlayer = IsUIThreadRunning("CWallPaperPlayerUIThread",&pWinThread);
			if(bHavePlayer && pWinThread)
			{
				// notifica al player
				CWnd* pWnd = pWinThread->GetMainWnd();
				if(pWnd)
				{
					HWND hWnd = pWnd->GetSafeHwnd();
					if(hWnd)
					{
						::SendMessage(hWnd,WM_AUDIOPLAYER_MODE,(WPARAM)AUDIOPLAYER_MODE_BALLOON_POPUP,(LPARAM)&m_findAudioIcons);
						::SendMessage(hWnd,WM_AUDIOPLAYER_POPUPLIST,(WPARAM)AUDIOPLAYER_MODE_LOADBALLOONLIST,(LPARAM)NULL);
					}
				}
			}
		}
		else
		{
			::MessageBoxResourceEx(this->GetSafeHwnd(),MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_NO_ICONS,szFolder);
			goto select;
		}
	}

	// resetta
	if(m_nAudioIconsCount <= 0)
	{
		m_findAudioIcons.Reset();
		m_nCurrentAudioIcon = -1;
		m_nAudioIconsCount = 0;
		
		m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_BALLOON_AUDIO_ICON_TYPE_KEY,BALLOON_ICON_DEFAULT);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_BALLOON_AUDIO_ICON_TYPE_KEY);
		m_Config.UpdateString(WALLPAPER_OPTIONS_KEY,WALLPAPER_BALLOON_AUDIO_ICON_DIR_KEY,"");
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_BALLOON_AUDIO_ICON_DIR_KEY);
	}
}

/*
	OnMenuOptionsAudioTaskbarPopup()

	Aggiorna il flag per il popup audio.
*/
void CWallPaperDlg::OnMenuOptionsAudioTaskbarPopup(void)
{
	m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_AUDIOPOPUP_KEY,AUDIOPOPUP_TASKBAR);
	m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_AUDIOPOPUP_KEY);

	// svuota la lista corrente	
	m_listAudioTaskbarBitmaps.RemoveAll();

	// estrae i bitmaps/.ini per i popups audio se non esistono
	int i;
	char szResource[_MAX_FILEPATH+1];
	char* popup[] = {
		"audiopopupcda",
		"audiopopupmp3",
		"audiopopupwav",
		NULL
		};
	int popupid[] = {
		IDR_AUDIOPOPUPCDA_PNG,
		IDR_AUDIOPOPUPMP3_PNG,
		IDR_AUDIOPOPUPWAV_PNG,
		-1
		};
	int popupiniid[] = {
		IDR_AUDIOPOPUPCDA_INI,
		IDR_AUDIOPOPUPMP3_INI,
		IDR_AUDIOPOPUPWAV_INI,
		-1
		};

	for(i = 0; popup[i]!=NULL; i++)
	{
		_snprintf(szResource,sizeof(szResource)-1,"%s%s.png",m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_AUDIOPOPUPDIR_KEY),popup[i]);
		if(!::FileExist(szResource))
			::ExtractResource(popupid[i],"GIF",szResource);
		_snprintf(szResource,sizeof(szResource)-1,"%s%s.ini",m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_AUDIOPOPUPDIR_KEY),popup[i]);
		if(!::FileExist(szResource))
			::ExtractResource(popupiniid[i],"TXT",szResource);
	}

	// carica la lista dei bitmaps per gli skins
	if(m_listAudioTaskbarBitmaps.Count() <= 0)
		LoadTaskbarPopupList(&m_listAudioTaskbarBitmaps,0);

	// controlla se l'istanza del player si trova in esecuzione
	CWinThread* pWinThread = NULL;
	BOOL bHavePlayer = IsUIThreadRunning("CWallPaperPlayerUIThread",&pWinThread);
	if(bHavePlayer && pWinThread)
	{
		// notifica al player
		CWnd* pWnd = pWinThread->GetMainWnd();
		if(pWnd)
		{
			HWND hWnd = pWnd->GetSafeHwnd();
			if(hWnd)
			{
				::SendMessage(hWnd,WM_AUDIOPLAYER_MODE,(WPARAM)AUDIOPLAYER_MODE_TASKBAR_POPUP,0L);
				::SendMessage(hWnd,WM_AUDIOPLAYER_POPUPLIST,(WPARAM)AUDIOPLAYER_MODE_LOADPOPUPLIST,(LPARAM)&m_listAudioTaskbarBitmaps);
			}
		}
	}
}

/*
	OnMenuOptionsAudioReloadTaskbarBitmapList()

	Ricarica la lista per i popup audio.
*/
void CWallPaperDlg::OnMenuOptionsAudioReloadTaskbarBitmapList(void)
{
	// ricarica la lista per i popups
	LoadTaskbarPopupList(&m_listAudioTaskbarBitmaps,0);

	// controlla se l'istanza del player si trova in esecuzione
	CWinThread* pWinThread = NULL;
	BOOL bHavePlayer = IsUIThreadRunning("CWallPaperPlayerUIThread",&pWinThread);
	if(bHavePlayer)
	{
		// notifica al player il nuovo contenuto della lista
		CWnd* pWnd = pWinThread->GetMainWnd();
		if(pWnd)
		{
			HWND hWnd = pWnd->GetSafeHwnd();
			if(hWnd)
				::SendMessage(hWnd,WM_AUDIOPLAYER_POPUPLIST,(WPARAM)AUDIOPLAYER_MODE_LOADPOPUPLIST,(LPARAM)&m_listAudioTaskbarBitmaps);
		}
	}
}

/*
	OnMenuOptionsPictNoPopup()

	Aggiorna il flag per il popup immagini.
*/
void CWallPaperDlg::OnMenuOptionsPictNoPopup(void)
{
	m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PICTPOPUP_KEY,PICTPOPUP_NONE);
	m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_PICTPOPUP_KEY);

	// rimuove tutti gli amenicoli
	if(m_pTrayIcon)
		m_pTrayIcon->Balloon();
	if(m_pBalloon)
		m_pBalloon->Balloon();
	if(m_pTaskbarNotifier)
		m_pTaskbarNotifier->UnShow();
}

/*
	OnMenuOptionsPictBalloonPopup()

	Aggiorna il flag per il balloon immagini.
*/
void CWallPaperDlg::OnMenuOptionsPictBalloonPopup(void)
{
	m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PICTPOPUP_KEY,PICTPOPUP_BALLOON);
	m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_PICTPOPUP_KEY);

	// rimuove tutti gli amenicoli
//	if(m_pTrayIcon)
//		m_pTrayIcon->Balloon();
//	if(m_pBalloon)
//		m_pBalloon->Balloon();
	if(m_pTaskbarNotifier)
		m_pTaskbarNotifier->UnShow();

	OnBalloon();
}

/*
	OnMenuOptionsPictBalloonDefaultIcon()

	Imposta l'icona di default per il balloon popup.
*/
void CWallPaperDlg::OnMenuOptionsPictBalloonDefaultIcon(void)
{
	// aggiorna il flag
	m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_BALLOON_PICT_ICON_TYPE_KEY,BALLOON_ICON_DEFAULT);
	m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_BALLOON_PICT_ICON_TYPE_KEY);
	
	// ripulisce la lista per le icone
	m_findPictIcons.Reset();
	m_nCurrentPictIcon = -1;
	m_nPictIconsCount = 0;
}

/*
	OnMenuOptionsPictBalloonCustomIcon()

	Imposta la directory da cui caricare le icone per il balloon popup.
*/
void CWallPaperDlg::OnMenuOptionsPictBalloonCustomIcon(void)
{
	char szFolder[_MAX_FILEPATH+1] = {0};
	CDirDialog dlg(m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_BALLOON_PICT_ICON_DIR_KEY),
				"Select Directory...",
				"Select a folder containing icons:",
				m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_ALWAYSONTOP_KEY)
				);
select:

	// selezione della directory contenente le icone
	if(dlg.DoModal(this->GetSafeHwnd())==IDOK)
	{
		strcpyn(szFolder,dlg.GetPathName(),sizeof(szFolder));
		::EnsureBackslash(szFolder,sizeof(szFolder));
		m_nCurrentPictIcon = -1;

		// cerca le icone nella directory specificata
		m_nPictIconsCount = m_findPictIcons.FindFile(szFolder,"*.ico",m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_RECURSE_SUBDIR_KEY));
		if(m_nPictIconsCount > 0)
		{
			// aggiorna la configurazione
			m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_BALLOON_PICT_ICON_TYPE_KEY,BALLOON_ICON_CUSTOM);
			m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_BALLOON_PICT_ICON_TYPE_KEY);
			m_Config.UpdateString(WALLPAPER_OPTIONS_KEY,WALLPAPER_BALLOON_PICT_ICON_DIR_KEY,szFolder);
			m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_BALLOON_PICT_ICON_DIR_KEY);
		}
		else
		{
			::MessageBoxResourceEx(this->GetSafeHwnd(),MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_NO_ICONS,szFolder);
			goto select;
		}
	}

	// resetta
	if(m_nPictIconsCount <= 0)
	{
		m_findPictIcons.Reset();
		m_nCurrentPictIcon = -1;
		m_nPictIconsCount = 0;
		
		m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_BALLOON_PICT_ICON_TYPE_KEY,BALLOON_ICON_DEFAULT);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_BALLOON_PICT_ICON_TYPE_KEY);
		m_Config.UpdateString(WALLPAPER_OPTIONS_KEY,WALLPAPER_BALLOON_PICT_ICON_DIR_KEY,"");
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_BALLOON_PICT_ICON_DIR_KEY);
	}
}

/*
	OnMenuOptionsPictTaskbarPopup()

	Aggiorna il flag per il popup per le immagini.
*/
void CWallPaperDlg::OnMenuOptionsPictTaskbarPopup(void)
{
	m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PICTPOPUP_KEY,PICTPOPUP_TASKBAR);
	m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_PICTPOPUP_KEY);

	// svuota la lista corrente	
	m_listPictTaskbarBitmaps.RemoveAll();

	// estrae i bitmaps/.ini per i popups audio se non esistono
	int i;
	char szResource[_MAX_FILEPATH+1];
	char* popup[] = {
		"pcpopup",
		"macpopup",
		NULL
		};
	int popupid[] = {
		IDR_PICTPCPOPUP_PNG,
		IDR_PICTMACPOPUP_PNG,
		-1
		};
	int popupiniid[] = {
		IDR_PICTPCPOPUP_INI,
		IDR_PICTMACPOPUP_INI,
		-1
		};

	for(i = 0; popup[i]!=NULL; i++)
	{
		_snprintf(szResource,sizeof(szResource)-1,"%s%s.png",m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_PICTPOPUPDIR_KEY),popup[i]);
		if(!::FileExist(szResource))
			::ExtractResource(popupid[i],"GIF",szResource);
		_snprintf(szResource,sizeof(szResource)-1,"%s%s.ini",m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_PICTPOPUPDIR_KEY),popup[i]);
		if(!::FileExist(szResource))
			::ExtractResource(popupiniid[i],"TXT",szResource);
	}

	// carica la lista dei bitmaps per gli skins
	if(m_listPictTaskbarBitmaps.Count() <= 0)
		LoadTaskbarPopupList(&m_listPictTaskbarBitmaps,1);

	// rimuove tutti gli amenicoli
	if(m_pTrayIcon)
		m_pTrayIcon->Balloon();
	if(m_pBalloon)
		m_pBalloon->Balloon();
//	if(m_pTaskbarNotifier)
//		m_pTaskbarNotifier->UnShow();

	OnTaskbarPopup();
}

/*
	OnMenuOptionsPictReloadTaskbarBitmapList()

	Ricarica la lista per i popup per le immagini.
*/
void CWallPaperDlg::OnMenuOptionsPictReloadTaskbarBitmapList(void)
{
	// ricarica la lista per i popups
	LoadTaskbarPopupList(&m_listPictTaskbarBitmaps,1);

	OnTaskbarPopup();
}

/*
	OnMenuOptionsGridlines()

	Aggiorna il flag per la griglia del dialogo.
*/
void CWallPaperDlg::OnMenuOptionsGridlines(void)
{
	m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_GRID_GRIDLINES_KEY,!m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_GRID_GRIDLINES_KEY));
	m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_GRID_GRIDLINES_KEY);
	
	DWORD dwStyle = m_wndPlaylist.GetExStyle();
	if(m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_GRID_GRIDLINES_KEY))
	{
		if(!(dwStyle & LVS_EX_GRIDLINES))
			dwStyle |= LVS_EX_GRIDLINES;
	}
	else
	{
		if(dwStyle & LVS_EX_GRIDLINES)
			dwStyle &= ~LVS_EX_GRIDLINES;
	}
	m_wndPlaylist.SetExStyle(dwStyle);
}

/*
	OnMenuOptionsGridlinesFgColor()

	Aggiorna il colore di primo piano per gli elementi della griglia.
*/
void CWallPaperDlg::OnMenuOptionsGridlinesFgColor(void)
{
	CColorDialog dlg;
	dlg.m_cc.Flags |= CC_RGBINIT;
	dlg.m_cc.Flags |= CC_FULLOPEN;
	dlg.m_cc.Flags |= CC_ANYCOLOR;
	dlg.m_cc.rgbResult = m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_GRID_FOREGROUNDCOLOR_KEY);
	
	if(dlg.DoModal()==IDOK)
	{
		m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_GRID_FOREGROUNDCOLOR_KEY,dlg.GetColor());
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_GRID_FOREGROUNDCOLOR_KEY);
		m_wndPlaylist.SetForegroundColor(m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_GRID_FOREGROUNDCOLOR_KEY));
		m_wndPlaylist.Invalidate(TRUE);
	}
}

/*
	OnMenuOptionsGridlinesBgColor()

	Aggiorna il colore di sfondo per gli elementi della griglia.
*/
void CWallPaperDlg::OnMenuOptionsGridlinesBgColor(void)
{
	CColorDialog dlg;
	dlg.m_cc.Flags |= CC_RGBINIT;
	dlg.m_cc.Flags |= CC_FULLOPEN;
	dlg.m_cc.Flags |= CC_ANYCOLOR;
	dlg.m_cc.rgbResult = m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_GRID_BACKGROUNDCOLOR_KEY);
	
	if(dlg.DoModal()==IDOK)
	{
		m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_GRID_BACKGROUNDCOLOR_KEY,dlg.GetColor());
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_GRID_BACKGROUNDCOLOR_KEY);
		m_wndPlaylist.SetBackgroundColor(m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_GRID_BACKGROUNDCOLOR_KEY));
		m_wndPlaylist.Invalidate(TRUE);
	}
}

/*
	OnMenuOptionsGridReset()

	Resetta i valori per la griglia.
*/
void CWallPaperDlg::OnMenuOptionsGridReset(void)
{
	m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_GRID_GRIDLINES_KEY,DEFAULTGRID_GRIDLINES);
	m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_GRID_GRIDLINES_KEY);
	DWORD dwStyle = m_wndPlaylist.GetExStyle();
	if(m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_GRID_GRIDLINES_KEY))
	{
		if(!(dwStyle & LVS_EX_GRIDLINES))
			dwStyle |= LVS_EX_GRIDLINES;
	}
	else
	{
		if(dwStyle & LVS_EX_GRIDLINES)
			dwStyle &= ~LVS_EX_GRIDLINES;
	}
	m_wndPlaylist.SetExStyle(dwStyle);

	m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_GRID_FOREGROUNDCOLOR_KEY,DEFAULTGRID_FOREGROUNDCOLOR);
	m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_GRID_FOREGROUNDCOLOR_KEY);
	m_wndPlaylist.SetForegroundColor(m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_GRID_FOREGROUNDCOLOR_KEY));

	m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_GRID_BACKGROUNDCOLOR_KEY,DEFAULTGRID_BACKGROUNDCOLOR);
	m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_GRID_BACKGROUNDCOLOR_KEY);
	m_wndPlaylist.SetBackgroundColor(m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_GRID_BACKGROUNDCOLOR_KEY));

	m_wndPlaylist.Invalidate(TRUE);
}

/*
	OnMenuOptionsClearDoNotAskMore()

	Azzera i flags utilizzati per le risposte automatiche.
*/
void CWallPaperDlg::OnMenuOptionsClearDoNotAskMore(void)
{
	m_Config.UpdateNumber(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_CLEANDPLFILE_KEY,0);
	m_Config.UpdateNumber(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_CLEANDPLFILE_VALUE_KEY,1);

	m_Config.UpdateNumber(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_CRAWLERLISTS_KEY,0);
	m_Config.UpdateNumber(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_CRAWLERLISTS_VALUE_KEY,1);

	m_Config.UpdateNumber(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_CRAWLERMODE_KEY,0);
	m_Config.UpdateNumber(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_CRAWLERMODE_VALUE_KEY,1);

	m_Config.UpdateNumber(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_CRAWLERMT_KEY,0);
	m_Config.UpdateNumber(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_CRAWLERMT_VALUE_KEY,1);

	m_Config.UpdateNumber(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_FEATURES_KEY,0);
	m_Config.UpdateNumber(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_FEATURES_VALUE_KEY,1);

	m_Config.UpdateNumber(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_LATESTVERSION_KEY,0);
	m_Config.UpdateNumber(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_LATESTVERSION_VALUE_KEY,1);

	m_Config.UpdateNumber(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_LOADBROWSER_KEY,0);
	m_Config.UpdateNumber(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_LOADBROWSER_VALUE_KEY,1);

	m_Config.UpdateNumber(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_PLAYRIPPEDSONGS_KEY,0);
	m_Config.UpdateNumber(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_PLAYRIPPEDSONGS_VALUE_KEY,1);

	m_Config.UpdateNumber(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_REMOVEFROMARCHIVE_KEY,0);
	m_Config.UpdateNumber(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_REMOVEFROMARCHIVE_VALUE_KEY,1);

	m_Config.UpdateNumber(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_RESETDPLFILE_KEY,0);
	m_Config.UpdateNumber(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_RESETDPLFILE_VALUE_KEY,1);

	m_Config.UpdateNumber(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_SCREENSAVER_KEY,0);
	m_Config.UpdateNumber(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_SCREENSAVER_VALUE_KEY,1);

	m_Config.UpdateNumber(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_TOOLTIP_MAIN_KEY,1);
	m_Config.UpdateNumber(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_TOOLTIP_MP3TAG_KEY,1);
	m_Config.UpdateNumber(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_TOOLTIP_CRAWLER_KEY,1);
	
	m_Config.UpdateNumber(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_TRAYICON_KEY,0);
	m_Config.UpdateNumber(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_TRAYICON_VALUE_KEY,1);

	m_Config.UpdateNumber(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_UNCOMPRESS_PICT_KEY,0);
	m_Config.UpdateNumber(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_UNCOMPRESS_PICT_VALUE_KEY,1);

	m_Config.SaveSection(WALLPAPER_DONOTASKMORE_KEY);
}

/*
	OnMenuOptionsOneClickChange()

	Aggiorna il flag per il cambio elemento con un solo click sulla icona della tray area.
*/
void CWallPaperDlg::OnMenuOptionsOneClickChange(void)
{
	m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_ONE_CLICK_CHANGE_KEY,!m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_ONE_CLICK_CHANGE_KEY));
	m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_ONE_CLICK_CHANGE_KEY);
}

/*
	OnMenuOptionsNervousTitle()

	Aggiorna il flag per il titolo a scorrimento.
*/
void CWallPaperDlg::OnMenuOptionsNervousTitle(void)
{
	if(!m_pTrayIcon)
	{
		m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_NERVOUS_TITLE_KEY,!m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_NERVOUS_TITLE_KEY));
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_NERVOUS_TITLE_KEY);
		
		if(!m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_NERVOUS_TITLE_KEY))
		{
			m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_INFAMOUS_SENTENCES_KEY,FALSE);
			m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_INFAMOUS_SENTENCES_KEY);
		}
		
		OnNervousTitle(m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_INFAMOUS_SENTENCES_KEY));
	}
	else
		::MessageBoxResource(this->GetSafeHwnd(),MB_OK|MB_ICONWARNING,WALLPAPER_PROGRAM_NAME,IDS_MESSAGE_TRAY_ICON_LIMITS);

	DoNotAskMoreMessageBox(this->GetSafeHwnd(),IDS_MESSAGE_TRAY_ICON,0,&m_Config,WALLPAPER_DONOTASKMORE_TRAYICON_KEY,WALLPAPER_DONOTASKMORE_TRAYICON_VALUE_KEY,-1,MB_ICONINFORMATION);
}

/*
	OnMenuOptionsContextMenu()

	Aggiorna il flag per il menu contestuale della shell.
*/
void CWallPaperDlg::OnMenuOptionsContextMenu(void)
{
	m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CONTEXT_MENU_KEY,!m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CONTEXT_MENU_KEY));
	m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_CONTEXT_MENU_KEY);
	OnContextMenu();
}

/*
	OnMenuOptionsPlaylistLoadingAdd()

	Aggiorna il flag per la modalita' di caricamento della playlist (.dpl).
*/
void CWallPaperDlg::OnMenuOptionsPlaylistLoadingAdd(void)
{
	m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PLAYLIST_LOADING_KEY,PLAYLIST_LOADING_ADD);
	m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_PLAYLIST_LOADING_KEY);
	m_pCmdLine->ResetDplFile();
	SetStatusBarDplPanel(NULL);
}

/*
	OnMenuOptionsPlaylistLoadingPut()

	Aggiorna il flag per la modalita' di caricamento della playlist (.dpl).
*/
void CWallPaperDlg::OnMenuOptionsPlaylistLoadingPut(void)
{
	m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PLAYLIST_LOADING_KEY,PLAYLIST_LOADING_PUT);
	m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_PLAYLIST_LOADING_KEY);
}

/*
	OnMenuOptionsRandomizePlaylistSelection()

	Aggiorna il flag per la riproduzione in ordine casuale (selezione).
*/
void CWallPaperDlg::OnMenuOptionsRandomizePlaylistSelection(void)
{
	m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_RANDOMIZE_PLAYLIST_KEY,RANDOMIZE_PLAYLIST_SELECTION);
	m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_RANDOMIZE_PLAYLIST_KEY);
}

/*
	ComparewParamItems()
*/
int ComparewParamItems(WPARAM w1,WPARAM w2)
{
	int nCmp = 0;
	if(w1 < w2)
		nCmp = -1;
	else if(w1 > w2)
		nCmp = 1;
	return(nCmp);
}

/*
	OnMenuOptionsRandomizePlaylistReorder()

	Aggiorna il flag per la riproduzione in ordine casuale (riordino).
*/
void CWallPaperDlg::OnMenuOptionsRandomizePlaylistReorder(void)
{
	// aggiorna la configurazione
	m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_RANDOMIZE_PLAYLIST_KEY,RANDOMIZE_PLAYLIST_REORDER);
	m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_RANDOMIZE_PLAYLIST_KEY);

	// elementi presenti nella playlist
	int nItemCount = m_wndPlaylist.GetItemCount();
	if(nItemCount > 0)
	{
		// riordina la playlist
		for(int i = 0; i < nItemCount; i++)
			m_wndPlaylist.SetItemwParam(i,(WPARAM)m_RandomNumberGenerator.RandRange(0,INT_MAX-1));
		m_wndPlaylist.SetwParamCallback(ComparewParamItems);
		m_wndPlaylist.Sort(SORT_WPARAM);

		// esegue quanto necessario dopo il riordino e riposiziona all'inizio
		OnPlaylistReorder(0,0);
		PlaylistSetModified();
	}
}

/*
	OnMenuOptionsRandomizePlaylistNone()

	Aggiorna il flag per la riproduzione in ordine sequenziale.
*/
void CWallPaperDlg::OnMenuOptionsRandomizePlaylistNone(void)
{
	m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_RANDOMIZE_PLAYLIST_KEY,RANDOMIZE_PLAYLIST_NONE);
	m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_RANDOMIZE_PLAYLIST_KEY);
}

/*
	OnMenuOptionsInfamousSentences()

	Aggiorna il flag per il titolo con le frasi a scorrimento.
*/
void CWallPaperDlg::OnMenuOptionsInfamousSentences(void)
{
	m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_INFAMOUS_SENTENCES_KEY,!m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_INFAMOUS_SENTENCES_KEY));
	m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_INFAMOUS_SENTENCES_KEY);
	OnNervousTitle(m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_INFAMOUS_SENTENCES_KEY));	
}

/*
	OnMenuOptionsRecurseSubdir()

	Aggiorna il flag per la ricerca ricorsiva nelle subdirectories.
*/
void CWallPaperDlg::OnMenuOptionsRecurseSubdir(void)
{
	m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_RECURSE_SUBDIR_KEY,!m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_RECURSE_SUBDIR_KEY));
	m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_RECURSE_SUBDIR_KEY);
}

/*
	OnMenuOptionsConfirmFileDelete()

	Aggiorna il flag per la conferma sull'eliminazione dei files.
*/
void CWallPaperDlg::OnMenuOptionsConfirmFileDelete(void)
{
	m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CONFIRM_FILE_DELETE_KEY,!m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CONFIRM_FILE_DELETE_KEY));
	m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_CONFIRM_FILE_DELETE_KEY);
}

/*
	OnMenuOptionsDeleteFilesToRecycleBin()

	Aggiorna il flag per l'eliminazione dei files nel cestino.
*/
void CWallPaperDlg::OnMenuOptionsDeleteFilesToRecycleBin(void)
{
	m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_DELETE_FILES_TO_RECYCLEBIN_KEY,!m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_DELETE_FILES_TO_RECYCLEBIN_KEY));
	m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_DELETE_FILES_TO_RECYCLEBIN_KEY);
}

/*
	OnMenuOptionsTempFilesClearAtExit()

	Aggiorna il flag per l'eliminazione dei files temporanei.
*/
void CWallPaperDlg::OnMenuOptionsTempFilesClearAtExit(void)
{
	m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CLEAR_TEMPORARY_FILES_KEY,TEMPORARY_FILES_CLEAR_AT_EXIT);
	m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_CLEAR_TEMPORARY_FILES_KEY);
}

/*
	OnMenuOptionsTempFilesClearAtRuntime()

	Aggiorna il flag per l'eliminazione dei files temporanei.
*/
void CWallPaperDlg::OnMenuOptionsTempFilesClearAtRuntime(void)
{
	m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CLEAR_TEMPORARY_FILES_KEY,TEMPORARY_FILES_CLEAR_AT_RUNTIME);
	m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_CLEAR_TEMPORARY_FILES_KEY);
}

/*
	OnMenuOptionsTempFilesDoNotClear()

	Aggiorna il flag per l'eliminazione dei files temporanei.
*/
void CWallPaperDlg::OnMenuOptionsTempFilesDoNotClear(void)
{
	m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CLEAR_TEMPORARY_FILES_KEY,TEMPORARY_FILES_DO_NOT_CLEAR);
	m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_CLEAR_TEMPORARY_FILES_KEY);
}

/*
	OnMenuOptionsProcessPriorityRealTime()

	Aggiorna il flag per la priorita' del processo.
*/
void CWallPaperDlg::OnMenuOptionsProcessPriorityRealTime(void)
{
	::SetPriorityClass(::GetCurrentProcess(),REALTIME_PRIORITY_CLASS);
	m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PRIORITY_PROCESS_KEY,REALTIME_PRIORITY_CLASS);
	m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_PRIORITY_PROCESS_KEY);
}

/*
	OnMenuOptionsProcessPriorityHigh()

	Aggiorna il flag per la priorita' del processo.
*/
void CWallPaperDlg::OnMenuOptionsProcessPriorityHigh(void)
{
	::SetPriorityClass(::GetCurrentProcess(),HIGH_PRIORITY_CLASS);
	m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PRIORITY_PROCESS_KEY,HIGH_PRIORITY_CLASS);
	m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_PRIORITY_PROCESS_KEY);
}

/*
	OnMenuOptionsProcessPriorityNormal()

	Aggiorna il flag per la priorita' del processo.
*/
void CWallPaperDlg::OnMenuOptionsProcessPriorityNormal(void)
{
	::SetPriorityClass(::GetCurrentProcess(),NORMAL_PRIORITY_CLASS);
	m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PRIORITY_PROCESS_KEY,NORMAL_PRIORITY_CLASS);
	m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_PRIORITY_PROCESS_KEY);
}

/*
	OnMenuOptionsProcessPriorityIdle()

	Aggiorna il flag per la priorita' del processo.
*/
void CWallPaperDlg::OnMenuOptionsProcessPriorityIdle(void)
{
	::SetPriorityClass(::GetCurrentProcess(),IDLE_PRIORITY_CLASS);
	m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PRIORITY_PROCESS_KEY,IDLE_PRIORITY_CLASS);
	m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_PRIORITY_PROCESS_KEY);
}

/*
	OnMenuOptionsThreadPriorityTimeCritical()

	Aggiorna il flag per la priorita' dei threads.
*/
void CWallPaperDlg::OnMenuOptionsThreadPriorityTimeCritical(void)
{
	m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PRIORITY_THREAD_KEY,THREAD_PRIORITY_TIME_CRITICAL);
	m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_PRIORITY_THREAD_KEY);
}

/*
	OnMenuOptionsThreadPriorityHighest()

	Aggiorna il flag per la priorita' dei threads.
*/
void CWallPaperDlg::OnMenuOptionsThreadPriorityHighest(void)
{
	m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PRIORITY_THREAD_KEY,THREAD_PRIORITY_HIGHEST);
	m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_PRIORITY_THREAD_KEY);
}

/*
	OnMenuOptionsThreadPriorityAboveNormal()

	Aggiorna il flag per la priorita' dei threads.
*/
void CWallPaperDlg::OnMenuOptionsThreadPriorityAboveNormal(void)
{
	m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PRIORITY_THREAD_KEY,THREAD_PRIORITY_ABOVE_NORMAL);
	m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_PRIORITY_THREAD_KEY);
}

/*
	OnMenuOptionsThreadPriorityNormal()

	Aggiorna il flag per la priorita' dei threads.
*/
void CWallPaperDlg::OnMenuOptionsThreadPriorityNormal(void)
{
	m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PRIORITY_THREAD_KEY,THREAD_PRIORITY_NORMAL);
	m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_PRIORITY_THREAD_KEY);
}

/*
	OnMenuOptionsThreadPriorityBelowNormal()

	Aggiorna il flag per la priorita' dei threads.
*/
void CWallPaperDlg::OnMenuOptionsThreadPriorityBelowNormal(void)
{
	m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PRIORITY_THREAD_KEY,(DWORD)THREAD_PRIORITY_BELOW_NORMAL);
	m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_PRIORITY_THREAD_KEY);
}

/*
	OnMenuOptionsThreadPriorityLowest()

	Aggiorna il flag per la priorita' dei threads.
*/
void CWallPaperDlg::OnMenuOptionsThreadPriorityLowest(void)
{
	m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PRIORITY_THREAD_KEY,(DWORD)THREAD_PRIORITY_LOWEST);
	m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_PRIORITY_THREAD_KEY);
}

/*
	OnMenuOptionsThreadPriorityIdle()

	Aggiorna il flag per la priorita' dei threads.
*/
void CWallPaperDlg::OnMenuOptionsThreadPriorityIdle(void)
{
	m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PRIORITY_THREAD_KEY,(DWORD)THREAD_PRIORITY_IDLE);
	m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_PRIORITY_THREAD_KEY);
}

/*
	OnMenuOptionsDrawSettings()

	Imposta le opzioni relative alla modalita' di visualizzazione.
*/
void CWallPaperDlg::OnMenuOptionsDrawSettings(void)
{
	char szLibraryName[_MAX_PATH+1];
	strcpyn(szLibraryName,m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_CURRENTLIBRARY_KEY),sizeof(szLibraryName));
	
	// dialogo per le opzioni
	CWallPaperDrawSettingsDlg dlg(this,&m_Config);
	if(dlg.DoModal()==IDOK)
	{
		// aggiorna il rettangolo per lo stretching
		m_rcDrawRect.SetRect(0,0,m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_DRAWRECTX_KEY),m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_DRAWRECTY_KEY));
		m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_DRAWRECTX_KEY,m_rcDrawRect.right);
		m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_DRAWRECTY_KEY,m_rcDrawRect.bottom);

		// aggiorna i valori per la trasparenza per il dialogo
		BYTE nLayered = LWA_ALPHA_OPAQUE;
		if(m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAYERED_KEY))
		{
			nLayered = (BYTE)m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAYERED_VALUE_KEY);
			if(nLayered >= LWA_ALPHA_INVISIBLE && nLayered <= LWA_ALPHA_OPAQUE)
			{
				BOOL bMustSetLayer = FALSE;
				if(nLayered < LWA_ALPHA_INVISIBLE_THRESHOLD)
					bMustSetLayer = ::MessageBoxResourceEx(this->GetSafeHwnd(),MB_YESNO|MB_ICONQUESTION,WALLPAPER_PROGRAM_NAME,IDS_QUESTION_LAYERED,nLayered,LWA_ALPHA_INVISIBLE,LWA_ALPHA_OPAQUE)==IDYES;
				else
					bMustSetLayer = TRUE;
				if(bMustSetLayer)
					m_wndLayered.SetLayer(this->GetSafeHwnd(),nLayered);
			}
		}
		else
			m_wndLayered.Reset();

		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAYERED_KEY);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAYERED_VALUE_KEY);

		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_DRAWMODE_KEY);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_DRAWAREARATIO_KEY);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_DRAWRECTX_KEY);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_DRAWRECTY_KEY);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_DRAWEFFECT_KEY);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_DRAWEFFECTVALUE_KEY);

		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_DRAWTEXTMODE_KEY);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_DRAWTEXTPOSITION_KEY);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_CF_iPointSize);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_CF_rgbColors);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_CF_nFontType);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfHeight);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfWidth);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfEscapement);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfOrientation);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfWeight);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfItalic);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfUnderline);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfStrikeOut);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfCharSet);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfOutPrecision);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfClipPrecision);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfQuality);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfPitchAndFamily);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfFaceName);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_TEXTCOLOR);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_BKCOLOR);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_BKCOLORTRANSPARENT);
	}
}

/*
	OnMenuOptionsThumbnailsSettings()

	Imposta le opzioni relative alle miniature.
*/
void CWallPaperDlg::OnMenuOptionsThumbnailsSettings(void)
{
	CWallPaperThumbSettingsDlg dlg(this,&m_Config);
	if(dlg.DoModal()==IDOK)
	{
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_X_KEY);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_Y_KEY);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_FORMAT_KEY);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_NUMBER_KEY);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_NAME_KEY);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_NAMING_KEY);

		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_HTMLFILE_KEY);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_GENERATETABLE_KEY);

		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_TABLETITLE_KEY);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_TABLEWIDTH_KEY);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_TABLECOLS_KEY);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_TABLEBORDER_KEY);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_TABLESPACING_KEY);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_TABLEPADDING_KEY);
	}
}

/*
	OnMenuOptionsCrawlerSettings()

	Imposta le opzioni relative al crawler.
*/
void CWallPaperDlg::OnMenuOptionsCrawlerSettings(void)
{
	CWallPaperCrawlerSettingsDlg dlg(this,&m_Config);
	if(dlg.DoModal()==IDOK)
	{
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_CONNECTIONRETRY_KEY);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_CONNECTIONTIMEOUT_KEY);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_USEWINSOCKTIMEOUT_KEY);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_COOKIES_ALLOWED_KEY);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_COOKIES_WARNINGONACCEPT_KEY);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_COOKIES_WARNINGONSEND_KEY);

		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_ADDPICTURES_KEY);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_ONLYCONTENT_KEY);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_MINSIZETYPE_KEY);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_MINSIZE_KEY);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_MAXSIZETYPE_KEY);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_MAXSIZE_KEY);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_SKIPEXISTING_KEY);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_SKIPEXISTING_FORCE_KEY);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_SKIPEXISTING_DONT_KEY);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_QUIETINSCRIPTMODE_KEY);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_ENABLECGI_KEY);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_ENABLEJAVASCRIPT_KEY);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_ENABLEROBOTSTXT_KEY);

		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_ENABLEJAVASCRIPT_KEY);

		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_FILETYPESACCEPTANCE_KEY);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_MULTITHREAD_KEY);

		m_Config.SaveSection(WALLPAPER_FILETYPELIST_KEY);
		m_Config.SaveSection(WALLPAPER_DOMAININCLUDELIST_KEY);
		m_Config.SaveSection(WALLPAPER_DOMAINEXCLUDELIST_KEY);

		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_DOMAINACCEPTANCE_KEY);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_INCLUDESUBDOMAINS_KEY);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_DOMAINUSEALWAYS_KEY);

		m_Config.SaveSection(WALLPAPER_PARENTINCLUDELIST_KEY);
		m_Config.SaveSection(WALLPAPER_PARENTEXCLUDELIST_KEY);

		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_PARENTACCEPTANCE_KEY);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_PARENTURLUSEALWAYS_KEY);

		m_Config.SaveSection(WALLPAPER_WILDCARDSEXCLUDELIST_KEY);

		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_SAVEINCLEXCLLIST_KEY);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_PROXY_ADDRESS_KEY);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_PROXY_PORTNUMBER_KEY);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_PROXY_USER_KEY);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_PROXY_PASSWORD_KEY);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_PROXY_EXCLUDEDOMAINS_KEY);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_USEPROXY_KEY);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_REPORTORDER_KEY);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_GENERATEASCII_KEY);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_GENERATEREPORT_KEY);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_GENERATETHUMBNAILS_KEY);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_DUMPDATABASE_KEY);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_EXTRACTTEXT_KEY);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_EXTRACTJAVASCRIPT_KEY);
	}
}

/*
	OnMenuOptionsSaveConfiguration()

	Salva la configurazione corrente nel file.
*/
void CWallPaperDlg::OnMenuOptionsSaveConfiguration(void)
{
	CFileSaveAsDialog dlg(	"Save Configuration File As...",
						m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_DIR_KEY),
						"*.cfg",
						"untitled",
						"Configuration files (*.cfg)|*.cfg||"
						);

	if(dlg.DoModal()==IDOK)
	{
		// ricava e salva la dimensione/posizione corrente della finestra
		WINDOWINFO wi = {0};
		GetWindowInfo(&wi);
		m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_WINDOW_POS_LEFT,wi.rcWindow.left);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_WINDOW_POS_LEFT);
		m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_WINDOW_POS_TOP,wi.rcWindow.top);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_WINDOW_POS_TOP);
		m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_WINDOW_POS_RIGHT,wi.rcWindow.right);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_WINDOW_POS_RIGHT);
		m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_WINDOW_POS_BOTTOM,wi.rcWindow.bottom);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_WINDOW_POS_BOTTOM);

		if(PlaylistIsModified())
			PlaylistSave();

		m_Config.Export(dlg.GetPathName());
	}
}

/*
	OnMenuOptionsLoadConfiguration()

	Carica la configurazione dal file.
*/
void CWallPaperDlg::OnMenuOptionsLoadConfiguration(void)
{
	CFileOpenDialog dlg("Open Configuration File...",
					m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_DIR_KEY),
					"*.cfg",
					"Configuration files (*.cfg)|*.cfg||"
					);

	if(dlg.DoModal()==IDOK)
	{
		m_Config.DeleteAll();			// elimina dalla lista
		m_Config.Load();				// carica la lista dal registro (usando i valori di default)
		m_Config.Import(dlg.GetPathName());// sovrascrive la lista con quanto presente nel file
		
		ApplySettings(FALSE);
	}
}

/*
	OnMenuHelpLatestVersionAtStartup()
*/
void CWallPaperDlg::OnMenuHelpLatestVersionAtStartup(void)
{
	m_Config.UpdateNumber(WALLPAPER_INSTALL_KEY,WALLPAPER_LATESTVERSIONCHECKTYPE_KEY,0L);
	m_Config.SaveKey(WALLPAPER_INSTALL_KEY,WALLPAPER_LATESTVERSIONCHECKTYPE_KEY);
}

/*
	OnMenuHelpLatestVersionEveryNDays()
*/
void CWallPaperDlg::OnMenuHelpLatestVersionEveryNDays(void)
{
	m_Config.UpdateNumber(WALLPAPER_INSTALL_KEY,WALLPAPER_LATESTVERSIONCHECKTYPE_KEY,DEFAULT_DAYS_FOR_LATESTVERSIONCHECK);
	m_Config.SaveKey(WALLPAPER_INSTALL_KEY,WALLPAPER_LATESTVERSIONCHECKTYPE_KEY);
}

/*
	OnMenuHelpLatestVersionNever()
*/
void CWallPaperDlg::OnMenuHelpLatestVersionNever(void)
{
	m_Config.UpdateNumber(WALLPAPER_INSTALL_KEY,WALLPAPER_LATESTVERSIONCHECKTYPE_KEY,1965L);
	m_Config.SaveKey(WALLPAPER_INSTALL_KEY,WALLPAPER_LATESTVERSIONCHECKTYPE_KEY);
}

/*
	OnPopupMenuLoad()

	Chiamata dal menu popup della playlist, carica l'elemento.
*/
void CWallPaperDlg::OnPopupMenuLoad(void)
{
	int nItem = m_wndPlaylist.GetCurrentItem();
	if(nItem >= 0)
		SendWallPaperMessage(nItem);
}

/*
	OnPopupMenuMoveUp()

	Chiamata dal menu popup della playlist, sposta l'elemento della lista in alto.
*/
void CWallPaperDlg::OnPopupMenuMoveUp(void)
{
	int nItem = m_wndPlaylist.GetCurrentItem();
	if(nItem >= 0)
	{
		m_wndPlaylist.MoveItemUp(nItem);
		PlaylistSetModified();
	}
}

/*
	OnPopupMenuMoveDown()

	Chiamata dal menu popup della playlist, sposta l'elemento della lista in basso.
*/
void CWallPaperDlg::OnPopupMenuMoveDown(void)
{
	int nItem = m_wndPlaylist.GetCurrentItem();
	if(nItem >= 0)
	{
		m_wndPlaylist.MoveItemDown(nItem);
		PlaylistSetModified();
	}
}

/*
	OnOpen()

	Apre l'elemento con il programma associato.
*/
void CWallPaperDlg::OnOpen(void)
{
	CWaitCursor cursor;
	int nItem;
	char szItem[MAX_ITEM_SIZE+1];
	
	// controlla la selezione corrente
	if((nItem = GetItem(szItem,sizeof(szItem))) >= 0)
	{
		// controlla se si tratta di un url
		if(m_Url.IsUrl(szItem))
		{
			// se l'url referenzia un immagine la passa prima in locale
			if(m_pImage->IsSupportedFormat(szItem))
			{
				char szFileName[_MAX_FILEPATH+1];

				SetWindowTitleID(IDS_MESSAGE_DOWNLOADING);
				SetStatusBarInfoPane(IDS_MESSAGE_DOWNLOADING);

				if(GetUrl(szItem,sizeof(szItem),szFileName,sizeof(szFileName)))
				{
					SetWindowTitleString(::StripPathFromFile(szItem));
					SetStatusBarInfoPane(IDS_MESSAGE_READY);
					CRegistry registry;
					if(!registry.OpenFileType(szFileName))
						::MessageBoxResourceEx(this->GetSafeHwnd(),MB_OK|MB_ICONWARNING,WALLPAPER_PROGRAM_NAME,IDS_MESSAGE_NO_ASSOCIATED_PROGRAM,szFileName);
				}
				else
				{
					SetWindowTitleID(IDS_ERROR_URL_RETRIEVE_FAILED);
					SetStatusBarInfoPane(IDS_MESSAGE_READY);
					::DeleteFile(szFileName);
					return;
				}
			}
			// se l'url referenzia una pagina web la apre col browser di default
			else
			{
				CBrowser browser(m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_PREFERRED_INTERNET_BROWSER_KEY));
				if(!browser.Browse(szItem))
					::MessageBoxResourceEx(this->GetSafeHwnd(),MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_BROWSER,szItem);
			}
		}
		// file locale, lo apre con il programma registrato
		else
		{
			CRegistry registry;
			if(!registry.OpenFileType(szItem))
				::MessageBoxResourceEx(this->GetSafeHwnd(),MB_OK|MB_ICONWARNING,WALLPAPER_PROGRAM_NAME,IDS_MESSAGE_NO_ASSOCIATED_PROGRAM,szItem);
		}
	}
}

/*
	OnSaveAs()

	Salva l'elemento della lista nel formato selezionato.
*/
void CWallPaperDlg::OnSaveAs(void)
{
	int nItem;
	char szItem[MAX_ITEM_SIZE+1];

	// controlla la selezione corrente	
	if((nItem = GetItem(szItem,sizeof(szItem))) < 0)
		return;

	// controlla se si tratta di un immagine
	if(m_pImage->IsSupportedFormat(szItem))
	{
		// salva il file in uno dei formati previsti
		CImageSaveAsDialog dlg(	this,
							NULL,
							m_pImage->GetLibraryName(),
							m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_SAVEAS_KEY)
							);
		if(dlg.DoModal()==IDOK)
		{
			CWaitCursor cursor;
			
			char szDir[_MAX_FILEPATH+1];
			strcpyn(szDir,dlg.GetPathName(),sizeof(szDir));
			strrev(szDir);
			int i = strchr(szDir,'\\') - szDir;
			strrev(szDir);
			if(i > 0)
				szDir[strlen(szDir)-i-1] = '\0';
			m_Config.UpdateString(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_SAVEAS_KEY,szDir);
			m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_SAVEAS_KEY);

			BOOL bExist = TRUE;
			
			// se l'immagine e' referenziata tramite un url la passa in locale
			if(m_Url.IsUrl(szItem))
			{
				char szFileName[_MAX_FILEPATH+1];

				SetWindowTitleID(IDS_MESSAGE_DOWNLOADING);
				SetStatusBarInfoPane(IDS_MESSAGE_DOWNLOADING);

				if(GetUrl(szItem,sizeof(szItem),szFileName,sizeof(szFileName)))
				{
					SetWindowTitleString(::StripPathFromFile(szItem));
					::CopyFile(szFileName,dlg.GetPathName(),FALSE);
					strcpyn(szItem,szFileName,sizeof(szItem));
				}
				else
				{
					SetWindowTitleID(IDS_ERROR_URL_RETRIEVE_FAILED);
					::DeleteFile(szFileName);
					bExist = FALSE;
				}
			}

			if(bExist)
			{
				SetStatusBarInfoPane(IDS_MESSAGE_SAVING_ITEM,szItem);
				CString c;
				c.Format(".%s",dlg.GetFileExt());
				dlg.SaveAs(szItem,dlg.GetPathName(),c);
			}
		}
	}
	// non e' un immagine
	else
	{
		// controlla se si tratta di un url
		if(m_Url.IsUrl(szItem))
		{
			// passa l'url in locale e la salva con il nome selezionato
			CFileSaveAsDialog dlg(	"Save As...",
								m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_SAVEAS_KEY),
								"*.htm",
								"untitled",
								"HTML files (*.htm)|*.htm||"
								);
			if(dlg.DoModal()==IDOK)
			{
				char szFileName[_MAX_FILEPATH+1];
			
				char szDir[_MAX_FILEPATH+1];
				strcpyn(szDir,dlg.GetPathName(),sizeof(szDir));
				strrev(szDir);
				int i = strchr(szDir,'\\') - szDir;
				strrev(szDir);
				if(i > 0)
					szDir[strlen(szDir)-i-1] = '\0';
				m_Config.UpdateString(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_SAVEAS_KEY,szDir);
				m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_SAVEAS_KEY);

				SetWindowTitleID(IDS_MESSAGE_DOWNLOADING);
				SetStatusBarInfoPane(IDS_MESSAGE_DOWNLOADING);

				if(GetUrl(szItem,sizeof(szItem),szFileName,sizeof(szFileName)))
				{
					SetWindowTitleString(::StripPathFromFile(szItem));
					::CopyFile(szFileName,dlg.GetPathName(),FALSE);
				}
				else
				{
					SetWindowTitleID(IDS_ERROR_URL_RETRIEVE_FAILED);
					::DeleteFile(szFileName);
				}
				
				SetStatusBarInfoPane(IDS_MESSAGE_READY);
			}
		}
	}

	SetStatusBarInfoPane(IDS_MESSAGE_READY);
}

/*
	OnDeleteFile()

	Elimina il file relativo all'elemento dalla lista.
*/
void CWallPaperDlg::OnDeleteFile(void)
{
	CWaitCursor cursor;
	int nItem;
	char szItem[MAX_ITEM_SIZE+1];
	
	// controlla la selezione corrente
	if((nItem = GetItem(szItem,sizeof(szItem))) >= 0)
	{
		char szLocation[MAX_ITEM_SIZE+1];
		m_wndPlaylist.GetItemText(nItem,CTRLLISTEX_LOCATION_INDEX,szLocation,sizeof(szLocation)-1);

		// i files contenuti in un file compresso non possono essere cancellati
		if(m_Archive.IsSupportedFormat(szLocation))
		{
			::MessageBoxResourceEx(this->GetSafeHwnd(),MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_DELETE_FILE_FROM_COMPRESSED_ARCHIVE,::StripPathFromFile(szItem));
			goto done;
			
		}
		// per le url cancella, se esiste, il file locale (quello corrispondente al download)
		if(m_Url.IsUrl(szItem))
		{
			char szFileName[_MAX_FILEPATH+1];
			char szDownloadDir[_MAX_PATH+1];
			strcpyn(szDownloadDir,m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_DOWNLOADDIR_KEY),sizeof(szDownloadDir));
			::EnsureBackslash(szDownloadDir,sizeof(szDownloadDir));
			_snprintf(szFileName,
					sizeof(szFileName)-1,
					"%s%s",
					szDownloadDir,
					::StripPathFromFile(szItem)
					);
			
			if(!::FileExist(szFileName))
			{
				::MessageBoxResourceEx(this->GetSafeHwnd(),MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_DELETE_URL,szItem);
				goto done;
			}
		}

		SetStatusBarInfoPane(IDS_MESSAGE_REMOVING);

		// rimuova l'elemento dalla lista (e dal player se si tratta di un file audio)
		OnRemove();

		// elimina il file
		BOOL bDelete = FALSE;
		BOOL bConfirmFileDelete = m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CONFIRM_FILE_DELETE_KEY);

		if(m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_DELETE_FILES_TO_RECYCLEBIN_KEY))
		{
			bDelete = ::DeleteFileToRecycleBin(this->GetSafeHwnd(),szItem,bConfirmFileDelete ? TRUE : FALSE);
		}
		else
		{
			if(bConfirmFileDelete)
				bDelete = ::MessageBoxResourceEx(this->GetSafeHwnd(),MB_YESNO|MB_ICONQUESTION,WALLPAPER_PROGRAM_NAME,IDS_QUESTION_DELETE_FILE,szItem)==IDYES;
			else
				bDelete = TRUE;
			if(bDelete)
				::DeleteFile(szItem);
		}
	}

done:

	SetStatusBarInfoPane(IDS_MESSAGE_READY);
}

/*
	OnDownloadProgress()
*/
LRESULT CWallPaperDlg::OnDownloadProgress(WPARAM /*wParam*/,LPARAM lParam)
{
	int nProgress = (int)lParam;
	char szBuffer[128] = {""};
	if(nProgress <= 100)
	{
		char szString[128];
		::FormatResourceString(szString,sizeof(szString),IDS_MESSAGE_DOWNLOAD_IN_PROGRESS);
		_snprintf(szBuffer,sizeof(szBuffer)-1,"%s (%d%%)",szString,nProgress);
	}
	SetWindowTitleString(szBuffer,FALSE,FALSE);
	SetStatusBarInfoPane(IDS_MESSAGE_DOWNLOADING);
	return(0L);
}

/*
	OnThreadParams()
*/
LRESULT CWallPaperDlg::OnThreadParams(WPARAM wParam,LPARAM lParam)
{
	ITERATOR iter;
	THREAD* thread;
	LRESULT lResult = NULL;

	if((iter = m_listRunningUIThreads.First())!=(ITERATOR)NULL)
	{
		do
		{
			thread = (THREAD*)iter->data;

			if(thread)
			{
				if(thread->pWinThread)
				{
					if(thread->dwThreadId==(DWORD)lParam)
					{
						if((int)wParam==0)
						{
							lResult = thread->lParam;
							break;
						}
					}
				}
			}

			iter = m_listRunningUIThreads.Next(iter);
		
		} while(iter!=(ITERATOR)NULL);
	}

	return(lResult);
}

/*
	OnQueryEndSession()
*/
LRESULT CWallPaperDlg::OnQueryEndSession(WPARAM /*wParam*/,LPARAM /*lParam*/)
{
	OnExit();
	
	if(m_hTerminateEvent)
	{
		if(::WaitForSingleObject(m_hTerminateEvent,10L)!=WAIT_TIMEOUT)
			return(0);
		else
			return(1);
	}

	return(1);
}

/*
	OnExit()

	Chiude il dialogo.
*/
void CWallPaperDlg::OnExit(void)
{
	// controlla che non ci siano threads (UI e interni) in esecuzione
	ITERATOR iter;
	THREAD* thread;
	DWORD dwStatus = 0L;

	// per sincronizzare *esclusivamente* i threads interni
	// crea l'evento e lo segnala, i threads interni controllano durante l'elaborazione lo stato
	// dell'evento, interrompendo l'elaborazione se segnalato, da parte sua il gestore sottostante
	// rimane in attesa fino a che il thread non termina
	m_hTerminateEvent = ::CreateEvent(NULL,TRUE,TRUE,NULL);

	// threads interni
	// controlla se ci sono threads in esecuzione, in tal caso si sincronizza creando
	// l'evento e aspettando il termine del thread (che sta in polling sull'evento)
	BOOL bHaveInternalThreadsRunning = FALSE;
	do {
		bHaveInternalThreadsRunning = FALSE;

		if((iter = m_listRunningInternalThreads.First())!=(ITERATOR)NULL)
		{
			do
			{
				thread = (THREAD*)iter->data;
			
				if(thread)
				{
					if(thread->pThread)
					{
						if(::GetExitCodeThread((thread->pThread)->GetHandle(),&dwStatus))
						{
							if(dwStatus==STILL_ACTIVE)
							{
								bHaveInternalThreadsRunning = TRUE;
								
								LPSTR lpThreadName = NULL;
								if(strcmp(thread->szThreadName,"LoadPlaylist")==0)
									lpThreadName = "playlist";
								else
									lpThreadName = "unknown";
								
								::PeekAndPump();
								::Sleep(65L);
							}
							else
								delete thread->pThread,thread->pThread = NULL;
						}
					}
				}

				iter = m_listRunningInternalThreads.Next(iter);
			
			} while(iter!=(ITERATOR)NULL);
		}
	} while(bHaveInternalThreadsRunning);

	// threads UI
	// controlla se ci sono threads in esecuzione, in tal caso l'utente
	// deve terminarli prima di poter chiudere l'applicazione principale
	if((iter = m_listRunningUIThreads.First())!=(ITERATOR)NULL)
	{
		do
		{
			thread = (THREAD*)iter->data;
			if(thread)
			{
				if(thread->pWinThread)
				{
					if(::GetExitCodeThread((thread->pWinThread)->m_hThread,&dwStatus))
					{
						if(dwStatus==STILL_ACTIVE)
						{
							LPSTR lpThreadName = NULL;
							if(strcmp(thread->szThreadName,"CWallPaperThumbnailsUIThread")==0)
								lpThreadName = "thumbnailer";
							else if(strcmp(thread->szThreadName,"CWallPaperCrawlerUIThread")==0)
								lpThreadName = "crawler";
							else if(strcmp(thread->szThreadName,"CWallPaperPlayerUIThread")==0 || strcmp(thread->szThreadName,"CWallPaperPreviewUIThread")==0)
							{
								lpThreadName = NULL;
								CWnd* pWnd = (thread->pWinThread)->GetMainWnd();
								if(pWnd)
								{
									HWND hWnd = pWnd->GetSafeHwnd();
									if(hWnd)
									{
										HANDLE hThread = (thread->pWinThread)->m_hThread;
										
										// send e non post per aspettare che termini il thread
										TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperDlg::OnExit(): sending WM_COMMAND\n"));
										::SendMessage(hWnd,WM_COMMAND,MAKELONG(IDM_MENU_EXIT,0),0L);
										
										if(hThread)
										{
											::WaitForSingleObject(hThread,INFINITE);
											delete thread->pWinThread,thread->pWinThread = NULL;
										}
									}
								}
							}
							else
								lpThreadName = "unknown";

							if(lpThreadName)
							{
								::MessageBoxResourceEx(this->GetSafeHwnd(),MB_OK|MB_ICONWARNING,WALLPAPER_PROGRAM_NAME,IDS_MESSAGE_CLOSE_UITHREAD_BEFORE_EXIT,lpThreadName);
								return;
							}
						}
						else
							delete thread->pWinThread,thread->pWinThread = NULL;
					}
				}
			}

			iter = m_listRunningUIThreads.Next(iter);
		
		} while(iter!=(ITERATOR)NULL);
	}
		
	if(m_pTrayIcon)
		delete m_pTrayIcon,m_pTrayIcon = NULL;

	// aggiorna la dimensione delle colonne
	if(m_pCmdLine->HaveDplFile())
	{
		::WritePrivateProfileInt(WALLPAPER_DPL_KEY,WALLPAPER_COLUMNWIDTH_FILENAME_KEY,m_wndPlaylist.GetColWidth(CTRLLISTEX_FILENAME_INDEX),m_pCmdLine->GetDplFile());
		::WritePrivateProfileInt(WALLPAPER_DPL_KEY,WALLPAPER_COLUMNWIDTH_SIZE_KEY,m_wndPlaylist.GetColWidth(CTRLLISTEX_SIZE_INDEX),m_pCmdLine->GetDplFile());
		::WritePrivateProfileInt(WALLPAPER_DPL_KEY,WALLPAPER_COLUMNWIDTH_LOCATION_KEY,m_wndPlaylist.GetColWidth(CTRLLISTEX_LOCATION_INDEX),m_pCmdLine->GetDplFile());
		::WritePrivateProfileInt(WALLPAPER_DPL_KEY,WALLPAPER_COLUMNWIDTH_TYPE_KEY,m_wndPlaylist.GetColWidth(CTRLLISTEX_TYPE_INDEX),m_pCmdLine->GetDplFile());
	}
	else
	{
		m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_COLUMNWIDTH_FILENAME_KEY,m_wndPlaylist.GetColWidth(CTRLLISTEX_FILENAME_INDEX));
		m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_COLUMNWIDTH_SIZE_KEY,m_wndPlaylist.GetColWidth(CTRLLISTEX_SIZE_INDEX));
		m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_COLUMNWIDTH_LOCATION_KEY,m_wndPlaylist.GetColWidth(CTRLLISTEX_LOCATION_INDEX));
		m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_COLUMNWIDTH_TYPE_KEY,m_wndPlaylist.GetColWidth(CTRLLISTEX_TYPE_INDEX));
	}
	
	// salva la playlist
	if(PlaylistIsModified())
		PlaylistSave();

	// ripulisce la directory per i temporanei (utilizzata per quanto estratto dal file compresso)
	if(m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CLEAR_TEMPORARY_FILES_KEY)==TEMPORARY_FILES_CLEAR_AT_EXIT)
	{
		char* pFileName;
		CFindFile findFile;
		char szTempFiles[_MAX_FILEPATH+1];
		strcpyn(szTempFiles,m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_TEMPDIR_KEY),sizeof(szTempFiles));
		while((pFileName = (LPSTR)findFile.FindEx(szTempFiles,"*.*",TRUE))!=NULL)
			::DeleteFile(pFileName);
	}

	// elimina il timer per lo sfondo
	if(m_nDesktopTimerId > 0)
	{
		KillTimer(m_nDesktopTimerId);
		m_nDesktopTimerId = 0;
	}

	// elimina il timer per il titolo della finestra
	if(m_nNervousTitleTimerId > 0)
	{
		KillTimer(m_nNervousTitleTimerId);
		m_nNervousTitleTimerId = 0;
	}

	// forza la visualizzazione delle icone del desktop
	m_bDesktopIcons = TRUE;
	OnDesktopIcons();

	::ResetEvent(m_hTerminateEvent);

	// hotkeys
	UnregisterHotKey(this->m_hWnd,HOTKEY_CTRL_ALT_MULTIPLY);
	UnregisterHotKey(this->m_hWnd,HOTKEY_CTRL_ALT_DIVIDE);
	UnregisterHotKey(this->m_hWnd,HOTKEY_CTRL_ALT_UP);
	UnregisterHotKey(this->m_hWnd,HOTKEY_CTRL_ALT_DOWN);
	UnregisterHotKey(this->m_hWnd,HOTKEY_CTRL_ALT_LEFT);
	UnregisterHotKey(this->m_hWnd,HOTKEY_CTRL_ALT_RIGHT);
	UnregisterHotKey(this->m_hWnd,HOTKEY_CTRL_ALT_BACKSPACE);
	UnregisterHotKey(this->m_hWnd,HOTKEY_CTRL_ALT_SPACE);
	UnregisterHotKey(this->m_hWnd,HOTKEY_CTRL_ALT_ADD);
	UnregisterHotKey(this->m_hWnd,HOTKEY_CTRL_ALT_SUBTRACT);

	// ricava e salva la dimensione/posizione corrente della finestra
	WINDOWINFO wi = {0};
	GetWindowInfo(&wi);
	m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_WINDOW_POS_LEFT,wi.rcWindow.left);
	m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_WINDOW_POS_LEFT);
	m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_WINDOW_POS_TOP,wi.rcWindow.top);
	m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_WINDOW_POS_TOP);
	m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_WINDOW_POS_RIGHT,wi.rcWindow.right);
	m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_WINDOW_POS_RIGHT);
	m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_WINDOW_POS_BOTTOM,wi.rcWindow.bottom);
	m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_WINDOW_POS_BOTTOM);

	// chiude il dialogo
	CDialogEx::OnExit();
}

/*
	OnCopy()

	Copia nella clipboard.
*/
void CWallPaperDlg::OnCopy(void)
{
	if(!IsCopyAvailable())
		return;

	CWaitCursor cursor;

	int nItem;
	char szItem[MAX_ITEM_SIZE+1];
	nItem = GetItem(szItem,sizeof(szItem));
	
	BOOL bComesFromArchiveFile = FALSE;
	char szLocation[MAX_ITEM_SIZE+1];
	m_wndPlaylist.GetItemText(nItem,CTRLLISTEX_LOCATION_INDEX,szLocation,sizeof(szLocation)-1);
	bComesFromArchiveFile = m_Archive.IsSupportedFormat(szLocation);

	// ricava l'elemento corrente ed apre la clipboard
	if(nItem >= 0)
	{
		if(::OpenClipboard(this->GetSafeHwnd()))
		{
			if(::EmptyClipboard())
			{
				HANDLE hClipboard = NULL;

				// immagine (copia il contenuto del bitmap)
				if(m_pImage->IsSupportedFormat(szItem) && !bComesFromArchiveFile)
				{
					CImage* pImage;
					CImageFactory ImageFactory;
					char szLibraryName[_MAX_PATH+1];
					char* p = (char*)m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_CURRENTLIBRARY_KEY);
					if(!strnull(p))
					{
						strcpyn(szLibraryName,p,sizeof(szLibraryName));
						pImage = ImageFactory.Create(szLibraryName,sizeof(szLibraryName));
					}
					else
						pImage = ImageFactory.Create();

					BOOL bHaveFile = TRUE;

					// se si tratta di un url, la scarica in locale prima di inviarla
					if(m_Url.IsUrl(szItem))
					{
						char szFileName[_MAX_FILEPATH+1];

						if(GetUrl(szItem,sizeof(szItem),szFileName,sizeof(szFileName)))
						{
							strcpyn(szItem,szFileName,sizeof(szItem));
							bHaveFile = TRUE;
						}
						else
						{
							SetWindowTitleID(IDS_ERROR_URL_RETRIEVE_FAILED);
							SetStatusBarInfoPane(IDS_MESSAGE_READY);
							::DeleteFile(szFileName);
							bHaveFile = FALSE;
						}
					}

					if(bHaveFile)
						if(pImage->Load(szItem))
						{
							HDIB hDib;
							if((hDib = pImage->GetDIB())!=NULL)
								hClipboard = ::SetClipboardData(CF_DIB,hDib);
						}
				}
				// immagine compressa (copia il contenuto del bitmap)
				if(m_pImage->IsSupportedFormat(szItem) && bComesFromArchiveFile)
				{
					char* pItem = szItem + strlen(szLocation) + 1;
					if(!*pItem)
						pItem = szItem;
					char szBuffer[1024];
					FormatResourceStringEx(szBuffer,sizeof(szBuffer),IDS_QUESTION_UNCOMPRESS_BEFORE_COPY,pItem);
					if(DoNotAskMoreMessageBox(this->GetSafeHwnd(),szBuffer,0,&m_Config,WALLPAPER_DONOTASKMORE_UNCOMPRESS_PICT_KEY,WALLPAPER_DONOTASKMORE_UNCOMPRESS_PICT_VALUE_KEY,1)==IDYES)
					{
						CImage* pImage;
						CImageFactory ImageFactory;
						char szLibraryName[_MAX_PATH+1];
						char* p = (char*)m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_CURRENTLIBRARY_KEY);
						if(!strnull(p))
						{
							strcpyn(szLibraryName,p,sizeof(szLibraryName));
							pImage = ImageFactory.Create(szLibraryName,sizeof(szLibraryName));
						}
						else
							pImage = ImageFactory.Create();

						BOOL bHaveFile = TRUE;

						// in lParam l'indice dell'elemento nella lista dei files compressi
						LPARAM lParam = m_wndPlaylist.GetItemlParam(nItem);
						if(lParam!=(LPARAM)-1L)
						{
							// ricava il nome del file compresso in cui si trova l'elemento
							LPCSTR pArchiveName = m_Archive.GetParent(lParam);
							if(pArchiveName)
							{
								// costruisce il nome dell'elemento (sul pathname temporaneo)
								char* p;
								char szItemFromCompressedFile[_MAX_FILEPATH+1];
								m_wndPlaylist.GetItemText(nItem,CTRLLISTEX_FILENAME_INDEX,szItemFromCompressedFile,sizeof(szItemFromCompressedFile)-1);
								while((p = strchr(szItemFromCompressedFile,'\\'))!=NULL)
									*p = '/';
								if((p = strrchr(szItemFromCompressedFile,'/'))!=NULL)
									p++;
								if(!p)
									p = szItemFromCompressedFile;
								_snprintf(szItem,sizeof(szItem)-1,"%s%s",m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_TEMPDIR_KEY),p);
								
								// estrae il file
								SetStatusBarInfoPane(IDS_MESSAGE_UNCOMPRESSING);
								LRESULT lRet = m_Archive.Extract(szItemFromCompressedFile,pArchiveName,m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_TEMPDIR_KEY));
								SetStatusBarInfoPane(IDS_MESSAGE_READY);
								if(lRet!=0L)
								{
									bHaveFile = FALSE;
									::MessageBoxResourceEx(this->GetSafeHwnd(),MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_EXTRACT_FILE,szItemFromCompressedFile,pArchiveName,m_Archive.GetErrorCodeDescription(lRet,pArchiveName),lRet);
									SetWindowTitleID(IDS_ERROR_EXTRACTFILE,szItemFromCompressedFile);
									SetStatusBarInfoPane(IDS_MESSAGE_READY);
								}
								else
								{
									bHaveFile = TRUE;
								}
							}
						}

						if(bHaveFile)
							if(pImage->Load(szItem))
							{
								HDIB hDib;
								if((hDib = pImage->GetDIB())!=NULL)
									hClipboard = ::SetClipboardData(CF_DIB,hDib);

								pImage->Unload();
								::DeleteFile(szItem);
							}
					}
				}

				// nome file (testo)
				{
					HGLOBAL hText = ::GlobalAlloc(GMEM_ZEROINIT|GMEM_MOVEABLE|GMEM_DDESHARE,strlen(szItem)+1);
					if(hText)
					{
						LPVOID p = ::GlobalLock(hText);
						if(p)
						{
							memcpy(p,szItem,strlen(szItem));
							::GlobalUnlock(hText);
							hClipboard = ::SetClipboardData(CF_TEXT,hText);
						}
					}
				}
				
				// nome file (shell)
				if(!m_Url.IsUrl(szItem) && !bComesFromArchiveFile)
				{
					// per copiare i files (in caso contrario la shell interpreta come spostamento)
					HGLOBAL hEffect = ::GlobalAlloc(GMEM_ZEROINIT|GMEM_MOVEABLE|GMEM_DDESHARE,sizeof(DWORD));
					DWORD *pDword = (DWORD*)::GlobalLock(hEffect);
					(*pDword) = DROPEFFECT_COPY;
					::GlobalUnlock(hEffect);
					hClipboard = ::SetClipboardData(::RegisterClipboardFormat(CFSTR_PREFERREDDROPEFFECT),hEffect);

					// costruisce la struttura per il drop dell'elemento (file) sulla shell
					DROPFILES dropfiles = {sizeof(DROPFILES),{0,0},0,0};
					int nLen = strlen(szItem);
					int nGlbLen = sizeof(DROPFILES) + nLen + 3;
					HGLOBAL hGlobal = ::GlobalAlloc(GMEM_ZEROINIT|GMEM_MOVEABLE|GMEM_DDESHARE,nGlbLen);
					char* pData = (char*)::GlobalLock(hGlobal);
					memcpy(pData,&dropfiles,sizeof(DROPFILES));
					pData += sizeof(DROPFILES);
					memcpy(pData,szItem,nLen);
					::GlobalUnlock(hGlobal);
					hClipboard = ::SetClipboardData(CF_HDROP,hGlobal);
				}
			}

			::CloseClipboard();
		}
	}
}

/*
	OnPaste()

	Incolla dalla clipboard.
*/
void CWallPaperDlg::OnPaste(void)
{
	if(!IsPasteAvailable())
		return;

	CWaitCursor cursor;

	// flag per file aggiunto e totale elementi nella lista prima dell'operazione
	BOOL bAdded = FALSE;
	int nItemCount = m_wndPlaylist.GetItemCount(); 

	// apre la clipboard ricavando il formato disponibile
	if(m_enumClipboardFormat!=CLIPBOARD_UNAVAILABLE)
	{
		if(::OpenClipboard(NULL))
		{
			HGLOBAL hClipboard = (HGLOBAL)NULL;
			switch(m_enumClipboardFormat)
			{
				case CLIPBOARD_PICTURE_FORMAT:
					hClipboard = ::GetClipboardData(CF_DIB);
					break;
				case CLIPBOARD_FILEDROP_FORMAT:
					hClipboard = ::GetClipboardData(CF_HDROP);
					break;
				case CLIPBOARD_URL_NETSCAPE_FORMAT:
					hClipboard =  ::GetClipboardData(m_nNetscapeClipboardFormat);
					break;
				case CLIPBOARD_URL_IEXPLORE_FORMAT:
					hClipboard = ::GetClipboardData(m_nExplorerClipboardFormat);
					break;
			}
			::CloseClipboard();

			if(hClipboard)
				switch(m_enumClipboardFormat)
				{
					// immagine
					case CLIPBOARD_PICTURE_FORMAT:
					{
						// crea una nuova immagine per non interferire con quella corrente
						CImage *pImage;
						CImageFactory ImageFactory;
						char szLibraryName[_MAX_PATH+1];
						char* p = (char*)m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_CURRENTLIBRARY_KEY);
						if(!strnull(p))
						{
							strcpyn(szLibraryName,p,sizeof(szLibraryName));
							pImage = ImageFactory.Create(szLibraryName,sizeof(szLibraryName));
						}
						else
							pImage = ImageFactory.Create();

						// imposta l'immagine con il contenuto della clipboard
						if(pImage->SetDIB((HDIB)hClipboard))
						{
							CImageSaveAsDialog dlg(	this,
												NULL,
												pImage->GetLibraryName(),
												m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_SAVEAS_KEY)
												);
							if(dlg.DoModal()==IDOK)
							{
								CWaitCursor cursor;
								
								char szPathName[_MAX_FILEPATH+1];
								strcpyn(szPathName,dlg.GetPathName(),sizeof(szPathName));
								strrev(szPathName);
								int i = strchr(szPathName,'\\') - szPathName;
								strrev(szPathName);
								if(i > 0)
									szPathName[strlen(szPathName)-i-1] = '\0';
								m_Config.UpdateString(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_SAVEAS_KEY,szPathName);
								m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_SAVEAS_KEY);

								char szExt[_MAX_EXT+1];
								_snprintf(szExt,sizeof(szExt)-1,".%s",dlg.GetFileExt());

								// salva l'immagine su disco e aggiorna la barra di stato
								SetStatusBarInfoPane(IDS_MESSAGE_SAVING_ITEM,dlg.GetPathName());
								bAdded = pImage->Save(dlg.GetPathName(),szExt);
								SetStatusBarInfoPane(IDS_MESSAGE_READY);
								if(bAdded)
								{
									// stabilisce (e posiziona la barra sul) l'elemento corrente
									if(nItemCount <= 0)
									{
										m_wndPlaylist.SelectItem(0);
										SendWallPaperMessage(WALLPAPER_GOTO_FIRST_PICTURE);
									}
									
									// aggiorna la barra di stato
									SetStatusBarPicturePanelCount(nItemCount);

									// aggiorna il dialogo
									OnUpdateGUI();
									PlaylistSetModified();
								}
								else
									::MessageBoxResource(this->GetSafeHwnd(),MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_SAVE_PICTURE);
							}
						}

						break;
					}

					// nomi files (dalla shell)
					case CLIPBOARD_FILEDROP_FORMAT:
					{
						UINT nTotDropped = 0;
						int nTotFiles = 0;

						if(::GlobalLock((HDROP)hClipboard))
						{
							// ricava il numero di files copiati nella clipboard dalla shell
							nTotDropped = ::DragQueryFile((HDROP)hClipboard,(UINT)-1,NULL,0);
							
							if(nTotDropped!=(UINT)-1)
							{
								CFileList filelist;
								FILELIST* f;
								ITERATOR iter;

								// con W98 viene generata un eccezione in SHELL32.DLL se i files vengono aggiunti al
								// controllo nello stesso ciclo utilizzato per ricavare i nomi con DragQueryFile()
								for(int i = 0; i < (int)nTotDropped; i++)
								{
									f = new FILELIST;
									if(f)
									{
										memset(f,'\0',sizeof(FILELIST));
										::DragQueryFile((HDROP)hClipboard,i,f->file,_MAX_FILEPATH);
										f->attribute = ::GetFileAttributes(f->file);
										filelist.Add(f);
									}
								}

								CAudioFilesList* pAudioFilesList = NULL;
								
								if((iter = filelist.First())!=(ITERATOR)NULL)
								{
									do
									{
										f = (FILELIST*)iter->data;
										if(f)
										{
											if(f->attribute & FILE_ATTRIBUTE_DIRECTORY)
											{
												nTotFiles += AddDir(f->file,m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_RECURSE_SUBDIR_KEY));
											}
											else if(striright(f->file,DPL_EXTENSION)==0)
											{
												m_pCmdLine->SetDplFile(f->file);
												// non lancia la versione thread perche' deve ottenere il numero di elementi
												//::PostMessage(this->GetSafeHwnd(),WM_LOADPLAYLIST,0L,0L);
												nTotFiles = LoadPlaylist();
											}
											// controlla se si tratta di un file audio (inserendolo nella lista per il player)
											else if(CAudioPlayer::IsSupportedFormat(f->file))
											{
												// aggiunge l'item alla lista (non controlla i duplicati)
												SetStatusBarInfoPane(IDS_MESSAGE_ADDING_ITEM,::StripPathFromFile(f->file));
												int nItem = m_wndPlaylist.AddItem(FormatPlaylistItem(f->file),GetIconIndex(f->file,&m_listPlaylistIcons,&m_wndPlaylist),IDM_WALLPAPER_POPUP_MENU,m_wndPlaylist.GetItemCount());
												if(nItem==LB_ERR)
												{
													::MessageBoxResourceEx(this->GetSafeHwnd(),MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_ADDITEM,f->file);
												}
												else
												{
													if(!pAudioFilesList)
														pAudioFilesList = new CAudioFilesList();
													if(pAudioFilesList)
													{
														AUDIOITEM* audioItem = (AUDIOITEM*)pAudioFilesList->Add();
														if(audioItem)
														{
															strcpyn(audioItem->file,f->file,_MAX_FILEPATH+1);
															audioItem->index = pAudioFilesList->Count();
															audioItem->lparam = (LPARAM)-1L;
															nTotFiles++;
														}
													}
												}
											}
											else
											{
												if(AddFile(f->file) >= 0)
													nTotFiles++;
											}
										}

										iter = filelist.Next(iter);
									
									} while(iter!=(ITERATOR)NULL);
								}

								// passa la lista dei files audio al player
								if(pAudioFilesList)
								{
									if(pAudioFilesList->Count() > 0)
										OnPlayer(NULL,pAudioFilesList,AUDIOPLAYER_COMMAND_QUEUE_FROM_LIST,TRUE);
									delete pAudioFilesList;
								}
							}

							::GlobalUnlock(hClipboard);
						}
						
						// stabilisce (e posiziona la barra sul) l'elemento corrente
						bAdded = nTotFiles > 0;
						if(bAdded && nItemCount <= 0)
						{
							m_wndPlaylist.SelectItem(0);
							SendWallPaperMessage(WALLPAPER_GOTO_FIRST_PICTURE);
						}

						// aggiorna la barra di stato
						SetStatusBarPicturePanelCount();
						SetStatusBarInfoPane(IDS_MESSAGE_FILEDROPPED,(UINT)nTotFiles);

						// aggiorna il dialogo
						OnUpdateGUI();
						PlaylistSetModified();

						break;
					}

					// url 
					case CLIPBOARD_URL_NETSCAPE_FORMAT:
					case CLIPBOARD_URL_IEXPLORE_FORMAT:
					{
						LPVOID p = ::GlobalLock(hClipboard);
						if(p)
						{
							char szUrl[MAX_URL+1];
							strcpyn(szUrl,(char*)p,sizeof(szUrl));
							::GlobalUnlock(hClipboard);
							bAdded = AddUrl(szUrl);
						}
						
						// stabilisce (e posiziona la barra sul) l'elemento corrente
						if(bAdded && nItemCount <= 0)
						{
							m_wndPlaylist.SelectItem(0);
							SendWallPaperMessage(WALLPAPER_GOTO_FIRST_PICTURE);
						}

						// aggiorna la barra di stato
						SetStatusBarPicturePanelCount();
						SetStatusBarInfoPane(IDS_MESSAGE_URLDROPPED,(UINT)1);

						// aggiorna il dialogo
						OnUpdateGUI();
						PlaylistSetModified();

						break;
					}
				}
		}
	}
}

/*
	OnEmptyClipboard()

	Svuota la clipboard.
*/
void CWallPaperDlg::OnEmptyClipboard(void)
{
	if(::OpenClipboard(NULL))
	{
		::EmptyClipboard();
		::CloseClipboard();
	}
}

/*
	IsCopyAvailable()

	Verifica se i dati possono essere messi nella clipboard.
*/
BOOL CWallPaperDlg::IsCopyAvailable(BOOL* bIsPicture/* = NULL*/)
{
	int nItem;
	char szItem[MAX_ITEM_SIZE+1];
	BOOL bCopyAvailable = FALSE;
	
	if((nItem = GetItem(szItem,sizeof(szItem))) >= 0)
	{
		bCopyAvailable = TRUE;
		if(bIsPicture!=NULL)
			*bIsPicture = m_pImage->IsSupportedFormat(szItem);
	}

	return(bCopyAvailable);
}

/*
	IsPasteAvailable()

	Verifica se la clipboard contiene dati.
*/
BOOL CWallPaperDlg::IsPasteAvailable(void)
{
	BOOL bAvailable = FALSE;
	m_enumClipboardFormat = CLIPBOARD_UNAVAILABLE;

	if(!bAvailable)
		if(::IsClipboardFormatAvailable(CF_DIB))
		{
			bAvailable = TRUE;
			m_enumClipboardFormat = CLIPBOARD_PICTURE_FORMAT;
		}

	if(!bAvailable)
		if(::IsClipboardFormatAvailable(CF_HDROP))
		{
			bAvailable = TRUE;
			m_enumClipboardFormat = CLIPBOARD_FILEDROP_FORMAT;
		}
	
	if(!bAvailable)
		if(::IsClipboardFormatAvailable(m_nNetscapeClipboardFormat))
		{
			bAvailable = TRUE;
			m_enumClipboardFormat = CLIPBOARD_URL_NETSCAPE_FORMAT;
		}
	
	if(!bAvailable)
		if(::IsClipboardFormatAvailable(m_nExplorerClipboardFormat))
		{
			bAvailable = TRUE;
			m_enumClipboardFormat = CLIPBOARD_URL_IEXPLORE_FORMAT;
		}

	return(bAvailable);
}

/*
	OnPause()

	Sospende il cambio dello sfondo.
*/
LRESULT CWallPaperDlg::OnPause(WPARAM /*wParam*/,LPARAM /*lParam*/)
{
	CWaitCursor cursor;

	// disabilita l'interfaccia
	OnEnableGUI(FALSE,MAKELPARAM(MF_GRAYED,MF_ENABLED));

	// aggiorna il menu di sistema
	CMenu* pSysmenu = GetSystemMenu(FALSE);
	if(pSysmenu)
	{
		// elimina 'Pause' ed inserisce 'Resume'
		pSysmenu->RemoveMenu(IDM_SYSMENU_PAUSERESUME_POSITION,MF_BYPOSITION);
		char szMenuItem[_MAX_MENU_STRING_LENGTH];
		_snprintf(szMenuItem,sizeof(szMenuItem)-1,"&Resume");
		pSysmenu->InsertMenu(IDM_SYSMENU_PAUSERESUME_POSITION,MF_BYPOSITION|MF_STRING,IDM_SYSMENU_RESUME,szMenuItem);
		
		// elimina 'Favourite', 'Next' e 'Previous'
		pSysmenu->RemoveMenu(IDM_SYSMENU_FAVOURITE_POSITION,MF_BYPOSITION);
		pSysmenu->RemoveMenu(IDM_SYSMENU_FAVOURITE_POSITION,MF_BYPOSITION);
		pSysmenu->RemoveMenu(IDM_SYSMENU_FAVOURITE_POSITION,MF_BYPOSITION);
	}

	// aggiorna l'icona del dialogo
	CDialogEx::SetIcon(IDI_ICON_PAUSED);
	if(m_pTrayIcon)
		m_pTrayIcon->SetIcon(IDI_ICON_PAUSED);

	// aggiorna lo status corrente
	m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_STATE_KEY,WALLPAPER_IN_PAUSED_STATE);
	m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_STATE_KEY);

	// aggiorna il titolo del dialogo
	char szTitle[128];
	char szString[128];
	::FormatResourceString(szString,sizeof(szString),IDS_MESSAGE_PAUSED);
	_snprintf(szTitle,sizeof(szTitle)-1," (%s)",szString);
	SetWindowTitleString(szTitle,FALSE,TRUE);

	return(0L);
}

/*
	OnResume()

	Ripristina il cambio dello sfondo.
*/
void CWallPaperDlg::OnResume(void)
{
	CWaitCursor cursor;

	// abilita l'interfaccia
	OnEnableGUI(TRUE,MAKELPARAM(MF_ENABLED,MF_GRAYED));

	// aggiorna il menu di sistema
	CMenu* pSysmenu = GetSystemMenu(FALSE);
	if(pSysmenu)
	{
		// elimina 'Resume' ed inserisce 'Pause'
		char szMenuItem[_MAX_MENU_STRING_LENGTH];
		pSysmenu->RemoveMenu(IDM_SYSMENU_PAUSERESUME_POSITION,MF_BYPOSITION);
		_snprintf(szMenuItem,sizeof(szMenuItem)-1,"Pa&use");
		pSysmenu->InsertMenu(IDM_SYSMENU_PAUSERESUME_POSITION,MF_BYPOSITION|MF_STRING,IDM_SYSMENU_PAUSE,szMenuItem);
		
		// inserisce 'Favourite', 'Next' e 'Previous'
		_snprintf(szMenuItem,sizeof(szMenuItem)-1,"Add to &Favourite");
		pSysmenu->InsertMenu(IDM_SYSMENU_FAVOURITE_POSITION,MF_BYPOSITION|MF_STRING,IDM_SYSMENU_FAVOURITE,szMenuItem);
		_snprintf(szMenuItem,sizeof(szMenuItem)-1,"Pre&vious");
		pSysmenu->InsertMenu(IDM_SYSMENU_NEXTPICT_POSITION,MF_BYPOSITION|MF_STRING,IDM_SYSMENU_PREVPIC,szMenuItem);
		_snprintf(szMenuItem,sizeof(szMenuItem)-1,"&Next");
		pSysmenu->InsertMenu(IDM_SYSMENU_PREVPICT_POSITION,MF_BYPOSITION|MF_STRING,IDM_SYSMENU_NEXTPIC,szMenuItem);
	}

	// aggiorna l'icona del dialogo
	CDialogEx::SetIcon(IDI_ICON_WALLPAPER);
	if(m_pTrayIcon)
		m_pTrayIcon->SetIcon(IDI_ICON_WALLPAPER);

	// status corrente
	m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_STATE_KEY,WALLPAPER_IN_ACTIVE_STATE);
	m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_STATE_KEY);

	// aggiorna il titolo del dialogo
	SetWindowTitleString(m_strTitleBar);
}

/*
	OnAddFile()

	Aggiunge un file alla lista.
*/
void CWallPaperDlg::OnAddFile(void)
{
	// flag per file aggiunto e totale elementi nella lista prima dell'operazione
	BOOL bAdded = FALSE;

	int nItemCount = m_wndPlaylist.GetItemCount(); 

	CFileOpenDialog dlg("Open File...",
					m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_ADDFILE_KEY),
					"*.*",
					"All files (*.*)|*.*|"
					"JPEG picture files (*.jpg;*.jpeg)|*.jpg;*.jpeg|"
					"PNG picture files (*.png)|*.png|"
					"BMP bitmaps files (*.bmp)|*.bmp|"
					"DPL playlist files (*.dpl)|*.dpl|"
					"GZW compressed files (*.gzw)|*.gzw|"
					"RAR compressed files (*.rar)|*.rar|"
					"ZIP compressed files (*.zip)|*.zip|"
					"MP3 audio files (*.mp3)|*.mp3|"
					"WAV audio files (*.wav)|*.wav|"
					"CDA audio CD tracks (*.cda)|*.cda||"
					);

	if(dlg.DoModal()==IDOK)
	{
		// distingue tra playlist (.dpl) e altri files
		if(striright(dlg.GetPathName(),DPL_EXTENSION)==0)
		{
			m_pCmdLine->SetDplFile(dlg.GetPathName());
			::PostMessage(this->GetSafeHwnd(),WM_LOADPLAYLIST,0L,0L);
			PlaylistSetModified();
			return;
		}
		else
		{
			SetStatusBarInfoPane(IDS_MESSAGE_ADDING_ITEM,dlg.GetPathName());
			bAdded = AddFile(dlg.GetPathName()) >= 0;
		}
	}

	if(bAdded)
	{
		char szDir[_MAX_FILEPATH+1];
		strcpyn(szDir,dlg.GetPathName(),sizeof(szDir));
		strrev(szDir);
		int i = strchr(szDir,'\\') - szDir;
		strrev(szDir);
		if(i > 0)
			szDir[strlen(szDir)-i-1] = '\0';
		m_Config.UpdateString(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_ADDFILE_KEY,szDir);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_ADDFILE_KEY);

		// stabilisce (e posiziona la barra sul) l'elemento corrente
		if(nItemCount <= 0)
		{
			m_wndPlaylist.SelectItem(0);
			SendWallPaperMessage(WALLPAPER_GOTO_FIRST_PICTURE);
		}

		// aggiorna il dialogo
		OnUpdateGUI();
		PlaylistSetModified();
	}
	
	SetStatusBarInfoPane(IDS_MESSAGE_READY);
}

/*
	OnAddFile()

	Aggiunge un file alla lista.
*/
LRESULT CWallPaperDlg::OnAddFile(WPARAM wParam/* int nPlayMode */,LPARAM lParam/* LPCSTR lpcszFileName */)
{
	if((LPSTR)lParam)
	{	
		// flag per file aggiunto e totale elementi nella lista prima dell'operazione
		BOOL bAdded = FALSE;
		int nItemCount = m_wndPlaylist.GetItemCount(); 
		
		char szFileName[_MAX_FILEPATH+1];
		strcpyn(szFileName,(LPSTR)lParam,sizeof(szFileName));
		
		SetStatusBarInfoPane(IDS_MESSAGE_ADDING_ITEM,szFileName);
		bAdded = AddFile(szFileName,(int)wParam) >= 0;

		if(bAdded)
		{
			// stabilisce (e posiziona la barra sul) l'elemento corrente
			if(nItemCount <= 0)
			{
				m_wndPlaylist.SelectItem(0);
				// diverso da zero per i file audio, dove wParam specifica la modalita' di riproduzione
				if((int)wParam==0)
					SendWallPaperMessage(WALLPAPER_GOTO_FIRST_PICTURE);
			}

			// aggiorna il dialogo
			OnUpdateGUI();
			PlaylistSetModified();
		}
		
		SetStatusBarInfoPane(IDS_MESSAGE_READY);
	}

	return(0L);
}

/*
	OnAddDir()

	Aggiunge il contenuto della directory alla lista.
*/
void CWallPaperDlg::OnAddDir(void)
{
	char szFolder[_MAX_FILEPATH+1] = {0};
	CDirDialog dlg(m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_ADDDIR_KEY),
				"Open Directory...",
				"Select a folder:",
				m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_ALWAYSONTOP_KEY)
				);

	// selezione della directory
	if(dlg.DoModal(this->GetSafeHwnd())==IDOK)
	{
		strcpyn(szFolder,dlg.GetPathName(),sizeof(szFolder));
		m_Config.UpdateString(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_ADDDIR_KEY,szFolder);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_ADDDIR_KEY);

		SetStatusBarInfoPane(IDS_MESSAGE_ADDING_ITEM,szFolder);

		// totale elementi nella lista prima dell'operazione
		int nItemCount = m_wndPlaylist.GetItemCount(); 

		AddDir(szFolder,m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_RECURSE_SUBDIR_KEY));

		// stabilisce (e posiziona la barra sul) l'elemento corrente
		if(nItemCount <= 0)
		{
			m_wndPlaylist.SelectItem(0);
			SendWallPaperMessage(WALLPAPER_GOTO_FIRST_PICTURE);
		}

		// aggiorna il dialogo
		OnUpdateGUI();
		PlaylistSetModified();
	}

	SetStatusBarInfoPane(IDS_MESSAGE_READY);
}

/*
	OnAddDir()

	Aggiunge il contenuto della directory alla lista.
*/
LRESULT CWallPaperDlg::OnAddDir(WPARAM /*wParam*/,LPARAM lParam)
{
	char szFolder[_MAX_FILEPATH+1] = {0};

	// directory selezionata
	if((LPSTR)lParam)
	{
		strcpyn(szFolder,(LPSTR)lParam,sizeof(szFolder));
		m_Config.UpdateString(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_ADDDIR_KEY,szFolder);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_ADDDIR_KEY);

		SetStatusBarInfoPane(IDS_MESSAGE_ADDING_ITEM,szFolder);

		// totale elementi nella lista prima dell'operazione
		int nItemCount = m_wndPlaylist.GetItemCount(); 

		AddDir(szFolder,m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_RECURSE_SUBDIR_KEY));

		// stabilisce (e posiziona la barra sul) l'elemento corrente
		if(nItemCount <= 0)
		{
			m_wndPlaylist.SelectItem(0);
			SendWallPaperMessage(WALLPAPER_GOTO_FIRST_PICTURE);
		}

		// aggiorna il dialogo
		OnUpdateGUI();
		PlaylistSetModified();
	}

	SetStatusBarInfoPane(IDS_MESSAGE_READY);

	return(0L);
}

/*
	OnAddUrl()

	Aggiunge una url alla lista.
*/
void CWallPaperDlg::OnAddUrl(void)
{
	// flag per file aggiunto e totale elementi nella lista prima dell'operazione
	BOOL bAdded = FALSE;
	int nItemCount = m_wndPlaylist.GetItemCount(); 
	
	// selezione dell'url da aggiungere
	CWallPaperAddUrlDlg dlg(this->GetSafeHwnd(),&m_Config);
	if(dlg.DoModal()==IDOK)
	{
		SetStatusBarInfoPane(IDS_MESSAGE_ADDING_ITEM,dlg.GetUrl());
		bAdded = AddUrl(dlg.GetUrl());
	}

	if(bAdded)
	{
		// stabilisce (e posiziona la barra sul) l'elemento corrente
		if(nItemCount <= 0)
		{
			m_wndPlaylist.SelectItem(0);
			SendWallPaperMessage(WALLPAPER_GOTO_FIRST_PICTURE);
		}

		// aggiorna il dialogo
		OnUpdateGUI();
		PlaylistSetModified();
	}
	
	SetStatusBarInfoPane(IDS_MESSAGE_READY);
}

/*
	OnRemove()

	Rimuove un file dalla lista (chiamata dai threads UI).
*/
LRESULT CWallPaperDlg::OnRemove(WPARAM /*wParam*/,LPARAM lParam)
{
	char* pItem = (char*)lParam;

	::PeekAndPump();
	
	if(pItem)
	{
		// ricava nome elemento e pathname relativo
		char szLocation[MAX_ITEM_SIZE+1];
		char szItemLocation[_MAX_FILEPATH+1];
		strcpyn(szLocation,pItem,sizeof(szLocation));
		char* p = strrchr(szLocation,'\\');
		if(p)
			*p = '\0';
		p = strrchr(pItem,'\\');
		if(p)
			pItem = ++p;

		int nItem;
		char szItem[MAX_ITEM_SIZE+1];
		int nItemCount = m_wndPlaylist.GetItemCount();
		
		// scorre la lista alla ricerca dell'elemento specificato
		for(nItem = 0; nItem < nItemCount; nItem++)
		{
			// ricava il nome elemento
			m_wndPlaylist.GetItemText(nItem,CTRLLISTEX_FILENAME_INDEX,szItem,sizeof(szItem)-1);
			
			if(stricmp(szItem,pItem)==0)
			{
				// ricava il pathname
				m_wndPlaylist.GetItemText(nItem,CTRLLISTEX_LOCATION_INDEX,szItemLocation,sizeof(szItemLocation)-1);

				// deve verificare anche il pathname dato che possono esistere elementi con lo stesso nome
				if(stricmp(szLocation,szItemLocation)==0)
				{
					// elimina l'elemento
					SetStatusBarInfoPane(IDS_MESSAGE_REMOVING);
					m_wndPlaylist.DeleteItem(nItem);

					// riposiziona sull'entrata corrente
					if((nItem >= 0) && (nItem < m_wndPlaylist.GetItemCount()))
					{
						m_wndPlaylist.SelectItem(nItem);
					}
					else
					{
						if(m_wndPlaylist.GetItemCount() > 0)
						{
							if(nItem < 0)
								nItem = 0;
							else if(nItem >= m_wndPlaylist.GetItemCount())
								nItem = m_wndPlaylist.GetItemCount()-1;

							m_wndPlaylist.SelectItem(nItem);
						}
					}

					// se era stata caricata una playlist (.dpl) controlla se deve azzerarne il contenuto o solo scaricarla
					if(m_pCmdLine->HaveDplFile() && m_wndPlaylist.GetItemCount() <= 0)
					{
						char szBuffer[1024];
						FormatResourceStringEx(szBuffer,sizeof(szBuffer),IDS_QUESTION_CLEAN_DPLFILE,m_pCmdLine->GetDplFile());
						if(DoNotAskMoreMessageBox(this->GetSafeHwnd(),szBuffer,0,&m_Config,WALLPAPER_DONOTASKMORE_CLEANDPLFILE_KEY,WALLPAPER_DONOTASKMORE_CLEANDPLFILE_VALUE_KEY)==IDYES)
							::WritePrivateProfileInt(WALLPAPER_DPL_KEY,WALLPAPER_TOTPICT_KEY,0,m_pCmdLine->GetDplFile());
						else
							OnUnloadPlaylistFile();
					}

					// aggiorna il dialogo
					OnUpdateGUI();

					// abilita il bottone per salvare i cambi
					PlaylistSetModified();

					SetStatusBarInfoPane(IDS_MESSAGE_READY);

					break;
				}
			}
		}
	}

	// aggiorna l'indice per l'elemento corrente
	if(m_wndPlaylist.GetItemCount() <= 0)
	{
		m_Config.UpdateNumber(WALLPAPER_PICTURES_KEY,WALLPAPER_CURRENTPICT_KEY,0);
		m_Config.SaveKey(WALLPAPER_PICTURES_KEY,WALLPAPER_CURRENTPICT_KEY);
	}

	::PeekAndPump();

	return(0L);
}

/*
	OnRemove()

	Rimuove un file dalla lista (chiamata internamente).
*/
void CWallPaperDlg::OnRemove(void)
{
	CWaitCursor cursor;
	int nItem;

	// controlla la selezione corrente	
	if((nItem = m_wndPlaylist.GetCurrentItem()) >= 0)
	{
		SetStatusBarInfoPane(IDS_MESSAGE_REMOVING);

		// ricava l'elemento (file+path)
		char szItem[MAX_ITEM_SIZE+1];
		GetItem(szItem,sizeof(szItem),nItem);

		// ricava il nome file (solo il nome, senza pathname)
		char szFilename[MAX_ITEM_SIZE+1];
		m_wndPlaylist.GetItemText(nItem,CTRLLISTEX_FILENAME_INDEX,szFilename,sizeof(szFilename)-1);
		
		// se il file e' contenuto in un archivio compresso, potrebbe essere presente il pathname
		char* p = strrchr(szFilename,'/');
		if(p && *(p+1))
		{
			char szName[_MAX_FILEPATH+1];
			strcpyn(szName,p+1,sizeof(szName));
			strcpyn(szFilename,szName,sizeof(szFilename));
		}
		
		// controlla se l'elemento e' contenuto in un file compresso
		// se si, al player viene inviato il solo nome file, senza path (bug: se esistono piu' elementi
		// con lo stesso nome provenienti da files compressi differenti viene cancellato sempre il primo...)
		LPARAM lParam = m_wndPlaylist.GetItemlParam(nItem);
		if(lParam!=(LPARAM)-1L)
		{
			// chiede se si vuole eliminare l'elemento corrente o tutti gli elementi provenienti dallo stesso file compresso
			int nRet = DoNotAskMoreMessageBox(this->GetSafeHwnd(),IDS_QUESTION_REMOVE_FROM_ARCHIVE,0,&m_Config,WALLPAPER_DONOTASKMORE_REMOVEFROMARCHIVE_KEY,WALLPAPER_DONOTASKMORE_REMOVEFROMARCHIVE_VALUE_KEY);
			if(nRet==IDYES)
			{
				// se si tratta di un file audio, notifica al player la sua rimozione
				if(CAudioPlayer::IsSupportedFormat(szItem))
				{
					CWinThread* pWinThread = NULL;
					BOOL bHavePlayer = IsUIThreadRunning("CWallPaperPlayerUIThread",&pWinThread);
					if(bHavePlayer && pWinThread)
					{
						CWnd* pWnd = pWinThread->GetMainWnd();
						if(pWnd)
						{
							HWND hWnd = pWnd->GetSafeHwnd();
							if(hWnd)
							{
								::SendMessage(hWnd,WM_AUDIOPLAYER_MODE,(WPARAM)AUDIOPLAYER_MODE_FEEDBACK,(LPARAM)FALSE);
								::SendMessage(hWnd,WM_AUDIOPLAYER_REMOVE,MAKEWPARAM(1,0),(LPARAM)szFilename);
								::SendMessage(hWnd,WM_AUDIOPLAYER_MODE,(WPARAM)AUDIOPLAYER_MODE_FEEDBACK,(LPARAM)TRUE);
							}
						}
					}
				}
				
				m_wndPlaylist.DeleteItem(nItem);
			}
			else if(nRet==IDNO)
			{
				// interfaccia utente
				m_wndPlaylist.SetRedraw(FALSE);

				// disabilita l'interfaccia utente
				OnEnableGUI(FALSE,MAKELPARAM(MF_GRAYED,MF_GRAYED));
			
				::PeekAndPump();

				// deve evitare il passaggio automatico al brano successivo durante l'eliminazione degli elementi
				// se chiama in sequenza la rimozione del singolo elemento ed il player passa automaticamente al
				// successivo, scasina alla grande dato che la lista utilizzata da quest'ultimo non viene sincronizzata
				CWinThread* pWinThread = NULL;
				BOOL bHavePlayer = IsUIThreadRunning("CWallPaperPlayerUIThread",&pWinThread);
				if(bHavePlayer && pWinThread)
				{
					CWnd* pWnd = pWinThread->GetMainWnd();
					if(pWnd)
					{
						HWND hWnd = pWnd->GetSafeHwnd();
						if(hWnd)
						{
							::SendMessage(hWnd,WM_AUDIOPLAYER_MODE,(WPARAM)AUDIOPLAYER_MODE_FEEDBACK,(LPARAM)FALSE);
							::SendMessage(hWnd,WM_AUDIOPLAYER_MODE,(WPARAM)AUDIOPLAYER_MODE_AUTOSKIP,(LPARAM)FALSE);
						}
					}
				}

				int n = 0;
				int nItemCount = m_wndPlaylist.GetItemCount();
				for(nItem = 0; nItem < nItemCount; nItem++)
				{
					if(m_wndPlaylist.GetItemlParam(nItem)==lParam)
					{
						// se si tratta di un file audio, notifica al player la sua rimozione
						m_wndPlaylist.GetItemText(nItem,CTRLLISTEX_FILENAME_INDEX,szFilename,sizeof(szFilename)-1);
						if(CAudioPlayer::IsSupportedFormat(szItem))
						{
							pWinThread = NULL;
							bHavePlayer = IsUIThreadRunning("CWallPaperPlayerUIThread",&pWinThread);
							if(bHavePlayer && pWinThread)
							{
								CWnd* pWnd = pWinThread->GetMainWnd();
								if(pWnd)
								{
									HWND hWnd = pWnd->GetSafeHwnd();
									if(hWnd)
										::SendMessage(hWnd,WM_AUDIOPLAYER_REMOVE,MAKEWPARAM(1,0),(LPARAM)szFilename);
								}
							}
						}
						
						m_wndPlaylist.DeleteItem(nItem);
						nItem--;
						nItemCount--;
					}
				
					if(++n > 15)
					{
						n = 0;
						::PeekAndPump();
					}
				}
				
				// ripristina il passaggio automatico al brano successivo
				pWinThread = NULL;
				bHavePlayer = IsUIThreadRunning("CWallPaperPlayerUIThread",&pWinThread);
				if(bHavePlayer && pWinThread)
				{
					CWnd* pWnd = pWinThread->GetMainWnd();
					if(pWnd)
					{
						HWND hWnd = pWnd->GetSafeHwnd();
						if(hWnd)
						{
							::SendMessage(hWnd,WM_AUDIOPLAYER_MODE,(WPARAM)AUDIOPLAYER_MODE_AUTOSKIP,(LPARAM)TRUE);
							::SendMessage(hWnd,WM_AUDIOPLAYER_MODE,(WPARAM)AUDIOPLAYER_MODE_FEEDBACK,(LPARAM)TRUE);
						}
					}

					// riavvia la riproduzione (durante l'eliminazione il player potrebbe stopparsi)
					LONG nPlayerStatus = ::SendMessage(pWnd->m_hWnd,WM_AUDIOPLAYER_GETSTATUS,0L,0L);
					if(nPlayerStatus==mmAudioPmPaused || nPlayerStatus==mmAudioPmStopped)
						::SendMessage(pWnd->m_hWnd,WM_AUDIOPLAYER_NEXT,0L,0L);
				}

				// riabilita l'interfaccia utente
				BOOL bPaused = m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_STATE_KEY)==WALLPAPER_IN_PAUSED_STATE;
				OnEnableGUI(TRUE,MAKELPARAM((bPaused ? MF_GRAYED : MF_ENABLED),(bPaused ? MF_ENABLED : MF_GRAYED)));

				// interfaccia utente
				//m_wndPlaylist.AutoSizeColumns();
				//m_wndPlaylist.EnsureVisible(nItem,FALSE);
				m_wndPlaylist.SetRedraw(TRUE);
			}
		}
		else
		{
			// se si tratta di un file audio, notifica al player la sua rimozione
			if(CAudioPlayer::IsSupportedFormat(szItem))
			{
				CWinThread* pWinThread = NULL;
				BOOL bHavePlayer = IsUIThreadRunning("CWallPaperPlayerUIThread",&pWinThread);;
				if(bHavePlayer && pWinThread)
				{
					CWnd* pWnd = pWinThread->GetMainWnd();
					if(pWnd)
					{
						HWND hWnd = pWnd->GetSafeHwnd();
						if(hWnd)
						{
							::SendMessage(hWnd,WM_AUDIOPLAYER_MODE,(WPARAM)AUDIOPLAYER_MODE_FEEDBACK,(LPARAM)FALSE);
							::SendMessage(hWnd,WM_AUDIOPLAYER_REMOVE,MAKEWPARAM(1,0),(LPARAM)szItem);
							::SendMessage(hWnd,WM_AUDIOPLAYER_MODE,(WPARAM)AUDIOPLAYER_MODE_FEEDBACK,(LPARAM)TRUE);
						}
					}
				}
			}
			
			m_wndPlaylist.DeleteItem(nItem);
		}

		// riposiziona sull'entrata corrente
		if((nItem >= 0) && (nItem < m_wndPlaylist.GetItemCount()))
		{
			m_wndPlaylist.SelectItem(nItem);
		}
		else
		{
			if(m_wndPlaylist.GetItemCount() > 0)
			{
				if(nItem < 0)
					nItem = 0;
				else if(nItem >= m_wndPlaylist.GetItemCount())
					nItem = m_wndPlaylist.GetItemCount()-1;

				m_wndPlaylist.SelectItem(nItem);
			}
		}

		// se era stata caricata una playlist (.dpl) controlla se deve azzerarne il contenuto o solo scaricarla
		if(m_pCmdLine->HaveDplFile() && m_wndPlaylist.GetItemCount() <= 0)
		{
			char szBuffer[1024];
			FormatResourceStringEx(szBuffer,sizeof(szBuffer),IDS_QUESTION_CLEAN_DPLFILE,m_pCmdLine->GetDplFile());
			if(DoNotAskMoreMessageBox(this->GetSafeHwnd(),szBuffer,0,&m_Config,WALLPAPER_DONOTASKMORE_CLEANDPLFILE_KEY,WALLPAPER_DONOTASKMORE_CLEANDPLFILE_VALUE_KEY)==IDYES)
				::WritePrivateProfileInt(WALLPAPER_DPL_KEY,WALLPAPER_TOTPICT_KEY,0,m_pCmdLine->GetDplFile());
			else
				OnUnloadPlaylistFile();
		}

		// aggiorna il dialogo
		OnUpdateGUI();

		// abilita il bottone per salvare i cambi
		PlaylistSetModified();

		SetStatusBarInfoPane(IDS_MESSAGE_READY);
	}

	// aggiorna l'indice per l'elemento corrente
	if(m_wndPlaylist.GetItemCount() <= 0)
	{
		m_Config.UpdateNumber(WALLPAPER_PICTURES_KEY,WALLPAPER_CURRENTPICT_KEY,0);
		m_Config.SaveKey(WALLPAPER_PICTURES_KEY,WALLPAPER_CURRENTPICT_KEY);
	}
}

/*
	OnRemoveAll()

	Rimuove tutti gli elementi dalla lista.
*/
void CWallPaperDlg::OnRemoveAll(void)
{
	CWaitCursor cursor;

	SetStatusBarInfoPane(IDS_MESSAGE_REMOVING);

	// notifica al player la rimozione di tutti gli elementi
	CWinThread* pWinThread = NULL;
	BOOL bHavePlayer = IsUIThreadRunning("CWallPaperPlayerUIThread",&pWinThread);
	if(bHavePlayer && pWinThread)
	{
		CWnd* pWnd = pWinThread->GetMainWnd();
		if(pWnd)
		{
			HWND hWnd = pWnd->GetSafeHwnd();
			if(hWnd)
				::PostMessage(hWnd,WM_AUDIOPLAYER_REMOVEALL,0L,(LPARAM)0L);
		}
	}

	// azzera la lista
	m_wndPlaylist.DeleteAllItems(TRUE);
	m_Config.UpdateNumber(WALLPAPER_PICTURES_KEY,WALLPAPER_CURRENTPICT_KEY,0);
	m_Config.SaveKey(WALLPAPER_PICTURES_KEY,WALLPAPER_CURRENTPICT_KEY);

	// se era stata caricata una playlist (.dpl) controlla se deve azzerarne il contenuto o solo scaricarla
	if(m_pCmdLine->HaveDplFile())
	{
		char szBuffer[1024];
		FormatResourceStringEx(szBuffer,sizeof(szBuffer),IDS_QUESTION_CLEAN_DPLFILE,m_pCmdLine->GetDplFile());
		if(DoNotAskMoreMessageBox(this->GetSafeHwnd(),szBuffer,0,&m_Config,WALLPAPER_DONOTASKMORE_CLEANDPLFILE_KEY,WALLPAPER_DONOTASKMORE_CLEANDPLFILE_VALUE_KEY)==IDYES)
			::WritePrivateProfileInt(WALLPAPER_DPL_KEY,WALLPAPER_TOTPICT_KEY,0,m_pCmdLine->GetDplFile());
		else
			OnUnloadPlaylistFile();
	}

	// aggiorna il dialogo
	OnUpdateGUI();

	// abilita il bottone per salvare i cambi
	PlaylistSetModified();

	SetStatusBarInfoPane(IDS_MESSAGE_READY);
}

/*
	OnUnloadPlaylistFile()

	Scarica la playlist (.dpl) corrente.
*/
void CWallPaperDlg::OnUnloadPlaylistFile(void)
{
	m_pCmdLine->ResetDplFile();
	SetStatusBarDplPanel(NULL);
	if(m_wndPlaylist.GetItemCount() > 0)
		OnRemoveAll();
}

/*
	OnNewPlaylistFile()

	Crea una nuova playlist (.dpl).
*/
void CWallPaperDlg::OnNewPlaylistFile(void)
{
	if(m_wndPlaylist.GetItemCount() > 0)
		OnRemoveAll();
	
	PlaylistResetModified();

	OnAddDir();

	if(PlaylistIsModified())
	{
		OnSavePlaylistFile();

		if(PlaylistIsModified())
			OnRemoveAll();
	}
}

/*
	OnLoadPlaylistFile()

	Seleziona e carica la playlist (.dpl).
*/
void CWallPaperDlg::OnLoadPlaylistFile(void)
{
	CFileOpenDialog dlg("Open "DPL_TYPEDESC,
					m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_LOADLIST_KEY),
					"*"DPL_EXTENSION,
					DPL_TYPEDESC" (*"DPL_EXTENSION")|*"DPL_EXTENSION"||"
					);

	if(dlg.DoModal()==IDOK)
	{
		m_pCmdLine->SetDplFile(dlg.GetPathName());
		::PostMessage(this->GetSafeHwnd(),WM_LOADPLAYLIST,0L,0L);
		PlaylistSetModified();
	}
}

/*
	OnSavePlaylistFile()

	Salva la playlist nel file (.dpl).
*/
void CWallPaperDlg::OnSavePlaylistFile(void)
{
	CFileSaveAsDialog dlg(	"Save "DPL_TYPEDESC,
						m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_SAVELIST_KEY),
						"*"DPL_EXTENSION,
						"untitled",
						DPL_TYPEDESC" (*"DPL_EXTENSION")|*"DPL_EXTENSION"||"
						);

	if(dlg.DoModal()==IDOK)
	{
		char szDir[_MAX_FILEPATH+1];
		strcpyn(szDir,dlg.GetPathName(),sizeof(szDir));
		strrev(szDir);
		int i = strchr(szDir,'\\') - szDir;
		strrev(szDir);
		if(i > 0)
			szDir[strlen(szDir)-i-1] = '\0';
		m_Config.UpdateString(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_SAVELIST_KEY,szDir);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_SAVELIST_KEY);
		m_pCmdLine->SetDplFile(dlg.GetPathName());

		// aggiorna la barra di stato
		SetStatusBarDplPanel(m_pCmdLine->GetDplFile());

		// aggiorna il dialogo
		OnUpdateGUI();
		
		PlaylistSave();
	}
}

/*
	OnFavourite()
*/
LRESULT CWallPaperDlg::OnFavourite(WPARAM wParam/* -1=lParam contiene il nome file, 0=picture, 1=audio, 2=ricava in proprio */,LPARAM lParam/* # item o nome file */)
{
	int nItem = -1;
	char* pFileName = NULL;

	if(wParam==(WPARAM)-1L)
	{
		pFileName = (char*)lParam;
		if(m_pImage->IsSupportedFormat(pFileName))
			lParam = 0;
		else if(CAudioPlayer::IsSupportedFormat(pFileName))
			lParam = 1;
	}

	nItem = (int)lParam;

	if(nItem >= 0)
	{
		char szItem[MAX_ITEM_SIZE+1] = {0};
		char szLocation[_MAX_FILEPATH+1] = {0};
		char szFileName[_MAX_FILEPATH+1] = {0};

		if(pFileName)
			_snprintf(szFileName,sizeof(szFileName)-1,"%s",pFileName);
		else
		{
			m_wndPlaylist.GetItemText(nItem,CTRLLISTEX_FILENAME_INDEX,szItem,sizeof(szItem)-1);
			m_wndPlaylist.GetItemText(nItem,CTRLLISTEX_LOCATION_INDEX,szLocation,sizeof(szLocation)-1);
			_snprintf(szFileName,sizeof(szFileName)-1,"%s\\%s",szLocation,szItem);
		}

		if((lParam==0 && !m_pImage->IsSupportedFormat(szFileName)) || (lParam==1 && !CAudioPlayer::IsSupportedFormat(szFileName)))
			;
		else
		{
			if(m_pImage->IsSupportedFormat(szFileName))
				lParam = 0;
			else if(CAudioPlayer::IsSupportedFormat(szFileName))
				lParam = 1;

			char szFavourite[_MAX_FILEPATH+1];
			_snprintf(szFavourite,sizeof(szFavourite)-1,"%s%s",m_Config.GetString(WALLPAPER_INSTALL_KEY,WALLPAPER_DIR_KEY),lParam==0 ? "Favourite Pictures.dpl" : "Favourite Audio.dpl");
			int nItemCount = ::GetPrivateProfileInt(WALLPAPER_DPL_KEY,WALLPAPER_TOTPICT_KEY,0,szFavourite);
			char szFileNo[16];
			BOOL bFound = FALSE;
			for(int i=0; !bFound && i < nItemCount; i++)
			{
				_snprintf(szFileNo,sizeof(szFileNo)-1,"File%d",i);
				if(::GetPrivateProfileString(WALLPAPER_DPL_KEY,szFileNo,"",szItem,sizeof(szItem)-1,szFavourite) > 0L)
					if(stricmp(szItem,szFileName)==0)
						bFound = TRUE;
			}
			if(!bFound)
			{
				_snprintf(szFileNo,sizeof(szFileNo)-1,"File%d",nItemCount);
				::WritePrivateProfileInt(WALLPAPER_DPL_KEY,WALLPAPER_TOTPICT_KEY,++nItemCount,szFavourite);
				::WritePrivateProfileString(WALLPAPER_DPL_KEY,szFileNo,szFileName,szFavourite);
			}
			else
				::MessageBoxResourceEx(this->GetSafeHwnd(),MB_OK|MB_ICONWARNING,WALLPAPER_PROGRAM_NAME,IDS_ERROR_FAVOURITE_ITEM_EXISTS,lParam==0 ? "Pictures" : "Audio",szFileName);
		}
	}

	return(0L);
}

/*
	OnFavouritePicture()
*/
void CWallPaperDlg::OnFavouritePicture(void)
{
	int nItem;
	if((nItem = m_wndPlaylist.GetCurrentItem()) >= 0)
		::PostMessage(this->GetSafeHwnd(),WM_FAVOURITE_ADD,(WPARAM)0,(LPARAM)nItem); // wParam = -1=lParam contiene il nome file, 0=picture, 1=audio, 2=ricava in proprio
}

/*
	OnFavouriteAudio()
*/
void CWallPaperDlg::OnFavouriteAudio(void)
{
	int nItem;
	if((nItem = m_wndPlaylist.GetCurrentItem()) >= 0)
	{
		char* pFavouriteAudio = NULL;

		// controlla se l'istanza del thread si trova in esecuzione
		CWinThread* pWinThread = NULL;
		BOOL bHaveThread = IsUIThreadRunning("CWallPaperPlayerUIThread",&pWinThread);
		if(bHaveThread && pWinThread)
		{
			// invia il messaggio al thread ricavando il nome del file in riproduzione
			CWnd* pWnd = pWinThread->GetMainWnd();
			if(pWnd)
			{
				HWND hWnd = pWnd->GetSafeHwnd();
				if(hWnd)
					pFavouriteAudio = (char*)::SendMessage(hWnd,WM_AUDIOPLAYER_FAVOURITE,0L,0L);
			}
		}

		// aggiunge alla lista dei favoriti audio
		if(pFavouriteAudio)
			::PostMessage(this->GetSafeHwnd(),WM_FAVOURITE_ADD,(WPARAM)-1,(LPARAM)pFavouriteAudio); // wParam = -1=lParam contiene il nome file, 0=picture, 1=audio, 2=ricava in proprio
	}
}

/*
	OnLoadFavouritePicture()
*/
void CWallPaperDlg::OnLoadFavouritePicture(void)
{
	char szFavourite[_MAX_FILEPATH+1];
	_snprintf(szFavourite,sizeof(szFavourite)-1,"%sFavourite Pictures.dpl",m_Config.GetString(WALLPAPER_INSTALL_KEY,WALLPAPER_DIR_KEY));
	int nItemCount = ::GetPrivateProfileInt(WALLPAPER_DPL_KEY,WALLPAPER_TOTPICT_KEY,-1,szFavourite);
	if(nItemCount <= 0)
		::MessageBoxResourceEx(this->GetSafeHwnd(),MB_OK|MB_ICONWARNING,WALLPAPER_PROGRAM_NAME,IDS_MESSAGE_EMPTY_FAVOURITE,"Favourite Pictures.dpl");
	else
	{
		m_pCmdLine->SetDplFile(szFavourite);
		::PostMessage(this->GetSafeHwnd(),WM_LOADPLAYLIST,0L,0L);
		PlaylistSetModified();
	}
}

/*
	OnLoadFavouriteAudio()
*/
void CWallPaperDlg::OnLoadFavouriteAudio(void)
{
	char szFavourite[_MAX_FILEPATH+1];
	_snprintf(szFavourite,sizeof(szFavourite)-1,"%sFavourite Audio.dpl",m_Config.GetString(WALLPAPER_INSTALL_KEY,WALLPAPER_DIR_KEY));
	int nItemCount = ::GetPrivateProfileInt(WALLPAPER_DPL_KEY,WALLPAPER_TOTPICT_KEY,-1,szFavourite);
	if(nItemCount <= 0)
		::MessageBoxResourceEx(this->GetSafeHwnd(),MB_OK|MB_ICONWARNING,WALLPAPER_PROGRAM_NAME,IDS_MESSAGE_EMPTY_FAVOURITE,"Favourite Audio.dpl");
	else
	{
		m_pCmdLine->SetDplFile(szFavourite);
		::PostMessage(this->GetSafeHwnd(),WM_LOADPLAYLIST,0L,0L);
		PlaylistSetModified();
	}
}

/*
	OnDesktopIcons()

	Nasconde/visualizza le icone del desktop.
*/
void CWallPaperDlg::OnDesktopIcons(void)
{
	HWND hWnd = ::FindWindow("Progman",NULL);
	if(hWnd)
	{
		::ShowWindow(hWnd,m_bDesktopIcons ? SW_SHOW : SW_HIDE);
		if(m_bDesktopIcons && !m_pTrayIcon)
			SetForegroundWindowEx(this->GetSafeHwnd());
		m_bDesktopIcons = !m_bDesktopIcons;
	}
}

/*
	OnDisplayProperties()

	Apre il dialogo per le proprieta' dello schermo.
*/
void CWallPaperDlg::OnDisplayProperties(void)
{
	::ShellExecute(this->GetSafeHwnd(),"open","rundll32","shell32.dll,Control_RunDLL desk.cpl",NULL,SW_SHOW);
}

/*
	OnBrowser()

	Apre il browser per le immagini (WallBrowser).
*/
void CWallPaperDlg::OnBrowser(void)
{
	char szBuffer[1024];
	_snprintf(szBuffer,sizeof(szBuffer)-1,"%s\\%s.exe",m_Config.GetString(WALLPAPER_INSTALL_KEY,WALLPAPER_DIR_KEY),WALLBROWSER_PROGRAM_NAME);
	STARTUPINFO si = {0};
	si.cb = sizeof(STARTUPINFO);
	PROCESS_INFORMATION pi = {0};
	if(::CreateProcess(NULL,szBuffer,NULL,NULL,FALSE,0L,NULL,NULL,&si,&pi))
		::CloseHandle(pi.hProcess);
}

/*
	OnThumbnails()

	Genera le miniature.
*/
void CWallPaperDlg::OnThumbnails(void)
{
	// crea il thread ed inserisce nella lista la referenza per il controllo all'uscita
	// (non termina fino a che esistano thread in esecuzione)
	CWinThread* pWinThread = AfxBeginThread(RUNTIME_CLASS(CWallPaperThumbnailsUIThread),m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PRIORITY_THREAD_KEY),0,CREATE_SUSPENDED);
	if(pWinThread)
	{
		THREAD* thread = (THREAD*)m_listRunningUIThreads.Add();
		if(thread)
		{
			thread->pWinThread = pWinThread;
			strcpyn(thread->szThreadName,"CWallPaperThumbnailsUIThread",MAX_THREAD_NAME+1);
			pWinThread->m_bAutoDelete = FALSE;
			pWinThread->ResumeThread();

			OnMenuFileMinimize();
		}
	}
}

/*
	OnThumbnailsDone()

	Notificazione per il termine del thread.
*/
LRESULT CWallPaperDlg::OnThumbnailsDone(WPARAM /*wParam*/,LPARAM /*lParam*/)
{
	// dato che il thread ricava il puntatore alla configurazione (non carica in proprio una nuova istanza)
	// controlla se sono state fatte modifiche abilitando il salvataggio
	if(m_Config.GetModified())
		m_Config.Save();
	
	return(0L);
}

/*
	OnCrawler()

	Lancia il thread per il crawler.
*/
LRESULT CWallPaperDlg::OnCrawler(WPARAM /*wParam = NULL*/,LPARAM lParam/* = NULL*/)
{
	int nRet = IDYES;
	char* pUrl = (char*)lParam;

	// chiede conferma se deve salvare la lista delle inclusioni/esclusioni
	if(nRet!=IDCANCEL)
		if(m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_SAVEINCLEXCLLIST_KEY))
		{
			if((nRet = DoNotAskMoreMessageBox(this->GetSafeHwnd(),IDS_QUESTION_CRALWER_LISTS,0,&m_Config,WALLPAPER_DONOTASKMORE_CRAWLERLISTS_KEY,WALLPAPER_DONOTASKMORE_CRAWLERLISTS_VALUE_KEY))==IDNO)
			{
				m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_SAVEINCLEXCLLIST_KEY,0);
				m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_SAVEINCLEXCLLIST_KEY);
			}
		}

	// chiede conferma se in modalita' multithread
	if(nRet!=IDCANCEL)
		if(m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_MULTITHREAD_KEY))
		{
			if((nRet = DoNotAskMoreMessageBox(this->GetSafeHwnd(),IDS_QUESTION_CRAWLER_MULTITHREAD_MODE,0,&m_Config,WALLPAPER_DONOTASKMORE_CRAWLERMT_KEY,WALLPAPER_DONOTASKMORE_CRAWLERMT_VALUE_KEY))==IDNO)
			{
				m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_MULTITHREAD_KEY,0);
				m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_MULTITHREAD_KEY);
			}
		}

	if(nRet!=IDCANCEL)
	{
		// la modalita' multithread e' incompatibile con il timeout interno
		if(m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_MULTITHREAD_KEY) && !m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_USEWINSOCKTIMEOUT_KEY))
		{
			::MessageBoxResource(this->GetSafeHwnd(),MB_OK|MB_ICONWARNING,WALLPAPER_PROGRAM_NAME,IDS_MESSAGE_WINSOCKTIMEOUTWITHMULTITHREAD);
			m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_USEWINSOCKTIMEOUT_KEY,1);
			m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_USEWINSOCKTIMEOUT_KEY);
		}

		// crea il thread ed inserisce nella lista la referenza per il controllo all'uscita
		// (non termina fino a che esistano thread in esecuzione)
		CWinThread* pWinThread = AfxBeginThread(RUNTIME_CLASS(CWallPaperCrawlerUIThread),m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PRIORITY_THREAD_KEY),0,CREATE_SUSPENDED);
		if(pWinThread)
		{
			THREAD* thread = (THREAD*)m_listRunningUIThreads.Add();
			if(thread)
			{
				thread->pWinThread = pWinThread;
				strcpyn(thread->szThreadName,"CWallPaperCrawlerUIThread",MAX_THREAD_NAME+1);
				thread->dwThreadId = pWinThread->m_nThreadID;
				thread->lParam = (LPARAM)NULL;

				CRAWLERPARAMS* pCrawlerParams = new CRAWLERPARAMS;
				if(pCrawlerParams)
				{
					memset(pCrawlerParams,'\0',sizeof(CRAWLERPARAMS));
					
					if(pUrl)
						strcpyn(pCrawlerParams->szUrl,pUrl,MAX_URL+1);
					pCrawlerParams->pConfig = &m_Config;
					thread->lParam = (LPARAM)pCrawlerParams;
				}

				pWinThread->m_bAutoDelete = FALSE;
				pWinThread->ResumeThread();

				OnMenuFileMinimize();
			}
		}
	}

	return(0L);
}

/*
	OnCrawlerDone()

	Notificazione per il termine del thread.
*/
LRESULT CWallPaperDlg::OnCrawlerDone(WPARAM /*wParam*/,LPARAM /*lParam*/)
{
	// dato che al thread non viene passato il puntatore alla configurazione (il thread carica in
	// proprio una nuova istanza) deve ricaricare solo quanto effettivamente modificato dal thread,
	// ossia le chiavi/sezioni relative
	m_Config.ReloadString(WALLPAPER_KEY,WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_ADDURL_KEY);
	m_Config.ReloadSection(WALLPAPER_HOSTNAMES_KEY);
	m_Config.ReloadSection(WALLPAPER_FILETYPELIST_KEY);
	m_Config.ReloadSection(WALLPAPER_DOMAININCLUDELIST_KEY);
	m_Config.ReloadSection(WALLPAPER_DOMAINEXCLUDELIST_KEY);
	m_Config.ReloadSection(WALLPAPER_PARENTINCLUDELIST_KEY);
	m_Config.ReloadSection(WALLPAPER_PARENTEXCLUDELIST_KEY);

	return(0L);
}

/*
	OnPlayer()

	Lancia il thread per il player audio relativamente ad un singolo file.
*/
void CWallPaperDlg::OnPlayer(LPCSTR lpcszAudioFileName,CAudioFilesList* pAudioFilesList,int nPlayMode,BOOL bForcePlayingIfIdle/*=FALSE*/)
{
	// controlla se l'istanza del player si trova gia' in esecuzione, distinguendo se si tratta di un proprio
	// thread o di quello di un altra istanza del programma (nel caso in cui siano ammesse istanze multiple)
	// in quest'ultimo caso (thread di un altra istanza) non aggiunge i files alla lista
	HWND hWndWinThread = NULL;
	CWinThread* pWinThread = NULL;
	BOOL bIsAnExternalInstance = FALSE;
	BOOL bHavePlayer = IsUIThreadRunning("CWallPaperPlayerUIThread",&pWinThread,TRUE,IDS_DIALOG_PLAYER_TITLE,&hWndWinThread,&bIsAnExternalInstance);

	if(bHavePlayer && bIsAnExternalInstance)
		return;

	if(m_syncAudioPlaylist.Lock(SYNC_5_SECS_TIMEOUT))
	{
		// carica la lista dei bitmaps per gli skins
		if(m_listAudioTaskbarBitmaps.Count() <= 0)
			LoadTaskbarPopupList(&m_listAudioTaskbarBitmaps,0);

		// deve eseguire l'istanza per il player
		if(!bHavePlayer)
		{
			// crea il thread ed inserisce nella lista la referenza per il controllo all'uscita
			// (non termina fino a che esistano thread in esecuzione)
			pWinThread = AfxBeginThread(RUNTIME_CLASS(CWallPaperPlayerUIThread),m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PRIORITY_THREAD_KEY),0,CREATE_SUSPENDED);
			if(pWinThread)
			{
				THREAD* thread = (THREAD*)m_listRunningUIThreads.Add();
				if(thread)
				{
					thread->pWinThread = pWinThread;
					strcpyn(thread->szThreadName,"CWallPaperPlayerUIThread",MAX_THREAD_NAME+1);
					bHavePlayer = TRUE;
				}

				pWinThread->m_bAutoDelete = FALSE;
				pWinThread->ResumeThread();

				// se la finestra del player viene chiusa prima di iniziare a riprodurre, la SendMessage() causa una gpf
				// comunque sia deve aspettare il termine della creazione della finestra per poter inviare i messaggi
				while(::FindWindow(NULL,IDS_DIALOG_PLAYER_TITLE)==NULL && IsUIThreadRunning("CWallPaperPlayerUIThread",&pWinThread))
					::Sleep(100L);
				::Sleep(100L);
			}
		}

		// il player e' gia' in esecuzione o e' stato appena creato
		if(bHavePlayer && pWinThread)
		{
			if(!hWndWinThread)
			{
				CWnd* pWnd = pWinThread->GetMainWnd();
				if(pWnd)
					hWndWinThread = pWnd->GetSafeHwnd();
			}
			
			if(hWndWinThread)
			{
				switch(m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_AUDIOPOPUP_KEY))
				{
					case AUDIOPOPUP_NONE:
						::SendMessage(hWndWinThread,WM_AUDIOPLAYER_MODE,(WPARAM)AUDIOPLAYER_MODE_NOPOPUP,0L);
						::SendMessage(hWndWinThread,WM_AUDIOPLAYER_POPUPLIST,(WPARAM)AUDIOPLAYER_MODE_SETPOPUPLIST,(LPARAM)NULL);
						break;
					case AUDIOPOPUP_TASKBAR:
						::SendMessage(hWndWinThread,WM_AUDIOPLAYER_MODE,(WPARAM)AUDIOPLAYER_MODE_TASKBAR_POPUP,0L);
						::SendMessage(hWndWinThread,WM_AUDIOPLAYER_POPUPLIST,(WPARAM)AUDIOPLAYER_MODE_SETPOPUPLIST,(LPARAM)&m_listAudioTaskbarBitmaps);
						break;
					case AUDIOPOPUP_BALLOON:
						::SendMessage(hWndWinThread,WM_AUDIOPLAYER_MODE,(WPARAM)AUDIOPLAYER_MODE_BALLOON_POPUP,(LPARAM)&m_findAudioIcons);
						::SendMessage(hWndWinThread,WM_AUDIOPLAYER_POPUPLIST,(WPARAM)AUDIOPLAYER_MODE_SETPOPUPLIST,(LPARAM)NULL);
						break;
				}

				if(bForcePlayingIfIdle)
				{
					LONG nPlayerStatus = ::SendMessage(hWndWinThread,WM_AUDIOPLAYER_GETSTATUS,0L,0L);
					BOOL bIsIdle = TRUE;
					switch(nPlayerStatus)
					{
						case mmAudioPmReady:
						case mmAudioPmClosed:
						case mmAudioPmDone:
							bIsIdle = TRUE;
							break;
						default:
							//mmAudioPmLoading
							//mmAudioPmOpened
							//mmAudioPmPlaying
							//mmAudioPmPaused
							//mmAudioPmStopped
							//mmAudioPmUndefined
							::Sleep(0L);
							bIsIdle = FALSE;
							break;
					}
					if(bIsIdle)
						nPlayMode = lpcszAudioFileName ? AUDIOPLAYER_COMMAND_PLAY_FROM_FILE : (pAudioFilesList ? AUDIOPLAYER_COMMAND_PLAY_FROM_LIST : AUDIOPLAYER_COMMAND_STOP);
				}
				
				::SendMessage(hWndWinThread,WM_AUDIOPLAYER,(WPARAM)nPlayMode,lpcszAudioFileName ? (LPARAM)lpcszAudioFileName : (LPARAM)pAudioFilesList);
			}
		}

		m_syncAudioPlaylist.Unlock();
	}
}

/*
	OnAudioPlayerEvent()

	Gestore per le notificazioni sullo status corrente del player audio.
*/
LRESULT CWallPaperDlg::OnAudioPlayerEvent(WPARAM wParam,LPARAM lParam)
{
	char szTooltipText[256];
	LPCSTR pAudioFileName = (LPCSTR)lParam;
	if(pAudioFileName && !strnull(pAudioFileName))
	{
		char szAudioFileName[_MAX_FILEPATH+1];
		char* p;
		strcpyn(szAudioFileName,::StripPathFromFile(pAudioFileName),sizeof(szAudioFileName));
		if((p = strrchr(szAudioFileName,'.'))!=NULL)
			*p = '\0';

		strcpyn(szTooltipText,szAudioFileName,sizeof(szTooltipText));
		
		char szEvent[64] = {"guru mode"};
		switch(wParam)
		{
			case AUDIOPLAYER_COMMAND_PAUSE:
				::FormatResourceString(szEvent,sizeof(szEvent),IDS_MESSAGE_PAUSED);
				break;
			case AUDIOPLAYER_COMMAND_STOP:
				::FormatResourceString(szEvent,sizeof(szEvent),IDS_MESSAGE_STOPPED);
				break;
			case AUDIOPLAYER_COMMAND_PLAY:
				::FormatResourceString(szEvent,sizeof(szEvent),IDS_MESSAGE_PLAYING);
				break;
		}
		
		// se il nome file sfora, lo accorcia con [...]
		CFilenameFactory fn;
		int n = _snprintf(szTooltipText,sizeof(szTooltipText)-1,"%s",fn.Abbreviate(szAudioFileName,TRAYICON_MAX_TOOLTIP_TEXT-(5+strlen(szEvent)+1)));
		_snprintf(szTooltipText+n,sizeof(szTooltipText)-1-n," ~ (%s)",szEvent);

		m_strTitleBar.Format("%s ~ (%s)",szAudioFileName,szEvent);
	}
	else
	{
		if(wParam==AUDIOPLAYER_COMMAND_EXIT)
			m_strTitleBar.Format("%s v.%s",WALLPAPER_PROGRAM_NAME_ALIAS,WALLPAPER_VERSION);
		strcpyn(szTooltipText,m_strTitleBar,sizeof(szTooltipText));
	}
	
	SetWindowTitleString(m_strTitleBar);

	if(m_pTrayIcon)
		m_pTrayIcon->SetToolTip(szTooltipText);
	
	return(0L);
}

/*
	LoadTaskbarPopupList()

	Carica la lista per i bitmaps per gli skins.
*/
BOOL CWallPaperDlg::LoadTaskbarPopupList(CTaskbarPopupList* pTaskbarList,int nType/* 0=audio, 1=pict */)
{
	int i = 0;
	char* pToken = NULL;
	char szBuffer[512] = {0};
	TASKBARPOPUP* pAudioPopupInfo = NULL;
	char* pFileName;
	CFindFile findFile;
	char szSkinsDirectory[_MAX_FILEPATH+1];
	char szBitmapDirectory[_MAX_FILEPATH+1];
	char szBitmapFileName[_MAX_FILEPATH+1];
	strcpyn(szSkinsDirectory,nType==0 ? m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_AUDIOPOPUPDIR_KEY) : m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_PICTPOPUPDIR_KEY),sizeof(szSkinsDirectory));
	::EnsureBackslash(szSkinsDirectory,sizeof(szSkinsDirectory));

	// usa l'oggetto immagine per ricavare (e confrontare) le dimensioni reali del bitmap
	CImage* pImage;
	CImageFactory ImageFactory;
	char szLibraryName[_MAX_PATH+1];
	char* p = (char*)m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_CURRENTLIBRARY_KEY);
	if(!strnull(p))
	{
		strcpyn(szLibraryName,p,sizeof(szLibraryName));
		pImage = ImageFactory.Create(szLibraryName,sizeof(szLibraryName));
	}
	else
		pImage = ImageFactory.Create();

	// azzera la lista corrente	
	pTaskbarList->RemoveAll();
	
	// estrae i bitmaps/.ini di default se i popup non esistono
	if(nType==0)
	{
		int i;
		char szResource[_MAX_FILEPATH+1];
		char* popup[] = {
			"audiopopupcda",
			"audiopopupmp3",
			"audiopopupwav",
			NULL
			};
		int popupid[] = {
			IDR_AUDIOPOPUPCDA_PNG,
			IDR_AUDIOPOPUPMP3_PNG,
			IDR_AUDIOPOPUPWAV_PNG,
			-1
			};
		int popupiniid[] = {
			IDR_AUDIOPOPUPCDA_INI,
			IDR_AUDIOPOPUPMP3_INI,
			IDR_AUDIOPOPUPWAV_INI,
			-1
			};

		for(i = 0; popup[i]!=NULL; i++)
		{
			_snprintf(szResource,sizeof(szResource)-1,"%s%s.png",m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_AUDIOPOPUPDIR_KEY),popup[i]);
			if(!::FileExist(szResource))
				::ExtractResource(popupid[i],"GIF",szResource);
			_snprintf(szResource,sizeof(szResource)-1,"%s%s.ini",m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_AUDIOPOPUPDIR_KEY),popup[i]);
			if(!::FileExist(szResource))
				::ExtractResource(popupiniid[i],"TXT",szResource);
		}
	}
	else if(nType==1)
	{
		int i;
		char szResource[_MAX_FILEPATH+1];
		char* popup[] = {
			"pcpopup",
			"macpopup",
			NULL
			};
		int popupid[] = {
			IDR_PICTPCPOPUP_PNG,
			IDR_PICTMACPOPUP_PNG,
			-1
			};
		int popupiniid[] = {
			IDR_PICTPCPOPUP_INI,
			IDR_PICTMACPOPUP_INI,
			-1
			};

		for(i = 0; popup[i]!=NULL; i++)
		{
			_snprintf(szResource,sizeof(szResource)-1,"%s%s.png",m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_PICTPOPUPDIR_KEY),popup[i]);
			if(!::FileExist(szResource))
				::ExtractResource(popupid[i],"GIF",szResource);
			_snprintf(szResource,sizeof(szResource)-1,"%s%s.ini",m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_PICTPOPUPDIR_KEY),popup[i]);
			if(!::FileExist(szResource))
				::ExtractResource(popupiniid[i],"TXT",szResource);
		}
	}
	else
		return(FALSE);

	// carica tutti i file (.ini) presenti nella directory di default (subdirectories incluse)
	while((pFileName = (char*)findFile.FindEx(szSkinsDirectory,"*.ini",TRUE))!=NULL)
	{
		// controlla se deve caricare il bitmap
		//
		// [General]
		// LoadIt=<yes|no>
		::GetPrivateProfileString("General","LoadIt","yes",szBuffer,sizeof(szBuffer)-1,pFileName);
		if(stricmp(szBuffer,"yes")==0 || stricmp(szBuffer,"1")==0)
		{
			pAudioPopupInfo = (TASKBARPOPUP*)pTaskbarList->Add();
			if(pAudioPopupInfo)
			{
				// ricava il pathname del .ini per impostare quello del bitmap se non specificato
				// (se viene specificato un pathname per il bitmap, assume che risieda in un altra directory)
				findFile.SplitPathName(pFileName,szBitmapDirectory,sizeof(szBitmapDirectory),szBitmapFileName,sizeof(szBitmapFileName),FALSE);

				memset(pAudioPopupInfo,'\0',sizeof(TASKBARPOPUP));

				// [About]
				// Name=<name of the skin>
				// Author=<name of the author of the skin>
				// Copyright=<copyright for the picture>
				// Url=<the url of the author, if any>
				::GetPrivateProfileString("About","Name","Unnamed",pAudioPopupInfo->szName,TASKBARPOPUP_MAX_NAME,pFileName);
				::GetPrivateProfileString("About","Author","Unknown",pAudioPopupInfo->szAuthor,TASKBARPOPUP_MAX_AUTHOR,pFileName);
				::GetPrivateProfileString("About","Copyright","",pAudioPopupInfo->szCopyright,TASKBARPOPUP_MAX_COPYRIGHT,pFileName);
				::GetPrivateProfileString("About","Url",WALLPAPER_WEB_SITE,pAudioPopupInfo->szUrl,TASKBARPOPUP_MAX_URL,pFileName);

				// [Bitmap]
				// Picture=<the full name of the picture>
				// TextArea=<n,n,n,n> (left,top,right,bottom)
				// TransparentColor=<n,n,n> (rgb 255,0,255)
				::GetPrivateProfileString("Bitmap","Picture","",szBuffer,sizeof(szBuffer)-1,pFileName);
				if(!strnull(szBuffer))
				{
					// imposta il pathname per il bitmap
					if(strchr(szBuffer,'\\'))
						strcpyn(pAudioPopupInfo->szPicture,szBuffer,TASKBARPOPUP_MAX_PICTURENAME+1);
					else
						_snprintf(pAudioPopupInfo->szPicture,TASKBARPOPUP_MAX_PICTURENAME,"%s%s",szBitmapDirectory,szBuffer);
				}
				else
				{
					// nessun bitmap specificato
					::MessageBoxResourceEx(this->GetSafeHwnd(),MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_SKIN_INVALID_BITMAP,pFileName,pAudioPopupInfo->szName);
					delete pAudioPopupInfo,pAudioPopupInfo = NULL;
					continue;
				}
				// carica l'immagine e ricava i valori per la dimensione della text area (left,top,right,bottom)
				::GetPrivateProfileString("Bitmap","TextArea","",szBuffer,sizeof(szBuffer)-1,pFileName);
				pToken = strtok(szBuffer,",");
				for(i=0; pToken; i++)
				{
					if(i==0)
						pAudioPopupInfo->nTextAreaLeft = atoi(pToken);
					else if(i==1)
						pAudioPopupInfo->nTextAreaTop = atoi(pToken);
					else if(i==2)
						pAudioPopupInfo->nTextAreaRight = atoi(pToken);
					else if(i==3)
						pAudioPopupInfo->nTextAreaBottom = atoi(pToken);
					pToken = strtok(NULL,",");
				}
				if(pImage)
				{
					// carica l'immagine
					if(pImage->Load(pAudioPopupInfo->szPicture))
					{
						// imposta le dimensioni del popup
						pAudioPopupInfo->nPictureWidth = pImage->GetWidth();
						pAudioPopupInfo->nPictureHeight = pImage->GetHeight();

						// normalizza le dimensioni della text area
						CRect rcBitmap(0,0,pImage->GetWidth(),pImage->GetHeight());
						CRect rcText(pAudioPopupInfo->nTextAreaLeft,pAudioPopupInfo->nTextAreaTop,pAudioPopupInfo->nTextAreaRight,pAudioPopupInfo->nTextAreaBottom);
						rcText.NormalizeRect();
						if(rcText.left==0 && rcText.top==0 && rcText.right==0 && rcText.bottom==0)
							rcText.SetRect(rcBitmap.left,rcBitmap.top,rcBitmap.right,rcBitmap.bottom);
						if(	rcText.left >= rcBitmap.left &&
							rcText.top >= rcBitmap.top &&
							rcText.right <= rcBitmap.right &&
							rcText.bottom <= rcBitmap.bottom)
							;
						else
						{
							rcText.SetRect(rcBitmap.left,rcBitmap.top,rcBitmap.right,rcBitmap.bottom);
							::MessageBoxResourceEx(this->GetSafeHwnd(),MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_SKIN_INVALID_TEXT_RECT,pAudioPopupInfo->szName);
						}
						pAudioPopupInfo->nTextAreaLeft = rcText.left;
						pAudioPopupInfo->nTextAreaTop = rcText.top;
						pAudioPopupInfo->nTextAreaRight = rcText.right;
						pAudioPopupInfo->nTextAreaBottom = rcText.bottom;
					}
				}
				// colore per le aree trasparenti
				::GetPrivateProfileString("Bitmap","TransparentColor","255,0,255",szBuffer,sizeof(szBuffer)-1,pFileName);
				pToken = strtok(szBuffer,",");
				for(i=0; pToken; i++)
				{
					if(i==0)
						pAudioPopupInfo->R_Transparent = (BYTE)atoi(pToken);
					else if(i==1)
						pAudioPopupInfo->G_Transparent = (BYTE)atoi(pToken);
					else if(i==2)
						pAudioPopupInfo->B_Transparent = (BYTE)atoi(pToken);
					pToken = strtok(NULL,",");
				}
				if(	pAudioPopupInfo->R_Transparent < 0 || pAudioPopupInfo->R_Transparent > 255 ||
					pAudioPopupInfo->G_Transparent < 0 || pAudioPopupInfo->G_Transparent > 255 ||
					pAudioPopupInfo->B_Transparent < 0 || pAudioPopupInfo->B_Transparent > 255
					)
				{
					::MessageBoxResourceEx(this->GetSafeHwnd(),MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_SKIN_INVALID_RGB_TRANSPARENCY,pAudioPopupInfo->R_Transparent,pAudioPopupInfo->G_Transparent,pAudioPopupInfo->B_Transparent,pAudioPopupInfo->szName);
					pAudioPopupInfo->R_Transparent = 255;
					pAudioPopupInfo->G_Transparent = 0;
					pAudioPopupInfo->B_Transparent = 255;
				}

				// [Font]
				// Name=<the name of the font>
				// Size=<n>
				// Color=<n,n,n>
				// OnMouseOverColor=<n,n,n>
				// Style=TN_TEXT_NORMAL	(TN_TEXT_BOLD,TN_TEXT_ITALIC,TN_TEXT_NORMAL)
				// Align=DT_LEFT		(DT_CENTER,DT_LEFT,DT_RIGHT,DT_BOTTOM,DT_TOP)
				::GetPrivateProfileString("Font","Name","Tahoma",pAudioPopupInfo->szFontName,TASKBARPOPUP_MAX_FONTNAME,pFileName);
				pAudioPopupInfo->nFontSize = ::GetPrivateProfileInt("Font","Size",80,pFileName);
				::GetPrivateProfileString("Font","Color","0,0,0",szBuffer,sizeof(szBuffer)-1,pFileName);
				pToken = strtok(szBuffer,",");
				for(i=0; pToken; i++)
				{
					if(i==0)
						pAudioPopupInfo->R_Text = (BYTE)atoi(pToken);
					else if(i==1)
						pAudioPopupInfo->G_Text = (BYTE)atoi(pToken);
					else if(i==2)
						pAudioPopupInfo->B_Text = (BYTE)atoi(pToken);
					pToken = strtok(NULL,",");
				}
				if(	pAudioPopupInfo->R_Text < 0 || pAudioPopupInfo->R_Text > 255 ||
					pAudioPopupInfo->G_Text < 0 || pAudioPopupInfo->G_Text > 255 ||
					pAudioPopupInfo->B_Text < 0 || pAudioPopupInfo->B_Text > 255
					)
				{
					::MessageBoxResourceEx(this->GetSafeHwnd(),MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_SKIN_INVALID_RGB_TEXT,pAudioPopupInfo->R_Text,pAudioPopupInfo->G_Text,pAudioPopupInfo->B_Text,pAudioPopupInfo->szName);
					pAudioPopupInfo->R_Text = 0;
					pAudioPopupInfo->G_Text = 0;
					pAudioPopupInfo->B_Text = 0;
				}
				::GetPrivateProfileString("Font","OnMouseOverColor","0,0,200",szBuffer,sizeof(szBuffer)-1,pFileName);
				pToken = strtok(szBuffer,",");
				for(i=0; pToken; i++)
				{
					if(i==0)
						pAudioPopupInfo->R_SelectedText = (BYTE)atoi(pToken);
					else if(i==1)
						pAudioPopupInfo->G_SelectedText = (BYTE)atoi(pToken);
					else if(i==2)
						pAudioPopupInfo->B_SelectedText = (BYTE)atoi(pToken);
					pToken = strtok(NULL,",");
				}
				if(	pAudioPopupInfo->R_SelectedText < 0 || pAudioPopupInfo->R_SelectedText > 255 ||
					pAudioPopupInfo->G_SelectedText < 0 || pAudioPopupInfo->G_SelectedText > 255 ||
					pAudioPopupInfo->B_SelectedText < 0 || pAudioPopupInfo->B_SelectedText > 255
					)
				{
					::MessageBoxResourceEx(this->GetSafeHwnd(),MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_SKIN_INVALID_RGB_TEXT,pAudioPopupInfo->R_SelectedText,pAudioPopupInfo->G_SelectedText,pAudioPopupInfo->B_SelectedText,pAudioPopupInfo->szName);
					pAudioPopupInfo->R_SelectedText = 0;
					pAudioPopupInfo->G_SelectedText = 0;
					pAudioPopupInfo->B_SelectedText = 0;
				}
				::GetPrivateProfileString("Font","Style","TN_TEXT_NORMAL",szBuffer,sizeof(szBuffer)-1,pFileName);
				if(stricmp(szBuffer,"TN_TEXT_BOLD")==0)
					pAudioPopupInfo->nFontStyle = TN_TEXT_BOLD;
				else if(stricmp(szBuffer,"TN_TEXT_ITALIC")==0)
					pAudioPopupInfo->nFontStyle = TN_TEXT_ITALIC;
				else if(stricmp(szBuffer,"TN_TEXT_NORMAL")==0)
					pAudioPopupInfo->nFontStyle = TN_TEXT_NORMAL;
				else
					pAudioPopupInfo->nFontStyle = TN_TEXT_NORMAL;
				::GetPrivateProfileString("Font","Align","DT_LEFT",szBuffer,sizeof(szBuffer)-1,pFileName);
				pAudioPopupInfo->dwFontAlign = DT_NOPREFIX|DT_WORDBREAK|DT_END_ELLIPSIS;
				if(stricmp(szBuffer,"DT_CENTER")==0)
					pAudioPopupInfo->dwFontAlign |= (DT_CENTER|DT_VCENTER);
				else if(stricmp(szBuffer,"DT_LEFT")==0)
					pAudioPopupInfo->dwFontAlign |= DT_LEFT;
				else if(stricmp(szBuffer,"DT_RIGHT")==0)
					pAudioPopupInfo->dwFontAlign |= DT_RIGHT;
				else if(stricmp(szBuffer,"DT_BOTTOM")==0)
					pAudioPopupInfo->dwFontAlign |= DT_BOTTOM;
				else if(stricmp(szBuffer,"DT_TOP")==0)
					pAudioPopupInfo->dwFontAlign |= DT_TOP;
				else
					pAudioPopupInfo->dwFontAlign = DT_LEFT;

				// [Popup]
				// Text=<text to display>
				// Tooltip=<yes|no>
				// Scroll=<yes|no>
				// Direction=<left|right|none>
				// Speed=200				(50...1000)
				// TimeToShow=600			(100...10000)
				// TimeToStay=5000			(100...10000/playtime)
				// TimeToHide=200			(100...10000)
				// Transparency=255			(0=invisible/255=opaque)
				// AlwaysOnTop=no			(yes/no)
				// AbsolutePosition=<n,n>	(-1,-1)
				// CloseButton=n,n,n,n		(left,top,right,bottom)
				::GetPrivateProfileString("Popup","Text","%t\n%a\n%d\n\n%B, %F, %C, %T",pAudioPopupInfo->szPopupText,TASKBARPOPUP_MAX_POPUPTEXT,pFileName);
				::GetPrivateProfileString("Popup","Tooltip","1",szBuffer,sizeof(szBuffer)-1,pFileName);
				if(stricmp(szBuffer,"yes")==0 || stricmp(szBuffer,"1")==0)
					pAudioPopupInfo->nPopupTooltip = 1;
				else
					pAudioPopupInfo->nPopupTooltip = 0;
				::GetPrivateProfileString("Popup","Scroll","0",szBuffer,sizeof(szBuffer)-1,pFileName);
				if(stricmp(szBuffer,"yes")==0 || stricmp(szBuffer,"1")==0)
					pAudioPopupInfo->nPopupScrollText = 1;
				else
					pAudioPopupInfo->nPopupScrollText = 0;
				if(pAudioPopupInfo->nPopupScrollText)
				{
					if(pAudioPopupInfo->dwFontAlign & DT_WORDBREAK)
					{
						pAudioPopupInfo->dwFontAlign &= ~DT_WORDBREAK;
						pAudioPopupInfo->dwFontAlign |= DT_SINGLELINE;
					}
					if(pAudioPopupInfo->dwFontAlign & DT_END_ELLIPSIS)
						pAudioPopupInfo->dwFontAlign &= ~DT_END_ELLIPSIS;
				}
				::GetPrivateProfileString("Popup","Direction","left",szBuffer,sizeof(szBuffer)-1,pFileName);
				if(stricmp(szBuffer,"left")==0)
					pAudioPopupInfo->nPopupScrollDirection = -1;
				else if(stricmp(szBuffer,"right")==0)
					pAudioPopupInfo->nPopupScrollDirection = 1;
				else
					pAudioPopupInfo->nPopupScrollDirection = 0;
				// 200 (50...1000)
				pAudioPopupInfo->nPopupScrollSpeed = ::GetPrivateProfileInt("Popup","Speed",200,pFileName);
				if(pAudioPopupInfo->nPopupScrollSpeed < 50 || pAudioPopupInfo->nPopupScrollSpeed > 1000)
					pAudioPopupInfo->nPopupScrollSpeed = 200;
				// 600 (100...10000)
				pAudioPopupInfo->nPopupTimeToShow = ::GetPrivateProfileInt("Popup","TimeToShow",TASKBARPOPUP_DEFAULT_TIMETOSHOW,pFileName);
				if(pAudioPopupInfo->nPopupTimeToShow < TASKBARPOPUP_MIN_POPUPTIME || pAudioPopupInfo->nPopupTimeToShow > TASKBARPOPUP_MAX_POPUPTIME)
					pAudioPopupInfo->nPopupTimeToShow = TASKBARPOPUP_DEFAULT_TIMETOSHOW;
				// 5000 (100...10000)
				::GetPrivateProfileString("Popup","TimeToStay",STR(TASKBARPOPUP_DEFAULT_TIMETOSTAY),szBuffer,sizeof(szBuffer)-1,pFileName);
				if(stricmp(szBuffer,"playtime")==0)
					pAudioPopupInfo->dwPopupTimeToStay = (DWORD)-1L;
				else
				{
					pAudioPopupInfo->dwPopupTimeToStay = strtoul(szBuffer,NULL,0);;
					if(pAudioPopupInfo->dwPopupTimeToStay < TASKBARPOPUP_MIN_POPUPTIME || pAudioPopupInfo->dwPopupTimeToStay > TASKBARPOPUP_MAX_POPUPTIME)
						pAudioPopupInfo->dwPopupTimeToStay = TASKBARPOPUP_DEFAULT_TIMETOSTAY;
				}
				// 200 (100...10000)
				pAudioPopupInfo->nPopupTimeToHide = ::GetPrivateProfileInt("Popup","TimeToHide",TASKBARPOPUP_DEFAULT_TIMETOHIDE,pFileName);
				if(pAudioPopupInfo->nPopupTimeToHide < TASKBARPOPUP_MIN_POPUPTIME || pAudioPopupInfo->nPopupTimeToHide > TASKBARPOPUP_MAX_POPUPTIME)
					pAudioPopupInfo->nPopupTimeToHide = TASKBARPOPUP_DEFAULT_TIMETOHIDE;
				// 255
				pAudioPopupInfo->nPopupTransparency = (BYTE)::GetPrivateProfileInt("Popup","Transparency",LWA_ALPHA_OPAQUE,pFileName);
				if(pAudioPopupInfo->nPopupTransparency < LWA_ALPHA_INVISIBLE || pAudioPopupInfo->nPopupTransparency > LWA_ALPHA_OPAQUE)
					pAudioPopupInfo->nPopupTransparency = LWA_ALPHA_OPAQUE;
				::GetPrivateProfileString("Popup","AlwaysOnTop","0",szBuffer,sizeof(szBuffer)-1,pFileName);
				if(stricmp(szBuffer,"yes")==0 || stricmp(szBuffer,"1")==0)
					pAudioPopupInfo->nPopupAlwaysOnTop = 1;
				else
					pAudioPopupInfo->nPopupAlwaysOnTop = 0;
				// coordinate assolute per la visualizzazione
				::GetPrivateProfileString("Popup","AbsolutePosition","-1,-1",szBuffer,sizeof(szBuffer)-1,pFileName);
				pToken = strtok(szBuffer,",");
				for(i=0; pToken; i++)
				{
					if(i==0)
						pAudioPopupInfo->nPopupXPos = atoi(pToken);
					else if(i==1)
						pAudioPopupInfo->nPopupYPos = atoi(pToken);
					pToken = strtok(NULL,",");
				}
				// coordinate per il bottone di chiusura (left,top,right,bottom)
				::GetPrivateProfileString("Popup","CloseButton","",szBuffer,sizeof(szBuffer)-1,pFileName);
				pToken = strtok(szBuffer,",");
				for(i=0; pToken; i++)
				{
					if(i==0)
						pAudioPopupInfo->nPopupCloseLeft = atoi(pToken);
					else if(i==1)
						pAudioPopupInfo->nPopupCloseTop = atoi(pToken);
					else if(i==2)
						pAudioPopupInfo->nPopupCloseRight = atoi(pToken);
					else if(i==3)
						pAudioPopupInfo->nPopupCloseBottom = atoi(pToken);
					pToken = strtok(NULL,",");
				}
			}
		}
	}

	return(pTaskbarList->Count() > 0);
}

/*
	OnUnCompress()

	Decomprime il file selezionato.
*/
void CWallPaperDlg::OnUnCompress(void)
{
	int nItem;
	char szItem[MAX_ITEM_SIZE+1];

	// controlla la selezione corrente	
	if((nItem = m_wndPlaylist.GetCurrentItem()) >= 0)
		m_wndPlaylist.GetItemText(nItem,CTRLLISTEX_FILENAME_INDEX,szItem,sizeof(szItem)-1);
	else
		return;

	// controlla che sia un file compresso
	char szLocation[MAX_ITEM_SIZE+1];
	m_wndPlaylist.GetItemText(nItem,CTRLLISTEX_LOCATION_INDEX,szLocation,sizeof(szLocation)-1);
	if(!m_Archive.IsSupportedFormat(szLocation))
	{
		::MessageBoxResource(this->GetSafeHwnd(),MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_NOT_COMPRESSED_FILE);
		return;
	}

	BOOL bExtractOne = ::MessageBoxResourceEx(this->m_hWnd,MB_ICONQUESTION|MB_YESNO,WALLPAPER_PROGRAM_NAME,IDS_QUESTION_UNCOMPRESS_FILE,szItem)==IDYES;

	// selezione della directory
	CDirDialog dlg(m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_UNCOMPRESS_KEY),
				"Select Directory...",
				"Select the output folder:",
				m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_ALWAYSONTOP_KEY)
				);
	if(dlg.DoModal(this->m_hWnd)==IDOK)
	{
		char szFolder[_MAX_FILEPATH+1] = {0};
		strcpyn(szFolder,dlg.GetPathName(),sizeof(szFolder));
		m_Config.UpdateString(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_UNCOMPRESS_KEY,szFolder);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_UNCOMPRESS_KEY);
		
		CWaitCursor cursor;
		SetStatusBarInfoPane(IDS_MESSAGE_UNCOMPRESSING);

		LONG lRet;
		if((lRet = m_Archive.Extract(bExtractOne ? szItem : "*",szLocation,szFolder))!=0L)
		{
			::MessageBoxResourceEx(this->m_hWnd,MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_EXTRACT_FILE,bExtractOne ? szItem : "*",szLocation,m_Archive.GetErrorCodeDescription(lRet,szLocation),lRet);
			SetWindowTitleID(IDS_ERROR_EXTRACTFILE,bExtractOne ? szItem : "*");
		}
		
		SetStatusBarInfoPane(IDS_MESSAGE_READY);
	}
}

/*
	OnNervousTitle()

	Aggiorna il flag per lo scorrimento del titolo.
*/
void CWallPaperDlg::OnNervousTitle(BOOL bFlag)
{
	if(m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_NERVOUS_TITLE_KEY))
	{
		// crea il timer per lo scorrimento del titolo
		if(m_nNervousTitleTimerId > 0)
		{
			KillTimer(m_nNervousTitleTimerId);
			m_nNervousTitleTimerId = 0;
		}
		if((m_nNervousTitleTimerId = SetTimer(ID_TIMER_NERVOUS_TITLE,200L,NULL))==0)
		{
			::MessageBoxResource(this->m_hWnd,MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_NERVOUSTITLE_TIMER);
			CDialogEx::OnExit();
			return;
		}
	}
	else
	{
		// elimina il timer per lo scorrimento del titolo
		if(m_nNervousTitleTimerId > 0)
		{
			KillTimer(m_nNervousTitleTimerId);
			m_nNervousTitleTimerId = 0;
		}

		// aggiorna l'icona del dialogo
		CDialogEx::SetIcon(IDI_ICON_WALLPAPER);
		if(m_pTrayIcon)
			m_pTrayIcon->SetIcon(IDI_ICON_WALLPAPER);
	}
	
	SetWindowTitleString(m_strTitleBar,bFlag);
}

/*
	OnContextMenu()

	Aggiorna il flag per il menu contestuale.
*/
void CWallPaperDlg::OnContextMenu(void)
{
	CWaitCursor cursor;
	CRegistry registry;
	LPIMAGETYPE p;
	char szShellCommand[_MAX_FILEPATH+1];

	// imposta il comando per la shell per i files grafici
	_snprintf(szShellCommand,sizeof(szShellCommand)-1,"%s /w%%1",m_Config.GetString(WALLPAPER_INSTALL_KEY,WALLPAPER_PROGRAM_KEY));
	
	// per ognuno dei formati grafici gestiti, aggiunge/rimuove nel registro l'entrata per il menu contestuale
	while((p = m_pImage->EnumReadableImageFormats())!=(LPIMAGETYPE)NULL)
	{
		if(p)
		{
			if(m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CONTEXT_MENU_KEY))
				registry.AddMenuEntryForRegisteredFileType(p->ext,"Set as WallPaper",szShellCommand);
			else
				registry.RemoveMenuEntryForRegisteredFileType(p->ext,"Set as WallPaper");
		}
	}

	// imposta il comando per la shell per i files audio
	_snprintf(szShellCommand,sizeof(szShellCommand)-1,"%s /a%%1",m_Config.GetString(WALLPAPER_INSTALL_KEY,WALLPAPER_PROGRAM_KEY));
	
	// per ognuno dei formati audio gestiti, aggiunge/rimuove nel registro l'entrata per il menu contestuale
	if(m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CONTEXT_MENU_KEY))
	{
		registry.AddMenuEntryForRegisteredFileType(".mp3","Play (now) in WallPaper",szShellCommand);
		registry.AddMenuEntryForRegisteredFileType(".wav","Play (now) in WallPaper",szShellCommand);
		registry.AddMenuEntryForRegisteredFileType(".cda","Play (now) in WallPaper",szShellCommand);
	}
	else
	{
		registry.RemoveMenuEntryForRegisteredFileType(".mp3","Play (now) in WallPaper");
		registry.RemoveMenuEntryForRegisteredFileType(".wav","Play (now) in WallPaper");
		registry.RemoveMenuEntryForRegisteredFileType(".cda","Play (now) in WallPaper");
	}

	// imposta il comando per la shell per i files audio
	_snprintf(szShellCommand,sizeof(szShellCommand)-1,"%s /A%%1",m_Config.GetString(WALLPAPER_INSTALL_KEY,WALLPAPER_PROGRAM_KEY));
	
	// per ognuno dei formati audio gestiti, aggiunge/rimuove nel registro l'entrata per il menu contestuale
	if(m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CONTEXT_MENU_KEY))
	{
		registry.AddMenuEntryForRegisteredFileType(".mp3","Play (queued) in WallPaper",szShellCommand);
		registry.AddMenuEntryForRegisteredFileType(".wav","Play (queued) in WallPaper",szShellCommand);
		registry.AddMenuEntryForRegisteredFileType(".cda","Play (queued) in WallPaper",szShellCommand);
	}
	else
	{
		registry.RemoveMenuEntryForRegisteredFileType(".mp3","Play (queued) in WallPaper");
		registry.RemoveMenuEntryForRegisteredFileType(".wav","Play (queued) in WallPaper");
		registry.RemoveMenuEntryForRegisteredFileType(".cda","Play (queued) in WallPaper");
	}
}

/*
	OnHotKeys()

	Visualizza gli hotkeys correnti.
*/
void CWallPaperDlg::OnHotKeys(void)
{
	::MessageBoxResourceEx(	this->m_hWnd,
						MB_OK|MB_ICONINFORMATION,
						WALLPAPER_PROGRAM_NAME,
						IDS_MESSAGE_HOTKEYS
						);
}

/*
	OnHomePage()

	Apre l'url relativa al sito web.
*/
void CWallPaperDlg::OnHomePage(void)
{
	CBrowser browser(m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_PREFERRED_INTERNET_BROWSER_KEY));
	if(!browser.Browse(m_Config.GetString(WALLPAPER_INSTALL_KEY,WALLPAPER_WEBSITE_KEY)))
		::MessageBoxResourceEx(this->m_hWnd,MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_BROWSER,m_Config.GetString(WALLPAPER_INSTALL_KEY,WALLPAPER_WEBSITE_KEY));
}

/*
	OnLatestVersion()

	Scarica l'ultima versione disponibile dal web.
*/
LRESULT CWallPaperDlg::OnLatestVersion(WPARAM wParam,LPARAM /*lParam*/)
{
	CWallPaperDownloadDlg dlg(this,&m_Config);
	BOOL bQuietMode = (BOOL)wParam;
	int nResult = IDOK;
	
	// scarica il file d'aggiornamento (update.txt)
	LPCSTR pUpdateMirrors[] = {
		WALLPAPER_SOURCEFORGE_MIRROR,
		//WALLPAPER_FILESDOTCOM_MIRROR,
		NULL
		};
	
	for(int i=0; pUpdateMirrors[i]!=NULL; i++)
	{
		dlg.Reset();
		dlg.SetWindowTitle("WallPaper - latest version check...");
		dlg.SetForceDownload(TRUE);
		dlg.SetFilename(WALLPAPER_UPDATE_FILE);
		dlg.SetWebSite(pUpdateMirrors[i]);
		dlg.SetDownloadDir(m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_TEMPDIR_KEY));
		dlg.ShowErrors(!bQuietMode);
		dlg.SetVisible(!bQuietMode);
		if((nResult = dlg.DoModal())==IDOK)
			break;
	}

	// download riuscito
	if(nResult==IDOK)
	{
		char szUpdateFile[_MAX_FILEPATH+1];
		char szBuffer[512];
		char szCurrentVersion[16] = {WALLPAPER_VERSION_NUMBER};
		char szVersionType[32] = {""};
		char szUpdateDate[16] = {0};

		// ricava il nome file locale (per update.txt)
		strcpyn(szUpdateFile,dlg.GetDownloadedFilename(),sizeof(szUpdateFile));

		// [WallPaper]
		// ricava il numero della versione (n.n.n)
		if(::GetPrivateProfileString("WallPaper","LastVersion","0.0.0",szBuffer,sizeof(szBuffer)-1,szUpdateFile) > 0)
		{
			// ricava il tipo della versione (x)
			::GetPrivateProfileString("WallPaper","VersionType","",szVersionType,sizeof(szVersionType)-1,szUpdateFile);
			
			char szNum[2] = {0};
			
			// versione on line
			szNum[0] = szBuffer[0];
			int nWebVersion = atoi(szNum);
			szNum[0] = szBuffer[2];
			int nWebRelease = atoi(szNum);
			szNum[0] = szBuffer[4];
			int nWebPatch = atoi(szNum);
			
			// versione corrente
			szNum[0] = szCurrentVersion[0];
			int nCurrentVersion = atoi(szNum);
			szNum[0] = szCurrentVersion[2];
			int nCurrentRelease = atoi(szNum);
			szNum[0] = szCurrentVersion[4];
			int nCurrentPatch = atoi(szNum);

			// [Web]
			// aggiorna la configurazione corrente con i valori presenti nel .ini
			if(::GetPrivateProfileString("Web","WebSite",DEFAULT_WEBSITE,szBuffer,sizeof(szBuffer)-1,szUpdateFile) > 0)
			{
				m_Config.UpdateString(WALLPAPER_INSTALL_KEY,WALLPAPER_WEBSITE_KEY,szBuffer);
				m_Config.SaveKey(WALLPAPER_INSTALL_KEY,WALLPAPER_WEBSITE_KEY);
			}
			if(::GetPrivateProfileString("Web","MailTo",DEFAULT_EMAILADDRESS,szBuffer,sizeof(szBuffer)-1,szUpdateFile) > 0)
			{
				m_Config.UpdateString(WALLPAPER_INSTALL_KEY,WALLPAPER_EMAILADDRESS_KEY,szBuffer);
				m_Config.SaveKey(WALLPAPER_INSTALL_KEY,WALLPAPER_EMAILADDRESS_KEY);
			}
			
			// [WallPaper]
			// data ultimo aggiornamento
			::GetPrivateProfileString("WallPaper","LastUpdate",WALLPAPER_RELEASE_DATE,szUpdateDate,sizeof(szUpdateDate)-1,szUpdateFile);

			// controlla se deve aggiornare (per numero versione)
			BOOL bNeedUpdate = FALSE;
			if(nWebVersion > nCurrentVersion)
			{
				bNeedUpdate = TRUE;
			}
			else if(nWebVersion < nCurrentVersion)
			{
				bNeedUpdate = FALSE;
			}
			else if(nWebVersion==nCurrentVersion)
			{
				if(nWebRelease > nCurrentRelease)
				{
					bNeedUpdate = TRUE;
				}
				else if(nWebRelease < nCurrentRelease)
				{
					bNeedUpdate = FALSE;
				}
				else if(nWebRelease==nCurrentRelease)
				{
					if(nWebPatch > nCurrentPatch)
					{
						bNeedUpdate = TRUE;
					}
					else if(nWebPatch < nCurrentPatch)
					{
						bNeedUpdate = FALSE;
					}
					else if(nWebPatch==nCurrentPatch)
					{
						bNeedUpdate = FALSE;
					}
				}
			}

			// controlla se deve aggiornare (per data di rilascio)
			if(!bNeedUpdate)
			{
				if(szUpdateDate[0]!='\0')
				{
					CDateTime releaseDate(WALLPAPER_RELEASE_DATE,BRITISH);
					CDateTime updateDate(szUpdateDate,BRITISH);
					
					if(updateDate.GetYear() > releaseDate.GetYear())
					{
						bNeedUpdate = TRUE;
					}
					else if(updateDate.GetYear() < releaseDate.GetYear())
					{
						bNeedUpdate = FALSE;
					}
					else if(updateDate.GetYear()==releaseDate.GetYear())
					{
						if(updateDate.GetMonth() > releaseDate.GetMonth())
						{
							bNeedUpdate = TRUE;
						}
						else if(updateDate.GetMonth() < releaseDate.GetMonth())
						{
							bNeedUpdate = FALSE;
						}
						else if(updateDate.GetMonth()==releaseDate.GetMonth())
						{
							if(updateDate.GetDay() > releaseDate.GetDay())
							{
								bNeedUpdate = TRUE;
							}
							else if(updateDate.GetDay() < releaseDate.GetDay())
							{
								bNeedUpdate = FALSE;
							}
							else if(updateDate.GetDay()==releaseDate.GetDay())
							{
								bNeedUpdate = FALSE;
							}
						}
					}
				}
			}

			if(bNeedUpdate)
			{
				bNeedUpdate = ::MessageBoxResourceEx(this->m_hWnd,MB_YESNO|MB_ICONQUESTION,WALLPAPER_PROGRAM_NAME,IDS_QUESTION_LATESTVERSIONDOWNLOAD,nWebVersion,nWebRelease,nWebPatch)==IDYES;
			}
			else
			{
				if(!bQuietMode)
				{
					char szVersion[32];
					_snprintf(szVersion,sizeof(szVersion)-1,"%s",WALLPAPER_VERSION);
					::MessageBoxResourceEx(this->m_hWnd,MB_OK|MB_ICONINFORMATION,WALLPAPER_PROGRAM_NAME,IDS_MESSAGE_NO_NEW_VER_AVAILABLE,szVersion);
				}
			}

			// [Download]
			// scarica la nuova versione
			if(bNeedUpdate)
			{
				BOOL bDownloaded = FALSE;
				
				if(szUpdateDate[0]=='\0')
					strcpyn(szUpdateDate,WALLPAPER_RELEASE_DATE,sizeof(szUpdateDate));
				CDateTime updateDate(szUpdateDate,BRITISH);
				updateDate.SetDateFormat(GMT_SHORT);

				/*
				[Download]
				Total=2
				Mirror1=http://downloads.sourceforge.net/project/crawlpaper/crawlpaper/4.2.2/;U.S.;1
				FileName1=wallpaper-fullbin.4.2.2.zip
				Mirror2=http://www.files.com/file/4d55b1a354e41/;AUS;1
				FileName2=wallpaper-fullbin.bin
				*/
				char szTotal[16] = {0};
				int nTotal = 0;
				int nCurrentMirror = 0;
				::GetPrivateProfileString("Download","Total","",szTotal,sizeof(szTotal)-1,szUpdateFile);
				nTotal = atoi(szTotal);
				nTotal = nTotal <= 0 ? 1 : nTotal;

				char szDownloadUrl[MAX_URL+1] = {0};
				char szBinaryFile[_MAX_FILEPATH+1] = {0};
				char szMirror[32] = {0};
				char szFilename[32] = {0};

				for(nResult = IDOK,nCurrentMirror = 1; nCurrentMirror <= nTotal && !bDownloaded && nResult==IDOK; nCurrentMirror++)
				{
					_snprintf(szMirror,sizeof(szMirror)-1,"Mirror%d",nCurrentMirror);
					::GetPrivateProfileString("Download",szMirror,"",szDownloadUrl,sizeof(szDownloadUrl)-1,szUpdateFile);
					char* p = strchr(szDownloadUrl,';');
					if(p)
						*p = '\0';
					_snprintf(szFilename,sizeof(szFilename)-1,"FileName%d",nCurrentMirror);
					::GetPrivateProfileString("Download",szFilename,"",szBinaryFile,sizeof(szBinaryFile)-1,szUpdateFile);
					
					// controlla se ci sono threads ui in esecuzione, in tal caso
					// l'utente deve chiuderli prima di poter scaricare l'aggiornamento
					ITERATOR iter;
					THREAD* thread;
					DWORD dwStatus;
					if((iter = m_listRunningUIThreads.First())!=(ITERATOR)NULL)
					{
						do
						{
							thread = (THREAD*)iter->data;
							if(thread)
							{
								if(thread->pWinThread)
								{
									if(::GetExitCodeThread((thread->pWinThread)->m_hThread,&dwStatus))
									{
										if(dwStatus==STILL_ACTIVE)
										{
											LPSTR lpThreadName = NULL;
											if(strcmp(thread->szThreadName,"CWallPaperThumbnailsUIThread")==0)
												lpThreadName = "thumbnailer";
											else if(strcmp(thread->szThreadName,"CWallPaperCrawlerUIThread")==0)
												lpThreadName = "crawler";
											else if(strcmp(thread->szThreadName,"CWallPaperPlayerUIThread")==0 || strcmp(thread->szThreadName,"CWallPaperPreviewUIThread")==0)
												lpThreadName = NULL;
											else
												lpThreadName = "unknown";

											if(lpThreadName)
											{
												if(!bQuietMode)
													::MessageBoxResourceEx(this->m_hWnd,MB_OK|MB_ICONWARNING,WALLPAPER_PROGRAM_NAME,IDS_MESSAGE_CLOSE_UITHREAD_BEFORE_LATESTVER,lpThreadName);
												goto done;
											}
										}
									}
								}
							}

							iter = m_listRunningUIThreads.Next(iter);
						
						} while(iter!=(ITERATOR)NULL);
					}

					// scarica l'ultima release
					dlg.Reset();
					dlg.SetWindowTitle("WallPaper - latest version download...");
					dlg.SetForceDownload(TRUE);
					dlg.SetFilename(szBinaryFile);
					dlg.SetWebSite(szDownloadUrl);
					dlg.SetDownloadDir(m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_TEMPDIR_KEY));
					dlg.ShowErrors(!bQuietMode);
					dlg.SetVisible(!bQuietMode);
					nResult = dlg.DoModal();
					
					// download riuscito
					if(nResult==IDOK)
					{
						// passa da .bin a .exe
						char szLatestVersion[_MAX_FILEPATH+1];
						strcpyn(szLatestVersion,dlg.GetDownloadedFilename(),sizeof(szLatestVersion));
						if(striright(szLatestVersion,".bin")==0)
						{
							char szExeName[_MAX_FILEPATH+1];
							strcpyn(szExeName,szLatestVersion,sizeof(szExeName));
							*stristr(szExeName,".bin") = '\0';
							strcatn(szExeName,".exe",sizeof(szExeName));
							::DeleteFile(szExeName);
							::MoveFile(szLatestVersion,szExeName);
							::DeleteFile(szLatestVersion);
							strcpyn(szLatestVersion,szExeName,sizeof(szLatestVersion));
							bDownloaded = TRUE;
						}
						else if(striright(szLatestVersion,".zip")==0)
						{
							if(m_Archive.List(szLatestVersion))
							{
								ITERATOR iter;
								ARCHIVEINFO* info;
								if((iter = m_Archive.GetList()->First())!=(ITERATOR)NULL)
								{
									info = (ARCHIVEINFO*)iter->data;
									if(info)
									{
										if(m_Archive.Extract(info->name,szLatestVersion,m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_TEMPDIR_KEY),FALSE)==0)
										{
											::DeleteFile(szLatestVersion);
											_snprintf(szLatestVersion,sizeof(szLatestVersion)-1,"%s%s",m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_TEMPDIR_KEY),info->name);
											bDownloaded = TRUE;
										}
									}
								}
							}
						}

						// esegue l'installatore
						if(bDownloaded)
						{
							STARTUPINFO si = {0};
							si.cb = sizeof(STARTUPINFO);
							PROCESS_INFORMATION pi = {0};
							if(::CreateProcess(szLatestVersion,NULL,NULL,NULL,FALSE,0L,NULL,NULL,&si,&pi))
							{
								::CloseHandle(pi.hProcess);
								
								// aggiorna la data dell'ultimo aggiornamento
								CDateTime dateTime;
								dateTime.SetDateFormat(GMT_SHORT);
								m_Config.UpdateString(WALLPAPER_INSTALL_KEY,WALLPAPER_LATESTVERSIONCHECK_KEY,dateTime.GetFormattedDate(TRUE));
								m_Config.SaveKey(WALLPAPER_INSTALL_KEY,WALLPAPER_LATESTVERSIONCHECK_KEY);
								
								// termina per permettere l'aggiornamento
								OnExit();
							}
						}
					}
				}
				
				//if(!bDownloaded && !bQuietMode)
					//msg err
			}
		}
		else // dati incorretti dal file d'aggiornamento
		{
			if(!bQuietMode)
			{
				if(::MessageBoxResourceEx(this->m_hWnd,MB_YESNO|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_WEBSITE_INVALID_DATA,m_Config.GetString(WALLPAPER_INSTALL_KEY,WALLPAPER_WEBSITE_KEY))==IDYES)
					OnHomePage();
			}
		}
	}
	else
	{
		if(!bQuietMode)
		{
			if(::MessageBoxResource(this->m_hWnd,MB_YESNO|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_MIRRORS_UNAVAILABLE)==IDYES)
				OnHomePage();
		}
	}

done:

	SetWindowTitleString(WALLPAPER_PROGRAM_TITLE,FALSE,TRUE);
	SetStatusBarInfoPane(IDS_MESSAGE_READY);

	return(0L);
}

/*
	OnAbout()

	Visualizza i crediti.
*/
void CWallPaperDlg::OnAbout(void)
{
	static BOOL bInAbout = FALSE;
	if(!bInAbout)
	{
		bInAbout = TRUE;
		OnMenuFileMinimize();
		CWallPaperAboutDlg dlg(this->GetSafeHwnd());
		dlg.DoModal();
		OnMenuFileMaximize();
		bInAbout = FALSE;
	}
}

/*
	PlaylistSave()
*/
void CWallPaperDlg::PlaylistSave(void)
{
	CWaitCursor cursor;

	SetStatusBarInfoPane(IDS_MESSAGE_SAVING);
	UpdateData(TRUE);

	// Current (elemento corrente)
	if(m_pCmdLine->HaveDplFile())
		::WritePrivateProfileInt(WALLPAPER_DPL_KEY,WALLPAPER_CURRENTPICT_KEY,m_wndPlaylist.GetCurrentItem(),m_pCmdLine->GetDplFile());
	else
	{
		m_Config.UpdateNumber(WALLPAPER_PICTURES_KEY,WALLPAPER_CURRENTPICT_KEY,m_wndPlaylist.GetCurrentItem());
		m_Config.SaveKey(WALLPAPER_PICTURES_KEY,WALLPAPER_CURRENTPICT_KEY);
	}
	
	// Total (totale elementi)
	int nItemCount = m_wndPlaylist.GetItemCount();
	if(m_pCmdLine->HaveDplFile())
	{
		if(nItemCount <= 0)
		{
			char szBuffer[1024];
			FormatResourceStringEx(szBuffer,sizeof(szBuffer),IDS_QUESTION_RESET_DPLFILE,m_pCmdLine->GetDplFile(),m_pCmdLine->GetDplFile());
			if(DoNotAskMoreMessageBox(this->GetSafeHwnd(),szBuffer,0,&m_Config,WALLPAPER_DONOTASKMORE_RESETDPLFILE_KEY,WALLPAPER_DONOTASKMORE_RESETDPLFILE_VALUE_KEY)==IDYES)
				::WritePrivateProfileInt(WALLPAPER_DPL_KEY,WALLPAPER_TOTPICT_KEY,nItemCount,m_pCmdLine->GetDplFile());
			else
				OnUnloadPlaylistFile();
		}
		else
			::WritePrivateProfileInt(WALLPAPER_DPL_KEY,WALLPAPER_TOTPICT_KEY,nItemCount,m_pCmdLine->GetDplFile());
	}
	else
	{
		m_Config.UpdateNumber(WALLPAPER_PICTURES_KEY,WALLPAPER_TOTPICT_KEY,nItemCount);
		m_Config.SaveKey(WALLPAPER_PICTURES_KEY,WALLPAPER_TOTPICT_KEY);
	}

	// Timeout
	if(m_pCmdLine->HaveDplFile())
		::WritePrivateProfileInt(WALLPAPER_DPL_KEY,WALLPAPER_TIMEOUT_KEY,m_nDesktopTimeout,m_pCmdLine->GetDplFile());
	else
	{
		m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_TIMEOUT_KEY,m_nDesktopTimeout);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_TIMEOUT_KEY);
	}
	
	// ...ColumnWidth (aggiorna la dimensione delle colonne)
	if(m_pCmdLine->HaveDplFile())
	{
		::WritePrivateProfileInt(WALLPAPER_DPL_KEY,WALLPAPER_COLUMNWIDTH_FILENAME_KEY,m_wndPlaylist.GetColWidth(CTRLLISTEX_FILENAME_INDEX),m_pCmdLine->GetDplFile());
		::WritePrivateProfileInt(WALLPAPER_DPL_KEY,WALLPAPER_COLUMNWIDTH_SIZE_KEY,m_wndPlaylist.GetColWidth(CTRLLISTEX_SIZE_INDEX),m_pCmdLine->GetDplFile());
		::WritePrivateProfileInt(WALLPAPER_DPL_KEY,WALLPAPER_COLUMNWIDTH_LOCATION_KEY,m_wndPlaylist.GetColWidth(CTRLLISTEX_LOCATION_INDEX),m_pCmdLine->GetDplFile());
		::WritePrivateProfileInt(WALLPAPER_DPL_KEY,WALLPAPER_COLUMNWIDTH_TYPE_KEY,m_wndPlaylist.GetColWidth(CTRLLISTEX_TYPE_INDEX),m_pCmdLine->GetDplFile());
	}
	else
	{
		m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_COLUMNWIDTH_FILENAME_KEY,m_wndPlaylist.GetColWidth(CTRLLISTEX_FILENAME_INDEX));
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_COLUMNWIDTH_FILENAME_KEY);
		m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_COLUMNWIDTH_SIZE_KEY,m_wndPlaylist.GetColWidth(CTRLLISTEX_SIZE_INDEX));
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_COLUMNWIDTH_SIZE_KEY);
		m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_COLUMNWIDTH_LOCATION_KEY,m_wndPlaylist.GetColWidth(CTRLLISTEX_LOCATION_INDEX));
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_COLUMNWIDTH_LOCATION_KEY);
		m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_COLUMNWIDTH_TYPE_KEY,m_wndPlaylist.GetColWidth(CTRLLISTEX_TYPE_INDEX));
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_COLUMNWIDTH_TYPE_KEY);
	}

	// File... (elementi)
	if(m_wndPlaylist.GetItemCount() > 0)
	{
		int i,n;
		CString cFileName;
		char szItem[MAX_ITEM_SIZE+1];

		// elimina le entrate correnti del registro
		int nItemCount = 0;

		if(m_pCmdLine->HaveDplFile())
			nItemCount = ::GetPrivateProfileInt(WALLPAPER_DPL_KEY,WALLPAPER_TOTPICT_KEY,0,m_pCmdLine->GetDplFile());
		else
			nItemCount = m_Config.GetNumber(WALLPAPER_PICTURES_KEY,WALLPAPER_TOTPICT_KEY);

		for(i = 0,n = 0; i < nItemCount; i++)
		{
			cFileName.Format("File%d",i);
			
			if(m_pCmdLine->HaveDplFile())
				::WritePrivateProfileString(WALLPAPER_DPL_KEY,cFileName,NULL,m_pCmdLine->GetDplFile());
			else
				m_Config.Delete(WALLPAPER_PICTURES_KEY,cFileName);
			
			if(++n > 15)
			{
				n = 0;
				::PeekAndPump();
			}
		}

		for(i = nItemCount,n = 0; i > 0; i++)
		{
			cFileName.Format("File%d",i);
			
			if(m_pCmdLine->HaveDplFile())
			{
				if(::GetPrivateProfileString(WALLPAPER_DPL_KEY,cFileName,"",szItem,sizeof(szItem)-1,m_pCmdLine->GetDplFile()) > 0)
					::WritePrivateProfileString(WALLPAPER_DPL_KEY,cFileName,NULL,m_pCmdLine->GetDplFile());
				else
					i = -1;
			}
			else
				if(!m_Config.Delete(WALLPAPER_PICTURES_KEY,cFileName))
					i = -1;
			
			if(++n > 15)
			{
				n = 0;
				::PeekAndPump();
			}
		}

		// salva le entrate della lista nel registro
		nItemCount = m_wndPlaylist.GetItemCount();

		for(i = 0,n = 0; i < nItemCount; i++)
		{
			GetItem(szItem,sizeof(szItem),i);

			cFileName.Format("File%d",i);
			
			if(m_pCmdLine->HaveDplFile())
				::WritePrivateProfileString(WALLPAPER_DPL_KEY,cFileName,szItem,m_pCmdLine->GetDplFile());
			else
				m_Config.Insert(WALLPAPER_PICTURES_KEY,cFileName,szItem);
			
			if(++n > 15)
			{
				n = 0;
				::PeekAndPump();
			}
		}
		if(!m_pCmdLine->HaveDplFile())
			m_Config.SaveSection(WALLPAPER_PICTURES_KEY);
	}

	// registro/.dpl
	m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_LOADLISTFROM_KEY,m_pCmdLine->HaveDplFile());
	m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_LOADLISTFROM_KEY);
	m_Config.UpdateString(WALLPAPER_OPTIONS_KEY,WALLPAPER_LISTNAME_KEY,m_pCmdLine->HaveDplFile() ? m_pCmdLine->GetDplFile() : DEFAULT_LISTNAME);
	m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_LISTNAME_KEY);

	m_wndPlaylist.SetFocus();
	PlaylistResetModified();
	SetStatusBarInfoPane(IDS_MESSAGE_READY);
}

/*
	OnUpdateGUI()

	Aggiorna l'interfaccia utente.
*/
void CWallPaperDlg::OnUpdateGUI(void)
{
	// se la playlist e' vuota aggiorna il titolo del dialogo
	int nItemCount = m_wndPlaylist.GetItemCount();
	if(nItemCount <= 0)
	{
		SetWindowTitleString(WALLPAPER_PROGRAM_TITLE,FALSE,TRUE);
		if(m_pTrayIcon)
			m_pTrayIcon->SetToolTip(WALLPAPER_PROGRAM_TITLE);
	}

	// modalita' anteprima
	OnPreviewMode(FALSE);

	// aggiorna il menu di sistema
	CMenu* pSysmenu = GetSystemMenu(FALSE);
	if(pSysmenu)
	{
		// abilita/disabilita 'Pause, Favourite, Next, Previous'
		pSysmenu->EnableMenuItem(IDM_SYSMENU_PAUSE,nItemCount > 1 ? MF_ENABLED : MF_DISABLED|MF_GRAYED);
		pSysmenu->EnableMenuItem(IDM_SYSMENU_FAVOURITE,nItemCount > 1 ? MF_ENABLED : MF_DISABLED|MF_GRAYED);
		pSysmenu->EnableMenuItem(IDM_SYSMENU_PREVPIC,nItemCount > 1 ? MF_ENABLED : MF_DISABLED|MF_GRAYED);
		pSysmenu->EnableMenuItem(IDM_SYSMENU_NEXTPIC,nItemCount > 1 ? MF_ENABLED : MF_DISABLED|MF_GRAYED);
	}

	// aggiorna la toolbar
	CDialogEx::EnableToolBarButton(IDC_BUTTON_REMOVE,nItemCount > 0 ? TRUE : FALSE);
	CDialogEx::EnableToolBarButton(IDC_BUTTON_REMOVEALL,nItemCount > 0 ? TRUE : FALSE);
	CDialogEx::EnableToolBarButton(IDC_BUTTON_DELETEFILE,nItemCount > 0 ? TRUE : FALSE);

	// aggiorna la barra di stato
	SetStatusBarDplPanel(m_pCmdLine->HaveDplFile() ? m_pCmdLine->GetDplFile() : NULL);
	SetStatusBarPicturePanelCount(nItemCount);
}

/*
	OnEnableGUI()

	Abilita/disabilita l'interfaccia utente.
*/
LRESULT CWallPaperDlg::OnEnableGUI(WPARAM bEnable,LPARAM lPauseResume)
{
	UINT nState = bEnable ? MF_ENABLED : MF_GRAYED;
	WORD nPauseState = LOWORD(lPauseResume);
	WORD nResumeState = HIWORD(lPauseResume);
	CWnd* pWnd = NULL;

	pWnd = GetDlgItem(IDM_MENU);
	if(pWnd)
		pWnd->EnableWindow(bEnable);

	// menu principale
	CMenu* pMenu = NULL;
	pMenu = GetMenu();

	if(pMenu)
	{
		pMenu->EnableMenuItem(IDM_FILE_OPEN,nState);
		pMenu->EnableMenuItem(IDM_FILE_SAVEAS,nState);
		pMenu->EnableMenuItem(IDM_PLAYLIST_SAVEPLAYLIST,nState);
		pMenu->EnableMenuItem(IDM_FILE_DELETE,nState);

		pMenu->EnableMenuItem(IDM_PLAYLIST_PREVIOUS,nState);
		pMenu->EnableMenuItem(IDM_PLAYLIST_NEXT,nState);
		pMenu->EnableMenuItem(IDM_PLAYLIST_PAUSE,nPauseState);		// special case
		pMenu->EnableMenuItem(IDM_PLAYLIST_RESUME,nResumeState);	// special case
		pMenu->EnableMenuItem(IDM_PLAYLIST_ADDFILE,nState);
		pMenu->EnableMenuItem(IDM_PLAYLIST_ADDDIR,nState);
		pMenu->EnableMenuItem(IDM_PLAYLIST_ADDURL,nState);
		pMenu->EnableMenuItem(IDM_PLAYLIST_REMOVE,nState);
		pMenu->EnableMenuItem(IDM_PLAYLIST_REMOVEALL,nState);	
		pMenu->EnableMenuItem(IDM_PLAYLIST_UNLOADLIST,nState);
		pMenu->EnableMenuItem(IDM_PLAYLIST_NEWLIST,nState);
		pMenu->EnableMenuItem(IDM_PLAYLIST_LOADLIST,nState);
		pMenu->EnableMenuItem(IDM_PLAYLIST_SAVELIST,nState);
		
		pMenu->EnableMenuItem(IDM_TOOLS_UNDESKTOP,nState);
		pMenu->EnableMenuItem(IDM_TOOLS_DESKTOPICONS,nState);
		pMenu->EnableMenuItem(IDM_TOOLS_DISPLAYPROPERTIES,nState);
		pMenu->EnableMenuItem(IDM_TOOLS_UNCOMPRESS,nState);
		pMenu->EnableMenuItem(IDM_TOOLS_WALLBROWSER,nState);
		pMenu->EnableMenuItem(IDM_TOOLS_THUMBNAILS,nState);
		pMenu->EnableMenuItem(IDM_TOOLS_CRAWLER,nState);
	}

	// preview
	if((pWnd = GetDlgItem(IDC_CHECK_PICTURE_PREVIEW))!=NULL)
		pWnd->EnableWindow(bEnable);

	// campo e bottone per il timeout
	if((pWnd = GetDlgItem(IDC_EDIT_TIMEOUT))!=NULL)
		pWnd->EnableWindow(bEnable);
	if((pWnd = GetDlgItem(IDC_SPIN_TIMEOUT))!=NULL)
		pWnd->EnableWindow(bEnable);

	// playlist
	if((pWnd = GetDlgItem(IDC_LIST_PLAYLIST))!=NULL)
		pWnd->EnableWindow(bEnable);

	// toolbar
	EnableToolBarButton(IDC_BUTTON_ADDFILE,bEnable);
	EnableToolBarButton(IDC_BUTTON_ADDDIR,bEnable);
	EnableToolBarButton(IDC_BUTTON_REMOVE,bEnable);
	EnableToolBarButton(IDC_BUTTON_REMOVEALL,bEnable);
	EnableToolBarButton(IDC_BUTTON_DELETEFILE,bEnable);
	EnableToolBarButton(IDC_BUTTON_UNDESKTOP,bEnable);
	EnableToolBarButton(IDC_BUTTON_WALLBROWSER,bEnable);
	EnableToolBarButton(IDC_BUTTON_CRAWLER,bEnable);

	return(0L);
}

/*
	OnPlaylistPopupMenu()

	Callback per il messaggio inviato dal controllo per la playlist prima della visualizzazione del menu popup.
*/
LRESULT CWallPaperDlg::OnPlaylistPopupMenu(WPARAM wParam,LPARAM lParam)
{
	CMenu* pMenu = (CMenu*)lParam;
	
	if(pMenu)
	{
		// wParam contiene l'indice dell'elemento selezionato, se e' -1 significa
		// che e' stato cliccato su di un elemento vuoto della lista (menu empty)
		if(wParam!=(WPARAM)-1L)
		{
			int nItem;
			char szItem[MAX_ITEM_SIZE+1];
			nItem = GetItem(szItem,sizeof(szItem));

			BOOL bIsPicture = m_pImage->IsSupportedFormat(szItem);
			BOOL bComesFromArchiveFile = FALSE;
			char szLocation[MAX_ITEM_SIZE+1];
			m_wndPlaylist.GetItemText(nItem,CTRLLISTEX_LOCATION_INDEX,szLocation,sizeof(szLocation)-1);
			bComesFromArchiveFile = m_Archive.IsSupportedFormat(szLocation);

			// TrackPopupMenu() does not return until the user selects a menu item or
			// cancels the menu, so EnableMenuItem() is called after the menu has gone
			if(!IsCopyAvailable())
				pMenu->DeleteMenu(IDM_POPUP_COPY,MF_BYCOMMAND);
			if(!IsPasteAvailable())
				pMenu->DeleteMenu(IDM_POPUP_PASTE,MF_BYCOMMAND);
			if(bComesFromArchiveFile)
				pMenu->DeleteMenu(IDM_POPUP_OPEN,MF_BYCOMMAND);
			if(!bComesFromArchiveFile)
				pMenu->DeleteMenu(IDM_POPUP_UNCOMPRESS,MF_BYCOMMAND);
			if(!m_pCmdLine->HaveDplFile())
				pMenu->DeleteMenu(IDM_POPUP_UNLOADLIST,MF_BYCOMMAND);
			if(bIsPicture)
				pMenu->DeleteMenu(IDM_POPUP_FAVOURITE_AUDIO,MF_BYCOMMAND);
			else
				pMenu->DeleteMenu(IDM_POPUP_FAVOURITE_PICTURE,MF_BYCOMMAND);
		}
		else
		{
			if(!IsPasteAvailable())
				pMenu->DeleteMenu(IDM_POPUP_PASTE,MF_BYCOMMAND);
		}
		
		// elimina il separatore se e' rimasto come prima voce del menu
		if(pMenu->GetMenuState(0,MF_BYPOSITION) & MF_SEPARATOR)
			pMenu->DeleteMenu(0,MF_BYPOSITION);
	}

	return(0L);
}

/*
	OnPlaylistButtonDown()

	Click sull'elemento della lista (drag and drop).
*/
LRESULT CWallPaperDlg::OnPlaylistButtonDown(WPARAM wParam,LPARAM /*lParam*/)
{
	int nItem = (int)wParam;
	//MOUSECLICK* m = (MOUSECLICK*)lParam;
	//UINT flags = m->flags;

	if(nItem >= 0)
		OnPreviewMode(TRUE);

	return(0L);
}

/*
	OnPlaylistButtonDoubleClick()

	Doppio click sull'elemento della lista (carica l'elemento selezionato).
*/
LRESULT CWallPaperDlg::OnPlaylistButtonDoubleClick(WPARAM/*wParam*/,LPARAM/*lParam*/)
{
	int nItem = m_wndPlaylist.GetCurrentItem();
	if(nItem >= 0)
		SendWallPaperMessage(nItem);

	return(0L);
}

/*
	OnPlaylistColumnClick()

	Doppio click sull'header della lista.
*/
LRESULT CWallPaperDlg::OnPlaylistColumnClick(WPARAM /*wParam*/,LPARAM /*lParam*/)
{
//	int nCol = (int)wParam;
//	int nOrder = (int)lParam;

	// esegue quanto necessario dopo il riordino e riposiziona all'inizio
	OnPlaylistReorder(0,0);
	PlaylistSetModified();

	return(0L);
}

/*
	OnPlaylistReorder()

	Riordina la playlist.
*/
LRESULT CWallPaperDlg::OnPlaylistReorder(WPARAM /*wParam*/,LPARAM /*lParam*/)
{
	// elementi presenti nella playlist
	int nItemCount = m_wndPlaylist.GetItemCount();
	if(nItemCount <= 0)
		return(0L);

	// azzera la playlist del player audio per reinserire gli elementi riordinati
	// notifica al player la rimozione di tutti gli elementi
	CWinThread* pWinThread = NULL;
	BOOL bHavePlayer = IsUIThreadRunning("CWallPaperPlayerUIThread",&pWinThread);
	if(bHavePlayer && pWinThread)
	{
		CWnd* pWnd = pWinThread->GetMainWnd();
		if(pWnd)
		{
			HWND hWnd = pWnd->GetSafeHwnd();
			if(hWnd)
				::SendMessage(hWnd,WM_AUDIOPLAYER_REMOVEALL,0L,(LPARAM)0L);
		}
	}

	// aspetta che il player termini
	while(IsUIThreadRunning("CWallPaperPlayerUIThread",&pWinThread))
	{
		::PeekAndPump();
		::Sleep(65L);
	}
	
	int i;
	LPARAM lParam;
	char szItem[MAX_ITEM_SIZE+1];
	char szLocation[MAX_ITEM_SIZE+1];
	char szFileName[_MAX_FILEPATH+1];
	CAudioFilesList* pAudioFilesList = NULL;

	// ricrea la playlist per il player audio
	for(i = 0; i < nItemCount; i++)
	{
		// compone il nome del file
		m_wndPlaylist.GetItemText(i,CTRLLISTEX_FILENAME_INDEX,szItem,sizeof(szItem)-1);
		m_wndPlaylist.GetItemText(i,CTRLLISTEX_LOCATION_INDEX,szLocation,sizeof(szLocation)-1);
		_snprintf(szFileName,sizeof(szFileName)-1,"%s\\%s",szLocation,szItem);
		
		// se si tratta di un file audio lo inserisce nella lista per il player
		if(CAudioPlayer::IsSupportedFormat(szFileName))
		{
			if(!pAudioFilesList)
				pAudioFilesList = new CAudioFilesList();
			if(pAudioFilesList)
			{
				AUDIOITEM* audioItem = (AUDIOITEM*)pAudioFilesList->Add();
				if(audioItem)
				{
					if((lParam = m_wndPlaylist.GetItemlParam(i))==(LPARAM)-1L)
					{
						strcpyn(audioItem->file,szFileName,_MAX_FILEPATH+1);
						audioItem->index = pAudioFilesList->Count();
						audioItem->lparam = (LPARAM)-1L;
					}
					else
					{
						strcpyn(audioItem->file,szItem,_MAX_FILEPATH+1);
						audioItem->index = pAudioFilesList->Count();
						audioItem->lparam = lParam;
					}
				}
			}
		}
	}
	
	// passa la lista dei files audio al player
	if(pAudioFilesList)
	{
		if(pAudioFilesList->Count() > 0)
			OnPlayer(NULL,pAudioFilesList,AUDIOPLAYER_COMMAND_PLAY_FROM_LIST,TRUE);
		delete pAudioFilesList;
	}
	
	// seleziona e ricarica il primo elemento della playlist
	m_wndPlaylist.SelectItem(0);
	OnPlaylistButtonDoubleClick(0,0);

	return(0L);
}

/*
	OnLoadPlaylist()

	Gestore per il caricamento della playlist, la carica lanciando un thread.
*/
LRESULT CWallPaperDlg::OnLoadPlaylist(WPARAM /*wParam*/,LPARAM /*lParam*/)
{
	// carica la lista con gli elementi
	// utilizza un thread perche' se la playlist contiene uno sbotto di elementi il dialogo non verrebbe visualizzato
	// fino al termine del caricamento, passato nel thread anche il caricamento del primo elemento della lista ed il
	// lancio del dialogo per l'anteprima
	CThread* pThread;
	if((pThread = ::BeginThread(LoadPlaylist,this,THREAD_PRIORITY_HIGHEST,0L,CREATE_SUSPENDED))!=(CThread*)NULL)
	{
		THREAD* thread = (THREAD*)m_listRunningInternalThreads.Add();
		if(thread)
		{
			thread->pThread = pThread;
			strcpyn(thread->szThreadName,"LoadPlaylist",MAX_THREAD_NAME+1);
			pThread->SetAutoDelete(FALSE);
			pThread->Resume();
		}
	}

	return(0L);
}

/*
	LoadPlaylist()

	Carica la playlist (distingue tra registro/.dpl).
*/
UINT CWallPaperDlg::LoadPlaylist(LPVOID lpVoid)
{
	UINT nRet = (UINT)-1L;
	CWallPaperDlg* This = (CWallPaperDlg*)lpVoid;
	
	if(This)
		nRet = This->LoadPlaylist();

	return(nRet);
}

/*
	LoadPlaylist()

	Carica la playlist (distingue tra registro/.dpl).
*/
int CWallPaperDlg::LoadPlaylist(void)
{
	// disabilita l'interfaccia utente
	OnEnableGUI(FALSE,MAKELPARAM(MF_GRAYED,MF_GRAYED));
	
	// se non viene specificato nessun .dpl, carica quanto presente nel registro
	LPCSTR lpcszDplFile = m_pCmdLine->HaveDplFile() ? m_pCmdLine->GetDplFile() : NULL;
	int nItemCount = 0;
	int nItemCountBeforeLoad = m_wndPlaylist.GetItemCount();
	CAudioFilesList* pAudioFilesList = NULL;

	// aggiorna le barre di stato
	SetWindowTitleID(IDS_MESSAGE_PLAYLIST_LOADING);
	SetStatusBarPicturePanelCount(0);
	SetStatusBarDplPanel(lpcszDplFile);
	SetStatusBarInfoPane(IDS_MESSAGE_PLAYLIST_LOADING);

	// se deve caricare la playlist eliminando quanto gia' presente
	int nPlaylistLoadingMode = m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PLAYLIST_LOADING_KEY);
	if(nPlaylistLoadingMode==PLAYLIST_LOADING_PUT)
	{
		// per fargli caricare (forzosamente) il primo elemento
		nItemCountBeforeLoad = 0;

		// notifica al player la rimozione di tutti gli elementi
		CWinThread* pWinThread = NULL;
		BOOL bHavePlayer = IsUIThreadRunning("CWallPaperPlayerUIThread",&pWinThread);
		if(bHavePlayer && pWinThread)
		{
			CWnd* pWnd = pWinThread->GetMainWnd();
			if(pWnd)
			{
				HWND hWnd = pWnd->GetSafeHwnd();
				if(hWnd)
					::SendMessage(hWnd,WM_AUDIOPLAYER_REMOVEALL,0L,(LPARAM)0L);
			}
		}

		// azzera la lista
		m_wndPlaylist.DeleteAllItems(TRUE);
		m_Config.UpdateNumber(WALLPAPER_PICTURES_KEY,WALLPAPER_CURRENTPICT_KEY,0);
		m_Config.SaveKey(WALLPAPER_PICTURES_KEY,WALLPAPER_CURRENTPICT_KEY);
	}

	// ricava il totale degli elementi presenti (.dpl o registro)
	if(lpcszDplFile)
		nItemCount = ::GetPrivateProfileInt(WALLPAPER_DPL_KEY,WALLPAPER_TOTPICT_KEY,0,lpcszDplFile);
	else
		nItemCount = m_Config.GetNumber(WALLPAPER_PICTURES_KEY,WALLPAPER_TOTPICT_KEY);
	
	// interfaccia utente
	m_wndPlaylist.SetRedraw(FALSE);

	// carica la lista con gli elementi presenti
	if(nItemCount > 0)
	{
		char szFile[_MAX_FILEPATH+1];
		char szFileName[_MAX_FILEPATH+1];
		char* p = NULL;
		register int i,n;
		
		// per tutti gli elementi
		for(i = 0,n = 0; i < nItemCount; i++)
		{
			// controlla se viene richiesta la chiusura del programma durante il caricamento della playlist
			if(m_hTerminateEvent)
			{
				DWORD dwRet = ::WaitForSingleObject(m_hTerminateEvent,5L);
				if(dwRet==WAIT_OBJECT_0)
					return(0);
			}

			// ricava il pathname per l'elemento
			_snprintf(szFile,sizeof(szFile)-1,"File%d",i);
			if(lpcszDplFile)
			{
				if(::GetPrivateProfileString(WALLPAPER_DPL_KEY,szFile,"",szFileName,sizeof(szFileName)-1,lpcszDplFile) > 0)
					p = szFileName;
				else
					p = NULL;
			}
			else
			{
				p = (char*)m_Config.GetString(WALLPAPER_PICTURES_KEY,szFile);
				if(!strnull(p))
					strcpyn(szFileName,p,sizeof(szFileName));
				else
					p = NULL;
			}

			// lo aggiunge alla lista
			if(p)
			{
				// nome del file in base al quale ricavare l'icona da associare nella playlist
				char* pFilename = szFileName;
				
				// se il file e' contenuto in un archivio compresso ricava l'icona per l'elemento della playlist a seconda del tipo
				char szLocation[MAX_ITEM_SIZE+1];
				char* pCompressedFileExt = {".arj"};
				BOOL bComesFromArchiveFile = m_Archive.IsSupportedFormat(::StripFileFromPath(szFileName,szLocation,sizeof(szLocation),TRUE));
				if(bComesFromArchiveFile)
				{
					switch(m_Archive.GetArchiveType(::StripFileFromPath(szFileName,szLocation,sizeof(szLocation),TRUE)))
					{
						case ARCHIVETYPE_GZW:
							pCompressedFileExt = GZW_EXTENSION;
							break;
						case ARCHIVETYPE_ZIP:
							pCompressedFileExt = ZIP_EXTENSION;
							break;
						case ARCHIVETYPE_RAR:
							pCompressedFileExt = RAR_EXTENSION;
							break;
					}
					
					pFilename = pCompressedFileExt;
				}
				else
				{
					// se si tratta di un url che referenzia un immagine
					if(m_Url.IsUrl(::StripFileFromPath(szFileName,szLocation,sizeof(szLocation),TRUE)))
						pFilename = DEFAULT_URL_EXT;
				}
				
				int nItem = m_wndPlaylist.AddItem(	FormatPlaylistItem(szFileName),
											GetIconIndex(pFilename,&m_listPlaylistIcons,&m_wndPlaylist),
											IDM_WALLPAPER_POPUP_MENU,
											m_wndPlaylist.GetItemCount()
											);
				if(nItem!=LB_ERR)
				{
					SetStatusBarPicturePanelCount(i+1);
					
					// controlla se si tratta di un file contenuto in un file compresso
					LPARAM lParam = (LPARAM)-1L;
					//m_wndPlaylist.GetItemText(nItem,CTRLLISTEX_LOCATION_INDEX,szLocation,sizeof(szLocation)-1);
					if(bComesFromArchiveFile)
					{
						lParam = m_Archive.Map(szLocation);
						m_wndPlaylist.SetItemlParam(nItem,lParam);
					}

					// se si tratta di un file audio lo inserisce nella lista per il player
					if(CAudioPlayer::IsSupportedFormat(szFileName))
					{
						if(!pAudioFilesList)
							pAudioFilesList = new CAudioFilesList();
						if(pAudioFilesList)
						{
							AUDIOITEM* audioItem = (AUDIOITEM*)pAudioFilesList->Add();
							if(audioItem)
							{
								strcpyn(audioItem->file,szFileName,_MAX_FILEPATH+1);
								audioItem->index = pAudioFilesList->Count();
								audioItem->lparam = (LPARAM)-1L;

								if(bComesFromArchiveFile)
								{
									p = stristr(audioItem->file,szLocation);
									if(p)
									{
										p += strlen(szLocation);
										while(*p=='\\')
											p++;
										strcpyn(audioItem->file,p,_MAX_FILEPATH+1);
									}

									audioItem->lparam = lParam;
								}
							}
						}
					}
				}
				else
				{
					SetStatusBarInfoPane(IDS_ERROR_ADDITEM,szFileName);
					::MessageBoxResourceEx(this->GetSafeHwnd(),MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_ADDITEM,szFileName);
					break;
				}
			}
			else
			{
				nItemCount = i;
				break;
			}

			// per i cartoni animati
			if(++n > 65)
			{
				n = 0;
				::PeekAndPump();
			}
		}
		
		// passa la lista dei files audio al player
		if(pAudioFilesList)
		{
			if(pAudioFilesList->Count() > 0)
				OnPlayer(NULL,pAudioFilesList,AUDIOPLAYER_COMMAND_QUEUE_FROM_LIST,FALSE);
			delete pAudioFilesList;
		}
	}

	// interfaccia utente
	m_wndPlaylist.AutoSizeColumns();
	//m_wndPlaylist.EnsureVisible(nItem,FALSE);
	m_wndPlaylist.SetRedraw(TRUE);

	// ricava il timeout per il cambio dello sfondo
	if(lpcszDplFile)
		m_nDesktopTimeout = ::GetPrivateProfileInt(WALLPAPER_DPL_KEY,WALLPAPER_TIMEOUT_KEY,WALLPAPER_MIN_VALUE_FOR_CHANGE_TIMEOUT,lpcszDplFile);
	else
		m_nDesktopTimeout = m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_TIMEOUT_KEY);

	// imposta il timer
	if(!Timerize())
		nItemCount = -1;

	// fatta (quasi) tutta
	SetWindowTitleString(WALLPAPER_PROGRAM_TITLE,FALSE,TRUE);
	SetStatusBarInfoPane(IDS_MESSAGE_READY);

	// carica l'elemento
	if(nItemCount > 0 && nItemCountBeforeLoad <= 0)
	{
		int nItem = 0;
		if(m_pCmdLine->HaveDplFile())
			nItem = ::GetPrivateProfileInt(WALLPAPER_DPL_KEY,WALLPAPER_CURRENTPICT_KEY,0,m_pCmdLine->GetDplFile());
		else
			nItem = m_Config.GetNumber(WALLPAPER_PICTURES_KEY,WALLPAPER_CURRENTPICT_KEY);
		if(nItem < 0 || nItem >= m_wndPlaylist.GetItemCount())
			nItem = 0;
		
		SendWallPaperMessage(nItem);
	}

	// se il contenuto della playlist e' misto (immagini/audio) e (al lancio) inizia con un immagine, i files audio verranno
	// riprodotti solo quando raggiungera (via timer) un elemento di tipo audio
	// quindi, se la playlist contiene files audio, per ovviare (ossia per iniziare a riprodurre da subito) controlla lo stato
	// del player (comunque aperto sopra se sono presenti files audio) e se lo trova in attesa (idle) invia il messaggio per
	// cominciare a riprodurre
	CWinThread* pWinThread = NULL;
	BOOL bHavePlayer = IsUIThreadRunning("CWallPaperPlayerUIThread",&pWinThread);
	if(bHavePlayer && pWinThread)
	{
		HWND hWndWinThread = NULL;
		CWnd* pWnd = pWinThread->GetMainWnd();
		if(pWnd)
			hWndWinThread = pWnd->GetSafeHwnd();
		if(hWndWinThread)
		{
			LONG nPlayerStatus = ::SendMessage(hWndWinThread,WM_AUDIOPLAYER_GETSTATUS,0L,0L);
			BOOL bIsIdle = TRUE;
			switch(nPlayerStatus)
			{
				case mmAudioPmReady:
				case mmAudioPmClosed:
				case mmAudioPmDone:
					bIsIdle = TRUE;
					break;
				default:
					//mmAudioPmLoading
					//mmAudioPmOpened
					//mmAudioPmPlaying
					//mmAudioPmPaused
					//mmAudioPmStopped
					//mmAudioPmUndefined
					::Sleep(0L);
					bIsIdle = FALSE;
					break;
			}
			if(bIsIdle)
			{
				int nItem = -1;
				BOOL bHaveAudioFile = FALSE;
				char szItem[MAX_ITEM_SIZE+1];
				
				for(nItem=0; nItem < nItemCount; nItem++)
				{
					GetItem(szItem,sizeof(szItem),nItem);
					if(CAudioPlayer::IsSupportedFormat(szItem))
					{
						bHaveAudioFile = TRUE;
						break;
					}
				}

				if(bHaveAudioFile)
				{
					// controlla se si tratta di un file contenuto in un file compresso
					LPARAM lParam = (LPARAM)-1L;

					char szLocation[MAX_ITEM_SIZE+1];
					m_wndPlaylist.GetItemText(nItem,CTRLLISTEX_LOCATION_INDEX,szLocation,sizeof(szLocation)-1);
					if(m_Archive.IsSupportedFormat(szLocation))
					{
						char* p = stristr(szItem,szLocation);
						if(p)
						{
							p += strlen(szLocation);
							while(*p=='\\')
								p++;
							strcpyn(szItem,p,sizeof(szItem)-1);
						}
						lParam = m_Archive.Map(szLocation);
						m_wndPlaylist.SetItemlParam(nItem,lParam);
					}

					::SendMessage(hWndWinThread,WM_AUDIOPLAYER,(WPARAM)AUDIOPLAYER_COMMAND_PLAY_FROM_FILE,(LPARAM)szItem);
				}
			}
		}
	}

	// invia il messaggio per la creazione del dialogo per l'anteprima
	::PostMessage(this->GetSafeHwnd(),WM_CREATEPREVIEWWINDOW,0L,0L);

	// riabilita l'interfaccia utente
	BOOL bPaused = m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_STATE_KEY)==WALLPAPER_IN_PAUSED_STATE;
	OnEnableGUI(TRUE,MAKELPARAM((bPaused ? MF_GRAYED : MF_ENABLED),(bPaused ? MF_ENABLED : MF_GRAYED)));
	
	// aggiorna il dialogo
	OnUpdateGUI();

	// se deve caricare la playlist senza eliminare quanto gia' presente
	if(nPlaylistLoadingMode==PLAYLIST_LOADING_ADD)
	{
		m_pCmdLine->ResetDplFile();
		SetStatusBarDplPanel(NULL);
	}

	// imposta lo status corrente (attivo/in pausa)
	if(m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_STATE_KEY)==WALLPAPER_IN_PAUSED_STATE)
		OnPause();

	return(nItemCount);
}

/*
	CreatePlaylistCtrl()

	Crea il controllo per la playlist (distingue tra registro/.dpl).
*/
BOOL CWallPaperDlg::CreatePlaylistCtrl(LPCSTR lpcszDplFile)
{
	BOOL bCreated = FALSE;

	// imposta lo stile
	DWORD dwStyle = LVS_EX_FULLROWSELECT;
	if(m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_GRID_GRIDLINES_KEY))
		dwStyle |= LVS_EX_GRIDLINES;
	m_wndPlaylist.SetExStyle(dwStyle);
	m_wndPlaylist.SetIlcColor(ILC_COLOR16);
	m_wndPlaylist.SetMultipleSelection(FALSE);

	// crea il controllo
	if(m_wndPlaylist.Create(this))
	{
		BOOL bIcon = TRUE;

		// messaggio per il menu popup
		m_wndPlaylist.SetMenuMessage(WM_ONPOPUPMENU);

		// messaggio per il click sull'header delle colonne
		m_wndPlaylist.SetOnColumnClickMessage(WM_ONCOLUMNCLICK);

		// menu per click fuori dall'elemento
		m_wndPlaylist.SetDefaultMenu(IDM_WALLPAPER_POPUP_EMPTYMENU);

		// inserisce nella lista le icone relative al tipo di file
		// l'ordine d'inserimento deve riflettersi nell'indice restituito da GetIconIndex()
		CRegistry registry;
		LPIMAGETYPE p;

		// icone di default
		ICONINDEX* iconindex;
		iconindex = new ICONINDEX;
		strcpyn(iconindex->ext,DEFAULT_URL_EXT,_MAX_EXT+1);
		//iconindex->index = m_wndPlaylist.AddIcon(registry.GetSafeIconForRegisteredFileType(DEFAULT_URL_EXT));
		iconindex->index = m_wndPlaylist.AddIcon(IDI_ICON_URL_FILE);
		m_listPlaylistIcons.Add(iconindex);

		iconindex = new ICONINDEX;
		strcpyn(iconindex->ext,GZW_EXTENSION,_MAX_EXT+1);
		iconindex->index = m_wndPlaylist.AddIcon(IDI_ICON_GZW);
		m_listPlaylistIcons.Add(iconindex);

		iconindex = new ICONINDEX;
		strcpyn(iconindex->ext,ZIP_EXTENSION,_MAX_EXT+1);
		iconindex->index = m_wndPlaylist.AddIcon(IDI_ICON_ZIP_FILE);
		m_listPlaylistIcons.Add(iconindex);

		iconindex = new ICONINDEX;
		strcpyn(iconindex->ext,RAR_EXTENSION,_MAX_EXT+1);
		iconindex->index = m_wndPlaylist.AddIcon(IDI_ICON_RAR_FILE);
		m_listPlaylistIcons.Add(iconindex);

		iconindex = new ICONINDEX;
		strcpyn(iconindex->ext,MP3_EXTENSION,_MAX_EXT+1);
		iconindex->index = m_wndPlaylist.AddIcon(IDI_ICON_AUDIO_FILE);
		m_listPlaylistIcons.Add(iconindex);

		iconindex = new ICONINDEX;
		strcpyn(iconindex->ext,WAV_EXTENSION,_MAX_EXT+1);
		iconindex->index = m_wndPlaylist.AddIcon(IDI_ICON_AUDIO_FILE);
		m_listPlaylistIcons.Add(iconindex);

		iconindex = new ICONINDEX;
		strcpyn(iconindex->ext,CDA_EXTENSION,_MAX_EXT+1);
		iconindex->index = m_wndPlaylist.AddIcon(IDI_ICON_AUDIO_FILE);
		m_listPlaylistIcons.Add(iconindex);

		while((p = m_pImage->EnumReadableImageFormats())!=(LPIMAGETYPE)NULL)
		{
			iconindex = new ICONINDEX;
			strcpyn(iconindex->ext,p->ext,_MAX_EXT+1);
			iconindex->index = m_wndPlaylist.AddIcon(IDI_ICON_PICTURE_FILE);
			m_listPlaylistIcons.Add(iconindex);
		}

		if(bIcon)
		{
			BOOL bColumn = TRUE;
			int nColWidth = 0;

			// inserisce nella lista le colonne (le righe vengono inserite durante il caricamento dei files)
			if(lpcszDplFile)
				nColWidth = ::GetPrivateProfileInt(WALLPAPER_DPL_KEY,WALLPAPER_COLUMNWIDTH_FILENAME_KEY,0,lpcszDplFile);
			else
				nColWidth = m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_COLUMNWIDTH_FILENAME_KEY);
			if(m_wndPlaylist.AddCol("Filename",'C',nColWidth)==LB_ERR)
				bColumn = FALSE;

			if(lpcszDplFile)
				nColWidth = ::GetPrivateProfileInt(WALLPAPER_DPL_KEY,WALLPAPER_COLUMNWIDTH_SIZE_KEY,0,lpcszDplFile);
			else
				nColWidth = m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_COLUMNWIDTH_SIZE_KEY);
			if(m_wndPlaylist.AddCol("Info",'C',nColWidth)==LB_ERR)
				bColumn = FALSE;
			
			if(lpcszDplFile)
				nColWidth = ::GetPrivateProfileInt(WALLPAPER_DPL_KEY,WALLPAPER_COLUMNWIDTH_SIZE_KEY,0,lpcszDplFile);
			else
				nColWidth = m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_COLUMNWIDTH_SIZE_KEY);
			if(m_wndPlaylist.AddCol("Size",'S',nColWidth)==LB_ERR)
				bColumn = FALSE;
			
			if(lpcszDplFile)
				nColWidth = ::GetPrivateProfileInt(WALLPAPER_DPL_KEY,WALLPAPER_COLUMNWIDTH_LOCATION_KEY,0,lpcszDplFile);
			else
				nColWidth = m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_COLUMNWIDTH_LOCATION_KEY);
			if(m_wndPlaylist.AddCol("Location",'C',nColWidth)==LB_ERR)
				bColumn = FALSE;
			
			if(lpcszDplFile)
				nColWidth = ::GetPrivateProfileInt(WALLPAPER_DPL_KEY,WALLPAPER_COLUMNWIDTH_TYPE_KEY,0,lpcszDplFile);
			else
				nColWidth = m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_COLUMNWIDTH_TYPE_KEY);
			if(m_wndPlaylist.AddCol("Type",'C',nColWidth)==LB_ERR)
				bColumn = FALSE;

			if(bColumn)
			{
				// imposta gli handler per i click sull'elemento della lista, il controllo
				// si occupera' di inviare il messaggio specificato alla finestra del dialogo
				m_wndPlaylist.SetLButtonDownMessage(WM_ONLBUTTONDOWN);
				m_wndPlaylist.SetLDoubleClickMessage(WM_ONLBUTTONDBLCLK);
				bCreated = TRUE;
			}
		}
	}

	if(bCreated)
	{
		m_wndPlaylist.SetForegroundColor(m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_GRID_FOREGROUNDCOLOR_KEY));
		m_wndPlaylist.SetBackgroundColor(m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_GRID_BACKGROUNDCOLOR_KEY));
		m_wndPlaylist.Invalidate(TRUE);
	}

	return(bCreated);
}

/*
	FormatPlaylistItem()

	Costruisce, a partire dal nome del file, la stringa per l'inserimento nella lista.
*/
LPCSTR CWallPaperDlg::FormatPlaylistItem(LPCSTR lpcszFileName,LPCSTR lpcszForcedPathname/* = NULL */,LPCSTR lpcszProperties/* = NULL */,QWORD qwSize/* = (QWORD)-1L */)
{
	static char szItem[MAX_ITEM_SIZE+1];
	char szFileName[_MAX_FILEPATH+1] = {0};
	char szPath[_MAX_FILEPATH+1] = {""};
	char szName[_MAX_FILEPATH+1] = {""};
	QWORD qwFileSize = 0L;

	// copia in locale il nome del file
	strcpyn(szFileName,lpcszFileName,sizeof(szFileName));

	// dimensione del file
	if(qwSize==(QWORD)-1L)
		qwFileSize = ::GetFileSizeExt(szFileName);
	else
		qwFileSize = qwSize;

	// estrae il nome del file ed il pathname, separandoli (distingue tra nomefile ed url)
	if(!lpcszForcedPathname)
	{
		char* p;
		if(m_Url.IsUrl(lpcszFileName))
		{
			;
		}
		else
		{
			strrev(szFileName);
			p = strchr(szFileName,'\\');
			if(p)
			{
				strcpyn(szPath,p+1,sizeof(szPath));
				*p = '\0';
				strcpyn(szName,szFileName,sizeof(szName));
				strrev(szName);
				strrev(szPath);
			}
			else
				strrev(szFileName);
		}
	}
	else
	{
		strcpyn(szName,szFileName,sizeof(szName));
	}

	if(strnull(szName))
		strcpy(szName," ");

	char* pType = "?";
	if(m_pImage->IsSupportedFormat(lpcszFileName))
		pType = "picture";
	else if(CAudioPlayer::IsSupportedFormat(lpcszFileName))
		pType = "audio";
	else if(m_Archive.IsSupportedFormat(lpcszFileName))
		pType = "compressed";
	else if(m_Url.IsUrl(lpcszFileName))
		pType = "url";

	// costruisce la stringa per l'inserimento nella lista
	if(m_Url.IsUrl(lpcszFileName))
	{
		URL url = {0};
		char szPath[MAX_URL+1]={0},szName[_MAX_FNAME+1]={0},szPort[10]={""};
		
		m_Url.SplitUrl(lpcszFileName,&url);
		if(url.port > 0)
			_snprintf(szPort,sizeof(szPort)-1,":%ld",url.port);
		_snprintf(szPath,sizeof(szPath)-1,"%s%s%s",url.host,szPort,url.dir);
		_snprintf(szName,sizeof(szName)-1,"%s%s%s%s",url.file,url.cgi,url.delim,url.query);

		_snprintf(szItem,
				sizeof(szItem)-1,
				"%s%c%s%c%s%c%s%c%s",
				(strnull(szName) ? " " : szName),
				ITEM_DELIMITER,
				lpcszProperties ? lpcszProperties : " ",
				ITEM_DELIMITER,
				qwFileSize==0L ? " " : strsize((double)qwFileSize),
				ITEM_DELIMITER,
				szPath,
				ITEM_DELIMITER,
				pType
				);
	}
	else
	{
		if((strlen(szName) + 24 + 6 + strlen(szPath) + 10 + 5) < sizeof(szItem))
			_snprintf(szItem,
					sizeof(szItem)-1,
					"%s%c%s%c%s%c%s%c%s",
					szName,
					ITEM_DELIMITER,
					lpcszProperties ? lpcszProperties : " ",
					ITEM_DELIMITER,
					qwFileSize==0L ? " " : strsize((double)qwFileSize),
					ITEM_DELIMITER,
					lpcszForcedPathname ? lpcszForcedPathname : szPath,
					ITEM_DELIMITER,
					pType
					);
		else
			_snprintf(szItem,
					sizeof(szItem)-1,
					" %c %c %c %c ",
					ITEM_DELIMITER,
					ITEM_DELIMITER,
					ITEM_DELIMITER,
					ITEM_DELIMITER
					);
	}
	
	return(szItem);
}

/*
	GetItem()

	Recupera i dati (pathname + nomefile) relativi all'elemento corrente.
*/
int CWallPaperDlg::GetItem(LPSTR lpszItemText,int cbItemText,int nItem/* = -1 */)
{
	memset(lpszItemText,'\0',cbItemText);

	if(nItem < 0)
		nItem = m_wndPlaylist.GetCurrentItem();

	if(nItem >= 0)
	{
		int n = m_wndPlaylist.GetItemText(nItem,CTRLLISTEX_LOCATION_INDEX,lpszItemText,cbItemText-1);
		if((*(lpszItemText + n - 1))=='\\' || (*(lpszItemText + n - 1))=='/')
			;
		else
			*(lpszItemText + n) = m_Url.IsUrl(lpszItemText) ? '/' : '\\';
		n = strlen(lpszItemText);
		m_wndPlaylist.GetItemText(nItem,CTRLLISTEX_FILENAME_INDEX,lpszItemText + n,cbItemText - n - 1);
		strrtrim(lpszItemText);
	}

	return(nItem);
}

/*
	AddFile()

	Aggiunge il file alla playlist.
*/
int CWallPaperDlg::AddFile(LPCSTR lpcszFileName,int nPlayMode/* = 0 */)
{
	int nItem = -1;
	SetStatusBarInfoPane(IDS_MESSAGE_ADDING_ITEM,::StripPathFromFile(lpcszFileName));

	// controlla se si tratta di un file compresso
	if(m_Archive.IsSupportedFormat(lpcszFileName))
	{
		BOOL bAdded = FALSE;

		// interfaccia utente
		m_wndPlaylist.SetRedraw(FALSE);

		// disabilita l'interfaccia utente
		OnEnableGUI(FALSE,MAKELPARAM(MF_GRAYED,MF_GRAYED));

		::PeekAndPump();

		// aggiunge il contenuto del file compresso, non il file compresso
		LPARAM lParam = m_Archive.Map(lpcszFileName);
		if(lParam!=(LPARAM)-1L)
			if(m_Archive.List(lpcszFileName))
			{
				if(!AddArchiveContentToPlaylist(lParam,lpcszFileName))
				{
					::MessageBoxResourceEx(this->GetSafeHwnd(),MB_OK|MB_ICONWARNING,WALLPAPER_PROGRAM_NAME,IDS_MESSAGE_NOITEMS_INTO_ARCHIVE_FILE,lpcszFileName);
					bAdded = FALSE;
				}
				else
					bAdded = TRUE;
			}

		// riabilita l'interfaccia utente
		BOOL bPaused = m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_STATE_KEY)==WALLPAPER_IN_PAUSED_STATE;
		OnEnableGUI(TRUE,MAKELPARAM((bPaused ? MF_GRAYED : MF_ENABLED),(bPaused ? MF_ENABLED : MF_GRAYED)));

		// interfaccia utente
		//m_wndPlaylist.AutoSizeColumns();
		//m_wndPlaylist.EnsureVisible(nItem,FALSE);
		m_wndPlaylist.SetRedraw(TRUE);

		return(bAdded);
	}

	// interfaccia utente
	m_wndPlaylist.SetRedraw(FALSE);

	// aggiunge l'item alla lista (non controlla i duplicati)
	if((nItem = m_wndPlaylist.AddItem(FormatPlaylistItem(lpcszFileName),GetIconIndex(lpcszFileName,&m_listPlaylistIcons,&m_wndPlaylist),IDM_WALLPAPER_POPUP_MENU,m_wndPlaylist.GetItemCount()))!=LB_ERR)
	{
		if(CAudioPlayer::IsSupportedFormat(lpcszFileName))
			OnPlayer(lpcszFileName,NULL,nPlayMode==0 ? AUDIOPLAYER_COMMAND_QUEUE_FROM_FILE : nPlayMode,TRUE);
	}
	else
	{
		::MessageBoxResourceEx(this->GetSafeHwnd(),MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_ADDITEM,lpcszFileName);
		nItem = LB_ERR;
	}

	// interfaccia utente
	if(nItem!=LB_ERR)
	{
		m_wndPlaylist.AutoSizeColumns();
		m_wndPlaylist.EnsureVisible(nItem,FALSE);
	}
	m_wndPlaylist.SetRedraw(TRUE);

	return(nItem);
}

/*
	AddDir()

	Aggiunge il contenuto della directory alla playlist (non considera i file compressi e le url, solo immagini e files audio).
*/
int CWallPaperDlg::AddDir(LPCSTR lpcszPathName,BOOL bRecurse)
{
	int nTotFiles = 0;
	LPSTR lpFileName;
	char szPathName[_MAX_FILEPATH+1];
	CFindFile findFile;
	CAudioFilesList* pAudioFilesList = NULL;

	// disabilita l'interfaccia utente
	CWaitCursor cursor;
	SetStatusBarInfoPane(IDS_MESSAGE_FINDING,lpcszPathName);
	OnEnableGUI(FALSE,MAKELPARAM(MF_GRAYED,MF_GRAYED));

	// interfaccia utente
	m_wndPlaylist.SetRedraw(FALSE);

	// controlla che la directory termini con '\'
	strcpyn(szPathName,lpcszPathName,sizeof(szPathName));
	int i = strlen(szPathName);
	if(szPathName[i-1]!='\\')
		strcatn(szPathName,"\\",sizeof(szPathName));

	// ricerca nella directory specificata
	i = 0;
	while((lpFileName = (LPSTR)findFile.FindEx(szPathName,"*.*",bRecurse))!=NULL)
	{
		if(m_pImage->IsSupportedFormat(lpFileName) || CAudioPlayer::IsSupportedFormat(lpFileName) || m_Archive.IsSupportedFormat(lpFileName))
		{
			// interfaccia utente
			SetStatusBarInfoPane(IDS_MESSAGE_ADDING_ITEM,::StripPathFromFile(lpFileName));
			
			// controlla se si tratta di un file compresso
			if(m_Archive.IsSupportedFormat(lpFileName))
			{
				// interfaccia utente
//				m_wndPlaylist.SetRedraw(FALSE);

				// disabilita l'interfaccia utente
				OnEnableGUI(FALSE,MAKELPARAM(MF_GRAYED,MF_GRAYED));

				::PeekAndPump();

				// aggiunge il contenuto del file compresso, non il file compresso
				LPARAM lParam = m_Archive.Map(lpFileName);
				if(lParam!=(LPARAM)-1L)
					if(m_Archive.List(lpFileName))
					{
						if(!AddArchiveContentToPlaylist(lParam,lpFileName))
						{
							::MessageBoxResourceEx(this->GetSafeHwnd(),MB_OK|MB_ICONWARNING,WALLPAPER_PROGRAM_NAME,IDS_MESSAGE_NOITEMS_INTO_ARCHIVE_FILE,lpFileName);
							continue;
						}
						else
							continue;
					}
					
				// riabilita l'interfaccia utente
				BOOL bPaused = m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_STATE_KEY)==WALLPAPER_IN_PAUSED_STATE;
				OnEnableGUI(TRUE,MAKELPARAM((bPaused ? MF_GRAYED : MF_ENABLED),(bPaused ? MF_ENABLED : MF_GRAYED)));

				// interfaccia utente
				//m_wndPlaylist.AutoSizeColumns();
				//m_wndPlaylist.EnsureVisible(nItem,FALSE);
				//m_wndPlaylist.SetRedraw(TRUE);
			}

			if(m_wndPlaylist.AddItem(FormatPlaylistItem(lpFileName),GetIconIndex(lpFileName,&m_listPlaylistIcons,&m_wndPlaylist),IDM_WALLPAPER_POPUP_MENU,m_wndPlaylist.GetItemCount())!=LB_ERR)
			{
				nTotFiles++;

				// se si tratta di un file audio lo inserisce nella lista per il player
				if(CAudioPlayer::IsSupportedFormat(lpFileName))
				{
					if(!pAudioFilesList)
						pAudioFilesList = new CAudioFilesList();
					if(pAudioFilesList)
					{
						AUDIOITEM* audioItem = (AUDIOITEM*)pAudioFilesList->Add();
						if(audioItem)
						{
							strcpyn(audioItem->file,lpFileName,_MAX_FILEPATH+1);
							audioItem->index = pAudioFilesList->Count();
							audioItem->lparam = (LPARAM)-1L;
						}
					}
				}
			}
		}

		if(++i > 15)
		{
			i = 0;
			::PeekAndPump();
		}
	}
		
	// passa la lista dei files audio al player
	if(pAudioFilesList)
	{
		if(pAudioFilesList->Count() > 0)
			OnPlayer(NULL,pAudioFilesList,AUDIOPLAYER_COMMAND_QUEUE_FROM_LIST,TRUE);
		delete pAudioFilesList;
	}

	// riabilita l'interfaccia utente
	BOOL bPaused = m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_STATE_KEY)==WALLPAPER_IN_PAUSED_STATE;
	OnEnableGUI(TRUE,MAKELPARAM((bPaused ? MF_GRAYED : MF_ENABLED),(bPaused ? MF_ENABLED : MF_GRAYED)));

	// interfaccia utente
	if(nTotFiles > 0)
	{
		m_wndPlaylist.AutoSizeColumns();
//		m_wndPlaylist.EnsureVisible(nItem,FALSE);
//		m_wndPlaylist.SetRedraw(TRUE);
	}

	m_wndPlaylist.SetRedraw(TRUE);

	return(nTotFiles);
}

/*
	AddUrl()

	Aggiunge l'url alla playlist.
*/
BOOL CWallPaperDlg::AddUrl(LPCSTR lpcszUrl)
{
	BOOL bAdded = TRUE;
	CUrl url;
	char szUrl[MAX_URL+1];

	// controlla che sia un url
	if(!m_Url.IsUrl(lpcszUrl))
	{
		::MessageBoxResourceEx(this->GetSafeHwnd(),MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_INVALID_HTTP_URL,lpcszUrl);
		return(bAdded);
	}

	strcpyn(szUrl,lpcszUrl,sizeof(szUrl));
	url.DecodeUrl(szUrl);

	// interfaccia utente
	m_wndPlaylist.SetRedraw(FALSE);

	// aggiunge l'item alla lista (non controlla i duplicati)
	int nItem = m_wndPlaylist.AddItem(FormatPlaylistItem(szUrl),GetIconIndex(DEFAULT_URL_EXT,&m_listPlaylistIcons,&m_wndPlaylist),IDM_WALLPAPER_POPUP_MENU,m_wndPlaylist.GetItemCount());
	if(nItem==LB_ERR)
		bAdded = FALSE;

	// interfaccia utente
	m_wndPlaylist.AutoSizeColumns();
	if(bAdded)
	{
		m_wndPlaylist.AutoSizeColumns();
		m_wndPlaylist.EnsureVisible(nItem,FALSE);
	}

	// interfaccia utente
	m_wndPlaylist.SetRedraw(TRUE);

	if(!bAdded)
		::MessageBoxResourceEx(this->GetSafeHwnd(),MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_ADDITEM,szUrl);

	return(bAdded);
}

/*
	GetUrl()

	Scarica l'url in locale.
*/
BOOL CWallPaperDlg::GetUrl(LPSTR lpszUrl,UINT /*cbUrl*/,LPSTR lpszLocalFile,UINT nLocalFileSize)
{
	BOOL bDownloaded = FALSE;

	// divide l'url in host/pathname e nomefile
	URL Url;
	char szFile[MAX_URL+1];
	char szUrl[MAX_URL+1];
	m_Url.SplitUrl(lpszUrl,&Url);
	if(Url.port <= 0)
		Url.port = HTTP_DEFAULT_PORT;
	strcpyn(szFile,Url.file,sizeof(szFile));
	strcpy(Url.file,"");
	m_Url.ComposeUrl(&Url,szUrl,sizeof(szUrl));

	CWallPaperDownloadDlg dlg(this,&m_Config,FALSE,szFile,szUrl);
	dlg.SetVisible(FALSE);
	dlg.ShowErrors(TRUE);
	if(dlg.DoModal()==IDOK)
	{
		strcpyn(lpszLocalFile,dlg.GetDownloadedFilename(),nLocalFileSize);
		bDownloaded = TRUE;
	}

	return(bDownloaded);
}

/*
	OnCreatePreviewWindow()

	Gestore per la creazione della finestra per l'anteprima (crea il thread UI che lancera' il dialogo).
*/
LRESULT CWallPaperDlg::OnCreatePreviewWindow(WPARAM /*wParam*/,LPARAM /*lParam*/)
{
	// crea il thread UI solo se gia' non esiste (una sola istanza)
	BOOL bCreated = FALSE;
	CWinThread* pWinThread = NULL;
	BOOL bHavePlayer = IsUIThreadRunning("CWallPaperPreviewUIThread",&pWinThread);
	
	if(bHavePlayer && pWinThread)
	{
		bCreated = TRUE;
	}
	else
	{
		pWinThread = AfxBeginThread(RUNTIME_CLASS(CWallPaperPreviewUIThread),m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PRIORITY_THREAD_KEY),0,CREATE_SUSPENDED);
		
		if(pWinThread)
		{
			THREAD* thread = (THREAD*)m_listRunningUIThreads.Add();
			if(thread)
			{
				thread->pWinThread = pWinThread;
				strcpyn(thread->szThreadName,"CWallPaperPreviewUIThread",MAX_THREAD_NAME+1);
				pWinThread->m_bAutoDelete = FALSE;
				pWinThread->ResumeThread();

				char szItem[MAX_ITEM_SIZE+1];
				GetItem(szItem,sizeof(szItem));

				if(!strnull(szItem))
				{
					// deve aspettare il termine della creazione della finestra per poter inviare i messaggi
					while(::FindWindow(NULL,IDS_DIALOG_PREVIEW_TITLE)==NULL && IsUIThreadRunning("CWallPaperPreviewUIThread",&pWinThread))
						::Sleep(100L);
					::Sleep(100L);

					CWnd* pWnd = pWinThread->GetMainWnd();
					if(pWnd)
					{
						HWND hWnd = pWnd->GetSafeHwnd();
						if(hWnd)
						{
							BOOL bIsPicture = m_pImage->IsSupportedFormat(szItem);
							
							::SendMessage(hWnd,WM_SETCONFIGURATION,(WPARAM)this->GetSafeHwnd(),(LPARAM)&m_Config);
							
							if(bIsPicture)
								::SendMessage(hWnd,m_bPreviewMode ? WM_PREVIEWENABLED : WM_PREVIEWDISABLED,(WPARAM)szItem,0L);
							else
								::SendMessage(hWnd,WM_PREVIEWDISABLED,0L,0L);
							
							bCreated = TRUE;
						}
					}
				}
			}
		}
	}

	return(bCreated);
}

/*
	OnClosePreviewWindow()

	Chiamata quando il dialogo per l'anteprima viene chiuso, aggiorna il check box relativo sul dialogo principale.
*/
LRESULT CWallPaperDlg::OnClosePreviewWindow(WPARAM /*wParam*/,LPARAM /*lParam*/)
{
	m_bPreviewMode = FALSE;
	m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PICTUREPREVIEW_KEY,m_bPreviewMode);
	m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_PICTUREPREVIEW_KEY);
	UpdateData(FALSE);
	((CWnd*)GetDlgItem(IDC_LIST_PLAYLIST))->SetFocus();
	return(0L);
}

/*
	OnCheckPreviewMode()

	Gestore per il check box del dialogo principale per l'anteprima.
*/
void CWallPaperDlg::OnCheckPreviewMode(void)
{
	m_bPreviewMode = !m_bPreviewMode;
	m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PICTUREPREVIEW_KEY,m_bPreviewMode);
	m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_PICTUREPREVIEW_KEY);
	OnPreviewMode(TRUE);
	((CWnd*)GetDlgItem(IDC_LIST_PLAYLIST))->SetFocus();
}

/*
	OnPreviewMode()

	Visualizza l'anteprima tramite il dialogo creato dal thread UI.
*/
void CWallPaperDlg::OnPreviewMode(BOOL bForceFocus)
{
	int nItem;
	char szItem[MAX_ITEM_SIZE+1] = {0};
	BOOL bLoaded = FALSE;
	BOOL bIsPicture = FALSE;
	HWND hWnd = NULL;

	// se si trova in modalita' anteprima
	if(m_bPreviewMode)
	{
		// ricava l'elemento corrente della playlist
		if((nItem = m_wndPlaylist.GetCurrentItem()) >= 0)
		{
			SetStatusBarPicturePanelMessage(IDS_MESSAGE_LOADING_PREVIEW);

			// ricava il nome file
			GetItem(szItem,sizeof(szItem),nItem);
			
			// url
			if(m_Url.IsUrl(szItem))
			{
				// l'url referenzia un immagine
				if(m_pImage->IsSupportedFormat(szItem))
				{
					URL Url;
					char szFilename[_MAX_FILEPATH+1];
					m_Url.SplitUrl(szItem,&Url);
					_snprintf(szFilename,sizeof(szFilename)-1,"%s%s",m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_DOWNLOADDIR_KEY),Url.file);
					strcpyn(szItem,szFilename,sizeof(szItem));
				}
			}

			BOOL bArchiveError = FALSE;

			if(!bArchiveError)
			{
				// controlla se si tratta di un file contenuto in un file compresso
				LPARAM lParam = m_wndPlaylist.GetItemlParam(nItem);
				
				char szLocation[MAX_ITEM_SIZE+1];
				m_wndPlaylist.GetItemText(nItem,CTRLLISTEX_LOCATION_INDEX,szLocation,sizeof(szLocation)-1);
				if(m_Archive.IsSupportedFormat(szLocation))
				{
					if(lParam==(LPARAM)-1L)
					{
						lParam = m_Archive.Map(szLocation);
						m_wndPlaylist.SetItemlParam(nItem,lParam);
					}
				}
				
				// in lParam l'indice dell'elemento nella lista dei files compressi
				if(lParam!=(LPARAM)-1L)
				{
					// ricava il nome del file compresso in cui si trova l'elemento
					LPCSTR pArchiveName = m_Archive.GetParent(lParam);
					if(pArchiveName)
					{
						// costruisce il nome dell'elemento (sul pathname temporaneo)
						char* p;
						char szItemFromCompressedFile[_MAX_FILEPATH+1];
						m_wndPlaylist.GetItemText(nItem,CTRLLISTEX_FILENAME_INDEX,szItemFromCompressedFile,sizeof(szItemFromCompressedFile)-1);
						while((p = strchr(szItemFromCompressedFile,'\\'))!=NULL)
							*p = '/';
						if((p = strrchr(szItemFromCompressedFile,'/'))!=NULL)
							p++;
						if(!p)
							p = szItemFromCompressedFile;
						_snprintf(szItem,sizeof(szItem)-1,"%s%s",m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_TEMPDIR_KEY),p);
						
						// estrae il file
						if(m_pImagePreview->IsSupportedFormat(szItem))
						{
							SetStatusBarInfoPane(IDS_MESSAGE_UNCOMPRESSING);
							LRESULT lRet = m_Archive.Extract(szItemFromCompressedFile,pArchiveName,m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_TEMPDIR_KEY));
							SetStatusBarInfoPane(IDS_MESSAGE_READY);
							bLoaded = TRUE;
							if(lRet!=0L)
							{
								bArchiveError = TRUE;
								bLoaded = FALSE;
								::MessageBoxResourceEx(this->GetSafeHwnd(),MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_EXTRACT_FILE,szItemFromCompressedFile,pArchiveName,/*handler->lpfnGetErrorCodeDescription(lRet)*/m_Archive.GetErrorCodeDescription(lRet,pArchiveName),lRet);
								SetWindowTitleID(IDS_ERROR_EXTRACTFILE,szItemFromCompressedFile);
								SetStatusBarInfoPane(IDS_MESSAGE_READY);
							}
						}
					}
				}
			}

			if(!bArchiveError)
			{
				// immagine (l'oggetto per l'anteprima, non quello utilizzato per lo sfondo)
				if(m_pImagePreview->IsSupportedFormat(szItem))
				{
					// carica l'immagine comunque perche' deve poi passare il puntatore all'oggetto al dialogo per
					// l'anteprima (se le proprieta fossero gia' impostate non servirebbe)
					bIsPicture = bLoaded = m_pImagePreview->Load(szItem);
				}
			}

			SetStatusBarPicturePanelMessage();
		}
	}

	// invia il messaggio per l'anteprima al dialogo
	CWinThread* pWinThread = NULL;
	BOOL bHavePlayer = IsUIThreadRunning("CWallPaperPreviewUIThread",&pWinThread);
	if(!bHavePlayer)
		OnCreatePreviewWindow(0,0);
	if(bHavePlayer && pWinThread)
	{
		CWnd* pWnd = pWinThread->GetMainWnd();
		if(pWnd)
		{
			if((hWnd = pWnd->GetSafeHwnd())!=(HWND)NULL)
			{
				::SendMessage(hWnd,WM_SETCONFIGURATION,(WPARAM)this->GetSafeHwnd(),(LPARAM)&m_Config);
				
				if(bIsPicture && bLoaded)
				{
					if(m_bPreviewMode)
						::SendMessage(hWnd,WM_PREVIEWFORCEFOCUS,(WPARAM)bForceFocus,(LPARAM)0L);
					::SendMessage(hWnd,m_bPreviewMode ? WM_PREVIEWENABLED : WM_PREVIEWDISABLED,(WPARAM)szItem,(LPARAM)m_pImagePreview);
				}
				else
				{
					::SendMessage(hWnd,WM_PREVIEWDISABLED,0L,0L);
				}
			}
		}
	}

	if(m_bPreviewMode && bForceFocus)
	{
		// se la finestra di preview si sovrappone alla principale la rimette sotto
		SetWindowPos(&wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
		SetWindowPos(&wndNoTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	}
	else if(m_bPreviewMode && !bForceFocus)
	{
		// se si trova minimizzato e la finestra di preview non e' visibile perche' l'elemento precedente non
		// era un immagine visualizza la finestra di preview iconizzata (solo se sopra ha caricato un immagine)
		if(IsIconic())
		{
			if(hWnd && bLoaded)
				::SendMessage(hWnd,WM_PREVIEWMINMAXIMIZE,(WPARAM)SC_MINIMIZE,(LPARAM)0L);
		}
	}
}

/*
	OnBalloon()
*/
void CWallPaperDlg::OnBalloon(void)
{
	// se deve visualizzare il (balloon) tooltip
	if(m_pImage && m_pImage->IsLoaded() && m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PICTPOPUP_KEY)==PICTPOPUP_BALLOON)
	{						
		// ricava le informazioni relative all'immagine
		// legge dall'header interno perche' e' dove conserva le info originali, se l'immagine viene manipolata (stretch,
		// fit, etc.) i metodi restituiscono i valori dell'immagine manipolata
		// NB: m_PictureInfo contiene i valori originali per cui usare m_PictureInfo o pImageHeader e' lo stesso
		LPIMAGEHEADERINFO pImageHeader = (LPIMAGEHEADERINFO)m_pImage->GetHeaderInfo();
		int nWidth  = pImageHeader->width;
		int nHeight = pImageHeader->height;
		int nColors = pImageHeader->colors;
		int nBPP    = pImageHeader->bpp;
		char szMemUsed[32];
		strsize(szMemUsed,sizeof(szMemUsed),pImageHeader->memused);
		char szFileSize[32];
		strsize(szFileSize,sizeof(szFileSize),pImageHeader->filesize);
		float nXRes = pImageHeader->xres,nYRes = pImageHeader->yres;
		int nResType = pImageHeader->restype;
		int nQuality = pImageHeader->quality;
		char szQuality[32] = {""};
		if(nQuality > 0)
			_snprintf(szQuality,sizeof(szQuality)-1,"\nquality factor of %d",nQuality);
		
		char szManipulated[128] = {0};
		char szMem[16] = {0};
		if(	m_PictureInfo.szEffect[0]!='\0' ||
			m_PictureInfo.nWidth!=(int)m_pImage->GetWidth()		||
			m_PictureInfo.nHeight!=(int)m_pImage->GetHeight()		||
			m_PictureInfo.nColors!=(int)m_pImage->GetNumColors()	||
			m_PictureInfo.nBPP!=(int)m_pImage->GetBPP()			||
			m_PictureInfo.dwMemUsed!=m_pImage->GetMemUsed()		||
			m_PictureInfo.dwFileSize!=m_pImage->GetFileSize()		||
			m_PictureInfo.nXRes!=m_pImage->GetXRes()			||
			m_PictureInfo.nYRes!=m_pImage->GetYRes()			||
			m_PictureInfo.nResType!=m_pImage->GetURes()			||
			m_PictureInfo.nQuality!=m_pImage->GetQuality()
			)
			_snprintf(szManipulated,
					sizeof(szManipulated)-1,
					"\n\nafter %s%s%s:\n%ld x %ld pixels\n%d bpp\n%s required",
					m_PictureInfo.szDrawmode,
					m_PictureInfo.szDrawmode[0]!='\0' ? (m_PictureInfo.szEffect[0]!='\0' ? ", " : "") : "",
					m_PictureInfo.szEffect,
					m_pImage->GetWidth(),
					m_pImage->GetHeight(),
					m_pImage->GetBPP(),
					strsize(szMem,sizeof(szMem),m_pImage->GetMemUsed())
					);

		BOOL bLoadCustomIcon = FALSE;
		if(m_nPictIconsCount > 0 && m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_BALLOON_PICT_ICON_TYPE_KEY)==BALLOON_ICON_CUSTOM)
		{
			if(++m_nCurrentPictIcon >= m_nPictIconsCount)
				m_nCurrentPictIcon = 0;
			
			bLoadCustomIcon = TRUE;
		}

		char szBalloonText[BALLOON_MAX_TOOLTIP_TEXT+1] = {0};
		_snprintf(szBalloonText,
				sizeof(szBalloonText)-1,
				"%d x %d pixels\n%d%s colors\n%d bpp\n%s required (%s file size)\nxRes of %3.2f, yRes of %3.2f (%s)%s%s\n",
				nWidth,
				nHeight,
				(nColors > 256 || nColors==0) ? 16 : nColors,
				(nColors > 256 || nColors==0) ? "M" : "",
				nBPP,
				szMemUsed,
				szFileSize,
				nXRes,nYRes,
				nResType==RESUNITINCH ? "inch" : (nResType==RESUNITCENTIMETER ? "centimeter" : "none"),
				szQuality,
				szManipulated[0]!='\0' ? szManipulated : ""
				);

		if(bLoadCustomIcon)
		{
			char szBuffer[256];
			_snprintf(szBuffer,sizeof(szBuffer),"\n(%s)",::StripPathFromFile(m_findPictIcons.GetFileName(m_nCurrentPictIcon)));
			strcatn(szBalloonText,szBuffer,sizeof(szBalloonText));
		}

		// plin!
		if(bLoadCustomIcon)
		{
			int nBiggerIcon = 32;
			FINDFILE* f = m_findPictIcons.GetFindFile(m_nCurrentPictIcon);
			if(f->wParam <= 0)
				f->wParam = ::GetBiggerIconSize(m_findPictIcons.GetFileName(m_nCurrentPictIcon),48);
			nBiggerIcon = f->wParam;

			if(m_pTrayIcon)
			{
				m_pTrayIcon->SetBalloonType(BALLOON_USE_EXTENDED);
				m_pTrayIcon->Balloon(	m_pImage->GetFileName(),
									szBalloonText,
									MB_ICONINFORMATION,
									BALLOON_DEFAULT_TIMEOUT,
									(UINT)-1L,
									m_findPictIcons.GetFileName(m_nCurrentPictIcon),
									CSize(nBiggerIcon,nBiggerIcon),
									IDI_ICON16X16_CLOSE
									);
			}
			else
			{
				if(!m_pBalloon)
					m_pBalloon = new CBalloon();
				if(m_pBalloon)
				{
					char szString[512];
					::FormatResourceString(szString,sizeof(szString),IDS_TOOLTIP_CSS_STYLE);
					m_pBalloon->SetCssStyles(szString);
					m_pBalloon->Balloon(m_pImage->GetFileName(),
									szBalloonText,
									MB_ICONINFORMATION,
									BALLOON_DEFAULT_TIMEOUT,
									(UINT)-1L,
									m_findPictIcons.GetFileName(m_nCurrentPictIcon),
									CSize(nBiggerIcon,nBiggerIcon),
									IDI_ICON16X16_CLOSE
									);
				}
			}
		}
		else
		{
			if(m_pTrayIcon)
			{
				m_pTrayIcon->SetBalloonType(BALLOON_USE_EXTENDED);
				m_pTrayIcon->Balloon(	m_pImage->GetFileName(),
									szBalloonText,
									MB_ICONINFORMATION,
									BALLOON_DEFAULT_TIMEOUT,
									IDI_ICON_WALLPAPER,
									NULL,
									CSize(32,32),
									IDI_ICON16X16_CLOSE
									);
			}
			else
			{
				if(!m_pBalloon)
					m_pBalloon = new CBalloon();
				if(m_pBalloon)
				{
					char szString[512];
					::FormatResourceString(szString,sizeof(szString),IDS_TOOLTIP_CSS_STYLE);
					m_pBalloon->SetCssStyles(szString);
					m_pBalloon->SetIconInformation((HICON)::LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICON_WALLPAPER),IMAGE_ICON,32,32,LR_DEFAULTCOLOR));
					m_pBalloon->SetBalloonCloseIcon(IDI_ICON16X16_CLOSE);
					m_pBalloon->Balloon(m_pImage->GetFileName(),
									szBalloonText
									);
				}
			}
		}
	}
}

/*
	OnTaskbarPopup()
*/
void CWallPaperDlg::OnTaskbarPopup(void)
{
	// controlla l'avvenuto caricamento
	if(m_pImage && m_pImage->IsLoaded() && m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PICTPOPUP_KEY)==PICTPOPUP_TASKBAR)
	{
		// crea l'oggetto se ancora non esiste
		if(!m_pTaskbarNotifier)
		{
			// crea la finestra interna a cui agganciare l'oggetto
			// se utilizasse la finestra principale, al minimizzarla verrebbe minimizzato anche il popup
			m_pWndTaskbarParent = new CWnd();
			if(m_pWndTaskbarParent)
			{
				if(m_pWndTaskbarParent->CreateEx(0,AfxRegisterWndClass(CS_BYTEALIGNCLIENT|CS_BYTEALIGNWINDOW,0,0,0),"TaskbarNotifierClassParent",0,1,1,1,1,NULL,NULL,NULL))
				{
					m_pTaskbarNotifier = new CTaskbarNotifier();
					if(m_pTaskbarNotifier)
					{
						if(m_pTaskbarNotifier->Create(m_pWndTaskbarParent))
						{
							m_pTaskbarNotifier->SetMessageWindow(this->GetSafeHwnd()); // i messaggi vanno inviati alla finestra principale, non a quella fittizzia
							m_pTaskbarNotifier->SetOnMouseClick(TRUE);
							m_pTaskbarNotifier->SetOnMouseClickMessage(WM_NOTIFYTASKBARPOPUP);
							m_pTaskbarNotifier->SetIDTimerAppearing(ID_TIMER_POPUP_APPEARING_PICT);
							m_pTaskbarNotifier->SetIDTimerDisappearing(ID_TIMER_POPUP_DISAPPEARING_PICT);
							m_pTaskbarNotifier->SetIDTimerWaiting(ID_TIMER_POPUP_WAITING_PICT);
							m_pTaskbarNotifier->SetIDTimerScrolling(ID_TIMER_POPUP_SCROLLING_PICT);
						}
						else
							delete m_pTaskbarNotifier,m_pTaskbarNotifier = NULL;
					}
				}
			}
		}

		// se dispone di un oggetto valido
		if(m_pTaskbarNotifier)
		{
			TAGS m_Tags[8];
			strcpy(m_Tags[0].tag,"%t");	// titolo / nome file
//$			strcpy(m_Tags[].tag,"%a");	// artista
//			strcpy(m_Tags[].tag,"%b");	// album
//			strcpy(m_Tags[].tag,"%g");	// genere
//			strcpy(m_Tags[].tag,"%y");	// anno
			strcpy(m_Tags[1].tag,"%r");	// traccia / memoria utilizzata
			strcpy(m_Tags[2].tag,"%d");	// durata / dimensione
//			strcpy(m_Tags[].tag,"%c");	// commento
			strcpy(m_Tags[3].tag,"%B");	// bitrate / dimensione (pixel)
			strcpy(m_Tags[4].tag,"%F");	// frequenza / colori
			strcpy(m_Tags[5].tag,"%C");	// modalita' / qualita
			strcpy(m_Tags[6].tag,"%T");	// formato / BPP
			strcpy(m_Tags[7].tag,"\\n");	// a capo
			for(int i = 0; i < ARRAY_SIZE(m_Tags); i++)
				m_Tags[i].value = NULL;

			// ricava l'indice relativo al popup corrente della lista
			int nPopupCount = m_listPictTaskbarBitmaps.Count();
			if(nPopupCount > 0)
			{
				TASKBARPOPUP* pAudioPopupInfoItem = (TASKBARPOPUP*)m_listPictTaskbarBitmaps.GetAt(m_nPopupIndex++);
				if(pAudioPopupInfoItem)
					memcpy(&m_TaskbarPopup,pAudioPopupInfoItem,sizeof(TASKBARPOPUP));
				if(m_nPopupIndex >= nPopupCount)
					m_nPopupIndex = 0;
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
					if(n > 0 && !strnull(m_TaskbarPopup.szAuthor))
						i = _snprintf(szSkinInfo+n,sizeof(szSkinInfo)-1-n," by <%s>",m_TaskbarPopup.szAuthor),n = (i > 0 ? n+i : -1);
					if(n > 0 && !strnull(m_TaskbarPopup.szCopyright))
						i = _snprintf(szSkinInfo+n,sizeof(szSkinInfo)-1-n," (%s)",m_TaskbarPopup.szCopyright),n = (i > 0 ? n+i : -1);

					n = _snprintf(	m_szTaskbarToolTipText,
								sizeof(m_szTaskbarToolTipText)-1,
								"%s\n%d x %d pixels\n%s colors, %d bpp\n%s required (%s file size)\nxRes of %3.2f, yRes of %3.2f (%s)%s",
								m_pImage->GetFileName(),
								m_PictureInfo.nWidth,
								m_PictureInfo.nHeight,
								m_PictureInfo.szColors,
								m_PictureInfo.nBPP,
								m_PictureInfo.szMemUsed,
								m_PictureInfo.szFileSize,
								m_PictureInfo.nXRes,m_PictureInfo.nYRes,
								m_PictureInfo.nResType==RESUNITINCH ? "inch" : (m_PictureInfo.nResType==RESUNITCENTIMETER ? "centimeter" : "none"),
								m_PictureInfo.szQuality
								);

					if(n > 0)
						_snprintf(m_szTaskbarToolTipText+n,sizeof(m_szTaskbarToolTipText)-1-n,"\n%s",szSkinInfo);

					// sostituisce le macro (utente) all'interno del testo per il popup
					// occhio!!! alla dichiarazione, aggiornare il numero di elementi per non rifare il botto dell'ultima volta...
					m_Tags[0].value = (char*)m_pImage->GetFileName();
					m_Tags[1].value = m_PictureInfo.szMemUsed;
					m_Tags[2].value = m_PictureInfo.szFileSize;
					m_Tags[3].value = m_PictureInfo.szWidthxHeight;
					m_Tags[4].value = m_PictureInfo.szColors;
					m_Tags[5].value = m_PictureInfo.szQuality;
					m_Tags[6].value = m_PictureInfo.szBPP;
					m_Tags[7].value = m_TaskbarPopup.nPopupScrollText ? " " : "\r\n";
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
					else if(m_TaskbarPopup.dwPopupTimeToStay==0L || m_TaskbarPopup.dwPopupTimeToStay < TASKBARPOPUP_MIN_POPUPTIME || m_TaskbarPopup.dwPopupTimeToStay > TASKBARPOPUP_MAX_POPUPTIME)
						m_TaskbarPopup.dwPopupTimeToStay = (DWORD)(((m_nDesktopTimeout / 60) / 1000) - (m_TaskbarPopup.nPopupTimeToShow + m_TaskbarPopup.nPopupTimeToHide + 100L));
					
					// visualizza la finestra popup
					bLoaded = m_pTaskbarNotifier->Show(m_TaskbarPopup.szPopupText,m_TaskbarPopup.nPopupTimeToShow,m_TaskbarPopup.dwPopupTimeToStay,m_TaskbarPopup.nPopupTimeToHide,m_TaskbarPopup.nPopupXPos,m_TaskbarPopup.nPopupYPos);
				}
				else
					::MessageBoxResourceEx(this->m_hWnd,MB_OK|MB_ICONERROR,IDS_DIALOG_PLAYER_TITLE,IDS_ERROR_INVALID_SKIN_FILENAME,m_TaskbarPopup .szPicture);
			}
		}
	}
}

/*
	OnTooltipCallback()
*/
LRESULT CWallPaperDlg::OnTooltipCallback(WPARAM wParam,LPARAM /*lParam*/)
{
	LPSTR pMsg = (LPSTR)wParam;
	if(pMsg)
	{
		if(stricmp(pMsg,"WallPaper")==0)
		{
			m_Config.UpdateNumber(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_TOOLTIP_MAIN_KEY,0);
			RemoveToolTip(IDC_LIST_PLAYLIST);
			AddToolTip(IDC_LIST_PLAYLIST,IDS_TOOLTIP_PLAYLIST);
		}
		m_Config.SaveSection(WALLPAPER_DONOTASKMORE_KEY);
	}
	
	return(0L);
}

/*
	OnBeginDrag()

	Gestore per il drag and drop dal dialogo principale verso l'esterno.
*/
void CWallPaperDlg::OnBeginDrag(NMHDR* /*pNMHDR*/,LRESULT* pResult) 
{
    *pResult = 0; // non utilizzato
	
	// dato che la gestione viene effettuata qui e non nella classe per la lista, al click sull'elemento per iniziare il drag
	// and drop viene chiamato prima questo gestore rispetto a quello della lista per ricavare l'elemento selezionato, per cui
	// deve ricavare l'elemento corrente in proprio, senza appoggiarsi alla classe per la lista
	int nItem = m_wndPlaylist.GetCurrentItem();
	POSITION pos = m_wndPlaylist.GetFirstSelectedItemPosition();
	nItem = m_wndPlaylist.GetNextSelectedItem(pos);

	if(nItem >= 0)
	{
		// ricava l'elemento selezionato (nome file completo di pathname)
		char szItem[MAX_ITEM_SIZE+1];
		GetItem(szItem,sizeof(szItem),nItem);

		// costruisce la struttura per il drop dell'elemento (file) sulla shell ('affanculo Unicode, sempre)
		DROPFILES dropfiles = {sizeof(DROPFILES),{0,0},0,0};
		int nLen = strlen(szItem);
		int nGlbLen = sizeof(DROPFILES) + nLen + 3;
		HGLOBAL hGlobal = ::GlobalAlloc(GMEM_ZEROINIT|GMEM_MOVEABLE|GMEM_DDESHARE,nGlbLen);
		char* pData = (char*)::GlobalLock(hGlobal);
		memcpy(pData,&dropfiles,sizeof(DROPFILES));
		pData += sizeof(DROPFILES);
		memcpy(pData,szItem,nLen);
		::GlobalUnlock(hGlobal);

		COleDataSource OleDataSource;
		COleDropSourceEx OleDropSource;
		OleDataSource.CacheGlobalData(CF_HDROP,hGlobal);
		DWORD dwEffects = ::GetKeyState(VK_SHIFT) < 0 ? DROPEFFECT_MOVE : DROPEFFECT_COPY;
		dwEffects = OleDataSource.DoDragDrop(dwEffects,NULL,&OleDropSource);
		dwEffects = OleDropSource.GetDropEffect();

		if(dwEffects==DROPEFFECT_NONE)
		{
			PostWallPaperMessage(nItem);
		}
		else if(dwEffects & DROPEFFECT_MOVE)
		{
			OnRemove();
		}
		else if(dwEffects & DROPEFFECT_COPY)
		{
			;
		}

		// modalita' anteprima
		if(!(dwEffects & DROPEFFECT_MOVE))
			OnPreviewMode(TRUE);
	}
}

/*
	OnDropFiles()

	Carica i files/directories ricevuti via drag and drop dalla shell.
	Notare che accetta il drop su se stesso.
*/
void CWallPaperDlg::OnDropFiles(void)
{
	// mette in primo piano quando riceve via drag & drop
	SetForegroundWindowEx(this->GetSafeHwnd());

	// controlla che non sia in pausa
	if(m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_STATE_KEY)==WALLPAPER_IN_PAUSED_STATE)
	{
		::MessageBoxResource(this->GetSafeHwnd(),MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_PAUSED);
		return;
	}

	// totale file droppati e totale elementi nella lista prima dell'operazione
	int nTotDrops = 0;
	int nItemCount = m_wndPlaylist.GetItemCount();
	CAudioFilesList* pAudioFilesList = NULL;

	DWORD dwAttribute;
	char szFileName[_MAX_FILEPATH+1];

	// per ogni file ricevuto
	while(CDialogEx::GetDroppedFile(szFileName,sizeof(szFileName),&dwAttribute))
	{
		// distingue tra directory e file
		if(!(dwAttribute & FILE_ATTRIBUTE_DIRECTORY))
		{
			// file (.dpl/.url)
			if(!m_pImage->IsSupportedFormat(szFileName))
			{
				// controlla se si tratta di una lista (.dpl)
				if(striright(szFileName,DPL_EXTENSION)==0)
				{
					m_pCmdLine->SetDplFile(szFileName);
					::PostMessage(this->GetSafeHwnd(),WM_LOADPLAYLIST,0L,0L);
					PlaylistSetModified();
				}
				// controlla se si tratta di un url (.url)
				else if(striright(szFileName,DEFAULT_URL_EXT)==0)
				{
					char szUrl[MAX_URL+1];
					if(::GetPrivateProfileString("InternetShortcut","URL","",szUrl,sizeof(szUrl)-1,szFileName) > 0)
					{
						if(AddUrl(szUrl))
							nTotDrops++;
					}
					else
					{
						::MessageBoxResourceEx(this->GetSafeHwnd(),MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_DROPPED_URL,szFileName);
					}
				}
				// controlla se si tratta di un file audio (inserendolo nella lista per il player)
				else if(CAudioPlayer::IsSupportedFormat(szFileName))
				{
					// aggiunge l'item alla lista (non controlla i duplicati)
					SetStatusBarInfoPane(IDS_MESSAGE_ADDING_ITEM,::StripPathFromFile(szFileName));

					int nItem = m_wndPlaylist.AddItem(FormatPlaylistItem(szFileName),GetIconIndex(szFileName,&m_listPlaylistIcons,&m_wndPlaylist),IDM_WALLPAPER_POPUP_MENU,m_wndPlaylist.GetItemCount());
					if(nItem!=LB_ERR)
					{
						if(!pAudioFilesList)
							pAudioFilesList = new CAudioFilesList();
						if(pAudioFilesList)
						{
							AUDIOITEM* audioItem = (AUDIOITEM*)pAudioFilesList->Add();
							if(audioItem)
							{
								strcpyn(audioItem->file,szFileName,_MAX_FILEPATH+1);
								audioItem->index = pAudioFilesList->Count();
								audioItem->lparam = (LPARAM)-1L;
								nTotDrops++;
							}
						}
					}
					else
						::MessageBoxResourceEx(this->GetSafeHwnd(),MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_ADDITEM,szFileName);
				}
				// tipo file non gestito (verra' aperto lanciando l'applicazione relativa)
				else
				{
					if(AddFile(szFileName) >= 0)
						nTotDrops++;
				}
			}
			else
			{
				// immagine
				if(AddFile(szFileName) >= 0)
					nTotDrops++;
			}
		}
		else
		{
			// directory
			nTotDrops += AddDir(szFileName,m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_RECURSE_SUBDIR_KEY));
		}
	}

	// passa la lista dei files audio al player
	if(pAudioFilesList)
	{
		if(pAudioFilesList->Count() > 0)
			OnPlayer(NULL,pAudioFilesList,AUDIOPLAYER_COMMAND_QUEUE_FROM_LIST,TRUE);
		delete pAudioFilesList;
	}

	// stabilisce (e posiziona la barra sul) l'elemento corrente
	if(nTotDrops > 0 && nItemCount <= 0)
	{
		m_wndPlaylist.SelectItem(0);
		SendWallPaperMessage(WALLPAPER_GOTO_FIRST_PICTURE);
	}

	// aggiorna la barra di stato
	SetStatusBarPicturePanelCount();
	SetStatusBarInfoPane(IDS_MESSAGE_FILEDROPPED,nTotDrops);

	// aggiorna il dialogo
	OnUpdateGUI();
	PlaylistSetModified();
}

/*
	OnDropOle()

	Carica gli oggetti ricevuti via drag and drop tramite OLE (al momento solo url).
	Il tipo degli oggetti ricevuti dipende da quanto registrato con la chiamata ad AddClipboardFormat().
*/
LRESULT CWallPaperDlg::OnDropOle(WPARAM /*wParam*/,LPARAM lParam)
{
	// mette in primo piano quando riceve via drag & drop
	SetForegroundWindowEx(this->GetSafeHwnd());

	// controlla che non sia in pausa
	if(m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_STATE_KEY)==WALLPAPER_IN_PAUSED_STATE)
	{
		::MessageBoxResource(this->GetSafeHwnd(),MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_PAUSED);
		return(0L);
	}

	LPSTR pUrl = (LPSTR)lParam;
	BOOL bAdded = FALSE;

	// aggiunge l'url alla lista
	SetStatusBarInfoPane(IDS_MESSAGE_ADDING_ITEM,pUrl);
	bAdded = AddUrl(pUrl);

	if(bAdded)
	{
		m_Config.UpdateString(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_ADDURL_KEY,pUrl);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_ADDURL_KEY);

		// aggiorna la barra di stato
		SetStatusBarPicturePanelCount();
		SetStatusBarInfoPane(IDS_MESSAGE_URLDROPPED,(UINT)1);

		// aggiorna il dialogo
		OnUpdateGUI();
		PlaylistSetModified();
	}

	return(0L);
}

/*
	OnTimer()

	Chiamata per ogni tick del timer.
*/
void CWallPaperDlg::OnTimer(UINT nID)
{
	if(nID==m_nDesktopTimerId)
	{
		// cambia lo sfondo
		if(m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_STATE_KEY)!=WALLPAPER_IN_PAUSED_STATE)
			SendWallPaperMessage(WALLPAPER_TIMER_CHANGE_PICTURE);
	}
	else if(nID==m_nNervousTitleTimerId)
	{
		// scorre il titolo della finestra
		NervousTitle();
	}

	CDialogEx::OnTimer(nID);
}

/*
	OnTimerChange()

	Controlla l'intervallo per il valore del timeout.
*/
void CWallPaperDlg::OnTimerChange(void) 
{
	int i,n;
	char szValue[8] = {0};
	char szNumber[8] = {0};
	GetDlgItemText(IDC_EDIT_TIMEOUT,szValue,sizeof(szValue)-1);
	for(i=0,n=0; szValue[i]; i++)
		if(isdigit(szValue[i]))
			szNumber[n++] = szValue[i];
	m_nDesktopTimeout = atoi(szNumber);
	
	if(m_nDesktopTimeout < WALLPAPER_MIN_VALUE_FOR_CHANGE_TIMEOUT || m_nDesktopTimeout > WALLPAPER_MAX_VALUE_FOR_CHANGE_TIMEOUT)
	{
		m_nDesktopTimeout = WALLPAPER_MIN_VALUE_FOR_CHANGE_TIMEOUT;
		sprintf(szValue,"%d",m_nDesktopTimeout);
		SetDlgItemText(IDC_EDIT_TIMEOUT,szValue);
	}

	if(m_pCmdLine->HaveDplFile())
		::WritePrivateProfileInt(WALLPAPER_DPL_KEY,WALLPAPER_TIMEOUT_KEY,m_nDesktopTimeout,m_pCmdLine->GetDplFile());
	else
	{
		m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_TIMEOUT_KEY,m_nDesktopTimeout);
		m_Config.SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_TIMEOUT_KEY);
	}

	Timerize();
}

/*
	OnVScroll()
*/
void CWallPaperDlg::OnVScroll(UINT nSBCode,UINT /*nPos*/,CScrollBar* /*pScrollBar*/)
{
	// per ogni cambio dello spin button se non passa il focus alla lista rimane sul campo di input
	if(nSBCode!=SB_ENDSCROLL)
		((CWnd*)GetDlgItem(IDC_LIST_PLAYLIST))->SetFocus();
}

/*
	Timerize()
*/
BOOL CWallPaperDlg::Timerize(void)
{
	BOOL bTimer = TRUE;

	if(m_nDesktopTimerId > 0)
	{
		KillTimer(m_nDesktopTimerId);
		m_nDesktopTimerId = 0;
	}
	if((m_nDesktopTimerId = SetTimer(ID_TIMER_DESKTOP,(m_nDesktopTimeout * 1000) * 60,NULL))==0)
	{
		::MessageBoxResource(this->GetSafeHwnd(),MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_DESKTOPTIMER);
		bTimer = FALSE;
	}

	return(bTimer);
}

/*
	SetWindowTitleString()

	Imposta il testo per il titolo della finestra con la stringa specificata.
*/
void CWallPaperDlg::SetWindowTitleString(LPCSTR lpcszTitle,BOOL bCheckForInfamousSentencesUsage/* = TRUE*/,BOOL bIsAnInternalString/* = FALSE*/)
{
	LPSTR pTitle = (LPSTR)lpcszTitle;

	// se deve usare il frasario, carica la frase seguente
	if(bCheckForInfamousSentencesUsage)
		if(m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_INFAMOUS_SENTENCES_KEY))
			if((pTitle = GetInfamous())==NULL)
			{
				pTitle = (LPSTR)lpcszTitle;
				m_Config.UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_INFAMOUS_SENTENCES_KEY,FALSE);
			}

	// controlla se il titolo deve essere visualizzato a scorrimento
	if(m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_NERVOUS_TITLE_KEY))
	{
		strcpyn(m_szNervousTitle,pTitle,sizeof(m_szNervousTitle)-1);
		if(!bIsAnInternalString)
		{
			char* p = "";
			static int nAlternate = 0;
			switch(nAlternate)
			{
				case 0:
					p = WALLPAPER_DEFAULT_TITLE_STRING_PRG;
					break;
				case 1:
					p = WALLPAPER_DEFAULT_TITLE_STRING_WEB;
					break;
				case 2:
				default:
					p = WALLPAPER_DEFAULT_TITLE_STRING_TAG;
					break;
			}
			strcpyn(	m_szNervousTitle+strlen(pTitle),
					m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_INFAMOUS_SENTENCES_KEY) ? WALLPAPER_DEFAULT_INFAMOUS_STRING : p,
					sizeof(m_szNervousTitle)-strlen(m_szNervousTitle)-1
					);
			if(++nAlternate > 2)
				nAlternate = 0;
		}

		// aggiunge al titolo (duplicando) fino a riempire il buffer relativo
		char s[WALLPAPER_MAX_TITLE_CHARS+1];
		_snprintf(s,sizeof(s)-1," %s",m_szNervousTitle);
		while(strlen(m_szNervousTitle) < WALLPAPER_TITLE_CHARS_IN_TASKBAR_ICON)
		{
			if(strlen(s) < (sizeof(m_szNervousTitle)-strlen(m_szNervousTitle)))
				strcatn(m_szNervousTitle,s,sizeof(m_szNervousTitle));
		}
	}

	SetWindowText(pTitle);
}

/*
	SetWindowTitleID()

	Imposta il testo per il titolo della finestra con la risorsa specificata dall'id.
*/
void CWallPaperDlg::SetWindowTitleID(UINT nID,LPCSTR lpcszText/* = NULL*/)
{
	char szTitle[WALLPAPER_MAX_TITLE_CHARS+1];
	if(lpcszText)
		::FormatResourceStringEx(szTitle,sizeof(szTitle),nID,lpcszText);
	else
		::FormatResourceString(szTitle,sizeof(szTitle),nID);
	
	SetWindowTitleString(szTitle,FALSE,TRUE);
}

/*
	NervousTitle()

	Scorre il testo del titolo della finestra.
*/
void CWallPaperDlg::NervousTitle(void)
{
	if(m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_STATE_KEY)!=WALLPAPER_IN_PAUSED_STATE)
	{
		char c = m_szNervousTitle[0];
		int i = strlen(m_szNervousTitle);
		int n;
		for(n = 0; n < i; n++)
			m_szNervousTitle[n] = m_szNervousTitle[n+1];
		m_szNervousTitle[n-1] = c;
		m_szNervousTitle[n] = '\0';
		SetWindowText(m_szNervousTitle);
	}
}

/*
	GetInfamous

	Restituisce la linea seguente dal file per le frasi.
*/
LPSTR CWallPaperDlg::GetInfamous(void)
{    
	FILE* pStream;
	static DWORD dwFilePointer = 0L;
	static char szInfamousSentence[2048] = {0};
	LPSTR lpszFileName = NULL;
	char* p;
	int nLoops = 1;

	char szInfamous[_MAX_FILEPATH+1];
	strcpyn(szInfamous,m_Config.GetString(WALLPAPER_INSTALL_KEY,WALLPAPER_DIR_KEY),sizeof(szInfamous));
	if(szInfamous[strlen(szInfamous)-1]!='\\')
		strcatn(szInfamous,"\\",sizeof(szInfamous));
	_snprintf(szInfamous,sizeof(szInfamous)-1,"%s%s",szInfamous,WALLPAPER_INFAMOUS_FILE);

	while(TRUE)
	{
		if((pStream = fopen(szInfamous,"rt"))!=(FILE*)NULL)
		{
			fseek(pStream,dwFilePointer,SEEK_SET);
			    
		    	if(fgets(szInfamousSentence,sizeof(szInfamousSentence)-1,pStream)!=NULL)
		    	{
				dwFilePointer += strlen(szInfamousSentence)+1;
			    	
				if((p = strrchr(szInfamousSentence,'\n'))!=NULL)
					*p = '\0';
				
				lpszFileName = szInfamousSentence;
			}
			else
			{
				dwFilePointer = 0L;
				if(--nLoops >= 0)
					continue;
			}
		
			fclose(pStream);
		}
		else
		{
			CreateInfamous();
			continue;
		}

		break;
	}

	return(lpszFileName);
}

/*
	CreateInfamous()

	Crea il file di default per le frasi.
*/
BOOL CWallPaperDlg::CreateInfamous(void)
{
	char szInfamous[_MAX_FILEPATH+1];
	strcpyn(szInfamous,m_Config.GetString(WALLPAPER_INSTALL_KEY,WALLPAPER_DIR_KEY),sizeof(szInfamous));
	if(szInfamous[strlen(szInfamous)-1]!='\\')
		strcatn(szInfamous,"\\",sizeof(szInfamous));
	_snprintf(szInfamous,sizeof(szInfamous)-1,"%s%s",szInfamous,WALLPAPER_INFAMOUS_FILE);
	return(::ExtractResource(IDR_INFAMOUS_TXT,"TXT",szInfamous));
}

/*
	SetStatusBarPicturePanelCount()

	Imposta il testo per la barra di stato (lista).
*/
void CWallPaperDlg::SetStatusBarPicturePanelCount(int nItemCount/* = -1*/)
{
	char szPanel[_MAX_PATH+1];
	::FormatResourceStringEx(szPanel,sizeof(szPanel),IDS_MESSAGE_ITEM_COUNT,nItemCount < 0 ? m_wndPlaylist.GetItemCount() : nItemCount);
	CDialogEx::SetStatusBarPanel(szPanel,0);
}

/*
	SetStatusBarPicturePanelMessage()
*/
void CWallPaperDlg::SetStatusBarPicturePanelMessage(UINT nID/* = (UINT)-1L */)
{
	char szPanel[_MAX_PATH+1] = {0};
	if(nID!=(UINT)-1L)
		::FormatResourceString(szPanel,sizeof(szPanel),nID);
	CDialogEx::SetStatusBarPanel(szPanel,0);
}

/*
	SetStatusBarDplPanel()

	Imposta il testo per la barra di stato (file .dpl).
*/
void CWallPaperDlg::SetStatusBarDplPanel(LPCSTR lpcszFileName)
{
	char szDplFile[_MAX_FILEPATH+1];
	memset(szDplFile,'\0',sizeof(szDplFile));

	if(lpcszFileName)
		strcpyn(szDplFile,::StripPathFromFile(lpcszFileName),sizeof(szDplFile));
	
	CDialogEx::SetStatusBarPanel(szDplFile,1);
}

/*
	SetStatusBarInfoPane()

	Imposta il testo per (le sezioni del)la barra di stato.
*/
void CWallPaperDlg::SetStatusBarInfoPane(LPCSTR lpcszText)
{
	CDialogEx::SetStatusBarPanel(lpcszText,2);
}
void CWallPaperDlg::SetStatusBarInfoPane(UINT nID)
{
	char szPanel[_MAX_PATH+1];
	::FormatResourceString(szPanel,sizeof(szPanel),nID);
	SetStatusBarInfoPane(szPanel);
}
void CWallPaperDlg::SetStatusBarInfoPane(UINT nID,LPCSTR lpcszText)
{
	char szPanel[_MAX_PATH+1];
	::FormatResourceStringEx(szPanel,sizeof(szPanel),nID,lpcszText);
	SetStatusBarInfoPane(szPanel);
}
void CWallPaperDlg::SetStatusBarInfoPane(UINT nID,UINT nNumber)
{
	char szPanel[_MAX_PATH+1];
	::FormatResourceStringEx(szPanel,sizeof(szPanel),nID,nNumber);
	SetStatusBarInfoPane(szPanel);
}

/*
	OnSetWallpaper()

	Gestore per il messaggio relativo al cambio sfondo (il cambio viene gestito tramite il messaggio
	per serializzare le richieste generate dal timer e sincronizzarle con quelle generate dall'utente).
*/
LRESULT CWallPaperDlg::OnSetWallpaper(WPARAM wParam,LPARAM /*lParam*/)
{
	int nItemCount;
	int nIndexForTheNextPicture = -1;

	// nessun elemento nella lista, ritorna
	if((nItemCount = m_wndPlaylist.GetItemCount()) <= 0)
		return(0L);

	// cambio sfondo
	CWaitCursor cursor;
	int nIndex = (int)wParam;
	int nItem = 0;
	char szItem[MAX_ITEM_SIZE+1];
	char szStatusBar[128] = {0};
	char szItemFromCompressedFile[_MAX_FILEPATH+1] = {0};
	BOOL bComesFromArchiveFile = FALSE;

	// aggiorna l'icona del dialogo
	CDialogEx::SetIcon(IDI_ICON_LOADING);
	if(m_pTrayIcon)
		m_pTrayIcon->SetIcon(IDI_ICON_LOADING);
	
	// caricamento
	SetWindowTitleID(IDS_MESSAGE_LOADING);
	SetStatusBarInfoPane(IDS_MESSAGE_LOADING);

	// chiamato con una delle macro per lo spostamento sequenziale
	if(nIndex < 0)
	{
		// modalita' (sequenziale/random)
		switch(m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_RANDOMIZE_PLAYLIST_KEY))
		{
			case RANDOMIZE_PLAYLIST_SELECTION:
			{
				//nItem = rand() % nItemCount;
				nItem = m_RandomNumberGenerator.RandRange(0,nItemCount-1);
				TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperDlg::OnSetWallpaper(): random generated index=%d\n",nItem));
				break;
			}

			case RANDOMIZE_PLAYLIST_REORDER:
			case RANDOMIZE_PLAYLIST_NONE:
			default:
			{
				nItem = m_wndPlaylist.GetCurrentItem();
				
				switch(nIndex)
				{
					case WALLPAPER_GOTO_FIRST_PICTURE:
						nItem = 0;
						break;
					case WALLPAPER_GOTO_LAST_PICTURE:
						nItem = (nItemCount-1);
						break;
					case WALLPAPER_GOTO_PREVIOUS_PICTURE:
						if(--nItem < 0)
							nItem = (nItemCount-1);
						break;
					case WALLPAPER_GOTO_NEXT_PICTURE:
					case WALLPAPER_TIMER_CHANGE_PICTURE:
						if(++nItem > (nItemCount-1))
							nItem = 0;
						break;
				}
				
				break;
			}
		}
	}
	else // chiamato su un elemento specifico della playlist
		nItem = nIndex;

	if(nItem < 0 || nItem > (nItemCount-1))
		nItem = 0;

	// seleziona l'elemento relativo all'indice
	m_wndPlaylist.SelectItem(nItem);

	// modalita' anteprima
	// la maggior parte delle chiamate che passano da qui vanno in automatico (ossia la finestra principale potrebbe non
	// avere il focus o essere iconizzata) per cui forza il focus per la finestra di preview solo se si trova il primo
	// piano e non e' iconizzato (quando si iconizza puo' rimanere in primo piano se non viene selezionata un altra app)
	OnPreviewMode(::GetForegroundWindow()==this->m_hWnd && !this->IsIconic());

	// ricava il nome file relativo all'elemento
	GetItem(szItem,sizeof(szItem),nItem);

	// se si tratta di un url
	if(m_Url.IsUrl(szItem))
	{
		SetWindowTitleID(IDS_MESSAGE_DOWNLOADING);
		SetStatusBarInfoPane(IDS_MESSAGE_DOWNLOADING);

		// se l'url referenzia una pagina web invece di un immagine la passa al browser di default e ritorna
		if(!m_pImage->IsSupportedFormat(szItem))
		{
			CBrowser browser(m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_PREFERRED_INTERNET_BROWSER_KEY));
			if(!browser.Browse(szItem))
				::MessageBoxResourceEx(this->GetSafeHwnd(),MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_BROWSER,szItem);
			SetWindowTitleString(::StripPathFromFile(szItem));
			SetStatusBarInfoPane(IDS_MESSAGE_READY);
			goto done;
		}
		else // aggiorna il nome dell'elemento corrente con quanto scaricato sopra
		{
			// scarica l'url in locale
			// occhio: qui controlla a pelo e salta il download se il file gia' esiste (ignorando la configurazione)
			// per evitare di caricare il dialogo di download, mentre la GetUrl() di cui sotto che chiama il dialogo 
			// etc. controlla se skippare o meno con quanto presente in configurazione
			char szFileName[_MAX_FILEPATH+1];
			BOOL bDownloaded = FALSE;
			char szDownloadDir[_MAX_PATH+1];
			strcpyn(szDownloadDir,m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_DOWNLOADDIR_KEY),sizeof(szDownloadDir));
			::EnsureBackslash(szDownloadDir,sizeof(szDownloadDir));
			_snprintf(szFileName,
					sizeof(szFileName)-1,
					"%s%s",
					szDownloadDir,
					::StripPathFromFile(szItem)
					);
			if(!bDownloaded)
				bDownloaded = ::FileExist(szFileName);
			if(!bDownloaded)
				bDownloaded = GetUrl(szItem,sizeof(szItem),szFileName,sizeof(szFileName));
			if(!bDownloaded)
			{
				::DeleteFile(szFileName);
				SetWindowTitleID(IDS_ERROR_URL_RETRIEVE_FAILED);
				SetStatusBarInfoPane(IDS_MESSAGE_READY);
				goto done;
			}
			else
			{
				SetWindowTitleString(::StripPathFromFile(szItem));
				SetStatusBarInfoPane(IDS_MESSAGE_READY);
				strcpyn(szItem,szFileName,sizeof(szItem));
			}
		}
	}
	// immagine o audio
	else
	{
		// controlla se si tratta di un file contenuto in un file compresso
		LPARAM lParam = m_wndPlaylist.GetItemlParam(nItem);
		
		char szLocation[MAX_ITEM_SIZE+1];
		m_wndPlaylist.GetItemText(nItem,CTRLLISTEX_LOCATION_INDEX,szLocation,sizeof(szLocation)-1);
		if(m_Archive.IsSupportedFormat(szLocation))
		{
			if(lParam==(LPARAM)-1L)
			{
				lParam = m_Archive.Map(szLocation);
				m_wndPlaylist.SetItemlParam(nItem,lParam);
			}
		}
		
		// in lParam l'indice dell'elemento nella lista dei files compressi
		if(lParam!=(LPARAM)-1L)
		{
			// ricava il nome del file compresso in cui si trova l'elemento
			LPCSTR pArchiveName = m_Archive.GetParent(lParam);
			if(pArchiveName)
			{
				// costruisce il nome dell'elemento (sul pathname temporaneo)
				char* p;
				m_wndPlaylist.GetItemText(nItem,CTRLLISTEX_FILENAME_INDEX,szItemFromCompressedFile,sizeof(szItemFromCompressedFile)-1);
				while((p = strchr(szItemFromCompressedFile,'\\'))!=NULL)
					*p = '/';						
				if((p = strrchr(szItemFromCompressedFile,'/'))!=NULL)
					p++;
				if(!p)
					p = szItemFromCompressedFile;
				_snprintf(szItem,sizeof(szItem)-1,"%s%s",m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_TEMPDIR_KEY),p);
				
				// estrae il file
				SetWindowTitleID(IDS_MESSAGE_UNCOMPRESSING);
				SetStatusBarInfoPane(IDS_MESSAGE_UNCOMPRESSING);
				LRESULT lRet = m_Archive.Extract(szItemFromCompressedFile,pArchiveName,m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_TEMPDIR_KEY));
				SetStatusBarInfoPane(IDS_MESSAGE_READY);
				if(lRet!=0L)
				{
					bComesFromArchiveFile = FALSE;
					
					// visualizza l'errore solo se non e' un file audio, il player gestisce l'errore per conto suo
					if(!CAudioPlayer::IsSupportedFormat(szItemFromCompressedFile))
					{
						::MessageBoxResourceEx(this->GetSafeHwnd(),MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_EXTRACT_FILE,szItemFromCompressedFile,pArchiveName,/*handler->lpfnGetErrorCodeDescription(lRet)*/m_Archive.GetErrorCodeDescription(lRet,pArchiveName),lRet);
						SetWindowTitleID(IDS_ERROR_EXTRACTFILE,szItemFromCompressedFile);
						SetStatusBarInfoPane(IDS_MESSAGE_READY);
					}
							
					char szString[_MAX_FILEPATH];
					::FormatResourceStringEx(szString,sizeof(szString),IDS_ERROR_EXTRACT_FAILED,szItemFromCompressedFile);
					SetWindowTitleString(szString,FALSE,TRUE);
				}
				else
					bComesFromArchiveFile = TRUE;
			}
		}

		// controlla che il file esista
		if(!::FileExist(szItem))
		{
			SetWindowTitleID(IDS_ERROR_FILENOTFOUND,szItem);
			SetStatusBarInfoPane(IDS_MESSAGE_READY);
			goto done;
		}
	}

	// controlla se si tratta di un file audio (.mp3, .wav, .cda)
	// se l'indice corrente e' negativo la chiamata e' avvenuta per timeout (next picture) per cui si limita a mettere in coda
	// il file, se l'indice e' positivo il file e' stato selezionato esplicitamente (load o doppio click), per cui lo riproduce
	if(CAudioPlayer::IsSupportedFormat(szItem))
	{
		char szProperties[MAX_ITEM_SIZE+1] = {0};
		m_wndPlaylist.GetItemText(nItem,CTRLLISTEX_INFO_INDEX,szProperties,sizeof(szProperties));
		if(szProperties[0]==' ' || szProperties[0]=='\0')
		{
			CAudioInfo audioInfo(szItem);
			QWORD qwFileSize = audioInfo.GetFileSize();
			long lMinutes = 0L;
			long lSeconds = 0L;
			audioInfo.GetLength(lMinutes,lSeconds);

			if(striright(szItem,MP3_EXTENSION)==0)
			{
				_snprintf(szProperties,
						sizeof(szProperties)-1,
						"%d kbps, "
						"%ld khz, "
						"%s "
						"(%02d:%02d)",
						audioInfo.GetBitRate(),
						audioInfo.GetFrequency(),
						audioInfo.GetChannelMode(),
						lMinutes,lSeconds
						);
			}
			else if(striright(szItem,WAV_EXTENSION)==0)
			{
				_snprintf(szProperties,
						sizeof(szProperties)-1,
						"%d bit, "
						"%ld khz, "
						"%s "
						"(%02d:%02d)",
						audioInfo.GetBitRate(),
						audioInfo.GetFrequency(),
						audioInfo.GetChannelMode(),
						lMinutes,lSeconds
						);
			}
			else if(striright(szItem,CDA_EXTENSION)==0)
			{
				_snprintf(szProperties,
						sizeof(szProperties)-1,
						"CD Audio Track - %s (%02d:%02d)",
						audioInfo.GetTitle(),lMinutes,lSeconds
						);
			}
			
			// aggiorna l'elemento della lista controllando se si tratta di un
			// file compresso per visualizzare correttamente l'icona relativa
			char szItemFile[_MAX_FILEPATH+1];
			GetItem(szItemFile,sizeof(szItemFile),nItem);				
			char szLocation[MAX_ITEM_SIZE+1];
			char* pCompressedFileExt = {".arj"};
			BOOL bComesFromArchiveFile = m_Archive.IsSupportedFormat(::StripFileFromPath(szItemFile,szLocation,sizeof(szLocation),TRUE));
			if(bComesFromArchiveFile)
			{
				switch(m_Archive.GetArchiveType(::StripFileFromPath(szItemFile,szLocation,sizeof(szLocation),TRUE)))
				{
					case ARCHIVETYPE_GZW:
						pCompressedFileExt = GZW_EXTENSION;
						break;
					case ARCHIVETYPE_ZIP:
						pCompressedFileExt = ZIP_EXTENSION;
						break;
					case ARCHIVETYPE_RAR:
						pCompressedFileExt = RAR_EXTENSION;
						break;
				}
			}
			m_wndPlaylist.UpdateItem(nItem,
								FormatPlaylistItem(szItemFile,NULL,szProperties,qwFileSize),
								GetIconIndex(bComesFromArchiveFile ? pCompressedFileExt : szItemFile,&m_listPlaylistIcons,&m_wndPlaylist),
								IDM_WALLPAPER_POPUP_MENU
								);
		}
		
		strcpyn(szStatusBar,szProperties,sizeof(szStatusBar));

		if(nIndex >= 0)
		{
			TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperDlg::OnSetWallpaper(): calling OnPlayer(play,false,%s)\n",szItem));
			OnPlayer(szItemFromCompressedFile[0]=='\0' ? szItem : szItemFromCompressedFile,NULL,AUDIOPLAYER_COMMAND_PLAY_FROM_FILE,FALSE);
		}
		else
		{
			TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperDlg::OnSetWallpaper(): calling OnPlayer(queue,true,%s)\n",szItem));
			OnPlayer(szItemFromCompressedFile[0]=='\0' ? szItem : szItemFromCompressedFile,NULL,AUDIOPLAYER_COMMAND_QUEUE_FROM_FILE,TRUE);
		}
	}

	// se si tratta di un immagine, aggiorna lo sfondo
	if(m_pImage->IsSupportedFormat(szItem))
	{
		SetDesktopWallPaper(szItem,nItem,FALSE);
		if(bComesFromArchiveFile && m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CLEAR_TEMPORARY_FILES_KEY)==TEMPORARY_FILES_CLEAR_AT_RUNTIME)
			::DeleteFile(szItem);
		_snprintf(szStatusBar,sizeof(szStatusBar)-1,"%d x %d pixels, %d%s colors, %d bpp",m_pImage->GetWidth(),m_pImage->GetHeight(),(m_pImage->GetNumColors() > 256 || m_pImage->GetNumColors()==0) ? 16 : m_pImage->GetNumColors(),(m_pImage->GetNumColors() > 256 || m_pImage->GetNumColors()==0) ? "M" : "",m_pImage->GetBPP());
	}
	else
	{
		if(nIndex==WALLPAPER_TIMER_CHANGE_PICTURE)
		{
			int i,n;
			int nItemCount = m_wndPlaylist.GetItemCount();
			char szCurrentItem[MAX_ITEM_SIZE+1];
			char szItem[MAX_ITEM_SIZE+1];
			
			GetItem(szCurrentItem,sizeof(szCurrentItem),nItem);
			
			for(i = nItem + 1,n = nItemCount-1; n >= 0; i++,n--)
			{
				if(i >= nItemCount)
					i = 0;
				
				GetItem(szItem,sizeof(szItem),i);
				if(m_pImage->IsSupportedFormat(szItem) && stricmp(szItem,szCurrentItem)!=0)
				{
					nIndexForTheNextPicture = i;
					break;
				}
			}
		}
	}

	// aggiorna la selezione corrente
	if(m_pCmdLine->HaveDplFile())
		::WritePrivateProfileInt(WALLPAPER_DPL_KEY,WALLPAPER_CURRENTPICT_KEY,m_wndPlaylist.GetCurrentItem(),m_pCmdLine->GetDplFile());
	else
	{
		m_Config.UpdateNumber(WALLPAPER_PICTURES_KEY,WALLPAPER_CURRENTPICT_KEY,m_wndPlaylist.GetCurrentItem());
		m_Config.SaveKey(WALLPAPER_PICTURES_KEY,WALLPAPER_CURRENTPICT_KEY);
		if(!PlaylistIsModified())
			m_Config.SetModified(FALSE);
	}

	// imposta il titolo del dialogo con il nome dell'elemento (eliminando il pathname)
	m_strTitleBar.Format("%s",::StripPathFromFile(szItem));
	SetWindowTitleString(m_strTitleBar);
	SetStatusBarInfoPane(szStatusBar);

done:

	// aggiorna l'icona del dialogo
	CDialogEx::SetIcon(IDI_ICON_WALLPAPER);
	if(m_pTrayIcon)
	{
		m_pTrayIcon->SetIcon(IDI_ICON_WALLPAPER);
		m_pTrayIcon->SetToolTip(m_strTitleBar);
	}

	if(nIndexForTheNextPicture!=-1)
		PostWallPaperMessage(nIndexForTheNextPicture);
	
	return(0L);
}

/*
	SetDesktopWallPaper()

	Cambia lo sfondo con l'immagine specificata.
*/
BOOL CWallPaperDlg::SetDesktopWallPaper(LPCSTR lpcszFileName,int nItem,BOOL bCalledFromCommandLine)
{
	BOOL bChanged = FALSE;
	BOOL bIsPictureFile = FALSE;
	char szItemFileName[_MAX_FILEPATH+1];
	strcpyn(szItemFileName,lpcszFileName,sizeof(szItemFileName));

	// controlla che il file esista
	if(bCalledFromCommandLine)
		if(!::FileExist(szItemFileName))
		{
			::MessageBoxResourceEx(this->GetSafeHwnd(),MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_OPEN_FILE,szItemFileName);
			return(bChanged);
		}

	if(m_mutexDesktop.Lock())
	{
		// imposta il pathname completo per il file da utilizzare come sfondo
		char szWallPaperDir[_MAX_FILEPATH+1];
		char szWallPaper[_MAX_FILEPATH+1];
		strcpyn(szWallPaperDir,m_Config.GetString(WALLPAPER_INSTALL_KEY,WALLPAPER_DIR_KEY),sizeof(szWallPaperDir));
		if(szWallPaperDir[strlen(szWallPaperDir)-1]!='\\')
			strcatn(szWallPaperDir,"\\",sizeof(szWallPaperDir));
		_snprintf(szWallPaper,sizeof(szWallPaper)-1,"%s%s.bmp",szWallPaperDir,WALLPAPER_PROGRAM_NAME);
		
		// per le info sull'immagine corrente
		memset(&m_PictureInfo,'\0',sizeof(PICTUREINFO));

		// carica l'immagine
		if(m_pImage->Load(szItemFileName))
		{
			bIsPictureFile = TRUE;

			// imposta le info relative all'immagine
			m_PictureInfo.nWidth = m_pImage->GetWidth();
			m_PictureInfo.nHeight = m_pImage->GetHeight();
			_snprintf(m_PictureInfo.szWidthxHeight,sizeof(m_PictureInfo.szWidthxHeight)-1,"%d x %d",m_PictureInfo.nWidth,m_PictureInfo.nHeight);
			m_PictureInfo.nColors = m_pImage->GetNumColors();
			_snprintf(m_PictureInfo.szColors,sizeof(m_PictureInfo.szColors)-1,"%d%s",(m_PictureInfo.nColors > 256 || m_PictureInfo.nColors==0) ? 16 : m_PictureInfo.nColors,(m_PictureInfo.nColors > 256 || m_PictureInfo.nColors==0) ? "M" : "");
			m_PictureInfo.nBPP = m_pImage->GetBPP();
			_snprintf(m_PictureInfo.szBPP,sizeof(m_PictureInfo.szBPP)-1,"%d",m_PictureInfo.nBPP);
			strsize(m_PictureInfo.szMemUsed,sizeof(m_PictureInfo.szMemUsed),m_pImage->GetMemUsed());
			m_PictureInfo.dwMemUsed = m_pImage->GetMemUsed();
			m_PictureInfo.dwFileSize = m_pImage->GetFileSize();
			strsize(m_PictureInfo.szFileSize,sizeof(m_PictureInfo.szFileSize),m_pImage->GetFileSize());
			m_pImage->GetDPI(m_PictureInfo.nXRes,m_PictureInfo.nYRes);
			m_PictureInfo.nResType = m_pImage->GetURes();
			m_PictureInfo.nQuality = m_pImage->GetQuality();
			_snprintf(m_PictureInfo.szQuality,sizeof(m_PictureInfo.szQuality)-1,"%d",m_PictureInfo.nQuality);
			strcpy(m_PictureInfo.szDrawmode,"");
			strcpy(m_PictureInfo.szEffect,"");
			
			// se chiamato da linea di comando non deve impostare le proprieta'
			if(!bCalledFromCommandLine)
			{
				// se le informazioni relative non sono gia' state impostate
				char szProperties[MAX_ITEM_SIZE+1] = {0};
				m_wndPlaylist.GetItemText(nItem,CTRLLISTEX_INFO_INDEX,szProperties,sizeof(szProperties));
				if(szProperties[0]==' ' || szProperties[0]=='\0')
				{
					// ricava le informazioni relative all'immagine
					_snprintf(szProperties,
							sizeof(szProperties)-1,
							"%d x %d pixels, %s colors, %d bpp, %s required",
							m_PictureInfo.nWidth,
							m_PictureInfo.nHeight,
							m_PictureInfo.szColors,
							m_PictureInfo.nBPP,
							m_PictureInfo.szMemUsed
							);

					// aggiorna l'elemento della lista controllando se si tratta di un
					// file compresso per visualizzare correttamente l'icona relativa
					char szItemFile[_MAX_FILEPATH+1];
					GetItem(szItemFile,sizeof(szItemFile),nItem);				
					char* pFilename = szItemFile;
					char szLocation[MAX_ITEM_SIZE+1];
					char* pCompressedFileExt = {".arj"};
					BOOL bComesFromArchiveFile = m_Archive.IsSupportedFormat(::StripFileFromPath(szItemFile,szLocation,sizeof(szLocation),TRUE));
					if(bComesFromArchiveFile)
					{
						switch(m_Archive.GetArchiveType(::StripFileFromPath(szItemFile,szLocation,sizeof(szLocation),TRUE)))
						{
							case ARCHIVETYPE_GZW:
								pCompressedFileExt = GZW_EXTENSION;
								break;
							case ARCHIVETYPE_ZIP:
								pCompressedFileExt = ZIP_EXTENSION;
								break;
							case ARCHIVETYPE_RAR:
								pCompressedFileExt = RAR_EXTENSION;
								break;
						}
						
						pFilename = pCompressedFileExt;
					}
					else
					{
						// se si tratta di un url che referenzia un immagine
						if(m_Url.IsUrl(::StripFileFromPath(szItemFile,szLocation,sizeof(szLocation),TRUE)))
							pFilename = DEFAULT_URL_EXT;
					}

					m_wndPlaylist.UpdateItem(nItem,
										FormatPlaylistItem(szItemFile,NULL,szProperties,m_PictureInfo.dwFileSize),
										GetIconIndex(pFilename,&m_listPlaylistIcons,&m_wndPlaylist),
										IDM_WALLPAPER_POPUP_MENU
										);
				}
			}
		}
		else
		{
			bIsPictureFile = FALSE;

			if(!bCalledFromCommandLine)
			{
				SetWindowTitleID(IDS_ERROR_LOAD_PICTURE,szItemFileName);
				SetStatusBarInfoPane(IDS_MESSAGE_READY);
			}

			CRegistry registry;
			bChanged = registry.OpenFileType(szItemFileName);
		}

		int nDrawMode = m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_DRAWMODE_KEY);
		
		// caricamento avvenuto
		if(bIsPictureFile)
		{
			// modalita'
			SetWallPaperMode(WINDOWS_DRAWMODE_NORMAL);
			if(nDrawMode!=DRAWMODE_ORIGINAL_SIZE)
			{
				CRect drawRect(0,0,0,0);
				int nScreenWidth = ::GetSystemMetrics(SM_CXSCREEN);
				int nScreenHeight = ::GetSystemMetrics(SM_CYSCREEN);
				BOOL bAspectRatio = TRUE;

				if(nDrawMode==DRAWMODE_TILE_STRETCH || nDrawMode==DRAWMODE_TILE_FIT)
				{
					int nScreenArea = nScreenWidth * nScreenHeight;
					int nPictureArea = m_pImage->GetWidth() * m_pImage->GetHeight();
					int nRatio = (100 * nPictureArea) / nScreenArea;
					if(nRatio < (int)m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_DRAWAREARATIO_KEY))
						nDrawMode = DRAWMODE_WINDOWS_TILE;
					else
						nDrawMode = nDrawMode==DRAWMODE_TILE_STRETCH ? DRAWMODE_STRETCH_TO_SCREEN : DRAWMODE_FIT_TO_SCREEN;
				}
				
				switch(nDrawMode)
				{
					case DRAWMODE_FIT_TO_SCREEN:
						bAspectRatio = FALSE;
						drawRect.right  = nScreenWidth;
						drawRect.bottom = nScreenHeight;
						strcpyn(m_PictureInfo.szDrawmode,"fit to screen",sizeof(m_PictureInfo.szDrawmode));
						break;
					
					case DRAWMODE_STRETCH_IF_DOES_NOT_FIT:
					{
						BOOL bStretch = FALSE;
						drawRect.right  = m_pImage->GetWidth();
						drawRect.bottom = m_pImage->GetHeight();
						if(drawRect.right > nScreenWidth)
						{
							drawRect.right = nScreenWidth;
							bStretch = TRUE;
						}
						if(drawRect.bottom > nScreenHeight)
						{
							drawRect.bottom = nScreenHeight;
							bStretch = TRUE;
						}
						if(!bStretch)
							drawRect.right = drawRect.bottom = 0;
						strcpyn(m_PictureInfo.szDrawmode,"stretch if does not fit",sizeof(m_PictureInfo.szDrawmode));
						break;
					}
					
					case DRAWMODE_STRETCH_TO_SCREEN:
						drawRect.right  = nScreenWidth;
						drawRect.bottom = nScreenHeight;
						strcpyn(m_PictureInfo.szDrawmode,"stretch to screen",sizeof(m_PictureInfo.szDrawmode));
						break;
					
					case DRAWMODE_STRETCH_TO_RECT:
						drawRect.right  = m_rcDrawRect.right;
						drawRect.bottom = m_rcDrawRect.bottom;
						drawRect.right  = drawRect.right > nScreenWidth ? nScreenWidth : drawRect.right;
						drawRect.bottom = drawRect.bottom > nScreenHeight ? nScreenHeight : drawRect.bottom;
						strcpyn(m_PictureInfo.szDrawmode,"stretch to rect",sizeof(m_PictureInfo.szDrawmode));
						break;
					
					case DRAWMODE_WINDOWS_NORMAL:
						SetWallPaperMode(WINDOWS_DRAWMODE_NORMAL);
						strcpyn(m_PictureInfo.szDrawmode,"(windows) normal",sizeof(m_PictureInfo.szDrawmode));
						break;
					
					case DRAWMODE_WINDOWS_TILE:
						SetWallPaperMode(WINDOWS_DRAWMODE_TILE);
						strcpyn(m_PictureInfo.szDrawmode,"(windows) tile",sizeof(m_PictureInfo.szDrawmode));
						break;
					
					case DRAWMODE_WINDOWS_STRETCH:
						SetWallPaperMode(WINDOWS_DRAWMODE_STRETCH);
						strcpyn(m_PictureInfo.szDrawmode,"(windows) stretch",sizeof(m_PictureInfo.szDrawmode));
						break;
					
					case DRAWMODE_OVERLAP_STRETCH:
					case DRAWMODE_OVERLAP_FIT:
					{
						bAspectRatio = nDrawMode==DRAWMODE_OVERLAP_STRETCH ? TRUE : FALSE;

						int nScreenArea = nScreenWidth * nScreenHeight;
						int nPictureArea = m_pImage->GetWidth() * m_pImage->GetHeight();
						int nRatio = (100 * nPictureArea) / nScreenArea;
						if(nRatio < (int)m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_DRAWAREARATIO_KEY))
						{
							CImageFactory ImageFactory;
							CImage* pImage = ImageFactory.Create();
							pImage->SetDIB(m_pImage->GetDIB());
							drawRect.right  = nScreenWidth;
							drawRect.bottom = nScreenHeight;
							m_pImage->Stretch(drawRect,bAspectRatio);
							m_pImage->Paste((nScreenWidth-pImage->GetWidth()) / 2,(nScreenHeight-pImage->GetHeight()) / 2,pImage->GetDIB());
						}
						else
						{
							drawRect.right  = nScreenWidth;
							drawRect.bottom = nScreenHeight;
						}
						strcpyn(m_PictureInfo.szDrawmode,nDrawMode==DRAWMODE_OVERLAP_STRETCH ? "overlap stretch" : "overlap fit",sizeof(m_PictureInfo.szDrawmode));
						break;
					}
						
					default:
						break;
				}
				
				// se deve ridimensionare l'immagine
				if(drawRect.right!=0 && drawRect.bottom!=0)
				{
					if(!bCalledFromCommandLine)
					{
						SetWindowTitleID(IDS_MESSAGE_STRETCHING);
						SetStatusBarInfoPane(IDS_MESSAGE_STRETCHING);
					}

					m_pImage->Stretch(drawRect,bAspectRatio);
				}
			}

			// effetto
			int nEffect;
			if((nEffect = m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_DRAWEFFECT_KEY))!=DRAWEFFECT_NONE)
			{
				if(!bCalledFromCommandLine)
				{
					SetWindowTitleID(IDS_MESSAGE_EFFECT);
					SetStatusBarInfoPane(IDS_MESSAGE_EFFECT);
				}

				CImageParams ImageParams;
				ImageParams.Reset();
				m_pImage->SetImageParamsDefaultValues(&ImageParams);
				m_pImage->SetImageParamsMinMax(&ImageParams);
				
				int nEffectValue = m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_DRAWEFFECTVALUE_KEY);
				
				// applica l'effetto specificato all'immagine
				switch(nEffect)
				{
					case DRAWEFFECT_GRAYSCALE:
						m_pImage->Grayscale(&ImageParams);
						strcpyn(m_PictureInfo.szEffect,"grayscale",sizeof(m_PictureInfo.szEffect));
						break;
					
					case DRAWEFFECT_ROTATE90LEFT:
						m_pImage->Rotate90Left(&ImageParams);
						strcpyn(m_PictureInfo.szEffect,"rotate 90 left",sizeof(m_PictureInfo.szEffect));
						break;
					
					case DRAWEFFECT_ROTATE90RIGHT:
						m_pImage->Rotate90Right(&ImageParams);
						strcpyn(m_PictureInfo.szEffect,"rotate 90 right",sizeof(m_PictureInfo.szEffect));
						break;
					
					case DRAWEFFECT_ROTATE180:
						m_pImage->Rotate180(&ImageParams);
						strcpyn(m_PictureInfo.szEffect,"rotate 180",sizeof(m_PictureInfo.szEffect));
						break;
					
					case DRAWEFFECT_BLUR:
						ImageParams.SetBlur(NormalizeEffectValue(nEffectValue,ImageParams.GetBlurMin(),ImageParams.GetBlurMax()));
						m_pImage->Blur(&ImageParams);
						strcpyn(m_PictureInfo.szEffect,"blur",sizeof(m_PictureInfo.szEffect));
						break;
					
					case DRAWEFFECT_BRIGHTNESS:
						ImageParams.SetBrightness((double)NormalizeEffectValue(nEffectValue,(int)ImageParams.GetBrightnessMin(),(int)ImageParams.GetBrightnessMax()));
						m_pImage->Brightness(&ImageParams);
						strcpyn(m_PictureInfo.szEffect,"brightness",sizeof(m_PictureInfo.szEffect));
						break;
					
					case DRAWEFFECT_CONTRAST:
						ImageParams.SetContrast(NormalizeEffectValue(nEffectValue,ImageParams.GetContrastMin(),ImageParams.GetContrastMax()));
						m_pImage->Contrast(&ImageParams);
						strcpyn(m_PictureInfo.szEffect,"contrast",sizeof(m_PictureInfo.szEffect));
						break;
					
					case DRAWEFFECT_EDGEENHANCE:
						m_pImage->EdgeEnhance(&ImageParams);
						strcpyn(m_PictureInfo.szEffect,"edge enhance",sizeof(m_PictureInfo.szEffect));
						break;
					
					case DRAWEFFECT_EMBOSS:
						ImageParams.SetEmboss(NormalizeEffectValue(nEffectValue,ImageParams.GetEmbossMin(),ImageParams.GetEmbossMax()));
						m_pImage->Emboss(&ImageParams);
						strcpyn(m_PictureInfo.szEffect,"emboss",sizeof(m_PictureInfo.szEffect));
						break;
					
					case DRAWEFFECT_FINDEDGE:
						ImageParams.SetFindEdge(NormalizeEffectValue(nEffectValue,ImageParams.GetFindEdgeMin(),ImageParams.GetFindEdgeMax()));
						m_pImage->FindEdge(&ImageParams);
						strcpyn(m_PictureInfo.szEffect,"find edge",sizeof(m_PictureInfo.szEffect));
						break;
					
					case DRAWEFFECT_INVERT:
						m_pImage->Invert(&ImageParams);
						strcpyn(m_PictureInfo.szEffect,"invert",sizeof(m_PictureInfo.szEffect));
						break;
					
					case DRAWEFFECT_MIRRORH:
						m_pImage->MirrorHorizontal(&ImageParams);
						strcpyn(m_PictureInfo.szEffect,"horiz mirror",sizeof(m_PictureInfo.szEffect));
						break;
					
					case DRAWEFFECT_MIRRORV:
						m_pImage->MirrorVertical(&ImageParams);
						strcpyn(m_PictureInfo.szEffect,"vert mirror",sizeof(m_PictureInfo.szEffect));
						break;
					
					case DRAWEFFECT_SHARPEN:
						ImageParams.SetSharpen((double)NormalizeEffectValue(nEffectValue,(int)ImageParams.GetSharpenMin(),(int)ImageParams.GetSharpenMax()));
						m_pImage->Sharpen(&ImageParams);
						strcpyn(m_PictureInfo.szEffect,"sharpen",sizeof(m_PictureInfo.szEffect));
						break;
					
					default:
						break;
				}
			}

			// testo (all'interno dell'immagine)
			int nDrawTextMode;
			nDrawTextMode = m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_DRAWTEXTMODE_KEY);
			if(nDrawTextMode!=0)
			{
				// ricava il tipo di stringa da usare per il testo
				char szText[1024] = {0};
				if(nDrawTextMode==DRAWTEXT_DATETIME) // data/ora
				{
					CDateTime dateTime(GMT);
					_snprintf(szText,sizeof(szText)-1,"%s",dateTime.GetFormattedDate(TRUE));
				}
				else if(nDrawTextMode==DRAWTEXT_FILENAME) // nomefile
				{
					_snprintf(szText,sizeof(szText)-1,"%s",::StripPathFromFile(szItemFileName));
				}
				else if(nDrawTextMode==DRAWTEXT_PATHNAME) // pathname
				{
					_snprintf(szText,sizeof(szText)-1,"%s",szItemFileName);
				}
				else if(nDrawTextMode==DRAWTEXT_QUOTES) // (in)famous sentences
				{
					_snprintf(szText,sizeof(szText)-1,"%s",GetInfamous());
				}

				// ricava il tipo di font da utilizzare
				CString strFontName;
				CHOOSEFONT cf = {0};
				LOGFONT lf = {0};
				cf.lStructSize = sizeof(CHOOSEFONT);
				cf.hwndOwner = this->GetSafeHwnd();
				cf.lpLogFont = &lf;
				cf.Flags = CF_SCREENFONTS | CF_EFFECTS;
				cf.iPointSize = m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_CF_iPointSize);
				cf.rgbColors = m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_CF_rgbColors);
				cf.nFontType = (WORD)m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_CF_nFontType);
				lf.lfHeight = m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfHeight);
				lf.lfWidth = m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfWidth);
				lf.lfEscapement = m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfEscapement);
				lf.lfOrientation = m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfOrientation);
				lf.lfWeight = m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfWeight);
				lf.lfItalic = (BYTE)m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfItalic);
				lf.lfUnderline = (BYTE)m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfUnderline);
				lf.lfStrikeOut = (BYTE)m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfStrikeOut);
				lf.lfCharSet = (BYTE)m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfCharSet);
				lf.lfOutPrecision = (BYTE)m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfOutPrecision);
				lf.lfClipPrecision = (BYTE)m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfClipPrecision);
				lf.lfQuality = (BYTE)m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfQuality);
				lf.lfPitchAndFamily = (BYTE)m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfPitchAndFamily);
				strFontName.Format("%s",m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfFaceName));
				lstrcpy(lf.lfFaceName,strFontName);

				// seleziona il font
				CDC* pDC = GetDC();
				CFont Font;
				Font.CreateFontIndirect(&lf);
				CFont* pOldFont = pDC->SelectObject(&Font);
				CSize sizeExtent = pDC->GetTextExtent(szText,strlen(szText));
				pDC->SelectObject(pOldFont);
				Font.DeleteObject();
				ReleaseDC(pDC);

				// ricava la posizione della taskbar per aggiustare le coordinate a cui scrivere il testo nell'immagine
				// (la taskbar potrebbe coprire il testo)
				TASKBARPOS tbi;
				::GetTaskBarPos(&tbi);

				// deve effettuare l'aggiustamento con la taskbar solo se l'immagine si interseca con la parte da essa occupata
				CRect rcScreen(0,0,tbi.nScreenWidth - tbi.nTaskbarWidth,tbi.nScreenHeight - tbi.nTaskbarHeight);
				if(m_pImage->GetWidth() <= (UINT)rcScreen.right)
					tbi.nTaskbarWidth = 0;
				if(m_pImage->GetHeight() <= (UINT)rcScreen.bottom)
					tbi.nTaskbarHeight = 0;

				// ricava l'orientazione (posizione) per la stringa di testo all'interno dell'immagine (lascia <n> pixel di margine)
				SIZE size = {0,0};
				int nTextPosition = m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_DRAWTEXTPOSITION_KEY);
				switch(nTextPosition)
				{
					case DRAWTEXT_POS_RIGHTDOWN:
						size.cx = m_pImage->GetWidth() - (sizeExtent.cx + DRAWTEXT_POS_H_MARGIN + (tbi.nTaskbarPlacement==ABE_RIGHT ? tbi.nTaskbarWidth : 0));
						size.cy = m_pImage->GetHeight() - (sizeExtent.cy + DRAWTEXT_POS_V_MARGIN + (tbi.nTaskbarPlacement==ABE_BOTTOM ? tbi.nTaskbarHeight : 0));
						break;
					
					case DRAWTEXT_POS_LEFTUP:
						size.cx = DRAWTEXT_POS_H_MARGIN + (tbi.nTaskbarPlacement==ABE_LEFT ? tbi.nTaskbarWidth : 0);
						size.cy = DRAWTEXT_POS_V_MARGIN + (tbi.nTaskbarPlacement==ABE_TOP ? tbi.nTaskbarHeight : 0);
						break;

					case DRAWTEXT_POS_RIGHTUP:
						size.cx = m_pImage->GetWidth() - (sizeExtent.cx + DRAWTEXT_POS_H_MARGIN + (tbi.nTaskbarPlacement==ABE_RIGHT ? tbi.nTaskbarWidth : 0));
						size.cy = DRAWTEXT_POS_V_MARGIN + (tbi.nTaskbarPlacement==ABE_TOP ? tbi.nTaskbarHeight : 0);
						break;

					case DRAWTEXT_POS_LEFTDOWN:
					default:
						size.cx = DRAWTEXT_POS_H_MARGIN + (tbi.nTaskbarPlacement==ABE_LEFT ? tbi.nTaskbarWidth : 0);
						size.cy = m_pImage->GetHeight() - (sizeExtent.cy + DRAWTEXT_POS_V_MARGIN + (tbi.nTaskbarPlacement==ABE_BOTTOM ? tbi.nTaskbarHeight : 0));
						break;
				}

				// scrive il testo all'interno dell'immagine
				m_pImage->Text(szText,
							&cf,
							m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_TEXTCOLOR),
							m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_BKCOLOR),
							m_Config.GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_BKCOLORTRANSPARENT) ? TRANSPARENT : OPAQUE,
							&size);
			}

			if(!bCalledFromCommandLine)
			{
				SetWindowTitleID(IDS_MESSAGE_CONVERTING);
				SetStatusBarInfoPane(IDS_MESSAGE_CONVERTING);
			}

			// i jpeg bianco e nero fanno sfarfallare nexgen
			if(m_pImage->GetBPP() <= 8)
				m_pImage->ConvertToBPP(32,0,NULL,0);

			// converte (in formato .bmp) e cambia lo sfondo
			if(m_pImage->Save(szWallPaper,".bmp"))
			{
				// se riesce il cambio dello sfondo visualizza il popup
				if((bChanged = ::SystemParametersInfo(SPI_SETDESKWALLPAPER,NULL,szWallPaper,SPIF_UPDATEINIFILE|SPIF_SENDCHANGE))==TRUE)
				{
					OnBalloon();
					OnTaskbarPopup();
				}
			}
			else
				::MessageBoxResource(this->GetSafeHwnd(),MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_SAVE_PICTURE);
		}
	
		m_mutexDesktop.Unlock();
	}
	
	return(bChanged);
}

/*
	EnableDesktopWallPaper()

	Abilita il desktop per l'utilizzo dello sfondo specificato.
*/
void CWallPaperDlg::EnableDesktopWallPaper(LPCSTR lpcszFileName,UINT nMode)
{
	CRegKey regkey;
	LONG reg;
	char value[REGKEY_MAX_KEY_VALUE+1];

	if((reg = regkey.Open(HKEY_CURRENT_USER,"Control Panel\\Desktop"))==ERROR_SUCCESS)
	{
		regkey.SetValue(lpcszFileName,"Wallpaper");
		_snprintf(value,sizeof(value)-1,"%d",nMode);
		regkey.SetValue(value,"TileWallPaper");
		regkey.Close();
	}
}

/*
	DisableDesktopWallPaper()

	Disabilita il desktop per l'utilizzo dello sfondo specificato, rimuovendo quello corrente.
*/
void CWallPaperDlg::DisableDesktopWallPaper(void)
{
	EnableDesktopWallPaper("",WINDOWS_DRAWMODE_NORMAL);
	::SystemParametersInfo(SPI_SETDESKWALLPAPER,NULL,"",SPIF_UPDATEINIFILE|SPIF_SENDCHANGE);
	
	// elimina il bitmap utilizzato per lo sfondo
	char szWallPaperDir[_MAX_FILEPATH+1];
	char szWallPaper[_MAX_FILEPATH+1];
	strcpyn(szWallPaperDir,m_Config.GetString(WALLPAPER_INSTALL_KEY,WALLPAPER_DIR_KEY),sizeof(szWallPaperDir));
	if(szWallPaperDir[strlen(szWallPaperDir)-1]!='\\')
		strcatn(szWallPaperDir,"\\",sizeof(szWallPaperDir));
	_snprintf(szWallPaper,sizeof(szWallPaper)-1,"%s%s.bmp",szWallPaperDir,WALLPAPER_PROGRAM_NAME);
	::DeleteFile(szWallPaper);
}

/*
	SetWallPaperMode()

	Imposta la modalita' per lo sfondo.
*/
void CWallPaperDlg::SetWallPaperMode(UINT nMode)
{
	CRegKey regkey;
	char value[REGKEY_MAX_KEY_VALUE+1];
	BOOL bHaveStyle = FALSE;

	/*
	mentre con W95/NT viene usata solo la chiave TileWallpaper (con valori da 0 a 2), la zoccola di W98 usa anche la chiave WallpaperStyle:
				normal	stretch	tile
	TileWallpaper	0		0		1
	WallpaperStyle	0		2		0
	*/
	if(regkey.Open(HKEY_CURRENT_USER,"Control Panel\\Desktop")==ERROR_SUCCESS)
	{
		DWORD dwMode = nMode==WINDOWS_DRAWMODE_STRETCH ? WINDOWS_DRAWMODE_STRETCH : WINDOWS_DRAWMODE_NORMAL;
		memset(value,'\0',sizeof(value));
		DWORD valuesize = sizeof(value);
		if(regkey.QueryValue(value,"WallpaperStyle",&valuesize)==ERROR_SUCCESS)
		{
			_snprintf(value,sizeof(value)-1,"%d",dwMode);
			regkey.SetValue(value,"WallpaperStyle");
			bHaveStyle = TRUE;
		}
		
		regkey.Close();
	}

	if(regkey.Open(HKEY_CURRENT_USER,"Control Panel\\Desktop")==ERROR_SUCCESS)
	{
		if(bHaveStyle && nMode==WINDOWS_DRAWMODE_STRETCH)
			nMode = WINDOWS_DRAWMODE_NORMAL;

		_snprintf(value,sizeof(value)-1,"%d",nMode);
		regkey.SetValue(value,"TileWallPaper");
		
		regkey.Close();
	}
}

/*
	NormalizeEffectValue()

	Adatta il valore della configurazione (con un intervallo da 0 a 100) al range previsto dalla libreria.
	Restituisce il valore adattato all'intervallo della libreria.
*/
int CWallPaperDlg::NormalizeEffectValue(int nValue,int nLibraryMin,int nLibraryMax)
{
	if(nValue==0)
		return(nLibraryMin);
	else if(nValue==100)
		return(nLibraryMax);

	if(nLibraryMin!=0 || nLibraryMax!=0)
	{
		int nRange = nLibraryMax - nLibraryMin;
		int nAdaptedValue = (nRange * nValue) / 100;
		nAdaptedValue += nLibraryMin;
		return(nAdaptedValue);
	}
	else
		return(0);
}

/*
	OnGetCurrentItem()

	Restituisce (tramite l'invio del messaggio relativo) il puntatore all'elemento corrente della playlist.
	Utilizzata per far si che il thread (UI) possa ricevere i dati dall'applicazione principale.
*/
LRESULT CWallPaperDlg::OnGetCurrentItem(WPARAM /*wParam*/,LPARAM /*lParam*/)
{
	/*HWND hWnd = (HWND)wParam;*/
	int nItem;
	static char szItem[MAX_ITEM_SIZE+1];
	memset(szItem,'\0',sizeof(szItem));

	// controlla la selezione corrente
	if((nItem = GetItem(szItem,sizeof(szItem))) < 0)
		return(0L);

	// se si tratta di un url, la scarica in locale prima di inviarla
	if(m_Url.IsUrl(szItem))
	{
		char szFileName[_MAX_FILEPATH+1];

		if(GetUrl(szItem,sizeof(szItem),szFileName,sizeof(szFileName)))
		{
			strcpyn(szItem,szFileName,sizeof(szItem));
		}
		else
		{
			SetWindowTitleID(IDS_ERROR_URL_RETRIEVE_FAILED);
			SetStatusBarInfoPane(IDS_MESSAGE_READY);
			::DeleteFile(szFileName);
			return(0L);
		}
	}

	return((LPARAM)szItem);
}

/*
	OnGetPlaylist()

	Restituisce (tramite l'invio del messaggio relativo) il puntatore alla lista utilizzata per la playlist.
	Utilizzata per far si che il thread (UI) possa ricevere i dati dall'applicazione principale.
*/
LRESULT CWallPaperDlg::OnGetPlaylist(WPARAM /*wParam*/,LPARAM /*lParam*/)
{
	return((LPARAM)&m_wndPlaylist);
}

/*
	OnGetConfiguration()

	Restituisce (tramite l'invio del messaggio relativo) il puntatore all'oggetto per la configurazione.
	Utilizzata per far si che il thread (UI) possa ricevere i dati dall'applicazione principale.
*/
LRESULT CWallPaperDlg::OnGetConfiguration(WPARAM /*wParam*/,LPARAM /*lParam*/)
{
	return((LPARAM)&m_Config);
}

/*
	IsUIThreadRunning()
	
	Controlla se l'istanza del thread (UI) si trova in esecuzione.
*/
BOOL CWallPaperDlg::IsUIThreadRunning(LPCSTR lpcszThreadName,CWinThread** pWinThread,BOOL bCheckForExistingWindow/* = FALSE*/,LPCSTR lpcszWindowTitle/* = NULL*/,HWND* phWnd/* = NULL*/,BOOL* pbIsAnExternalInstance/* = NULL */)
{
	ITERATOR iter;
	THREAD* thread;
	BOOL bHaveThread = FALSE;
	DWORD dwStatus = 0L;
	
	*pWinThread = NULL;
	if(phWnd)
		*phWnd = NULL;
	if(pbIsAnExternalInstance)
		*pbIsAnExternalInstance = FALSE;

	// scorre la lista cercando il thread in questione
	if((iter = m_listRunningUIThreads.First())!=(ITERATOR)NULL)
	{
		do
		{
			thread = (THREAD*)iter->data;
			if(thread)
			{
				if(thread->pWinThread)
				{
					*pWinThread = thread->pWinThread;
					
					// controlla se il thread in questione e' attivo
					if(::GetExitCodeThread((thread->pWinThread)->m_hThread,&dwStatus))
					{
						if(dwStatus==STILL_ACTIVE)
						{
							if(strcmp(thread->szThreadName,lpcszThreadName)==0)
							{
								bHaveThread = TRUE;
								break;
							}
						}
					}
					else // approfitta e ripulisce la lista
						delete thread->pWinThread,thread->pWinThread = NULL;
				}
			}

			iter = m_listRunningUIThreads.Next(iter);
		
		} while(iter!=(ITERATOR)NULL);
	}

	// se non ha trovato un istanza del thread nella lista interna, ma si tra le finestre attive, significa
	// che il thread e' stato creato da un altra istanza (gia' in esecuzione anteriormente) del programma
	if(!bHaveThread)
	{
		if(bCheckForExistingWindow && lpcszWindowTitle && phWnd)
		{
			HWND hWnd = ::FindWindow(NULL,lpcszWindowTitle);
			if(hWnd)
			{
				*phWnd = hWnd;
				*pWinThread = NULL;
				if(pbIsAnExternalInstance)
					*pbIsAnExternalInstance = TRUE;
				bHaveThread = TRUE;
			}
		}
	}

	return(bHaveThread);
}

/*
	IsInternalThreadRunning()
	
	Controlla se l'istanza del thread (interno) specificata si trova in esecuzione.
*/
BOOL CWallPaperDlg::IsInternalThreadRunning(LPCSTR lpcszThreadName,CThread** pThread)
{
	ITERATOR iter;
	THREAD* thread;
	BOOL bHaveThread = FALSE;
	DWORD dwStatus = 0L;
	
	*pThread = NULL;

	// scorre la lista cercando il thread in questione
	if((iter = m_listRunningInternalThreads.First())!=(ITERATOR)NULL)
	{
		do
		{
			thread = (THREAD*)iter->data;
			if(thread)
			{
				if(thread->pThread)
				{
					*pThread = thread->pThread;
					
					// controlla se il thread in questione e' attivo
					if(::GetExitCodeThread((thread->pThread)->GetHandle(),&dwStatus))
					{
						if(dwStatus==STILL_ACTIVE)
						{
							if(strcmp(thread->szThreadName,lpcszThreadName)==0)
							{
								bHaveThread = TRUE;
								break;
							}
						}
					}
					else // approfitta e ripulisce la lista
						delete thread->pThread,thread->pThread = NULL;
				}
			}

			iter = m_listRunningInternalThreads.Next(iter);
		
		} while(iter!=(ITERATOR)NULL);
	}

	return(bHaveThread);
}

/*
	AddArchiveContentToPlaylist()

	Aggiunge il contenuto del file compresso alla playlist.
*/
BOOL CWallPaperDlg::AddArchiveContentToPlaylist(LPARAM lParam,LPCSTR lpcszCompressedFileName)
{
	ITERATOR iter;
	ARCHIVEINFO* info;
	BOOL bAdded = FALSE;
	int nItem = m_wndPlaylist.GetCurrentItem();

	// interfaccia utente
	m_wndPlaylist.SetRedraw(FALSE);

	// per la lista dei files audio (eventualmente presenti) da passare al player
	CAudioFilesList* pAudioFilesList = NULL;

	// per tutti i files presenti nell'archivio compresso
	if((iter = m_Archive.GetList()->First())!=(ITERATOR)NULL)
	{
		int i = 0;
		char* pCompressedFileExt = {".arj"};
		switch(m_Archive.GetArchiveType(lpcszCompressedFileName))
		{
			case ARCHIVETYPE_GZW:
				pCompressedFileExt = GZW_EXTENSION;
				break;
			case ARCHIVETYPE_ZIP:
				pCompressedFileExt = ZIP_EXTENSION;
				break;
			case ARCHIVETYPE_RAR:
				pCompressedFileExt = RAR_EXTENSION;
				break;
		}
		
		do
		{
			info = (ARCHIVEINFO*)iter->data;
			if(info)
			{
				// aggiunge alla playlist se si tratta di un immagine
				// in info->name il nome del file compresso, nel caso in cui si volesse
				// visualizzare l'icona relativa invece di quella del .zip/.rar etc.
				if(m_pImage->IsSupportedFormat(info->name))
				{
					if((nItem = m_wndPlaylist.AddItem(	FormatPlaylistItem(info->name,lpcszCompressedFileName),
												GetIconIndex(pCompressedFileExt/*info->name*/,&m_listPlaylistIcons,&m_wndPlaylist),
												IDM_WALLPAPER_POPUP_MENU,
												m_wndPlaylist.GetItemCount(),
												lParam)
												)!=LB_ERR)
						bAdded = TRUE;
				}
				// aggiunge alla playlist se si tratta di un file audio
				else if(CAudioPlayer::IsSupportedFormat(info->name))
				{
					if((nItem = m_wndPlaylist.AddItem(	FormatPlaylistItem(info->name,lpcszCompressedFileName),
												GetIconIndex(pCompressedFileExt/*info->name*/,&m_listPlaylistIcons,&m_wndPlaylist),
												IDM_WALLPAPER_POPUP_MENU,
												m_wndPlaylist.GetItemCount(),
												lParam)
												)!=LB_ERR)
						bAdded = TRUE;
					
					// aggiunge il file audio alla lista per non passare al thread un file alla volta
					if(bAdded)
					{
						if(!pAudioFilesList)
							pAudioFilesList = new CAudioFilesList();
						if(pAudioFilesList)
						{
							AUDIOITEM* audioItem = (AUDIOITEM*)pAudioFilesList->Add();
							if(audioItem)
							{
								strcpyn(audioItem->file,info->name,_MAX_FILEPATH+1);
								audioItem->index = pAudioFilesList->Count();
								audioItem->lparam = lParam;
							}
						}
					}
				}
			}

			// per i cartoni animati
			if(++i > 15)
			{
				i = 0;
				::PeekAndPump();
			}

			iter = m_Archive.GetList()->Next(iter);
		
		} while(iter!=(ITERATOR)NULL);
	}

	// passa la lista dei files audio (se presenti) al player
	if(pAudioFilesList)
	{
		if(pAudioFilesList->Count() > 0)
			OnPlayer(NULL,pAudioFilesList,AUDIOPLAYER_COMMAND_QUEUE_FROM_LIST,TRUE);
		delete pAudioFilesList;
	}

	// interfaccia utente
	if(bAdded)
	{
		m_wndPlaylist.AutoSizeColumns();
		m_wndPlaylist.EnsureVisible(nItem,FALSE);
	}
	m_wndPlaylist.SetRedraw(TRUE);

	return(bAdded);
}

/*
	OnArchiveGetParent()

	Gestore per il messaggio relativo, ricava il nome dell'archivio in cui e' contenuto il file.
*/
LRESULT CWallPaperDlg::OnArchiveGetParent(WPARAM /*wParam*/,LPARAM lParam)
{
	LRESULT lRet = (LRESULT)-1L;
	EXTRACTFILEINFO* pExtractFileInfo = (EXTRACTFILEINFO*)lParam;
	
	if(pExtractFileInfo)
	{
		// ricava il nome dell'archivio compresso in cui si trova il file
		LPCSTR pArchiveName = m_Archive.GetParent(pExtractFileInfo->lParam);
		if(pArchiveName)
		{
			strcpyn(pExtractFileInfo->lpszParentFileName,pArchiveName,pExtractFileInfo->cbParentFileName);
			lRet = 0L;
		}
	}

	return(lRet);
}

/*
	OnArchiveExtractFile()

	Gestore per il messaggio relativo, estrae il file.
	Il chiamante deve preoccuparsi di visualizzare l'eventuale errore relativo al codice di ritorno.
*/
LRESULT CWallPaperDlg::OnArchiveExtractFile(WPARAM /*wParam*/,LPARAM lParam)
{
	LRESULT lRet = (LRESULT)-1L;
	EXTRACTFILEINFO* pExtractFileInfo = (EXTRACTFILEINFO*)lParam;
	
	if(pExtractFileInfo)
	{
		// ricava il nome dell'archivio compresso in cui si trova il file
		LPCSTR pArchiveName = m_Archive.GetParent(pExtractFileInfo->lParam);
		if(pArchiveName)
		{
			// memorizza il nome dell'archivio compresso in cui e' contenuto il file
			if(pExtractFileInfo->cbParentFileName > 0)
				_snprintf(pExtractFileInfo->lpszParentFileName,pExtractFileInfo->cbParentFileName-1,"%s",pArchiveName);
			
			// costruisce (sul pathname temporaneo) il nome per il file da estrarre
			char szItemFromCompressedFile[_MAX_FILEPATH+1];
			strcpyn(szItemFromCompressedFile,pExtractFileInfo->lpszInputFileName,sizeof(szItemFromCompressedFile)-1);
			char* p;
			while((p = strchr(szItemFromCompressedFile,'\\'))!=NULL)
				*p = '/';
			if((p = strrchr(szItemFromCompressedFile,'/'))!=NULL)
				p++;
			if(!p)
				p = szItemFromCompressedFile;
			_snprintf(pExtractFileInfo->lpszOutputFileName,pExtractFileInfo->cbOutputFileName-1,"%s%s",m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_TEMPDIR_KEY),p);
			
			// estrae il file
			SetStatusBarInfoPane(IDS_MESSAGE_UNCOMPRESSING);
			lRet = m_Archive.Extract(pExtractFileInfo->lpszInputFileName,pArchiveName,m_Config.GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_TEMPDIR_KEY));
			SetStatusBarInfoPane(IDS_MESSAGE_READY);
		}
	}

	return(lRet);
}

#ifdef _DEBUG
void DoTest(void)
{
/*	CProgressCtrlEx bar(_T(""), 100, 5000, TRUE, 0);

	for (int i = 0; i < 5000; i++) {
		bar.StepIt();
		PeekAndPump();
	}
*/
}
#endif
