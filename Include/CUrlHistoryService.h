/*
	CUrlHistoryService.h
	Implementazione delle classi per l'accesso alle tabelle.
	Luca Piergentili, 30/10/30
	lpiergentili@yahoo.com
*/
#ifndef _CURLHISTORYSERVICE_H
#define _CURLHISTORYSERVICE_H 1

#include "typedef.h"
#include <stdlib.h>
#include "strings.h"
#include "window.h"
#include "CSync.h"
#include "CDateTime.h"
#include "CImageFactory.h"
#include "CUrl.h"
#include "CUrlHistory.h"

/*
	CUrlHistoryService
	tabella per lo storico delle url
*/
class CUrlHistoryService
{
public:
	CUrlHistoryService(LPCSTR lpcszTableName = NULL,LPCSTR lpcszDataPath = NULL);
	virtual ~CUrlHistoryService();
	
	CUrlHistoryTable*	GetTable	(void) const;

	inline BOOL		IsValid	(void) {return(m_bIsValid);}
	
	BOOL				Add		(LPCSTR lpcszUrl);

	BOOL				Delete	(LPCSTR lpcszUrl);
	void				DeleteAll	(void);

protected:
	BOOL				m_bIsValid;
	CUrlHistoryTable*	m_pUrlHistoryTable;

private:
	CUrl			m_Url;
	char			m_szUrl[MAX_URL+1];
};

#endif // _CURLHISTORYSERVICE_H
