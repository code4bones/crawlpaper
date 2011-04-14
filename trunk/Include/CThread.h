/*
	CThread.h
	Classe per il thread.
	Luca Piergentili, 19/11/02
	lpiergentili@yahoo.com
*/
#ifndef _CTHREAD_H
#define _CTHREAD_H 1

#include "window.h"

/*
	PTHREADPROC
	tipo per il puntatore alla funzione per il thread
*/
typedef UINT (__cdecl *PTHREADPROC)(LPVOID);

/*
	THREADPARAMS
	struttura per il passaggio dei parametri
*/
struct THREADPARAMS {
	LPVOID		lpVoid;
	PTHREADPROC	pfnProc;
	LPVOID		pParam;
};

/*
	THREAD_STATE
	enum per gli stati del thread
*/
enum THREAD_STATE {
	THREAD_UNDEFINED,
	THREAD_RUNNING,
	THREAD_DONE
};

/*
	CThread

	Classe per la creazione del thread, non usare direttamente ma chiamare la BeginThread() per
	creare un nuovo thread. Non eliminare il puntatore restituito da BeginThread() con una
	delete a meno che non si imposti il flag m_bAutoDelete su TRUE.
*/
class CThread
{
public:
	CThread();
	virtual ~CThread();

	void				SetAutoDelete	(BOOL bAutoDelete);
	
	const HANDLE		GetHandle		(void) const;
	const UINT		GetId		(void) const;
	const THREAD_STATE	GetStatus		(void) const;
	
	DWORD			GetPriority	(void);
	BOOL				SetPriority	(DWORD nPriority);

	BOOL				Suspend		(void);
	BOOL				Resume		(void);
	BOOL				Abort		(void);
	
	HANDLE			Create		(
								PTHREADPROC pfnThreadProc,
								LPVOID pParam,
								int nPriority = THREAD_PRIORITY_NORMAL,
								UINT nStackSize = 0L,
								DWORD dwCreateFlags = 0L,
								LPSECURITY_ATTRIBUTES lpSecurityAttrs = NULL
								);

private:
	static UINT APIENTRY ThreadProc	(LPVOID lpVoid);

	BOOL			m_bAutoDelete;
	HANDLE		m_hHandle;
	UINT			m_nID;
	THREAD_STATE	m_nState;
	int			m_nPriority;
	THREADPARAMS	m_ThreadParams;
};

/*
	BeginThread()

	Crea un nuovo thread restituendo il puntatore all'oggetto.
	Non eliminare l'oggetto con una delete a meno che non si imposti il flag m_bAutoDelete su TRUE.
*/
CThread* BeginThread(
				PTHREADPROC pfnThreadProc,
				LPVOID pParam,
				int nPriority = THREAD_PRIORITY_NORMAL,
				UINT nStackSize = 0L,
				DWORD dwCreateFlags = 0L,
				LPSECURITY_ATTRIBUTES lpSecurityAttrs = NULL
				);

#endif // _CTHREAD_H
