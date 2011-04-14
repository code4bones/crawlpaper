/*
	CFilenameFactory.h
	Classe base per la gestione dei nomi files (SDK/MFC).
	Luca Piergentili, 02/09/03
	lpiergentili@yahoo.com
*/
#ifndef _CFILENAMEFACTORY_H
#define _CFILENAMEFACTORY_H 1

#include <stdio.h>
#include <stdlib.h>
#include "window.h"
#include "win32api.h"

#define ABBREVIATE_DEFAULT_STRING	"[...]"
#define ABBREVIATE_MAX_FNAME		64

/*
	CFilenameFactory
*/
class CFilenameFactory
{
public:
	CFilenameFactory() {memset(m_szFileName,'\0',sizeof(m_szFileName));}
	virtual ~CFilenameFactory() {}

	LPCSTR	GetFollowing	(
						LPCSTR	pFilename,					// (solo) nome file
						LPCSTR	pPathname = NULL				// pathname (senza o con \ finale)
						);

	LPCSTR	Abbreviate	(
						LPCSTR	pFilename,					// nome file (con eventuale pathname)
						int		nMaxLength = ABBREVIATE_MAX_FNAME,	// massima lunghezza (oltre la quale tronca), se <= 0 elimina solo il pathname
						BOOL		bSaveExt = FALSE,				// per includere l'estensione del file
						BOOL		bStripPath = TRUE,				// per eliminare il pathname dal nome file di output
						LPCSTR	pDefaultString = NULL			// se non specificato diversamente assume <ABBREVIATE_DEFAULT_STRING>
						);

private:
	char m_szFileName[_MAX_FILEPATH+1];
};

#endif // _CFILENAMEFACTORY_H
