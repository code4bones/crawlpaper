/*
	CBase.cpp
	Classe derivata per l'interfaccia con il database.
	Definisce l'oggetto per il database (CBase) usando la classe che si interfaccia con la libreria (CBerkeleyDB).
	Luca Piergentili, 04/11/99
	lpiergentili@yahoo.com
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include <ctype.h>
#include <locale.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "strings.h"
#include "typedef.h"
#include "CSync.h"
#include "CDateTime.h"
#include "CTextFile.h"
#include "CBase.h"
#include "CBerkeleyDB.h"

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

#define DEF_MAX_SIZE 1024

/*
	CBase()
*/
CBase::CBase()
{
	SetValid(FALSE);

	m_pBerkeleyDB = new CBerkeleyDB();

	m_pDatabase = NULL;

	m_pTableStruct = NULL;
	m_nTotRows = -1;
	m_pIdxStruct = NULL;
	m_nTotIdx = -1;

	m_pRecordBuffer = NULL;

	memset(&m_Field,'\0',sizeof(m_Field));

	memset(m_szPrimaryKeyValue,'\0',sizeof(m_szPrimaryKeyValue));

	m_bSoftseek = FALSE;

	m_DateTime.SetDateFormat(BRITISH);
	m_DateTime.SetTimeFormat(HHMMSS);
	m_bCentury = TRUE;

#ifdef _USE_FIELD_PICTURES
	m_pPictureNumber = m_pPictureChar = m_pPicturePunct = m_pPictureUserDefined = NULL;
#endif
}

/*
	~CBase()
*/
CBase::~CBase()
{
	Close();

	// record per la tabella
	if(m_pRecordBuffer)
		delete [] m_pRecordBuffer,m_pRecordBuffer = NULL;

	// strutture per creazione dinamica
	if(m_pTableStruct && m_nTotRows!=-1)
	{
		for(int i=0; i < m_nTotRows; i++)
			if(m_pTableStruct[i].field)
				delete [] m_pTableStruct[i].field;
		delete [] m_pTableStruct,m_pTableStruct = NULL;
	}
	if(m_pIdxStruct && m_nTotIdx!=-1)
	{
		for(int i=0; i < m_nTotIdx; i++)
		{
			if(m_pIdxStruct[i].field)
				delete [] m_pIdxStruct[i].field;
			if(m_pIdxStruct[i].name)
				delete [] m_pIdxStruct[i].name;
			if(m_pIdxStruct[i].file)
				delete [] m_pIdxStruct[i].file;
		}
		delete [] m_pIdxStruct,m_pIdxStruct = NULL;
	}

	// struttura per la tabella
	if(m_pDatabase->table.row)
		delete [] m_pDatabase->table.row,m_pDatabase->table.row = NULL;
	if(m_pDatabase->table.index)
		delete [] m_pDatabase->table.index,m_pDatabase->table.index = NULL;

	// oggetto per la tabella
	if(m_pBerkeleyDB)
		delete m_pBerkeleyDB,m_pBerkeleyDB = NULL;

#ifdef _USE_FIELD_PICTURES
	// pictures
	if(m_pPictureNumber)
		delete [] m_pPictureNumber,m_pPictureNumber = NULL;
	if(m_pPictureChar)
		delete [] m_pPictureChar,m_pPictureChar = NULL;
	if(m_pPicturePunct)
		delete [] m_pPicturePunct,m_pPicturePunct = NULL;
	if(m_pPictureUserDefined)
		delete [] m_pPictureUserDefined,m_pPictureUserDefined = NULL;
#endif

	SetValid(FALSE);
}

/*
	Lock()
*/
BOOL CBase::Lock(int nTimeout/*=SYNC_5_SECS_TIMEOUT*/)
{
	BOOL bLocked = FALSE;

	if(IsValid())
	{
		if(*m_mutexTable.GetName()=='\0')
		{
			char szMutexName[_MAX_PATH+1];
			_snprintf(szMutexName,sizeof(szMutexName)-1,"%s\\%sTableMutex",GetTablePath(),GetTableName());
			m_mutexTable.SetName(szMutexName);
		}
			
		bLocked = m_mutexTable.Lock(nTimeout);
	}

	return(bLocked);
}

/*
	Unlock()
*/
BOOL CBase::Unlock(void)
{
	BOOL bUnlocked = FALSE;

	if(IsValid())
		bUnlocked = m_mutexTable.Unlock();

	return(bUnlocked);
}

/*
	Create()

	Carica le strutture per la tabella e gli indici, associandole all'oggetto per il database.
	Il nome della tabella deve contenere il nome/pathname completo di estensione.

	Se viene passato il nome del file contenente la definizione della tabella, crea e carica le strutture
	dinamicamente (in tal caso i puntatori alle strutture per la tabella e gli indici devono essere a NULL).
	Il file con la definizione della tabella (.def) deve contenere quanto segue:
	
	tablename=clienti.db
	totfields=5
	field0=PRIMARY_KEY,C,10,0,0
	field1=NOME,C,32,0,1
	field2=COGNOME,C,32,0,2
	field3=INDIRIZZO,C,64,0,3
	field4=CITTA,C,32,0,4
	totindex=4
	index0=clienti.nome.idx,IDX_NOME,NOME,1
	index1=clienti.cognome.idx,IDX_COGNOME,COGNOME,2
	index2=clienti.indirizzo.idx,IDX_INDIRIZZO,INDIRIZZO,3
	index3=clienti.citta.idx,IDX_CITTA,CITTA,4

	Il campo PRIMARY_KEY (e gli indici) va specificato solo se la tabella prevede indici, in caso contrario
	field0 deve riferirsi al primo campo.
	Il campo field[n] specifica: nome campo,dimensione,decimali,progressivo(a base 0).
	Il campo index[n] specifica: nome file indice,nome indice,progressivo del campo(a base 0)
*/
BOOL CBase::Create(const char* pTableName,CBASE_TABLE* pTable/*=NULL*/,CBASE_INDEX* pIdx/*=NULL*/)
{
	// occhio che le funzioni chiamate non possono effettuare il test sulla validita' dell'oggetto
	int i,n;
	int nTotFields = -1;
	int nTotIndex = -1;
	int nTotSize;
	int nFieldNum;
	char szTableName[_MAX_PATH+1] = {0};

	// controlla l'esito del costruttore e che non venga chiamata piu' volte
	if(!m_pBerkeleyDB || IsValid())
		return(FALSE);

	SetValid(FALSE);

	// puntatore alla struttura per la tabella (l'oggetto viene creato nel costruttore)
	m_pDatabase = m_pBerkeleyDB->GetDatabase();

#ifdef _USE_FIELD_PICTURES
	// imposta la picture di default
	SetDefaultNumberPicture();
	SetDefaultCharPicture();
	SetDefaultPunctPicture();
	SetDefaultUserPicture();
#endif

	// se deve caricare la definizione della tabella dal file
	if(stristr(pTableName,".def") || !pTable || !pIdx)
	{
		CBinFile deffile;
		if(deffile.Open(pTableName,FALSE))
		{
			char* p;
			char* pElement;
			char szBuffer[DEF_MAX_SIZE+1] = {0};
			char szDefBuffer[DEF_MAX_SIZE+1] = {0};
			char szDataPath[_MAX_PATH+1] = {0};
			
			// legge in una botta sola e cerca con stristr()
			if(deffile.Read(szDefBuffer,sizeof(szDefBuffer)-1))
			{
				// ricava il pathname da aggiungere al nome tabella/indici
				strcpyn(szDataPath,pTableName,sizeof(szDataPath));
				if((p = strrchr(szDataPath,'\\'))!=NULL)
					*p = '\0';
				else
					szDataPath[0] = '\0';

				// nome della tabella
				if((p = stristr(szDefBuffer,"tablename="))!=NULL)
				{
					p += 10;
					for(i=0; *p && *p!='\r' && *p!='\n' && i < sizeof(szBuffer)-1; i++)
						szBuffer[i] = *p++;
					szBuffer[i] = '\0';
				
					_snprintf(szTableName,sizeof(szTableName)-1,"%s%s%s",szDataPath,szDataPath[0]!='\0' ? "\\" : "",szBuffer);
				}

				// totale dei campi
				if((p = stristr(szDefBuffer,"totfields="))!=NULL)
				{
					p += 10;
					for(i=0; *p && isdigit(*p) && i < sizeof(szBuffer)-1; i++)
						szBuffer[i] = *p++;
					szBuffer[i] = '\0';
					nTotFields = atoi(szBuffer);
				}
				
				// crea e riempie la struttura per la definizione della tabella (i campi)
				if(nTotFields > 0)
				{
					m_nTotRows = nTotFields+1;
					if(stristr(szDefBuffer,"PRIMARY_KEY"))
						m_nTotRows--;

					m_pTableStruct = new CBASE_TABLE[m_nTotRows];
					for(i=0; i < m_nTotRows; i++)
					{
						m_pTableStruct[i].field = NULL;
						m_pTableStruct[i].type = 0;
						m_pTableStruct[i].size = 0;
						m_pTableStruct[i].dec = 0;
					}
					
					char szField[16];
					int x=0;
					for(i=0; i < nTotFields; i++)
					{
						sprintf(szField,"field%d=",i);
						if((p = stristr(szDefBuffer,szField))!=NULL)
						{
							p += strlen(szField);
							for(n=0; *p && *p!=',' && *p!='\r' && *p!='\n' && n < sizeof(szBuffer)-1; n++)
								szBuffer[n] = *p++;
							szBuffer[n] = '\0';
							
							// salta il campo interno utilizzato per la chiave primaria
							if(strcmp(szBuffer,"PRIMARY_KEY")!=0)
							{
								pElement = new char[strlen(szBuffer)+1];
								strcpy(pElement,szBuffer);
								m_pTableStruct[x].field = pElement;
								
								p++;
								for(n=0; *p && *p!=',' && *p!='\r' && *p!='\n' && n < sizeof(szBuffer)-1; n++)
									szBuffer[n] = *p++;
								szBuffer[n] = '\0';
								m_pTableStruct[x].type = szBuffer[0];
								
								p++;
								for(n=0; *p && *p!=',' && *p!='\r' && *p!='\n' && n < sizeof(szBuffer)-1; n++)
									szBuffer[n] = *p++;
								szBuffer[n] = '\0';
								m_pTableStruct[x].size = atoi(szBuffer);
								
								p++;
								for(n=0; *p && *p!=',' && *p!='\r' && *p!='\n' && n < sizeof(szBuffer)-1; n++)
									szBuffer[n] = *p++;
								szBuffer[n] = '\0';
								m_pTableStruct[x].dec = atoi(szBuffer);

								x++;
							}
						}
					}
				}

				// totale degli indici associati
				if((p = stristr(szDefBuffer,"totindex="))!=NULL)
				{
					p += 9;
					for(i=0; *p && isdigit(*p) && i < sizeof(szBuffer)-1; i++)
						szBuffer[i] = *p++;
					szBuffer[i] = '\0';
					nTotIndex = atoi(szBuffer);
				}
				else
					nTotIndex = 0;
				
				// crea e riempie la struttura per la definizione degli indici (i campi)
				if(nTotIndex > 0)
				{
					m_nTotIdx = nTotIndex+1;
					m_pIdxStruct = new CBASE_INDEX[m_nTotIdx];
					for(i=0; i < m_nTotIdx; i++)
					{
						m_pIdxStruct[i].file = NULL;
						m_pIdxStruct[i].name = NULL;
						m_pIdxStruct[i].field = NULL;
					}
					
					char szField[16];
					for(i=0; i < nTotIndex; i++)
					{
						sprintf(szField,"index%d=",i);
						if((p = stristr(szDefBuffer,szField))!=NULL)
						{
							p += strlen(szField);
							for(n=0; *p && *p!=',' && *p!='\r' && *p!='\n' && n < sizeof(szBuffer)-1; n++)
								szBuffer[n] = *p++;
							szBuffer[n] = '\0';
							
							pElement = new char[strlen(szBuffer)+(strlen(szDataPath)+1)+1];
							sprintf(pElement,"%s%s%s",szDataPath,szDataPath[0]!='\0' ? "\\" : "",szBuffer);
							m_pIdxStruct[i].file = pElement;
							
							p++;
							for(n=0; *p && *p!=',' && *p!='\r' && *p!='\n' && n < sizeof(szBuffer)-1; n++)
								szBuffer[n] = *p++;
							szBuffer[n] = '\0';
							
							pElement = new char[strlen(szBuffer)+1];
							strcpy(pElement,szBuffer);
							m_pIdxStruct[i].name = pElement;
							
							p++;
							for(n=0; *p && *p!=',' && *p!='\r' && *p!='\n' && n < sizeof(szBuffer)-1; n++)
								szBuffer[n] = *p++;
							szBuffer[n] = '\0';

							pElement = new char[strlen(szBuffer)+1];
							strcpy(pElement,szBuffer);
							m_pIdxStruct[i].field = pElement;
						}
					}
				}
			}

			deffile.Close();
			
			// imposta i parametri con i dati interni
			if(nTotFields!=-1 && nTotIndex!=-1)
			{
				pTableName = szTableName;
				pTable = m_pTableStruct;
				pIdx = m_pIdxStruct;
			}
		}

		if(nTotFields==-1 || nTotIndex==-1 || !pTable)
			return(FALSE);
	}

	// a partire da qui, per il codice deve essere indifferente se vengono passate le strutture per la
	// tabella/indici o se vengono create dinamicamente a partire dalla definizione caricata dal file (.def)

	// ricava il numero di campi e la dim. totale del record
	for(i = 0,nTotFields = 0,nTotSize = 0; pTable[i].field && *(pTable[i].field); i++)
	{
		nTotFields++;
		nTotSize += pTable[i].size;
	}

	// controlla i valori
	if(nTotFields <= 0 || nTotFields >= MAX_FIELDCOUNT || nTotSize <= 0)
		return(FALSE);

	// se la tabella prevede indici imposta la dimensione della chiave primaria, incrementando il numero campi e la dim. del record
	int nPrimaryKeySize = (pIdx==NULL ? 0 : MAX_PRIMARYKEY_SIZE);
	if(nPrimaryKeySize > 0)
	{
		nTotFields++;
		nTotSize += nPrimaryKeySize;
	}

	// tot. campi della tabella
	if((m_pDatabase->table.totfield = nTotFields) > 0)
	{
		// nome file per la tabella
		strcpyn(m_pDatabase->table.filename,pTableName,_MAX_PATH+1);

		// array per la definizione dei campi della tabella
		m_pDatabase->table.row = (ROW*) new char[sizeof(ROW) * nTotFields];
		memset((m_pDatabase->table.row),'\0',sizeof(ROW) * nTotFields);

		// buffer (interno) per il record della tabella
		m_pRecordBuffer = new char[nTotSize + 1];
		memset(m_pRecordBuffer,'\0',nTotSize + 1);

		// imposta la struttura della classe base per la definizione della tabella
		n = 0;
		nFieldNum = 0;

		// se la tabella prevede indici, inserisce il campo per la chiave primaria come primo elemento
		if(nPrimaryKeySize > 0)
		{
			m_pDatabase->table.row[0].num   = 0;
			m_pDatabase->table.row[0].ofs   = n;
			m_pDatabase->table.row[0].name  = "PRIMARY_KEY";
			m_pDatabase->table.row[0].type  = 'C';
			m_pDatabase->table.row[0].size  = nPrimaryKeySize;
			m_pDatabase->table.row[0].dec   = 0;
			m_pDatabase->table.row[0].value = m_pRecordBuffer;
			m_pDatabase->table.row[0].flags = 0;

			n += nPrimaryKeySize;
		}

		int nMaxFieldSize = 0;

		// definisce i campi della tabella
		for(i = 0; i < (nPrimaryKeySize > 0 ? nTotFields-1 : nTotFields); i++)
		{
			// numero del campo (salta il campo relativo alla chiave primaria)
			nFieldNum = i + (nPrimaryKeySize > 0 ? 1 : 0);

			// progressivo, offset e nome
			m_pDatabase->table.row[nFieldNum].num   = nFieldNum;
			m_pDatabase->table.row[nFieldNum].ofs   = n;
			m_pDatabase->table.row[nFieldNum].name  = pTable[i].field;
			
			// controlla il tipo
			switch(pTable[i].type)
			{
				// carattere (MAX_FIELDSIZE=1024)
				case 'C':
					nMaxFieldSize = FIELD_CHAR_MAXSIZE;
					break;
				
				// ora (6=hhmmss)
				case 'T':
					nMaxFieldSize = FIELD_TIME_MAXSIZE;
					break;
				
				// data (8=yyyymmdd)
				case 'D':
					nMaxFieldSize = FIELD_DATE_MAXSIZE;
					break;
				
				// data gmt (19=yyyymmddhhmmss+|-nnnn)
				case 'G':
					nMaxFieldSize = FIELD_GMTDATE_MAXSIZE;
					break;

				// logico (1=F|T)
				case 'L':
					nMaxFieldSize = FIELD_LOGICAL_MAXSIZE;
					break;
				
				// numerici (5,10,17)
				// short (5) - 32768
				case 'S':
					nMaxFieldSize = FIELD_SHORTINT_MAXSIZE;
					break;
				// int (10) - 2147483647
				case 'I':
					nMaxFieldSize = FIELD_INT_MAXSIZE;
					break;
				// long (10) - 2147483647L
				case 'N':
					nMaxFieldSize = FIELD_LONG_MAXSIZE;
					break;
				// unsigned long (10) - 0xffffffffUL
				case 'U':
					nMaxFieldSize = FIELD_UNSIGNEDLONG_MAXSIZE;
					break;
				// double (17=10+,+6)
				case 'R':
					nMaxFieldSize = FIELD_REAL_MAXSIZE;
					break;
				
				default:
					pTable[i].type = 'C';
					nMaxFieldSize = MAX_FIELDSIZE;
					break;
			}
			m_pDatabase->table.row[nFieldNum].type = pTable[i].type;

			// dimensione del campo
			pTable[i].size = pTable[i].size > nMaxFieldSize ? nMaxFieldSize : pTable[i].size;
			m_pDatabase->table.row[nFieldNum].size = pTable[i].size;

			// decimali (massimo 6 posizioni)
			pTable[i].dec = (pTable[i].type=='R') ? (pTable[i].dec > FIELD_REAL_MAXDECS ? FIELD_REAL_MAXDECS : pTable[i].dec) : 0;
			m_pDatabase->table.row[nFieldNum].dec = pTable[i].dec;

			// offset nel buffer (interno) per il valore del campo
			m_pDatabase->table.row[nFieldNum].value = m_pRecordBuffer + n;
			
			// flags
#ifdef _USE_FIELD_PICTURES
			m_pDatabase->table.row[nFieldNum].flags = CBASE_FLAG_NONE;
#else
			m_pDatabase->table.row[nFieldNum].flags = 0;
#endif

			n += pTable[i].size;
		}
	}

	// ricava il numero di indici
	if(pIdx)
	{
		for(i = 0,nTotIndex = 0; pIdx[i].file && *(pIdx[i].file); i++)
			nTotIndex++;
	}
	else
		nTotIndex = 0;

	// tot. indici della tabella
	if((m_pDatabase->table.totindex = nTotIndex) > 0)
	{
		// array per la definizione degli indici della tabella
		m_pDatabase->table.index = (INDEX*) new char[sizeof(INDEX) * nTotIndex];
		memset(m_pDatabase->table.index,'\0',sizeof(INDEX) * nTotIndex);

		// definisce gli indici della tabella
		for(i = 0; pIdx[i].file && *(pIdx[i].file); i++)
		{
			// nome del file indice
			strcpyn(m_pDatabase->table.index[i].filename,pIdx[i].file,_MAX_PATH+1);

			// ricava il numero del campo relativo alla chiave
			for(n = 0; n < nTotFields; n++)
			{
				if(strcmp(pIdx[i].field,pTable[n].field)==0)
					break;
			}

			if(n==nTotFields)
				n = -1; 

			// nome indice, nome del campo relativo, numero del campo relativo
			if(n >= 0)
			{
				m_pDatabase->table.index[i].name      = pIdx[i].name;
				m_pDatabase->table.index[i].fieldname = pTable[n].field;
				m_pDatabase->table.index[i].fieldnum  = n + 1; // il + 1 serve per saltare il campo relativo alla chiave primaria
			}
			else
			{
				m_pDatabase->table.index[i].name      = pTable[0].field;
				m_pDatabase->table.index[i].fieldname = pTable[0].field;
				m_pDatabase->table.index[i].fieldnum  = 0;
			}
		}
	}

	// salva la definizione della tabella nel file relativo (.def)
	{
		CBinFile def;
		char* p;
		char szDir[_MAX_PATH+1];
		_snprintf(szDir,sizeof(szDir)-1,"%s\\%s.def",GetTablePath(),GetTableName());
		
		if(def.Create(szDir))
		{
			int i;
			char szBuffer[DEF_MAX_SIZE+1];
			
			p = (char*)strrchr(pTableName,'\\');
			if(p)
				p++;
			else
				p = (char*)pTableName;
			_snprintf(szBuffer,sizeof(szBuffer)-1,"tablename=%s\r\n",p);
			def.Write(szBuffer,strlen(szBuffer));

			_snprintf(szBuffer,sizeof(szBuffer)-1,"totfields=%d\r\n",m_pDatabase->table.totfield);
			def.Write(szBuffer,strlen(szBuffer));

			for(i=0; i < m_pDatabase->table.totfield; i++)
			{
				_snprintf(szBuffer,
						sizeof(szBuffer)-1,
						"field%d=%s,%c,%d,%d,%d\r\n",
						i,
						m_pDatabase->table.row[i].name,
						m_pDatabase->table.row[i].type,
						m_pDatabase->table.row[i].size,
						m_pDatabase->table.row[i].dec,
						m_pDatabase->table.row[i].num
						);

				def.Write(szBuffer,strlen(szBuffer));
			}

			_snprintf(szBuffer,sizeof(szBuffer)-1,"totindex=%d\r\n",m_pDatabase->table.totindex);
			def.Write(szBuffer,strlen(szBuffer));

			for(i=0; i < m_pDatabase->table.totindex; i++)
			{
				p = strrchr(m_pDatabase->table.index[i].filename,'\\');
				if(!p)
					p = m_pDatabase->table.index[i].filename;
				else
					p++;
				_snprintf(szBuffer,
						sizeof(szBuffer)-1,
						"index%d=%s,%s,%s,%d\r\n",
						i,
						p,
						m_pDatabase->table.index[i].name,
						m_pDatabase->table.index[i].fieldname,
						m_pDatabase->table.index[i].fieldnum
						);

				def.Write(szBuffer,strlen(szBuffer));
			}

			def.Close();
		}
	}

	// valore di ritorno del costruttore
	SetValid(TRUE);

	return(TRUE);
}

/*
	Open()
	
	Apre la tabella e gli indici associati, recuperando il primo record.
*/
BOOL CBase::Open(void)
{
	BOOL bIsOpen = FALSE;

	if(IsValid())
	{
		// apre la tabella e posiziona sul primo record (recuperando i dati)
		if((bIsOpen = m_pBerkeleyDB->IsOpen())==FALSE)
			if(m_pBerkeleyDB->Open()==DB_NO_ERROR)
			{
				bIsOpen = TRUE;
				GetFirst();
			}
	}
		
	return(bIsOpen);
}

/*
	Zap()

	Elimina tutti i record della tabella.
*/
BOOL CBase::Zap(void)
{
	BOOL bZap = FALSE;

	if(IsValid())
	{
		// azzera la taballa ricreandola
		if(m_pBerkeleyDB->IsOpen())
			m_pBerkeleyDB->Close();

		bZap = m_pBerkeleyDB->Create()==DB_NO_ERROR;
		
		if(bZap)
			m_pDatabase->table.stat.bof = m_pDatabase->table.stat.eof = TRUE;
	}

	return(bZap);
}

/*
	Close()

	Chiude la tabella e rilascia le risorse associate.
*/
void CBase::Close(void)
{
	if(IsValid())
		if(m_pBerkeleyDB->IsOpen())
			m_pBerkeleyDB->Close();
}

/*
	Load()

	Carica la tabella con quanto contenuto nel file SDF.
	Assume che il file SDF contenga solo dati, senza il valore della chiave primaria.
*/
int CBase::Load(const char* pFileName)
{
	int nRet = DB_RETCODE_EINVALIDHANDLE;

	if(IsValid())
	{
		CTextFile textfile;
		int i,nPrimaryKeySize,nRecordSize;
#ifdef _USE_FIELD_PICTURES
		char szField[MAX_KEYSIZE + 1];
		int nFieldLen;
#endif

		// la tabella non deve essere gia' aperta
		if(m_pBerkeleyDB->IsOpen())
			return(m_pBerkeleyDB->SetLastError(DB_RETCODE_EALREADYOPEN));

		// apre la tabella in proprio
		if(m_pBerkeleyDB->Open()!=DB_NO_ERROR)
			return(m_pBerkeleyDB->SetLastError(DB_RETCODE_EOPENTABLE));

		// dimensione della chiave primaria
		if(m_pDatabase->table.totindex > 0)
			nPrimaryKeySize = m_pBerkeleyDB->GetPrimaryKeySize();
		else
			nPrimaryKeySize = 0;

		// dimensione del record (salta la chiave primaria)
		nRecordSize = 0;
		for(i = m_pDatabase->table.totindex > 0 ? 1 : 0; i < m_pDatabase->table.totfield; i++)
			nRecordSize += m_pDatabase->table.row[i].size;
		
		if(nRecordSize <= 0)
		{
			m_pBerkeleyDB->Close();
			return(m_pBerkeleyDB->SetLastError(DB_RETCODE_EINVALIDFIELDSIZE));
		}

		// buffer per la lettura del record dal file sdf (dim. record + crlf + null)
		char* pBuffer = new char[nRecordSize + 2 + 1];
		if(!pBuffer)
		{
			m_pBerkeleyDB->Close();
			return(m_pBerkeleyDB->SetLastError(DB_RETCODE_EALLOCFAILURE));
		}

		// chiude la tabella, la elimina (insieme agli indici relativi) e la ricrea per il caricamento
		m_pBerkeleyDB->Close();

		remove(m_pDatabase->table.filename);

		if(m_pDatabase->table.totindex > 0)
			for(i = 0; i < m_pDatabase->table.totindex; i++)
				remove(m_pDatabase->table.index[i].filename);

		if(m_pBerkeleyDB->Open()!=DB_NO_ERROR)
			return(m_pBerkeleyDB->SetLastError(DB_RETCODE_EOPENTABLE));

		// carica la tabella con il contenuto del file SDF
		if(textfile.Open(pFileName))
		{
			// azzera il buffer interno
			memset(m_pRecordBuffer,' ',nRecordSize + nPrimaryKeySize + 1);

			// legge una linea dal file
			while(textfile.ReadLine(pBuffer,nRecordSize + 2 + 1)!=FILE_EOF)
			{
				// passa la linea nel buffer interno (salta la chiave primaria)
				memcpy(m_pRecordBuffer + nPrimaryKeySize,pBuffer,nRecordSize);

#ifdef _USE_FIELD_PICTURES
				// filtra il contenuto del campo secondo la picture corrente
				for(i = 0; i < m_pDatabase->table.totfield; i++)
				{
					if(m_pDatabase->table.row[i].flags!=CBASE_FLAG_NONE)
					{
						memset(szField,'\0',sizeof(szField));
						memcpy(szField,m_pDatabase->table.row[i].value,m_pDatabase->table.row[i].size);
						nFieldLen = strlen(szField);
						
						SetFieldFormat(szField,nFieldLen,m_pDatabase->table.row[i].flags);
						
						memset(m_pDatabase->table.row[i].value,' ',m_pDatabase->table.row[i].size);
						memcpy(m_pDatabase->table.row[i].value,szField,nFieldLen);
					}
				}
#endif

				// inserisce il record nella tabella
				Insert();
			}

			textfile.Close();
		}
		else
		{
			m_pBerkeleyDB->Close();
			return(m_pBerkeleyDB->SetLastError(DB_RETCODE_EOPENFAILURE));
		}

		delete [] pBuffer;
		
		m_pBerkeleyDB->Close();

		nRet = DB_OK;
	}

	return(nRet);
}

/*
	Dump()

	Scarica nel file SDF quanto contenuto nella tabella.
	Il secondo parametro serve per stampare il valore della chiave primaria.
	Notare che Load() assume che il file SDF contenga solo dati, senza il 
	valore della chiave primaria.
*/
int CBase::Dump(const char* pFileName,BOOL bDumpPrimaryKey/*=FALSE*/,char cSeparator/*=0*/)
{
	// controlla il codice di ritorno del costruttore
	if(!IsValid())
		return(DB_RETCODE_EINVALIDHANDLE);

	CTextFile textfile;
	int nRecordSize;
	int nPrimaryKeySize;
	int i;

	int nFieldCount = GetFieldCount();
	char szBuffer[MAX_FIELDSIZE+1];

	// la tabella deve essere gia' aperta
	if(!m_pBerkeleyDB->IsOpen())
		return(m_pBerkeleyDB->SetLastError(DB_RETCODE_ETABLENOTOPEN));

	// dimensione della chiave primaria
	if(m_pDatabase->table.totindex > 0)
		nPrimaryKeySize = m_pBerkeleyDB->GetPrimaryKeySize();
	else
		nPrimaryKeySize = 0;

	// dimensione del record (non include la dimensione della chiave primaria)
	nRecordSize = 0;
	for(i = m_pDatabase->table.totindex > 0 ? 1 : 0; i < m_pDatabase->table.totfield; i++)
		nRecordSize += m_pDatabase->table.row[i].size;
	
	if(nRecordSize <= 0)
		return(m_pBerkeleyDB->SetLastError(DB_RETCODE_EINVALIDFIELDSIZE));

	remove(pFileName);

	// scarica il contenuto della tabella nel file SDF
	if(textfile.Create(pFileName))
	{
		memset(m_pRecordBuffer,'\0',nRecordSize + nPrimaryKeySize + 1);

		// include o salta la chiave primaria
		char* p = bDumpPrimaryKey ? m_pRecordBuffer : m_pRecordBuffer + nPrimaryKeySize;
		int   n = bDumpPrimaryKey ? nRecordSize + nPrimaryKeySize : nRecordSize;

		if(GoTop())
		{
			while(!Eof())
			{
				if(cSeparator!=0)
				{
					for(i = 0; i < nFieldCount; i++)
					{
						textfile.Write(GetPrimaryKeyValue(),nPrimaryKeySize);
						textfile.Write(&cSeparator,1);
						
						strcpyn(szBuffer,GetFieldRaw(i),sizeof(szBuffer));
						strrtrim(szBuffer);
						strltrim(szBuffer);
						textfile.Write(szBuffer,strlen(szBuffer));
						if(i!=nFieldCount-1)
							textfile.Write(&cSeparator,1);
						else
							textfile.Write("\r\n",2);
					}
				}
				else
				{
					textfile.WriteLine(p,n);
				}

				GetNext();
			}
		}

		textfile.Close();
	}
	else
		return(m_pBerkeleyDB->SetLastError(DB_RETCODE_EOPENFAILURE));

	return(DB_OK);
}

/*
	Insert()

	Inserisce il record corrente nella tabella.
*/
BOOL	CBase::Insert(void)
{
	return(IsValid() ? m_pBerkeleyDB->Insert()==0 : FALSE);
}

/*
	Delete()

	Elimina il record corrente dalla tabella.
*/
BOOL CBase::Delete(void)
{
	return(IsValid() ? m_pBerkeleyDB->Delete()==0 : FALSE);
}

/*
	Replace()

	Sostituisce il record corrente della tabella.
*/
BOOL	CBase::Replace(int nFieldNum,const char* pOldValue,const char* pNewValue)
{
	BOOL bReplaced = FALSE;
	
	if(IsValid())
	{
		// aggiorna il campo con il nuovo valore
		PutField(nFieldNum,pNewValue);

		// sostituisce il campo della tabella (se la tabella prevede indici, salta la chiave primaria)
		bReplaced = m_pBerkeleyDB->Replace(m_pDatabase->table.totindex > 0 ? nFieldNum + 1 : nFieldNum,pOldValue,pNewValue)==0;
	}

	return(bReplaced);
}
BOOL	CBase::Replace(int nFieldNum,BOOLEAN bOldValue,BOOLEAN bNewValue)
{
	BOOL bReplaced = FALSE;
	
	if(IsValid())
	{
		char szOld[3];
		char szNew[3];
		
		// aggiorna il campo con il nuovo valore
		PutField(nFieldNum,bNewValue);
		
		// trasforma da boolean a char
		FormatField(szOld,sizeof(szOld),bOldValue);
		FormatField(szNew,sizeof(szNew),bNewValue);
		
		// sostituisce il campo della tabella (se la tabella prevede indici, salta la chiave primaria)
		bReplaced = m_pBerkeleyDB->Replace(m_pDatabase->table.totindex > 0 ? nFieldNum + 1 : nFieldNum,szOld,szNew)==0;
	}

	return(bReplaced);
}
BOOL	CBase::Replace(int nFieldNum,CDateTime& pOldValue,CDateTime& pNewValue)
{
	BOOL bReplaced = FALSE;
	
	if(IsValid())
	{
		char szOld[32];
		char szNew[32];
		
		// aggiorna il campo con il nuovo valore
		PutField(nFieldNum,pNewValue);
		
		// deve ricavare il tipo del campo data (T,D,G)
		char cType = 0;

		// se la tabella prevede indici salta il primo campo (relativo alla chiave primaria)
		int n = m_pDatabase->table.totindex > 0 ? nFieldNum + 1 : nFieldNum;
		for(int i = 0; i < m_pDatabase->table.totfield; i++)
			if(i==n)
			{
				cType = m_pDatabase->table.row[i].type;
				break;
			}
		
		// trasforma da data a char
		FormatField(szOld,sizeof(szOld),pOldValue,cType);
		FormatField(szNew,sizeof(szNew),pNewValue,cType);
		
		// sostituisce il campo della tabella (se la tabella prevede indici, salta la chiave primaria)
		bReplaced = m_pBerkeleyDB->Replace(m_pDatabase->table.totindex > 0 ? nFieldNum + 1 : nFieldNum,szOld,szNew)==0;
	}

	return(bReplaced);
}
BOOL	CBase::Replace(int nFieldNum,short int nOldValue,short int nNewValue)
{
	BOOL bReplaced = FALSE;
	
	if(IsValid())
	{
		char szOld[32];
		char szNew[32];
		
		// aggiorna il campo con il nuovo valore
		PutField(nFieldNum,nNewValue);

		// da numero a short int
		NUMBER Ns = {0};
		Ns.shortint = nOldValue;		
		FormatField(szOld,sizeof(szOld),Ns,shortint_type,nFieldNum);
		Ns.shortint = nNewValue;		
		FormatField(szNew,sizeof(szNew),Ns,shortint_type,nFieldNum);
		
		// sostituisce il campo della tabella (se la tabella prevede indici, salta la chiave primaria)
		bReplaced = m_pBerkeleyDB->Replace(m_pDatabase->table.totindex > 0 ? nFieldNum + 1 : nFieldNum,szOld,szNew)==0;
	}

	return(bReplaced);
}
BOOL	CBase::Replace(int nFieldNum,int nOldValue,int nNewValue)
{
	BOOL bReplaced = FALSE;
	
	if(IsValid())
	{
		char szOld[32];
		char szNew[32];

		// aggiorna il campo con il nuovo valore
		PutField(nFieldNum,nNewValue);

		// da numero a int
		NUMBER Ni = {0};
		Ni.integer = nOldValue;		
		FormatField(szOld,sizeof(szOld),Ni,integer_type,nFieldNum);
		Ni.integer = nNewValue;		
		FormatField(szNew,sizeof(szNew),Ni,integer_type,nFieldNum);
		
		// sostituisce il campo della tabella (se la tabella prevede indici, salta la chiave primaria)
		bReplaced = m_pBerkeleyDB->Replace(m_pDatabase->table.totindex > 0 ? nFieldNum + 1 : nFieldNum,szOld,szNew)==0;
	}

	return(bReplaced);
}
BOOL	CBase::Replace(int nFieldNum,long nOldValue,long nNewValue)
{
	BOOL bReplaced = FALSE;
	
	if(IsValid())
	{
		char szOld[32];
		char szNew[32];

		// aggiorna il campo con il nuovo valore
		PutField(nFieldNum,nNewValue);

		// da numero a long
		NUMBER Nl = {0};
		Nl.longint = nOldValue;		
		FormatField(szOld,sizeof(szOld),Nl,longint_type,nFieldNum);
		Nl.longint = nNewValue;		
		FormatField(szNew,sizeof(szNew),Nl,longint_type,nFieldNum);

		// sostituisce il campo della tabella (se la tabella prevede indici, salta la chiave primaria)
		bReplaced = m_pBerkeleyDB->Replace(m_pDatabase->table.totindex > 0 ? nFieldNum + 1 : nFieldNum,szOld,szNew)==0;
	}

	return(bReplaced);
}
BOOL	CBase::Replace(int nFieldNum,unsigned long nOldValue,unsigned long nNewValue)
{
	BOOL bReplaced = FALSE;
	
	if(IsValid())
	{
		char szOld[32];
		char szNew[32];

		// aggiorna il campo con il nuovo valore
		PutField(nFieldNum,nNewValue);

		// da numero a unsigned long
		NUMBER Nu = {0};
		Nu.ulongint = nOldValue;		
		FormatField(szOld,sizeof(szOld),Nu,ulongint_type,nFieldNum);
		Nu.ulongint = nNewValue;		
		FormatField(szNew,sizeof(szNew),Nu,ulongint_type,nFieldNum);

		// sostituisce il campo della tabella (se la tabella prevede indici, salta la chiave primaria)
		bReplaced = m_pBerkeleyDB->Replace(m_pDatabase->table.totindex > 0 ? nFieldNum + 1 : nFieldNum,szOld,szNew)==0;
	}

	return(bReplaced);
}
BOOL	CBase::Replace(int nFieldNum,double nOldValue,double nNewValue)
{
	BOOL bReplaced = FALSE;
	
	if(IsValid())
	{
		char szOld[32];
		char szNew[32];

		// aggiorna il campo con il nuovo valore
		PutField(nFieldNum,nNewValue);

		// da numero a double
		NUMBER Nr = {0};
		Nr.realnum = nOldValue;		
		FormatField(szOld,sizeof(szOld),Nr,realnum_type,nFieldNum);
		Nr.realnum = nNewValue;		
		FormatField(szNew,sizeof(szNew),Nr,realnum_type,nFieldNum);

		// sostituisce il campo della tabella (se la tabella prevede indici, salta la chiave primaria)
		bReplaced = m_pBerkeleyDB->Replace(m_pDatabase->table.totindex > 0 ? nFieldNum + 1 : nFieldNum,szOld,szNew)==0;
	}

	return(bReplaced);
}

/*
	Bof()

	Verifica se si trova all'inizio della tabella.
*/
BOOL CBase::Bof(void) const
{
	return(IsValid() ? m_pDatabase->table.stat.bof : TRUE);
}

/*
	Eof()

	Verifica se si trova alla fine della tabella.
*/
BOOL CBase::Eof(void) const
{
	return(IsValid() ? m_pDatabase->table.stat.eof : TRUE);
}

/*
	GoTop()

	Posiziona all'inizio della tabella, recuperando il record corrente.
	Se non viene specificato altrimenti, utilizza l'indice corrente.
*/
BOOL CBase::GoTop(int nIndex/* = -1*/)
{
	return(IsValid() ? CBase::GetFirst(nIndex) : FALSE);
}

/*
	GoBottom()

	Posiziona alla fine della tabella, recuperando il record corrente.
	Se non viene specificato altrimenti, utilizza l'indice corrente.
*/
BOOL CBase::GoBottom(int nIndex/* = -1*/)
{
	return(IsValid() ? CBase::GetLast(nIndex) : FALSE);
}

/*
	GetCurrent()

	Recupera il record corrente della tabella.
	Se non viene specificato altrimenti, utilizza l'indice corrente.
*/
BOOL CBase::GetCurrent(int nIndex/*=-1*/)
{
	int nCode = DB_RETCODE_NOTFOUND;

	if(IsValid())
	{
		nCode = DB_OK;

		// imposta l'indice
		if(nIndex >= 0)
			SetIndex(nIndex);

		// ricava il record corrente
		if(nCode==DB_OK)
		{
			m_szPrimaryKeyValue[0] = '\0';

			switch((nCode = m_pBerkeleyDB->Get(DB_SEARCH,DB_SET)))
			{
				case DB_NO_ERROR:
				case DB_OK:
					nCode = DB_OK;
					m_pDatabase->table.stat.bof = m_pDatabase->table.stat.eof = FALSE;
					strcpyn(m_szPrimaryKeyValue,m_pDatabase->table.row[0].value,sizeof(m_szPrimaryKeyValue));
					break;

				case DB_RETCODE_BOF:
					m_pDatabase->table.stat.bof = TRUE;
					break;

				case DB_RETCODE_EOF:
					m_pDatabase->table.stat.eof = TRUE;
					break;

				case DB_RETCODE_NOTFOUND:
					m_pDatabase->table.stat.bof = m_pDatabase->table.stat.eof = TRUE;
					break;
			}

			if(nCode!=DB_OK)
				m_pBerkeleyDB->SetLastError(nCode);
		}
	}

	return(nCode==DB_OK);
}

/*
	GetFirst()

	Recupera il primo record della tabella.
	Se non viene specificato altrimenti, utilizza l'indice corrente.
*/
BOOL CBase::GetFirst(int nIndex/*=-1*/)
{
	int nCode = DB_RETCODE_NOTFOUND;

	if(IsValid())
	{
		nCode = DB_OK;

		// imposta l'indice
		if(nIndex >= 0)
			SetIndex(nIndex);

		// ricava il primo record
		if(nCode==DB_OK)
		{
			m_szPrimaryKeyValue[0] = '\0';

			switch((nCode = m_pBerkeleyDB->GetFirst()))
			{
				case DB_NO_ERROR:
				case DB_OK:
					nCode = DB_OK;
					m_pDatabase->table.stat.bof = m_pDatabase->table.stat.eof = FALSE;
					strcpyn(m_szPrimaryKeyValue,m_pDatabase->table.row[0].value,sizeof(m_szPrimaryKeyValue));
					break;

				case DB_RETCODE_NOTFOUND:
				case DB_RETCODE_BOF:
				case DB_RETCODE_EOF:
					m_pDatabase->table.stat.bof = m_pDatabase->table.stat.eof = TRUE;
					break;
			}
			
			if(nCode!=DB_OK)
				m_pBerkeleyDB->SetLastError(nCode);
		}
	}

	return(nCode==DB_OK);
}

/*
	GetNext()

	Recupera il record sucessivo della tabella.
	Se non viene specificato altrimenti, utilizza l'indice corrente.
*/
BOOL CBase::GetNext(int nIndex/*=-1*/)
{
	int nCode = DB_RETCODE_NOTFOUND;

	if(IsValid())
	{
		nCode = DB_OK;

		// imposta l'indice
		if(nIndex >= 0)
			SetIndex(nIndex);

		// ricava il record successivo
		if(nCode==DB_OK)
		{
			m_szPrimaryKeyValue[0] = '\0';

			switch((nCode = m_pBerkeleyDB->GetNext()))
			{
				case DB_NO_ERROR:
				case DB_OK:
					nCode = DB_OK;
					m_pDatabase->table.stat.bof = m_pDatabase->table.stat.eof = FALSE;
					strcpyn(m_szPrimaryKeyValue,m_pDatabase->table.row[0].value,sizeof(m_szPrimaryKeyValue));
					break;

				case DB_RETCODE_BOF:
					nCode = DB_RETCODE_BOF;
					m_pDatabase->table.stat.bof = TRUE;
					break;

				case DB_RETCODE_NOTFOUND:
				case DB_RETCODE_EOF:
					nCode = DB_RETCODE_EOF;
					m_pDatabase->table.stat.eof = TRUE;
					break;
			}
			
			if(nCode!=DB_OK)
				m_pBerkeleyDB->SetLastError(nCode);
		}
	}

	return(nCode==DB_OK);
}

/*
	GetPrev()

	Recupera il record precedente della tabella.
	Se non viene specificato altrimenti, utilizza l'indice corrente.
*/
BOOL CBase::GetPrev(int nIndex/*=-1*/)
{
	int nCode = DB_RETCODE_NOTFOUND;

	if(IsValid())
	{
		nCode = DB_OK;

		// imposta l'indice
		if(nIndex >= 0)
			SetIndex(nIndex);

		// ricava il record precedente
		if(nCode==DB_OK)
		{
			m_szPrimaryKeyValue[0] = '\0';

			switch((nCode = m_pBerkeleyDB->GetPrev()))
			{
				case DB_NO_ERROR:
				case DB_OK:
					nCode = DB_OK;
					m_pDatabase->table.stat.bof = m_pDatabase->table.stat.eof = FALSE;
					strcpyn(m_szPrimaryKeyValue,m_pDatabase->table.row[0].value,sizeof(m_szPrimaryKeyValue));
					break;

				case DB_RETCODE_NOTFOUND:
				case DB_RETCODE_BOF:
					m_pDatabase->table.stat.bof = TRUE;
					break;

				case DB_RETCODE_EOF:
					m_pDatabase->table.stat.eof = TRUE;
					break;
			}
			
			if(nCode!=DB_OK)
				m_pBerkeleyDB->SetLastError(nCode);
		}
	}

	return(nCode==DB_OK);
}

/*
	GetLast()

	Recupera l'ultimo record della tabella.
	Se non viene specificato altrimenti, utilizza l'indice corrente.
*/
BOOL CBase::GetLast(int nIndex/*=-1*/)
{
	int nCode = DB_RETCODE_NOTFOUND;

	if(IsValid())
	{
		nCode = DB_OK;

		// imposta l'indice
		if(nIndex >= 0)
			SetIndex(nIndex);

		// ricava l'ultimo record
		if(nCode==DB_OK)
		{
			m_szPrimaryKeyValue[0] = '\0';

			switch((nCode = m_pBerkeleyDB->GetLast()))
			{
				case DB_NO_ERROR:
				case DB_OK:
					nCode = DB_OK;
					m_pDatabase->table.stat.bof = m_pDatabase->table.stat.eof = FALSE;
					strcpyn(m_szPrimaryKeyValue,m_pDatabase->table.row[0].value,sizeof(m_szPrimaryKeyValue));
					break;

				case DB_RETCODE_NOTFOUND:
				case DB_RETCODE_BOF:
				case DB_RETCODE_EOF:
					m_pDatabase->table.stat.bof = m_pDatabase->table.stat.eof = TRUE;
					break;
			}
			
			if(nCode!=DB_OK)
				m_pBerkeleyDB->SetLastError(nCode);
		}
	}

	return(nCode==DB_OK);
}

/*
	Seek()

	Cerca la chiave (stringa) con l'indice specificato.
	Se si usa con un indice numerico direttamente, senza chiamare il membro relativo al tipo numerico, non formattare a spazi
	il numero nel buffer per la chiave da ricercare, dato che provvede in proprio a formattare a seconda del tipo (stringhe a
	sx e numeri a dx).
	I membri a seguire permettono la chiamata con il valore numerico, senza dover effettuare la formattazione.
	Da usare solo se la tabella prevede indici.
	Se non viene specificato altrimenti, utilizza l'indice corrente.
*/
BOOL CBase::Seek(const char* pValue,int nIndex/*=-1*/)
{
	int nCode = DB_RETCODE_NOTFOUND;

	if(IsValid())
	{
		int	nKeySize;
		char	szKey[MAX_KEYSIZE+1];
		int	nFieldNum,nFieldSize,nFieldDec;
		char	cFieldType;

		nCode = DB_OK;

		// imposta l'indice corrente
		if(nIndex >= 0)
			SetIndex(nIndex);

		// imposta la chiave, allineando a seconda del tipo del campo (stringhe a sx, numeri a dx)
		m_szPrimaryKeyValue[0] = '\0';
		memset(szKey,' ',sizeof(szKey));
		nKeySize = strlen(pValue);
		nKeySize = nKeySize >= sizeof(szKey) ? sizeof(szKey) : nKeySize;

		nFieldNum  = m_pDatabase->table.index[GetIndex()].fieldnum;
		nFieldSize = m_pDatabase->table.row[nFieldNum].size;
		nFieldDec  = m_pDatabase->table.row[nFieldNum].dec;
		cFieldType = m_pDatabase->table.row[nFieldNum].type;

		switch(cFieldType)
		{
			case 'S':
			case 'I':
			case 'N':
			case 'U':
			case 'R':
				memcpy(szKey + (nFieldSize-nKeySize),pValue,nKeySize);
				break;
			default:
				memcpy(szKey,pValue,nKeySize);
				break;
		}

		// imposta il campo (chiave) per la ricerca
		m_pBerkeleyDB->PutKey(szKey,GetIndex());

		// ricava il record relativo alla chiave
		nCode = m_pBerkeleyDB->Get(DB_SEARCH,m_bSoftseek ? DB_SET_RANGE : DB_SET);

		// esito ricerca
		switch(nCode)
		{
			case DB_NO_ERROR:
			case DB_OK:
				nCode = DB_OK;
				m_pDatabase->table.stat.bof = m_pDatabase->table.stat.eof = FALSE;
				strcpyn(m_szPrimaryKeyValue,m_pDatabase->table.row[0].value,sizeof(m_szPrimaryKeyValue));
				break;

			case DB_RETCODE_BOF:
				m_pDatabase->table.stat.bof = TRUE;
				break;

			case DB_RETCODE_EOF:
				m_pDatabase->table.stat.eof = TRUE;
				break;
		}
		
		if(nCode!=DB_OK)
			m_pBerkeleyDB->SetLastError(nCode);
	}

	return(nCode==DB_OK);
}
BOOL CBase::Seek(short int nValue,int nIndex/*=-1*/)
{
	char	szKey[MAX_KEYSIZE+1];
	_snprintf(szKey,sizeof(szKey)-1,"%d",nValue);
	return(Seek(szKey,nIndex));
}
BOOL CBase::Seek(int nValue,int nIndex/*=-1*/)
{
	char	szKey[MAX_KEYSIZE+1];
	_snprintf(szKey,sizeof(szKey)-1,"%d",nValue);
	return(Seek(szKey,nIndex));
}
BOOL CBase::Seek(long nValue,int nIndex/*=-1*/)
{
	char	szKey[MAX_KEYSIZE+1];
	_snprintf(szKey,sizeof(szKey)-1,"%ld",nValue);
	return(Seek(szKey,nIndex));
}
BOOL CBase::Seek(unsigned long nValue,int nIndex/*=-1*/)
{
	char	szKey[MAX_KEYSIZE+1];
	_snprintf(szKey,sizeof(szKey)-1,"%lu",nValue);
	return(Seek(szKey,nIndex));
}
BOOL CBase::Seek(double nValue,int nIndex/*=-1*/)
{
	// deve ricavare il numero di decimali del campo per la formattazione
	char	szKey[MAX_KEYSIZE+1];
	int nIndexNum = GetIndex();
	int nFieldNum = -1;
	int nDec = 0;

	// cerca il campo della tabella relativo all'indice (corrente)
	for(int i = 0; i < m_pDatabase->table.totindex; i++)
		if(i==nIndexNum)
		{
			nFieldNum = m_pDatabase->table.index[i].fieldnum;
			break;
		}

	// ricava il numero di decimali del campo
	if(nFieldNum!=-1)
		nDec = m_pDatabase->table.row[nFieldNum].dec;

	// formatta il valore secondo i decimali del campo
	char szFormat[10];
	_snprintf(szFormat,sizeof(szFormat)-1,"%c.%d%c",'%',nDec,'f');
	_snprintf(szKey,sizeof(szKey)-1,szFormat,nValue);

	return(Seek(szKey,nIndex));
}

/*
	SeekPrimaryKey()

	Cerca la chiave primaria.
	La tabella deve prevedere indici.
*/
BOOL CBase::SeekPrimaryKey(const char* pPrimaryKey)
{
	return(IsValid() ? m_pBerkeleyDB->GetPrimaryKey(pPrimaryKey)==0 : FALSE);
}

/*
	Find()

	Cerca il record relativo alla chiave.
	Da usare quando la tabella non prevede indici (notare che in tal caso viene considerato come chiave
	l'intero record).
*/
BOOL CBase::Find(const char* pValue)
{
	int nCode = DB_RETCODE_NOTFOUND;

	if(IsValid())
	{
		int	nKeySize;
		char	szKey[MAX_KEYSIZE+1];
		int	nFieldNum,nFieldSize;
		char	cFieldType;

		nCode = DB_OK;

		// imposta la chiave, allineando a seconda del tipo del campo (stringhe a sx, numeri a dx)
		m_szPrimaryKeyValue[0] = '\0';
		memset(szKey,' ',sizeof(szKey));
		nKeySize = strlen(pValue);
		nKeySize = nKeySize >= sizeof(szKey) ? sizeof(szKey) : nKeySize;

		nFieldNum  = 0;
		nFieldSize = m_pDatabase->table.row[nFieldNum].size;
		cFieldType = m_pDatabase->table.row[nFieldNum].type;

		switch(cFieldType)
		{
			case 'S':
			case 'I':
			case 'N':
			case 'U':
			case 'R':
				memcpy(szKey + (nFieldSize-nKeySize),pValue,nKeySize);
				break;
			default:
				memcpy(szKey,pValue,nKeySize);
				break;
		}

		// imposta il campo (chiave) per la ricerca
		m_pBerkeleyDB->PutKey(szKey,0);

		// ricava il record relativo alla chiave
		nCode = m_pBerkeleyDB->Get(DB_SEARCH,DB_SET);

		// esito ricerca
		switch(nCode)
		{
			case DB_NO_ERROR:
			case DB_OK:
				nCode = DB_OK;
				m_pDatabase->table.stat.bof = m_pDatabase->table.stat.eof = FALSE;
				break;

			case DB_RETCODE_BOF:
				m_pDatabase->table.stat.bof = TRUE;
				break;

			case DB_RETCODE_EOF:
				m_pDatabase->table.stat.eof = TRUE;
				break;
		}
		
		if(nCode!=DB_OK)
			m_pBerkeleyDB->SetLastError(nCode);
	}

	return(nCode==DB_OK);
}

/*
	GetIndex()

	Ricava l'indice corrente (a base 0), o -1 per errore.
*/
int CBase::GetIndex(void)
{
	int nCurrentIndex = -1;

	if(IsValid())
		nCurrentIndex = m_pBerkeleyDB->GetCursorNumber();

	return(nCurrentIndex);
}

/*
	SetIndex()

	Imposta l'indice corrente (a base 0), restituendo il valore anteriore o -1 per errore.
*/
int CBase::SetIndex(int nIndex)
{
	int nCurrentIndex = -1;

	if(IsValid())
	{
		nCurrentIndex = m_pBerkeleyDB->GetCursorNumber();
		m_pBerkeleyDB->SetCursor(nIndex);
	}

	return(nCurrentIndex);
}

/*
	ResetIndex()

	Resetta l'indice corrente (reimposta sulla chiave primaria).
*/
void CBase::ResetIndex(void)
{
	if(IsValid())
		m_pBerkeleyDB->ResetCursor();
}

/*
	Reindex()

	Ricrea l'indice specificato, se viene passato -1 ricrea tutti gli indici.
*/
BOOL CBase::Reindex(int nIndex/*=-1*/)
{
	return(IsValid() ? m_pBerkeleyDB->Reindex(nIndex)==0 : FALSE);
}

/*
	CheckIndex()

	Verifica l'indice specificato, se viene passato -1 verifica tutti gli indici.
*/
BOOL CBase::CheckIndex(int nIndex/* = -1*/)
{
	return(IsValid() ? m_pBerkeleyDB->CheckIndex(nIndex)==0 : FALSE);
}

/*
	GetIndexCount()

	Restituisce il numero di indici della tabella, o -1 per errore.
*/
int CBase::GetIndexCount(void) const
{
	return(IsValid() ? m_pDatabase->table.totindex : -1);
}

/*
	GetIndexNames()

	Restituisce i nomi degli indici della tabella (da chiamare in un ciclo fino a che non restituisce NULL).
*/
const char* CBase::GetIndexNames(void)
{
	char* pIndexName = NULL;

	if(IsValid())
	{
		static int nIndexNum = 0;

		if(nIndexNum < m_pDatabase->table.totindex)
		{
			pIndexName = m_pDatabase->table.index[nIndexNum].name;
			nIndexNum++;
		}
		else
		{
			pIndexName = NULL;
			nIndexNum = 0;
		}
	}

	return(pIndexName);
}

/*
	GetIndexNumberByName()

	Restituisce il numero dell'indice (a base 0) relativo al nome, o -1 per errore.
*/
int CBase::GetIndexNumberByName(const char* pIndexName)
{
	int nIndexNum = -1;

	if(IsValid())
	{
		if(m_pDatabase->table.totindex > 0)
		{
			for(nIndexNum = 0; nIndexNum < m_pDatabase->table.totindex; nIndexNum++)
				if(strcmp(m_pDatabase->table.index[nIndexNum].name,pIndexName)==0)
					break;
		}
	}

	return(nIndexNum);
}

/*
	GetIndexNameByNumber()

	Restituisce il nome dell'indice relativo al numero (a base 0), o -1 per errore.
*/
const char* CBase::GetIndexNameByNumber(int nIndex)
{
	char* pIndexName = "";

	if(IsValid())
	{
		if(m_pDatabase->table.totindex > 0)
		{
			for(int i = 0; i < m_pDatabase->table.totindex; i++)
				if(i==nIndex)
				{
					pIndexName = m_pDatabase->table.index[i].name;
					break;
				}
		}
	}

	return(pIndexName);
}

/*
	GetFieldNumberOfIndex()

	Ricava il progressivo del campo (a base 0) relativo all'indice specificato, o -1 per errore.
*/
int CBase::GetFieldNumberOfIndex(int nIndex)
{
	int nFieldNum = -1;

	if(IsValid())
	{
		if(m_pDatabase->table.totindex > 0)
			if(nIndex >= 0 && nIndex < m_pDatabase->table.totindex)
				nFieldNum = m_pDatabase->table.index[nIndex].fieldnum - 1;
	}

	return(nFieldNum);
}

/*
	GetFieldNameOfIndex()

	Ricava il nome del campo (a base 0) relativo all'indice specificato, o "" per errore.
*/
const char* CBase::GetFieldNameOfIndex(int nIndex)
{
	char* pFieldName = "";

	if(IsValid())
	{
		if(m_pDatabase->table.totindex > 0)
			if(nIndex >= 0 && nIndex < m_pDatabase->table.totindex)
				pFieldName = (char*)m_pDatabase->table.index[nIndex].fieldname;
	}

	return(pFieldName);
}

/*
	GetFieldCount()

	Restituisce il numero di campi della tabella, senza includere l'eventuale chiave primaria.
*/
int CBase::GetFieldCount(void)
{
	int nFields = 0;

	// se la tabella prevede indici salta il primo campo (relativo alla chiave primaria)
	if(IsValid())
		nFields = m_pDatabase->table.totindex > 0 ? m_pDatabase->table.totfield - 1 : m_pDatabase->table.totfield;

	return(nFields);
}

/*
	GetFieldNames()

	Restituisce i nomi dei campi della tabella (da chiamare in un ciclo fino a che non restituisce NULL),
	senza includere l'eventuale chiave primaria.
*/
const char* CBase::GetFieldNames(void)
{
	char* pFieldName = NULL;

	if(IsValid())
	{
		static int nFieldNum = -1;

		// se la tabella prevede indici salta il primo campo (relativo alla chiave primaria)
		if(nFieldNum < 0)
			nFieldNum = m_pDatabase->table.totindex > 0 ? 1 : 0;

		if(nFieldNum < m_pDatabase->table.totfield)
		{
			pFieldName = m_pDatabase->table.row[nFieldNum].name;
			nFieldNum++;
		}
		else
		{
			pFieldName = NULL;
			nFieldNum = -1;
		}
	}

	return(pFieldName);
}

/*
	GetFieldNumberByName()

	Restituisce il numero del campo (a base 0) o -1 per errore,
	senza includere l'eventuale chiave primaria.
*/
int CBase::GetFieldNumberByName(const char* pFieldName)
{
	int nFieldNum = -1;

	if(IsValid())
	{
		if(m_pDatabase->table.totfield > 0)
		{
			for(nFieldNum = 0; nFieldNum < m_pDatabase->table.totfield; nFieldNum++)
			{
				if(strcmp(m_pDatabase->table.row[nFieldNum].name,pFieldName)==0)
				{
					// se la tabella prevede indici salta il primo campo (relativo alla chiave primaria)
					nFieldNum = m_pDatabase->table.totindex > 0 ? nFieldNum-1 : nFieldNum;
					break;
				}
			}

			if(nFieldNum==m_pDatabase->table.totfield)
				nFieldNum = -1;
		}
	}

	return(nFieldNum);
}

/*
	GetFieldNameByNumber()

	Restituisce il nome del campo relativo al numero (a base 0), o -1 per errore,
	senza includere l'eventuale chiave primaria.
*/
const char* CBase::GetFieldNameByNumber(int nFieldNum)
{
	char* pFieldName = "";

	if(IsValid() && nFieldNum >= 0)
	{
		if(m_pDatabase->table.totfield > 0)
		{
			// se la tabella prevede indici salta il primo campo (relativo alla chiave primaria)
			nFieldNum = m_pDatabase->table.totindex > 0 ? nFieldNum + 1 : nFieldNum;
			
			for(int i = 0; i < m_pDatabase->table.totfield; i++)
				if(i==nFieldNum)
				{
					pFieldName = m_pDatabase->table.row[i].name;
					break;
				}
		}
	}

	return(pFieldName);
}

/*
	GetFieldInfoByNumber()

	Restituisce le informazioni relative al campo (passare il progressivo).
*/
BOOL CBase::GetFieldInfoByNumber(int nFieldNum,char* pFieldName/*=NULL*/,int nSize/*=0*/,int* nFieldType/*=NULL*/,int* nFieldSize/*=NULL*/,int* nFieldDec/*=NULL*/,int* nFieldNumber/*=NULL*/,int* nIndexNum/*=NULL*/)
{
	BOOL bGet = FALSE;

	if(pFieldName)   memset(pFieldName,'\0',nSize);
	if(nFieldType)   *nFieldType   = '?';
	if(nFieldSize)   *nFieldSize   = -1;
	if(nFieldDec)    *nFieldDec    = -1;
	if(nFieldNumber) *nFieldNumber = -1;
	if(nIndexNum)    *nIndexNum    = -1;

	if(IsValid() && nFieldNum >= 0)
	{
		if(m_pDatabase->table.totfield > 0)
		{
			// se la tabella prevede indici salta il primo campo (relativo alla chiave primaria)
			nFieldNum = m_pDatabase->table.totindex > 0 ? nFieldNum + 1 : nFieldNum;
			
			for(int i = 0; i < m_pDatabase->table.totfield; i++)
				if(i==nFieldNum)
				{
					if(pFieldName)   strcpyn(pFieldName,m_pDatabase->table.row[i].name,nSize);
					if(nFieldType)   *nFieldType   = m_pDatabase->table.row[i].type;
					if(nFieldSize)   *nFieldSize   = m_pDatabase->table.row[i].size;
					if(nFieldDec)    *nFieldDec    = m_pDatabase->table.row[i].dec;
					if(nFieldNumber) *nFieldNumber = m_pDatabase->table.row[i].num;
					if(nIndexNum && m_pDatabase->table.totindex > 0)
					{
						for(int n = 0; n < m_pDatabase->table.totindex; n++)
							if(m_pDatabase->table.index[n].fieldnum==nFieldNum)
							{
								*nIndexNum = n;
								break;
							}
					}
					bGet = TRUE;
					break;
				}
		}
	}
	
	return(bGet);
}

/*
	GetFieldInfoByName()

	Restituisce le informazioni relative al campo (passare il nome).
*/
BOOL CBase::GetFieldInfoByName(const char* pFieldName,int* nFieldType/*=NULL*/,int* nFieldSize/*=NULL*/,int* nFieldDec/*=NULL*/,int* nFieldNumber/*=NULL*/,int* nIndexNum/*=NULL*/)
{
	BOOL bGet = FALSE;

	if(nFieldType)   *nFieldType   = '?';
	if(nFieldSize)   *nFieldSize   = -1;
	if(nFieldDec)    *nFieldDec    = -1;
	if(nFieldNumber) *nFieldNumber = -1;
	if(nIndexNum)    *nIndexNum    = -1;

	if(IsValid())
	{
		if(m_pDatabase->table.totfield > 0)
		{
			for(int i = 0; i < m_pDatabase->table.totfield; i++)
				if(strcmp(m_pDatabase->table.row[i].name,pFieldName)==0)
				{
					if(nFieldType)   *nFieldType   = m_pDatabase->table.row[i].type;
					if(nFieldSize)   *nFieldSize   = m_pDatabase->table.row[i].size;
					if(nFieldDec)    *nFieldDec    = m_pDatabase->table.row[i].dec;
					if(nFieldNumber) *nFieldNumber = m_pDatabase->table.row[i].num;
					if(nIndexNum && m_pDatabase->table.totindex > 0)
					{
						for(int n = 0; n < m_pDatabase->table.totindex; n++)
							if(strcmp(m_pDatabase->table.index[n].fieldname,pFieldName)==0)
							{
								*nIndexNum = n;
								break;
							}
					}
					bGet = TRUE;
					break;
				}
		}
	}
	
	return(bGet);
}


/*
	NOTA:
	dato che il concetto di chiave primaria/indici viene implementato qui, nell'interfaccia (CBase), e non nel motore
	del database (CBerkeleyDB), tutte le funzioni che operano sui campi per numero progressivo invece che per nome
	devono saltare l'eventuale (primo) campo relativo alla chiave primaria, dato che per il chiamante i numeri progressivi
	per i campi sono sempre a base 0, a prescindere se la tabella usi indici o meno

*/


/*
	GetFieldRaw()

	Ricava il contenuto del campo interno (relativo al record corrente della tabella) come stringa,
	ignorando le formattazioni relative al tipo.
	Specificare il progressivo numerico e non il nome del campo.
*/
const char* CBase::GetFieldRaw(int nFieldNum)
{
	char* pField = "";

	if(IsValid() && nFieldNum >= 0)
	{
		// se la tabella prevede indici salta il primo campo (relativo alla chiave primaria)
		nFieldNum = m_pDatabase->table.totindex > 0 ? nFieldNum + 1 : nFieldNum;

		// restituisce il puntatore al un buffer interno, sovrascritto dalle chiamate successive
		pField = (char*)m_pBerkeleyDB->GetField(nFieldNum);
	}
	
	return(pField);
}

/*
	GetFieldRaw()

	Ricava il contenuto del campo interno (relativo al record corrente della tabella) come stringa,
	ignorando le formattazioni relative al tipo.
	Specificare il nome del campo e non il progressivo numerico.
*/
const char* CBase::GetFieldRaw(const char* pFieldName)
{
	return(GetFieldRaw(GetFieldNumberByName(pFieldName)));
}

/*
	GetFieldUnion()

	Ricava il contenuto del campo interno (relativo al record corrente della tabella), impostando
	la struttura (union) interna.
	Specificare il nome del campo e non il progressivo numerico.
*/
FIELD* CBase::GetFieldUnion(int nFieldNum)
{
	FIELD* f = (FIELD*)NULL;

	if(IsValid() && nFieldNum >= 0)
	{
		int nField = -1;
		char cType = 0;
		memset(&m_Field,'\0',sizeof(m_Field));

		// ricava le informazioni sul campo
		if(m_pDatabase->table.totfield > 0)
		{
			// se la tabella prevede indici salta il primo campo (relativo alla chiave primaria)
			int n = m_pDatabase->table.totindex > 0 ? nFieldNum + 1 : nFieldNum;
			for(int i = 0; i < m_pDatabase->table.totfield; i++)
				if(i==n)
				{
					m_Field.type = cType = m_pDatabase->table.row[i].type;
					m_Field.size = m_pDatabase->table.row[i].size;
					m_Field.dec = m_pDatabase->table.row[i].dec;
					nField = m_pDatabase->table.totindex > 0 ? nFieldNum + 1 : nFieldNum;
					break;
				}
		}

		if(nField!=-1)
		{
			// copia nel buffer interno il valore del campo della tabella
			char szBuffer[MAX_FIELDSIZE+1];
			strcpyn(szBuffer,m_pBerkeleyDB->GetField(nField),sizeof(szBuffer));

			// formatta il contenuto del buffer nel campo relativo della union a seconda del tipo
			switch(cType)
			{
				// carattere
				case 'C':
					strcpyn(m_Field.field.buffer,szBuffer,sizeof(m_Field.field.buffer));
					break;
				
				// ora
				case 'T':
					memcpy(&(m_Field.field.buffer),szBuffer,2);
					m_DateTime.SetHour(atoi(m_Field.field.buffer));
					memcpy(&(m_Field.field.buffer),szBuffer+2,2);
					m_DateTime.SetMin(atoi(m_Field.field.buffer));
					memcpy(&(m_Field.field.buffer),szBuffer+4,2);
					m_DateTime.SetSec(atoi(m_Field.field.buffer));
					strcpyn(m_Field.field.buffer,m_DateTime.GetFormattedTime(FALSE),sizeof(m_Field.field.buffer));
					break;
				
				// data
				case 'D':
					memcpy(&(m_Field.field.buffer),szBuffer+6,2);
					m_DateTime.SetDay(atoi(m_Field.field.buffer));
					memcpy(&(m_Field.field.buffer),szBuffer+4,2);
					m_DateTime.SetMonth(atoi(m_Field.field.buffer));
					memcpy(&(m_Field.field.buffer),m_bCentury ? szBuffer : szBuffer+2,m_bCentury ? 4 : 2);
					m_DateTime.SetYear(atoi(m_Field.field.buffer));
					strcpyn(m_Field.field.buffer,m_DateTime.GetFormattedDate(FALSE),sizeof(m_Field.field.buffer));
					break;
				
				// data gmt
				case 'G':
					{
						memcpy(&(m_Field.field.buffer),szBuffer+6,2);
						m_DateTime.SetDay(atoi(m_Field.field.buffer));
						memcpy(&(m_Field.field.buffer),szBuffer+4,2);
						m_DateTime.SetMonth(atoi(m_Field.field.buffer));
						memcpy(&(m_Field.field.buffer),szBuffer,4);
						m_DateTime.SetYear(atoi(m_Field.field.buffer));

						memset(&(m_Field.field.buffer),'\0',sizeof(m_Field.field.buffer));

						memcpy(&(m_Field.field.buffer),szBuffer+8,2);
						m_DateTime.SetHour(atoi(m_Field.field.buffer));
						memcpy(&(m_Field.field.buffer),szBuffer+10,2);
						m_DateTime.SetMin(atoi(m_Field.field.buffer));
						memcpy(&(m_Field.field.buffer),szBuffer+12,2);
						m_DateTime.SetSec(atoi(m_Field.field.buffer));
						
						DATEFORMAT df = m_DateTime.GetDateFormat();
						m_DateTime.SetDateFormat(GMT_SHORT);
						_snprintf(m_Field.field.buffer,sizeof(m_Field.field.buffer)-1,"%s",m_DateTime.GetFormattedDate(FALSE));
						char* p = strstr(m_Field.field.buffer,"GMT");
						if(p)
							*p = '\0';
						int i = strlen(m_Field.field.buffer);
						_snprintf(m_Field.field.buffer+i,sizeof(m_Field.field.buffer)-i-1,"%s",szBuffer+14);
						m_DateTime.SetDateFormat(df);
					}
					break;
				
				// logico
				case 'L':
					m_Field.field.boolean = (szBuffer[0]=='T' ? TRUE : (szBuffer[0]=='F') ? FALSE : FALSE);
					break;
				
				// short int
				case 'S':
					m_Field.field.shortint = (short int)atoi(szBuffer);
					break;
				
				// int
				case 'I':
					m_Field.field.integer = (int)atoi(szBuffer);
					break;
				
				// long
				case 'N':
					m_Field.field.longint = (long)atol(szBuffer);
					break;
				
				// unsigned long
				case 'U':
					m_Field.field.ulongint = strtoul(szBuffer,NULL,0);
					break;
				
				// double
				case 'R':
					m_Field.field.realnum = (double)atof(szBuffer);
					break;
			}

			f = &m_Field;
		}
	}

	// le chiamate successive sovrascrivono
	return(f);
}

/*
	GetFieldUnion()

	Ricava il contenuto del campo interno (relativo al record corrente della tabella), impostando
	la struttura (union) interna.
	Specificare il progressivo numerico e non il nome del campo.
*/
FIELD* CBase::GetFieldUnion(const char* pFieldName)
{
	return(GetFieldUnion(GetFieldNumberByName(pFieldName)));
}

/*
	GetField()

	Ricava, come carattere, il contenuto del campo interno (relativo al record corrente della tabella).
	Specificare il progressivo numerico e non il nome del campo.
*/
char* CBase::GetField(int nFieldNum,char* pBuffer,int nSize,BOOL bTrim/*=FALSE*/)
{
	char* pField = "";

	if(IsValid() && nFieldNum >= 0)
	{
		// se la tabella prevede indici salta il primo campo (relativo alla chiave primaria)
		nFieldNum = m_pDatabase->table.totindex > 0 ? nFieldNum + 1 : nFieldNum;

		// size deve corrispondere alla dim. del buffer
		memset(pBuffer,'\0',nSize);
		memcpy(pBuffer,m_pBerkeleyDB->GetField(nFieldNum),nSize-1);
		if(bTrim)
			strrtrim(pBuffer);
		pField = pBuffer;
	}

	return(pField);
}

/*
	GetField()

	Ricava, come carattere, il contenuto del campo interno (relativo al record corrente della tabella).
	Specificare il nome del campo e non il progressivo numerico.
*/
char* CBase::GetField(const char* pFieldName,char* pBuffer,int nSize,BOOL bTrim/*=FALSE*/)
{
	return(GetField(GetFieldNumberByName(pFieldName),pBuffer,nSize,bTrim));
}

/*
	GetField()

	Ricava, come boolean, il contenuto del campo interno (relativo al record corrente della tabella).
	Specificare il progressivo numerico e non il nome del campo.
*/
BOOLEAN CBase::GetField(int nFieldNum,BOOLEAN& bValue)
{
	bValue = FALSE;

	if(IsValid() && nFieldNum >= 0)
	{
		// da carattere a boolean
		char* pValue = (char*)GetFieldRaw(nFieldNum);

		if(pValue[0]=='F')
			bValue = FALSE;
		else if(pValue[0]=='T')
			bValue = TRUE;
	}

	return(bValue);
}

/*
	GetField()

	Ricava, come boolean, il contenuto del campo interno (relativo al record corrente della tabella).
	Specificare il nome del campo e non il progressivo numerico.
*/
BOOLEAN CBase::GetField(const char* pFieldName,BOOLEAN& bValue)
{
	return(GetField(GetFieldNumberByName(pFieldName),bValue));
}

/*
	GetField()

	Ricava, come data, il contenuto del campo interno (relativo al record corrente della tabella).
	Specificare il progressivo numerico e non il nome del campo.
*/
CDateTime& CBase::GetField(int nFieldNum,CDateTime& pDateTime)
{
	if(IsValid() && nFieldNum >= 0)
	{
		// da carattere a data
		char szField[MAX_FIELDSIZE+1] = {0};
		int nField = -1;
		char cType = 0;
		char year[5]  = {0};
		char month[3] = {0};
		char day[3]   = {0};
		char hour[3]  = {0};
		char min[3]   = {0};
		char sec[3]   = {0};

		// ricava il tipo del campo (T,D,G)
		// se la tabella prevede indici salta il primo campo (relativo alla chiave primaria)
		int n = m_pDatabase->table.totindex > 0 ? nFieldNum + 1 : nFieldNum;
		for(int i = 0; i < m_pDatabase->table.totfield; i++)
			if(i==n)
			{
				cType = m_pDatabase->table.row[i].type;
				nField = m_pDatabase->table.totindex > 0 ? nFieldNum + 1 : nFieldNum;
				break;
			}

		if(nField!=-1)
		{
			// ricava il contenuto del campo
			strncpy(szField,m_pBerkeleyDB->GetField(nField),sizeof(szField)-1);
			
			switch(cType)
			{
				// ora (hhmmss)
				case 'T':
					memcpy(hour,szField,2);
					memcpy(min,szField+2,2);
					memcpy(sec,szField+4,2);
					pDateTime.SetHour(atoi(hour));
					pDateTime.SetMin(atoi(min));
					pDateTime.SetSec(atoi(sec));
					break;
				
				// data (yyyymmdd)
				case 'D':
					memcpy(year,szField,4);
					memcpy(month,szField+4,2);
					memcpy(day,szField+6,2);
					pDateTime.SetYear(atoi(year));
					pDateTime.SetMonth(atoi(month));
					pDateTime.SetDay(atoi(day));
					break;
				
				// data gmt (yyyymmddhhmmss+|-nnnn)
				case 'G':
					memcpy(year,szField,4);
					memcpy(month,szField+4,2);
					memcpy(day,szField+6,2);
					memcpy(hour,szField+8,2);
					memcpy(min,szField+10,2);
					memcpy(sec,szField+12,2);
					pDateTime.SetYear(atoi(year));
					pDateTime.SetMonth(atoi(month));
					pDateTime.SetDay(atoi(day));
					pDateTime.SetHour(atoi(hour));
					pDateTime.SetMin(atoi(min));
					pDateTime.SetSec(atoi(sec));
					break;
			}
		}
	}

	return(pDateTime);
}

/*
	GetField()

	Ricava, come data, il contenuto del campo interno (relativo al record corrente della tabella).
	Specificare il nome del campo e non il progressivo numerico.
*/
CDateTime& CBase::GetField(const char* pFieldName,CDateTime& pDateTime)
{
	return(GetField(GetFieldNumberByName(pFieldName),pDateTime));
}

/*
	GetField()

	Ricava, come short int, il contenuto del campo interno (relativo al record corrente della tabella).
	Specificare il progressivo numerico e non il nome del campo.
*/
short int CBase::GetField(int nFieldNum,short int& nValue)
{
	nValue = (short int)-1;

	if(IsValid() && nFieldNum >= 0)
	{
		// da numero a short int
		NUMBER Ns = {0};
		GetNumericField(nFieldNum,Ns,shortint_type);
		nValue = Ns.shortint;
	}

	return(nValue);
}

/*
	GetField()

	Ricava, come short int, il contenuto del campo interno (relativo al record corrente della tabella).
	Specificare il nome del campo e non il progressivo numerico.
*/
short int CBase::GetField(const char* pFieldName,short int& nValue)
{
	return(GetField(GetFieldNumberByName(pFieldName),nValue));
}

/*
	GetField()

	Ricava, come int, il contenuto del campo interno (relativo al record corrente della tabella).
	Specificare il progressivo numerico e non il nome del campo.
*/
int CBase::GetField(int nFieldNum,int& nValue)
{
	nValue = (int)-1;

	if(IsValid() && nFieldNum >= 0)
	{
		// da numero a int
		NUMBER Ni = {0};
		GetNumericField(nFieldNum,Ni,integer_type);
		nValue = Ni.integer;
	}

	return(nValue);
}

/*
	GetField()

	Ricava, come int, il contenuto del campo interno (relativo al record corrente della tabella).
	Specificare il nome del campo e non il progressivo numerico.
*/
int CBase::GetField(const char* pFieldName,int& nValue)
{
	return(GetField(GetFieldNumberByName(pFieldName),nValue));
}

/*
	GetField()

	Ricava, come long, il contenuto del campo interno (relativo al record corrente della tabella).
	Specificare il progressivo numerico e non il nome del campo.
*/
long CBase::GetField(int nFieldNum,long& nValue)
{
	nValue = (long)-1L;

	if(IsValid() && nFieldNum >= 0)
	{
		// da numero a long
		NUMBER Nl = {0};
		GetNumericField(nFieldNum,Nl,longint_type);
		nValue = Nl.longint;
	}

	return(nValue);
}

/*
	GetField()

	Ricava, come long, il contenuto del campo interno (relativo al record corrente della tabella).
	Specificare il nome del campo e non il progressivo numerico.
*/
long CBase::GetField(const char* pFieldName,long& nValue)
{
	return(GetField(GetFieldNumberByName(pFieldName),nValue));
}

/*
	GetField()

	Ricava, come unsigned long, il contenuto del campo interno (relativo al record corrente della tabella).
	Specificare il progressivo numerico e non il nome del campo.
*/
unsigned long CBase::GetField(int nFieldNum,unsigned long& nValue)
{
	nValue = (unsigned long)-1L;

	if(IsValid() && nFieldNum >= 0)
	{
		// da numero a unsigned long
		NUMBER Nu = {0};
		GetNumericField(nFieldNum,Nu,ulongint_type);
		nValue = Nu.ulongint;
	}

	return(nValue);
}

/*
	GetField()

	Ricava, come unsigned long, il contenuto del campo interno (relativo al record corrente della tabella).
	Specificare il nome del campo e non il progressivo numerico.
*/
unsigned long CBase::GetField(const char* pFieldName,unsigned long& nValue)
{
	return(GetField(GetFieldNumberByName(pFieldName),nValue));
}

/*
	GetField()

	Ricava, come double, il contenuto del campo interno (relativo al record corrente della tabella).
	Specificare il progressivo numerico e non il nome del campo.
*/
double CBase::GetField(int nFieldNum,double& nValue)
{
	nValue = (double)-1.0;

	if(IsValid() && nFieldNum >= 0)
	{
		// da numero a double
		NUMBER Nr = {0};
		GetNumericField(nFieldNum,Nr,realnum_type);
		nValue = Nr.realnum;
	}

	return(nValue);
}

/*
	GetField()

	Ricava, come double, il contenuto del campo interno (relativo al record corrente della tabella).
	Specificare il nome del campo e non il progressivo numerico.
*/
double CBase::GetField(const char* pFieldName,double& nValue)
{
	return(GetField(GetFieldNumberByName(pFieldName),nValue));
}

/*
	GetNumericField()

	Utilizzata internamente per convertire il valore del campo (stringa) in numerico.
	Specificare il progressivo numerico e non il nome del campo.
*/
void CBase::GetNumericField(int nFieldNum,NUMBER& value,NUMTYPE type)
{
	if(IsValid() && nFieldNum >= 0)
	{
		char szField[MAX_FIELDSIZE+1] = {0};

		// se la tabella prevede indici salta il primo campo (relativo alla chiave primaria)
		nFieldNum = m_pDatabase->table.totindex > 0 ? nFieldNum + 1 : nFieldNum;

		if(nFieldNum >= 0 && nFieldNum < m_pDatabase->table.totfield)
		{
			strncpy(szField,m_pBerkeleyDB->GetField(nFieldNum),sizeof(szField)-1);

			switch(type)
			{
				// short int
				case shortint_type:
					value.shortint = (short int)atoi(szField);
					break;

				// int
				case integer_type:
					value.integer = (int)atoi(szField);
					break;

				// long
				case longint_type:
					value.longint = (long)atol(szField);
					break;
				
				// unsigned long
				case ulongint_type:
					value.ulongint = strtoul(szField,NULL,0);
					break;

				// double
				case realnum_type:
					value.realnum = (double)atof(szField);
					break;
			}
		}
	}
}

/*
	PutField()

	Imposta il campo interno (relativo al record corrente della tabella) con la stringa passata in input.
	La chiamata modifica il campo interno, NON il campo della tabella, da aggiornare con i metodi
	relativi (insert, update, etc.).
	Specificare il progressivo numerico e non il nome del campo.
*/
void CBase::PutField(int nFieldNum,const char* pValue)
{
	if(IsValid() && nFieldNum >= 0)
	{
		// se la tabella prevede indici salta il primo campo (relativo alla chiave primaria)
		nFieldNum = m_pDatabase->table.totindex > 0 ? nFieldNum + 1 : nFieldNum;

		if(nFieldNum >= 0 && nFieldNum < m_pDatabase->table.totfield)
		{
			char szField[MAX_KEYSIZE+1];
			memset(szField,'\0',sizeof(szField));
			strncpy(szField,pValue,sizeof(szField)-1);

#ifdef _USE_FIELD_PICTURES
			// formatta il contenuto del buffer prima di passarlo nel campo
			if(m_pDatabase->table.row[nFieldNum].flags!=CBASE_FLAG_NONE)
				SetFieldFormat(szField,strlen(szField),m_pDatabase->table.row[nFieldNum].flags);
#endif

			// passa il contenuto del buffer nel campo interno (relativo al campo della tabella)
			m_pBerkeleyDB->PutField(nFieldNum,szField);
		}
	}
}

/*
	PutField()

	Imposta il campo interno (relativo al record corrente della tabella) con la stringa passata in input.
	La chiamata modifica il campo interno, NON il campo della tabella, da aggiornare con i metodi
	relativi (insert, update, etc.).
	Specificare il nome del campo e non il progressivo numerico.
*/
void CBase::PutField(const char* pFieldName,const char* pValue)
{
	PutField(GetFieldNumberByName(pFieldName),pValue);
}

/*
	PutField()

	Imposta il campo interno (relativo al record corrente della tabella) con il boolean passato in input.
	La chiamata modifica il campo interno, NON il campo della tabella, da aggiornare con i metodi
	relativi (insert, update, etc.).
	Specificare il progressivo numerico e non il nome del campo.
*/
void CBase::PutField(int nFieldNum,BOOLEAN bValue)
{
	if(IsValid() && nFieldNum >= 0)
	{
		// da boolean a carattere
		char szValue[3];
		FormatField(szValue,sizeof(szValue),bValue);
		PutField(nFieldNum,szValue);
	}
}

/*
	PutField()

	Imposta il campo interno (relativo al record corrente della tabella) con il boolean passato in input.
	La chiamata modifica il campo interno, NON il campo della tabella, da aggiornare con i metodi
	relativi (insert, update, etc.).
	Specificare il nome del campo e non il progressivo numerico.
*/
void CBase::PutField(const char* pFieldName,BOOLEAN bValue)
{
	PutField(GetFieldNumberByName(pFieldName),bValue);
}

/*
	PutField()

	Imposta il campo interno (relativo al record corrente della tabella) con la data passata in input.
	La chiamata modifica il campo interno, NON il campo della tabella, da aggiornare con i metodi
	relativi (insert, update, etc.).
	Specificare il progressivo numerico e non il nome del campo.
*/
void CBase::PutField(int nFieldNum,CDateTime& pDateTime)
{
	if(IsValid() && nFieldNum >= 0)
	{
		char szValue[32] = {0};
		
		// ricava il tipo del campo (T,D,G)
		char cType = 0;

		// se la tabella prevede indici salta il primo campo (relativo alla chiave primaria)
		int n = m_pDatabase->table.totindex > 0 ? nFieldNum + 1 : nFieldNum;
		for(int i = 0; i < m_pDatabase->table.totfield; i++)
			if(i==n)
			{
				cType = m_pDatabase->table.row[i].type;
				break;
			}

		// formatta da data a char
		FormatField(szValue,sizeof(szValue),pDateTime,cType);

		PutField(nFieldNum,szValue);
	}
}

/*
	PutField()

	Imposta il campo interno (relativo al record corrente della tabella) con la data passata in input.
	La chiamata modifica il campo interno, NON il campo della tabella, da aggiornare con i metodi
	relativi (insert, update, etc.).
	Specificare il nome del campo e non il progressivo numerico.
*/
void CBase::PutField(const char* pFieldName,CDateTime& pDateTime)
{
	PutField(GetFieldNumberByName(pFieldName),pDateTime);
}

/*
	PutField()

	Imposta il campo interno (relativo al record corrente della tabella) con lo short int passato in input.
	La chiamata modifica il campo interno, NON il campo della tabella, da aggiornare con i metodi
	relativi (insert, update, etc.).
	Specificare il progressivo numerico e non il nome del campo.
*/
void CBase::PutField(int nFieldNum,short int nValue)
{
	if(IsValid() && nFieldNum >= 0)
	{
		// da numero a short int
		NUMBER Ns = {0};
		Ns.shortint = nValue;		
		PutNumericField(nFieldNum,Ns,shortint_type);
	}
}

/*
	PutField()

	Imposta il campo interno (relativo al record corrente della tabella) con lo short int passato in input.
	La chiamata modifica il campo interno, NON il campo della tabella, da aggiornare con i metodi
	relativi (insert, update, etc.).
	Specificare il nome del campo e non il progressivo numerico.
*/
void CBase::PutField(const char* pFieldName,short int nValue)
{
	PutField(GetFieldNumberByName(pFieldName),(short int)nValue);
}

/*
	PutField()

	Imposta il campo interno (relativo al record corrente della tabella) con l'int passato in input.
	La chiamata modifica il campo interno, NON il campo della tabella, da aggiornare con i metodi
	relativi (insert, update, etc.).
	Specificare il progressivo numerico e non il nome del campo.
*/
void CBase::PutField(int nFieldNum,int nValue)
{
	if(IsValid() && nFieldNum >= 0)
	{
		// da numero a int
		NUMBER Ni = {0};
		Ni.integer = nValue;		
		PutNumericField(nFieldNum,Ni,integer_type);
	}
}

/*
	PutField()

	Imposta il campo interno (relativo al record corrente della tabella) con l'int passato in input.
	La chiamata modifica il campo interno, NON il campo della tabella, da aggiornare con i metodi
	relativi (insert, update, etc.).
	Specificare il nome del campo e non il progressivo numerico.
*/
void CBase::PutField(const char* pFieldName,int nValue)
{
	PutField(GetFieldNumberByName(pFieldName),(int)nValue);
}

/*
	PutField()

	Imposta il campo interno (relativo al record corrente della tabella) con il long passato in input.
	La chiamata modifica il campo interno, NON il campo della tabella, da aggiornare con i metodi
	relativi (insert, update, etc.).
	Specificare il progressivo numerico e non il nome del campo.
*/
void CBase::PutField(int nFieldNum,long nValue)
{
	if(IsValid() && nFieldNum >= 0)
	{
		// da numero a long
		NUMBER Nl = {0};
		Nl.longint = nValue;
		PutNumericField(nFieldNum,Nl,longint_type);
	}
}

/*
	PutField()

	Imposta il campo interno (relativo al record corrente della tabella) con il long passato in input.
	La chiamata modifica il campo interno, NON il campo della tabella, da aggiornare con i metodi
	relativi (insert, update, etc.).
	Specificare il nome del campo e non il progressivo numerico.
*/
void CBase::PutField(const char* pFieldName,long nValue)
{
	PutField(GetFieldNumberByName(pFieldName),(long)nValue);
}

/*
	PutField()

	Imposta il campo interno (relativo al record corrente della tabella) con l'unsigned long passato in input.
	La chiamata modifica il campo interno, NON il campo della tabella, da aggiornare con i metodi
	relativi (insert, update, etc.).
	Specificare il progressivo numerico e non il nome del campo.
*/
void CBase::PutField(int nFieldNum,unsigned long nValue)
{
	if(IsValid() && nFieldNum >= 0)
	{
		// da numero a unsigned long
		NUMBER Nu = {0};
		Nu.ulongint = nValue;
		PutNumericField(nFieldNum,Nu,ulongint_type);
	}
}

/*
	PutField()

	Imposta il campo interno (relativo al record corrente della tabella) con l'unsigned long passato in input.
	La chiamata modifica il campo interno, NON il campo della tabella, da aggiornare con i metodi
	relativi (insert, update, etc.).
	Specificare il nome del campo e non il progressivo numerico.
*/
void CBase::PutField(const char* pFieldName,unsigned long nValue)
{
	PutField(GetFieldNumberByName(pFieldName),(unsigned long)nValue);
}

/*
	PutField()

	Imposta il campo interno (relativo al record corrente della tabella) con il double passato in input.
	La chiamata modifica il campo interno, NON il campo della tabella, da aggiornare con i metodi
	relativi (insert, update, etc.).
	Specificare il progressivo numerico e non il nome del campo.
*/
void CBase::PutField(int nFieldNum,double nValue)
{
	if(IsValid() && nFieldNum >= 0)
	{
		// da numero a double
		NUMBER Nr = {0};
		Nr.realnum = nValue;
		PutNumericField(nFieldNum,Nr,realnum_type);
	}
}

/*
	PutField()

	Imposta il campo interno (relativo al record corrente della tabella) con il double passato in input.
	La chiamata modifica il campo interno, NON il campo della tabella, da aggiornare con i metodi
	relativi (insert, update, etc.).
	Specificare il nome del campo e non il progressivo numerico.
*/
void CBase::PutField(const char* pFieldName,double nValue)
{
	PutField(GetFieldNumberByName(pFieldName),(double)nValue);
}

/*
	PutNumericField()

	Utilizzata internamente per convertire il valore del campo (numerico) in stringa.
	Il valore impostato e' quello relativo al record corrente (imposta con quanto ricevuto in input).
	Specificare il progressivo numerico e non il nome del campo.
*/
void CBase::PutNumericField(int nFieldNum,NUMBER value,NUMTYPE type)
{
	if(IsValid() && nFieldNum >= 0)
	{
		char szField[MAX_FIELDSIZE+1];

		// formatta da numero a char
		FormatField(szField,sizeof(szField),value,type,nFieldNum);
		
		PutField(nFieldNum,(const char*)szField);
	}
}

/*
	FormatField()

	Formatta il valore (boolean, data, numerico) nel buffer.
*/
char* CBase::FormatField(char* pBuffer,int nSize,BOOLEAN bValue)
{
	_snprintf(pBuffer,nSize-1,"%c",bValue ? 'T' : 'F');
	return(pBuffer);
}
char* CBase::FormatField(char* pBuffer,int nSize,CDateTime& pDateTime,char cType)
{
	switch(cType)
	{
		// ora (hhmmss)
		case 'T':
			_snprintf(pBuffer,nSize-1,"%.2d%.2d%.2d",pDateTime.GetHour(),pDateTime.GetMin(),pDateTime.GetSec());
			break;
		
		// data (yyyymmdd)
		case 'D':
			_snprintf(pBuffer,nSize-1,"%.4d%.2d%.2d",pDateTime.GetYear(),pDateTime.GetMonth(),pDateTime.GetDay());		
			break;
		
		// data gmt (yyyymmddhhmmss+|-nnnn)
		case 'G':
			int i = _snprintf(pBuffer,nSize-1,"%.4d%.2d%.2d%.2d%.2d%.2d",pDateTime.GetYear(),pDateTime.GetMonth(),pDateTime.GetDay(),pDateTime.GetHour(),pDateTime.GetMin(),pDateTime.GetSec());
			if(i > 0)
			{
				// ricava la differenza oraria rispetto al GMT (divide i secondi in minuti ed i minuti in ore)
				double Tzd = ((double)pDateTime.GetTimeZoneDiff() / (double)60.0);
				double Min = fmod(Tzd,(double)60.0);
				Tzd /= (double)60.0;
				int tzd = (int)Tzd;
				int min = (int)Min;

				_snprintf(
						pBuffer+i,
						nSize-(1+i),
						"%s0%d%02d",
						tzd==0 ? "" : (tzd > 0 ? "-" : "+"),
						tzd < 0 ? tzd * -1 : tzd,
						min < 0 ? min * -1 : min
						);
			}
			break;
	}

	return(pBuffer);
}
char* CBase::FormatField(char* pBuffer,int nSize,NUMBER value,NUMTYPE type,int nFieldNum)
{
	char cFieldType;
	int  nFieldNumber,nFieldLen,nFieldSize,nFieldDec;
	char szBuffer[MAX_FIELDSIZE+1];
	char szFormat[10];

	nFieldNumber = nFieldNum;

	// se la tabella prevede indici salta il primo campo (relativo alla chiave primaria)
	nFieldNum = m_pDatabase->table.totindex > 0 ? nFieldNum + 1 : nFieldNum;

	if(nFieldNum >= 0 && nFieldNum < m_pDatabase->table.totfield)
	{
		memset(pBuffer,' ',nSize);

		// tipo, dimensione e decimali del campo
		cFieldType = m_pDatabase->table.row[nFieldNum].type;
		nFieldSize = m_pDatabase->table.row[nFieldNum].size;
		nFieldDec  = m_pDatabase->table.row[nFieldNum].dec;
		nFieldDec  = nFieldDec > 6 ? 6 : nFieldDec;

		switch(type)
		{
			// short int
			case shortint_type:
				_snprintf(szBuffer,sizeof(szBuffer)-1,"%d",value.shortint);
				break;
			
			// int
			case integer_type:
				_snprintf(szBuffer,sizeof(szBuffer)-1,"%d",value.integer);
				break;
			
			// long
			case longint_type:
				_snprintf(szBuffer,sizeof(szBuffer)-1,"%ld",value.longint);
				break;
			
			// unsigned long
			case ulongint_type:
				_snprintf(szBuffer,sizeof(szBuffer)-1,"%lu",value.ulongint);
				break;
			
			// double
			case realnum_type:
				_snprintf(szFormat,sizeof(szFormat)-1,"%c.%d%c",'%',nFieldDec,'f');
				_snprintf(szBuffer,sizeof(szBuffer)-1,szFormat,value.realnum);
				break;
		}

		nFieldLen = strlen(szBuffer);
		nFieldLen = nFieldLen > nFieldSize ? nFieldSize : nFieldLen;
		
		// allineamento (stringhe a sx, numeri a dx)
		switch(cFieldType)
		{
			case 'S':
			case 'I':
			case 'N':
			case 'U':
			case 'R':
				memcpy(pBuffer + (nFieldSize-nFieldLen),szBuffer,nFieldLen);
				break;
			default:
				memcpy(pBuffer,szBuffer,nFieldLen);
				break;
		}

		pBuffer[nFieldSize] = '\0';
	}

	return(pBuffer);
}

/*
	SetNumberPicture()

	Imposta la picture numerica.
*/
#ifdef _USE_FIELD_PICTURES
BOOL CBase::SetNumberPicture(const char* pPicture)
{
	BOOL bPict = FALSE;

	if(IsValid())
		if(m_pPictureNumber)
		{
			memset(m_pPictureNumber,'\0',CBASE_MAX_NUMBER_PICTURE + 1);
			strncpy(m_pPictureNumber,pPicture,CBASE_MAX_NUMBER_PICTURE);
			bPict = TRUE;
		}

	return(bPict);
}
#endif

/*
	SetCharPicture()

	Imposta la picture carattere.
*/
#ifdef _USE_FIELD_PICTURES
BOOL CBase::SetCharPicture(const char* pPicture)
{
	BOOL bPict = FALSE;

	if(IsValid())
		if(m_pPictureChar)
		{
			memset(m_pPictureChar,'\0',CBASE_MAX_CHAR_PICTURE + 1);
			strncpy(m_pPictureChar,pPicture,CBASE_MAX_CHAR_PICTURE);
			bPict = TRUE;
		}

	return(bPict);
}
#endif

/*
	SetPunctPicture()

	Imposta la picture per la punteggiatura.
*/
#ifdef _USE_FIELD_PICTURES
BOOL CBase::SetPunctPicture(const char* pPicture)
{
	BOOL bPict = FALSE;

	if(IsValid())
		if(m_pPicturePunct)
		{
			memset(m_pPicturePunct,'\0',CBASE_MAX_PUNCT_PICTURE + 1);
			strncpy(m_pPicturePunct,pPicture,CBASE_MAX_PUNCT_PICTURE);
			bPict = TRUE;
		}

	return(bPict);
}
#endif

/*
	SetUserPicture()

	Imposta la picture definita dall'utente.
*/
#ifdef _USE_FIELD_PICTURES
BOOL CBase::SetUserPicture(const char* pPicture)
{
	BOOL bPict = FALSE;

	if(IsValid())
		if(m_pPictureUserDefined)
		{
			memset(m_pPictureUserDefined,'\0',CBASE_MAX_USER_PICTURE + 1);
			strncpy(m_pPictureUserDefined,pPicture,CBASE_MAX_USER_PICTURE);
			bPict = TRUE;
		}

	return(bPict);
}
#endif

/*
	SetFieldFlags()

	Imposta i flags per il campo.
*/
#ifdef _USE_FIELD_PICTURES
void CBase::SetFieldFlags(int nFieldNum,unsigned long nFlags)
{
	if(IsValid())
	{
		// se la tabella prevede indici salta il primo campo (relativo alla chiave primaria)
		nFieldNum = m_pDatabase->table.totindex > 0 ? nFieldNum + 1 : nFieldNum;	
		if(nFieldNum >= 0 && nFieldNum < m_pDatabase->table.totfield)
			m_pDatabase->table.row[nFieldNum].flags = nFlags;
	}
}
#endif

/*
	SetDefaultNumberPicture()

	Imposta la picture numerica di default.
*/
#ifdef _USE_FIELD_PICTURES
void CBase::SetDefaultNumberPicture(void)
{
	if(!m_pPictureNumber)
		m_pPictureNumber = new char[CBASE_MAX_NUMBER_PICTURE + 1];
	
	if(m_pPictureNumber)
	{
		memset(m_pPictureNumber,'\0',CBASE_MAX_NUMBER_PICTURE + 1);
		//strncpy(m_pPictureNumber,"0123456789",CBASE_MAX_NUMBER_PICTURE);
	}
}
#endif

/*
	SetDefaultCharPicture()

	Imposta la picture carattere di default.
*/
#ifdef _USE_FIELD_PICTURES
void CBase::SetDefaultCharPicture(void)
{
	if(!m_pPictureChar)
		m_pPictureChar = new char[CBASE_MAX_CHAR_PICTURE + 1];
	
	if(m_pPictureChar)
	{
		memset(m_pPictureChar,'\0',CBASE_MAX_CHAR_PICTURE + 1);
		//strncpy(m_pPictureChar," abcdefghilmnopqrstuvzABCDEFGHILMNOPQRSTUVZ",CBASE_MAX_CHAR_PICTURE);
	}
}
#endif

/*
	SetDefaultPunctPicture()

	Imposta la picture per la punteggiatura di default.
*/
#ifdef _USE_FIELD_PICTURES
void CBase::SetDefaultPunctPicture(void)
{
	if(!m_pPicturePunct)
		m_pPicturePunct = new char[CBASE_MAX_PUNCT_PICTURE + 1];
	
	if(m_pPicturePunct)
	{
		memset(m_pPicturePunct,'\0',CBASE_MAX_PUNCT_PICTURE + 1);
		//strncpy(m_pPicturePunct,".,:;",CBASE_MAX_PUNCT_PICTURE);
	}
}
#endif

/*
	SetDefaultUserPicture()

	Imposta la picture utente di default.
*/
#ifdef _USE_FIELD_PICTURES
void CBase::SetDefaultUserPicture(void)
{
	if(!m_pPictureUserDefined)
		m_pPictureUserDefined = new char[CBASE_MAX_USER_PICTURE + 1];
	
	if(m_pPictureUserDefined)
		memset(m_pPictureUserDefined,'\0',CBASE_MAX_USER_PICTURE + 1);
}
#endif

/*
	SetFieldFormat()

	Formatta il contenuto del campo a seconda dei flags.
*/
#ifdef _USE_FIELD_PICTURES
void CBase::SetFieldFormat(char* pField,int nSize,unsigned long nFlags)
{
	if(IsValid())
	{
		if(nFlags & CBASE_FLAG_TOUPPER)
			strupr(pField);
		if(nFlags & CBASE_FLAG_TOLOWER)
			strlwr(pField);
		if((nFlags & CBASE_FLAG_NUMBER && *m_pPictureNumber) || (nFlags & CBASE_FLAG_CHAR && *m_pPictureChar) || (nFlags & CBASE_FLAG_PUNCT && *m_pPicturePunct) || (nFlags & CBASE_FLAG_USERDEFINED && *m_pPictureUserDefined))
			SetFieldFormatByPicture(pField,nSize,nFlags);
	}
}
#endif

/*
	SetFieldFormatByPicture()

	Formatta il contenuto del campo secondo la picture.
*/
#ifdef _USE_FIELD_PICTURES
void CBase::SetFieldFormatByPicture(char* pField,int nSize,unsigned long nFlags)
{
	if(IsValid())
	{
		char szBuffer[MAX_FIELDSIZE+1];
		register char* p = szBuffer;
		register int i;

		memset(szBuffer,' ',sizeof(szBuffer));

		for(i = 0; pField[i] && i < nSize && i < sizeof(szBuffer); i++)
		{
			if(nFlags & CBASE_FLAG_NUMBER)
				if(strchr(m_pPictureNumber,pField[i]))
				{
					*p++ = pField[i];
					continue;
				}
			
			if(nFlags & CBASE_FLAG_CHAR)
				if(strchr(m_pPictureChar,pField[i]))
				{
					*p++ = pField[i];
					continue;
				}
			
			if(nFlags & CBASE_FLAG_PUNCT)
				if(strchr(m_pPictureChar,pField[i]))
				{
					*p++ = pField[i];
					continue;
				}
			
			if(nFlags & CBASE_FLAG_USERDEFINED)
				if(strchr(m_pPictureUserDefined,pField[i]))
				{
					*p++ = pField[i];
					continue;
				}
		}

		memset(pField,' ',nSize);

		if((i = p-szBuffer) > 0)
		{
			// allineamento (stringhe a sx, numeri a dx), occhio: == e non &
			if(nFlags==CBASE_FLAG_NUMBER)
				memcpy(pField + (nSize-i),szBuffer,i);
			else
				memcpy(pField,szBuffer,i);
		}
	}
}
#endif
