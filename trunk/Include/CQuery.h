/*
	CQuery.h
	Classi per le query sul database (CRT/SDK/MFC).
	Luca Piergentili, 04/11/99
	lpiergentili@yahoo.com
*/
#ifndef _CQUERY_H
#define _CQUERY_H 1

#include "typedef.h"
#include "window.h"
#include <stdlib.h>
#include "strings.h"
#include "CNodeList.h"
#include "CBase.h"
#include "CTable.h"

#define EQUERY_NO_ERROR						0
#define EQUERY_INVALID_QUERY					2999
#define EQUERY_COMMAND_REQUIRED				3000
#define EQUERY_INVALID_COMMAND				3001
#define EQUERY_SPECIFYER_REQUIRED				3002
#define EQUERY_INVALID_SPECIFYER				3003
#define EQUERY_CONDITION_REQUIRED				3004
#define EQUERY_INVALID_CONDITION				3005
#define EQUERY_CLAUSE_REQUIRED				3006
#define EQUERY_INVALID_CLAUSE					3007
#define EQUERY_TABLE_REQUIRED					3008
#define EQUERY_INVALID_TABLE					3009
#define EQUERY_FIELD_REQUIRED					3010
#define EQUERY_INVALID_FIELD					3011
#define EQUERY_FIELDVALUE_REQUIRED				3012
#define EQUERY_INVALID_FIELDVALUE				3013
#define EQUERY_UNBALANCED_BRAKETS				3014
#define EQUERY_LOGICALOPERATOR_MISSING			3015
#define EQUERY_LOGICALOPERATOR_ARGUMENT_MISSING	3016

/*
	CCursor
*/
class CCursor
{
public:
	CCursor(const char* pName = NULL,const char* pPath = NULL,BOOL bPersistent = FALSE);
	virtual ~CCursor();

	BOOL Open(void);
	
	BOOL Eof(void) {return(m_pCursor->Eof());}
	BOOL GetNext(void) {return(m_pCursor->GetNext());}

	int			GetFieldCount		(void) {return(m_pCursor->GetFieldCount());}
	const char*	GetFieldRaw		(int			nFieldNum) {return(m_pCursor->GetFieldRaw(nFieldNum));}

	char*		GetField			(int			nFieldNum,	char* pBuffer,int nSize,BOOL bTrim = FALSE) {return(m_pCursor->GetField(nFieldNum,pBuffer,nSize,bTrim));}
	char*		GetField			(const char*	pFieldName,	char* pBuffer,int nSize,BOOL bTrim = FALSE) {return(m_pCursor->GetField(pFieldName,pBuffer,nSize,bTrim));}
	BOOLEAN		GetField			(int			nFieldNum,	BOOLEAN& bValue) {return(m_pCursor->GetField(nFieldNum,bValue));}
	BOOLEAN		GetField			(const char*	pFieldName,	BOOLEAN& bValue) {return(m_pCursor->GetField(pFieldName,bValue));}
	CDateTime&	GetField			(int			nFieldNum,	CDateTime& pDateTime) {return(m_pCursor->GetField(nFieldNum,pDateTime));}
	CDateTime&	GetField			(const char*	pFieldName,	CDateTime& pDateTime) {return(m_pCursor->GetField(pFieldName,pDateTime));}
	short int		GetField			(int			nFieldNum,	short int& nValue) {return(m_pCursor->GetField(nFieldNum,nValue));}
	short int		GetField			(const char*	pFieldName,	short int& nValue) {return(m_pCursor->GetField(pFieldName,nValue));}
	int			GetField			(int			nFieldNum,	int& nValue) {return(m_pCursor->GetField(nFieldNum,nValue));}
	int			GetField			(const char*	pFieldName,	int& nValue) {return(m_pCursor->GetField(pFieldName,nValue));}
	long			GetField			(int			nFieldNum,	long& nValue) {return(m_pCursor->GetField(nFieldNum,nValue));}
	long			GetField			(const char*	pFieldName,	long& nValue) {return(m_pCursor->GetField(pFieldName,nValue));}
	unsigned long	GetField			(int			nFieldNum,	unsigned long& nValue) {return(m_pCursor->GetField(nFieldNum,nValue));}
	unsigned long	GetField			(const char*	pFieldName,	unsigned long& nValue) {return(m_pCursor->GetField(pFieldName,nValue));}
	double		GetField			(int			nFieldNum,	double& nValue) {return(m_pCursor->GetField(nFieldNum,nValue));}
	double		GetField			(const char*	pFieldName,	double& nValue) {return(m_pCursor->GetField(pFieldName,nValue));}

	void Close(void);

	void SetName(const char* pName) {if(m_szCursorName[0]=='\0') strcpyn(m_szCursorName,pName,sizeof(m_szCursorName));}
	void SetPath(const char* pPath) {if(m_szCursorPath[0]=='\0') strcpyn(m_szCursorPath,pPath,sizeof(m_szCursorPath));}
	void SetAutoDelete(BOOL bFlag) {m_bAutoDelete = bFlag;}

private:
	BOOL m_bAutoDelete;
	BOOL m_bPersistent;
	CTable* m_pCursor;
	char m_szCursorName[_MAX_PATH+1];
	char m_szCursorPath[_MAX_PATH+1];
};

#define MAX_QUERY_COMMAND	MAX_FIELDNAME
#define MAX_QUERY_SPECIFYER	MAX_FIELDNAME
#define MAX_QUERY_CONDITION	5
#define MAX_QUERY_LOGICALOPERATOR	5
#define MAX_QUERY_ARG		1024
#define MAX_QUERY_SIZE		2048

/////////////////////////////////////////////////////////////////////////////////////////////////////////

enum QUERY_EXPRESSIONTYPE {							// elenco delle espressioni
	INVALID_EXPRESSIONTYPE = 0,						// invalido
	NUMERIC,										// numerico
	CHARACTER,									// carattere
	FIELD,										// campo tabella
	VARIABLE,										// variabile di memoria
};

enum QUERY_LOGICALOPERATOR {							// elenco degli operatori logici
	INVALID_LOGICALOPERATOR = 0,						// invalido
	AND,											// &&
	OR,											// ||
};

struct QUERY_LOGICALOPERATORS {						// struttura per la definizione degli operatori logici
	QUERY_LOGICALOPERATOR logicaloperator_id;			// enum operatore logico
	char logicaloperator[MAX_QUERY_LOGICALOPERATOR+1];	// operatore logico (stringa)
	char internal_logicaloperator;					// carattere usato internamente per l'operatore logico
};

enum QUERY_CONDITION {								// elenco delle condizioni
	INVALID_CONDITION = 0,							// invalida
	EQUAL,										// =
	NOT_EQUAL,									// !=
	MAYOR,										// >
	MINOR,										// <
	MAYOR_OR_EQUAL,								// >=
	MINOR_OR_EQUAL									// <=
};

struct QUERY_CONDITIONS {							// struttura per la definizione delle condizioni
	QUERY_CONDITION condition_id;						// enum condizione
	char condition[MAX_QUERY_CONDITION+1];				// condizione (stringa)
};

enum QUERY_SPECIFYER {								// elenco degli specificatori (per i comandi)
	INVALID_SPECIFYER = 0,							// invalido
	FROM,										// SELECT
	WHERE,										// SELECT
	ORDER_BY,										// SELECT
	BY,
	VALUES,										// INSERT INTO
	ON,											// CREATE INDEX
};

struct QUERY_SPECIFYERS {							// struttura per la definizione degli specificatori (per i comandi)
	QUERY_SPECIFYER specifyer_id;						// enum specificatore
	char specifyer[MAX_QUERY_SPECIFYER+1];				// specificatore (stringa)
	int composed;									// se lo specificatore e' composto da una o due stringhe (separate da spazio)
	int argc;										// se lo specificatore prevede un argomento
};

struct QUERY_SPECIFYERS_EX : public QUERY_SPECIFYERS {		// struttura per la definizione degli specificatori (per i comandi)
	char args[MAX_QUERY_ARG+1];						// buffer per l'argomento dello specificatore
};

class CSpecifyersExList : public CNodeList
{
public:
	CSpecifyersExList() : CNodeList() {}
	virtual ~CSpecifyersExList() {CNodeList::DeleteAll();}
	BOOL PreDelete(ITERATOR iter)
	{
		if((QUERY_SPECIFYERS_EX*)iter->data)
		{
			delete ((QUERY_SPECIFYERS_EX*)iter->data);
			iter->data = (QUERY_SPECIFYERS_EX*)NULL;
		}
		return(TRUE);
	}
	int Size(void) {return(sizeof(QUERY_SPECIFYERS_EX));}
#ifdef _DEBUG
	const char* Signature(void) {return("CSpecifyersExList");}
#endif
};

enum QUERY_COMMAND {								// elenco dei comandi
	INVALID_COMMAND = 0,							// invalido
	CREATE_TABLE,									// CREATE TABLE <table> (<field> <type><(dim)>[,<field> <type><(dim)>,...])
	CREATE_INDEX,									// CREATE INDEX <index> ON <table> <(field)>
	SELECT,										// SELECT <field[,field,...]|*> FROM <table[,table,...]> [WHERE <field='condition'[,field='condition',...]> [AND|OR <field='condition'[,field='condition',...]> ...] [ORDER BY <field>]
	INSERT_INTO,									// INSERT INTO <table> VALUES (<'value'>,...)
	DELETE_FROM,									// DELETE FROM <table> WHERE <field='condition'[,field='condition',...]> [AND|OR <field='condition'[,field='condition',...]> ...]
	DROP_TABLE,									// DROP TABLE <table>
	DROP_INDEX									// DROP INDEX <index>
};

struct QUERY_COMMANDS {								// struttura per la definizione dei comandi
	QUERY_COMMAND command_id;						// enum comando
	char command[MAX_QUERY_COMMAND+1];					// comando (stringa)
	int composed;									// se il comando e' composto da una o due stringhe (separate da spazio)
	int specifyer;									// se il comando prevede uno specificatore
	int argc;										// se il comando prevede un argomento (del comando, non dello specificatore)
};

struct QUERY_COMMANDS_EX : public QUERY_COMMANDS {		// struttura per la definizione dei comandi
	char args[MAX_QUERY_ARG+1];						// buffer per l'argomento del comando
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////

struct QUERY_TABLES {								// struttura per l'elenco delle tabelle
	char table[_MAX_PATH+1];							// nome della tabella (minuscolo)
	CTable* pTable;								// oggetto per la tabella
};

struct QUERY_FIELDS {								// struttura per l'elenco dei campi
	char table[_MAX_PATH+1];							// nome della tabella relativa (minuscolo)
	char field[MAX_FIELDNAME+1];						// nome del campo (maiuscolo)
	int type;										// tipo del campo
	int size;										// dimensione del campo
	int decs;										// decimali del campo
	char value[MAX_FIELDSIZE+1];						// valore (per valutare la clausola della SELECT)
};

struct QUERY_CLAUSES {								// struttura per l'elenco delle clausole
	QUERY_FIELDS field;								// campo della tabella (per valutare la clausola della SELECT)
	QUERY_EXPRESSIONTYPE expression;					// tipo espressione (per la valutazione della clausola)
	QUERY_SPECIFYER specifyer;						// specificatore (WHERE)
	QUERY_LOGICALOPERATOR logicaloperator;				// operatore logico (AND, OR)
	QUERY_CONDITION condition;						// condizione (EQUAL, NOT_EQUAL, ...)
	int id;										// id della clausola per la macro-sostituzione nell'espressione
	int result;									// risultato della valutazione della clausola
};

struct QUERY_ORDER {								// struttura per l'ordinamento
	int index;									// se viene specificato un ordinamento
	QUERY_FIELDS field;								// campo della tabella
	QUERY_SPECIFYER specifyer;						// specificatore (ORDER BY)
};

/*
	CQuery
*/
class CQuery
{
public:
	CQuery(const char* pDataPath = NULL);
	virtual ~CQuery();

	void					SetDataPath			(const char* pDataPath) {strcpyn(m_szDataPath,pDataPath ? pDataPath : ".",sizeof(m_szDataPath));}
	const char*			GetDataPath			(void) {return(m_szDataPath);}

	void					SetCursorName			(const char* pCursorName) {strcpyn(m_szCursorName,pCursorName,sizeof(m_szCursorName));}
	const char*			GetCursorName			(void) {return(m_szCursorName);}

	void					SetPersistent			(BOOL bFlag) {m_bPersistentCursor = bFlag;}
	BOOL					GetPersistent			(void) {return(m_bPersistentCursor);}

	CCursor*				Exec					(const char* pQuery);
	int					Count				(void) {return(m_nRowCount);}
	int					GetLastError			(void) {return(m_nLastError);}

private:
	QUERY_COMMAND			GetCommand			(const char* pQuery);
	BOOL					GetSelectCommand		(void);
	
	BOOL					GetTables				(QUERY_COMMAND qc);
	BOOL					GetFields				(QUERY_COMMAND qc);
	
	BOOL					GetClauses			(void);
	BOOL					GetWhereClause			(const char* pArgs);
	BOOL					GetOrderByClause		(const char* pArgs);

	CCursor*				DoSelect				(void);

	const char*			GetFieldFromTable		(const char* pFieldName,const char* pValue,int& nCurrentClause);

	BOOL					IsLogicalOperator		(const char* pString,char** pLogicalOperator = NULL);
	QUERY_LOGICALOPERATOR	GetLogicalOperator		(const char* pLogicalOperator);
	char					GetLogicalOperatorChar	(const char* pLogicalOperator);
	char					GetLogicalOperatorChar	(QUERY_LOGICALOPERATOR qlo);
	BOOL					IsCondition			(const char cChr);
	BOOL					IsCondition			(const char* pStr);
	QUERY_CONDITION		GetCondition			(const char* pCondition);
	BOOL					IsSpecifyer			(const char* pString,int* nIndex = NULL);
	BOOL					IsComposedSpecifyer		(const char* pString,int* nIndex = NULL);
	BOOL					Evaluate				(const char* pCondition);

	void					SetLastError			(int nCode) {m_nLastError = nCode;}

	BOOL					m_bPersistentCursor;
	char					m_szCursorName[_MAX_PATH+1];
	char					m_szDataPath[_MAX_PATH+1];
	
	char					m_szExpression[MAX_QUERY_ARG+1];
	
	int					m_nRowCount;
	
	char					m_szQuery[MAX_QUERY_SIZE+1];
	QUERY_COMMANDS_EX		m_QueryCommandEx;
	CSpecifyersExList		m_QuerySpecifyersExList;
	QUERY_TABLES*			m_pQueryTablesArray;
	QUERY_FIELDS*			m_pQueryFieldsArray;
	QUERY_CLAUSES*			m_pQueryClausesArray;
	QUERY_ORDER			m_QueryOrder;
	
	int					m_nLastError;	
};

#endif // _CQUERY_H
