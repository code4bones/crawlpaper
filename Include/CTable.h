/*
	CTable.h
	Classe derivata per l'interfaccia con il database (CRT/SDK/MFC).
	Luca Piergentili, 04/11/99
	lpiergentili@yahoo.com
*/
#ifndef _CTABLE_H
#define _CTABLE_H 1

#include "typedef.h"
#include "CBase.h"

/*
	CTable
*/
class CTable : public CBase
{
public:
	CTable(	const char* lpcszTableName = NULL,	// solo nome tabella
			const char* lpcszDataPath = NULL,	// solo pathname senza \ finale
			BOOL bOpenTable = TRUE			// flag per apertura
			);

	virtual ~CTable();

	// derivate virtuali
	virtual const char*	GetTableName		(void)		{return(m_szTableName);}
	virtual const char*	GetTablePath		(void)		{return(m_szTablePath);}
	virtual int		GetRecordLength	(void);
	virtual const char*	GetRecordAsString	(void);
	virtual void		ResetMemvars		(void)		{}
	virtual void		GatherMemvars		(void)		{}
	virtual void		ScatterMemvars		(BOOL = TRUE)	{}

private:
	struct RECORD {
		int len;
		char* record;
	};

	RECORD		m_Record;							// struttura per il record
	char			m_szTable[_MAX_PATH+1];				// nome completo (di pathname) per il .def
	char			m_szTableName[_MAX_PATH+1];			// solo nome tabella
	char			m_szTablePath[_MAX_PATH+_MAX_FNAME+1];	// solo pathname senza \ finale
};

#endif // _CTABLE_H
