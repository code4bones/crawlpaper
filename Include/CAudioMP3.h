/*
	CAudioMP3.h
	Classe (derivata) per l'interfaccia audio verso i files di tipo .mp3
	Luca Piergentili, 07/06/03
	lpiergentili@yahoo.com

	decoder mpeg utilizzati fino ad ora (modificati dove necessario):
	- mad, MPEG audio decoder library by Robert Leslie - Underbit Technologies Inc. (http://www.underbit.com/products/mad/)
	- amp11 decoder by Niklas Beisert (http://www1.physik.tu-muenchen.de/~nbeisert/amp11.html)
	- MPEG 1,2,3 decoder/MAPlay decoder
*/
#ifndef _CAUDIOMP3_H
#define _CAUDIOMP3_H 1

#include <stdlib.h>
#include "macro.h"
#include "window.h"
#include "CThread.h"
#include <mmsystem.h>
#include "mmaudio.h"
#include "CAudioObject.h"
#include "maddll.h"
#include "CMP3Info.h"
#include "CId3Lib.h"

// referenza alla dll per il linker
#pragma comment(lib,"WinMM.lib")
#ifdef PRAGMA_MESSAGE_VERBOSE
#pragma message("\t\t\t"__FILE__"("STR(__LINE__)"): automatically linking with WinMM.dll")
#endif

/*
	CAudioMP3
*/
class CAudioMP3 : public CAudioObject
{
public:
	CAudioMP3(HWND hWnd = NULL);
	virtual ~CAudioMP3();

	void			SetWindow		(HWND /*hWnd*/) {}
	void			SetMessage	(UINT /*nMsg*/) {}
	void			SetPriority	(UINT /*nPriority*/) {}
	
	MMAERROR		Open			(LPCSTR lpcszAudioFileName);
	MMAERROR		Play			(void);
	MMAERROR		Pause		(void);
	MMAERROR		Resume		(void);
	MMAERROR		Stop			(void);
	MMAERROR		Close		(void);

	mmAudioPlayerMode GetStatus	(void) {return(m_nPlayerMode);}
	void			SetStatus		(mmAudioPlayerMode nPlayerMode) {m_nPlayerMode=nPlayerMode;}

	LPCSTR		GetTitle		(void) {return(m_id3Lib.GetTitle());}
	LPCSTR		GetArtist		(void) {return(m_id3Lib.GetArtist());}
	LPCSTR		GetAlbum		(void) {return(m_id3Lib.GetAlbum());}
	LPCSTR		GetGenre		(void) {return(m_id3Lib.GetGenre());}
	LPCSTR		GetYear		(void) {return(m_id3Lib.GetYear());}
	int			GetTrack		(void) {return(m_id3Lib.GetTrack());}
	LPCSTR		GetComment	(void) {return(m_id3Lib.GetComment());}

	mmAudioFormat	GetFormat		(void) {return(mmAudioAfMp3);};
	QWORD		GetLength		(long& lMinutes,long& lSeconds);
	int			GetBitRate	(void) {return(m_Mp3Info.GetBitRate());}
	long			GetFrequency	(void) {return(m_Mp3Info.GetFrequency());}
	LPCSTR		GetChannelMode	(void) {return(m_Mp3Info.GetChannelMode());}
	LPCSTR		GetFormatType	(void) {_snprintf(m_szMPEGType,sizeof(m_szMPEGType)-1,"MPEG layer %s v.%.1f",m_Mp3Info.GetLayer(),m_Mp3Info.GetVersion()); return(m_szMPEGType);}

private:

	HWND				m_hWnd;
	UINT				m_nMsg;
	UINT				m_nThreadPriority;
	char				m_szAudioFileName[_MAX_PATH+1];
	MAD_DATA*			m_pMadData;
	MAD_THREAD_PARAMS	m_playerThreadParams;
//	CThread*			m_pPlayThread;
	CWinThread*		m_pPlayThread;
	mmAudioPlayerMode	m_nPlayerMode;
	CMP3Info			m_Mp3Info;
	CId3Lib			m_id3Lib;
	char				m_szMPEGType[32];
};

#endif // _CAUDIOMP3_H
