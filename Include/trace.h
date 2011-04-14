/*
	trace.h
	Luca Piergentili, 16/01/97
	l.pierge@teleline.es

	Controllo esecuzione, definisce il codice solo se in modalita' _DEBUG.
*/
#ifndef _TRACE_H
#define _TRACE_H 1

/* interfaccia C++ */
#ifdef  __cplusplus
	extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define	TRACE_LOG_FILE		"trace.log"		/* nome del file di log */
#define	TRACECALL_LOG_FILE	"tracecall.log"	/* nome del file di log per le chiamate di funzione */
#define	TRACE_BUF			8192				/* dimensione del buffer per l'output */
#define	_TRFLAG_TRACE		0x0002			/* flag */
#define	_TRFLAG_NOTRACE	0x0004

#define __NOFILE__ ((const char *)NULL)
#define __NOLINE__ (0)

/* abilita TRACEEXPR() solo se viene definito _DEBUG */
#ifdef _DEBUG
	#ifdef _WINDOWS
		/*
		definire a seconda del tipo di output desiderato (file/video)
		notare che un processo puo' avere una sola console attiva, vedi AllocConsole()
		*/
		#ifdef _TRACE_C
			#define TRACE_FILE 1
	//		#define TRACE_CONSOLE 1
		#endif

		#ifdef TRACE_FILE
			#pragma message("\t\t\t"__FILE__": TRACE_FILE defined")
		#endif 

		#ifdef TRACE_CONSOLE
			#pragma message("\t\t\t"__FILE__": TRACE_CONSOLE defined")
		#endif 

		/* output generico */
		void trace(int,const char*,unsigned int,char*,...);
		#define TRACEEXPR(a) {trace a;}

		/* chiamate di funzione */
		void trace_call(int,int,char *);
		#define TRACECALLS_BEGIN(b,a)	trace_call(b,0,a)
		#define TRACECALLS_END(b,a)	trace_call(b,1,a)
		#define TRACECALLS_ABEND(b,a)	trace_call(b,1,a)

		#ifdef TRACE_FILE
			/* per azzerare il file di log */
			void trace_init(void);
			#define TRACE_INIT trace_init
		#endif

		void assert_expr(void *,void *,unsigned);
		#define ASSERTEXPR(e) (void)((e)||(assert_expr(#e,__FILE__,__LINE__),0))
		#define ASSERTCALL(e) (void)((e)||(assert_expr(#e,__FILE__,__LINE__),0))
	#else
		#define TRACECALLS_BEGIN(b,a)
		#define TRACECALLS_END(b,a)
		#define TRACECALLS_ABEND(b,a)
		#define TRACEEXPR(a) {;}
		#define TRACE_INIT()

		#define ASSERTEXPR(e)	assert(e)
		#define ASSERTCALL(e)	e
	#endif
#else
	#define TRACECALLS_BEGIN(b,a)
	#define TRACECALLS_END(b,a)
	#define TRACECALLS_ABEND(b,a)
	#define TRACEEXPR(a) {;}
	#define TRACE_INIT()

	#define ASSERTEXPR(e)
	//#define ASSERTEXPR(e)	((void)0)
	#define ASSERTCALL(e)	e
#endif

/* interfaccia C++ */
#ifdef  __cplusplus
	}
#endif

#endif /* _TRACE_H */
