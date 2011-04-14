/*
	WallBrowserConfig.cpp
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
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include "window.h"
#include "CConfig.h"
#include "CImageFactory.h"
#include "WallBrowserConfig.h"
#include "WallPaperVersion.h"

#include "traceexpr.h"
//#define _TRACE_FLAG	_TRFLAG_TRACEOUTPUT
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
	ReloadSection()

	Ricarica la sezione della lista dal registro.
*/
void CWallBrowserConfig::ReloadSection(LPCSTR lpcszSectionName)
{
	CConfig::DeleteSection(WALLBROWSER_KEY,lpcszSectionName,FALSE);
	CWallBrowserConfig::Load(lpcszSectionName);
	CConfig::SetModified(TRUE);
}

/*
	DeleteSection()

	Elimina la sezione della lista dal registro.
*/
void CWallBrowserConfig::DeleteSection(LPCSTR lpcszSectionName)
{
	CConfig::DeleteSection(WALLBROWSER_KEY,lpcszSectionName,FALSE);
	CConfig::SetModified(TRUE);
}

/*
	Load()

	Carica nella lista i valori di configurazione presenti nel registro.
*/
void CWallBrowserConfig::Load(LPCSTR lpcszSectionName/*=NULL*/,BOOL bLoadDefaults/*=FALSE*/)
{
	BOOL bLoadSection;
	LONG lRet = 0;
	DWORD nValue = 0L;
	char szValue[REGKEY_MAX_KEY_VALUE+1] = {0};

	CConfig::m_pRegistry->Attach(HKEY_CURRENT_USER);
	
	/*
		WallPaper
	*/
	if((lRet = CConfig::m_pRegistry->Open(HKEY_CURRENT_USER,DEFAULT_WALLBROWSER_KEY))!=ERROR_SUCCESS)
		lRet = CConfig::m_pRegistry->Create(HKEY_CURRENT_USER,DEFAULT_WALLBROWSER_KEY);
	if(lRet==ERROR_SUCCESS)
		CConfig::m_pRegistry->Close();

	/*
		WallPaper\\Install
	*/
	bLoadSection = lpcszSectionName ? (strcmp(lpcszSectionName,WALLBROWSER_INSTALL_KEY)==0 ? TRUE : FALSE) : TRUE;
	if(bLoadSection)
	{
		if((lRet = CConfig::m_pRegistry->Open(HKEY_CURRENT_USER,DEFAULT_WALLBROWSER_KEY"\\"WALLBROWSER_INSTALL_KEY))!=ERROR_SUCCESS)
			lRet = CConfig::m_pRegistry->Create(HKEY_CURRENT_USER,DEFAULT_WALLBROWSER_KEY"\\"WALLBROWSER_INSTALL_KEY);
		if(lRet==ERROR_SUCCESS)
		{
			// Directory
			LoadString(WALLBROWSER_DIR_KEY,szValue,sizeof(szValue),DEFAULT_DIR,bLoadDefaults);
			CConfig::Insert(WALLBROWSER_INSTALL_KEY,WALLBROWSER_DIR_KEY,szValue);

			// Program
			LoadString(WALLBROWSER_PROGRAM_KEY,szValue,sizeof(szValue),DEFAULT_PROGRAM,bLoadDefaults);
			CConfig::Insert(WALLBROWSER_INSTALL_KEY,WALLBROWSER_PROGRAM_KEY,szValue);

			// WebSite
			LoadString(WALLBROWSER_WEBSITE_KEY,szValue,sizeof(szValue),DEFAULT_WEBSITE,bLoadDefaults);
			CConfig::Insert(WALLBROWSER_INSTALL_KEY,WALLBROWSER_WEBSITE_KEY,szValue);

			// HomePage
			LoadString(WALLBROWSER_HOMEPAGE_KEY,szValue,sizeof(szValue),DEFAULT_HOMEPAGE,bLoadDefaults);
			CConfig::Insert(WALLBROWSER_INSTALL_KEY,WALLBROWSER_HOMEPAGE_KEY,szValue);

			// EmailAddress
			LoadString(WALLBROWSER_EMAILADDRESS_KEY,szValue,sizeof(szValue),DEFAULT_EMAILADDRESS,bLoadDefaults);
			CConfig::Insert(WALLBROWSER_INSTALL_KEY,WALLBROWSER_EMAILADDRESS_KEY,szValue);

			CConfig::m_pRegistry->Close();
		}
	}

	/*
		WallPaper\\Options
	*/
	bLoadSection = lpcszSectionName ? (strcmp(lpcszSectionName,WALLBROWSER_OPTIONS_KEY)==0 ? TRUE : FALSE) : TRUE;
	if(bLoadSection)
	{
		if((lRet = CConfig::m_pRegistry->Open(HKEY_CURRENT_USER,DEFAULT_WALLBROWSER_KEY"\\"WALLBROWSER_OPTIONS_KEY))!=ERROR_SUCCESS)
			lRet = CConfig::m_pRegistry->Create(HKEY_CURRENT_USER,DEFAULT_WALLBROWSER_KEY"\\"WALLBROWSER_OPTIONS_KEY);
		if(lRet==ERROR_SUCCESS)
		{
			// DrawMode
			LoadNumber(WALLBROWSER_DRAWMODE_KEY,nValue,DEFAULT_DRAWMODE,bLoadDefaults);
			CConfig::Insert(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_DRAWMODE_KEY,nValue);

			// ConfirmFileDelete
			LoadNumber(WALLBROWSER_CONFIRMFILEDELETE_KEY,nValue,DEFAULT_CONFIRMFILEDELETE,bLoadDefaults);
			CConfig::Insert(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_CONFIRMFILEDELETE_KEY,nValue);

			// DeleteFilesToRecycleBin
			LoadNumber(WALLBROWSER_DELETEFILESTORECYCLEBIN_KEY,nValue,DEFAULT_DELETEFILESTORECYCLEBIN,bLoadDefaults);
			CConfig::Insert(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_DELETEFILESTORECYCLEBIN_KEY,nValue);

			// AutoSizeColumns
			LoadNumber(WALLBROWSER_AUTOSIZECOLUMNS_KEY,nValue,DEFAULT_AUTOSIZECOLUMNS,bLoadDefaults);
			CConfig::Insert(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_AUTOSIZECOLUMNS_KEY,nValue);

			// CurrentLibrary
			LoadString(WALLBROWSER_CURRENTLIBRARY_KEY,szValue,sizeof(szValue),DEFAULT_CURRENTLIBRARY,bLoadDefaults);
			CConfig::Insert(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_CURRENTLIBRARY_KEY,szValue);

			// UseAsDefaultLibrary
			LoadNumber(WALLBROWSER_USEASDEFAULTLIBRARY_KEY,nValue,DEFAULT_USEASDEFAULTLIBRARY,bLoadDefaults);
			CConfig::Insert(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_USEASDEFAULTLIBRARY_KEY,nValue);

			// Layered
			LoadNumber(WALLBROWSER_LAYERED_KEY,nValue,DEFAULT_LAYERED,bLoadDefaults);
			CConfig::Insert(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_LAYERED_KEY,nValue);

			// LayeredValue
			LoadNumber(WALLBROWSER_LAYEREDVALUE_KEY,nValue,DEFAULT_LAYEREDVALUE,bLoadDefaults);
			CConfig::Insert(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_LAYEREDVALUE_KEY,nValue);

			// LastBrowseDir
			LoadString(WALLBROWSER_LASTBROWSEDIR_KEY,szValue,sizeof(szValue),DEFAULT_LASTBROWSEDIR,bLoadDefaults);
			CConfig::Insert(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_LASTBROWSEDIR_KEY,szValue);

			// LastMoveToDir
			LoadString(WALLBROWSER_LASTMOVETODIR_KEY,szValue,sizeof(szValue),DEFAULT_LASTMOVETODIR,bLoadDefaults);
			CConfig::Insert(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_LASTMOVETODIR_KEY,szValue);

			// LastSaveAs
			LoadString(WALLBROWSER_LASTSAVEAS_KEY,szValue,sizeof(szValue),DEFAULT_LASTSAVEAS,bLoadDefaults);
			CConfig::Insert(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_LASTSAVEAS_KEY,szValue);

			// ViewType
			LoadNumber(WALLBROWSER_VIEWTYPE_KEY,nValue,DEFAULT_VIEWTYPE,bLoadDefaults);
			CConfig::Insert(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_VIEWTYPE_KEY,nValue);

			// FileSplitter
			LoadNumber(WALLBROWSER_FILESPLITTER_KEY,nValue,DEFAULT_FILESPLITTER,bLoadDefaults);
			CConfig::Insert(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_FILESPLITTER_KEY,nValue);

			// DirSplitter
			LoadNumber(WALLBROWSER_DIRSPLITTER_KEY,nValue,DEFAULT_DIRSPLITTER,bLoadDefaults);
			CConfig::Insert(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_DIRSPLITTER_KEY,nValue);

			// WindowPlacementShow
			LoadNumber(WALLBROWSER_WINDOWPLACEMENT_SHOW_KEY,nValue,DEFAULT_WINDOWPLACEMENT_SHOW,bLoadDefaults);
			CConfig::Insert(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_WINDOWPLACEMENT_SHOW_KEY,nValue);

			// WindowPlacementFlags
			LoadNumber(WALLBROWSER_WINDOWPLACEMENT_FLAGS_KEY,nValue,DEFAULT_WINDOWPLACEMENT_FLAGS,bLoadDefaults);
			CConfig::Insert(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_WINDOWPLACEMENT_FLAGS_KEY,nValue);

			// WindowPlacementShowCmd
			LoadNumber(WALLBROWSER_WINDOWPLACEMENT_SHOWCMD_KEY,nValue,DEFAULT_WINDOWPLACEMENT_SHOWCMD,bLoadDefaults);
			CConfig::Insert(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_WINDOWPLACEMENT_SHOWCMD_KEY,nValue);

			// WindowPlacementMinPosX
			LoadNumber(WALLBROWSER_WINDOWPLACEMENT_MINPOSX_KEY,nValue,DEFAULT_WINDOWPLACEMENT_MINPOSX,bLoadDefaults);
			CConfig::Insert(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_WINDOWPLACEMENT_MINPOSX_KEY,nValue);

			// WindowPlacementMinPosY
			LoadNumber(WALLBROWSER_WINDOWPLACEMENT_MINPOSY_KEY,nValue,DEFAULT_WINDOWPLACEMENT_MINPOSY,bLoadDefaults);
			CConfig::Insert(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_WINDOWPLACEMENT_MINPOSY_KEY,nValue);

			// WindowPlacementMaxPosX
			LoadNumber(WALLBROWSER_WINDOWPLACEMENT_MAXPOSX_KEY,nValue,DEFAULT_WINDOWPLACEMENT_MAXPOSX,bLoadDefaults);
			CConfig::Insert(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_WINDOWPLACEMENT_MAXPOSX_KEY,nValue);

			// WindowPlacementMaxPosY
			LoadNumber(WALLBROWSER_WINDOWPLACEMENT_MAXPOSY_KEY,nValue,DEFAULT_WINDOWPLACEMENT_MAXPOSY,bLoadDefaults);
			CConfig::Insert(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_WINDOWPLACEMENT_MAXPOSY_KEY,nValue);

			// WindowPlacementNPosLeft
			LoadNumber(WALLBROWSER_WINDOWPLACEMENT_NPOSLEFT_KEY,nValue,DEFAULT_WINDOWPLACEMENT_NPOSLEFT,bLoadDefaults);
			CConfig::Insert(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_WINDOWPLACEMENT_NPOSLEFT_KEY,nValue);

			// WindowPlacementNPosTop
			LoadNumber(WALLBROWSER_WINDOWPLACEMENT_NPOSTOP_KEY,nValue,DEFAULT_WINDOWPLACEMENT_NPOSTOP,bLoadDefaults);
			CConfig::Insert(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_WINDOWPLACEMENT_NPOSTOP_KEY,nValue);

			// WindowPlacementNPosRight
			LoadNumber(WALLBROWSER_WINDOWPLACEMENT_NPOSRIGHT_KEY,nValue,DEFAULT_WINDOWPLACEMENT_NPOSRIGHT,bLoadDefaults);
			CConfig::Insert(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_WINDOWPLACEMENT_NPOSRIGHT_KEY,nValue);

			// WindowPlacementNPosBottom
			LoadNumber(WALLBROWSER_WINDOWPLACEMENT_NPOSBOTTOM_KEY,nValue,DEFAULT_WINDOWPLACEMENT_NPOSBOTTOM,bLoadDefaults);
			CConfig::Insert(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_WINDOWPLACEMENT_NPOSBOTTOM_KEY,nValue);

			CConfig::m_pRegistry->Close();
		}
	}

	CConfig::m_pRegistry->Detach();

	CConfig::SetModified(FALSE);
}

/*
	ReadString()

	Ricava dal registro il valore stringa associato alla chiave.
*/
BOOL CWallBrowserConfig::ReadString(LPCSTR lpcszKey,LPSTR lpszValue,int nSize)
{
	memset(lpszValue,'\0',nSize);
	DWORD valuesize = nSize;
	return(CConfig::m_pRegistry->QueryValue(lpszValue,lpcszKey,&valuesize)==ERROR_SUCCESS);
}

/*
	ReadNumber()

	Ricava dal registro il valore numerico associato alla chiave.
*/
BOOL CWallBrowserConfig::ReadNumber(LPCSTR lpcszKey,DWORD& dwValue)
{
	dwValue = 0L;
	return(CConfig::m_pRegistry->QueryValue(dwValue,lpcszKey)==ERROR_SUCCESS);
}

/*
	LoadString()

	Ricava dal registro il valore stringa associato alla chiave, creandola con il valore di default se non esiste.
*/
void CWallBrowserConfig::LoadString(LPCSTR lpcszKey,LPSTR lpszValue,int nSize,LPCSTR lpcszDefaultValue,BOOL bLoadDefaults)
{
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

	Ricava dal registro il valore numerico associato alla chiave, creandola con il valore di default se non esiste.
*/
void CWallBrowserConfig::LoadNumber(LPCSTR lpcszKey,DWORD& dwValue,DWORD dwDefaultValue,BOOL bLoadDefaults)
{
	dwValue = 0L;

	if(!bLoadDefaults)
		bLoadDefaults = CConfig::m_pRegistry->QueryValue(dwValue,lpcszKey)!=ERROR_SUCCESS;
	if(bLoadDefaults)
	{
		dwValue = dwDefaultValue;
		CConfig::m_pRegistry->SetValue(dwValue,lpcszKey);
	}
}
