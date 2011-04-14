/*
	CUrlHistory.h
	Implementazione delle classi per la definizione della tabella.
	Luca Piergentili, 30/10/03
	lpiergentili@yahoo.com
*/
#ifndef _CURLHISTORY_H
#define _CURLHISTORY_H 1

#include "macro.h"
#include "typedef.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "strings.h"
#include "CBase.h"
#include "CUrl.h"

// tabella
#define URL_HISTORY_TABLE		"urlhistorytable"
#define URL_HISTORY_ID_LEN		FIELD_INT_MAXSIZE
#define URL_HISTORY_URL_LEN		MAX_URL
#define URL_HISTORY_DATETIME_LEN	14
#define URL_HISTORY_RECORD_LENGTH	(URL_HISTORY_ID_LEN + URL_HISTORY_URL_LEN + URL_HISTORY_DATETIME_LEN)

// indici
#define URL_HISTORY_IDX_ID		0
#define URL_HISTORY_IDX_URL		1
#define URL_HISTORY_IDX_DATETIME	3

/*
	CUrlHistoryTable
*/
class CUrlHistoryTable : public CBase
{
public:
	CUrlHistoryTable(LPCSTR lpcszTableName = NULL,LPCSTR lpcszDataPath = NULL,BOOL bOpenTable = TRUE);
	virtual ~CUrlHistoryTable() {}

	// derivate virtuali
	inline const char*		GetTableName		(void)				{return(m_szTableName);}
	inline const char*		GetTablePath		(void)				{return(m_szTablePath);}
	inline int			GetRecordLength	(void)				{return(URL_HISTORY_RECORD_LENGTH);}
	const char*			GetRecordAsString	(void);
	inline void			ResetMemvars		(void)				{memset(&record,'\0',sizeof(record));}
	void					GatherMemvars		(void);
	void					ScatterMemvars		(BOOL = TRUE);

	// campi
	inline int			GetField_Id		(void) const			{return(record.id);}
	inline const char*		GetField_Url		(void) const			{return(record.url);}
	inline const char*		GetField_DateTime	(void) const			{return(record.datetime);}

	// campi
	inline void			PutField_Id		(int value)			{record.id = value;}
	inline void			PutField_Url		(const char* value)		{strcpyn(record.url,value,sizeof(record.url));}
	inline void			PutField_DateTime	(const char* value)		{strcpyn(record.datetime,value,sizeof(record.datetime));}

private:
	// definizione del record
	struct RECORD {
		int			id;
		char			url[URL_HISTORY_URL_LEN+1];
		char			datetime[URL_HISTORY_DATETIME_LEN+1];
	};

	CBASE_TABLE*		table_struct;
	CBASE_INDEX*		idx_struct;
	
	RECORD			record;
	char				m_szRecord[URL_HISTORY_RECORD_LENGTH+1];
	
	char				m_szTable[_MAX_PATH+1];
	char				m_szTableName[_MAX_PATH+1];
	char				m_szTablePath[_MAX_FILEPATH+1];
	
	char				m_szIndexId[_MAX_FILEPATH+1];
	char				m_szIndexUrl[_MAX_FILEPATH+1];
	char				m_szIndexDateTime[_MAX_FILEPATH+1];
};

#endif // _CURLHISTORY_H
