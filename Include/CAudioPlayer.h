/*
	CAudioPlayer.h
	Classe (semi-factory) per il player audio.
	Gestisce la riproduzione dei files audio supportati tramite gli oggetti derivati dalla classe CAudioObject.
	Luca Piergentili, 12/07/03
	lpiergentili@yahoo.com
*/
#ifndef _CAUDIOPLAYER_H
#define _CAUDIOPLAYER_H 1

#include <string.h>
#include "strings.h"
#include "window.h"
#include <mmsystem.h>
#include "mmaudio.h"
#include "CAudioObject.h"
#include "CAudioVolume.h"
#include "CAudioWav.h"
#include "CAudioCDA.h"
#include "CAudioMP3.h"

/*
	CAudioPlayer

	Notare che la Open() e la Close() non sono relative all'oggetto <player> in se', ma al file
	che deve essere riprodotto, motivo per cui il chiamante non puo' aprire nel costruttore e
	chiudere nel distruttore, ma deve farlo per ogni file da riprodurre.
	La Open() distingue poi a seconda del tipo file che viene passato per utilizzare l'oggetto
	(derivato) appropiato.
*/
class CAudioPlayer
{
public:
	CAudioPlayer(HWND hWnd = NULL);
	virtual ~CAudioPlayer();

	static BOOL			IsAudioFile	(LPCSTR lpcszAudioFileName);		// formato audio, non necessariamente supportato dal player
	static BOOL			IsSupportedFormat(LPCSTR lpcszAudioFileName);	// formato audio supportato dal player

	inline CAudioObject*	GetObject		(void) const {return(m_pAudioObject);}

	inline void			SetPriority	(UINT nPriority) {if(m_pAudioObject) m_pAudioObject->SetPriority(nPriority);}

	BOOL					Open			(LPCSTR lpcszAudioFileName);
	inline BOOL			Play			(void) {return(m_pAudioObject ? m_pAudioObject->Play()==MMSYSERR_NOERROR : FALSE);}
	inline BOOL			Pause		(void) {return(m_pAudioObject ? m_pAudioObject->Pause()==MMSYSERR_NOERROR : FALSE);}
	inline BOOL			Resume		(void) {return(m_pAudioObject ? m_pAudioObject->Resume()==MMSYSERR_NOERROR : FALSE);}
	inline BOOL			Stop			(void) {return(m_pAudioObject ? m_pAudioObject->Stop()==MMSYSERR_NOERROR : FALSE);}
	inline BOOL			Close		(void) {return(m_pAudioObject ? m_pAudioObject->Close()==MMSYSERR_NOERROR : FALSE);}

	inline mmAudioPlayerMode GetStatus		(void) {return(m_pAudioObject ? m_pAudioObject->GetStatus() : mmAudioPmClosed);}
	inline void			SetStatus		(mmAudioPlayerMode nPlayerMode) {if(m_pAudioObject) m_pAudioObject->SetStatus(nPlayerMode);}

	inline LPCSTR			GetTitle		(void) {return(m_pAudioObject ? m_pAudioObject->GetTitle() : "");}
	inline LPCSTR			GetArtist		(void) {return(m_pAudioObject ? m_pAudioObject->GetArtist() : "");}
	inline LPCSTR			GetAlbum		(void) {return(m_pAudioObject ? m_pAudioObject->GetAlbum() : "");}
	inline LPCSTR			GetGenre		(void) {return(m_pAudioObject ? m_pAudioObject->GetGenre() : "");}
	inline LPCSTR			GetYear		(void) {return(m_pAudioObject ? m_pAudioObject->GetYear() : "");}
	inline UINT			GetTrack		(void) {return(m_pAudioObject ? m_pAudioObject->GetTrack() : 0);}
	inline LPCSTR			GetComment	(void) {return(m_pAudioObject ? m_pAudioObject->GetComment() : "");}

	inline mmAudioFormat	GetFormat		(void) {return(m_pAudioObject ? m_pAudioObject->GetFormat() : mmAudioAfUnknow);}
	inline QWORD			GetLength		(long& lMinutes,long& lSeconds) {return(m_pAudioObject ? m_pAudioObject->GetLength(lMinutes,lSeconds) : 0L);}
	inline int			GetBitRate	(void) {return(m_pAudioObject ? m_pAudioObject->GetBitRate() : 0);}
	inline long			GetFrequency	(void) {return(m_pAudioObject ? m_pAudioObject->GetFrequency() : 0L);}
	inline LPCSTR			GetChannelMode	(void) {return(m_pAudioObject ? m_pAudioObject->GetChannelMode() : "");}
	inline LPCSTR			GetFormatType	(void) {return(m_pAudioObject ? m_pAudioObject->GetFormatType() : "");}

	inline BOOL			IncreaseVolume	(long lValue = -1L) {return(m_audioVolume.Increase(lValue));}
	inline BOOL			DecreaseVolume	(long lValue = -1L) {return(m_audioVolume.Decrease(lValue));}

private:
	HWND					m_hWnd;
	CAudioObject*			m_pAudioObject;
	CAudioVolume			m_audioVolume;
	CAudioWav*			m_pAudioWav;
	CAudioCDA*			m_pAudioCDA;
	CAudioMP3*			m_pAudioMPeg;
};

#endif // _CAUDIOPLAYER_H
