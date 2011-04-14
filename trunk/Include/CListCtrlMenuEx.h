/*
	CListCtrlMenuEx.h
	Classe derivata per la gestione di CListCtrl con menu a comparsa (MFC).
	Luca Piergentili, 06/07/98
	l.pierge@terra.es
*/
#ifndef _CLISTCTRLMENU_H
#define _CLISTCTRLMENU_H

// headers
#include "window.h"
#include <stdio.h>
#include <stdlib.h>
#include "CNodeList.h"

#define MAX_CTRL_COLS     32							// numero max di colonne
#define MAX_CTRLITEM_SIZE 128							// dimensione max del campo della colonna
#define MAX_CTRLCOL_SIZE  MAX_CTRLITEM_SIZE				// dimensione max della colonna
#define MAX_CTRLROW_SIZE  (MAX_CTRLITEM_SIZE*MAX_CTRL_COLS)	// dimensione max della riga (somma dei campi delle colonne)

#define CLISTCTRLMENU_RBUTTONDBLCLK 0					// w/l Param per doppio click destro
#define CLISTCTRLMENU_LBUTTONDBLCLK 1					// w/l Param per doppio click sinistro

/*
	CLISTCTRLMENU_SKIP
	valori per MoveItem()
*/
enum CLISTCTRLMENU_SKIP {
	CLISTCTRLMENU_PREV = -1,
	CLISTCTRLMENU_NEXT = +1
};

/*
	CTRLLISTMENU_COL_ARRAY
	array per le colonne
*/
struct CTRLLISTMENU_COL_ARRAY {
	char	item[MAX_CTRLCOL_SIZE+1];	// intestazione
	char type;					// tipo dati per ordinamento (C=carattere, N=numerico)
	BOOL order;					// flag per ordinamento (TRUE=ascendente, FALSE=discendente)
};

/*
	CTRLLISTMENU_ROW_ARRAY
	array per le righe
*/
struct CTRLLISTMENU_ROW_ARRAY {
	int	ico;					// indice (base 0) nella lista delle immagini per l'icona
	char	item[MAX_CTRLROW_SIZE+1];// testo della riga (separare i campi delle colonne con ;)
	int	menu;				// id risorsa del menu (carica i sottomenu 0 e 1, -1 se non prevede menu)
	BOOL	selected;				// flag per elemento selezionato
	int	index;				// indice interno
};

/*
	CTRLLISTMENU_IMG_ARRAY
	array per le immagini
*/
struct CTRLLISTMENU_IMG_ARRAY {
	int img;					// id risorsa icona
};

/*-------------------------------------------------------------------------------------------------------------

	CColList
	Classe derivata per la lista delle colonne.

-------------------------------------------------------------------------------------------------------------*/
class CColList : public CNodeList
{
public:
	CColList() {}
	virtual ~CColList() {CNodeList::DeleteAll();}
	BOOL PreDelete(ITERATOR iter)
	{
		if((CTRLLISTMENU_COL_ARRAY*)iter->data)
		{
			delete ((CTRLLISTMENU_COL_ARRAY*)iter->data);
			iter->data = (CTRLLISTMENU_COL_ARRAY*)NULL;
		}
		return(TRUE);
	}
};

/*-------------------------------------------------------------------------------------------------------------

	CRowList
	Classe derivata per la lista delle righe.

-------------------------------------------------------------------------------------------------------------*/
class CRowList : public CNodeList
{
public:
	CRowList() {}
	virtual ~CRowList() {CNodeList::DeleteAll();}
	BOOL PreDelete(ITERATOR iter)
	{
		if((CTRLLISTMENU_ROW_ARRAY*)iter->data)
		{
			delete ((CTRLLISTMENU_ROW_ARRAY*)iter->data);
			iter->data = (CTRLLISTMENU_ROW_ARRAY*)NULL;
		}
		return(TRUE);
	}
};

/*-------------------------------------------------------------------------------------------------------------

	CImgList
	Classe derivata per la lista delle immagini.

-------------------------------------------------------------------------------------------------------------*/
class CImgList : public CNodeList
{
public:
	CImgList() {}
	virtual ~CImgList() {CNodeList::DeleteAll();}
	BOOL PreDelete(ITERATOR iter)
	{
		if((CTRLLISTMENU_IMG_ARRAY*)iter->data)
		{
			delete ((CTRLLISTMENU_IMG_ARRAY*)iter->data);
			iter->data = (CTRLLISTMENU_IMG_ARRAY*)NULL;
		}
		return(TRUE);
	}
};

/*-------------------------------------------------------------------------------------------------------------

	CListCtrlMenuEx
	Classe derivata per la gestione estesa della lista.

-------------------------------------------------------------------------------------------------------------*/
class CListCtrlMenuEx : public CListCtrl
{
public:
	CListCtrlMenuEx()	{Initialize();}							// costruttore
	~CListCtrlMenuEx()	{Destroy();}								// distruttore

	BOOL	Create(CWnd* = AfxGetMainWnd(),CWinApp* = AfxGetApp());		// crea la lista (CWnd* per i messaggi, CWinApp* per gli id delle icone)

	int	AddCol(LPCSTR,char,int = -1,BOOL = FALSE);					// aggiunge la colonna (testo, tipo, dimensione, ordinamento)
	int	AddItem(LPCSTR,int = 0,int = -1);							// aggiunge la riga (testo diviso da ';', icona, menu)
	int	MoveItemUp(int i)	{return(MoveItem(i,CLISTCTRLMENU_PREV));}	// sposta la riga in alto
	int	MoveItemDown(int i)	{return(MoveItem(i,CLISTCTRLMENU_NEXT));}	// sposta la riga in basso
	BOOL	DeleteItem(int);										// elimina la riga
	BOOL	DeleteAllItems(void);									// elimina tutte le righe
	int	FindItem(LPCSTR);										// cerca la riga
	int	AddIcon(int);											// aggiunge l'icona al elenco delle immagini
	int	AddIcon(HICON);										// aggiunge l'icona al elenco delle immagini

	int	GetItemCount(void)	{return(m_iRowListCount);}				// tot. righe
	int	GetColCount(void)	{return(m_iColListCount);}				// tot. colonne
	int	GetColWidth(int i)										// dimensione della colonna
	{
		if(m_iColListCount <= 0)
			return(-1);
		else
			return(GetColumnWidth(i));
	}

	BOOL SetItemSelection(int,BOOL);
	BOOL GetItemSelection(int);									// status (interno) della riga (selezionata o meno)
	void ResetItemsSelection(void);
	
	void RightClickSelectsItem(BOOL flag) {m_bRightClickSelects = flag;}

	int	SelectItem(int);										// imposta l'item come selezionato
	int	SetCurrentItem(int);									// imposta la riga (interna) corrente
	int	GetCurrentItem(void);									// restituisce la riga (interna) corrente

	void	SetDoubleClickMessage(UINT i) {m_uMessage = i;}				// messaggio da inviare alla finestra per il doppio click sulla riga

	void SetDefaultMenu(int menu) {null_row.menu = menu;}

private:
	// membri
	void	Initialize(void);										// inizializza le variabili membro
	void	Destroy(void);											// rilascia le variabili membro
	CTRLLISTMENU_ROW_ARRAY* GetRowByItem(int);						// ricava il puntatore alla riga
	CTRLLISTMENU_COL_ARRAY* GetColByItem(int);						// ricava il puntatore alla colonna
	int	MoveItem(int,CLISTCTRLMENU_SKIP);							// muove la riga
	void	PopUpMenu(const CPoint&);								// visualizza il menu a comparsa

	// gestori
	void	OnColumnClick(NMHDR*,LRESULT*);							// click sull'header della colonna
	void	OnLButtonDown(UINT,CPoint);								// click sinistro sulla riga
	void	OnRButtonDown(UINT,CPoint);								// click destro sulla riga
	void	OnRButtonDblClk(UINT,CPoint);								// doppio click destro sulla riga
	void	OnLButtonDblClk(UINT,CPoint);								// doppio click sinistro sulla riga

	// variabili
	int			m_iCurrentItem;								// indice (interno) per la riga corrente
	CRowList*		m_pRowList;									// lista per le righe
	int			m_iRowListCount;								// totale delle righe
	CColList*		m_pColList;									// lista per le colonne
	int			m_iColListCount;								// totale delle colonne
	CImgList*		m_pImgList;									// lista per le icone
	CImageList*	m_pCImageList;									// lista immagini
	CImageList*	m_pCImageListSmall;								// lista immagini
	CWnd*		m_pCWnd;										// puntatore alla finestra principale per l'invio dei messaggi
	CWinApp*		m_pCWinApp;									// puntatore all'applicazione principale per gli id delle icone
	UINT			m_uMessage;									// messaggio da inviare al doppio click sulla riga
	BOOL			m_bRightClickSelects;							// 
	CTRLLISTMENU_ROW_ARRAY null_row;								// 

	DECLARE_MESSAGE_MAP()
};

#endif // _CLISTCTRLMENU_H
