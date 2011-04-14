/*
	CBase.h
	Classe derivata per l'interfaccia con il database.
	Definisce l'oggetto per il database (CBase) usando la classe che si interfaccia con la libreria (CBerkeleyDB).
	Luca Piergentili, 04/11/99
	lpiergentili@yahoo.com
*/
#ifndef _CBASE_H
#define _CBASE_H 1

#include "typedef.h"
#include "CSync.h"
#include "CDateTime.h"
#include "CBerkeleyDB.h"
#include "CBase.config.h"

/*
	CBASE_FLAG_...
	flags per filtri sui campi
*/
#ifdef _USE_FIELD_PICTURES
  #define CBASE_FLAG_NONE		0x0000	// nessun azione
  #define CBASE_FLAG_TOUPPER		0x0002	// a -> A
  #define CBASE_FLAG_TOLOWER		0x0004	// A -> a
  #define CBASE_FLAG_NUMBER		0x0008	// 0-9
  #define CBASE_FLAG_CHAR		0x0010	// a-z
  #define CBASE_FLAG_PUNCT		0x0020	// .,;:?¿!¡&%$#@(){}[]-+*/
  #define CBASE_FLAG_USERDEFINED	0x0040	// definito dall'utente
  #define CBASE_MAX_NUMBER_PICTURE	32		// dimensioni dei buffer per la picture
  #define CBASE_MAX_CHAR_PICTURE	128
  #define CBASE_MAX_PUNCT_PICTURE	128
  #define CBASE_MAX_USER_PICTURE	128
#endif

/*
	CBASE_TABLE
	struttura per la definizione della tabella
*/
struct CBASE_TABLE {
	char*	field;		// nome campo
	char		type;		// tipo
	int		size;		// dimensione
	int		dec;			// decimali
};

/*
	CBASE_INDEX
	struttura per la definizione dell'indice
*/
struct CBASE_INDEX {
	char*	file;		// nome file
	char*	name;		// nome indice
	char*	field;		// nome campo associato
};

/*
	FIELD_DELIM
	carattere usato per delimitare i campi all'interno del record
	(quando il record viene restituito come stringa)
*/
#define FIELD_DELIM '|'

/*
	FIELD_TYPE
	union per il valore del campo
*/
union FIELD_TYPE {
	BOOLEAN		boolean;
	short int		shortint;
	int			integer;
	long			longint;
	unsigned long	ulongint;
	double		realnum;
	char			chr;
	char			buffer[MAX_FIELDSIZE+1];
};

/*
	FIELD
	struttura per la definizione del campo
*/
struct FIELD {
	char			type;
	int			size;
	int			dec;
	FIELD_TYPE	field;
};

/*
	FIELD_...SIZE
	stabiliscono le dimensioni dei campi a seconda del tipo
*/
#define FIELD_CHAR_MINSIZE		1
#define FIELD_CHAR_MAXSIZE		MAX_FIELDSIZE
#define FIELD_TIME_MINSIZE		6
#define FIELD_TIME_MAXSIZE		6
#define FIELD_DATE_MINSIZE		8
#define FIELD_DATE_MAXSIZE		8
#define FIELD_GMTDATE_MINSIZE		19
#define FIELD_GMTDATE_MAXSIZE		19
#define FIELD_LOGICAL_MINSIZE		1
#define FIELD_LOGICAL_MAXSIZE		1
#define FIELD_SHORTINT_MINSIZE	5
#define FIELD_SHORTINT_MAXSIZE	5
#define FIELD_INT_MINSIZE		10
#define FIELD_INT_MAXSIZE		10
#define FIELD_LONG_MINSIZE		10
#define FIELD_LONG_MAXSIZE		10
#define FIELD_UNSIGNEDLONG_MINSIZE	10
#define FIELD_UNSIGNEDLONG_MAXSIZE	10
#define FIELD_REAL_MINDECS		0
#define FIELD_REAL_MAXDECS		6
#define FIELD_REAL_MINSIZE		10
#define FIELD_REAL_MAXSIZE		(FIELD_REAL_MINSIZE + 1 + FIELD_REAL_MAXDECS)

/*
	CBase
*/
class CBase
{
public:
	// tipi numerici
	union NUMBER {
		short int		shortint;	// intero corto
		int			integer;	// intero
		long			longint;	// lungo
		unsigned long	ulongint;	// esageratamente lungo
		double		realnum;	// virgola mobile
	};
	enum NUMTYPE {
		shortint_type,
		integer_type,
		longint_type,
		ulongint_type,
		realnum_type
	};

	CBase();
	virtual ~CBase();

	// oggetto interno
	inline BOOL	IsValid			(void) const {return(m_bIsValid);}
	inline void	SetValid			(BOOL bFlag) {m_bIsValid = bFlag;}

	// lock
	BOOL			Lock				(int nTimeout = SYNC_5_SECS_TIMEOUT);
	BOOL			Unlock			(void);

	// database
	BOOL			Create			(const char* pTableName,CBASE_TABLE* pTable = NULL,CBASE_INDEX* pIdx = NULL);
	BOOL			Open				(void);
	BOOL			Zap				(void);
	void			Close			(void);

	// import/export
	int			Load				(const char* pFileName);
	int			Dump				(const char* pFileName,BOOL bDumpPrimaryKey = FALSE,char cSeparator = 0);

	// operazioni
	BOOL			Insert			(void);
	BOOL			Delete			(void);

	// implementare con nome campo
	BOOL			Replace			(int nFieldNum,const char* pOldValue,const char* pNewValue);
	BOOL			Replace			(int nFieldNum,BOOLEAN bOldValue,BOOLEAN bNewValue);
	BOOL			Replace			(int nFieldNum,CDateTime& pOldValue,CDateTime& pNewValue);
	BOOL			Replace			(int nFieldNum,short int nOldValue,short int nNewValue);
	BOOL			Replace			(int nFieldNum,int nOldValue,int nNewValue);
	BOOL			Replace			(int nFieldNum,long nOldValue,long nNewValue);
	BOOL			Replace			(int nFieldNum,unsigned long nOldValue,unsigned long nNewValue);
	BOOL			Replace			(int nFieldNum,double nOldValue,double nNewValue);

	// posizionamento
	BOOL			Bof				(void) const;
	BOOL			Eof				(void) const;

	BOOL			GoTop			(int nIndex  = -1);
	BOOL			GoBottom			(int nIndex = -1);

	BOOL			GetCurrent		(int nIndex = -1);
	BOOL			GetFirst			(int nIndex = -1);
	BOOL			GetNext			(int nIndex = -1);
	BOOL			GetPrev			(int nIndex = -1);
	BOOL			GetLast			(int nIndex = -1);
	
	// ricerca
	inline void	SetSoftseek		(BOOL bFlag) {m_bSoftseek = bFlag;}
	inline BOOL	GetSoftseek		(void) const {return(m_bSoftseek);}

	BOOL			Seek				(const char* pValue,int nIndex = -1);
	BOOL			Seek				(short int nValue,int nIndex = -1);
	BOOL			Seek				(int nValue,int nIndex = -1);
	BOOL			Seek				(long nValue,int nIndex = -1);
	BOOL			Seek				(unsigned long nValue,int nIndex = -1);
	BOOL			Seek				(double nValue,int nIndex = -1);

	BOOL			SeekPrimaryKey		(const char* pPrimaryKey);
	
	BOOL			Find				(const char* pValue);
	
	// indici
	int			GetIndex			(void);
	int			SetIndex			(int nIndex);
	void			ResetIndex		(void);

	BOOL			Reindex			(int nIndex = -1);
	BOOL			CheckIndex		(int nIndex = -1);

	int			GetIndexCount		(void) const;
	const char*	GetIndexNames		(void);
	int			GetIndexNumberByName(const char* pIndexName);
	const char*	GetIndexNameByNumber(int nIndex);
	
	int			GetFieldNumberOfIndex(int nIndex);
	const char*	GetFieldNameOfIndex(int nIndex);

	// campi
	int			GetFieldCount		(void);
	const char*	GetFieldNames		(void);
	int			GetFieldNumberByName(const char* pFieldName);
	const char*	GetFieldNameByNumber(int nFieldNum);

	BOOL			GetFieldInfoByNumber(int         nFieldNum,	char* pFieldName = NULL,int nSize = 0,int* cFieldType = NULL,int* nFieldSize = NULL,int* nFieldDec = NULL,int* nFieldNumber = NULL,int* nIndexNum = NULL);
	BOOL			GetFieldInfoByName	(const char* pFieldName,	int* cFieldType = NULL,int* nFieldSize = NULL,int* nFieldDec = NULL,int* nFieldNumber = NULL,int* nIndexNum = NULL);

	const char*	GetPrimaryKeyValue(void) const {return(m_szPrimaryKeyValue);}

	// per ricavare il valore del campo interno (relativo al campo corrente della tabella) in base al progressivo numerico
	const char*	GetFieldRaw		(int			nFieldNum);
	const char*	GetFieldRaw		(const char*	pFieldName);

	FIELD*		GetFieldUnion		(int			nFieldNum);
	FIELD*		GetFieldUnion		(const char*	pFieldName);
	
	char*		GetField			(int			nFieldNum,	char* pBuffer,int nSize,BOOL bTrim = FALSE);
	char*		GetField			(const char*	pFieldName,	char* pBuffer,int nSize,BOOL bTrim = FALSE);
	BOOLEAN		GetField			(int			nFieldNum,	BOOLEAN& bValue);
	BOOLEAN		GetField			(const char*	pFieldName,	BOOLEAN& bValue);
	CDateTime&	GetField			(int			nFieldNum,	CDateTime& pDateTime);
	CDateTime&	GetField			(const char*	pFieldName,	CDateTime& pDateTime);
	short int		GetField			(int			nFieldNum,	short int& nValue);
	short int		GetField			(const char*	pFieldName,	short int& nValue);
	int			GetField			(int			nFieldNum,	int& nValue);
	int			GetField			(const char*	pFieldName,	int& nValue);
	long			GetField			(int			nFieldNum,	long& nValue);
	long			GetField			(const char*	pFieldName,	long& nValue);
	unsigned long	GetField			(int			nFieldNum,	unsigned long& nValue);
	unsigned long	GetField			(const char*	pFieldName,	unsigned long& nValue);
	double		GetField			(int			nFieldNum,	double& nValue);
	double		GetField			(const char*	pFieldName,	double& nValue);

	void			PutField			(int			nFieldNum,	const char* pValue);
	void			PutField			(const char*	pFieldName,	const char* pValue);
	void			PutField			(int			nFieldNum,	BOOLEAN bValue);
	void			PutField			(const char*	pFieldName,	BOOLEAN bValue);
	void			PutField			(int			nFieldNum,	CDateTime& pDateTime);
	void			PutField			(const char*	pFieldName,	CDateTime& pDateTime);
	void			PutField			(int			nFieldNum,	short int nValue);
	void			PutField			(const char*	pFieldName,	short int nValue);
	void			PutField			(int			nFieldNum,	int nValue);
	void			PutField			(const char*	pFieldName,	int nValue);
	void			PutField			(int			nFieldNum,	long nValue);
	void			PutField			(const char*	pFieldName,	long nValue);
	void			PutField			(int			nFieldNum,	unsigned long nValue);
	void			PutField			(const char*	pFieldName,	unsigned long nValue);
	void			PutField			(int			nFieldNum,	double nValue);
	void			PutField			(const char*	pFieldName,	double nValue);

#ifdef _USE_FIELD_PICTURES
	// picture
	BOOL			SetNumberPicture	(const char* pPicture);
	BOOL			SetCharPicture		(const char* pPicture);
	BOOL			SetPunctPicture	(const char* pPicture);
	BOOL			SetUserPicture		(const char* pPicture);
	void			SetFieldFlags		(int nFieldNum,unsigned long nFlags);
#endif

	// data
	inline void	SetDateFormat		(DATEFORMAT format)	{m_DateTime.SetDateFormat(format);}
	inline void	SetTimeFormat		(TIMEFORMAT format)	{m_DateTime.SetTimeFormat(format);}
	inline void	SetCentury		(BOOL bCentury)	{m_bCentury = bCentury;}
	inline DATEFORMAT GetDateFormat	(void)			{return(m_DateTime.GetDateFormat());}
	inline TIMEFORMAT GetTimeFormat	(void)			{return(m_DateTime.GetTimeFormat());}

	// errori
	inline int	GetLastError		(void)			{return(IsValid() ? m_pBerkeleyDB->GetLastError() : DB_RETCODE_ETABLENOTOPEN);}
	inline const char* GetLastErrorString(void)			{return(IsValid() ? m_pBerkeleyDB->GetLastErrorString() : "DB_RETCODE_ETABLENOTOPEN");}

	// strutture della tabella e degli indici
	virtual const CBASE_TABLE* GetTableStruct(void)		{return(m_pTableStruct);}
	virtual const CBASE_INDEX* GetIndexStruct(void)		{return(m_pIdxStruct);}

	// virtuali pure, la derivata dovra' definirle con i dati relativi all'implementazione della tabella
	virtual const char*	GetTableName		(void) = 0;
	virtual const char*	GetTablePath		(void) = 0;
	virtual int		GetRecordLength	(void) = 0;
	virtual const char*	GetRecordAsString	(void) = 0;
	virtual void		ResetMemvars		(void) = 0;
	virtual void		GatherMemvars		(void) = 0;
	virtual void		ScatterMemvars		(BOOL = TRUE) = 0;

private:
	// flag per validita' oggetto
	BOOL			m_bIsValid;

	// per il lock della tabella
	CSyncProcesses	m_mutexTable;
	
	// oggetto per l'interfaccia con il database
	CBerkeleyDB*	m_pBerkeleyDB;
	
	// puntatore alla definizione del database
	DATABASE*		m_pDatabase;

	// per la creazione dinamica delle strutture
	CBASE_TABLE*	m_pTableStruct;
	int			m_nTotRows;
	CBASE_INDEX*	m_pIdxStruct;
	int			m_nTotIdx;

	// puntatore al buffer per il record
	char*		m_pRecordBuffer;
	
	// per le Get...()/Put...() che usano i nomi dei campi
	FIELD		m_Field;

	// per la chiave primaria corrente
	char			m_szPrimaryKeyValue[MAX_PRIMARYKEY_SIZE+1];
	
	// tipo ricerca
	BOOL			m_bSoftseek;

	// data
	CDateTime		m_DateTime;
	BOOL			m_bCentury;

#ifdef _USE_FIELD_PICTURES
	char*		m_pPictureNumber;
	char*		m_pPictureChar;
	char*		m_pPicturePunct;
	char*		m_pPictureUserDefined;
#endif

	// membri	
	void			GetNumericField		(int,NUMBER&,NUMTYPE);
	void			PutNumericField		(int,NUMBER,NUMTYPE);

	char*		FormatField			(char* pBuffer,int nSize,BOOLEAN bValue);
	char*		FormatField			(char* pBuffer,int nSize,CDateTime& pDateTime,char cType);
	char*		FormatField			(char* pBuffer,int nSize,NUMBER value,NUMTYPE type,int nFieldNum);

#ifdef _USE_FIELD_PICTURES
	void			SetDefaultNumberPicture	(void);
	void			SetDefaultCharPicture	(void);
	void			SetDefaultPunctPicture	(void);
	void			SetDefaultUserPicture	(void);
	void			SetFieldFormat			(char* pField,int nSize,unsigned long nFlags);
	void			SetFieldFormatByPicture	(char* pField,int nSize,unsigned long nFlags);
#endif
};

#endif // _CBASE_H
