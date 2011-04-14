/*
	CAudioCDA.h
	Classe (derivata) per l'interfaccia audio verso i files di tipo .cda (tracce audio del CD)
	Come decoder viene utilizzata l'interfaccia MCI, acceduta tramite le classi presenti in mciaudio.
	Luca Piergentili, 10/06/03
	lpiergentili@yahoo.com
*/
#ifndef _CAUDIOCDA_H
#define _CAUDIOCDA_H 1

#include <stdlib.h>
#include "window.h"
#include "mmaudio.h"
#include "mciaudio.h"
#include "CAudioObject.h"

/*
	CAudioCDA
*/
class CAudioCDA : public CAudioObject
{
public:
	CAudioCDA(HWND hWnd = NULL);
	virtual ~CAudioCDA();

	void			SetWindow		(HWND hWnd) {m_hWnd = hWnd;}
	void			SetMessage	(UINT nMsg) {m_nMsg = nMsg;}
	void			SetPriority	(UINT /*nPriority*/) {}

	MMAERROR		Open			(LPCSTR lpcszAudioFileName) {return(m_pMCIAudioCda ? m_pMCIAudioCda->Open(lpcszAudioFileName) : MCIERR_DEVICE_NOT_READY);}
	MMAERROR		Play			(void) {return(m_pMCIAudioCda ? m_pMCIAudioCda->Play() : MCIERR_DEVICE_NOT_READY);}
	MMAERROR		Pause		(void) {return(m_pMCIAudioCda ? m_pMCIAudioCda->Pause() : MCIERR_DEVICE_NOT_READY);}
	MMAERROR		Resume		(void) {return(m_pMCIAudioCda ? m_pMCIAudioCda->Resume() : MCIERR_DEVICE_NOT_READY);}
	MMAERROR		Stop			(void) {return(m_pMCIAudioCda ? m_pMCIAudioCda->Stop() : MCIERR_DEVICE_NOT_READY);}
	MMAERROR		Close		(void) {return(m_pMCIAudioCda ? m_pMCIAudioCda->Close() : MCIERR_DEVICE_NOT_READY);}

	mmAudioPlayerMode GetStatus	(void) {return(m_pMCIAudioCda ? m_pMCIAudioCda->GetStatus() : mmAudioPmClosed);}
	void			SetStatus		(mmAudioPlayerMode nPlayerMode) {m_pMCIAudioCda->SetStatus(nPlayerMode);}

	LPCSTR		GetTitle		(void) {return("");}
	LPCSTR		GetArtist		(void) {return("");}
	LPCSTR		GetAlbum		(void) {return("");}
	LPCSTR		GetGenre		(void) {return("");}
	LPCSTR		GetYear		(void) {return("");}
	int			GetTrack		(void) {return(0);}
	LPCSTR		GetComment	(void) {return("");}

	mmAudioFormat	GetFormat		(void) {return(mmAudioAfCda);};
	QWORD		GetLength		(long& lMinutes,long& lSeconds);
	int			GetBitRate	(void) {return(16);}
	long			GetFrequency	(void) {return(44100L);}
	LPCSTR		GetChannelMode	(void) {return(m_pMCIAudioCda ? "stereo" : "");}
	LPCSTR		GetFormatType	(void) {return("Audio CD");}

private:
	HWND			m_hWnd;
	UINT			m_nMsg;
	CMCIAudioCDA*	m_pMCIAudioCda;
};

#endif // _CAUDIOCDA_H
