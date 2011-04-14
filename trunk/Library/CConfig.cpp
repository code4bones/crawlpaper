/*
	CConfig.cpp
	Classe base per la gestione della configurazione tramite il registro (SDK/MFC).
	Luca Piergentili, 14/07/99
	lpiergentili@yahoo.com
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "strings.h"
#include "window.h"
#include "CConfig.h"
#include "CNodeList.h"
#include "CRegKey.h"

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
	CConfig()
*/
CConfig::CConfig()
{
	m_plistConfig = new CConfigList();
	m_pRegistry = new CRegKey();
	SetModified(FALSE);
}

/*
	~CConfig()
*/
CConfig::~CConfig()
{
	if(m_plistConfig)
		delete m_plistConfig,m_plistConfig = NULL;
	if(m_pRegistry)
		delete m_pRegistry,m_pRegistry = NULL;
}

/*
	Insert()

	Inserisce nella lista il valore (stringa) per la sezione/chiave specificati.

	LPCSTR	lpcszSectionName	nome sezione
	LPCSTR	lpcszKeyName		nome chiave
	LPCSTR	lpcszKeyValue		valore chiave (stringa)
*/
BOOL CConfig::Insert(LPCSTR lpcszSectionName,LPCSTR lpcszKeyName,LPCSTR lpcszKeyValue)
{
	BOOL bInserted = FALSE;

	if(m_plistConfig)
	{
		CONFIG* c = (CONFIG*)m_plistConfig->Add();
		if(c)
		{
			c->Init(lpcszSectionName,lpcszKeyName,lpcszKeyValue);
			SetModified(TRUE);
			bInserted = TRUE;
		}
	}

	return(bInserted);
}

/*
	Insert()

	Inserisce nella lista il valore (numerico) per la sezione/chiave specificati.

	LPCSTR	lpcszSectionName	nome sezione
	LPCSTR	lpcszKeyName		nome chiave
	LPCSTR	lpcszKeyValue		valore chiave (numero)
*/
BOOL CConfig::Insert(LPCSTR lpcszSectionName,LPCSTR lpcszKeyName,DWORD dwKeyValue)
{
	BOOL bInserted = FALSE;

	if(m_plistConfig)
	{
		CONFIG* c = (CONFIG*)m_plistConfig->Add();
		if(c)
		{
			c->Init(lpcszSectionName,lpcszKeyName,dwKeyValue);
			SetModified(TRUE);
			bInserted = TRUE;
		}
	}

	return(bInserted);
}

/*
	Delete()

	Elimina della lista il valore relativo alla coppia sezione/chiave.
	Non elimina fisicamente l'elemento ma si limita a marcarlo in modo tale che quando la lista
	viene salvata sul registro si possano individuare le chiavi da eliminare fisicamente.

	LPCSTR	lpcszSectionName	nome sezione
	LPCSTR	lpcszKeyName		nome chiave
*/
BOOL CConfig::Delete(LPCSTR lpcszSectionName,LPCSTR lpcszKeyName)
{
	BOOL bDeleted = FALSE;
	CONFIG* c;
	ITERATOR iter;

	// scorre la lista cercando l'entrata relativa alla coppia sezione/chiave
	if((iter = m_plistConfig->First())!=(ITERATOR)NULL)
	{
		do
		{
			c = (CONFIG*)iter->data;
			
			if(c)
				if(strcmp(lpcszSectionName,c->GetSection())==0)
				{
					if(strcmp(lpcszKeyName,c->GetName())==0)
					{
						// marca l'elemento per l'eliminazione
						c->SetType(NULL_TYPE);
						SetModified(TRUE);
						bDeleted = TRUE;
						break;
					}
				}

			iter = m_plistConfig->Next(iter);
		
		} while(iter!=(ITERATOR)NULL);
	}
	
	return(bDeleted);
}

/*
	DeleteAll()

	Elimina della lista tutti i valori, rimuovendoli anche dal registro.
*/
BOOL CConfig::DeleteAll(LPCSTR lpcszRootKey)
{
	BOOL bDeleted = TRUE;
	CONFIG* c;
	ITERATOR iter;
	char szKey[REGKEY_MAX_KEY_VALUE+1];

	if(m_pRegistry)
	{
		m_pRegistry->Attach(HKEY_CURRENT_USER);

		if((iter = m_plistConfig->First())!=(ITERATOR)NULL)
		{
			do
			{
				c = (CONFIG*)iter->data;
			
				if(c)
				{
					_snprintf(szKey,sizeof(szKey)-1,DEFAULT_REG_KEY"\\%s\\%s",lpcszRootKey,c->GetSection());
					if(m_pRegistry->Open(HKEY_CURRENT_USER,szKey)==ERROR_SUCCESS)
					{
						m_pRegistry->DeleteValue(c->GetName());
						m_pRegistry->Close();
					}
				}

				iter = m_plistConfig->Next(iter);
			
			} while(iter!=(ITERATOR)NULL);
		}
		
		m_pRegistry->Detach();
	
		SetModified(TRUE);
	}

	m_plistConfig->RemoveAll();

	return(bDeleted);
}

/*
	ReloadString()

	Ricarica dal registro il valore stringa associato alla chiave, aggiornando l'elemento
	corrispondente della lista.
*/
LPCSTR CConfig::ReloadString(LPCSTR lpcszRootKey,LPCSTR section,LPCSTR name,LPCSTR defaultvalue/*=NULL*/)
{
	char key[_MAX_PATH+1];
	static char value[_MAX_PATH+1];
	LONG reg;

	memset(key,'\0',sizeof(key));
	_snprintf(key,sizeof(key)-1,"%s\\%s",lpcszRootKey,section);
	memset(value,'\0',sizeof(value));

	m_pRegistry->Attach(HKEY_CURRENT_USER);

	if((reg = m_pRegistry->Open(HKEY_CURRENT_USER,key))==ERROR_SUCCESS)
	{
		memset(value,'\0',sizeof(value));
		DWORD valuesize = sizeof(value);
		if(m_pRegistry->QueryValue(value,name,&valuesize)!=ERROR_SUCCESS)
		{
			if(defaultvalue)
				strcpyn(value,defaultvalue,sizeof(value));
			else
				memset(value,'\0',sizeof(value));
			m_pRegistry->SetValue(value,name);
		}

		strcpyn(value,(char*)UpdateString(section,name,value),sizeof(value));
		
		m_pRegistry->Close();
	}

	m_pRegistry->Detach();

	return(value);
}

/*
	ReloadNumber()

	Ricarica dal registro il valore numerico associato alla chiave, aggiornando l'elemento
	corrispondente della lista.
*/
DWORD CConfig::ReloadNumber(LPCSTR lpcszRootKey,LPCSTR section,LPCSTR name,DWORD defaultvalue/*=0L*/)
{
	char key[_MAX_PATH+1];
	DWORD value = 0;
	LONG reg;

	memset(key,'\0',sizeof(key));
	_snprintf(key,sizeof(key)-1,"%s\\%s",lpcszRootKey,section);

	m_pRegistry->Attach(HKEY_CURRENT_USER);

	if((reg = CConfig::m_pRegistry->Open(HKEY_CURRENT_USER,key))==ERROR_SUCCESS)
	{
		if(m_pRegistry->QueryValue(value,name)!=ERROR_SUCCESS)
		{
			value = defaultvalue;
			m_pRegistry->SetValue(value,name);
		}

		value = UpdateNumber(section,name,value);

		m_pRegistry->Close();
	}

	m_pRegistry->Detach();

	return(value);
}

/*
	String()

	Restituisce o aggiorna il valore (stringa) associato alla sezione/chiave a seconda del parametro
	associato al valore (NULL/-1 recupera, in caso contrario aggiorna).

	LPCSTR	lpcszSectionName	nome sezione
	LPCSTR	lpcszKeyName		nome chiave
	LPCSTR	lpcszKeyValue		valore chiave (stringa)
*/
LPCSTR CConfig::String(LPCSTR lpcszSectionName,LPCSTR lpcszKeyName,LPCSTR lpcszKeyValue/*=NULL*/)
{
	static char* p;
	CONFIG* c;
	ITERATOR iter;

	p = "";

	// scorre la lista cercando l'entrata relativa alla coppia sezione/chiave
	if((iter = m_plistConfig->First())!=(ITERATOR)NULL)
	{
		do
		{
			c = (CONFIG*)iter->data;
			if(c)
				if(strcmp(lpcszSectionName,c->GetSection())==0)
				{
					if(strcmp(lpcszKeyName,c->GetName())==0)
					{
						// salta i valori marcati per l'eliminazione (NULL_TYPE)
						if(c->GetType()!=NULL_TYPE)
						{
							if(lpcszKeyValue!=NULL)
							{
								// aggiorna il valore
								c->SetValue(lpcszKeyValue);
								p = (char*)c->GetValue(p);
								break;
							}
							else
							{
								// ricava il valore
								p = (char*)c->GetValue(p);
								if(!*p)
									p = "";
								break;
							}
						}
					}
				}

			iter = m_plistConfig->Next(iter);
		
		} while(iter!=(ITERATOR)NULL);
	}
	
	return(p);
}

/*
	Number()

	Restituisce o aggiorna il valore (numerico) associato alla sezione/nome a seconda del parametro
	associato al valore (NULL/-1 recupera, in caso contrario aggiorna).

	LPCSTR	lpcszSectionName	nome sezione
	LPCSTR	lpcszKeyName		nome chiave
	LPCSTR	lpcszKeyValue		valore chiave (numero)
*/
DWORD CConfig::Number(LPCSTR lpcszSectionName,LPCSTR lpcszKeyName,DWORD dwKeyValue/*=(DWORD)-1L*/)
{
	DWORD l = (DWORD)-1L;
	CONFIG* c;
	ITERATOR iter;

	// scorre la lista cercando l'entrata relativa alla coppia sezione/chiave
	if((iter = m_plistConfig->First())!=(ITERATOR)NULL)
	{
		do
		{
			c = (CONFIG*)iter->data;
			
			if(c)
				if(strcmp(lpcszSectionName,c->GetSection())==0)
				{
					if(strcmp(lpcszKeyName,c->GetName())==0)
					{
						// salta i valori marcati per l'eliminazione (NULL_TYPE)
						if(c->GetType()!=NULL_TYPE)
						{
							if(dwKeyValue!=(DWORD)-1L)
							{
								// aggiorna il valore
								c->SetValue(dwKeyValue);
								l = c->GetValue(l);
								break;
							}
							else
							{
								// ricava il valore
								l = c->GetValue(l);
								break;
							}
						}
					}
				}

			iter = m_plistConfig->Next(iter);
		
		} while(iter!=(ITERATOR)NULL);
	}
	
	return(l);
}

/*
	Save()

	Salva nel registro i valori presenti nella lista.

	LPCSTR	lpcszRootKey	nome della chiave base
*/
void CConfig::Save(LPCSTR lpcszRootKey)
{
	CONFIG* c;
	ITERATOR iter;
	LONG lRet;
	char szKey[REGKEY_MAX_KEY_VALUE+1];
	memset(szKey,'\0',sizeof(szKey));

	if(m_pRegistry)
	{
		m_pRegistry->Attach(HKEY_CURRENT_USER);

		// elimina (dalla lista e dal registro) le chiavi marcate per la cancellazione
		if((iter = m_plistConfig->First())!=(ITERATOR)NULL)
		{
			do
			{
				c = (CONFIG*)iter->data;
				
				if(c)
					// chiave da eliminare
					if(c->GetType()==NULL_TYPE)
					{
						// elimina la chiave dal registro
						_snprintf(szKey,sizeof(szKey)-1,DEFAULT_REG_KEY"\\%s\\%s",lpcszRootKey,c->GetSection());
						if(m_pRegistry->Open(HKEY_CURRENT_USER,szKey)==ERROR_SUCCESS)
						{
							m_pRegistry->DeleteValue(c->GetName());
							m_pRegistry->Close();
						}

						// elimina la chiave dalla lista
						m_plistConfig->Remove(iter);
					}

				iter = m_plistConfig->Next(iter);
			
			} while(iter!=(ITERATOR)NULL);
		}
		
		// salva nel registro le chiavi presenti nella lista
		if((iter = m_plistConfig->First())!=(ITERATOR)NULL)
		{
			do
			{
				c = (CONFIG*)iter->data;
				if(c)
					// non inserisce nel registro le chiavi eliminate
					if(c->GetType()!=NULL_TYPE)
					{
						_snprintf(szKey,sizeof(szKey)-1,DEFAULT_REG_KEY"\\%s\\%s",lpcszRootKey,c->GetSection());

						if((lRet = m_pRegistry->Open(HKEY_CURRENT_USER,szKey))!=ERROR_SUCCESS)
							lRet = m_pRegistry->Create(HKEY_CURRENT_USER,szKey);
						if(lRet==ERROR_SUCCESS)
						{
							if(c->GetType()==LPSTR_TYPE)
								m_pRegistry->SetValue(c->GetValue((LPCSTR)NULL),c->GetName());
							else if(c->GetType()==DWORD_TYPE)
								m_pRegistry->SetValue(c->GetValue((DWORD)0L),c->GetName());
							
							m_pRegistry->Close();
						}
					}

				iter = m_plistConfig->Next(iter);
			
			} while(iter!=(ITERATOR)NULL);
		}
		
		m_pRegistry->Detach();
	
		SetModified(FALSE);
	}
}

/*
	SaveKey()

	Salva nel registro il valori della chiave presente nella lista.

	LPCSTR	lpcszRootKey		nome della chiave base
	LPCSTR	lpcszSectionKey	nome della sezione
	LPCSTR	lpcszNameKey		nome della chiave
*/
void CConfig::SaveKey(LPCSTR lpcszRootKey,LPCSTR lpcszSectionKey,LPCSTR lpcszNameKey)
{
	CONFIG* c;
	ITERATOR iter;
	LONG lRet;
	char szKey[REGKEY_MAX_KEY_VALUE+1];

	if(m_pRegistry)
	{
		m_pRegistry->Attach(HKEY_CURRENT_USER);

		// scorre la lista aggiornando la chiave
		if((iter = m_plistConfig->First())!=(ITERATOR)NULL)
		{
			do
			{
				c = (CONFIG*)iter->data;
			
				if(c)
					// non inserisce nel registro le chiavi eliminate
					if(c->GetType()!=NULL_TYPE && strcmp(c->GetSection(),lpcszSectionKey)==0 && strcmp(c->GetName(),lpcszNameKey)==0)
					{
						// salva la chiave nel registro
						_snprintf(szKey,sizeof(szKey)-1,DEFAULT_REG_KEY"\\%s\\%s",lpcszRootKey,lpcszSectionKey);

						if((lRet = m_pRegistry->Open(HKEY_CURRENT_USER,szKey))!=ERROR_SUCCESS)
							lRet = m_pRegistry->Create(HKEY_CURRENT_USER,szKey);
						if(lRet==ERROR_SUCCESS)
						{
							if(c->GetType()==LPSTR_TYPE)
								m_pRegistry->SetValue(c->GetValue((LPCSTR)NULL),lpcszNameKey);
							else if(c->GetType()==DWORD_TYPE)
								m_pRegistry->SetValue(c->GetValue((DWORD)0L),lpcszNameKey);
							
							m_pRegistry->Close();
						}
					}

				iter = m_plistConfig->Next(iter);
			
			} while(iter!=(ITERATOR)NULL);
		}
		
		m_pRegistry->Detach();
	}
}

/*
	SaveSection()

	Salva nel registro i valori presenti nella lista relativi alla sezione.

	LPCSTR	lpcszRootKey		nome della chiave base
	LPCSTR	lpcszSectionKey	nome della sezione
*/
void CConfig::SaveSection(LPCSTR lpcszRootKey,LPCSTR lpcszSectionKey)
{
	CONFIG* c;
	ITERATOR iter;
	LONG lRet;
	char szKey[REGKEY_MAX_KEY_VALUE+1];

	if(m_pRegistry)
	{
		m_pRegistry->Attach(HKEY_CURRENT_USER);

		// scorre la lista aggiornando la sezione
		if((iter = m_plistConfig->First())!=(ITERATOR)NULL)
		{
			do
			{
				c = (CONFIG*)iter->data;
			
				if(c)
					// non inserisce nel registro le chiavi eliminate
					if(c->GetType()!=NULL_TYPE && strcmp(c->GetSection(),lpcszSectionKey)==0)
					{
						// salva la chiave nel registro
						_snprintf(szKey,sizeof(szKey)-1,DEFAULT_REG_KEY"\\%s\\%s",lpcszRootKey,c->GetSection());

						if((lRet = m_pRegistry->Open(HKEY_CURRENT_USER,szKey))!=ERROR_SUCCESS)
							lRet = m_pRegistry->Create(HKEY_CURRENT_USER,szKey);
						if(lRet==ERROR_SUCCESS)
						{
							if(c->GetType()==LPSTR_TYPE)
								m_pRegistry->SetValue(c->GetValue((LPCSTR)NULL),c->GetName());
							else if(c->GetType()==DWORD_TYPE)
								m_pRegistry->SetValue(c->GetValue((DWORD)0L),c->GetName());
							
							m_pRegistry->Close();
						}
					}

				iter = m_plistConfig->Next(iter);
			
			} while(iter!=(ITERATOR)NULL);
		}
		
		m_pRegistry->Detach();
	}
}

/*
	DeleteSection()

	Elimina dal registro (e dalla lista) i valori presenti nella lista relativi alla sezione.

	LPCSTR	lpcszRootKey		nome della chiave base
	LPCSTR	lpcszSectionKey	nome della sezione
	BOOL		bDeleteFromRegistry	flag per eliminazione delle chiavi dal registro
*/
void CConfig::DeleteSection(LPCSTR lpcszRootKey,LPCSTR lpcszSectionKey,BOOL bDeleteFromRegistry/*=FALSE*/)
{
	CONFIG* c;
	ITERATOR iter;
	char szKey[REGKEY_MAX_KEY_VALUE+1];

	if(m_pRegistry)
	{
		m_pRegistry->Attach(HKEY_CURRENT_USER);

		// elimina (dalla lista e dal registro) le chiavi relative alla sezione
		if((iter = m_plistConfig->First())!=(ITERATOR)NULL)
		{
			do
			{
				c = (CONFIG*)iter->data;
				
				if(c)
					// chiave da eliminare
					if(c->GetType()!=NULL_TYPE && strcmp(c->GetSection(),lpcszSectionKey)==0)
					{
						// elimina la chiave dal registro
						if(bDeleteFromRegistry)
						{
							_snprintf(szKey,sizeof(szKey)-1,DEFAULT_REG_KEY"\\%s\\%s",lpcszRootKey,c->GetSection());
							if(m_pRegistry->Open(HKEY_CURRENT_USER,szKey)==ERROR_SUCCESS)
							{
								m_pRegistry->DeleteValue(c->GetName());
								m_pRegistry->Close();
							}
						}

						// elimina la chiave dalla lista
						//m_plistConfig->Remove(iter);
						
						// eliminando l'elemento della lista (iter) e non la chiave (marcandola come cancellata) i salvataggi successivi
						// non possono eliminare dal registro le serie di chiavi come Key[0]...Key[n]
						//Delete(c->GetSection(),c->GetName());
						
						c->SetType(NULL_TYPE);
						SetModified(TRUE);
					}

				iter = m_plistConfig->Next(iter);
			
			} while(iter!=(ITERATOR)NULL);
		}
		
		m_pRegistry->Detach();
	
		SetModified(TRUE);
	}
}

/*
	Export()

	Esporta la configurazione corrente nel file specificato.
*/
BOOL	CConfig::Export(LPCSTR /*lpcszRootKey*/,LPCSTR lpcszFileName)
{
	BOOL bSaved = FALSE;
	FILE* fp;

	if((fp = fopen(lpcszFileName,"w"))!=(FILE*)NULL)
	{
		CONFIG* c;
		ITERATOR iter;

		fprintf(fp,"[%s]\n","Configuration File");

		// salva nel file le chiavi presenti nella lista
		if((iter = m_plistConfig->First())!=(ITERATOR)NULL)
		{
			do
			{
				c = (CONFIG*)iter->data;
				
				if(c)
					// non inserisce le chiavi eliminate
					if(c->GetType()!=NULL_TYPE)
					{
						if(c->GetType()==LPSTR_TYPE)
							fprintf(fp,"%s;%s;%s;%s\n",c->GetSection(),c->GetName(),c->GetValue((LPCSTR)NULL),"SZ");
						else if(c->GetType()==DWORD_TYPE)
							fprintf(fp,"%s;%s;%ld;%s\n",c->GetSection(),c->GetName(),c->GetValue((DWORD)0L),"DW");
					}

				iter = m_plistConfig->Next(iter);
			
			} while(iter!=(ITERATOR)NULL);
		}
		
		fclose(fp);
		
		bSaved = TRUE;
	}

	return(bSaved);
}

/*
	Import()

	Importa la configurazione corrente dal file specificato.
*/
BOOL	CConfig::Import(LPCSTR lpcszRootKey,LPCSTR lpcszFileName)
{
	BOOL bLoaded = FALSE;
	FILE* fp;

	if((fp = fopen(lpcszFileName,"r"))!=(FILE*)NULL)
	{
		char* p;
		char szBuffer[REGKEY_MAX_SECTION_NAME+REGKEY_MAX_KEY_NAME+REGKEY_MAX_KEY_VALUE+16];
		char szSection[REGKEY_MAX_SECTION_NAME+1];
		char szName[REGKEY_MAX_KEY_NAME+1];
		char szValue[REGKEY_MAX_KEY_VALUE+1];
		char szType[5];

		// legge la prima linea controllando se si tratta di un file di configurazione
		fgets(szBuffer,sizeof(szBuffer)-1,fp);
		if((p = strchr(szBuffer,'\r'))==NULL)
			p = strchr(szBuffer,'\n');
		if(p)
			*p = '\0';
		if(strcmp(szBuffer,"[Configuration File]")!=0)
		{
			fclose(fp);
			return(bLoaded);
		}

		// legge il file di configurazione per linee
		while(fgets(szBuffer,sizeof(szBuffer)-1,fp))
		{
			if((p = strchr(szBuffer,'\r'))==NULL)
				p = strchr(szBuffer,'\n');
			if(p)
				*p = '\0';
			
			// estrae i valori dalla linea (sezione, chiave, valore, tipo)
			memset(szSection,'\0',sizeof(szSection));
			memset(szName,'\0',sizeof(szName));
			memset(szValue,'\0',sizeof(szValue));
			memset(szType,'\0',sizeof(szType));

			p = strrchr(szBuffer,';');
			if(p)
			{
				strcpyn(szType,p+1,sizeof(szType));
				*p = '\0';
			}
			p = strrchr(szBuffer,';');
			if(p)
			{
				strcpyn(szValue,p+1,sizeof(szValue));
				*p = '\0';
			}
			p = strrchr(szBuffer,';');
			if(p)
			{
				strcpyn(szName,p+1,sizeof(szName));
				*p = '\0';
			}
			p = szBuffer;
			if(p)
				strcpyn(szSection,p,sizeof(szSection));

			if(szSection[0]!='\0' && szName[0]!='\0' && szValue[0]!='\0' && szType[0]!='\0')
			{
				if(strcmp(szType,"SZ")==0)
				{
					if(strcmp(UpdateString(szSection,szName,szValue),"")==0)
					{
						CONFIG* c = (CONFIG*)m_plistConfig->Add();
						if(c)
							c->Init(szSection,szName,szValue);
					}
				}
				else if(strcmp(szType,"DW")==0)
				{
					if(UpdateNumber(szSection,szName,strtoul(szValue,NULL,0))==(DWORD)-1)
					{
						CONFIG* c = (CONFIG*)m_plistConfig->Add();
						if(c)
							c->Init(szSection,szName,strtoul(szValue,NULL,0));
					}
				}
			}
		}

		fclose(fp);
		
		Save(lpcszRootKey);
		
		bLoaded = TRUE;
	}

	return(bLoaded);
}

/*
	RegistryCreateValue()

	Crea la coppia nome/valore dentro la chiave specificata, es.:
	key   = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"
	name  = "WallPaper"
	value = "C:\\WallPaper\\WallPaper.exe"
*/
BOOL CConfig::RegistryCreateValue(LPCSTR lpcszKey,LPCSTR lpcszName,LPCSTR lpcszValue)
{
	BOOL flag = FALSE;

	m_pRegistry->Attach(HKEY_CURRENT_USER);

	if(m_pRegistry->Open(HKEY_CURRENT_USER,lpcszKey)==ERROR_SUCCESS)
	{
		char name[_MAX_PATH+1];
		DWORD namesize = sizeof(name);

		if(m_pRegistry->QueryValue(name,lpcszName,&namesize)!=ERROR_SUCCESS)
			flag = m_pRegistry->SetValue(lpcszValue,lpcszName)==ERROR_SUCCESS;
		
		m_pRegistry->Close();
	}
	
	m_pRegistry->Detach();

	return(flag);
}

/*
	RegistryCreateValue()

	Crea la coppia nome/valore dentro la chiave specificata, es.:
	key   = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"
	name  = "WallPaper"
	value = "C:\\WallPaper\\WallPaper.exe"
*/
BOOL CConfig::RegistryCreateValue(LPCSTR lpcszKey,LPCSTR lpcszName,DWORD dwValue)
{
	BOOL flag = FALSE;

	m_pRegistry->Attach(HKEY_CURRENT_USER);

	if(m_pRegistry->Open(HKEY_CURRENT_USER,lpcszKey)==ERROR_SUCCESS)
	{
		if(m_pRegistry->QueryValue(dwValue,lpcszName)!=ERROR_SUCCESS)
			flag = m_pRegistry->SetValue(dwValue,lpcszName)==ERROR_SUCCESS;
		
		m_pRegistry->Close();
	}
	
	m_pRegistry->Detach();

	return(flag);
}

/*
	RegistryDeleteValue()

	Elimina la coppia nome/valore dentro la chiave specificata, es.:
	key   = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"
	name  = "WallPaper"
	value = "C:\\WallPaper\\WallPaper.exe"
*/
BOOL CConfig::RegistryDeleteValue(LPCSTR lpcszKey,LPCSTR lpcszName)
{
	BOOL flag = FALSE;

	m_pRegistry->Attach(HKEY_CURRENT_USER);

	if(m_pRegistry->Open(HKEY_CURRENT_USER,lpcszKey)==ERROR_SUCCESS)
	{
		char name[_MAX_PATH+1];
		DWORD namesize = sizeof(name);

		if(m_pRegistry->QueryValue(name,lpcszName,&namesize)==ERROR_SUCCESS)
			flag = m_pRegistry->DeleteValue(lpcszName)==ERROR_SUCCESS;

		m_pRegistry->Close();
	}
	
	m_pRegistry->Detach();

	return(flag);
}

/*
	Reset()
*/
void CConfig::CONFIG::Reset(void)
{
	memset(m_szSection,'\0',sizeof(m_szSection));
	memset(m_szName,'\0',sizeof(m_szName));
	memset(&m_Value,'\0',sizeof(VALUE));
	m_Type = NULL_TYPE;
}

/*
	Init()
*/
void CConfig::CONFIG::Init(LPCSTR lpcszSectionName/*=NULL*/,LPCSTR lpcszKeyName/*=NULL*/,LPCSTR lpcszKeyValue/*=NULL*/)
{
	SetSection(lpcszSectionName);
	SetName(lpcszKeyName);
	SetValue(lpcszKeyValue);
}

/*
	Init()
*/
void CConfig::CONFIG::Init(LPCSTR lpcszSectionName/*=NULL*/,LPCSTR lpcszKeyName/*=NULL*/,DWORD dwKeyValue/*=(DWORD)-1L*/)
{
	SetSection(lpcszSectionName);
	SetName(lpcszKeyName);
	SetValue(dwKeyValue);
}

/*
	SetSection()
*/
void CConfig::CONFIG::SetSection(LPCSTR lpcszSectionName)
{
	if(lpcszSectionName)
		strcpyn(m_szSection,lpcszSectionName,sizeof(m_szSection));
}

/*
	SetName()
*/
void CConfig::CONFIG::SetName(LPCSTR lpcszKeyName)
{
	if(lpcszKeyName)
		strcpyn(m_szName,lpcszKeyName,sizeof(m_szName));
}

/*
	SetValue()
*/
void CConfig::CONFIG::SetValue(LPCSTR lpcszKeyValue)
{	
	if(lpcszKeyValue)
	{
		memset(m_Value.szValue,'\0',REGKEY_MAX_KEY_VALUE+1);
		strcpyn(m_Value.szValue,lpcszKeyValue,REGKEY_MAX_KEY_VALUE+1);
		m_Type = LPSTR_TYPE;
	}
}

/*
	SetValue()
*/
void CConfig::CONFIG::SetValue(DWORD dwKeyValue)
{
	if(dwKeyValue!=(DWORD)-1L)
	{
		m_Value.dwValue = dwKeyValue;
		m_Type = DWORD_TYPE;
	}
}
