/*
	env.h
	Definizione delle macro per l'ambiente (da includere solo nei .c/.cpp).
	Luca Piergentili, 22/01/93
	lpiergentili@yahoo.com
*/
#ifndef _ENV_H
#define _ENV_H 1

/*
	_MSC_VER
*/
#ifndef _MSC_VER
  #pragma message("\t\t\terror: _MSC_VER not defined")
  #error unsupported compiler
#endif

/*
	C++
*/
#ifdef PRAGMA_MESSAGE_VERBOSE
	#ifdef __cplusplus
	  #ifdef _DEBUG
	    #pragma message("\t\t\tC++ compile mode (Debug)")
	  #else
	    #pragma message("\t\t\tC++ compile mode (Release)")
	  #endif
	#else
	  #ifdef _DEBUG
	    #pragma message("\t\t\tC compile mode (Debug)")
	  #else
	    #pragma message("\t\t\tC compile mode (Release)")
	  #endif
	#endif
#endif

/*
	_CONSOLE
*/
#ifdef PRAGMA_MESSAGE_VERBOSE
	#ifndef _CONSOLE
	  #pragma message("\t\t\t_CONSOLE not defined")
	#else
	  #pragma message("\t\t\t_CONSOLE defined")
	#endif
#endif

/*
	WIN32/_WIN32
*/
#ifdef WIN32
  #ifndef _WIN32
    #define _WIN32
  #endif
#endif

#ifdef _WIN32
  #ifndef WIN32
    #define WIN32
  #endif
#endif

#ifdef PRAGMA_MESSAGE_VERBOSE
	#ifndef _WIN32
	  #pragma message("\t\t\t_WIN32 not defined")
	#else
	  #pragma message("\t\t\t_WIN32 defined")
	#endif
#endif

/*
	_WINDOWS
*/
#ifdef PRAGMA_MESSAGE_VERBOSE
	#ifndef _WINDOWS
	  #pragma message("\t\t\t_WINDOWS not defined")
	#else
	  #pragma message("\t\t\t_WINDOWS defined")
	#endif
#endif

/*
	MFC
*/
#ifdef PRAGMA_MESSAGE_VERBOSE
	#if defined(_AFX) || defined(_AFXDLL)
	  #pragma message("\t\t\tMFC defined")
	#endif
#endif

#endif /* _ENV_H */
