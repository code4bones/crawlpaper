/*
	WallPaperSFX.cpp
	Eseguibile sfx per installazione WallPaper (SDK).
	Luca Piergentili, 24/08/00
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
#include <io.h>
#include <direct.h>
#include <string.h>
#include "strings.h"
#define STRICT 1
#include <windows.h>
#include <windowsx.h>
#include "win32api.h"
#include <commctrl.h>
#include <shlobj.h>
#include "resource.h"
#include "gzwhdr.h"
#include "CProgressBar.h"
#include "CFindFile.h"
#include "CDirDialog.h"
#include "CSEFileInfo.h"
#include "CSelfExtractor.h"
#include "WallBrowserVersion.h"
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

// macro
#define WALLPAPERSFX_DEFAULT_INSTALL_FOLDER	WALLPAPER_PROGRAM_NAME
#define WALLPAPERSFX_PROGRAM_TITLE			" "WALLPAPER_PROGRAM_NAME" Self Extractor Installer "

// prototipi
int WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR,int);

/*
	CDialog
	Classe base per il dialogo.
*/
class CDialog
{
public:
	CDialog(	HINSTANCE hInstance = NULL,
			UINT nTemplateID = (UINT)-1,
			UINT nIconID = (UINT)-1,
			LPCSTR lpcszTitle = NULL
			);

	virtual ~CDialog() {}
	
	UINT DoModal(void);

	// inizializza, se ritorna FALSE il dialogo non viene visualizzato
	virtual BOOL OnInitDialog(void) {return(TRUE);}

	// gestori comandi
	virtual void OnCommand(int nID,HWND hWndCtrl,UINT uiNotify) = 0;

	// ricava l'handle del controllo
	HWND GetDlgItem(UINT nID) {return(::GetDlgItem(m_hWnd,nID));}

	// chiude il dialogo
	void EndDialog(UINT nRet) {::EndDialog(m_hWnd,nRet);}

protected:
	// per il passaggio dei parametri
	typedef struct Param {
		HWND   hWnd;
		UINT   uMsg;
		WPARAM wParam;
		LPARAM lParam;
	};

	HINSTANCE	m_hInstance;	// handle dell'istanza
	HWND		m_hWnd;		// handle del dialogo
	UINT		m_nTemplateID;	// id del dialogo (resource)
	UINT		m_nIconID;	// id dell'icona (resource)
	LPCSTR	m_lpcszTitle;	// titolo del dialogo

private:
	// inizializza il dialogo e chiama la virtuale
	BOOL OnInitDialog(HWND hwndFocus,LPARAM lParam);

	// per processare i messaggi di sistema (WM_...), chiama il membro non statico
	// chiama il gestore OnCommand() per i messaggi (deve essere definito nella classe derivata)
	static BOOL CALLBACK __stdcall DialogProcedure(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
	BOOL DialogProcedure(Param* p);
};

/*
	CDialog()
*/
CDialog::CDialog(HINSTANCE hInstance,UINT nTemplateID,UINT nIconID,LPCSTR lpcszTitle)
{
	m_hInstance   = hInstance;
	m_hWnd        = NULL;
	m_nTemplateID = nTemplateID;
	m_nIconID     = nIconID;
	m_lpcszTitle  = lpcszTitle;
}

/*
	DoModal()
*/
UINT CDialog::DoModal(void)
{
	// da inizializzare col costruttore
	if(m_hInstance==NULL || m_nTemplateID==(UINT)-1 || m_nIconID==(UINT)-1)
		return(IDCANCEL);
	
	// chiama la DialogBoxParam(), non la DialogBox(), per gestire la callback C++
	return(::DialogBoxParam(m_hInstance,MAKEINTRESOURCE(m_nTemplateID),(HWND)NULL,this->DialogProcedure,(LPARAM)this));
}

/*
	DialogProcedure()

	Callback per la gestione dei messaggi (come WndProc() per le finestre).
	Con la chiamata a DialogBoxParam() riceve il puntatore alla classe, ottenendo l'accesso ai membri.
*/
BOOL CALLBACK __stdcall CDialog::DialogProcedure(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	static CDialog* This = (CDialog*)NULL;
	static Param p;

	switch(uMsg)
	{
		// in lParam il puntatore alla classe
		case WM_INITDIALOG:
		{
			p.hWnd   = hWnd;
			p.uMsg   = uMsg;
			p.wParam = wParam;
			p.lParam = lParam;
			This = (CDialog*)lParam;
			return(This->DialogProcedure(&p));
			break;
		}

		// messaggi, chiama la callback reale
		default:
		{
			if(This!=(CDialog*)NULL)
			{
				p.hWnd   = hWnd;
				p.uMsg   = uMsg;
				p.wParam = wParam;
				p.lParam = lParam;
				return(This->DialogProcedure(&p));
			}
			else
				return(FALSE);
			break;
		}
	}
}

/*
	DialogProcedure()

	Callback per la gestione dei messaggi (chiamata dal membro di cui sopra).
*/
BOOL CDialog::DialogProcedure(Param* p)
{
	switch(p->uMsg)
	{
		// se restituisce FALSE termina
		case WM_INITDIALOG:
		{
			m_hWnd = p->hWnd;
			if(!this->OnInitDialog((HWND)p->wParam,p->lParam))
				::PostMessage(p->hWnd,WM_QUIT,0,0);
			return(TRUE);
		}

		// comandi
		case WM_COMMAND:
		{
			this->OnCommand((int)LOWORD(p->wParam),(HWND)p->lParam,(UINT)HIWORD(p->wParam));
			return(TRUE);
		}

		default:
			return(FALSE);
	}
}

/*
	OnInitDialog()

	Inizializza il dialogo e chiama il membro virtuale (deve essere definito nella classe derivata).
	Se il membro della classe derivata restituisce FALSE il dialogo viene chiuso.
*/
BOOL CDialog::OnInitDialog(HWND hwndFocus,LPARAM lParam)
{
	if(m_hInstance==NULL || m_hWnd==NULL || m_nTemplateID==(UINT)-1 || m_nIconID==(UINT)-1)
		return(FALSE);

	// icona
	::SetClassLong(m_hWnd,GCL_HICON,(LONG)::LoadIcon((HINSTANCE)::GetWindowLong(m_hWnd,GWL_HINSTANCE),MAKEINTRESOURCE(m_nIconID)));

	// titolo
	if(m_lpcszTitle)
		::SetWindowText(m_hWnd,m_lpcszTitle);

	return(this->OnInitDialog());
}

/*
	CLicenseDialog
	Classe derivata per il dialogo per la licenza.
*/
class CLicenseDialog : public CDialog
{
public:
	// ctor/dtor
	CLicenseDialog(HINSTANCE hInstance = NULL,UINT nTemplateID = (UINT)-1,UINT nIconID = (UINT)-1,LPCSTR lpcszTitle = NULL) : CDialog(hInstance,nTemplateID,nIconID,lpcszTitle)
	{
		memset(m_szLicense,'\0',sizeof(m_szLicense));
	}
	virtual ~CLicenseDialog() {}

	// da definire
	BOOL	OnInitDialog(void);
	void OnCommand(int,HWND,UINT);

	// gestori
	void OnButtonAccept(void);
	void OnButtonDecline(void);
	
private:
	char m_szLicense[8192];
};

/*
	OnInitDialog()
*/
BOOL CLicenseDialog::OnInitDialog(void)
{
	ExtractResourceIntoBuffer(IDR_LICENSE,"TXT",m_szLicense,sizeof(m_szLicense)-1);

	// visualizza l'about
	Edit_SetText(GetDlgItem(IDC_EDIT_LICENSE),m_szLicense);

	return(TRUE);
}

/*
	OnCommand()
*/
void CLicenseDialog::OnCommand(int nID,HWND hWndCtrl,UINT uiNotify)
{
	// processa i comandi ricevuti dai controli del dialogo
	switch(nID)
	{
		case IDOK:
		{
			OnButtonAccept();
			break;
		}

		case IDCANCEL:
		{
			if(uiNotify==BN_CLICKED)
				OnButtonDecline();
			break;
		}
	}
}

/*
	OnButtonAccept()
*/
void CLicenseDialog::OnButtonAccept(void)
{
	EndDialog(IDOK);
}

/*
	OnButtonDecline()
*/
void CLicenseDialog::OnButtonDecline(void)
{
	EndDialog(IDCANCEL);
}


/*
	CSelfExtractDialog
	Classe derivata per il dialogo per l'installatore.
*/
class CSelfExtractDialog : public CDialog
{
public:
	// ctor/dtor
	CSelfExtractDialog(HINSTANCE hInstance = NULL,UINT nTemplateID = (UINT)-1,UINT nIconID = (UINT)-1,LPCSTR lpcszTitle = NULL) : CDialog(hInstance,nTemplateID,nIconID,lpcszTitle)
	{
		memset(m_szInstallFolder,'\0',sizeof(m_szInstallFolder));
		memset(m_szStatus,'\0',sizeof(m_szStatus));
	}
	virtual ~CSelfExtractDialog() {}

	// da definire
	BOOL	OnInitDialog(void);
	void OnCommand(int,HWND,UINT);

	// gestori
	void	OnEditPath(void);
	void OnButtonInstall(void);
	void OnButtonBrowse(void);
	void OnButtonAbout(void);
	void OnButtonExit(void);
	
	// callback per la barra di progresso
	static UINT ExtractCallBack(LPVOID,LPVOID);

private:
	char m_szInstallFolder[_MAX_FILEPATH+1];
	char m_szStatus[_MAX_PATH+1];
	CProgressBar m_wndProgressBar;
};

/*
	OnInitDialog()
*/
BOOL CSelfExtractDialog::OnInitDialog(void)
{
	char szBuffer[1024];

	// barra di progresso
	m_wndProgressBar.Attach(m_hWnd,IDC_PROGRESS);
	m_wndProgressBar.SetStep(1);
	m_wndProgressBar.Hide();

	// visualizza/nasconde i controlli
	::ShowWindow(GetDlgItem(IDC_INSTALL),SW_SHOW);
	::ShowWindow(GetDlgItem(IDC_STATUS),SW_HIDE);
	::ShowWindow(GetDlgItem(IDC_PROGRESS),SW_HIDE);
	::ShowWindow(GetDlgItem(IDC_PATH),SW_SHOW);
	::ShowWindow(GetDlgItem(IDC_BROWSE),SW_SHOW);

	// visualizza l'about
	Edit_SetText(GetDlgItem(IDC_EDIT_ABOUT),g_lpcszCredits);

	// imposta la directory per l'installazione
	_snprintf(szBuffer,
			sizeof(szBuffer)-1,
			"%s will be installed into the following folder.\nClick on the Install button when ready.",
			WALLPAPER_PROGRAM_NAME);
	Edit_SetText(GetDlgItem(IDC_INSTALL),szBuffer);

	// ricava la Program Files sucaminchia
	ITEMIDLIST *id = NULL;
	BOOL bSuccess = FALSE;
	if(::SHGetSpecialFolderLocation(NULL,CSIDL_PROGRAM_FILES,&id)==NOERROR)
	{
		char szProgramFiles[_MAX_FILEPATH+1] = {0};
		if(::SHGetPathFromIDList(id,&szProgramFiles[0]))
		{
			bSuccess = TRUE;
			_snprintf(m_szInstallFolder,sizeof(m_szInstallFolder)-1,"%s\\%s",szProgramFiles,WALLPAPERSFX_DEFAULT_INSTALL_FOLDER);
		}
	}
	if(!bSuccess)
		_snprintf(m_szInstallFolder,sizeof(m_szInstallFolder)-1,"C:\\%s",WALLPAPERSFX_DEFAULT_INSTALL_FOLDER);
	Edit_SetText(GetDlgItem(IDC_PATH),m_szInstallFolder);

	return(TRUE);
}

/*
	OnCommand()
*/
void CSelfExtractDialog::OnCommand(int nID,HWND hWndCtrl,UINT uiNotify)
{
	// processa i comandi ricevuti dai controli del dialogo
	switch(nID)
	{
		case IDC_PATH:
		{
			if(uiNotify==EN_CHANGE)
				OnEditPath();
			break;
		}

		case IDC_EXTRACT:
		{
			if(uiNotify==BN_CLICKED)
				OnButtonInstall();
			break;
		}

		case IDC_BROWSE:
		{
			if(uiNotify==BN_CLICKED)
				OnButtonBrowse();
			break;
		}

		case IDC_ABOUT:
		{
			if(uiNotify==BN_CLICKED)
				OnButtonAbout();
			break;
		}

		case IDCANCEL:
		{
			if(uiNotify==BN_CLICKED)
				OnButtonExit();
			break;
		}
	}
}

/*
	OnEditPath()
*/
void CSelfExtractDialog::OnEditPath(void)
{
	// dal controllo al buffer interno
	Edit_GetText(GetDlgItem(IDC_PATH),m_szInstallFolder,sizeof(m_szInstallFolder)-1);
	::RemoveBackslash(m_szInstallFolder);
}

/*
	OnButtonInstall()
*/
void CSelfExtractDialog::OnButtonInstall(void)
{
	int nRet = 0;
	char szBuffer[2048];

	// directory di output
	if(strempty(m_szInstallFolder))
	{
		::MessageBox(m_hWnd,"No kidding please, select a valid folder as destination.",WALLPAPERSFX_PROGRAM_TITLE,MB_APPLMODAL|MB_SETFOREGROUND|MB_TOPMOST|MB_OK|MB_ICONWARNING);
		::MessageBeep(MB_ICONEXCLAMATION);
		return;
	}

	CFindFile findfile;
	if(!findfile.CreatePathName(m_szInstallFolder))
	{
		_snprintf(szBuffer,sizeof(szBuffer)-1,"Unable to create %s.",m_szInstallFolder);
		::MessageBox(m_hWnd,szBuffer,WALLPAPERSFX_PROGRAM_TITLE,MB_APPLMODAL|MB_SETFOREGROUND|MB_TOPMOST|MB_OK|MB_ICONERROR);
		::MessageBeep(MB_ICONEXCLAMATION);
		return;
	}

	// estrae i files
	CSelfExtractor	m_Extractor;
	char szThisModule[_MAX_FILEPATH+1];
	::GetThisModuleFileName(szThisModule,sizeof(szThisModule));

	// visualizza/nasconde i controlli
	::ShowWindow(GetDlgItem(IDC_INSTALL),SW_HIDE);
	::ShowWindow(GetDlgItem(IDC_STATUS),SW_SHOW);
	::ShowWindow(GetDlgItem(IDC_PROGRESS),SW_SHOW);
	::ShowWindow(GetDlgItem(IDC_PATH),SW_HIDE);
	::ShowWindow(GetDlgItem(IDC_BROWSE),SW_HIDE);
	::EnableWindow(GetDlgItem(IDC_EXTRACT),FALSE);
	::EnableWindow(GetDlgItem(IDC_ABOUT),FALSE);
	::EnableWindow(GetDlgItem(IDCANCEL),FALSE);

#ifdef _DEBUG
	nRet = SFX_SUCCESS;
#else
	if((nRet = m_Extractor.ReadTOC(szThisModule))!=SFX_SUCCESS)
		goto return_code;
#endif

	// status bar
#ifdef _DEBUG
	m_wndProgressBar.SetRange(0,100);
#else
	m_wndProgressBar.SetRange(0,m_Extractor.GetFileCount());
#endif
	m_wndProgressBar.Show();
	Edit_SetText(GetDlgItem(IDC_STATUS),"");

#ifdef _DEBUG
	strcpy(m_szStatus,"Extracting ...");
	Edit_SetText(GetDlgItem(IDC_STATUS),m_szStatus);
	for(int i=0; i < 100; i++)
	{
		m_wndProgressBar.StepIt();
		::Sleep(50L);
	}
	nRet = SFX_SUCCESS;
#else
	nRet = m_Extractor.ExtractAll(m_szInstallFolder,sizeof(m_szInstallFolder),CSelfExtractDialog::ExtractCallBack,this);
#endif

	// status bar
	m_wndProgressBar.Hide();
	Edit_SetText(GetDlgItem(IDC_STATUS),"");

return_code:

	::ShowWindow(GetDlgItem(IDC_INSTALL),SW_SHOW);
	::ShowWindow(GetDlgItem(IDC_STATUS),SW_HIDE);
	::ShowWindow(GetDlgItem(IDC_PROGRESS),SW_HIDE);
	::ShowWindow(GetDlgItem(IDC_PATH),SW_SHOW);
	::ShowWindow(GetDlgItem(IDC_BROWSE),SW_SHOW);
	::EnableWindow(GetDlgItem(IDC_EXTRACT),TRUE);
	::EnableWindow(GetDlgItem(IDC_ABOUT),TRUE);
	::EnableWindow(GetDlgItem(IDCANCEL),TRUE);
	::SetFocus(GetDlgItem(IDCANCEL));
	
	switch(nRet)
	{
		case SFX_SUCCESS:
		{
			_snprintf(szBuffer,
					sizeof(szBuffer)-1,
					"%s has been installed successful.\n\n"
					"For general notes about the program, see the readme.txt file.\n"
					"For a list of changes of the current version (%s%s), see the\nchangelog.txt file.\n"
					"License terms into the license.txt file, credits available into the\ncredits.txt file.\n"
					"\nDo not miss to visit the web site (%s)\t\n"
					"\nThat's all, crawl everything and have fun...\n",
					WALLPAPER_PROGRAM_NAME,
					WALLPAPER_VERSION_NUMBER,
					WALLPAPER_VERSION_TYPE,
					WALLPAPER_WEB_SITE
					);
			::MessageBeep(MB_OK);
			::MessageBox(m_hWnd,szBuffer,WALLPAPERSFX_PROGRAM_TITLE,MB_APPLMODAL|MB_SETFOREGROUND|MB_TOPMOST|MB_OK|MB_ICONINFORMATION);

			// esegue quanto installato
			STARTUPINFO si = {0};
			PROCESS_INFORMATION pi = {0};
			::RemoveBackslash(m_szInstallFolder);
			_snprintf(szBuffer,sizeof(szBuffer)-1,"%s\\%s.exe /i%s\\%s.exe",m_szInstallFolder,WALLPAPER_PROGRAM_NAME,m_szInstallFolder,WALLPAPER_PROGRAM_NAME);
			memset(&si,'\0',sizeof(STARTUPINFO));
			si.cb = sizeof(STARTUPINFO);
			memset(&pi,'\0',sizeof(PROCESS_INFORMATION));
#ifdef _DEBUG
			::MessageBox(m_hWnd,szBuffer,"Now executing...",MB_APPLMODAL|MB_SETFOREGROUND|MB_TOPMOST|MB_OK|MB_ICONINFORMATION);
#else
			if(::CreateProcess(NULL,szBuffer,NULL,NULL,FALSE,0L,NULL,NULL,&si,&pi))
				::CloseHandle(pi.hProcess);
			::Sleep(500L);
#endif
			EndDialog(IDOK);

			break;
		}

		case SFX_NO_SOURCE:
		case SFX_INVALID_SIG:
		case SFX_COPY_FAILED:
		case SFX_NOTHING_TO_DO:
		case SFX_OUTPUT_FILE_ERROR:
		case SFX_INPUT_FILE_ERROR:
		case SFX_RESOURCE_ERROR:
		case SFX_COMPRESS_ERROR:
		case SFX_UNCOMPRESS_ERROR:
		case SFX_UNKNOWN_ERROR:
		case SFX_SAME_FILES:
		default:
		{
			_snprintf(szBuffer,sizeof(szBuffer)-1,"Extraction error: %s.",m_Extractor.GetLastErrorString());
			::MessageBox(m_hWnd,szBuffer,WALLPAPERSFX_PROGRAM_TITLE,MB_APPLMODAL|MB_SETFOREGROUND|MB_TOPMOST|MB_OK|MB_ICONWARNING);
			::MessageBeep(MB_ICONEXCLAMATION);
			break;
		}
	}
}

/*
	OnButtonBrowse()
*/
void CSelfExtractDialog::OnButtonBrowse(void)
{
	CDirDialog dlg(m_szInstallFolder,"Select the output directory");
	
	if(dlg.DoModal(m_hWnd)==IDOK)
	{
		strcpyn(m_szInstallFolder,dlg.GetPathName(),sizeof(m_szInstallFolder));
		Edit_SetText(GetDlgItem(IDC_PATH),m_szInstallFolder);
	}
}

/*
	OnButtonAbout()
*/
void CSelfExtractDialog::OnButtonAbout(void)
{
	char szAuthorCopyright[256];
	_snprintf(szAuthorCopyright,
			sizeof(szAuthorCopyright)-1,
			WALLPAPER_AUTHOR_COPYRIGHT,
			WALLPAPER_AUTHOR_EMAIL
			);

	char szAbout[512];
	_snprintf(szAbout,
			sizeof(szAbout)-1,
			"GZW Self Extractor Installer\n\n"
			"%s.\n\n"
			"The GZW API uses a modified version of the zLib library.\n"
			"The self-extract portion of this code uses a modified version of the work of James Spibey.\n\n",
			szAuthorCopyright
			);
	
	::MessageBeep(MB_OK);
	::MessageBox(m_hWnd,szAbout," About ",MB_APPLMODAL|MB_SETFOREGROUND|MB_TOPMOST|MB_OK|MB_ICONINFORMATION);
}

/*
	OnButtonExit()
*/
void CSelfExtractDialog::OnButtonExit(void)
{
	EndDialog(IDOK);
}

/*
	ExtractCallBack()
*/
UINT CSelfExtractDialog::ExtractCallBack(void *ExtractData,void* userData)
{
	CSEFileInfo* pData = (CSEFileInfo*)ExtractData;
	CSelfExtractDialog* pDlg = (CSelfExtractDialog*)userData;

	_snprintf(pDlg->m_szStatus,_MAX_PATH-4,"Extracting %s...",pData->GetFileName());
	char* p = strstr(pDlg->m_szStatus,GZW_EXTENSION);
	if(p)
		*p = '\0';
	strcat(pDlg->m_szStatus,"...");
	Edit_SetText(pDlg->GetDlgItem(IDC_STATUS),pDlg->m_szStatus);

	pDlg->m_wndProgressBar.StepIt();

	return(0L);
}

/*
	WinMain()
*/
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpszCmdLine,int nCmdShow)
{
	if(::FindWindow((LPCSTR)NULL,WALLPAPERSFX_PROGRAM_TITLE)==(HWND)NULL)
	{
		CLicenseDialog dlgLicense(hInstance,IDD_LICENSE_DIALOG,IDI_GZW_EXTRACTOR,WALLPAPERSFX_PROGRAM_TITLE);
		if(dlgLicense.DoModal()==IDOK)
		{
			CSelfExtractDialog dlgInstall(hInstance,IDD_EXTRACTOR_DIALOG,IDI_GZW_EXTRACTOR,WALLPAPERSFX_PROGRAM_TITLE);
			dlgInstall.DoModal();
		}
	}
	else
		::MessageBox((HWND)NULL,"No kidding please, only one instance at time.",WALLPAPERSFX_PROGRAM_TITLE,MB_APPLMODAL|MB_SETFOREGROUND|MB_TOPMOST|MB_OK|MB_ICONERROR);

	return(0);
}
