/*
	heap.h
	Luca Piergentili, 22/11/96
	l.pierge@teleline.es

	Gestione memoria.
	Le allocazioni/rilasci di memoria vanno effattuati con HEAP_ALLOC(), HEAP_FREE().
	La coppia di macro HEAP_INIT(), HEAP_END() vanno usate al livello piu' alto della
	applicazione, ad es. in main(), dato che effettuano un TRACE con le statistiche.
*/
#ifndef _HEAP_H
#define _HEAP_H 1

/* interfaccia C++ */
#ifdef  __cplusplus
  extern "C" {
#endif

/* headers */
#define STRICT 1
#include <windows.h>
#include "macro.h"

#ifdef _HEAP_C
/*
	DEBUG_HEAP_ALLOC
	struttura per la gestione delle chiamate
*/
typedef struct _alloc_t {
	BOOL		bInit;					/* flag per inizializzazione */
	HANDLE	hProcessHeap;				/* handle per l'heap del processo corrente */
	DWORD	dwAllocCalls;				/* tot. chiamate per allocazione */
	DWORD	dwFreeCalls;				/* tot. chiamate per rilascio */
	DWORD	dwTotAlloc;				/* tot. bytes allocati (richiesti) */
	DWORD	dwTotSize;				/* tot. bytes allocati (effettivi) */
	DWORD	dwTotInUse;				/* tot. bytes in uso */
	DWORD	dwTotFree;				/* tot. bytes rilasciati */
} DEBUG_HEAP_ALLOC;
#endif

/*
	HEAPALLOC(), HEAPFREE()
	macro generiche per HeapAlloc() HeapFree() - le macro usano l'heap del processo corrente
*/
#define HEAPALLOC(n)	HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,(DWORD)(n))
#define HEAPFREE(p)		HeapFree(GetProcessHeap(),0L,(LPVOID)p)

/*
	HEAP_INIT(), HEAP_END(), HEAP_ALLOC(), HEAP_FREE()
	mappa le macro di cui sopra a seconda se compila in _DEBUG o meno
*/
#ifdef _DEBUG
	/* mappa le HEAP_() sulle funzioni di debug */
	void		Heap_Init	(void);
	void		Heap_End	(void);
	LPVOID	Heap_Alloc(LPCSTR,UINT,DWORD);
	void		Heap_Free	(LPCSTR,UINT,LPVOID);

	/* controlla che vengno definite le macro per TRACEEXPR() */
	#ifndef _file_
		#define _file_ "[undefined]"
		#pragma PRAGMA_MESSAGE("warning: macro _file_ NOT defined, default to "_file_)
	#endif
	#ifndef _line_
		#define _line_ 666
		#pragma PRAGMA_MESSAGE("warning: macro _line_ NOT defined, default to "STR(_line_))
	#endif

	#define	HEAP_INIT		Heap_Init
	#define	HEAP_END		Heap_End
	#define	HEAP_ALLOC(n)	Heap_Alloc(_file_,_line_,n)
	#define	HEAP_FREE(p)	Heap_Free(_file_,_line_,p)
#else
	/* mappa le HEAP_() sulle funzioni di sistema */
	#define	HEAP_INIT();
	#define	HEAP_END();
	#define	HEAP_ALLOC(n)	HEAPALLOC(n)
	#define	HEAP_FREE(p)	HEAPFREE(p)
#endif

/* interfaccia C++ */
#ifdef  __cplusplus
  }
#endif

#endif /* _HEAP_H */
