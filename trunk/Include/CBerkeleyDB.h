/*
	CBerkeleyDB.h
	Classe base per l'interfaccia con la libreria Berkeley DB 2.7.7 (http://www.sleepycat.com).
	Luca Piergentili, 04/11/99
	lpiergentili@yahoo.com
*/
#ifndef _CBERKELEYDB_H
#define _CBERKELEYDB_H 1

#include <string.h>
#if defined(_WINDOWS)
  #include "window.h"
#endif
#include "typedef.h"
#include "db.h"

// crea la referenza alla DLL
#ifdef _DEBUG
  #pragma comment(lib,"BerkeleyDB.d.lib")
#ifdef PRAGMA_MESSAGE_VERBOSE
  #pragma message("\t\t\t"__FILE__"("STR(__LINE__)"): automatically linking with BerkeleyDB.d.dll")
#endif
#else
  #pragma comment(lib,"BerkeleyDB.lib")
#ifdef PRAGMA_MESSAGE_VERBOSE
  #pragma message("\t\t\t"__FILE__"("STR(__LINE__)"): automatically linking with BerkeleyDB.dll")
#endif
#endif

// generiche
#define MAX_PRIMARYKEY_SIZE	10					// dimensione massima della chiave primaria
#define MAX_KEYSIZE			1024					// dimensione massima della chiave per gli indici secondari
#define MAX_FIELDSIZE		MAX_KEYSIZE			// dimensione massima del campo del record
#define MAX_FIELDCOUNT		128					// numero massimo di campi per la tabella
#define MAX_FIELDNAME		64					// dimensione massima per il nome del campo della tabella
#define MAX_ERRORCODE_STRING	128					// stringa per messaggio d'errore

/*
	ROW
	struttura per la definizione del campo del record
*/
struct ROW {
	int			num;							// numero progressivo del campo nel record (a base 0)
	int			ofs;							// offset (in bytes) del campo nel record (a base 0)
	char*		name;						// nome del campo
	char			type;						// tipo del campo
	int			size;						// dimensione del campo
	int			dec;							// decimali del campo
	char*		value;						// contenuto del campo (il buffer relativo e' l'istanza della struct per il record)
	unsigned long	flags;						// flag per operazioni (filtri) sul campo
};

/*
	INDEX
	struttura per la definizione dell'indice
*/
struct INDEX {
	char			filename[_MAX_PATH+1];			// nome file indice
	char*		name;						// nome indice
	char*		fieldname;					// nome del campo della tabella utilizzato come chiave
	int			fieldnum;						// numero progressivo del campo della tabella utilizzato come chiave (a base 0)
};

/*
	TABLE_STAT
	struttura per lo status della tabella
*/
struct TABLE_STAT {
	int			bof;							// flag per inizio file
	int			eof;							// flag per fine file
	int			errnum;						// codice numerico ultimo errore
	char			errstr[MAX_ERRORCODE_STRING+1];	// descrizione ultimo errore
};

/*
	TABLE
	struttura per la definizione della tabella
*/
struct TABLE {
	char			filename[_MAX_PATH+1];			// nome tabella
	int			totfield;						// numero totale di campi
	ROW*			row;							// array per la definizione del record, da allocare a run-time
	int			totindex;						// numero totale indici
	INDEX*		index;						// array per la definizione degli indici, da allocare a run-time
	TABLE_STAT	stat;						// status
};

/*
	DATABASE
	struttura per la definizione del database
*/
struct DATABASE {
	TABLE		table;						// definizione della tabella
	unsigned long	flags;						// flags per porcherie varie
};

/*
	DB_...
	codici di ritorno:
	1000		info
	2000		status
	>=3000	errori
*/
#define DB_NO_ERROR						0
// 1000 (info)
#define DB_OK							1000
// 2000 (status)
#define DB_RETCODE_STATUS_BASE			2000	// base per test
#define DB_RETCODE_NOTFOUND				DB_RETCODE_STATUS_BASE
#define DB_RETCODE_BOF					DB_RETCODE_STATUS_BASE+1
#define DB_RETCODE_EOF					DB_RETCODE_STATUS_BASE+2
// 3000 (errori)
#define DB_RETCODE_ERROR_BASE				3000 // base per test
#define DB_RETCODE_EOPENTABLE				DB_RETCODE_ERROR_BASE
#define DB_RETCODE_EOPENCURSOR			DB_RETCODE_ERROR_BASE+1
#define DB_RETCODE_EOPENINDEX				DB_RETCODE_ERROR_BASE+2
#define DB_RETCODE_EOPENIDXCURSOR			DB_RETCODE_ERROR_BASE+3
#define DB_RETCODE_EALREADYOPEN			DB_RETCODE_ERROR_BASE+4
#define DB_RETCODE_ETABLENOTOPEN			DB_RETCODE_ERROR_BASE+5
#define DB_RETCODE_EINDEXREQUIRED			DB_RETCODE_ERROR_BASE+6
#define DB_RETCODE_ECREATETABLE			DB_RETCODE_ERROR_BASE+7
#define DB_RETCODE_ECLOSETABLE			DB_RETCODE_ERROR_BASE+8
#define DB_RETCODE_ECLOSECURSOR			DB_RETCODE_ERROR_BASE+9
#define DB_RETCODE_ECLOSEINDEX			DB_RETCODE_ERROR_BASE+10
#define DB_RETCODE_ECLOSEIDXCURSOR			DB_RETCODE_ERROR_BASE+12
#define DB_RETCODE_EINVALIDHANDLE			DB_RETCODE_ERROR_BASE+13
#define DB_RETCODE_EINVALIDPARAM			DB_RETCODE_ERROR_BASE+14
#define DB_RETCODE_EINVALIDRESOURCE		DB_RETCODE_ERROR_BASE+15
#define DB_RETCODE_EINVALIDCURSOR			DB_RETCODE_ERROR_BASE+16
#define DB_RETCODE_EINVALIDINDEX			DB_RETCODE_ERROR_BASE+17
#define DB_RETCODE_EINVALIDFIELDNUMBER		DB_RETCODE_ERROR_BASE+18
#define DB_RETCODE_EINVALIDFIELDSIZE		DB_RETCODE_ERROR_BASE+19
#define DB_RETCODE_EINVALIDPRIMARYKEYSIZE	DB_RETCODE_ERROR_BASE+20
#define DB_RETCODE_ELOCKFAILURE			DB_RETCODE_ERROR_BASE+21
#define DB_RETCODE_EALLOCFAILURE			DB_RETCODE_ERROR_BASE+22
#define DB_RETCODE_EOPENFAILURE			DB_RETCODE_ERROR_BASE+23
#define DB_RETCODE_ESYNCFAILURE			DB_RETCODE_ERROR_BASE+24
#define DB_RETCODE_ESECONDARYKEYNOTFOUND	DB_RETCODE_ERROR_BASE+24
// >3000 (errori)
#define DB_RETCODE_ERROR					4000
#define DB_RETCODE_UNKNOWERROR			6666

// da usare per il posizionamento (se specificato al posto di DB_FIRST|DB_NEXT|DB_PREV|DB_LAST, usa DB_SET)
#define DB_SEARCH 0

/*
	CBerkeleyDB
*/
class CBerkeleyDB
{
public:
	CBerkeleyDB();
	virtual ~CBerkeleyDB();

	int			Open					(u_int32_t = DB_CREATE);
	int			IsOpen				(void) const;
	int			Create				(void);
	int			Close				(void);

	int			Insert				(void);
	int			Delete				(void);
	int			Replace				(int nFieldNum,const char* pCurrentValue,const char* pReplaceValue);

	int			Reindex				(int nIndex = -1);
	int			CheckIndex			(int nIndex = -1);

	inline void	ResetCursor			(void)				{m_pCurrentCursor = m_pDbCursor; m_nCurrentCursorNumber = -1;}
	int			SetCursor				(int);
	inline DBC*	GetCursor				(void) const			{return(m_pCurrentCursor);}
	inline int	GetCursorNumber		(void) const			{return(m_nCurrentCursorNumber);}

	inline char*	GetPrimaryKeyData		(void) const			{return(m_Database.table.row[0].value);}
	inline int	GetPrimaryKeySize		(void) const			{return(m_Database.table.row[0].size);}
	inline char*	GetSecondaryKeyData		(int nFieldNum) const	{return(m_Database.table.row[nFieldNum].value);}
	inline int	GetSecondaryKeySize		(int nFieldNum) const	{return(m_Database.table.row[nFieldNum].size);}
	inline int	GetSecondaryKeyField	(int nIndex) const		{return(m_Database.table.index[nIndex].fieldnum);}
	
	int			Get					(u_int32_t db_goto_flag/*DB_SEARCH(usa DB_SET)|DB_FIRST|DB_NEXT|DB_PREV|DB_LAST*/,u_int32_t db_set_flag = DB_SET/*DB_SET|DB_SET_RANGE*/);
	inline int	GetFirst				(void)				{return(Get(DB_FIRST,DB_SET));}
	inline int	GetNext				(void)				{return(Get(DB_NEXT,DB_SET));}
	inline int	GetPrev				(void)				{return(Get(DB_PREV,DB_SET));}
	inline int	GetLast				(void)				{return(Get(DB_LAST,DB_SET));}

	int			GetPrimaryKey			(const char* pPrimaryKey);
	int			GetSecondaryKey		(int nIndex,u_int32_t db_set_flag = DB_SET/*DB_SET|DB_SET_RANGE*/);
	void			PutKey				(char* pValue,int nIndex);

	const char*	GetField				(int nFieldNum);
	void			PutField				(int nFieldNum,const char* pValue);

	inline DATABASE* GetDatabase			(void)				{return(&m_Database);}

	int			SetLastError			(int);
	void			ResetLastError			(void);
	int			GetLastError			(void);
	const char*	GetLastErrorString		(void);

#if defined(_WINDOWS)
	inline void	ShowErrors			(BOOL bFlag)			{m_bShowErrors = bFlag;}
#endif

private:
	int			SyncDb				(BOOL bFlush = FALSE);
	int			SyncIdx				(int nIndex,BOOL bFlush = FALSE);
	inline int	FlushDb				(void)				{return(SyncDb(TRUE));}
	inline int	FlushIdx				(int nIndex)			{return(SyncIdx(nIndex,TRUE));}

	inline void	ResetKeyPair			(void)				{memset(&m_Key,'\0',sizeof(m_Key));}
	inline void	ResetDataPair			(void)				{memset(&m_Data,'\0',sizeof(m_Data));}
	inline void	ResetPair				(void)				{memset(&m_Key,'\0',sizeof(m_Key)); memset(&m_Data,'\0',sizeof(m_Data));}

	void			ResetRecord			(void);
	void			BlankRecord			(void);
	void			MemoryToRecord			(void);
	void			RecordToMemory			(void);

	DATABASE		m_Database;							// struct per la tabella
	char*		m_pDataBuffer;							// buffer per il campo data.data per i dati (campi del record)
	int			m_nDataBufferSize;						// dimensione
	int			m_nRecordSize;							// dimensione del record (somma dei campi)
	DB_INFO		m_dbinfo;								// (db)
	DBT			m_Key,m_Data;							// coppia chiave/valore (db)
	DB*			m_pDbHandle;							// handle della tabella (db)
	DBC*			m_pDbCursor;							// cursore per la vista sulla tabella (db)
	DBC*			m_pCurrentCursor;						// cursore corrente
	int			m_nCurrentCursorNumber;					// indice del cursore corrente
	DB**			m_pIdxHandleArray;						// array per gli handles degli indici
	DBC**		m_pIdxCursorArray;						// array per i cursori degli indici
	char			m_szLastPrimaryKey[MAX_PRIMARYKEY_SIZE+1];	// buffer per la generazione della chiave primaria
#if defined(_WINDOWS)
	BOOL			m_bShowErrors;							// flag per visualizzazione errori
#endif
};

#endif // _CBERKELEYDB_H
