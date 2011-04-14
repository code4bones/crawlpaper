/*
	WallPaperPlayerDlg.h
	Dialogo per il player audio.
	Luca Piergentili, 09/11/01
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
#ifndef _WALLPAPERPLAYERDLG_H
#define _WALLPAPERPLAYERDLG_H 1

#include "window.h"
#include "win32api.h"
#include "CNodeList.h"
#include "CRand.h"
#include "CSync.h"
#include "CUrl.h"
#include "CTrayIcon.h"
#include "CTaskbarNotifier.h"
#include "CId3Lib.h"
#include "CAudioPlayer.h"
#include "CDialogEx.h"
#include "CArchive.h"
#include "WallPaperConfig.h"
#include "resource.h"

// titolo dialogo e nomi oggetti per sincronizzazione
#define IDS_DIALOG_PLAYER_TITLE	"WallPaper Audio Player"
#define IDS_PLAYER_MUTEX_NAME		IDS_DIALOG_PLAYER_TITLE"Mutex"
#define IDS_PLAYER_EVENT_NAME		IDS_DIALOG_PLAYER_TITLE"Eventc"

// comandi, mantenere allineato con OnPlayerSetStatus()
#define AUDIOPLAYER_COMMAND_NULL				0	// mantenere a 0
#define AUDIOPLAYER_COMMAND_EXIT				1
#define AUDIOPLAYER_COMMAND_STOP				2
#define AUDIOPLAYER_COMMAND_PLAY				3
#define AUDIOPLAYER_COMMAND_PAUSE				4
#define AUDIOPLAYER_COMMAND_PLAY_FROM_FILE		5
#define AUDIOPLAYER_COMMAND_QUEUE_FROM_FILE		6
#define AUDIOPLAYER_COMMAND_PLAY_FROM_LIST		7
#define AUDIOPLAYER_COMMAND_QUEUE_FROM_LIST		8
#define AUDIOPLAYER_COMMAND_INCREASE_VOLUME		9
#define AUDIOPLAYER_COMMAND_DECREASE_VOLUME		10
#define AUDIOPLAYER_COMMAND_TOGGLE_PLAYING		11

// modalita'
#define AUDIOPLAYER_MODE_NOPOPUP				12
#define AUDIOPLAYER_MODE_TASKBAR_POPUP			13
#define AUDIOPLAYER_MODE_BALLOON_POPUP			14
#define AUDIOPLAYER_MODE_RELOAD_BALLOON			15
#define AUDIOPLAYER_MODE_SETPOPUPLIST			16
#define AUDIOPLAYER_MODE_LOADPOPUPLIST			17
#define AUDIOPLAYER_MODE_LOADBALLOONLIST		18
#define AUDIOPLAYER_MODE_AUTOSKIP				19
#define AUDIOPLAYER_MODE_FEEDBACK				20

/*
	AUDIOINFO
	struttura per le info sul file audio
*/
struct AUDIOINFO {
	char		szAudioFileName[_MAX_FILEPATH+1];
	char		szAudioFileExt[8];
	char		szTitle[ID3V1_TITLE_SIZE+1];
	char		szArtist[ID3V1_ARTIST_SIZE+1];
	char		szAlbum[ID3V1_ALBUM_SIZE+1];
	char		szYear[ID3V1_YEAR_SIZE+1];
	char		szTrack[ID3V1_TRACK_SIZE+1];
	char		szDuration[16];
	char		szGenre[128];
	long		lMinutes;
	long		lSeconds;
	QWORD	qwSize;
	char		szBitrate[16];
	char		szFrequency[16];
	char		szChannelMode[16];
	char		szFormatType[32];
	char		szComment[ID3V1_COMMENT_SIZE+1];
};

/*
	AUDIOITEM
	struttura per l'elemento della lista dei files audio
*/
struct AUDIOITEM {
	LPARAM	lparam;
	int		index;
	char		file[_MAX_FILEPATH+1];
};

/*
	CAudioFilesList
	classe per la lista dei files audio
*/
class CAudioFilesList : public CNodeList, public CSyncThreads
{
public:
	CAudioFilesList() : CNodeList(), CSyncThreads("CAudioFilesList") {}
	virtual ~CAudioFilesList() {CNodeList::DeleteAll();}
	void* Create(void)
	{
		return(new AUDIOITEM);
	}
	void* Initialize(void* pVoid)
	{
		AUDIOITEM* pData = (AUDIOITEM*)pVoid;
		if(!pData)
			pData = (AUDIOITEM*)Create();
		if(pData)
			memset(pData,'\0',sizeof(AUDIOITEM));
		return(pData);
	}
	BOOL PreDelete(ITERATOR iter)
	{
		if((AUDIOITEM*)iter->data)
			delete ((AUDIOITEM*)iter->data),iter->data = (AUDIOITEM*)NULL;
		return(TRUE);
	}
	int Size(void) {return(sizeof(AUDIOITEM));}
#ifdef _DEBUG
	const char* Signature(void) {return("CAudioFilesList");}
#endif
};

/*
	CWallPaperPlayerDlg
*/
class CWallPaperPlayerDlg : public CDialogEx
{
	DECLARE_DYNCREATE(CWallPaperPlayerDlg)

protected: // provide default constructor only for dynamic creation, private/protected to prevent it from being called from outside the class implementation
	CWallPaperPlayerDlg() {::MessageBox(NULL,"PANIC! This shouldn't happen!","CWallPaperPlayerDlg()",MB_OK|MB_ICONERROR|MB_TASKMODAL|MB_SETFOREGROUND|MB_TOPMOST);}

public:
	CWallPaperPlayerDlg(HWND hWndParent);
	~CWallPaperPlayerDlg() {}

	void				DoDataExchange			(CDataExchange* pDX);
	LRESULT			OnNotifyTrayIcon		(WPARAM /*wParam*/,LPARAM lParam);
	LRESULT			OnNotifyTaskbarPopup	(WPARAM /*wParam*/,LPARAM lParam);
	void				OnSysCommand			(UINT nID,LPARAM lParam);
	BOOL				OnInitDialog			(void);
	inline BOOL		OnInitDialogEx			(UINT /*nInstanceCount*/ = -1,LPCSTR /*lpcszCommandLine*/ = NULL) {return(FALSE);}
	void				OnExit				(void);
	void				OnTimer				(UINT nIDEvent);
	void				OnFavouriteAudio		(void);
	LRESULT			OnFavouriteAudio		(WPARAM /*wParam*/,LPARAM /*lParam*/);
	void				OnMP3Tag				(void);
	inline void		OnOpenSoundControl		(void) {OpenSoundControl();}
	void				OnEd2kQuery			(void);

	void				OnButtonWebSearch		(void);
	void				OnButtonInfo			(void);
	void				OnButtonPlay			(void);
	void				OnButtonStop			(void);
	void				OnButtonPause			(void);
	void				OnButtonPrev			(void);
	void				OnButtonNext			(void);
	void				OnButtonRemove			(void);
	void				OnButtonDelete			(void);
	void				OnButtonMoveTo			(void);
	void				OnButtonCopyTo			(void);

	LRESULT			OnPlayer				(WPARAM wParam,LPARAM lParam);
	LRESULT			OnPlayerSetMode		(WPARAM wParam,LPARAM lParam);
	LRESULT			OnPlayerSetVolume		(WPARAM wParam,LPARAM /*lParam*/);
	LRESULT			OnPlayerPlay			(WPARAM /*wParam*/,LPARAM /*lParam*/);
	LRESULT			OnPlayerStop			(WPARAM /*wParam*/,LPARAM /*lParam*/);
	LRESULT			OnPlayerPause			(WPARAM /*wParam*/,LPARAM /*lParam*/);
	LRESULT			OnPlayerNext			(WPARAM /*wParam*/,LPARAM /*lParam*/);
	LRESULT			OnPlayerPrev			(WPARAM /*wParam*/,LPARAM /*lParam*/);
	LRESULT			OnPlayerRemove			(WPARAM wParam,LPARAM lParam);
	LRESULT			OnPlayerRemoveAll		(WPARAM /*wParam*/,LPARAM /*lParam*/);
	LRESULT			OnPlayerExit			(WPARAM /*wParam*/,LPARAM /*lParam*/);
	LRESULT			OnPlayerGetStatus		(WPARAM /*wParam*/,LPARAM /*lParam*/);
	LRESULT			OnPlayerSetStatus		(WPARAM wParam,LPARAM /*lParam*/);
	LRESULT			OnPlayerSetPopupList	(WPARAM wParam,LPARAM lParam);
	LRESULT			OnPlayerException		(WPARAM wParam,LPARAM lParam);

private:
	void				GetAudioInfo			(void);
	void				ShowTaskbarPopup		(void);
	void				ShowBalloonPopup		(BOOL bForceBalloon = FALSE);
	void				SetTrayIcon			(LPCSTR lpcszTooltipText = NULL,UINT nIconID = IDI_ICON_PLAYER);
	BOOL				OpenSoundControl		(void);

	BOOL				m_bFirstCall;					// per ricavare l'ultimo brano riprodotto una sola volta
	HWND				m_hWndParent;					// finestra padre (chiamante)
	UINT				m_nTimerID;					// id timer per passaggio al brano successivo
	CTrayIcon*		m_pTrayIcon;					// oggetto per l'icona nella system tray
	char				m_szTitleMenuText[1024];			// testo per il titolo del menu popup per la tray icon
	char				m_szTrayIconTooltipText[256];		// testo per il tooltip per l'icona nella system tray
	CTaskbarNotifier*	m_pTaskbarNotifier;				// oggetto per la finestra popup
	char				m_szTaskbarToolTipText[1024];		// testo per il tooltip per la finestra popup
	int				m_nTaskbarPopupMode;			// flag per visualizzazione finestra popup
	CTaskbarPopupList*	m_pListTaskbarBitmaps;			// lista dei bitmap per i popup
	BOOL				m_bAutoSkip;					// per il passaggio automatico al brano successivo
	BOOL				m_bFeedback;					// per l'invio dei messaggi al chiamante
	AUDIOINFO			m_AudioInfo;					// elemento corrente
	TASKBARPOPUP		m_TaskbarPopup;				// elemento corrente nella lista dei popup
	int				m_nPopupIndex;					// indice per l'elemento corrente nella lista dei popup
	char				m_szLastPlayedSong[_MAX_FILEPATH+1];// ultimo file audio riprodotto
	char				m_szAudioFileName[_MAX_FILEPATH+1];// nome file audio in riproduzione
	char				m_szFavouriteName[_MAX_FILEPATH+1];// nome file audio per aggiunta alla playlist favoriti
	TAGS				m_Tags[13];					// struttura per le macro da sostituire nel testo del tooltip per il popup
	CWallPaperConfig*	m_pConfig;					// puntatore alla configurazione corrente
	int				m_nCurrentAudioIndex;			// indice nella lista dei files audio al brano corrente
	CAudioFilesList	m_AudioFilesList;				// lista per i files audio da riprodurre
	CAudioPlayer		m_AudioPlayer;					// oggetto per la riproduzione dei files audio
	mmAudioPlayerMode	m_mmInternalMode;				// per evitare che il chiamante spari a raffica files da riprodurre
	CSyncThreads		m_syncAudioPlay;				// per sincronizzare la riproduzione
	CRand			m_randomNumberGenerator;			// per la modalita' random
	CWinThread*		m_pMP3TagWinThread;				// per il dialogo per i tags mp3
	CFindFile*		m_pFindAudioIcons;				// per le icone per il balloon
	int				m_nCurrentAudioIcon;
	int				m_nAudioIconsCount;

	DECLARE_MESSAGE_MAP()
};

#endif // _WALLPAPERPLAYERDLG_H
