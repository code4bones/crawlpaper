/*
	CLogDatabase.h
	Implementazione della classe relativa alle tabella.
	Luca Piergentili, 26/06/00
	lpiergentili@yahoo.com
*/
#ifndef _CLOGDATABASE_H
#define _CLOGDATABASE_H 1

#include "macro.h"
#include "typedef.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "strings.h"
#include "window.h"
#include "win32api.h"
#include "CBase.h"
#include "CHttp.h"
#include "CUrl.h"

// tabella
#define LOG_TABLE			"logtable"
#define LOG_IP_LEN			MAX_URL
#define LOG_DATE_LEN		32
#define LOG_GET_LEN			MAX_URL
#define LOG_CODE_LEN		5
#define LOG_SIZE_LEN		10
#define LOG_REFERER_LEN		MAX_URL
#define LOG_USERAGENT_LEN	MAX_URL
#define LOG_RECORD_LENGTH	(LOG_IP_LEN + LOG_DATE_LEN + LOG_GET_LEN + LOG_CODE_LEN + LOG_SIZE_LEN + LOG_REFERER_LEN + LOG_USERAGENT_LEN)

// indici
#define LOG_IDX_IP			0
#define LOG_IDX_GET			1
#define LOG_IDX_REFERER		2
#define LOG_IDX_USERAGENT	3
#define LOG_IDX_DATE		4

/*
	CLogTable
*/
class CLogTable : public CBase
{
public:
	CLogTable(LPCSTR lpcszTableName = NULL,LPCSTR lpcszDataPath = NULL,BOOL bOpenTable = TRUE);
	virtual ~CLogTable();

	// derivate virtuali
	inline const char*	GetTableName		(void)				{return(m_szTableName);}
	inline const char*	GetTablePath		(void)				{return(m_szTablePath);}
	inline int		GetRecordLength	(void)				{return(LOG_RECORD_LENGTH);}
	const char*		GetRecordAsString	(void);
	inline void		ResetMemvars		(void)				{memset(&record,'\0',sizeof(record));}
	void				GatherMemvars		(void);
	void				ScatterMemvars		(BOOL = TRUE);

	// campi
	inline const char*	GetField_Ip		(void) const			{return(record.ip);}
	inline const char*	GetField_Date		(void) const			{return(record.date);}
	inline const char*	GetField_Get		(void) const			{return(record.get);}
	inline int		GetField_Code		(void) const			{return(record.code);}
	inline long		GetField_Size		(void) const			{return(record.size);}
	inline const char*	GetField_Referer	(void) const			{return(record.referer);}
	inline const char*	GetField_UserAgent	(void) const			{return(record.useragent);}

	// campi
	inline void		PutField_Ip		(const char* value)		{strcpyn(record.ip,value,sizeof(record.ip));}
	inline void		PutField_Date		(const char* value)		{strcpyn(record.date,value,sizeof(record.date));}
	inline void		PutField_Get		(const char* value)		{strcpyn(record.get,value,sizeof(record.get));}
	inline void		PutField_Code		(int value)			{record.code = value;}
	inline void		PutField_Size		(long value)			{record.size = value;}
	inline void		PutField_Referer	(const char* value)		{strcpyn(record.referer,value,sizeof(record.referer));}
	inline void		PutField_UserAgent	(const char* value)		{strcpyn(record.useragent,value,sizeof(record.useragent));}

private:
	struct RECORD {
		char	ip[LOG_IP_LEN+1];
		char	date[LOG_DATE_LEN+1];
		char	get[LOG_GET_LEN+1];
		int	code;
		long	size;
		char	referer[LOG_REFERER_LEN+1];
		char	useragent[LOG_USERAGENT_LEN+1];
	};

	CBASE_TABLE*	table_struct;
	CBASE_INDEX*	idx_struct;
	RECORD		record;
	char			m_szRecord[LOG_RECORD_LENGTH+1];
	char			m_szTable[_MAX_PATH+1];
	char			m_szTableName[_MAX_PATH+1];
	char			m_szTablePath[_MAX_PATH+_MAX_FNAME+1];
	char			m_szIndexIp[_MAX_PATH+_MAX_FNAME+1];
	char			m_szIndexGet[_MAX_PATH+_MAX_FNAME+1];
	char			m_szIndexReferer[_MAX_PATH+_MAX_FNAME+1];
	char			m_szIndexUserAgent[_MAX_PATH+_MAX_FNAME+1];
	char			m_szIndexDate[_MAX_PATH+_MAX_FNAME+1];
};

// tabella
#define REPORT_TABLE		"reporttable"
#define REPORT_TOT_LEN		10
#define REPORT_RATE_LEN		10
#define REPORT_DATA_LEN		1023
#define REPORT_DATE_LEN		32
#define REPORT_RECORD_LENGTH	(REPORT_TOT_LEN + REPORT_RATE_LEN + REPORT_DATA_LEN + REPORT_DATE_LEN)

// indici
#define REPORT_IDX_TOT		0
#define REPORT_IDX_RATE		1
#define REPORT_IDX_DATA		2
#define REPORT_IDX_DATE		3

/*
	CReportTable
*/
class CReportTable : public CBase
{
public:
	CReportTable(LPCSTR lpcszTableName = NULL,LPCSTR lpcszDataPath = NULL,BOOL bOpenTable = TRUE);
	virtual ~CReportTable();

	// derivate virtuali
	inline const char*	GetTableName		(void)			{return(m_szTableName);}
	inline const char*	GetTablePath		(void)			{return(m_szTablePath);}
	inline int		GetRecordLength	(void)			{return(REPORT_RECORD_LENGTH);}
	const char*		GetRecordAsString	(void);
	inline void		ResetMemvars		(void)			{memset(&record,'\0',sizeof(record));}
	void				GatherMemvars		(void);
	void				ScatterMemvars		(BOOL = TRUE);

	// campi
	inline long		GetField_Tot		(void) const		{return(record.tot);}
	inline float		GetField_Rate		(void) const		{return((float)atof(record.rate));}
	inline const char*	GetField_Data		(void) const		{return(record.data);}
	inline const char*	GetField_Date		(void) const		{return(record.date);}

	// campi
	inline void		PutField_Tot		(const long value)	{record.tot = value;}
	inline void		PutField_Rate		(const float value)	{_snprintf(record.rate,sizeof(record.rate)-1,"%0.5f",value);}
	inline void		PutField_Data		(const char* value)	{strcpyn(record.data,value,sizeof(record.data));}
	inline void		PutField_Date		(const char* value)	{strcpyn(record.date,value,sizeof(record.date));}

private:
	// definizione del record
	struct RECORD {
		long		tot;
		char		rate[REPORT_RATE_LEN+1];
		char		data[REPORT_DATA_LEN+1];
		char		date[REPORT_DATE_LEN+1];
	};

	CBASE_TABLE*	table_struct;
	CBASE_INDEX*	idx_struct;
	RECORD		record;
	char			m_szRecord[REPORT_RECORD_LENGTH+1];
	char			m_szTable[_MAX_PATH+1];
	char			m_szTableName[_MAX_PATH+1];
	char			m_szTablePath[_MAX_PATH+_MAX_FNAME+1];
	char			m_szIndexTot[_MAX_PATH+_MAX_FNAME+1];
	char			m_szIndexRate[_MAX_PATH+_MAX_FNAME+1];
	char			m_szIndexData[_MAX_PATH+_MAX_FNAME+1];
	char			m_szIndexDate[_MAX_PATH+_MAX_FNAME+1];
};

#endif // _CLOGDATABASE_H
