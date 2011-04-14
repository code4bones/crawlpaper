/*
	CLicenseService.h
	Implementazione delle funzioni di base per l'accesso alle tabelle per il servizio delle licenze.
	Luca Piergentili, 02/06/00
	lpiergentili@yahoo.com
*/
#ifndef _CLICENSESERVICE_H
#define _CLICENSESERVICE_H 1

#if defined(_AFX) || defined(_AFXDLL)
	#include <afxwin.h>
	#define STRICT 1
	#include <windows.h>
#endif
#include "typedef.h"
#include <stdlib.h>
#include "strcpyn.h"
#include "CLicenseDatabase.h"

/*
	CLicenseService
*/
class CLicenseService
{
public:
	CLicenseService(const char* pDataPath);
	virtual ~CLicenseService();
	
	const char*	GetLabel			(const char* id);
	BOOLEAN		PutLabel			(const char* id,const char* text);
	BOOLEAN		RemoveLabel		(const char* id);
	int			CountLabels		(void);

protected:
	char			m_szDatapath		[_MAX_PATH + 1];
#ifdef USE_CBASE
	CLabelsTable*	labels;
#endif
};

#endif // _CLICENSESERVICE_H
