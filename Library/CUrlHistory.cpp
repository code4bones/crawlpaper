/*
	CUrlHistory.cpp
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
#include "win32api.h"
#include "CBase.h"
#include "CUrlHistory.h"

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
	CUrlHistoryTable()
*/
CUrlHistoryTable::CUrlHistoryTable(LPCSTR lpcszTableName/*=NULL*/,LPCSTR lpcszDataPath/*=NULL*/,BOOL bOpenTable/*=TRUE*/) : CBase()
{
	// pathname/nome tabella
	strcpyn(m_szTablePath,lpcszDataPath ? lpcszDataPath : ".",sizeof(m_szTablePath));
	::RemoveBackslash(m_szTablePath);
	strcpyn(m_szTableName,lpcszTableName ? lpcszTableName : URL_HISTORY_TABLE,sizeof(m_szTableName));
	_snprintf(m_szTable,sizeof(m_szTable)-1,"%s\\%s.db",m_szTablePath,m_szTableName);

	// indici
	_snprintf(m_szIndexId,        sizeof(m_szIndexId)-1,        "%s\\%s.id.idx",       m_szTablePath,m_szTableName);
	_snprintf(m_szIndexUrl,       sizeof(m_szIndexUrl)-1,       "%s\\%s.url.idx",      m_szTablePath,m_szTableName);
	_snprintf(m_szIndexDateTime,  sizeof(m_szIndexDateTime)-1,  "%s\\%s.datetime.idx", m_szTablePath,m_szTableName);

	// definisce la tabella
	static CBASE_TABLE table[] = {
		{"ID",		'I',	URL_HISTORY_ID_LEN,		0},	// progressivo unico per l'url
		{"URL",		'C',	URL_HISTORY_URL_LEN,	0},	// url
		{"DATETIME",	'C',	URL_HISTORY_DATETIME_LEN,0},	// data/ora
		{NULL,		 0,	0,					0}
	};

	// definisce l'indice
	static CBASE_INDEX idx[] = {
		{NULL,	"IDX_ID",			"ID"},
		{NULL,	"IDX_URL",		"URL"},
		{NULL,	"IDX_DATETIME",	"DATETIME"},
		{NULL,	 NULL,			 NULL}
	};
	
	idx[0].file = m_szIndexId;
	idx[1].file = m_szIndexUrl;
	idx[2].file = m_szIndexDateTime;

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
const char* CUrlHistoryTable::GetRecordAsString(void)
{
	if(strcmp(record.url,"")==0)
		strsetn(record.url,' ',sizeof(record.url));
	if(strcmp(record.datetime,"")==0)
		strsetn(record.datetime,' ',sizeof(record.datetime));

	_snprintf(m_szRecord,
			sizeof(m_szRecord)-1,
			"%ld%c%s%c%s",
			record.id,
			FIELD_DELIM,
			record.url,
			FIELD_DELIM,
			record.datetime
			);

	return(m_szRecord);
}

/*
	GatherMemvars()
*/
void CUrlHistoryTable::GatherMemvars(void)
{
	// passa le variabili di memoria al record
	PutField(0,record.id);
	PutField(1,record.url);
	PutField(2,record.datetime);
}

/*
	ScatterMemvars()
*/
void CUrlHistoryTable::ScatterMemvars(BOOL bTrim/*=TRUE*/)
{
	// passa il record alle variabili di memoria
	GetField(0,record.id);
	GetField(1,record.url,sizeof(record.url)-1);
	GetField(2,record.datetime,sizeof(record.datetime)-1);

	if(bTrim)
	{
		strrtrim(record.url);
		strrtrim(record.datetime);
	}
}
