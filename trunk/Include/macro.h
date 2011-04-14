/*
	macro.h
	Macro di utilizzo generico.
	Luca Piergentili, 27/07/93
	lpiergentili@yahoo.com
*/
#ifndef _MACRO_H
#define _MACRO_H 1

#define nop
#define AND &&
#define OR ||

/*
	sizeofmacro()
	dimensione della macro
*/
#define sizeofmacro(m) (sizeof(m)-1)

/*
	ARRAY_SIZE
	dimensione dell'array
*/
#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))

/*
	IIF()
	test in linea
*/
#define IIF(x,y,z) ((x)?(y):(z))

/*
	DIV()
	evita la divisione per zero
*/
#define DIV(x,y) (((y)==0) ? ((x)/1) : ((x)/(y)))
#define FDIV(x,y) (((y)==0.0) ? ((x)/1) : ((x)/(y)))

/*
	STR()
	converte in stringa il valore numerico
	il primo passaggio espande il valore numerico (sostituisce il nome di macro con il numero),
	mentre il secondo trasforma in stringa il valore (ad es. STR(666) viene espanso in "666")
*/
#define STR(x) VAL(x)
#define VAL(x) #x

/*
	CAST()
	permette il cast sulle macro se non si puo' accedere direttamente al valore della costante
	(ad es. se BYTEMASK vale 0xff, espande CAST(BYTEMASK,L) in 0xffL).
*/
#define EXPAND(x)	x
#define PASTE(x,y)	(x##y)
#define CAST(x,y)	PASTE(EXPAND(x),y)

/*
	NEARLYEQUAL()
*/
#define NEARLYEQUAL(a,b,epsilon) (abs((int)a - (int)b) < ((int)epsilon / 2))

/*
	PRAGMA_MESSAGE()
	indenta i messaggi inviati con la direttiva #pragma
*/
#ifdef _DEBUG
  #define PRAGMAMESSAGE(s) "   "s
  #define PRAGMA_MESSAGE(s) message(PRAGMAMESSAGE(s))
#else
  #define PRAGMAMESSAGE(s)
  #define PRAGMA_MESSAGE(s)
#endif

#endif /* _MACRO_H */
