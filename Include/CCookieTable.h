/*
	CCookieTable.h
	Implementazione delle classi per la definizione della tabella.
	Luca Piergentili, 30/10/03
	lpiergentili@yahoo.com
*/
#ifndef _CCOOKIETABLE_H
#define _CCOOKIETABLE_H 1

#include "macro.h"
#include "typedef.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "strings.h"
#include "CBase.h"
#include "CUrl.h"
#include "CHttp.h"

// tabella
#define COOKIEDATA_TABLE			"cookies"
#define COOKIEDATA_NAME_LEN		COOKIE_NAME_LEN
#define COOKIEDATA_VALUE_LEN		COOKIE_VALUE_LEN
#define COOKIEDATA_EXPIRES_LEN	COOKIE_EXPIRES_LEN
#define COOKIEDATA_PATH_LEN		COOKIE_PATH_LEN
#define COOKIEDATA_DOMAIN_LEN		COOKIE_DOMAIN_LEN
#define COOKIEDATA_SECURE_LEN		FIELD_INT_MAXSIZE
#define COOKIEDATA_RECORD_LENGTH	(COOKIEDATA_NAME_LEN + COOKIEDATA_VALUE_LEN + COOKIEDATA_EXPIRES_LEN + COOKIEDATA_PATH_LEN + COOKIEDATA_DOMAIN_LEN + COOKIEDATA_SECURE_LEN)

// indici
#define COOKIEDATA_IDX_NAME		0
#define COOKIEDATA_IDX_DOMAIN		1

/*
	CCookieTable
*/
class CCookieTable : public CBase
{
public:
	CCookieTable(LPCSTR lpcszTableName = NULL,LPCSTR lpcszDataPath = NULL,BOOL bOpenTable = TRUE);
	virtual ~CCookieTable() {}

	// derivate virtuali
	inline const char*		GetTableName		(void)				{return(m_szTableName);}
	inline const char*		GetTablePath		(void)				{return(m_szTablePath);}
	inline int			GetRecordLength	(void)				{return(COOKIEDATA_RECORD_LENGTH);}
	const char*			GetRecordAsString	(void);
	inline void			ResetMemvars		(void)				{memset(&record,'\0',sizeof(record));}
	void					GatherMemvars		(void);
	void					ScatterMemvars		(BOOL = TRUE);

	// campi
	inline const char*		GetField_Name		(void) const			{return(record.name);}
	inline const char*		GetField_Value		(void) const			{return(record.value);}
	inline const char*		GetField_Expires	(void) const			{return(record.expires);}
	inline const char*		GetField_Path		(void) const			{return(record.path);}
	inline const char*		GetField_Domain	(void) const			{return(record.domain);}
	inline int			GetField_Secure	(void) const			{return(record.secure);}

	// campi
	inline void			PutField_Name		(const char* value)		{strcpyn(record.name,value,sizeof(record.name));}
	inline void			PutField_Value		(const char* value)		{strcpyn(record.value,value,sizeof(record.value));}
	inline void			PutField_Expires	(const char* value)		{strcpyn(record.expires,value,sizeof(record.expires));}
	inline void			PutField_Path		(const char* value)		{strcpyn(record.path,value,sizeof(record.path));}
	inline void			PutField_Domain	(const char* value)		{strcpyn(record.domain,value,sizeof(record.domain));}
	inline void			PutField_Secure	(int value)			{record.secure = value;}

private:
	// definizione del record
	struct RECORD {
		char			name[COOKIEDATA_NAME_LEN+1];
		char			value[COOKIEDATA_VALUE_LEN+1];
		char			expires[COOKIEDATA_EXPIRES_LEN+1];
		char			path[COOKIEDATA_PATH_LEN+1];
		char			domain[COOKIEDATA_DOMAIN_LEN+1];
		int			secure;
	};

	CBASE_TABLE*		table_struct;
	CBASE_INDEX*		idx_struct;
	
	RECORD			record;
	char				m_szRecord[COOKIEDATA_RECORD_LENGTH+1];
	
	char				m_szTable[_MAX_PATH+1];
	char				m_szTableName[_MAX_PATH+1];
	char				m_szTablePath[_MAX_FILEPATH+1];
	
	char				m_szIndexName[_MAX_FILEPATH+1];
	char				m_szIndexDomain[_MAX_FILEPATH+1];
};

#endif // _CCOOKIETABLE_H
