/*
	CAudioWavInfo.h
	Classe dericata per le info sui files .wav.
	Luca Piergentili, 15/07/03
	lpiergentili@yahoo.com
*/
#ifndef _CAUDIOWAVINFO_H
#define _CAUDIOWAVINFO_H 1

#include <string.h>
#include "strings.h"
#include "window.h"
#include "mmaudio.h"
#include "mciaudio.h"
#include "CAudioInfoObject.h"

/*
	CAudioWavInfo
*/
class CAudioWavInfo : public CAudioInfoObject
{
public:
	CAudioWavInfo(LPCSTR lpcszWavFileName = NULL);
	virtual ~CAudioWavInfo();

	int		Link			(LPCSTR lpcszWavFileName) {return(m_pMCIAudioWave ? (m_pMCIAudioWave->Load(lpcszWavFileName)==MMSYSERR_NOERROR ? 1 : 0) : 0);}
	void		Unlink		(void);

	QWORD	GetFileSize	(void) {return(m_pMCIAudioWave ? m_mcifi.qwFileSize : 0L);}
	LPCSTR	GetFileName	(void) {return(m_pMCIAudioWave ? m_mcifi.szFilePath : "");}

	LPCSTR	GetTitle		(void) {return(m_pMCIAudioWave ? m_mcifi.szFileTitle : "");}

	QWORD	GetLength		(long& lMinutes,long& lSeconds);
	int		GetBitRate	(void) {return(m_pMCIAudioWave ? (m_mcifi.pwfx ? m_mcifi.pwfx->wBitsPerSample : 0) : 0);}
	long		GetFrequency	(void) {return(m_pMCIAudioWave ? (m_mcifi.pwfx ? m_mcifi.pwfx->nSamplesPerSec : 0L) : 0L);}
	LPCSTR	GetVersion	(void) {return(m_pMCIAudioWave ? m_mcifi.szFormatTag : "");}
	LPCSTR	GetChannelMode	(void) {return(m_pMCIAudioWave ? (m_mcifi.pwfx ? (m_mcifi.pwfx->nChannels==1 ? "mono" : "stereo") : "") : "");}

private:
	MCIFILEINFO	m_mcifi;
	CMCIAudioWave*	m_pMCIAudioWave;
};

#endif // _CAUDIOWAVINFO_H
