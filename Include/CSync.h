/*
	CSync.h
	Classi per la sincronizzazione tra processi/threads.
	Luca Piergentili, 18/11/02
	lpiergentili@yahoo.com
*/
#ifndef _CSYNC_H
#define _CSYNC_H 1

#include <stdlib.h>
#include "window.h"
#include "win32api.h"
#include "CNodeList.h"
#include "CWindowsVersion.h"

#define SYNC_MS_PER_SEC			1000					// ms contenuti in un secondo
#define SYNC_MS_TIMEOUT_INCREMENT	10					// ms per incremento sul calcolo del timeout
#define SYNC_DEFAULT_TIMEOUT		(3 * SYNC_MS_PER_SEC)	// unita' per ms per sec. (3 * 1000 = 3 sec.)

#define SYNC_1_SECS_TIMEOUT		1000					// timeout predefiniti
#define SYNC_2_SECS_TIMEOUT		2000
#define SYNC_3_SECS_TIMEOUT		3000
#define SYNC_4_SECS_TIMEOUT		4000
#define SYNC_5_SECS_TIMEOUT		5000
#define SYNC_10_SECS_TIMEOUT		10000
#define SYNC_15_SECS_TIMEOUT		15000
#define SYNC_30_SECS_TIMEOUT		30000
#define SYNC_60_SECS_TIMEOUT		60000
#define SYNC_INFINITE_TIMEOUT		-1

/*
	GetUniqueMutexName()
*/
LPCSTR GetUniqueMutexName(LPCSTR lpcszName = NULL);

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
class CSyncThreads
{
public:
	CSyncThreads(const char* pName = NULL,int nTimeout = 0);
	virtual ~CSyncThreads();

	virtual BOOL	Lock			(int nTimeout = 0);
	virtual BOOL	Unlock		(void);

	const HANDLE	GetHandle		(void) const {return(NULL);}

	BOOL			SetName		(const char* pName);
	const char*	GetName		(void) const {return(m_szName);}

	void			SetTimeout	(int nTimeout);

private:
	OSVERSIONTYPE	m_osversiontype;
	BOOL			m_bLocked;
	CRITICAL_SECTION m_CriticalSection;
	int			m_nTimeout;
	char			m_szName[_MAX_FILEPATH+1];
};

/*
	CSyncProcesses()

	Classe per la sincronizzazione tra processi differenti o tra threads appartenenti a processi differenti
	La sincronizzazione viene effettuata tramite l'utilizzo di un mutex che deve essere nominato obbligatoriamente.

	Tutti i valori per i timeout vanno espressi in ms (1000 ms = 1 sec).
*/
class CSyncProcesses
{
public:
	CSyncProcesses(const char* pName = NULL,int nTimeout = 0);
	virtual ~CSyncProcesses();

	virtual BOOL	Lock			(int nTimeout = 0);
	virtual BOOL	Unlock		(void);

	const HANDLE	GetHandle		(void) const {return(m_hHandle);}

	BOOL			SetName		(const char* pName);
	const char*	GetName		(void) const {return(m_szName);}

	void			SetTimeout	(int nTimeout);

private:
	int			m_nLockCount;
	CRITICAL_SECTION m_csLockCount;
	BOOL			m_bLocked;
	HANDLE		m_hHandle;
	int			m_nTimeout;
	char			m_szName[_MAX_FILEPATH+1];
};

#endif // _CSYNC_H
