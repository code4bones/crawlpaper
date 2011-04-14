/*
	WallPaperCrawlerDlg.cpp
	Dialogo per il crawler.
	Luca Piergentili, 05/10/00
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
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "strings.h"
#include "window.h"
#include "win32api.h"
#include "CSync.h"
#include "CBrowser.h"
#include "CComboBoxExt.h"
#include "CDialogEx.h"
#include "CDibCtrl.h"
#include "CFilenameFactory.h"
#include "CFileDialogEx.h"
#include "CFindFile.h"
#include "CTextFile.h"
#include "CHtml.h"
#include "CHttp.h"
#include "CImageFactory.h"
#include "COleDropTargetEx.h"
#include "CProgressBar.h"
#include "CRegKey.h"
#include "CBase64.h"
#include "CMP3Info.h"
#include "CSock.h"
#include "srdll.h"
#include "CIcy.h"
#include "CId3Lib.h"
#include "CThread.h"
#include "CUrl.h"
#include "CUrlDatabaseService.h"
#include "CUrlHistoryService.h"
#include "CWildCards.h"
#include "CWndLayered.h"
#include "CWindowsVersion.h"
#include "WallPaperDoNotAskMore.h"
#include "WallBrowserVersion.h"
#include "WallPaperConfig.h"
#include "WallPaperCrawlerDlg.h"
#include "WallPaperDomainDlg.h"
#include "WallPaperMessages.h"
#include "WallPaperPlayerDlg.h"
#include "WallPaperParentUrlDlg.h"
#include "WallPaperThreadList.h"
#include "WallPaperThumbnailsDlg.h"
#include "WallPaperVersion.h"
#include "resource.h"

#include "traceexpr.h"
//#define _TRACE_FLAG	_TRFLAG_TRACEOUTPUT
//#define _TRACE_FLAG	_TRFLAG_NOTRACE
#define _TRACE_FLAG		_TRFLAG_TRACEOUTPUT
#define _TRACE_FLAG_INFO	_TRFLAG_TRACEOUTPUT
#define _TRACE_FLAG_WARN	_TRFLAG_TRACEOUTPUT
#define _TRACE_FLAG_ERR	_TRFLAG_TRACEOUTPUT

#if (defined(_DEBUG) && defined(_WINDOWS)) && (defined(_AFX) || defined(_AFXDLL))
#ifdef PRAGMA_MESSAGE_VERBOSE
  #pragma message("\t\t\t"__FILE__"("STR(__LINE__)"): using DEBUG_NEW macro")
#endif
  #define new DEBUG_NEW
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CWallPaperCrawlerDlg,CDialogEx)
	ON_WM_QUERYNEWPALETTE()
	ON_WM_PALETTECHANGED()
	ON_WM_SETFOCUS()
	ON_MESSAGE(WM_DROPOLE,OnDropOle)
	ON_MESSAGE(WM_CRAWLER_DONE,OnCrawlerDone)
	ON_MESSAGE(WM_TOOLTIP_CALLBACK,OnTooltipCallback)
	ON_BN_CLICKED(IDC_CHECK_CRAWLER_PREVIEW,OnCheckPicturePreview)
	ON_BN_CLICKED(IDC_BUTTON_SCRIPTFILE,OnScriptFile)
	ON_BN_CLICKED(IDOK,OnOk)
END_MESSAGE_MAP()

IMPLEMENT_DYNCREATE(CWallPaperCrawlerDlg,CDialogEx)

/*
	DoDataExchange()
*/
void CWallPaperCrawlerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_COMBO_URL,m_wndComboUrl);
	DDX_Control(pDX,IDC_DOWNLOAD,m_wndAnimatedAvi);
	DDX_Check(pDX,IDC_CHECK_CRAWLER_PREVIEW,m_bPicturePreview);
}

/*
	CWallPaperCrawlerDlg()
*/
CWallPaperCrawlerDlg::CWallPaperCrawlerDlg(HWND hWndParent)
: CDialogEx(	/*nTemplateID*/		IDD_DIALOG_CRAWLER,
			/*nToolbarID*/			0L,
			/*nStatusbarID*/		0L,
			/*nIconID*/			IDI_ICON_CRAWLER,
			/*nMenuID*/			0L,
#ifdef _RESIZABLE_DIALOG_STYLE
			/*bAllowResize*/		FALSE,
#endif
			/*bAllowSnap*/			TRUE,
			/*bAllowDragAndDrop*/	TRUE,
			/*lpcszUniqueName*/		::GetUniqueMutexName(IDS_DIALOG_CRAWLER_TITLE),
			/*bAllowMultipleInstances*/TRUE,
			/*hWndParent*/			NULL)
{
	m_hWndParent = hWndParent;
	m_bAutoRun = FALSE;

	m_pConfig = NULL;
		
	// chiede al chiamante se sono stati impostati dei parametri (per il lancio automatico quando l'url viene droppata sulla finestra principale)
	if(m_hWndParent)
	{
		LPARAM lParam = ::SendMessage(m_hWndParent,WM_THREAD_PARAMS,0,::GetCurrentThreadId());
		if(lParam)
		{
			// ricava i parametri
			CRAWLERPARAMS* pParams = (CRAWLERPARAMS*)lParam;
			if(pParams)
			{
				// ricava il pntatore alla configurazione
				if(pParams->pConfig)
					m_pConfig = pParams->pConfig;
				
				// ricava l'url impostata dal chiamante
				if(!strnull(pParams->szUrl))
				{
					// imposta per l'avvio automatico
					m_bAutoRun = TRUE;

					char szUrl[MAX_URL+1] = {0};
					strcpyn(szUrl,pParams->szUrl,sizeof(szUrl));
				
					// aggiorna il valore dell'url di default nella configurazione e nel database
					if(m_pConfig)
					{
						m_pConfig->UpdateString(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_ADDURL_KEY,szUrl);
						m_pConfig->SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_ADDURL_KEY);
					
						CUrlHistoryService UrlHistoryService(URL_HISTORY_TABLE,m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_DATADIR_KEY));
						if(UrlHistoryService.IsValid())
							UrlHistoryService.Add(szUrl);
					}
				}
			
				delete (CRAWLERPARAMS*)lParam;
			}
		}
	}
	
	if(!m_pConfig)
	{
		static CWallPaperConfig m_Config;
		m_pConfig = &m_Config;
	}

	strcpyn(m_szTitle,IDS_DIALOG_CRAWLER_TITLE,sizeof(m_szTitle));
	m_nCancel = 0;
	m_nCancelScript = 0;
	m_nCrawlerMode = CRAWLER_DOWNLOAD_MODE;
	m_bInCrawling = FALSE;
	m_nActiveThreads = 0;
	m_nThreadsPeak = 0;
	m_bAddSongsToPlaylist = TRUE;
	m_bAddFilesToPlaylist = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_ADDPICTURES_KEY);
	
	m_PictPreviewSync.SetName(::GetUniqueMutexName("PicturePreview"));
	m_PictPreviewSync.SetTimeout(SYNC_3_SECS_TIMEOUT);

	m_StreamRipperSync.SetName("CrawlPaperStreamRipper");
	m_StreamRipperSync.SetTimeout(SYNC_1_SECS_TIMEOUT);

	memset(m_szDownloadDir,'\0',sizeof(m_szDownloadDir));
	memset(m_szUrl,'\0',sizeof(m_szUrl));
	m_bUseScript = FALSE;
	memset(m_szScriptFile,'\0',sizeof(m_szScriptFile));
	m_dlTotalTime = 0.0f;
	m_dlDownloadTime = 0.0f;
	memset(m_szStartTime,'\0',sizeof(m_szStartTime));
	memset(m_szEndTime,'\0',sizeof(m_szEndTime));
	memset(m_szScriptStartTime,'\0',sizeof(m_szScriptStartTime));
	memset(m_szScriptEndTime,'\0',sizeof(m_szScriptEndTime));
	m_bPicturePreview = TRUE;
	strcpyn(m_szLibraryName,m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_CURRENTLIBRARY_KEY),sizeof(m_szLibraryName));
	m_pImage = m_ImageFactory.Create(m_szLibraryName,sizeof(m_szLibraryName));
	m_pImage->ShowErrors(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_QUIETINSCRIPTMODE_KEY) ? TRUE : FALSE);
	m_nConnectionTimeout = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_CONNECTIONTIMEOUT_KEY);
	m_bUseWinsockTimeout = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_USEWINSOCKTIMEOUT_KEY);
	memset(m_szCurrentLocation,'\0',sizeof(m_szCurrentLocation));
	m_qwCurrentAmount = (QWORD)-1L;
	m_hTimeoutEvent = (HANDLE)NULL;
	m_hStreamRipperEvent = (HANDLE)NULL;
	m_bInBreakMessage = m_bInCancelDialog = FALSE;
}

/*
	OnInitDialog()
*/
BOOL CWallPaperCrawlerDlg::OnInitDialog(void)
{
	// classe base
	if(!CDialogEx::OnInitDialog())
	{
		EndDialog(IDCANCEL);
		return(FALSE);
	}

	// modifica il menu di sistema
	CMenu* pSysmenu = GetSystemMenu(FALSE);
	if(pSysmenu)
	{
		pSysmenu->RemoveMenu(0,MF_BYPOSITION); // restore
		//pSysmenu->RemoveMenu(0,MF_BYPOSITION); // move
		pSysmenu->RemoveMenu(1,MF_BYPOSITION); // size
		pSysmenu->RemoveMenu(1,MF_BYPOSITION); // min
		pSysmenu->RemoveMenu(1,MF_BYPOSITION); // max
		pSysmenu->RemoveMenu(1,MF_BYPOSITION); // ---
		pSysmenu->RemoveMenu(1,MF_BYPOSITION); // close
		pSysmenu->RemoveMenu(1,MF_BYPOSITION); // ---

		CWindowsVersion winver;
		char* p = winver.IsRunningOnCartoons() ? "   " : "";
		char szMenuItem[32];
		_snprintf(szMenuItem,sizeof(szMenuItem)-1,"%s&Move",p);
		pSysmenu->ModifyMenu(0,MF_BYPOSITION|MF_STRING,SC_MOVE,szMenuItem);
		_snprintf(szMenuItem,sizeof(szMenuItem)-1,"%s&Close\tAlt+F4",p);
		pSysmenu->AppendMenu(MF_STRING,SC_CLOSE,szMenuItem);
		_snprintf(szMenuItem,sizeof(szMenuItem)-1,"%sMinimi&ze",p);
		pSysmenu->AppendMenu(MF_STRING,SC_MINIMIZE,szMenuItem);
		_snprintf(szMenuItem,sizeof(szMenuItem)-1,"%sMa&ximize",p);
		pSysmenu->AppendMenu(MF_STRING,SC_RESTORE,szMenuItem);

		DrawMenuBar();
	}

	// tooltip
	if(m_pConfig->GetNumber(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_TOOLTIP_CRAWLER_KEY))
	{
		char szTooltipText[2048];
		::FormatResourceStringEx(szTooltipText,
							sizeof(szTooltipText)-1,
							IDS_TOOLTIP_EX_CRAWLER,
							WALLPAPER_WEB_SITE,
							IDI_ICON_CRAWLER,
							DEFAULT_SHOUTCAST_WEBPAGE,
							WALLPAPER_WEB_SITE,
							WALLPAPER_WEB_SITE,
							IDI_ICON16X16_CLOSE
							);

		char szCss[512];
		::FormatResourceString(szCss,sizeof(szCss),IDS_TOOLTIP_CSS_STYLE);
		// la zoccola dell'ultima ver. per i tooltips non funge con IDC_STATIC_BACKGROUND
		//AddExtendedToolTip(IDC_STATIC_BACKGROUND,szTooltipText,szCss);
		AddExtendedToolTip(IDC_CRAWLER_PICTURE_PREVIEW,szTooltipText,szCss);
		AddExtendedToolTip(IDC_DOWNLOAD,szTooltipText,szCss);
		AddExtendedToolTip(IDC_STATIC_STATUS,szTooltipText,szCss);
		AddExtendedToolTip(IDC_COMBO_URL,szTooltipText,szCss);
		CPPToolTip* pTooltip = GetExtendedToolTipCtrl();
		pTooltip->SetCallbackHyperlink(this->GetSafeHwnd(),WM_TOOLTIP_CALLBACK);
		//AddToolTip(IDC_COMBO_URL,IDS_TOOLTIP_CRAWLER_ADDURL);
	}
	else
		AddToolTip(IDC_COMBO_URL,IDS_TOOLTIP_CRAWLER_DLG);
	
	AddToolTip(IDC_BUTTON_SCRIPTFILE,IDS_TOOLTIP_CRAWLER_SCRIPT);
	AddToolTip(IDOK,IDS_TOOLTIP_CRAWL);
	AddToolTip(IDC_CHECK_CRAWLER_PREVIEW,IDS_TOOLTIP_PREVIEW);
	AddToolTip(IDCANCEL,IDS_TOOLTIP_CLOSE);

	// abilita il drag and drop via OLE
	// la classe base (CDialogEx) gestisce solo il drag and drop tramite la shell (files)
	if(m_oleDropTarget.Attach(this,WM_DROPOLE,TRUE))
	{
		m_oleDropTarget.Accept(DROPEFFECT_COPY);
		m_oleDropTarget.AddClipboardFormat(NETSCAPE_CLIPBOARDFORMAT_NAME);
		m_oleDropTarget.AddClipboardFormat(IEXPLORER_CLIPBOARDFORMAT_NAME);
	}

	// ricava l'ultima url utilizzata (se sono stati passati parametri e' quella di cui sopra)
	char szUrl[MAX_URL+1] = {0};
	strcpyn(szUrl,m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_ADDURL_KEY),sizeof(szUrl));
	if(strcmp(szUrl,"")==0)
		strcpyn(szUrl,LOCAL_HOST,sizeof(szUrl));

	// inserisce nel combo le url presenti nel database
	CUrlHistoryService UrlHistoryService(URL_HISTORY_TABLE,m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_DATADIR_KEY));
	if(UrlHistoryService.IsValid())
	{
		// inserisce nel database l'ultima url utilizzata
		UrlHistoryService.Add(szUrl);

		// ricava le url presenti nel database
		CUrlHistoryTable* pUrlHistory = UrlHistoryService.GetTable();
		if(pUrlHistory)
		{
			pUrlHistory->SetIndex(URL_HISTORY_IDX_URL);
			if(pUrlHistory->GoTop())
			{
				while(!pUrlHistory->Eof())
				{
					pUrlHistory->ScatterMemvars();

					if(m_wndComboUrl.FindStringExact(-1,pUrlHistory->GetField_Url())==CB_ERR)
						m_wndComboUrl.AddString(pUrlHistory->GetField_Url());

					pUrlHistory->GetNext();
				}
			}
		}
	}

	// imposta come corrente l'ultima url utilizzata
	strcpyn(m_szUrl,szUrl,sizeof(m_szUrl));
	int nIndex = m_wndComboUrl.FindStringExact(-1,m_szUrl);
	if(nIndex==CB_ERR)
		nIndex = m_wndComboUrl.AddString(m_szUrl);
	m_wndComboUrl.SetCurSel(nIndex);

	// progress bar
	m_wndProgressBar.Attach(this->m_hWnd,IDC_PROGRESS);

	// preview
	m_wndStaticDib.SubclassDlgItem(IDC_CRAWLER_PICTURE_PREVIEW,this);

	// interfaccia utente
	SetWindowText(m_szTitle);
	SetDlgItemText(IDC_STATIC_STATUS,"");
	SetDlgItemText(IDC_STATIC_ESTIMATED_TIME_LEFT,"");
	SetDlgItemText(IDC_ESTIMATED_TIME_LEFT,"");
	SetDlgItemText(IDC_STATIC_CURRENT_STATUS,"");
	SetDlgItemText(IDC_CURRENT_STATUS,"");
	SetDlgItemText(IDC_STATIC_TRANSFER_RATE,"");
	SetDlgItemText(IDC_TRANSFER_RATE,"");
	m_wndProgressBar.SetPos(0);
	m_wndProgressBar.Hide();
	UpdateStatusBar();
	SetDlgItemText(IDC_STATIC_TOTAL_THREADS,"");
	m_wndStaticDib.Unload();
	if(m_nCrawlerMode==CRAWLER_STREAMRIPPER_MODE)
		m_bPicturePreview = FALSE;
	else
		m_bPicturePreview = TRUE;
	GetDlgItem(IDC_CHECK_CRAWLER_PREVIEW)->EnableWindow(m_bPicturePreview);
	SetDlgItemText(IDC_TEXT_INFO,"");
	SetDlgItemText(IDOK,"Cra&wl");
	GetDlgItem(IDOK)->EnableWindow(TRUE);
	SetDlgItemText(IDCANCEL,"&Close");

	// inizializza il dialogo
	OnInitDialogEx();

	UpdateData(FALSE);

	// trasparenza
	if(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAYERED_KEY))
		m_wndLayered.SetLayer(this->m_hWnd,(BYTE)m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAYERED_VALUE_KEY));

	// mette in primo piano
	SetForegroundWindowEx(this->m_hWnd);

	if(m_bAutoRun)
		PostMessage(WM_COMMAND,MAKELONG(IDOK,0),0L);

	return(TRUE);
}

/*
	OnQueryNewPalette()
*/
BOOL CWallPaperCrawlerDlg::OnQueryNewPalette(void)
{
	m_wndStaticDib.SendMessage(WM_QUERYNEWPALETTE);
	return(CDialog::OnQueryNewPalette());
}

/*
	OnPaletteChanged()
*/
void CWallPaperCrawlerDlg::OnPaletteChanged(CWnd* pFocusWnd)
{
	CDialog::OnPaletteChanged(pFocusWnd);
	m_wndStaticDib.SendMessage(WM_PALETTECHANGED,(WPARAM)pFocusWnd->GetSafeHwnd());
}

/*
	OnSetFocus()
*/
void CWallPaperCrawlerDlg::OnSetFocus(CWnd* pOldWnd)
{
	CDialog::OnSetFocus(pOldWnd);
	m_wndStaticDib.SendMessage(WM_QUERYNEWPALETTE);
}

/*
	OnOk()
*/
void CWallPaperCrawlerDlg::OnOk(void)
{
	CUrl url;
	m_bUseScript = FALSE;
	BOOL bUpdated = UpdateData(TRUE);
	BOOL bHaveStreamServer = FALSE;

	// imposta la modalita' di default
	m_nCrawlerMode = CRAWLER_DOWNLOAD_MODE;

	// ricava l'url specificata nel combo
	OnGetUrlFromCombo();

	// controlla l'url
	if(bUpdated)
	{
		// lunghezza
		int nLen = strlen(m_szUrl);
		if(nLen <= 0 || nLen > MAX_URL)
		{
			::MessageBoxResourceEx(this->m_hWnd,MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_INVALID_URL_LEN,m_szUrl,1,MAX_URL);
			bUpdated = FALSE;
		}
	}
	
	if(bUpdated)
	{
		// controlla se l'url punta ad uno stream audio
		if(url.IsUrlType(m_szUrl,ICY_URL))
		{
			m_nCrawlerMode = CRAWLER_STREAMRIPPER_MODE;
			bHaveStreamServer = TRUE;
		}
		// controlla se l'url punta ad una playlist audio
		else if(url.IsUrlType(m_szUrl,HTTP_URL) && (striright(m_szUrl,".pls")==0 || striright(m_szUrl,".m3u")==0))
		{
			m_nCrawlerMode = CRAWLER_STREAMRIPPER_MODE;
			bHaveStreamServer = TRUE;
		}
		// non si tratta di un url http
		else if(!url.IsUrlType(m_szUrl,HTTP_URL))
		{
			BOOL bInvalidUrl = TRUE;
			
			// controlla se si tratta di un file script
			if(m_szUrl[0]=='@')
			{
				// distingue tra files script (.txt), files generati dal ripper (.inf) e playlist (.pls, .m3u)
				
				// file script (.txt)
				if(striright(m_szUrl,".txt")==0)
				{
					// carica il contenuto (le url) del file script nella lista ed invia il messaggio
					// il gestore relativo si occupa di lanciare il thread per il download
					strcpyn(m_szScriptFile,m_szUrl+1,sizeof(m_szScriptFile));
					bInvalidUrl = !LoadUrlsFromScript(m_szScriptFile,&m_listUrls);
					if(!bInvalidUrl)
					{
						m_bUseScript = TRUE;
						
						// data/ora inizio download
						CDateTime datetime(GMT);
						strcpyn(m_szScriptStartTime,datetime.GetFormattedDate(TRUE),sizeof(m_szScriptStartTime));

						bUpdated = FALSE;
						::PostMessage(this->m_hWnd,WM_CRAWLER_DONE,0,0L);
					}
				}
				// file generato dal ripper (.inf)
				// al ripper viene passata l'url http relativa al file .pls/.m3u
				else if(striright(m_szUrl,".inf")==0)
				{
					// ricava dal file l'url relativa alla playlist (http://[...]/playlist.pls|m3u)
					strcpyn(m_szScriptFile,m_szUrl+1,sizeof(m_szScriptFile));
					if(::GetPrivateProfileString("Info","Playlist","",m_szUrl,sizeof(m_szUrl),m_szScriptFile) > 0)
					{
						if((url.IsUrlType(m_szUrl,HTTP_URL) && (striright(m_szUrl,".pls")==0 || striright(m_szUrl,".m3u")==0)))
						{
							bInvalidUrl = FALSE;
							m_nCrawlerMode = CRAWLER_STREAMRIPPER_MODE;
						}
					}
				}
				// playlist (.pls)
				// al ripper viene passata l'url icy relativa allo stream (icy://12.34.56.78:9012)
				else if(striright(m_szUrl,".pls")==0)
				{
					bHaveStreamServer = FALSE;

					// ricava dal file l'url relativa modificandone il protocollo (da http a icy)
					strcpyn(m_szScriptFile,m_szUrl+1,sizeof(m_szScriptFile));
					if(::GetPrivateProfileString("playlist","File1","",m_szUrl,sizeof(m_szUrl),m_szScriptFile) <= 0)
						if(::GetPrivateProfileString("playlist","File2","",m_szUrl,sizeof(m_szUrl),m_szScriptFile) <= 0)
							memset(m_szUrl,'\0',sizeof(m_szUrl));
					if((bInvalidUrl = !url.IsUrlType(m_szUrl,HTTP_URL))==FALSE)
					{
						// se il .pls contiene riferimenti a files audio invece che all'url del server
						// per lo stream cambia la modalita' per il download (da ripper a crawler)
						if(!CAudioPlayer::IsAudioFile(m_szUrl))
						{
							char szIcyUrl[MAX_URL+1];
							substr(m_szUrl,"http:","icy:",szIcyUrl,sizeof(szIcyUrl));
							strcpyn(m_szUrl,szIcyUrl,sizeof(m_szUrl));
							m_nCrawlerMode = CRAWLER_STREAMRIPPER_MODE;
							bHaveStreamServer = TRUE;
						}
					}
				}
				// playlist (.m3u)
				// al ripper viene passata l'url icy relativa allo stream (icy://12.34.56.78:9012)
				else if(striright(m_szUrl,".m3u")==0)
				{
					bHaveStreamServer = FALSE;

					// ricava dal file l'url relativa modificandone il protocollo (da http a icy)
					strcpyn(m_szScriptFile,m_szUrl+1,sizeof(m_szScriptFile));
					CTextFile m3uFile;
					if(m3uFile.Open(m_szScriptFile,FALSE))
					{
						while(m3uFile.ReadLine(m_szUrl,sizeof(m_szUrl)-1)!=FILE_EOF)
						{
							if((bInvalidUrl = !url.IsUrlType(m_szUrl,HTTP_URL))==FALSE)
							{
								// se il .m3u contiene riferimenti a files audio invece che all'url del server
								// per lo stream cambia la modalita' per il download (da ripper a crawler)
								if(!CAudioPlayer::IsAudioFile(m_szUrl))
								{
									char szIcyUrl[MAX_URL+1];
									substr(m_szUrl,"http:","icy:",szIcyUrl,sizeof(szIcyUrl));
									strcpyn(m_szUrl,szIcyUrl,sizeof(m_szUrl));
									m_nCrawlerMode = CRAWLER_STREAMRIPPER_MODE;
									bHaveStreamServer = TRUE;
									break;
								}
							}
						}

						m3uFile.Close();
					}
				}
			}

			if(bInvalidUrl)
			{
				::MessageBoxResourceEx(this->m_hWnd,MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_INVALID_HTTPURL_SCRIPTFILE,m_szUrl);
				bUpdated = FALSE;
			}
		}
	}

	// se la playlist contiene dei riferimenti http a files audio invece che al server per lo stream, forza la modalita' (da ripper a crawler)
	if(!bHaveStreamServer)
		m_nCrawlerMode = CRAWLER_DOWNLOAD_MODE;

	// lancia il thread per il download
	if(bUpdated)
	{
		GetDlgItem(IDOK)->EnableWindow(FALSE);
		//SetDlgItemText(IDCANCEL,"&Cancel");
		
		if(m_nCrawlerMode==CRAWLER_STREAMRIPPER_MODE)
		{
			SetDlgItemText(IDCANCEL,"&Stop");
			if((m_hStreamRipperEvent = ::CreateEvent(NULL,TRUE,FALSE,NULL))!=(HANDLE)NULL)
				AfxBeginThread(AudioCrawler,this,m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PRIORITY_THREAD_KEY));
		}
		else
		{
			SetDlgItemText(IDCANCEL,"&Cancel");
			AfxBeginThread(Crawler,this,m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PRIORITY_THREAD_KEY));
		}
	}
}

/*
	OnExit()
*/
void CWallPaperCrawlerDlg::OnExit(void)
{
	static int nCancelled = 0;

	// cancel disabilitato
	if(!GetDlgItem(IDCANCEL)->IsWindowEnabled())
		return;

	// imposta il flag
	m_nCancel++;
	m_nCancelScript++;

	// controlla il prompt del bottone
	CString cText;
	GetDlgItemText(IDCANCEL,cText);
	
	// il thread NON ha terminato il download, arriva qui perche' il collegamento bloccante col
	// socket non permette di sentire il cancel, chiude quindi la connessione, nel caso di uno
	// stream audio imposta l'evento per sbloccare il thread rimasto in attesa
	if(cText=="&Cancel" || cText=="&Stop")
	{
		if(m_nCrawlerMode==CRAWLER_STREAMRIPPER_MODE)
		{
			m_httpConnection.Abort();
			
			m_icy.Abort();
			
			if(m_hStreamRipperEvent)
				::SetEvent(m_hStreamRipperEvent);
		}
		else if(m_nCrawlerMode==CRAWLER_DOWNLOAD_MODE)
		{
			if(++nCancelled > 2)
			{
				nCancelled = 0;
				if(m_bInCrawling)
				{
					if(!m_bInCancelDialog)
					{
						m_bInBreakMessage = TRUE;
						::MessageBeep(MB_ICONEXCLAMATION);
						if(::MessageBoxResource(this->m_hWnd,MB_YESNO|MB_ICONQUESTION,WALLPAPER_PROGRAM_NAME,IDS_QUESTION_BREAK_CONNECTION)==IDYES)
							m_httpConnection.Abort();
						m_bInBreakMessage = FALSE;
					}
				}
			}
		}
	}
	// il thread ha terminato il download, chiude il dialogo inviando il messaggio relativo alla finestra principale
	else if(cText=="&Close")
	{
		if(m_hWndParent)
			::SendMessage(m_hWndParent,WM_CRAWLER_DONE,0,0);

		// termina
		CDialogEx::OnExit();
	}
}

/*
	OnScriptFile()
*/
void CWallPaperCrawlerDlg::OnScriptFile(void)
{
	CFileOpenDialog dlg("Open Script File...",
					m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_DIR_KEY),
					"*.txt",
					"Script files (*.txt)|*.txt|Ripper info files (*.inf)|*.inf|Playlist files (*.pls)|*.pls|Winamp playlist (*.m3u)|*.m3u||"
					);

	if(dlg.DoModal()==IDOK)
	{
		_snprintf(m_szUrl,sizeof(m_szUrl)-1,"@%s",dlg.GetPathName());
		
		// aggiorna il combo senza aggiornare la configurazione
		int nIndex;
		if((nIndex = m_wndComboUrl.AddString(m_szUrl))!=CB_ERR)
			m_wndComboUrl.SetCurSel(nIndex);
		
		UpdateData(FALSE);
	}
}

/*
	OnGetUrlFromCombo()

	Aggiorna il valore dell'url utilizzato per il download e la configurazione.
*/
void CWallPaperCrawlerDlg::OnGetUrlFromCombo(void)
{
	// per far si che non venga modificata la url durante il download
	if(m_bInCrawling)
		return;

	// ricava il testo del combo
	if(m_wndComboUrl.GetWindowText(m_szUrl,sizeof(m_szUrl)) > 0)
	{
		// aggiorna il valore dell'url di default nella configurazione e nel database
		m_pConfig->UpdateString(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_ADDURL_KEY,m_szUrl);
		m_pConfig->SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_ADDURL_KEY);

		CUrlHistoryService UrlHistoryService(URL_HISTORY_TABLE,m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_DATADIR_KEY));
		if(UrlHistoryService.IsValid())
			UrlHistoryService.Add(m_szUrl);
	}
}

/*
	OnCheckPicturePreview()
*/
void CWallPaperCrawlerDlg::OnCheckPicturePreview(void)
{
	m_bPicturePreview = !m_bPicturePreview;
	if(!m_bPicturePreview)
	{
		if(m_PictPreviewSync.Lock())
		{
			m_listPictPreview.RemoveAll();
			m_wndStaticDib.Unload();
			SetDlgItemText(IDC_TEXT_INFO,"");
			m_PictPreviewSync.Unlock();
		}
	}
}

/*
	OnDropFiles()

	Imposta l'url con quanto ricevuto via drag and drop dalla shell.
*/
void CWallPaperCrawlerDlg::OnDropFiles(void)
{
	char szFileName[_MAX_PATH+1];
	DWORD dwAttribute;
	int nTotDrops = 0;

	// mette in primo piano quando riceve via drag & drop
	SetForegroundWindowEx(this->m_hWnd);

	// per ogni file ricevuto
	while(CDialogEx::GetDroppedFile(szFileName,sizeof(szFileName),&dwAttribute))
	{
		// controlla che si tratti di un file e non di una directory
		if(!(dwAttribute & FILE_ATTRIBUTE_DIRECTORY))
		{
			// controlla se si tratta di un URL (.url)
			if(striright(szFileName,DEFAULT_URL_EXT)==0)
			{
				char szUrl[MAX_URL+1];
				if(::GetPrivateProfileString("InternetShortcut","URL","",szUrl,sizeof(szUrl),szFileName) > 0)
				{
					// aggiorna l'url
					strcpyn(m_szUrl,szUrl,sizeof(m_szUrl));
					UpdateData(FALSE);

					// aggiorna il combo
					int nIndex;
					if((nIndex = m_wndComboUrl.FindStringExact(-1,m_szUrl))==CB_ERR)
						nIndex = m_wndComboUrl.AddString(m_szUrl);
					m_wndComboUrl.SetCurSel(nIndex);

					// aggiorna il valore dell'url di default nella configurazione e nel database
					m_pConfig->UpdateString(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_ADDURL_KEY,m_szUrl);
					m_pConfig->SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_ADDURL_KEY);

					CUrlHistoryService UrlHistoryService(URL_HISTORY_TABLE,m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_DATADIR_KEY));
					if(UrlHistoryService.IsValid())
						UrlHistoryService.Add(m_szUrl);
				
					nTotDrops++;
				}
				else
					::MessageBoxResourceEx(this->m_hWnd,MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_DROPPED_URL,szFileName);
			}
			// file script, playlist (.pls|.m3u), info (.inf)
			else if(striright(szFileName,".txt")==0 || striright(szFileName,".inf")==0 || striright(szFileName,".pls")==0 || striright(szFileName,".m3u")==0)
			{
				// aggiorna l'url
				_snprintf(m_szUrl,sizeof(m_szUrl)-1,"@%s",szFileName);
				UpdateData(FALSE);

				// aggiorna il combo
				int nIndex;
				if((nIndex = m_wndComboUrl.FindStringExact(-1,m_szUrl))==CB_ERR)
					nIndex = m_wndComboUrl.AddString(m_szUrl);
				m_wndComboUrl.SetCurSel(nIndex);

				// aggiorna il valore dell'url di default nella configurazione e nel database
				// (occhio che visualizza il file script come url nel dialogo addurl)
				m_pConfig->UpdateString(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_ADDURL_KEY,m_szUrl);
				m_pConfig->SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_ADDURL_KEY);

				CUrlHistoryService UrlHistoryService(URL_HISTORY_TABLE,m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_DATADIR_KEY));
				if(UrlHistoryService.IsValid())
					UrlHistoryService.Add(m_szUrl);

				nTotDrops++;
			}
			// file sconosciuto
			else
			{
				::MessageBoxResourceEx(this->m_hWnd,MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_DROPPED_FILE,szFileName);
			}
		}
	}
}

/*
	OnDropOle()

	Imposta l'url con quanto ricevuto via drag and drop tramite OLE.
	Il tipo dell'oggetto ricevuto dipende da quanto registrato con la chiamata ad AddClipboardFormat().
*/
LONG CWallPaperCrawlerDlg::OnDropOle(UINT /*wParam*/,LONG lParam)
{
	LPSTR pUrl = (LPSTR)lParam;

	// mette in primo piano quando riceve via drag & drop
	SetForegroundWindowEx(this->m_hWnd);
	
	// aggiorna l'url
	strcpyn(m_szUrl,pUrl,sizeof(m_szUrl));
	UpdateData(FALSE);

	// aggiorna il combo
	int nIndex;
	if((nIndex = m_wndComboUrl.FindStringExact(-1,m_szUrl))==CB_ERR)
		nIndex = m_wndComboUrl.AddString(m_szUrl);
	m_wndComboUrl.SetCurSel(nIndex);

	// aggiorna il valore dell'url di default nella configurazione e nel database
	m_pConfig->UpdateString(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_ADDURL_KEY,m_szUrl);
	m_pConfig->SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_ADDURL_KEY);

	CUrlHistoryService UrlHistoryService(URL_HISTORY_TABLE,m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_DATADIR_KEY));
	if(UrlHistoryService.IsValid())
		UrlHistoryService.Add(m_szUrl);

	return(0L);
}

/*
	OnTooltipCallback()
*/
LRESULT CWallPaperCrawlerDlg::OnTooltipCallback(WPARAM wParam,LPARAM /*lParam*/)
{
	LPSTR pMsg = (LPSTR)wParam;
	if(pMsg)
	{
		if(stricmp(pMsg,"Crawler")==0)
		{
			m_pConfig->UpdateNumber(WALLPAPER_DONOTASKMORE_KEY,WALLPAPER_DONOTASKMORE_TOOLTIP_CRAWLER_KEY,0);
			RemoveToolTip(IDC_CRAWLER_PICTURE_PREVIEW);
			RemoveToolTip(IDC_DOWNLOAD);
			RemoveToolTip(IDC_STATIC_STATUS);
			RemoveToolTip(IDC_COMBO_URL);
			AddToolTip(IDC_COMBO_URL,IDS_TOOLTIP_CRAWLER_DLG);
		}
		m_pConfig->SaveSection(WALLPAPER_DONOTASKMORE_KEY);
	}
	
	return(0L);
}

/*
	PicturePreview()

	Wrapper per il thread.
*/
UINT CWallPaperCrawlerDlg::PicturePreview(LPVOID lpVoid)
{
	UINT nRet = 0L;

	CWallPaperCrawlerDlg* pThis = (CWallPaperCrawlerDlg*)lpVoid;
	if(pThis)
		/*nRet = */pThis->PicturePreview();
	
	return(nRet);
}

/*
	PicturePreview()

	Thread per la previsualizzazione dell'immagine.
*/
void CWallPaperCrawlerDlg::PicturePreview(void)
{
	if(m_bPicturePreview && m_nCancel==0 && m_listPictPreview.Count() > 0)
		if(m_PictPreviewSync.Lock())
		{
			BOOL bLoaded = FALSE;
			ITERATOR iter = m_listPictPreview.First();
			if(iter!=(ITERATOR)NULL)
			{
				ITEM* item = (ITEM*)iter->data;
				if(item->item)
				{
					if(m_pImage->Load(item->item))
					{
						if(m_wndStaticDib.Load(m_pImage))
						{
							bLoaded = TRUE;
							char* p = strrchr(item->item,'\\');
							
							char szMemUsed[32];
							strsize(szMemUsed,sizeof(szMemUsed),m_pImage->GetMemUsed());

							int nColors = m_pImage->GetNumColors();

							char szImageInfo[_MAX_PATH+1];
							_snprintf(szImageInfo,
									sizeof(szImageInfo)-1,
									"%s\n%d x %d pixels\n%d%s colors x %d bpp\n%s required",
									(p && p+1) ? p+1 : item->item,
									m_pImage->GetWidth(),
									m_pImage->GetHeight(),
									(nColors > 256 || nColors==0) ? 16 : nColors,
									(nColors > 256 || nColors==0) ? "M" : "",
									m_pImage->GetBPP(),
									szMemUsed
									);

							SetDlgItemText(IDC_TEXT_INFO,szImageInfo);
						}
					}
				}

				m_listPictPreview.Remove(iter);
			}
			
			if(!bLoaded)
			{
				m_wndStaticDib.Unload();
				SetDlgItemText(IDC_TEXT_INFO,"");
			}

			m_PictPreviewSync.Unlock();
		}
}

/*
	DoPreview()

	Lancia il thread per l'anteprima dell'immagine.
*/
void CWallPaperCrawlerDlg::DoPreview(LPCSTR lpcszFileName)
{
	if(m_bPicturePreview && m_nCancel==0)
		if(m_pImage->IsSupportedFormat(lpcszFileName))
			if(m_PictPreviewSync.Lock())
			{
				ITEM* item = (ITEM*)m_listPictPreview.Add();
				if(item)
					strcpyn(item->item,lpcszFileName,_MAX_ITEM+1);

				m_PictPreviewSync.Unlock();

				if(item)
					::AfxBeginThread(PicturePreview,this,m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PRIORITY_THREAD_KEY));
			}
}

/*
	GetHttpUrlCallback()

	Wrapper per la callback per il download del file.
*/
UINT CWallPaperCrawlerDlg::GetHttpUrlCallback(LPVOID pHttpConnection,LPVOID lpVoid)
{
	UINT nRet = (UINT)-1L;

	CWallPaperCrawlerDlg* pThis = (CWallPaperCrawlerDlg*)lpVoid;
	if(pThis)
		nRet = pThis->GetHttpUrlCallback(pHttpConnection);

	return(nRet);
}

/*
	GetHttpUrlCallback()

	Callback per il download del file.
*/
UINT CWallPaperCrawlerDlg::GetHttpUrlCallback(LPVOID lpVoid)
{
	URL Url;
	CUrl url;
	char szUrl[MAX_URL+1];
	char szBuffer[1024];
	CHttpConnection* pHttpConnection = (CHttpConnection*)lpVoid;
	int nRet = (m_nCancel==0 ? IDOK : IDCANCEL);

	// nome di default per .html
	if(pHttpConnection->GetObject()->stat==HTTPOBJECTSTAT_DEFAULT_HTMLFNAME)
	{
		strcpyn(pHttpConnection->GetObject()->defaultfname,m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_DEFAULTHTMLFILENAME_KEY),_MAX_FNAME+1);
		nRet = (m_nCancel==0 ? IDOK : IDCANCEL);
	}
	// nome di default per .mp3
	else if(pHttpConnection->GetObject()->stat==HTTPOBJECTSTAT_DEFAULT_MP3FNAME)
	{
		strcpyn(pHttpConnection->GetObject()->defaultfname,m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_DEFAULTMP3FILENAME_KEY),_MAX_FNAME+1);
		nRet = (m_nCancel==0 ? IDOK : IDCANCEL);
	}
	// stream audio
	else if(pHttpConnection->GetObject()->stat==HTTPOBJECTSTAT_ICECAST_STREAM || pHttpConnection->GetObject()->stat==HTTPOBJECTSTAT_SHOUTCAST_STREAM)
	{
		if(::MessageBoxResourceEx(	this->m_hWnd,
								MB_OKCANCEL|MB_ICONQUESTION,
								WALLPAPER_PROGRAM_NAME,
								IDS_QUESTION_AUDIO_STREAM,
								m_szUrl,
								pHttpConnection->GetObject()->stat==HTTPOBJECTSTAT_ICECAST_STREAM ? "an Icecast" : "a Shoutcast"
								)==IDOK)
		{
			nRet = IDOK;
		}
		else
		{
			/* inizia bene ma poi fa badaplin! su pThis nella callback quando si stoppa il ripper
			memcpy(m_szUrl," icy://",7);
			strltrim(m_szUrl);
			PostMessage(WM_COMMAND,MAKELONG(IDOK,0),0L);
			*/
			nRet = IDCANCEL;
		}
	}
	// prima del download e della creazione del nomefile locale corrispondente all'url, da usare per 
	// effettuare gli eventuali controlli/modifiche del nomefile in questione prima della sua creazione
	else if(pHttpConnection->GetObject()->stat==HTTPOBJECTSTAT_LOCAL_FILENAME)
	{
		// per non sovrascrivere eventuali .mp3/files audio gia' esistenti
		CAudioPlayer audioPlayer;
		if(audioPlayer.IsAudioFile(pHttpConnection->GetObject()->filename))
		{
			if(::FileExist(pHttpConnection->GetObject()->filename))
			{
				int n = 0;
				char* p;
				char szBuffer[_MAX_FILEPATH+1] = {0};
				char szFilename[_MAX_FILEPATH+1] = {0};
				char szExt[_MAX_FILEPATH+1] = {0};
				
				strcpyn(szFilename,pHttpConnection->GetObject()->filename,sizeof(szFilename));
				if((p = strrchr(szFilename,'.'))!=NULL)
				{
					strcpyn(szExt,p,sizeof(szExt));
					*p = '\0';
				}

				do {
					_snprintf(szBuffer,sizeof(szBuffer)-1,"%s(%d)%s",szFilename,++n,szExt);
				} while(::FileExist(szBuffer));
				
				strcpyn(pHttpConnection->GetObject()->filename,szBuffer,_MAX_FILEPATH+1);
			}
		}
		
		// imposta il codice di ritorno	
		nRet = (m_nCancel==0 ? IDOK : IDCANCEL);
	}
	// inizio download
	else if(pHttpConnection->GetObject()->stat==HTTPOBJECTSTAT_BEGIN_RECEIVING)
	{
		// per l'aggiunta al volo x files audio > 25MB (wParam) e per la 
		// barra di progresso per gli stream (audio) infiniti (lParam)
		pHttpConnection->GetObject()->wParam = pHttpConnection->GetObject()->lParam = 0L;
		
		// se deve usare il timeout interno, imposta i valori relativi
		if(m_nConnectionTimeout > 0 && !m_bUseWinsockTimeout)
		{
			if(m_TimeoutCritSect.Lock(SYNC_3_SECS_TIMEOUT))
			{
				strcpyn(m_szCurrentLocation,pHttpConnection->GetObject()->filename,sizeof(m_szCurrentLocation));
				m_qwCurrentAmount = 0L;
				m_TimeoutCritSect.Unlock();
			}
		}

		// interfaccia utente
		CFilenameFactory fn;
		CFilenameFactory ul;
		SetWindowText(::StripPathFromFile(pHttpConnection->GetObject()->filename));
		url.SplitUrl(pHttpConnection->GetObject()->url,&Url);
		strcpyn(Url.file,"",sizeof(Url.file));
		strcpyn(Url.cgi,"",sizeof(Url.cgi));
		strcpyn(Url.delim,"",sizeof(Url.delim));
		strcpyn(Url.query,"",sizeof(Url.query));
		url.ComposeUrl(&Url,szUrl,sizeof(szUrl)-1,FALSE);
		_snprintf(szBuffer,
				sizeof(szBuffer)-1,
				"Retrieving: %s\r\nFrom: %s",
				fn.Abbreviate(::StripPathFromFile(pHttpConnection->GetObject()->filename),40),
				ul.Abbreviate(szUrl,54,FALSE,FALSE)
				);

		SetDlgItemText(IDC_STATIC_STATUS,szBuffer);
		m_wndProgressBar.SetRange(0,100);
		m_wndProgressBar.SetStep(1);
		m_wndProgressBar.SetPos(0);
		m_wndProgressBar.Show();
		
		SetDlgItemText(IDC_STATIC_ESTIMATED_TIME_LEFT,"Estimated time left:");
		SetDlgItemText(IDC_STATIC_CURRENT_STATUS,"Downloaded:");
		SetDlgItemText(IDC_STATIC_TRANSFER_RATE,"Transfer rate:");
		SetDlgItemText(IDC_ESTIMATED_TIME_LEFT,"-");
		SetDlgItemText(IDC_CURRENT_STATUS,"-");
		SetDlgItemText(IDC_TRANSFER_RATE,"-");

		// bytes left
		if(pHttpConnection->GetObject()->size <= (QWORD)url.GetDownloadBufferSize())
		{
			if(pHttpConnection->GetObject()->size > 0L)
			{
				char szAmount[64] = {0};
				strsize(szAmount,sizeof(szAmount),(double)pHttpConnection->GetObject()->size);
				_snprintf(szBuffer,sizeof(szBuffer)-1,"%s left",szAmount);
				SetDlgItemText(IDC_CURRENT_STATUS,szBuffer);
			}

			// estimated time left
			SetDlgItemText(IDC_ESTIMATED_TIME_LEFT,"-");
		}

		// per l'aggiunta al volo x files audio > 25MB
		CAudioPlayer audioPlayer;
		if(audioPlayer.IsSupportedFormat(pHttpConnection->GetObject()->filename))
		{
			QWORD qwSize = pHttpConnection->GetObject()->size;
			if(qwSize <= 0L)
				qwSize = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_STREAMRIPPER_USTREAMSLIMIT_KEY)+1;
			if(	m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_STREAMRIPPER_USTREAMSLIMIT_KEY)!=0 &&
				qwSize > m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_STREAMRIPPER_USTREAMSLIMIT_KEY)
				)
				pHttpConnection->GetObject()->wParam = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_STREAMRIPPER_USTREAMSLIMIT_KEY);
		}
		
		// imposta il codice di ritorno	
		nRet = (m_nCancel==0 ? IDOK : IDCANCEL);
	}
	// download
	else if(pHttpConnection->GetObject()->stat==HTTPOBJECTSTAT_RECEIVING)
	{
		char szSize[64] = {0};
		char szAmount[64] = {0};
		double dlProgress = 0.0f;
		double dlTransferRate = 0.0f;
		
		// se deve usare il timeout interno, imposta i valori relativi
		if(m_nConnectionTimeout > 0 && !m_bUseWinsockTimeout)
		{
			if(m_TimeoutCritSect.Lock(SYNC_3_SECS_TIMEOUT))
			{
				m_qwCurrentAmount = pHttpConnection->GetObject()->amount;
				m_TimeoutCritSect.Unlock();
			}
		}

		// tempi ('endtime' non viene impostato a fine ricezione ma per ogni 'amount' ricevuto)
		DWORD nElapsedTime = (pHttpConnection->GetObject()->endtime - pHttpConnection->GetObject()->starttime) / 1000L;

		// interfaccia utente
		if(pHttpConnection->GetObject()->size > (QWORD)url.GetDownloadBufferSize())
		{
			// progress (bar)
			dlProgress = FDIV((100.0f * pHttpConnection->GetObject()->amount),pHttpConnection->GetObject()->size);
			m_wndProgressBar.SetPos((int)dlProgress);

			// estimated time left
			CDateTime datetime;
			char szTime[128] = {0};
			double dlElapsed = FDIV((pHttpConnection->GetObject()->size * (double)nElapsedTime),pHttpConnection->GetObject()->amount) - (double)nElapsedTime+1;
			_snprintf(szTime,
					sizeof(szTime)-1,
					"%s",
					datetime.GetElapsedTime((double)dlElapsed)
					);
			SetDlgItemText(IDC_ESTIMATED_TIME_LEFT,szTime);

			// downloaded
			strsize(szAmount,sizeof(szAmount),(double)pHttpConnection->GetObject()->amount);
			strsize(szSize,sizeof(szSize),(double)pHttpConnection->GetObject()->size);
			_snprintf(szBuffer,sizeof(szBuffer)-1,"%s of %s (%d%%)",szAmount,szSize,(int)dlProgress);
			SetDlgItemText(IDC_CURRENT_STATUS,szBuffer);

			// transfer rate
			dlTransferRate = 0.0f;
			if(pHttpConnection->GetObject()->amount > 1024.0f)
				dlTransferRate = FDIV(pHttpConnection->GetObject()->amount,(double)nElapsedTime) / 1024.0f;
			if(dlTransferRate < 1024.0f)
			{
				if(dlTransferRate > 0.0f && dlTransferRate < 1.0f)
					_snprintf(szBuffer,sizeof(szBuffer)-1,"%0.2f KB/s",dlTransferRate);
				else
					_snprintf(szBuffer,sizeof(szBuffer)-1,"%ld KB/s",(int)dlTransferRate);
			}
			else
				_snprintf(szBuffer,sizeof(szBuffer)-1,"%0.2f MB/s",FDIV(dlTransferRate,1024.0f));
			SetDlgItemText(IDC_TRANSFER_RATE,dlTransferRate > 0.0f ? szBuffer : "");
		}
		else
		{
			// progress (bar/title)
			if(pHttpConnection->GetObject()->size > 0L)
			{
				dlProgress = FDIV((100.0f * pHttpConnection->GetObject()->amount),pHttpConnection->GetObject()->size);
			}
			else
			{
				pHttpConnection->GetObject()->lParam += 1L;
				if(pHttpConnection->GetObject()->lParam > 1000L)
					pHttpConnection->GetObject()->lParam = 0L;
				dlProgress = (double)(pHttpConnection->GetObject()->lParam/10L);
			}
			m_wndProgressBar.SetPos((int)dlProgress);

			// estimated time left

			// downloaded
			if(pHttpConnection->GetObject()->size > 0L)
			{
				strsize(szAmount,sizeof(szAmount),(double)(pHttpConnection->GetObject()->size - pHttpConnection->GetObject()->amount));
				_snprintf(szBuffer,sizeof(szBuffer)-1,"%s left",szAmount);
			}
			else
			{
				strsize(szBuffer,sizeof(szBuffer),(double)pHttpConnection->GetObject()->amount);
			}
			SetDlgItemText(IDC_CURRENT_STATUS,szBuffer);

			// transfer rate
			dlTransferRate = 0.0f;
			if(pHttpConnection->GetObject()->amount > 1024.0f)
				dlTransferRate = FDIV(pHttpConnection->GetObject()->amount,(double)nElapsedTime) / 1024.0f;
			if(dlTransferRate < 1024.0f)
			{
				if(dlTransferRate > 0.0f && dlTransferRate < 1.0f)
					_snprintf(szBuffer,sizeof(szBuffer)-1,"%0.2f KB/s",dlTransferRate);
				else
					_snprintf(szBuffer,sizeof(szBuffer)-1,"%ld KB/s",(int)dlTransferRate);
			}
			else
				_snprintf(szBuffer,sizeof(szBuffer)-1,"%0.2f MB/s",FDIV(dlTransferRate,1024.0f));
			SetDlgItemText(IDC_TRANSFER_RATE,dlTransferRate > 0.0f ? szBuffer : "");
		}

		if(pHttpConnection->GetObject()->wParam > 0)
		{
			if(pHttpConnection->GetObject()->amount > pHttpConnection->GetObject()->wParam)
			{
				if(m_hWndParent)
					::SendMessage(m_hWndParent,WM_PLAYLIST_ADDFILE,AUDIOPLAYER_COMMAND_QUEUE_FROM_FILE,(LONG)pHttpConnection->GetObject()->filename);
				pHttpConnection->GetObject()->wParam = 0;
			}
		}
		
		// imposta il codice di ritorno	
		nRet = (m_nCancel==0 ? IDOK : IDCANCEL);
	}
	// fine download
	else if(pHttpConnection->GetObject()->stat==HTTPOBJECTSTAT_END_RECEIVING)
	{
		// se deve usare il timeout interno, imposta i valori relativi
		if(m_nConnectionTimeout > 0 && !m_bUseWinsockTimeout)
		{
			if(m_TimeoutCritSect.Lock(SYNC_3_SECS_TIMEOUT))
			{
				memset(m_szCurrentLocation,'\0',sizeof(m_szCurrentLocation));
				m_qwCurrentAmount = (QWORD)-1L;
				m_TimeoutCritSect.Unlock();
			}
		}

		// tempi (se amount <= 0 e' stata inviata una HEAD e non una GET)
		if(pHttpConnection->GetObject()->amount > 0L)
			m_dlDownloadTime += (((double)(pHttpConnection->GetObject()->endtime - pHttpConnection->GetObject()->starttime)) / 1000.0f);
		
		// interfaccia utente
		SetWindowText(m_szTitle);
		//SetDlgItemText(IDC_STATIC_STATUS,"");
		//SetDlgItemText(IDC_STATIC_ESTIMATED_TIME_LEFT,"");
		//SetDlgItemText(IDC_STATIC_CURRENT_STATUS,"");
		//SetDlgItemText(IDC_STATIC_TRANSFER_RATE,"");
		SetDlgItemText(IDC_ESTIMATED_TIME_LEFT,"-");
		SetDlgItemText(IDC_CURRENT_STATUS,"-");
		SetDlgItemText(IDC_TRANSFER_RATE,"-");
		m_wndProgressBar.SetPos(0);
		//m_wndProgressBar.Hide();

		// imposta il codice di ritorno	
		nRet = (m_nCancel==0 ? IDOK : IDCANCEL);
	}
	// ricezione cookie
	else if(pHttpConnection->GetObject()->stat==HTTPOBJECTSTAT_SET_COOKIE && m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_COOKIES_ALLOWED_KEY))
	{
		if(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_COOKIES_WARNINGONACCEPT_KEY))
		{
			COOKIE* cookie = &(pHttpConnection->GetObject()->cookie);

			::FormatResourceStringEx(szBuffer,
								sizeof(szBuffer),
								IDS_QUESTION_SETCOOKIE,
								pHttpConnection->GetHostName(),
								cookie->name,
								cookie->value,
								cookie->path,
								cookie->expires,
								cookie->secure
								);

			if(m_bUseScript && m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_QUIETINSCRIPTMODE_KEY))
				nRet = IDOK;
			else
				nRet = ::MessageBox(this->m_hWnd,szBuffer,WALLPAPER_PROGRAM_NAME,MB_YESNOCANCEL|MB_ICONQUESTION|MB_TASKMODAL|MB_SETFOREGROUND|MB_TOPMOST);
		}
		else
			nRet = IDOK;
	}
	// invio cookie
	else if(pHttpConnection->GetObject()->stat==HTTPOBJECTSTAT_GET_COOKIE && m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_COOKIES_ALLOWED_KEY))
	{
		if(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_COOKIES_WARNINGONSEND_KEY))
		{
			COOKIE* cookie = &(pHttpConnection->GetObject()->cookie);

			::FormatResourceStringEx(szBuffer,
								sizeof(szBuffer),
								IDS_QUESTION_GETCOOKIE,
								pHttpConnection->GetHostName(),
								cookie->value
								);

			if(m_bUseScript && m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_QUIETINSCRIPTMODE_KEY))
				nRet = IDOK;
			else
				nRet = ::MessageBox(this->m_hWnd,szBuffer,WALLPAPER_PROGRAM_NAME,MB_YESNOCANCEL|MB_ICONQUESTION|MB_TASKMODAL|MB_SETFOREGROUND|MB_TOPMOST);
		}
		else
			nRet = IDOK;
	}

	return(nRet);
}

/*
	ExtractLinksCallback()

	Wrapper per la callback per l'estrazione delle referenze (links) dal file.
*/
UINT CWallPaperCrawlerDlg::ExtractLinksCallback(LPVOID lpFileParse,LPVOID lpVoid)
{
	UINT nRet = (UINT)-1L;

	CWallPaperCrawlerDlg* pThis = (CWallPaperCrawlerDlg*)lpVoid;
	if(pThis)
		nRet = pThis->ExtractLinksCallback(lpFileParse);

	return(nRet);
}

/*
	ExtractLinksCallback()

	Callback per l'estrazione delle referenze (links) dal file.
*/
UINT CWallPaperCrawlerDlg::ExtractLinksCallback(LPVOID lpVoid)
{
	CUrl url;
	char szBuffer[1024];
	HTMLOBJECT* lpFileParse = (HTMLOBJECT*)lpVoid;

	// inizio download
	if(lpFileParse->stat==HTMLOBJECTSTAT_BEGINPARSING)
	{
		// interfaccia utente
		SetWindowText(::StripPathFromFile(lpFileParse->filename));
		_snprintf(szBuffer,sizeof(szBuffer)-1,"Parsing: %s\r\n(%s)",::StripPathFromFile(lpFileParse->filename),lpFileParse->filename);
		SetDlgItemText(IDC_STATIC_STATUS,szBuffer);
		m_wndProgressBar.SetRange(0,100);
		m_wndProgressBar.SetStep(1);
		m_wndProgressBar.SetPos(0);
		m_wndProgressBar.Show();
	}
	// download
	else if(lpFileParse->stat==HTMLOBJECT_PARSING)
	{
		// interfaccia utente
		QWORD qwProgress = DIV((100L * lpFileParse->amount),lpFileParse->size);
		_snprintf(szBuffer,sizeof(szBuffer)-1,"%s (%d%%)",::StripPathFromFile(lpFileParse->filename),(int)qwProgress);
		SetWindowText(szBuffer);
		m_wndProgressBar.SetPos((int)qwProgress);
	}
	// fine download
	else if(lpFileParse->stat==HTMLOBJECT_ENDPARSING)
	{
		// interfaccia utente
		SetWindowText(m_szTitle);
		SetDlgItemText(IDC_STATIC_STATUS,"");
		m_wndProgressBar.SetPos(0);
		//m_wndProgressBar.Hide();
	}

	return(m_nCancel==0 ? m_nCancel : 1);
}

/*
	TimeoutManager()

	Wrapper per il thread per il controllo del timeout durante il download.
*/
UINT CWallPaperCrawlerDlg::TimeoutManager(LPVOID lpVoid)
{
	UINT nRet = (UINT)-1L;

	CWallPaperCrawlerDlg* pThis = (CWallPaperCrawlerDlg*)lpVoid;
	if(pThis)
		nRet = pThis->TimeoutManager();
	
	return(nRet);
}

/*
	TimeoutManager()

	Thread per il controllo del timeout durante il download.
*/
UINT CWallPaperCrawlerDlg::TimeoutManager(void)
{
	char szCurrentLocation[_MAX_PATH+1];
	QWORD qwCurrentAmount = 0L;

	if(m_bInCrawling)
		if(m_TimeoutCritSect.Lock(SYNC_3_SECS_TIMEOUT))
		{
			strcpyn(szCurrentLocation,m_szCurrentLocation,sizeof(szCurrentLocation));
			qwCurrentAmount = m_qwCurrentAmount;
			m_TimeoutCritSect.Unlock();

			while(m_bInCrawling)
			{
				if(m_hTimeoutEvent==(HANDLE)NULL)
					break;

				if(::WaitForSingleObject(m_hTimeoutEvent,1000 * 5)!=WAIT_TIMEOUT)
					break;

				if(m_TimeoutCritSect.Lock(SYNC_3_SECS_TIMEOUT))
				{
					if(m_szCurrentLocation[0]!='\0' && strcmp(szCurrentLocation,m_szCurrentLocation)==0)
					{
						if(m_qwCurrentAmount!=(DWORD)-1L && qwCurrentAmount==m_qwCurrentAmount)
							m_httpConnection.Abort();
						else
							qwCurrentAmount = m_qwCurrentAmount;
					}
					else
					{
						strcpyn(szCurrentLocation,m_szCurrentLocation,sizeof(szCurrentLocation));
						qwCurrentAmount = m_qwCurrentAmount;
					}
					
					m_TimeoutCritSect.Unlock();
				}
			}
		}

	return(0L);
}

/*
	Crawler()

	Wrapper per il thread per il download.
*/
UINT CWallPaperCrawlerDlg::Crawler(LPVOID lpVoid)
{
	UINT nRet = (UINT)-1L;

	CWallPaperCrawlerDlg* pThis = (CWallPaperCrawlerDlg*)lpVoid;
	if(pThis)
	{
		HWND hWnd = pThis->GetSafeHwnd();
		nRet = pThis->Crawler();
		if(hWnd && ::IsWindow(hWnd))
			::PostMessage(hWnd,WM_CRAWLER_DONE,0,0L);
	}

	return(nRet);
}

/*
	Crawler()

	Thread per il download.
*/
UINT CWallPaperCrawlerDlg::Crawler(void)
{
	int i;
	char* p;
	char szBuffer[1024];
	
	CUrl url;
	URL Url;
	URL ThisUrl;
	URL DomainUrl;
	char szRequestedUrl[MAX_URL+1];
	char szUrl[MAX_URL+1];
	char szParentUrl[MAX_URL+1];
	char szTranslatedUrl[MAX_URL+1];
	char szLocalFile[_MAX_PATH+1];
	char szDownloadPath[_MAX_PATH+1];
	int nDownloadCode = 0;
	BOOL bDownloaded = FALSE;
	BOOL bConnected = FALSE;
	CHRefList* pHRefList;
	CONTENTTYPE ctype;
	
	DWORD dwExtractLinksFlags = 0L;
	if(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_ENABLEJAVASCRIPT_KEY))
		dwExtractLinksFlags |= CURL_FLAG_ENABLE_JAVASCRIPT;
	if(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_EXTRACTTEXT_KEY))
		dwExtractLinksFlags |= CURL_FLAG_EXTRACT_TEXT_FROM_HTML;
	if(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_EXTRACTJAVASCRIPT_KEY))
		dwExtractLinksFlags |= CURL_FLAG_EXTRACT_JAVASCRIPT_FROM_HTML;

	BOOL bOnlyPageContent = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_ONLYCONTENT_KEY);
	
	CDateTime datetime(GMT);
	
	UINT nID = 0;
	UINT nNextID = 0;	
	UINT nPict = (UINT)-1;
	
	BOOL bAsk = FALSE;
	BOOL bAskForDomainAcceptance = TRUE;
	BOOL bAskForParentAcceptance = TRUE;
	DOMAINACCEPTANCE domainacceptance = UNDEFINED_DOMAIN_ACCEPTANCE;
	PARENTURLACCEPTANCE parenturlacceptance = UNDEFINED_PARENT_ACCEPTANCE;
	BOOL bFileDirMismatched = FALSE;
	
	CFileTypeList FileTypeList;
	CItemList listDomainsInclude;
	CItemList listDomainsExclude;
	CItemList listParentsInclude;
	CItemList listParentsExclude;
	CItemList listWildcards;
	CItemList listDomainsIncludeSave;
	CItemList listDomainsExcludeSave;
	CItemList listParentsIncludeSave;
	CItemList listParentsExcludeSave;
	BOOL bUseRobotsTxt = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_ENABLEROBOTSTXT_KEY);
	CItemList listRobotsTxtExclude;
	FILETYPE* pFiletype;
	ITEM* item;
	ITEM* pWildcard;
	CWildCards Wildcards;

	int nTotFileTypeKeys = 0;
	int nTotWildcardKeys = 0;
	int nTotDomainIncludeKeys = 0;
	int nTotDomainExcludeKeys = 0;
	int nTotParentIncludeKeys = 0;
	int nTotParentExcludeKeys = 0;
	
	char szKey[REGKEY_MAX_KEY_NAME+1];
	CWindowsVersion winver;
	
	// inizio crawling
	m_bInCrawling = TRUE;
	m_oleDropTarget.Disable();
	CDialogEx::AllowDragAndDrop(FALSE);
	
	// interfaccia utente
	SetIcon(IDI_ICON_CRAWLER);
	strcpyn(m_szTitle,m_szUrl,sizeof(m_szTitle));
	SetWindowText(m_szTitle);
	SetDlgItemText(IDC_STATIC_STATUS,"Initializing...");
	GetDlgItem(IDC_COMBO_URL)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SCRIPTFILE)->EnableWindow(FALSE);
	SetDlgItemText(IDC_STATIC_ESTIMATED_TIME_LEFT,"");
	SetDlgItemText(IDC_STATIC_CURRENT_STATUS,"");
	SetDlgItemText(IDC_STATIC_TRANSFER_RATE,"");
	SetDlgItemText(IDC_ESTIMATED_TIME_LEFT,"");
	SetDlgItemText(IDC_CURRENT_STATUS,"");
	SetDlgItemText(IDC_TRANSFER_RATE,"");
	m_wndProgressBar.SetPos(0);
	m_wndProgressBar.Hide();
	if(m_PictPreviewSync.Lock())
	{
		m_listPictPreview.RemoveAll();
		m_wndStaticDib.Unload();
		SetDlgItemText(IDC_TEXT_INFO,"");
		m_PictPreviewSync.Unlock();
	}
	GetDlgItem(IDC_CHECK_CRAWLER_PREVIEW)->EnableWindow(FALSE);
	UpdateStatusBar();

	// data/ora inizio download
	strcpyn(m_szStartTime,datetime.GetFormattedDate(TRUE),sizeof(m_szStartTime));
	m_dlTotalTime = ::GetTickCount();
	m_dlDownloadTime = 0.0f;
	
	// se deve usare il timeout interno, crea il thread relativo
	if(m_nConnectionTimeout > 0 && !m_bUseWinsockTimeout)
	{
		if((m_hTimeoutEvent = ::CreateEvent(NULL,TRUE,FALSE,NULL))!=(HANDLE)NULL)
			::AfxBeginThread(TimeoutManager,this,THREAD_PRIORITY_HIGHEST);
	}
	
	// per la modalita' multithread
	m_listActiveCrawlerThreads.RemoveAll();
	m_nActiveThreads = 0;
	m_nThreadsPeak = 0;

	// l'url richiesta puo' essere modificata nel corso dell'elaborazione, salva quindi l'originale
	strcpyn(szRequestedUrl,m_szUrl,sizeof(szRequestedUrl));

	// notare che effettuando la decodifica, un url come http://localhost/pictures/mil/marine corp/Sunset%20Landing.jpg
	// (dove il file sul server *contiene* i caratteri '%20') viene trasformata generando un url inesistente
	url.DecodeUrl(m_szUrl);

	// divide l'url in nome host, numero porta, directory e nome file
	// se non viene specificata la porta, assume quella di default (80)
	url.SplitUrl(m_szUrl,&Url);
	if(Url.port <= 0)
		Url.port = HTTP_DEFAULT_PORT;
	
	// compone (non crea) il pathname locale relativo all'url
	url.CreatePathNameFromHttpUrl(m_szUrl,szDownloadPath,sizeof(szDownloadPath),m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_DOWNLOADDIR_KEY),FALSE);
	strcpyn(m_szDownloadDir,szDownloadPath,sizeof(m_szDownloadDir));
	strstrim(m_szDownloadDir);

	// costruisce il nome per il report html
	char szHtmlReport[_MAX_PATH+1];
	_snprintf(szHtmlReport,sizeof(szHtmlReport)-1,"%sreport.",m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_REPORTSDIR_KEY));
	strcatn(szHtmlReport,url.IsUrl(Url.host) ? url.StripUrlType(Url.host) : Url.host,sizeof(szHtmlReport));
	i = strlen(szHtmlReport);
	_snprintf(szHtmlReport+i,(sizeof(szHtmlReport)-1)-i,"_%ld",Url.port);
	if(Url.dir[0])
		strcatn(szHtmlReport,Url.dir,sizeof(szHtmlReport));
	if(Url.file[0])
		strcatn(szHtmlReport,Url.file,sizeof(szHtmlReport));
	url.EnsureValidFileName(szHtmlReport,sizeof(szHtmlReport),FALSE,DEFAULT_HTML_EXT);

	// costruisce il nome per il report ascii
	char szAsciiReport[_MAX_PATH+1] = {0};
	FILE* fpAsciiReport = NULL;
	if(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_GENERATEASCII_KEY))
	{
		_snprintf(szAsciiReport,sizeof(szAsciiReport)-1,"%sreport.",m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_REPORTSDIR_KEY));
		strcatn(szAsciiReport,url.IsUrl(Url.host) ? url.StripUrlType(Url.host) : Url.host,sizeof(szAsciiReport));
		i = strlen(szAsciiReport);
		_snprintf(szAsciiReport+i,(sizeof(szAsciiReport)-1)-i,"_%ld",Url.port);
		if(Url.dir[0])
			strcatn(szAsciiReport,Url.dir,sizeof(szAsciiReport));
		if(Url.file[0])
			strcatn(szAsciiReport,Url.file,sizeof(szAsciiReport));
		url.EnsureValidFileName(szAsciiReport,sizeof(szAsciiReport),FALSE,".txt");
		fpAsciiReport = fopen(szAsciiReport,"wb");
		if(fpAsciiReport) {
			_snprintf(szBuffer,sizeof(szBuffer)-1,WALLPAPER_AUTHOR_COPYRIGHT,m_pConfig->GetString(WALLPAPER_INSTALL_KEY,WALLPAPER_AUTHOREMAIL_KEY));
			fprintf(	fpAsciiReport,
					"; %s\r\n; %s (%s)\r\n;\r\n; %s\r\n;\r\n;\r\n",
					WALLPAPER_PROGRAM_TITLE,
					m_pConfig->GetString(WALLPAPER_INSTALL_KEY,WALLPAPER_WEBSITE_KEY),
					szBuffer,
					m_szUrl);
		}
	}

	// interfaccia utente
	SetDlgItemText(IDC_STATIC_STATUS,"Synchronizing the local database...");
	
	// costruisce i nomi per la base dati
	char szDataTableName[_MAX_PATH+1];
	strcpy(szDataTableName,URL_TABLE".");
	strcatn(szDataTableName,url.IsUrl(Url.host) ? url.StripUrlType(Url.host) : Url.host,sizeof(szDataTableName));
	i = strlen(szDataTableName);
	_snprintf(szDataTableName+i,(sizeof(szDataTableName)-1)-i,"_%ld",Url.port);
	if(Url.dir[0])
		strcatn(szDataTableName,Url.dir,sizeof(szDataTableName));
	if(Url.file[0])
		strcatn(szDataTableName,Url.file,sizeof(szDataTableName));
	url.EnsureValidFileName(szDataTableName,sizeof(szDataTableName),FALSE);

	// apre i database
	CUrlService UrlService(szDataTableName,m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_DATADIR_KEY));
	CUrlDatabaseService	UrlDatabaseService(URLDATABASE_TABLE,m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_DATADIR_KEY));
	if(!UrlService.IsValid() || !UrlDatabaseService.IsValid())
	{
		::MessageBoxResourceEx(this->m_hWnd,MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_TABLE_LOCK,szDataTableName);
		goto terminate;
	}

	// controlla se ha gia' effettuato il download dell'url
	// (solo se in modalita' download, se in esplorazione non deve controllare)
	char szGMT[MAX_DATE_STRING+1];
	CUrlStatus::URL_STATUS nStatus;
	nStatus = CUrlStatus::URL_STATUS_UNKNOWN;
	BOOL bExist;
	if(m_nCrawlerMode==CRAWLER_DOWNLOAD_MODE || m_nCrawlerMode==CRAWLER_STREAMRIPPER_MODE)
		bExist = UrlDatabaseService.Exist(m_szUrl,szGMT,sizeof(szGMT),nStatus,nNextID);
	else
		bExist = FALSE;
	if(bExist)
	{
		switch(nStatus)
		{
			// download gia' effettuato
			case CUrlStatus::URL_STATUS_DONE:
			case CUrlStatus::URL_STATUS_DOWNLOADED:
			case CUrlStatus::URL_STATUS_NOT_MODIFIED:
			case CUrlStatus::URL_STATUS_MOVED:
			{
				if(m_bUseScript && m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_QUIETINSCRIPTMODE_KEY))
				{
					nNextID = 0;
					UrlService.DeleteAll();
				}
				else
				{
					if(::MessageBoxResourceEx(this->m_hWnd,MB_YESNO|MB_ICONQUESTION,WALLPAPER_PROGRAM_NAME,IDS_QUESTION_CRAWL_AGAIN,m_szUrl,szGMT)==IDNO)
					{
						m_nCancel = 1;
						goto terminate;
					}
					else
					{
						nNextID = 0;
						UrlService.DeleteAll();
					}
				}
				break;
			}
			
			// download interrotto
			case CUrlStatus::URL_STATUS_CANCELLED:
			case CUrlStatus::URL_STATUS_WINSOCK_ERROR:
			case CUrlStatus::URL_STATUS_HTTP_ERROR:
			{
				int nRet;
				if(m_bUseScript && m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_QUIETINSCRIPTMODE_KEY))
					nRet = IDNO;
				else
				{
					char* pReason;
					if(nStatus==CUrlStatus::URL_STATUS_CANCELLED)
						pReason = "the download process has been cancelled";
					else if(nStatus==CUrlStatus::URL_STATUS_WINSOCK_ERROR)
						pReason = "a winsock error";
					else if(nStatus==CUrlStatus::URL_STATUS_HTTP_ERROR)
						pReason = "an http error";
					else
						pReason = "an unknown reason";
					nRet = ::MessageBoxResourceEx(this->m_hWnd,MB_YESNOCANCEL|MB_ICONQUESTION,WALLPAPER_PROGRAM_NAME,IDS_QUESTION_CRAWL_PENDING,m_szUrl,szGMT,pReason);
				}
				if(nRet==IDYES)
				{
					;
				}
				else if(nRet==IDNO)
				{
					nNextID = 0;
					UrlService.DeleteAll();
				}
				else if(nRet==IDCANCEL)
				{
					m_nCancel = 1;
					UrlDatabaseService.Update(	m_szUrl,
											"",
											0.0f,
											0.0f,
											CUrlStatus::URL_STATUS_CANCELLED,
											nNextID,
											"",
											NULL);
					goto terminate;
				}
				break;
			}

			// se il sito e' stato esplorato e non scaricato elimina tutto e ricomincia da zero
			case CUrlStatus::URL_STATUS_CHECKED:
			default:
				nNextID = 0;
				UrlService.DeleteAll();
				break;
		}
	}
	else
	{
		nNextID = 0;
		UrlService.DeleteAll();
	}

	// inserisce l'url nel database
	UrlDatabaseService.Insert(m_szUrl,m_szStartTime,szDataTableName);

	// interfaccia utente
	SetDlgItemText(IDC_STATIC_STATUS,"Loading filters...");

	// carica dalla configurazione i tipi di file registrati
	for(i = 0;; i++)
	{
		_snprintf(szKey,sizeof(szKey)-1,"%s%d",WALLPAPER_FILETYPE_KEY,i);
		if(strcmp((p = (char*)m_pConfig->GetString(WALLPAPER_FILETYPELIST_KEY,szKey)),"")!=0)
		{
			pFiletype = (FILETYPE*)FileTypeList.Add();
			if(pFiletype)
			{
				strcpyn(pFiletype->ext,p,MAX_FILETYPE_EXT+1);
				pFiletype->len = strlen(pFiletype->ext);
				nTotFileTypeKeys++;
			}
		}
		else
			break;
	}
	// nessun tipo file presente, carica con i tipi di default (immagini)
	if(nTotFileTypeKeys <= 0)
	{
		LPIMAGETYPE p;
		while((p = m_pImage->EnumReadableImageFormats())!=(LPIMAGETYPE)NULL)
		{
			// carica nella lista
			pFiletype = (FILETYPE*)FileTypeList.Add();
			if(pFiletype)
			{
				strcpyn(pFiletype->ext,p->ext,MAX_FILETYPE_EXT+1);
				pFiletype->len = strlen(pFiletype->ext);
			}

			// carica nella configurazione
			_snprintf(szKey,sizeof(szKey)-1,"%s%d",WALLPAPER_FILETYPE_KEY,nTotFileTypeKeys);
			m_pConfig->Insert(WALLPAPER_FILETYPELIST_KEY,szKey,p->ext);

			nTotFileTypeKeys++;
		}
		
		// salva i tipi file nel registro
		if(nTotFileTypeKeys > 0)
			m_pConfig->SaveSection(WALLPAPER_FILETYPELIST_KEY);
	}

	// aggiunge i tipi di file registrati alla lista che verra' utilizzata per estrarre le url
	ITERATOR iter;
	if((iter = FileTypeList.First())!=(ITERATOR)NULL)
		do
		{
			pFiletype = (FILETYPE*)iter->data;
			if(pFiletype)
				url.AddUrlFile(pFiletype->ext);
			iter = FileTypeList.Next(iter);
		} while(iter!=(ITERATOR)NULL);

	// carica dalla configurazione la lista delle wildcards
	for(i = 0;; i++)
	{
		_snprintf(szKey,sizeof(szKey)-1,"%s%d",WALLPAPER_WILDCARDSEXCLUDE_KEY,i);
		if(strcmp((p = (char*)m_pConfig->GetString(WALLPAPER_WILDCARDSEXCLUDELIST_KEY,szKey)),"")!=0)
		{
			pWildcard = (ITEM*)listWildcards.Add();
			if(pWildcard)
			{
				strcpyn(pWildcard->item,p,_MAX_ITEM+1);
				nTotWildcardKeys++;
			}
		}
		else
			break;
	}

	// carica dalla configurazione la lista delle inclusioni per i domini
	for(i = 0;; i++)
	{
		_snprintf(szKey,sizeof(szKey)-1,"%s%d",WALLPAPER_DOMAININCLUDE_KEY,i);
		if(strcmp((p = (char*)m_pConfig->GetString(WALLPAPER_DOMAININCLUDELIST_KEY,szKey)),"")!=0)
		{
			// lista per inclusioni
			item = (ITEM*)listDomainsInclude.Add();
			if(item)
				strcpyn(item->item,p,_MAX_ITEM+1);

			// lista per salvataggio inclusioni
			item = (ITEM*)listDomainsIncludeSave.Add();
			if(item)
				strcpyn(item->item,p,_MAX_ITEM+1);

			nTotDomainIncludeKeys++;
		}
		else
			break;
	}
	
	// carica dalla configurazione la lista delle esclusioni per i domini
	for(i = 0;; i++)
	{
		_snprintf(szKey,sizeof(szKey)-1,"%s%d",WALLPAPER_DOMAINEXCLUDE_KEY,i);
		if(strcmp((p = (char*)m_pConfig->GetString(WALLPAPER_DOMAINEXCLUDELIST_KEY,szKey)),"")!=0)
		{
			// lista per esclusioni
			item = (ITEM*)listDomainsExclude.Add();
			if(item)
				strcpyn(item->item,p,_MAX_ITEM+1);

			// lista per salvataggio esclusioni
			item = (ITEM*)listDomainsExcludeSave.Add();
			if(item)
				strcpyn(item->item,p,_MAX_ITEM+1);

			nTotDomainExcludeKeys++;
		}
		else
			break;
	}
	
	// carica dalla configurazione la lista delle inclusioni per le directories padre
	for(i = 0;; i++)
	{
		_snprintf(szKey,sizeof(szKey)-1,"%s%d",WALLPAPER_PARENTINCLUDE_KEY,i);
		if(strcmp((p = (char*)m_pConfig->GetString(WALLPAPER_PARENTINCLUDELIST_KEY,szKey)),"")!=0)
		{
			// lista per inclusioni
			item = (ITEM*)listParentsInclude.Add();
			if(item)
				strcpyn(item->item,p,_MAX_ITEM+1);

			// lista per salvataggio inclusioni
			item = (ITEM*)listParentsIncludeSave.Add();
			if(item)
				strcpyn(item->item,p,_MAX_ITEM+1);

			nTotParentIncludeKeys++;
		}
		else
			break;
	}
	
	// carica dalla configurazione la lista delle esclusioni per le directories padre
	for(i = 0;; i++)
	{
		_snprintf(szKey,sizeof(szKey)-1,"%s%d",WALLPAPER_PARENTEXCLUDE_KEY,i);
		if(strcmp((p = (char*)m_pConfig->GetString(WALLPAPER_PARENTEXCLUDELIST_KEY,szKey)),"")!=0)
		{
			// lista per esclusioni
			item = (ITEM*)listParentsExclude.Add();
			if(item)
				strcpyn(item->item,p,_MAX_ITEM+1);

			// lista per salvataggio esclusioni
			item = (ITEM*)listParentsExcludeSave.Add();
			if(item)
				strcpyn(item->item,p,_MAX_ITEM+1);

			nTotParentExcludeKeys++;
		}
		else
			break;
	}

	// imposta per l'accettazione dei domini
	switch(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_DOMAINACCEPTANCE_KEY))
	{
		case DOMAINACCEPTANCE_EXCLUDE_THIS:
			bAskForDomainAcceptance = FALSE;
			domainacceptance = EXCLUDE_THIS_DOMAIN;
			break;
		case DOMAINACCEPTANCE_EXCLUDE_ALL_OUTSIDE:
			bAskForDomainAcceptance = FALSE;
			domainacceptance = EXCLUDE_ALL_DOMAINS;
			break;
		case DOMAINACCEPTANCE_EXCLUDE_ALL_OUTSIDE_EXCEPT_PICTURES:
			bAskForDomainAcceptance = FALSE;
			domainacceptance = EXCLUDE_ALL_DOMAINS_EXCEPT_PICTURES;
			break;
		case DOMAINACCEPTANCE_EXCLUDE_ALL_OUTSIDE_EXCEPT_REGFILETYPES:
			bAskForDomainAcceptance = FALSE;
			domainacceptance = EXCLUDE_ALL_DOMAINS_EXCEPT_REGFILETYPES;
			break;
		case DOMAINACCEPTANCE_EXCLUDE_ALL_FROM_THIS:
			bAskForDomainAcceptance = FALSE;
			domainacceptance = EXCLUDE_ALL_URLS_FROM_THIS_DOMAIN;
			break;
		case DOMAINACCEPTANCE_FOLLOW_THIS:
			bAskForDomainAcceptance = FALSE;
			domainacceptance = FOLLOW_THIS_DOMAIN;
			break;
		case DOMAINACCEPTANCE_FOLLOW_ALL_OUTSIDE:
			bAskForDomainAcceptance = FALSE;
			domainacceptance = FOLLOW_ALL_DOMAINS;
			break;
		case DOMAINACCEPTANCE_FOLLOW_ALL_FROM_THIS:
			bAskForDomainAcceptance = FALSE;
			domainacceptance = FOLLOW_ALL_URLS_FROM_THIS_DOMAIN;
			break;
		case DOMAINACCEPTANCE_ASK:
		default:
			bAskForDomainAcceptance = TRUE;
			domainacceptance = UNDEFINED_DOMAIN_ACCEPTANCE;
			break;
	}
	
	// imposta per l'accettazione delle directories padre
	switch(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_PARENTACCEPTANCE_KEY))
	{
		case PARENTACCEPTANCE_EXCLUDE_THIS:
			bAskForParentAcceptance = FALSE;
			parenturlacceptance = EXCLUDE_THIS_PARENTURL;
			break;
		case PARENTACCEPTANCE_EXCLUDE_ALL_OUTSIDE:
			bAskForParentAcceptance = FALSE;
			parenturlacceptance = EXCLUDE_ALL_PARENTURLS;
			break;
		case PARENTACCEPTANCE_EXCLUDE_ALL_OUTSIDE_EXCEPT_PICTURES:
			bAskForParentAcceptance = FALSE;
			parenturlacceptance = EXCLUDE_ALL_PARENTURLS_EXCEPT_PICTURES;
			break;
		case PARENTACCEPTANCE_EXCLUDE_ALL_OUTSIDE_EXCEPT_REGFILETYPES:
			bAskForParentAcceptance = FALSE;
			parenturlacceptance = EXCLUDE_ALL_PARENTURLS_EXCEPT_REGFILETYPES;
			break;
		case PARENTACCEPTANCE_EXCLUDE_ALL_FROM_THIS:
			bAskForParentAcceptance = FALSE;
			parenturlacceptance = EXCLUDE_ALL_PARENTURLS_FROM_THIS_PATHNAME;
			break;
		case PARENTACCEPTANCE_FOLLOW_THIS:
			bAskForParentAcceptance = FALSE;
			parenturlacceptance = FOLLOW_THIS_PARENTURL;
			break;
		case PARENTACCEPTANCE_FOLLOW_ALL_OUTSIDE:
			bAskForParentAcceptance = FALSE;
			parenturlacceptance = FOLLOW_ALL_PARENTURLS;
			break;
		case PARENTACCEPTANCE_FOLLOW_ALL_FROM_THIS:
			bAskForParentAcceptance = FALSE;
			parenturlacceptance = FOLLOW_ALL_PARENTURLS_FROM_THIS_PATHNAME;
			break;
		case PARENTACCEPTANCE_ASK:
		default:
			bAskForParentAcceptance = TRUE;
			parenturlacceptance = UNDEFINED_PARENT_ACCEPTANCE;
			break;
	}

	// interfaccia utente
	SetDlgItemText(IDC_STATIC_STATUS,"Setting connection's defaults...");

	// imposta i defaults per la connessione
	m_httpConnection.Reset();
	m_httpConnection.SetDelay(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_WINSOCKDELAY_KEY));
	m_httpConnection.SetSkipExisting(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_SKIPEXISTING_KEY));
	m_httpConnection.SetForceSkipExisting(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_SKIPEXISTING_FORCE_KEY));
	m_httpConnection.SetDoNotSkipExisting(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_SKIPEXISTING_DONT_KEY));
	if(!m_httpConnection.GetSkipExisting())
	{
		m_httpConnection.AddHeader("Pragma","no-cache");
		// Cache-Control e' specifico per HTTP/1.1, non tutti i server lo gestiscono con una richiesta HTTP/1.0
		//m_httpConnection.AddHeader("Cache-Control","no-cache");
	}
	m_httpConnection.SetConnectionRetry(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_CONNECTIONRETRY_KEY));
	if(m_nConnectionTimeout > 0 && m_bUseWinsockTimeout)
		m_httpConnection.SetConnectionTimeout(m_nConnectionTimeout);
	m_httpConnection.AllowCookies(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_COOKIES_ALLOWED_KEY));
	m_httpConnection.SetCookieFileName(m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_COOKIES_FILE_KEY));
	if(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_USEPROXY_KEY))
	{
		m_httpConnection.SetProxyAddress(m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_PROXY_ADDRESS_KEY));
		m_httpConnection.SetProxyPortNumber(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PROXY_PORTNUMBER_KEY));
		if(strcmp(m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_PROXY_USER_KEY),"")!=0 && strcmp(m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_PROXY_PASSWORD_KEY),"")!=0)
		{
			m_httpConnection.SetProxyUser(m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_PROXY_USER_KEY));
			m_httpConnection.SetProxyPassword(m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_PROXY_PASSWORD_KEY));
			CBase64 base64;
			char szEncoded[MAX_USER_LEN+1+MAX_PASS_LEN+1];
			_snprintf(szEncoded,sizeof(szEncoded)-1,"%s:%s",m_httpConnection.GetProxyUser(),m_httpConnection.GetProxyPassword());
			char* pAuth = base64.Encode(szEncoded);
			m_httpConnection.SetProxyAuth(pAuth);
			delete [] pAuth;
		}
		if(strcmp(m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_PROXY_EXCLUDEDOMAINS_KEY),"")!=0)
		{
			CHostNameList* plistProxyExclude = m_httpConnection.GetProxyExcludeList();
			char szExclude[2048];
			strcpyn(szExclude,m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_PROXY_EXCLUDEDOMAINS_KEY),sizeof(szExclude));
			char* token = strtok(szExclude,",");
			HOSTNAME* n;
			for(int i = 0; token; i++)
			{
				if((n = (HOSTNAME*)plistProxyExclude->Add())!=(HOSTNAME*)NULL)
					strcpyn(n->name,token,HOSTNAME_SIZE+1);
				token = strtok((char*)NULL,",");
			}
		}
	}
	m_httpConnection.SetPortNumber(Url.port);
	char szUserAgent[HTTP_MAX_USERAGENT+1];
	_snprintf(szUserAgent,sizeof(szUserAgent)-1,WALLPAPER_USER_AGENT,winver.GetVersionString());
	m_httpConnection.AddHeader("User-Agent",szUserAgent);
	UINT nMinSize,nMaxSize;
	int nSizeType;
	nMinSize = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_MINSIZE_KEY);
	nMaxSize = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_MAXSIZE_KEY);
	nSizeType = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_MINSIZETYPE_KEY);
	if(nSizeType==SIZETYPE_KBYTES)
		nMinSize *= 1024L;
	else if(nSizeType==SIZETYPE_MBYTES)
		nMinSize *= 1048576L;
	nSizeType = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_MAXSIZETYPE_KEY);
	if(nSizeType==SIZETYPE_KBYTES)
		nMaxSize *= 1024L;
	else if(nSizeType==SIZETYPE_MBYTES)
		nMaxSize *= 1048576L;
	m_httpConnection.SetMinSize(nMinSize);
	m_httpConnection.SetMaxSize(nMaxSize);

	// interfaccia utente
	if(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_USEPROXY_KEY))
		_snprintf(szBuffer,sizeof(szBuffer)-1,"Connecting to (%s:%d): %s...",m_httpConnection.GetProxyAddress(),m_httpConnection.GetProxyPortNumber(),Url.host);
	else
		_snprintf(szBuffer,sizeof(szBuffer)-1,"Connecting to: %s:%d...",Url.host,Url.port);
	SetDlgItemText(IDC_STATIC_STATUS,szBuffer);

	// inizia il download (controlla se deve ripartire dall'ultima url o se si tratta di un nuovo download)
	if(nNextID!=0)
	{
		CUrlStatus::URL_STATUS nStat;
		if(!UrlService.Get(nNextID,szUrl,sizeof(szUrl),szParentUrl,sizeof(szParentUrl),nStat))
		{
			// nel caso in cui il download interrotto fosse in modalita' multithread
			// in tal caso il numero dell'ultima url scaricata coincide con l'id dell'ultimo thread lanciato
			// ad es. se la lista delle url da scaricare comprende un .html e 2 files e viene interrotto durante
			// il download dei due files l'ultimo id e' 4 (il .html viene scaricato sequenzialmente mentre i due
			// files da due threads separati), mentre in realta dovrebbe ripartire dall'id 2 (il primo file dopo
			// il .html) - in tal caso imposta l'ultimo id con quello della prima url interrotta
			nNextID = 0;
			UrlService.GetLastID(nNextID);
			if(nNextID!=0)
				if(!UrlService.Get(nNextID,szUrl,sizeof(szUrl),szParentUrl,sizeof(szParentUrl),nStat))
					nID = nNextID = 0;
		}
		nID = UrlService.Count();
	}
	else
		strcpyn(szUrl,m_szUrl,sizeof(szUrl));

	// interfaccia utente
	m_wndAnimatedAvi.ShowWindow(SW_SHOW);
	m_wndAnimatedAvi.Open((m_nCrawlerMode==CRAWLER_DOWNLOAD_MODE || m_nCrawlerMode==CRAWLER_STREAMRIPPER_MODE) ? IDR_DOWNLOAD_AVI : IDR_EXPLORE_AVI);
	m_wndAnimatedAvi.Play(0,(UINT)-1,(UINT)-1);

	/*
		piu' o meno il ciclo funziona nel modo seguente:

		if geturl from web
		do {
			parse url content
				insert urls into database
			while get next url from database
				get url from web
				if is html break
				else continue
		} while url has been retrieved
	*/
	if(bUseRobotsTxt)
	{
		char szRobotsTxtUrl[MAX_URL+1] = {0};
		CUrl url;
		URL u;
		URL uClean;

		// se l'url originale fosse un cgi, quando ricompone deve eliminare la query
		url.SplitUrl(szUrl,&u);
		strcpy(uClean.file,u.file);
		strcpy(uClean.host,u.host);
		strcpyn(uClean.file,"robots.txt",MAX_URL_FILE+1);
		strcpy(uClean.dir,"/");
		url.ComposeUrl(&uClean,szRobotsTxtUrl,sizeof(szRobotsTxtUrl));
		if(url.GetHttpUrl(	&m_httpConnection,
						szRobotsTxtUrl,
						NULL,
						szTranslatedUrl,
						sizeof(szTranslatedUrl),
						szDownloadPath,
						szLocalFile,
						sizeof(szLocalFile),
						&ctype))
		{
			CTextFile robotsFile;
			if(robotsFile.Open(szLocalFile,FALSE))
			{
				int i;
				char* p;
				ITEM* item;
				char szBuffer[_MAX_PATH+1] = {0};
				char szRobotName[_MAX_PATH+1] = {0};
				char szDisallow[_MAX_PATH+1] = {0};
				CWildCards wildCards;
				while(robotsFile.ReadLine(szBuffer,sizeof(szBuffer)-1)!=FILE_EOF)
				{
					p = stristr(szBuffer,"User-agent:");
					if(p)
					{
						p += 11;
						while(*p && *p==' ')
							p++;
						i = 0;
						while(*p && *p!=' ' && *p!='\r' && *p!='\n' && i < sizeof(szRobotName)-1)
							szRobotName[i++] = *p++;
					}
					else
					{
						memset(szDisallow,'\0',sizeof(szDisallow));
						p = stristr(szBuffer,"Disallow:");
						if(p)
						{
							p += 9;
							while(*p && *p==' ')
								p++;
							i = 0;
							while(*p && *p!=' ' && *p!='\r' && *p!='\n' && i < sizeof(szDisallow)-1)
								szDisallow[i++] = *p++;
						}
						if(szDisallow[0]!='\0' && wildCards.Match(szRobotName,WALLPAPER_INTERNAL_NAME))
						{
							item = (ITEM*)listRobotsTxtExclude.Add();
							if(item)
								strcpyn(item->item,szDisallow,_MAX_ITEM+1);
						}
					}
				}
				
				robotsFile.Close();
			}
		}
	
		if(listRobotsTxtExclude.Count() <= 0)
			bUseRobotsTxt = FALSE;
	}

	/*
		robots.txt
	*/
	{BOOL bDisallow = FALSE;
	if(bUseRobotsTxt)
	{
		ITERATOR iter;
		ITEM* item;

		if((iter = listRobotsTxtExclude.First())!=(ITERATOR)NULL)
			do
			{
				if((item = (ITEM*)iter->data)!=(ITEM*)NULL)
				{
					if(stristr(szUrl,item->item))
					{
						bDisallow = TRUE;
						break;
					}
				}
				iter = listRobotsTxtExclude.Next(iter);
			} while(iter!=(ITERATOR)NULL);
	}
	if(bDisallow)
	{
		UrlService.Insert(nID,szUrl,szTranslatedUrl,CUrlStatus::URL_STATUS_EXCLUDED_BY_ROBOTSTXT);
		_snprintf(szBuffer,sizeof(szBuffer)-1,"Excluded by robots.txt:\r\n%s",szUrl);
		SetDlgItemText(IDC_STATIC_STATUS,szBuffer);
		goto done;
	}}

	// scarica (se necessario) l'icona per i favoriti per impostarla sul dialogo
	SetIcon(IDI_ICON_CRAWLER);
	_snprintf(szLocalFile,sizeof(szLocalFile)-1,"%sfavicon.ico",szDownloadPath);
	if(!::FileExist(szLocalFile))
	{
		char szFavIconUrl[MAX_URL+1];
		_snprintf(szFavIconUrl,sizeof(szFavIconUrl)-1,"%s:%ld/favicon.ico",Url.host,Url.port);
		url.GetHttpUrl(&m_httpConnection,szFavIconUrl,NULL,NULL,0,szDownloadPath,szLocalFile,sizeof(szLocalFile),&ctype);
	}
	if(::FileExist(szLocalFile))
		SetIcon(szLocalFile);

	// scarica l'url in locale
	if(url.GetHttpUrl(	&m_httpConnection,
					szUrl,
					&bFileDirMismatched,
					szTranslatedUrl,
					sizeof(szTranslatedUrl),
					szDownloadPath,
					szLocalFile,
					sizeof(szLocalFile),
					&ctype,
					FALSE,
					GetHttpUrlCallback,
					this))
	{
		// connessione riuscita
		if(!IS_HTTP_ERROR((nDownloadCode = m_httpConnection.GetHttpError())) && m_httpConnection.GetSocketError()==0)
		{
			TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperCrawlerDlg::Crawler(): GetHttpUrl(%s) download code=%ld\n",szUrl,nDownloadCode));
			
			bConnected = TRUE;

			if(nDownloadCode <= (int)HTTP_STATUS_SUCCESSFUL)
				nDownloadCode = (int)CUrlStatus::URL_STATUS_DOWNLOADED;
			else if(nDownloadCode==(int)HTTP_STATUS_NOT_MODIFIED)
				nDownloadCode = (int)CUrlStatus::URL_STATUS_NOT_MODIFIED;
			else
				nDownloadCode = (int)CUrlStatus::URL_STATUS_MOVED;

			// riprende il download interrotto
			if(nNextID!=0)
			{
				UrlService.Update(szUrl,(CUrlStatus::URL_STATUS)nDownloadCode);
				UpdateStatusBar(nID,nNextID,++nPict);
			}
			else // nuovo download
			{
				if(UrlService.Insert(	nID,
									m_szUrl,
									szTranslatedUrl,
									(CUrlStatus::URL_STATUS)nDownloadCode,
									szLocalFile,
									(double)::GetFileSizeExt(szLocalFile),
									nDownloadCode==(int)CUrlStatus::URL_STATUS_DOWNLOADED ? (((double)(m_httpConnection.GetObject()->endtime - m_httpConnection.GetObject()->starttime)) / 1000.0f) : 0.0f)
									)
				{
					UpdateStatusBar(nID,nNextID,++nPict);
					DoPreview(szLocalFile);
				}
			}
			
			if(bFileDirMismatched)
			{
				char* p = strrchr(m_szUrl,'/');
				if(p)
					p++;
				if(p)
					*p = '\0';
			}

			// cicla fino a che riesce a scaricare le url in locale:
			// - analizza il file html estraendo le referenze (links) ed inserendole nel database
			// - entra in un ciclo fino ad esaurire i records inseriti nel database: se si tratta di un tipo 
			//   file registrato lo scarica e continua il ciclo, se si tratta di un file html esce dal ciclo
			//   per estrarre le referenze ed inserirle nel database
			do {
				// analizza il contenuto del file html estraendo le referenze (links)
				pHRefList = url.ExtractLinks(szLocalFile,szUrl,dwExtractLinksFlags,ExtractLinksCallback,this);

				// report ASCII
				if(fpAsciiReport)
					fprintf(fpAsciiReport,"\r\n%s\r\n",szUrl);

				// per ognuna delle referenze (links) trovate
				if(pHRefList->Count() > 0)
				{
					// interfaccia utente
					char* pFile = strrchr(szLocalFile,'\\');
					pFile = (pFile && *(pFile+1)) ? pFile+1 : szLocalFile;
					int nLen = _snprintf(szBuffer,sizeof(szBuffer)-1,"%d link(s) found into: %s",pHRefList->Count(),pFile);
					SetDlgItemText(IDC_STATIC_STATUS,szBuffer);

					// inserisce le referenze (links) nel database
					HREF* href;
					ITERATOR iter;
					if((iter = pHRefList->First())!=(ITERATOR)NULL)
						for(int i = 1; iter!=(ITERATOR)NULL; i++)
						{
							href = (HREF*)iter->data;
							if(href)
							{
								BOOL bIsValidType = TRUE;

								// controlla se deve scaricare solo il contenuto della pagina
								if(bOnlyPageContent)
								{
									// non include le referenze a files html
									if(bIsValidType)
										bIsValidType = !url.IsHtmlFile(href->href);

									// controlla se il pathname/dominio rientra tra quelli da includere
									url.SplitUrl(href->href,&DomainUrl);
									if(!bIsValidType && m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_DOMAINUSEALWAYS_KEY))
										bIsValidType = FindExternal(DomainUrl.host,&listDomainsInclude);
									if(!bIsValidType && m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_PARENTURLUSEALWAYS_KEY))
										bIsValidType = FindExternal(DomainUrl.dir,&listParentsInclude);
								}
								else
									bIsValidType = TRUE;

								if(bIsValidType)
								{
									// interfaccia utente
									_snprintf(szBuffer+nLen,sizeof(szBuffer)-nLen-1,"\r\n(inserting link %d of %d)",i,pHRefList->Count());
									SetDlgItemText(IDC_STATIC_STATUS,szBuffer);
									
									// se l'url non specifica un numero porta, assume quello presente nella classe per
									// la connessione (url principale)
									// notare che l'url (oltre ad essere di tipo http) deve essere quella principale
									// se si tratta di un altro dominio, non deve assumere la stessa porta
									if(url.IsUrlType(href->href,HTTP_URL) && url.CompareHttpHost(Url.host,href->href,m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_INCLUDESUBDOMAINS_KEY)))
									{
										URL u;

										memset(&u,'\0',sizeof(URL));
										url.SplitUrl(href->href,&u);
										if(u.port <= 0)
											u.port = m_httpConnection.GetPortNumber();
										url.ComposeUrl(&u,href->href,MAX_URL+1);
										
										memset(&u,'\0',sizeof(URL));
										url.SplitUrl(szUrl,&u);
										if(u.port <= 0)
											u.port = m_httpConnection.GetPortNumber();
										url.ComposeUrl(&u,szUrl,MAX_URL+1);
									}

									// inserisce la referenza (link)
									TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperCrawlerDlg::Crawler(): database url: %s\n",href->href));
									if(UrlService.Insert(nID,szUrl,href->href))
										UpdateStatusBar(nID,nNextID,nPict);
								}

								// report ASCII
								if(fpAsciiReport)
									fprintf(fpAsciiReport,"\t%s\r\n",href->href);
							}

							iter = pHRefList->Next(iter);
						}
				}

				// interfaccia utente
				::PeekAndPump();
				if(m_nCancel!=0)
				{
					if(!m_bInBreakMessage)
					{
						m_bInCancelDialog = TRUE;
						switch(::MessageBoxResource(this->m_hWnd,MB_YESNOCANCEL|MB_ICONQUESTION,WALLPAPER_PROGRAM_NAME,IDS_QUESTION_CANCEL_CRAWLING))
						{
							// annulla il download del file
							case IDYES:
								m_nCancel = 0;
								m_nCancelScript = 0;
								SetWindowText(m_szTitle);
								SetDlgItemText(IDC_STATIC_STATUS,"");
								SetDlgItemText(IDC_STATIC_ESTIMATED_TIME_LEFT,"");
								SetDlgItemText(IDC_STATIC_CURRENT_STATUS,"");
								SetDlgItemText(IDC_STATIC_TRANSFER_RATE,"");
								SetDlgItemText(IDC_ESTIMATED_TIME_LEFT,"");
								SetDlgItemText(IDC_CURRENT_STATUS,"");
								SetDlgItemText(IDC_TRANSFER_RATE,"");
								m_wndProgressBar.SetPos(0);
								m_wndProgressBar.Hide();
								::DeleteFile(szLocalFile);
								UrlService.Update(szUrl,CUrlStatus::URL_STATUS_CANCELLED);
								m_bInCancelDialog = FALSE;
								break;
							// annulla il download del sito
							case IDNO:
								::DeleteFile(szLocalFile);
								UrlService.Update(szUrl,CUrlStatus::URL_STATUS_CANCELLED);
								m_bInCancelDialog = FALSE;
								goto done;
							// riprende il download
							case IDCANCEL:
							default:
								m_nCancel = 0;
								m_nCancelScript = 0;
								m_bInCancelDialog = FALSE;
								continue;
						}
					}
				}

				// deve uscire dal ciclo se il download non riesce
				bDownloaded = FALSE;
				
				// download delle referenze inserite sopra
				CUrlStatus::URL_STATUS nStat;
				while(UrlService.Get(++nNextID,szUrl,sizeof(szUrl),szParentUrl,sizeof(szParentUrl),nStat)!=NULL)
				{
					// interfaccia utente
					::PeekAndPump();
					if(m_nCancel!=0)
					{
						if(!m_bInBreakMessage)
						{
							m_bInCancelDialog = TRUE;
							switch(::MessageBoxResource(this->m_hWnd,MB_YESNOCANCEL|MB_ICONQUESTION,WALLPAPER_PROGRAM_NAME,IDS_QUESTION_CANCEL_CRAWLING))
							{
								// annulla il download del file
								case IDYES:
									m_nCancel = 0;
									m_nCancelScript = 0;
									SetWindowText(m_szTitle);
									SetDlgItemText(IDC_STATIC_STATUS,"");
									SetDlgItemText(IDC_STATIC_ESTIMATED_TIME_LEFT,"");
									SetDlgItemText(IDC_STATIC_CURRENT_STATUS,"");
									SetDlgItemText(IDC_STATIC_TRANSFER_RATE,"");
									SetDlgItemText(IDC_ESTIMATED_TIME_LEFT,"");
									SetDlgItemText(IDC_CURRENT_STATUS,"");
									SetDlgItemText(IDC_TRANSFER_RATE,"");
									m_wndProgressBar.SetPos(0);
									m_wndProgressBar.Hide();
									::DeleteFile(szLocalFile);
									UrlService.Update(szUrl,CUrlStatus::URL_STATUS_CANCELLED);
									m_bInCancelDialog = FALSE;
									continue;
								// annulla il download del sito
								case IDNO:
									::DeleteFile(szLocalFile);
									UrlService.Update(szUrl,CUrlStatus::URL_STATUS_CANCELLED);
									m_bInCancelDialog = FALSE;
									goto done;
								// riprende il download
								case IDCANCEL:
								default:
									m_nCancel = 0;
									m_nCancelScript = 0;
									m_bInCancelDialog = FALSE;
									break;
							}
						}
					}
					UpdateStatusBar(nID,nNextID,nPict);

					// controlla lo status dell'url
					if(nStat==CUrlStatus::URL_STATUS_DOWNLOADED || nStat==CUrlStatus::URL_STATUS_NOT_MODIFIED || nStat==CUrlStatus::URL_STATUS_MOVED)
					{
						_snprintf(szBuffer,sizeof(szBuffer)-1,"Already retrived:\r\n%s",szUrl);
						SetDlgItemText(IDC_STATIC_STATUS,szBuffer);
						continue;
					}

					// interfaccia utente
					SetDlgItemText(IDC_STATIC_STATUS,"Verifying filters...");

					// controlla che sia un url HTTP
					if(!url.IsUrlType(szUrl,HTTP_URL))
					{
						UrlService.Update(szUrl,CUrlStatus::URL_STATUS_INVALID_URL);
						_snprintf(szBuffer,sizeof(szBuffer)-1,"Not an http url: %s",szUrl);
						SetDlgItemText(IDC_STATIC_STATUS,szBuffer);
						continue;
					}

					//$ da verificare (correggere?) il disegno generale: se viene scaricato un insieme di urls
					// via script i controlli vengono effettuati _dopo_ aver scaricato l'url e non prima

					/*
						robots.txt
					*/
					BOOL bDisallow = FALSE;
					if(bUseRobotsTxt)
					{
						ITERATOR iter;
						ITEM* item;

						if((iter = listRobotsTxtExclude.First())!=(ITERATOR)NULL)
							do
							{
								if((item = (ITEM*)iter->data)!=(ITEM*)NULL)
								{
									if(stristr(szUrl,item->item))
									{
										bDisallow = TRUE;
										break;
									}
								}
								iter = listRobotsTxtExclude.Next(iter);
							} while(iter!=(ITERATOR)NULL);
					}
					if(bDisallow)
					{
						UrlService.Update(szUrl,CUrlStatus::URL_STATUS_EXCLUDED_BY_ROBOTSTXT);
						_snprintf(szBuffer,sizeof(szBuffer)-1,"Excluded by robots.txt:\r\n%s",szUrl);
						SetDlgItemText(IDC_STATIC_STATUS,szBuffer);
						continue;
					}

					/*
						wildcards
						considera stringhe, come '/cgi/', '*banner*', etc. e non domini/directory
					*/
					if(FindWildcards(szUrl,&Wildcards,&listWildcards))
					{
						UrlService.Update(szUrl,CUrlStatus::URL_STATUS_EXCLUDED_BY_WILDCARDS);
						_snprintf(szBuffer,sizeof(szBuffer)-1,"Excluded by wildcards:\r\n%s",szUrl);
						SetDlgItemText(IDC_STATIC_STATUS,szBuffer);
						continue;
					}

					/*
						domini
						considera i domini, come http://..., e non le stringhe
					*/
					bAsk = TRUE;

					// url esterna al dominio
					if(!url.CompareHttpHost(Url.host,szUrl,m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_INCLUDESUBDOMAINS_KEY)))
					{
						url.SplitUrl(szUrl,&DomainUrl);

						// controlla se il dominio rientra tra quelli da escludere
						if(FindExternal(DomainUrl.host,&listDomainsExclude))
						{
							UrlService.Update(szUrl,CUrlStatus::URL_STATUS_EXTERNAL_DOMAIN);
							_snprintf(szBuffer,sizeof(szBuffer)-1,"Excluded by domain:\r\n%s",szUrl);
							SetDlgItemText(IDC_STATIC_STATUS,szBuffer);
							continue;
						}
						
						// controlla se il dominio rientra tra quelli da includere
						bAsk = !FindExternal(DomainUrl.host,&listDomainsInclude);
					}
					else
						bAsk = FALSE;

					if(bAsk)
					{
						// ricava il tipo di azione da intraprendere
						if(bOnlyPageContent)
						{
							domainacceptance = EXCLUDE_ALL_DOMAINS_EXCEPT_PICTURES;
							bAskForDomainAcceptance = FALSE;
						}
						else
						{
							if(bAskForDomainAcceptance)
							{
								CWallPaperDomainDlg dlg(this,m_pConfig,szUrl,domainacceptance);
								switch(dlg.DoModal())
								{
									case IDOK:
									{
										domainacceptance = dlg.GetDomainAcceptance();
										bAskForDomainAcceptance = dlg.AskForDomainAcceptance();
										
										// da mantenere in corrispondenza con il codice del dialogo
										switch(domainacceptance)
										{
											case EXCLUDE_ALL_DOMAINS:
											case EXCLUDE_ALL_DOMAINS_EXCEPT_PICTURES:
											case EXCLUDE_ALL_DOMAINS_EXCEPT_REGFILETYPES:
											case FOLLOW_ALL_DOMAINS:
												bAskForDomainAcceptance = FALSE;
										}
										
										break;
									}

									case IDCANCEL:
									default:
										UrlService.Update(szUrl,CUrlStatus::URL_STATUS_CANCELLED);
										m_nCancel = 1;
										m_bInCancelDialog = FALSE;
										goto done;
								}
							}
						}

						// include/esclude l'url fuori dal dominio
						switch(domainacceptance)
						{
							case EXCLUDE_THIS_DOMAIN:
							case EXCLUDE_ALL_DOMAINS:
							case EXCLUDE_ALL_URLS_FROM_THIS_DOMAIN:
							{
								if(domainacceptance==EXCLUDE_ALL_URLS_FROM_THIS_DOMAIN)
									AddExternal(DomainUrl.host,&listDomainsExclude);
								AddExternal(DomainUrl.host,&listDomainsExcludeSave);
								UrlService.Update(szUrl,CUrlStatus::URL_STATUS_EXTERNAL_DOMAIN);
								_snprintf(szBuffer,sizeof(szBuffer)-1,"Excluded by domain:\r\n%s",szUrl);
								SetDlgItemText(IDC_STATIC_STATUS,szBuffer);
								continue;
							}

							// EXCLUDE_ALL_DOMAINS_EXCEPT_PICTURES: solo le immagini
							// EXCLUDE_ALL_DOMAINS_EXCEPT_REGFILETYPES: solo i tipi registrati (che per default includono anche le immagini)

							case EXCLUDE_ALL_DOMAINS_EXCEPT_PICTURES:
							{
								BOOL bIsPictureFile = FALSE;
								LPIMAGETYPE pImagetype;
								while((pImagetype = m_pImage->EnumImageFormats())!=NULL)
									if(striright(szUrl,pImagetype->ext)==0)
										bIsPictureFile = TRUE;
								if(striright(szUrl,".ico")==0) // non gestito cd CImage
									bIsPictureFile = TRUE;
								if(!bIsPictureFile)
								{
									AddExternal(DomainUrl.host,&listDomainsExcludeSave);
									UrlService.Update(szUrl,CUrlStatus::URL_STATUS_EXTERNAL_DOMAIN);
									_snprintf(szBuffer,sizeof(szBuffer)-1,"Excluded by domain:\r\n%s",szUrl);
									SetDlgItemText(IDC_STATIC_STATUS,szBuffer);
									continue;
								}
								else
									break;
							}

							case EXCLUDE_ALL_DOMAINS_EXCEPT_REGFILETYPES:
							{
								if(!FindFileType(szUrl,&FileTypeList))
								{
									AddExternal(DomainUrl.host,&listDomainsExcludeSave);
									UrlService.Update(szUrl,CUrlStatus::URL_STATUS_EXTERNAL_DOMAIN);
									_snprintf(szBuffer,sizeof(szBuffer)-1,"Excluded by domain:\r\n%s",szUrl);
									SetDlgItemText(IDC_STATIC_STATUS,szBuffer);
									continue;
								}
								else
									break;
							}

							case FOLLOW_THIS_DOMAIN:
							case FOLLOW_ALL_DOMAINS:
							case FOLLOW_ALL_URLS_FROM_THIS_DOMAIN:
							{
								if(domainacceptance==FOLLOW_ALL_URLS_FROM_THIS_DOMAIN)
									AddExternal(DomainUrl.host,&listDomainsInclude);
								AddExternal(DomainUrl.host,&listDomainsIncludeSave);
								break;
							}

							case UNDEFINED_DOMAIN_ACCEPTANCE:
							default:
								continue;
						}
					}

					/*
						directories padre
						controlla se l'url fa riferimento ad una directory esterna a quella specficata
					*/
					bAsk = TRUE;

					// pathname esterno all'url
					if(url.IsParentHttpUrl(szUrl,m_szUrl))
					{
						url.SplitUrl(szUrl,&DomainUrl);

						// controlla se il pathname rientra tra quelli da escludere
						if(FindExternal(DomainUrl.dir,&listParentsExclude))
						{
							UrlService.Update(szUrl,CUrlStatus::URL_STATUS_PARENT_URL);
							_snprintf(szBuffer,sizeof(szBuffer)-1,"Excluded by parent url:\r\n%s",szUrl);
							SetDlgItemText(IDC_STATIC_STATUS,szBuffer);
							continue;
						}
						
						// controlla se il pathname rientra tra quelli da includere
						bAsk = !FindExternal(DomainUrl.dir,&listParentsInclude);
					}
					else
						bAsk = FALSE;

					if(bAsk)
					{
						// ricava il tipo di azione da intraprendere
						if(bOnlyPageContent)
						{
							parenturlacceptance = EXCLUDE_ALL_PARENTURLS_EXCEPT_PICTURES;
							bAskForParentAcceptance = FALSE;
						}
						else
						{
							if(bAskForParentAcceptance)
							{
								CWallPaperParentUrlDlg dlg(this,m_pConfig,szUrl,parenturlacceptance);
								switch(dlg.DoModal())
								{
									case IDOK:
									{
										parenturlacceptance = dlg.GetParentAcceptance();
										bAskForParentAcceptance = dlg.AskForParentAcceptance();
										
										// da mantenere in corrispondenza con il codice del dialogo
										switch(domainacceptance)
										{
											case EXCLUDE_ALL_PARENTURLS:
											case EXCLUDE_ALL_PARENTURLS_EXCEPT_PICTURES:
											case EXCLUDE_ALL_PARENTURLS_EXCEPT_REGFILETYPES:
											case FOLLOW_ALL_PARENTURLS:
												bAskForParentAcceptance = FALSE;
										}
										
										break;
									}

									case IDCANCEL:
									default:
										UrlService.Update(szUrl,CUrlStatus::URL_STATUS_CANCELLED);
										m_nCancel = 1;
										m_bInCancelDialog = FALSE;
										goto done;
								}
							}
						}

						// include/esclude il pathname fuori dall'url
						switch(parenturlacceptance)
						{
							case EXCLUDE_THIS_PARENTURL:
							case EXCLUDE_ALL_PARENTURLS:
							case EXCLUDE_ALL_PARENTURLS_FROM_THIS_PATHNAME:
							{
								if(parenturlacceptance==EXCLUDE_ALL_PARENTURLS_FROM_THIS_PATHNAME)
									AddExternal(DomainUrl.dir,&listParentsExclude);
								AddExternal(DomainUrl.dir,&listParentsExcludeSave);
								UrlService.Update(szUrl,CUrlStatus::URL_STATUS_PARENT_URL);
								_snprintf(szBuffer,sizeof(szBuffer)-1,"Excluded by parent url:\r\n%s",szUrl);
								SetDlgItemText(IDC_STATIC_STATUS,szBuffer);
								continue;
							}

							// EXCLUDE_ALL_PARENTURLS_EXCEPT_PICTURES: solo le immagini
							// EXCLUDE_ALL_PARENTURLS_EXCEPT_REGFILETYPES: solo i tipi registrati (che per default includono anche le immagini)

							case EXCLUDE_ALL_PARENTURLS_EXCEPT_PICTURES:
							{
								BOOL bIsPictureFile = FALSE;
								LPIMAGETYPE pImagetype;
								while((pImagetype = m_pImage->EnumImageFormats())!=NULL)
									if(striright(szUrl,pImagetype->ext)==0)
										bIsPictureFile = TRUE;
								if(striright(szUrl,".ico")==0) // non gestito cd CImage
									bIsPictureFile = TRUE;
								if(!bIsPictureFile)
								{
									AddExternal(DomainUrl.dir,&listParentsExcludeSave);
									UrlService.Update(szUrl,CUrlStatus::URL_STATUS_PARENT_URL);
									_snprintf(szBuffer,sizeof(szBuffer)-1,"Excluded by parent url:\r\n%s",szUrl);
									SetDlgItemText(IDC_STATIC_STATUS,szBuffer);
									continue;
								}
								else
									break;
							}

							case EXCLUDE_ALL_PARENTURLS_EXCEPT_REGFILETYPES:
							{
								if(!FindFileType(szUrl,&FileTypeList))
								{
									AddExternal(DomainUrl.dir,&listParentsExcludeSave);
									UrlService.Update(szUrl,CUrlStatus::URL_STATUS_PARENT_URL);
									_snprintf(szBuffer,sizeof(szBuffer)-1,"Excluded by parent url:\r\n%s",szUrl);
									SetDlgItemText(IDC_STATIC_STATUS,szBuffer);
									continue;
								}
								else
									break;
							}

							case FOLLOW_THIS_PARENTURL:
							case FOLLOW_ALL_PARENTURLS:
							case FOLLOW_ALL_PARENTURLS_FROM_THIS_PATHNAME:
							{
								if(parenturlacceptance==FOLLOW_ALL_PARENTURLS_FROM_THIS_PATHNAME)
									AddExternal(DomainUrl.dir,&listParentsInclude);
								AddExternal(DomainUrl.dir,&listParentsIncludeSave);
								break;
							}

							case UNDEFINED_PARENT_ACCEPTANCE:
							default:
								continue;
						}
					}
download_file:
					// interfaccia utente
					CFilenameFactory fn;
					CFilenameFactory ul;
					_snprintf(szBuffer,
							sizeof(szBuffer)-1,
							"Retrieving: %s\r\nFrom: %s",
							fn.Abbreviate(url.StripPathFromUrl(szUrl),40),
							ul.Abbreviate(szUrl,54,FALSE,FALSE)
							);
					SetDlgItemText(IDC_STATIC_STATUS,szBuffer);

					/*
						recupera l'url
						distingue a seconda del tipo (file HTML, immagine, directory, cgi)
					*/

					// file HTML
					if(url.IsHtmlFile(szUrl))
					{
						url.SplitUrl(szUrl,&ThisUrl);
						strcpyn(Url.dir,ThisUrl.dir,sizeof(Url.dir));
						
						// compone (non crea) il pathname locale relativo all'url
						if(url.CreatePathNameFromHttpUrl(szUrl,szDownloadPath,sizeof(szDownloadPath),m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_DOWNLOADDIR_KEY),FALSE))
						{
							// download dell'url
							if(url.GetHttpUrl(	&m_httpConnection,
											szUrl,
											&bFileDirMismatched,
											szTranslatedUrl,
											sizeof(szTranslatedUrl),
											szDownloadPath,
											szLocalFile,
											sizeof(szLocalFile),
											&ctype,
											FALSE,
											GetHttpUrlCallback,
											this))
							{
								// download riuscito, esce dal ciclo per estrarre le referenze
								if(!IS_HTTP_ERROR((nDownloadCode = m_httpConnection.GetHttpError())) && m_httpConnection.GetSocketError()==0)
								{
									TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperCrawlerDlg::Crawler(): GetHttpUrl(%s) download code=%ld\n",szUrl,nDownloadCode));
									
									if(nDownloadCode <= (int)HTTP_STATUS_SUCCESSFUL)
										nDownloadCode = (int)CUrlStatus::URL_STATUS_DOWNLOADED;
									else if(nDownloadCode==(int)HTTP_STATUS_NOT_MODIFIED)
										nDownloadCode = (int)CUrlStatus::URL_STATUS_NOT_MODIFIED;
									else
										nDownloadCode = (int)CUrlStatus::URL_STATUS_MOVED;

									// aggiorna il database
									UrlService.Update(	szUrl,
													(CUrlStatus::URL_STATUS)nDownloadCode,
													szLocalFile,
													(double)::GetFileSizeExt(szLocalFile),
													nDownloadCode==(int)CUrlStatus::URL_STATUS_DOWNLOADED ? (((double)(m_httpConnection.GetObject()->endtime - m_httpConnection.GetObject()->starttime)) / 1000.0f) : 0.0f
													);

									// aggiunge il file alla playlist
									AddDownloadToPlaylist(nDownloadCode,szLocalFile);

									if(!bOnlyPageContent)
									{
										bDownloaded = TRUE;
										break;
									}
								}
								else // download fallito, passa all'url seguente
								{
									TRACEEXPR((_TRACE_FLAG_ERR,__NOFILE__,__NOLINE__,"CWallPaperCrawlerDlg::Crawler(): GetHttpUrl(%s) error: http=%ld, socket=%ld\n",szUrl,m_httpConnection.GetHttpError(),m_httpConnection.GetSocketError()));
									UrlService.Update(szUrl,m_httpConnection.IsSocketError() ? (CUrlStatus::URL_STATUS)m_httpConnection.GetSocketError() : (m_httpConnection.IsHttpError() ? (CUrlStatus::URL_STATUS)m_httpConnection.GetHttpError() : CUrlStatus::URL_STATUS_UNKNOWN),NULL,(double)m_httpConnection.GetObject()->size);
								}
							}
							else // download interrotto
							{
								::DeleteFile(szLocalFile);
								if(!m_bInBreakMessage)
								{
									m_bInCancelDialog = TRUE;
									switch(::MessageBoxResource(this->m_hWnd,MB_YESNOCANCEL|MB_ICONQUESTION,WALLPAPER_PROGRAM_NAME,IDS_QUESTION_CANCEL_CRAWLING))
									{
										// annulla il download del file
										case IDYES:
											m_nCancel = 0;
											m_nCancelScript = 0;
											SetWindowText(m_szTitle);
											SetDlgItemText(IDC_STATIC_STATUS,"");
											SetDlgItemText(IDC_STATIC_ESTIMATED_TIME_LEFT,"");
											SetDlgItemText(IDC_STATIC_CURRENT_STATUS,"");
											SetDlgItemText(IDC_STATIC_TRANSFER_RATE,"");
											SetDlgItemText(IDC_ESTIMATED_TIME_LEFT,"");
											SetDlgItemText(IDC_CURRENT_STATUS,"");
											SetDlgItemText(IDC_TRANSFER_RATE,"");
											m_wndProgressBar.SetPos(0);
											m_wndProgressBar.Hide();
											UrlService.Update(szUrl,CUrlStatus::URL_STATUS_CANCELLED);
											m_bInCancelDialog = FALSE;
											break;
										// annulla il download del sito
										case IDNO:
											UrlService.Update(szUrl,CUrlStatus::URL_STATUS_CANCELLED);
											m_bInCancelDialog = FALSE;
											goto done;
										// riprende il download
										case IDCANCEL:
										default:
											m_nCancel = 0;
											m_nCancelScript = 0;
											m_bInCancelDialog = FALSE;
											goto download_file;
											break;
									}
								}
							}
						}
					}
					// immagine o tipo file registrato
					else if(FindFileType(szUrl,&FileTypeList))
					{
						url.SplitUrl(szUrl,&ThisUrl);
						strcpyn(Url.dir,ThisUrl.dir,sizeof(Url.dir));
						
						// compone (non crea) il pathname locale relativo all'url
						if(url.CreatePathNameFromHttpUrl(szUrl,szDownloadPath,sizeof(szDownloadPath),m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_DOWNLOADDIR_KEY),FALSE))
						{
							// download multithread
							if(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_MULTITHREAD_KEY))
							{
								int nMaxThreads = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_MAXTHREADS_KEY);
								if(m_nActiveThreads >= nMaxThreads)
								{
									TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperCrawlerDlg::Crawler(): max # of thread (%d) reached (%ld), waiting...\n",nMaxThreads,m_nActiveThreads));
									SetDlgItemText(IDC_STATIC_TOTAL_THREADS,"Thread overhead...");
									while(m_nActiveThreads >= nMaxThreads)
									{
										::PeekAndPump();
										::Sleep(500L);
									}
									TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperCrawlerDlg::Crawler(): max # of thread (%d) normalized\n",m_nActiveThreads));
								}
								
								// crea il thread per il download dell'url
								//CThread* pThread = NULL;
								CWinThread* pThread = NULL;
								CRAWLTHREAD* pCrawlThread = (CRAWLTHREAD*)m_listActiveCrawlerThreads.Add();
								if(pCrawlThread)
								{
									pThread = ::AfxBeginThread(Crawl,(LPVOID)pCrawlThread,m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PRIORITY_THREAD_KEY),0,CREATE_SUSPENDED);
									if(pThread)
									{
										// il distruttore dell'elemento della lista si occupa di eliminare l'oggetto per il thread
										pThread->m_bAutoDelete = FALSE;
										//pThread->SetAutoDelete(FALSE);
										
										// inserisce il thread nella lista
										// (alla fine del download dovra' attendere il termine di ognuno di essi)
										pCrawlThread->pWinThread = pThread;
										//pCrawlThread->pThread = pThread;
										pCrawlThread->pThis = (LPVOID)this;
										pCrawlThread->nStatus = WAIT_TIMEOUT;
										pCrawlThread->uStatus = CUrlStatus::URL_STATUS_UNKNOWN;
										pCrawlThread->dwSize = 0L;
										pCrawlThread->dlTotalTime = 0.0f;
										pCrawlThread->nDownloadCode = -1;
										strcpyn(pCrawlThread->szThreadName,szUrl,MAX_THREAD_NAME+1);
										strcpyn(pCrawlThread->szUrl,szUrl,MAX_URL+1);
										strcpyn(pCrawlThread->szDownloadPath,szDownloadPath,_MAX_PATH+1);
										strcpyn(pCrawlThread->szFileName,szLocalFile,_MAX_PATH+1);
										pCrawlThread->pUrlService = &UrlService;
									}
								}

								if(pThread)
									pThread->ResumeThread();
									//pThread->Resume();

								// interfaccia utente
								UpdateStatusBar(nID,nNextID,++nPict);
							}
							else // download sequenziale
							{
								// download dell'url
								if(url.GetHttpUrl(	&m_httpConnection,
												szUrl,
												&bFileDirMismatched,
												szTranslatedUrl,
												sizeof(szTranslatedUrl),
												szDownloadPath,
												szLocalFile,
												sizeof(szLocalFile),
												&ctype,
												FALSE,
												GetHttpUrlCallback,
												this))
								{
									// download riuscito, passa all'url seguente
									if(!IS_HTTP_ERROR((nDownloadCode = m_httpConnection.GetHttpError())) && m_httpConnection.GetSocketError()==0)
									{
										TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperCrawlerDlg::Crawler(): GetHttpUrl(%s) download code=%ld\n",szUrl,nDownloadCode));
				
										if(nDownloadCode <= (int)HTTP_STATUS_SUCCESSFUL)
											nDownloadCode = (int)CUrlStatus::URL_STATUS_DOWNLOADED;
										else if(nDownloadCode==(int)HTTP_STATUS_NOT_MODIFIED)
											nDownloadCode = (int)CUrlStatus::URL_STATUS_NOT_MODIFIED;
										else
											nDownloadCode = (int)CUrlStatus::URL_STATUS_MOVED;

										// aggiorna il database
										UrlService.Update(	szUrl,
														(CUrlStatus::URL_STATUS)nDownloadCode,
														szLocalFile,
														(double)::GetFileSizeExt(szLocalFile),
														nDownloadCode==(int)CUrlStatus::URL_STATUS_DOWNLOADED ? (((double)(m_httpConnection.GetObject()->endtime - m_httpConnection.GetObject()->starttime)) / 1000.0f) : 0.0f
														);

										// aggiunge il file alla playlist
										AddDownloadToPlaylist(nDownloadCode,szLocalFile);
										
										// per i cgi che contengono un estensione, ad esempio .cgi, presente nella
										// lista dei tipi di file registrati, come in "http://[...].cgi?[...]"
										// se si tratta di un file html esce dal ciclo per estrarre le referenze
										if(url.IsCgiUrl(szUrl))
											if(url.IsHtmlFile(szLocalFile))
											{
												if(!bOnlyPageContent)
												{
													bDownloaded = TRUE;
													break;
												}
											}
										
										// interfaccia utente
										UpdateStatusBar(nID,nNextID,++nPict);
										DoPreview(szLocalFile);
									}
									else // download fallito, passa all'url seguente
									{
										TRACEEXPR((_TRACE_FLAG_ERR,__NOFILE__,__NOLINE__,"CWallPaperCrawlerDlg::Crawler(): GetHttpUrl(%s) error: http=%ld, socket=%ld\n",szUrl,m_httpConnection.GetHttpError(),m_httpConnection.GetSocketError()));
										UrlService.Update(szUrl,m_httpConnection.IsSocketError() ? (CUrlStatus::URL_STATUS)m_httpConnection.GetSocketError() : (m_httpConnection.IsHttpError() ? (CUrlStatus::URL_STATUS)m_httpConnection.GetHttpError() : CUrlStatus::URL_STATUS_UNKNOWN),NULL,(double)m_httpConnection.GetObject()->size);
									}
								}
								else // download interrotto
								{
									::DeleteFile(szLocalFile);
									if(!m_bInBreakMessage)
									{
										m_bInCancelDialog = TRUE;
										switch(::MessageBoxResource(this->m_hWnd,MB_YESNOCANCEL|MB_ICONQUESTION,WALLPAPER_PROGRAM_NAME,IDS_QUESTION_CANCEL_CRAWLING))
										{
											// annulla il download del file
											case IDYES:
												m_nCancel = 0;
												m_nCancelScript = 0;
												SetWindowText(m_szTitle);
												SetDlgItemText(IDC_STATIC_STATUS,"");
												SetDlgItemText(IDC_STATIC_ESTIMATED_TIME_LEFT,"");
												SetDlgItemText(IDC_STATIC_CURRENT_STATUS,"");
												SetDlgItemText(IDC_STATIC_TRANSFER_RATE,"");
												SetDlgItemText(IDC_ESTIMATED_TIME_LEFT,"");
												SetDlgItemText(IDC_CURRENT_STATUS,"");
												SetDlgItemText(IDC_TRANSFER_RATE,"");
												m_wndProgressBar.SetPos(0);
												m_wndProgressBar.Hide();
												UrlService.Update(szUrl,CUrlStatus::URL_STATUS_CANCELLED);
												m_bInCancelDialog = FALSE;
												break;
											// annulla il download del sito
											case IDNO:
												UrlService.Update(szUrl,CUrlStatus::URL_STATUS_CANCELLED);
												m_bInCancelDialog = FALSE;
												goto done;
											// riprende il download
											case IDCANCEL:
											default:
												m_nCancel = 0;
												m_nCancelScript = 0;
												m_bInCancelDialog = FALSE;
												goto download_file;
										}
									}
								}
							}
						}
					}
					// directory/dominio o cgi che restituisce un immagine o tipo file non registrato
					else
					{
						// controlla che si tratti di un url HTTP
						if(url.SplitUrl(szUrl,&ThisUrl))
						{
							// controlla che si tratti di una directory/dominio (nome del file vuoto)
							
							// se e' un cgi che contiene un & con parametri alla fine (non e' presente nessuna estensione)
							//http://www.agfanet.com/en/cafe/tutor/fset_enlarge.php3?p=photos/20002301_l.jpg&x=420&y=270
							if(url.IsCgiUrl(szUrl) && !m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_ENABLECGI_KEY))
							{
								UrlService.Update(szUrl,CUrlStatus::URL_STATUS_CGI_URL);
							}
							else if(strcmp(ThisUrl.file,"")==0 || (url.IsCgiUrl(szUrl) && m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_ENABLECGI_KEY)))
							{
								strcpyn(Url.dir,ThisUrl.dir,sizeof(Url.dir));
								
								// compone (non crea) il pathname locale relativo all'url
								if(url.CreatePathNameFromHttpUrl(szUrl,szDownloadPath,sizeof(szDownloadPath),m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_DOWNLOADDIR_KEY),FALSE))
								{
									// download dell'url
									if(url.GetHttpUrl(	&m_httpConnection,
													szUrl,
													&bFileDirMismatched,
													szTranslatedUrl,
													sizeof(szTranslatedUrl),
													szDownloadPath,
													szLocalFile,
													sizeof(szLocalFile),
													&ctype,
													FALSE,
													GetHttpUrlCallback,
													this))
									{
										// download riuscito, esce dal ciclo per estrarre le referenze
										if(!IS_HTTP_ERROR((nDownloadCode = m_httpConnection.GetHttpError())) && m_httpConnection.GetSocketError()==0)
										{
											TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperCrawlerDlg::Crawler(): GetHttpUrl(%s) download code=%ld\n",szUrl,nDownloadCode));
											
											if(nDownloadCode <= (int)HTTP_STATUS_SUCCESSFUL)
												nDownloadCode = (int)CUrlStatus::URL_STATUS_DOWNLOADED;
											else if(nDownloadCode==(int)HTTP_STATUS_NOT_MODIFIED)
												nDownloadCode = (int)CUrlStatus::URL_STATUS_NOT_MODIFIED;
											else
												nDownloadCode = (int)CUrlStatus::URL_STATUS_MOVED;

											// aggiorna il database
											UrlService.Update(	szUrl,
															(CUrlStatus::URL_STATUS)nDownloadCode,
															szLocalFile,
															(double)::GetFileSizeExt(szLocalFile),
															nDownloadCode==(int)CUrlStatus::URL_STATUS_DOWNLOADED ? (((double)(m_httpConnection.GetObject()->endtime - m_httpConnection.GetObject()->starttime)) / 1000.0f) : 0.0f
															);

											// immagine come risultato dell'esecuzione di un cgi
											{
												if(m_pImage->IsSupportedFormat(szLocalFile))
													++nPict;
												UpdateStatusBar(nID,nNextID,nPict);
												DoPreview(szLocalFile);
											}

											// aggiunge il file alla playlist
											AddDownloadToPlaylist(nDownloadCode,szLocalFile);

											if(!bOnlyPageContent)
											{
												bDownloaded = TRUE;
												break;
											}
										}
										else // download fallito, passa all'url seguente
										{
											TRACEEXPR((_TRACE_FLAG_ERR,__NOFILE__,__NOLINE__,"CWallPaperCrawlerDlg::Crawler(): GetHttpUrl(%s) error: http=%ld, socket=%ld\n",szUrl,m_httpConnection.GetHttpError(),m_httpConnection.GetSocketError()));
											UrlService.Update(szUrl,m_httpConnection.IsSocketError() ? (CUrlStatus::URL_STATUS)m_httpConnection.GetSocketError() : (m_httpConnection.IsHttpError() ? (CUrlStatus::URL_STATUS)m_httpConnection.GetHttpError() : CUrlStatus::URL_STATUS_UNKNOWN),NULL,(double)m_httpConnection.GetObject()->size);
										}
									}
									else // download interrotto
									{
										::DeleteFile(szLocalFile);
										if(!m_bInBreakMessage)
										{
											m_bInCancelDialog = TRUE;
											switch(::MessageBoxResource(this->m_hWnd,MB_YESNOCANCEL|MB_ICONQUESTION,WALLPAPER_PROGRAM_NAME,IDS_QUESTION_CANCEL_CRAWLING))
											{
												// annulla il download del file
												case IDYES:
													m_nCancel = 0;
													m_nCancelScript = 0;
													SetWindowText(m_szTitle);
													SetDlgItemText(IDC_STATIC_STATUS,"");
													SetDlgItemText(IDC_STATIC_ESTIMATED_TIME_LEFT,"");
													SetDlgItemText(IDC_STATIC_CURRENT_STATUS,"");
													SetDlgItemText(IDC_STATIC_TRANSFER_RATE,"");
													SetDlgItemText(IDC_ESTIMATED_TIME_LEFT,"");
													SetDlgItemText(IDC_CURRENT_STATUS,"");
													SetDlgItemText(IDC_TRANSFER_RATE,"");
													m_wndProgressBar.SetPos(0);
													m_wndProgressBar.Hide();
													UrlService.Update(szUrl,CUrlStatus::URL_STATUS_CANCELLED);
													m_bInCancelDialog = FALSE;
													break;
												// annulla il download sito
												case IDNO:
													UrlService.Update(szUrl,CUrlStatus::URL_STATUS_CANCELLED);
													m_bInCancelDialog = FALSE;
													goto done;
												// riprende il download
												case IDCANCEL:
												default:
													m_nCancel = 0;
													m_nCancelScript = 0;
													m_bInCancelDialog = FALSE;
													goto download_file;
											}
										}
									}
								}
							}
							else
							{
								// tipo non riconosciuto, chiede se aggiungerlo al volo prima di marcarlo come non valido
								if(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_FILETYPESACCEPTANCE_KEY)==FILETYPESACCEPTANCE_ASK)
								{
									url.SplitUrl(szUrl,&ThisUrl);
									char* pExt = strrchr(ThisUrl.file,'.');
									if(::MessageBoxResourceEx(this->m_hWnd,MB_YESNO|MB_ICONQUESTION,WALLPAPER_PROGRAM_NAME,IDS_QUESTION_FILETYPESACCEPTANCE,ThisUrl.file,pExt ? pExt : "<file with no extension>")==IDYES)
									{
										FILETYPE* pFiletype = (FILETYPE*)FileTypeList.Add();
										if(pFiletype)
										{
											strcpyn(pFiletype->ext,pExt ? pExt : "",MAX_FILETYPE_EXT+1);
											pFiletype->len = strlen(pFiletype->ext);
											goto download_file;
										}
									}
								}
								
								UrlService.Update(szUrl,CUrlStatus::URL_STATUS_INVALID_FILE);
							}
						}
						else
						{
							UrlService.Update(szUrl,CUrlStatus::URL_STATUS_INVALID_URL);
						}
					}

					// interfaccia utente
					::PeekAndPump();
					if(m_nCancel!=0)
					{
						if(!m_bInBreakMessage)
						{
							m_bInCancelDialog = TRUE;
							switch(::MessageBoxResource(this->m_hWnd,MB_YESNOCANCEL|MB_ICONQUESTION,WALLPAPER_PROGRAM_NAME,IDS_QUESTION_CANCEL_CRAWLING))
							{
								// annulla il download del file
								case IDYES:
									m_nCancel = 0;
									m_nCancelScript = 0;
									SetWindowText(m_szTitle);
									SetDlgItemText(IDC_STATIC_STATUS,"");
									SetDlgItemText(IDC_STATIC_ESTIMATED_TIME_LEFT,"");
									SetDlgItemText(IDC_STATIC_CURRENT_STATUS,"");
									SetDlgItemText(IDC_STATIC_TRANSFER_RATE,"");
									SetDlgItemText(IDC_ESTIMATED_TIME_LEFT,"");
									SetDlgItemText(IDC_CURRENT_STATUS,"");
									SetDlgItemText(IDC_TRANSFER_RATE,"");
									m_wndProgressBar.SetPos(0);
									m_wndProgressBar.Hide();
									::DeleteFile(szLocalFile);
									UrlService.Update(szUrl,CUrlStatus::URL_STATUS_CANCELLED);
									m_bInCancelDialog = FALSE;
									break;
								// annulla il download del sito
								case IDNO:
									::DeleteFile(szLocalFile);
									UrlService.Update(szUrl,CUrlStatus::URL_STATUS_CANCELLED);
									m_bInCancelDialog = FALSE;
									goto done;
								// riprende il download
								case IDCANCEL:
								default:
									m_nCancel = 0;
									m_nCancelScript = 0;
									m_bInCancelDialog = FALSE;
									continue;
							}
						}
					}
				}
			} while(bDownloaded);
		}
		else
		{
			TRACEEXPR((_TRACE_FLAG_ERR,__NOFILE__,__NOLINE__,"CWallPaperCrawlerDlg::Crawler(): GetHttpUrl(%s) error: http=%ld, socket=%ld\n",szUrl,m_httpConnection.GetHttpError(),m_httpConnection.GetSocketError()));
			
			if((nDownloadCode = m_httpConnection.GetHttpError())==0)
				nDownloadCode = m_httpConnection.GetSocketError();

			if(m_bUseScript && m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_QUIETINSCRIPTMODE_KEY))
				;
			else
			{
				char szAddress[256];
				if(m_httpConnection.GetProxyPortNumber() > 0)
					_snprintf(szAddress,sizeof(szAddress)-1,"%s (through the %s:%ld proxy)",szUrl,m_httpConnection.GetProxyAddress(),m_httpConnection.GetProxyPortNumber());
				else
					strcpyn(szAddress,szUrl,sizeof(szAddress));
				::MessageBoxResourceEx(this->m_hWnd,MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_DOWNLOAD,szAddress,m_httpConnection.GetErrorString());
			}
		}
	}
	else
		TRACEEXPR((_TRACE_FLAG_ERR,__NOFILE__,__NOLINE__,"CWallPaperCrawlerDlg::Crawler(): GetHttpUrl() failed\n"));

done:

	// se e' stato effettuato il download in modalita' multithread, deve attendere il termine di ognuno dei threads
	if(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_MULTITHREAD_KEY))
	{
		// interfaccia utente
		SetDlgItemText(IDC_STATIC_STATUS,"Waiting for threads termination...");

		UINT nActiveThreads = 0;
		ITERATOR iter;
		CRAWLTHREAD* pCrawlThread;
		//CThread* pThread;
		CWinThread* pThread;
		DWORD dwStatus = 0L;

		do
		{
			nActiveThreads = 0;

			if((iter = m_listActiveCrawlerThreads.First())!=(ITERATOR)NULL)
			{
				do
				{
					pCrawlThread = (CRAWLTHREAD*)iter->data;
					if(pCrawlThread)
					{
						if(pCrawlThread->nStatus!=WAIT_OBJECT_0)
						{
							pThread = pCrawlThread->pWinThread;
							//pThread = pCrawlThread->pThread;
							if(pThread)
								if(pThread->m_hThread!=INVALID_HANDLE_VALUE)
								//if(pThread->GetHandle()!=INVALID_HANDLE_VALUE)
								{
									if(::GetExitCodeThread(pThread->m_hThread,&dwStatus))
									//if(::GetExitCodeThread(pThread->GetHandle(),&dwStatus))
									{
										if(dwStatus==STILL_ACTIVE)
										{
											nActiveThreads++;
											
											if(m_nCancel!=0)
												pCrawlThread->httpConnection.Abort();
										}
									}
								}
						}
					}

				} while((iter = m_listActiveCrawlerThreads.Next(iter))!=(ITERATOR)NULL);
			}
			
			if(nActiveThreads!=0)
			{
				// interfaccia utente
				_snprintf(szBuffer,sizeof(szBuffer)-1,"Waiting for threads termination (%d still active)...",nActiveThreads);
				SetDlgItemText(IDC_STATIC_STATUS,szBuffer);	
				::Sleep(1000);
			}
			//else
			//	break;
		} while(nActiveThreads > 0);
	}

	// download terminato
	GetDlgItem(IDCANCEL)->EnableWindow(FALSE);

	// data/ora fine download
	strcpyn(m_szEndTime,datetime.GetFormattedDate(TRUE),sizeof(m_szEndTime));
	m_dlTotalTime = ::GetTickCount() - m_dlTotalTime;
	m_dlTotalTime = m_dlTotalTime / 1000.0f;

	// interfaccia utente
	SetWindowText(m_szTitle);
	//GetDlgItem(IDC_COMBO_URL)->EnableWindow(TRUE);
	//GetDlgItem(IDC_BUTTON_SCRIPTFILE)->EnableWindow(TRUE);
	SetDlgItemText(IDC_STATIC_STATUS,"Crawling process terminated...");
	SetDlgItemText(IDC_STATIC_ESTIMATED_TIME_LEFT,"");
	SetDlgItemText(IDC_STATIC_CURRENT_STATUS,"");
	SetDlgItemText(IDC_STATIC_TRANSFER_RATE,"");
	SetDlgItemText(IDC_ESTIMATED_TIME_LEFT,"");
	SetDlgItemText(IDC_CURRENT_STATUS,"");
	SetDlgItemText(IDC_TRANSFER_RATE,"");
	m_wndProgressBar.SetPos(0);
	m_wndProgressBar.Hide();
	m_wndAnimatedAvi.Stop();
	m_wndAnimatedAvi.Close();
	m_wndAnimatedAvi.ShowWindow(SW_HIDE);
	if(m_PictPreviewSync.Lock())
	{
		m_listPictPreview.RemoveAll();
		m_wndStaticDib.Unload();
		SetDlgItemText(IDC_TEXT_INFO,"");
		m_PictPreviewSync.Unlock();
	}

	UpdateStatusBar();
	SetDlgItemText(IDC_STATIC_TOTAL_THREADS,"");

	// interfaccia utente
	SetDlgItemText(IDC_STATIC_STATUS,"Updating database...");
	
	// aggiorna il database per i download con il risultato
	UrlDatabaseService.Update(	m_szUrl,
							m_szEndTime,
							m_dlTotalTime,
							m_dlDownloadTime,
							bConnected ? (m_nCancel!=0 ? CUrlStatus::URL_STATUS_CANCELLED : CUrlStatus::URL_STATUS_DONE) : (nDownloadCode <= (int)HTTP_STATUS_LEN ? (CUrlStatus::URL_STATUS)m_httpConnection.GetHttpError() : (CUrlStatus::URL_STATUS)m_httpConnection.GetSocketError()),
							nNextID,
							bConnected ? szHtmlReport : "",
							UrlService.GetTable());

	// interfaccia utente
	SetDlgItemText(IDC_STATIC_STATUS,"Updating configuration...");

	// aggiorna la lista delle inclusioni/esclusioni salvando nel registro i valori
	if(bConnected && m_nCancel==0 && m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_SAVEINCLEXCLLIST_KEY))
	{
		// interfaccia utente
		SetDlgItemText(IDC_STATIC_STATUS,"Updating inclusion/exclusion list(s)...");

		int n;
		BOOL bFound;
		ITERATOR iter;
		
		// inclusione domini
		if((iter = listDomainsIncludeSave.First())!=(ITERATOR)NULL)
		{
			n = 0;
			
			do
			{
				item = (ITEM*)iter->data;
				
				if(item)
				{
					bFound = FALSE;
					for(i = 0; i < nTotDomainIncludeKeys; i++)
					{
						_snprintf(szKey,sizeof(szKey)-1,"%s%d",WALLPAPER_DOMAININCLUDE_KEY,i);
						if(strcmp((p = (char*)m_pConfig->GetString(WALLPAPER_DOMAININCLUDELIST_KEY,szKey)),"")!=0)
							if(strcmp(p,item->item)==0)
							{
								bFound = TRUE;
								break;
							}
					}
					if(!bFound)
					{
						if(n==0)
							n = i;
						_snprintf(szKey,sizeof(szKey)-1,"%s%d",WALLPAPER_DOMAININCLUDE_KEY,n++);
						m_pConfig->Insert(WALLPAPER_DOMAININCLUDELIST_KEY,szKey,item->item);
					}
				}

				iter = listDomainsIncludeSave.Next(iter);
			
			} while(iter!=(ITERATOR)NULL);
			
			m_pConfig->SaveSection(WALLPAPER_DOMAININCLUDELIST_KEY);
		}

		// esclusione domini
		if((iter = listDomainsExcludeSave.First())!=(ITERATOR)NULL)
		{
			n = 0;
			
			do
			{
				item = (ITEM*)iter->data;
			
				if(item)
				{
					bFound = FALSE;
					for(i = 0; i < nTotDomainExcludeKeys; i++)
					{
						_snprintf(szKey,sizeof(szKey)-1,"%s%d",WALLPAPER_DOMAINEXCLUDE_KEY,i);
						if(strcmp((p = (char*)m_pConfig->GetString(WALLPAPER_DOMAINEXCLUDELIST_KEY,szKey)),"")!=0)
							if(strcmp(p,item->item)==0)
							{
								bFound = TRUE;
								break;
							}
					}
					if(!bFound)
					{
						if(n==0)
							n = i;
						_snprintf(szKey,sizeof(szKey)-1,"%s%d",WALLPAPER_DOMAINEXCLUDE_KEY,n++);
						m_pConfig->Insert(WALLPAPER_DOMAINEXCLUDELIST_KEY,szKey,item->item);
					}
				}

				iter = listDomainsExcludeSave.Next(iter);
			
			} while(iter!=(ITERATOR)NULL);
			
			m_pConfig->SaveSection(WALLPAPER_DOMAINEXCLUDELIST_KEY);
		}

		// inclusione directories padre
		if((iter = listParentsIncludeSave.First())!=(ITERATOR)NULL)
		{
			n = 0;
			
			do
			{
				item = (ITEM*)iter->data;
			
				if(item)
				{
					bFound = FALSE;
					for(i = 0; i < nTotParentIncludeKeys; i++)
					{
						_snprintf(szKey,sizeof(szKey)-1,"%s%d",WALLPAPER_PARENTINCLUDE_KEY,i);
						if(strcmp((p = (char*)m_pConfig->GetString(WALLPAPER_PARENTINCLUDELIST_KEY,szKey)),"")!=0)
							if(strcmp(p,item->item)==0)
							{
								bFound = TRUE;
								break;
							}
					}
					if(!bFound)
					{
						if(n==0)
							n = i;
						_snprintf(szKey,sizeof(szKey)-1,"%s%d",WALLPAPER_PARENTINCLUDE_KEY,n++);
						m_pConfig->Insert(WALLPAPER_PARENTINCLUDELIST_KEY,szKey,item->item);
					}
				}

				iter = listParentsIncludeSave.Next(iter);
			
			} while(iter!=(ITERATOR)NULL);
			
			m_pConfig->SaveSection(WALLPAPER_PARENTINCLUDELIST_KEY);
		}

		// esclusione directories padre
		if((iter = listParentsExcludeSave.First())!=(ITERATOR)NULL)
		{
			n = 0;
			
			do
			{
				item = (ITEM*)iter->data;
			
				if(item)
				{
					bFound = FALSE;
					for(i = 0; i < nTotParentExcludeKeys; i++)
					{
						_snprintf(szKey,sizeof(szKey)-1,"%s%d",WALLPAPER_PARENTEXCLUDE_KEY,i);
						if(strcmp((p = (char*)m_pConfig->GetString(WALLPAPER_PARENTEXCLUDELIST_KEY,szKey)),"")!=0)
							if(strcmp(p,item->item)==0)
							{
								bFound = TRUE;
								break;
							}
					}
					if(!bFound)
					{
						if(n==0)
							n = i;
						_snprintf(szKey,sizeof(szKey)-1,"%s%d",WALLPAPER_PARENTEXCLUDE_KEY,n++);
						m_pConfig->Insert(WALLPAPER_PARENTEXCLUDELIST_KEY,szKey,item->item);
					}
				}

				iter = listParentsExcludeSave.Next(iter);
			
			} while(iter!=(ITERATOR)NULL);
			
			m_pConfig->SaveSection(WALLPAPER_PARENTEXCLUDELIST_KEY);
		}
	}

	// generazione report html
	if(bConnected && m_nCancel==0)
		if(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_GENERATEREPORT_KEY))
			GenerateHtmlReport(szRequestedUrl,szHtmlReport,szAsciiReport,&UrlService,&UrlDatabaseService);
	if(fpAsciiReport)
		fclose(fpAsciiReport);

	// interfaccia utente
	SetDlgItemText(IDC_STATIC_STATUS,"");

	// apre il browser
	if((UrlDatabaseService.GetTable())->GetField_TotalPict() > 0)
		if(bConnected && m_nCancel==0 && m_szDownloadDir[0]!='\0')
		{
			int nRet = IDNO;
			BOOL bAsk = TRUE;

			if(m_bUseScript)
				if(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_QUIETINSCRIPTMODE_KEY))
					bAsk = FALSE;

			if(bAsk)
				nRet = DoNotAskMoreMessageBox(this->m_hWnd,IDS_QUESTION_LOAD_BROWSER,0,m_pConfig,WALLPAPER_DONOTASKMORE_LOADBROWSER_KEY,WALLPAPER_DONOTASKMORE_LOADBROWSER_VALUE_KEY,1);
			
			if(nRet==IDYES)
			{
				_snprintf(szBuffer,sizeof(szBuffer)-1,"%s\\%s.exe -d\"%s\"",m_pConfig->GetString(WALLPAPER_INSTALL_KEY,WALLPAPER_DIR_KEY),WALLBROWSER_PROGRAM_NAME,m_szDownloadDir);
				STARTUPINFO si = {0};
				si.cb = sizeof(STARTUPINFO);
				PROCESS_INFORMATION pi = {0};
				if(::CreateProcess(NULL,szBuffer,NULL,NULL,FALSE,0L,NULL,NULL,&si,&pi))
					::CloseHandle(pi.hProcess);
			}
		}

terminate:

	// download terminato
	m_bInCrawling = FALSE;
	m_oleDropTarget.Enable();
	CDialogEx::AllowDragAndDrop(TRUE);

	// elimina il file che stava scaricando se e' stato cancellato il download
//	if(m_nCancel!=0)
//		::DeleteFile(m_httpConnection.GetObject()->filename);
	
	// se deve usare il timeout interno, termina il thread relativo
	if(m_nConnectionTimeout > 0 && !m_bUseWinsockTimeout)
	{
		if(m_hTimeoutEvent!=(HANDLE)NULL)
		{
			::SetEvent(m_hTimeoutEvent);
			::Sleep(1000);
			::CloseHandle(m_hTimeoutEvent);
			m_hTimeoutEvent = (HANDLE)NULL;
		}
	}

	// interfaccia utente
	GetDlgItem(IDC_CHECK_CRAWLER_PREVIEW)->EnableWindow(m_bPicturePreview);

	_snprintf(m_szTitle,sizeof(m_szTitle)-1,"%s - %s",m_nCancel!=0 ? "cancelled" : "done",m_szUrl);
	SetWindowText(m_szTitle);
	SetDlgItemText(IDC_STATIC_STATUS,"");

	// reabilita i flags per l'uscita (trasforma cancel in close)
	GetDlgItem(IDC_COMBO_URL)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_SCRIPTFILE)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_CRAWLER_PREVIEW)->EnableWindow(m_bPicturePreview);
	GetDlgItem(IDOK)->EnableWindow(TRUE);
	SetDlgItemText(IDCANCEL,"&Close");
	m_nCancel = 0;
	
	GetDlgItem(IDCANCEL)->EnableWindow(TRUE);

	return(0L);
}

/*
	AudioRipperCallback()

	Callback per la dll per la cattura dello stream, aggiorna l'interfaccia utente.
*/
void AudioRipperCallback(int nMsg,void *pData)
{
	char szBuffer[512];
	SRINFO* srinfo = NULL;
	SRERRORINFO* srerrorinfo = NULL;

	switch(nMsg)
	{
		// inizializzazione
		case RM_INIT:
			TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"AudioRipperCallback(): RM_INIT\n"));
			break;

		// termine registrazione (non brano)
		case RM_DONE:
		{
			SROPTIONS* sroptions = (SROPTIONS*)pData;
			if(sroptions)
			{
				STREAMRIPPER_CALLBACK_DATA* streamripper_callback_data = (STREAMRIPPER_CALLBACK_DATA*)sroptions->pVoid;
				if(streamripper_callback_data)
				{
					TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"AudioRipperCallback(): RM_DONE (%s)\n",streamripper_callback_data->szFilenameOfCurrentSong));
					
					// aggiorna il database con i dati dell'ultimo brano
					CUrlService* pUrlService = streamripper_callback_data->pUrlService;
					if(pUrlService && streamripper_callback_data->szFilenameOfCurrentSong[0]!='\0')
					{
						char szUrl[MAX_URL+1];
						_snprintf(szUrl,sizeof(szUrl)-1,"%s/%s.mp3",sroptions->url,streamripper_callback_data->szFilenameOfCurrentSong);

						// non e' detto che il brano sia stato registrato correttamente (potrebbe trovarsi in 'incomplete')
						char szFullPathName[_MAX_PATH+1];
						_snprintf(szFullPathName,sizeof(szFullPathName)-1,"%s%s.mp3",sroptions->output_directory,streamripper_callback_data->szFilenameOfCurrentSong);
						CUrlStatus::URL_STATUS urlStat = CUrlStatus::URL_STATUS_DOWNLOADED;
						QWORD qwFileSize = ::GetFileSizeExt(szFullPathName);
						if(qwFileSize==0L)
						{	
							_snprintf(szFullPathName,sizeof(szFullPathName)-1,"%s%sincomplete\\%s.mp3",sroptions->output_directory,sroptions->output_directory[strlen(sroptions->output_directory)-1]=='\\' ? "" : "\\",streamripper_callback_data->szFilenameOfCurrentSong);
							qwFileSize = ::GetFileSizeExt(szFullPathName);
							urlStat = CUrlStatus::URL_STATUS_INCOMPLETE;
						}
						
						TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"AudioRipperCallback(): RM_DONE %s: %s\n",urlStat==CUrlStatus::URL_STATUS_DOWNLOADED ? "downloaded" : "incomplete",szFullPathName));
						pUrlService->Update(szUrl,
										urlStat,
										szFullPathName,
										(double)qwFileSize,
										(::GetTickCount() / 1000L) - streamripper_callback_data->nDownloadTimeOfCurrentSong);

						// aggiunge l'ultimo brano (valido) alla playlist corrente
						if(streamripper_callback_data->bAddSongsToPlaylist && urlStat!=CUrlStatus::URL_STATUS_INCOMPLETE)
						{
							CWallPaperCrawlerDlg* pThis = (CWallPaperCrawlerDlg*)streamripper_callback_data->pThis;
							if(pThis)
							{
								CWnd* pWnd = pThis->GetDlgItem(IDC_STATIC_STATUS);
								if(pWnd)
								{
									_snprintf(szBuffer,sizeof(szBuffer)-1,"Adding %s.mp3 to the playlist...",streamripper_callback_data->szFilenameOfCurrentSong);
									pWnd->SetWindowText(szBuffer);
								}
								if(pThis->m_hWndParent)
								{
									::SendMessage(pThis->m_hWndParent,WM_PLAYLIST_ADDFILE,AUDIOPLAYER_COMMAND_QUEUE_FROM_FILE,(LONG)szFullPathName);
									TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"AudioRipperCallback(): RM_DONE adding %s to the playlist\n",szFullPathName));
								}
							}
						}
					}
				}
			}
			break;
		}

		// inizio registrazione
		case RM_STARTED:
		{
			TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"AudioRipperCallback(): RM_STARTED\n"));
			
			// struct per le info sulla registrazione
			srinfo = (SRINFO*)pData;
			if(srinfo && srinfo->options)
			{
				// struct per la callback (contiene il ptr al chiamante)
				STREAMRIPPER_CALLBACK_DATA* streamripper_callback_data = (STREAMRIPPER_CALLBACK_DATA*)srinfo->options->pVoid;
				if(streamripper_callback_data)
				{
					// ptr alla classe
					CWallPaperCrawlerDlg* pThis = (CWallPaperCrawlerDlg*)streamripper_callback_data->pThis;
					if(pThis)
					{
						pThis->m_wndAnimatedAvi.ShowWindow(SW_SHOW);
						pThis->m_wndAnimatedAvi.Open(IDR_DOWNLOAD_AVI);
						pThis->m_wndAnimatedAvi.Play(0,(UINT)-1,(UINT)-1);
					}
				}
			}
			break;
		}

		// nuovo brano
		case RM_NEW_TRACK:
		{
			// struct per le info sulla registrazione
			srinfo = (SRINFO*)pData;
			if(srinfo && srinfo->options)
			{
				// normalizza il nome della traccia per usarlo per il nome del file
				TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"AudioRipperCallback(): RM_NEW_TRACK %s\n",srinfo->filename));
				{
				char filename[MAX_TRACKNAME_LEN] = {0};
				CFindFile f;
				f.EnsureValidFileName(srinfo->filename,filename,sizeof(filename));
				strcpyn(srinfo->filename,filename,MAX_TRACKNAME_LEN);
				}
				TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"AudioRipperCallback(): RM_NEW_TRACK normalized: [%s]\n",srinfo->filename));
				
				// struct per la callback (contiene il ptr al chiamante)
				STREAMRIPPER_CALLBACK_DATA* streamripper_callback_data = (STREAMRIPPER_CALLBACK_DATA*)srinfo->options->pVoid;
				if(streamripper_callback_data)
				{
					// ptr alla classe
					CWallPaperCrawlerDlg* pThis = (CWallPaperCrawlerDlg*)streamripper_callback_data->pThis;
					if(pThis)
					{
						// nuovo brano
						if(srinfo->filename[0]!='\0')
						{
							CWnd* pWnd;

							// inizializza la barra di avanzamento (1 ciclo per ogni MB)
							pThis->m_wndProgressBar.SetRange(0,100);
							pThis->m_wndProgressBar.SetStep(1);
							pThis->m_wndProgressBar.SetPos(0);
							pThis->m_wndProgressBar.Show();

							// aggiorna il database con i dati del brano precedente, l'inserimento
							// del nuovo viene effettuato con la ricezione del messaggio RM_UPDATE
							CUrlService* pUrlService = streamripper_callback_data->pUrlService;
							if(pUrlService && streamripper_callback_data->szFilenameOfCurrentSong[0]!='\0')
							{
								char szUrl[MAX_URL+1];
								_snprintf(szUrl,sizeof(szUrl)-1,"%s/%s.mp3",srinfo->options->url,streamripper_callback_data->szFilenameOfCurrentSong);
								
								// non e' detto che il brano sia stato registrato correttamente (potrebbe trovarsi in 'incomplete')
								char szFullPathName[_MAX_PATH+1];
								_snprintf(szFullPathName,sizeof(szFullPathName)-1,"%s%s.mp3",srinfo->options->output_directory,streamripper_callback_data->szFilenameOfCurrentSong);
								CUrlStatus::URL_STATUS urlStat = CUrlStatus::URL_STATUS_DOWNLOADED;
								QWORD qwFileSize = ::GetFileSizeExt(szFullPathName);
								if(qwFileSize==0L)
								{	
									_snprintf(szFullPathName,sizeof(szFullPathName)-1,"%s%sincomplete\\%s.mp3",srinfo->options->output_directory,srinfo->options->output_directory[strlen(srinfo->options->output_directory)-1]=='\\' ? "" : "\\",streamripper_callback_data->szFilenameOfCurrentSong);
									qwFileSize = ::GetFileSizeExt(szFullPathName);
									urlStat = CUrlStatus::URL_STATUS_INCOMPLETE;
								}

								{CId3Lib id3Lib;
								if(id3Lib.Link(szFullPathName))
								{
									id3Lib.SetGenre(streamripper_callback_data->pIcyData->genre);
									id3Lib.SetComment(streamripper_callback_data->pIcyData->station);
									id3Lib.Update();
									id3Lib.Unlink();
								}}
								
								TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"AudioRipperCallback(): RM_NEW_TRACK %s: %s\n",urlStat==CUrlStatus::URL_STATUS_DOWNLOADED ? "downloaded" : "incomplete",szFullPathName));
								pUrlService->Update(szUrl,
												urlStat,
												szFullPathName,
												(double)qwFileSize,
												(::GetTickCount() / 1000L) - streamripper_callback_data->nDownloadTimeOfCurrentSong);
								
								// totale brani registrati fino al momento
								streamripper_callback_data->nRippedSongsCount++;
								
								// azzera l'amount per la barra di progresso
								streamripper_callback_data->nRippedAmount = 0L;
								
								// aggiorna i controlli del dialogo
/*								pWnd = pThis->GetDlgItem(IDC_STATIC_CURRENT_STATUS);
								if(pWnd)
									pWnd->SetWindowText("Bitrate: ");

								pWnd = pThis->GetDlgItem(IDC_CURRENT_STATUS);
								if(pWnd)
								{
									_snprintf(szBuffer,sizeof(szBuffer)-1,"%d kbps",streamripper_callback_data->pIcyData->bitrate);
									pWnd->SetWindowText(szBuffer);
								}

								pWnd = pThis->GetDlgItem(IDC_STATIC_TRANSFER_RATE);
								if(pWnd)
									pWnd->SetWindowText("Genre: ");
								pWnd = pThis->GetDlgItem(IDC_TRANSFER_RATE);
								if(pWnd)
								{
									_snprintf(szBuffer,sizeof(szBuffer)-1,"%s",streamripper_callback_data->pIcyData->genre);
									pWnd->SetWindowText(szBuffer);
								}
*/
								pWnd = pThis->GetDlgItem(IDC_STATIC_TOTAL_URLS);
								if(pWnd)
								{
									_snprintf(szBuffer,sizeof(szBuffer)-1,"(%ld song%s ripped)",streamripper_callback_data->nRippedSongsCount,streamripper_callback_data->nRippedSongsCount > 1 ? "s" : "");
									pWnd->SetWindowText(szBuffer);
									strcpyn(szBuffer,strnull(streamripper_callback_data->pIcyData->station) ? srinfo->streamname : streamripper_callback_data->pIcyData->station,sizeof(szBuffer));
									pThis->SetWindowText(szBuffer);
								}
								
								// aggiunge il brano precedente (quello appena registrato) alla playlist corrente solo se e' un brano valido (non risiede in incomplete)
								if(streamripper_callback_data->bAddSongsToPlaylist && urlStat!=CUrlStatus::URL_STATUS_INCOMPLETE)
								{
									pWnd = pThis->GetDlgItem(IDC_STATIC_STATUS);
									if(pWnd)
									{
										_snprintf(szBuffer,sizeof(szBuffer)-1,"Adding %s.mp3 to the playlist...",streamripper_callback_data->szFilenameOfCurrentSong);
										pWnd->SetWindowText(szBuffer);
									}
									if(pThis->m_hWndParent)
									{
										::SendMessage(pThis->m_hWndParent,WM_PLAYLIST_ADDFILE,AUDIOPLAYER_COMMAND_QUEUE_FROM_FILE,(LONG)szFullPathName);
										TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"AudioRipperCallback(): RM_NEW_TRACK adding %s to the playlist\n",szFullPathName));
									}
								}
							}
							
							strcpyn(streamripper_callback_data->szFilenameOfCurrentSong,srinfo->filename,_MAX_PATH+1);
							
							if(pUrlService)
							{
								char szUrl[_MAX_PATH+1];
								_snprintf(szUrl,sizeof(szUrl)-1,"%s/%s.mp3",srinfo->options->url,streamripper_callback_data->szFilenameOfCurrentSong);

								// i brani vengono registrati in 'incomplete' e poi passati sulla dir padre
								char szFullPathName[_MAX_PATH+1];
								_snprintf(szFullPathName,sizeof(szFullPathName)-1,"%s%sincomplete\\%s.mp3",srinfo->options->output_directory,srinfo->options->output_directory[strlen(srinfo->options->output_directory)-1]=='\\' ? "" : "\\",streamripper_callback_data->szFilenameOfCurrentSong);
								streamripper_callback_data->nDownloadTimeOfCurrentSong = ::GetTickCount() / 1000L;
								pUrlService->Insert(streamripper_callback_data->nID,srinfo->streamname,szUrl,CUrlStatus::URL_STATUS_INCOMPLETE,szFullPathName);
								
								// registrazione nuovo brano
								pWnd = pThis->GetDlgItem(IDC_STATIC_STATUS);
								if(pWnd)
								{
									_snprintf(szBuffer,sizeof(szBuffer)-1,"Ripping: %s.mp3\r\nFrom: %s - %s",streamripper_callback_data->szFilenameOfCurrentSong,srinfo->streamname,srinfo->streamdesc);
									pWnd->SetWindowText(szBuffer);
								}
							}
						}
						
						// aggiornamento brano corrente
						CWnd* pWnd = pThis->GetDlgItem(IDC_STATIC_STATUS);
						if(pWnd)
						{
							_snprintf(szBuffer,sizeof(szBuffer)-1,"Ripping: %s.mp3\r\nFrom: %s - %s",srinfo->filename,srinfo->streamname,srinfo->streamdesc);
							pWnd->SetWindowText(szBuffer);
						}
							// aggiorna i controlli del dialogo
							pWnd = pThis->GetDlgItem(IDC_STATIC_CURRENT_STATUS);
							if(pWnd)
								pWnd->SetWindowText("Bitrate: ");

							pWnd = pThis->GetDlgItem(IDC_CURRENT_STATUS);
							if(pWnd)
							{
								_snprintf(szBuffer,sizeof(szBuffer)-1,"%d kbps",streamripper_callback_data->pIcyData->bitrate);
								pWnd->SetWindowText(szBuffer);
							}

							pWnd = pThis->GetDlgItem(IDC_STATIC_TRANSFER_RATE);
							if(pWnd)
								pWnd->SetWindowText("Genre: ");
							pWnd = pThis->GetDlgItem(IDC_TRANSFER_RATE);
							if(pWnd)
							{
								_snprintf(szBuffer,sizeof(szBuffer)-1,"%s",streamripper_callback_data->pIcyData->genre);
								pWnd->SetWindowText(szBuffer);
							}

					}
				}
			}

			break;
		}
		
		// aggiornamento brano corrente
		case RM_UPDATE:
		{
			// struct per le info sulla registrazione
			srinfo = (SRINFO*)pData;
			if(srinfo && srinfo->options)
			{
				// struct per la callback (contiene il ptr al chiamante)
				STREAMRIPPER_CALLBACK_DATA* streamripper_callback_data = (STREAMRIPPER_CALLBACK_DATA*)srinfo->options->pVoid;
				if(streamripper_callback_data)
				{	
					// se supera il limite (in bytes) assume che sia uno stream infinito per cui lo aggiunge al volo alla playlist
					// occhio ad abbassare tale limite, si corre il rischio di aggiungere i brani prima che vengano completati
					CWallPaperCrawlerDlg* pThis = (CWallPaperCrawlerDlg*)streamripper_callback_data->pThis;
					if(pThis)
						if(	pThis->m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_STREAMRIPPER_USTREAMSLIMIT_KEY)!=0 && 
							srinfo->filesize > pThis->m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_STREAMRIPPER_USTREAMSLIMIT_KEY) && 
							streamripper_callback_data->bAddSongsToPlaylist && 
							!streamripper_callback_data->bAddedToPlaylist
							)
						{
							if(pThis->m_hWndParent)
							{
								// il brano deve trovarsi in 'incomplete'
								char szFullPathName[_MAX_PATH+1];
								_snprintf(szFullPathName,sizeof(szFullPathName)-1,"%s%sincomplete\\%s.mp3",srinfo->options->output_directory,srinfo->options->output_directory[strlen(srinfo->options->output_directory)-1]=='\\' ? "" : "\\",srinfo->filename/*streamripper_callback_data->szFilenameOfCurrentSong*/);
								::SendMessage(pThis->m_hWndParent,WM_PLAYLIST_ADDFILE,AUDIOPLAYER_COMMAND_QUEUE_FROM_FILE,(LONG)szFullPathName);
								streamripper_callback_data->bAddedToPlaylist = TRUE;
								TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"AudioRipperCallback(): RM_UPDATE ustream=%s, size exceeds %ld limit, added to the playlist\n",szFullPathName,pThis->m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_STREAMRIPPER_USTREAMSLIMIT_KEY)));
							}
						}
					
					if(pThis)
					{
						CWnd* pWnd;
						double dlProgress = 0.0f;

						// aggiorna i controlli del dialogo
						pWnd = pThis->GetDlgItem(IDC_STATIC_ESTIMATED_TIME_LEFT);
						if(pWnd)
							pWnd->SetWindowText("Current download: ");
						
						pWnd = pThis->GetDlgItem(IDC_ESTIMATED_TIME_LEFT);
						if(pWnd)
						{
							dlProgress = FDIV((100.0f * (srinfo->filesize - streamripper_callback_data->nRippedAmount)),1048576.0f);
							if(dlProgress > 100.0f)
							{
								dlProgress = 0.0f;
								streamripper_callback_data->nRippedAmount += 1048576L;
							}
							char szDownloaded[16];
							_snprintf(szBuffer,sizeof(szBuffer)-1,"%s",strsize(szDownloaded,sizeof(szDownloaded),srinfo->filesize));
							pWnd->SetWindowText(szBuffer);
						}

/*						pWnd = pThis->GetDlgItem(IDC_STATIC_CURRENT_STATUS);
						if(pWnd)
							pWnd->SetWindowText("Bitrate: ");

						pWnd = pThis->GetDlgItem(IDC_CURRENT_STATUS);
						if(pWnd)
						{
							_snprintf(szBuffer,sizeof(szBuffer)-1,"%d kbps",streamripper_callback_data->pIcyData->bitrate);
							pWnd->SetWindowText(szBuffer);
						}

						pWnd = pThis->GetDlgItem(IDC_STATIC_TRANSFER_RATE);
						if(pWnd)
							pWnd->SetWindowText("Genre: ");
						pWnd = pThis->GetDlgItem(IDC_TRANSFER_RATE);
						if(pWnd)
						{
							_snprintf(szBuffer,sizeof(szBuffer)-1,"%s",streamripper_callback_data->pIcyData->genre);
							pWnd->SetWindowText(szBuffer);
						}
*/
						if(!streamripper_callback_data->bProgressBarShowed)
						{
							streamripper_callback_data->bProgressBarShowed = TRUE;
							pThis->m_wndProgressBar.SetRange(0,100);
							pThis->m_wndProgressBar.SetStep(1);
							pThis->m_wndProgressBar.SetPos(0);
							pThis->m_wndProgressBar.Show();
						}
						
						pThis->m_wndProgressBar.SetPos((int)dlProgress);
					}
				}
			}
			
			break;
		}

		case RM_TRACK_DONE:
		{
			TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"AudioRipperCallback(): RM_TRACK_DONE\n"));
			break;
		}
		
		// errore
		case RM_ERROR:
		{
			TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"AudioRipperCallback(): RM_ERROR\n"));
			
			srerrorinfo = (SRERRORINFO*)pData;
			if(srerrorinfo)
			{
				CWallPaperCrawlerDlg* pThis = NULL;

				//$ per evitare la gpf, ma rimane incastrato, bisogna spostare il pThis per renderlo accessibile comunque...
				if(srerrorinfo->pInfo->options)
				{
					STREAMRIPPER_CALLBACK_DATA* streamripper_callback_data = (STREAMRIPPER_CALLBACK_DATA*)srerrorinfo->pInfo->options->pVoid;
					if(streamripper_callback_data)
					{
						pThis = (CWallPaperCrawlerDlg*)streamripper_callback_data->pThis;
						if(pThis)
						{
							CWnd* pWnd = pThis->GetDlgItem(IDC_STATIC_STATUS);
							if(pWnd)
							{
								_snprintf(szBuffer,sizeof(szBuffer)-1,"Error %d: %s",srerrorinfo->error_code,srerrorinfo->error_str);
								pWnd->SetWindowText(szBuffer);
							}
							
							// termina la registrazione
							if(pThis->m_hStreamRipperEvent)
								::SetEvent(pThis->m_hStreamRipperEvent);
						}
					}
				}
			}
			break;
		}
	}
}

/*
	AudioCrawler()

	Wrapper per il thread per la cattura dello stream audio.
*/
UINT CWallPaperCrawlerDlg::AudioCrawler(LPVOID lpVoid)
{
	UINT nRet = (UINT)-1L;

	CWallPaperCrawlerDlg* pThis = (CWallPaperCrawlerDlg*)lpVoid;
	if(pThis)
	{
		HWND hWnd = pThis->m_hWnd;
		nRet = pThis->AudioCrawler();
		if(hWnd && ::IsWindow(hWnd))
			::PostMessage(hWnd,WM_CRAWLER_DONE,0,0L);
	}

	return(nRet);
}

/*
	AudioCrawler()

	Thread per la cattura dello stream audio.
*/
UINT CWallPaperCrawlerDlg::AudioCrawler(void)
{
	int i;
	char szBuffer[1024];
	ICYDATA icydata = {0};
	CUrl url;
	URL Url;
	CONTENTTYPE ctype;
	DWORD dwIcyErrorCode = 0L;
	char szIcyErrorCode[256] = {0};
	char szLocalFile[_MAX_PATH+1];
	BOOL bHaveEntries = FALSE;
	char szDownloadPath[_MAX_PATH+1];
	int nDownloadCode = 0;
	BOOL bConnected = FALSE;
	CDateTime datetime(GMT);
	CFindFile findFile;
	CWindowsVersion winver;
	
	// inizio crawling
	m_bInCrawling = TRUE;
	m_oleDropTarget.Disable();
	CDialogEx::AllowDragAndDrop(FALSE);
	
	// interfaccia utente
	strcpyn(m_szTitle,m_szUrl,sizeof(m_szTitle));
	SetWindowText(m_szTitle);
	SetDlgItemText(IDC_STATIC_STATUS,"Initializing...");
	GetDlgItem(IDC_COMBO_URL)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SCRIPTFILE)->EnableWindow(FALSE);
	SetDlgItemText(IDC_STATIC_ESTIMATED_TIME_LEFT,"");
	SetDlgItemText(IDC_ESTIMATED_TIME_LEFT,"");
	SetDlgItemText(IDC_STATIC_CURRENT_STATUS,"");
	SetDlgItemText(IDC_CURRENT_STATUS,"");
	SetDlgItemText(IDC_STATIC_TRANSFER_RATE,"");
	SetDlgItemText(IDC_TRANSFER_RATE,"");
	m_bPicturePreview = FALSE;
	GetDlgItem(IDC_CHECK_CRAWLER_PREVIEW)->EnableWindow(m_bPicturePreview);
	UpdateStatusBar();

	// al momento una sola istanza per volta (la dll suca un poco la minchia...)
	if(m_StreamRipperSync.Lock())
	{
		// se non specificato in configurazione, chiede se aggiungere i downloads alla playlist
		if(!m_bAddFilesToPlaylist)
			m_bAddSongsToPlaylist = DoNotAskMoreMessageBox(this->m_hWnd,IDS_QUESTION_PLAY_RIPPED_SONGS,DEFAULT_DONOTASKMORE_TIMEOUT,m_pConfig,WALLPAPER_DONOTASKMORE_PLAYRIPPEDSONGS_KEY,WALLPAPER_DONOTASKMORE_PLAYRIPPEDSONGS_VALUE_KEY)==IDYES;
		else
			m_bAddSongsToPlaylist = TRUE;

		// data/ora inizio download
		strcpyn(m_szStartTime,datetime.GetFormattedDate(TRUE),sizeof(m_szStartTime));
		m_dlTotalTime = ::GetTickCount();
		m_dlDownloadTime = 0.0f;

		// divide l'url in nome host, numero porta, directory e nome file
		// se non viene specificata la porta, assume quella di default (80)
		url.SplitUrl(m_szUrl,&Url);
		if(Url.port <= 0)
			Url.port = HTTP_DEFAULT_PORT;
		
		// crea il pathname locale relativo all'url
		url.CreatePathNameFromHttpUrl(m_szUrl,szDownloadPath,sizeof(szDownloadPath),m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_DOWNLOADDIR_KEY));
		strcpyn(m_szDownloadDir,szDownloadPath,sizeof(m_szDownloadDir));
		strstrim(m_szDownloadDir);

		// costruisce il nome per il report html
		char szHtmlReport[_MAX_PATH+1];
		_snprintf(szHtmlReport,sizeof(szHtmlReport)-1,"%sreport.",m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_REPORTSDIR_KEY));
		strcatn(szHtmlReport,url.IsUrl(Url.host) ? url.StripUrlType(Url.host) : Url.host,sizeof(szHtmlReport));
		i = strlen(szHtmlReport);
		_snprintf(szHtmlReport+i,(sizeof(szHtmlReport)-1)-i,"_%ld",Url.port);
		if(Url.dir[0])
			strcatn(szHtmlReport,Url.dir,sizeof(szHtmlReport));
		if(Url.file[0])
			strcatn(szHtmlReport,Url.file,sizeof(szHtmlReport));
		url.EnsureValidFileName(szHtmlReport,sizeof(szHtmlReport),FALSE,DEFAULT_HTML_EXT);

		// il report ascii al momento non viene generato

		// interfaccia utente
		SetDlgItemText(IDC_STATIC_STATUS,"Synchronizing the local database...");
		
		// costruisce i nomi per la base dati
		char szDataTableName[_MAX_PATH+1];
		strcpy(szDataTableName,URL_TABLE".");
		strcatn(szDataTableName,url.IsUrl(Url.host) ? url.StripUrlType(Url.host) : Url.host,sizeof(szDataTableName));
		i = strlen(szDataTableName);
		_snprintf(szDataTableName+i,(sizeof(szDataTableName)-1)-i,"_%ld",Url.port);
		if(Url.dir[0])
			strcatn(szDataTableName,Url.dir,sizeof(szDataTableName));
		if(Url.file[0])
			strcatn(szDataTableName,Url.file,sizeof(szDataTableName));
		url.EnsureValidFileName(szDataTableName,sizeof(szDataTableName),FALSE);

		// apre i database
		CUrlService UrlService(szDataTableName,m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_DATADIR_KEY));
		CUrlDatabaseService	UrlDatabaseService(URLDATABASE_TABLE,m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_DATADIR_KEY));
		if(!UrlService.IsValid() || !UrlDatabaseService.IsValid())
		{
			m_StreamRipperSync.Unlock();
			::MessageBoxResourceEx(this->m_hWnd,MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_TABLE_LOCK,szDataTableName);
			goto terminate;
		}

		UrlService.DeleteAll();
		UrlDatabaseService.Insert(m_szUrl,m_szStartTime,szDataTableName);

		// interfaccia utente
		if(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_USEPROXY_KEY))
			_snprintf(szBuffer,sizeof(szBuffer)-1,"Connecting to (%s:%d): %s...",m_httpConnection.GetProxyAddress(),m_httpConnection.GetProxyPortNumber(),Url.host);
		else
			_snprintf(szBuffer,sizeof(szBuffer)-1,"Connecting to: %s:%d...",Url.host,Url.port);
		SetDlgItemText(IDC_STATIC_STATUS,szBuffer);

		// azzera la lista per le entrate della playlist dello stream
		m_listPls.DeleteAll();

		// imposta lo useragent
		char szUserAgent[HTTP_MAX_USERAGENT+1];
		_snprintf(szUserAgent,sizeof(szUserAgent)-1,WALLPAPER_USER_AGENT,winver.GetVersionString());

		// l'url puo' essere http (deve specificare una playlist) o icy (deve specificare ip+porta del server per lo stream)
		// l'url http puo' essere letta dal file info locale (il .inf generato dal ripper) o specificata a mano
		// l'url icy puo' essere letta dal file playlist locale (il .pls|.m3u) o specificata a mano
		if(url.IsUrlType(m_szUrl,HTTP_URL))
		{
			// imposta i defaults per la connessione
			m_httpConnection.Reset();
			m_httpConnection.SetDelay(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_WINSOCKDELAY_KEY));
			m_httpConnection.SetSkipExisting(FALSE);
			m_httpConnection.SetForceSkipExisting(FALSE);
			m_httpConnection.SetDoNotSkipExisting(TRUE);
			m_httpConnection.AddHeader("Pragma","no-cache");
			// Cache-Control e' specifico per HTTP/1.1, non tutti i server lo gestiscono con una richiesta HTTP/1.0
			//m_httpConnection.AddHeader("Cache-Control","no-cache");
			m_httpConnection.SetConnectionRetry(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_CONNECTIONRETRY_KEY));
			if(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_CONNECTIONTIMEOUT_KEY) > 0 && m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_USEWINSOCKTIMEOUT_KEY))
				m_httpConnection.SetConnectionTimeout(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_CONNECTIONTIMEOUT_KEY));
			m_httpConnection.AllowCookies(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_COOKIES_ALLOWED_KEY));
			m_httpConnection.SetCookieFileName(m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_COOKIES_FILE_KEY));
			if(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_USEPROXY_KEY))
			{
				m_httpConnection.SetProxyAddress(m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_PROXY_ADDRESS_KEY));
				m_httpConnection.SetProxyPortNumber(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PROXY_PORTNUMBER_KEY));
				if(strcmp(m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_PROXY_USER_KEY),"")!=0 && strcmp(m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_PROXY_PASSWORD_KEY),"")!=0)
				{
					m_httpConnection.SetProxyUser(m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_PROXY_USER_KEY));
					m_httpConnection.SetProxyPassword(m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_PROXY_PASSWORD_KEY));
					CBase64 base64;
					char szEncoded[MAX_USER_LEN+1+MAX_PASS_LEN+1];
					_snprintf(szEncoded,sizeof(szEncoded)-1,"%s:%s",m_httpConnection.GetProxyUser(),m_httpConnection.GetProxyPassword());
					char* pAuth = base64.Encode(szEncoded);
					m_httpConnection.SetProxyAuth(pAuth);
					delete [] pAuth;
				}
				if(strcmp(m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_PROXY_EXCLUDEDOMAINS_KEY),"")!=0)
				{
					CHostNameList* pProxyExcludeList = m_httpConnection.GetProxyExcludeList();
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
			m_httpConnection.SetPortNumber(Url.port);
			m_httpConnection.AddHeader("User-Agent",szUserAgent);
		}

		// crea il pathname locale relativo all'url
		// l'url e' quella del sito su cui si trova la playlist (.pls|.m3u), non l'url di cui sotto utilizzata per lo stream
		url.CreatePathNameFromHttpUrl(m_szUrl,szDownloadPath,sizeof(szDownloadPath),m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_DOWNLOADDIR_KEY));
		
		// interfaccia utente
		_snprintf(szBuffer,sizeof(szBuffer)-1,"Loading playlist from %s:%ld...",Url.host,Url.port);
		SetDlgItemText(IDC_STATIC_STATUS,szBuffer);

		// scarica il file per la playlist (.pls|.m3u)
		if(strcmp(Url.file,"")==0)
			strcpyn(szLocalFile,Url.cgi,sizeof(szLocalFile));
		else
			strcpyn(szLocalFile,Url.file,sizeof(szLocalFile));
		
		// url http
		if(url.IsUrlType(m_szUrl,HTTP_URL))
		{
			if(url.GetHttpUrl(&m_httpConnection,m_szUrl,NULL,NULL,0,szDownloadPath,szLocalFile,sizeof(szLocalFile),&ctype))
			{
				if(IS_HTTP_ERROR(m_httpConnection.GetHttpError()) || m_httpConnection.GetSocketError()!=0)
				{
					::MessageBoxResourceEx(this->m_hWnd,MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_DOWNLOAD,m_szUrl,m_httpConnection.GetErrorString());
					bHaveEntries = FALSE;
				}
				else
				{
					// carica le entrate nella lista interna
					char szPlsFile[_MAX_PATH+1];
					_snprintf(szPlsFile,sizeof(szPlsFile)-1,"%s",szLocalFile);

					if(striright(szPlsFile,".pls")==0)
					{
						int nNumberOfEntries = ::GetPrivateProfileInt("playlist","numberofentries",0,szPlsFile);
						if(nNumberOfEntries > 0)
						{
							char szFilen[_MAX_PATH+1];
							char szTitlen[_MAX_PATH+1];
							for(int i = 1; i <= nNumberOfEntries; i++)
							{
								PLS* pls = (PLS*)m_listPls.Add();
								if(pls)
								{
									_snprintf(szFilen,sizeof(szFilen)-1,"File%d",i);
									if(::GetPrivateProfileString("playlist",szFilen,"",pls->host,MAX_URL,szPlsFile) > 0)
									{
										bHaveEntries = TRUE;
										
										// host, port, dir, file
										URL Url;
										CUrl url;
										url.SplitUrl(pls->host,&Url);
										strcpyn(pls->host,Url.host,MAX_URL+1);
										if(Url.port <= 0)
											Url.port = HTTP_DEFAULT_PORT;
										pls->port = Url.port;
										strcpyn(pls->dir,Url.dir,MAX_URL+1);
										strcpyn(pls->file,Url.file,MAX_URL+1);

										// title
										_snprintf(szTitlen,sizeof(szTitlen)-1,"Title%d",i);
										::GetPrivateProfileString("playlist",szTitlen,"",pls->title,_MAX_PATH,szPlsFile);
										
										// entry #
										pls->entry = i;
									}
								}
							}
						}
					}
					else if(striright(szPlsFile,".m3u")==0)
					{
						CTextFile m3uFile;
						if(m3uFile.Open(szPlsFile,FALSE))
						{
							int i = 0;
							char szBuffer[512];
							while(m3uFile.ReadLine(szBuffer,sizeof(szBuffer)-1)!=FILE_EOF)
							{
								if(url.IsUrlType(szBuffer,HTTP_URL))
								{
									PLS* pls = (PLS*)m_listPls.Add();
									if(pls)
									{
										bHaveEntries = TRUE;
										
										// host, port, dir, file
										strcpyn(pls->host,szBuffer,MAX_URL);										
										URL Url;
										CUrl url;
										url.SplitUrl(pls->host,&Url);
										strcpyn(pls->host,Url.host,MAX_URL+1);
										if(Url.port <= 0)
											Url.port = HTTP_DEFAULT_PORT;
										pls->port = Url.port;
										strcpyn(pls->dir,Url.dir,MAX_URL+1);
										strcpyn(pls->file,Url.file,MAX_URL+1);

										// title
										strcpyn(pls->title,szBuffer,_MAX_PATH);
										
										// entry #
										pls->entry = ++i;
									}
								}
							}

							m3uFile.Close();
						}
					}
				}
			}
		}
		else // url icy
		{
			PLS* pls = (PLS*)m_listPls.Add();
			if(pls)
			{
				memset(pls,'\0',sizeof(PLS));
				strcpyn(pls->host,m_szUrl,MAX_URL);
				
				// host, port, dir, file
				URL Url;
				CUrl url;
				url.SplitUrl(pls->host,&Url);
				strcpyn(pls->host,Url.host,MAX_URL+1);
				if(Url.port <= 0)
					Url.port = HTTP_DEFAULT_PORT;
				pls->port = Url.port;
				strcpyn(pls->dir,Url.dir,MAX_URL+1);
				strcpyn(pls->file,Url.file,MAX_URL+1);

				// entry #
				pls->entry = 1;
				
				bHaveEntries = TRUE;
			}
		}

		// se nella playlist (.pls|.m3u) esistono entrate (urls)
		if(bHaveEntries)
		{
			// interfaccia utente
			_snprintf(szBuffer,sizeof(szBuffer)-1,"Loading site icon from %s:%ld...",Url.host,Url.port);
			SetDlgItemText(IDC_STATIC_STATUS,szBuffer);

			// scarica (se necessario) l'icona per i favoriti per impostarla sul dialogo
			SetIcon(IDI_ICON_CRAWLER);
			_snprintf(szLocalFile,sizeof(szLocalFile)-1,"%sfavicon.ico",szDownloadPath);
			if(!::FileExist(szLocalFile))
			{
				char szFavIconUrl[MAX_URL+1];
				_snprintf(szFavIconUrl,sizeof(szFavIconUrl)-1,"%s:%ld/favicon.ico",Url.host,Url.port);
				url.GetHttpUrl(&m_httpConnection,szFavIconUrl,NULL,NULL,0,szDownloadPath,szLocalFile,sizeof(szLocalFile),&ctype);
			}
			if(::FileExist(szLocalFile))
				SetIcon(szLocalFile);

			// struct per i dati utilizzati dalla callback chiamata dal ripper
			STREAMRIPPER_CALLBACK_DATA* streamripper_callback_data = new STREAMRIPPER_CALLBACK_DATA;
			memset(streamripper_callback_data,'\0',sizeof(STREAMRIPPER_CALLBACK_DATA));
			streamripper_callback_data->pUrlService = &UrlService;
			streamripper_callback_data->pUrlDatabaseService = &UrlDatabaseService;
			streamripper_callback_data->pThis = (LPVOID)this;
			streamripper_callback_data->bAddSongsToPlaylist = m_bAddSongsToPlaylist;
			streamripper_callback_data->bAddedToPlaylist = FALSE;
			streamripper_callback_data->pIcyData = &icydata;

			// struct per le opzioni per il ripper
			SROPTIONS* sroptions = new SROPTIONS;
			memset(sroptions,'\0',sizeof(SROPTIONS));
			if(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_USEPROXY_KEY))
			{	
				strcpyn(sroptions->proxyurl,m_httpConnection.GetProxyAddress(),MAX_URL_LEN);
				char* p = "";
				_snprintf(sroptions->proxyurl,MAX_URL_LEN-1,"%s%s:%ld",m_httpConnection.GetProxyAddress(),p,m_httpConnection.GetProxyPortNumber());
			}
			else
				strcpy(sroptions->proxyurl,"");
			sroptions->relay_port = -1;
			sroptions->flags = OPT_AUTO_RECONNECT|OPT_ADD_ID3|OPT_DATE_STAMP|OPT_KEEP_INCOMPLETE;
			sroptions->max_port = 18000;
			if(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_STREAMRIPPER_FORCEFAKEUSERAGENT_KEY))
				_snprintf(sroptions->useragent,MAX_USERAGENT_STR-1,"%s",m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_FAKE_AUDIOUSERAGENT_KEY));
			else
				_snprintf(sroptions->useragent,MAX_USERAGENT_STR-1,WALLPAPER_USER_AGENT,winver.GetVersionString());
			sroptions->pVoid = streamripper_callback_data;

			// interfaccia utente
			SetDlgItemText(IDC_STATIC_STATUS,"Retrieving the stream meta data...");

			// delay send()/receive()
			m_icy.SetDelay(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_WINSOCKDELAY_KEY));
			
			// user agent
			m_icy.SetUserAgent(sroptions->useragent);
			BOOL bUserAgentChanged = FALSE;

try_to_connect:

			// carica i valori relativi alla prima entrata valida
			// il  codice sottostante serve soltanto per ricavare le info relative allo stream,
			// il ripping vero e proprio viene eseguito piu' sotto tramite le chiamate alla dll
			bHaveEntries = FALSE;
			int nChannelNumber = 0;
			ITERATOR iter;
			PLS* pls = (PLS*)NULL;
			if((iter = m_listPls.First())!=(ITERATOR)NULL)
				do
				{
					pls = (PLS*)iter->data;
				
					if(pls)
					{
						char	szTitle[MAX_URL+1];
						strcpyn(szTitle,pls->title,sizeof(szTitle));
						strstrim(szTitle);
						_snprintf(sroptions->output_directory,
								MAX_PATH_LEN-1,
								"%s%s",
								m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_DOWNLOADDIR_KEY),
								szTitle);
						_snprintf(sroptions->url,
								MAX_URL_LEN-1,
								"%s:%ld%s%s",
								pls->host,
								pls->port,
//								strcmp(pls->dir,"/")==0 ? "" : pls->dir,
								(strcmp(pls->dir,"/")==0 && strnull(pls->file)) ? "" : pls->dir,
								pls->file
								);
						nChannelNumber = pls->entry;

						// ricava i meta dati relativi allo stream
						if(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_USEPROXY_KEY))
						{	
							m_icy.SetProxyAddress(m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_PROXY_ADDRESS_KEY));
							m_icy.SetProxyPortNumber(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PROXY_PORTNUMBER_KEY));
						}
						if(m_icy.Open(sroptions->url)!=INVALID_SOCKET)
						{
							// interfaccia utente
							_snprintf(szBuffer,sizeof(szBuffer)-1,"Retrieving the stream meta data from channel #%d...",nChannelNumber);
							SetDlgItemText(IDC_STATIC_STATUS,szBuffer);

							// ricava l'header ICY per il report html e per la callback sullo stream
							m_icy.GetIcyData(&icydata);

							if(strcmp(icydata.station,"")!=0)
							{
								char	szStation[ICY_MAX_STATION+1];
								strcpyn(szStation,icydata.station,sizeof(szStation));
								strstrim(szStation);
								_snprintf(sroptions->output_directory,MAX_PATH_LEN-1,"%s%s",m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_DOWNLOADDIR_KEY),szStation);
							}
							
							m_icy.Close();
							
							bHaveEntries = TRUE;
							break;
						}
						else
						{
							dwIcyErrorCode = m_icy.GetLastError();
							strcpyn(szIcyErrorCode,m_icy.GetLastErrorString(),sizeof(szIcyErrorCode));
							_snprintf(szBuffer,sizeof(szBuffer)-1,"Error connecting to the channel #%d: %ld - %s",nChannelNumber,dwIcyErrorCode,szIcyErrorCode);
							SetDlgItemText(IDC_STATIC_STATUS,szBuffer);
						}
					}

					iter = m_listPls.Next(iter);
				
				} while(iter!=(ITERATOR)NULL && m_nCancel==0);

			// riprova il collegamento se esistono entrate nella playlist, se non ha gia' provato a cambiare user-agent,
			// se non si collega per default con lo user-agent fittizio e se il codice di ritorno e' >= 400
			if(	!bHaveEntries && 
				!bUserAgentChanged && 
				!m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_STREAMRIPPER_FORCEFAKEUSERAGENT_KEY) && 
				(dwIcyErrorCode==ICY_CODE_SERVER_FULL || dwIcyErrorCode==ICY_CODE_FORBIDDEN)
				)
			{
				m_icy.SetUserAgent(m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_FAKE_AUDIOUSERAGENT_KEY));
				bUserAgentChanged = TRUE;
				_snprintf(szBuffer,sizeof(szBuffer)-1,"Connection failed, trying with the fake useragent (%s)",m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_FAKE_AUDIOUSERAGENT_KEY));
				SetDlgItemText(IDC_STATIC_STATUS,szBuffer);
				goto try_to_connect;
			}

			if(bHaveEntries)
			{
				// crea il pathname per il download relativo allo stream
				findFile.CreatePathName(sroptions->output_directory,MAX_PATH_LEN);

				// genera il .inf solo se l'url da rippare e' una playlist (.pls)
				// sovradimensionare i buffer perche' alcune emittenti sforano i _MAX_PATH caratteri con il titolo
				if(url.IsUrlType(m_szUrl,HTTP_URL))
				{
					CFilenameFactory fn;
					char szInfFile[1024] = {0};
					char szValidFileName[1024] = {0};
					char* pInfFile = "[noname]";
					if(!strnull(icydata.station))
					{
						pInfFile = icydata.station;
					}
					else
					{
						if(pls)
							if(!strnull(pls->title))
								pInfFile = pls->title;
					}
					findFile.EnsureValidFileName(pInfFile,szValidFileName,sizeof(szValidFileName));
					strstrim(szValidFileName);
					_snprintf(szInfFile,
							sizeof(szInfFile)-1,
							"%s%s.inf",
							sroptions->output_directory,
							fn.Abbreviate(szValidFileName,ABBREVIATE_MAX_FNAME)
							);
					CBinFile infoFile;
					if(infoFile.Create(szInfFile))
					{
						char szInfo[4096] = {0};
						int n = _snprintf(	szInfo,
										sizeof(szInfo)-1,
										"[Info]\r\n"
										"Station=%s\r\n"
										"Genre=%s\r\n"
										"MainSite=%s\r\n"
										"StreamServer=%s\r\n"
										"Playlist=%s\r\n"
										"RippingTo=%s\r\n"
										"RippedBy=%s\r\n"
										"RipperUrl=%s\r\n",
										strnull(icydata.station) ? pls->title : icydata.station,
										icydata.genre,
										icydata.url,
										sroptions->url,
										m_szUrl,
										sroptions->output_directory,
										szUserAgent,
										WALLPAPER_WEB_SITE
										);
						infoFile.Write(szInfo,n);
						infoFile.Close();
					}
				}

				// interfaccia utente
				_snprintf(szBuffer,sizeof(szBuffer)-1,"Starting the stream ripper on channel #%d...",nChannelNumber);
				SetDlgItemText(IDC_STATIC_STATUS,szBuffer);
				if(!strnull(icydata.station))
				{
					strcpyn(szBuffer,icydata.station,sizeof(szBuffer));
					SetWindowText(szBuffer);
				}

				// se esiste l'immagine di default nella directory di output la carica
				char szDefaultPicture[_MAX_PATH+1];
				_snprintf(szDefaultPicture,sizeof(szDefaultPicture)-1,"%sdefault.bmp",sroptions->output_directory);
				if(::FileExist(szDefaultPicture))
					m_wndStaticDib.Load(szDefaultPicture,m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_CURRENTLIBRARY_KEY));
				else
				{
					_snprintf(szDefaultPicture,sizeof(szDefaultPicture)-1,"%sdefault.jpg",sroptions->output_directory);
					if(::FileExist(szDefaultPicture))
						m_wndStaticDib.Load(szDefaultPicture,m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_CURRENTLIBRARY_KEY));
				}

				// chiama la callback per l'inizializzazione
				AudioRipperCallback(RM_INIT,sroptions);

				// inizia la registrazione dello stream
				bConnected = StreamRipperStart(sroptions,AudioRipperCallback)==0;

				// attende la fine della registrazione
				if(m_hStreamRipperEvent)
				{
					::WaitForSingleObject(m_hStreamRipperEvent,INFINITE);
					::CloseHandle(m_hStreamRipperEvent);
					m_hStreamRipperEvent = NULL;

					// ferma la registrazione dello stream
					StreamRipperStop();
				
					// chiama la callback per la terminazione
					AudioRipperCallback(RM_DONE,sroptions);
				}
				
				m_wndStaticDib.Unload();
			}
			else
			{
				char szError[512] = {0};
				if(dwIcyErrorCode!=0L)
					_snprintf(szError,sizeof(szError)-1,"Protocol error code:\r\n%ld - %s\r\n",dwIcyErrorCode,szIcyErrorCode);
				else
					dwIcyErrorCode = (DWORD)-1L;
				::MessageBoxResourceEx(this->m_hWnd,MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_RIPPER_NOCHANNELS,m_szUrl,szError[0]=='\0' ? "" : szError);
			}
			
			if(streamripper_callback_data)
				delete streamripper_callback_data;
			if(sroptions)
				delete sroptions;
		}
			
		// download terminato
		GetDlgItem(IDCANCEL)->EnableWindow(FALSE);

		// data/ora fine download
		strcpyn(m_szEndTime,datetime.GetFormattedDate(TRUE),sizeof(m_szEndTime));
		m_dlTotalTime = ::GetTickCount() - m_dlTotalTime;
		m_dlTotalTime = m_dlTotalTime / 1000.0f;

		// interfaccia utente
		SetWindowText(m_szTitle);
		GetDlgItem(IDC_COMBO_URL)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_SCRIPTFILE)->EnableWindow(TRUE);
		SetDlgItemText(IDC_STATIC_STATUS,"Ripping process terminated...");
		SetDlgItemText(IDC_STATIC_ESTIMATED_TIME_LEFT,"");
		SetDlgItemText(IDC_ESTIMATED_TIME_LEFT,"");
		SetDlgItemText(IDC_STATIC_CURRENT_STATUS,"");
		SetDlgItemText(IDC_CURRENT_STATUS,"");
		SetDlgItemText(IDC_STATIC_TRANSFER_RATE,"");
		SetDlgItemText(IDC_TRANSFER_RATE,"");
		m_wndProgressBar.SetPos(0);
		m_wndProgressBar.Hide();
		m_wndAnimatedAvi.Stop();
		m_wndAnimatedAvi.Close();
		m_wndAnimatedAvi.ShowWindow(SW_HIDE);
		if(m_PictPreviewSync.Lock())
		{
			m_listPictPreview.RemoveAll();
			m_wndStaticDib.Unload();
			SetDlgItemText(IDC_TEXT_INFO,"");
			m_PictPreviewSync.Unlock();
		}
		GetDlgItem(IDC_CHECK_CRAWLER_PREVIEW)->EnableWindow(m_bPicturePreview);
		UpdateStatusBar();
		SetDlgItemText(IDC_STATIC_TOTAL_THREADS,"");

		// interfaccia utente
		SetDlgItemText(IDC_STATIC_STATUS,"Updating database...");
		
		// aggiorna il database per i download con il risultato
		UrlDatabaseService.Update(	m_szUrl,
								m_szEndTime,
								m_dlTotalTime,
								m_dlDownloadTime,
								bConnected ? (m_nCancel!=0 ? CUrlStatus::URL_STATUS_CANCELLED : CUrlStatus::URL_STATUS_DONE) : (nDownloadCode <= (int)HTTP_STATUS_LEN ? (CUrlStatus::URL_STATUS)m_httpConnection.GetHttpError() : (CUrlStatus::URL_STATUS)m_httpConnection.GetSocketError()),
								0,
								bConnected ? szHtmlReport : "",
								UrlService.GetTable());

		// generazione report html
		if(bConnected && bHaveEntries && dwIcyErrorCode==0L)
			if(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_GENERATEREPORT_KEY))
			{
				HTMLREPORTEXTRAPARAMS htmlparams = {CRAWLER_STREAMRIPPER_MODE,&icydata};
				GenerateHtmlReport(m_szUrl,szHtmlReport,"",&UrlService,&UrlDatabaseService,&htmlparams);
			}
			
		// interfaccia utente
		SetDlgItemText(IDC_STATIC_STATUS,"");

		m_StreamRipperSync.Unlock();
	}
	else
	{
		::MessageBoxResource(this->m_hWnd,MB_OK|MB_ICONWARNING,WALLPAPER_PROGRAM_NAME,IDS_ERROR_RIPPER_ALREADYRUNNING);
		goto terminate;
	}

terminate:

	// download terminato
	m_bInCrawling = FALSE;
	m_oleDropTarget.Enable();
	CDialogEx::AllowDragAndDrop(TRUE);

	// interfaccia utente
	_snprintf(m_szTitle,sizeof(m_szTitle)-1,"done - %s",m_szUrl);
	SetWindowText(m_szTitle);
	SetDlgItemText(IDC_STATIC_STATUS,"");

	// reabilita i flags per l'uscita (trasforma cancel in close)
	GetDlgItem(IDC_COMBO_URL)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_SCRIPTFILE)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_CRAWLER_PREVIEW)->EnableWindow(m_bPicturePreview);
	GetDlgItem(IDOK)->EnableWindow(TRUE);
	SetDlgItemText(IDCANCEL,"&Close");
	m_nCancel = 0;
	
	GetDlgItem(IDCANCEL)->EnableWindow(TRUE);
	
	return(0L);
}

/*
	AddDownloadToPlaylist()

	Aggiunge il file alla playlist.
*/
void CWallPaperCrawlerDlg::AddDownloadToPlaylist(int nDownloadCode,LPCSTR lpcszFileName)
{
	BOOL bIsPicture = FALSE;
	BOOL bIsAudio = FALSE;

	// aggiunge in base al flag relativo, allo status (file scaricato) e al tipo file
	if(m_bAddFilesToPlaylist)
		if(nDownloadCode==(int)CUrlStatus::URL_STATUS_DOWNLOADED)
			if((bIsPicture = m_pImage->IsSupportedFormat(lpcszFileName))==TRUE || (bIsAudio = CAudioPlayer::IsSupportedFormat(lpcszFileName))==TRUE)
				if(m_hWndParent)
				{
					// interfaccia utente
					CWnd* pWnd = GetDlgItem(IDC_STATIC_STATUS);
					if(pWnd)
					{
						char szBuffer[256];
						char* p = (char*)strrchr(lpcszFileName,'\\');
						if(p)
							p++;
						if(!p)
							p = (char*)lpcszFileName;
						_snprintf(szBuffer,sizeof(szBuffer)-1,"Adding %s to the playlist...",p);
						pWnd->SetWindowText(szBuffer);
					}

					// 0 per le immagini, n (modalita' riproduzione) per gli audio
					int nPlayMode = (bIsPicture ? 0 : (bIsAudio ? AUDIOPLAYER_COMMAND_QUEUE_FROM_FILE : 0));
					::SendMessage(m_hWndParent,WM_PLAYLIST_ADDFILE,(WPARAM)nPlayMode,(LPARAM)lpcszFileName);
				}
}

/*
	GenerateHtmlReport()

	Genera il report relativo al download.
*/
void CWallPaperCrawlerDlg::GenerateHtmlReport(	LPCSTR				lpcszUrl,
										LPCSTR				lpcszReportName,
										LPCSTR				lpcszAsciiReportName,
										CUrlService*			pUrlService,
										CUrlDatabaseService*	pUrlDatabaseService,
										HTMLREPORTEXTRAPARAMS*	htmlparams/* = NULL*/
										)
{
	int i;
	char* p;
	CUrl url;
	CUrlStatus urlstatus;
	CUrlStatus::URL_STATUS url_status;
	char szUrl[MAX_URL+1];
	char szDecodedUrl[MAX_URL+1];
	char szBuffer[1024];
	char szLocalFile[_MAX_PATH+1];
	char szLocalFileName[_MAX_PATH+1];
	char szIcon[(_MAX_PATH*2)+1];
	double dlTotBytes = 0.0f;
	UINT nUrls = 0;
	UINT nDownloadedFiles = 0;
	UINT nDownloadedPictures = 0;
	UINT nTotalPictures = 0;
	CHtml html;
	CDateTime datetime;
	CMP3Info mp3info;
	BOOL bHaveInvalidUrl = FALSE;
	BOOL bHaveTruncatedUrl = FALSE;

	// interfaccia utente
	GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
	_snprintf(szBuffer,sizeof(szBuffer)-1,"Generating report:\n%s...",lpcszReportName);
	SetDlgItemText(IDC_STATIC_STATUS,szBuffer);

	// per l'accesso ai database
	CUrlDatabaseTable* pUrlDatabaseTable = pUrlDatabaseService->GetTable();
	CUrlTable* pUrlTable = pUrlService->GetTable();

	// totali
	nUrls = pUrlDatabaseTable->GetField_TotalUrls();
	if(m_nCrawlerMode!=CRAWLER_STREAMRIPPER_MODE)
	{
		nTotalPictures = pUrlDatabaseTable->GetField_TotalPict();
		nDownloadedFiles = pUrlDatabaseTable->GetField_DownloadedUrls();
		dlTotBytes = pUrlDatabaseTable->GetField_Downloaded();
	}
	else
		dlTotBytes = pUrlService->CountItems(nTotalPictures,nDownloadedFiles);
	nDownloadedPictures	= pUrlDatabaseTable->GetField_DownloadedPict();

	// imposta il layout per il report
	HTMLLAYOUT htmllayout;
	htmllayout.font.type  = "verdana";
	htmllayout.font.size  = 1;
	htmllayout.font.color = NULL;
	htmllayout.align      = HTMLALIGN_NONE;
	htmllayout.style      = HTMLSTYLE_NONE;
	htmllayout.border     = 0;
	htmllayout.width      = 0;

	HTMLTABLELAYOUT htmltablelayout;
	memcpy(&(htmltablelayout.htmllayout),&htmllayout,sizeof(HTMLLAYOUT));
	htmltablelayout.title       = NULL;
	htmltablelayout.border      = 1;
	htmltablelayout.cellpadding = 1;
	htmltablelayout.cellspacing = 1;
	htmltablelayout.width       = 100;
	strcpyn(htmltablelayout.classname,"",sizeof(htmltablelayout.classname));

	CSSLAYOUT csslayout = {"","",HTMLALIGN_NONE,0};
	
	// crea il file html per il report
	if(!html.Open(lpcszReportName))
	{
		if(m_bUseScript && m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_QUIETINSCRIPTMODE_KEY))
			;
		else
			::MessageBoxResourceEx(this->m_hWnd,MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_OPENING_HTML_REPORT,lpcszReportName);
		goto done;
	}
	
	// meta tags
	_snprintf(szBuffer,
			sizeof(szBuffer)-1,
			"<meta name=\"generator\" content=\"%s, %s\">"
			"\r\n"
			"<link rel=\"stylesheet\" href=\"style.css\" type=\"text/css\">"
			,
			WALLPAPER_PROGRAM_TITLE,
			m_pConfig->GetString(WALLPAPER_INSTALL_KEY,WALLPAPER_WEBSITE_KEY)
			);
	html.Header(lpcszUrl,"<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 3.2//EN\">",szBuffer);

	// tabella per l'intestazione, composta da 1 riga e 2 colonne (una per l'icona e l'altra per i totali)
	htmltablelayout.border = 0;
	strcpyn(htmltablelayout.classname,"",sizeof(htmltablelayout.classname));
	html.TableOpen(&htmltablelayout);
	html.TableOpenRow();
	
	// colonna per l'icona
	_snprintf(szBuffer,sizeof(szBuffer)-1,"%s%s",m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_BITMAPSDIR_KEY),"wallpaper.gif");
	url.LocalFileToUrl(szBuffer,szLocalFile,sizeof(szLocalFile));
	_snprintf(szBuffer,
			sizeof(szBuffer)-1,
			"<br><a href=\"%s\" target=\"_blank\"><img src=\"%s\" border=0><br>%s</a><br><br>",
			m_pConfig->GetString(WALLPAPER_INSTALL_KEY,WALLPAPER_WEBSITE_KEY),
			szLocalFile,
			WALLPAPER_PROGRAM_NAME_ALIAS" - "WALLPAPER_PROGRAM_DESCRIPTION" - v."WALLPAPER_VERSION
			);
	strcpy(csslayout.tagname,"p");
	strcpy(csslayout.classname,"");
	csslayout.align = HTMLALIGN_CENTER;
	csslayout.width = 20;
	html.TablePutCol(szBuffer,&csslayout);

	// colonna per i totali
	html.TableOpenCol();
	html.TableOpen();

	strcpy(csslayout.tagname,"p");
	csslayout.align = HTMLALIGN_LEFT;
	
	// intestazione audio (ex icydata)
	if(htmlparams)
	{
		if(htmlparams->type==CRAWLER_STREAMRIPPER_MODE)
		{
			if(!strnull(htmlparams->icydata->station))
			{
				// radio station
				html.TableOpenRow();
					strcpy(csslayout.classname,"xbold");
					csslayout.width = 25;
						html.TablePutCol("&nbsp;radio station",&csslayout);
					strcpy(csslayout.classname,"");
					csslayout.width = 0;
						html.TablePutCol(htmlparams->icydata->station,&csslayout);
				html.TableCloseRow();
			}
			if(!strnull(htmlparams->icydata->url))
			{
				// url
				html.TableOpenRow();
					strcpy(csslayout.classname,"xbold");
					csslayout.width = 25;
						html.TablePutCol("&nbsp;station url",&csslayout);
					strcpy(csslayout.classname,"");
					csslayout.width = 0;
						_snprintf(szBuffer,sizeof(szBuffer)-1,"<a href=\"%s\" target=\"_blank\">%s</a>",htmlparams->icydata->url,htmlparams->icydata->url);
						html.TablePutCol(szBuffer,&csslayout);
				html.TableCloseRow();
			}
			if(!strnull(htmlparams->icydata->genre))
			{
				// genre
				html.TableOpenRow();
					strcpy(csslayout.classname,"xbold");
					csslayout.width = 25;
						html.TablePutCol("&nbsp;genre",&csslayout);
					strcpy(csslayout.classname,"");
					csslayout.width = 0;
						html.TablePutCol(htmlparams->icydata->genre,&csslayout);
				html.TableCloseRow();
			}
			if(htmlparams->icydata->bitrate > 0)
			{
				// bitrate
				html.TableOpenRow();
					strcpy(csslayout.classname,"xbold");
					csslayout.width = 25;
						html.TablePutCol("&nbsp;bitrate",&csslayout);
					strcpy(csslayout.classname,"");
					csslayout.width = 0;
						_snprintf(szBuffer,
								sizeof(szBuffer)-1,
								"%d kbps",
								htmlparams->icydata->bitrate
								);
						html.TablePutCol(szBuffer,&csslayout);
				html.TableCloseRow();
			}
		}
	}

	// url
	html.TableOpenRow();
		strcpy(csslayout.classname,"xbold");
		csslayout.width = 25;
			html.TablePutCol("&nbsp;url",&csslayout);
		i = _snprintf(szBuffer,sizeof(szBuffer)-1,"<a href=\"%s\" target=\"_blank\">%s</a>",lpcszUrl,lpcszUrl);
		if(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_USEPROXY_KEY))
			_snprintf(szBuffer+i,(sizeof(szBuffer)-1)-i," (through the %s:%ld proxy)",m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_PROXY_ADDRESS_KEY),m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PROXY_PORTNUMBER_KEY));
		strcpy(csslayout.classname,"");
		csslayout.width = 0;
			html.TablePutCol(szBuffer,&csslayout);
	html.TableCloseRow();

	// host
	if(!strnull(m_httpConnection.GetHostName()))
	{
		html.TableOpenRow();
			strcpy(csslayout.classname,"xbold");
			csslayout.width = 25;
				html.TablePutCol("&nbsp;host name",&csslayout);
			strcpy(csslayout.classname,"");
			csslayout.width = 0;
				html.TablePutCol(m_httpConnection.GetHostName(),&csslayout);
		html.TableCloseRow();
		html.TableOpenRow();
			strcpy(csslayout.classname,"xbold");
			csslayout.width = 25;
				html.TablePutCol("&nbsp;ip address",&csslayout);
			strcpy(csslayout.classname,"");
			csslayout.width = 0;
				html.TablePutCol(m_httpConnection.GetHostAddr(),&csslayout);
		html.TableCloseRow();
	}
	
	// port
	html.TableOpenRow();
		strcpy(csslayout.classname,"xbold");
		csslayout.width = 25;
			html.TablePutCol("&nbsp;port number",&csslayout);
		_snprintf(szBuffer,sizeof(szBuffer)-1,"%ld",m_httpConnection.GetPortNumber());
		strcpy(csslayout.classname,"");
		csslayout.width = 0;
			html.TablePutCol(szBuffer,&csslayout);
	html.TableCloseRow();

	// server
	if(!m_httpConnection.GetServerName())
	{
		html.TableOpenRow();
			strcpy(csslayout.classname,"xbold");
			csslayout.width = 25;
				html.TablePutCol("&nbsp;server name",&csslayout);
			strcpy(csslayout.classname,"");
			csslayout.width = 0;
				html.TablePutCol(m_httpConnection.GetServerName(),&csslayout);
		html.TableCloseRow();
	}
	
	// start time
	html.TableOpenRow();
		strcpy(csslayout.classname,"xbold");
		csslayout.width = 25;
			html.TablePutCol("&nbsp;start time",&csslayout);
		strcpy(csslayout.classname,"");
		csslayout.width = 0;
			html.TablePutCol(m_szStartTime,&csslayout);
	html.TableCloseRow();
	
	// end time
	html.TableOpenRow();
		strcpy(csslayout.classname,"xbold");
		csslayout.width = 25;
			html.TablePutCol("&nbsp;end time",&csslayout);
		strcpy(csslayout.classname,"");
		csslayout.width = 0;
			html.TablePutCol(m_szEndTime,&csslayout);
	html.TableCloseRow();
	
	// elapsed
	html.TableOpenRow();
		strcpy(csslayout.classname,"xbold");
		csslayout.width = 25;
		html.TablePutCol("&nbsp;elapsed",&csslayout);
		i = _snprintf(szBuffer,sizeof(szBuffer)-1,"%s (total time)",m_dlTotalTime <= 0.0f ? "< 1 sec." : datetime.GetElapsedTime(m_dlTotalTime));
		strcpy(csslayout.classname,"");
		csslayout.width = 0;
		html.TablePutCol(szBuffer,&csslayout);
	html.TableCloseRow();
	
	// transfer rate
	html.TableOpenRow();
		strcpy(csslayout.classname,"xbold");
		csslayout.width = 25;
			html.TablePutCol("&nbsp;average transfer rate",&csslayout);
		double dlTransferRate = 0.0f;
		if(dlTotBytes > 1024.0f)
			dlTransferRate = FDIV(dlTotBytes,m_dlTotalTime) / 1024.0f;
		if(dlTransferRate < 1024.0f)
			_snprintf(szBuffer,sizeof(szBuffer)-1,"%0.2f KB/s",dlTransferRate);
		else
			_snprintf(szBuffer,sizeof(szBuffer)-1,"%0.2f MB/s",FDIV(dlTransferRate,1024.0f));
		strcpy(csslayout.classname,"");
		csslayout.width = 0;
			html.TablePutCol(szBuffer,&csslayout);
	html.TableCloseRow();
	
	// downloaded
	html.TableOpenRow();
		strcpy(csslayout.classname,"xbold");
		csslayout.width = 25;
			html.TablePutCol("&nbsp;downloaded",&csslayout);
		dtos(dlTotBytes,szBuffer,sizeof(szBuffer));
		i = strlen(szBuffer);
		if(dlTotBytes < 1024.0f)
			_snprintf(szBuffer+i,sizeof(szBuffer)-1-i," bytes, %ld files (including all kind of downloaded files)",nDownloadedFiles);
		else if(dlTotBytes < 1048576.0f)
			_snprintf(szBuffer+i,sizeof(szBuffer)-1-i," bytes (%0.2f KB), %ld files (including all kind of downloaded files)",FDIV((dlTotBytes),1024.0f),nDownloadedFiles);
		else if(dlTotBytes < 1073741824.0f)
			_snprintf(szBuffer+i,sizeof(szBuffer)-1-i," bytes (%0.2f MB), %ld files (including all kind of downloaded files)",FDIV((dlTotBytes),1048576.0f),nDownloadedFiles);
		else
			_snprintf(szBuffer+i,sizeof(szBuffer)-1-i," bytes (%0.2f GB), %ld files (including all kind of downloaded files)",FDIV((dlTotBytes),1073741824.0f),nDownloadedFiles);
		strcpy(csslayout.classname,"");
		csslayout.width = 0;
			html.TablePutCol(szBuffer,&csslayout);
	html.TableCloseRow();
	
	// urls
	if(m_nCrawlerMode!=CRAWLER_STREAMRIPPER_MODE)
	{
		html.TableOpenRow();
			strcpy(csslayout.classname,"xbold");
			csslayout.width = 25;
				html.TablePutCol("&nbsp;urls",&csslayout);
			_snprintf(szBuffer,sizeof(szBuffer)-1,"%ld (including all kind of parsed urls)",nUrls);
			strcpy(csslayout.classname,"");
			csslayout.width = 0;
				html.TablePutCol(szBuffer,&csslayout);
		html.TableCloseRow();
	}

	// pictures/songs
	html.TableOpenRow();
		strcpy(csslayout.classname,"xbold");
		csslayout.width = 25;
		if(m_nCrawlerMode==CRAWLER_STREAMRIPPER_MODE)
		{
			html.TablePutCol("&nbsp;ripped songs",&csslayout);
			_snprintf(szBuffer,
					sizeof(szBuffer)-1,
					"%ld",
					nDownloadedFiles
					);
		}
		else
		{
			html.TablePutCol("&nbsp;pictures",&csslayout);
			_snprintf(szBuffer,
					sizeof(szBuffer)-1,
					"total: %ld (%ld%% of total urls), downloaded: %ld (%ld%% of downloaded urls)",
					nTotalPictures,
					DIV((100*nTotalPictures),nUrls),
					nDownloadedPictures,
					DIV((100*nDownloadedPictures),nDownloadedFiles)
					);
		}
		strcpy(csslayout.classname,"");
		csslayout.width = 0;
			html.TablePutCol(szBuffer,&csslayout);
	html.TableCloseRow();

	html.TableClose();
	html.TableCloseCol();
	html.TableCloseRow();
	html.TableClose();
	html.NewLine();

	// tabella per l'elenco delle url (download)
	htmltablelayout.border = 1;
	strcpyn(htmltablelayout.classname,"white",sizeof(htmltablelayout.classname));
	html.TableOpen(&htmltablelayout);
	
	// intestazioni per le colonne
	html.TableOpenRow();
		strcpy(csslayout.tagname,"p");
		strcpy(csslayout.classname,"xbold");
		csslayout.width = 0;
		csslayout.align = HTMLALIGN_LEFT;
			if(m_nCrawlerMode!=CRAWLER_STREAMRIPPER_MODE)
			{
				html.TablePutCol("&nbsp;ID&nbsp;",&csslayout);
				html.TablePutCol("&nbsp;Parent Url",&csslayout);
			}
			html.TablePutCol("&nbsp;Url",&csslayout);
			html.TablePutCol("&nbsp;Local file",&csslayout);
		csslayout.align = HTMLALIGN_CENTER;
			if(m_nCrawlerMode==CRAWLER_STREAMRIPPER_MODE)
				html.TablePutCol("&nbsp;Duration",&csslayout);
			html.TablePutCol("Size",&csslayout);
			if(m_nCrawlerMode==CRAWLER_STREAMRIPPER_MODE)
				html.TablePutCol("Download time",&csslayout);
			else
				html.TablePutCol("Time",&csslayout);
			html.TablePutCol("&nbsp;Status",&csslayout);
	html.TableCloseRow();

	// stampa i record
	i = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_REPORTORDER_KEY);
	switch(i)
	{
		case REPORT_BY_PARENT_URL:
			i = URL_IDX_PARENTURL;
			break;
		case REPORT_BY_URL:
			i = URL_IDX_URL;
			break;
		case REPORT_BY_STAT:
			i = URL_IDX_STAT;
			break;
		case REPORT_BY_ID:
		default:
			i = URL_IDX_ID;
			break;
	}
	// imposta l'indice per l'ordinamento delle url
	pUrlTable->SetIndex(i);
	if(pUrlTable->GoTop())
	{
		// interfaccia utente
		::PeekAndPump();

		// per ognuna delle url presenti nel database
		while(!pUrlTable->Eof())
		{
			// nuova riga
			html.TableOpenRow();
			memset(szBuffer,'\0',sizeof(szBuffer));

			// carica i valori del record
			pUrlTable->ScatterMemvars();
			url_status = (CUrlStatus::URL_STATUS)pUrlTable->GetField_Stat();
			urlstatus.GetStatus(url_status);

			if(m_nCrawlerMode!=CRAWLER_STREAMRIPPER_MODE)
			{
				// id
				_snprintf(szBuffer,sizeof(szBuffer)-1,"%ld",pUrlTable->GetField_Id());
				strcpy(csslayout.tagname,"p");
				strcpy(csslayout.classname,"");
				csslayout.align = HTMLALIGN_CENTER;
				csslayout.width = 0;
				html.TablePutCol(szBuffer,&csslayout);

				// parent url/station
				bHaveTruncatedUrl = FALSE;
				strcpyn(szDecodedUrl,pUrlTable->GetField_ParentUrl(),sizeof(szDecodedUrl));
				p = strchr(szDecodedUrl,'\r');
				if(!p)
					p = strchr(szDecodedUrl,'\n');
				if(p)
				{
					*p = '\0';
					strrev(szDecodedUrl);
					memcpy(szDecodedUrl,"]...[",5);
					strrev(szDecodedUrl);
					bHaveTruncatedUrl = TRUE;
				}
				url.DecodeUrl(szDecodedUrl);
				_snprintf(szBuffer,sizeof(szBuffer)-1,"&nbsp;%s",szDecodedUrl);
				strcpy(csslayout.tagname,"p");
				strcpy(csslayout.classname,"");
				csslayout.align = HTMLALIGN_LEFT;
				csslayout.width = 0;
				html.TablePutCol(szBuffer,&csslayout);
			}

			// url
			strcpyn(szIcon,m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_BITMAPSDIR_KEY),sizeof(szIcon));
			switch(url_status)
			{
				case CUrlStatus::URL_STATUS_DONE:
				case CUrlStatus::URL_STATUS_DOWNLOADED:
				case CUrlStatus::URL_STATUS_MOVED:
				case CUrlStatus::URL_STATUS_NOT_MODIFIED:
				{
					if(url.IsHtmlFile(pUrlTable->GetField_Url()))
					{
						strcatn(szIcon,"url.gif",sizeof(szIcon));
					}
					else
					{
						if(CImage::IsImageFile(pUrlTable->GetField_Url()))
							strcatn(szIcon,"picture.gif",sizeof(szIcon));
						else
							strcatn(szIcon,"urldir.gif",sizeof(szIcon));
					}
					break;
				}
				case CUrlStatus::URL_STATUS_CANCELLED:
				case CUrlStatus::URL_STATUS_CGI_URL:
				case CUrlStatus::URL_STATUS_WINSOCK_ERROR:
				case CUrlStatus::URL_STATUS_HTTP_ERROR:
				case CUrlStatus::URL_STATUS_INCOMPLETE:
				case CUrlStatus::URL_STATUS_UNKNOWN:
					strcatn(szIcon,"urlnotfound.gif",sizeof(szIcon));
					break;
				case CUrlStatus::URL_STATUS_EXCLUDED_BY_ROBOTSTXT:
				case CUrlStatus::URL_STATUS_EXCLUDED_BY_SIZE:
				case CUrlStatus::URL_STATUS_EXCLUDED_BY_WILDCARDS:
					strcatn(szIcon,"filter.gif",sizeof(szIcon));
					break;
				case CUrlStatus::URL_STATUS_EXTERNAL_DOMAIN:
					strcatn(szIcon,"netserver.gif",sizeof(szIcon));
					break;
				case CUrlStatus::URL_STATUS_PARENT_URL:
					strcatn(szIcon,"network.gif",sizeof(szIcon));
					break;
				case CUrlStatus::URL_STATUS_FILE_CREATION_ERROR:
				case CUrlStatus::URL_STATUS_INVALID_FILE:
					strcatn(szIcon,"invalidfile.gif",sizeof(szIcon));
					break;
				case CUrlStatus::URL_STATUS_INVALID_URL:
					strcatn(szIcon,"invalidurl.gif",sizeof(szIcon));
					bHaveInvalidUrl = TRUE;
					break;
				case CUrlStatus::URL_STATUS_CHECKED:
					strcatn(szIcon,"checked.gif",sizeof(szIcon));
					break;
				default:
					strcatn(szIcon,"file.gif",sizeof(szIcon));
					break;
			}
			bHaveTruncatedUrl = FALSE;
			strcpyn(szUrl,pUrlTable->GetField_Url(),sizeof(szUrl));
			p = strchr(szUrl,'\r');
			if(!p)
				p = strchr(szUrl,'\n');
			if(p)
			{
				*p = '\0';
				strrev(szUrl);
				memcpy(szUrl,"]...[",5);
				strrev(szUrl);
				bHaveTruncatedUrl = TRUE;
			}
			url.EncodeUrl(szUrl,sizeof(szUrl));

			strcpyn(szDecodedUrl,pUrlTable->GetField_Url(),sizeof(szDecodedUrl));
			p = strchr(szDecodedUrl,'\r');
			if(!p)
				p = strchr(szDecodedUrl,'\n');
			if(p)
			{
				*p = '\0';
				strrev(szDecodedUrl);
				memcpy(szDecodedUrl,"]...[",5);
				strrev(szDecodedUrl);
				bHaveTruncatedUrl = TRUE;
			}
			url.DecodeUrl(szDecodedUrl);

			if(m_nCrawlerMode==CRAWLER_STREAMRIPPER_MODE)
			{
				_snprintf(szBuffer,
						sizeof(szBuffer)-1,
						"&nbsp;<img src=\"%s\" border=0>&nbsp;%s",
						url.LocalFileToUrl(szIcon,szLocalFile,sizeof(szLocalFile)),
						szDecodedUrl
						);
			}
			else
			{
				if(bHaveTruncatedUrl)
					_snprintf(szBuffer,
							sizeof(szBuffer)-1,
							"&nbsp;<img src=\"%s\" border=0>&nbsp;%s",
							url.LocalFileToUrl(szIcon,szLocalFile,sizeof(szLocalFile)),
							szDecodedUrl
							);
				else
					_snprintf(szBuffer,
							sizeof(szBuffer)-1,
							"&nbsp;<a href=\"%s\" target=\"_blank\"><img src=\"%s\" border=0>%s</a>",
							szUrl,
							url.LocalFileToUrl(szIcon,szLocalFile,sizeof(szLocalFile)),
							szDecodedUrl
							);
			}
			strcpy(csslayout.tagname,"p");
			strcpy(csslayout.classname,"");
			csslayout.align = HTMLALIGN_LEFT;
			csslayout.width = 0;
			html.TablePutCol(szBuffer,&csslayout);
			
			// file
			if(	!strempty(pUrlTable->GetField_File())											&&
				(m_nCrawlerMode==CRAWLER_STREAMRIPPER_MODE ? TRUE : (pUrlTable->GetField_Size() > 0))	&&
				(CUrlStatus::URL_STATUS)pUrlTable->GetField_Stat()!=CUrlStatus::URL_STATUS_CHECKED
			)
			{
				bHaveTruncatedUrl = FALSE;
				strcpyn(szLocalFileName,pUrlTable->GetField_File(),sizeof(szLocalFileName));
				
				p = strchr(szLocalFileName,'\r');
				if(!p)
					p = strchr(szLocalFileName,'\n');
				if(p)
				{
					*p = '\0';
					strrev(szLocalFileName);
					memcpy(szLocalFileName,"]...[",5);
					strrev(szLocalFileName);
					bHaveTruncatedUrl = TRUE;
				}

				p = strrchr(szLocalFileName,'\\');
				if(p && *(p+1))
					p++;
				else
					p = szLocalFileName;

				if(bHaveTruncatedUrl)
					_snprintf(szBuffer,
							sizeof(szBuffer)-1,
							"&nbsp;%s",
							p);
				else
					_snprintf(szBuffer,
							sizeof(szBuffer)-1,
							"&nbsp;<a href=\"%s\" target=\"_blank\">%s</a>",
							url.LocalFileToUrl(szLocalFileName,szLocalFile,sizeof(szLocalFile)),
							p);
				
				strcpy(csslayout.tagname,"p");
				strcpy(csslayout.classname,"");
				csslayout.align = HTMLALIGN_LEFT;
				csslayout.width = 0;
				html.TablePutCol(szBuffer,&csslayout);
			}
			else
			{
				memset(szLocalFileName,'\0',sizeof(szLocalFileName));
				strcpy(csslayout.tagname,"p");
				strcpy(csslayout.classname,"");
				csslayout.align = HTMLALIGN_CENTER;
				csslayout.width = 0;
				html.TablePutCol("-",&csslayout);
			}
			
			if(m_nCrawlerMode==CRAWLER_STREAMRIPPER_MODE)
			{
				// duration
				strcpy(szBuffer,"-");
				if(!strnull(szLocalFileName))
				{
					if(mp3info.Load(szLocalFileName))
					{
						long lMinutes = 0,lSeconds = 0;
						mp3info.GetLength(lMinutes,lSeconds);
						_snprintf(szBuffer,sizeof(szBuffer)-1,"%02ld:%02ld&nbsp;",lMinutes,lSeconds);
					}
				}
				strcpy(csslayout.tagname,"p");
				strcpy(csslayout.classname,"");
				csslayout.align = HTMLALIGN_CENTER;
				csslayout.width = 0;
				html.TablePutCol(szBuffer,&csslayout);
			}
			
			// size
			if(pUrlTable->GetField_Size() > 0)
			{
				dtos(pUrlTable->GetField_Size(),szBuffer,sizeof(szBuffer));
				strcatn(szBuffer,"&nbsp;",sizeof(szBuffer));
				strcpy(csslayout.tagname,"p");
				strcpy(csslayout.classname,"");
				csslayout.align = HTMLALIGN_RIGHT;
				csslayout.width = 0;
				html.TablePutCol(szBuffer,&csslayout);
			}
			else
			{
				strcpy(csslayout.tagname,"p");
				strcpy(csslayout.classname,"");
				csslayout.align = HTMLALIGN_CENTER;
				csslayout.width = 0;
				html.TablePutCol("-",&csslayout);
			}
			
			// time
			if(pUrlTable->GetField_Seconds() > 0.0f)
			{
				_snprintf(szBuffer,sizeof(szBuffer)-1,"%s&nbsp;",datetime.GetElapsedTime(pUrlTable->GetField_Seconds()));
				url.EncodeToHTML(szBuffer,sizeof(szBuffer));
				strcpy(csslayout.tagname,"p");
				strcpy(csslayout.classname,"");
				csslayout.align = HTMLALIGN_RIGHT;
				csslayout.width = 0;
				html.TablePutCol(szBuffer,&csslayout);
			}
			else
			{
				strcpy(csslayout.tagname,"p");
				strcpy(csslayout.classname,"");
				csslayout.align = HTMLALIGN_CENTER;
				csslayout.width = 0;
				html.TablePutCol("-",&csslayout);
			}

			// status
			url_status = (CUrlStatus::URL_STATUS)pUrlTable->GetField_Stat();
			if(url_status==CUrlStatus::URL_STATUS_UNKNOWN)
			{
				csslayout.align = HTMLALIGN_CENTER;
				strcpyn(szBuffer,"-",sizeof(szBuffer));
			}
			else
			{
				csslayout.align = HTMLALIGN_LEFT;
				_snprintf(szBuffer,sizeof(szBuffer)-1,"&nbsp;%s",urlstatus.GetStatus(url_status));
				url.EncodeToHTML(szBuffer,sizeof(szBuffer));
			}
			switch(url_status)
			{
				case CUrlStatus::URL_STATUS_DONE:
				case CUrlStatus::URL_STATUS_DOWNLOADED:
				case CUrlStatus::URL_STATUS_NOT_MODIFIED:
					strcpy(csslayout.classname,"");
					break;
				case CUrlStatus::URL_STATUS_MOVED:
				case CUrlStatus::URL_STATUS_CANCELLED:
				case CUrlStatus::URL_STATUS_EXCLUDED_BY_ROBOTSTXT:
				case CUrlStatus::URL_STATUS_EXCLUDED_BY_SIZE:
				case CUrlStatus::URL_STATUS_EXCLUDED_BY_WILDCARDS:
				case CUrlStatus::URL_STATUS_EXTERNAL_DOMAIN:
				case CUrlStatus::URL_STATUS_PARENT_URL:
				case CUrlStatus::URL_STATUS_FILE_CREATION_ERROR:
				case CUrlStatus::URL_STATUS_INCOMPLETE:
				case CUrlStatus::URL_STATUS_INVALID_FILE:
				case CUrlStatus::URL_STATUS_INVALID_URL:
				case CUrlStatus::URL_STATUS_CGI_URL:
				case CUrlStatus::URL_STATUS_WINSOCK_ERROR:
				case CUrlStatus::URL_STATUS_HTTP_ERROR:
				case CUrlStatus::URL_STATUS_UNKNOWN:
					strcpy(csslayout.classname,"red");
					break;
				case CUrlStatus::URL_STATUS_CHECKED:
					strcpy(csslayout.classname,"");
					break;
				default:
					strcpy(csslayout.classname,"");
					break;
			}
			strcpy(csslayout.tagname,"p");
			csslayout.width = 0;
			html.TablePutCol(szBuffer,&csslayout);
			
			// chiude la riga
			html.TableCloseRow();

			pUrlTable->GetNext();
		}

		htmllayout.style = HTMLSTYLE_BOLD;

		// riga per i totali
		html.TableOpenRow();
		
			strcpy(csslayout.tagname,"p");
			strcpy(csslayout.classname,"xbold");
			csslayout.width = 0;
			
			if(m_nCrawlerMode!=CRAWLER_STREAMRIPPER_MODE)
			{
				// id
				csslayout.align = HTMLALIGN_LEFT;
				html.TablePutCol("&nbsp;",&csslayout);
				// parent url
				csslayout.align = HTMLALIGN_LEFT;
				html.TablePutCol("&nbsp;",&csslayout);
			}

			// url
			if(m_nCrawlerMode!=CRAWLER_STREAMRIPPER_MODE)
				_snprintf(szBuffer,sizeof(szBuffer)-1,"&nbsp;&nbsp;&nbsp;&nbsp;%ld url(s)",nUrls);
			else
				strcpy(szBuffer,"&nbsp;");
			csslayout.align = HTMLALIGN_LEFT;
			html.TablePutCol(szBuffer,&csslayout);

			// file
			_snprintf(szBuffer,sizeof(szBuffer)-1," %ld file(s)",nDownloadedFiles);
			url.EncodeToHTML(szBuffer,sizeof(szBuffer));
			csslayout.align = HTMLALIGN_LEFT;
			html.TablePutCol(szBuffer,&csslayout);

			if(m_nCrawlerMode==CRAWLER_STREAMRIPPER_MODE)
			{
				// duration
				csslayout.align = HTMLALIGN_CENTER;
				html.TablePutCol("&nbsp;",&csslayout);
			}

			// size
			strsize(szBuffer,sizeof(szBuffer),dlTotBytes);
			url.EncodeToHTML(szBuffer,sizeof(szBuffer));
			csslayout.align = HTMLALIGN_RIGHT;
			html.TablePutCol(szBuffer,&csslayout);

			// time
			csslayout.align = HTMLALIGN_RIGHT;
			html.TablePutCol("&nbsp;",&csslayout);

			// status
			csslayout.align = HTMLALIGN_LEFT;
			html.TablePutCol("&nbsp;",&csslayout);
		
		html.TableCloseRow();
	}

	html.TableClose();

	// tabella per i tipi di url non riconosciuti
	if(bHaveInvalidUrl)
	{
		html.Text("<br><br><p class=\"bold\">&nbsp;Invalid url(s)</p>");
		htmltablelayout.border = 1;
		strcpyn(htmltablelayout.classname,"white",sizeof(htmltablelayout.classname));
		html.TableOpen(&htmltablelayout);

		strcpy(csslayout.tagname,"p");
		strcpy(csslayout.classname,"");
		csslayout.align = HTMLALIGN_LEFT;
		csslayout.width = 0;

		if(pUrlTable->GoTop())
		{
			::PeekAndPump();

			while(!pUrlTable->Eof())
			{
				pUrlTable->ScatterMemvars();

				strcpyn(szUrl,pUrlTable->GetField_Url(),sizeof(szUrl));
				if(!url.IsUrlType(szUrl,HTTP_URL))
				{
					html.TableOpenRow();
						url.DecodeUrl(szUrl);
						_snprintf(szBuffer,sizeof(szBuffer)-1,"&nbsp;%s",szUrl);
						htmllayout.align = HTMLALIGN_LEFT;
						html.TablePutCol(szBuffer,&csslayout);
					html.TableCloseRow();
				}

				pUrlTable->GetNext();
			}
		}
		
		html.TableClose();
	}

	// inserisce la tabella relativa alla modalita' (download/explore)
	if(m_nCrawlerMode==CRAWLER_DOWNLOAD_MODE)
	{
		// tabella per le miniature
		if(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_GENERATETHUMBNAILS_KEY))
			GenerateThumbnails(&html,pUrlService);
	}

	// tabella per le opzioni correnti
	if(m_nCrawlerMode!=CRAWLER_STREAMRIPPER_MODE)
	{
		html.Text("<br><br><p class=\"bold\">&nbsp;Settings</p>");
		htmltablelayout.border = 1;
		strcpyn(htmltablelayout.classname,"white",sizeof(htmltablelayout.classname));
		html.TableOpen(&htmltablelayout);

		strcpy(csslayout.tagname,"p");
		strcpy(csslayout.classname,"");
		csslayout.align = HTMLALIGN_LEFT;
		csslayout.width = 35;

		// per non ripeterlo su tutte le righe della tabella (basta sulla prima)
		csslayout.width = 0;

		// AddPictures
		html.TableOpenRow();
			strcpy(csslayout.classname,"xbold");
			html.TablePutCol("&nbsp;Add downloads to the playlist &nbsp;",&csslayout);
			_snprintf(szBuffer,sizeof(szBuffer)-1,"&nbsp;%s &nbsp;",m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_ADDPICTURES_KEY) ? "yes" : "no");
			strcpy(csslayout.classname,"");
			html.TablePutCol(szBuffer,&csslayout);
		html.TableCloseRow();

		// OnlyContent
		html.TableOpenRow();
			strcpy(csslayout.classname,"xbold");
			html.TablePutCol("&nbsp;Download only page content &nbsp;",&csslayout);
			_snprintf(szBuffer,sizeof(szBuffer)-1,"&nbsp;%s &nbsp;",m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_ONLYCONTENT_KEY) ? "yes" : "no");
			strcpy(csslayout.classname,"");
			html.TablePutCol(szBuffer,&csslayout);
		html.TableCloseRow();

		// SkipExisting
		html.TableOpenRow();
			strcpy(csslayout.classname,"xbold");
			html.TablePutCol("&nbsp;Skip existing file(s) (by date/time)&nbsp;",&csslayout);
			_snprintf(szBuffer,sizeof(szBuffer)-1,"&nbsp;%s &nbsp;",m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_SKIPEXISTING_KEY) ? "yes" : "no");
			strcpy(csslayout.classname,"");
			html.TablePutCol(szBuffer,&csslayout);
		html.TableCloseRow();

		// ForceSkipExisting
		html.TableOpenRow();
			strcpy(csslayout.classname,"xbold");
			html.TablePutCol("&nbsp;Force skip existing file(s) (by existence)&nbsp;",&csslayout);
			_snprintf(szBuffer,sizeof(szBuffer)-1,"&nbsp;%s &nbsp;",m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_SKIPEXISTING_FORCE_KEY) ? "yes" : "no");
			strcpy(csslayout.classname,"");
			html.TablePutCol(szBuffer,&csslayout);
		html.TableCloseRow();

		// <Min/Max>Size
		html.TableOpenRow();
			strcpy(csslayout.classname,"xbold");
			html.TablePutCol("&nbsp;Download limits (KB) &nbsp;",&csslayout);
			UINT nSize;
			char szMinSize[48];
			if((nSize = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_MINSIZE_KEY)) > 0)
			{
				char* pType = "";
				int nSizeType = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_MINSIZETYPE_KEY);
				if(nSizeType==SIZETYPE_BYTES)
					pType = "bytes";
				else if(nSizeType==SIZETYPE_KBYTES)
					pType = "KB";
				else if(nSizeType==SIZETYPE_MBYTES)
					pType = "MB";
				_snprintf(szMinSize,sizeof(szMinSize)-1,"%ld %s",nSize,pType);
			}
			else
				strcpyn(szMinSize,"[none]",sizeof(szMinSize));
			char szMaxSize[48];
			if((nSize = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_MAXSIZE_KEY)) > 0)
			{
				char* pType = "";
				int nSizeType = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_MAXSIZETYPE_KEY);
				if(nSizeType==SIZETYPE_BYTES)
					pType = "bytes";
				else if(nSizeType==SIZETYPE_KBYTES)
					pType = "KB";
				else if(nSizeType==SIZETYPE_MBYTES)
					pType = "MB";
				_snprintf(szMaxSize,sizeof(szMaxSize)-1,"%ld %s",nSize,pType);
			}
			else
				strcpyn(szMaxSize,"[none]",sizeof(szMaxSize));
			_snprintf(szBuffer,sizeof(szBuffer)-1,"&nbsp;min. %s, max. %s &nbsp;",szMinSize,szMaxSize);
			strcpy(csslayout.classname,"");
			html.TablePutCol(szBuffer,&csslayout);
		html.TableCloseRow();

		// ScriptMode
		html.TableOpenRow();
			strcpy(csslayout.classname,"xbold");
			html.TablePutCol("&nbsp;Use script file &nbsp;",&csslayout);
			_snprintf(szBuffer,sizeof(szBuffer)-1,"&nbsp;%s &nbsp;",m_bUseScript ? "yes" : "no");
			strcpy(csslayout.classname,"");
			html.TablePutCol(szBuffer,&csslayout);
		html.TableCloseRow();

		// QuietInScriptMode
		html.TableOpenRow();
			strcpy(csslayout.classname,"xbold");
			html.TablePutCol("&nbsp;Be quiet when in script mode &nbsp;",&csslayout);
			_snprintf(szBuffer,sizeof(szBuffer)-1,"&nbsp;%s &nbsp;",m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_QUIETINSCRIPTMODE_KEY) ? "yes" : "no");
			strcpy(csslayout.classname,"");
			html.TablePutCol(szBuffer,&csslayout);
		html.TableCloseRow();

		// Multithread
		html.TableOpenRow();
			strcpy(csslayout.classname,"xbold");
			html.TablePutCol("&nbsp;Use multithread with registered file types &nbsp;",&csslayout);
			_snprintf(szBuffer,sizeof(szBuffer)-1,"&nbsp;%s &nbsp;",m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_MULTITHREAD_KEY) ? "yes" : "no");
			strcpy(csslayout.classname,"");
			html.TablePutCol(szBuffer,&csslayout);
		html.TableCloseRow();
		
		int nFound;
		char szKey[REGKEY_MAX_KEY_NAME+1];

		htmllayout.font.type  = NULL;
		htmllayout.font.size  = 0;
		htmllayout.font.color = NULL;
		htmllayout.align      = HTMLALIGN_NONE;
		htmllayout.style      = HTMLSTYLE_NONE;
		htmllayout.border     = 0;
		htmllayout.width      = 0;

		// FileTypeList
		html.TableOpenRow();
			strcpy(csslayout.classname,"xbold");
			html.TablePutCol("&nbsp;Registered file types &nbsp;",&csslayout);
			strcpy(csslayout.classname,"");
			html.TableOpenCol();
			html.TablePutColText("<p>",&htmllayout);
			// carica dalla configurazione i tipi di file riconosciuti
			for(i = 0,nFound = 0;; i++)
			{
				_snprintf(szKey,sizeof(szKey)-1,"%s%d",WALLPAPER_FILETYPE_KEY,i);
				if(strcmp((p = (char*)m_pConfig->GetString(WALLPAPER_FILETYPELIST_KEY,szKey)),"")!=0)
				{	
					if(nFound <= 0)
						html.TablePutColText("&nbsp;",&htmllayout);
					else
						html.TablePutColText(", ",&htmllayout);
					html.TablePutColText(p,&htmllayout);
					nFound++;
				}
				else
					break;
			}
			// nessun tipo file presente, carica con i tipi di default
			if(nFound==0)
			{
				LPIMAGETYPE p;
				while((p = m_pImage->EnumReadableImageFormats())!=(LPIMAGETYPE)NULL)
				{
					if(nFound <= 0)
						html.TablePutColText("&nbsp;",&htmllayout);
					else
						html.TablePutColText(", ",&htmllayout);
					html.TablePutColText(p->ext,&htmllayout);
					nFound++;
				}
			}
			html.TablePutColText("</p>",&htmllayout);
			html.TableCloseCol();
		html.TableCloseRow();

		// WildcardsExcludeList
		html.TableOpenRow();
			strcpy(csslayout.classname,"xbold");
			html.TablePutCol("&nbsp;Wildcards exclude list &nbsp;",&csslayout);
			strcpy(csslayout.classname,"");
			html.TableOpenCol();
			html.TablePutColText("<p>",&htmllayout);
			// carica dalla configurazione la lista delle wildcards
			for(i = 0,nFound = 0;; i++)
			{
				_snprintf(szKey,sizeof(szKey)-1,"%s%d",WALLPAPER_WILDCARDSEXCLUDE_KEY,i);
				if(strcmp((p = (char*)m_pConfig->GetString(WALLPAPER_WILDCARDSEXCLUDELIST_KEY,szKey)),"")!=0)
				{
					if(nFound <= 0)
						html.TablePutColText("&nbsp;",&htmllayout);
					else
						html.TablePutColText(", ",&htmllayout);
					html.TablePutColText(p,&htmllayout);
					nFound++;
				}
				else
					break;
			}
			if(nFound==0)
				html.TablePutColText("&nbsp;none",&htmllayout);
			html.TablePutColText("</p>",&htmllayout);
			html.TableCloseCol();
		html.TableCloseRow();

		// DomainAcceptance
		html.TableOpenRow();
			strcpy(csslayout.classname,"xbold");
			html.TablePutCol("&nbsp;Domain acceptance &nbsp;",&csslayout);
			strcpy(szBuffer,"&nbsp;");
			switch(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_DOMAINACCEPTANCE_KEY))
			{
				case DOMAINACCEPTANCE_EXCLUDE_THIS:
					strcatn(szBuffer,"exclude this &nbsp;",sizeof(szBuffer));
					break;
				case DOMAINACCEPTANCE_EXCLUDE_ALL_OUTSIDE:
					strcatn(szBuffer,"exclude all outside the current domain &nbsp;",sizeof(szBuffer));
					break;
				case DOMAINACCEPTANCE_EXCLUDE_ALL_OUTSIDE_EXCEPT_PICTURES:
					strcatn(szBuffer,"exclude all outside the current domain except pictures &nbsp;",sizeof(szBuffer));
					break;
				case DOMAINACCEPTANCE_EXCLUDE_ALL_OUTSIDE_EXCEPT_REGFILETYPES:
					strcatn(szBuffer,"exclude all outside the current domain except registered file types &nbsp;",sizeof(szBuffer));
					break;
				case DOMAINACCEPTANCE_EXCLUDE_ALL_FROM_THIS:
					strcatn(szBuffer,"exclude all from this domain &nbsp;",sizeof(szBuffer));
					break;
				case DOMAINACCEPTANCE_FOLLOW_THIS:
					strcatn(szBuffer,"follow this &nbsp;",sizeof(szBuffer));
					break;
				case DOMAINACCEPTANCE_FOLLOW_ALL_OUTSIDE:
					strcatn(szBuffer,"follow all outside the current domain &nbsp;",sizeof(szBuffer));
					break;
				case DOMAINACCEPTANCE_FOLLOW_ALL_FROM_THIS:
					strcatn(szBuffer,"follow all from this domain &nbsp;",sizeof(szBuffer));
					break;
				case DOMAINACCEPTANCE_ASK:
				default:
					strcatn(szBuffer,"ask &nbsp;",sizeof(szBuffer));
					break;
			}
			strcpy(csslayout.classname,"");
			html.TablePutCol(szBuffer,&csslayout);
		html.TableCloseRow();

		// DomainIncludeList
		html.TableOpenRow();
			strcpy(csslayout.classname,"xbold");
			html.TablePutCol("&nbsp;Domains include list &nbsp;",&csslayout);
			strcpy(csslayout.classname,"");
			html.TableOpenCol();
			html.TablePutColText("<p>",&htmllayout);
			// carica dalla configurazione la lista delle inclusioni
			for(i = 0,nFound = 0;; i++)
			{
				_snprintf(szKey,sizeof(szKey)-1,"%s%d",WALLPAPER_DOMAININCLUDE_KEY,i);
				if(strcmp((p = (char*)m_pConfig->GetString(WALLPAPER_DOMAININCLUDELIST_KEY,szKey)),"")!=0)
				{
					if(nFound <= 0)
						html.TablePutColText("&nbsp;",&htmllayout);
					else
						html.TablePutColText(", ",&htmllayout);
					_snprintf(szBuffer,
							sizeof(szBuffer)-1,
							"<a href=\"%s\" target=\"_blank\">%s</a>",
							p,
							p);
					html.TablePutColText(szBuffer,&htmllayout);
					nFound++;
				}
				else
					break;
			}
			if(nFound==0)
				html.TablePutColText("&nbsp;none",&htmllayout);
			html.TablePutColText("</p>",&htmllayout);
			html.TableCloseCol();
		html.TableCloseRow();

		// DomainExcludeList
		html.TableOpenRow();
			strcpy(csslayout.classname,"xbold");
			html.TablePutCol("&nbsp;Domains exclude list &nbsp;",&csslayout);
			strcpy(csslayout.classname,"");
			html.TableOpenCol();
			html.TablePutColText("<p>",&htmllayout);
			// carica dalla configurazione la lista delle esclusioni
			for(i = 0,nFound = 0;; i++)
			{
				_snprintf(szKey,sizeof(szKey)-1,"%s%d",WALLPAPER_DOMAINEXCLUDE_KEY,i);
				if(strcmp((p = (char*)m_pConfig->GetString(WALLPAPER_DOMAINEXCLUDELIST_KEY,szKey)),"")!=0)
				{
					if(nFound <= 0)
						html.TablePutColText("&nbsp;",&htmllayout);
					else
						html.TablePutColText(", ",&htmllayout);
					_snprintf(szBuffer,
							sizeof(szBuffer)-1,
							"<a href=\"%s\" target=\"_blank\">%s</a>",
							p,
							p);
					html.TablePutColText(szBuffer,&htmllayout);
					nFound++;
				}
				else
					break;
			}
			if(nFound==0)
				html.TablePutColText("&nbsp;none",&htmllayout);
			html.TablePutColText("</p>",&htmllayout);
			html.TableCloseCol();
		html.TableCloseRow();

		// ParentAcceptance
		html.TableOpenRow();
			strcpy(csslayout.classname,"xbold");
			html.TablePutCol("&nbsp;Parent url acceptance &nbsp;",&csslayout);
			strcpy(szBuffer,"&nbsp;");
			switch(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_PARENTACCEPTANCE_KEY))
			{
				case PARENTACCEPTANCE_EXCLUDE_ALL_OUTSIDE:
					strcatn(szBuffer,"no parents &nbsp;",sizeof(szBuffer));
					break;
				case PARENTACCEPTANCE_EXCLUDE_ALL_OUTSIDE_EXCEPT_PICTURES:
					strcatn(szBuffer,"no parents except pictures &nbsp;",sizeof(szBuffer));
					break;
				case PARENTACCEPTANCE_EXCLUDE_ALL_OUTSIDE_EXCEPT_REGFILETYPES:
					strcatn(szBuffer,"no parents except registered file types &nbsp;",sizeof(szBuffer));
					break;
				case PARENTACCEPTANCE_FOLLOW_ALL_OUTSIDE:
					strcatn(szBuffer,"include parents &nbsp;",sizeof(szBuffer));
					break;
				case PARENTACCEPTANCE_ASK:
				default:
					strcatn(szBuffer,"ask &nbsp;",sizeof(szBuffer));
					break;
			}
			strcpy(csslayout.classname,"");
			html.TablePutCol(szBuffer,&csslayout);
		html.TableCloseRow();

		// ParentIncludeList
		html.TableOpenRow();
			strcpy(csslayout.classname,"xbold");
			html.TablePutCol("&nbsp;Parent url include list &nbsp;",&csslayout);
			strcpy(csslayout.classname,"");
			html.TableOpenCol();
			html.TablePutColText("<p>",&htmllayout);
			// carica dalla configurazione la lista delle inclusioni
			for(i = 0,nFound = 0;; i++)
			{
				_snprintf(szKey,sizeof(szKey)-1,"%s%d",WALLPAPER_PARENTINCLUDE_KEY,i);
				if((p = (char*)m_pConfig->GetString(WALLPAPER_PARENTINCLUDELIST_KEY,szKey))!=NULL)
				if(strcmp((p = (char*)m_pConfig->GetString(WALLPAPER_PARENTINCLUDELIST_KEY,szKey)),"")!=0)
				{
					if(nFound <= 0)
						html.TablePutColText("&nbsp;",&htmllayout);
					else
						html.TablePutColText(", ",&htmllayout);
					html.TablePutColText(p,&htmllayout);
					nFound++;
				}
				else
					break;
			}
			if(nFound==0)
				html.TablePutColText("&nbsp;none",&htmllayout);
			html.TablePutColText("</p>",&htmllayout);
			html.TableCloseCol();
		html.TableCloseRow();

		// ParentExcludeList
		html.TableOpenRow();
			strcpy(csslayout.classname,"xbold");
			html.TablePutCol("&nbsp;Parent url exclude list &nbsp;",&csslayout);
			strcpy(csslayout.classname,"");
			html.TableOpenCol();
			html.TablePutColText("<p>",&htmllayout);
			// carica dalla configurazione la lista delle esclusioni
			for(i = 0,nFound = 0;; i++)
			{
				_snprintf(szKey,sizeof(szKey)-1,"%s%d",WALLPAPER_PARENTEXCLUDE_KEY,i);
				if(strcmp((p = (char*)m_pConfig->GetString(WALLPAPER_PARENTEXCLUDELIST_KEY,szKey)),"")!=0)
				{
					if(nFound <= 0)
						html.TablePutColText("&nbsp;",&htmllayout);
					else
						html.TablePutColText(", ",&htmllayout);
					html.TablePutColText(p,&htmllayout);
					nFound++;
				}
				else
					break;
			}
			if(nFound==0)
				html.TablePutColText("&nbsp;none",&htmllayout);
			html.TablePutColText("</p>",&htmllayout);
			html.TableCloseCol();
		html.TableCloseRow();

		// ParentUrlUseAlways
		html.TableOpenRow();
			strcpy(csslayout.classname,"xbold");
			html.TablePutCol("&nbsp;Use include list with the 'download only page content' also &nbsp;",&csslayout);
			_snprintf(szBuffer,sizeof(szBuffer)-1,"&nbsp;%s &nbsp;",m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_PARENTURLUSEALWAYS_KEY) ? "yes" : "no");
			strcpy(csslayout.classname,"");
			html.TablePutCol(szBuffer,&csslayout);
		html.TableCloseRow();

		// SaveIncludeExcludeList
		html.TableOpenRow();
			strcpy(csslayout.classname,"xbold");
			html.TablePutCol("&nbsp;Save include/exclude list during crawl when in ask mode &nbsp;",&csslayout);
			_snprintf(szBuffer,sizeof(szBuffer)-1,"&nbsp;%s &nbsp;",m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_SAVEINCLEXCLLIST_KEY) ? "yes" : "no");
			strcpy(csslayout.classname,"");
			html.TablePutCol(szBuffer,&csslayout);
		html.TableCloseRow();

		// FollowCGI
		html.TableOpenRow();
			strcpy(csslayout.classname,"xbold");
			html.TablePutCol("&nbsp;Follow CGI links &nbsp;",&csslayout);
			_snprintf(szBuffer,sizeof(szBuffer)-1,"&nbsp;%s &nbsp;",m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_ENABLECGI_KEY) ? "yes" : "no");
			strcpy(csslayout.classname,"");
			html.TablePutCol(szBuffer,&csslayout);
		html.TableCloseRow();

		// FollowJavascript
		html.TableOpenRow();
			strcpy(csslayout.classname,"xbold");
			html.TablePutCol("&nbsp;Follow Javascript links &nbsp;",&csslayout);
			_snprintf(szBuffer,sizeof(szBuffer)-1,"&nbsp;%s &nbsp;",m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_ENABLEJAVASCRIPT_KEY) ? "yes" : "no");
			strcpy(csslayout.classname,"");
			html.TablePutCol(szBuffer,&csslayout);
		html.TableCloseRow();

		// ConnectionTimeout
		html.TableOpenRow();
			strcpy(csslayout.classname,"xbold");
			html.TablePutCol("&nbsp;Connection timeout &nbsp;",&csslayout);
			_snprintf(szBuffer,sizeof(szBuffer)-1,"&nbsp;%d %s &nbsp;",m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_CONNECTIONRETRY_KEY),m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_CONNECTIONRETRY_KEY) > 1 ? "retries" : "retry");
			strcpy(csslayout.classname,"");
			html.TablePutCol(szBuffer,&csslayout);
		html.TableCloseRow();

		// DownloadTimeout
		html.TableOpenRow();
			strcpy(csslayout.classname,"xbold");
			html.TablePutCol("&nbsp;Download timeout &nbsp;",&csslayout);
			_snprintf(szBuffer,sizeof(szBuffer)-1,"&nbsp;%d secs &nbsp;",m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_CONNECTIONTIMEOUT_KEY));
			strcpy(csslayout.classname,"");
			html.TablePutCol(szBuffer,&csslayout);
		html.TableCloseRow();

		// UseWinsockTimeout
		html.TableOpenRow();
			strcpy(csslayout.classname,"xbold");
			html.TablePutCol("&nbsp;Use Winsock timeout &nbsp;",&csslayout);
			_snprintf(szBuffer,sizeof(szBuffer)-1,"&nbsp;%s &nbsp;",m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_USEWINSOCKTIMEOUT_KEY) ? "yes" : "no");
			strcpy(csslayout.classname,"");
			html.TablePutCol(szBuffer,&csslayout);
		html.TableCloseRow();

		// UseProxy
		html.TableOpenRow();
			strcpy(csslayout.classname,"xbold");
			html.TablePutCol("&nbsp;Connect through a proxy &nbsp;",&csslayout);
			_snprintf(szBuffer,sizeof(szBuffer)-1,"&nbsp;%s &nbsp;",m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_USEPROXY_KEY) ? "yes" : "no");
			strcpy(csslayout.classname,"");
			html.TablePutCol(szBuffer,&csslayout);
		html.TableCloseRow();
		if(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_USEPROXY_KEY))
		{
			html.TableOpenRow();
				strcpy(csslayout.classname,"xbold");
				html.TablePutCol("&nbsp;Proxy settings &nbsp;",&csslayout);
				_snprintf(szBuffer,sizeof(szBuffer)-1,"&nbsp;address: %s, port number: %ld &nbsp;",m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_PROXY_ADDRESS_KEY),m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PROXY_PORTNUMBER_KEY));
				strcpy(csslayout.classname,"");
				html.TablePutCol(szBuffer,&csslayout);
			html.TableCloseRow();
		}

		// EnableRobotsTxt
		html.TableOpenRow();
			strcpy(csslayout.classname,"xbold");
			html.TablePutCol("&nbsp;Use robots.txt rules &nbsp;",&csslayout);
			_snprintf(szBuffer,sizeof(szBuffer)-1,"&nbsp;%s &nbsp;",m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_ENABLEROBOTSTXT_KEY) ? "yes" : "no");
			strcpy(csslayout.classname,"");
			html.TablePutCol(szBuffer,&csslayout);
		html.TableCloseRow();

		// AllowCookies
		html.TableOpenRow();
			strcpy(csslayout.classname,"xbold");
			html.TablePutCol("&nbsp;Allow Cookies &nbsp;",&csslayout);
			_snprintf(szBuffer,sizeof(szBuffer)-1,"&nbsp;%s &nbsp;",m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_COOKIES_ALLOWED_KEY) ? "yes" : "no");
			strcpy(csslayout.classname,"");
			html.TablePutCol(szBuffer,&csslayout);
		html.TableCloseRow();

		// CookiesWarningOnAccept
		html.TableOpenRow();
			strcpy(csslayout.classname,"xbold");
			html.TablePutCol("&nbsp;Warning on accepting cookies &nbsp;",&csslayout);
			_snprintf(szBuffer,sizeof(szBuffer)-1,"&nbsp;%s &nbsp;",m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_COOKIES_WARNINGONACCEPT_KEY) ? "yes" : "no");
			strcpy(csslayout.classname,"");
			html.TablePutCol(szBuffer,&csslayout);
		html.TableCloseRow();

		// CookiesWarningOnSend
		html.TableOpenRow();
			strcpy(csslayout.classname,"xbold");
			html.TablePutCol("&nbsp;Warning on sending cookies &nbsp;",&csslayout);
			_snprintf(szBuffer,sizeof(szBuffer)-1,"&nbsp;%s &nbsp;",m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_COOKIES_WARNINGONSEND_KEY) ? "yes" : "no");
			strcpy(csslayout.classname,"");
			html.TablePutCol(szBuffer,&csslayout);
		html.TableCloseRow();

		// ReportName
		html.TableOpenRow();
			strcpy(csslayout.classname,"xbold");
			html.TablePutCol("&nbsp;This report &nbsp;",&csslayout);
			_snprintf(szBuffer,sizeof(szBuffer)-1,"&nbsp;%s &nbsp;",lpcszReportName);
			strcpy(csslayout.classname,"");
			html.TablePutCol(szBuffer,&csslayout);
		html.TableCloseRow();

		// ReportOrder
		html.TableOpenRow();
			strcpy(csslayout.classname,"xbold");
			html.TablePutCol("&nbsp;Report sorted by &nbsp;",&csslayout);
			char* pIndex;
			switch(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_REPORTORDER_KEY))
			{
				case REPORT_BY_ID:
					pIndex = "ID";
					break;
				case REPORT_BY_PARENT_URL:
					pIndex = "parent url";
					break;
				case REPORT_BY_URL:
					pIndex = "url";
					break;
				case REPORT_BY_STAT:
					pIndex = "status";
					break;
				default:
					pIndex = "?";
					break;
			}
			_snprintf(szBuffer,sizeof(szBuffer)-1,"&nbsp;%s &nbsp;",pIndex);
			strcpy(csslayout.classname,"");
			html.TablePutCol(szBuffer,&csslayout);
		html.TableCloseRow();

		// GenerateAscii
		html.TableOpenRow();
			strcpy(csslayout.classname,"xbold");
			html.TablePutCol("&nbsp;Generate ASCII report &nbsp;",&csslayout);
			if(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_GENERATEASCII_KEY))
				_snprintf(szBuffer,sizeof(szBuffer)-1,"&nbsp;yes (%s) &nbsp;",lpcszAsciiReportName);
			else
				strcpyn(szBuffer,"&nbsp;no &nbsp;",sizeof(szBuffer));
			strcpy(csslayout.classname,"");
			html.TablePutCol(szBuffer,&csslayout);
		html.TableCloseRow();

		// GenerateThumbnails
		html.TableOpenRow();
			strcpy(csslayout.classname,"xbold");
			html.TablePutCol("&nbsp;Generate thumbnails &nbsp;",&csslayout);
			_snprintf(szBuffer,sizeof(szBuffer)-1,"&nbsp;%s &nbsp;",m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_GENERATETHUMBNAILS_KEY) ? "yes" : "no");
			strcpy(csslayout.classname,"");
			html.TablePutCol(szBuffer,&csslayout);
		html.TableCloseRow();

		// DumpDatabase
		html.TableOpenRow();
			strcpy(csslayout.classname,"xbold");
			html.TablePutCol("&nbsp;Dump database content &nbsp;",&csslayout);
			_snprintf(szBuffer,sizeof(szBuffer)-1,"&nbsp;%s &nbsp;",m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_DUMPDATABASE_KEY) ? "yes" : "no");
			strcpy(csslayout.classname,"");
			html.TablePutCol(szBuffer,&csslayout);
		html.TableCloseRow();

		// ExtractText
		html.TableOpenRow();
			strcpy(csslayout.classname,"xbold");
			html.TablePutCol("&nbsp;Extract text (.txt) from html files &nbsp;",&csslayout);
			_snprintf(szBuffer,sizeof(szBuffer)-1,"&nbsp;%s &nbsp;",m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_EXTRACTTEXT_KEY) ? "yes" : "no");
			strcpy(csslayout.classname,"");
			html.TablePutCol(szBuffer,&csslayout);
		html.TableCloseRow();

		// ExtractJavascript
		html.TableOpenRow();
			strcpy(csslayout.classname,"xbold");
			html.TablePutCol("&nbsp;Extract javascript (.js) from html files &nbsp;",&csslayout);
			_snprintf(szBuffer,sizeof(szBuffer)-1,"&nbsp;%s &nbsp;",m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_EXTRACTJAVASCRIPT_KEY) ? "yes" : "no");
			strcpy(csslayout.classname,"");
			html.TablePutCol(szBuffer,&csslayout);
		html.TableCloseRow();

		// DownloadDir
		html.TableOpenRow();
			strcpy(csslayout.classname,"xbold");
			html.TablePutCol("&nbsp;Download directory &nbsp;",&csslayout);
			html.TableOpenCol();
			strcpy(csslayout.classname,"");
			html.TablePutColText("&nbsp;",&htmllayout);
			url.LocalFileToUrl(m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_DOWNLOADDIR_KEY),szLocalFile,sizeof(szLocalFile));
			_snprintf(szBuffer,
					sizeof(szBuffer)-1,
					"<a href=\"%s\" target=\"_blank\">%s</a>",
					szLocalFile,
					m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_DOWNLOADDIR_KEY)
					);
			html.TablePutColText(szBuffer,&htmllayout);
			html.TablePutColText(" &nbsp;",&htmllayout);
			html.TableCloseCol();
		html.TableCloseRow();

		html.TableClose();
	}

	// DumpDatabase
	if(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_DUMPDATABASE_KEY))
	{
		html.Text("<br><br><p class=\"bold\">&nbsp;Database content:</p>");
		
		html.FormattedText("<p class=\"bold\">&nbsp;%s.db<br>(id,&nbsp;parenturl,&nbsp;url,&nbsp;stat,&nbsp;file,&nbsp;size,&nbsp;seconds)</p>",pUrlTable->GetTableName());
		htmltablelayout.border = 1;
		strcpyn(htmltablelayout.classname,"white",sizeof(htmltablelayout.classname));
		html.TableOpen(&htmltablelayout);

		pUrlTable->SetIndex(URL_IDX_URL);
		if(pUrlTable->GoTop())
		{
			while(!pUrlTable->Eof())
			{
				pUrlTable->ScatterMemvars();

				html.TableOpenRow();
				strcpy(csslayout.classname,"");
				html.TablePutCol(pUrlTable->GetRecordAsString(),&csslayout);
				html.TableCloseRow();

				pUrlTable->GetNext();
			}
		}

		html.TableClose();

		html.FormattedText("<br><p class=\"bold\">&nbsp;%s.db<br>(url,&nbsp;date,&nbsp;starttime,&nbsp;endtime,&nbsp;totaltime,&nbsp;downloadtime,&nbsp;downloaded,&nbsp;totalurls,&nbsp;totalpict,&nbsp;downloadedurls,&nbsp;downloadedpict,&nbsp;datatable,&nbsp;report,&nbsp;status,&nbsp;id)</p>",pUrlDatabaseTable->GetTableName());
		htmltablelayout.border = 1;
		strcpyn(htmltablelayout.classname,"white",sizeof(htmltablelayout.classname));
		html.TableOpen(&htmltablelayout);

		pUrlDatabaseTable->SetIndex(URLDATABASE_IDX_URL);
		if(pUrlDatabaseTable->GoTop())
		{
			while(!pUrlDatabaseTable->Eof())
			{
				pUrlDatabaseTable->ScatterMemvars();

				html.TableOpenRow();
				strcpy(csslayout.classname,"");
				html.TablePutCol(pUrlDatabaseTable->GetRecordAsString(),&csslayout);
				html.TableCloseRow();

				pUrlDatabaseTable->GetNext();
			}
		}

		html.TableClose();
	}

	// footer
	_snprintf(szBuffer,sizeof(szBuffer)-1,WALLPAPER_AUTHOR_HTML_COPYRIGHT,m_pConfig->GetString(WALLPAPER_INSTALL_KEY,WALLPAPER_AUTHOREMAIL_KEY));
	html.FormattedLine(	"<br><br>"
					"<p>"
					"<a href=\"%s\" target=\"_blank\">%s</a>"
					"<br>"
					"%s"
					"</p>",
					m_pConfig->GetString(WALLPAPER_INSTALL_KEY,WALLPAPER_WEBSITE_KEY),
					WALLPAPER_PROGRAM_NAME_ALIAS" - "WALLPAPER_PROGRAM_DESCRIPTION" - v."WALLPAPER_VERSION,
					szBuffer
					);
	html.Footer();
	html.Close();

	// apre il report
	if(!m_bUseScript && m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_OPENREPORT_KEY))
	{
		// lancia il browser
		if(::FileExist(lpcszReportName))
		{
			CBrowser browser(m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_PREFERRED_INTERNET_BROWSER_KEY));
			_snprintf(szBuffer,sizeof(szBuffer)-1,"Opening report:\r\n%s...",lpcszReportName);
			SetDlgItemText(IDC_STATIC_STATUS,szBuffer);
			browser.Browse(lpcszReportName);
		}
	}

done:

	// interfaccia utente
	SetDlgItemText(IDC_STATIC_STATUS,"");
	GetDlgItem(IDCANCEL)->EnableWindow(TRUE);
}

/*
	GenerateHtmlReportForScriptFile()

	Genera il report relativo al download effettuato tramite file script.
*/
void CWallPaperCrawlerDlg::GenerateHtmlReportForScriptFile(void)
{
	char* p;
	CUrl url;
	ITEM* item;
	CUrlStatus urlstatus;
	CUrlStatus::URL_STATUS url_status;
	char szUrl[MAX_URL+1];
	char szCrawledUrl[MAX_URL+1];
	char szDecodedUrl[MAX_URL+1];
	char szBuffer[1024];
	char szLocalFile[_MAX_PATH+1];
	char szIcon[(_MAX_PATH*2)+1];
	char szHtmlScriptReport[_MAX_PATH+1];
	CHtml html;
	ITERATOR iter;

	// imposta il layout per il report
	HTMLLAYOUT htmllayout;
	htmllayout.font.type  = "verdana";
	htmllayout.font.size  = 1;
	htmllayout.font.color = NULL;
	htmllayout.align      = HTMLALIGN_NONE;
	htmllayout.style      = HTMLSTYLE_NONE;
	htmllayout.border     = 0;
	htmllayout.width      = 0;

	HTMLTABLELAYOUT htmltablelayout;
	memcpy(&(htmltablelayout.htmllayout),&htmllayout,sizeof(HTMLLAYOUT));
	htmltablelayout.title       = NULL;
	htmltablelayout.border      = 1;
	htmltablelayout.cellpadding = 1;
	htmltablelayout.cellspacing = 1;
	htmltablelayout.width       = 100;
	strcpyn(htmltablelayout.classname,"",sizeof(htmltablelayout.classname));

	CSSLAYOUT csslayout = {"","",HTMLALIGN_NONE,0};

	// apre il database
	CUrlDatabaseService	UrlDatabaseService(URLDATABASE_TABLE,m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_DATADIR_KEY));
	CUrlDatabaseTable* pUrlDatabaseTable = UrlDatabaseService.GetTable();

	// carica la lista delle url contenute nel file script
	if(!LoadUrlsFromScript(m_szScriptFile,&m_listUrls))
		goto done;

	// costruisce il nome per il report html
	p = strrchr(m_szScriptFile,'\\');
	if(p && *(p+1))
		p++;
	else
		p = m_szScriptFile;
	_snprintf(szHtmlScriptReport,
			sizeof(szHtmlScriptReport)-1,
			"%sreport.%s",
			m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_REPORTSDIR_KEY),
			p
			);
	url.EnsureValidFileName(szHtmlScriptReport,sizeof(szHtmlScriptReport),FALSE,DEFAULT_HTML_EXT);

	// interfaccia utente
	GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
	_snprintf(szBuffer,sizeof(szBuffer)-1,"Generating report:\r\n%s...",szHtmlScriptReport);
	SetDlgItemText(IDC_STATIC_STATUS,szBuffer);

	// crea il file html per il report
	if(!html.Open(szHtmlScriptReport))
	{
		if(m_bUseScript && m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_QUIETINSCRIPTMODE_KEY))
			;
		else
			::MessageBoxResourceEx(this->m_hWnd,MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_OPENING_HTML_REPORT,szHtmlScriptReport);
		goto done;
	}
	
	// meta tags
	_snprintf(szBuffer,
			sizeof(szBuffer)-1,
			"<meta name=\"generator\" content=\"%s, %s\">"
			"\r\n"
			"<link rel=\"stylesheet\" href=\"style.css\" type=\"text/css\">"
			,
			WALLPAPER_PROGRAM_TITLE,
			m_pConfig->GetString(WALLPAPER_INSTALL_KEY,WALLPAPER_WEBSITE_KEY)
			);
	html.Header(m_szUrl,"<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 3.2//EN\">",szBuffer);

	// tabella per l'intestazione, composta da 1 riga e 2 colonne (una per l'icona e l'altra per i totali)
	htmltablelayout.border = 0;
	strcpyn(htmltablelayout.classname,"",sizeof(htmltablelayout.classname));
	html.TableOpen(&htmltablelayout);
	html.TableOpenRow();
	
	// colonna per l'icona
	_snprintf(szBuffer,sizeof(szBuffer)-1,"%s%s",m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_BITMAPSDIR_KEY),"wallpaper.gif");
	url.LocalFileToUrl(szBuffer,szLocalFile,sizeof(szLocalFile));
	_snprintf(szBuffer,
			sizeof(szBuffer)-1,
			"<br><a href=\"%s\" target=\"_blank\"><img src=\"%s\" border=0><br>%s</a><br><br>",
			m_pConfig->GetString(WALLPAPER_INSTALL_KEY,WALLPAPER_WEBSITE_KEY),
			szLocalFile,
			WALLPAPER_PROGRAM_NAME_ALIAS" - "WALLPAPER_PROGRAM_DESCRIPTION" - v."WALLPAPER_VERSION
			);
	strcpy(csslayout.tagname,"p");
	strcpy(csslayout.classname,"");
	csslayout.align = HTMLALIGN_CENTER;
	csslayout.width = 20;
	html.TablePutCol(szBuffer,&csslayout);

	// colonna per i totali
	html.TableOpenCol();
	html.TableOpen();
	
	// script
	html.TableOpenRow();
		htmllayout.align = HTMLALIGN_LEFT;
		htmllayout.style = HTMLSTYLE_BOLD;
		htmllayout.width = 13;
		html.TablePutCol("&nbsp;script file",&htmllayout);
		_snprintf(szBuffer,sizeof(szBuffer)-1,"<a href=\"%s\" target=\"_blank\">%s</a>",m_szScriptFile,m_szScriptFile);
		htmllayout.align = HTMLALIGN_LEFT;
		htmllayout.style = HTMLSTYLE_NORMAL;
		htmllayout.width = 0;
		html.TablePutCol(szBuffer,&htmllayout);
	html.TableCloseRow();

	// start time
	html.TableOpenRow();
		htmllayout.align = HTMLALIGN_LEFT;
		htmllayout.style = HTMLSTYLE_BOLD;
		htmllayout.width = 13;
		html.TablePutCol("&nbsp;start time",&htmllayout);
		htmllayout.align = HTMLALIGN_LEFT;
		htmllayout.style = HTMLSTYLE_NORMAL;
		htmllayout.width = 0;
		html.TablePutCol(m_szScriptStartTime,&htmllayout);
	html.TableCloseRow();

	// end time
	html.TableOpenRow();
		htmllayout.align = HTMLALIGN_LEFT;
		htmllayout.style = HTMLSTYLE_BOLD;
		htmllayout.width = 13;
		html.TablePutCol("&nbsp;end time",&htmllayout);
		htmllayout.align = HTMLALIGN_LEFT;
		htmllayout.style = HTMLSTYLE_NORMAL;
		htmllayout.width = 0;
		html.TablePutCol(m_szScriptEndTime,&htmllayout);
	html.TableCloseRow();

	// urls
	html.TableOpenRow();
		htmllayout.align = HTMLALIGN_LEFT;
		htmllayout.style = HTMLSTYLE_BOLD;
		htmllayout.width = 13;
		html.TablePutCol("&nbsp;urls",&htmllayout);
		_snprintf(szBuffer,sizeof(szBuffer)-1,"%ld",m_listUrls.Count());
		htmllayout.align = HTMLALIGN_LEFT;
		htmllayout.style = HTMLSTYLE_NORMAL;
		htmllayout.width = 0;
		html.TablePutCol(szBuffer,&htmllayout);
	html.TableCloseRow();

	html.TableClose();
	html.TableCloseCol();
	html.TableCloseRow();
	html.TableClose();

	html.NewLine();

	// tabella per il download
	htmltablelayout.border = 1;
	strcpyn(htmltablelayout.classname,"white",sizeof(htmltablelayout.classname));
	html.TableOpen(&htmltablelayout);
	
	// intestazioni per le colonne
	html.TableOpenRow();
		htmllayout.style = HTMLSTYLE_BOLD;
		htmllayout.width = 0;
		htmllayout.align = HTMLALIGN_LEFT;
		html.TablePutCol("&nbsp;Url",&htmllayout);
		htmllayout.align = HTMLALIGN_LEFT;
		html.TablePutCol("&nbsp;Report",&htmllayout);
		htmllayout.align = HTMLALIGN_LEFT;
		html.TablePutCol("&nbsp;Status",&htmllayout);
	html.TableCloseRow();

	htmllayout.style = HTMLSTYLE_NORMAL;

	// per ognuna delle url presenti nel database per i downloads
	if((iter = m_listUrls.First())!=(ITERATOR)NULL)
		do
		{
			item = (ITEM*)iter->data;
		
			if(item)
			{
				strcpyn(szCrawledUrl,item->item,sizeof(szCrawledUrl));

				// interfaccia utente
				::PeekAndPump();

				// cerca l'url nel database, ricavando i dati relativi
				if(pUrlDatabaseTable->Lock(SYNC_3_SECS_TIMEOUT))
				{
					if(pUrlDatabaseTable->Seek(szCrawledUrl,URLDATABASE_IDX_URL))
					{
						// interfaccia utente
						::PeekAndPump();

						// nuova riga
						html.TableOpenRow();
						memset(szBuffer,'\0',sizeof(szBuffer));

						// carica i valori del record
						pUrlDatabaseTable->ScatterMemvars();
						url_status = (CUrlStatus::URL_STATUS)pUrlDatabaseTable->GetField_Status();
						urlstatus.GetStatus(url_status);

						// url
						strcpyn(szIcon,m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_BITMAPSDIR_KEY),sizeof(szIcon));
						switch(url_status)
						{
							case CUrlStatus::URL_STATUS_DONE:
							case CUrlStatus::URL_STATUS_DOWNLOADED:
							case CUrlStatus::URL_STATUS_MOVED:
							case CUrlStatus::URL_STATUS_NOT_MODIFIED:
							{
								if(url.IsHtmlFile(pUrlDatabaseTable->GetField_Url()))
								{
									strcatn(szIcon,"url.gif",sizeof(szIcon));
								}
								else
								{
									if(CImage::IsImageFile(pUrlDatabaseTable->GetField_Url()))
										strcatn(szIcon,"picture.gif",sizeof(szIcon));
									else
										strcatn(szIcon,"urldir.gif",sizeof(szIcon));
								}
								break;
							}
							case CUrlStatus::URL_STATUS_CANCELLED:
							case CUrlStatus::URL_STATUS_CGI_URL:
							case CUrlStatus::URL_STATUS_WINSOCK_ERROR:
							case CUrlStatus::URL_STATUS_HTTP_ERROR:
							case CUrlStatus::URL_STATUS_INCOMPLETE:
							case CUrlStatus::URL_STATUS_UNKNOWN:
								strcatn(szIcon,"urlnotfound.gif",sizeof(szIcon));
								break;
							case CUrlStatus::URL_STATUS_EXCLUDED_BY_ROBOTSTXT:
							case CUrlStatus::URL_STATUS_EXCLUDED_BY_SIZE:
							case CUrlStatus::URL_STATUS_EXCLUDED_BY_WILDCARDS:
								strcatn(szIcon,"filter.gif",sizeof(szIcon));
								break;
							case CUrlStatus::URL_STATUS_EXTERNAL_DOMAIN:
								strcatn(szIcon,"netserver.gif",sizeof(szIcon));
								break;
							case CUrlStatus::URL_STATUS_PARENT_URL:
								strcatn(szIcon,"network.gif",sizeof(szIcon));
								break;
							case CUrlStatus::URL_STATUS_FILE_CREATION_ERROR:
							case CUrlStatus::URL_STATUS_INVALID_FILE:
								strcatn(szIcon,"invalidfile.gif",sizeof(szIcon));
								break;
							case CUrlStatus::URL_STATUS_INVALID_URL:
								strcatn(szIcon,"invalidurl.gif",sizeof(szIcon));
								break;
							default:
								strcatn(szIcon,"file.gif",sizeof(szIcon));
								break;
						}
						strcpyn(szUrl,pUrlDatabaseTable->GetField_Url(),sizeof(szUrl));
						url.EncodeUrl(szUrl,sizeof(szUrl));
						strcpyn(szDecodedUrl,pUrlDatabaseTable->GetField_Url(),sizeof(szDecodedUrl));
						url.DecodeUrl(szDecodedUrl);
						_snprintf(szBuffer,
								sizeof(szBuffer)-1,
								"&nbsp;<a href=\"%s\" target=\"_blank\"><img src=\"%s\" border=0>%s</a>",
								szUrl,
								url.LocalFileToUrl(szIcon,szLocalFile,sizeof(szLocalFile)),
								szDecodedUrl
								);
						htmllayout.align = HTMLALIGN_LEFT;
						html.TablePutCol(szBuffer,&htmllayout);
						
						// report
						if(!strempty(pUrlDatabaseTable->GetField_Report()))
						{
							char* p = (char*)strrchr(pUrlDatabaseTable->GetField_Report(),'\\');
							if(p && *(p+1))
								p++;
							else
								p = (char*)pUrlDatabaseTable->GetField_Report();
							_snprintf(szBuffer,sizeof(szBuffer)-1,"&nbsp;<a href=\"%s\" target=\"_blank\">%s</a>",url.LocalFileToUrl(pUrlDatabaseTable->GetField_Report(),szLocalFile,sizeof(szLocalFile)),p);
							
							htmllayout.align = HTMLALIGN_LEFT;
							html.TablePutCol(szBuffer,&htmllayout);
						}
						else
						{
							htmllayout.align = HTMLALIGN_CENTER;
							html.TablePutCol("-",&htmllayout);
						}

						// status
						url_status = (CUrlStatus::URL_STATUS)pUrlDatabaseTable->GetField_Status();
						if(url_status==CUrlStatus::URL_STATUS_UNKNOWN)
						{
							htmllayout.align = HTMLALIGN_CENTER;
							strcpyn(szBuffer,"-",sizeof(szBuffer));
						}
						else
						{
							htmllayout.align = HTMLALIGN_LEFT;
							strcpyn(szBuffer,urlstatus.GetStatus(url_status),sizeof(szBuffer));
							url.EncodeToHTML(szBuffer,sizeof(szBuffer));
						}
						switch(url_status)
						{
							case CUrlStatus::URL_STATUS_DONE:
							case CUrlStatus::URL_STATUS_DOWNLOADED:
							case CUrlStatus::URL_STATUS_MOVED:
							case CUrlStatus::URL_STATUS_NOT_MODIFIED:
								htmllayout.font.color = NULL;
								break;
							case CUrlStatus::URL_STATUS_CANCELLED:
							case CUrlStatus::URL_STATUS_EXCLUDED_BY_ROBOTSTXT:
							case CUrlStatus::URL_STATUS_EXCLUDED_BY_SIZE:
							case CUrlStatus::URL_STATUS_EXCLUDED_BY_WILDCARDS:
							case CUrlStatus::URL_STATUS_EXTERNAL_DOMAIN:
							case CUrlStatus::URL_STATUS_PARENT_URL:
							case CUrlStatus::URL_STATUS_FILE_CREATION_ERROR:
							case CUrlStatus::URL_STATUS_INVALID_FILE:
							case CUrlStatus::URL_STATUS_INVALID_URL:
							case CUrlStatus::URL_STATUS_CGI_URL:
							case CUrlStatus::URL_STATUS_WINSOCK_ERROR:
							case CUrlStatus::URL_STATUS_HTTP_ERROR:
							case CUrlStatus::URL_STATUS_UNKNOWN:
								htmllayout.font.color = "#FF0000";
								break;
							default:
								htmllayout.font.color = NULL;
								break;
						}
						html.TablePutCol(szBuffer,&htmllayout);
						htmllayout.font.color = NULL;
						
						// chiude la riga
						html.TableCloseRow();
					}

					pUrlDatabaseTable->Unlock();
				}

				m_listUrls.Remove(iter);
			}

			iter = m_listUrls.Next(iter);
		
		} while(iter!=(ITERATOR)NULL);

	html.TableClose();

	// footer
	_snprintf(szBuffer,sizeof(szBuffer)-1,WALLPAPER_AUTHOR_HTML_COPYRIGHT,m_pConfig->GetString(WALLPAPER_INSTALL_KEY,WALLPAPER_AUTHOREMAIL_KEY));
	html.FormattedLine(	"<br><br>"
					"<p>"
					"<a href=\"%s\" target=\"_blank\">%s</a>"
					"<br>"
					"%s"
					"</p>",
					m_pConfig->GetString(WALLPAPER_INSTALL_KEY,WALLPAPER_WEBSITE_KEY),
					WALLPAPER_PROGRAM_NAME_ALIAS" - "WALLPAPER_PROGRAM_DESCRIPTION" - v."WALLPAPER_VERSION,
					szBuffer
					);
	html.Footer();
	html.Close();

	// apre il report
	if(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_OPENREPORT_KEY))
	{
		// interfaccia utente
		_snprintf(szBuffer,sizeof(szBuffer)-1,"Opening report:\r\n%s...",szHtmlScriptReport);
		SetDlgItemText(IDC_STATIC_STATUS,szBuffer);
		
		// lancia il browser
		CBrowser browser(m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_PREFERRED_INTERNET_BROWSER_KEY));
		browser.Browse(szHtmlScriptReport);
	}

done:

	// interfaccia utente
	SetDlgItemText(IDC_STATIC_STATUS,"");
	GetDlgItem(IDCANCEL)->EnableWindow(TRUE);
}

/*
	GenerateThumbnails()

	Genera le miniature relative al download.
*/
void CWallPaperCrawlerDlg::GenerateThumbnails(CHtml* html,CUrlService* pUrlService)
{
	char* p;
	CUrl url;
	CRect thumbnailsRect(0,0,50,50);
	BOOL bThumbnail = FALSE;
	THUMBNAIL* thumbnail;
	CThumbnailsList listThumbnails;
	ITERATOR iter;
	char szThumbnail[_MAX_PATH+1];
	char szFileName[_MAX_PATH+1];
	char szPathName[_MAX_PATH+1];
	char szExt[_MAX_EXT+1];
	char szBuffer[1024];
	CUrlTable* pUrlTable = pUrlService->GetTable();

	int nTotFiles = 0;
	int nCurrentFile = 0;
	int nTotRows = 0;
	int nCurrentCol = 0;

	// imposta il layout per il report
	HTMLLAYOUT htmllayout;
	htmllayout.font.type  = "verdana";
	htmllayout.font.size  = 1;
	htmllayout.font.color = NULL;
	htmllayout.align      = HTMLALIGN_CENTER;
	htmllayout.style      = HTMLSTYLE_NONE;
	htmllayout.border     = 0;
	htmllayout.width      = 0;

	HTMLTABLELAYOUT htmltablelayout;
	memcpy(&(htmltablelayout.htmllayout),&htmllayout,sizeof(HTMLLAYOUT));
	htmltablelayout.title       = NULL;
	htmltablelayout.border      = 1;
	htmltablelayout.cellpadding = 5;
	htmltablelayout.cellspacing = 1;
	htmltablelayout.width       = 100;
	strcpyn(htmltablelayout.classname,"whitefn",sizeof(htmltablelayout.classname));
	
	// interfaccia utente
	m_wndProgressBar.SetPos(0);
	m_wndProgressBar.Hide();
	SetDlgItemText(IDC_STATIC_STATUS,"Generating thumbnails...");

	// carica la lista per le miniature con i nomi dei files
	pUrlTable->SetIndex(URL_IDX_ID);
	if(pUrlTable->GoTop())
	{
		while(!pUrlTable->Eof())
		{
			pUrlTable->ScatterMemvars();
			switch((CUrlStatus::URL_STATUS)pUrlTable->GetField_Stat())
			{
				case CUrlStatus::URL_STATUS_DOWNLOADED:
				case CUrlStatus::URL_STATUS_MOVED:
				case CUrlStatus::URL_STATUS_NOT_MODIFIED:
					if(pUrlTable->GetField_Size() > 0 && CImage::IsImageFile(pUrlTable->GetField_File()))
					{
						thumbnail = new THUMBNAIL;
						if(thumbnail)
						{
							memset(thumbnail,'\0',sizeof(THUMBNAIL));
							strcpyn(thumbnail->file,pUrlTable->GetField_File(),_MAX_FILEPATH+1);
							strcpyn(thumbnail->url,pUrlTable->GetField_Url(),MAX_URL+1);
							if((p = strrchr(thumbnail->url,'/'))!=NULL)
								*p = '\0';
							listThumbnails.Add(thumbnail);
							nTotFiles++;
						}
					}
					break;
			}

			pUrlTable->GetNext();
		}
	}
	if(nTotFiles <= 0)
		goto done;

	// per ognuno dei files presenti nella lista
	if((iter = listThumbnails.First())!=(ITERATOR)NULL)
	{
		m_wndProgressBar.SetRange(0,100);
		m_wndProgressBar.SetStep(1);
		m_wndProgressBar.SetPos(0);
		m_wndProgressBar.Show();

		do
		{
			::PeekAndPump();

			thumbnail = (THUMBNAIL*)iter->data;
			if(thumbnail)
			{
				p = strrchr(thumbnail->file,'\\');
				strcpyn(szFileName,p ? p+1 : thumbnail->file,sizeof(szFileName));
				
				// interfaccia utente
				_snprintf(szBuffer,
						sizeof(szBuffer)-1,
						"Generating thumbnails... %d%% (%d of %d)\r\n%s",
						DIV((100 * (nCurrentFile+1)),nTotFiles),
						nCurrentFile+1,
						nTotFiles,
						thumbnail->file
						);
				SetDlgItemText(IDC_STATIC_STATUS,szBuffer);

				// crea il pathname locale relativo all'url
				// (non puo' mettere le miniature in un unica directory perche' quelle con lo stesso nome verrebbero sovrascritte)
				url.CreatePathNameFromHttpUrl(thumbnail->url,szPathName,sizeof(szPathName),m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILSDIR_KEY));

				// compone il nome della miniatura
				_snprintf(szThumbnail,sizeof(szThumbnail)-1,"%sthumbnail.%s",szPathName,szFileName);

				// interfaccia utente
				nCurrentFile++;
				m_wndProgressBar.SetPos(DIV((100 * nCurrentFile),nTotFiles));

				bThumbnail = FALSE;

				// carica l'immagine e genera la miniatura
				if(m_pImage->Load(thumbnail->file))
				{
					BOOL bNeedConversion = TRUE; 
					p = strrchr(thumbnail->file,'.');
					if(p)
					{
						if(stricmp(p,".gif")==0 || stricmp(p,".jpg")==0 || stricmp(p,".png")==0)
							bNeedConversion = FALSE;
					}
					if(bNeedConversion)
					{
						strcpyn(szExt,".jpg",sizeof(szExt));
						strcatn(szThumbnail,szExt,sizeof(szThumbnail));
					}
					else
						strcpyn(szExt,p,sizeof(szExt));

					thumbnail->width = thumbnail->thumb_width = m_pImage->GetWidth();
					thumbnail->height = thumbnail->thumb_height = m_pImage->GetHeight();
					thumbnail->colors = m_pImage->GetNumColors();
					thumbnail->bpp = m_pImage->GetBPP();
					
					if(thumbnail->width > thumbnailsRect.right || thumbnail->height > thumbnailsRect.bottom)
					{
						m_pImage->Stretch(thumbnailsRect);
						thumbnail->thumb_width  = m_pImage->GetWidth();
						thumbnail->thumb_height = m_pImage->GetHeight();
					}
					
					bThumbnail = m_pImage->Save(szThumbnail,szExt);
				}

				// aggiorna l'elemento della lista con il nome della miniatura
				strcpyn(thumbnail->thumbnail,bThumbnail ? szThumbnail : "",_MAX_FILEPATH+1);
			}

			iter = listThumbnails.Next(iter);
		
		} while(iter!=(ITERATOR)NULL);

		// genera il report
		html->Text("<br><br><p class=\"bold\">&nbsp;Thumbnails</p>");
		html->TableOpen(&htmltablelayout);
		
		if((iter = listThumbnails.First())!=(ITERATOR)NULL)
		{
			do
			{
				thumbnail = (THUMBNAIL*)iter->data;
			
				if(thumbnail)
				{
					char* p = strrchr(thumbnail->file,'\\');
					if(p)
					{
						p++;
						if(!*p)
							p = NULL;
					}
					if(!p)
						p = thumbnail->file;

					if(strcmp(thumbnail->thumbnail,"")==0)
					{
						_snprintf(szBuffer,
								sizeof(szBuffer)-1,
								"<a href=\"%s\" target=\"_blank\">%s</a>",
								url.LocalFileToUrl(thumbnail->file,szFileName,sizeof(szFileName)),
								p
								);
					}
					else
					{
						char szFileSize[32];
						strsize(szFileSize,sizeof(szFileSize),(double)::GetFileSizeExt(thumbnail->file));
						_snprintf(szBuffer,
								sizeof(szBuffer)-1,
								"<a href=\"%s\" target=\"_blank\"><img border=0 width=%d height=%d alt=\"%s\" src=\"%s\"></a><br><br><b>%s</b><br>%d x %d x %d%s colors<br>%s",
								url.LocalFileToUrl(thumbnail->file,szFileName,sizeof(szFileName)),
								thumbnail->thumb_width,
								thumbnail->thumb_height,
								thumbnail->file,
								url.LocalFileToUrl(thumbnail->thumbnail,szThumbnail,sizeof(szThumbnail)),
								p,
								thumbnail->width,
								thumbnail->height,
								(thumbnail->colors > 256 || thumbnail->colors <= 0) ? 16 : thumbnail->colors,
								(thumbnail->colors > 256 || thumbnail->colors <= 0) ? "M" : "",
								szFileSize
								);
					}

					if(nCurrentCol >= 5)
					{
						html->TableCloseRow();
						nCurrentCol = 0;
					}

					if(nCurrentCol==0)
					{
						html->TableOpenRow();
						nTotRows++;
					}
					
					html->TablePutCol(szBuffer,&htmllayout);
					
					nCurrentCol++;
				}

				iter = listThumbnails.Next(iter);
			
			} while(iter!=(ITERATOR)NULL);
			
			if(nCurrentCol < 5)
			{
				if(nTotRows > 1)
					while(nCurrentCol++ < 5)
						html->TablePutCol("&nbsp;",&htmllayout);
				html->TableCloseRow();
			}
		}

		html->TableClose();
	}

done:

	// interfaccia utente
	m_wndProgressBar.SetPos(0);
	m_wndProgressBar.Hide();
	SetDlgItemText(IDC_STATIC_STATUS,"");
}

/*
	FindFileType()

	Verifica se l'url sta referenziando un tipo file registrato.
*/
BOOL CWallPaperCrawlerDlg::FindFileType(LPCSTR lpcszUrl,CFileTypeList* pFileTypeList)
{
	BOOL bFound = FALSE;
	FILETYPE* pFiletype;
	ITERATOR iter;
	char szExt[_MAX_EXT+1];
	CWildCards Wildcards;
	CUrl url;
	URL Url;

	url.SplitUrl(lpcszUrl,&Url);

	char* p = strrchr(Url.file,'.');
	
	if(p)
		strcpyn(szExt,p,sizeof(szExt));
	else
		strcpy(szExt,"");
	
	// se l'url contiene un cgi lo elimina
	if(url.IsCgiUrl(szExt))
		if((p = (char*)url.GetCgiUrl(szExt))!=NULL)
			*p = '\0';

	// cerca l'estensione nella lista dei tipi di files registrati
	// (uguaglianza/wildcards)
	if((iter = pFileTypeList->First())!=(ITERATOR)NULL)
		do
		{
			pFiletype = (FILETYPE*)iter->data;
		
			if(pFiletype)
				if(stricmp(pFiletype->ext,szExt)==0 || Wildcards.Match(pFiletype->ext,szExt))
				{
					bFound = TRUE;
					break;
				}
			
			iter = pFileTypeList->Next(iter);
		
		} while(iter!=(ITERATOR)NULL);

	return(bFound);
}

/*	
	FindExternal()

	Verifica se il dominio/url e' presente nella lista delle esclusioni.
*/
BOOL CWallPaperCrawlerDlg::FindExternal(LPCSTR lpcszExternal,CItemList* plistUrls)
{
	BOOL bFound = FALSE;
	ITERATOR iter;
	ITEM* item;
	
	// cerca la il domino/url nella lista
	if((iter = plistUrls->First())!=(ITERATOR)NULL)
		do
		{
			item = (ITEM*)iter->data;
		
			if(item)
				if(stricmp(lpcszExternal,item->item)==0)
				{
					bFound = TRUE;
					break;
				}

			iter = plistUrls->Next(iter);
		
		} while(iter!=(ITERATOR)NULL);

	return(bFound);
}

/*	
	AddExternal()

	Aggiunge alla lista il dominio/url da escludere.
*/
BOOL CWallPaperCrawlerDlg::AddExternal(LPCSTR lpcszExternal,CItemList* plistUrls)
{
	BOOL bFound = FALSE;
	ITERATOR iter;
	ITEM* item;
	
	// cerca il dominio/url nella lista inserendolo se gia' non esiste
	if((iter = plistUrls->First())!=(ITERATOR)NULL)
		do
		{
			item = (ITEM*)iter->data;
		
			if(item)
				if(stricmp(lpcszExternal,item->item)==0)
				{
					bFound = TRUE;
					break;
				}

			iter = plistUrls->Next(iter);
		
		} while(iter!=(ITERATOR)NULL);

	if(!bFound)
	{
		item = (ITEM*)plistUrls->Add();
		if(item)
			strcpyn(item->item,lpcszExternal,_MAX_ITEM+1);
	}

	return(bFound);
}

/*
	FindWildcards()

	Verifica se la stringa soddisfa le condizioni delle wildcards.
*/
BOOL CWallPaperCrawlerDlg::FindWildcards(LPCSTR lpcszString,CWildCards* pWildcard,CItemList* plistWildcards)
{
	BOOL bFound = FALSE;
	ITERATOR iter;
	ITEM* pItem;
	
	// confronta la stringa con quanto presente nella lista
	if((iter = plistWildcards->First())!=(ITERATOR)NULL)
		do
		{
			pItem = (ITEM*)iter->data;
		
			if(pItem)
			{
				if(pWildcard->Match(pItem->item,(LPSTR)lpcszString))
				{
					bFound = TRUE;
					break;
				}
			}
			
			iter = plistWildcards->Next(iter);
		
		} while(iter!=(ITERATOR)NULL);

	return(bFound);
}

/*
	UpdateStatusBar()

	Aggiorna i totali relativi a immagini/url scaricate/da scaricare.
*/
void CWallPaperCrawlerDlg::UpdateStatusBar(UINT nID,UINT nNextID,UINT nPict)
{
	char szPanel[_MAX_PATH+1];
	
	if(nID==(UINT)-1 && nNextID==(UINT)-1 && nPict==(UINT)-1)
	{
		memset(szPanel,'\0',sizeof(szPanel));
		SetDlgItemText(IDC_STATIC_TOTAL_PICTURES,szPanel);
		SetDlgItemText(IDC_STATIC_TOTAL_URLS,szPanel);
	}
	else
	{
		_snprintf(szPanel,sizeof(szPanel)-1,"Pictures: %ld",(int)nPict);
		SetDlgItemText(IDC_STATIC_TOTAL_PICTURES,szPanel);
		
		_snprintf(szPanel,sizeof(szPanel)-1,"Urls: %ld (parsed %ld, left %ld)\t",(int)nID,(int)nNextID,(int)nID-(int)nNextID);
		SetDlgItemText(IDC_STATIC_TOTAL_URLS,szPanel);
	}
}

/*
	CrawlCallback()

	Wrapper per la callback per il download del file tramite thread independente.
*/
UINT CWallPaperCrawlerDlg::CrawlCallback(LPVOID pHttpConnection,LPVOID lpVoid)
{
	UINT nRet = (UINT)-1L;

	CWallPaperCrawlerDlg* pThis = (CWallPaperCrawlerDlg*)lpVoid;
	if(pThis)
		nRet = pThis->CrawlCallback(pHttpConnection);
	
	return(nRet);
}

/*
	CrawlCallback()

	Callback per il download del file tramite thread independente.
*/
UINT CWallPaperCrawlerDlg::CrawlCallback(LPVOID lpVoid)
{
	CHttpConnection* pHttpConnection = (CHttpConnection*)lpVoid;
	int nRet = (m_nCancel==0 ? IDOK : IDCANCEL);

	if(nRet!=IDCANCEL)
	{
		// inizio download
		if(pHttpConnection->GetObject()->stat==HTTPOBJECTSTAT_BEGIN_RECEIVING)
		{
			nRet = (m_nCancel==0 ? IDOK : IDCANCEL);
		}
		// download
		else if(pHttpConnection->GetObject()->stat==HTTPOBJECTSTAT_RECEIVING)
		{
			nRet = (m_nCancel==0 ? IDOK : IDCANCEL);
		}
		// fine download
		else if(pHttpConnection->GetObject()->stat==HTTPOBJECTSTAT_END_RECEIVING)
		{
			nRet = (m_nCancel==0 ? IDOK : IDCANCEL);
		}
	}

	return(nRet);
}

/*
	Crawl()

	Wrapper per il thread per il download del file tramite thread independente.
*/
UINT CWallPaperCrawlerDlg::Crawl(LPVOID lpVoid)
{
	UINT nRet = (UINT)-1L;
	
	CRAWLTHREAD* pCrawlThread = (CRAWLTHREAD*)lpVoid;
	if(pCrawlThread)
	{
		CWallPaperCrawlerDlg* pThis = (CWallPaperCrawlerDlg*)pCrawlThread->pThis;	
		if(pThis)
			nRet = pThis->Crawl(pCrawlThread);
	}

	return(nRet);
}

/*
	Crawl()

	Thread per il download dell'url (modalita' multithread).
*/
UINT CWallPaperCrawlerDlg::Crawl(CRAWLTHREAD* pCrawlThread)
{
	URL Url;
	CUrl url;
	CONTENTTYPE ctype;
	char szTranslatedUrl[MAX_URL+1] = {0};

	// parametri sballati
	if(!pCrawlThread)
		return(0L);

	// notifica l'esistenza del nuovo thread
	{
	char szBuffer[64];
	m_nActiveThreads++;
	if(m_nActiveThreads > m_nThreadsPeak)
		m_nThreadsPeak = m_nActiveThreads;	
	_snprintf(szBuffer,sizeof(szBuffer)-1,"Threads: %d (%d)",m_nActiveThreads,m_nThreadsPeak);
	TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperCrawlerDlg::OnThreadEvent(): threads: %d (%d)\n",m_nActiveThreads,m_nThreadsPeak));
	SetDlgItemText(IDC_STATIC_TOTAL_THREADS,szBuffer);
	}

	// imposta i defaults per la connessione
	TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperCrawlerDlg::Crawl(%ld): setting connection defaults\n",::GetCurrentThreadId()));
	pCrawlThread->httpConnection.Reset();
	pCrawlThread->httpConnection.SetSkipExisting(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_SKIPEXISTING_KEY));
	pCrawlThread->httpConnection.SetForceSkipExisting(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_SKIPEXISTING_FORCE_KEY));
	pCrawlThread->httpConnection.SetDoNotSkipExisting(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_SKIPEXISTING_DONT_KEY));
	if(!pCrawlThread->httpConnection.GetSkipExisting())
	{
		pCrawlThread->httpConnection.AddHeader("Pragma","no-cache");
		// Cache-Control e' specifico per HTTP/1.1, non tutti i server lo gestiscono con una richiesta HTTP/1.0
		//pCrawlThread->httpConnection.AddHeader("Cache-Control","no-cache");
	}
	pCrawlThread->httpConnection.SetConnectionRetry(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_CONNECTIONRETRY_KEY));
	if(m_nConnectionTimeout > 0)
		pCrawlThread->httpConnection.SetConnectionTimeout(m_nConnectionTimeout);
	pCrawlThread->httpConnection.AllowCookies(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_COOKIES_ALLOWED_KEY));
	pCrawlThread->httpConnection.SetCookieFileName(m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_COOKIES_FILE_KEY));
	if(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_USEPROXY_KEY))
	{
		pCrawlThread->httpConnection.SetProxyAddress(m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_PROXY_ADDRESS_KEY));
		pCrawlThread->httpConnection.SetProxyPortNumber(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PROXY_PORTNUMBER_KEY));
		if(strcmp(m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_PROXY_USER_KEY),"")!=0 && strcmp(m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_PROXY_PASSWORD_KEY),"")!=0)
		{
			pCrawlThread->httpConnection.SetProxyUser(m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_PROXY_USER_KEY));
			pCrawlThread->httpConnection.SetProxyPassword(m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_PROXY_PASSWORD_KEY));
			CBase64 base64;
			char szEncoded[MAX_USER_LEN+1+MAX_PASS_LEN+1];
			_snprintf(szEncoded,sizeof(szEncoded)-1,"%s:%s",pCrawlThread->httpConnection.GetProxyUser(),pCrawlThread->httpConnection.GetProxyPassword());
			char* pAuth = base64.Encode(szEncoded);
			pCrawlThread->httpConnection.SetProxyAuth(pAuth);
			delete [] pAuth;
		}
		if(strcmp(m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_PROXY_EXCLUDEDOMAINS_KEY),"")!=0)
		{
			CHostNameList* plistProxyExclude = pCrawlThread->httpConnection.GetProxyExcludeList();
			char szExclude[2048];
			strcpyn(szExclude,m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_PROXY_EXCLUDEDOMAINS_KEY),sizeof(szExclude));
			char* token = strtok(szExclude,",");
			HOSTNAME* n;
			for(int i = 0; token; i++)
			{
				if((n = (HOSTNAME*)plistProxyExclude->Add())!=(HOSTNAME*)NULL)
					strcpyn(n->name,token,HOSTNAME_SIZE+1);
				token = strtok((char*)NULL,",");
			}
		}
	}
	url.SplitUrl(pCrawlThread->szUrl,&Url);
	if(Url.port <= 0)
		Url.port = HTTP_DEFAULT_PORT;
	pCrawlThread->httpConnection.SetPortNumber(Url.port);
	CWindowsVersion winver;
	char szUserAgent[HTTP_MAX_USERAGENT+1];
	_snprintf(szUserAgent,sizeof(szUserAgent)-1,WALLPAPER_USER_AGENT,winver.GetVersionString());
	pCrawlThread->httpConnection.AddHeader("User-Agent",szUserAgent);
	UINT nMinSize = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_MINSIZE_KEY);
	UINT nMaxSize = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_MAXSIZE_KEY);
	int nSizeType = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_MINSIZETYPE_KEY);
	if(nSizeType==SIZETYPE_KBYTES)
		nMinSize *= 1024L;
	else if(nSizeType==SIZETYPE_MBYTES)
		nMinSize *= 1048576L;
	nSizeType = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_MAXSIZETYPE_KEY);
	if(nSizeType==SIZETYPE_KBYTES)
		nMaxSize *= 1024L;
	else if(nSizeType==SIZETYPE_MBYTES)
		nMaxSize *= 1048576L;
	pCrawlThread->httpConnection.SetMinSize(nMinSize);
	pCrawlThread->httpConnection.SetMaxSize(nMaxSize);

	// download dell'url
	TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperCrawlerDlg::Crawl(%ld): getting url\n",::GetCurrentThreadId()));
	BOOL bDownloaded = url.GetHttpUrl(	&(pCrawlThread->httpConnection),
								pCrawlThread->szUrl,
								NULL,
								szTranslatedUrl,
								sizeof(szTranslatedUrl),
								pCrawlThread->szDownloadPath,
								pCrawlThread->szFileName,
								_MAX_PATH+1,
								&ctype,
								FALSE,
								CrawlCallback,
								this
								);
	
	::PeekAndPump();
	::Sleep(100L);

	// aggiorna la base dati con il risultato del download
	if(bDownloaded)
	{
		// download riuscito
		int nDownloadCode;
		if(!IS_HTTP_ERROR((nDownloadCode = pCrawlThread->httpConnection.GetHttpError())) && pCrawlThread->httpConnection.GetSocketError()==0)
		{
			if(nDownloadCode <= (int)HTTP_STATUS_SUCCESSFUL)
				nDownloadCode = (int)CUrlStatus::URL_STATUS_DOWNLOADED;
			else if(nDownloadCode==(int)HTTP_STATUS_NOT_MODIFIED)
				nDownloadCode = (int)CUrlStatus::URL_STATUS_NOT_MODIFIED;
			else
				nDownloadCode = (int)CUrlStatus::URL_STATUS_MOVED;
			
			::Sleep(100);
			TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperCrawlerDlg::Crawl(%ld): updating url\n",::GetCurrentThreadId()));
			// aggiorna il database
			pCrawlThread->pUrlService->Update(	pCrawlThread->szUrl,
										(CUrlStatus::URL_STATUS)nDownloadCode,
										pCrawlThread->szFileName,
										(double)::GetFileSizeExt(pCrawlThread->szFileName),
										nDownloadCode==(int)CUrlStatus::URL_STATUS_DOWNLOADED ? (((double)(pCrawlThread->httpConnection.GetObject()->endtime - pCrawlThread->httpConnection.GetObject()->starttime)) / 1000.0f) : 0.0f
										);
			TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperCrawlerDlg::Crawl(%ld): url updated\n",::GetCurrentThreadId()));

			// aggiunge il file alla playlist
			AddDownloadToPlaylist(nDownloadCode,pCrawlThread->szFileName);
		}
		else // download fallito
		{
			pCrawlThread->pUrlService->Update(	pCrawlThread->szUrl,
										pCrawlThread->httpConnection.IsSocketError() ? (CUrlStatus::URL_STATUS)pCrawlThread->httpConnection.GetSocketError() : (pCrawlThread->httpConnection.IsHttpError() ? (CUrlStatus::URL_STATUS)pCrawlThread->httpConnection.GetHttpError() : CUrlStatus::URL_STATUS_UNKNOWN),
										NULL,
										(double)pCrawlThread->httpConnection.GetObject()->size);
		}
	}
	else // download interrotto
	{
		pCrawlThread->pUrlService->Update(pCrawlThread->szUrl,CUrlStatus::URL_STATUS_CANCELLED);
	}

	::PeekAndPump();
//	::Sleep(100L);

	// visualizza l'anteprima
	if(bDownloaded)
		DoPreview(pCrawlThread->szFileName);

	// notifica il termine del thread
	{
	char szBuffer[64];
	m_nActiveThreads--;
	if(m_nActiveThreads > m_nThreadsPeak)
		m_nThreadsPeak = m_nActiveThreads;	
	_snprintf(szBuffer,sizeof(szBuffer)-1,"Threads: %d (%d)",m_nActiveThreads,m_nThreadsPeak);
	TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CWallPaperCrawlerDlg::OnThreadEvent(): threads: %d (%d)\n",m_nActiveThreads,m_nThreadsPeak));
	SetDlgItemText(IDC_STATIC_TOTAL_THREADS,szBuffer);
	}

	// finito
	pCrawlThread->nStatus = WAIT_OBJECT_0;

	return(0L);
}

/*
	LoadUrlsFromScript()

	Carica nella lista interna le url dal file script.
*/
BOOL CWallPaperCrawlerDlg::LoadUrlsFromScript(LPCSTR lpcszFileName,CItemList* plistUrls)
{
	plistUrls->RemoveAll();

	FILE* fp = fopen(lpcszFileName,"rt");
	if(fp)
	{
		char* p;
		CUrl url;
		char szUrl[MAX_URL+1];
		
		while(fgets(szUrl,sizeof(szUrl)-1,fp))
		{
			// elimina il CRLF finale
			p = strchr(szUrl,'\r');
			if(!p)
				p = strchr(szUrl,'\n');
			if(p)
				*p = '\0';

			// elimina gli spazi iniziali
			for(p = szUrl; *p && ISSPACE(*p);)
				p++;

			// salta i commenti
			if(*p==';' || *p=='#')
				continue;

			// inserisce l'url nella lista
			if(url.IsUrlType(p,HTTP_URL))
			{
				ITEM* item = (ITEM*)plistUrls->Add();
				if(item)
					strcpyn(item->item,p,_MAX_ITEM+1);
			}
		}

		fclose(fp);
	}

	return(plistUrls->Count() > 0);
}

/*
	OnCrawlerDone()

	Gestore per il termine del download.
*/
LONG CWallPaperCrawlerDlg::OnCrawlerDone(UINT /*wParam*/,LONG /*lParam*/)
{
	// esecuzione del file script cancellata
	if(m_nCancelScript!=0)
	{
		m_nCancelScript = 0;
		m_listUrls.RemoveAll();
		MSG msg;
		while(::PeekMessage(&msg,NULL,WM_CRAWLER_DONE,WM_CRAWLER_DONE,PM_REMOVE))
			;
	}
	else // verifica se deve lanciare un nuovo thread per l'url successiva (nel caso si stia usando un file script)
	{
		ITERATOR iter;
		ITEM* item;
		if((iter = m_listUrls.First())!=(ITERATOR)NULL)
		{
			item = (ITEM*)iter->data;
			if(item)
			{
				strcpyn(m_szUrl,item->item,sizeof(m_szUrl));
				m_listUrls.Remove(iter);
				GetDlgItem(IDOK)->EnableWindow(FALSE);
				SetDlgItemText(IDCANCEL,"&Cancel");
				AfxBeginThread(Crawler,this,m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PRIORITY_THREAD_KEY));
			}
		}
		else
		{
			if(m_bUseScript)
			{
				// data/ora fine download
				CDateTime datetime(GMT);
				strcpyn(m_szScriptEndTime,datetime.GetFormattedDate(TRUE),sizeof(m_szScriptEndTime));
				
				// genera il report html
				if(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_GENERATEREPORT_KEY))
					GenerateHtmlReportForScriptFile();
			}
		}
	}

	return(0L);
}
