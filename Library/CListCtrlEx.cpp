/*
	CListCtrlEx.cpp
	Classe derivata per la gestione di CListCtrl con menu a comparsa (MFC).
	Luca Piergentili, 06/07/98
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
#include "win32api.h"
#include "CNodeList.h"
#include "CListCtrlEx.h"

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

BEGIN_MESSAGE_MAP(CListCtrlEx,CListCtrl)
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()							// click bottone sinistro
	ON_WM_LBUTTONUP()							// rilascio bottone sinistro
	ON_WM_RBUTTONDOWN()							// click bottone destro
	ON_WM_RBUTTONUP()							// rilascio bottone destro
	ON_WM_LBUTTONDBLCLK()						// doppio click bottone sinistro
	ON_WM_RBUTTONDBLCLK()						// doppio click bottone destro
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO,OnGetDispInfo)	// per impostare il valore della riga
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK,OnColumnClick)	// click sull'header della colonna
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW,OnCustomDraw)
END_MESSAGE_MAP()

/*
	CListCtrlEx()
*/
CListCtrlEx::CListCtrlEx()
{
	Init();
}

/*
	~CListCtrlEx()
*/
CListCtrlEx::~CListCtrlEx()
{
	Destroy();
}

/*
	Init()
*/
void CListCtrlEx::Init(void)
{
	m_nRedrawCount          = 0;
	m_nCurrentItem          = LB_ERR;
	m_RowList.RemoveAll();
	m_ColList.RemoveAll();
	m_nTotColumns           = 0;
	m_nSelectedCol          = 0;
	m_cColumnType           = 'C';
	m_bAscending            = FALSE;
	m_nOrder                = SORT_AUTO;
	m_pfnSortwParamCallback  = NULL;
	m_pfnSortlParamCallback  = NULL;
/*	#define ILC_MASK          0x0001
	#define ILC_COLOR         0x0000
	#define ILC_COLORDDB      0x00FE
	#define ILC_COLOR4        0x0004
	#define ILC_COLOR8        0x0008
	#define ILC_COLOR16       0x0010
	#define ILC_COLOR24       0x0018
	#define ILC_COLOR32       0x0020
	#define ILC_PALETTE       0x0800      // (not implemented)
	m_IlcColor              = ILC_COLOR16; //ILC_MASK; */
	m_nForegroundColor      = RGB(0,0,0);
	m_nBackgroundColor      = RGB(255,255,255);
	m_ImageListSmall.DeleteImageList();
	m_pWnd                  = NULL;
	m_pWinApp               = NULL;
	m_Point.x               = 0;
	m_Point.y               = 0;
	m_MouseClick.point.x    = 0;
	m_MouseClick.point.y    = 0;
	m_MouseClick.flags      = 0;
	m_bOnColumnClicked      = TRUE;
	m_nColumnClickMessage   = (UINT)-1L;
	m_nMenuMessage          = (UINT)-1L;
	m_nLButtonDownMessage   = (UINT)-1L;
	m_nLButtonUpMessage     = (UINT)-1L;
	m_nRButtonDownMessage   = (UINT)-1L;
	m_nRButtonUpMessage     = (UINT)-1L;
	m_nLButtonDblClkMessage = (UINT)-1L;
	m_nRButtonDblClkMessage = (UINT)-1L;
	m_bRightClickSelects    = FALSE;
	m_NullRow.ico           = -1;
	m_NullRow.item          = "";
	m_NullRow.menu          = -1;
	m_NullRow.selected      = FALSE;
	m_NullRow.index         = -1;
	m_bMultipleSelection    = FALSE;
}

/*
	Destroy()
*/
void CListCtrlEx::Destroy(void)
{
	m_nRedrawCount          = 0;
	m_nCurrentItem          = LB_ERR;
	m_RowList.RemoveAll();
	m_ColList.RemoveAll();
	m_nTotColumns           = 0;
	m_nSelectedCol          = 0;
	m_cColumnType           = 'C';
	m_bAscending            = FALSE;
	m_nOrder                = SORT_AUTO;
	m_IlcColor              = -1;
	m_ImageListSmall.DeleteImageList();
	m_pWnd                  = NULL;
	m_pWinApp               = NULL;
	m_Point.x               = 0;
	m_Point.y               = 0;
	m_MouseClick.point.x    = 0;
	m_MouseClick.point.y    = 0;
	m_MouseClick.flags      = 0;
	m_bOnColumnClicked      = TRUE;
	m_nColumnClickMessage   = (UINT)-1L;
	m_nMenuMessage          = (UINT)-1L;
	m_nLButtonDownMessage   = (UINT)-1L;
	m_nLButtonUpMessage     = (UINT)-1L;
	m_nRButtonDownMessage   = (UINT)-1L;
	m_nRButtonUpMessage     = (UINT)-1L;
	m_nLButtonDblClkMessage = (UINT)-1L;
	m_nRButtonDblClkMessage = (UINT)-1L;
	m_bRightClickSelects    = FALSE;
	m_NullRow.ico           = -1;
	m_NullRow.item          = "";
	m_NullRow.menu          = -1;
	m_NullRow.selected      = FALSE;
	m_NullRow.index         = -1;
	m_bMultipleSelection    = FALSE;
}

/*
	Create()

	Crea il controllo.
	
	CWnd* pCWnd = AfxGetMainWnd()		puntatore alla finestra principale per l'invio dei messaggi
	CWinApp* pCWinApp = AfxGetApp()	puntatore all'applicazione principale gli id delle icone
*/
BOOL CListCtrlEx::Create(CWnd* pCWnd,CWinApp* pCWinApp)
{
	// inizializza
	Destroy();
	Init();

	// puntatore alla finestra/applicazione principale
	m_pWnd = pCWnd;
	m_pWinApp = pCWinApp;

	// elimina le colonne eventualmente presenti
	while(CListCtrl::DeleteColumn(0))
		;

	// elimina le righe eventualmente presenti
	CListCtrl::DeleteAllItems();

	// liste immagini
	BOOL bCreated = TRUE;
	m_IlcColor = m_IlcColor <= 0 ? ILC_COLOR16 : m_IlcColor;
	if(bCreated)
	{
		m_ImageListSmall.DeleteImageList();
		bCreated = m_ImageListSmall.Create(16,16,ILC_MASK | ILC_COLORDDB,0,5);
		m_ImageListSmall.SetBkColor(CLR_NONE/*ILD_TRANSPARENT*/);
	}

	return(bCreated);
}

/*
	SetStyle()
*/
void	CListCtrlEx::SetStyle(DWORD nFlags)
{
	//LVS_REPORT|LVS_SHOWSELALWAYS|LVS_SINGLESEL|LVS_NOSORTHEADER
	DWORD dwStyle = GetStyle();

	if(!(dwStyle & nFlags))
		dwStyle |= nFlags;

	::SetWindowLong(this->m_hWnd,GWL_STYLE,dwStyle);
}

/*
	SetRedraw()
*/
void CListCtrlEx::SetRedraw(BOOL bRedraw)
{
	if(!bRedraw)
	{
		if(m_nRedrawCount++ <= 0)
			CListCtrl::SetRedraw(false);
	}
	else
	{
		if(--m_nRedrawCount <= 0)
		{
			CListCtrl::SetRedraw(true);
			m_nRedrawCount = 0;
			CListCtrl::Invalidate();
		}
	}
}

/*
	AutoSizeColumns()

	Dimensiona automaticamente le colonne in base al contenuto/etichetta (solo se il controllo si trova in modalita' report).
	Da chiamare dopo aver riempito il controllo.
*/
void CListCtrlEx::AutoSizeColumns(int nCol/*=-1*/,int nWidth/*=0*/) 
{
	SetRedraw(FALSE);

	if(!(GetStyle() & LVS_REPORT))
		return;

	int nMinCol = nCol < 0 ? 0 : nCol;
	int nMaxCol = nCol < 0 ? GetColumnCount()-1 : nCol;
	
	for(nCol = nMinCol; nCol <= nMaxCol; nCol++) 
	{
		if(nWidth==0)
		{
			CListCtrl::SetColumnWidth(nCol,LVSCW_AUTOSIZE);
			int nWidthAutosize = CListCtrl::GetColumnWidth(nCol);

			CListCtrl::SetColumnWidth(nCol,LVSCW_AUTOSIZE_USEHEADER);
			int nWidthUseHeader = CListCtrl::GetColumnWidth(nCol);

			CListCtrl::SetColumnWidth(nCol,max(MIN_ITEM_WIDTH,max(nWidthAutosize,nWidthUseHeader)));
		}
		else
			CListCtrl::SetColumnWidth(nCol,nWidth);     
	}

	SetRedraw(TRUE);
}

/*
	GetColumnCount()

	Ricava il numero di colonne del controllo.
*/
int CListCtrlEx::GetColumnCount(void)
{
	CHeaderCtrl* pHeader = (CHeaderCtrl*)CListCtrl::GetDlgItem(0);
	return(pHeader ? pHeader->GetItemCount() : 0);
}

/*
	GetRowByItem()

	Ricava il puntatore alla riga relativa all'elemento.
	L'elemento (indice base 0 dell'elemento di CListCtrl) corrisponde con l'indice (base 0) della riga,
	da mantenere aggiornato in base agli spostamenti degli items di ClistCtrl.
	Da non confondere con l'indice interno di CNodeList.

	int nItem		indice (base 0) dell'item per la riga da cercare
*/
CTRL_ROW* CListCtrlEx::GetRowByItem(int nItem)
{
	ITERATOR iter;
	CTRL_ROW* ra = NULL;

	if((iter = m_RowList.First())!=(ITERATOR)NULL)
	{
		do
		{
			ra = (CTRL_ROW*)iter->data;
			
			if(ra)
			{
				if(ra->index==nItem)
					break;
				else
					ra = NULL;
			}
			
			iter = m_RowList.Next(iter);
		
		} while(iter!=(ITERATOR)NULL);
	}
	
	return(ra);
}

/*
	GetColByItem()

	Ricava il puntatore alla colonna relativa all'elemento.
	L'elemento (indice base 0 dell'elemento di CListCtrl) corrisponde con l'indice (base 0) della riga.
	Dato che le colonne non cambiano di posizione, tale indice corrisponde con l'indice interno di
	CNodeList, per cui la ricerca puo essere effettuata con FindAt().

	int nItem		indice (base 0) dell'item per la colonna da cercare
*/
CTRL_COL* CListCtrlEx::GetColByItem(int nItem)
{
	ITERATOR iter;
	CTRL_COL* ca = NULL;

	if((iter = m_ColList.FindAt(nItem))!=NULL)
		ca = (CTRL_COL*)iter->data;

	return(ca);
}

/*
	AddCol()

	Aggiunge la colonna al controllo.

	LPCSTR lpcszColumnText	testo per l'intestazione della colonna
	char cDataType	= 'C'	carattere per il tipo dei dati contenuti nella colonna: 'C' carattere, 'N' numerico, 'S' dimensione in bytes
	int nWidth = -1		dimensione della colonna (in pixel)
	BOOL bDescend = FALSE	flag per ordinamento (FALSE=discendente, TRUE=ascendente)
*/
int CListCtrlEx::AddCol(LPCSTR lpcszColumnText,char cDataType,int nWidth,BOOL bDescend)
{
	int nCurrentCol = LB_ERR;

	// crea la colonna
	CTRL_COL* ca = (CTRL_COL*)m_ColList.Add();

	if(ca)
	{
		memset(ca,'\0',sizeof(CTRL_COL));
		int len = strlen(lpcszColumnText)+1;
		ca->item = new char[len];
		memset(ca->item,'\0',len);
		
		// aggiunge la colonna alla lista
		CSize size;
		LPSTR lpToken;
		LV_COLUMN lvcolumn;

		// indice (base 0) della nuova colonna
		int nCol = m_nTotColumns++;

		strcpy(ca->item,lpcszColumnText);

		// ordinamento colonna (default FALSE = discendente)
		ca->type  = cDataType;
		ca->order = bDescend;
		
		// elemento
		lvcolumn.mask     = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
		lvcolumn.fmt      = LVCFMT_LEFT;
		lvcolumn.pszText  = ca->item;
		lvcolumn.iSubItem = nCol;
		
		// calcola la dimensione (in pixel) della colonna in base alla dimensione del testo
		// della colonna e del testo della prima riga (+15 per l'icona)
		if(nWidth <= 0)
		{
			lpToken = "";

			if(CListCtrl::GetItemCount() > 0)
			{
				CTRL_ROW* ra;
				if((ra = GetRowByItem(0))!=NULL)
					lpToken = (LPSTR)GetItemToken(nCol,ra->item);
			}
			else
			{
				lpToken = ca->item;
			}

			int nColSize  = CListCtrl::GetStringWidth(lvcolumn.pszText);
			int nItemSize = CListCtrl::GetStringWidth(lpToken);
			size.cx       = nColSize > nItemSize ? nColSize : nItemSize;
			lvcolumn.cx   = size.cx + 15;
		}
		else
		{
			lvcolumn.cx = nWidth;
		}
		
		// inserisce la colonna
		if((nCurrentCol = CListCtrl::InsertColumn(nCol,&lvcolumn)) < 0)
			nCurrentCol = LB_ERR;
	}

	return(nCurrentCol);
}

/*
	AddItem()

	Aggiunge la riga al controllo.

	LPCSTR lpcszItemText	testo per l'elemento della riga (separare i campi ';')
	int nIconIndex = 0		indice dell'icona relativa
	int nMenuIndex = -1		indice del menu relativo
*/
int CListCtrlEx::AddItem(LPCSTR lpcszItemText,int nIconIndex/* = 0*/,int nMenuIndex/* = -1*/,WPARAM wParam/* = (WPARAM)-1*/,LPARAM lParam/* = (LPARAM)-1L*/) 
{
	int nCurrentItem = LB_ERR;

	// crea la riga
	CTRL_ROW* ra = (CTRL_ROW*)m_RowList.Add();
	
	if(ra)
	{
		int len = strlen(lpcszItemText)+1;
		if(!ra->item)
		{
			memset(ra,'\0',sizeof(CTRL_ROW));
			ra->item = new char[len];
			ra->len = len;
			memset(ra->item,'\0',len);
		}
		else
		{
			if(len < ra->len)
				memset(ra->item,'\0',ra->len);
			else
			{
				delete [] ra->item;
				ra->item = new char[len];
				ra->len = len;
				memset(ra->item,'\0',len);
			}
		}
		strcpy(ra->item,lpcszItemText);
		ra->wParam = wParam;
		ra->lParam = lParam;

		// aggiunge la riga alla lista
		int nItem,nSubItem;
		LV_ITEM lvitem;

		// indice (base 0) della nuova riga
		nItem = CListCtrl::GetItemCount();

		// colonne della riga
		for(nSubItem = 0,nCurrentItem = 0; nSubItem < m_nTotColumns && nCurrentItem!=LB_ERR; nSubItem++)
		{
			// status della riga (default su non selezionata)
			ra->ico      = nIconIndex;
			ra->menu     = nMenuIndex;
			ra->selected = FALSE;
			ra->index    = nItem;

			// la riga viene passata in un unico buffer, diviso in colonne dal carattere ';'
			lvitem.pszText = (LPSTR)GetItemToken(nSubItem,ra->item);
			
			int nItemSize = CListCtrl::GetStringWidth(lvitem.pszText);
			//int nColSize = CListCtrl::GetColumnWidth(nSubItem);
			if(nItemSize >= CListCtrl::GetColumnWidth(nSubItem))
				CListCtrl::SetColumnWidth(nSubItem,nItemSize+15);

			lvitem.iSubItem = nSubItem;

			if(nSubItem==0)
			{
				lvitem.mask   = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
				lvitem.iItem  = nItem;
				lvitem.iImage = ra->ico; // icona relativa (indice base 0 nell'array delle immagini)
				lvitem.lParam = (LPARAM)ra;
				lvitem.pszText = LPSTR_TEXTCALLBACK;
				
				if((nCurrentItem  = CListCtrl::InsertItem(&lvitem)) < 0)
					nCurrentItem = LB_ERR;
			}
			else
			{
				lvitem.mask  = LVIF_TEXT;
				lvitem.iItem = nCurrentItem;
				
				CListCtrl::SetItem(&lvitem);
			}
		}
	}

	return(nCurrentItem);
}

/*
	UpdateItem()

	Aggiorna l'elemento del controllo.
*/
BOOL CListCtrlEx::UpdateItem(int nItem,LPCSTR lpcszItemText,int nIconIndex/*=0*/,int nMenuIndex/*=-1*/)
{
	BOOL bUpdated = FALSE;
	ITERATOR iter;
	CTRL_ROW* ra = NULL;

	// cerca la riga relativa all'indice
	if((iter = m_RowList.First())!=(ITERATOR)NULL)
	{
		do
		{
			ra = (CTRL_ROW*)iter->data;
		
			if(ra)
			{
				if(ra->index==nItem)
					break;
				else
					ra = NULL;
			}
		
			iter = m_RowList.Next(iter);
		
		} while(iter!=(ITERATOR)NULL);
	}

	if(ra)
	{
		int nCurrent = GetCurrentItem();
		
		CListCtrl::DeleteItem(nItem);

		if(ra->item)
		{
			delete [] ra->item;
			ra->item = NULL;
		}
		int len = strlen(lpcszItemText)+1;
		ra->item = new char[len];
		memset(ra->item,'\0',len);
		strcpy(ra->item,lpcszItemText);

		int nSubItem,nCurrentItem;
		LV_ITEM lvitem;

		// colonne della riga
		for(nSubItem = 0,nCurrentItem = 0; nSubItem < m_nTotColumns && nCurrentItem!=LB_ERR; nSubItem++)
		{
			// status della riga (default su non selezionata)
			ra->ico      = nIconIndex;
			ra->menu     = nMenuIndex;
			ra->selected = FALSE;
			ra->index    = nItem;

			// la riga viene passata in un unico buffer, diviso in colonne dal carattere ';'
			lvitem.pszText = (LPSTR)GetItemToken(nSubItem,ra->item);
			
			int nItemSize = CListCtrl::GetStringWidth(lvitem.pszText);
			//int nColSize = CListCtrl::GetColumnWidth(nSubItem);
			if(nItemSize >= CListCtrl::GetColumnWidth(nSubItem))
				CListCtrl::SetColumnWidth(nSubItem,nItemSize+15);

			lvitem.iSubItem = nSubItem;

			if(nSubItem==0)
			{
				lvitem.mask   = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
				lvitem.iItem  = nItem;
				lvitem.iImage = ra->ico; // icona relativa (indice base 0 nell'array delle immagini)
				lvitem.lParam = (LPARAM)ra;
				lvitem.pszText = LPSTR_TEXTCALLBACK;
				
				if((nCurrentItem  = CListCtrl::InsertItem(&lvitem)) < 0)
					nCurrentItem = LB_ERR;
			}
			else
			{
				lvitem.mask  = LVIF_TEXT;
				lvitem.iItem = nCurrentItem;
				
				CListCtrl::SetItem(&lvitem);
			}
		}
		
		SelectItem(nCurrent);
		CListCtrl::Update(nItem);
		
		bUpdated = TRUE;
	}

	return(bUpdated);
}

/*
	DeleteItem()

	Elimina l'elemento dal controllo.

	int nItem		indice (base 0) della riga.
*/
BOOL CListCtrlEx::DeleteItem(int nItem)
{
	ITERATOR iter;
	CTRL_ROW* ra = NULL;

	// elimina la riga dal controllo
	BOOL bDeleted = CListCtrl::DeleteItem(nItem);

	// cerca la riga relativa all'indice e la elimina dalla lista
	if((iter = m_RowList.First())!=(ITERATOR)NULL)
	{
		do
		{
			ra = (CTRL_ROW*)iter->data;
		
			if(ra)
			{
				if(ra->index==nItem)
					break;
				else
					ra = NULL;
			}
		
			iter = m_RowList.Next(iter);
		
		} while(iter!=(ITERATOR)NULL);
	}
	
	if(ra)
		m_RowList.Remove(iter);

	// rinumera gli indici delle righe presenti nella lista
	int tot = m_RowList.Count()+1;

	for(int n = ++nItem; n < tot; n++)
	{
		if((ra = GetRowByItem(n))!=(CTRL_ROW*)NULL)
			ra->index = n-1;
	}

	return(bDeleted);
}

/*
	DeleteAllItems()

	Elimina tutti gli elementi dal controllo.
*/
BOOL CListCtrlEx::DeleteAllItems(BOOL bFlush/* = FALSE*/)
{
	// elimina le righe dal controllo
	BOOL bDeleted = CListCtrl::DeleteAllItems();

	// elimina le righe dalla lista
	if(bFlush)
		m_RowList.DeleteAll();
	else
		m_RowList.RemoveAll();

	return(bDeleted);
}

/*
	Flush()

	Elimina tutti gli elementi dal controllo azzerando la lista interna.
*/
void CListCtrlEx::Flush(void)
{
	if(CListCtrl::GetItemCount() > 0)
		CListCtrl::DeleteAllItems();
	m_RowList.DeleteAll();
}

/*
	FindItem()

	Cerca l'elemento nel controllo.

	LPCSTR lpcszText	testo dell'elemento da cercare.
*/
int CListCtrlEx::FindItem(LPCSTR lpcszText,BOOL bIgnoreCase/*=TRUE*/)
{
	int nItem = -1;
	LVFINDINFO lv = {0};
	lv.flags = LVFI_STRING;
	lv.psz = lpcszText;
	
	if((nItem = CListCtrl::FindItem(&lv,-1)) >= 0)
		if(!bIgnoreCase)
		{
			char szItem[MAX_ITEM_SIZE+1] = {0};
			CListCtrl::GetItemText(nItem,0,szItem,sizeof(szItem)-1);
			if(strcmp(szItem,lpcszText)!=0)
				nItem = -1;
		}
	
	return(nItem);
}

/*
	FindItem()

	Cerca la coppia di elementi nel controllo.

	LPCSTR lpcszItemText	testo dell'elemento da cercare.
	LPCSTR lpcszColumnText	testo addizionale dell'elemento da cercare.
	UINT nCol				indice (base 0) della colonna per il testo addizionale
*/
int CListCtrlEx::FindItem(LPCSTR lpcszItemText,LPCSTR lpcszColumnText,UINT nCol)
{
	char szItem[MAX_ITEM_SIZE+1] = {0};
	LVFINDINFO lv = {0};
	lv.flags = LVFI_STRING;
	lv.psz = lpcszItemText;
	int nItem = CListCtrl::FindItem(&lv,-1);
	
	while(nItem >= 0)
	{
		CListCtrl::GetItemText(nItem,nCol,szItem,sizeof(szItem)-1);
		if(strcmp(szItem,lpcszColumnText)==0)
			break;
		else
			nItem = CListCtrl::FindItem(&lv,nItem);
	}

	return(nItem);
}

/*
	MoveItem()

	Sposta l'elemento in alto o in basso di una posizione.

	int nItem			indice (base 0) dell'elemento
	ITEM_SKIP skip		flag per tipo spostamento (precedente/successivo)
*/
int CListCtrlEx::MoveItem(int nItem,ITEM_SKIP skip)
{
	int nCurrentItem = LB_ERR;
	int nNewItem = (nItem + (skip==PREVIOUS_ITEM ? -1 : (skip==NEXT_ITEM ? +1 : -1)));

	// controlla l'intervallo
	if(nNewItem >= 0 && nNewItem < CListCtrl::GetItemCount())
	{
		// riga da spostare
		CTRL_ROW* ra = GetRowByItem(nItem);
		
		// riga precedente/successiva
		CTRL_ROW* ra_prevnext = GetRowByItem(nNewItem);
		
		if(ra && ra_prevnext)
		{
			int nSubItem;
			LV_ITEM lvitem;
			char szItem[MAX_ITEM_SIZE+1];

			// elimina l'elemento dal controllo (chiama la classe base perche' non deve aggiornare il contatore interno)
			CListCtrl::DeleteItem(nItem);

			// testo dell'elemento
			strcpyn(szItem,ra->item,sizeof(szItem));
			
			// rinumera gli indici dei due elementi coinvolti nello spostamento
			ra->index = nNewItem;
			ra_prevnext->index = nItem;

			// colonne dell'elemento
			for(nSubItem = 0,nCurrentItem = 0; nSubItem < m_nTotColumns && nCurrentItem!=LB_ERR; nSubItem++)
			{
				// l'elemento (riga) viene passata in un unico buffer, diviso in colonne dal carattere ';'
				lvitem.pszText = (LPSTR)GetItemToken(nSubItem,szItem);
				
				lvitem.iSubItem = nSubItem;

				if(nSubItem==0)
				{
					lvitem.mask   = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
					lvitem.iItem  = nNewItem;
					lvitem.iImage = ra->ico;
					lvitem.lParam = (LPARAM)ra;
					lvitem.pszText = LPSTR_TEXTCALLBACK;

					if((nCurrentItem  = CListCtrl::InsertItem(&lvitem)) < 0)
						nCurrentItem = LB_ERR;
				}
				else
				{
					lvitem.mask  = LVIF_TEXT;
					lvitem.iItem = nCurrentItem;
					
					CListCtrl::SetItem(&lvitem);
				}
			}

			// imposta la riga come selezionata
			nCurrentItem = SelectItem(nNewItem);
		}
	}

	return(nCurrentItem);
}

/*
	AddIcon()

	Aggiunge l'icona alla lista delle immagini per la lista.

	int nIconID		id della risorsa relativo all'icona
*/
int CListCtrlEx::AddIcon(int nIconID)
{
	int nCurrentIcon = LB_ERR;

	if(!m_pWinApp)
		return(nCurrentIcon);

	if((nCurrentIcon = m_ImageListSmall.Add(m_pWinApp->LoadIcon(nIconID))) < 0)
		return(nCurrentIcon);

	CListCtrl::SetImageList(&m_ImageListSmall,LVSIL_SMALL);

	return(nCurrentIcon);
}

/*
	AddIcon()

	Aggiunge l'icona alla lista delle immagini per la lista.

	HICON hIcon	handle relativo all'icona
*/
int CListCtrlEx::AddIcon(HICON hIcon)
{
	int nCurrentIcon = LB_ERR;

	if((nCurrentIcon = m_ImageListSmall.Add(hIcon)) < 0)
		return(nCurrentIcon);

	CListCtrl::SetImageList(&m_ImageListSmall,LVSIL_SMALL);

	return(nCurrentIcon);
}

/*
	RemoveIcon()
*/
void CListCtrlEx::RemoveIcon(int nIconID)
{
	m_ImageListSmall.Remove(nIconID);
}

/*
	RemoveAllIcons()
*/
void CListCtrlEx::RemoveAllIcons(void)
{
	int i,nTot;
	for(i = 0,nTot = m_ImageListSmall.GetImageCount(); i < nTot; i++)
		m_ImageListSmall.Remove(0);
}

/*
	SelectItem()

	Imposta l'elemento come selezionato.
	Se la selezione riesce, imposta l'indice interno relativo alla riga corrente.

	int nItem		indice (base 0) dell'elemento
*/
int CListCtrlEx::SelectItem(int nItem)
{
	// riga corrente
	int iCurrentItem = GetCurrentItem();

	// controlla l'indice
	if(nItem >=0 && nItem < CListCtrl::GetItemCount())
	{
		/* M$VC 6.0
		POSITION position;
		position = GetFirstSelectedItemPosition();
		while(position)
			iCurrentItem = GetNextSelectedItem(position);
		*/
		LV_ITEM lvitem;

		// ricava la riga selezionata
		if((iCurrentItem = CListCtrl::GetNextItem(0,LVNI_ALL|LVNI_FOCUSED|LVNI_SELECTED)) < 0)
			iCurrentItem = 0;

		if(iCurrentItem >= 0)
		{
			// elimina la selezione per la riga corrente
			memset(&lvitem,'\0',sizeof(LV_ITEM));
			lvitem.mask = LVIF_STATE;
			lvitem.iItem = iCurrentItem;
			lvitem.iSubItem = 0;
			lvitem.state = 0;
			lvitem.stateMask = LVIS_FOCUSED | LVIS_SELECTED ;
			if(CListCtrl::SetItemState(iCurrentItem,&lvitem))
			{
				// imposta come selezionata la riga specificata
				memset(&lvitem,'\0',sizeof(LV_ITEM));
				lvitem.mask = LVIF_STATE;
				lvitem.iItem = nItem;
				lvitem.iSubItem = 0;
				lvitem.state = LVIS_FOCUSED | LVIS_SELECTED;
				lvitem.stateMask = LVIS_FOCUSED | LVIS_SELECTED;
				if(CListCtrl::SetItemState(nItem,&lvitem))
				{
					// si assicura che la riga sia visibile ed aggiorna l'indice interno
					SetRedraw(FALSE);
					CListCtrl::EnsureVisible(nItem,FALSE);
					SetRedraw(TRUE);

					SetCurrentItem(nItem);
				}
			}
		}
	}

	return(iCurrentItem);
}

/*
	SetCurrentItem()

	Aggiorna l'indice interno relativo alla riga corrente.
	Notare che la riga corrente puo' non corrispondere con quella selezionata (focused).

	int nItem		indice (base 0) dell'elemento
*/
int CListCtrlEx::SetCurrentItem(int nItem)
{
	if(CListCtrl::GetItemCount() <= 0)
		return(LB_ERR);

	if(nItem < 0 || nItem > CListCtrl::GetItemCount())
		nItem = 0;

	m_nCurrentItem = nItem;

	return(m_nCurrentItem);
}

/*
	GetCurrentItem()

	Restituisce l'indice interno relativo alla riga corrente.
	Notare che la riga corrente puo' non corrispondere con quella selezionata (focused).
*/
int CListCtrlEx::GetCurrentItem(void)
{
	if(CListCtrl::GetItemCount() <= 0)
		return(LB_ERR);

	if(m_nCurrentItem==LB_ERR)
		m_nCurrentItem = 0;

	return(m_nCurrentItem);
}

/*
	SetItemSelection()

	Imposta lo status (interno) dell'elemento (selezionato o meno), restituendo l'anteriore.

	int nItem			indice (base 0) dell'elemento
	BOOL bSelected		flag per selezione elemento
*/
BOOL CListCtrlEx::SetItemSelection(int nItem,BOOL bSelected)
{
	BOOL bIsSelected = FALSE;
	CTRL_ROW* ra = GetRowByItem(nItem);
	
	if(ra)
	{
		bIsSelected = ra->selected;
		ra->selected = bSelected;
	}

	return(bIsSelected);
}

/*
	GetItemSelection()

	Restituisce lo status (interno) dell'elemento (selezionata o meno).

	int nItem			indice (base 0) dell'elemento
*/
BOOL CListCtrlEx::GetItemSelection(int nItem)
{
	BOOL bIsSelected = FALSE;
	CTRL_ROW* ra = GetRowByItem(nItem);
	
	if(ra)
		bIsSelected = ra->selected;

	return(bIsSelected);
}

/*
	SetItemwParam()

	Imposta i dati extra.

	int nItem			indice (base 0) dell'elemento
*/
void CListCtrlEx::SetItemwParam(int nItem,WPARAM wParam)
{
	CTRL_ROW* ra = GetRowByItem(nItem);
	
	if(ra)
		ra->wParam = wParam;
}

/*
	GetItemwParam()

	Restituisce i dati extra.

	int nItem			indice (base 0) dell'elemento
*/
WPARAM CListCtrlEx::GetItemwParam(int nItem)
{
	WPARAM wParam = NULL;
	CTRL_ROW* ra = GetRowByItem(nItem);
	
	if(ra)
		wParam = ra->wParam;

	return(wParam);
}

/*
	SetItemlParam()

	Imposta i dati extra.

	int nItem			indice (base 0) dell'elemento
*/
void CListCtrlEx::SetItemlParam(int nItem,LPARAM lParam)
{
	CTRL_ROW* ra = GetRowByItem(nItem);
	
	if(ra)
		ra->lParam = lParam;
}

/*
	GetItemlParam()

	Restituisce i dati extra.

	int nItem			indice (base 0) dell'elemento
*/
LPARAM CListCtrlEx::GetItemlParam(int nItem)
{
	LPARAM lParam = (LPARAM)-1L;
	CTRL_ROW* ra = GetRowByItem(nItem);
	
	if(ra)
		lParam = ra->lParam;

	return(lParam);
}

/*
	ResetItemsSelection()

	Azzera lo status (interno) degli elementi (imposta su non selezionati).
*/
void CListCtrlEx::ResetItemsSelection(void)
{
	CTRL_ROW* ra;

	for(int i = 0; i < CListCtrl::GetItemCount(); i++)
		if((ra = GetRowByItem(i))!=(CTRL_ROW*)NULL)
			ra->selected = FALSE;
}

/*
	CompareItems()

	Confronta i due elementi per l'ordinamento del controllo.
*/
int CALLBACK CListCtrlEx::CompareItems(LPARAM lParam1,LPARAM lParam2,LPARAM lParam)
{
	CListCtrlEx* pThis = (CListCtrlEx*)lParam;
	return(pThis ? pThis->CompareItems(lParam1,lParam2) : 0);
}
int CListCtrlEx::CompareItems(LPARAM lParam1,LPARAM lParam2)
{
	int nCmp = 0;
	static char s1[MAX_ITEM_SIZE + 1],s2[MAX_ITEM_SIZE + 1];
	static int n = 0;

	CTRL_ROW* ra1 = (CTRL_ROW*)lParam1;
	CTRL_ROW* ra2 = (CTRL_ROW*)lParam2;
	strcpyn(s1,GetItemToken(m_nSelectedCol,(LPCSTR)ra1->item),sizeof(s1));
	strcpyn(s2,GetItemToken(m_nSelectedCol,(LPCSTR)ra2->item),sizeof(s2));

	if(m_nOrder==SORT_WPARAM && m_pfnSortwParamCallback)
	{
		nCmp = m_pfnSortwParamCallback(ra1->wParam,ra2->wParam);
	}
	else if(m_nOrder==SORT_LPARAM && m_pfnSortlParamCallback)
	{
		nCmp = m_pfnSortlParamCallback(ra1->lParam,ra2->lParam);
	}
	else // SORT_AUTO, SORT_ASC, SORT_DESC
	{
		if(m_bAscending)
		{
			if(m_cColumnType=='C')
				nCmp = strcmp(s1,s2);
			else if(m_cColumnType=='N')
			{
				unsigned long n1 = strtoul(s1,NULL,0);
				unsigned long n2 = strtoul(s2,NULL,0);
				
				if(n1 < n2)
					nCmp = -1;
				else if(n1 > n2)
					nCmp = 1;
			}
			else if(m_cColumnType=='S')
			{
				double d1 = atof(s1);
				double d2 = atof(s2);
				
				if(stristr(s1," KB"))
					d1 *= 1024.0f;
				else if(stristr(s1," MB"))
					d1 *= 1048576.0f;
				if(stristr(s2," KB"))
					d2 *= 1024.0f;
				else if(stristr(s2," MB"))
					d2 *= 1048576.0f;
				
				if(d1 < d2)
					nCmp = -1;
				else if(d1 > d2)
					nCmp = 1;
			}
		}
		else
		{
			if(m_cColumnType=='C')
				nCmp = strcmp(s2,s1);
			else if(m_cColumnType=='N')
			{
				unsigned long n1 = strtoul(s1,NULL,0);
				unsigned long n2 = strtoul(s2,NULL,0);
				
				if(n2 < n1)
					nCmp = -1;
				else if(n2 > n1)
					nCmp = 1;
			}
			else if(m_cColumnType=='S')
			{
				double d1 = atof(s1);
				double d2 = atof(s2);
				
				if(stristr(s1," KB"))
					d1 *= 1024.0f;
				else if(stristr(s1," MB"))
					d1 *= 1048576.0f;
				if(stristr(s2," KB"))
					d2 *= 1024.0f;
				else if(stristr(s2," MB"))
					d2 *= 1048576.0f;
				
				if(d2 < d1)
					nCmp = -1;
				else if(d2 > d1)
					nCmp = 1;
			}
		}
	}

	if(++n > 5)
	{
		n = 0;
		::PeekAndPump();
	}

	return(nCmp);
}

/*
	GetItemToken()

	Estrae dalla riga l'elemento relativo alla colonna.
	Gli elementi, all'interno della riga, devono essere separati dal carattere ';'.

	int nItem			indice (base 0) dell'elemento
	LPCSTR lpcszRow	riga contenente gli elementi separati dal carattere ';'
*/
LPCSTR CListCtrlEx::GetItemToken(int nItem,LPCSTR lpcszRow)
{
	int i;
	LPSTR lpToken;
	static char szItem[MAX_ITEM_SIZE + 1];

	strcpy(szItem,"[null]");

	if(lpcszRow && *lpcszRow)
	{
		strcpyn(szItem,lpcszRow,sizeof(szItem));

		for(i = 0,lpToken = strtok(szItem,";"); lpToken!=NULL; i++)
		{
			if(i==nItem)
			{
				strcpyn(szItem,lpToken,sizeof(szItem));
				break;
			}

			lpToken = strtok(NULL,";");
		}
	}

	return(szItem);
}

/*
	PopUpMenu()

	Visualizza il menu popup sul controllo.
	Il menu visualizzato e' il primo sottomenu (base 0) del menu relativo all'id nel caso in cui venga
	fatto click su un elemento o il secondo se viene fatto click in una zona del controllo senza elementi.

	const CPoint& point		coordinate del click
*/
void CListCtrlEx::PopUpMenu(const CPoint& point)
{
	int i;
	CRect rect;
	CMenu menu;
	CMenu* pSubMenu = NULL;
	CTRL_ROW* ra;

	// coordinate dell'area client (CWnd, base 0,0)
	CWnd::GetClientRect(&rect);
	
	// trasforma le coordinate in assolute (schermo)
	// da aggiungere alle coordinate del click del mouse per ottenere le coordinate a cui visualizzare il menu a comparsa
	CWnd::ClientToScreen(&rect);	
	
	// click su un elemento del controllo
	if((i = CListCtrl::HitTest(point)) >= 0)
	{
		// carica il menu relativo all'id specificato dall'elemento su cui e' stato fatto click
		// (primo sottomenu, base 0)
		if((ra = GetRowByItem(i))!=(CTRL_ROW*)NULL)
			if(ra->menu > 0)
				if(menu.LoadMenu(ra->menu))
					pSubMenu = menu.GetSubMenu(0);
	}
	// click fuori dall'elemento
	else
	{
		// click su una zona del controllo contenente almeno un elemento
		if((ra = GetRowByItem(0))!=(CTRL_ROW*)NULL)
		{
			// carica il menu relativo all'id specificato dal primo elemento della lista
			// (secondo sottomenu, base 0)
			if(ra->menu > 0)
				if(menu.LoadMenu(ra->menu))
					pSubMenu = menu.GetSubMenu(1);
		}
		// click su una zona del controllo senza nessun elemento
		else
		{
			// carica il menu di default
			// (primo sottomenu, base 0)
			if(m_NullRow.menu > 0)
				if(menu.LoadMenu(m_NullRow.menu))
					pSubMenu = menu.GetSubMenu(0);
		}
	}

	// visualizza il menu popup
	// TrackPopupMenu() does not return until the user selects a menu item or cancels the menu, so EnableMenuItem() is called after the menu has gone
	if(pSubMenu && m_pWnd)
	{
		if(m_nMenuMessage!=(UINT)-1L)
			::SendMessage(m_pWnd->m_hWnd,m_nMenuMessage,(WPARAM)m_nCurrentItem,(LPARAM)pSubMenu);

		CPoint ptCursor;
		GetCursorPos(&ptCursor);
		::TrackPopupMenu(pSubMenu->m_hMenu,0,ptCursor.x,ptCursor.y,0,m_pWnd->m_hWnd,0);
		::PostMessage(m_pWnd->m_hWnd,WM_NULL,0,0);
	}
}

/*
	Sort()
*/
void	CListCtrlEx::Sort(int nOrder/* = SORT_AUTO*/,int nCol/* = 0*/)
{
	LRESULT lResult = 0L;
	NM_LISTVIEW nm_listview;
	
	memset(&nm_listview,'\0',sizeof(NM_LISTVIEW));
	nm_listview.iSubItem = nCol;
	m_nOrder = nOrder;

	// per far si che non venga inviato il messaggio quando la Sort() viene chiamata direttamente
	m_bOnColumnClicked = FALSE;
	OnColumnClick((NMHDR*)&nm_listview,&lResult);
	m_bOnColumnClicked = TRUE;
}

/*
	OnGetDispInfo()

	Callback per impostare il contenuto del campo della lista.
*/
void CListCtrlEx::OnGetDispInfo(NMHDR* pnmh,LRESULT* /*pResult*/)
{
	LV_DISPINFO* plvdispinfo = (LV_DISPINFO*)pnmh;

	// verifica che la richiesta sia relativa al testo del campo
	if(plvdispinfo->item.mask & LVIF_TEXT)
	{
		// ricava il puntatore ai dati (impostato all'inserimento dell'elemento nella lista)
		// i dati contengono il record della tabella, con i campi separati da ITEM_DELIMITER
		CTRL_ROW* pItem = (CTRL_ROW*)plvdispinfo->item.lParam;

		if(pItem)
		{
			char* p = (char*)pItem->item;
			if(p)
			{
				// in <plvdispinfo->item.iSubItem> viene specificato l'indice (a base 0) del campo
				// di cui viene richiesto il contenuto, scorre quindi il numero relativo di
				// ITEM_DELIMITER per posizionarsi sul campo richiesto all'interno del record
				int i;
				for(i=0; i < plvdispinfo->item.iSubItem; i++)
				{
					while(*p && *p!=ITEM_DELIMITER)
						p++;
					if(*p==ITEM_DELIMITER)
						p++;
				}

				// copia il contenuto del campo
				for(i=0; *p && *p!=ITEM_DELIMITER && i < plvdispinfo->item.cchTextMax-1; i++,p++)
					plvdispinfo->item.pszText[i] = *p;
				plvdispinfo->item.pszText[i] = '\0';
			}
		}
	}
}

/*
	OnColumnClick()

	Gestore per il click sull'header della colonna.
	Ordina le righe presenti nella colonna.
*/
void CListCtrlEx::OnColumnClick(NMHDR* pNMHDR,LRESULT* pResult) 
{
	// verifica che il controllo contenga almeno un elemento
	if(CListCtrl::GetItemCount() > 0)
	{
		NM_LISTVIEW* pNM_LISTVIEW = (NM_LISTVIEW*)pNMHDR;
		int nCol = pNM_LISTVIEW->iSubItem;
		CTRL_COL* ca = GetColByItem(nCol);

		if(ca)
		{
			// imposta l'ordinamento corrente (ascendente/discendente) in base alla modalita'
			switch(m_nOrder)
			{
				case SORT_AUTO:
					ca->order = (ca->order==FALSE ? TRUE : FALSE);
					m_bAscending = ca->order;
					break;
				case SORT_ASC:
					m_bAscending = TRUE;
					break;
				case SORT_DESC:
					m_bAscending = FALSE;
					break;
				case SORT_WPARAM:
				case SORT_LPARAM:
					break;
			}
			
			// distingue tra dati numerici e carattere
			m_cColumnType = ca->type;
			if(m_cColumnType!='C' && m_cColumnType!='N' && m_cColumnType!='S')
				m_cColumnType = 'C';
			
			// ordina le righe della colonna
			m_nSelectedCol = nCol;
			CListCtrl::SortItems((PFNLVCOMPARE)CompareItems,(DWORD)this);

			// rinumera l'indice (interno) delle rige secondo l'ordinamento corrente
			
			// notare che l'ordinamento avviene considerando la prima colonna (base 0) della lista
			// motivo per cui tale colonna non puo' contenere elementi duplicati: in tal caso il
			// confronto (vedi la strcmp() di cui sotto) tra i due elementi selezionerebbe sempre
			// il primo e tanti saluti alla rinumerazione dell'indice interno che se ne andrebbe a
			// puttane
			// il confronto non puo' essere fatto con memcmp() su n caratteri perche' fallirebbe
			// nel caso in cui ci siano elementi che iniziano con gli stessi caratteri (ftp e ftp-wpd)
			// per cui deve estrarre il testo relativo dal buffer interno (testo;testo;testo;etc.) e
			// confrontarlo con quello della riga con strcmp()
			ITERATOR iter;
			CTRL_ROW* ra = NULL;
			LPSTR p;
			char szItem[MAX_ITEM_SIZE+1];
			char buffer[MAX_ITEM_SIZE+1];
			int tot = CListCtrl::GetItemCount();
			register int i,n;

			// per ogni riga
			for(i = 0,n = 0; i < tot; i++)
			{
				// ricava il testo della riga a colonna 0
				if(CListCtrl::GetItemText(i,0,buffer,sizeof(buffer)-1) > 0)
					// cerca l'entrata corrispondente (testo;testo;testo;etc.) nella lista interna
					if((iter = m_RowList.First())!=(ITERATOR)NULL)
					{
						do
						{
							// puntatore alla riga
							ra = (CTRL_ROW*)iter->data;
						
							if(ra)
							{
								// estrae il testo corrispondente alla colonna 0
								if((p = strchr(ra->item,';'))!=NULL)
								{
									int n = p - ra->item;
									memcpy(szItem,ra->item,n);
									szItem[n] = '\0';
									
									if(strcmp(buffer,szItem)==0)
									{
										ra->index = i;
										break;
									}
								}
							}
						
							iter = m_RowList.Next(iter);
						
						} while(iter!=(ITERATOR)NULL);
					}
					
				if(++n > 5)
				{
					n = 0;
					::PeekAndPump();
				}
			}

			if(m_bOnColumnClicked)
				if(m_pWnd!=NULL && m_nColumnClickMessage!=(UINT)-1L)
					::SendMessage(m_pWnd->m_hWnd,m_nColumnClickMessage,(WPARAM)nCol,(LPARAM)m_nOrder);
			
			m_nOrder = SORT_AUTO;
		}
	}

	*pResult = 0;
}

/*
	OnCustomDraw()
*/
void CListCtrlEx::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LPNMLVCUSTOMDRAW lplvcd = (LPNMLVCUSTOMDRAW)pNMHDR;

	switch(lplvcd->nmcd.dwDrawStage)
	{
		// request prepaint notifications for each item
		case CDDS_PREPAINT:
			*pResult = CDRF_NOTIFYITEMDRAW;
			break;

		// requested notification
		case CDDS_ITEMPREPAINT:
		{
			BOOL bAlternate = TRUE;
			if(m_nForegroundColor==CTRLLISTEX_DEFAULT_FOREGROUND_COLOR && m_nBackgroundColor==CTRLLISTEX_DEFAULT_BACKGROUND_COLOR)
				bAlternate = FALSE;
			else
				bAlternate = lplvcd->nmcd.dwItemSpec % 2;
			//if(lplvcd->nmcd.dwItemSpec % 2)
			if(bAlternate)
			{
				lplvcd->clrText = m_nForegroundColor;
				lplvcd->clrTextBk = m_nBackgroundColor;
			}
			*pResult = CDRF_DODEFAULT;
			break;
		}
	}
}

/*
	OnLButtonDown()

	Gestore per il click con il bottone sinistro del mouse sull'elemento del controllo.
	Imposta la riga come selezionata ed invia il messaggio alla finestra principale (in wParam l'indice
	dell'elemento selezionato ed in lParam le coordinate del click).
	Per catturare il rilascio del bottone sinistro del mouse con OnLButtonUp() anche quando il cursore
	del mouse e' stato spostato fuori dall'area client, utilizzare la SetCapture().
*/
void CListCtrlEx::OnLButtonDown(UINT nFlags,CPoint point)
{
	// salva le coordinate del click
	m_Point.x = point.x;
	m_Point.y = point.y;

	m_MouseClick.point.x = point.x;
	m_MouseClick.point.y = point.y;
	m_MouseClick.flags = nFlags;

	// chiama il gestore originale
	CListCtrl::OnLButtonDown(nFlags,point);

	// per continuare a ricevere i messaggi quando il mouse si sposta fuori dell'area client
	//::SetCapture(this->m_hWnd);
	
	// indice dell'elemento cliccato
	m_nCurrentItem = CListCtrl::HitTest(point);

	// imposta il flag relativo alla selezione
	// se viene cambiato l'ordinamento del controllo l'elemento relativo all'indice non corrisponde a quanto
	// presente nella lista del chiamante, per cui deve cercare l'elemento relativo prima di impostare il
	// flag per la selezione
	// effettua tale ricerca con il primo subitem (indice 0), per cui il valore presente in tale posizione
	// deve essere unico
	int nTotItems = CListCtrl::GetItemCount();

	if(m_nCurrentItem >= 0 && m_nCurrentItem < nTotItems)
	{
		CTRL_ROW* ra = NULL;
		ITERATOR iter;
		register int i;
		char buffer[MAX_ITEM_SIZE+1];

		// azzera le selezioni correnti
		if((iter = m_RowList.First())!=(ITERATOR)NULL)
		{
			do
			{
				ra = (CTRL_ROW*)iter->data;
				if(ra)
					ra->selected = FALSE;
				iter = m_RowList.Next(iter);
			} while(iter!=(ITERATOR)NULL);
		}
		
		// per ogni elemento del controllo verifica se e' stato selezionato, ricava il valore relativo al
		// subitem con indice 0 e lo confronta con l'elemento corrente: se coincidono imposta il flag per
		// la selezione
		// deve scorrere il controllo per gestire le selezioni multiple (click sul primo, shift+click
		// sull'ultimo)
		for(i = 0; i < nTotItems/*CListCtrl::GetItemCount()*/; i++)
		{
			// il cazzo di elemento e' stato cliccato ?
			// (click sull'elemento o elemento nel mezzo di una selezione click su i, shift+click su i+n)
			if(CListCtrl::GetItemState(i,LVIS_SELECTED) & LVIS_SELECTED)
			{
				memset(buffer,'\0',sizeof(buffer));

				LV_ITEM lvitem;
				lvitem.mask = LVIF_TEXT;
				lvitem.iItem = i;
				lvitem.iSubItem = 0;
				lvitem.pszText = buffer;
				lvitem.cchTextMax = sizeof(buffer)-1;
				CListCtrl::GetItem(&lvitem);

				if((ra = GetRowByItem(i))!=(CTRL_ROW*)NULL)
				{
					if(memcmp(ra->item,buffer,strlen(buffer))==0)
					{
						ra->selected = TRUE;
						break;
					}
				}

				if(!m_bMultipleSelection)
					break;
			}
		}
	}

	// salva l'indice (base 0) dell'elemento corrente (selezionato), controllando l'intervallo
	if(m_nCurrentItem < 0)
		m_nCurrentItem = LB_ERR;
	else if(m_nCurrentItem >= nTotItems)
		m_nCurrentItem = nTotItems - 1;

	// se e' stato specificato un gestore, invia il messaggio relativo
	// in wParam l'indice dell'elemento ed in lParam le coordinate del click
	if(m_pWnd!=NULL && m_nLButtonDownMessage!=(UINT)-1L)
		::SendMessage(m_pWnd->m_hWnd,m_nLButtonDownMessage,(WPARAM)m_nCurrentItem,(LPARAM)&m_MouseClick);
}

/*
	OnLButtonUp()

	Gestore per il rilascio del bottone sinistro del mouse.
	Invia il messaggio alla finestra principale (in wParam l'indice dell'elemento su cui e' stato rilasciato
	il mouse ed in lParam le coordinate del rilascio).
	Vedi le note in OnLButtonDown() a proposito di SetCapture().
*/
void CListCtrlEx::OnLButtonUp(UINT nFlags,CPoint point)
{
	// salva le coordinate del click
	m_Point.x = point.x;
	m_Point.y = point.y;

	// chiama il gestore originale
	CListCtrl::OnLButtonUp(nFlags,point);

	// per terminare di ricevere i messaggi con il mouse fuori dell'area client
	//if(::GetCapture()==this->m_hWnd)
	//	::ReleaseCapture();

	// indice dell'elemento su cui e' stato effettuato il rilascio
	int m_nCurrentItem = CListCtrl::HitTest(point);

	// se e' stato specificato un gestore, invia il messaggio relativo
	// in wParam l'indice dell'elemento ed in lParam le coordinate del rilascio
	if(m_pWnd!=NULL && m_nLButtonUpMessage!=(UINT)-1L)
		::SendMessage(m_pWnd->m_hWnd,m_nLButtonUpMessage,(WPARAM)m_nCurrentItem,(LPARAM)&m_Point);
}

/*
	OnRButtonDown()

	Gestore per il click con il bottone destro del mouse sull'elemento del controllo.
	Visualizza il menu popup ed invia il messaggio alla finestra principale (in wParam
	l'indice dell'elemento selezionato ed in lParam le coordinate del click).
*/
void CListCtrlEx::OnRButtonDown(UINT nFlags,CPoint point)
{
	// salva le coordinate del click
	m_Point.x = point.x;
	m_Point.y = point.y;

	// chiama il gestore originale
	CListCtrl::OnRButtonDown(nFlags,point);

	// indice dell'item cliccato
	m_nCurrentItem = CListCtrl::HitTest(point);

	// a seconda del flag imposta l'item corrente come selezionato
	// (per default solo il click con il sinistro imposta l'elemento come selezionato)
	if(m_bRightClickSelects)
	{
		// resetta le selezioni correnti dato la selezione multiple puo' effettuarsi solo con CTRL+click destro
		ResetItemsSelection();
		
		// seleziona l'item
		SetItemSelection(m_nCurrentItem,TRUE);
	}

	// visualizza il menu a comparsa
	PopUpMenu(point);

	// se e' stato specificato un gestore, invia il messaggio relativo
	// in wParam l'indice dell'elemento ed in lParam le coordinate del click
	if(m_pWnd!=NULL && m_nRButtonDownMessage!=(UINT)-1L)
		::PostMessage(m_pWnd->m_hWnd,m_nRButtonDownMessage,(WPARAM)m_nCurrentItem,(LPARAM)&m_Point);
}

/*
	OnRButtonUp()

	Gestore per il rilascio del bottone destro del mouse.
	Invia il messaggio alla finestra principale (in wParam l'indice dell'elemento su cui e' stato rilasciato
	il mouse ed in lParam le coordinate del rilascio).
*/
void CListCtrlEx::OnRButtonUp(UINT nFlags,CPoint point)
{
	// salva le coordinate del click
	m_Point.x = point.x;
	m_Point.y = point.y;

	// chiama il gestore originale
	CListCtrl::OnRButtonUp(nFlags,point);

	// indice dell'elemento su cui e' stato effettuato il rilascio
	int m_nCurrentItem = CListCtrl::HitTest(point);

	// se e' stato specificato un gestore, invia il messaggio relativo
	// in wParam l'indice dell'elemento ed in lParam le coordinate del rilascio
	if(m_pWnd!=NULL && m_nRButtonUpMessage!=(UINT)-1L)
		::SendMessage(m_pWnd->m_hWnd,m_nRButtonUpMessage,(WPARAM)m_nCurrentItem,(LPARAM)&m_Point);
}

/*
	OnLButtonDblClk()

	Gestore per il doppio click con il bottone sinistro del mouse sull'elemento del controllo.
	Invia il messaggio alla finestra principale (in wParam l'indice dell'elemento selezionato ed in lParam
	le coordinate del click).
*/
void CListCtrlEx::OnLButtonDblClk(UINT nFlags,CPoint point)
{
	// salva le coordinate del click
	m_Point.x = point.x;
	m_Point.y = point.y;

	// chiama il gestore originale
	CListCtrl::OnLButtonDblClk(nFlags,point);

	// indice dell'elemento cliccato
	m_nCurrentItem = CListCtrl::HitTest(point);

	// se e' stato specificato un gestore, invia il messaggio relativo
	// in wParam l'indice dell'elemento ed in lParam le coordinate del click
	if(m_pWnd!=NULL && m_nLButtonDblClkMessage!=(UINT)-1L)
		::PostMessage(m_pWnd->m_hWnd,m_nLButtonDblClkMessage,(WPARAM)m_nCurrentItem,(LPARAM)&m_Point);
}

/*
	OnRButtonDblClk()

	Gestore per il doppio click con il bottone destro del mouse sull'elemento del controlo.
	Invia il messaggio alla finestra principale (in wParam l'indice dell'elemento selezionato ed in lParam
	le coordinate del click).
*/
void CListCtrlEx::OnRButtonDblClk(UINT nFlags,CPoint point)
{
	// salva le coordinate del click
	m_Point.x = point.x;
	m_Point.y = point.y;

	// chiama il gestore originale
	CListCtrl::OnRButtonDblClk(nFlags,point);

	// indice dell'elemento cliccato
	m_nCurrentItem = CListCtrl::HitTest(point);

	// se e' stato specificato un gestore, invia il messaggio relativo
	// in wParam l'indice dell'elemento ed in lParam le coordinate del click
	if(m_pWnd!=NULL && m_nRButtonDblClkMessage!=(UINT)-1L)
		::PostMessage(m_pWnd->m_hWnd,m_nRButtonDblClkMessage,(WPARAM)m_nCurrentItem,(LPARAM)&m_Point);
}
