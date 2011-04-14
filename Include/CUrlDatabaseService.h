/*
	CUrlDatabaseService.h
	Implementazione delle classi per l'accesso alle tabelle.
	Luca Piergentili, 02/06/00
	lpiergentili@yahoo.com
*/
#ifndef _CURLDATABASESERVICE_H
#define _CURLDATABASESERVICE_H 1

#include "typedef.h"
#include <stdlib.h>
#include "strings.h"
#include "window.h"
#include "CSync.h"
#include "CDateTime.h"
#include "CUrl.h"
#include "CUrlDatabase.h"

/*
	CUrlService
	tabella per i link relativi all'url (elenco dei link presenti nel sito relativo all'url)
*/
class CUrlService
{
public:
	CUrlService(LPCSTR lpcszTableName = NULL,LPCSTR lpcszDataPath = NULL);
	virtual ~CUrlService();
	
	CUrlTable*	GetTable		(void) const;

	inline BOOL	IsValid		(void) {return(m_bIsValid);}
	
	BOOL			Insert		(
							UINT&				nID,
							LPCSTR				lpcszParentUrl,
							LPCSTR				lpcszUrl,
							CUrlStatus::URL_STATUS	nStat = CUrlStatus::URL_STATUS_UNKNOWN,
							LPCSTR				lpcszFile = NULL,
							double				dlSize = (double)-1.0f,
							double				dlTotalTime = 0.0f
							);
	
	BOOL			Update		(
							LPCSTR				lpcszUrl,
							CUrlStatus::URL_STATUS	nStat = CUrlStatus::URL_STATUS_UNKNOWN,
							LPCSTR				lpcszFile = NULL,
							double				dlSize = (double)-1.0f,
							double				dlTotalTime = 0.0f
							);
	
	LPCSTR		Get			(
							UINT					nID,
							LPSTR				lpszUrl,
							int					nUrlSize,
							LPSTR				lpszParentUrl,
							int					nParentUrlSize,
							CUrlStatus::URL_STATUS&	nStat
							);
	
	UINT			Count		(void);
	double		CountItems	(UINT& nPictures,UINT& nAudio);

	void			GetLastID		(UINT& nID);
	
	BOOL			Delete		(UINT nID);
	void			DeleteAll		(void);

protected:
	BOOL			m_bIsValid;
	CUrlTable*	m_pUrlTable;
	CSyncProcesses	m_mutexTable;

private:
	CUrl			m_Url;
	char			m_szUrl[MAX_URL+1];
	char			m_szParentUrl[MAX_URL+1];
};

/*
	CUrlDatabaseService
	tabella per le url (elenco interno dei siti scaricati)
*/
class CUrlDatabaseService
{
public:
	CUrlDatabaseService(LPCSTR lpcszTableName = NULL,LPCSTR lpcszDataPath = NULL);
	virtual ~CUrlDatabaseService();
	
	CUrlDatabaseTable* GetTable		(void) const;

	inline BOOL	IsValid			(void) {return(m_bIsValid);}

	BOOL			Exist			(
								LPCSTR				lpcszUrl,
								LPSTR				lpszDate,
								UINT					nDateSize,
								CUrlStatus::URL_STATUS&	nStat,
								UINT&				nID
								);

	BOOL			Insert			(
								LPCSTR lpcszUrl,
								LPCSTR lpcszStartTime,
								LPCSTR lpcszDataTable
								);

	BOOL			Update			(
								LPCSTR				lpcszUrl,
								LPCSTR				lpcszEndTime,
								double				dlTotalTime,
								double				dlDownloadTime,
								CUrlStatus::URL_STATUS	nStat,
								UINT					nID,
								LPCSTR				lpcszReportName,
								CUrlTable*			pHttptable
								);

protected:
	BOOL				m_bIsValid;
	CUrlDatabaseTable*	m_pUrlDatabaseTable;

private:
	CDateTime		m_DateTime;
	CUrl			m_Url;
	URL			m_stUrl;
	char			m_szUrl[MAX_URL+1];
	char			m_szParentUrl[MAX_URL+1];
};

#endif // _CURLDATABASESERVICE_H
