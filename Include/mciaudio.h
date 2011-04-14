/*
	mciaudio.h
	Interfaccia con l'api MCI per i vari formati audio.
	Le classi definite sotto sono quelle che vengono utilizzate dai players relativi (CAudioWav e CAudioCDA).
	Luca Piergentili, 14/07/03
	lpiergentili@yahoo.com

	Parte del codice e' stata derivata (modificata e riadattata) da esempi MS:
	
	ACMAPP:
	This is a sample application that demonstrates how to use the Audio Compression
	Manager API's in Windows. This application is also useful as an ACM driver test.
	WARNING: There are known bugs with MCI Wave EDITING operations on non-PCM formats.
	THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
	EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES
	OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
	Copyright (C) 1992 - 1997 Microsoft Corporation.  All Rights Reserved.

	CDPLAYER: Accesses Multimedia Devices Using MCI
	Note: The source files for this sample application may be incomplete. These files were
	acquired directly from the Microsoft(R) Systems Journal. We have decided to include the
	source files in the Microsoft Development Library in untested, unmodified form
	[...]
	CDPLAYER originally appeared in the November 24, 1992, issue of PC Magazine. Version 2.0
	[...]
	CDPLAYER is a companion application for "Simplify Access to Complex Multimedia Devices with
	the Media Control Interface" by Jeff Prosise (Microsoft Systems Journal, Vol. 9, No. 4).
*/
#ifndef _MCIAUDIO_H
#define _MCIAUDIO_H 1

#include <stdlib.h>
#include "macro.h"
#include "window.h"
#include "CNodeList.h"
#include <mmsystem.h>
#include <mmreg.h>
#include "mmaudio.h"

// crea la referenza alla dll per il linker
#pragma comment(lib,"MsAcm32.lib")
#ifdef PRAGMA_MESSAGE_VERBOSE
#pragma message("\t\t\t"__FILE__"("STR(__LINE__)"): automatically linking with MsAcm32.dll")
#endif

// generiche
#define MCIAUDIO_TIMER_RESOLUTION		500L
#define MCIAUDIO_MAX_COMMAND_CHARS		255
#define MCIAUDIO_MAX_WAV_FORMAT_TAG	64

/*
	MCIERROR
*/
#ifndef _MCIERROR_
#define _MCIERROR_ 1
typedef DWORD MCIERROR;
#endif

/*
	MCIFILEINFO
*/
#ifndef _MCIFILEINFO_DEFINED
#define _MCIFILEINFO_DEFINED 1
typedef struct stMCIFILEINFO
{
	char			szFilePath[_MAX_PATH+1];					// pathname nome file
	char			szFileTitle[_MAX_PATH+1];				// solo nome
	QWORD		qwFileSize;							// dimensione in bytes
	UINT			uDosChangeDate;						// data
	UINT			uDosChangeTime;						// ora
	DWORD		dwFileAttributes;						// attributi

	char			szFormatTag[MCIAUDIO_MAX_WAV_FORMAT_TAG+1];	// tipo formato (PCM, etc.)
	LPWAVEFORMATEX	pwfx;								// ptr ai dati relativi al .wav
	UINT			cbwfx;								// dimensione struct di cui sopra

	DWORD		dwDataBytes;							// dimensioni in bytes del blocco
	DWORD		dwDataSamples;							// numero di samples
	double		dlTotalLength;							// durata (in minuti)
	long			lMinutes;								// min. totali
	long			lSeconds;								// sec. totali

	mmAudioPlayerMode mmaPlayerStatus;						// status corrente

} MCIFILEINFO,*PMCIFILEINFO;
#endif

/*
	CMCIAudioWave
*/
class CMCIAudioWave {
public:
	CMCIAudioWave(PMCIFILEINFO pMcifi);
	virtual ~CMCIAudioWave();

	MCIERROR	Load			(LPCSTR lpcszWavFileName);
	
	MCIERROR	Open			(LPCSTR lpcszWavFileName = NULL,UINT uWaveInId = WAVE_MAPPER,UINT uWaveOutId = WAVE_MAPPER);
	MCIERROR	Play			(void);
	MCIERROR	Pause		(void);
	MCIERROR	Resume		(void);
	MCIERROR	Stop			(void);
	MCIERROR	Close		(void);

	MCIERROR	SetPos		(UINT uCode,int nPos,int nMinPos/*scroll range*/,int nMaxPos/*scroll range*/);
	MCIERROR	Start		(void);
	MCIERROR	End			(void);

	MCIERROR	Send			(LPCSTR lpcszCommand,LPSTR pszReturn,UINT cbReturn);
	LPCSTR	GetStatus		(void);

	static VOID CALLBACK WaveTimerProc(HWND hwnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTime);

private:
	BOOL					m_bInitialized;
	PMCIFILEINFO			m_pMcifi;
	UINT					m_nTimerId;
	MCIDEVICEID			m_nMCIDeviceId;
	char					m_szMCIDeviceAlias[32];
	char					m_szMCIDeviceStatus[MCIAUDIO_MAX_COMMAND_CHARS+1];
};

/*
	CDATRACKINFO
*/
struct CDATRACKINFO {
	char		szTrackName[_MAX_PATH+1];
	int		nTrack;
	char		szTrackNumber[8];
	QWORD	qwTrackSize;
	long		lMinutes;
	long		lSeconds;
	long		lFrames;
};

/*
	CCDATrackInfoList
*/
class CCDATrackInfoList : public CNodeList
{
public:
	CCDATrackInfoList() : CNodeList() {}
	virtual ~CCDATrackInfoList() {CNodeList::DeleteAll();}
	void* Create(void)
	{
		return(new CDATRACKINFO);
	}	
	void* Initialize(void* pVoid)
	{
		CDATRACKINFO* pData = (CDATRACKINFO*)pVoid;
		if(!pData)
			pData = (CDATRACKINFO*)Create();
		if(pData)
			memset(pData,'\0',sizeof(CDATRACKINFO));
		return(pData);
	}
	BOOL PreDelete(ITERATOR iter)
	{
		if((CDATRACKINFO*)iter->data)
			delete ((CDATRACKINFO*)iter->data),iter->data = (CDATRACKINFO*)NULL;
		return(TRUE);
	}
	int Size(void) {return(sizeof(CDATRACKINFO));}
#ifdef _DEBUG
	const char* Signature(void) {return("CCDATrackInfoList");}
#endif
};

/*
	CMCIAudioCDA
*/
class CMCIAudioCDA : public CWnd
{
public:
	CMCIAudioCDA();
	virtual ~CMCIAudioCDA();

	MCIERROR				Open			(LPCSTR lpcszCdaFileName);
	MCIERROR				Play			(void);
	MCIERROR				Pause		(void);
	MCIERROR				Resume		(void);
	MCIERROR				Stop			(void);
	MCIERROR				Close		(void);
	
	inline mmAudioPlayerMode	GetStatus		(void) const {return(m_mmaPlayerStatus);}
	inline void			SetStatus		(mmAudioPlayerMode nPlayerMode) {m_mmaPlayerStatus = nPlayerMode;}
	
	MCIERROR				GetTrackInfo	(CDATRACKINFO* pCdaTrackInfo);

static VOID CALLBACK CdaTimerProc(HWND hwnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTime);

protected:
	void					OnDestroy		(void);
	LRESULT				OnMciMessage	(WPARAM wParam,LPARAM lParam);

private:
	static int			m_nRefCount;
	static BOOL			m_bInitialized;
	
	mmAudioPlayerMode		m_mmaPlayerStatus;
	int					m_nTrackNumber;
	char					m_szTrackNumber[8];
	static CCDATrackInfoList* m_plistCDATrackInfo;
	UINT					m_nTimerId;
	HWND					m_hWndTimer;
	MCIDEVICEID			m_nMCIDeviceId;

	DECLARE_MESSAGE_MAP()
};

#endif // _MCIAUDIO_H
