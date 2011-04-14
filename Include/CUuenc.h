/*
	CUuenc.h
	Classe per la codifica/decodifica in formato UUENC (CRT).
	Luca Piergentili, 24/11/96
	lpiergentili@yahoo.com
*/
#ifndef _CUUENC_H
#define _CUUENC_H 1

/*
	CUuenc
*/
class CUuenc
{
public:
	CUuenc() {}
	virtual ~CUuenc() {}
	
	int Encode(const char*,const char*);
	int Decode(const char*,const char*);
};

#endif // _CUUENC_H
