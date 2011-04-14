/*
	CMutexEx.h
	Classe derivata per la gestione dei mutex (MFC).
	Luca Piergentili, 06/07/98
	lpiergentili@yahoo.com
*/
#ifndef _CMUTEXEX_H
#define _CMUTEXEX_H 1

#include <stdio.h>
#include <stdlib.h>
#include "strings.h"
#include "window.h"

#define MUTEX_LOCK_TIMEOUT 30

#ifndef _THREAD_STATE_DEFINED
#define _THREAD_STATE_DEFINED 1
enum THREAD_STATE {
	THREAD_UNDEFINED,
	THREAD_RUNNING,
	THREAD_DONE
};
#endif

class CMutexEx : public CMutex
{
public:
	CMutexEx(LPCSTR lpszName = NULL,BOOL bInitiallyOwn = FALSE,LPSECURITY_ATTRIBUTES lpsaAttribute = NULL);
	virtual ~CMutexEx() {}

	inline HANDLE		GetHandle		(void)			{return(CMutex::m_hObject);}
	inline void		SetTimeout	(UINT nTimeout)	{m_nTimeout = nTimeout;}

// la zoccola di MFC nomina il mutex solo se in debug
#ifdef _DEBUG
	inline LPCSTR		GetName		(void)			{return(CMutex::m_strName);}
#else
	inline LPCSTR		GetName		(void)			{return(m_szMutexName);}
#endif

	BOOL				LockEx		(UINT);
	inline BOOL		LockEx		(void)			{return(CMutexEx::LockEx(m_nTimeout));}
	inline BOOL		UnlockEx		(void)			{return(CMutex::Unlock());}

private:
	UINT				m_nTimeout;
	char				m_szMutexName[_MAX_PATH+1];
};

#endif // _CMUTEXEX_H
