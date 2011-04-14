/*
	CConfig.h
	Classe base per la gestione della configurazione tramite il registro.
	Luca Piergentili, 14/07/99
	lpiergentili@yahoo.com
*/
#ifndef _CCONFIG_H
#define _CCONFIG_H 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "strings.h"
#include "window.h"
#include "CNodeList.h"
#include "CRegKey.h"

#define DEFAULT_REG_KEY "SOFTWARE"				// chiave base

/*
	CConfig
*/
class CConfig
{
public:
	// tipi interni
	enum TYPE {							// tipo
		NULL_TYPE = -1,					// nullo
		DWORD_TYPE = 0,					// valore numerico
		LPSTR_TYPE = 1						// valore stringa
	};

	union VALUE {							// valore
		DWORD dwValue;						// numero
		char szValue[REGKEY_MAX_KEY_VALUE+1];	// stringa
	};

	CConfig();
	virtual ~CConfig();

	// per impostare il flag di modificato
	inline BOOL	GetModified		(void) const	{return(m_bDirty);}
	inline void	SetModified		(BOOL bDirty)	{m_bDirty = bDirty;}

	// inserisce il valore nella lista
	BOOL			Insert			(LPCSTR lpcszSectionName,LPCSTR lpcszKeyName,LPCSTR lpcszKeyValue);
	BOOL			Insert			(LPCSTR lpcszSectionName,LPCSTR lpcszKeyName,DWORD dwKeyValue);

	// elimina il valore dalla lista (aggiungere un flag opzionale per eliminare il valore anche dal registro)
	BOOL			Delete			(LPCSTR lpcszSectionName,LPCSTR lpcszKeyName);

	// elimina tutti i valori dalla lista e dal registro
	BOOL			DeleteAll			(LPCSTR lpcszRootKey);
	
	// restituisce/aggiorna il valore della lista
	inline LPCSTR	GetString			(LPCSTR lpcszSectionName,LPCSTR lpcszKeyName)					{return(String(lpcszSectionName,lpcszKeyName,NULL));}
	inline DWORD	GetNumber			(LPCSTR lpcszSectionName,LPCSTR lpcszKeyName)					{return(Number(lpcszSectionName,lpcszKeyName,(DWORD)-1L));}
	inline LPCSTR	UpdateString		(LPCSTR lpcszSectionName,LPCSTR lpcszKeyName,LPCSTR lpcszKeyValue)	{SetModified(TRUE); return(String(lpcszSectionName,lpcszKeyName,lpcszKeyValue));}
	inline DWORD	UpdateNumber		(LPCSTR lpcszSectionName,LPCSTR lpcszKeyName,DWORD dwKeyValue)		{SetModified(TRUE); return(Number(lpcszSectionName,lpcszKeyName,dwKeyValue));}

	// ricarica il valore dal registro aggiornando la lista
	LPCSTR		ReloadString		(LPCSTR lpcszRootKey,LPCSTR,LPCSTR,LPCSTR defaultvalue = NULL);
	DWORD		ReloadNumber		(LPCSTR lpcszRootKey,LPCSTR,LPCSTR,DWORD = 0L);
	
	// puntatore all'oggetto per l'accesso al registro
	inline CRegKey* GetRegistry		(void) {return(m_pRegistry);}

	// crea/elimina la coppia nome/valore dentro la chiave specificata
	BOOL			RegistryCreateValue	(LPCSTR,LPCSTR,LPCSTR);
	BOOL			RegistryCreateValue	(LPCSTR,LPCSTR,DWORD);
	BOOL			RegistryDeleteValue	(LPCSTR,LPCSTR);

	virtual BOOL	Export			(LPCSTR lpcszRootKey,LPCSTR lpcszFileName);
	virtual BOOL	Import			(LPCSTR lpcszRootKey,LPCSTR lpcszFileName);

protected:
	// salva/elimina/carica la lista nel/dal registro
	void			Save				(LPCSTR lpcszRootKey);
	void			SaveKey			(LPCSTR lpcszRootKey,LPCSTR lpcszSectionKey,LPCSTR lpcszNameKey);
	void			SaveSection		(LPCSTR lpcszRootKey,LPCSTR lpcszSectionKey);
	void			DeleteSection		(LPCSTR lpcszRootKey,LPCSTR lpcszSectionKey,BOOL bDeleteFromRegistry = FALSE);
	virtual void	Load				(LPCSTR lpcszSectionName = NULL,BOOL bLoadDefaults = TRUE) = 0;

	// oggetto per l'accesso al registro
	CRegKey*		m_pRegistry;

private:
	// per sapere se viene modificata la configurazione
	BOOL			m_bDirty;

	// gestione del valore della lista
	LPCSTR		String		(LPCSTR lpcszSectionName,LPCSTR lpcszKeyName,LPCSTR lpcszKeyValue = NULL);
	DWORD		Number		(LPCSTR lpcszSectionName,LPCSTR lpcszKeyName,DWORD dwKeyValue = (DWORD)-1L);

	// struttura per l'elemento della lista
	class CONFIG
	{
	public:
		CONFIG() {Reset();}
		~CONFIG() {}
		
		// inizializzazione
		void			Init			(LPCSTR lpcszSectionName = NULL,LPCSTR lpcszKeyName = NULL,LPCSTR lpcszKeyValue = NULL);
		void			Init			(LPCSTR lpcszSectionName = NULL,LPCSTR lpcszKeyName = NULL,DWORD dwKeyValue = (DWORD)-1L);
		void			Reset		(void);
		
		// copia
		void			SetSection	(LPCSTR lpcszSectionName);
		void			SetName		(LPCSTR lpcszKeyName);
		void			SetValue		(LPCSTR lpcszKeyValue);
		void			SetValue		(DWORD dwKeyValue);
		
		// recupera
		inline LPCSTR	GetSection	(void)			{return(m_szSection);}
		inline LPCSTR	GetName		(void)			{return(m_szName);}
		inline LPCSTR	GetValue		(LPCSTR = NULL)	{return(m_Value.szValue);}
		inline DWORD	GetValue		(DWORD = 0L)		{return(m_Value.dwValue);}

		// tipo
		inline TYPE	GetType		(void)			{return(m_Type);}
		inline void	SetType		(TYPE type)		{m_Type = type;}

	private:
		char		m_szSection[REGKEY_MAX_SECTION_NAME+1];	// nome sezione
		char		m_szName[REGKEY_MAX_KEY_NAME+1];		// nome chiave
		VALUE	m_Value;							// valore
		TYPE		m_Type;							// tipo
	};

	// classe per la gestione della lista
	class CConfigList : public CNodeList
	{
	public:
		CConfigList() : CNodeList() {}
		virtual ~CConfigList() {CNodeList::DeleteAll();}
		void* Create(void)
		{
			return(new CONFIG);
		}
		void* Initialize(void* pVoid)
		{
			CONFIG* pData = (CONFIG*)pVoid;
			if(!pData)
				pData = (CONFIG*)Create();
			if(pData)
				pData->Reset();
			return(pData);
		}
		BOOL PreDelete(ITERATOR iter)
		{
			if((CONFIG*)iter->data)
				delete ((CONFIG*)iter->data),iter->data = (CONFIG*)NULL;
			return(TRUE);
		}
		int Size(void) {return(-1);}
#ifdef _DEBUG
		const char* Signature(void) {return("CConfigList");}
#endif
	};

	// oggetto per la gestione della lista
	CConfigList* m_plistConfig;
};

#endif // _CCONFIG_H
