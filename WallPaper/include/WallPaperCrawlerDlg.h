/*
	WallPaperCrawlerDlg.h
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
#ifndef _WALLPAPERCRAWLERDLG_H
#define _WALLPAPERCRAWLERDLG_H 1

#include "window.h"
#include "CComboBoxExt.h"
#include "CDialogEx.h"
#include "CNodeList.h"
#include "CSync.h"
#include "CDibCtrl.h"
#include "CHtml.h"
#include "CHttp.h"
#include "CImageFactory.h"
#include "COleDropTargetEx.h"
#include "CProgressBar.h"
#include "CUrl.h"
#include "CIcy.h"
#include "CUrlDatabaseService.h"
#include "CWildCards.h"
#include "CWndLayered.h"
#include "WallPaperConfig.h"
#include "WallPaperThreadList.h"

#define IDS_DIALOG_CRAWLER_TITLE " Crawler "

/*
	PLS
	elemento per l'entrata della playlist (.pls)
*/
struct PLS {
	char	host[MAX_URL+1];
	int	port;
	char	dir[MAX_URL+1];
	char	file[MAX_URL+1];
	char	title[MAX_URL+1];
	int	entry;
};

/*
	CPlsList
	classe per la playlist (.pls)
*/
class CPlsList : public CNodeList
{
public:
	CPlsList() : CNodeList() {}
	virtual ~CPlsList() {CNodeList::DeleteAll();}
	void* Create(void)
	{
		return(new PLS);
	}	
	void* Initialize(void* pVoid)
	{
		PLS* pData = (PLS*)pVoid;
		if(!pData)
			pData = (PLS*)Create();
		if(pData)
			memset(pData,'\0',sizeof(PLS));
		return(pData);
	}
	BOOL PreDelete(ITERATOR iter)
	{
		if((PLS*)iter->data)
			delete ((PLS*)iter->data),iter->data = (PLS*)NULL;
		return(TRUE);
	}
	int Size(void) {return(sizeof(PLS));}
#ifdef _DEBUG
	const char* Signature(void) {return("CPlsList");}
#endif
};

/*
	STREAMRIPPER_CALLBACK_DATA
	struttura per i dati per la callback per il ripper audio
*/
struct STREAMRIPPER_CALLBACK_DATA {
	LPVOID				pThis;				// ptr alla classe
	CUrlService*			pUrlService;			// ptr alle variabili locali al thread per la registrazioni (che non sono membri della classe)
	CUrlDatabaseService*	pUrlDatabaseService;
	BOOL					bAddSongsToPlaylist;	// variabili ad uso della callback (per non dichiararle static nella callback)
	ICYDATA*				pIcyData;
	char					szFilenameOfCurrentSong[_MAX_PATH+1];
	unsigned long			nDownloadTimeOfCurrentSong;
	unsigned long			nRippedSongsCount;
	unsigned long			nRippedAmount;
	BOOL					bProgressBarShowed;
	BOOL					bAddedToPlaylist;
	UINT					nID;
};

/*
	CRAWLTHREAD
	elemento per la lista dei threads
*/
struct CRAWLTHREAD : public THREAD {
	LPVOID			pThis;
	DWORD			nStatus;
	char				szUrl[MAX_URL+1];
	CUrlStatus::URL_STATUS uStatus;
	char				szDownloadPath[_MAX_PATH+1];
	char				szFileName[_MAX_PATH+1];
	DWORD			dwSize;
	double			dlTotalTime;
	int				nDownloadCode;
	CUrlService*		pUrlService;
	CHttpConnection	httpConnection;

	void Reset(void)
	{
		THREAD::Reset();
		pThis = NULL;
		nStatus = WAIT_OBJECT_0;
		memset(szUrl,'\0',MAX_URL+1);
		memset(szDownloadPath,'\0',_MAX_PATH+1);
		memset(szFileName,'\0',_MAX_PATH+1);
		pUrlService = NULL;
		httpConnection.Reset();
	}
};

/*
	CRAWLERPARAMS
	per il passaggio dei parametri al thread UI
*/
struct CRAWLERPARAMS {
	char szUrl[MAX_URL+1];
	CWallPaperConfig* pConfig;
};

/*
	CWallPaperCrawlThreadList
	classe per la lista dei threads attivi
*/
class CWallPaperCrawlThreadList : public CNodeList
{
public:
	CWallPaperCrawlThreadList() : CNodeList() {}
	virtual ~CWallPaperCrawlThreadList() {CNodeList::DeleteAll();}
	void* Create(void)
	{
		return(new CRAWLTHREAD);
	}
	void* Initialize(void* pVoid)
	{
		CRAWLTHREAD* pData = (CRAWLTHREAD*)pVoid;
		if(!pData)
			pData = (CRAWLTHREAD*)Create();
		if(pData)
			pData->Reset();
		return(pData);
	}
	BOOL PreDelete(ITERATOR iter)
	{
		if((CRAWLTHREAD*)iter->data)
			delete ((CRAWLTHREAD*)iter->data),iter->data = (CRAWLTHREAD*)NULL;
		return(TRUE);
	}
	int Size(void) {return(-1);}
#ifdef _DEBUG
	const char* Signature(void) {return("CWallPaperCrawlThreadList");}
#endif
};

/*
	CWallPaperCrawlerDlg
*/
class CWallPaperCrawlerDlg : public CDialogEx
{
	DECLARE_DYNCREATE(CWallPaperCrawlerDlg)

protected: // provide default constructor only for dynamic creation, private/protected to prevent it from being called from outside the class implementation
	CWallPaperCrawlerDlg() {::MessageBox(NULL,"PANIC! This shouldn't happen!","CWallPaperCrawlerDlg()",MB_OK|MB_ICONERROR|MB_TASKMODAL|MB_SETFOREGROUND|MB_TOPMOST);}

public:
	CWallPaperCrawlerDlg(HWND hWndParent);
	~CWallPaperCrawlerDlg() {}

	// gestori
	void			DoDataExchange			(CDataExchange*);
	BOOL			OnInitDialog			(void);
	BOOL			OnInitDialogEx			(UINT = -1,LPCSTR = NULL) {return(FALSE);}
	BOOL			OnQueryNewPalette		(void);
	void			OnPaletteChanged		(CWnd* pFocusWnd);
	void			OnSetFocus			(CWnd* pOldWnd);
	void			OnOk					(void);
	void			OnExit				(void);
	BOOL			OnEscapeKey			(void) {ShowWindow(SW_MINIMIZE); return(TRUE);}
	
	void			OnScriptFile			(void);
	void			OnGetUrlFromCombo		(void);
	void			OnCheckPicturePreview	(void);
	void			OnDropFiles			(void);
	LONG			OnDropOle				(UINT,LONG);
	LRESULT		OnTooltipCallback		(WPARAM wParam,LPARAM lParam);

	// callback per l'anteprima dell'immagine
	static UINT	PicturePreview			(LPVOID lpVoid);
	void			PicturePreview			(void);
	void			DoPreview				(LPCSTR);

	// callback per il download dell'url
	static UINT	GetHttpUrlCallback		(LPVOID lpFileDownload,LPVOID lpVoid);
	UINT			GetHttpUrlCallback		(LPVOID lpVoid);
	
	// callback per il parsing del file html
	static UINT	ExtractLinksCallback	(LPVOID lpFileParse,LPVOID lpVoid);
	UINT			ExtractLinksCallback	(LPVOID lpVoid);
	
	// threads per il crawler (HTTP/ICY)
	static UINT	Crawler				(LPVOID lpVoid);
	UINT			Crawler				(void);
	static UINT	AudioCrawler			(LPVOID lpVoid);
	UINT			AudioCrawler			(void);

	void			AddDownloadToPlaylist	(int nDownloadCode,LPCSTR lpcszFileName);

	static UINT	TimeoutManager			(LPVOID lpVoid);
	UINT			TimeoutManager			(void);

	static UINT	Crawl				(LPVOID lpVoid);
	UINT			Crawl				(CRAWLTHREAD* pCrawlUrl);
	static UINT	CrawlCallback			(LPVOID lpFileDownload,LPVOID lpVoid);
	UINT			CrawlCallback			(LPVOID lpVoid);

	// generazione report html
	typedef struct stHTMLREPORTEXTRAPARAMS {
		int	type;		// CRAWLER_DOWNLOAD_MODE, CRAWLER_STREAMRIPPER_MODE
		ICYDATA* icydata;	// header icy (streamripper)
	} HTMLREPORTEXTRAPARAMS;
	void			GenerateHtmlReport		(LPCSTR lpcszUrl,LPCSTR lpcszReportName,LPCSTR lpcszAsciiReportName,CUrlService* pUrlService,CUrlDatabaseService* pUrlDatabaseService,HTMLREPORTEXTRAPARAMS* htmlparams = NULL);
	void			GenerateHtmlReportForScriptFile(void);
	void			GenerateThumbnails		(CHtml* html,CUrlService* pUrlService);
	
	// liste inclusione/esclusione
	BOOL			FindFileType			(LPCSTR,CFileTypeList*);
	BOOL			FindExternal			(LPCSTR lpcszExternal,CItemList* plistUrls);
	BOOL			AddExternal			(LPCSTR lpcszExternal,CItemList* plistUrls);
	BOOL			FindWildcards			(LPCSTR lpcszString,CWildCards* pWildcard,CItemList* plistWildcards);
	
	// barra di stato
	void			UpdateStatusBar		(UINT nID = (UINT)-1,UINT nNextID = (UINT)-1,UINT nPict = (UINT)-1);

	BOOL			LoadUrlsFromScript		(LPCSTR lpcszFileName,CItemList* plistUrls);
	LONG			OnCrawlerDone			(UINT,LONG);

	HWND			m_hWndParent;					// finestra padre
	char			m_szTitle[MAX_URL+1];			// titolo del dialogo
	CWallPaperConfig* m_pConfig;					// configurazione

	BOOL			m_bAutoRun;					// per il lancio automatico del crawler
	int			m_nCrawlerMode;				// modalita' (head/get/audio)
		
	UINT			m_nCancel;					// flag per interruzione download
	UINT			m_nCancelScript;				// flag per interruzione download
	BOOL			m_bInCrawling;					// flag per download in corso
	CWallPaperCrawlThreadList m_listActiveCrawlerThreads; // lista dei threads attivi
	int			m_nActiveThreads;				// numero di threads attivi
	int			m_nThreadsPeak;				// picco massimo di threads attivi
	BOOL			m_bAddSongsToPlaylist;			// flag per aggiunta files (ripper)
	BOOL			m_bAddFilesToPlaylist;			// flag per aggiunta files (crawler)
	
	CIcy			m_icy;						// per le info sullo stream audio
	CHttpConnection m_httpConnection;				// struttura per il download
	char			m_szDownloadDir[_MAX_PATH+1];		// directory per il download
	char			m_szUrl[MAX_URL+1];				// url da scaricare
	CItemList		m_listUrls;					// lista delle url da scaricare (per file script)
	BOOL			m_bUseScript;					// flag per utilizzo file script
	char			m_szScriptFile[_MAX_PATH+1];		// nome del file script
	double		m_dlTotalTime;					// tempo totale (in secs.)
	double		m_dlDownloadTime;				// tempo download (in secs.)
	char			m_szStartTime[MAX_DATE_STRING+1];	// ora inizio
	char			m_szEndTime[MAX_DATE_STRING+1];	// ora fine
	char			m_szScriptStartTime[MAX_DATE_STRING+1]; // ora inizio (per file script)
	char			m_szScriptEndTime[MAX_DATE_STRING+1]; // ora fine (per file script)
	
	BOOL			m_bPicturePreview;				// flag per previsualizzazione immagini
	CDibCtrl		m_wndStaticDib;				// controllo per previsualizzazione immagini
	CSyncProcesses	m_PictPreviewSync;				// per la sincronizzazione durante il preview
	CItemList		m_listPictPreview;
	char			m_szLibraryName[_MAX_PATH+1];		// nome della libreria di default
	CImageFactory	m_ImageFactory;				// fattoria per l'immagine
	CImage*		m_pImage;						// puntatore all'oggetto immagine

	CPlsList		m_listPls;					// elementi della playlist audio

	CComboBoxExt	m_wndComboUrl;					// combo per le url
	CAnimateCtrl	m_wndAnimatedAvi;				// controllo AVI
	CProgressBar	m_wndProgressBar;				// barra progressiva
	COleDropTargetEx m_oleDropTarget;				// drag via OLE

	UINT			m_nConnectionTimeout;			// timeout connessione
	BOOL			m_bUseWinsockTimeout;
	CSyncProcesses	m_TimeoutCritSect;
	char			m_szCurrentLocation[MAX_URL+1];
	QWORD		m_qwCurrentAmount;
	HANDLE		m_hTimeoutEvent;

	HANDLE		m_hStreamRipperEvent;
	CSyncProcesses	m_StreamRipperSync;

	BOOL			m_bInBreakMessage;				// sincronizzazione
	BOOL			m_bInCancelDialog;

	CWndLayered	m_wndLayered;					// trasparenza

	DECLARE_MESSAGE_MAP()
};

#endif // _WALLPAPERCRAWLERDLG_H
