/*
	CBase64.h
	Classe per la codifica/decodifica in base64.
	Luca Piergentili, 14/09/96
	lpiergentili@yahoo.com

	I due metodi per la codifica/decodifica del buffer sono stati ripresi dal
	codice di "Pavuk" di Stefan Ondrejicka (http://www.idata.sk/~ondrej/pavuk/).
*/
#ifndef _CBASE64_H
#define _CBASE64_H 1

/*
	CBase64
*/
class CBase64
{
public:
	CBase64() {}
	virtual ~CBase64() {}

	char*	Encode(const char* data);
	int		Decode(const char* inbuf,char** outbuf);

	int		Encode(const char* pPlainFile,const char* pEncodedFile);
	int		Decode(const char* pEncodedFile,const char* pPlainFile);

private:
	int		ConvertToAscii(unsigned char c);
	int		ChrIndex(const char* pString,int c);
};

#endif // _CBASE64_H
