/*
	CCookieTable.cpp
	Implementazione delle classi per la definizione della tabella.
	Luca Piergentili, 30/10/03
	lpiergentili@yahoo.com
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include "typedef.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "strings.h"
#include "window.h"
#include "CBase.h"
#include "CCookieTable.h"

#include "traceexpr.h"
//#define _TRACE_FLAG _TRFLAG_TRACEOUTPUT
//#define _TRACE_FLAG _TRFLAG_NOTRACE
#define _TRACE_FLAG_INFO _TRFLAG_NOTRACE
#define _TRACE_FLAG_WARN _TRFLAG_NOTRACE
#define _TRACE_FLAG_ERR _TRFLAG_NOTRACE

#if (defined(_DEBUG) && defined(_WINDOWS)) && (defined(_AFX) || defined(_AFXDLL))
  #pragma message("\t\t\t"__FILE__"("STR(__LINE__)"): using DEBUG_NEW macro")
  #define new DEBUG_NEW
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif

/*
	CCookieTable()
*/
CCookieTable::CCookieTable(LPCSTR lpcszTableName/*=NULL*/,LPCSTR lpcszDataPath/*=NULL*/,BOOL bOpenTable/*=TRUE*/) : CBase()
{
	// pathname/nome tabella
	strcpyn(m_szTablePath,lpcszDataPath ? lpcszDataPath : ".",sizeof(m_szTablePath));
	::RemoveBackslash(m_szTablePath);
	strcpyn(m_szTableName,lpcszTableName ? lpcszTableName : COOKIEDATA_TABLE,sizeof(m_szTableName));
	_snprintf(m_szTable,sizeof(m_szTable)-1,"%s\\%s.db",m_szTablePath,m_szTableName);

	// indici
	_snprintf(m_szIndexName,   sizeof(m_szIndexName)-1,   "%s\\%s.name.idx",   m_szTablePath,m_szTableName);
	_snprintf(m_szIndexDomain, sizeof(m_szIndexDomain)-1, "%s\\%s.domain.idx", m_szTablePath,m_szTableName);

	// definisce la tabella
	static CBASE_TABLE table[] = {
		{"NAME",		'C',	COOKIEDATA_NAME_LEN,	0},
		{"VALUE",		'C',	COOKIEDATA_VALUE_LEN,	0},
		{"EXPIRES",	'C',	COOKIEDATA_EXPIRES_LEN,	0},
		{"PATH",		'C',	COOKIEDATA_PATH_LEN,	0},
		{"DOMAIN",	'C',	COOKIEDATA_DOMAIN_LEN,	0},
		{"SECURE",	'I',	COOKIEDATA_SECURE_LEN,	0},
		{NULL,		 0,	0,					0}
	};

	// definisce l'indice
	static CBASE_INDEX idx[] = {
		{NULL,	"IDX_NAME",		"NAME"},
		{NULL,	"IDX_DOMAIN",		"DOMAIN"},
		{NULL,	 NULL,			 NULL}
	};
	
	idx[0].file = m_szIndexName;
	idx[1].file = m_szIndexDomain;

	// puntatori alla struttura della tabella/indice
	table_struct = &table[0];
	idx_struct = &idx[0];

	// carica la definizione della tabella
	if(Create(m_szTable,&table[0],&idx[0]))
	{
		// azzera il record
		ResetMemvars();
		
		// apre la tabella
		if(bOpenTable)
			if(Open())
				SetIndex(0);
	}
}

/*
	GetRecordAsString()
*/
const char* CCookieTable::GetRecordAsString(void)
{
	if(strcmp(record.name,"")==0)
		strsetn(record.name,' ',sizeof(record.name));
	if(strcmp(record.value,"")==0)
		strsetn(record.value,' ',sizeof(record.value));
	if(strcmp(record.expires,"")==0)
		strsetn(record.expires,' ',sizeof(record.expires));
	if(strcmp(record.path,"")==0)
		strsetn(record.path,' ',sizeof(record.path));
	if(strcmp(record.domain,"")==0)
		strsetn(record.domain,' ',sizeof(record.domain));

	_snprintf(m_szRecord,
			sizeof(m_szRecord)-1,
			"%s%c%s%c%s%c%s%c%s%c%d",
			record.name,
			FIELD_DELIM,
			record.value,
			FIELD_DELIM,
			record.expires,
			FIELD_DELIM,
			record.path,
			FIELD_DELIM,
			record.domain,
			FIELD_DELIM,
			record.secure
			);

	return(m_szRecord);
}

/*
	GatherMemvars()
*/
void CCookieTable::GatherMemvars(void)
{
	// passa le variabili di memoria al record
	PutField(0,record.name);
	PutField(1,record.value);
	PutField(2,record.expires);
	PutField(3,record.path);
	PutField(4,record.domain);
	PutField(5,record.secure);
}

/*
	ScatterMemvars()
*/
void CCookieTable::ScatterMemvars(BOOL bTrim/*=TRUE*/)
{
	// passa il record alle variabili di memoria
	GetField(0,record.name,sizeof(record.name)-1);
	GetField(1,record.value,sizeof(record.value)-1);
	GetField(2,record.expires,sizeof(record.expires)-1);
	GetField(3,record.path,sizeof(record.path)-1);
	GetField(4,record.domain,sizeof(record.domain)-1);
	GetField(5,record.secure);

	if(bTrim)
	{
		strrtrim(record.name);
		strrtrim(record.value);
		strrtrim(record.expires);
		strrtrim(record.path);
		strrtrim(record.domain);
	}
}
