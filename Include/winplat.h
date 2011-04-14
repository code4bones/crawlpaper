/*
	winplat.h
	Headers/macro per Win32.
	Luca Piergentili, 31/10/96
	l.piergentili@ifies.es
*/
#ifndef _WINPLAT_H
#define _WINPLAT_H 1

/* headers */
#include <stdlib.h>
#define STRICT 1
#include <windows.h>
#include "env.h"
#include "macro.h"
#include "typedef.h"
#include "typeval.h"

/* ENTRYPOINT WinMain() */
#if defined(_WIN32)
  #define	EXPORT
  #define	ENTRYPOINT		WINAPI
#elif defined(_WIN16)
  #define	EXPORT			_export
  #define	ENTRYPOINT		FAR PASCAL
#endif

/* macro per LoadIcon(), da non usare quando non viene passato NULL/IDI_WINLOGO (icona predefinita) */
#if defined(_WIN32)
  #define LOADICON(x,y)		LoadIcon((HINSTANCE)x,(LPCSTR)MAKEINTRESOURCE(y))
#elif defined(_WIN16)
  #define LOADICON(x,y)		LoadIcon((HINSTANCE)x,(LPCSTR)#y)
#endif

/* macro per LoadCursor(), da non usare quando vien passato NULL/IDC_ARROW (cursore predefinito) */
#if defined(_WIN32)
  #define	LOADCURSOR(x,y)	LoadCursor((HINSTANCE)x,(LPCSTR)MAKEINTRESOURCE(y))
#elif defined(_WIN16)
  #define	LOADCURSOR(x,y)	LoadCursor((HINSTANCE)x,(LPCSTR)#y)
#endif

/* drive */
#ifndef MAX_DRIVE
  #if defined(_WIN32)
    #define MAX_DRIVE		_MAX_DRIVE
  #elif defined(_WIN16)
    #define MAX_DRIVE		3
  #endif
#endif
//#pragma PRAGMA_MESSAGE(__FILE__": MAX_DRIVE size: "STR(MAX_DRIVE))

/* pathname */
#ifndef MAX_PATH
  #if defined(_WIN32)
    #define MAX_PATH		_MAX_PATH
  #elif defined(_WIN16)
    #define MAX_PATH		128
  #endif
#endif
//#pragma PRAGMA_MESSAGE(__FILE__": MAX_PATH size: "STR(MAX_PATH))

/* nome file */
#ifndef MAX_FNAME
  #if defined(_WIN32)
    #define MAX_FNAME		_MAX_FNAME
  #elif defined(_WIN16)
    #define MAX_FNAME		8
  #endif
#endif
//#pragma PRAGMA_MESSAGE(__FILE__": MAX_FNAME size: "STR(MAX_FNAME))

/* estensione */
#ifndef MAX_EXT
  #if defined(_WIN32)
    #define MAX_EXT			_MAX_EXT
  #elif defined(_WIN16)
    #define MAX_EXT			3
  #endif
#endif
//#pragma PRAGMA_MESSAGE(__FILE__": MAX_EXT size: "STR(MAX_EXT))

/* full pathname */
#if defined(_WIN16) || defined(_WIN32)
  #define MAX_PATHNAME		MAX_DRIVE+MAX_PATH+MAX_FNAME+1+MAX_EXT
#endif
//#pragma PRAGMA_MESSAGE(__FILE__": MAX_PATHNAME size: "STR(MAX_PATHNAME))

#endif /* _WINPLAT_H */
