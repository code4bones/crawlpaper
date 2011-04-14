/*
	CSync.cpp
	Classi per la sincronizzazione tra processi/threads.
	Luca Piergentili, 18/11/02
	lpiergentili@yahoo.com
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include <string.h>
#include <ctype.h>
#include "strings.h"
#include "window.h"
#include "win32api.h"
#include "CWindowsVersion.h"
#include "CSync.h"

//#define _CSYNC_VERBOSE 1

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
	GetUniqueMutexName()
*/
LPCSTR GetUniqueMutexName(LPCSTR lpcszName/* = NULL */)
{
	static int n = 0;
	static char szMutexName[_MAX_FILEPATH+1];
	_snprintf(szMutexName,
			sizeof(szMutexName)-1,
			"%s_%ld",
			lpcszName ? lpcszName : "Lucarella",
			::GetTickCount() + (++n)
			);
	TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"GetUniqueMutexName(): %s\n",szMutexName));
	return(szMutexName);
}

/*
	CSyncThreads()
	
	Classe per la sincronizzazione tra threads appartenenti allo stesso processo.
	La sincronizzazione viene effettuata tramite l'utilizzo di una sezione critica e funziona solo con threads facenti parte dello stesso
	processo.

	Tutti i valori per i timeout vanno espressi in ms (1000 ms = 1 sec).

	Occhio: se non ricordo male, quando si usa una sezione critica, lo stesso thread puo' bloccare piu' volte lo stesso oggetto (nel senso
	che la sezione critica accetta piu' locks sullo stesso oggetto da parte dello stesso thread) per cui se piu' threads (distinti) chiamano
	una porzione statica di codice che cerca di bloccare una sezione critica, i blocchi multipli riescono e va tutto a puttane relativamente
	alla sincronizzazione.
*/
CSyncThreads::CSyncThreads(const char* pName/* = NULL */,int nTimeout/* = 0 */)
{
	CWindowsVersion winver;
	m_osversiontype = winver.GetVersionType();

	memset(m_szName,'\0',sizeof(m_szName));
	if(pName)
		SetName(pName);
	
	m_nTimeout = nTimeout;
	if(m_nTimeout <= 0)
		m_nTimeout = SYNC_DEFAULT_TIMEOUT;
	
	memset(&m_CriticalSection,'\0',sizeof(m_CriticalSection));
	::InitializeCriticalSection(&m_CriticalSection);
}

/*
	~CSyncThreads()
*/
CSyncThreads::~CSyncThreads()
{
	::LeaveCriticalSection(&m_CriticalSection);
	::DeleteCriticalSection(&m_CriticalSection);
}

/*
	Lock()
*/
BOOL CSyncThreads::Lock(int nTimeout/* = 0 */)
{
	BOOL bLocked = FALSE;
	int nElapsed = 0;
	int nMsTimeoutIncrement = SYNC_MS_TIMEOUT_INCREMENT;

	if(nTimeout==0)
	{
		if(m_nTimeout <= 0)
			m_nTimeout = SYNC_DEFAULT_TIMEOUT;
		nTimeout = m_nTimeout;
	}
	if(nTimeout!=SYNC_INFINITE_TIMEOUT)
	{
		nMsTimeoutIncrement = nTimeout / 10;
		if(nMsTimeoutIncrement < SYNC_MS_TIMEOUT_INCREMENT)
			nMsTimeoutIncrement = SYNC_MS_TIMEOUT_INCREMENT;
	}

	// la zoccola di Windows9.x non ha la TryEnter...(), per cui deve
	// rimappare sulla Enter...(), perdendo l'opzione per il timeout
	switch(m_osversiontype)
	{
		case WINDOWS_NT:
		case WINDOWS_2000:
		case WINDOWS_XP:
		case WINDOWS_VISTA:
		case WINDOWS_SEVEN:
		{
			bLocked = FALSE;

			do
			{
				if((bLocked=::TryEnterCriticalSection(&m_CriticalSection))==FALSE)
				{
					if(nTimeout!=SYNC_INFINITE_TIMEOUT)
					{
						::Sleep(nMsTimeoutIncrement);
						nElapsed += nMsTimeoutIncrement;
						if(nElapsed >= nTimeout)
							break;
					}
				}
			} while(!bLocked);
		}
		break;
		
		default:
		{
			::EnterCriticalSection(&m_CriticalSection);
			bLocked = TRUE;
		}
		break;
	}

	TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CSyncThreads::Lock(): [%s] lock %s\n",m_szName[0]!='\0' ? m_szName : "NONAME",bLocked ? "succeed" : "FAILED"));

#ifdef _CSYNC_VERBOSE
	if(!bLocked)
	{
		char buffer[512];
		_snprintf(buffer,sizeof(buffer)-1,"[%s]: lock failed, try again ?",m_szName[0]!='\0' ? m_szName : "NONAME");
		if(::MessageBox(NULL,buffer,"CSyncThreads::Lock()",MB_YESNO|MB_ICONWARNING|MB_TASKMODAL|MB_SETFOREGROUND|MB_TOPMOST)==IDYES)
			return(Lock(nTimeout));
	}
#endif

	return(bLocked);
}

/*
	Unlock()
*/
BOOL CSyncThreads::Unlock(void)
{
	::LeaveCriticalSection(&m_CriticalSection);
	TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CSyncThreads::Unlock(): [%s] unlocked\n",m_szName[0]!='\0' ? m_szName : "NONAME"));
	return(TRUE);
}

/*
	SetName()
*/
BOOL CSyncThreads::SetName(const char* pName)
{
	BOOL bFlag = FALSE;

	if(pName && m_szName[0]=='\0')
	{
		for(int i=0; *pName && i < sizeof(m_szName)-1;)
		{
			if(isalnum(*pName) || *pName=='_')
				m_szName[i++] = *pName;
			pName++;
		}
		bFlag = TRUE;
	}

	return(bFlag);
}

/*
	SetTimeout()
*/
void CSyncThreads::SetTimeout(int nTimeout)
{
	m_nTimeout = nTimeout;
	if(m_nTimeout <= 0)
		m_nTimeout = SYNC_DEFAULT_TIMEOUT;
}

/*
	CSyncProcesses()

	Classe per la sincronizzazione tra processi differenti o tra threads appartenenti a processi differenti
	La sincronizzazione viene effettuata tramite l'utilizzo di un mutex che deve essere nominato obbligatoriamente.

	Tutti i valori per i timeout vanno espressi in ms (1000 ms = 1 sec).
*/
CSyncProcesses::CSyncProcesses(const char* pName/*=NULL*/,int nTimeout/*=0*/)
{
	m_hHandle = NULL;

	memset(m_szName,'\0',sizeof(m_szName));
	if(pName)
		SetName(pName);
	
	m_nTimeout = nTimeout;
	if(m_nTimeout <= 0)
		m_nTimeout = SYNC_DEFAULT_TIMEOUT;

	m_nLockCount = 0;
	::InitializeCriticalSection(&m_csLockCount);
}

/*
	~CSyncProcesses()
*/
CSyncProcesses::~CSyncProcesses()
{
	if(m_hHandle)
		::ReleaseMutex(m_hHandle);
	if(m_hHandle)
		::CloseHandle(m_hHandle);
	m_hHandle = NULL;

	memset(m_szName,'\0',sizeof(m_szName));

	m_nLockCount = 0;
	::DeleteCriticalSection(&m_csLockCount);
}

/*
	Lock()
*/
BOOL CSyncProcesses::Lock(int nTimeout/* = 0 */)
{
	int nMsTimeoutIncrement = SYNC_MS_TIMEOUT_INCREMENT;
	int nElapsed = 0;
	BOOL bLocked = FALSE;	
	DWORD dwRet = 0L;

	// nome mutex	
	if(m_szName[0]=='\0')
	{
		TRACEEXPR((_TRACE_FLAG_ERR,__NOFILE__,__NOLINE__,"CSyncProcesses::Lock(): invalid mutex name\n"));

#ifdef _CSYNC_VERBOSE
		::MessageBox(NULL,"Invalid mutex name.","CSyncProcesses::Lock()",MB_OK|MB_ICONERROR|MB_TASKMODAL|MB_SETFOREGROUND|MB_TOPMOST);
#endif
		return(bLocked);
	}

	// handle
	if(!m_hHandle)
	{
		TRACEEXPR((_TRACE_FLAG_ERR,__NOFILE__,__NOLINE__,"CSyncProcesses::Lock(): [%s] invalid mutex handle\n",m_szName));

#ifdef _CSYNC_VERBOSE
		::MessageBox(NULL,"Invalid mutex handle.","CSyncProcesses::Lock()",MB_OK|MB_ICONERROR|MB_TASKMODAL|MB_SETFOREGROUND|MB_TOPMOST);
#endif
		return(bLocked);
	}

	// valore timeout
	if(nTimeout==0)
	{
		if(m_nTimeout <= 0)
			m_nTimeout = SYNC_DEFAULT_TIMEOUT;
		nTimeout = m_nTimeout;
	}
	if(nTimeout!=SYNC_INFINITE_TIMEOUT)
	{
		nMsTimeoutIncrement = nTimeout / 10;
		if(nMsTimeoutIncrement < SYNC_MS_TIMEOUT_INCREMENT)
			nMsTimeoutIncrement = SYNC_MS_TIMEOUT_INCREMENT;
	}

	do
	{
		if(m_nLockCount > 0)
		{
			dwRet = WAIT_FAILED;
			::Sleep(nMsTimeoutIncrement);
		}
		else
			dwRet = ::WaitForSingleObject(m_hHandle,nMsTimeoutIncrement);

		if(dwRet==WAIT_FAILED || dwRet==WAIT_ABANDONED || dwRet==WAIT_TIMEOUT)
			bLocked = FALSE;
		else if(dwRet==WAIT_OBJECT_0)
			bLocked = TRUE;

		if(bLocked)
		{
			::EnterCriticalSection(&m_csLockCount);
			m_nLockCount++;
			::LeaveCriticalSection(&m_csLockCount);
		}
		else
		{
			if(nTimeout!=SYNC_INFINITE_TIMEOUT)
			{
				nElapsed += nMsTimeoutIncrement;
				if(nElapsed >= nTimeout)
					break;
			}
			else
			{
				::Sleep(nMsTimeoutIncrement);
			}
		}
	} while(!bLocked);

	TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CSyncProcesses::Lock(): [%s] lock %s\n",m_szName,bLocked ? "succeed" : "FAILED"));

	// panic!
	if(!bLocked && nTimeout==SYNC_INFINITE_TIMEOUT)
	{
		TRACEEXPR((_TRACE_FLAG_ERR,__NOFILE__,__NOLINE__,"CSyncProcesses::Lock(): [%s] PANIC!\n",m_szName));
		_asm int 3;
	}

#ifdef _CSYNC_VERBOSE
	if(!bLocked)
	{
		char buffer[512];
		_snprintf(buffer,sizeof(buffer)-1,"%s: lock failed, try again ?",m_szName);
		if(::MessageBox(NULL,buffer,"CSyncProcesses::Lock()",MB_YESNO|MB_ICONWARNING|MB_TASKMODAL|MB_SETFOREGROUND|MB_TOPMOST)==IDYES)
			return(Lock(nTimeout));
	}
#endif

	return(bLocked);
}

/*
	Unlock()
*/
BOOL CSyncProcesses::Unlock(void)
{
	BOOL bUnlocked = FALSE;
	
	if(m_hHandle)
	{
		bUnlocked = ::ReleaseMutex(m_hHandle);
		TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CSyncProcesses::Unlock(): [%s] %s\n",m_szName,bUnlocked ? "unlocked" : "unlock FAILED"));

		::EnterCriticalSection(&m_csLockCount);
		m_nLockCount--;
		::LeaveCriticalSection(&m_csLockCount);
	}

#ifdef _CSYNC_VERBOSE
	if(!bUnlocked)
	{
		char buffer[512];
		_snprintf(buffer,sizeof(buffer)-1,"[%s]: unlock failed.",m_szName);
		::MessageBox(NULL,buffer,"CSyncProcesses::Unlock()",MB_OK|MB_ICONERROR|MB_TASKMODAL|MB_SETFOREGROUND|MB_TOPMOST);
	}
#endif

	return(bUnlocked);
}

/*
	SetName()
*/
BOOL CSyncProcesses::SetName(const char* pName)
{
	BOOL bFlag = FALSE;

	if(pName && m_szName[0]=='\0')
	{
		for(int i=0; *pName && i < sizeof(m_szName)-1;)
		{
			if(isalnum(*pName) || *pName=='_')
				m_szName[i++] = *pName;
			pName++;
		}

		if(!m_hHandle)
			m_hHandle = ::CreateMutex(NULL,FALSE,m_szName);

		bFlag = TRUE;
	}

	return(bFlag);
}

/*
	SetTimeout()
*/
void CSyncProcesses::SetTimeout(int nTimeout)
{
	m_nTimeout = nTimeout;
	if(m_nTimeout <= 0)
		m_nTimeout = SYNC_DEFAULT_TIMEOUT;
}
