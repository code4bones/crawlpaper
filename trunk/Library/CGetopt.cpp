/*
	CGetopt.cpp
	Gestione linea di comando.
	Luca Piergentili, 31/08/98
	lpiergentili@yahoo.com
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include <stdlib.h>
#include <string.h>
#include "strings.h"
#include "window.h"
#include "typeval.h"
#include "CGetopt.h"

#include "traceexpr.h"
//#define _TRACE_FLAG	_TRFLAG_TRACEOUTPUT
//#define _TRACE_FLAG	_TRFLAG_NOTRACE
#define _TRACE_FLAG		_TRFLAG_NOTRACE
#define _TRACE_FLAG_INFO	_TRFLAG_NOTRACE
#define _TRACE_FLAG_WARN	_TRFLAG_NOTRACE
#define _TRACE_FLAG_ERR	_TRFLAG_NOTRACE

#if (defined(_DEBUG) && defined(_WINDOWS)) && (defined(_AFX) || defined(_AFXDLL))
#ifdef PRAGMA_MESSAGE_VERBOSE
  #pragma message("\t\t\t"__FILE__"("STR(__LINE__)"): using DEBUG_NEW macro")
#endif
  #define new DEBUG_NEW
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif

/*
	GetCommandLineOptions()
	
	Carica le opzioni presenti sulla linea di comando, impostando i flag relativi.
	Le opzioni che prevedono un argomento devono essere seguite dall'argomento senza spazi.
	Per verificare (dal chiamante) il passaggio di un opzione controllare i campi bFound e 
	bArgs della struttura.
*/
int CGetopt::GetCommandLineOptions(LPCSTR lpszCmdLine,LPGETOPT lpOpt,UINT iSize)
{
	char cOpt;
	char* p;
	char buffer[MAX_CMDLINE+1];
	char arg[MAX_CMDLINE+1];
	int iCmdParam=0;

	// copia in locale la linea di comando
	strcpyn(buffer,lpszCmdLine,sizeof(buffer));

	p = buffer;
	while(*p)
	{
		if(*p=='/')
		{
			cOpt = *++p;

			for(int i = 0; i < (int)iSize; i++)
			{
				// se si tratta dell'opzione trovata sopra
				if(lpOpt[i].cOpt==cOpt)
				{
					lpOpt[i].bFound = TRUE;
					
					// ricava l'eventuale argomento previsto dall'opzione
					if(lpOpt[i].bArgs)
					{
						memset(arg,'\0',sizeof(arg));
						++p;
						for(int n = 0; *p && *p!='/' && n < sizeof(arg)-1; n++)
						{
							arg[n] = *p;
							p++;
						}

						strrtrim(arg);

						switch(lpOpt[i].eType)
						{
							// intero
							case integer:
								if(arg[0]!='\0')
									lpOpt[i].uValue.iValue = atoi(arg);
								else
									lpOpt[i].bArgs = FALSE;
								break;

							// stringa
							case string:
								if(arg[0]!='\0')
									strcpyn(lpOpt[i].uValue.szValue,arg,MAX_VALUE+1);
								else
									lpOpt[i].bArgs = FALSE;
								break;
						}
					}

					break;
				} // opzione
			} // for
		}
		else
			p++;
	}

	// restituisce il numero di parametri trovati
	return(iCmdParam);
}
