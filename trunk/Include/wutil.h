/*
	wutil.h
	Utilita' generiche.
	Luca Piergentili, 06/07/96
	l.piergentili@ifies.es
*/
#ifndef _WUTIL_H
#define _WUTIL_H 1

/* interfaccia C++ */
#ifdef __cplusplus
  extern "C" {
#endif

/* headers */
#define STRICT 1
#include <windows.h>

/* stringhe */
void		UnCrlf			(LPSTR);
UINT		CountWords		(LPSTR);
BOOL		GetWord			(LPSTR,UINT,LPSTR);
BOOL		GetLine			(LPSTR,UINT,LPSTR);
void		NullToSpace		(LPSTR);
int		ltrim			(char *);
int		rtrim			(char *);
int		alltrim			(char *);
void		padr				(char *,char,int);
void		padl				(char *,char,int);
BOOL		empty			(char *);

/* messaggi */
#define MESSAGEBOXFMT_MAX_BUFFER 8192

int		MessageBoxFmt		(HWND,UINT,LPCSTR,LPSTR,...);
int		MessageBoxLastError	(HWND,UINT);

/* interfaccia C++ */
#ifdef __cplusplus
  }
#endif

#endif /* _WUTIL_H */
