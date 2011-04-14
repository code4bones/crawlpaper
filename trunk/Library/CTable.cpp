/*
	CTable.cpp
	Classe derivata per l'interfaccia con il database (CRT/SDK/MFC).
	Luca Piergentili, 04/11/99
	lpiergentili@yahoo.com
*/
#include "env.h"
#include "pragma.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "strings.h"
#include "typedef.h"
#include "CBase.h"
#include "CTable.h"

#include "traceexpr.h"
//#define _TRACE_FLAG	_TRFLAG_TRACEOUTPUT
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
	CTable()
*/
CTable::CTable(const char* lpcszTableName/*=NULL*/,const char* lpcszDataPath/*=NULL*/,BOOL bOpenTable/*=TRUE*/)
{
	strcpyn(m_szTablePath,lpcszDataPath ? lpcszDataPath : ".",sizeof(m_szTablePath));
	strcpyn(m_szTableName,lpcszTableName ? lpcszTableName : "CTable",sizeof(m_szTableName));
	_snprintf(m_szTable,sizeof(m_szTable)-1,"%s\\%s.def",m_szTablePath,lpcszTableName ? lpcszTableName : "CTable");

	m_Record.len = 0;
	m_Record.record = NULL;

	// carica la definizione della tabella
	if(Create(m_szTable))
	{
		m_Record.len = GetRecordLength();
		m_Record.record = new char[m_Record.len + GetFieldCount() + 1];

		// apre la tabella
		if(bOpenTable)
			Open();
	}
}

/*
	~CTable()
*/
CTable::~CTable()
{
	if(m_Record.record)
		delete [] m_Record.record,m_Record.record = NULL;
}

/*
	GetRecordLength()
*/
int CTable::GetRecordLength(void)
{
	int nRecordLength = 0;

	CBASE_TABLE* pTableStruct = (CBASE_TABLE*)GetTableStruct();
	if(pTableStruct)
		for(int i = 0; pTableStruct[i].field; i++)
			nRecordLength += pTableStruct[i].size;

	return(nRecordLength);
}

/*
	GetRecordAsString()
*/
const char* CTable::GetRecordAsString(void)
{
	int nOfs = 0;
	int nLen = 0;
	int nTotFields = GetFieldCount();
	char szField[MAX_FIELDSIZE+1];

	for(int i = 0; i < nTotFields; i++)
	{
		strcpyn(szField,GetFieldRaw(i),sizeof(szField));
		nOfs += sprintf(m_Record.record + nOfs,"%s%c",szField,FIELD_DELIM);
	}
	
	if(nTotFields > 0)
	{
		nLen = strlen(m_Record.record)-1;
		if(m_Record.record[nLen]==FIELD_DELIM)
			m_Record.record[nLen] = '\0';
	}

	return(m_Record.record);
}
