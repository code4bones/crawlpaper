/*
	WallPaperDownloadDlg.h
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
#ifndef _CWALLPAPERDOWNLOADDLG_H
#define _CWALLPAPERDOWNLOADDLG_H 1

#include "window.h"
#include "CHttp.h"
#include "CUrl.h"
#include "CThread.h"
#include "CProgressBar.h"
#include "CWndLayered.h"
#include "WallPaperConfig.h"

// 10 sec. d'attesa sulla connessione
#define DOWNLOAD_TIMEOUT_VALUE 10000L

/*
	CWallPaperDownloadDlg
*/
class CWallPaperDownloadDlg : public CDialog
{
public:
	CWallPaperDownloadDlg(	CWnd* pWndParent,											// finestra chiamante
						CWallPaperConfig* pConfig = NULL,								// configurazione corrente
						BOOL bForceDownload = TRUE,									// ignora la configurazione e forza il download
						LPCSTR lpcszFileName = NULL,									// nome file da scaricare (senza percorso)
						LPCSTR lpcszWebSite = NULL,									// se non viene specificato un sito web, assume il proprio
						LPCSTR lpcszDownloadDir = NULL								// se non viene specificata una directory per il download, assume quella di default
					);
	~CWallPaperDownloadDlg();
	
	void				Reset			(void);
	
	// per impostare i valori previsti dal ctor
	inline void		SetForceDownload	(BOOL bForceDownload) {m_bForceDownload = bForceDownload;}
	inline void		SetFilename		(LPCSTR lpcszFileName) {strcpyn(m_szFileName,lpcszFileName,sizeof(m_szFileName));}
	inline void		SetWebSite		(LPCSTR lpcszWebSite) {strcpyn(m_szWebSite,lpcszWebSite ? lpcszWebSite : m_pConfig->GetString(WALLPAPER_INSTALL_KEY,WALLPAPER_WEBSITE_KEY),sizeof(m_szWebSite));}
	inline void		SetDownloadDir		(LPCSTR lpcszDownloadDir) {strcpyn(m_szDownloadDir,lpcszDownloadDir ? lpcszDownloadDir : m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_DOWNLOADDIR_KEY),sizeof(m_szDownloadDir));}
	
	inline void		SetWindowTitle		(LPCSTR lpcszTitle) {strcpyn(m_szWindowTitle,lpcszTitle,sizeof(m_szWindowTitle));}
	inline void		SetVisible		(BOOL bVisible) {m_bVisible = bVisible;}			// impostare a FALSE per scaricare senza interfaccia
	inline void		ShowErrors		(BOOL bShowErrors) {m_bShowErrors = bShowErrors;}		// impostare a FALSE per non visualizzare gli errori
	inline LPCSTR		GetDownloadedFilename(void) const {return(m_szDownloadedFile);}			// nome file completo del file scaricato

	void				DoDataExchange		(CDataExchange* pDX);
	BOOL				OnInitDialog		(void);
	virtual void		OnWindowPosChanging	(WINDOWPOS FAR* lpWndPos);
	void				OnCancel			(void);
	LRESULT			OnUpdateGui		(WPARAM wParam,LPARAM lParam);

	static UINT		DownloadCallback	(LPVOID pHttpConnection,LPVOID lpVoid);
	UINT				DownloadCallback	(LPVOID lpVoid);
	static UINT		Download			(LPVOID lpVoid);
	UINT				Download			(void);

	static VOID CALLBACK TimeoutTimerProc	(HWND hWnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTime);

private:
	CWnd*			m_pWndParent;
	CWallPaperConfig*	m_pConfig;
	BOOL				m_bVisible;
	char				m_szWindowTitle[256];
	int				m_nCancel;
	BOOL				m_bShowErrors;
	char				m_szStatus[1024];
	UINT				m_nTimerId;
	BOOL				m_bConnected;
	BOOL				m_bInDownload;
	BOOL				m_bDownloaded;
	CProgressBar		m_wndProgressBar;
	CHttpConnection	m_HttpConnection;
	BOOL				m_bForceDownload;
	char				m_szFileName[_MAX_PATH+1];
	char				m_szWebSite[MAX_URL+1];
	char				m_szDownloadDir[_MAX_PATH+1];
	char				m_szDownloadedFile[_MAX_PATH+1];
	CThread*			m_pThread;
	CWndLayered		m_wndLayered;

	DECLARE_MESSAGE_MAP()
};

#endif // _CWALLPAPERDOWNLOADDLG_H
