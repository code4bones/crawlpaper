/*
	CUrlDatabaseService.cpp
	Implementazione delle classi per l'accesso alle tabelle.
	Luca Piergentili, 02/06/00
	lpiergentili@yahoo.com
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include "typedef.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "strings.h"
#include "window.h"
#include "CSync.h"
#include "CDateTime.h"
#include "CImage.h"
#include "CAudioPlayer.h"
#include "CUrl.h"
#include "CUrlDatabase.h"
#include "CUrlDatabaseService.h"

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
	CUrlService()
*/
CUrlService::CUrlService(LPCSTR lpcszTableName/* = NULL*/,LPCSTR lpcszDataPath/* = NULL*/)
{
	// tabella per i link estratti a partire dall'url

	// usa due lock, il primo sulla tabella (via classe derivata) per evitare piu' downloads
	// in contemporanea sulla stessa url, il secondo per sincronizzare gli accessi alla tabella
	// se il chiamante utilizza piu' threads 
	m_pUrlTable = new CUrlTable(lpcszTableName,lpcszDataPath);

	// primo lock (mutex per la tabella)
	// blocca per l'intera durata dell'oggetto
	m_bIsValid = m_pUrlTable ? m_pUrlTable->Lock() : FALSE;

	// secondo lock (mutex per la modifica del contenuto della tabella)
	// blocca solo durante l'accesso
	if(m_bIsValid)
	{
		char szMutexName[_MAX_PATH+1];
		_snprintf(szMutexName,sizeof(szMutexName)-1,"%s\\%sAccessMutex",lpcszDataPath,lpcszTableName);
		m_mutexTable.SetName(szMutexName);
		m_mutexTable.SetTimeout(SYNC_10_SECS_TIMEOUT);
	}
}

/*
	~CUrlService()
*/
CUrlService::~CUrlService()
{
	// sblocca il lock (quello sul mutex) e chiude la tabella
	if(m_pUrlTable)
	{
		m_pUrlTable->Unlock();
		m_pUrlTable->Close();
		delete m_pUrlTable,m_pUrlTable = (CUrlTable*)NULL;
	}
}

/*
	GetTable()
*/
CUrlTable* CUrlService::GetTable(void) const
{
	return(m_pUrlTable);
}

/*
	Insert()
*/
BOOL CUrlService::Insert(UINT&	nID,
					LPCSTR	lpcszParentUrl,
					LPCSTR	lpcszUrl,
					CUrlStatus::URL_STATUS nStat/* = CUrlStatus::URL_STATUS_UNKNOWN*/,
					LPCSTR	lpcszFile/* = NULL*/,
					double	dlSize/* = (double)-1.0f*/,
					double	dlTotalTime/* = 0.0f*/
					)
{
	BOOL bInserted = FALSE;

	if(m_bIsValid)
	{
		// sincronizza l'accesso alla tabella
		if(m_mutexTable.Lock())
		{
			// decodifica le url
			strcpyn(m_szUrl,lpcszUrl,sizeof(m_szUrl));
			m_Url.DecodeUrl(m_szUrl);
			
			strcpyn(m_szParentUrl,lpcszParentUrl,sizeof(m_szParentUrl));
			m_Url.DecodeUrl(m_szParentUrl);

			// inserisce solo se gia' non esiste
			if(!m_pUrlTable->Seek(m_szUrl,URL_IDX_URL))
			{
				m_pUrlTable->ResetMemvars();
				
				m_pUrlTable->PutField_Id			(	++nID						);
				m_pUrlTable->PutField_ParentUrl	(	m_szParentUrl					);
				m_pUrlTable->PutField_Url		(	m_szUrl						);
				m_pUrlTable->PutField_Stat		(	(short int)nStat				);
				m_pUrlTable->PutField_File		(	lpcszFile ? lpcszFile : ""		);
				m_pUrlTable->PutField_Size		(	dlSize!=(double)-1.0f ? dlSize : 0	);
				m_pUrlTable->PutField_Seconds		(	dlTotalTime					);
				
				m_pUrlTable->GatherMemvars();
				
				bInserted = m_pUrlTable->Insert();
			}

			m_mutexTable.Unlock();
		}
	}

	return(bInserted);
}

/*
	Update()
*/
BOOL CUrlService::Update(LPCSTR	lpcszUrl,
					CUrlStatus::URL_STATUS nStat/* = CUrlStatus::URL_STATUS_UNKNOWN*/,
					LPCSTR	lpcszFile/* = NULL*/,
					double	dlSize/* = (double)-1.0f*/,
					double	dlTotalTime/* = 0.0f*/
					)
{
	BOOL bUpdated = FALSE;
	BOOL bFound = FALSE;
	UINT nID = 0L;
	char szParentUrl[URL_URL_LEN+1];
	char szUrl[URL_URL_LEN+1];
	char	szFile[URL_FILE_LEN+1];
	double dlTableSize = 0.0f;
	double dlTableSecs = 0.0f;

	if(m_bIsValid)
	{
		// sincronizza l'accesso alla tabella
		TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CUrlService::Update(): lock...\n"));
		if(m_mutexTable.Lock())
		{	
			strcpyn(m_szUrl,lpcszUrl,sizeof(m_szUrl));

			// se il valore gia' esiste lo elimina
			TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CUrlService::Update(): seek...\n"));
			if(m_pUrlTable->Seek(m_szUrl,URL_IDX_URL))
			{
				m_pUrlTable->ScatterMemvars();
				
				nID = m_pUrlTable->GetField_Id();
				strcpyn(szParentUrl,m_pUrlTable->GetField_ParentUrl(),sizeof(szParentUrl));
				strcpyn(szUrl,m_pUrlTable->GetField_Url(),sizeof(szUrl));
				strcpyn(szFile,m_pUrlTable->GetField_File(),sizeof(szFile));
				dlTableSize = m_pUrlTable->GetField_Size();
				dlTableSecs = m_pUrlTable->GetField_Seconds();
				
				m_pUrlTable->Delete();
				
				bFound = TRUE;
			}

			// inserisce
			if(bFound)
			{
				TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CUrlService::Update(): insert...\n"));
				
				m_pUrlTable->ResetMemvars();
				
				m_pUrlTable->PutField_Id			(	nID									);
				m_pUrlTable->PutField_ParentUrl	(	szParentUrl							);
				m_pUrlTable->PutField_Url		(	szUrl								);
				m_pUrlTable->PutField_Stat		(	(short int)nStat						);
				m_pUrlTable->PutField_File		(	lpcszFile ? lpcszFile : szFile			);
				m_pUrlTable->PutField_Size		(	dlSize!=(double)-1.0f ? dlSize : dlTableSize	);
				m_pUrlTable->PutField_Seconds		(	dlTotalTime!=0.0f ? dlTotalTime : dlTableSecs);
				
				m_pUrlTable->GatherMemvars();
				
				bUpdated = m_pUrlTable->Insert();
			}

			TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CUrlService::Update(): unlock...\n"));
			m_mutexTable.Unlock();
		}
		else
		{
			TRACEEXPR((_TRACE_FLAG_ERR,__NOFILE__,__NOLINE__,"CUrlService::Update(): lock failed\n"));
		}
	}
	
	return(bUpdated);
}

/*
	Get()
*/
LPCSTR CUrlService::Get(	UINT		nID,
					LPSTR	lpszUrl,
					int		nUrlSize,
					LPSTR	lpszParentUrl,
					int		nParentUrlSize,
					CUrlStatus::URL_STATUS& nStat
					)
{
	char* p = NULL;
	nStat = CUrlStatus::URL_STATUS_UNKNOWN;
	memset(lpszUrl,'\0',nUrlSize);
	memset(lpszParentUrl,'\0',nParentUrlSize);

	if(m_bIsValid)
	{
		// sincronizza l'accesso alla tabella
		if(m_mutexTable.Lock())
		{
			// cerca il valore richesto
			if(m_pUrlTable->Seek((int)nID,URL_IDX_ID))
			{
				m_pUrlTable->ScatterMemvars();
				
				strcpyn(lpszUrl,(char*)m_pUrlTable->GetField_Url(),nUrlSize);
				if(!strempty(lpszUrl))
				{
					strcpyn(lpszParentUrl,(char*)m_pUrlTable->GetField_ParentUrl(),nParentUrlSize);
					p = lpszUrl;
					nStat = (CUrlStatus::URL_STATUS)m_pUrlTable->GetField_Stat();
				}
			}

			m_mutexTable.Unlock();
		}
	}

	return(p);
}

/*
	Count()
*/
UINT CUrlService::Count(void)
{
	UINT nTot = 0;

	if(m_bIsValid)
	{
		// sincronizza l'accesso alla tabella
		if(m_mutexTable.Lock())
		{
			m_pUrlTable->SetIndex(URL_IDX_ID);
			
			if(m_pUrlTable->GoBottom())
			{
				m_pUrlTable->ScatterMemvars();
				nTot = m_pUrlTable->GetField_Id();
			}

			m_mutexTable.Unlock();
		}
	}

	return(nTot);
}

/*
	CountItems()
*/
double CUrlService::CountItems(UINT& nPictures,UINT& nAudio)
{
	double dlTotBytes = 0.0f;
	nPictures = nAudio = 0L;

	if(m_bIsValid)
	{
		// sincronizza l'accesso alla tabella
		if(m_mutexTable.Lock())
		{
			m_pUrlTable->SetIndex(URL_IDX_ID);

			if(m_pUrlTable->GoTop())
				do
				{
					m_pUrlTable->ScatterMemvars();
					
					dlTotBytes += m_pUrlTable->GetField_Size();
					
					if(CImage::IsImageFile(m_pUrlTable->GetField_Url()))
						nPictures++;
					else if(CAudioPlayer::IsAudioFile(m_pUrlTable->GetField_Url()))
						 nAudio++;

					m_pUrlTable->GetNext();
				
				} while(!m_pUrlTable->Eof());

			m_mutexTable.Unlock();
		}
	}

	return(dlTotBytes);
}

/*
	GetLastID()
*/
void CUrlService::GetLastID(UINT& nID)
{
	nID = 0;
	CUrlStatus::URL_STATUS stat;

	if(m_bIsValid)
	{
		// sincronizza l'accesso alla tabella
		if(m_mutexTable.Lock())
		{
			m_pUrlTable->SetIndex(URL_IDX_ID);
			
			if(m_pUrlTable->GoTop())
				do
				{
					m_pUrlTable->ScatterMemvars();
					
					stat = (CUrlStatus::URL_STATUS)m_pUrlTable->GetField_Stat();
					if(stat==CUrlStatus::URL_STATUS_CANCELLED || stat==CUrlStatus::URL_STATUS_UNKNOWN)
					{
						nID = m_pUrlTable->GetField_Id();
						break;
					}

					m_pUrlTable->GetNext();
				
				} while(!m_pUrlTable->Eof());

			m_mutexTable.Unlock();
		}
	}
}

/*
	Delete()
*/
BOOL CUrlService::Delete(UINT nID)
{
	BOOL bDeleted = FALSE;

	if(m_bIsValid)
	{
		// sincronizza l'accesso alla tabella
		if(m_mutexTable.Lock())
		{
			if(m_pUrlTable->Seek((int)nID,URL_IDX_ID))
				bDeleted = m_pUrlTable->Delete();

			m_mutexTable.Unlock();
		}
	}

	return(bDeleted);
}

/*
	DeleteAll()
*/
void CUrlService::DeleteAll(void)
{
	if(m_bIsValid)
	{
		// sincronizza l'accesso alla tabella
		if(m_mutexTable.Lock())
		{
			m_pUrlTable->Zap();
			m_mutexTable.Unlock();
		}
	}
}

/*
	CUrlDatabaseService()
*/
CUrlDatabaseService::CUrlDatabaseService(LPCSTR lpcszTableName/*=NULL*/,LPCSTR lpcszDataPath/*=NULL*/)
{
	// tabella per il database locale delle url (elenco interno dei siti scaricati)

	// usa il solo lock sulla tabella (via classe derivata) per sincronizzare gli accessi
	m_pUrlDatabaseTable = new CUrlDatabaseTable(lpcszTableName,lpcszDataPath);

	m_bIsValid = m_pUrlDatabaseTable!=NULL;
}

/*
	~CUrlDatabaseService()
*/
CUrlDatabaseService::~CUrlDatabaseService()
{
	// chiude la tabella
	if(m_pUrlDatabaseTable)
	{
		m_pUrlDatabaseTable->Close();
		delete m_pUrlDatabaseTable,m_pUrlDatabaseTable = (CUrlDatabaseTable*)NULL;
	}
}

/*
	GetTable()
*/
CUrlDatabaseTable* CUrlDatabaseService::GetTable(void) const
{
	return(m_pUrlDatabaseTable);
}

/*
	Exist()
*/
BOOL CUrlDatabaseService::Exist(LPCSTR lpcszUrl,LPSTR lpszDate,UINT nDateSize,CUrlStatus::URL_STATUS& nStat,UINT& nID)
{
	BOOL bExist = FALSE;
	nStat = CUrlStatus::URL_STATUS_UNKNOWN;
	nID = 0;

	if(m_bIsValid)
	{
		// sincronizza gli accessi
		if(m_pUrlDatabaseTable->Lock(SYNC_5_SECS_TIMEOUT))
		{
			strcpyn(m_szUrl,lpcszUrl,sizeof(m_szUrl));
			m_Url.DecodeUrl(m_szUrl);

			// controlla se il valore specificato esiste
			if(m_pUrlDatabaseTable->Seek(m_szUrl,URLDATABASE_IDX_URL))
			{
				m_pUrlDatabaseTable->ScatterMemvars();

				CDateTime* pDate = m_pUrlDatabaseTable->GetField_Date();
				CDateTime* pTime = m_pUrlDatabaseTable->GetField_StartTime();
				m_DateTime.SetYear(pDate->GetYear());
				m_DateTime.SetMonth(pDate->GetMonth());
				m_DateTime.SetDay(pDate->GetDay());
				m_DateTime.SetHour(pTime->GetHour());
				m_DateTime.SetMin(pTime->GetMin());
				m_DateTime.SetSec(pTime->GetSec());
				m_DateTime.SetDateFormat(GMT);
				strcpyn(lpszDate,m_DateTime.GetFormattedDate(FALSE),nDateSize);
				nStat = (CUrlStatus::URL_STATUS)m_pUrlDatabaseTable->GetField_Status();
				nID = m_pUrlDatabaseTable->GetField_Id();
				
				bExist = TRUE;
			}
			else
			{
				memset(lpszDate,'\0',nDateSize);
			}

			m_pUrlDatabaseTable->Unlock();
		}
	}

	return(bExist);
}

/*
	Insert()
*/
BOOL CUrlDatabaseService::Insert(LPCSTR lpcszUrl,LPCSTR lpcszStartTime,LPCSTR lpcszDataTable)
{
	BOOL bInserted = FALSE;

	if(m_bIsValid)
	{
		// sincronizza gli accessi
		if(m_pUrlDatabaseTable->Lock(SYNC_5_SECS_TIMEOUT))
		{
			strcpyn(m_szUrl,lpcszUrl,sizeof(m_szUrl));
			m_Url.DecodeUrl(m_szUrl);

			// se gia' esiste un entrata a fronte dell'url la elimina
			if(m_pUrlDatabaseTable->Seek(m_szUrl,URLDATABASE_IDX_URL))
				m_pUrlDatabaseTable->Delete();
			
			// inserisce l'url
			m_pUrlDatabaseTable->ResetMemvars();
			
			m_pUrlDatabaseTable->PutField_Url		(	m_szUrl						);
			m_DateTime.ConvertDate(GMT,ANSI_SHORT,lpcszStartTime,NULL);
			m_pUrlDatabaseTable->PutField_Date		(	&m_DateTime					);
			m_DateTime.ConvertTime(HHMMSS_GMT,HHMMSS,lpcszStartTime,NULL);
			m_pUrlDatabaseTable->PutField_StartTime	(	&m_DateTime					);
			m_pUrlDatabaseTable->PutField_DataTable	(	lpcszDataTable					);
			m_pUrlDatabaseTable->PutField_Status	(	(int)CUrlStatus::URL_STATUS_UNKNOWN);
			
			m_pUrlDatabaseTable->GatherMemvars();
			
			bInserted = m_pUrlDatabaseTable->Insert();

			m_pUrlDatabaseTable->Unlock();
		}
	}

	return(bInserted);
}

/*
	Update()
*/
BOOL CUrlDatabaseService::Update(	LPCSTR	lpcszUrl,
							LPCSTR	lpcszEndTime,
							double	dlTotalTime,
							double	dlDownloadTime,
							CUrlStatus::URL_STATUS nStat,
							UINT		nID,
							LPCSTR	lpcszReportName,
							CUrlTable* pHttptable
							)
{
	BOOL bInserted = FALSE;

	if(m_bIsValid)
	{
		// sincronizza gli accessi
		if(m_pUrlDatabaseTable->Lock(SYNC_5_SECS_TIMEOUT))
		{
			strcpyn(m_szUrl,lpcszUrl,sizeof(m_szUrl));

			// scorre il database per ricavare i totali
			double dlTotBytes = 0.0f;
			UINT nUrls = 0;
			UINT nDownloadedUrls = 0;
			UINT nDownloadedItems = 0;
			UINT nTotalPict = 0;
			if(pHttptable)
			{
				pHttptable->SetIndex(URL_IDX_ID);

				if(pHttptable->GoTop())
				{
					do
					{
						pHttptable->ScatterMemvars();
						
						nUrls++;
						
						if((CUrlStatus::URL_STATUS)pHttptable->GetField_Stat()==CUrlStatus::URL_STATUS_DOWNLOADED)
						{
							nDownloadedUrls++;
							dlTotBytes += pHttptable->GetField_Size();
						}
						
						m_Url.SplitUrl(pHttptable->GetField_Url(),&m_stUrl);

						if(CImage::IsImageFile(m_stUrl.file) || CImage::IsImageFile(m_stUrl.cgi))
						{
							nTotalPict++;
							if((CUrlStatus::URL_STATUS)pHttptable->GetField_Stat()==CUrlStatus::URL_STATUS_DOWNLOADED)
								nDownloadedItems++;
						}
						
						pHttptable->GetNext();
					
					} while(!pHttptable->Eof());
				}
			}

			CDateTime* pDate = NULL;
			CDateTime* pTime = NULL;
			CDateTime Date;
			CDateTime Time;
			CString cData = "";
			BOOL bFound = FALSE;

			if(m_pUrlDatabaseTable->Seek(m_szUrl,URLDATABASE_IDX_URL))
			{
				m_pUrlDatabaseTable->ScatterMemvars();
				
				pDate = m_pUrlDatabaseTable->GetField_Date();
				pTime = m_pUrlDatabaseTable->GetField_StartTime();
				Date.SetYear(pDate->GetYear());
				Date.SetMonth(pDate->GetMonth());
				Date.SetDay(pDate->GetDay());
				Time.SetHour(pTime->GetHour());
				Time.SetMin(pTime->GetMin());
				Time.SetSec(pTime->GetSec());
				cData = m_pUrlDatabaseTable->GetField_DataTable();
				
				m_pUrlDatabaseTable->Delete();

				bFound = TRUE;
			}
			
			if(bFound)
			{
				m_pUrlDatabaseTable->ResetMemvars();
				
				m_pUrlDatabaseTable->PutField_Url			(	m_szUrl			);
				m_pUrlDatabaseTable->PutField_Date			(	&Date			);
				m_pUrlDatabaseTable->PutField_StartTime		(	&Time			);
				m_DateTime.ConvertTime(HHMMSS_GMT,HHMMSS,lpcszEndTime,NULL);
				m_pUrlDatabaseTable->PutField_EndTime		(	&m_DateTime		);
				m_pUrlDatabaseTable->PutField_TotalTime		(	dlTotalTime		);
				m_pUrlDatabaseTable->PutField_DownloadTime	(	dlDownloadTime		);
				m_pUrlDatabaseTable->PutField_Downloaded	(	dlTotBytes		);
				m_pUrlDatabaseTable->PutField_TotalUrls		(	nUrls			);
				m_pUrlDatabaseTable->PutField_TotalPict		(	nTotalPict		);
				m_pUrlDatabaseTable->PutField_DownloadedUrls	(	nDownloadedUrls	);
				m_pUrlDatabaseTable->PutField_DownloadedItems(	nDownloadedItems	);
				m_pUrlDatabaseTable->PutField_DataTable		(	cData			);
				m_pUrlDatabaseTable->PutField_Report		(	lpcszReportName	);
				m_pUrlDatabaseTable->PutField_Status		(	(short int)nStat	);
				m_pUrlDatabaseTable->PutField_Id			(	nID				);
				
				m_pUrlDatabaseTable->GatherMemvars();
				
				bInserted = m_pUrlDatabaseTable->Insert();
			}

			m_pUrlDatabaseTable->Unlock();
		}
	}

	return(bInserted);
}
