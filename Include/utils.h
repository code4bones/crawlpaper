/*
	utils.h
	Funzioni di utilita' per quanto omesso dall'API (SDK).
	Luca Piergentili, 13/09/98
	lpiergentili@yahoo.com
*/
#ifndef _UTILS_H
#define _UTILS_H 1

#include "window.h"

BOOL WritePrivateProfileInt(LPCSTR lpcszSectioneName,LPCSTR lpcszKeyName,int iValue,LPCSTR lpcszIniFile);
LPSTR GetThisModuleFileName(LPSTR lpszFileName,DWORD nSize);
char* ltos(long,char*);

#endif // _UTILS_H
