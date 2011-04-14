/*
	WallPaperConfig.cpp
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
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include <stdio.h>
#include "strings.h"
#include "window.h"
#include "CConfig.h"
#include "CImageFactory.h"
#include "CDialogEx.h"
#include "CRegKey.h"
#include "WallPaperConfig.h"

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

/*
	CWallPaperConfig()
*/
CWallPaperConfig::CWallPaperConfig()
{
	CWallPaperConfig::Load(NULL,FALSE);
}

/*
	Export()
*/
BOOL CWallPaperConfig::Export(LPCSTR lpcszFileName)
{
	// esporta la configurazione corrente nel file specificato
	return(CConfig::Export(WALLPAPER_KEY,lpcszFileName));
}

/*
	Import()
*/
BOOL CWallPaperConfig::Import(LPCSTR lpcszFileName)
{
	// importa la configurazione dal file specificato
	return(CConfig::Import(WALLPAPER_KEY,lpcszFileName));
}

/*
	Save()
*/
void CWallPaperConfig::Save(void)
{
	// salva la lista nel registro
	CConfig::Save(WALLPAPER_KEY);
}

/*
	DeleteAll()
*/
void CWallPaperConfig::DeleteAll(void)
{
	// elimina tutti i valori presenti
	CConfig::DeleteAll(WALLPAPER_KEY);
}

/*
	SaveKey()
*/
void CWallPaperConfig::SaveKey(LPCSTR lpcszSectionName,LPCSTR lpcszKeyName)
{
	// salva la chiave della lista nel registro
	CConfig::SaveKey(WALLPAPER_KEY,lpcszSectionName,lpcszKeyName);
}

/*
	SaveSection()
*/
void CWallPaperConfig::SaveSection(LPCSTR lpcszSectionName)
{
	// salva la sezione della lista nel registro
	CConfig::SaveSection(WALLPAPER_KEY,lpcszSectionName);
}

/*
	ReloadSection()
*/
void CWallPaperConfig::ReloadSection(LPCSTR lpcszSectionName)
{
	// ricarica la sezione della lista dal registro
	CConfig::DeleteSection(WALLPAPER_KEY,lpcszSectionName,FALSE);
	CWallPaperConfig::Load(lpcszSectionName,FALSE);
	CConfig::SetModified(TRUE);
}

/*
	DeleteSection()
*/
void CWallPaperConfig::DeleteSection(LPCSTR lpcszSectionName)
{
	// elimina la sezione della lista dal registro
	CConfig::DeleteSection(WALLPAPER_KEY,lpcszSectionName,FALSE);
	CConfig::SetModified(TRUE);
}

/*
	Load()
*/
void CWallPaperConfig::Load(LPCSTR lpcszSectionName/*=NULL*/,BOOL bLoadDefaults/*=TRUE*/)
{
	// carica nella lista i valori di configurazione presenti nel registro
	BOOL bLoadSection;
	LONG lRet = 0;
	DWORD nValue = 0L;
	char szValue[REGKEY_MAX_KEY_VALUE+1] = {0};

	CConfig::m_pRegistry->Attach(HKEY_CURRENT_USER);
	
	/*
		WallPaper
	*/
	if((lRet = CConfig::m_pRegistry->Open(HKEY_CURRENT_USER,DEFAULT_WALLPAPER_KEY))!=ERROR_SUCCESS)
		lRet = CConfig::m_pRegistry->Create(HKEY_CURRENT_USER,DEFAULT_WALLPAPER_KEY);
	if(lRet==ERROR_SUCCESS)
		CConfig::m_pRegistry->Close();

	/*
		WallPaper\\Install
	*/
	bLoadSection = lpcszSectionName ? (strcmp(lpcszSectionName,WALLPAPER_INSTALL_KEY)==0 ? TRUE : FALSE) : TRUE;
	if(bLoadSection)
	{
		if((lRet = CConfig::m_pRegistry->Open(HKEY_CURRENT_USER,DEFAULT_WALLPAPER_KEY"\\"WALLPAPER_INSTALL_KEY))!=ERROR_SUCCESS)
			lRet = CConfig::m_pRegistry->Create(HKEY_CURRENT_USER,DEFAULT_WALLPAPER_KEY"\\"WALLPAPER_INSTALL_KEY);
		if(lRet==ERROR_SUCCESS)
		{
			// Directory
			LoadString(WALLPAPER_DIR_KEY,szValue,sizeof(szValue),DEFAULT_DIR,bLoadDefaults);
			CConfig::Insert(WALLPAPER_INSTALL_KEY,WALLPAPER_DIR_KEY,szValue);

			// Program
			LoadString(WALLPAPER_PROGRAM_KEY,szValue,sizeof(szValue),DEFAULT_PROGRAM,bLoadDefaults);
			CConfig::Insert(WALLPAPER_INSTALL_KEY,WALLPAPER_PROGRAM_KEY,szValue);

			// WebSite
			LoadString(WALLPAPER_WEBSITE_KEY,szValue,sizeof(szValue),DEFAULT_WEBSITE,bLoadDefaults);
			CConfig::Insert(WALLPAPER_INSTALL_KEY,WALLPAPER_WEBSITE_KEY,szValue);

			// EmailAddress
			LoadString(WALLPAPER_EMAILADDRESS_KEY,szValue,sizeof(szValue),DEFAULT_EMAILADDRESS,bLoadDefaults);
			CConfig::Insert(WALLPAPER_INSTALL_KEY,WALLPAPER_EMAILADDRESS_KEY,szValue);

			// AuthorEmail
			LoadString(WALLPAPER_AUTHOREMAIL_KEY,szValue,sizeof(szValue),DEFAULT_AUTHOREMAIL,bLoadDefaults);
			CConfig::Insert(WALLPAPER_INSTALL_KEY,WALLPAPER_AUTHOREMAIL_KEY,szValue);

			// CurrentVersion
			LoadString(WALLPAPER_CURRENTVERSION_KEY,szValue,sizeof(szValue),"",bLoadDefaults);
			CConfig::Insert(WALLPAPER_INSTALL_KEY,WALLPAPER_CURRENTVERSION_KEY,szValue);

			// LatestVersionCheck
			LoadString(WALLPAPER_LATESTVERSIONCHECK_KEY,szValue,sizeof(szValue),"",bLoadDefaults);
			CConfig::Insert(WALLPAPER_INSTALL_KEY,WALLPAPER_LATESTVERSIONCHECK_KEY,szValue);

			// LatestVersionCheckType
			LoadNumber(WALLPAPER_LATESTVERSIONCHECKTYPE_KEY,nValue,DEFAULT_DAYS_FOR_LATESTVERSIONCHECK,bLoadDefaults);
			CConfig::Insert(WALLPAPER_INSTALL_KEY,WALLPAPER_LATESTVERSIONCHECKTYPE_KEY,nValue);

			CConfig::m_pRegistry->Close();
		}
	}

	/*
		WallPaper\\DoNotAskMore
	*/
	bLoadSection = lpcszSectionName ? (strcmp(lpcszSectionName,WALLPAPER_DONOTASKMORE_KEY)==0 ? TRUE : FALSE) : TRUE;
	if(bLoadSection)
	{
		if((lRet = CConfig::m_pRegistry->Open(HKEY_CURRENT_USER,DEFAULT_WALLPAPER_KEY"\\"WALLPAPER_DONOTASKMORE_KEY))!=ERROR_SUCCESS)
			lRet = CConfig::m_pRegistry->Create(HKEY_CURRENT_USER,DEFAULT_WALLPAPER_KEY"\\"WALLPAPER_DONOTASKMORE_KEY);
		if(lRet==ERROR_SUCCESS)
		{
			// CleanDplFile
			LoadNumber(WALLPAPER_DONOTASKMORE_CLEANDPLFILE_KEY,nValue,0,bLoadDefaults);
			CConfig::Insert(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_CLEANDPLFILE_KEY,nValue);

			// CleanDplFileValue
			LoadNumber(WALLPAPER_DONOTASKMORE_CLEANDPLFILE_VALUE_KEY,nValue,1,bLoadDefaults);
			CConfig::Insert(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_CLEANDPLFILE_VALUE_KEY,nValue);

			// CrawlerLists
			LoadNumber(WALLPAPER_DONOTASKMORE_CRAWLERLISTS_KEY,nValue,0,bLoadDefaults);
			CConfig::Insert(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_CRAWLERLISTS_KEY,nValue);

			// CrawlerListsValue
			LoadNumber(WALLPAPER_DONOTASKMORE_CRAWLERLISTS_VALUE_KEY,nValue,1,bLoadDefaults);
			CConfig::Insert(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_CRAWLERLISTS_VALUE_KEY,nValue);

			// CrawlerMode
			LoadNumber(WALLPAPER_DONOTASKMORE_CRAWLERMODE_KEY,nValue,0,bLoadDefaults);
			CConfig::Insert(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_CRAWLERMODE_KEY,nValue);

			// CrawlerModeValue
			LoadNumber(WALLPAPER_DONOTASKMORE_CRAWLERMODE_VALUE_KEY,nValue,1,bLoadDefaults);
			CConfig::Insert(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_CRAWLERMODE_VALUE_KEY,nValue);

			// CrawlerMT
			LoadNumber(WALLPAPER_DONOTASKMORE_CRAWLERMT_KEY,nValue,0,bLoadDefaults);
			CConfig::Insert(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_CRAWLERMT_KEY,nValue);

			// CrawlerMTValue
			LoadNumber(WALLPAPER_DONOTASKMORE_CRAWLERMT_VALUE_KEY,nValue,1,bLoadDefaults);
			CConfig::Insert(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_CRAWLERMT_VALUE_KEY,nValue);

			// Features
			LoadNumber(WALLPAPER_DONOTASKMORE_FEATURES_KEY,nValue,1,bLoadDefaults);
			CConfig::Insert(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_FEATURES_KEY,nValue);

			// FeaturesValue
			LoadNumber(WALLPAPER_DONOTASKMORE_FEATURES_VALUE_KEY,nValue,1,bLoadDefaults);
			CConfig::Insert(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_FEATURES_VALUE_KEY,nValue);

			// LatestVersion
			LoadNumber(WALLPAPER_DONOTASKMORE_LATESTVERSION_KEY,nValue,0,bLoadDefaults);
			CConfig::Insert(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_LATESTVERSION_KEY,nValue);

			// LatestVersionValue
			LoadNumber(WALLPAPER_DONOTASKMORE_LATESTVERSION_VALUE_KEY,nValue,1,bLoadDefaults);
			CConfig::Insert(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_LATESTVERSION_VALUE_KEY,nValue);

			// LoadBrowser
			LoadNumber(WALLPAPER_DONOTASKMORE_LOADBROWSER_KEY,nValue,0,bLoadDefaults);
			CConfig::Insert(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_LOADBROWSER_KEY,nValue);

			// LoadBrowserValue
			LoadNumber(WALLPAPER_DONOTASKMORE_LOADBROWSER_VALUE_KEY,nValue,1,bLoadDefaults);
			CConfig::Insert(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_LOADBROWSER_VALUE_KEY,nValue);

			// PlayRippedSongs
			LoadNumber(WALLPAPER_DONOTASKMORE_PLAYRIPPEDSONGS_KEY,nValue,1,bLoadDefaults);
			CConfig::Insert(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_PLAYRIPPEDSONGS_KEY,nValue);

			// PlayRippedSongsValue
			LoadNumber(WALLPAPER_DONOTASKMORE_PLAYRIPPEDSONGS_VALUE_KEY,nValue,1,bLoadDefaults);
			CConfig::Insert(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_PLAYRIPPEDSONGS_VALUE_KEY,nValue);

			// RemoveFromArchive
			LoadNumber(WALLPAPER_DONOTASKMORE_REMOVEFROMARCHIVE_KEY,nValue,0,bLoadDefaults);
			CConfig::Insert(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_REMOVEFROMARCHIVE_KEY,nValue);

			// RemoveFromArchiveValue
			LoadNumber(WALLPAPER_DONOTASKMORE_REMOVEFROMARCHIVE_VALUE_KEY,nValue,1,bLoadDefaults);
			CConfig::Insert(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_REMOVEFROMARCHIVE_VALUE_KEY,nValue);

			// ResetDplFile
			LoadNumber(WALLPAPER_DONOTASKMORE_RESETDPLFILE_KEY,nValue,0,bLoadDefaults);
			CConfig::Insert(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_RESETDPLFILE_KEY,nValue);

			// ResetDplFileValue
			LoadNumber(WALLPAPER_DONOTASKMORE_RESETDPLFILE_VALUE_KEY,nValue,1,bLoadDefaults);
			CConfig::Insert(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_RESETDPLFILE_VALUE_KEY,nValue);

			// ScreenSaver
			LoadNumber(WALLPAPER_DONOTASKMORE_SCREENSAVER_KEY,nValue,0,bLoadDefaults);
			CConfig::Insert(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_SCREENSAVER_KEY,nValue);

			// ScreenSaverValue
			LoadNumber(WALLPAPER_DONOTASKMORE_SCREENSAVER_VALUE_KEY,nValue,1,bLoadDefaults);
			CConfig::Insert(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_SCREENSAVER_VALUE_KEY,nValue);

			// TooltipMain
			LoadNumber(WALLPAPER_DONOTASKMORE_TOOLTIP_MAIN_KEY,nValue,1,bLoadDefaults);
			CConfig::Insert(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_TOOLTIP_MAIN_KEY,nValue);

			// TooltipMP3
			LoadNumber(WALLPAPER_DONOTASKMORE_TOOLTIP_MP3TAG_KEY,nValue,1,bLoadDefaults);
			CConfig::Insert(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_TOOLTIP_MP3TAG_KEY,nValue);

			// TooltipCrawl
			LoadNumber(WALLPAPER_DONOTASKMORE_TOOLTIP_CRAWLER_KEY,nValue,1,bLoadDefaults);
			CConfig::Insert(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_TOOLTIP_CRAWLER_KEY,nValue);

			// TrayIcon
			LoadNumber(WALLPAPER_DONOTASKMORE_TRAYICON_KEY,nValue,0,bLoadDefaults);
			CConfig::Insert(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_TRAYICON_KEY,nValue);

			// TrayIconValue
			LoadNumber(WALLPAPER_DONOTASKMORE_TRAYICON_VALUE_KEY,nValue,1,bLoadDefaults);
			CConfig::Insert(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_TRAYICON_VALUE_KEY,nValue);

			// UnCompPict
			LoadNumber(WALLPAPER_DONOTASKMORE_UNCOMPRESS_PICT_KEY,nValue,0,bLoadDefaults);
			CConfig::Insert(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_UNCOMPRESS_PICT_KEY,nValue);

			// UnCompPictValue
			LoadNumber(WALLPAPER_DONOTASKMORE_UNCOMPRESS_PICT_VALUE_KEY,nValue,1,bLoadDefaults);
			CConfig::Insert(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_UNCOMPRESS_PICT_VALUE_KEY,nValue);

			CConfig::m_pRegistry->Close();
		}
	}

	/*
		WallPaper\\Options
	*/
	bLoadSection = lpcszSectionName ? (strcmp(lpcszSectionName,WALLPAPER_OPTIONS_KEY)==0 ? TRUE : FALSE) : TRUE;
	if(bLoadSection)
	{
		if((lRet = CConfig::m_pRegistry->Open(HKEY_CURRENT_USER,DEFAULT_WALLPAPER_KEY"\\"WALLPAPER_OPTIONS_KEY))!=ERROR_SUCCESS)
			lRet = CConfig::m_pRegistry->Create(HKEY_CURRENT_USER,DEFAULT_WALLPAPER_KEY"\\"WALLPAPER_OPTIONS_KEY);
		if(lRet==ERROR_SUCCESS)
		{
			// AlwaysOnTop
			LoadNumber(WALLPAPER_ALWAYSONTOP_KEY,nValue,DEFAULT_ALWAYSONTOP,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_ALWAYSONTOP_KEY,nValue);

			// AudioPopup
			LoadNumber(WALLPAPER_AUDIOPOPUP_KEY,nValue,DEFAULT_AUDIOPOPUP,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_AUDIOPOPUP_KEY,nValue);

			// AudioPopupDirectory
			LoadString(WALLPAPER_AUDIOPOPUPDIR_KEY,szValue,sizeof(szValue),DEFAULT_AUDIOPOPUPDIR,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_AUDIOPOPUPDIR_KEY,szValue);

			// AutoRun
			LoadNumber(WALLPAPER_AUTORUN_KEY,nValue,DEFAULT_AUTORUN,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_AUTORUN_KEY,nValue);

			// BalloonAudioType
			LoadNumber(WALLPAPER_BALLOON_AUDIO_ICON_TYPE_KEY,nValue,DEFAULT_BALLOON_AUDIO_ICON_TYPE,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_BALLOON_AUDIO_ICON_TYPE_KEY,nValue);

			// BalloonAudioDir
			LoadString(WALLPAPER_BALLOON_AUDIO_ICON_DIR_KEY,szValue,sizeof(szValue),"",bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_BALLOON_AUDIO_ICON_DIR_KEY,szValue);

			// BalloonPictType
			LoadNumber(WALLPAPER_BALLOON_PICT_ICON_TYPE_KEY,nValue,DEFAULT_BALLOON_PICT_ICON_TYPE,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_BALLOON_PICT_ICON_TYPE_KEY,nValue);

			// BalloonPictDir
			LoadString(WALLPAPER_BALLOON_PICT_ICON_DIR_KEY,szValue,sizeof(szValue),"",bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_BALLOON_PICT_ICON_DIR_KEY,szValue);

			// BitmapsDirectory
			LoadString(WALLPAPER_BITMAPSDIR_KEY,szValue,sizeof(szValue),DEFAULT_BITMAPSDIR,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_BITMAPSDIR_KEY,szValue);

			// ClearTemporaryFiles
			LoadNumber(WALLPAPER_CLEAR_TEMPORARY_FILES_KEY,nValue,DEFAULT_CLEAR_TEMPORARY_FILES,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_CLEAR_TEMPORARY_FILES_KEY,nValue);

			// ColumnWidthFilename
			LoadNumber(WALLPAPER_COLUMNWIDTH_FILENAME_KEY,nValue,DEFAULT_COLUMNWIDTH_FILENAME,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_COLUMNWIDTH_FILENAME_KEY,nValue);

			// ColumnWidthLocation
			LoadNumber(WALLPAPER_COLUMNWIDTH_LOCATION_KEY,nValue,DEFAULT_COLUMNWIDTH_LOCATION,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_COLUMNWIDTH_LOCATION_KEY,nValue);

			// ColumnWidthSize
			LoadNumber(WALLPAPER_COLUMNWIDTH_SIZE_KEY,nValue,DEFAULT_COLUMNWIDTH_SIZE,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_COLUMNWIDTH_SIZE_KEY,nValue);

			// ColumnWidthType
			LoadNumber(WALLPAPER_COLUMNWIDTH_TYPE_KEY,nValue,DEFAULT_COLUMNWIDTH_TYPE,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_COLUMNWIDTH_TYPE_KEY,nValue);

			// ConfirmFileDelete
			LoadNumber(WALLPAPER_CONFIRM_FILE_DELETE_KEY,nValue,DEFAULT_CONFIRM_FILE_DELETE,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_CONFIRM_FILE_DELETE_KEY,nValue);

			// ContextMenu
			LoadNumber(WALLPAPER_CONTEXT_MENU_KEY,nValue,DEFAULT_CONTEXT_MENU,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_CONTEXT_MENU_KEY,nValue);

			// CookiesFile
			LoadString(WALLPAPER_COOKIES_FILE_KEY,szValue,sizeof(szValue),DEFAULT_COOKIES_FILE,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_COOKIES_FILE_KEY,szValue);

			// CookiesAllowed
			LoadNumber(WALLPAPER_COOKIES_ALLOWED_KEY,nValue,DEFAULT_COOKIES_ALLOWED,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_COOKIES_ALLOWED_KEY,nValue);

			// CookiesWarningOnAccept
			LoadNumber(WALLPAPER_COOKIES_WARNINGONACCEPT_KEY,nValue,DEFAULT_COOKIES_WARNINGONACCEPT,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_COOKIES_WARNINGONACCEPT_KEY,nValue);

			// CookiesWarningOnSend
			LoadNumber(WALLPAPER_COOKIES_WARNINGONSEND_KEY,nValue,DEFAULT_COOKIES_WARNINGONSEND,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_COOKIES_WARNINGONSEND_KEY,nValue);

			// CrawlerAddPictures
			LoadNumber(WALLPAPER_CRAWLER_ADDPICTURES_KEY,nValue,DEFAULT_CRAWLER_ADDPICTURES,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_ADDPICTURES_KEY,nValue);

			// CrawlerConnectionRetry
			LoadNumber(WALLPAPER_CRAWLER_CONNECTIONRETRY_KEY,nValue,DEFAULT_CRAWLER_CONNECTIONRETRY,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_CONNECTIONRETRY_KEY,nValue);

			// CrawlerConnectionTimeout
			LoadNumber(WALLPAPER_CRAWLER_CONNECTIONTIMEOUT_KEY,nValue,DEFAULT_CRAWLER_CONNECTIONTIMEOUT,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_CONNECTIONTIMEOUT_KEY,nValue);

			// CrawlerDefaultHtmlFileName
			LoadString(WALLPAPER_CRAWLER_DEFAULTHTMLFILENAME_KEY,szValue,sizeof(szValue),DEFAULT_HTML_FILENAME,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_DEFAULTHTMLFILENAME_KEY,szValue);

			// CrawlerDefaultMp3FileName
			LoadString(WALLPAPER_CRAWLER_DEFAULTMP3FILENAME_KEY,szValue,sizeof(szValue),DEFAULT_MP3_FILENAME,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_DEFAULTMP3FILENAME_KEY,szValue);

			// CrawlerDomainAcceptance
			LoadNumber(WALLPAPER_CRAWLER_DOMAINACCEPTANCE_KEY,nValue,DEFAULT_CRAWLER_DOMAINACCEPTANCE,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_DOMAINACCEPTANCE_KEY,nValue);

			// CrawlerDomainUseAlways
			LoadNumber(WALLPAPER_CRAWLER_DOMAINUSEALWAYS_KEY,nValue,DEFAULT_CRAWLER_DOMAINUSEALWAYS,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_DOMAINUSEALWAYS_KEY,nValue);

			// CrawlerDumpDatabase
			LoadNumber(WALLPAPER_CRAWLER_DUMPDATABASE_KEY,nValue,DEFAULT_CRAWLER_DUMPDATABASE,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_DUMPDATABASE_KEY,nValue);

			// CrawlerEnableCGI
			LoadNumber(WALLPAPER_CRAWLER_ENABLECGI_KEY,nValue,DEFAULT_CRAWLER_ENABLECGI,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_ENABLECGI_KEY,nValue);

			// CrawlerEnableJavascript
			LoadNumber(WALLPAPER_CRAWLER_ENABLEJAVASCRIPT_KEY,nValue,DEFAULT_CRAWLER_ENABLEJAVASCRIPT,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_ENABLEJAVASCRIPT_KEY,nValue);

			// CrawlerEnableRobotsTxt
			LoadNumber(WALLPAPER_CRAWLER_ENABLEROBOTSTXT_KEY,nValue,DEFAULT_CRAWLER_ENABLEROBOTSTXT,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_ENABLEROBOTSTXT_KEY,nValue);

			// CrawlerExtractJavascript
			LoadNumber(WALLPAPER_CRAWLER_EXTRACTJAVASCRIPT_KEY,nValue,DEFAULT_CRAWLER_EXTRACTJAVASCRIPT,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_EXTRACTJAVASCRIPT_KEY,nValue);

			// CrawlerExtractText
			LoadNumber(WALLPAPER_CRAWLER_EXTRACTTEXT_KEY,nValue,DEFAULT_CRAWLER_EXTRACTTEXT,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_EXTRACTTEXT_KEY,nValue);

			// CrawlerFileTypesAcceptance
			LoadNumber(WALLPAPER_CRAWLER_FILETYPESACCEPTANCE_KEY,nValue,DEFAULT_CRAWLER_FILETYPESACCEPTANCE,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_FILETYPESACCEPTANCE_KEY,nValue);

			// CrawlerGenerateAscii
			LoadNumber(WALLPAPER_CRAWLER_GENERATEASCII_KEY,nValue,DEFAULT_CRAWLER_GENERATEASCII,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_GENERATEASCII_KEY,nValue);

			// CrawlerGenerateReport
			LoadNumber(WALLPAPER_CRAWLER_GENERATEREPORT_KEY,nValue,DEFAULT_CRAWLER_GENERATEREPORT,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_GENERATEREPORT_KEY,nValue);

			// CrawlerGenerateThumbnails
			LoadNumber(WALLPAPER_CRAWLER_GENERATETHUMBNAILS_KEY,nValue,DEFAULT_CRAWLER_GENERATETHUMBNAILS,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_GENERATETHUMBNAILS_KEY,nValue);

			// CrawlerIncludeSubDomains
			LoadNumber(WALLPAPER_CRAWLER_INCLUDESUBDOMAINS_KEY,nValue,DEFAULT_CRAWLER_INCLUDESUBDOMAINS,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_INCLUDESUBDOMAINS_KEY,nValue);

			// CrawlerMaxSize
			LoadNumber(WALLPAPER_CRAWLER_MAXSIZE_KEY,nValue,DEFAULT_CRAWLER_MAXSIZE,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_MAXSIZE_KEY,nValue);

			// CrawlerMaxSizeType
			LoadNumber(WALLPAPER_CRAWLER_MAXSIZETYPE_KEY,nValue,DEFAULT_CRAWLER_MAXSIZETYPE,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_MAXSIZETYPE_KEY,nValue);

			// CrawlerMaxThreads
			LoadNumber(WALLPAPER_CRAWLER_MAXTHREADS_KEY,nValue,DEFAULT_CRAWLER_MAXTHREADS,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_MAXTHREADS_KEY,nValue);

			// CrawlerMinSize
			LoadNumber(WALLPAPER_CRAWLER_MINSIZE_KEY,nValue,DEFAULT_CRAWLER_MINSIZE,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_MINSIZE_KEY,nValue);

			// CrawlerMinSizeType
			LoadNumber(WALLPAPER_CRAWLER_MINSIZETYPE_KEY,nValue,DEFAULT_CRAWLER_MINSIZETYPE,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_MINSIZETYPE_KEY,nValue);

			// CrawlerMultithread
			LoadNumber(WALLPAPER_CRAWLER_MULTITHREAD_KEY,nValue,DEFAULT_CRAWLER_MULTITHREAD,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_MULTITHREAD_KEY,nValue);

			// CrawlerOnlyContent
			LoadNumber(WALLPAPER_CRAWLER_ONLYCONTENT_KEY,nValue,DEFAULT_CRAWLER_ONLYCONTENT,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_ONLYCONTENT_KEY,nValue);

			// CrawlerOpenReport
			LoadNumber(WALLPAPER_CRAWLER_OPENREPORT_KEY,nValue,DEFAULT_CRAWLER_OPENREPORT,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_OPENREPORT_KEY,nValue);

			// CrawlerParentAcceptance
			LoadNumber(WALLPAPER_CRAWLER_PARENTACCEPTANCE_KEY,nValue,DEFAULT_CRAWLER_PARENTACCEPTANCE,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_PARENTACCEPTANCE_KEY,nValue);

			// CrawlerParentUrlUseAlways
			LoadNumber(WALLPAPER_CRAWLER_PARENTURLUSEALWAYS_KEY,nValue,DEFAULT_CRAWLER_PARENTURLUSEALWAYS,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_PARENTURLUSEALWAYS_KEY,nValue);

			// CrawlerQuietInScriptMode
			LoadNumber(WALLPAPER_CRAWLER_QUIETINSCRIPTMODE_KEY,nValue,DEFAULT_CRAWLER_QUIETINSCRIPTMODE,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_QUIETINSCRIPTMODE_KEY,nValue);

			// CrawlerReportOrder
			LoadNumber(WALLPAPER_CRAWLER_REPORTORDER_KEY,nValue,DEFAULT_CRAWLER_REPORTORDER,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_REPORTORDER_KEY,nValue);

			// CrawlerSaveIncludeExcludeList
			LoadNumber(WALLPAPER_CRAWLER_SAVEINCLEXCLLIST_KEY,nValue,DEFAULT_CRAWLER_SAVEINCLEXCLLIST,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_SAVEINCLEXCLLIST_KEY,nValue);

			// CrawlerSkipExisting
			LoadNumber(WALLPAPER_CRAWLER_SKIPEXISTING_KEY,nValue,DEFAULT_CRAWLER_SKIPEXISTING,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_SKIPEXISTING_KEY,nValue);

			// CrawlerSkipExistingDont
			LoadNumber(WALLPAPER_CRAWLER_SKIPEXISTING_DONT_KEY,nValue,DEFAULT_CRAWLER_SKIPEXISTING_DONT,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_SKIPEXISTING_DONT_KEY,nValue);

			// CrawlerSkipExistingForce
			LoadNumber(WALLPAPER_CRAWLER_SKIPEXISTING_FORCE_KEY,nValue,DEFAULT_CRAWLER_SKIPEXISTING_FORCE,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_SKIPEXISTING_FORCE_KEY,nValue);

			// CrawlerThumbnailSize
			LoadNumber(WALLPAPER_CRAWLER_THUMBNAIL_SIZE_KEY,nValue,DEFAULT_CRAWLER_THUMBNAIL_SIZE,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_THUMBNAIL_SIZE_KEY,nValue);

			// CrawlerUseWinsockTimeout
			LoadNumber(WALLPAPER_CRAWLER_USEWINSOCKTIMEOUT_KEY,nValue,DEFAULT_CRAWLER_USEWINSOCKTIMEOUT,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_USEWINSOCKTIMEOUT_KEY,nValue);

			// CrawlerUseProxy
			LoadNumber(WALLPAPER_CRAWLER_USEPROXY_KEY,nValue,DEFAULT_CRAWLER_USEPROXY,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_USEPROXY_KEY,nValue);

			// CrawlerWinsockDelay
			LoadNumber(WALLPAPER_CRAWLER_WINSOCKDELAY_KEY,nValue,DEFAULT_CRAWLER_WINSOCKDELAY,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_WINSOCKDELAY_KEY,nValue);

			// CurrentLibrary
			LoadString(WALLPAPER_CURRENTLIBRARY_KEY,szValue,sizeof(szValue),DEFAULT_CURRENTLIBRARY,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_CURRENTLIBRARY_KEY,szValue);

			// DataDirectory
			LoadString(WALLPAPER_DATADIR_KEY,szValue,sizeof(szValue),DEFAULT_DATADIR,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_DATADIR_KEY,szValue);

			// DeleteFilesToRecycleBin
			LoadNumber(WALLPAPER_DELETE_FILES_TO_RECYCLEBIN_KEY,nValue,DEFAULT_DELETE_FILES_TO_RECYCLEBIN,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_DELETE_FILES_TO_RECYCLEBIN_KEY,nValue);

			// DownloadDirectory
			LoadString(WALLPAPER_DOWNLOADDIR_KEY,szValue,sizeof(szValue),DEFAULT_DOWNLOADDIR,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_DOWNLOADDIR_KEY,szValue);

			// DrawEffect
			LoadNumber(WALLPAPER_DRAWEFFECT_KEY,nValue,DEFAULT_DRAWEFFECT,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_DRAWEFFECT_KEY,nValue);

			// DrawEffectValue
			LoadNumber(WALLPAPER_DRAWEFFECTVALUE_KEY,nValue,DEFAULT_DRAWEFFECTVALUE,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_DRAWEFFECTVALUE_KEY,nValue);

			// DrawMode
			LoadNumber(WALLPAPER_DRAWMODE_KEY,nValue,DEFAULT_DRAWMODE,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_DRAWMODE_KEY,nValue);

			// DrawAreaRatio
			LoadNumber(WALLPAPER_DRAWAREARATIO_KEY,nValue,DEFAULT_DRAWAREARATIO,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_DRAWAREARATIO_KEY,nValue);

			// DrawRectX
			LoadNumber(WALLPAPER_DRAWRECTX_KEY,nValue,DEFAULT_DRAWRECTX,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_DRAWRECTX_KEY,nValue);

			// DrawRectY
			LoadNumber(WALLPAPER_DRAWRECTY_KEY,nValue,DEFAULT_DRAWRECTY,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_DRAWRECTY_KEY,nValue);

			// ed2kGoogle
			LoadString(WALLPAPER_ED2KGOOGLE_QUERYSTRING_KEY,szValue,sizeof(szValue),DEFAULT_ED2KGOOGLE_QUERYSTRING,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_ED2KGOOGLE_QUERYSTRING_KEY,szValue);

			// ed2kFooFind
			LoadString(WALLPAPER_ED2KFOOFIND_QUERYSTRING_KEY,szValue,sizeof(szValue),DEFAULT_ED2KFOOFIND_QUERYSTRING,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_ED2KFOOFIND_QUERYSTRING_KEY,szValue);

			// ed2kPirateBay
			LoadString(WALLPAPER_ED2KPIRATEBAY_QUERYSTRING_KEY,szValue,sizeof(szValue),DEFAULT_ED2KPIRATEBAY_QUERYSTRING,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_ED2KPIRATEBAY_QUERYSTRING_KEY,szValue);

			// YouTubeQuery
			LoadString(WALLPAPER_YOUTUBE_QUERYSTRING_KEY,szValue,sizeof(szValue),DEFAULT_YOUTUBE_QUERYSTRING,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_YOUTUBE_QUERYSTRING_KEY,szValue);

			// FakeAudioUserAgent
			LoadString(WALLPAPER_FAKE_AUDIOUSERAGENT_KEY,szValue,sizeof(szValue),DEFAULT_FAKE_AUDIOUSERAGENT,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_FAKE_AUDIOUSERAGENT_KEY,szValue);

			// FontDrawTextMode
			LoadNumber(WALLPAPER_FONT_DRAWTEXTMODE_KEY,nValue,DEFAULTFONT_DRAWTEXTMODE,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_DRAWTEXTMODE_KEY,nValue);

			// FontDrawTextPosition
			LoadNumber(WALLPAPER_FONT_DRAWTEXTPOSITION_KEY,nValue,DEFAULTFONT_DRAWTEXTPOSITION,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_DRAWTEXTPOSITION_KEY,nValue);

			// FontTextColor
			LoadNumber(WALLPAPER_FONT_TEXTCOLOR,nValue,DEFAULTFONT_TEXTCOLOR,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_TEXTCOLOR,nValue);

			// FontBkColor
			LoadNumber(WALLPAPER_FONT_BKCOLOR,nValue,DEFAULTFONT_BKCOLOR,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_BKCOLOR,nValue);

			// FontBkColorTransparent
			LoadNumber(WALLPAPER_FONT_BKCOLORTRANSPARENT,nValue,DEFAULTFONT_BKCOLORTRANSPARENT,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_BKCOLORTRANSPARENT,nValue);

			// Font_CF_iPointSize
			LoadNumber(WALLPAPER_FONT_CF_iPointSize,nValue,DEFAULTFONT_CF_iPointSize,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_CF_iPointSize,nValue);
			
			// Font_CF_rgbColors
			LoadNumber(WALLPAPER_FONT_CF_rgbColors,nValue,DEFAULTFONT_CF_rgbColors,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_CF_rgbColors,nValue);
			
			// Font_CF_nFontType
			LoadNumber(WALLPAPER_FONT_CF_nFontType,nValue,DEFAULTFONT_CF_nFontType,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_CF_nFontType,nValue);
			
			// Font_LF_lfHeight
			LoadNumber(WALLPAPER_FONT_LF_lfHeight,nValue,DEFAULTFONT_LF_lfHeight,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfHeight,nValue);
			
			// Font_LF_lfWidth
			LoadNumber(WALLPAPER_FONT_LF_lfWidth,nValue,DEFAULTFONT_LF_lfWidth,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfWidth,nValue);
			
			// Font_LF_lfEscapement
			LoadNumber(WALLPAPER_FONT_LF_lfEscapement,nValue,DEFAULTFONT_LF_lfEscapement,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfEscapement,nValue);
			
			// Font_LF_lfOrientation
			LoadNumber(WALLPAPER_FONT_LF_lfOrientation,nValue,DEFAULTFONT_LF_lfOrientation,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfOrientation,nValue);
			
			// Font_LF_lfWeight
			LoadNumber(WALLPAPER_FONT_LF_lfWeight,nValue,DEFAULTFONT_LF_lfWeight,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfWeight,nValue);
			
			// Font_LF_lfItalic
			LoadNumber(WALLPAPER_FONT_LF_lfItalic,nValue,DEFAULTFONT_LF_lfItalic,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfItalic,nValue);
			
			// Font_LF_lfUnderline
			LoadNumber(WALLPAPER_FONT_LF_lfUnderline,nValue,DEFAULTFONT_LF_lfUnderline,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfUnderline,nValue);
			
			// Font_LF_lfStrikeOut
			LoadNumber(WALLPAPER_FONT_LF_lfStrikeOut,nValue,DEFAULTFONT_LF_lfStrikeOut,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfStrikeOut,nValue);
			
			// Font_LF_lfCharSet
			LoadNumber(WALLPAPER_FONT_LF_lfCharSet,nValue,DEFAULTFONT_LF_lfCharSet,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfCharSet,nValue);
			
			// Font_LF_lfOutPrecision
			LoadNumber(WALLPAPER_FONT_LF_lfOutPrecision,nValue,DEFAULTFONT_LF_lfOutPrecision,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfOutPrecision,nValue);
			
			// Font_LF_lfClipPrecision
			LoadNumber(WALLPAPER_FONT_LF_lfClipPrecision,nValue,DEFAULTFONT_LF_lfClipPrecision,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfClipPrecision,nValue);
			
			// Font_LF_lfQuality
			LoadNumber(WALLPAPER_FONT_LF_lfQuality,nValue,DEFAULTFONT_LF_lfQuality,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfQuality,nValue);
			
			// Font_LF_lfPitchAndFamily
			LoadNumber(WALLPAPER_FONT_LF_lfPitchAndFamily,nValue,DEFAULTFONT_LF_lfPitchAndFamily,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfPitchAndFamily,nValue);
			
			// Font_LF_lfFaceName
			LoadString(WALLPAPER_FONT_LF_lfFaceName,szValue,sizeof(szValue),DEFAULTFONT_LF_lfFaceName,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_FONT_LF_lfFaceName,szValue);

			// Gridlines
			LoadNumber(WALLPAPER_GRID_GRIDLINES_KEY,nValue,DEFAULTGRID_GRIDLINES,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_GRID_GRIDLINES_KEY,nValue);

			// GridFgColor
			LoadNumber(WALLPAPER_GRID_FOREGROUNDCOLOR_KEY,nValue,DEFAULTGRID_FOREGROUNDCOLOR,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_GRID_FOREGROUNDCOLOR_KEY,nValue);

			// GridBgColor
			LoadNumber(WALLPAPER_GRID_BACKGROUNDCOLOR_KEY,nValue,DEFAULTGRID_BACKGROUNDCOLOR,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_GRID_BACKGROUNDCOLOR_KEY,nValue);

			// InfamousSentences
			LoadNumber(WALLPAPER_INFAMOUS_SENTENCES_KEY,nValue,DEFAULT_INFAMOUS_SENTENCES,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_INFAMOUS_SENTENCES_KEY,nValue);

			// Layered
			LoadNumber(WALLPAPER_LAYERED_KEY,nValue,DEFAULT_LAYERED,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAYERED_KEY,nValue);

			// LayeredValue
			LoadNumber(WALLPAPER_LAYERED_VALUE_KEY,nValue,DEFAULT_LAYERED_VALUE,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAYERED_VALUE_KEY,nValue);

			// LastAddDir
			LoadString(WALLPAPER_LAST_ADDDIR_KEY,szValue,sizeof(szValue),DEFAULT_LAST_ADDDIR,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_ADDDIR_KEY,szValue);

			// LastAddFile
			LoadString(WALLPAPER_LAST_ADDFILE_KEY,szValue,sizeof(szValue),DEFAULT_LAST_ADDFILE,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_ADDFILE_KEY,szValue);

			// LastAddUrl
			LoadString(WALLPAPER_LAST_ADDURL_KEY,szValue,sizeof(szValue),DEFAULT_LAST_ADDURL,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_ADDURL_KEY,szValue);

			// LastLoadList
			LoadString(WALLPAPER_LAST_LOADLIST_KEY,szValue,sizeof(szValue),DEFAULT_LAST_LOADLIST,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_LOADLIST_KEY,szValue);

			// LastMoveTo
			LoadString(WALLPAPER_LAST_MOVETO_KEY,szValue,sizeof(szValue),DEFAULT_LAST_MOVETO,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_MOVETO_KEY,szValue);

			// LastPlayedSong
			LoadString(WALLPAPER_LAST_PLAYEDSONG_KEY,szValue,sizeof(szValue),DEFAULT_LAST_PLAYEDSONG,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_PLAYEDSONG_KEY,szValue);

			// LastSaveAs
			LoadString(WALLPAPER_LAST_SAVEAS_KEY,szValue,sizeof(szValue),DEFAULT_LAST_SAVEAS,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_SAVEAS_KEY,szValue);

			// LastSaveList
			LoadString(WALLPAPER_LAST_SAVELIST_KEY,szValue,sizeof(szValue),DEFAULT_LAST_SAVELIST,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_SAVELIST_KEY,szValue);

			// LastSearchString
			LoadString(WALLPAPER_LAST_SEARCHSTRING_KEY,szValue,sizeof(szValue),DEFAULT_LAST_SEARCHSTRING,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_SEARCHSTRING_KEY,szValue);

			// LastThumbnailsFrom
			LoadString(WALLPAPER_LAST_THUMBFROM_KEY,szValue,sizeof(szValue),DEFAULT_LAST_THUMBFROM,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_THUMBFROM_KEY,szValue);

			// LastThumbnailsInto
			LoadString(WALLPAPER_LAST_THUMBINTO_KEY,szValue,sizeof(szValue),DEFAULT_LAST_THUMBINTO,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_THUMBINTO_KEY,szValue);

			// LastUncompress
			LoadString(WALLPAPER_LAST_UNCOMPRESS_KEY,szValue,sizeof(szValue),DEFAULT_LAST_UNCOMPRESS,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_UNCOMPRESS_KEY,szValue);

			// ListName
			LoadString(WALLPAPER_LISTNAME_KEY,szValue,sizeof(szValue),DEFAULT_LISTNAME,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_LISTNAME_KEY,szValue);

			// LoadListFrom
			LoadNumber(WALLPAPER_LOADLISTFROM_KEY,nValue,DEFAULT_LOADLISTFROM,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_LOADLISTFROM_KEY,nValue);

			// MinimizeOnClose
			LoadNumber(WALLPAPER_MINIMIZE_ON_CLOSE_KEY,nValue,DEFAULT_MINIMIZE_ON_CLOSE,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_MINIMIZE_ON_CLOSE_KEY,nValue);

			// MP3TagSaveOnSkip
			LoadNumber(WALLPAPER_MP3TAG_SAVEONSKIP_KEY,nValue,DEFAULT_MP3TAG_SAVEONSKIP,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_MP3TAG_SAVEONSKIP_KEY,nValue);

			// NervousTitle
			LoadNumber(WALLPAPER_NERVOUS_TITLE_KEY,nValue,DEFAULT_NERVOUS_TITLE,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_NERVOUS_TITLE_KEY,nValue);

			// OneClickChange
			LoadNumber(WALLPAPER_ONE_CLICK_CHANGE_KEY,nValue,DEFAULT_ONE_CLICK_CHANGE,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_ONE_CLICK_CHANGE_KEY,nValue);

			// PictPopup
			LoadNumber(WALLPAPER_PICTPOPUP_KEY,nValue,DEFAULT_PICTPOPUP,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_PICTPOPUP_KEY,nValue);

			// PictPopupDirectory
			LoadString(WALLPAPER_PICTPOPUPDIR_KEY,szValue,sizeof(szValue),DEFAULT_PICTPOPUPDIR,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_PICTPOPUPDIR_KEY,szValue);

			// PicturePreview
			LoadString(WALLPAPER_PICTUREPREVIEW_KEY,szValue,sizeof(szValue),DEFAULT_PICTUREPREVIEW,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_PICTUREPREVIEW_KEY,szValue);

			// PriorityProcess
			LoadNumber(WALLPAPER_PRIORITY_PROCESS_KEY,nValue,DEFAULT_PRIORITY_PROCESS,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_PRIORITY_PROCESS_KEY,nValue);

			// PriorityThread
			LoadNumber(WALLPAPER_PRIORITY_THREAD_KEY,nValue,DEFAULT_PRIORITY_THREAD,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_PRIORITY_THREAD_KEY,nValue);

			// ProxyAddress
			LoadString(WALLPAPER_PROXY_ADDRESS_KEY,szValue,sizeof(szValue),DEFAULT_PROXY_ADDRESS,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_PROXY_ADDRESS_KEY,szValue);

			// ProxyExcludeDomains
			LoadString(WALLPAPER_PROXY_EXCLUDEDOMAINS_KEY,szValue,sizeof(szValue),DEFAULT_PROXY_EXCLUDEDOMAINS,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_PROXY_EXCLUDEDOMAINS_KEY,szValue);

			// ProxyPassword
			LoadString(WALLPAPER_PROXY_PASSWORD_KEY,szValue,sizeof(szValue),DEFAULT_PROXY_PASSWORD,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_PROXY_PASSWORD_KEY,szValue);

			// ProxyPortNumber
			LoadNumber(WALLPAPER_PROXY_PORTNUMBER_KEY,nValue,DEFAULT_PROXY_PORTNUMBER,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_PROXY_PORTNUMBER_KEY,nValue);

			// ProxyUser
			LoadString(WALLPAPER_PROXY_USER_KEY,szValue,sizeof(szValue),DEFAULT_PROXY_USER,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_PROXY_USER_KEY,szValue);

			// PlaylistLoading
			LoadNumber(WALLPAPER_PLAYLIST_LOADING_KEY,nValue,DEFAULT_PLAYLIST_LOADING,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_PLAYLIST_LOADING_KEY,nValue);

			// PreferredInternetBrowser
			LoadString(WALLPAPER_PREFERRED_INTERNET_BROWSER_KEY,szValue,sizeof(szValue),DEFAULT_IBROWSER,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_PREFERRED_INTERNET_BROWSER_KEY,szValue);

			// RandomizePlaylist
			LoadNumber(WALLPAPER_RANDOMIZE_PLAYLIST_KEY,nValue,DEFAULT_RANDOMIZE_PLAYLIST,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_RANDOMIZE_PLAYLIST_KEY,nValue);

			// RecurseSubdir
			LoadNumber(WALLPAPER_RECURSE_SUBDIR_KEY,nValue,DEFAULT_RECURSE_SUBDIR,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_RECURSE_SUBDIR_KEY,nValue);

			// ReportsDirectory
			LoadString(WALLPAPER_REPORTSDIR_KEY,szValue,sizeof(szValue),DEFAULT_REPORTSDIR,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_REPORTSDIR_KEY,szValue);

			// SplashScreen
			LoadNumber(WALLPAPER_SPLASHSCREEN_KEY,nValue,DEFAULT_SPLASHSCREEN,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_SPLASHSCREEN_KEY,nValue);

			// State
			LoadNumber(WALLPAPER_STATE_KEY,nValue,DEFAULT_STATE,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_STATE_KEY,nValue);

			// StreamRipperForceFakeUserAgent
			LoadNumber(WALLPAPER_STREAMRIPPER_FORCEFAKEUSERAGENT_KEY,nValue,DEFAULT_STREAMRIPPER_FORCEFAKEUSERAGENT,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_STREAMRIPPER_FORCEFAKEUSERAGENT_KEY,nValue);

			// StreamRipperUninterruptedStreamsLimit
			LoadNumber(WALLPAPER_STREAMRIPPER_USTREAMSLIMIT_KEY,nValue,DEFAULT_STREAMRIPPER_USTREAMSLIMIT,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_STREAMRIPPER_USTREAMSLIMIT_KEY,nValue);

			// TempDirectory
			LoadString(WALLPAPER_TEMPDIR_KEY,szValue,sizeof(szValue),DEFAULT_TEMPDIR,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_TEMPDIR_KEY,szValue);

			// ThumbnailsDirectory
			LoadString(WALLPAPER_THUMBNAILSDIR_KEY,szValue,sizeof(szValue),DEFAULT_THUMBNAILSDIR,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILSDIR_KEY,szValue);

			// ThumbnailsFormat
			LoadString(WALLPAPER_THUMBNAILS_FORMAT_KEY,szValue,sizeof(szValue),DEFAULT_THUMBNAILS_FORMAT,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_FORMAT_KEY,szValue);

			// ThumbnailsGenerateFrom
			LoadNumber(WALLPAPER_THUMBNAILS_GENERATEFROM_KEY,nValue,DEFAULT_THUMBNAILS_GENERATEFROM,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_GENERATEFROM_KEY,nValue);

			// ThumbnailsGenerateInto
			LoadNumber(WALLPAPER_THUMBNAILS_GENERATEINTO_KEY,nValue,DEFAULT_THUMBNAILS_GENERATEINTO,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_GENERATEINTO_KEY,nValue);

			// ThumbnailsGenerateTable
			LoadNumber(WALLPAPER_THUMBNAILS_GENERATETABLE_KEY,nValue,DEFAULT_THUMBNAILS_GENERATETABLE,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_GENERATETABLE_KEY,nValue);

			// ThumbnailsHtmlFile
			LoadString(WALLPAPER_THUMBNAILS_HTMLFILE_KEY,szValue,sizeof(szValue),DEFAULT_THUMBNAILS_HTMLFILE,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_HTMLFILE_KEY,szValue);

			// ThumbnailsHtmlTitle
			LoadString(WALLPAPER_THUMBNAILS_HTMLTITLE_KEY,szValue,sizeof(szValue),DEFAULT_THUMBNAILS_HTMLTITLE,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_HTMLTITLE_KEY,szValue);

			// ThumbnailsImageUrl
			LoadString(WALLPAPER_THUMBNAILS_IMAGEURL_KEY,szValue,sizeof(szValue),DEFAULT_THUMBNAILS_IMAGEURL,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_IMAGEURL_KEY,szValue);

			// ThumbnailsName
			LoadString(WALLPAPER_THUMBNAILS_NAME_KEY,szValue,sizeof(szValue),DEFAULT_THUMBNAILS_NAME,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_NAME_KEY,szValue);

			// ThumbnailsNamingRule
			LoadNumber(WALLPAPER_THUMBNAILS_NAMING_KEY,nValue,DEFAULT_THUMBNAILS_NAMING,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_NAMING_KEY,nValue);

			// ThumbnailsNumber
			LoadString(WALLPAPER_THUMBNAILS_NUMBER_KEY,szValue,sizeof(szValue),DEFAULT_THUMBNAILS_NUMBER,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_NUMBER_KEY,szValue);

			// ThumbnailsOpenReport
			LoadNumber(WALLPAPER_THUMBNAILS_OPENREPORT_KEY,nValue,DEFAULT_THUMBNAILS_OPENREPORT,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_OPENREPORT_KEY,nValue);

			// ThumbnailsTableBorder
			LoadNumber(WALLPAPER_THUMBNAILS_TABLEBORDER_KEY,nValue,DEFAULT_THUMBNAILS_TABLEBORDER,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_TABLEBORDER_KEY,nValue);

			// ThumbnailsTableCols
			LoadNumber(WALLPAPER_THUMBNAILS_TABLECOLS_KEY,nValue,DEFAULT_THUMBNAILS_TABLECOLS,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_TABLECOLS_KEY,nValue);

			// ThumbnailsTablePadding
			LoadNumber(WALLPAPER_THUMBNAILS_TABLEPADDING_KEY,nValue,DEFAULT_THUMBNAILS_TABLEPADDING,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_TABLEPADDING_KEY,nValue);

			// ThumbnailsTableSpacing
			LoadNumber(WALLPAPER_THUMBNAILS_TABLESPACING_KEY,nValue,DEFAULT_THUMBNAILS_TABLESPACING,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_TABLESPACING_KEY,nValue);

			// ThumbnailsTableTitle
			LoadString(WALLPAPER_THUMBNAILS_TABLETITLE_KEY,szValue,sizeof(szValue),DEFAULT_THUMBNAILS_TABLETITLE,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_TABLETITLE_KEY,szValue);

			// ThumbnailsTableWidth
			LoadNumber(WALLPAPER_THUMBNAILS_TABLEWIDTH_KEY,nValue,DEFAULT_THUMBNAILS_TABLEWIDTH,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_TABLEWIDTH_KEY,nValue);

			// ThumbnailsThumbUrl
			LoadString(WALLPAPER_THUMBNAILS_THUMBURL_KEY,szValue,sizeof(szValue),DEFAULT_THUMBNAILS_THUMBURL,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_THUMBURL_KEY,szValue);

			// ThumbnailsX
			LoadNumber(WALLPAPER_THUMBNAILS_X_KEY,nValue,DEFAULT_THUMBNAILS_X,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_X_KEY,nValue);

			// ThumbnailsY
			LoadNumber(WALLPAPER_THUMBNAILS_Y_KEY,nValue,DEFAULT_THUMBNAILS_Y,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_Y_KEY,nValue);

			// Timeout
			LoadNumber(WALLPAPER_TIMEOUT_KEY,nValue,DEFAULT_TIMEOUT,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_TIMEOUT_KEY,nValue);

			// TrayIcon
			LoadNumber(WALLPAPER_TRAYICON_KEY,nValue,DEFAULT_TRAYICON,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_TRAYICON_KEY,nValue);

			// WindowPos
			LoadNumber(WALLPAPER_WINDOW_POS_LEFT,nValue,DEFAULT_WINDOW_POS_LEFT,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_WINDOW_POS_LEFT,nValue);
			LoadNumber(WALLPAPER_WINDOW_POS_TOP,nValue,DEFAULT_WINDOW_POS_TOP,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_WINDOW_POS_TOP,nValue);
			LoadNumber(WALLPAPER_WINDOW_POS_RIGHT,nValue,DEFAULT_WINDOW_POS_RIGHT,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_WINDOW_POS_RIGHT,nValue);
			LoadNumber(WALLPAPER_WINDOW_POS_BOTTOM,nValue,DEFAULT_WINDOW_POS_BOTTOM,bLoadDefaults);
			CConfig::Insert(WALLPAPER_OPTIONS_KEY,WALLPAPER_WINDOW_POS_BOTTOM,nValue);

			CConfig::m_pRegistry->Close();
		}
	}

	/*
		WallPaper\\Pictures
	*/
	bLoadSection = lpcszSectionName ? (strcmp(lpcszSectionName,WALLPAPER_PICTURES_KEY)==0 ? TRUE : FALSE) : TRUE;
	if(bLoadSection)
	{
		if((lRet = CConfig::m_pRegistry->Open(HKEY_CURRENT_USER,DEFAULT_WALLPAPER_KEY"\\"WALLPAPER_PICTURES_KEY))!=ERROR_SUCCESS)
			lRet = CConfig::m_pRegistry->Create(HKEY_CURRENT_USER,DEFAULT_WALLPAPER_KEY"\\"WALLPAPER_PICTURES_KEY);
		if(lRet==ERROR_SUCCESS)
		{
			char szKey[REGKEY_MAX_KEY_VALUE+1];

			// Current
			LoadNumber(WALLPAPER_CURRENTPICT_KEY,nValue,DEFAULT_CURRENTPICT,bLoadDefaults);
			CConfig::Insert(WALLPAPER_PICTURES_KEY,WALLPAPER_CURRENTPICT_KEY,nValue);

			// Total
			LoadNumber(WALLPAPER_TOTPICT_KEY,nValue,DEFAULT_TOTPICT,bLoadDefaults);
			CConfig::Insert(WALLPAPER_PICTURES_KEY,WALLPAPER_TOTPICT_KEY,nValue);

			// File[0...n]
			for(int i = 0; i < (int)nValue; i++)
			{
				_snprintf(szKey,sizeof(szKey)-1,"%s%d",WALLPAPER_PICTURE_KEY,i);
				if(ReadString(szKey,szValue,sizeof(szValue)))
					CConfig::Insert(WALLPAPER_PICTURES_KEY,szKey,szValue);
			}

			CConfig::m_pRegistry->Close();
		}
	}

	/*
		WallPaper\\HostNames
	*/
	bLoadSection = lpcszSectionName ? (strcmp(lpcszSectionName,WALLPAPER_HOSTNAMES_KEY)==0 ? TRUE : FALSE) : TRUE;
	if(bLoadSection)
	{
		if((lRet = CConfig::m_pRegistry->Open(HKEY_CURRENT_USER,DEFAULT_WALLPAPER_KEY"\\"WALLPAPER_HOSTNAMES_KEY))!=ERROR_SUCCESS)
			lRet = CConfig::m_pRegistry->Create(HKEY_CURRENT_USER,DEFAULT_WALLPAPER_KEY"\\"WALLPAPER_HOSTNAMES_KEY);
		if(lRet==ERROR_SUCCESS)
		{
			// Host[0...n]
			BOOL bFound = TRUE;
			char szKey[REGKEY_MAX_KEY_VALUE+1];
			for(int i = 0; bFound; i++)
			{
				_snprintf(szKey,sizeof(szKey)-1,"%s%d",WALLPAPER_HOST_KEY,i);
				bFound = ReadString(szKey,szValue,sizeof(szValue));
				if(bFound)
					CConfig::Insert(WALLPAPER_HOSTNAMES_KEY,szKey,szValue);
			}

			CConfig::m_pRegistry->Close();
		}
	}

	/*
		WallPaper\\FileTypeList
	*/
	bLoadSection = lpcszSectionName ? (strcmp(lpcszSectionName,WALLPAPER_FILETYPELIST_KEY)==0 ? TRUE : FALSE) : TRUE;
	if(bLoadSection)
	{
		if((lRet = CConfig::m_pRegistry->Open(HKEY_CURRENT_USER,DEFAULT_WALLPAPER_KEY"\\"WALLPAPER_FILETYPELIST_KEY))!=ERROR_SUCCESS)
			lRet = CConfig::m_pRegistry->Create(HKEY_CURRENT_USER,DEFAULT_WALLPAPER_KEY"\\"WALLPAPER_FILETYPELIST_KEY);
		if(lRet==ERROR_SUCCESS)
		{
			// Type[0...n]
			int i;
			BOOL bFound = TRUE;
			BOOL bEmpty = TRUE;
			char szKey[REGKEY_MAX_KEY_VALUE+1];
			
			for(i = 0; bFound; i++)
			{
				_snprintf(szKey,sizeof(szKey)-1,"%s%d",WALLPAPER_FILETYPE_KEY,i);
				bFound = ReadString(szKey,szValue,sizeof(szValue));
				if(bFound)
				{
					CConfig::Insert(WALLPAPER_FILETYPELIST_KEY,szKey,szValue);
					if(bEmpty)
						bEmpty = FALSE;
				}
			}
			
			if(bEmpty && !bLoadDefaults)
			{
				char szDefaultValue[REGKEY_MAX_KEY_VALUE+1];
				char szLibraryName[_MAX_PATH+1];
				strcpyn(szLibraryName,CConfig::GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_CURRENTLIBRARY_KEY),sizeof(szLibraryName));
				CImageFactory ImageFactory;
				CImage* pImage = ImageFactory.Create(szLibraryName,sizeof(szLibraryName));
				LPIMAGETYPE p;

				for(i = 0; (p = pImage->EnumReadableImageFormats())!=(LPIMAGETYPE)NULL; i++)
				{
					_snprintf(szKey,sizeof(szKey)-1,"%s%d",WALLPAPER_FILETYPE_KEY,i);
					strcpyn(szValue,p->ext,sizeof(szValue));
					strcpyn(szDefaultValue,p->ext,sizeof(szDefaultValue));
					LoadString(szKey,szValue,sizeof(szValue),szDefaultValue,bLoadDefaults);
					CConfig::Insert(WALLPAPER_FILETYPELIST_KEY,szKey,szValue);
				}

				// idem in WallPaperCrawlerSettingsFileTypesDlg.cpp
				DIRTY_DECLARATION_FOR_FILE_TYPES(szExtraExtArray)
				
				for(int n = 0; szExtraExtArray[n]; n++,i++)
				{
					_snprintf(szKey,sizeof(szKey)-1,"%s%d",WALLPAPER_FILETYPE_KEY,i);
					strcpyn(szValue,szExtraExtArray[n],sizeof(szValue));
					strcpyn(szDefaultValue,szExtraExtArray[n],sizeof(szDefaultValue));
					LoadString(szKey,szValue,sizeof(szValue),szDefaultValue,bLoadDefaults);
					CConfig::Insert(WALLPAPER_FILETYPELIST_KEY,szKey,szValue);
				}
			}

			CConfig::m_pRegistry->Close();
		}
	}

	/*
		WallPaper\\ParentIncludeList
	*/
	bLoadSection = lpcszSectionName ? (strcmp(lpcszSectionName,WALLPAPER_PARENTINCLUDELIST_KEY)==0 ? TRUE : FALSE) : TRUE;
	if(bLoadSection)
	{
		if((lRet = CConfig::m_pRegistry->Open(HKEY_CURRENT_USER,DEFAULT_WALLPAPER_KEY"\\"WALLPAPER_PARENTINCLUDELIST_KEY))!=ERROR_SUCCESS)
			lRet = CConfig::m_pRegistry->Create(HKEY_CURRENT_USER,DEFAULT_WALLPAPER_KEY"\\"WALLPAPER_PARENTINCLUDELIST_KEY);
		if(lRet==ERROR_SUCCESS)
		{
			// Include[0...n]
			BOOL bFound = TRUE;
			char szKey[REGKEY_MAX_KEY_VALUE+1];
			for(int i = 0; bFound; i++)
			{
				_snprintf(szKey,sizeof(szKey)-1,"%s%d",WALLPAPER_PARENTINCLUDE_KEY,i);
				bFound = ReadString(szKey,szValue,sizeof(szValue));
				if(bFound)
					CConfig::Insert(WALLPAPER_PARENTINCLUDELIST_KEY,szKey,szValue);
			}

			CConfig::m_pRegistry->Close();
		}
	}

	/*
		WallPaper\\ParentExcludeList
	*/
	bLoadSection = lpcszSectionName ? (strcmp(lpcszSectionName,WALLPAPER_PARENTEXCLUDELIST_KEY)==0 ? TRUE : FALSE) : TRUE;
	if(bLoadSection)
	{
		if((lRet = CConfig::m_pRegistry->Open(HKEY_CURRENT_USER,DEFAULT_WALLPAPER_KEY"\\"WALLPAPER_PARENTEXCLUDELIST_KEY))!=ERROR_SUCCESS)
			lRet = CConfig::m_pRegistry->Create(HKEY_CURRENT_USER,DEFAULT_WALLPAPER_KEY"\\"WALLPAPER_PARENTEXCLUDELIST_KEY);
		if(lRet==ERROR_SUCCESS)
		{
			// Exclude[0...n]
			BOOL bFound = TRUE;
			char szKey[REGKEY_MAX_KEY_VALUE+1];
			for(int i = 0; bFound; i++)
			{
				_snprintf(szKey,sizeof(szKey)-1,"%s%d",WALLPAPER_PARENTEXCLUDE_KEY,i);
				bFound = ReadString(szKey,szValue,sizeof(szValue));
				if(bFound)
					CConfig::Insert(WALLPAPER_PARENTEXCLUDELIST_KEY,szKey,szValue);
			}

			CConfig::m_pRegistry->Close();
		}
	}

	/*
		WallPaper\\DomainIncludeList
	*/
	bLoadSection = lpcszSectionName ? (strcmp(lpcszSectionName,WALLPAPER_DOMAININCLUDELIST_KEY)==0 ? TRUE : FALSE) : TRUE;
	if(bLoadSection)
	{
		if((lRet = CConfig::m_pRegistry->Open(HKEY_CURRENT_USER,DEFAULT_WALLPAPER_KEY"\\"WALLPAPER_DOMAININCLUDELIST_KEY))!=ERROR_SUCCESS)
			lRet = CConfig::m_pRegistry->Create(HKEY_CURRENT_USER,DEFAULT_WALLPAPER_KEY"\\"WALLPAPER_DOMAININCLUDELIST_KEY);
		if(lRet==ERROR_SUCCESS)
		{
			// Include[0...n]
			BOOL bFound = TRUE;
			char szKey[REGKEY_MAX_KEY_VALUE+1];
			for(int i = 0; bFound; i++)
			{
				_snprintf(szKey,sizeof(szKey)-1,"%s%d",WALLPAPER_DOMAININCLUDE_KEY,i);
				bFound = ReadString(szKey,szValue,sizeof(szValue));
				if(bFound)
					CConfig::Insert(WALLPAPER_DOMAININCLUDELIST_KEY,szKey,szValue);
			}

			CConfig::m_pRegistry->Close();
		}
	}

	/*
		WallPaper\\DomainExcludeList
	*/
	bLoadSection = lpcszSectionName ? (strcmp(lpcszSectionName,WALLPAPER_DOMAINEXCLUDELIST_KEY)==0 ? TRUE : FALSE) : TRUE;
	if(bLoadSection)
	{
		if((lRet = CConfig::m_pRegistry->Open(HKEY_CURRENT_USER,DEFAULT_WALLPAPER_KEY"\\"WALLPAPER_DOMAINEXCLUDELIST_KEY))!=ERROR_SUCCESS)
			lRet = CConfig::m_pRegistry->Create(HKEY_CURRENT_USER,DEFAULT_WALLPAPER_KEY"\\"WALLPAPER_DOMAINEXCLUDELIST_KEY);
		if(lRet==ERROR_SUCCESS)
		{
			// Exclude[0...n]
			BOOL bFound = TRUE;
			char szKey[REGKEY_MAX_KEY_VALUE+1];
			for(int i = 0; bFound; i++)
			{
				_snprintf(szKey,sizeof(szKey)-1,"%s%d",WALLPAPER_DOMAINEXCLUDE_KEY,i);
				bFound = ReadString(szKey,szValue,sizeof(szValue));
				if(bFound)
					CConfig::Insert(WALLPAPER_DOMAINEXCLUDELIST_KEY,szKey,szValue);
			}

			CConfig::m_pRegistry->Close();
		}
	}

	/*
		WallPaper\\WildcardsExcludeList
	*/
	bLoadSection = lpcszSectionName ? (strcmp(lpcszSectionName,WALLPAPER_WILDCARDSEXCLUDELIST_KEY)==0 ? TRUE : FALSE) : TRUE;
	if(bLoadSection)
	{
		if((lRet = CConfig::m_pRegistry->Open(HKEY_CURRENT_USER,DEFAULT_WALLPAPER_KEY"\\"WALLPAPER_WILDCARDSEXCLUDELIST_KEY))!=ERROR_SUCCESS)
			lRet = CConfig::m_pRegistry->Create(HKEY_CURRENT_USER,DEFAULT_WALLPAPER_KEY"\\"WALLPAPER_WILDCARDSEXCLUDELIST_KEY);
		if(lRet==ERROR_SUCCESS)
		{
			// Exclude[0...n]
			int i;
			BOOL bFound = TRUE;
			BOOL bEmpty = TRUE;
			char szKey[REGKEY_MAX_KEY_VALUE+1];

			for(i = 0; bFound; i++)
			{
				_snprintf(szKey,sizeof(szKey)-1,"%s%d",WALLPAPER_WILDCARDSEXCLUDE_KEY,i);
				bFound = ReadString(szKey,szValue,sizeof(szValue));
				if(bFound)
				{
					CConfig::Insert(WALLPAPER_WILDCARDSEXCLUDELIST_KEY,szKey,szValue);
					if(bEmpty)
						bEmpty = FALSE;
				}
			}

			CConfig::m_pRegistry->Close();
		}
	}

	CConfig::m_pRegistry->Detach();

	CConfig::SetModified(FALSE);
}

/*
	ReadString()
*/
BOOL CWallPaperConfig::ReadString(LPCSTR lpcszKey,LPSTR lpszValue,int nSize)
{
	// ricava dal registro il valore stringa associato alla chiave
	memset(lpszValue,'\0',nSize);
	DWORD valuesize = nSize;

	return(CConfig::m_pRegistry->QueryValue(lpszValue,lpcszKey,&valuesize)==ERROR_SUCCESS);
}

/*
	ReadNumber()
*/
BOOL CWallPaperConfig::ReadNumber(LPCSTR lpcszKey,DWORD& dwValue)
{
	// ricava dal registro il valore numerico associato alla chiave
	dwValue = 0L;

	return(CConfig::m_pRegistry->QueryValue(dwValue,lpcszKey)==ERROR_SUCCESS);
}

/*
	LoadString()
*/
void CWallPaperConfig::LoadString(LPCSTR lpcszKey,LPSTR lpszValue,int nSize,LPCSTR lpcszDefaultValue,BOOL bLoadDefaults)
{
	// ricava dal registro il valore stringa associato alla chiave, creandola con il valore di
	// default se non esiste.
	memset(lpszValue,'\0',nSize);
	DWORD valuesize = nSize;

	if(!bLoadDefaults)
		bLoadDefaults = CConfig::m_pRegistry->QueryValue(lpszValue,lpcszKey,&valuesize)!=ERROR_SUCCESS;
	if(bLoadDefaults)
	{
		strcpyn(lpszValue,lpcszDefaultValue,nSize);
		CConfig::m_pRegistry->SetValue(lpszValue,lpcszKey);
	}
}

/*
	LoadNumber()
*/
void CWallPaperConfig::LoadNumber(LPCSTR lpcszKey,DWORD& dwValue,DWORD dwDefaultValue,BOOL bLoadDefaults)
{
	// ricava dal registro il valore numerico associato alla chiave, creandola con il valore di
	// default se non esiste
	dwValue = 0L;

	if(!bLoadDefaults)
		bLoadDefaults = CConfig::m_pRegistry->QueryValue(dwValue,lpcszKey)!=ERROR_SUCCESS;
	if(bLoadDefaults)
	{
		dwValue = dwDefaultValue;
		CConfig::m_pRegistry->SetValue(dwValue,lpcszKey);
	}
}
