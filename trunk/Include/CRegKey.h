/*
	CRegKey.h
	Classe base per l'accesso al registro (SDK/MFC).
	Luca Piergentili, 14/07/99
	lpiergentili@yahoo.com

	Riadattata dal codice originale ATL (M$) (vedi http://codeguru.earthweb.com/system/CRegKey.shtml).
*/
#ifndef _CREGKEY_H
#define _CREGKEY_H

#include <stdio.h>
#include <stdlib.h>
#include "window.h"
#include <winreg.h>

#define REGKEY_MAX_SECTION_NAME	_MAX_PATH				// dim. max. nome sezione
#define REGKEY_MAX_KEY_NAME		REGKEY_MAX_SECTION_NAME	// dim. max. nome chiave
#define REGKEY_MAX_KEY_VALUE		REGKEY_MAX_SECTION_NAME	// dim. max. chiave

/*
	CRegKey
*/
class CRegKey
{
public:
	CRegKey() {m_hKey = NULL;}
	virtual ~CRegKey() {CRegKey::Close();}

	inline void	Attach			(HKEY hKey) {m_hKey = hKey;}
	HKEY			Detach			(void);

	LONG			Create			(
								HKEY hKeyParent,
								LPCSTR lpcszKeyName,
								LPSTR = REG_NONE,
								DWORD = REG_OPTION_NON_VOLATILE,
								REGSAM = KEY_ALL_ACCESS,
								LPSECURITY_ATTRIBUTES = NULL,
								LPDWORD = NULL
								);

	LONG			Open				(HKEY hKeyParent,LPCSTR lpcszKeyName,REGSAM = KEY_ALL_ACCESS);
	LONG			Close			(void);

	LONG			QueryValue		(LPSTR lpszValue,LPCSTR lpcszValueName,DWORD* pdwCount);
	LONG			QueryValue		(DWORD& dwValue,LPCSTR lpcszValueName);

	LONG			SetValue			(LPCSTR lpcszValue,LPCSTR lpcszValueName = NULL);
	LONG			SetValue			(DWORD dwValue,LPCSTR lpcszValueName);
	LONG			SetBinaryValue		(DWORD dwValue,LPCSTR lpcszValueName);
	LONG WINAPI	SetValue			(HKEY hKeyParent,LPCSTR lpcszKeyName,LPCSTR lpcszValue,LPCSTR lpcszValueName = NULL);
	LONG			SetKeyValue		(LPCSTR lpcszKeyName,LPCSTR lpcszValue,LPCSTR lpcszValueName = NULL);

	BOOL			DeleteValue		(LPCSTR lpcszValue);
	BOOL			DeleteKey			(LPCSTR lpcszKey);
	BOOL			DeleteSubKey		(LPCSTR lpcszSubKey);
	
	operator		HKEY() const		{return(m_hKey);}

private:
	HKEY m_hKey;
};

#endif // _CREGKEY_H
