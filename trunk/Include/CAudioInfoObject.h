/*
	CAudioInfoObject.h
	Classe base per la definizione dell'oggetto da cui derivare le classi (CAudioWavInfo,
	CAudioCDAInfo, CAudioMP3Info, etc.) per le info sul formato audio in questione.
	Luca Piergentili, 14/08/03
	lpiergentili@yahoo.com
*/
#ifndef _CAUDIOINFOOBJECT_H
#define _CAUDIOINFOOBJECT_H 1

#include "window.h"
#include "mmaudio.h"

/*
	CAudioInfoObject
*/
class CAudioInfoObject {
public:
	CAudioInfoObject(LPCSTR lpcszFileName = NULL) {lpcszFileName;/*if(lpcszFileName) Link(lpcszFileName);*/}
	virtual ~CAudioInfoObject() {}

	virtual int	Link			(LPCSTR /*lpcszFileName*/) {return(0);}
	virtual void	Unlink		(void) {}

	virtual QWORD	GetFileSize	(void) {return(0L);}
	virtual LPCSTR	GetFileName	(void) {return("");}

	virtual LPCSTR	GetTitle		(void) {return("");}
	virtual LPCSTR	GetArtist		(void) {return("");}
	virtual LPCSTR	GetAlbum		(void) {return("");}
	virtual LPCSTR	GetGenre		(void) {return("");}
	virtual LPCSTR GetYear		(void) {return("");}
	virtual int	GetTrack		(void) {return(0);}
	virtual LPCSTR	GetComment	(void) {return("");}

	virtual QWORD	GetLength		(long& /*lMinutes*/,long& /*lSeconds*/) {return(0L);}
	virtual int	GetBitRate	(void) {return(0);}
	virtual long	GetFrequency	(void) {return(0L);}
	virtual LPCSTR	GetLayer		(void) {return("");}
	virtual LPCSTR	GetVersion	(void) {return("");}
	virtual LPCSTR	GetChannelMode	(void) {return("");}
};

#endif // _CAUDIOINFOOBJECT_H
