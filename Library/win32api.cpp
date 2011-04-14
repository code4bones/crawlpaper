/*
	win32api.cpp
	Implementazione di quanto omesso dall' API (SDK/MFC).
	Luca Piergentili, 13/09/98
	lpiergentili@yahoo.com
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "strings.h"
#include "window.h"
#include "win32api.h"
#include <shellapi.h>
#include <shlwapi.h>
#include "CRegKey.h"
#include "CBinFile.h"

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

static int InternalMessageBox(HWND hWnd,LPCSTR lpcszText,LPCSTR lpcszTitle,UINT nStyle);

/*
	GetBiggerIconSize()
*/
int GetBiggerIconSize(LPCSTR lpcszIconFile,int nPreferredSize/* = 0*/)
{
	int nBiggerIcon = -1;
	BOOL bPreferredIcon = FALSE;
	ICONHEADER iconheader;
	ICONIMAGE* pIconImageArray;
	CBinFile f;
	if(f.OpenExistingReadOnly(lpcszIconFile))
	{
		if(f.Read(&iconheader,sizeof(ICONHEADER))!=FILE_EOF)
		{
			pIconImageArray = new ICONIMAGE[iconheader.nImageCount];
			if(pIconImageArray)
			{
				int i;
				for(i=0; i < iconheader.nImageCount; i++)
					if(f.Read((LPVOID)&pIconImageArray[i],sizeof(ICONIMAGE))!=FILE_EOF)
					{
						if(nPreferredSize!=0)
							if(pIconImageArray[i].nWidth==nPreferredSize)
								bPreferredIcon = TRUE;
						if(pIconImageArray[i].nWidth > nBiggerIcon)
							nBiggerIcon = pIconImageArray[i].nWidth;
					}
				delete [] pIconImageArray;
			}
		}
		f.Close();
	}
	if(nBiggerIcon < 0)
		nBiggerIcon = 32;
	if(bPreferredIcon)
		nBiggerIcon = nPreferredSize;
	
	return(nBiggerIcon);
}

/*
	WritePrivateProfileInt()

	Scrive un intero nel file .ini (nell'API e' presente solo la WritePrivateProfileString()).
*/
BOOL WritePrivateProfileInt(LPCSTR lpcszSectioneName,LPCSTR lpcszKeyName,int nValue,LPCSTR lpcszIniFile)
{
	char buffer[16];
	memset(buffer,'\0',sizeof(buffer));
	_snprintf(buffer,sizeof(buffer)-1,"%d",nValue);
	return(::WritePrivateProfileString(lpcszSectioneName,lpcszKeyName,buffer,lpcszIniFile));
}

/*
	GetThisModuleFileName()

	Recupera il nome del file eseguibile corrente.
	Considera come eseguibile solo quanto termina con ".exe".
	Non usa GetModuleFileName() perche' quest'ultima sotto W95/98 se il numero di versione interno
	del file e' inferiore a 4 restituisce il nome corto.
*/
LPSTR GetThisModuleFileName(LPSTR lpszFileName,UINT nSize)
{
	int i = 0;
	char* p;
	char value[_MAX_PATH+1];

	memset(lpszFileName,'\0',nSize);
	
	strcpyn(value,::GetCommandLine(),sizeof(value));
	if((p = stristr(value,".exe"))!=NULL)
		i = p - value;
	if(i > 0)
	{
		strcpyn(value,::GetCommandLine(),sizeof(value));
		memcpy(value+i,".exe",4);
	}
	else
		return(NULL);

	p = value;
	while(*p)
	{
		if(*p=='"')
			*p = ' ';
		p++;
	}

	p = value;
	while(*p==' ')
		p++;
	
	for(i = 0; i < (int)nSize+1; i++)
	{
		if(*p==' ')
			if(stristr(lpszFileName,".exe"))
				break;

		lpszFileName[i] = *p++;
	}
	
	lpszFileName[i] = '\0';

	return(lpszFileName);
}

/*
	GetWindowsVersion()
*/
OSVERSIONTYPE GetWindowsVersion(LPSTR lpszWindowsPlatform,UINT nSize,DWORD* dwMajorVersion/*=NULL*/,DWORD* dwMinorVersion/*=NULL*/)
{
	OSVERSIONINFOEX os = {0};
	os.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	BOOL bOsVersionInfo = FALSE;
	OSVERSIONTYPE osversiontype = UNKNOW_WINDOWS_VERSION;
	strcpyn(lpszWindowsPlatform,"UNKNOW_WINDOWS_VERSION",nSize);

	// try calling GetVersionEx using the OSVERSIONINFOEX structure, which is supported on Windows 2000
	// if that fails, try using the OSVERSIONINFO structure
	if((bOsVersionInfo = ::GetVersionEx((OSVERSIONINFO*)&os))==FALSE)
	{
		// if OSVERSIONINFOEX doesn't work, try OSVERSIONINFO
		memset(&os,'\0',sizeof(OSVERSIONINFO));
		os.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		bOsVersionInfo = ::GetVersionEx((OSVERSIONINFO*)&os);
	}

	if(bOsVersionInfo)
	{
		// service pack
		char szServiceRelease[_MAX_PATH+1] = {0};
		strcpyn(szServiceRelease,os.szCSDVersion,sizeof(szServiceRelease));
		
		switch(os.dwPlatformId)
		{
			// Win3.1
			case VER_PLATFORM_WIN32s:
				osversiontype = WINDOWS_31;
				strcpyn(lpszWindowsPlatform,"Microsoft® Windows 3.1 (TM)",nSize);
				break;
			
			// Win95/98
			case VER_PLATFORM_WIN32_WINDOWS:
				if(os.dwMinorVersion==0)
				{
					osversiontype = WINDOWS_95;
					_snprintf(lpszWindowsPlatform,nSize-1,"Microsoft® Windows 95 (TM) %s",szServiceRelease);
				}
				else if(os.dwMinorVersion==10)
				{
					osversiontype = WINDOWS_98;
					_snprintf(lpszWindowsPlatform,nSize-1,"Microsoft® Windows 98 (TM) %s",szServiceRelease);
				}
				if(os.dwMinorVersion==90)
				{
					osversiontype = WINDOWS_MILLENNIUM;
					_snprintf(lpszWindowsPlatform,nSize-1,"Microsoft® Windows Millenium (TM) %s",szServiceRelease);
				}
				break;
				
			// WinNT
			case VER_PLATFORM_WIN32_NT:
			{
				DWORD dwVersion = 1L;
				typedef DWORD (WINAPI* PRtlGetNtProductType) (PDWORD pVersion);
				PRtlGetNtProductType pfnRtlGetNtProductType = (PRtlGetNtProductType)::GetProcAddress(GetModuleHandle("ntdll.dll"),"RtlGetNtProductType");
				if(pfnRtlGetNtProductType)
					pfnRtlGetNtProductType(&dwVersion);

				if(os.dwMajorVersion==4)
				{
					osversiontype = WINDOWS_NT;
					_snprintf(lpszWindowsPlatform,nSize-1,"Microsoft® Windows NT 4.%ld %s (TM) %s",os.dwMinorVersion,dwVersion!=1L ? "Server" : "Workstation",szServiceRelease);
				}
				else if(os.dwMajorVersion==5)
				{
					osversiontype = os.dwMinorVersion==0 ? WINDOWS_2000 : WINDOWS_XP;
					_snprintf(lpszWindowsPlatform,nSize-1,"Microsoft® Windows %s %s Edition (TM) %s",os.dwMinorVersion==0 ? "2000" : "XP",dwVersion!=1L ? "Professional" : "Home",szServiceRelease);
				}
				else if(os.dwMajorVersion==6)
				{
					osversiontype = WINDOWS_VISTA;
					_snprintf(lpszWindowsPlatform,nSize-1,"Microsoft® Windows Vista %s Edition (TM) %s",dwVersion!=1L ? "Professional" : "Home",szServiceRelease);
				}
				else if(os.dwMajorVersion==7)
				{
					osversiontype = WINDOWS_SEVEN;
					_snprintf(lpszWindowsPlatform,nSize-1,"Microsoft® Windows Seven %s Edition (TM) %s",dwVersion!=1L ? "Professional" : "Home",szServiceRelease);
				}
				
				break;
			}
		}
	
		if(dwMajorVersion && dwMinorVersion)
		{
			*dwMajorVersion = os.dwMajorVersion;
			*dwMinorVersion = os.dwMinorVersion;
		}
	}
	
	return(osversiontype);
}

/*
	GetDllVersion()

	The following code fragment illustrates how you can use GetDllVersion to test if Comctl32.dll is
	version 4.71 or later.

	if(GetDllVersion(TEXT("comctl32.dll")) >= PACKVERSION(4,71))
		//Proceed
	else
		//MicrosoftReallySucksBalls...
*/
DWORD GetDllVersion(LPCTSTR lpszDllName)
{
	HINSTANCE hDll;
	DWORD dwVersion = 0L;

	if((hDll = ::LoadLibrary(lpszDllName))!=(HINSTANCE)NULL)
	{
		DLLGETVERSIONPROC pDllGetVersion = (DLLGETVERSIONPROC)::GetProcAddress(hDll,"DllGetVersion");

		/*
		HRESULT CALLBACK DllGetVersion(DLLVERSIONINFO *pdvi);
		
		Version 5.0. DLLs that are shipped with Windows 2000 or later systems may return a
		DLLVERSIONINFO2 structure. To maintain backward compatibility, the first member of
		a DLLVERSIONINFO2 structure is a DLLVERSIONINFO structure.

		Because some DLLs may not implement this function, you must test for it explicitly.
		Depending on the particular DLL, the lack of a DllGetVersion function may be a useful
		indicator of the version.
		*/
		if(pDllGetVersion)
		{
			DLLVERSIONINFO dvi;
			HRESULT hr;

			::ZeroMemory(&dvi,sizeof(dvi));
			dvi.cbSize = sizeof(dvi);

			hr = (*pDllGetVersion)(&dvi);
			if(SUCCEEDED(hr))
				dwVersion = PACKVERSION(dvi.dwMajorVersion,dvi.dwMinorVersion);
		}

		::FreeLibrary(hDll);
	}

	return(dwVersion);
}

/*
	GetLastErrorString()
*/
void GetLastErrorString(void)
{
	LPVOID pBuffer;
	::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				::GetLastError(),
				MAKELANGID(LANG_NEUTRAL,SUBLANG_SYS_DEFAULT/*SUBLANG_DEFAULT*/),
				(LPTSTR)&pBuffer,
				0,
				NULL);
	::MessageBox(NULL,(LPCSTR)pBuffer,"Error",MB_OK|MB_ICONWARNING|MB_TASKMODAL|MB_SETFOREGROUND|MB_TOPMOST);
	::LocalFree(pBuffer);
}

/*
	GetLastErrorString()
*/
LPVOID GetLastErrorString(DWORD dwError)
{
	LPVOID pBuffer;
	::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				dwError,
				MAKELANGID(LANG_NEUTRAL,SUBLANG_SYS_DEFAULT/*SUBLANG_DEFAULT*/),
				(LPTSTR)&pBuffer,
				0,
				NULL);
	
	char* p = (char*)pBuffer;
	int i = strlen(p)-1;
	if(p[i]=='\r' || p[i]=='\n')
		do {
			p[i] = '\0';
			i = strlen(p)-1;
		} while(i > 0 && p[i]=='\r' || p[i]=='\n');

	return(pBuffer);
}

/*
	MessageBoxResource()
*/
int MessageBoxResource(HWND hWnd,UINT nStyle,LPCSTR lpcszTitle,UINT nID)
{
	char szBuffer[2048] = {"<unable to load the string from resources>"};
	::LoadString(NULL,nID,szBuffer,sizeof(szBuffer)-1);
	
	return(InternalMessageBox(hWnd,szBuffer,lpcszTitle,nStyle));
}

/*
	MessageBoxResourceEx()
*/
int MessageBoxResourceEx(HWND hWnd,UINT nStyle,LPCSTR lpcszTitle,UINT nID,...)
{
	LPSTR pArgs;
	char szBuffer[2048] = {"<unable to load the string from resources>"};
	char szFormat[2048] = {0};

	if(::LoadString(NULL,nID,szFormat,sizeof(szFormat)-1) > 0)
	{
		pArgs = (LPSTR)&nID + sizeof(nID);
		memset(szBuffer,'\0',sizeof(szBuffer));
		_vsnprintf(szBuffer,sizeof(szBuffer)-1,szFormat,pArgs);
	}

	return(InternalMessageBox(hWnd,szBuffer,lpcszTitle,nStyle));
}

/*
	InternalMessageBox()
*/
int InternalMessageBox(HWND hWnd,LPCSTR lpcszText,LPCSTR lpcszTitle,UINT nStyle)
{
	UINT nRet = ::MessageBox(hWnd,lpcszText,lpcszTitle,nStyle|MB_TASKMODAL|MB_SETFOREGROUND|MB_TOPMOST);
	return(nRet);
}

/*
	FormatResourceString()
*/
int FormatResourceString(LPSTR buffer,UINT nSize,UINT nID)
{
	memset(buffer,'\0',nSize);
	return(::LoadString(NULL,nID,buffer,nSize-1));
}

/*
	FormatResourceStringEx()
*/
int FormatResourceStringEx(LPSTR buffer,UINT nSize,UINT nID,...)
{
	int nRet = -1;
	LPSTR pArgs;
	char szFormat[2048] = {0};

	memset(buffer,'\0',nSize);

	if(::LoadString(NULL,nID,szFormat,sizeof(szFormat)-1) > 0)
	{
		pArgs = (LPSTR)&nID + sizeof(nID);
		nRet = _vsnprintf(buffer,nSize-1,szFormat,pArgs);
	}

	return(nRet);
}

/*
	ExtractResource()
*/
BOOL ExtractResource(UINT nID,LPCSTR lpcszResName,LPCSTR lpcszOutputFile)
{
	BOOL bExtracted = FALSE;

	HRSRC hRes = ::FindResource(NULL,MAKEINTRESOURCE(nID),lpcszResName);
	if(hRes)
	{
		HGLOBAL hGlobal = ::LoadResource(NULL,hRes);
		if(hGlobal)
		{
			LPVOID lpVoid = ::LockResource(hGlobal);
			if(lpVoid)
			{
				HANDLE handle;
				if((handle = ::CreateFile(lpcszOutputFile,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL))!=INVALID_HANDLE_VALUE)
				{
					DWORD dwToWrite = ::SizeofResource(NULL,hRes);
					DWORD dwWritten = 0L;
					::WriteFile(handle,lpVoid,(UINT)dwToWrite,&dwWritten,NULL);
					::CloseHandle(handle);
					bExtracted = (dwToWrite==dwWritten);
				}
			}
		}
	}

	return(bExtracted);
}

/*
	ExtractResourceIntoBuffer()
*/
BOOL ExtractResourceIntoBuffer(UINT nID,LPCSTR lpcszResName,LPSTR lpBuffer,UINT nBufferSize)
{
	BOOL bExtracted = FALSE;

	HRSRC hRes = ::FindResource(NULL,MAKEINTRESOURCE(nID),lpcszResName);
	if(hRes)
	{
		HGLOBAL hGlobal = ::LoadResource(NULL,hRes);
		if(hGlobal)
		{
			LPVOID lpVoid = ::LockResource(hGlobal);
			if(lpVoid)
			{
				memset(lpBuffer,'\0',nBufferSize);
				UINT nResSize = ::SizeofResource(NULL,hRes);
				if(nResSize <= nBufferSize)
				{
					memcpy(lpBuffer,lpVoid,nResSize);
					bExtracted = TRUE;
				}
			}
		}
	}

	return(bExtracted);
}

/*
	CreateShortcut()
*/
BOOL CreateShortcut(LPCSTR Target,LPCSTR Arguments,LPCSTR LinkFileName,LPCSTR LinkLocation,LPCSTR WorkingDir,UINT nIconIndex)
{
	BOOL bCreated = FALSE;
	HRESULT hres;
	ITEMIDLIST *id; 
	char szLocation[_MAX_PATH+1];
	char szLink[_MAX_PATH+1];
	
	// se non viene specificato nessun percorso, crea il link sul desktop
	if(!LinkLocation)
	{
		::SHGetSpecialFolderLocation(NULL,CSIDL_DESKTOPDIRECTORY,&id); 
		::SHGetPathFromIDList(id,&szLocation[0]); 
	}
	else
		strcpyn(szLocation,LinkLocation,sizeof(szLocation));
	
	// compone il pathname completo per il link
	_snprintf(szLink,sizeof(szLink)-1,"%s\\%s.lnk",szLocation,LinkFileName);

	hres = ::CoInitialize(NULL);
	if(SUCCEEDED(hres))
	{
		IShellLink* psl;
		hres = ::CoCreateInstance(	CLSID_ShellLink,
								NULL,
								CLSCTX_INPROC_SERVER,
								IID_IShellLink,
								(LPVOID*)&psl
								);

		if(SUCCEEDED(hres))
		{
			IPersistFile* ppf;
			psl->SetPath(Target);
			if(Arguments)
				psl->SetArguments(Arguments);
			if(WorkingDir)
				psl->SetWorkingDirectory(WorkingDir);

			hres = psl->QueryInterface(IID_IPersistFile,(LPVOID*)&ppf);
			if(SUCCEEDED(hres))
			{
				WORD wsz[_MAX_PATH+1];
				MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,szLink,-1,(LPWSTR)wsz,sizeof(wsz));
				hres = ppf->Save((LPCOLESTR)wsz,TRUE);
				ppf->Release();
				bCreated = TRUE;
			}

			if(nIconIndex!=(UINT)-1)
				psl->SetIconLocation(Target,nIconIndex);
			
			psl->Release();
		}
	
		::CoUninitialize();
	}

	return(bCreated);
}

/*
	CreateRegistryKey()

	Crea la coppia nome/valore dentro la chiave specificata, es.:
	key   = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"
	name  = "FRC"
	value = "C:\\MWCR\\FRC.exe"
*/
BOOL CreateRegistryKey(LPCSTR lpcszKey,LPCSTR lpcszName,LPCSTR lpcszValue)
{
	BOOL bCreated = FALSE;

	CRegKey Registry;
	Registry.Attach(HKEY_LOCAL_MACHINE);

	if(Registry.Open(HKEY_LOCAL_MACHINE,lpcszKey)==ERROR_SUCCESS)
	{
		char szKey[_MAX_PATH+1];
		DWORD dwKeySize = sizeof(szKey);

		if(Registry.QueryValue(szKey,lpcszName,&dwKeySize)!=ERROR_SUCCESS)
			bCreated = Registry.SetValue(lpcszValue,lpcszName)==ERROR_SUCCESS;
		
		Registry.Close();
	}
	
	Registry.Detach();

	return(bCreated);
}

/*
	GetRegistryKey()

	Cerca la coppia nome/valore dentro la chiave specificata, es.:
	key   = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"
	name  = "FRC"
	value = "C:\\MWCR\\FRC.exe"
*/
BOOL GetRegistryKey(LPCSTR lpcszKey,LPCSTR lpcszName,LPSTR lpszValue,int nValueSize)
{
	BOOL bGet = FALSE;

	memset(lpszValue,'\0',nValueSize);

	CRegKey Registry;
	Registry.Attach(HKEY_LOCAL_MACHINE);

	if(Registry.Open(HKEY_LOCAL_MACHINE,lpcszKey)==ERROR_SUCCESS)
	{
		char szKey[_MAX_PATH+1];
		DWORD dwKeySize = sizeof(szKey);

		if(Registry.QueryValue(szKey,lpcszName,&dwKeySize)==ERROR_SUCCESS)
		{
			_snprintf(lpszValue,sizeof(nValueSize)-1,"%s",szKey);
			bGet = TRUE;
		}
		
		Registry.Close();
	}
	
	Registry.Detach();

	return(bGet);
}

/*
	DeleteRegistryKey()

	Elimina la coppia nome/valore dentro la chiave specificata, es.:
	key   = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"
	name  = "FRC"
	value = "C:\\MWCR\\FRC.exe"
*/
BOOL DeleteRegistryKey(LPCSTR lpcszKey,LPCSTR lpcszName)
{
	BOOL bDeleted = FALSE;

	CRegKey Registry;
	Registry.Attach(HKEY_LOCAL_MACHINE);

	if(Registry.Open(HKEY_LOCAL_MACHINE,lpcszKey)==ERROR_SUCCESS)
	{
		char szKey[_MAX_PATH+1];
		DWORD dwKeySize = sizeof(szKey);

		if(Registry.QueryValue(szKey,lpcszName,&dwKeySize)==ERROR_SUCCESS)
			bDeleted = Registry.DeleteValue(lpcszName)==ERROR_SUCCESS;

		Registry.Close();
	}
	
	Registry.Detach();

	return(bDeleted);
}

/*
	Delay()
*/
void Delay(int delay)
{
	DWORD start = ::GetTickCount();
	DWORD elapsed = 0L;

	do
	{
		::PeekAndPump();
		elapsed = ::GetTickCount() - start;
	}
	while(elapsed < (DWORD)(delay * 1000));
}

/*
	PeekAndPump()
*/
BOOL PeekAndPump(void)
{
	MSG msg;

#if defined(_AFX) || defined(_AFXDLL)
	while(::PeekMessage(&msg,NULL,0,0,PM_NOREMOVE))
	{
		if(!AfxGetApp()->PumpMessage())
		{
			::PostQuitMessage(0);
			return(FALSE);
		}
	}
	LONG lIdle = 0;
	while(AfxGetApp()->OnIdle(lIdle++))
		;
#else
	if(::PeekMessage(&msg,NULL,0,0,PM_REMOVE))
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}
#endif

	return(TRUE);
}

/*
	CopyFileTo()
*/
BOOL CopyFileTo(HWND hWnd,LPCSTR lpcszSourceFile,LPCSTR lpcszDestFile,BOOL bMoveInsteadCopy/* = FALSE*/,BOOL bShowDialog/* = TRUE*/)
{
	SHFILEOPSTRUCT sh;
	memset(&sh,'\0',sizeof(sh));
	sh.hwnd = hWnd;
	sh.wFunc = bMoveInsteadCopy ? FO_MOVE : FO_COPY;
	sh.pFrom = lpcszSourceFile;
	sh.pTo = lpcszDestFile;
	sh.fFlags = bShowDialog ? (FOF_NORECURSION|FOF_SIMPLEPROGRESS) : (FOF_NORECURSION|FOF_NOCONFIRMATION|FOF_NOCONFIRMMKDIR|FOF_NOERRORUI|FOF_RENAMEONCOLLISION|FOF_SILENT);

	int nRet = ::SHFileOperation(&sh);

	BOOL bFileExists = FALSE;
	HANDLE hHandle;
	if((hHandle = ::CreateFile(lpcszDestFile,GENERIC_READ,0/*FILE_SHARE_READ|FILE_SHARE_WRITE*/,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL))!=INVALID_HANDLE_VALUE)
	{
		::CloseHandle(hHandle);
		bFileExists = TRUE;
	}

	return(sh.fAnyOperationsAborted ? FALSE : (bFileExists ? FALSE : (nRet==0)));
}

/*
	DeleteFileToRecycleBin()
*/
BOOL DeleteFileToRecycleBin(HWND hWnd,LPCSTR lpcszFileName,BOOL bShowDialog/* = TRUE*/,BOOL bAllowUndo/* = TRUE*/)
{
	SHFILEOPSTRUCT sh;
	memset(&sh,'\0',sizeof(sh));
	sh.hwnd = hWnd;
	sh.wFunc = FO_DELETE;
	sh.pFrom = lpcszFileName;

	FILEOP_FLAGS fProgress = bAllowUndo ? ((FILEOP_FLAGS)FOF_ALLOWUNDO|FOF_SIMPLEPROGRESS) : ((FILEOP_FLAGS)FOF_SIMPLEPROGRESS);
	FILEOP_FLAGS fConfirm = bAllowUndo ? ((FILEOP_FLAGS)FOF_ALLOWUNDO|FOF_NOCONFIRMATION|FOF_SILENT) : ((FILEOP_FLAGS)FOF_NOCONFIRMATION|FOF_SILENT);
	sh.fFlags = bShowDialog ? fProgress : fConfirm;
	//sh.fFlags = bShowDialog ? (FOF_ALLOWUNDO|FOF_SIMPLEPROGRESS) : (FOF_ALLOWUNDO|FOF_NOCONFIRMATION|FOF_SILENT);

	int nRet = ::SHFileOperation(&sh);

	BOOL bFileExists = FALSE;
	HANDLE hHandle;
	if((hHandle = ::CreateFile(lpcszFileName,GENERIC_READ,0/*FILE_SHARE_READ|FILE_SHARE_WRITE*/,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL))!=INVALID_HANDLE_VALUE)
	{
		::CloseHandle(hHandle);
		bFileExists = TRUE;
	}

	return(sh.fAnyOperationsAborted ? FALSE : (bFileExists ? FALSE : (nRet==0)));
}

/*
	FileExist()
*/
BOOL FileExist(LPCSTR lpcszFileName)
{
	BOOL bFileExists = FALSE;
	HANDLE hHandle = INVALID_HANDLE_VALUE;
	if((hHandle = ::CreateFile(lpcszFileName,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL))!=INVALID_HANDLE_VALUE)
	{
		::CloseHandle(hHandle);
		bFileExists = TRUE;
	}
	return(bFileExists);
}

typedef BOOL (WINAPI* LPFNGETFILESIZEEX)(HANDLE hFile,PLARGE_INTEGER lpLargeInt);

/*
	GetFileSizeExt()
*/
BOOL GetFileSizeExt(HANDLE hFile,PLARGE_INTEGER lpLargeInt)
{
	BOOL bRet = FALSE;
	HMODULE hModule = ::LoadLibrary("kernel32.DLL");

	if(NULL!=hModule)
	{
		LPFNGETFILESIZEEX lpFnGetFileSizeEx = (LPFNGETFILESIZEEX)::GetProcAddress(hModule,"GetFileSizeEx");
		
		if(NULL!=lpFnGetFileSizeEx)
			bRet = lpFnGetFileSizeEx(hFile,lpLargeInt);
		
		::FreeLibrary(hModule);
	}

	return(bRet);
}
QWORD GetFileSizeExt(HANDLE hFile)
{
	QWORD qwFileSize = 0;
	HMODULE hModule = ::LoadLibrary("kernel32.DLL");

	if(NULL!=hModule)
	{
		LPFNGETFILESIZEEX lpFnGetFileSizeEx = (LPFNGETFILESIZEEX)::GetProcAddress(hModule,"GetFileSizeEx");
		
		if(NULL!=lpFnGetFileSizeEx)
		{
			LARGE_INTEGER li = {0};
			if(lpFnGetFileSizeEx(hFile,&li))
				qwFileSize = li.QuadPart;
		}
		
		::FreeLibrary(hModule);
	}

	return(qwFileSize);
}
QWORD GetFileSizeExt(LPCSTR lpcszFileName)
{
	QWORD qwFileSize = 0L;
	WIN32_FIND_DATA find_data = {0};
	HANDLE hHandle = INVALID_HANDLE_VALUE;

	if((hHandle=::FindFirstFile(lpcszFileName,&find_data))!=INVALID_HANDLE_VALUE)
	{
		::FindClose(hHandle);
		
		if((find_data.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)==0) // Is it a file?
		{
			union {
				struct {DWORD low,high;} lh;
				QWORD size; // MS large int extension
			} file;

			file.lh.low  = find_data.nFileSizeLow;
			file.lh.high = find_data.nFileSizeHigh;
			qwFileSize = file.size;
		}
	}

	return(qwFileSize);
}

/*
	StripPathFromFile()

	Restituisce il puntatore al nome file eliminando quanto presente prima del nome.
	Il puntatore fa riferimento all'offset a cui inizia il nome file nel buffer.
	es. c:\...\file.ext -> file.ext
*/
LPCSTR StripPathFromFile(LPCSTR lpcszFileName)
{
	// elimina fino all'ultimo '\'
	char* pFile = (char*)strrchr(lpcszFileName,'\\');
	pFile = (pFile && *(pFile+1)) ? pFile+1 : (LPSTR)lpcszFileName;
	// elimina fino all'ultimo '/' se presente
	char* p = (char*)strrchr(lpcszFileName,'/');
	if(p && *(p+1))
		pFile = p+1;
	return(pFile);
}

/*
	StripFileFromPath()
*/
LPSTR StripFileFromPath(LPCSTR lpcszFileName,LPSTR pPath,UINT nPathSize,BOOL bRemoveBackslash)
{
	strcpyn(pPath,lpcszFileName,nPathSize);
	char* p = (char*)strrchr(pPath,'\\');
	if(p)
		*((bRemoveBackslash ? p : p+1)) = '\0';
	return(pPath);
}

/*
	EnsureBackslash()
*/
LPSTR EnsureBackslash(LPSTR lpszFileName,UINT nFileSize)
{
	int i = strlen(lpszFileName);
	if(lpszFileName[i-1]!='\\')
		if(i < (int)(nFileSize-1))
			strcat(lpszFileName,"\\");
		else
			lpszFileName[i-1] = '\\';

	return(lpszFileName);
}

/*
	RemoveBackslash()
*/
LPSTR RemoveBackslash(LPSTR lpszFileName)
{
	int i = strlen(lpszFileName);
	do {
		if(lpszFileName[i-1]=='\\')
		{
			lpszFileName[i-1] = '\0';
			i = strlen(lpszFileName);
		}
	} while(i-1 >= 0 && lpszFileName[i-1]=='\\');

	return(lpszFileName);
}

/*
	GetTaskBarPos()
*/
BOOL GetTaskBarPos(TASKBARPOS* tbi)
{
	memset(tbi,'\0',sizeof(TASKBARPOS));
	tbi->nTaskbarPlacement = -1;
	tbi->nScreenWidth = ::GetSystemMetrics(SM_CXSCREEN);
	tbi->nScreenHeight = ::GetSystemMetrics(SM_CYSCREEN);
	
	tbi->hWnd = ::FindWindow("Shell_TrayWnd",NULL);
	if(tbi->hWnd)
	{
		::GetWindowRect(tbi->hWnd,&(tbi->rc));

		tbi->nTaskbarWidth = tbi->rc.right - tbi->rc.left;
		tbi->nTaskbarHeight = tbi->rc.bottom - tbi->rc.top;

		// Daniel Lohmann: Calculate taskbar position from its window rect. However, on XP it may be that the
		// taskbar is slightly larger or smaller than the screen size. Therefore we allow some tolerance here.
		if(NEARLYEQUAL(tbi->rc.left,0,TASKBAR_X_TOLERANCE) && NEARLYEQUAL(tbi->rc.right,tbi->nScreenWidth,TASKBAR_X_TOLERANCE))
			tbi->nTaskbarPlacement = NEARLYEQUAL(tbi->rc.top,0,TASKBAR_Y_TOLERANCE) ? ABE_TOP : ABE_BOTTOM;
		else 
			tbi->nTaskbarPlacement = NEARLYEQUAL(tbi->rc.left,0,TASKBAR_X_TOLERANCE ) ? ABE_LEFT : ABE_RIGHT;

		return(TRUE);
	}

	return(FALSE);
}

/*
	SetForegroundWindowEx()
*/
void SetForegroundWindowEx(HWND hWnd,BOOL bInvalidate/* = TRUE */)
{
	// occhio che AttachThreadInput() scricca il debugger...
#ifndef _DEBUG
	::AttachThreadInput(::GetWindowThreadProcessId(::GetForegroundWindow(),NULL),GetCurrentThreadId(),TRUE);
#endif

	::ShowWindow(hWnd,SW_RESTORE);
	::SetForegroundWindow(hWnd);
	::SetFocus(hWnd);
	if(bInvalidate)
		::InvalidateRect(hWnd,NULL,TRUE);

#ifndef _DEBUG
	::AttachThreadInput(::GetWindowThreadProcessId(::GetForegroundWindow(),NULL),GetCurrentThreadId(),FALSE);
#endif
}
