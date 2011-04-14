/*
	CUrlDatabase.cpp
	Implementazione delle classi per la definizione delle tabelle.
	Luca Piergentili, 26/06/00
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
#include "CUrlDatabase.h"

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
	CUrlTable()
*/
CUrlTable::CUrlTable(LPCSTR lpcszTableName/* = NULL*/,LPCSTR lpcszDataPath/* = NULL*/,BOOL bOpenTable/* = TRUE*/) : CBase()
{
	// pathname/nome tabella
	strcpyn(m_szTablePath,lpcszDataPath ? lpcszDataPath : ".",sizeof(m_szTablePath));
	::RemoveBackslash(m_szTablePath);
	strcpyn(m_szTableName,lpcszTableName ? lpcszTableName : URL_TABLE,sizeof(m_szTableName));
	_snprintf(m_szTable,sizeof(m_szTable)-1,"%s\\%s.db",m_szTablePath,m_szTableName);

	// indici
	_snprintf(m_szIndexId,        sizeof(m_szIndexId)-1,        "%s\\%s.id.idx",     m_szTablePath,m_szTableName);
	_snprintf(m_szIndexParentUrl, sizeof(m_szIndexParentUrl)-1, "%s\\%s.parent.idx", m_szTablePath,m_szTableName);
	_snprintf(m_szIndexUrl,       sizeof(m_szIndexUrl)-1,       "%s\\%s.url.idx",    m_szTablePath,m_szTableName);
	_snprintf(m_szIndexFile,      sizeof(m_szIndexFile)-1,      "%s\\%s.file.idx",   m_szTablePath,m_szTableName);
	_snprintf(m_szIndexStat,      sizeof(m_szIndexStat)-1,      "%s\\%s.stat.idx",   m_szTablePath,m_szTableName);

	// definisce la tabella
	static CBASE_TABLE table[] = {
		{"ID",		'I',	URL_ID_LEN,		0},	// progressivo unico per l'url
		{"PARENTURL",	'C',	URL_PARENTURL_LEN,	0},	// parent url
		{"URL",		'C',	URL_URL_LEN,		0},	// url
		{"STAT",		'S',	URL_STAT_LEN,		0},	// status
		{"FILE",		'C',	URL_FILE_LEN,		0},	// file locale
		{"SIZE",		'R',	URL_SIZE_LEN,		0},	// dimensione dell'oggetto
		{"SECONDS",	'R',	URL_SECONDS_LEN,	2},	// durata download
		{NULL,		 0,	0,				0}
	};

	// definisce l'indice
	static CBASE_INDEX idx[] = {
		{NULL,	"IDX_ID",			"ID"},
		{NULL,	"IDX_PARENTURL",	"PARENTURL"},
		{NULL,	"IDX_URL",		"URL"},
		{NULL,	"IDX_FILE",		"FILE"},
		{NULL,	"IDX_STAT",		"STAT"},
		{NULL,	 NULL,			 NULL}
	};
	
	idx[0].file = m_szIndexId;
	idx[1].file = m_szIndexParentUrl;
	idx[2].file = m_szIndexUrl;
	idx[3].file = m_szIndexFile;
	idx[4].file = m_szIndexStat;

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
const char* CUrlTable::GetRecordAsString(void)
{
	if(strcmp(record.parenturl,"")==0)
		strsetn(record.parenturl,' ',sizeof(record.parenturl));
	if(strcmp(record.url,"")==0)
		strsetn(record.url,' ',sizeof(record.url));
	if(strcmp(record.file,"")==0)
		strsetn(record.file,' ',sizeof(record.file));

	_snprintf(m_szRecord,
			sizeof(m_szRecord)-1,
			"%ld%c%s%c%s%c%d%c%s%c%0.f%c%0.2f",
			record.id,
			FIELD_DELIM,
			record.parenturl,
			FIELD_DELIM,
			record.url,
			FIELD_DELIM,
			record.stat,
			FIELD_DELIM,
			record.file,
			FIELD_DELIM,
			record.size,
			FIELD_DELIM,
			record.seconds
			);

	return(m_szRecord);
}

/*
	GatherMemvars()
*/
void CUrlTable::GatherMemvars(void)
{
	// passa le variabili di memoria al record
	PutField(0,record.id);
	PutField(1,record.parenturl);
	PutField(2,record.url);
	PutField(3,record.stat);
	PutField(4,record.file);
	PutField(5,record.size);
	PutField(6,record.seconds);
}

/*
	ScatterMemvars()
*/
void CUrlTable::ScatterMemvars(BOOL bTrim/* = TRUE*/)
{
	// passa il record alle variabili di memoria
	GetField(0,record.id);
	GetField(1,record.parenturl,sizeof(record.parenturl)-1);
	GetField(2,record.url,sizeof(record.url)-1);
	GetField(3,record.stat);
	GetField(4,record.file,sizeof(record.file)-1);
	GetField(5,record.size);
	GetField(6,record.seconds);

	if(bTrim)
	{
		strrtrim(record.parenturl);
		strrtrim(record.url);
		strrtrim(record.file);
	}
}

/*
	CUrlDatabaseTable()
*/
CUrlDatabaseTable::CUrlDatabaseTable(LPCSTR lpcszTableName/* = NULL*/,LPCSTR lpcszDataPath/* = NULL*/,BOOL bOpenTable/* = TRUE*/) : CBase()
{
	// pathname/nome tabella
	strcpyn(m_szTablePath,lpcszDataPath ? lpcszDataPath : ".",sizeof(m_szTablePath));
	::RemoveBackslash(m_szTablePath);
	strcpyn(m_szTableName,lpcszTableName ? lpcszTableName : URLDATABASE_TABLE,sizeof(m_szTableName));
	_snprintf(m_szTable,sizeof(m_szTable)-1,"%s\\%s.db",m_szTablePath,m_szTableName);

	// indici
	_snprintf(m_szIndexUrl,  sizeof(m_szIndexUrl)-1,  "%s\\%s.url.idx",  m_szTablePath,m_szTableName);
	_snprintf(m_szIndexDate, sizeof(m_szIndexDate)-1, "%s\\%s.date.idx", m_szTablePath,m_szTableName);

	// definisce la tabella
	static CBASE_TABLE table[] = {
		{"URL",		'C',	URLDATABASE_URL_LEN,			0},	// url
		{"DATE",		'D',	URLDATABASE_DATE_LEN,			0},	// data
		{"STARTTIME",	'T',	URLDATABASE_STARTTIME_LEN,		0},	// ora inizio
		{"ENDTIME",	'T',	URLDATABASE_ENDTIME_LEN,			0},	// ora fine
		{"TOTTIME",	'R',	URLDATABASE_TOTALTIME_LEN,		2},	// durata
		{"DOWNTIME",	'R',	URLDATABASE_DOWNLOADTIME_LEN,		2},	// durata download
		{"DOWNLOADED",	'R',	URLDATABASE_DOWNLOADED_LEN,		0},	// bytes scaricati
		{"TOTALURLS",	'I',	URLDATABASE_TOTALURLS_LEN,		0},	// tot url trovate
		{"TOTALPICT",	'I',	URLDATABASE_TOTALPICT_LEN,		0},	// tot immagini trovate
		{"DOWNURLS",	'I',	URLDATABASE_DOWNLOADEDURLS_LEN,	0},	// tot url scaricate
		{"DOWNPICT",	'I',	URLDATABASE_DOWNLOADEDPICT_LEN,	0},	// tot immagini scaricate
		{"DATATABLE",	'C',	URLDATABASE_DATATABLE_LEN,		0},	// nome tabella relativa
		{"REPORT",	'C',	URLDATABASE_REPORT_LEN,			0},	// nome report relativo
		{"STATUS",	'S',	URLDATABASE_STATUS_LEN,			0},	// status
		{"ID",		'I',	URLDATABASE_ID_LEN,				0},	// ultimo id per download interrotto
		{NULL,		 0,	0,							0}
	};

	// definisce l'indice
	static CBASE_INDEX idx[] = {
		{NULL,	"IDX_URL",	"URL"},
		{NULL,	"IDX_DATE",	"DATE"},
		{NULL,	 NULL,		 NULL}
	};
	
	idx[0].file = m_szIndexUrl;
	idx[1].file = m_szIndexDate;

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
const char* CUrlDatabaseTable::GetRecordAsString(void)
{
	if(strcmp(record.url,"")==0)
		strsetn(record.url,' ',sizeof(record.url));
	if(strcmp(record.datatable,"")==0)
		strsetn(record.datatable,' ',sizeof(record.datatable));
	if(strcmp(record.report,"")==0)
		strsetn(record.report,' ',sizeof(record.report));

	DATEFORMAT df = record.date.GetDateFormat();
	record.date.SetDateFormat(ANSI_SHORT);
	TIMEFORMAT tfs = record.starttime.GetTimeFormat();
	record.starttime.SetTimeFormat(HHMMSS_SHORT);
	TIMEFORMAT tfe = record.endtime.GetTimeFormat();
	record.endtime.SetTimeFormat(HHMMSS_SHORT);

	_snprintf(m_szRecord,
			sizeof(m_szRecord)-1,
			"%s%c%s%c%s%c%s%c%0.2f%c%0.2f%c%0.f%c%ld%c%ld%c%ld%c%ld%c%s%c%s%c%d%c%ld",
			record.url,
			FIELD_DELIM,
			record.date.GetFormattedDate(FALSE),
			FIELD_DELIM,
			record.starttime.GetFormattedTime(FALSE),
			FIELD_DELIM,
			record.endtime.GetFormattedTime(FALSE),
			FIELD_DELIM,
			record.totaltime,
			FIELD_DELIM,
			record.downloadtime,
			FIELD_DELIM,
			record.downloaded,
			FIELD_DELIM,
			record.totalurls,
			FIELD_DELIM,
			record.totalpict,
			FIELD_DELIM,
			record.downloadedurls,
			FIELD_DELIM,
			record.downloadedpict,
			FIELD_DELIM,
			record.datatable,
			FIELD_DELIM,
			record.report,
			FIELD_DELIM,
			record.status,
			FIELD_DELIM,
			record.id
			);

	record.date.SetDateFormat(df);
	record.starttime.SetTimeFormat(tfs);
	record.endtime.SetTimeFormat(tfe);

	return(m_szRecord);
}

/*
	ResetMemvars()
*/
void CUrlDatabaseTable::ResetMemvars(void)
{
	memset(&record.url,'\0',sizeof(record.url));
	record.date.Reset();
	record.starttime.Reset();
	record.endtime.Reset();
	record.totaltime = 0.0f;
	record.downloadtime = 0.0f;
	record.downloaded = 0.0f;
	record.totalurls = 0;
	record.totalpict = 0;
	record.downloadedurls = 0;
	record.downloadedpict = 0;
	memset(&record.datatable,'\0',sizeof(record.datatable));
	memset(&record.report,'\0',sizeof(record.report));
	record.status = 0;
	record.id = 0;
}

/*
	GatherMemvars()
*/
void CUrlDatabaseTable::GatherMemvars(void)
{
	// passa le variabili di memoria al record
	PutField(0,record.url);
	PutField(1,record.date);
	PutField(2,record.starttime);
	PutField(3,record.endtime);
	PutField(4,record.totaltime);
	PutField(5,record.downloadtime);
	PutField(6,record.downloaded);
	PutField(7,record.totalurls);
	PutField(8,record.totalpict);
	PutField(9,record.downloadedurls);
	PutField(10,record.downloadedpict);
	PutField(11,record.datatable);
	PutField(12,record.report);
	PutField(13,record.status);
	PutField(14,record.id);
}

/*
	ScatterMemvars()
*/
void CUrlDatabaseTable::ScatterMemvars(BOOL bTrim/* = TRUE*/)
{
	// passa il record alle variabili di memoria
	GetField(0,record.url,sizeof(record.url)-1);
	GetField(1,record.date);
	GetField(2,record.starttime);
	GetField(3,record.endtime);
	GetField(4,record.totaltime);
	GetField(5,record.downloadtime);
	GetField(6,record.downloaded);
	GetField(7,record.totalurls);
	GetField(8,record.totalpict);
	GetField(9,record.downloadedurls);
	GetField(10,record.downloadedpict);
	GetField(11,record.datatable,sizeof(record.datatable)-1);
	GetField(12,record.report,sizeof(record.report)-1);
	GetField(13,record.status);
	GetField(14,record.id);

	if(bTrim)
	{
		strrtrim(record.url);
		strrtrim(record.datatable);
		strrtrim(record.report);
	}
}
