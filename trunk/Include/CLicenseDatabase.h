/*
	CLicenseDatabase.h
	Implementazione delle tabelle per il servizio delle licenze.
	La classe base del servizio (CLicenseService) deve fornire l'accesso ai dati alle
	derivate (CLicenseProtocol) attraverso le tabelle qui (CLicenseDatabase) definite.
	Luca Piergentili, 26/06/00
	lpiergentili@yahoo.com
*/
#ifndef _CLICENSEDATABASE_H
#define _CLICENSEDATABASE_H 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "strcpyn.h"
#include "strsetn.h"
#ifdef _WINDOWS
	#include "window.h"
	#ifdef WIN32_MFC
		#include "CMutexEx.h"
	#endif
#else
	#include "typedef.h"
#endif
#include "CBase.h"
#include "CTable.h"

/*
	labels.db
*/

// tabella
#define LABELS_TABLE		"labels.db"
#define LABELS_ID_LEN		5
#define LABELS_TEXT_LEN		256
#define LABELS_RECORD_LENGTH	(LABELS_ID_LEN+LABELS_TEXT_LEN)

// indici
#define LABELS_INDEX_ID		"labels.id.idx"
#define LABELS_IDX_ID		0

/*
	CLabelsTable
*/
class CLabelsTable : public CTable
{
private:
	// definizione del record
	struct RECORD {
		char	id	[LABELS_ID_LEN + 1];
		char	text	[LABELS_TEXT_LEN + 1];
	};

	char			table_name[_MAX_PATH + _MAX_FNAME + 1];
	char			idx_email[_MAX_PATH + _MAX_FNAME + 1];
	CBASE_TABLE*	table_struct;
	CBASE_INDEX*	idx_struct;
	RECORD		record;
	char			record_string[LABELS_RECORD_LENGTH + 1];

public:
	CLabelsTable(BOOL bOpenTable,const char* pPathname);
	virtual ~CLabelsTable();

	// derivate virtuali
	const char*		GetClassName		(void)			{return("CLabelsTable");}
	const char*		GetStaticTableName	(void)			{return(LABELS_TABLE);}
	const char*		GetTableName		(void)			{return(LABELS_TABLE);}
	const char*		GetTablePathName	(void)			{return(table_name);}
	const CBASE_TABLE*	GetTableStruct		(void)			{return(table_struct);}
	const CBASE_INDEX*	GetIndexStruct		(void)			{return(idx_struct);}
	const int			GetRecordLength	(void)			{return(LABELS_RECORD_LENGTH);}
	const char*		GetRecordAsString	(void);
	void				ResetMemvars		(void);
	void				GatherMemvars		(void);
	void				ScatterMemvars		(BOOL bTrim = TRUE);

	// campi
	inline const char*	GetField_Id		(void)			{return(record.id);}
	inline const char*	GetField_Text		(void)			{return(record.text);}

	// campi
	inline void		PutField_Id		(const char* value)	{strcpyn(record.id,value,LABELS_ID_LEN+1);}
	inline void		PutField_Text		(const char* value)	{strcpyn(record.text,value,LABELS_TEXT_LEN+1);}
};

#endif // _CLICENSEDATABASE_H
