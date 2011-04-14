/*
	typedef.h
	Definizione dei tipi.
	Luca Piergentili, 27/07/93
	lpiergentili@yahoo.com
*/
#ifndef _TYPEDEF_H
#define _TYPEDEF_H 1

#define VIRTUAL
#define IN
#define OUT

/*
	_BOOL_TYPE
*/
#ifndef _BOOL_TYPE
  typedef int BOOL;
  #define TRUE 1
  #define FALSE 0
  #define _BOOL_TYPE 1
#endif

/*
	_QWORD_TYPE
*/
#ifndef _QWORD_TYPE
  #if defined(_WINDOWS) && defined(_MSC_VER)
    typedef unsigned __int64 QWORD;
    typedef signed __int64 QWORDSIGNED;
  #else
    typedef unsigned long long QWORD;
    typedef signed long long QWORDSIGNED;
  #endif
  typedef QWORD *LPQWORD;
  typedef QWORDSIGNED *LPQWORDSIGNED;
  #define _QWORD_TYPE 1
#endif

/*
	FPCALLBACK
	tipo per il puntatore a funzione per le callback
*/
#ifndef _FPCALLBACK_TYPE
  #define _FPCALLBACK_TYPE 1
  //typedef UINT (*FPCALLBACK)(LPVOID,LPVOID);
  typedef unsigned int (*FPCALLBACK)(void*,void*);
#endif // _FPCALLBACK_TYPE

#endif /* _TYPEDEF_H */
