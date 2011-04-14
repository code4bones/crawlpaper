/*
	WallBrowser.cpp
	Classe per l'applicazione.
	Luca Piergentili, 30/07/01
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
#include "MainFrm.h"
#include "WallBrowserDriveView.h"
#include "WallBrowserFileView.h"
#include "WallBrowserStretchView.h"
#include "WallBrowser.h"
#include "WallBrowserDoc.h"

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

CWallBrowserApp theApp;

BEGIN_MESSAGE_MAP(CWallBrowserApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/*
	InitInstance()
*/
BOOL CWallBrowserApp::InitInstance(void)
{
	AfxEnableControlContainer();

#ifdef _AFXDLL
	Enable3dControls();
#else
	Enable3dControlsStatic();
#endif

	// Register document templates
	// la classe specificata per la vista (CWallBrowserDriveView) e' quella che viene
	// passata al frame durante la creazione della finestra (usata per la vista principale)
	CSingleDocTemplate* pDocTemplate = new CSingleDocTemplate(	IDR_MAINFRAME,
													RUNTIME_CLASS(CWallBrowserDoc),
													RUNTIME_CLASS(CMainFrame),
													RUNTIME_CLASS(CWallBrowserDriveView));
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if(!ProcessShellCommand(cmdInfo))
		return(FALSE);

	// ricava la modalita' di visualizzazione per la finestra
	int nShow = m_Config.GetNumber(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_WINDOWPLACEMENT_SHOW_KEY);

	switch(nShow)
	{
		case SW_MAXIMIZE:
		case SW_MINIMIZE:
		case SW_RESTORE:
		case SW_SHOW:
		case SW_SHOWDEFAULT:
//		case SW_SHOWMAXIMIZED:
		case SW_SHOWMINIMIZED:
		case SW_SHOWMINNOACTIVE:
		case SW_SHOWNA:
		case SW_SHOWNOACTIVATE: 
		case SW_SHOWNORMAL:
			break; 
		
		case SW_HIDE:
		default:
			nShow = SW_SHOW;
			break;
	} 

	m_pMainWnd->ShowWindow(nShow);
	m_pMainWnd->UpdateWindow();

	return(TRUE);
}

/*
	OnAppAbout()
*/
void CWallBrowserApp::OnAppAbout(void)
{
	CString strAbout;
	strAbout.Format(	"%s\n%s\n\n"WALLPAPER_AUTHOR_COPYRIGHT".\n\nFor license and credits on third party, see the About Box of WallPaper.",
					WALLBROWSER_PROGRAM_TITLE,
					m_Config.GetString(WALLBROWSER_INSTALL_KEY,WALLBROWSER_WEBSITE_KEY),
					WALLPAPER_AUTHOR_EMAIL
					);

	::MessageBox(AfxGetMainWnd()->m_hWnd,LPCSTR(strAbout),WALLBROWSER_PROGRAM_NAME,MB_OK|MB_ICONINFORMATION|MB_TASKMODAL|MB_SETFOREGROUND|MB_TOPMOST);
}
