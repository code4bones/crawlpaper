/*
	WallPaperDownloadDlg.cpp
	Dialogo per il download del file dal sito web.
	Luca Piergentili, 20/08/01
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
#include "window.h"
#include "win32api.h"
#include "CFindFile.h"
#include "CBase64.h"
#include "CHttp.h"
#include "CUrl.h"
#include "CFilenameFactory.h"
#include "CProgressBar.h"
#include "CWndLayered.h"
#include "CWindowsVersion.h"
#include "WallPaperConfig.h"
#include "WallPaperDownloadDlg.h"
#include "WallPaperMessages.h"
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

BEGIN_MESSAGE_MAP(CWallPaperDownloadDlg,CDialog)
	ON_BN_CLICKED(IDOK,OnOK)
	ON_BN_CLICKED(IDCANCEL,OnCancel)
	ON_MESSAGE(WM_UPDATE_GUI,OnUpdateGui)
	ON_WM_WINDOWPOSCHANGING()
END_MESSAGE_MAP()

/*
	DoDataExchange()
*/
void CWallPaperDownloadDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

/*
	CWallPaperDownloadDlg()
*/
CWallPaperDownloadDlg::CWallPaperDownloadDlg(CWnd* pWndParent,
									CWallPaperConfig* pConfig/* = NULL*/,
									BOOL bForceDownload/* = TRUE*/,
									LPCSTR lpcszFileName/* = NULL*/,
									LPCSTR lpcszWebSite/* = NULL*/,
									LPCSTR lpcszDownloadDir/* = NULL*/) 
									: CDialog(IDD_DIALOG_DOWNLOAD,pWndParent)
{
	m_pWndParent = pWndParent;
	m_pConfig = pConfig;
	m_bVisible = TRUE;
	memset(m_szWindowTitle,'\0',sizeof(m_szWindowTitle));
	m_nCancel = 0;
	m_bShowErrors = FALSE;
	memset(m_szStatus,'\0',sizeof(m_szStatus));
	m_nTimerId = 0L;
	m_bConnected = FALSE;
	m_bInDownload = FALSE;
	m_bDownloaded = FALSE;
	m_HttpConnection.Reset();
	m_HttpConnection.SetDelay(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_WINSOCKDELAY_KEY));
	m_bForceDownload = bForceDownload;
	strcpyn(m_szFileName,lpcszFileName,sizeof(m_szFileName));
	// se non viene specificato un sito web, assume il proprio
	strcpyn(m_szWebSite,lpcszWebSite ? lpcszWebSite : m_pConfig->GetString(WALLPAPER_INSTALL_KEY,WALLPAPER_WEBSITE_KEY),sizeof(m_szWebSite));
	// se non viene specificata una directory per il download, assume quella di default
	strcpyn(m_szDownloadDir,lpcszDownloadDir ? lpcszDownloadDir : m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_DOWNLOADDIR_KEY),sizeof(m_szDownloadDir));
	memset(m_szDownloadedFile,'\0',sizeof(m_szDownloadedFile));
	m_pThread = (CThread*)NULL;
}

/*
	~CWallPaperDownloadDlg()
*/
CWallPaperDownloadDlg::~CWallPaperDownloadDlg()
{
}

/*
	Reset()
*/
void CWallPaperDownloadDlg::Reset(void)
{
	m_bVisible = TRUE;
	memset(m_szWindowTitle,'\0',sizeof(m_szWindowTitle));
	m_nCancel = 0;
	m_bShowErrors = FALSE;
	memset(m_szStatus,'\0',sizeof(m_szStatus));
	m_nTimerId = 0L;
	m_bConnected = FALSE;
	m_bInDownload = FALSE;
	m_bDownloaded = FALSE;
	m_HttpConnection.Reset();
	m_HttpConnection.SetDelay(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_WINSOCKDELAY_KEY));
	m_bForceDownload = TRUE;
	memset(m_szFileName,'\0',sizeof(m_szFileName));
	memset(m_szWebSite,'\0',sizeof(m_szWebSite));
	memset(m_szDownloadDir,'\0',sizeof(m_szDownloadDir));
	memset(m_szDownloadedFile,'\0',sizeof(m_szDownloadedFile));
	m_pThread = (CThread*)NULL;
}

/*
	OnInitDialog()
*/
BOOL CWallPaperDownloadDlg::OnInitDialog(void)
{
	// classe base
	CDialog::OnInitDialog();

	// barra di progresso per il download
	m_wndProgressBar.Attach(this->m_hWnd,IDC_PROGRESS);
	m_wndProgressBar.SetRange(0,100);
	m_wndProgressBar.SetStep(1);
	m_wndProgressBar.SetPos(0);
	m_wndProgressBar.Hide();

	// trasparenza
	if(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAYERED_KEY))
		m_wndLayered.SetLayer(this->m_hWnd,(BYTE)m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAYERED_VALUE_KEY));

	// titolo
	if(m_szWindowTitle[0]!='\0')
		SetWindowText(m_szWindowTitle);

	// rispetto alla principale
	CenterWindow();
	
	// lancia il thread per scaricare il file in locale
	m_pThread = BeginThread(Download,this,m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PRIORITY_THREAD_KEY),0,CREATE_SUSPENDED);
	if(m_pThread)
	{
		m_pThread->SetAutoDelete(FALSE);
		m_pThread->Resume();
	}

	return(TRUE);
}

/*
	OnWindowPosChanging()
*/
void CWallPaperDownloadDlg::OnWindowPosChanging(WINDOWPOS FAR* lpWndPos) 
{
	// per non visualizzare il dialogo
	if(!m_bVisible)
		lpWndPos->flags &= ~SWP_SHOWWINDOW;
	
	CDialog::OnWindowPosChanging(lpWndPos);
}

/*
	OnCancel()
*/
void CWallPaperDownloadDlg::OnCancel(void)
{
	// click sul bottone o sulla x
	m_nCancel++;

	if(m_bInDownload)
	{
		// chiude la connessione per forzare (anticipare) la chiusura del thread
		m_HttpConnection.Close();
	}
		
	// attende che il thread per download termini
	if(m_pThread)
	{
		HANDLE hThread = m_pThread->GetHandle();
		if(hThread)
		{
			::WaitForSingleObject(hThread,INFINITE);
			delete m_pThread,m_pThread = NULL;
		}
	}

	// chiude il dialogo
	EndDialog(m_bDownloaded ? IDOK : IDCANCEL);
}

/*
	OnUpdateGui()
*/
LRESULT CWallPaperDownloadDlg::OnUpdateGui(WPARAM wParam,LPARAM lParam)
{
	// sembra che l'aggiornamento dal thread incriccha il tutto...
	if((UINT)wParam==IDC_PROGRESS)
	{
		if((UINT)lParam==(UINT)-1L)
		{
			m_wndProgressBar.SetPos(0);
			m_wndProgressBar.Hide();
		}
		else
		{
			m_wndProgressBar.Show();
			m_wndProgressBar.SetPos((UINT)lParam);
		}
	}
	else
		SetDlgItemText((UINT)wParam,(LPCSTR)lParam);
	
	return(0L);
}

/*
	DownloadCallback()
*/
UINT CWallPaperDownloadDlg::DownloadCallback(LPVOID pHttpConnection,LPVOID lpVoid)
{
	CWallPaperDownloadDlg* This = (CWallPaperDownloadDlg*)lpVoid;
	return(This ? This->DownloadCallback(pHttpConnection) : (UINT)-1L);
}

/*
	DownloadCallback()
*/
UINT CWallPaperDownloadDlg::DownloadCallback(LPVOID lpVoid)
{
	CUrl url;
	CFilenameFactory fn;
	CHttpConnection* pHttpConnection = (CHttpConnection*)lpVoid;
	int nRet = (m_nCancel==0 ? IDOK : IDCANCEL);

	// per il timeout sulla connessione
	m_bConnected = TRUE;
	
	if(nRet==IDOK)
	{
		// inizio download
		if(pHttpConnection->GetObject()->stat==HTTPOBJECTSTAT_BEGIN_RECEIVING)
		{
			// interfaccia utente
			_snprintf(m_szStatus,
					sizeof(m_szStatus)-1,
					"Retrieving: %s",
					fn.Abbreviate(url.StripPathFromUrl(pHttpConnection->GetObject()->url),40)
					);
			::PostMessage(this->m_hWnd,WM_UPDATE_GUI,IDC_STATIC_STATUS,(LPARAM)m_szStatus);
			
			if(pHttpConnection->GetObject()->size <= (DWORD)url.GetDownloadBufferSize())
				::PostMessage(this->m_hWnd,WM_UPDATE_GUI,IDC_PROGRESS,(LPARAM)-1L);

			// imposta il codice di ritorno	
			nRet = (m_nCancel==0 ? IDOK : IDCANCEL);
		}
		// download
		else if(pHttpConnection->GetObject()->stat==HTTPOBJECTSTAT_RECEIVING)
		{
			// interfaccia utente
			if(pHttpConnection->GetObject()->size > (DWORD)url.GetDownloadBufferSize())
			{
				double nProgress = FDIV((100.0 * pHttpConnection->GetObject()->amount),pHttpConnection->GetObject()->size);
				_snprintf(m_szStatus,
						sizeof(m_szStatus)-1,
						"%s (%d%%)",
						fn.Abbreviate(::StripPathFromFile(pHttpConnection->GetObject()->filename),40),
						(int)nProgress);
				
				::PostMessage(this->m_hWnd,WM_UPDATE_GUI,IDC_STATIC_STATUS,(LPARAM)m_szStatus);
				::PostMessage(this->m_hWnd,WM_UPDATE_GUI,IDC_PROGRESS,(LPARAM)nProgress);

				::PostMessage(m_pWndParent->m_hWnd,WM_DOWNLOAD_PROGRESS,0,(LPARAM)nProgress);
			}

			// imposta il codice di ritorno	
			nRet = (m_nCancel==0 ? IDOK : IDCANCEL);
		}
		// fine download
		else if(pHttpConnection->GetObject()->stat==HTTPOBJECTSTAT_END_RECEIVING)
		{
			// interfaccia utente
			strcpy(m_szStatus,"Done");
			
			::PostMessage(this->m_hWnd,WM_UPDATE_GUI,IDC_STATIC_STATUS,(LPARAM)m_szStatus);
			::PostMessage(this->m_hWnd,WM_UPDATE_GUI,IDC_PROGRESS,(LPARAM)-1L);

			// imposta il codice di ritorno	
			nRet = (m_nCancel==0 ? IDOK : IDCANCEL);
		}
	}

	return(nRet);
}

/*
	Download()
*/
UINT CWallPaperDownloadDlg::Download(LPVOID lpVoid)
{
	CWallPaperDownloadDlg* pThis = (CWallPaperDownloadDlg*)lpVoid;
	return(pThis ? pThis->Download() : (UINT)-1L);
}

/*
	Download()
*/
UINT CWallPaperDownloadDlg::Download(void)
{
	CUrl url;
	CONTENTTYPE ctype;
	char szUrl[_MAX_PATH+1];
	char szTranslatedUrl[MAX_URL+1];
	char szWebSite[MAX_URL+1];
	char szFileName[_MAX_PATH+1];

	m_nCancel = 0;
	m_bInDownload = TRUE;

	// interfaccia utente
	strcpy(m_szStatus,"Connecting...");
	//_snprintf(m_szStatus,sizeof(m_szStatus)-1,"Connecting to: %s",m_szWebSite);
	::PostMessage(this->m_hWnd,WM_UPDATE_GUI,IDC_STATIC_STATUS,(LPARAM)m_szStatus);

	// costruisce l'url completa per il download del file
	strcpyn(szWebSite,m_szWebSite,sizeof(szWebSite));
	int i = strlen(szWebSite);
	if(szWebSite[i-1]!='/')
		strcat(szWebSite,"/");
	_snprintf(szUrl,sizeof(szUrl)-1,"%s%s",szWebSite,m_szFileName);

	// imposta i default per la connessione
	m_HttpConnection.Reset();
	m_HttpConnection.SetDelay(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_WINSOCKDELAY_KEY));
	m_HttpConnection.SetSkipExisting(m_bForceDownload ? FALSE : m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_SKIPEXISTING_KEY));
	m_HttpConnection.SetForceSkipExisting(m_bForceDownload ? FALSE : m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_SKIPEXISTING_FORCE_KEY));
	m_HttpConnection.SetDoNotSkipExisting(m_bForceDownload ? TRUE : m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_SKIPEXISTING_DONT_KEY));

	m_HttpConnection.SetConnectionRetry(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_CONNECTIONRETRY_KEY));
	if(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_CONNECTIONTIMEOUT_KEY) > 0 && m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_USEWINSOCKTIMEOUT_KEY))
		m_HttpConnection.SetConnectionTimeout(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_CONNECTIONTIMEOUT_KEY));

	m_HttpConnection.AllowCookies(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_COOKIES_ALLOWED_KEY));
	m_HttpConnection.SetCookieFileName(m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_COOKIES_FILE_KEY));

	if(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_USEPROXY_KEY))
	{
		m_HttpConnection.SetProxyAddress(m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_PROXY_ADDRESS_KEY));
		m_HttpConnection.SetProxyPortNumber(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PROXY_PORTNUMBER_KEY));
		if(strcmp(m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_PROXY_USER_KEY),"")!=0 && strcmp(m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_PROXY_PASSWORD_KEY),"")!=0)
		{
			m_HttpConnection.SetProxyUser(m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_PROXY_USER_KEY));
			m_HttpConnection.SetProxyPassword(m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_PROXY_PASSWORD_KEY));
			CBase64 base64;
			char szEncoded[MAX_USER_LEN+1+MAX_PASS_LEN+1];
			_snprintf(szEncoded,sizeof(szEncoded)-1,"%s:%s",m_HttpConnection.GetProxyUser(),m_HttpConnection.GetProxyPassword());
			char* pAuth = base64.Encode(szEncoded);
			m_HttpConnection.SetProxyAuth(pAuth);
			delete [] pAuth;
		}
		if(strcmp(m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_PROXY_EXCLUDEDOMAINS_KEY),"")!=0)
		{
			CHostNameList* pProxyExcludeList = m_HttpConnection.GetProxyExcludeList();
			char szExclude[2048];
			strcpyn(szExclude,m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_PROXY_EXCLUDEDOMAINS_KEY),sizeof(szExclude));
			char* token = strtok(szExclude,",");
			HOSTNAME* n;
			for(int i = 0; token; i++)
			{
				if((n = new HOSTNAME)!=(HOSTNAME*)NULL)
				{
					strcpyn(n->name,token,HOSTNAME_SIZE+1);
					pProxyExcludeList->Add(n);
					token = strtok((char*)NULL,",");
				}
			}
		}
	}

	CWindowsVersion winver;
	char szUserAgent[HTTP_MAX_USERAGENT+1];
	_snprintf(szUserAgent,sizeof(szUserAgent)-1,WALLPAPER_USER_AGENT,winver.GetVersionString());
	m_HttpConnection.AddHeader("User-Agent",szUserAgent);
	m_HttpConnection.AddHeader("Pragma","no-cache");

	// per il timeout sulla connessione
	m_bConnected = FALSE;
	if(m_nTimerId!=0L)
	{
		::KillTimer(AfxGetMainWnd()->m_hWnd,m_nTimerId);
		m_nTimerId = 0L;
	}
	m_nTimerId = ::SetTimer(this->GetSafeHwnd(),(UINT)this,DOWNLOAD_TIMEOUT_VALUE,(TIMERPROC)TimeoutTimerProc);

	// scarica il file in locale
	url.GetHttpUrl(&m_HttpConnection,
				szUrl,
				NULL,
				szTranslatedUrl,
				sizeof(szTranslatedUrl),
				m_szDownloadDir,
				szFileName,
				sizeof(szFileName),
				&ctype,
				FALSE,
				DownloadCallback,
				this);

	// per il timeout sulla connessione
	m_bConnected = FALSE;
	if(m_nTimerId!=0L)
	{
		::KillTimer(this->GetSafeHwnd(),m_nTimerId);
		m_nTimerId = 0L;
	}
	
	// controlla l'esito del download
	if((m_HttpConnection.GetHttpError() <= (int)HTTP_STATUS_SUCCESSFUL || m_HttpConnection.GetHttpError()==HTTP_STATUS_NOT_MODIFIED) && m_HttpConnection.GetSocketError()==0)
	{
		if(::FileExist(szFileName))
		{
			m_bDownloaded = TRUE;
			strcpyn(m_szDownloadedFile,szFileName,sizeof(m_szDownloadedFile));
		}
	}
	else
		::DeleteFile(szFileName);

	// visualizza l'errore
	if(m_bShowErrors && !m_bDownloaded && m_nCancel <= 0)
	{
		char szAddress[256];
		if(m_HttpConnection.GetProxyPortNumber() > 0)
			_snprintf(szAddress,sizeof(szAddress)-1,"%s (through the %s:%ld proxy)",szUrl,m_HttpConnection.GetProxyAddress(),m_HttpConnection.GetProxyPortNumber());
		else
			strcpyn(szAddress,szUrl,sizeof(szAddress));
		::MessageBoxResourceEx(this->m_hWnd,MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_DOWNLOAD,szAddress,m_HttpConnection.GetErrorString());
	}

	// interfaccia utente
	strcpy(m_szStatus,m_nCancel!=0 ? "Cancelled" : "Done");
	::PostMessage(this->m_hWnd,WM_UPDATE_GUI,IDC_STATIC_STATUS,(LPARAM)m_szStatus);

	// riabilita i flags per l'uscita (trasforma cancel in close)
	m_nCancel = 0;
	m_bInDownload = FALSE;

	::PostMessage(this->m_hWnd,WM_CLOSE,0L,0L);

	return(0L);
}

/*
	TimeoutTimerProc()
*/
VOID CALLBACK CWallPaperDownloadDlg::TimeoutTimerProc(HWND /*hWnd*/,UINT /*uMsg*/,UINT_PTR idEvent,DWORD /*dwTime*/)
{
	CWallPaperDownloadDlg* pDownloadDlg = (CWallPaperDownloadDlg*)idEvent;	
	if(pDownloadDlg)
	{
		// se non e' riuscito a connettersi e non e' prevista la visualizzazione dell'interfaccia
		if(!pDownloadDlg->m_bConnected && !pDownloadDlg->m_bVisible)
		{
			// chiude la connessione per forzare la chiusura del thread e del dialogo
			pDownloadDlg->m_HttpConnection.Close();
			::Sleep(0L);
		}

		if(pDownloadDlg->m_nTimerId!=0L)
		{
			::KillTimer(pDownloadDlg->GetSafeHwnd(),pDownloadDlg->m_nTimerId);
			pDownloadDlg->m_nTimerId = 0L;
		}
	}
}
