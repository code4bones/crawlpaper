/*
	strings.h
	Operazioni sulle stringhe.
	Luca Piergentili, 13/09/98
	lpiergentili@yahoo.com
*/
#ifndef _STRINGS_H
#define _STRINGS_H 1

#include <stdio.h>
#include <string.h>
#include "typedef.h"

#define ISSPACE(c) ((c==' ') || (c=='\t') || (c=='\v') || (c=='\f') || (c=='\r') || (c=='\n'))
#define ISALPHA(c) (c >= 0 && c <= 255)
#define ISSEP(c) ((c=='.') || (c==',') || (c==';') || (c==':') || (c=='-'))

inline void*	bzero	(void* dest) {return(memset(dest,'\0',sizeof(dest)));}
inline void*	bzero	(void* dest,unsigned int size) {return(memset(dest,'\0',size));}

void			memnxor	(char* buffer,char* psw,unsigned int len);

char*		strichr	(const char* s,int c);
char*		stristr	(const char* s1,const char* s2);

char*		strsetn	(char* s,const char c,int size);
char*		strcpyn	(char* s1,const char *s2,int size);
char*		strcatn	(char* s1,const char *s2,int size);

int			substr	(const char* str,const char* subst,const char* replace,char* buffer,int size);

inline bool	strnull	(const char* str) {return(str ? (str[0]=='\0' ? true : (bool)(strcmp(str,"")==0)) : true);}
int			strempty	(const char*);
int			strrtrim	(char*);
int			strltrim	(char*);
int			strintrim	(char* str);
int			strstrim	(char* str);

char*		strrot	(char* str,int n,int t);

int			strright	(const char* s,const char* str);
int			strleft	(const char* s,const char* str);
int			striright	(const char* s,const char* str);
int			strileft	(const char* s,const char* str);

const char*	strsize	(double bytes);
char*		strsize	(char* s,int n,double bytes);

char*		strdupl	(const char* str);

char*		ltos		(long,char*,int);
char*		ultos	(unsigned long,char*,int);
char*		dtos		(double,char*,int);
char*		qwtos	(QWORD,char*,int);

#ifdef _WINDOWS
char*		datetostr	(unsigned int d,unsigned int t,char* str,int size);
#endif

#endif // _STRINGS_H
