/*
	CQuotedp.h
	Luca Piergentili, 27/08/97
	lpiergentili@yahoo.com
*/
#ifndef _CQUOTEDP_H
#define _CQUOTEDP_H 1

class CQuotedp
{
public:
	CQuotedp();
	~CQuotedp();
	int Encode(const char*,const char*);
	int Decode(const char*,const char*);
};

#endif // _CQUOTEDP_H
