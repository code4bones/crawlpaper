/*
	CUrlHistoryService.cpp
	Implementazione delle classi per l'accesso alle tabelle.
	Luca Piergentili, 30/10/30
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
#include "CImageFactory.h"
#include "CAudioPlayer.h"
#include "CUrl.h"
#include "CUrlHistory.h"
#include "CUrlHistoryService.h"

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
	CUrlHistoryService()
*/
CUrlHistoryService::CUrlHistoryService(LPCSTR lpcszTableName/*=NULL*/,LPCSTR lpcszDataPath/*=NULL*/)
{
	// tabella per lo storico delle url

	// usa il solo lock sulla tabella (via classe derivata) per sincronizzare gli accessi
	m_pUrlHistoryTable = new CUrlHistoryTable(lpcszTableName,lpcszDataPath);

	m_bIsValid = m_pUrlHistoryTable!=NULL;
}

/*
	~CUrlHistoryService()
*/
CUrlHistoryService::~CUrlHistoryService()
{
	// sblocca il lock (quello sul mutex) e chiude la tabella
	if(m_pUrlHistoryTable)
	{
		m_pUrlHistoryTable->Close();
		delete m_pUrlHistoryTable,m_pUrlHistoryTable = (CUrlHistoryTable*)NULL;
	}
}

/*
	GetTable()
*/
CUrlHistoryTable* CUrlHistoryService::GetTable(void) const
{
	return(m_pUrlHistoryTable);
}

/*
	Add()
*/
BOOL CUrlHistoryService::Add(LPCSTR lpcszUrl)
{
	BOOL bInserted = FALSE;

	if(m_bIsValid)
	{
		// sincronizza gli accessi
		if(m_pUrlHistoryTable->Lock(SYNC_5_SECS_TIMEOUT))
		{
			// decodifica l'url
			strcpyn(m_szUrl,lpcszUrl,sizeof(m_szUrl));
			m_Url.DecodeUrl(m_szUrl);

			// inserisce solo se gia' non esiste
			if(!m_pUrlHistoryTable->Seek(m_szUrl,URL_HISTORY_IDX_URL))
			{
				char szDateTime[URL_HISTORY_DATETIME_LEN+1];
				CDateTime dateTime(ANSI_SHORT,HHMMSS_SHORT);
				_snprintf(szDateTime,sizeof(szDateTime)-1,"%s%s",dateTime.GetFormattedDate(TRUE),dateTime.GetFormattedTime(TRUE));
				
				m_pUrlHistoryTable->ResetMemvars();
				
				m_pUrlHistoryTable->PutField_Id(0);
				m_pUrlHistoryTable->PutField_Url(m_szUrl);
				m_pUrlHistoryTable->PutField_DateTime(szDateTime);

				m_pUrlHistoryTable->GatherMemvars();
				
				bInserted = m_pUrlHistoryTable->Insert();
			}

			m_pUrlHistoryTable->Unlock();
		}
	}

	return(bInserted);
}

/*
	Delete()
*/
BOOL CUrlHistoryService::Delete(LPCSTR lpcszUrl)
{
	BOOL bDeleted = FALSE;

	if(m_bIsValid)
	{
		// sincronizza gli accessi
		if(m_pUrlHistoryTable->Lock(SYNC_5_SECS_TIMEOUT))
		{
			if(m_pUrlHistoryTable->Seek(lpcszUrl,URL_HISTORY_IDX_URL))
				bDeleted = m_pUrlHistoryTable->Delete();

			m_pUrlHistoryTable->Unlock();
		}
	}

	return(bDeleted);
}

/*
	DeleteAll()
*/
void CUrlHistoryService::DeleteAll(void)
{
	if(m_bIsValid)
	{
		// sincronizza gli accessi
		if(m_pUrlHistoryTable->Lock(SYNC_5_SECS_TIMEOUT))
		{
			m_pUrlHistoryTable->Zap();
			m_pUrlHistoryTable->Unlock();
		}
	}
}
