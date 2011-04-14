/*
	CAudioInfo.h
	Classe (semi-factory) per le info sull'audio.
	Ricava le info sui files audio supportati tramite gli oggetti derivati dalla classe CAudioInfoObject.
	Luca Piergentili, 14/07/03
	lpiergentili@yahoo.com
*/
#ifndef _CAUDIOINFO_H
#define _CAUDIOINFO_H 1

#include <stdlib.h>
#include <string.h>
#include "strings.h"
#include "window.h"
#include <mmsystem.h>
#include "mmaudio.h"
#include "CAudioInfoObject.h"
#include "CAudioWavInfo.h"
#include "CAudioCDAInfo.h"
#include "CAudioMP3Info.h"

/*
	CAudioInfo
*/
class CAudioInfo
{
public:
	CAudioInfo(LPCSTR lpcszAudioFileName = NULL);
	virtual ~CAudioInfo();

	CAudioInfoObject* GetObject(void) {return(m_pAudioInfoObject);}

	int		Link			(LPCSTR lpcszAudioFileName);
	void		Unlink		(void) {if(m_pAudioInfoObject) m_pAudioInfoObject->Unlink();}

	QWORD	GetFileSize	(void) {return(m_pAudioInfoObject ? m_pAudioInfoObject->GetFileSize() : 0L);}
	LPCSTR	GetFileName	(void) {return(m_pAudioInfoObject ? m_pAudioInfoObject->GetFileName() : "");}

	LPCSTR	GetTitle		(void) {return(m_pAudioInfoObject ? m_pAudioInfoObject->GetTitle() : "");}
	LPCSTR	GetArtist		(void) {return(m_pAudioInfoObject ? m_pAudioInfoObject->GetArtist() : "");}
	LPCSTR	GetAlbum		(void) {return(m_pAudioInfoObject ? m_pAudioInfoObject->GetAlbum() : "");}
	LPCSTR	GetGenre		(void) {return(m_pAudioInfoObject ? m_pAudioInfoObject->GetGenre() : "");}
	LPCSTR	GetYear		(void) {return(m_pAudioInfoObject ? m_pAudioInfoObject->GetYear() : "");}
	int		GetTrack		(void) {return(m_pAudioInfoObject ? m_pAudioInfoObject->GetTrack() : 0);}
	LPCSTR	GetComment	(void) {return(m_pAudioInfoObject ? m_pAudioInfoObject->GetComment() : "");}

	QWORD	GetLength		(long& lMinutes,long& lSeconds) {return(m_pAudioInfoObject ? m_pAudioInfoObject->GetLength(lMinutes,lSeconds) : 0L);}
	int		GetBitRate	(void) {return(m_pAudioInfoObject ? m_pAudioInfoObject->GetBitRate() : 0L);}
	long		GetFrequency	(void) {return(m_pAudioInfoObject ? m_pAudioInfoObject->GetFrequency() : 0L);}
	LPCSTR	GetLayer		(void) {return(m_pAudioInfoObject ? m_pAudioInfoObject->GetLayer() : "");}
	LPCSTR	GetVersion	(void) {return(m_pAudioInfoObject ? m_pAudioInfoObject->GetVersion() : "");}
	LPCSTR	GetChannelMode	(void) {return(m_pAudioInfoObject ? m_pAudioInfoObject->GetChannelMode() : "");}

private:
	CAudioInfoObject*		m_pAudioInfoObject;
	CAudioWavInfo*			m_pAudioInfoWav;
	CAudioCDAInfo*			m_pAudioInfoCDA;
	CAudioMp3Info*			m_pAudioInfoMPeg;
};

#endif // _CAUDIOINFO_H
