/*
	CListCtrlEx.h
	Classe derivata per la gestione di CListCtrl con menu a comparsa (MFC).
	Luca Piergentili, 06/07/98
	lpiergentili@yahoo.com
*/
#ifndef _CLISTCTRLMENU_H
#define _CLISTCTRLMENU_H

#include "window.h"
#include <stdio.h>
#include <stdlib.h>
#include "CNodeList.h"

#define MIN_ITEM_WIDTH		10		// dimensione minima per la colonna (in pixels)
#define MAX_ITEM_SIZE		1024		// dimensione max del campo (in bytes)
#define ITEM_DELIMITER		';'		// carattere utilizzato come separatore per i campi

#define SORT_AUTO			0		// modalita' ordinamento
#define SORT_ASC			1
#define SORT_DESC			2
#define SORT_WPARAM			3
#define SORT_LPARAM			4

// tipi per le callback per l'ordinamento della lista sui wparam/lparam
typedef int (__cdecl *PFNSORTWPARAMCALLBACK)(WPARAM wParam1,WPARAM wParam2);
typedef int (__cdecl *PFNSORTLPARAMCALLBACK)(LPARAM lParam1,LPARAM lParam2);

// valori di default per i colori della griglia
#define CTRLLISTEX_DEFAULT_FOREGROUND_COLOR	0
#define CTRLLISTEX_DEFAULT_BACKGROUND_COLOR	16777215

/*
	ITEM_SKIP
	valori per MoveItem()
*/
enum ITEM_SKIP {
	PREVIOUS_ITEM = -1,
	NEXT_ITEM = +1
};

/*
	CTRL_COL
	struttura per la colonna
*/
struct CTRL_COL {
	char*	item;	// intestazione
	char		type;	// tipo dati per ordinamento (C=carattere, N=numerico, S=dimensione)
	BOOL		order;	// flag per ordinamento (TRUE=ascendente, FALSE=discendente)
};

/*
	CColList
	classe derivata per la lista delle colonne
*/
class CColList : public CNodeList
{
public:
	CColList() : CNodeList() {}
	virtual ~CColList() {CNodeList::DeleteAll();}
	void* Create(void)
	{
		CTRL_COL* pData = new CTRL_COL;
		// inizializza qui perche' in caso contrario il test su pData->item in Initialize() causa una GPF
		if(pData)
			memset(pData,'\0',sizeof(CTRL_COL));
		return(pData);
	}
	void* Initialize(void* pVoid)
	{
		CTRL_COL* pData = (CTRL_COL*)pVoid;
		if(pData)
			if(pData->item)
				delete [] pData->item,pData->item = NULL;
		if(!pData)
			pData = (CTRL_COL*)Create();
		if(pData)
			memset(pData,'\0',sizeof(CTRL_COL));
		return(pData);
	}
	BOOL PreDelete(ITERATOR iter)
	{
		if((CTRL_COL*)iter->data)
		{
			if(((CTRL_COL*)iter->data)->item)
			{
				delete [] ((CTRL_COL*)iter->data)->item;
				((CTRL_COL*)iter->data)->item = NULL;
			}
			delete ((CTRL_COL*)iter->data);
			iter->data = (CTRL_COL*)NULL;
		}
		return(TRUE);
	}
	int Size(void) {return(sizeof(CTRL_COL));}
#ifdef _DEBUG
	const char* Signature(void) {return("CColList");}
#endif
};

/*
	CTRL_ROW
	struttura per l'elemento (riga)
*/
struct CTRL_ROW {
	int		ico;			// indice (base 0) nella lista delle immagini per l'icona
	char*	item;		// testo della riga (separare i campi delle colonne con ;)
	int		len;			// lunghezza testo
	WPARAM	wParam;		// dati extra (definiti dal chiamante)
	LPARAM	lParam;		// dati extra (definiti dal chiamante)
	int		menu;		// id risorsa del menu (carica i sottomenu 0 e 1, -1 se non prevede menu)
	BOOL		selected;		// flag per elemento selezionato
	int		index;		// indice interno
};

/*
	CRowList
	classe derivata per la lista delle righe
*/
class CRowList : public CNodeList
{
public:
	CRowList() : CNodeList() {}
	virtual ~CRowList() {CNodeList::DeleteAll();}
	void* Create(void)
	{
		CTRL_ROW* pData = new CTRL_ROW;
		if(pData)
		{
			memset(pData,'\0',sizeof(CTRL_ROW));
			pData->wParam = (WPARAM)-1L;
			pData->lParam = (LPARAM)-1L;
			pData->menu = -1;
		}
		return(pData);
	}	
	void* Initialize(void* pVoid)
	{
		CTRL_ROW* pData = (CTRL_ROW*)pVoid;
		if(!pData)
			pData = (CTRL_ROW*)Create();
		else
		{
			pData->wParam = (WPARAM)-1L;
			pData->lParam = (LPARAM)-1L;
		}
		return(pData);
	}
	BOOL PreDelete(ITERATOR iter)
	{
		if((CTRL_ROW*)iter->data)
		{
			if(((CTRL_ROW*)iter->data)->item)
			{
				delete [] ((CTRL_ROW*)iter->data)->item;
				((CTRL_ROW*)iter->data)->item = NULL;
			}
			delete ((CTRL_ROW*)iter->data);
			iter->data = (CTRL_ROW*)NULL;
		}
		return(TRUE);
	}
	int Size(void) {return(sizeof(CTRL_ROW));}
#ifdef _DEBUG
	const char* Signature(void) {return("CRowList");}
#endif
};

/*
	CTRL_IMG
	struttura per l'icona
*/
struct CTRL_IMG {
	int	img;	// id risorsa icona
};

/*
	MOUSECLICK
	struttura per le coordinate del click del mouse
*/
struct MOUSECLICK {
	POINT	point;
	UINT		flags;
};

/*
	CListCtrlEx
*/
class CListCtrlEx : public CListCtrl
{
public:
	CListCtrlEx();
	virtual ~CListCtrlEx();

	// creazione
	BOOL			Create				(CWnd* = AfxGetMainWnd(),CWinApp* = AfxGetApp());
	void			SetStyle				(DWORD nFlags);
	inline DWORD	GetStyle				(void) {return(::GetWindowLong(this->m_hWnd,GWL_STYLE));}
	inline void	SetExStyle			(DWORD nFlags) {ListView_SetExtendedListViewStyle(this->m_hWnd,nFlags);}
	inline DWORD	GetExStyle			(void) {return(ListView_GetExtendedListViewStyle(this->m_hWnd));}

	// flag per selezione elemento con click destro del mouse
	inline void	RightClickSelects		(BOOL bSelect) {m_bRightClickSelects = bSelect;}

	// messaggi da inviare
	inline void	SetOnColumnClickMessage	(UINT nColumnClickMessage)	{m_nColumnClickMessage = nColumnClickMessage;}
	inline void	SetMenuMessage			(UINT nMenuMessage)			{m_nMenuMessage = nMenuMessage;}
	inline void	SetLButtonDownMessage	(UINT nLButtonDownMessage)	{m_nLButtonDownMessage = nLButtonDownMessage;}
	inline void	SetLButtonUpMessage		(UINT nLButtonUpMessage)		{m_nLButtonUpMessage = nLButtonUpMessage;}
	inline void	SetRButtonDownMessage	(UINT nRButtonDownMessage)	{m_nRButtonDownMessage = nRButtonDownMessage;}
	inline void	SetRButtonUpMessage		(UINT nRButtonUpMessage)		{m_nRButtonUpMessage = nRButtonUpMessage;}
	inline void	SetLDoubleClickMessage	(UINT nLButtonDblClkMessage)	{m_nLButtonDblClkMessage = nLButtonDblClkMessage;}
	inline void	SetRDoubleClickMessage	(UINT nRButtonDblClkMessage)	{m_nRButtonDblClkMessage = nRButtonDblClkMessage;}

	// imposta il menu popup di default
	inline void	SetDefaultMenu			(int nMenuID) {m_NullRow.menu = nMenuID;}

	inline void	SetIlcColor			(int nIlcColor) {m_IlcColor = nIlcColor;}
	inline void	SetForegroundColor		(COLORREF nForegroundColor) {m_nForegroundColor = nForegroundColor;}
	inline void	SetBackgroundColor		(COLORREF nBackgroundColor) {m_nBackgroundColor = nBackgroundColor;}

	void			SetRedraw				(BOOL bRedraw);
	void			AutoSizeColumns		(int nCol = -1,int nWidth = 0) ;
	int			GetColumnCount			(void);

	// elementi
	int			AddCol				(LPCSTR lpcszColumnText,char cDataType = 'C',int nWidth = -1,BOOL bDescend = FALSE);
	int			AddItem				(LPCSTR lpcszItemText,int nIconIndex = 0,int nMenuIndex = -1,WPARAM wParam = (WPARAM)-1,LPARAM lParam = (LPARAM)-1L);
	BOOL			UpdateItem			(int nItem,LPCSTR lpcszItemText,int nIconIndex = 0,int nMenuIndex = -1);
	BOOL			DeleteItem			(int nItem);
	BOOL			DeleteAllItems			(BOOL bFlush = FALSE);
	void			Flush				(void);
	int			FindItem				(LPCSTR lpcszText,BOOL bIgnoreCase = TRUE);
	int			FindItem				(LPCSTR lpcszItemText,LPCSTR lpcszColumnText,UINT nCol);
	inline int	MoveItemUp			(int nItem) {return(MoveItem(nItem,PREVIOUS_ITEM));}
	inline int	MoveItemDown			(int nItem) {return(MoveItem(nItem,NEXT_ITEM));}
	int			AddIcon				(int nIconID);
	int			AddIcon				(HICON hIcon);
	void			RemoveIcon			(int nIconID);
	void			RemoveAllIcons			(void);

	// conteggi/dimensioni
	inline int	GetItemCount			(void) {return(CListCtrl::GetItemCount());}
	inline int	GetColCount			(void) {return(m_nTotColumns);}
	inline int	GetColWidth			(int nCol) {return(m_nTotColumns <= 0 ? -1 : CListCtrl::GetColumnWidth(nCol));}
	LPCSTR		GetItemToken			(int,LPCSTR);
	
	// selezione elementi
	int			SelectItem			(int nItem);
	int			SetCurrentItem			(int nItem);
	int			GetCurrentItem			(void);

	// elementi selezionati
	BOOL			SetItemSelection		(int nItem,BOOL bSelected);
	BOOL			GetItemSelection		(int nItem);
	
	// dati extra
	void			SetItemwParam			(int nItem,WPARAM wParam);
	WPARAM		GetItemwParam			(int nItem);
	void			SetItemlParam			(int nItem,LPARAM lParam);
	LPARAM		GetItemlParam			(int nItem);

	// ordinamento
	void			Sort					(int nOrder = SORT_AUTO,int nCol = 0);
	inline void	SetwParamCallback		(PFNSORTWPARAMCALLBACK pfnSortwParamCallback) {m_pfnSortwParamCallback = pfnSortwParamCallback;}
	inline void	SetlParamCallback		(PFNSORTLPARAMCALLBACK pfnSortlParamCallback) {m_pfnSortlParamCallback = pfnSortlParamCallback;}

	inline void	SetMultipleSelection	(BOOL bMultipleSelection) {m_bMultipleSelection = bMultipleSelection;}

private:
	// membri
	void			Init					(void);
	void			Destroy				(void);
	CTRL_ROW*		GetRowByItem			(int nItem);
	CTRL_COL*		GetColByItem			(int nItem);
	void			ResetItemsSelection		(void);
	int			MoveItem				(int nItem,ITEM_SKIP skip);
	void			PopUpMenu				(const CPoint& point);

	// gestori
	void			OnGetDispInfo			(NMHDR* pnmh,LRESULT* /*pResult*/);
	void			OnColumnClick			(NMHDR*,LRESULT*);
	void			OnCustomDraw			(NMHDR* pNMHDR,LRESULT* pResult);
	void			OnLButtonDown			(UINT,CPoint);
	void			OnLButtonUp			(UINT,CPoint);
	void			OnRButtonDown			(UINT,CPoint);
	void			OnRButtonUp			(UINT,CPoint);
	void			OnLButtonDblClk		(UINT,CPoint);
	void			OnRButtonDblClk		(UINT,CPoint);

	// ordinamento
	int			CompareItems			(LPARAM,LPARAM);
	static int CALLBACK	CompareItems		(LPARAM,LPARAM,LPARAM);

	// variabili
	int			m_nRedrawCount;				// flag per refresh
	int			m_nCurrentItem;				// indice (interno) per la riga corrente
	CRowList		m_RowList;					// lista per le righe
	CColList		m_ColList;					// lista per le colonne
	int			m_nTotColumns;					// totale delle colonne
	int			m_nSelectedCol;				// colonna selezionata
	char			m_cColumnType;					// tipo dati (colonna) per ordinamento
	BOOL			m_bAscending;					// flag per direzione ordinamento righe
	int			m_nOrder;						// flag per modalita' ordinamento righe
	int			m_IlcColor;					// flag per visualizzazione icona
	COLORREF		m_nForegroundColor;				// colore di primo piano per le righe
	COLORREF		m_nBackgroundColor;				// colore di sfondo per le righe
	CImageList	m_ImageListSmall;				// lista immagini
	CWnd*		m_pWnd;						// puntatore alla finestra principale per l'invio dei messaggi
	CWinApp*		m_pWinApp;					// puntatore all'applicazione principale per gli id delle icone
	POINT		m_Point;						// coordinate per i click del mouse sugli elementi del controllo
	MOUSECLICK	m_MouseClick;					// coordinate per i click del mouse sugli elementi del controllo
	UINT			m_nColumnClickMessage;			// messaggio per il click sulla colonna
	BOOL			m_bOnColumnClicked;				// flag per distinguere tra il click reale e la chiamata a Sort()
	UINT			m_nMenuMessage;				// messaggio per il menu popup
	UINT			m_nLButtonDownMessage;			// messaggio da inviare per click bottone sinistro
	UINT			m_nLButtonUpMessage;			// messaggio da inviare per rilascio bottone sinistro
	UINT			m_nRButtonDownMessage;			// messaggio da inviare per click bottone destro
	UINT			m_nRButtonUpMessage;			// messaggio da inviare per rilascio bottone destro
	UINT			m_nLButtonDblClkMessage;			// messaggio da inviare per doppio click bottone sinistro
	UINT			m_nRButtonDblClkMessage;			// messaggio da inviare per doppio click bottone destro
	BOOL			m_bRightClickSelects;			// flag per selezione con click bottone destro
	CTRL_ROW		m_NullRow;					// elemento interno
	BOOL			m_bMultipleSelection;			// flag per selezione multiple
	PFNSORTWPARAMCALLBACK m_pfnSortwParamCallback;	// callback per l'ordinamento su wparam
	PFNSORTLPARAMCALLBACK m_pfnSortlParamCallback;	// callback per l'ordinamento su lparam

	DECLARE_MESSAGE_MAP()
};

#endif // _CLISTCTRLMENU_H
