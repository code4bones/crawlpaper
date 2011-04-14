/*
	wgetopt.h
	Gestione linea di comando.
	Luca Piergentili, 31/08/97
	l.piergentili@ifies.es
*/
#ifndef _WGETOPT_H
#define _WGETOPT_H 1

/* headers */
#define STRICT 1
#include <windows.h>
#include "typeval.h"

/* defines */
#define MAX_CMDLINE 1024					/* dim. max. linea di comando */

/*
	GETOPT
	struttura per le opzioni da linea di comando
	
	nel chiamante:

	- definire un array di tipo GETOPT di n elementi (con n=numero opzioni possibili), es:
	  GETOPT getopt[n]={0};

	- definire n macro OPT_HAVEFLAG_<nome flag>, corrispondenti agli elementi dell'array di cui
	  sopra, per le opzioni semplici, es:
       #define	OPT_HAVEFLAG_QUIET getopt[0].bFound

	- definire n macro OPT_HAVEFLAG_<nome flag>, OPT_HAVEARGS_<nome flag>, OPT_ARGS_<nome flag>,
	  corrispondenti agli elementi dell'array di cui sopra, per le opzioni seguite da un 
	  argomento, es:
	  #define OPT_HAVEFLAG_INI getopt[n].bFound
	  #define OPT_HAVEARGS_INI getopt[n].bArgs
	  #define OPT_ARGS_INI     getopt[n].uValue.szValue

	- definire n macro OPT_HAVEPARAM_<nome parametro>, OPT_PARAM_<nome parametro>, corrispondenti
	  agli elementi dell'array di cui sopra, per i parametri, es:
	  #define OPT_HAVEPARAM_FILE getopt[n].bFound
	  #define OPT_PARAM_FILE     getopt[n].uValue.szValue

	- definire la funzione: GetOpt(LPCSTR,LPGETOPT,UINT) a cui passare la linea di comando,
	  l'array GETOPT ed il numero (n) di elementi dell'array; la funzione GetOpt() deve:
	  - inizializzare gli elementi dell'array con la macro SET_GETOPT(): mentre per le opzioni
	    va specificata una lettera, per i parametri deve essere specificato un numero a partire
	    da 1
	  - impostare i defaults per i valori desiderati
	  - chiamare la GetCommandLineOptions(LPCSTR,LPGETOPT,UINT) e controllare i parametri passati

	notare che, nel caso in cui da linea di comando sia possibile specificare il file .ini da
	utilizzare, va chiamata prima la GetOpt() e poi la GetIni(), senza recuperare i valori gia'
	specificati da linea di comando
*/
typedef struct getopt_t {
	char cOpt;						/* carattere opzione */
	BOOL bFound;						/* flag per opzione presente */
	BOOL bArgs;						/* flag per opzione con argomenti */
	TYPE eType;						/* tipo argomento (enum) */
	VALUE uValue;						/* valore argomento (union) */
} GETOPT,*LPGETOPT;

/*-----------------------------------------------------------------------------------------------

	SET_GETOPT()

	Macro per impostare gli elementi della struttura, passare nell'ordine:

	indice array
	nome array
	carattere opzione
	flag per argomento
	tipo

-----------------------------------------------------------------------------------------------*/
#define SET_GETOPT(_1,_2,_3,_4,_5)	{\
								_2[_1].cOpt=_3;\
								_2[_1].bFound=FALSE;\
								_2[_1].bArgs=_4;\
								_2[_1].eType=_5;\
							}

/*-----------------------------------------------------------------------------------------------

	GetCommandLineOptions()

	input:
	LPCSTR			puntatore alla linea di comando
	LPGETOPT			puntatore alla struttura per le opzioni/parametri
	UINT				dimensione dell'array (numero elementi)

	output:
	void

-----------------------------------------------------------------------------------------------*/
int GetCommandLineOptions(LPCSTR,LPGETOPT,UINT);

/*-----------------------------------------------------------------------------------------------

	GetOpt()

	Definire GetOpt() nel chiamante e chiamarla prima di GetIni()

	input:
	LPCSTR			puntatore alla linea di comando
	LPGETOPT			puntatore alla struttura per le opzioni/parametri
	UINT				dimensione dell'array (numero elementi)

	output:
	void

-----------------------------------------------------------------------------------------------*/
#define GETOPT(a,b) GetOpt(a,&b[0],ARRAY_SIZE(b))

#endif /* _WGETOPT_H */
