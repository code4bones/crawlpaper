/*
	CImageFactory.h
	Classe fattoria per l'oggetto immagine.
	Luca Piergentili, 10/05/00
	lpiergentili@yahoo.com

	Ad memoriam - Nemo me impune lacessit.
*/
#ifndef _CIMAGEFACTORY_H
#define _CIMAGEFACTORY_H 1

#include "window.h"
#include "CNodeList.h"
#include "ImageLibraryName.h"
#include "CImage.h"

/*
	SUPPORTED_LIBRARIES
	struttura per l'elemento per l'elenco delle librerie supportate
*/
struct SUPPORTED_LIBRARIES {
	char name[_MAX_PATH+1];
};

/*
	CSupportedLibrariesList
	Classe per l'elenco delle librerie supportate
*/
class CSupportedLibrariesList : public CNodeList
{
public:
	CSupportedLibrariesList() : CNodeList() {}
	virtual ~CSupportedLibrariesList() {CNodeList::DeleteAll();}
	
	void* Create(void)
	{
		SUPPORTED_LIBRARIES* pData = new SUPPORTED_LIBRARIES;
		return(pData);
	}
	
	void* Initialize(void* pVoid)
	{
		SUPPORTED_LIBRARIES* pData = (SUPPORTED_LIBRARIES*)pVoid;
		if(!pData)
			pData = (SUPPORTED_LIBRARIES*)Create();
		if(pData)
			memset(pData,'\0',sizeof(SUPPORTED_LIBRARIES));
		return(pData);
	}
	
	BOOL PreDelete(ITERATOR iter)
	{
		if((SUPPORTED_LIBRARIES*)iter->data)
		{
			delete ((SUPPORTED_LIBRARIES*)iter->data);
			iter->data = (SUPPORTED_LIBRARIES*)NULL;
		}
		return(TRUE);
	}

	int Size(void) {return(sizeof(SUPPORTED_LIBRARIES));}

#ifdef _DEBUG
	const char* Signature(void) {return("CSupportedLibrariesList");}
#endif
};

/*
	CImageFactory
	Classe fattoria per la creazione dell'oggetto immagine
*/
class CImageFactory
{
public:
	CImageFactory();
	virtual ~CImageFactory();
	
	// crea l'oggetto immagine (viene eliminato automaticamente nel distruttore)
	CImage*	Create(LPSTR lpszLibraryName = NULL,UINT nSize = (UINT)-1);

	// elimina l'oggetto immagine esplicitmente
	void		Delete(void);

	// puntatore all'oggetto immagine corrente
	CImage*	GetImage(void) const {return(m_pImage);}

	// elenco delle librerie
	LPCSTR	EnumLibraryNames(void);

private:
	BOOL		m_bCreated;
	CImage*	m_pImage;
	CSupportedLibrariesList m_LibrariesList;
};

#endif // _CIMAGEFACTORY_H
