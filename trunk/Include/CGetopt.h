/*
	CGetopt.h
	Gestione linea di comando.
	Luca Piergentili, 31/08/98
	lpiergentili@yahoo.com
*/
#ifndef _CGETOPT_H
#define _CGETOPT_H 1

#include "window.h"
#include "typeval.h"

// defines
#define MAX_CMDLINE 1024

struct GETOPT {
	char		cOpt;		// carattere per l'opzione
	BOOL		bFound;		// flag per opzione presente
	BOOL		bArgs;		// flag per opzione con argomenti
	TYPE		eType;		// tipo argomento (enum)
	VALUE	uValue;		// valore argomento (union)
};
typedef GETOPT* LPGETOPT;

/*
	CGetopt
*/
class CGetopt
{
public:
	CGetopt() {}
	virtual ~CGetopt() {}
	
	int GetCommandLineOptions(LPCSTR,LPGETOPT,UINT);
};

#endif // _CGETOPT_H
