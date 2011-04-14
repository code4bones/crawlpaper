/*
	CWindowsVersion.cpp
	Classe per ricavare la versione di Windows.
	Luca Piergentili, 20/11/02
	lpiergentili@yahoo.com
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include "strings.h"
#include "window.h"
#include "win32api.h"
#include "CRegKey.h"
//#include "CWindowsXPTheme.h"
#include "CWindowsVersion.h"

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

// statiche per la classe (comuni a tutte le istanze della classe)
// il contatore per le referenze viene usato solo per sapere se inizializzare i dati
int			CWindowsVersion::m_nRefCount = 0;
DWORD		CWindowsVersion::m_dwMajorVersion = 0L;
DWORD		CWindowsVersion::m_dwMinorVersion = 0L;
OSVERSIONTYPE	CWindowsVersion::m_OsVersionType = UNKNOW_WINDOWS_VERSION;
char			CWindowsVersion::m_szOsType[] = {0};
char			CWindowsVersion::m_szWindowsPlatform[] = {0};
BOOL			CWindowsVersion::m_bIsRunningOnCartoons = FALSE;
BOOL			CWindowsVersion::m_bIsRunningOnNT = FALSE;
DWORD		CWindowsVersion::m_dwCommonControlsDllVersion = 0L;

/*
	CWindowsVersion()
*/
CWindowsVersion::CWindowsVersion()
{
	// si suppone che il sistema operativo non cambi ogni 5 minuti...
	// il contatore per le referenze viene usato solo per sapere quando inizializzare i dati
	// (una sola inizializzazione per tutte le istanze della classe)
	if(m_nRefCount++==0)
	{
		// versione OS
		switch((m_OsVersionType = ::GetWindowsVersion(m_szWindowsPlatform,sizeof(m_szWindowsPlatform),&m_dwMajorVersion,&m_dwMinorVersion)))
		{
			case WINDOWS_31:
				strcpyn(m_szOsType,"3.1",sizeof(m_szOsType));
				break;
			case WINDOWS_95:
				strcpyn(m_szOsType,"95",sizeof(m_szOsType));
				m_bIsRunningOnCartoons = TRUE;
				break;
			case WINDOWS_98:
				strcpyn(m_szOsType,"98",sizeof(m_szOsType));
				m_bIsRunningOnCartoons = TRUE;
				break;
			case WINDOWS_MILLENNIUM:
				strcpyn(m_szOsType,"ME",sizeof(m_szOsType));
				m_bIsRunningOnCartoons = TRUE;
				break;
			case WINDOWS_NT:
				_snprintf(m_szOsType,sizeof(m_szOsType)-1,"NT %ld.%ld",m_dwMajorVersion,m_dwMinorVersion);
				m_bIsRunningOnNT = TRUE;
				break;
			case WINDOWS_2000:
				_snprintf(m_szOsType,sizeof(m_szOsType)-1,"NT %ld.%ld [2000]",m_dwMajorVersion,m_dwMinorVersion);
				m_bIsRunningOnNT = TRUE;
				break;
			case WINDOWS_XP:
				_snprintf(m_szOsType,sizeof(m_szOsType)-1,"NT %ld.%ld [XP]",m_dwMajorVersion,m_dwMinorVersion);
				m_bIsRunningOnCartoons = TRUE;
				m_bIsRunningOnNT = TRUE;
				break;
			case WINDOWS_VISTA:
				_snprintf(m_szOsType,sizeof(m_szOsType)-1,"NT %ld.%ld [Vista]",m_dwMajorVersion,m_dwMinorVersion);
				m_bIsRunningOnCartoons = TRUE;
				m_bIsRunningOnNT = TRUE;
				break;
			case WINDOWS_SEVEN:
				_snprintf(m_szOsType,sizeof(m_szOsType)-1,"NT %ld.%ld [Seven]",m_dwMajorVersion,m_dwMinorVersion);
				m_bIsRunningOnCartoons = TRUE;
				m_bIsRunningOnNT = TRUE;
				break;
			default:
				strcpy(m_szOsType,"UNKNOW_WINDOWS_VERSION");
				break;
		}

		// versione dll per i controlli
		m_dwCommonControlsDllVersion = ::GetDllVersion("comctl32.dll");
	}

	// per far caricare la DLL alla classe relativa solo quando serve
// 	m_pXPTheme = NULL;
}

/*
	~CWindowsVersion()
*/
CWindowsVersion::~CWindowsVersion()
{
// 	if(m_pXPTheme)
// 		delete m_pXPTheme,m_pXPTheme = NULL;
}

/*
	GetPlatformInfo()
*/
void CWindowsVersion::GetPlatformInfo(LPSTR pBuffer,int cbBuffer)
{
	CRegKey regkey;
	char key[REGKEY_MAX_KEY_NAME+1];
	char value[REGKEY_MAX_KEY_VALUE+1];
	int n = 0;

	n += _snprintf(pBuffer+n,cbBuffer-1-n,"%s - %s\r\n",m_szWindowsPlatform,m_szOsType);
	
	// NT
	if(m_OsVersionType==WINDOWS_NT || m_OsVersionType==WINDOWS_2000 || m_OsVersionType==WINDOWS_XP || m_OsVersionType==WINDOWS_VISTA || m_OsVersionType==WINDOWS_SEVEN)
	{
		regkey.Attach(HKEY_LOCAL_MACHINE);
		
		strcpyn(key,"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",sizeof(key));
		if(regkey.Open(HKEY_LOCAL_MACHINE,key)==ERROR_SUCCESS)
		{
			DWORD dwSize;

			dwSize = sizeof(value);
			memset(value,'\0',sizeof(value));
			if(regkey.QueryValue(value,"CurrentBuildNumber",&dwSize)==ERROR_SUCCESS)
				n += _snprintf(pBuffer+n,cbBuffer-1-n,"build: %s\r\n",value);

			dwSize = sizeof(value);
			memset(value,'\0',sizeof(value));
			if(regkey.QueryValue(value,"CurrentType",&dwSize)==ERROR_SUCCESS)
				n += _snprintf(pBuffer+n,cbBuffer-1-n,"type: %s\r\n",value);

			dwSize = sizeof(value);
			memset(value,'\0',sizeof(value));
			if(regkey.QueryValue(value,"ProductId",&dwSize)==ERROR_SUCCESS)
				n += _snprintf(pBuffer+n,cbBuffer-1-n,"product id: %s\r\n",value);

			dwSize = sizeof(value);
			memset(value,'\0',sizeof(value));
			if(regkey.QueryValue(value,"RegisteredOrganization",&dwSize)==ERROR_SUCCESS)
				if(value[0]!='\0')
					n += _snprintf(pBuffer+n,cbBuffer-1-n,"registered to: %s\r\n",value);

			regkey.Close();
		}
		
		regkey.Detach();

		regkey.Attach(HKEY_LOCAL_MACHINE);
		
		strcpyn(key,"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",sizeof(key));
		if(regkey.Open(HKEY_LOCAL_MACHINE,key)==ERROR_SUCCESS)
		{
			DWORD dwSize;

			dwSize = sizeof(value);
			memset(value,'\0',sizeof(value));
			if(regkey.QueryValue(value,"VendorIdentifier",&dwSize)==ERROR_SUCCESS)
				n += _snprintf(pBuffer+n,cbBuffer-1-n,"\r\nMain CPU:\r\n%s\r\n",value);

			dwSize = sizeof(value);
			memset(value,'\0',sizeof(value));
			if(regkey.QueryValue(value,"Identifier",&dwSize)==ERROR_SUCCESS)
				n += _snprintf(pBuffer+n,cbBuffer-1-n,"%s\r\n",value);

			DWORD dwValue = 0L;
			if(regkey.QueryValue(dwValue,"~MHz")==ERROR_SUCCESS)
				n += _snprintf(pBuffer+n,cbBuffer-1-n,"%ld Mhz\r\n",dwValue);

			regkey.Close();
		}
		
		regkey.Detach();
	}
	else // Cartoons
	{
		regkey.Attach(HKEY_LOCAL_MACHINE);
		
		strcpyn(key,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion",sizeof(key));
		if(regkey.Open(HKEY_LOCAL_MACHINE,key)==ERROR_SUCCESS)
		{
			DWORD dwSize;

			dwSize = sizeof(value);
			memset(value,'\0',sizeof(value));
			if(regkey.QueryValue(value,"VersionNumber",&dwSize)==ERROR_SUCCESS)
				n += _snprintf(pBuffer+n,cbBuffer-1-n,"version number: %s\r\n",value);

			dwSize = sizeof(value);
			memset(value,'\0',sizeof(value));
			if(regkey.QueryValue(value,"ProductId",&dwSize)==ERROR_SUCCESS)
				n += _snprintf(pBuffer+n,cbBuffer-1-n,"product id: %s\r\n",value);

			dwSize = sizeof(value);
			memset(value,'\0',sizeof(value));
			if(regkey.QueryValue(value,"RegisteredOrganization",&dwSize)==ERROR_SUCCESS)
				if(value[0]!='\0')
					n += _snprintf(pBuffer+n,cbBuffer-1-n,"registered to: %s\r\n",value);

			regkey.Close();
		}
		
		regkey.Detach();
	}
}

/*
	GetWindowsXPTheme()
*/
/*
const CWindowsXPTheme* CWindowsVersion::GetWindowsXPTheme(void)
{
	// i temi vengono supportati solo a partire da XP
	if(m_OsVersionType >= WINDOWS_XP)
		if(!m_pXPTheme)
			m_pXPTheme = new CWindowsXPTheme();

	return(m_pXPTheme);
}
*/