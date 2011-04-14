/*
	CId3Lib.h
	Classe d'interfaccia con la libreria id3lib (http://www.id3lib.org/) per info/tags sui files .mp3
	Luca Piergentili, 21/06/03
	lpiergentili@yahoo.com
*/
#ifndef _CID3LIB_H
#define _CID3LIB_H 1

#include <stdlib.h>
#include "macro.h"
#include "window.h"
#include "CId3v1Tag.h"

/*
	CId3Lib
*/
class CId3Lib : public CId3v1Tag
{
public:
	CId3Lib(LPCSTR lpcszFileName = NULL);
	virtual ~CId3Lib() {}

	BOOL		Link		(LPCSTR lpcszFileName);
	void		Unlink	(void) {return(CId3v1Tag::Unlink());}
	BOOL		Update	(void) {return(CId3v1Tag::Update());}
	LPCSTR	GetYear	(void) {_snprintf(m_szYear,sizeof(m_szYear)-1,"%ld",CId3v1Tag::GetYear()); return(m_szYear);}

private:
	char		m_szYear[ID3V1_YEAR_SIZE+1];
};

#endif // _CID3LIB_H
