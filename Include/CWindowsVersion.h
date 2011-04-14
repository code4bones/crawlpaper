/*
	CWindowsVersion.h
	Classe per ricavare la versione di Windows.
	Luca Piergentili, 20/11/02
	lpiergentili@yahoo.com
*/
#ifndef _CWINDOWSVERSION
#define _CWINDOWSVERSION 1

#include "window.h"
#include "win32api.h"
//#include "CWindowsXPTheme.h"

/*
	CWindowsVersion
*/
class CWindowsVersion
{
public:
	CWindowsVersion();
	virtual ~CWindowsVersion();
	
	inline OSVERSIONTYPE	GetVersionNumber	(DWORD& dwMajor,DWORD& dwMinor) {dwMajor = m_dwMajorVersion; dwMinor = m_dwMinorVersion; return(m_OsVersionType);}
	inline OSVERSIONTYPE	GetVersionType		(void) const {return(m_OsVersionType);}
	inline LPCSTR			GetVersionString	(void) const {return(m_szOsType);}
	
	inline LPCSTR			GetPlatformString	(void) const {return(m_szWindowsPlatform);}
	void					GetPlatformInfo	(LPSTR pBuffer,int cbBuffer);

	inline BOOL			IsRunningOnCartoons	(void) const {return(m_bIsRunningOnCartoons);}
	inline BOOL			IsRunningOnNT		(void) const {return(m_bIsRunningOnNT);}
	inline BOOL			IsRunningOnXP		(void) const {return(m_bIsRunningOnCartoons && m_bIsRunningOnNT);}

	inline DWORD			GetCommonControlsVer(void) {return(m_dwCommonControlsDllVersion);}

private:
	static int			m_nRefCount;

	static DWORD			m_dwMajorVersion;
	static DWORD			m_dwMinorVersion;
	static OSVERSIONTYPE	m_OsVersionType;
	static char			m_szOsType[32];
	static char			m_szWindowsPlatform[128];
	static BOOL			m_bIsRunningOnCartoons;
	static BOOL			m_bIsRunningOnNT;
	static DWORD			m_dwCommonControlsDllVersion;
//	CWindowsXPTheme*		m_pXPTheme;
};

#endif // _CWINDOWSVERSION
