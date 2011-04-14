/*
	typeval.h
	Tipi e valori.
	Luca Piergentili, 27/07/93
	lpiergentili@yahoo.com
*/
#ifndef _TYPEVAL_H
#define _TYPEVAL_H 1

// headers
#if defined(_WINDOWS)
  #define STRICT 1
  #include "window.h"
#endif

// defines
#define	MAX_VALUE	256

//#define	BYTE		1
#define	KILOBYTE	1024
#define	MEGABYTE	1048576
#define	GIGABYTE	1073741824
#define	TERABYTE	1099511627776

/*
	TYPE
	struttura per i tipi
*/
#ifdef __cplusplus
	enum TYPE {
		none,
		bool_t,
		bool_pointer,
		chr,
		chr_pointer,
		string,
		string_pointer,
		lpstr_pointer,
		lpvoid_pointer,
		integer,
		integer_pointer,
		doubleword,
		doubleword_pointer
	};
#else
	typedef enum type_t {
		none,
		bool,
		bool_pointer,
		chr,
		chr_pointer,
		string,
		string_pointer,
		lpstr_pointer,
		lpvoid_pointer,
		integer,
		integer_pointer,
		doubleword,
		doubleword_pointer
	} TYPE;
#endif

/*
	VALUE
	struttura per i valori
*/
#ifdef  __cplusplus
	union VALUE {
		BOOL		bValue;
		BOOL*	bValuePtr;
		char		cValue;
		char		cValuePtr;
		char		szValue[MAX_VALUE+1];
		char*	lpValuePtr;
		LPSTR	lpstrValuePtr;
		LPVOID	lpvoidPtr;
		int		iValue;
		int*		iValuePtr;
		DWORD	dwValue;
		DWORD*	dwValuePtr;
	};
#else
	typedef union value_t {
		BOOL		bValue;
		BOOL		*bValuePtr;
		char		cValue;
		char		cValuePtr;
		char		szValue[MAX_VALUE+1];
		char		*lpValuePtr;
		LPSTR	lpstrValuePtr;
		LPVOID	lpvoidPtr;
		int		iValue;
		int		*iValuePtr;
		DWORD	dwValue;
		DWORD	*dwValuePtr;
	} VALUE;
#endif

/*
	VALUE_EXT
	struttura per valori (con dimensione campo)
*/
#ifdef  __cplusplus
	struct VALUE_EXT {
		DWORD	dwSize;
		TYPE		eType;
		VALUE	eValue;
	};
#else
	typedef struct value_ext_t {
		DWORD	dwSize;
		TYPE		eType;
		VALUE	eValue;
	} VALUE_EXT;
#endif

#endif // _TYPEVAL_H
