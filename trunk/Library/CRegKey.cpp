/*
	CRegKey.cpp
	Classe base per l'accesso al registro (SDK/MFC).
	Luca Piergentili, 14/07/99
	lpiergentili@yahoo.com

	Riadattata dal codice originale ATL (M$) (vedi http://codeguru.earthweb.com/system/CRegKey.shtml).
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include <stdlib.h>
#include "window.h"
#include <winreg.h>
#include "CRegKey.h"

#include "traceexpr.h"
//#define _TRACE_FLAG	_TRFLAG_TRACEOUTPUT
//#define _TRACE_FLAG	_TRFLAG_NOTRACE
#define _TRACE_FLAG		_TRFLAG_NOTRACE
#define _TRACE_FLAG_INFO	_TRFLAG_NOTRACE
#define _TRACE_FLAG_WARN	_TRFLAG_NOTRACE
#define _TRACE_FLAG_ERR	_TRFLAG_NOTRACE

#if (defined(_DEBUG) && defined(_WINDOWS)) && (defined(_AFX) || defined(_AFXDLL))
#ifdef PRAGMA_MESSAGE_VERBOSE
  #pragma message("\t\t\t"__FILE__"("STR(__LINE__)"): using DEBUG_NEW macro")
#endif
  #define new DEBUG_NEW
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif

/*
	Detach()
*/
HKEY CRegKey::Detach(void)
{
	HKEY hKey = m_hKey;
	m_hKey = NULL;
	return(hKey);
}

/*
	Create()
*/
LONG CRegKey::Create(HKEY hKeyParent,LPCSTR lpcszKeyName,LPSTR lpszClass/*=REG_NONE*/,DWORD dwOptions/*=REG_OPTION_NON_VOLATILE*/,REGSAM samDesired/*=KEY_ALL_ACCESS*/,LPSECURITY_ATTRIBUTES lpSecAttr/*=NULL*/,LPDWORD lpdwDisposition/*=NULL*/)
{
	DWORD dw;
	HKEY hKey = NULL;
	LONG lRes = ::RegCreateKeyEx(	hKeyParent,
							(LPCTSTR)lpcszKeyName,
							0L,
							(LPTSTR)lpszClass,
							dwOptions,
							samDesired,
							lpSecAttr,
							&hKey,
							&dw
							);

	if(lpdwDisposition!=NULL)
		*lpdwDisposition = dw;

	if(lRes==ERROR_SUCCESS)
	{
		lRes = CRegKey::Close();
		m_hKey = hKey;
	}

	return(lRes);
}

/*
	Open()
*/
LONG CRegKey::Open(HKEY hKeyParent,LPCSTR lpcszKeyName,REGSAM samDesired/*=KEY_ALL_ACCESS*/)
{
	HKEY hKey = NULL;
	LONG lRes = ::RegOpenKeyEx(hKeyParent,(LPCTSTR)lpcszKeyName,0,samDesired,&hKey);

	if(lRes==ERROR_SUCCESS)
	{
		lRes = CRegKey::Close();
		m_hKey = hKey;
	}

	return(lRes);
}

/*
	Close()
*/
LONG CRegKey::Close(void)
{
	LONG lRes = ERROR_SUCCESS;
	
	if(m_hKey!=NULL)
	{
		lRes = ::RegCloseKey(m_hKey);
		m_hKey = NULL;
	}

	return(lRes);
}

/*
	QueryValue()
*/
LONG CRegKey::QueryValue(LPSTR lpszValue,LPCSTR lpcszValueName,DWORD* pdwCount)
{
	DWORD dwType = 0L;
	return(::RegQueryValueEx(m_hKey,(LPTSTR)lpcszValueName,NULL,&dwType,(LPBYTE)lpszValue,pdwCount));
}

/*
	QueryValue()
*/
LONG CRegKey::QueryValue(DWORD& dwValue,LPCSTR lpcszValueName)
{
	DWORD dwType = 0L;
	DWORD dwCount = sizeof(DWORD);
	return(::RegQueryValueEx(m_hKey,(LPTSTR)lpcszValueName,NULL,&dwType,(LPBYTE)&dwValue,&dwCount));
}

/*
	SetValue()
*/
HRESULT CRegKey::SetValue(LPCSTR lpcszValue,LPCSTR lpcszValueName/*=NULL*/)
{
//	return(::RegSetValueEx(m_hKey,(LPCTSTR)lpcszValueName,NULL,REG_SZ,(CONST BYTE *)lpcszValue,(lstrlen(lpcszValue)+1)*sizeof(TCHAR)));
	return(::RegSetValueEx(m_hKey,(LPCTSTR)lpcszValueName,NULL,REG_SZ,(CONST BYTE *)lpcszValue,(DWORD)(lstrlen(lpcszValue)+1)));
}

/*
	SetValue()
*/
LONG CRegKey::SetValue(DWORD dwValue,LPCSTR lpcszValueName)
{
	return(::RegSetValueEx(m_hKey,(LPCTSTR)lpcszValueName,NULL,REG_DWORD,(CONST BYTE *)&dwValue,sizeof(DWORD)));
}

/*
	SetBinaryValue()
*/
LONG CRegKey::SetBinaryValue(DWORD dwValue,LPCSTR lpcszValueName)
{
	return(::RegSetValueEx(m_hKey,(LPCTSTR)lpcszValueName,NULL,REG_BINARY,(CONST BYTE *)&dwValue,sizeof(DWORD)));
}

/*
	SetValue()
*/
LONG WINAPI CRegKey::SetValue(HKEY hKeyParent,LPCSTR lpcszKeyName,LPCSTR lpcszValue,LPCSTR lpcszValueName/* = NULL */)
{
	CRegKey key;
	LONG lRes = key.Create(hKeyParent,lpcszKeyName);

	if(lRes==ERROR_SUCCESS)
		lRes = key.SetValue(lpcszValue,lpcszValueName);

	return(lRes);
}

/*
	SetKeyValue()
*/
LONG CRegKey::SetKeyValue(LPCSTR lpcszKeyName,LPCSTR lpcszValue,LPCSTR lpcszValueName/*=NULL*/)
{
	CRegKey key;
	LONG lRes = key.Create(m_hKey,lpcszKeyName);

	if(lRes==ERROR_SUCCESS)
		lRes = key.SetValue(lpcszValue,lpcszValueName);
	
	return(lRes);
}

/*
	DeleteValue()
*/
BOOL CRegKey::DeleteValue(LPCSTR lpcszValue)
{
	return(::RegDeleteValue(m_hKey,(LPCTSTR)lpcszValue)==ERROR_SUCCESS);
}

/*
	DeleteKey()
*/
BOOL CRegKey::DeleteKey(LPCSTR lpcszKey)
{
	CRegKey key;
	
	if(key.Open(m_hKey,lpcszKey)!=ERROR_SUCCESS)
		return(FALSE);

	FILETIME time;
	char szBuffer[_MAX_PATH+1];
	DWORD dwSize = sizeof(szBuffer);
	
	// cerca le eventuali sotto-chiavi presenti eliminandole singolarmente perche' su NT
	// ::RegDeleteKey() non elimina la chiave se quest'ultima possiede sotto-chiavi
	while(::RegEnumKeyEx(key.m_hKey,0L,(LPTSTR)szBuffer,&dwSize,NULL,NULL,NULL,&time)==ERROR_SUCCESS)
	{
		if(!key.DeleteKey(szBuffer))
			return(FALSE);
		
		dwSize = sizeof(szBuffer);
	}

	key.Close();

	return(DeleteSubKey(lpcszKey));
}

/*
	DeleteSubKey()
*/
BOOL CRegKey::DeleteSubKey(LPCSTR lpcszSubKey)
{
	return(::RegDeleteKey(m_hKey,(LPCTSTR)lpcszSubKey)==ERROR_SUCCESS);
}
