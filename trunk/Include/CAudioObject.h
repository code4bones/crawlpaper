/*
	CAudioObject.h
	Classe base per la definizione dell'oggetto da cui derivare le classi
	(CAudioWav, CAudioCDA, CAudioMP3, etc.) per il supporto dei formati audio.
	Luca Piergentili, 12/07/03
	lpiergentili@yahoo.com
*/
#ifndef _CAUDIOOBJECT_H
#define _CAUDIOOBJECT_H 1

#include "window.h"
#include "mmaudio.h"

/*
	CAudioObject
*/
class CAudioObject {
public:
	CAudioObject() {}
	virtual ~CAudioObject() {}

	virtual void				SetWindow		(HWND hWnd) = 0;
	virtual void				SetMessage	(UINT nMsg) = 0;
	virtual void				SetPriority	(UINT nPriority) = 0;

	virtual MMAERROR			Open			(LPCSTR lpcszAudioFileName) = 0;		
	virtual MMAERROR			Play			(void) = 0;
	virtual MMAERROR			Pause		(void) = 0;
	virtual MMAERROR			Resume		(void) = 0;
	virtual MMAERROR			Stop			(void) = 0;
	virtual MMAERROR			Close		(void) = 0;

	virtual mmAudioPlayerMode	GetStatus		(void) = 0;
	virtual void				SetStatus		(mmAudioPlayerMode) = 0;

	virtual LPCSTR				GetTitle		(void) = 0;
	virtual LPCSTR				GetArtist		(void) = 0;
	virtual LPCSTR				GetAlbum		(void) = 0;
	virtual LPCSTR				GetGenre		(void) = 0;
	virtual LPCSTR				GetYear		(void) = 0;
	virtual int				GetTrack		(void) = 0;
	virtual LPCSTR				GetComment	(void) = 0;

	virtual mmAudioFormat		GetFormat		(void) = 0;
	virtual QWORD				GetLength		(long& lMinutes,long& lSeconds) = 0;
	virtual int				GetBitRate	(void) = 0;						// 8, 16, etc. bit/128, 256, etc. kbps
	virtual long				GetFrequency	(void) = 0;						// 48000, 44100, etc. khz
	virtual LPCSTR				GetChannelMode	(void) = 0;						// stereo, mono, etc.
	virtual LPCSTR				GetFormatType	(void) = 0;						// PCM/MPEG layer 1, etc.
};

#endif // _CAUDIOOBJECT_H
