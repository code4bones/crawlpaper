/*
	stristr.cpp
	Cerca la prima occorrenza della sottostringa nella stringa ignorando la differenza tra maiusc./minusc. (CRT).
	Luca Piergentili, 20/10/00
	lpiergentili@yahoo.com
*/
#include "env.h"
#include "pragma.h"
#include <stdio.h>
#include <ctype.h>
#include "strichr.h"
#include "stristr.h"

char* stristr(const char* s1,const char* s2)
{
	for(; (s1 = strichr(s1,*s2))!=NULL; ++s1)
	{
		const char *sc1,*sc2;

		for(sc1 = s1,sc2 = s2; ; )
			if(*++sc2=='\0')
				return((char*)s1);
			else if(toupper(*++sc1)!=toupper(*sc2))
				break;
	}

	return(NULL);
}
