/*
	CAudioWav.h
	Classe (derivata) per l'interfaccia audio verso i files di tipo .wav
	Come decoder viene utilizzata l'interfaccia MCI, acceduta tramite le classi presenti in mciaudio.
	Luca Piergentili, 13/07/03
	lpiergentili@yahoo.com
*/
#ifndef _CAUDIOWAV_H
#define _CAUDIOWAV_H 1

#include <stdlib.h>
#include "window.h"
#include "mmaudio.h"
#include "mciaudio.h"
#include "CAudioObject.h"

/*
	CAudioWav
*/
class CAudioWav : public CAudioObject
{
public:
	CAudioWav(HWND hWnd = NULL);
	virtual ~CAudioWav();

	void			SetWindow		(HWND /*hWnd*/) {}
	void			SetMessage	(UINT /*nMsg*/) {}
	void			SetPriority	(UINT /*nPriority*/) {}

	MMAERROR		Open			(LPCSTR lpcszAudioFileName) {return(m_pMCIAudioWave ? m_pMCIAudioWave->Open(lpcszAudioFileName) : MCIERR_DEVICE_NOT_READY);}
	MMAERROR		Play			(void) {return(m_pMCIAudioWave ? m_pMCIAudioWave->Play() : MCIERR_DEVICE_NOT_READY);}
	MMAERROR		Pause		(void) {return(m_pMCIAudioWave ? m_pMCIAudioWave->Pause() : MCIERR_DEVICE_NOT_READY);}
	MMAERROR		Resume		(void) {return(m_pMCIAudioWave ? m_pMCIAudioWave->Resume() : MCIERR_DEVICE_NOT_READY);}
	MMAERROR		Stop			(void) {return(m_pMCIAudioWave ? m_pMCIAudioWave->Stop() : MCIERR_DEVICE_NOT_READY);}
	MMAERROR		Close		(void) {return(m_pMCIAudioWave ? m_pMCIAudioWave->Close() : MCIERR_DEVICE_NOT_READY);}

	mmAudioPlayerMode GetStatus	(void) {return(m_pMCIAudioWave ? m_mcifi.mmaPlayerStatus : mmAudioPmClosed);}
	void			SetStatus		(mmAudioPlayerMode nPlayerMode) {m_mcifi.mmaPlayerStatus = nPlayerMode;}

	LPCSTR		GetTitle		(void) {return(m_pMCIAudioWave ? m_mcifi.szFileTitle : "");}
	LPCSTR		GetArtist		(void) {return("");}
	LPCSTR		GetAlbum		(void) {return("");}
	LPCSTR		GetGenre		(void) {return("");}
	LPCSTR		GetYear		(void) {return("");}
	int			GetTrack		(void) {return(0);}
	LPCSTR		GetComment	(void) {return("");}

	mmAudioFormat	GetFormat		(void) {return(mmAudioAfWav);};
	QWORD		GetLength		(long& lMinutes,long& lSeconds);
	int			GetBitRate	(void) {return(m_pMCIAudioWave ? m_mcifi.pwfx->wBitsPerSample : 0);}
	long			GetFrequency	(void) {return(m_pMCIAudioWave ? m_mcifi.pwfx->nSamplesPerSec : 0L);}
	LPCSTR		GetChannelMode	(void) {return(m_pMCIAudioWave ? (m_mcifi.pwfx->nChannels==1 ? "mono" : "stereo") : "");}
	LPCSTR		GetFormatType	(void) {return(m_pMCIAudioWave ? m_mcifi.szFormatTag : "");}

private:
	HWND			m_hWnd;
	UINT			m_nMsg;
	MCIFILEINFO	m_mcifi;
	CMCIAudioWave*	m_pMCIAudioWave;
};

#endif // _CAUDIOWAV_H
