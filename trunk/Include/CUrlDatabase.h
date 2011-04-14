/*
	CUrlDatabase.h
	Implementazione delle classi per la definizione delle tabelle.
	Luca Piergentili, 26/06/00
	lpiergentili@yahoo.com
*/
#ifndef _CURLDATABASE_H
#define _CURLDATABASE_H 1

#include "macro.h"
#include "typedef.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "strings.h"
#include "CBase.h"
#include "CUrl.h"

// tabella
#define URL_TABLE			"urltable"
#define URL_ID_LEN			FIELD_INT_MAXSIZE
#define URL_PARENTURL_LEN	MAX_URL
#define URL_URL_LEN			MAX_URL
#define URL_STAT_LEN		FIELD_SHORTINT_MAXSIZE
#define URL_FILE_LEN		_MAX_PATH
#define URL_SIZE_LEN		FIELD_REAL_MAXSIZE
#define URL_SECONDS_LEN		FIELD_REAL_MAXSIZE
#define URL_RECORD_LENGTH	(URL_ID_LEN + URL_PARENTURL_LEN + URL_URL_LEN + URL_STAT_LEN + URL_FILE_LEN + URL_SIZE_LEN + URL_SECONDS_LEN)

// indici
#define URL_IDX_ID			0
#define URL_IDX_PARENTURL	1
#define URL_IDX_URL			2
#define URL_IDX_FILE		3
#define URL_IDX_STAT		4

/*
	CUrlTable
*/
class CUrlTable : public CBase
{
public:
	CUrlTable(LPCSTR lpcszTableName = NULL,LPCSTR lpcszDataPath = NULL,BOOL bOpenTable = TRUE);
	virtual ~CUrlTable() {}

	// derivate virtuali
	inline const char*		GetTableName		(void)				{return(m_szTableName);}
	inline const char*		GetTablePath		(void)				{return(m_szTablePath);}
	inline int			GetRecordLength	(void)				{return(URL_RECORD_LENGTH);}
	const char*			GetRecordAsString	(void);
	inline void			ResetMemvars		(void)				{memset(&record,'\0',sizeof(record));}
	void					GatherMemvars		(void);
	void					ScatterMemvars		(BOOL = TRUE);

	// campi
	inline int			GetField_Id		(void) const			{return(record.id);}
	inline const char*		GetField_ParentUrl	(void) const			{return(record.parenturl);}
	inline const char*		GetField_Url		(void) const			{return(record.url);}
	inline short int		GetField_Stat		(void) const			{return(record.stat);}
	inline const char*		GetField_File		(void) const			{return(record.file);}
	inline double			GetField_Size		(void) const			{return(record.size);}
	inline double			GetField_Seconds	(void) const			{return(record.seconds);}

	// campi
	inline void			PutField_Id		(int value)			{record.id = value;}
	inline void			PutField_ParentUrl	(const char* value)		{strcpyn(record.parenturl,value,sizeof(record.parenturl));}
	inline void			PutField_Url		(const char* value)		{strcpyn(record.url,value,sizeof(record.url));}
	inline void			PutField_Stat		(short int value)		{record.stat = value;}
	inline void			PutField_File		(const char* value)		{strcpyn(record.file,value,sizeof(record.file));}
	inline void			PutField_Size		(double value)			{record.size = value;}
	inline void			PutField_Seconds	(double value)			{record.seconds = value;}

private:
	// definizione del record
	struct RECORD {
		int			id;							// progressivo unico per l'url
		char			parenturl[URL_PARENTURL_LEN+1];	// parent url
		char			url[URL_URL_LEN+1];				// url
		short int		stat;						// status
		char			file[URL_FILE_LEN+1];			// file locale
		double		size;						// dimensione dell'oggetto
		double		seconds;						// durata download
	};

	CBASE_TABLE*		table_struct;
	CBASE_INDEX*		idx_struct;
	
	RECORD			record;
	char				m_szRecord[URL_RECORD_LENGTH+1];
	
	char				m_szTable[_MAX_PATH+1];
	char				m_szTableName[_MAX_PATH+1];
	char				m_szTablePath[_MAX_FILEPATH+1];
	
	char				m_szIndexId[_MAX_FILEPATH+1];
	char				m_szIndexParentUrl[_MAX_FILEPATH+1];
	char				m_szIndexUrl[_MAX_FILEPATH+1];
	char				m_szIndexFile[_MAX_FILEPATH+1];
	char				m_szIndexStat[_MAX_FILEPATH+1];
};

// tabella
#define URLDATABASE_TABLE			"urldatabase"
#define URLDATABASE_URL_LEN			MAX_URL
#define URLDATABASE_DATE_LEN			FIELD_DATE_MAXSIZE
#define URLDATABASE_STARTTIME_LEN		FIELD_TIME_MAXSIZE
#define URLDATABASE_ENDTIME_LEN		FIELD_TIME_MAXSIZE
#define URLDATABASE_TOTALTIME_LEN		FIELD_REAL_MAXSIZE
#define URLDATABASE_DOWNLOADTIME_LEN	FIELD_REAL_MAXSIZE
#define URLDATABASE_DOWNLOADED_LEN		FIELD_REAL_MAXSIZE
#define URLDATABASE_TOTALURLS_LEN		FIELD_INT_MAXSIZE
#define URLDATABASE_TOTALPICT_LEN		FIELD_INT_MAXSIZE
#define URLDATABASE_DOWNLOADEDURLS_LEN	FIELD_INT_MAXSIZE
#define URLDATABASE_DOWNLOADEDPICT_LEN	FIELD_INT_MAXSIZE
#define URLDATABASE_DATATABLE_LEN		_MAX_PATH
#define URLDATABASE_REPORT_LEN		_MAX_PATH
#define URLDATABASE_STATUS_LEN		FIELD_SHORTINT_MAXSIZE
#define URLDATABASE_ID_LEN			FIELD_INT_MAXSIZE
#define URLDATABASE_RECORD_LENGTH		(URLDATABASE_URL_LEN+URLDATABASE_DATE_LEN+URLDATABASE_STARTTIME_LEN+URLDATABASE_ENDTIME_LEN+URLDATABASE_TOTALTIME_LEN+URLDATABASE_DOWNLOADTIME_LEN+URLDATABASE_DOWNLOADED_LEN+URLDATABASE_TOTALURLS_LEN+URLDATABASE_TOTALPICT_LEN+URLDATABASE_DOWNLOADEDURLS_LEN+URLDATABASE_DOWNLOADEDPICT_LEN+URLDATABASE_DATATABLE_LEN+URLDATABASE_REPORT_LEN+URLDATABASE_STATUS_LEN+URLDATABASE_ID_LEN)

// indici
#define URLDATABASE_IDX_URL	0
#define URLDATABASE_IDX_DATE	1

/*
	CUrlDatabaseTable
*/
class CUrlDatabaseTable : public CBase
{
public:
	CUrlDatabaseTable(LPCSTR lpcszTableName = NULL,LPCSTR lpcszDataPath = NULL,BOOL bOpenTable = TRUE);
	virtual ~CUrlDatabaseTable() {}

	// derivate virtuali
	inline const char*		GetTableName			(void)				{return(m_szTableName);}
	inline const char*		GetTablePath			(void)				{return(m_szTablePath);}
	inline int			GetRecordLength		(void)				{return(URLDATABASE_RECORD_LENGTH);}
	const char*			GetRecordAsString		(void);
	void					ResetMemvars			(void);
	void					GatherMemvars			(void);
	void					ScatterMemvars			(BOOL = TRUE);

	// campi
	inline const char*		GetField_Url			(void)				{return(record.url);}
	inline CDateTime*		GetField_Date			(void)				{return(&(record.date));}
	inline CDateTime*		GetField_StartTime		(void)				{return(&(record.starttime));}
	inline CDateTime*		GetField_EndTime		(void)				{return(&(record.endtime));}
	inline double			GetField_TotalTime		(void)				{return(record.totaltime);}
	inline double			GetField_DownloadTime	(void)				{return(record.downloadtime);}
	inline double			GetField_Downloaded		(void)				{return(record.downloaded);}
	inline int			GetField_TotalUrls		(void)				{return(record.totalurls);}
	inline int			GetField_TotalPict		(void)				{return(record.totalpict);}
	inline int			GetField_DownloadedUrls	(void)				{return(record.downloadedurls);}
	inline int			GetField_DownloadedPict	(void)				{return(record.downloadedpict);}
	inline const char*		GetField_DataTable		(void)				{return(record.datatable);}
	inline const char*		GetField_Report		(void)				{return(record.report);}
	inline short int		GetField_Status		(void)				{return(record.status);}
	inline int			GetField_Id			(void)				{return(record.id);}

	// campi
	inline void			PutField_Url			(const char* value)		{strcpyn(record.url,value,sizeof(record.url));}
	inline void			PutField_Date			(const CDateTime* value)	{record.date.SetYear(value->GetYear()); record.date.SetMonth(value->GetMonth()); record.date.SetDay(value->GetDay());}
	inline void			PutField_StartTime		(const CDateTime* value)	{record.starttime.SetHour(value->GetHour()); record.starttime.SetMin(value->GetMin()); record.starttime.SetSec(value->GetSec());}
	inline void			PutField_EndTime		(const CDateTime* value)	{record.endtime.SetHour(value->GetHour()); record.endtime.SetMin(value->GetMin()); record.endtime.SetSec(value->GetSec());}
	inline void			PutField_TotalTime		(const double value)	{record.totaltime = value;}
	inline void			PutField_DownloadTime	(const double value)	{record.downloadtime = value;}
	inline void			PutField_Downloaded		(const double value)	{record.downloaded = value;}
	inline void			PutField_TotalUrls		(const int value)		{record.totalurls = value;}
	inline void			PutField_TotalPict		(const int value)		{record.totalpict = value;}
	inline void			PutField_DownloadedUrls	(const int value)		{record.downloadedurls = value;}
	inline void			PutField_DownloadedItems	(const int value)		{record.downloadedpict = value;}
	inline void			PutField_DataTable		(const char* value)		{strcpyn(record.datatable,value,sizeof(record.datatable));}
	inline void			PutField_Report		(const char* value)		{strcpyn(record.report,value,sizeof(record.report));}
	inline void			PutField_Status		(const short int value)	{record.status = value;}
	inline void			PutField_Id			(const int value)		{record.id = value;}

private:
	// definizione del record
	struct RECORD {
		char			url[URLDATABASE_URL_LEN+1];				// url
		CDateTime		date;								// data 
		CDateTime		starttime;							// ora inizio
		CDateTime		endtime;								// ora fine
		double		totaltime;							// durata
		double		downloadtime;							// durata download
		double		downloaded;							// bytes scaricati
		int			totalurls;							// tot url trovate
		int			totalpict;							// tot immagini trovate
		int			downloadedurls;						// tot url scaricate
		int			downloadedpict;						// tot immagini scaricate
		char			datatable[URLDATABASE_DATATABLE_LEN+1];		// nome tabella relativa
		char			report[URLDATABASE_REPORT_LEN+1];			// nome report relativo
		short int		status;								// status
		int			id;									// ultimo id per download interrotto
	};

	CBASE_TABLE*		table_struct;
	CBASE_INDEX*		idx_struct;
	
	RECORD			record;
	char				m_szRecord[URLDATABASE_RECORD_LENGTH+1];
	
	char				m_szTable[_MAX_PATH+1];
	char				m_szTableName[_MAX_PATH+1];
	char				m_szTablePath[_MAX_FILEPATH+1];
	
	char				m_szIndexUrl[_MAX_FILEPATH+1];
	char				m_szIndexDate[_MAX_FILEPATH+1];
};

#endif // _CURLDATABASE_H
