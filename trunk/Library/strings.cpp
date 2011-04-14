/*
	strings.cpp
	Operazioni sulle stringhe.
	Luca Piergentili, 13/09/98
	lpiergentili@yahoo.com
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "typedef.h"
#include "strings.h"
#ifdef _WINDOWS
  #include "window.h"
#endif

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

// interne
int _strright		(const char*,const char*,int ignorecase);
int _strleft		(const char*,const char*,int ignorecase);
int _safetoupper	(int c);
int _safetolower	(int c);

/*
	memnxor()
	
	Pone in xor ogni carattere del buffer con tutti i caratteri della password.
	La lunghezza specifica il numero massimo di caratteri del buffer da mettere in xor.
*/
void memnxor(char* buffer,char* psw,unsigned int len)
{
	register int i;
	register char* p;
	for(i = 0; i < (int)len; i++)
	{
		p = psw;
		for(; *p; p++)
			buffer[i] = (char)(buffer[i]^(*p));
	}
}

/*
	strichr()

	Cerca la prima occorrenza del carattere nella stringa, ignorando la differenza tra maiusc./minusc.
*/
char* strichr(const char* s,int c)
{
	const char ch = (char)_safetoupper(c);

	for(; _safetoupper(*s)!=ch; ++s)
		if(*s=='\0')
			return(NULL);
	
	return((char*)s);
}

/*
	stristr()
	
	Cerca la prima occorrenza della sottostringa (s2) nella stringa (s1), ignorando la differenza tra maiusc./minusc.
*/
char* stristr(const char* s1,const char* s2)
{
	for(; (s1 = strichr(s1,*s2))!=NULL; ++s1)
	{
		const char *sc1,*sc2;

		for(sc1 = s1,sc2 = s2; ; )
			if(*++sc2=='\0')
				return((char*)s1);
			else if(_safetoupper(*++sc1)!=_safetoupper(*sc2))
				break;
	}

	return(NULL);
}

/*
	strsetn()

	Imposta il contenuto della stringa con il carattere specificato.
	Passare la dimensione totale (reale) della stringa, imposta l'ultimo carattere a '\0'.
*/
char* strsetn(char* s,const char c,int size)
{
	memset(s,c,size);
	s[size-1] = '\0';
	return(s);
}

/*
	strcpyn()

	Copia una stringa sull'altra (s2 su s1), controllando la dimensione della stringa di destinazione.
	Passare la dimensione totale (reale) della stringa di destino, imposta l'ultimo carattere a '\0'.
*/
char* strcpyn(char* s1,const char *s2,int size)
{
	if(!s1)
		return(NULL);

	if(!s2 || !*s2)
	{
		memset(s1,'\0',size);
		return(NULL);
	}

	char* p = strncpy(s1,s2,size);
	s1[size-1] = '\0';

	return(p);
}

/*
	strcatn()
*/
char* strcatn(char* s1,const char *s2,int size)
{
	int n = strlen(s1);
	if((size - n - 1) > 0)
	{
		memcpy(s1+n,s2,size-n-1);
		s1[size-1] = '\0';
	}
	return(s1);
}

/*
	substr()

	Sostituisce nel buffer la prima occorrenza della sottostringa nella stringa con quanto specificato.
	Passare la dimensione totale (reale) del buffer.
	Restituisce 0 se non effettua nessuna sostituzione, 1 in caso contrario.
	In input:	stringa originale (dove cercare)
			stringa da cercare e sost.
			valore con cui sostituire
			buffer di output
			dim. del buffer di output
*/
int substr(const char* str,const char* subst,const char* replace,char* buffer,int size)
{
	char* p;
	strcpyn(buffer,str,size);

	if((p = strstr(buffer,subst))!=NULL)
	{
		int i = size - (p-buffer);
		strcpyn(p,replace,i);
		p = (char*)strstr(str,subst) + strlen(subst);
		i = strlen(buffer);
		strcpyn(buffer+i,p,size-i);
		return(1);
	}
	else
		return(0);
}

/*
	strempty()

	Verifica se la stringa e' vuota (blank), restituendo 0 se la stringa contiene solo spazi o 1
	in caso contrario.
*/
int strempty(const char* s)
{
	register char* p = (char*)s;
	while(*p)
	{
		if(!ISSPACE(*p))
			return(0);
		*p++;
	}
	return(1);
}

/*
	strrtrim()

	Elimina gli spazi (finali) alla destra della stringa.
*/
int strrtrim(char* s)
{
	register int i = strlen(s)-1;
	
	while(i >= 0)
	{
		if(ISSPACE(s[i]))
			s[i] = '\0';
		else
			break;
		i--;
	}
	
	return(i);
}

/*
	strltrim()

	Elimina gli spazi (iniziali) alla sinistra della stringa.
*/
int strltrim(char* s)
{
	register int i = 0;
	register int n = strlen(s);
	
	while(i < n)
	{
		if(!ISSPACE(s[i]))
			break;
		i++;
	}

	if(i < n)
	{
		memmove(s,s+i,n-i);
		s[n-i] = '\0';
	}
	else
		i = 0;

	return(n-i);
}

/*
	strintrim()
	
	Elimina tutti gli spazi dalla stringa.
*/
int strintrim(char* str)
{
	char* p = str;

	while(*p)
	{
		if(ISSPACE(*p))
		{
			char* P = p;
			while(*(P+1))
			{
				*P = *(P+1);
				P++;
			}
			*P = '\0';
			p--;
		}

		p++;
	}

	return(strlen(str));
}

/*
	strstrim()
	
	Riduce in tutta la stringa due o piu' spazi ad uno solo.
*/
int strstrim(char* str)
{
	char* p = str;

	while(*p)
	{
		if(ISSPACE(*p))
			if(*(p+1))
				if(ISSPACE(*(p+1)))
				{
					char* P = p;
					while(*(P+1))
					{
						*P = *(P+1);
						P++;
					}
					*P = '\0';
					p = str;
				}

		p++;
	}

	return(strlen(str));
}

/*
	strrot()

	Ruota  di <n> caratteri la stringa verso destra o sinistra.
	In input: la stringa da ruotare
			verso rotazione (-1 a sinistra, 1 a destra, 0 nessuna rotazione)
			numero di caratteri da ruotare
*/
char* strrot(char* str,int n,int t)
{
	char c = '\0';
	int len = strlen(str);
	if(n < 0)
	{
		c = str[0];
		memmove(str,str+1,len-1);
		str[len-1] = c;
	}
	else if(n >= 1)
	{
		c = str[len-1];
		memmove(str+1,str,len-1);
		str[0] = c;
	}

	return(t > 1 ? strrot(str,n,--t) : str);
}

/*
	strright(), striright(), strleft(), strileft()

	Controllano se la stringa specificata appare alla destra/sinistra della stringa
	di input, distinguendo o meno tra maiuscole e minuscole.
*/
int _strright(const char* s,const char* str,int ignorecase)
{
	int len = strlen(str);
	int at = strlen(s) - len;
	return(at >= 0 ? (ignorecase ? strnicmp(s+at,str,len) : strncmp(s+at,str,len)) : -1);
}

int _strleft(const char* s,const char* str,int ignorecase)
{
	int len = strlen(str);
	return(len > 0 ? (ignorecase ? strnicmp(s,str,len) : strncmp(s,str,len)) : -1);
}

int strright(const char* s,const char* str)
{
	return(_strright(s,str,0));
}

int strleft(const char* s,const char* str)
{
	return(_strleft(s,str,0));
}

int striright(const char* s,const char* str)
{
	return(_strright(s,str,1));
}

int strileft(const char* s,const char* str)
{
	return(_strleft(s,str,1));
}

/*
	strsize()
*/
const char* strsize(double bytes)
{
	static char str[32] = {0};

	// bytes
	if(bytes < 1024.0f)
	{
		_snprintf(str,sizeof(str)-1,"%d bytes",(int)bytes);
	}
	// KB
	else if(bytes < 1048576.0f)
	{
		_snprintf(str,sizeof(str)-1,"%0.2f",FDIV(bytes,1024.0f));
		char* p = strstr(str,".00");
		if(p)
			*p = '\0';
		strcat(str," KB");
	}
	// MB
	else if(bytes < 1073741824.0f)
	{
		_snprintf(str,sizeof(str)-1,"%0.2f",FDIV(bytes,1048576.0f));
		char* p = strstr(str,".00");
		if(p)
			*p = '\0';
		strcat(str," MB");
	}
	// GB
	else
	{
		_snprintf(str,sizeof(str)-1,"%0.2f",FDIV(bytes,1073741824.0f));
		char* p = strstr(str,".00");
		if(p)
			*p = '\0';
		strcat(str," GB");
	}

	return(str);
}

/*
	strsize()
*/
char* strsize(char* s,int n,double bytes)
{
	// bytes
	if(bytes < 1024.0f)
	{
		_snprintf(s,n-1,"%d bytes",(int)bytes);
	}
	// KB
	else if(bytes < 1048576.0f)
	{
		_snprintf(s,n-1,"%0.2f",FDIV(bytes,1024.0f));
		char* p = strstr(s,".00");
		if(p)
			*p = '\0';
		strcat(s," KB");
	}
	// MB
	else if(bytes < 1073741824.0f)
	{
		_snprintf(s,n-1,"%0.2f",FDIV(bytes,1048576.0f));
		char* p = strstr(s,".00");
		if(p)
			*p = '\0';
		strcat(s," MB");
	}
	// GB
	else
	{
		_snprintf(s,n-1,"%0.2f",FDIV(bytes,1073741824.0f));
		char* p = strstr(s,".00");
		if(p)
			*p = '\0';
		strcat(s," GB");
	}

	return(s);
}

/*
	strdupl()
*/
char* strdupl(const char* str)
{
	if(!str)
		return(NULL);
	
	char* s;
	int n = strlen(str);
	s = new char[n+1];
	if(s)
		strcpy(s,str);
	
	return(s);
}

/*
	_safetolower()
*/
int _safetolower(int c)
{
	// se non si tratta di un carattere la libreria di runtime fa il botto
	if(c > 0)
		if(isalpha(c))
			if(isupper(c))
				return(tolower(c));
	return(c);
}

/*
	_safetoupper()
*/
int _safetoupper(int c)
{
	// se non si tratta di un carattere la libreria di runtime fa il botto
	if(c > 0)
		if(isalpha(c))
			if(islower(c))
				return(toupper(c));
	return(c);
}

/*
	ltos(), ultos(), dtos()

	Convertono il numero in stringa formattata.
*/
typedef enum number_type_t {
	long_number,
	unsigned_long_number,
	double_number,
	qword_number
} NUMBER_TYPE_T;

typedef union number_value_t {
	long			lValue;
	unsigned long	ulValue;
	double		dValue;
	QWORD		qwValue;
} NUMBER_VALUE_T;

typedef struct number_t {
	NUMBER_TYPE_T type;
	NUMBER_VALUE_T value;
} NUMBER_T;

char* _ntos(NUMBER_T* n,char* str,int size)
{
	int i = 0;
	char buffer[48] = {0};
	char* p = buffer;
	char* s = str;

	switch(n->type)
	{
		case long_number:
			_snprintf(buffer,sizeof(buffer)-1,"%ld",n->value.lValue);
			break;
		case unsigned_long_number:
			_snprintf(buffer,sizeof(buffer)-1,"%u",n->value.ulValue);
			break;
		case double_number:
		case qword_number:
			_snprintf(buffer,sizeof(buffer)-1,"%0.f",n->value.dValue);
			break;
	}

	strrev(buffer);
	
	memset(str,'\0',size);

	while(*p)
	{
		while(*p && (s - str < size) && ++i < 4)
			*s++ = *p++;

		if(s - str >= size)
			break;

		*s++ = ',';
		
		i = 0;
	}

	if(*(s-1)==',')
		--s;

	*s = '\0';

	strrev(str);

	return(str);
}

char* ltos(long num,char* str,int size)
{
	NUMBER_T n;
	n.type = long_number;
	n.value.lValue = num;
	return(_ntos(&n,str,size));
}

char* ultos(unsigned long num,char* str,int size)
{
	NUMBER_T n;
	n.type = unsigned_long_number;
	n.value.ulValue = num;
	return(_ntos(&n,str,size));
}

char* dtos(double num,char* str,int size)
{
	NUMBER_T n;
	n.type = double_number;
	n.value.dValue = num;
	return(_ntos(&n,str,size));
}

char* qwtos(QWORD num,char* str,int size)
{
	NUMBER_T n;
	n.type = qword_number;
	n.value.qwValue = num;
	return(_ntos(&n,str,size));
}

/*
	datetostr()

	Da data a stringa.
*/
#ifdef _WINDOWS
char* datetostr(unsigned int d,unsigned int t,char* str,int size)
{
	char* m[] = {"???","jan","feb","mar","apr","may","jun","jul","aug","sep","oct","nov","dec"};
	FILETIME filetime = {0};
	SYSTEMTIME systemtime = {0};
	memset(str,'\0',size);

	if(::DosDateTimeToFileTime((WORD)d,(WORD)t,&filetime))
		if(::FileTimeToSystemTime(&filetime,&systemtime))
			_snprintf(str,size-1,"%02d %s %04d %02d:%02d:%02d",systemtime.wDay,m[systemtime.wMonth],systemtime.wYear,systemtime.wHour,systemtime.wMinute,systemtime.wSecond);

	return(str);
}
#endif
