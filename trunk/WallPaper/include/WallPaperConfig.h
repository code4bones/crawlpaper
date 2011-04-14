/*
	WallPaperConfig.h
	Classe derivata per la gestione della configurazione.
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
#ifndef _WALLPAPERCONFIG_H
#define _WALLPAPERCONFIG_H 1

#include "window.h"
#include "CConfig.h"
#include "CImageFactory.h"
#include "CWndLayered.h"
#include "CListCtrlEx.h"
#include "CDialogEx.h"
#include "CSock.h"
#include "CUrl.h"
#include "lmhosts.h"
#include "WallPaperVersion.h"

// per dichiarare al volo (senza usare costanti) i tipi riconosciuti per i downloads
// dovrebbe includere _come_minimo_ i tipi dichiarati come immagine, audio, html, etc.
#define DIRTY_DECLARATION_FOR_FILE_TYPES(arrayname) char* arrayname [] = {\
	".aac",\
	".ac3",\
	".arj",\
	".asx",\
	".avi",\
	".css",\
	".doc",\
	".dts",\
	".exe",\
	".fla",\
	".gz",\
	".gzw",\
	".hqx",\
	".ico",\
	".icl",\
	".lzh",\
	".m3u",\
	".mid",\
	".mp3",\
	".mpeg",\
	".mpg",\
	".ogg",\
	".pdf",\
	".pls",\
	".psd",\
	".ram",\
	".rar",\
	".rm",\
	".rss",\
	".sit",\
	".swf",\
	".tar",\
	".tgz",\
	".txt",\
	".zip",\
	".wav",\
	".wma",\
	".wmv",\
	".xml",\
	NULL};

/*
	Opzioni NON modificabili via GUI:
	
	\HKEY_CURRENT_USER\Software\WallPaper\Install
		LatestVersionCheckType = 1965|0|>0 per check ultima versione, 1965=mai, 0=allo startup, >0=ogni n giorni

	\HKEY_CURRENT_USER\Software\WallPaper\Options
		CrawlerDefaultHtmlFileName = default .html filename ("index.html")
		CrawlerDefaultMp3FileName = default .mp3 filename ("default.mp3")
		CrawlerOpenReport = 1|0 (per aprire o meno il report alla fine del processo di crawling)
		CrawlerMaxThreads = n (numero massimo di threads utilizzato dal crawler se in modalita' multithreads)
		CrawlerThumbnailSize = n (dimensione entro la quale l'immagine viene considerata un thumbnail/banner)
		CrawlerWinsockDelay = n (delay in ms tra la chiamata alla send() e quella alla receive())
		FakeAudioUserAgent = "..." (user agent per il ripper se l'host risponde 403)
		PreferredInternetBrowser = ammette NETSCAPE,IEXPLORE,FIREFOX,OPERA,CHROME
		StreamRipperForceFakeUserAgent = 1|0 forza l'uso dell'user agent fittizio
		StreamRipperUninterruptedStreamsLimit = 26214400 (25MB) limite in bytes per stream infiniti, 0=nessun limite
*/

// macro generali
#define MAX_HOST_COMBO_ENTRIES							128

// numero giorni per controllo ultima versione
#define DEFAULT_DAYS_FOR_LATESTVERSIONCHECK					15

// modalita' visualizzazione
#define DRAWMODE_ORIGINAL_SIZE							0
#define DRAWMODE_FIT_TO_SCREEN							1
#define DRAWMODE_STRETCH_IF_DOES_NOT_FIT					2
#define DRAWMODE_STRETCH_TO_SCREEN							3
#define DRAWMODE_STRETCH_TO_RECT							4
#define DRAWMODE_WINDOWS_NORMAL							5
#define DRAWMODE_WINDOWS_TILE								6
#define DRAWMODE_WINDOWS_STRETCH							7
#define DRAWMODE_TILE_STRETCH								8
#define DRAWMODE_TILE_FIT								9
#define DRAWMODE_OVERLAP_STRETCH							10
#define DRAWMODE_OVERLAP_FIT								11

#define WINDOWS_DRAWMODE_NORMAL							0
#define WINDOWS_DRAWMODE_TILE								1
#define WINDOWS_DRAWMODE_STRETCH							2

// effetti visualizzazione
#define DRAWEFFECT_NONE									0
#define DRAWEFFECT_GRAYSCALE								1
#define DRAWEFFECT_ROTATE90LEFT							2
#define DRAWEFFECT_ROTATE90RIGHT							3
#define DRAWEFFECT_ROTATE180								4
#define DRAWEFFECT_BLUR									5
#define DRAWEFFECT_BRIGHTNESS								6
#define DRAWEFFECT_CONTRAST								7
#define DRAWEFFECT_EDGEENHANCE							8
#define DRAWEFFECT_EMBOSS								9
#define DRAWEFFECT_FINDEDGE								10
#define DRAWEFFECT_INVERT								11
#define DRAWEFFECT_MIRRORH								12
#define DRAWEFFECT_MIRRORV								13
#define DRAWEFFECT_SHARPEN								14
#define DRAWEFFECT_RANDOM								15

// testo all'interno dell'immagine
#define DRAWTEXT_NONE									0
#define DRAWTEXT_DATETIME								1
#define DRAWTEXT_FILENAME								2
#define DRAWTEXT_PATHNAME								3
#define DRAWTEXT_QUOTES									4
#define DRAWTEXT_POS_LEFTUP								0
#define DRAWTEXT_POS_RIGHTDOWN							1
#define DRAWTEXT_POS_LEFTDOWN								2
#define DRAWTEXT_POS_RIGHTUP								3
#define DRAWTEXT_POS_H_MARGIN								1
#define DRAWTEXT_POS_V_MARGIN								1
#define DEFAULTFONT_DRAWTEXTMODE							0
#define DEFAULTFONT_DRAWTEXTPOSITION						0
#define DEFAULTFONT_TEXTCOLOR								16777215
#define DEFAULTFONT_BKCOLOR								0
#define DEFAULTFONT_BKCOLORTRANSPARENT						0
#define DEFAULTFONT_CF_iPointSize							100
#define DEFAULTFONT_CF_rgbColors							16777215
#define DEFAULTFONT_CF_nFontType							8452
#define DEFAULTFONT_LF_lfHeight							13
#define DEFAULTFONT_LF_lfWidth							0
#define DEFAULTFONT_LF_lfEscapement						0
#define DEFAULTFONT_LF_lfOrientation						0
#define DEFAULTFONT_LF_lfWeight							700
#define DEFAULTFONT_LF_lfItalic							0
#define DEFAULTFONT_LF_lfUnderline							0
#define DEFAULTFONT_LF_lfStrikeOut							0
#define DEFAULTFONT_LF_lfCharSet							0
#define DEFAULTFONT_LF_lfOutPrecision						3
#define DEFAULTFONT_LF_lfClipPrecision						2
#define DEFAULTFONT_LF_lfQuality							1
#define DEFAULTFONT_LF_lfPitchAndFamily						34
#define DEFAULTFONT_LF_lfFaceName							"Tahoma"

// griglia finestra principale
#define DEFAULTGRID_GRIDLINES								1
#define DEFAULTGRID_FOREGROUNDCOLOR						CTRLLISTEX_DEFAULT_FOREGROUND_COLOR
#define DEFAULTGRID_BACKGROUNDCOLOR						CTRLLISTEX_DEFAULT_BACKGROUND_COLOR

// icona per balloon
#define BALLOON_ICON_DEFAULT								0
#define BALLOON_ICON_CUSTOM								1

// modalita' audio popup
#define AUDIOPOPUP_NONE									0
#define AUDIOPOPUP_TASKBAR								1
#define AUDIOPOPUP_BALLOON								2

// modalita' pict popup
#define PICTPOPUP_NONE									0
#define PICTPOPUP_TASKBAR								1
#define PICTPOPUP_BALLOON								2

// modalita' crawler
// HEAD (HTTP)											0
#define CRAWLER_DOWNLOAD_MODE								1
#define CRAWLER_STREAMRIPPER_MODE							2

// accettazione domini
#define DOMAINACCEPTANCE_ASK								0
#define DOMAINACCEPTANCE_EXCLUDE_THIS						1
#define DOMAINACCEPTANCE_EXCLUDE_ALL_OUTSIDE					2
#define DOMAINACCEPTANCE_EXCLUDE_ALL_OUTSIDE_EXCEPT_PICTURES	7
#define DOMAINACCEPTANCE_EXCLUDE_ALL_OUTSIDE_EXCEPT_REGFILETYPES	8
#define DOMAINACCEPTANCE_EXCLUDE_ALL_FROM_THIS				3
#define DOMAINACCEPTANCE_FOLLOW_THIS						4
#define DOMAINACCEPTANCE_FOLLOW_ALL_OUTSIDE					5
#define DOMAINACCEPTANCE_FOLLOW_ALL_FROM_THIS				6

// accettazione tipi file
#define FILETYPESACCEPTANCE_LIST							0
#define FILETYPESACCEPTANCE_ASK							1

// accettazione directories padre
#define PARENTACCEPTANCE_ASK								0
#define PARENTACCEPTANCE_EXCLUDE_THIS						1
#define PARENTACCEPTANCE_EXCLUDE_ALL_OUTSIDE					2
#define PARENTACCEPTANCE_EXCLUDE_ALL_OUTSIDE_EXCEPT_PICTURES	7
#define PARENTACCEPTANCE_EXCLUDE_ALL_OUTSIDE_EXCEPT_REGFILETYPES	8
#define PARENTACCEPTANCE_EXCLUDE_ALL_FROM_THIS				3
#define PARENTACCEPTANCE_FOLLOW_THIS						4
#define PARENTACCEPTANCE_FOLLOW_ALL_OUTSIDE					5
#define PARENTACCEPTANCE_FOLLOW_ALL_FROM_THIS				6

// tipo dimensione (bytes, KB, MB)
#define SIZETYPE_BYTES									0
#define SIZETYPE_KBYTES									1
#define SIZETYPE_MBYTES									2

// ordinamento report
#define REPORT_BY_ID									0
#define REPORT_BY_PARENT_URL								1
#define REPORT_BY_URL									2
#define REPORT_BY_STAT									3

// caricamento playlist (.dpl)
#define PLAYLIST_LOADING_ADD								0
#define PLAYLIST_LOADING_PUT								1

// ordinamento playlist
#define RANDOMIZE_PLAYLIST_SELECTION						2
#define RANDOMIZE_PLAYLIST_REORDER							1
#define RANDOMIZE_PLAYLIST_NONE							0

// eliminazione files temporanei
#define TEMPORARY_FILES_CLEAR_AT_EXIT						0
#define TEMPORARY_FILES_CLEAR_AT_RUNTIME					1
#define TEMPORARY_FILES_DO_NOT_CLEAR						2

// chiavi registro
#define WALLPAPER_KEY						WALLPAPER_PROGRAM_NAME
#define WALLPAPER_DPL_KEY					WALLPAPER_PROGRAM_NAME" Desktop Picture List"
#define DEFAULT_WALLPAPER_KEY					DEFAULT_REG_KEY"\\"WALLPAPER_KEY

// playlist
#define DPL_SUFFIX		".dpl"							// estensione per il tipo file
#define DPL_EXTENSION	DPL_SUFFIX						// estensione per il tipo file
#define DPL_TYPENAME	"dplfile"							// nome del tipo
#define DPL_TYPEDESC	WALLPAPER_DPL_KEY					// descrizione del tipo
#define DPL_SHELLARGS	"/l%1"							// argomenti per la shell
#define DPL_CONTENTTYPE	"application/x-desktop-picture-list"	// tipo mime

// Install
#define DEFAULT_DIR							"C:\\"WALLPAPER_PROGRAM_NAME"\\"
#define DEFAULT_PROGRAM						DEFAULT_DIR""WALLPAPER_PROGRAM_NAME".exe"
#define DEFAULT_WEBSITE						WALLPAPER_WEB_SITE
#define DEFAULT_EMAILADDRESS					WALLPAPER_EMAIL_ADDRESS
#define DEFAULT_AUTHOREMAIL					WALLPAPER_AUTHOR_EMAIL
#define WALLPAPER_INSTALL_KEY					"Install"
#define WALLPAPER_DIR_KEY						"Directory"
#define WALLPAPER_PROGRAM_KEY						"Program"
#define WALLPAPER_WEBSITE_KEY						"WebSite"
#define WALLPAPER_EMAILADDRESS_KEY					"EmailAddress"
#define WALLPAPER_AUTHOREMAIL_KEY					"AuthorEmail"
#define WALLPAPER_CURRENTVERSION_KEY				"CurrentVersion"
#define WALLPAPER_LATESTVERSIONCHECK_KEY			"LatestVersionCheck"
#define WALLPAPER_LATESTVERSIONCHECKTYPE_KEY			"LatestVersionCheckType"

// directories
#define DEFAULT_BITMAPSDIR_NAME				"Bitmaps"
#define DEFAULT_BITMAPSDIR					DEFAULT_DIR""DEFAULT_BITMAPSDIR_NAME"\\"
#define DEFAULT_AUDIOPOPUPDIR_NAME				"AudioPopup"
#define DEFAULT_AUDIOPOPUPDIR					DEFAULT_DIR""DEFAULT_AUDIOPOPUPDIR_NAME"\\"
#define DEFAULT_PICTPOPUPDIR_NAME				"PicturePopup"
#define DEFAULT_PICTPOPUPDIR					DEFAULT_DIR""DEFAULT_PICTPOPUPDIR_NAME"\\"
#define DEFAULT_DATADIR_NAME					"Data"
#define DEFAULT_DATADIR						DEFAULT_DIR""DEFAULT_DATADIR_NAME"\\"
#define DEFAULT_DOWNLOADDIR_NAME				"Download"
#define DEFAULT_DOWNLOADDIR					DEFAULT_DIR""DEFAULT_DOWNLOADDIR_NAME"\\"
#define DEFAULT_HTTP_THUMBDIR_NAME				"Thumbnails"
#define DEFAULT_HTTPTHUMBDIR					DEFAULT_DIR""DEFAULT_HTTPDIR_NAME"\\"DEFAULT_HTTP_THUMBDIR_NAME
#define DEFAULT_REPORTSDIR_NAME				"Reports"
#define DEFAULT_REPORTSDIR					DEFAULT_DIR""DEFAULT_REPORTSDIR_NAME"\\"
#define DEFAULT_TEMPDIR_NAME					"Temp"
#define DEFAULT_TEMPDIR						DEFAULT_DIR""DEFAULT_TEMPDIR_NAME"\\"
#define DEFAULT_THUMBNAILSDIR_NAME				"Thumbnails"
#define DEFAULT_THUMBNAILSDIR					DEFAULT_DIR""DEFAULT_THUMBNAILSDIR_NAME"\\"
#define DEFAULT_COOKIES_FILE_NAME				"cookies.txt"
#define DEFAULT_COOKIES_FILE					DEFAULT_DOWNLOADDIR""DEFAULT_COOKIES_FILE_NAME
#define DEFAULT_THUMBNAILS_HTMLFILE_NAME		"thumbnails.html"
#define DEFAULT_THUMBNAILS_HTMLFILE			DEFAULT_THUMBNAILSDIR""DEFAULT_THUMBNAILS_HTMLFILE_NAME

// Options
#define DEFAULT_ALWAYSONTOP					0
#define DEFAULT_AUDIOPOPUP					AUDIOPOPUP_BALLOON
#define DEFAULT_AUTOMINIMIZE					0
#define DEFAULT_AUTORUN						1
#define DEFAULT_AUTOSAVE						1
#define DEFAULT_BALLOON_AUDIO_ICON_TYPE			BALLOON_ICON_DEFAULT
#define DEFAULT_BALLOON_PICT_ICON_TYPE			BALLOON_ICON_DEFAULT
#define DEFAULT_CLEAR_TEMPORARY_FILES			TEMPORARY_FILES_CLEAR_AT_RUNTIME
#define DEFAULT_COLUMNWIDTH_FILENAME			0
#define DEFAULT_COLUMNWIDTH_LOCATION			0
#define DEFAULT_COLUMNWIDTH_SIZE				0
#define DEFAULT_COLUMNWIDTH_TYPE				0
#define DEFAULT_CONFIRM_FILE_DELETE			1
#define DEFAULT_CONTEXT_MENU					1
#define DEFAULT_COOKIES_ALLOWED				1
#define DEFAULT_COOKIES_WARNINGONACCEPT			0
#define DEFAULT_COOKIES_WARNINGONSEND			0
#define DEFAULT_CRAWLER_ADDPICTURES			0
#define DEFAULT_CRAWLER_CONNECTIONRETRY			3
#define DEFAULT_CRAWLER_CONNECTIONTIMEOUT		60
#define DEFAULT_CRAWLER_DOMAINACCEPTANCE		DOMAINACCEPTANCE_EXCLUDE_ALL_OUTSIDE
#define DEFAULT_CRAWLER_DOMAINUSEALWAYS			0
#define DEFAULT_CRAWLER_DUMPDATABASE			0
#define DEFAULT_CRAWLER_ENABLECGI				1
#define DEFAULT_CRAWLER_ENABLEJAVASCRIPT		1
#define DEFAULT_CRAWLER_ENABLEROBOTSTXT			1
#define DEFAULT_CRAWLER_EXTRACTJAVASCRIPT		0
#define DEFAULT_CRAWLER_EXTRACTTEXT			0
#define DEFAULT_CRAWLER_FILETYPESACCEPTANCE		FILETYPESACCEPTANCE_LIST
#define DEFAULT_CRAWLER_GENERATEASCII			0
#define DEFAULT_CRAWLER_GENERATEREPORT			1
#define DEFAULT_CRAWLER_GENERATETHUMBNAILS		1
#define DEFAULT_CRAWLER_INCLUDESUBDOMAINS		1
#define DEFAULT_CRAWLER_MAXSIZE				0
#define DEFAULT_CRAWLER_MAXSIZETYPE			SIZETYPE_KBYTES
#define DEFAULT_CRAWLER_MAXTHREADS				32
#define DEFAULT_CRAWLER_MINSIZE				0
#define DEFAULT_CRAWLER_MINSIZETYPE			SIZETYPE_KBYTES
#define DEFAULT_CRAWLER_MODE					CRAWLER_DOWNLOAD_MODE
#define DEFAULT_CRAWLER_MULTITHREAD			0
#define DEFAULT_CRAWLER_ONLYCONTENT			0
#define DEFAULT_CRAWLER_OPENREPORT				1
#define DEFAULT_CRAWLER_PARENTACCEPTANCE		PARENTACCEPTANCE_EXCLUDE_ALL_OUTSIDE
#define DEFAULT_CRAWLER_PARENTURLUSEALWAYS		0
#define DEFAULT_CRAWLER_QUIETINSCRIPTMODE		1
#define DEFAULT_CRAWLER_REPORTORDER			REPORT_BY_PARENT_URL
#define DEFAULT_CRAWLER_SAVEINCLEXCLLIST		0
#define DEFAULT_CRAWLER_SKIPEXISTING			1
#define DEFAULT_CRAWLER_SKIPEXISTING_DONT		0
#define DEFAULT_CRAWLER_SKIPEXISTING_FORCE		0
#define DEFAULT_CRAWLER_THUMBNAIL_SIZE			8192
#define DEFAULT_CRAWLER_USEWINSOCKTIMEOUT		1
#define DEFAULT_CRAWLER_USEPROXY				0
#define DEFAULT_CRAWLER_WINSOCKDELAY			WSA_WINSOCK_DELAY
#define DEFAULT_CURRENTLIBRARY				IMAGE_DEFAULT_LIBRARY
#define DEFAULT_DELETE_FILES_TO_RECYCLEBIN		1
#define DEFAULT_DONOTASKMORE_TIMEOUT			10
#define DEFAULT_DRAWEFFECT					DRAWEFFECT_NONE
#define DEFAULT_DRAWEFFECTVALUE				0
#define DEFAULT_DRAWMODE						DRAWMODE_STRETCH_IF_DOES_NOT_FIT
#define DEFAULT_DRAWAREARATIO					30
#define DEFAULT_DRAWRECTX					150
#define DEFAULT_DRAWRECTY					150
#define DEFAULT_DRAWRECTX_MIN					5
#define DEFAULT_DRAWRECTY_MIN					5
#define DEFAULT_ED2KGOOGLE_QUERYSTRING			"http://www.google.com/search?q=%2B%22ed2k%3A%2F%2F%22%%QUERY%%&hl=en&safe=off&tbo=1&output=search&source=lnt&tbs=ww:1"
#define DEFAULT_ED2KFOOFIND_QUERYSTRING			"http://foofind.com/en/search/?q=%%QUERY%%&submit=Search&src=eg"
#define DEFAULT_ED2KPIRATEBAY_QUERYSTRING		"http://thepiratebay.org/search/%%QUERY%%"
#define DEFAULT_YOUTUBE_QUERYSTRING			"http://www.youtube.com/results?search_query=%%QUERY%%"
/*
	WinAmp/2.x
	WinAmp/3.0
	Nullsoft Winamp3 version 3.0d build 488
	WinampMPEG/5.54
---> Winamp/5.54 <---
	iTunes/4.7 (Macintosh; N; PPC)
	RMA/1.0 (compatible; RealMedia)

	http://forums.winamp.com/showthread.php?t=280105
	------------------------------------------------
	A: Since one of the internet radio stations I listen to frequently is now blocking Winamp 
	(forums.winamp.com/showthread.php?postid=2260515#post2260515) for no good reason, I have the following question:
	Is it possible to change the 'user agent' information Winamp sends out when connecting to internet radio streams? 
	------------------------------------------------
	R: Hi, it is possible to change user agent of winamp, using third party tool, such as internet junkbuster, which will act as 
	a proxy between your browser/player/whatever and server.
	http://www.junkbusters.com/ijb20.zip
	1. Download and unpack the zip somewhere (i.e. c:\program files)
	2. edit file junkbstr.ini and add the line:
	user-agent NSPlayer/10.0.0.3802
	3. run junkbstr.exe and minimize it
	4. setup your winamp to use proxy 127.0.0.1:8000
	You're done. If everything works, you can add following line to junkbstr.ini (it will hide the ugly black window after next 
	start):
	hide-console
	To test, if junkbuster cloaks your user-agent properly, you can set your browser proxy to 127.0.0.1:8000 and go to some some 
	test page like mine: http://stopar.borec*****envdump.php and you should see the faked useg agent
	------------------------------------------------
	R: I have the same problem and i don't really want to install an unnecessary proxy server.
	So, i just changed the User-Agent in in_mp3.dll. Just open the file in a Hex Editor (for example XVI32), search for the 
	text "WinampMPEG" (occurs twice) and change it in something else of the same length, for example 'Blablablaa'.
	Works for me!
*/
#define DEFAULT_FAKE_AUDIOUSERAGENT			"Winamp/3.0"
#define DEFAULT_SHOUTCAST_WEBPAGE				"http://www.shoutcast.com/"
#define DEFAULT_INFAMOUS_SENTENCES				0
#define DEFAULT_IBROWSER						"IEXPLORE"
#define DEFAULT_LAYERED						0
#define DEFAULT_LAYERED_VALUE					LWA_ALPHA_DEFAULT
#define DEFAULT_LAST_ADDDIR					".\\"
#define DEFAULT_LAST_ADDFILE					".\\"
#define DEFAULT_LAST_ADDURL					"http://"LOCAL_HOST"/"
#define DEFAULT_LAST_LOADLIST					".\\"
#define DEFAULT_LAST_MOVETO					".\\"
#define DEFAULT_LAST_PLAYEDSONG				"?"
#define DEFAULT_LAST_SAVEAS					".\\"
#define DEFAULT_LAST_SAVELIST					".\\"
#define DEFAULT_LAST_SEARCHSTRING				""
#define DEFAULT_LAST_THUMBFROM				".\\"
#define DEFAULT_LAST_THUMBINTO				".\\"
#define DEFAULT_LAST_UNCOMPRESS				".\\"
#define DEFAULT_LISTNAME						"reg"
#define DEFAULT_LOADLISTFROM					0
#define DEFAULT_MINIMIZE_ON_CLOSE				0
#define DEFAULT_HTML_FILENAME					INDEX_HTML
#define DEFAULT_MP3_FILENAME					DEFAULT_MP3_NAME
#define DEFAULT_MP3TAG_SAVEONSKIP				1
#define DEFAULT_NERVOUS_TITLE					0
#define DEFAULT_ONE_CLICK_CHANGE				1
#define DEFAULT_PICTPOPUP					PICTPOPUP_BALLOON
#define DEFAULT_PICTUREPREVIEW				0
#define DEFAULT_PRIORITY_PROCESS				NORMAL_PRIORITY_CLASS
#define DEFAULT_PRIORITY_THREAD				THREAD_PRIORITY_NORMAL
#define DEFAULT_PROXY_ADDRESS					LOCAL_HOST_NAME
#define DEFAULT_PROXY_PORTNUMBER				8000
#define DEFAULT_PROXY_USER					""
#define DEFAULT_PROXY_PASSWORD				""
#define DEFAULT_PROXY_EXCLUDEDOMAINS			LOCAL_HOST
#define DEFAULT_PLAYLIST_LOADING				PLAYLIST_LOADING_PUT
#define DEFAULT_RANDOMIZE_PLAYLIST				RANDOMIZE_PLAYLIST_NONE
#define DEFAULT_RECURSE_SUBDIR				1
#define DEFAULT_SPLASHSCREEN					1
#define DEFAULT_STATE						1
#define DEFAULT_STREAMRIPPER_FORCEFAKEUSERAGENT	0
#define DEFAULT_STREAMRIPPER_USTREAMSLIMIT		26214400 // 25MB
#define DEFAULT_THUMBNAILS_FORMAT				"*"
#define DEFAULT_THUMBNAILS_GENERATEFROM			0
#define DEFAULT_THUMBNAILS_GENERATEINTO			0
#define DEFAULT_THUMBNAILS_GENERATETABLE		1
#define DEFAULT_THUMBNAILS_HTMLTITLE			"Untitled"
#define DEFAULT_THUMBNAILS_IMAGEURL			""
#define DEFAULT_THUMBNAILS_NAME				"*"
#define DEFAULT_THUMBNAILS_NAMING				0
#define DEFAULT_THUMBNAILS_NUMBER				"0"
#define DEFAULT_THUMBNAILS_OPENREPORT			1
#define DEFAULT_THUMBNAILS_TABLEBORDER			1
#define DEFAULT_THUMBNAILS_TABLECOLS			5
#define DEFAULT_THUMBNAILS_TABLEPADDING			5
#define DEFAULT_THUMBNAILS_TABLESPACING			1
#define DEFAULT_THUMBNAILS_TABLETITLE			"Untitled"
#define DEFAULT_THUMBNAILS_TABLEWIDTH			100
#define DEFAULT_THUMBNAILS_THUMBURL			""
#define DEFAULT_THUMBNAILS_X					50
#define DEFAULT_THUMBNAILS_Y					50
#define DEFAULT_TIMEOUT						5
#define DEFAULT_TRAYICON						0
#define DEFAULT_WINDOW_POS_LEFT				0
#define DEFAULT_WINDOW_POS_TOP				0
#define DEFAULT_WINDOW_POS_RIGHT				800
#define DEFAULT_WINDOW_POS_BOTTOM				495

// Options
#define WALLPAPER_OPTIONS_KEY					"Options"
#define WALLPAPER_ALWAYSONTOP_KEY					"AlwaysOnTop"
#define WALLPAPER_AUDIOPOPUP_KEY					"AudioPopup"
#define WALLPAPER_AUDIOPOPUPDIR_KEY				"AudioPopupDirectory"
#define WALLPAPER_AUTORUN_KEY						"AutoRun"
#define WALLPAPER_BALLOON_AUDIO_ICON_TYPE_KEY		"BalloonAudioType"
#define WALLPAPER_BALLOON_AUDIO_ICON_DIR_KEY			"BalloonAudioDir"
#define WALLPAPER_BALLOON_PICT_ICON_TYPE_KEY			"BalloonPictType"
#define WALLPAPER_BALLOON_PICT_ICON_DIR_KEY			"BalloonPictDir"
#define WALLPAPER_BITMAPSDIR_KEY					"BitmapsDirectory"
#define WALLPAPER_CAPTUREDIR_KEY					"CaptureDirectory"
#define WALLPAPER_CLEAR_TEMPORARY_FILES_KEY			"ClearTemporaryFiles"
#define WALLPAPER_COLUMNWIDTH_FILENAME_KEY			"ColumnWidthFilename"
#define WALLPAPER_COLUMNWIDTH_LOCATION_KEY			"ColumnWidthLocation"
#define WALLPAPER_COLUMNWIDTH_SIZE_KEY				"ColumnWidthSize"
#define WALLPAPER_COLUMNWIDTH_TYPE_KEY				"ColumnWidthType"
#define WALLPAPER_CONFIRM_FILE_DELETE_KEY			"ConfirmFileDelete"
#define WALLPAPER_CONTEXT_MENU_KEY					"ContextMenu"
#define WALLPAPER_COOKIES_FILE_KEY					"CookiesFile"
#define WALLPAPER_COOKIES_ALLOWED_KEY				"CookiesAllowed"
#define WALLPAPER_COOKIES_WARNINGONACCEPT_KEY		"CookiesWarningOnAccept"
#define WALLPAPER_COOKIES_WARNINGONSEND_KEY			"CookiesWarningOnSend"
#define WALLPAPER_CRAWLER_ADDPICTURES_KEY			"CrawlerAddPictures"
#define WALLPAPER_CRAWLER_CONNECTIONRETRY_KEY		"CrawlerConnectionRetry"
#define WALLPAPER_CRAWLER_CONNECTIONTIMEOUT_KEY		"CrawlerConnectionTimeout"
#define WALLPAPER_CRAWLER_DEFAULTHTMLFILENAME_KEY		"CrawlerDefaultHtmlFileName"
#define WALLPAPER_CRAWLER_DEFAULTMP3FILENAME_KEY		"CrawlerDefaultMp3FileName"
#define WALLPAPER_CRAWLER_DOMAINACCEPTANCE_KEY		"CrawlerDomainAcceptance"
#define WALLPAPER_CRAWLER_DOMAINUSEALWAYS_KEY		"CrawlerDomainUseAlways"
#define WALLPAPER_CRAWLER_DUMPDATABASE_KEY			"CrawlerDumpDatabase"
#define WALLPAPER_CRAWLER_ENABLECGI_KEY				"CrawlerEnableCGI"
#define WALLPAPER_CRAWLER_ENABLEJAVASCRIPT_KEY		"CrawlerEnableJavascript"
#define WALLPAPER_CRAWLER_ENABLEROBOTSTXT_KEY		"CrawlerEnableRobotsTxt"
#define WALLPAPER_CRAWLER_EXTRACTJAVASCRIPT_KEY		"CrawlerExtractJavascript"
#define WALLPAPER_CRAWLER_EXTRACTTEXT_KEY			"CrawlerExtractText"
#define WALLPAPER_CRAWLER_FILETYPESACCEPTANCE_KEY		"CrawlerFileTypesAcceptance"
#define WALLPAPER_CRAWLER_GENERATEASCII_KEY			"CrawlerGenerateAscii"
#define WALLPAPER_CRAWLER_GENERATEREPORT_KEY			"CrawlerGenerateReport"
#define WALLPAPER_CRAWLER_GENERATETHUMBNAILS_KEY		"CrawlerGenerateThumbnails"
#define WALLPAPER_CRAWLER_INCLUDESUBDOMAINS_KEY		"CrawlerIncludeSubDomains"
#define WALLPAPER_CRAWLER_MAXSIZE_KEY				"CrawlerMaxSize"
#define WALLPAPER_CRAWLER_MAXSIZETYPE_KEY			"CrawlerMaxSizeType"
#define WALLPAPER_CRAWLER_MAXTHREADS_KEY			"CrawlerMaxThreads"
#define WALLPAPER_CRAWLER_MINSIZE_KEY				"CrawlerMinSize"
#define WALLPAPER_CRAWLER_MINSIZETYPE_KEY			"CrawlerMinSizeType"
#define WALLPAPER_CRAWLER_MULTITHREAD_KEY			"CrawlerMultithread"
#define WALLPAPER_CRAWLER_ONLYCONTENT_KEY			"CrawlerOnlyContent"
#define WALLPAPER_CRAWLER_OPENREPORT_KEY			"CrawlerOpenReport"
#define WALLPAPER_CRAWLER_PARENTACCEPTANCE_KEY		"CrawlerParentAcceptance"
#define WALLPAPER_CRAWLER_PARENTURLUSEALWAYS_KEY		"CrawlerParentUrlUseAlways"
#define WALLPAPER_CRAWLER_QUIETINSCRIPTMODE_KEY		"CrawlerQuietInScriptMode"
#define WALLPAPER_CRAWLER_REPORTORDER_KEY			"CrawlerReportOrder"
#define WALLPAPER_CRAWLER_SAVEINCLEXCLLIST_KEY		"CrawlerSaveIncludeExcludeList"
#define WALLPAPER_CRAWLER_SKIPEXISTING_KEY			"CrawlerSkipExisting"
#define WALLPAPER_CRAWLER_SKIPEXISTING_DONT_KEY		"CrawlerSkipExistingDont"
#define WALLPAPER_CRAWLER_SKIPEXISTING_FORCE_KEY		"CrawlerSkipExistingForce"
#define WALLPAPER_CRAWLER_THUMBNAIL_SIZE_KEY			"CrawlerThumbnailSize"
#define WALLPAPER_CRAWLER_USEWINSOCKTIMEOUT_KEY		"CrawlerUseWinsockTimeout"
#define WALLPAPER_CRAWLER_USEPROXY_KEY				"CrawlerUseProxy"
#define WALLPAPER_CRAWLER_WINSOCKDELAY_KEY			"CrawlerWinsockDelay"
#define WALLPAPER_CURRENTLIBRARY_KEY				"CurrentLibrary"
#define WALLPAPER_DATADIR_KEY						"DataDirectory"
#define WALLPAPER_DELETE_FILES_TO_RECYCLEBIN_KEY		"DeleteFilesToRecycleBin"
#define WALLPAPER_DOWNLOADDIR_KEY					"DownloadDirectory"
#define WALLPAPER_DRAWEFFECT_KEY					"DrawEffect"
#define WALLPAPER_DRAWEFFECTVALUE_KEY				"DrawEffectValue"
#define WALLPAPER_DRAWMODE_KEY					"DrawMode"
#define WALLPAPER_DRAWAREARATIO_KEY				"DrawAreaRatio"
#define WALLPAPER_DRAWRECTX_KEY					"DrawRectX"
#define WALLPAPER_DRAWRECTY_KEY					"DrawRectY"
#define WALLPAPER_ED2KGOOGLE_QUERYSTRING_KEY			"ed2kGoogle"
#define WALLPAPER_ED2KFOOFIND_QUERYSTRING_KEY		"ed2kFooFind"
#define WALLPAPER_ED2KPIRATEBAY_QUERYSTRING_KEY		"ed2kPirateBay"
#define WALLPAPER_YOUTUBE_QUERYSTRING_KEY			"YouTubeQuery"
#define WALLPAPER_FAKE_AUDIOUSERAGENT_KEY			"FakeAudioUserAgent"
#define WALLPAPER_FONT_DRAWTEXTMODE_KEY				"FontDrawTextMode"
#define WALLPAPER_FONT_DRAWTEXTPOSITION_KEY			"FontDrawTextPosition"
#define WALLPAPER_FONT_TEXTCOLOR					"FontTextColor"
#define WALLPAPER_FONT_BKCOLOR					"FontBkColor"
#define WALLPAPER_FONT_BKCOLORTRANSPARENT			"FontBkColorTransparent"
#define WALLPAPER_FONT_CF_iPointSize				"Font_CF_iPointSize"
#define WALLPAPER_FONT_CF_rgbColors				"Font_CF_rgbColors"
#define WALLPAPER_FONT_CF_nFontType				"Font_CF_nFontType"
#define WALLPAPER_FONT_LF_lfHeight					"Font_LF_lfHeight"
#define WALLPAPER_FONT_LF_lfWidth					"Font_LF_lfWidth"
#define WALLPAPER_FONT_LF_lfEscapement				"Font_LF_lfEscapement"
#define WALLPAPER_FONT_LF_lfOrientation				"Font_LF_lfOrientation"
#define WALLPAPER_FONT_LF_lfWeight					"Font_LF_lfWeight"
#define WALLPAPER_FONT_LF_lfItalic					"Font_LF_lfItalic"
#define WALLPAPER_FONT_LF_lfUnderline				"Font_LF_lfUnderline"
#define WALLPAPER_FONT_LF_lfStrikeOut				"Font_LF_lfStrikeOut"
#define WALLPAPER_FONT_LF_lfCharSet				"Font_LF_lfCharSet"
#define WALLPAPER_FONT_LF_lfOutPrecision			"Font_LF_lfOutPrecision"
#define WALLPAPER_FONT_LF_lfClipPrecision			"Font_LF_lfClipPrecision"
#define WALLPAPER_FONT_LF_lfQuality				"Font_LF_lfQuality"
#define WALLPAPER_FONT_LF_lfPitchAndFamily			"Font_LF_lfPitchAndFamily"
#define WALLPAPER_FONT_LF_lfFaceName				"Font_LF_lfFaceName"
#define WALLPAPER_GRID_GRIDLINES_KEY				"Gridlines"
#define WALLPAPER_GRID_FOREGROUNDCOLOR_KEY			"GridFgColor"
#define WALLPAPER_GRID_BACKGROUNDCOLOR_KEY			"GridBkColor"
#define WALLPAPER_INFAMOUS_SENTENCES_KEY			"InfamousSentences"
#define WALLPAPER_LAYERED_KEY						"Layered"
#define WALLPAPER_LAYERED_VALUE_KEY				"LayeredValue"
#define WALLPAPER_LAST_ADDDIR_KEY					"LastAddDir"
#define WALLPAPER_LAST_ADDFILE_KEY					"LastAddFile"
#define WALLPAPER_LAST_ADDURL_KEY					"LastAddUrl"
#define WALLPAPER_LAST_LOADLIST_KEY				"LastLoadList"
#define WALLPAPER_LAST_MOVETO_KEY					"LastMoveTo"
#define WALLPAPER_LAST_PLAYEDSONG_KEY				"LastPlayedSong"
#define WALLPAPER_LAST_SAVEAS_KEY					"LastSaveAs"
#define WALLPAPER_LAST_SAVELIST_KEY				"LastSaveList"
#define WALLPAPER_LAST_SEARCHSTRING_KEY				"LastSearchString"
#define WALLPAPER_LAST_THUMBFROM_KEY				"LastThumbnailsFrom"
#define WALLPAPER_LAST_THUMBINTO_KEY				"LastThumbnailsInto"
#define WALLPAPER_LAST_UNCOMPRESS_KEY				"LastUncompress"
#define WALLPAPER_LISTNAME_KEY					"ListName"
#define WALLPAPER_LOADLISTFROM_KEY					"LoadListFrom"
#define WALLPAPER_MINIMIZE_ON_CLOSE_KEY				"MinimizeOnClose"
#define WALLPAPER_MP3TAG_SAVEONSKIP_KEY				"M3TagSaveOnSkip"
#define WALLPAPER_NERVOUS_TITLE_KEY				"NervousTitle"
#define WALLPAPER_ONE_CLICK_CHANGE_KEY				"OneClickChange"
#define WALLPAPER_PICTPOPUP_KEY					"PictPopup"
#define WALLPAPER_PICTPOPUPDIR_KEY					"PictPopupDirectory"
#define WALLPAPER_PICTUREPREVIEW_KEY				"PicturePreview"
#define WALLPAPER_PRIORITY_PROCESS_KEY				"PriorityProcess"
#define WALLPAPER_PRIORITY_THREAD_KEY				"PriorityThread"
#define WALLPAPER_PROXY_ADDRESS_KEY				"ProxyAddress"
#define WALLPAPER_PROXY_EXCLUDEDOMAINS_KEY			"ProxyExcludeDomains"
#define WALLPAPER_PROXY_PASSWORD_KEY				"ProxyPassword"
#define WALLPAPER_PROXY_PORTNUMBER_KEY				"ProxyPortNumber"
#define WALLPAPER_PROXY_USER_KEY					"ProxyUser"
#define WALLPAPER_PLAYLIST_LOADING_KEY				"PlaylistLoading"
#define WALLPAPER_PREFERRED_INTERNET_BROWSER_KEY		"PreferredInternetBrowser"
#define WALLPAPER_RANDOMIZE_PLAYLIST_KEY			"RandomizePlaylist"
#define WALLPAPER_RECURSE_SUBDIR_KEY				"RecurseSubdir"
#define WALLPAPER_REPORTSDIR_KEY					"ReportsDirectory"
#define WALLPAPER_SPLASHSCREEN_KEY					"SplashScreen"
#define WALLPAPER_STATE_KEY						"State"
#define WALLPAPER_STREAMRIPPER_FORCEFAKEUSERAGENT_KEY	"StreamRipperForceFakeUserAgent"
#define WALLPAPER_STREAMRIPPER_USTREAMSLIMIT_KEY		"StreamRipperUninterruptedStreamsLimit"
#define WALLPAPER_TEMPDIR_KEY						"TempDirectory"
#define WALLPAPER_THUMBNAILSDIR_KEY				"ThumbnailsDirectory"
#define WALLPAPER_THUMBNAILS_FORMAT_KEY				"ThumbnailsFormat"
#define WALLPAPER_THUMBNAILS_GENERATEFROM_KEY		"ThumbnailsGenerateFrom"
#define WALLPAPER_THUMBNAILS_GENERATEINTO_KEY		"ThumbnailsGenerateInto"
#define WALLPAPER_THUMBNAILS_GENERATETABLE_KEY		"ThumbnailsGenerateTable"
#define WALLPAPER_THUMBNAILS_HTMLFILE_KEY			"ThumbnailsHtmlFile"
#define WALLPAPER_THUMBNAILS_HTMLTITLE_KEY			"ThumbnailsHtmlTitle"
#define WALLPAPER_THUMBNAILS_IMAGEURL_KEY			"ThumbnailsImageUrl"
#define WALLPAPER_THUMBNAILS_NAME_KEY				"ThumbnailsName"
#define WALLPAPER_THUMBNAILS_NAMING_KEY				"ThumbnailsNamingRule"
#define WALLPAPER_THUMBNAILS_NUMBER_KEY				"ThumbnailsNumber"
#define WALLPAPER_THUMBNAILS_OPENREPORT_KEY			"ThumbnailsOpenReport"
#define WALLPAPER_THUMBNAILS_TABLEBORDER_KEY			"ThumbnailsTableBorder"
#define WALLPAPER_THUMBNAILS_TABLECOLS_KEY			"ThumbnailsTableCols"
#define WALLPAPER_THUMBNAILS_TABLEPADDING_KEY		"ThumbnailsTablePadding"
#define WALLPAPER_THUMBNAILS_TABLESPACING_KEY		"ThumbnailsTableSpacing"
#define WALLPAPER_THUMBNAILS_TABLETITLE_KEY			"ThumbnailsTableTitle"
#define WALLPAPER_THUMBNAILS_TABLEWIDTH_KEY			"ThumbnailsTableWidth"
#define WALLPAPER_THUMBNAILS_THUMBURL_KEY			"ThumbnailsThumbUrl"
#define WALLPAPER_THUMBNAILS_X_KEY					"ThumbnailsX"
#define WALLPAPER_THUMBNAILS_Y_KEY					"ThumbnailsY"
#define WALLPAPER_TIMEOUT_KEY						"Timeout"
#define WALLPAPER_TRAYICON_KEY					"TrayIcon"
#define WALLPAPER_WINDOW_POS_LEFT					"WindowPosLeft"
#define WALLPAPER_WINDOW_POS_TOP					"WindowPosTop"
#define WALLPAPER_WINDOW_POS_RIGHT					"WindowPosRight"
#define WALLPAPER_WINDOW_POS_BOTTOM				"WindowPosBottom"

// DoNotAskMore
// ..._KEY: boolean per flag abilitato
// ...VALUE_KEY: valore: 0=No, 1=Yes, 2=Cancel
#define WALLPAPER_DONOTASKMORE_KEY					"DoNotAskMore"
#define WALLPAPER_DONOTASKMORE_CLEANDPLFILE_KEY			"CleanDplFile"
#define WALLPAPER_DONOTASKMORE_CLEANDPLFILE_VALUE_KEY		"CleanDplFileValue"
#define WALLPAPER_DONOTASKMORE_CRAWLERLISTS_KEY			"CrawlerLists"
#define WALLPAPER_DONOTASKMORE_CRAWLERLISTS_VALUE_KEY		"CrawlerListsValue"
#define WALLPAPER_DONOTASKMORE_CRAWLERMODE_KEY			"CrawlerMode"
#define WALLPAPER_DONOTASKMORE_CRAWLERMODE_VALUE_KEY		"CrawlerModeValue"
#define WALLPAPER_DONOTASKMORE_CRAWLERMT_KEY				"CrawlerMT"
#define WALLPAPER_DONOTASKMORE_CRAWLERMT_VALUE_KEY		"CrawlerMTValue"
#define WALLPAPER_DONOTASKMORE_FEATURES_KEY				"Features"
#define WALLPAPER_DONOTASKMORE_FEATURES_VALUE_KEY			"FeaturesValue"
#define WALLPAPER_DONOTASKMORE_LATESTVERSION_KEY			"LatestVersion"
#define WALLPAPER_DONOTASKMORE_LATESTVERSION_VALUE_KEY		"LatestVersionValue"
#define WALLPAPER_DONOTASKMORE_LOADBROWSER_KEY			"LoadBrowser"
#define WALLPAPER_DONOTASKMORE_LOADBROWSER_VALUE_KEY		"LoadBrowserValue"
#define WALLPAPER_DONOTASKMORE_PLAYRIPPEDSONGS_KEY		"PlayRippedSongs"
#define WALLPAPER_DONOTASKMORE_PLAYRIPPEDSONGS_VALUE_KEY	"PlayRippedSongsValue"
#define WALLPAPER_DONOTASKMORE_REMOVEFROMARCHIVE_KEY		"RemoveFromArchive"
#define WALLPAPER_DONOTASKMORE_REMOVEFROMARCHIVE_VALUE_KEY	"RemoveFromArchiveValue"
#define WALLPAPER_DONOTASKMORE_RESETDPLFILE_KEY			"ResetDplFile"
#define WALLPAPER_DONOTASKMORE_RESETDPLFILE_VALUE_KEY		"ResetDplFileValue"
#define WALLPAPER_DONOTASKMORE_SCREENSAVER_KEY			"ScreenSaver"
#define WALLPAPER_DONOTASKMORE_SCREENSAVER_VALUE_KEY		"ScreenSaverValue"
#define WALLPAPER_DONOTASKMORE_TOOLTIP_MAIN_KEY			"TooltipMain"
#define WALLPAPER_DONOTASKMORE_TOOLTIP_MP3TAG_KEY			"TooltipMP3"
#define WALLPAPER_DONOTASKMORE_TOOLTIP_CRAWLER_KEY		"TooltipCrawl"
#define WALLPAPER_DONOTASKMORE_TRAYICON_KEY				"TrayIcon"
#define WALLPAPER_DONOTASKMORE_TRAYICON_VALUE_KEY			"TrayIconValue"
#define WALLPAPER_DONOTASKMORE_UNCOMPRESS_PICT_KEY		"UnCompPict"
#define WALLPAPER_DONOTASKMORE_UNCOMPRESS_PICT_VALUE_KEY	"UnCompPictValue"

// Pictures
#define DEFAULT_CURRENTPICT				0
#define DEFAULT_TOTPICT					0
#define WALLPAPER_PICTURES_KEY			"Pictures"
#define WALLPAPER_CURRENTPICT_KEY				"Current"
#define WALLPAPER_PICTURE_KEY					"File"
#define WALLPAPER_TOTPICT_KEY					"Total"

// Host Names
#define WALLPAPER_HOSTNAMES_KEY			"HostNames"
#define WALLPAPER_HOST_KEY					"Host"

#define FILETYPE_INCLUDE_LIST				0
#define PARENT_INCLUDE_LIST				1
#define PARENT_EXCLUDE_LIST				2
#define DOMAIN_INCLUDE_LIST				3
#define DOMAIN_EXCLUDE_LIST				4
#define WILDCARDS_EXCLUDE_LIST			5

// File Type
#define WALLPAPER_FILETYPELIST_KEY			"FileTypeList"
#define WALLPAPER_FILETYPE_KEY				"Type"

// Parent Include List
#define WALLPAPER_PARENTINCLUDELIST_KEY		"ParentIncludeList"
#define WALLPAPER_PARENTINCLUDE_KEY			"Include"

// Parent Exclude List
#define WALLPAPER_PARENTEXCLUDELIST_KEY		"ParentExcludeList"
#define WALLPAPER_PARENTEXCLUDE_KEY			"Exclude"

// Domain Include List
#define WALLPAPER_DOMAININCLUDELIST_KEY		"DomainIncludeList"
#define WALLPAPER_DOMAININCLUDE_KEY			"Include"

// Domain Exclude List
#define WALLPAPER_DOMAINEXCLUDELIST_KEY		"DomainExcludeList"
#define WALLPAPER_DOMAINEXCLUDE_KEY			"Exclude"

// Wildcards Exclude List
#define WALLPAPER_WILDCARDSEXCLUDELIST_KEY	"WildcardsExcludeList"
#define WALLPAPER_WILDCARDSEXCLUDE_KEY			"Exclude"

/*
	CWallPaperConfig
*/
class CWallPaperConfig : public CConfig
{
public:
	CWallPaperConfig();
	~CWallPaperConfig() {}

	BOOL		Export		(LPCSTR lpcszFileName);
	BOOL		Import		(LPCSTR lpcszFileName);

	// salva la lista nel registro
	void		Save			(void);

	// stronca
	void		DeleteAll		(void);
		
	// salva la chiave della lista nel registro
	void		SaveKey		(LPCSTR lpcszSectionName,LPCSTR lpcszKeyName);
	void		SaveNumber	(LPCSTR lpcszSectionName,LPCSTR lpcszKeyName) {SaveKey(lpcszSectionName,lpcszKeyName);}
	void		SaveString	(LPCSTR lpcszSectionName,LPCSTR lpcszKeyName) {SaveKey(lpcszSectionName,lpcszKeyName);}
	
	// salva la sezione della lista nel registro
	void		SaveSection	(LPCSTR lpcszSectionName);
	
	// carica la lista dal registro
	void		Load			(LPCSTR lpcszSectionName = NULL,BOOL bLoadDefaults = TRUE);

	// ricarica la sezione della lista dal registro
	void		ReloadSection	(LPCSTR lpcszSectionName);

	// elimina la sezione della lista dal registro
	void		DeleteSection	(LPCSTR lpcszSectionName);

private:
	// carica il valore dal registro (la chiave deve esistere)
	// (utilizzata durante il caricamento della lista)
	BOOL		ReadNumber	(LPCSTR,DWORD&);
	BOOL		ReadString	(LPCSTR,LPSTR,int);

	// carica il valore dal registro (crea la chiave col valore di default se non esiste)
	// (utilizzata durante il caricamento della lista)
	void		LoadNumber	(LPCSTR,DWORD&,DWORD,BOOL);
	void		LoadString	(LPCSTR,LPSTR,int,LPCSTR,BOOL);
};

#endif // _WALLPAPERCONFIG_H
