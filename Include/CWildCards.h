/*
	CWildCards.h
	Classe base per gestione wildcards (?*).
	Luca Piergentili, 26/06/00
	lpiergentili@yahoo.com

	Incapsulato il codice originale di Florian Schintke (vedi sotto) e aggiunto
	il codice per gestire piu' wildcards in un unica stringa (separate da spazio).
	
	Implementation of the UN*X wildcards in C. So they are available in a portable way and can be used
	whereever	needed.
	Author(s):
	Florian Schintke (schintke@cs.tu-berlin.de)
	Dates:
	First editing: unknown, but before 04/02/1997
	Last Change  : 11/10/1998
*/
#ifndef _CWILDCARDS_H
#define _CWILDCARDS_H 1

#include "typedef.h"
#include "CNodeList.h"

/*
	CWildCard
	classe per l'elemento della lista delle wildcards
*/
class CWildCard {
public:
	CWildCard() {m_pString = NULL;}
	CWildCard(char* pString);
	~CWildCard();

	inline const char* GetString(void) const {return(m_pString);}
	inline void Reset(void) {if(m_pString) delete [] m_pString; m_pString = NULL;}

private:
	char* m_pString;
};

/*
	CWildCardList
	classe per la lista delle wildcards
*/
class CWildCardList : public CNodeList
{
public:
	CWildCardList() : CNodeList() {}
	virtual ~CWildCardList() {CNodeList::DeleteAll();}
	void* Create(void)
	{
		return(new CWildCard());
	}	
	void* Initialize(void* pVoid)
	{
		CWildCard* pData = (CWildCard*)pVoid;
		if(!pData)
			pData = (CWildCard*)Create();
		if(pData)
			pData->Reset();
		return(pData);
	}
	BOOL PreDelete(ITERATOR iter)
	{
		if((CWildCard*)iter->data)
			delete ((CWildCard*)iter->data),iter->data = (CWildCard*)NULL;
		return(TRUE);
	}
	int Size(void) {return(-1);}
#ifdef _DEBUG
	const char* Signature(void) {return("CWildCardList");}
#endif
};

/*
	CWildCards
	classe base per gestione wildcards (?*)
	distingue tra maiuscole e minuscole
	a seconda del flag (m_bIgnoreSpaces) considera la stringa contenente le wildcards (la stringa passata
	a Match()) come un unica wildcards o come contenente piu' wildcards (separate con uno spazio)
*/
class CWildCards
{
public:
	CWildCards(BOOL bIgnoreCase = FALSE,BOOL bIgnoreSpaces = TRUE) {m_bIgnoreCase = bIgnoreCase; m_bIgnoreSpaces = bIgnoreSpaces;}
	virtual ~CWildCards() {}

	BOOL Match(const char* wildcards,const char* test);
	
	inline void SetIgnoreCase(BOOL bFlag) {m_bIgnoreCase = bFlag;}
	inline BOOL GetIgnoreCase(void) const {return(m_bIgnoreCase);}
	
	inline void SetIgnoreSpaces(BOOL bFlag) {m_bIgnoreSpaces = bFlag;}
	inline BOOL GetIgnoreSpaces(void) const {return(m_bIgnoreSpaces);}

private:
	int match(const char* wildcards,const char* test);
	int set(const char** wildcard,const char** test);
	int asterisk(const char** wildcard,const char** test);

	BOOL m_bIgnoreCase;
	BOOL m_bIgnoreSpaces;
	CWildCardList m_listWildcards;
};

#endif // _CWILDCARDS_H
