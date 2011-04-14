/*
	mfcapi.h
	Implementazione di quanto omesso dall' API (SDK/MFC).
	Luca Piergentili, 13/09/98
	lpiergentili@yahoo.com
*/
#ifndef _MFCAPI_H
#define _MFCAPI_H 1

#include "window.h"

void AppendFileType(CString& strFilter,CString& strTypeFilter,LPCSTR lpcszDesc,LPCSTR lpcszExt);

#endif // _MFCAPI_H
