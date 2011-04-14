/*
	traceexpr.c
	Luca Piergentili, 16/01/97
	lpiergentili@yahoo.com
*/
#if defined(_DEBUG) && defined(_WINDOWS)

#include "env.h"
#include "pragma.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <signal.h>
#define STRICT 1
#include <windows.h>
#include "traceexpr.h"

static HANDLE hConsoleHandle = INVALID_HANDLE_VALUE;

/*
	trace()
*/
void trace(unsigned long flag,const char* file,unsigned int line,char* fmt,...)
{
	char* pArgs = NULL;
	DWORD dwWrite = 0L;
	char trace_buf[TRACE_BUF+1] = {0};
	char buf[TRACE_BUF+1] = {0};

	/* ricava gli argomenti */
	pArgs = (char*)&fmt + sizeof(fmt);
	_vsnprintf(buf,sizeof(buf)-1,fmt,pArgs);

	/* costruisce la linea di debug */
	if(file!=__NOFILE__ && line!=__NOLINE__)
		_snprintf(trace_buf,sizeof(trace_buf)-1,"%s(%d): %s",file,line,buf);
	else
		_snprintf(trace_buf,sizeof(trace_buf)-1,"%s",buf);

	/* output su file */
	if(flag & _TRFLAG_TRACEFILE)
	{
		/* registra su file */
		HANDLE hFileHandle = INVALID_HANDLE_VALUE;
		if((hFileHandle = CreateFile(TRACE_LOG_FILE,GENERIC_WRITE,0,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL))!=INVALID_HANDLE_VALUE)
		{
			int n = strlen(trace_buf);
			if(trace_buf[n-1]=='\n' && trace_buf[n-2]!='\r')
			{
				trace_buf[n-1] = '\r',trace_buf[n] = '\n',trace_buf[n+1] = '\0';
				n++;
			}
			SetFilePointer(hFileHandle,0L,NULL,FILE_END);
			WriteFile(hFileHandle,trace_buf,n,&dwWrite,NULL);
			CloseHandle(hFileHandle);
		}
	}

	/* output in finestra */
	if(flag & _TRFLAG_TRACECONSOLE)
	{
		/* visualizza in finestra */
		if(hConsoleHandle==INVALID_HANDLE_VALUE)
		{
			if(AllocConsole())
			{
				SetConsoleTitle("TRACE");
				hConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
			}
		}
		if(hConsoleHandle!=INVALID_HANDLE_VALUE)
			WriteConsole(hConsoleHandle,trace_buf,strlen(trace_buf),&dwWrite,NULL);
	}

	/* output nella finestra del debugger */
	if(flag & _TRFLAG_TRACEOUTPUT)
	{
		OutputDebugString(trace_buf);
	}
}

/*
	trace_call()
*/
void trace_call(int flag,int call,char *func)
{
	static int nDeep = 0;
	DWORD dwWrite;
	char trace_buf[TRACE_BUF],buf[(TRACE_BUF/2)];

	if(call==0)
	{
		nDeep += 2;
		_snprintf(buf,sizeof(buf)-1,"%c%ds%cs %c%c",'%',nDeep,'%','\r','\n');
		_snprintf(trace_buf,sizeof(trace_buf)-1,buf," ",func);
	}
	else
	{
		_snprintf(buf,sizeof(buf)-1,"%c%ds%cs %c%c",'%',nDeep,'%','\r','\n');
		_snprintf(trace_buf,sizeof(trace_buf)-1,buf," ",func);
		nDeep -= 2;
	}

	if(nDeep <= 0)
		lstrcat(trace_buf,"\r\n");

	if(flag & _TRFLAG_TRACEFILE)
	{
		/* registra su file */
		HANDLE hFileCallHandle = INVALID_HANDLE_VALUE;
		if((hFileCallHandle = CreateFile(TRACECALL_LOG_FILE,GENERIC_WRITE,0,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL))!=INVALID_HANDLE_VALUE)
		{
			SetFilePointer(hFileCallHandle,0L,NULL,FILE_END);
			WriteFile(hFileCallHandle,trace_buf,lstrlen(trace_buf),&dwWrite,NULL);
			CloseHandle(hFileCallHandle);
		}
	}

	if(flag & _TRFLAG_TRACECONSOLE)
	{
		/* visualizza in finestra */
		if(hConsoleHandle==INVALID_HANDLE_VALUE)
		{
			if(AllocConsole())
			{
				SetConsoleTitle("TRACE");
				hConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
			}
		}
		if(hConsoleHandle!=INVALID_HANDLE_VALUE)
			WriteConsole(hConsoleHandle,trace_buf,lstrlen(trace_buf),&dwWrite,NULL);
	}

	if(flag & _TRFLAG_TRACEOUTPUT)
	{
		/* visualizza nella finestra del debugger */
		OutputDebugString(trace_buf);
	}
}

/*
	trace_init()
*/
void trace_init(void)
{
	HANDLE hFileHandle = INVALID_HANDLE_VALUE;
	HANDLE hFileCallHandle = INVALID_HANDLE_VALUE;
	if((hFileHandle=CreateFile(TRACE_LOG_FILE,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL))!=INVALID_HANDLE_VALUE)
		CloseHandle(hFileHandle);
	if((hFileCallHandle = CreateFile(TRACECALL_LOG_FILE,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL))!=INVALID_HANDLE_VALUE)
		CloseHandle(hFileCallHandle);
}

/*
	assert_expr()
*/
void assert_expr(void *expr,void *file,unsigned line)
{
	int nCode;
	char assertbuf[TRACE_BUF];
	char assertfmt[] = {
		"Assertion failed:\r\n\r\n"\
		"program: %s\r\n"\
		"file: %s\r\n"\
		"line: %ld\r\n\r\n"\
		"expression:\r\n"\
		"%s\r\n\r\n"\
		"Press Cancel to abort application or Ok to return..."
	};

	char progname[_MAX_PATH+1];
	if(!GetModuleFileName(NULL,progname,sizeof(progname)))
		strcpy(progname,"<unknown>");

	memset(assertbuf,'\0',sizeof(assertbuf));
	_snprintf(assertbuf,sizeof(assertbuf)-1,assertfmt,progname,file,line,expr);

	nCode = MessageBox(NULL,assertbuf,"assert()",MB_OKCANCEL|MB_ICONHAND|MB_TASKMODAL|MB_SETFOREGROUND|MB_TOPMOST);

	if(nCode==IDCANCEL)
	{
		/* raise abort signal */
		raise(SIGABRT);

		/* we usually won't get here, but it's possible that SIGABRT was ignored, so exit the program anyway */
		_exit(3);
	}
	/* Ignore: continue execution */
	else if(nCode==IDOK)
		return;

	abort();
}

#endif /* defined(_DEBUG) && defined(_WINDOWS) */

#undef _TRACE_C
