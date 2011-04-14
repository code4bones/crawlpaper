/*
	CMP3Info.h
	Info sul file MP3.
	Codice modificato e riadattato a partire dall'originale di Gustav Munkby.
	Luca Piergentili, 07/08/03
	lpiergentili@yahoo.com
*/
#ifndef _CMP3INFO_H
#define _CMP3INFO_H 1

#include <stdio.h>
#include <stdlib.h>
#include "window.h"
#include "CVBitRate.h"
#include "CFrameHeader.h"

/*
	CMP3Info
*/
class CMP3Info
{
public:
	CMP3Info() {}
	virtual ~CMP3Info() {}

	BOOL			Load			(LPCSTR lpcszFilename);
	inline QWORD	GetFileSize	(void) const {return(m_qwFileSize);}
	inline float	GetVersion	(void) {return(m_FrameHeader.getVersion());}
	inline int	GetLayerNumber	(void) {return(m_FrameHeader.GetLayerNumber());}
	LPCSTR		GetLayer		(void);
	int			GetBitRate	(void);
	inline long	GetFrequency	(void) {return(m_FrameHeader.getFrequency());}
	LPCSTR		GetChannelMode	(void) {return(m_FrameHeader.getMode());}
	int			GetFrameCount	(void);
	long			GetLength		(void);
	QWORD		GetLength		(long& lMinutes,long& lSeconds);
	inline BOOL	IsVBitRate	(void) const {return(m_bVBitRate);}

private:
	char			m_szLayer[32];
	CFrameHeader	m_FrameHeader;
	CVBitRate		m_VBitRate;
	BOOL			m_bVBitRate;
	QWORD		m_qwFileSize;
};

#endif // _CMP3INFO_H
