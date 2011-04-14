/*
	WallBrowserConfig.h
	Classe per la gestione della configurazione.
	Luca Piergentili, 04/08/01
	lpiergentili@yahoo.com

	WallBrowser - the smart picture browser
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
#ifndef _WALLBROWSERCONFIG_H
#define _WALLBROWSERCONFIG_H 1

#include "window.h"
#include "CConfig.h"
#include "CImageDraw.h"
#include "CImageFactory.h"
#include "WallPaperVersion.h"
#include "WallPaperVersion.h"
#include "WallBrowserVersion.h"

// chiavi registro
#define WALLBROWSER_KEY						WALLBROWSER_PROGRAM_NAME
#define DEFAULT_WALLBROWSER_KEY				DEFAULT_REG_KEY"\\"WALLBROWSER_KEY

// Install
#define DEFAULT_DIR							"C:\\"WALLPAPER_PROGRAM_NAME"\\"
#define DEFAULT_PROGRAM						DEFAULT_DIR""WALLBROWSER_PROGRAM_NAME".exe"
#define DEFAULT_WEBSITE						WALLPAPER_WEB_SITE
#define DEFAULT_EMAILADDRESS					WALLPAPER_EMAIL_ADDRESS
#define DEFAULT_HOMEPAGE						WALLPAPER_WEB_SITE

#define WALLBROWSER_INSTALL_KEY				"Install"
#define WALLBROWSER_DIR_KEY						"Directory"
#define WALLBROWSER_PROGRAM_KEY					"Program"
#define WALLBROWSER_WEBSITE_KEY					"WebSite"
#define WALLBROWSER_HOMEPAGE_KEY					"HomePage"
#define WALLBROWSER_EMAILADDRESS_KEY				"EmailAddress"

// Options
#define DEFAULT_DRAWMODE							DRAW_STRETCHDIBITS
#define DEFAULT_CONFIRMFILEDELETE					1
#define DEFAULT_DELETEFILESTORECYCLEBIN				1
#define DEFAULT_AUTOSIZECOLUMNS					1
#define DEFAULT_CURRENTLIBRARY					IMAGE_DEFAULT_LIBRARY
#define DEFAULT_USEASDEFAULTLIBRARY				1
#define DEFAULT_LASTSAVEAS						"."
#define DEFAULT_FILESPLITTER						100
#define DEFAULT_DIRSPLITTER						200
#define DEFAULT_LAYERED							0
#define DEFAULT_LAYEREDVALUE						70
#define DEFAULT_LASTBROWSEDIR						"C:\\"
#define DEFAULT_LASTMOVETODIR						"C:\\"
#define VIEWTYPE_STRETCH							0
#define VIEWTYPE_SCROLL							1
#define DEFAULT_VIEWTYPE							VIEWTYPE_SCROLL
#define DEFAULT_WINDOWPLACEMENT_SHOW				0
#define DEFAULT_WINDOWPLACEMENT_LENGTH				0
#define DEFAULT_WINDOWPLACEMENT_FLAGS				0
#define DEFAULT_WINDOWPLACEMENT_SHOWCMD				0
#define DEFAULT_WINDOWPLACEMENT_MINPOSX				0
#define DEFAULT_WINDOWPLACEMENT_MINPOSY				0
#define DEFAULT_WINDOWPLACEMENT_MAXPOSX				0
#define DEFAULT_WINDOWPLACEMENT_MAXPOSY				0
#define DEFAULT_WINDOWPLACEMENT_NPOSLEFT			0
#define DEFAULT_WINDOWPLACEMENT_NPOSTOP				0
#define DEFAULT_WINDOWPLACEMENT_NPOSRIGHT			0
#define DEFAULT_WINDOWPLACEMENT_NPOSBOTTOM			0

#define WALLBROWSER_OPTIONS_KEY				"Options"
#define WALLBROWSER_DRAWMODE_KEY					"DrawMode"
#define WALLBROWSER_CONFIRMFILEDELETE_KEY			"ConfirmFileDelete"
#define WALLBROWSER_DELETEFILESTORECYCLEBIN_KEY		"DeleteFilesToRecycleBin"
#define WALLBROWSER_AUTOSIZECOLUMNS_KEY				"AutoSizeColumns"
#define WALLBROWSER_CURRENTLIBRARY_KEY				"CurrentLibrary"
#define WALLBROWSER_USEASDEFAULTLIBRARY_KEY			"UseAsDefaultLibrary"
#define WALLBROWSER_LASTSAVEAS_KEY					"LastSaveAs"
#define WALLBROWSER_FILESPLITTER_KEY				"FileSplitter"
#define WALLBROWSER_DIRSPLITTER_KEY				"DirSplitter"
#define WALLBROWSER_LAYERED_KEY					"Layered"
#define WALLBROWSER_LAYEREDVALUE_KEY				"LayeredValue"
#define WALLBROWSER_LASTBROWSEDIR_KEY				"LastBrowseDir"
#define WALLBROWSER_LASTMOVETODIR_KEY				"LastMoveToDir"
#define WALLBROWSER_VIEWTYPE_KEY					"ViewType"
#define WALLBROWSER_WINDOWPLACEMENT_SHOW_KEY			"WindowPlacementShow"
#define WALLBROWSER_WINDOWPLACEMENT_FLAGS_KEY		"WindowPlacementFlags"
#define WALLBROWSER_WINDOWPLACEMENT_SHOWCMD_KEY		"WindowPlacementShowCmd"
#define WALLBROWSER_WINDOWPLACEMENT_MINPOSX_KEY		"WindowPlacementMinPosX"
#define WALLBROWSER_WINDOWPLACEMENT_MINPOSY_KEY		"WindowPlacementMinPosY"
#define WALLBROWSER_WINDOWPLACEMENT_MAXPOSX_KEY		"WindowPlacementMaxPosX"
#define WALLBROWSER_WINDOWPLACEMENT_MAXPOSY_KEY		"WindowPlacementMaxPosY"
#define WALLBROWSER_WINDOWPLACEMENT_NPOSLEFT_KEY		"WindowPlacementNPosLeft"
#define WALLBROWSER_WINDOWPLACEMENT_NPOSTOP_KEY		"WindowPlacementNPosTop"
#define WALLBROWSER_WINDOWPLACEMENT_NPOSRIGHT_KEY		"WindowPlacementNPosRight"
#define WALLBROWSER_WINDOWPLACEMENT_NPOSBOTTOM_KEY	"WindowPlacementNPosBottom"

/*
	CWallBrowserConfig
*/
class CWallBrowserConfig : public CConfig
{
public:
	CWallBrowserConfig() {CWallBrowserConfig::Load();}
	~CWallBrowserConfig() {}

	// salva/carica dal file
	inline BOOL	Export		(LPCSTR lpcszFileName)						{return(CConfig::Export(WALLBROWSER_KEY,lpcszFileName));}
	inline BOOL	Import		(LPCSTR lpcszFileName)						{return(CConfig::Import(WALLBROWSER_KEY,lpcszFileName));}

	// salva la lista nel registro
	inline void	Save			(void)									{CConfig::Save(WALLBROWSER_KEY);}

	// azzera il contenuto della lista
	inline void	DeleteAll		(void)									{CConfig::DeleteAll(WALLBROWSER_KEY);}
		
	// salva la chiave della lista nel registro
	inline void	SaveKey		(LPCSTR lpcszSectionName,LPCSTR lpcszKeyName)	{CConfig::SaveKey(WALLBROWSER_KEY,lpcszSectionName,lpcszKeyName);}
	inline void	SaveNumber	(LPCSTR lpcszSectionName,LPCSTR lpcszKeyName)	{SaveKey(lpcszSectionName,lpcszKeyName);}
	inline void	SaveString	(LPCSTR lpcszSectionName,LPCSTR lpcszKeyName)	{SaveKey(lpcszSectionName,lpcszKeyName);}
	
	// salva la sezione della lista nel registro
	inline void	SaveSection	(LPCSTR lpcszSectionName)					{CConfig::SaveSection(WALLBROWSER_KEY,lpcszSectionName);}
	
	// carica la lista dal registro
	void			Load			(LPCSTR lpcszSectionName = NULL,BOOL bLoadDefaults = FALSE);

	// ricarica la sezione della lista dal registro
	void			ReloadSection	(LPCSTR lpcszSectionName);

	// elimina la sezione della lista dal registro
	void			DeleteSection	(LPCSTR lpcszSectionName);

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

#endif // _WALLBROWSERCONFIG_H
