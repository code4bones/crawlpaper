/*
	CLogDatabaseService.h
	Implementazione della classe relativa alle operazioni sulla tabella.
	Luca Piergentili, 02/06/00
	lpiergentili@yahoo.com
*/
#ifndef _CURLDATABASESERVICE_H
#define _CURLDATABASESERVICE_H 1

#include "typedef.h"
#include <stdlib.h>
#include "strings.h"
#include "window.h"
#include "CDateTime.h"
#include "CUrl.h"
#include "CLogDatabase.h"

/*
	CLogDatabaseService
*/
class CLogDatabaseService
{
public:
	CLogDatabaseService				(
								LPCSTR lpcszTableName = NULL,
								LPCSTR lpcszDataPath = NULL
								);
	virtual ~CLogDatabaseService		();
	
	inline CLogTable* GetTable		(void) {return(m_pLogTable);}

	BOOL			Insert			(
								LPCSTR lpcszIp,
								LPCSTR lpcszDate,
								LPCSTR lpcszGet,
								UINT nCode,
								UINT nSize,
								LPCSTR lpcszReferer,
								LPCSTR lpcszUserAgent
								);
	
	UINT			Count			(void);

protected:
	BOOL			LockTable			(void);
	void			UnlockTable		(void);

	BOOL			m_bOpened;
	CLogTable*	m_pLogTable;

private:
	CDateTime		m_DateTime;
	char			m_szUrl[MAX_URL+1];
	char			m_szParentUrl[MAX_URL+1];
};

/*
	CReportDatabaseService
*/
class CReportDatabaseService
{
public:
	CReportDatabaseService			(
								LPCSTR lpcszTableName = NULL,
								LPCSTR lpcszDataPath = NULL
								);
	virtual ~CReportDatabaseService	();
	
	inline CReportTable* GetTable		(void) {return(m_ReportTable);}

	BOOL			Insert			(
								long nTot,
								long nTotal,
								LPCSTR lpcszData
								);

protected:
	BOOL			LockTable			(void);
	void			UnlockTable		(void);

	BOOL			m_bOpened;
	CReportTable*	m_ReportTable;

private:
	CDateTime		m_DateTime;
	char			m_szUrl[MAX_URL+1];
	char			m_szParentUrl[MAX_URL+1];
};

#endif // _CURLDATABASESERVICE_H
