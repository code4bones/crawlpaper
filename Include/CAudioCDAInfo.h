/*
	CAudioCDAInfo.h
	Classe derivata per le info sui files .cda.
	Luca Piergentili, 14/07/03
	lpiergentili@yahoo.com
*/
#ifndef _CAUDIOCDAINFO_H
#define _CAUDIOCDAINFO_H 1

#include <stdlib.h>
#include <string.h>
#include "strings.h"
#include "window.h"
#include "mmaudio.h"
#include "mciaudio.h"
#include "CAudioInfoObject.h"

/*
	CAudioCDAInfo
*/
class CAudioCDAInfo : public CAudioInfoObject
{
public:
	CAudioCDAInfo(LPCSTR lpcszCdaFileName = NULL);
	virtual ~CAudioCDAInfo();

	int		Link			(LPCSTR lpcszCdaFileName);
	void		Unlink		(void);

	QWORD	GetFileSize	(void) {return(m_cdaTrackInfo.qwTrackSize);}
	LPCSTR	GetFileName	(void) {return(m_cdaTrackInfo.szTrackName);}

	LPCSTR	GetTitle		(void) {return(m_cdaTrackInfo.szTrackName);}
	int		GetTrack		(void) {return(atoi(m_cdaTrackInfo.szTrackNumber));}

	QWORD	GetLength		(long& lMinutes,long& lSeconds);
	LPCSTR	GetChannelMode	(void) {return("stereo");}

private:
	CDATRACKINFO	m_cdaTrackInfo;
	CMCIAudioCDA*	m_pMCIAudioCDA;
};

#endif // _CAUDIOCDAINFO_H
