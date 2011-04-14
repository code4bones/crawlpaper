/*
	CNodeList.h
	Classe base per la gestione di una lista semplice con riutilizzo (automatico) degli elementi eliminati.
	'Affanculo i templates.
	Luca Piergentili, 05/01/98
	lpiergentili@yahoo.com
*/
#ifndef _CNODELIST_H
#define _CNODELIST_H 1

#include <stdio.h>
#include <stdlib.h>
#include "typedef.h"

#ifdef _DEBUG
  #define SIGNATURE_LEN 32
#endif

#define USED_NODE   1965
#define UNUSED_NODE 1981
#define REUSED_NODE 1971
#define ERASED_NODE 2002

#define RELEASE_REMOVE_MODE	0
#define RELEASE_DELETE_MODE	1
#define RELEASE_ERASE_MODE	2

/*
	CNode
	Elemento per la lista.
*/
class CNodeList;
class CNode
{
	friend CNodeList;

public:
	CNode() {}
	virtual ~CNode() {}

private:
#ifdef _DEBUG
	char		signature[SIGNATURE_LEN+1];	// signature del nodo (senza lo '\0' finale)
#endif
	int		index;					// indice (base 0)
	int		status;					// status del nodo (in uso, disponibile, etc.)
	CNode*	next;					// puntatore al nodo successivo della lista
public:
	void*	data;					// puntatore ai dati
};

/*
	ITERATOR
	Puntatore per la ricerca nella lista.
*/
typedef CNode* ITERATOR;

/*
	CNodeList
	Classe base per la gestione di una lista semplice con riutilizzo (automatico) degli elementi eliminati.

	1) definire una struct/classe per i dati da inserire nella lista:
		
		struct/class myData {
			...
		};
		se la struct/classe alloca dati, eliminarli nel distruttore che verra chiamato dalla PreDelete()
		della classe derivata (vedi sotto).

	2) derivare dalla classe base (CNodeList) una classe che contenga il codice per il riutilizzo e per
	   l'eliminazione dell'elemento:
			
		class CMyList : public CNodeList
		{
		public:
			CMyList() : CNodeList() {}
			virtual ~CMyList() {CNodeList::DeleteAll();}	
			
			void* Create(void) // deve creare e restituire il ptr ai dati
			{
				myData* pData = new myData;
				return(pData);
			}
		
			void* Initialize(void* pVoid) // riceve il ptr ai dati, deve restituire il ptr ai dati
			{
				myData* pData = (myData*)pVoid;
				if(!pData)
					pData = (myData*)Create();
				if(pData)
					memset(pData,'\0',sizeof(myData));
				return(pData);
			}
			
			BOOL PreDelete(ITERATOR iter)
			{
				if((myData*)iter->data)
				{
					delete ((myData*)iter->data);
					iter->data = (myData*)NULL;
				}
				return(TRUE);
			}
			
			int Size(void) {return(sizeof(myData));}
		
		#ifdef _DEBUG
			const char* Signature(void) {return("CMyList");}
		#endif
		};

	Il distruttore della derivata deve eliminare gli elementi della lista chiamando DeleteAll(). La DeleteAll() della classe base
	chiama la virtuale PreDelete() che deve occuparsi di eliminare i dati (myData), per cui se la struct/classe per i dati contiene
	dati complessi o effettua allocazioni, nel distruttore eseguire le operazioni appropiate.
	L'elemento puo' essere aggiunto o direttamente, come in:
		myData* pData = new myData;
		CMyList.Add(myData);
	o indirettamente (la classe base si occupa di chiamare la virtuale Create()), come in:
		myData* pData = (myData*)CMyList.Add();
		strcpyn(pData,...,...);
	Se l'elemento/lista viene eliminato con la Delete()/DeleteAll(), i dati vengono eliminati tramite la chiamata alla virtuale PreDelete(),
	se invece si usa Remove()/RemoveAll(), i dati non vengono eliminati (delete), ma solo marcati come inutilizzati, tramite la chiamata alla
	virtuale Initialize(). La Delete() riusa il nodo ed elimina i dati associati, la Remove() riusa il nodo *ed* i dati associati. La Erase()
	si comporta come la Delete() con la differenza che marca l'elemento della lista (non i dati) come eliminato, per cui tale elemento non
	viene piu' riutilizzato.
	La virtuale Create() deve creare un nuovo oggetto, mentre la virtuale Initialize() deve reinizializzare i dati, *creando* un nuovo oggetto
	(con la chiamata a Create()) se il puntatore che riceve e' NULL (in tal caso l'elemento e' stato eliminato con Delete() o con Erase() e
	non con Remove()).
	La virtuale Size() specifica la dimensione dei dati utilizzati, se si tratta di una classe, restituire -1.

	Occhio a distinguere tra il puntatore all'elemento (nodo) della lista (CNode*, ITERATOR, etc.) ed il puntatore ai dati associati
	all'elemento (iter->data).
*/
class CNodeList
{
public:
	CNodeList();
	virtual ~CNodeList();

	// inserimento
	inline void*		Add		(void)			{return(CreateNode());}					// aggiunge con la virtuale Create() che deve restituire il ptr ai dati
	inline BOOL		Add		(void* ptr)		{return(CreateNode(ptr)!=(CNode*)NULL);}	// aggiunge il ptr ai dati
	inline ITERATOR	Insert	(void* ptr)		{return(CreateNode(ptr));}				// aggiunge il ptr ai dati
	
	// rimozione
	inline void		Remove	(ITERATOR iter)	{ReleaseNode(iter,RELEASE_REMOVE_MODE);}	// elimina l'elemento senza rilasciare i dati
	inline void		RemoveAll	(void)			{ReleaseNodeList(RELEASE_REMOVE_MODE);}		// elimina tutti gli elementi senza rilasciare i dati
	
	virtual BOOL		PreDelete	(ITERATOR) = 0;										// virtuale per l'eliminazione (rilascio) dei dati
	inline void		Delete	(ITERATOR iter)	{ReleaseNode(iter,RELEASE_DELETE_MODE);}	// elimina (marca per riuso) l'elemento rilasciando i dati
	inline void		DeleteAll	(void)			{ReleaseNodeList(RELEASE_DELETE_MODE);}		// elimina (marca per riuso) tutti gli elementi rilasciando i dati

	inline void		Erase	(ITERATOR iter)	{ReleaseNode(iter,RELEASE_ERASE_MODE);}		// elimina (marca come eliminato) l'elemento rilasciando i dati
	inline void		EraseAll	(void)			{ReleaseNodeList(RELEASE_ERASE_MODE);}		// elimina (marca come eliminati) tutti gli elementi rilasciando i dati

	// status
	inline int		Count	(void)			{return(CountNodes());}					// restituisce il numero di elementi presenti nella lista

	// ricerca
	inline ITERATOR	First	(void)			{return(FindFirstNode(USED_NODE));}		// restituisce il ptr al primo elemento della lista
	inline ITERATOR	Next		(ITERATOR iter)	{return(FindNextNode(iter));}				// restituisce il ptr all'elemento successivo della lista
	inline ITERATOR	FindAt	(int index)		{return(FindNodeByIndex(index));}			// restituisce il ptr elemento della lista relativo all'indice
	void*			GetAt	(int index);											// restituisce il ptr ai dati relativi all'elemento della lista relativo all'indice

	// virtuali pure (da definire obbligatoriamente nella derivata)
	virtual void*		Initialize(void*) = 0;											// riceve il ptr ai dati, deve restituire il ptr ai dati
	virtual void*		Create	(void) = 0;											// deve creare e restituire il ptr ai dati
	virtual int		Size		(void) = 0;											// dimensione dei dati associati all'elemento

#ifdef _DEBUG
	virtual const char*	Signature	(void) = 0;
#endif

private:
	// inserimento
	void*	CreateNode		(void);
	CNode*	CreateNode		(void* ptr);
	void		InitializeNode		(CNode* pNode,int status,void* ptr);
	void		InsertNode		(CNode* pNode);

	// ricerca
#ifdef _DEBUG
	BOOL		CheckNode			(CNode* pNode);
#endif
	int		CountNodes		(void);
	void		EnumerateNodes		(void);
	CNode*	FindFirstNode		(int status);
	CNode*	FindNextNode		(CNode* pNode);
	CNode*	FindNodeByIndex	(int index);
	
	// rimozione
	BOOL		ReleaseNode		(CNode* pNode,int nMode);
	void		ReleaseNodeList	(int nMode);

	int		m_nTot;
	CNode*	m_pFirstNode;
	CNode*	m_pLastNode;
};

/*
	ITEM
	elemento generico per la lista (nomi files, etc.)
*/
#define _MAX_ITEM (_MAX_DIR+_MAX_FNAME)
struct ITEM {
	char	item[_MAX_ITEM+1];
	int	index;
};

/*
	CItemList
	classe per la lista degli elementi (nomi files, etc.)
*/
class CItemList : public CNodeList
{
public:
	CItemList() : CNodeList() {}
	virtual ~CItemList() {CNodeList::DeleteAll();}
	void* Create(void)
	{
		return(new ITEM);
	}	
	void* Initialize(void* pVoid)
	{
		ITEM* pData = (ITEM*)pVoid;
		if(!pData)
			pData = (ITEM*)Create();
		if(pData)
		{
			memset(pData->item,'\0',_MAX_ITEM+1);
			pData->index = 0;
		}
		return(pData);
	}
	BOOL PreDelete(ITERATOR iter)
	{
		if((ITEM*)iter->data) delete ((ITEM*)iter->data),iter->data = (ITEM*)NULL;
		return(TRUE);
	}
	int Size(void) {return(sizeof(ITEM));}
#ifdef _DEBUG
	const char* Signature(void) {return("CItemList");}
#endif
};

#endif // _CNODELIST_H
