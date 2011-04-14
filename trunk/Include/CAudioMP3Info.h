/*
	CAudioMP3Info.h
	Classe derivata per le info/tags sui files .mp3.
	Luca Piergentili, 05/08/03
	lpiergentili@yahoo.com
*/
#ifndef _CAUDIOMP3INFO_H
#define _CAUDIOMP3INFO_H 1

#include <string.h>
#include "strings.h"
#include "window.h"
#include "mmaudio.h"
#include "mciaudio.h"
#include "CAudioInfoObject.h"
#include "CMP3Info.h"
#include "CId3Lib.h"

/*
	CAudioMp3Info
*/
class CAudioMp3Info : public CAudioInfoObject
{
public:
	CAudioMp3Info(LPCSTR lpcszMp3FileName = NULL);
	virtual ~CAudioMp3Info() {};

	int		Link			(LPCSTR lpcszMp3FileName);
	void		Unlink		(void);

	QWORD	GetFileSize	(void) {return(m_qwFileSize);}
	LPCSTR	GetFileName	(void) {return(m_szFileName);}

	LPCSTR	GetTitle		(void) {return(m_Id3Lib.GetTitle());}
	LPCSTR	GetArtist		(void) {return(m_Id3Lib.GetArtist());}
	LPCSTR	GetAlbum		(void) {return(m_Id3Lib.GetAlbum());}
	LPCSTR	GetYear		(void) {return(m_Id3Lib.GetYear());}
	int		GetTrack		(void) {return(m_Id3Lib.GetTrack());}
	LPCSTR	GetComment	(void) {return(m_Id3Lib.GetComment());}

	QWORD	GetLength		(long& lMinutes,long& lSeconds);
	int		GetBitRate	(void) {return(m_nBitRate);}
	long		GetFrequency	(void) {return(m_lSampleRate);}
	LPCSTR	GetLayer		(void);
	LPCSTR	GetVersion	(void);
	LPCSTR	GetChannelMode	(void) {return(m_pChannelMode);}

private:
	QWORD	m_qwFileSize;
	char		m_szFileName[_MAX_PATH+1];
	long		m_lLength;
	int		m_nBitRate;
	long		m_lSampleRate;
	LPSTR	m_pChannelMode;
	double	m_dlMPEGVersion;
	int		m_nMPEGLayer;
	BOOL		m_bCopyrighted;
	BOOL		m_bOriginal;
	CId3Lib	m_Id3Lib;
	CMP3Info	m_MP3Info;
};

#endif // _CAUDIOMP3INFO_H
