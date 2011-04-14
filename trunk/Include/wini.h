/*
	wini.h
	Gestione file .ini.
	Luca Piergentili, 28/04/97
	l.piergentili@ifies.es
*/
#ifndef _WINI_H
#define _WINI_H 1

/* headers */
#define STRICT 1
#include <windows.h>
#include "typeval.h"

/* defines */
#define MAX_INI_SECTION	64				/* dim. max. nome sezione */
#define MAX_INI_FIELD	64				/* dim. max. nome campo */
#define MAX_INI_VALUE	MAX_VALUE			/* dim. max. valore campo */

/*
	INIVALUE
	struttura per il caricamento dei valori presenti nel .ini
	
	nel chiamante:

	- definire un array di tipo INIVALUE di n elementi (con n=numero voci del .ini), es:
	  INIVALUE inivalue[n]={0};

	- definire n macro INI_VALUE_<nome campo .ini>, corrispondenti agli elementi dell'array
	  di cui sopra (utilizzare il tipo corrispondente della union), es:
       #define	INI_VALUE_VERSION inivalue[n].uValue.szValue

	- definire la funzione: GetIni(LPCSTR,LPINIVALUE,UINT) a cui passare il pathname per il
	  .ini, l'array INIVALUE ed il numero (n) di elementi dell'array; la funzione GetIni() 
	  deve:
	  - inizializzare gli elementi dell'array con le macro SET_INIVALUE_<tipo>()
	  - chiamare la GetValuesFromIni(LPCSTR,LPINIVALUE,UINT)
	  - impostare con quanto letto dal .ini (solo) i valori non presenti sulla linea di comando
	    dato che si suppone che le opzioni da linea di comando sovrascrivano quanto presente nel
	    .ini

	notare che, nel caso in cui da linea di comando sia possibile specificare il file .ini da
	utilizzare, va chiamata prima la GetOpt() e poi la GetIni(), senza recuperare i valori gia'
	specificati da linea di comando
*/
typedef struct inivalue_t {
	char szSection[MAX_INI_SECTION];		/* nome sezione */
	char szField[MAX_INI_FIELD];			/* nome campo */
	TYPE eType;						/* tipo campo (enum) */
	VALUE uValue;						/* valore campo (union) */
	VALUE uDefaultValue;				/* valore di default del campo (union) */
	BOOL bGet;						/* flag per caricamento valore */
} INIVALUE,*LPINIVALUE;

/*-----------------------------------------------------------------------------------------------

	SET_INIVALUE_...()

	macro per impostare gli elementi della struttura, passare nell'ordine:

	indice array
	nome array
	nome sezione
	nome campo
	valore di default
	flag per caricamento valore

-----------------------------------------------------------------------------------------------*/
#define SET_INIVALUE_STR(_1,_2,_3,_4,_5,_6)	{\
									lstrcpy(_2[_1].szSection,_3);\
									lstrcpy(_2[_1].szField,_4);\
									_2[_1].eType=string;\
									lstrcpy(_2[_1].uDefaultValue.szValue,_5);\
									_2[_1].bGet=_6;\
									}
#define SET_INIVALUE_BOOL(_1,_2,_3,_4,_5,_6)	{\
									lstrcpy(_2[_1].szSection,_3);\
									lstrcpy(_2[_1].szField,_4);\
									_2[_1].eType=bool;\
									_2[_1].uDefaultValue.bValue=_5;\
									_2[_1].bGet=_6;\
									}
#define SET_INIVALUE_INT(_1,_2,_3,_4,_5,_6)	{\
									lstrcpy(_2[_1].szSection,_3);\
									lstrcpy(_2[_1].szField,_4);\
									_2[_1].eType=integer;\
									_2[_1].uDefaultValue.iValue=_5;\
									_2[_1].bGet=_6;\
									}

/*-----------------------------------------------------------------------------------------------

	GetValuesFromIni()

	input:
	LPCSTR			puntatore al pathname per il .ini
	LPINIVALUE		puntatore alla struttura per i valori
	UINT				dimensione dell'array (numero elementi)

	output:
	void

-----------------------------------------------------------------------------------------------*/
void GetValuesFromIni(LPCSTR,LPINIVALUE,UINT);

/*-----------------------------------------------------------------------------------------------

	GetIni()

	Definire GetIni() nel chiamante e chiamarla dopo GetOpt()

	input:
	LPCSTR			puntatore al pathname per il .ini
	LPINIVALUE		puntatore alla struttura per i valori
	UINT				dimensione dell'array (numero elementi)

	output:
	nessuno

-----------------------------------------------------------------------------------------------*/
#define GETINI(a,b) GetIni(a,&b[0],ARRAY_SIZE(b))

#endif /* _WINI_H */
