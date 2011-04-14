/*
	strichr.cpp
	Cerca la prima occorrenza del carattere nella stringa ignorando la differenza tra maiusc./minusc. (CRT).
	Luca Piergentili, 20/10/00
	lpiergentili@yahoo.com
*/
#include "env.h"
#include "pragma.h"
#include <stdio.h>
#include <ctype.h>
#include "strichr.h"

char* strichr(const char* s,int c)
{
	const char ch = (char)toupper(c);

	for(; toupper(*s)!=ch; ++s)
		if(*s=='\0')
			return(NULL);
	
	return((char*)s);
}
