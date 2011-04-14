/*
	CListViewEx.cpp
	Classe derivata (CListView) per la lista (MFC).
	
	Imposta la lista in modalita' report, richiesta da alcune parti del codice. Per default non imposta
	il flag LVS_NOSORTHEADER, per cui le colonne possono essere cliccate per l'ordinamento.
	
	Incorpora il codice per l'editing del campo a riga,colonna. In tal caso la derivata deve aggiungere
	il flag LVS_EDITLABELS allo stile nella redifinizione della PreCreateWindow().
	Nel file delle risorse bisogna definire l'id relativo al campo per l'editing (IDC_IN_PLACE_EDIT), in
	caso contrario non include il codice relativo in compilazione.
	Se viene usato l'editing del campo, il gestore di default per il messaggio LVN_ENDLABELEDIT aggiorna
	il campo della lista con SetItemText(). Se la derivata gestisce gli elementi della lista in proprio
	(LVN_GETDISPINFO), l'aggiornamento con SetItemText() obbliga la lista a creare un nuovo elemento per
	il campo, mentre l'elemento gestito in proprio non viene modificato. In tal caso ridefinire, nella
	derivata, il gestore per LVN_ENDLABELEDIT e aggiornare l'elemento in proprio.

	Alcune parti del codice, incluso l'editing del campo a riga/colonna, sono state riprese e modificate
	da quanto presente su www.codeguru.com nella sezione relativa al controllo CListView.

	Luca Piergentili, 12/12/02
	lpiergentili@yahoo.com
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include "window.h"
#include <afxcview.h>
#include "CListViewEx.h"

#include "resource.h"
#ifdef IDC_IN_PLACE_EDIT
  #include "CInPlaceEdit.h"
#endif

#include "traceexpr.h"
//#define _TRACE_FLAG	_TRFLAG_TRACEOUTPUT
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

IMPLEMENT_DYNCREATE(CListViewEx,CListView)

BEGIN_MESSAGE_MAP(CListViewEx,CListView)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
#ifdef IDC_IN_PLACE_EDIT
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT,OnEndLabelEdit)
#endif
END_MESSAGE_MAP()

/*
	CListViewEx()
*/
CListViewEx::CListViewEx()
{
	m_nCurrentItem = 0;
	m_nRedrawCount = 0;
}

/*
	PreTranslateMessage()
*/
VIRTUAL BOOL CListViewEx::PreTranslateMessage(MSG* pMsg) 
{
	// per far passare i messaggi del mouse al tooltip
	m_ToolTipCtrl.RelayEvent(pMsg);	

	// classe base
	return(CListView::PreTranslateMessage(pMsg));
}

/*
	PreCreateWindow()
*/
VIRTUAL BOOL CListViewEx::PreCreateWindow(CREATESTRUCT& cs)
{
	// classe base
	if(!CListView::PreCreateWindow(cs))
		return(FALSE);

	// modalita' report, visualizza sempre e comunque la selezione corrente, selezione di una sola riga per volta
	cs.style &= ~LVS_TYPEMASK;
	cs.style |= LVS_REPORT|LVS_SHOWSELALWAYS|LVS_SINGLESEL;

	return(TRUE);
}

/*
	OnCreate()
*/
VIRTUAL int CListViewEx::OnCreate(LPCREATESTRUCT lpcs)
{
	// classe base
	if(CListView::OnCreate(lpcs)==-1)
		return(-1);

	// tooltip
	m_ToolTipCtrl.Create(this);
	m_ToolTipCtrl.SetMaxTipWidth(MAX_TOOLTIP_TEXT); 

	// stili estesi: seleziona l'intera riga
	DWORD dwStyle = ListView_GetExtendedListViewStyle(this->m_hWnd);
	if(!(dwStyle & LVS_EX_FULLROWSELECT))
	{
		dwStyle |= LVS_EX_FULLROWSELECT;
		ListView_SetExtendedListViewStyle(this->m_hWnd,dwStyle);
	}

	return(0);
}

/*
	OnClose()
*/
VIRTUAL void CListViewEx::OnClose(void)
{
	// classe base
    CListView::OnClose();
}

/*
	OnDestroy()
*/
VIRTUAL void CListViewEx::OnDestroy(void)
{
	// classe base
    CListView::OnDestroy();
}

/*
	OnActivateView()
*/
VIRTUAL void CListViewEx::OnActivateView(BOOL bActivate,CView* pActivateView,CView* pDeactiveView)
{
	// classe base
	CListView::OnActivateView(bActivate,pActivateView,pDeactiveView);
}

/*
	OnInitialUpdate()
*/
VIRTUAL void CListViewEx::OnInitialUpdate(void)
{
	// classe base
	CListView::OnInitialUpdate();
}
/*
	OnUpdate()
*/
VIRTUAL void CListViewEx::OnUpdate(CView* pView,LPARAM lHint,CObject* pHint)
{
	// classe base
	CListView::OnUpdate(pView,lHint,pHint);
}

/*
	OnNotify()
*/
VIRTUAL BOOL CListViewEx::OnNotify(WPARAM wParam,LPARAM lParam,LRESULT* pResult) 
{
	HD_NOTIFY	*pHDN = (HD_NOTIFY*)lParam;

	// se viene cambiata la dimensione della colonna, deve ricalcolare le aree per i tooltips
	if(pHDN->hdr.code==HDN_ENDTRACKA || pHDN->hdr.code==HDN_ENDTRACKW || pHDN->hdr.code==HDN_ITEMCHANGEDW || pHDN->hdr.code==HDN_ITEMCHANGEDA)
	{
		BOOL bFlag = CListView::OnNotify(wParam,lParam,pResult);
          RecalcHeaderTips();
          return(bFlag);
	}

	// classe base
	return(CListView::OnNotify(wParam,lParam,pResult));
}

/*
	OnChar()
*/
VIRTUAL void CListViewEx::OnChar(UINT nChar,UINT nRepCnt,UINT nFlags)
{
	// classe base
	CView::OnChar(nChar,nRepCnt,nFlags);
}

/*
	OnKeyDown()
*/
VIRTUAL void CListViewEx::OnKeyDown(UINT nChar,UINT nRepCnt,UINT nFlags)
{
	int nItem = GetCurrentItem();
	int nTotItems = GetListCtrl().GetItemCount();
	int nItemPerPage = GetListCtrl().GetCountPerPage();

	// gestisce solo i tasti per lo scorrimento
	switch(nChar)
	{
		case VK_HOME:
			nItem = 0;
			break;

		case VK_END:
			nItem = nTotItems - 1;
			if(nItem < 0)
				nItem = 0;
			break;

		case VK_UP:
			if(--nItem < 0)
				//nItem = 0;							// blocca a inizio fine
				nItem = nTotItems - 1;					// wrap
			break;

		case VK_DOWN:
			if(++nItem >= nTotItems)
				//nItem = nTotItems > 0 ? nTotItems - 1 : 0;	// blocca a inizio fine
				nItem = 0;							// wrap
			break;

		case VK_PRIOR:
			nItem -= nItemPerPage;
			if(nItem < 0)
				nItem = 0;
			break;

		case VK_NEXT:
			nItem += nItemPerPage;
			if(nItem >= nTotItems)
				nItem = nTotItems > 0 ? nTotItems-1 : 0;
			break;

		case VK_LEFT:
		case VK_RIGHT:
		{	
			CSize size(nChar==VK_RIGHT ? HSCROLL_PIXEL_TO_SCROLL : -HSCROLL_PIXEL_TO_SCROLL,0);
			GetListCtrl().Scroll(size);
			return;
		}

		default:
			nItem = -1;
			break;
	}

	// chiama la classe base solo se non viene selezionato l'elemento
	if(nItem >= 0)
		OnSelectItem(nItem);
	else
		CListView::OnKeyDown(nChar,nRepCnt,nFlags);
}

/*
	per ricavare la riga/colonna a cui e' stato effettuato il click:

	1)	HitTestEx(...);

	2)	LVHITTESTINFO lvhti;
		lvhti.pt = point;
		GetListCtrl().SubItemHitTest(&lvhti);
		if(lvhti.flags & LVHT_ONITEM)
		{
			CString str;
			str.Format("iItem = %d, iSubItem = %d", lvhti.iItem, lvhti.iSubItem);
			AfxMessageBox(str);
		}
*/

/*
	OnLButtonDown()
*/
VIRTUAL void CListViewEx::OnLButtonDown(UINT nFlags,CPoint point)
{
	// classe base
	CListView::OnLButtonDown(nFlags,point);
}

/*
	OnRButtonDown()
*/
VIRTUAL void CListViewEx::OnRButtonDown(UINT nFlags,CPoint point)
{
	// classe base
	CListView::OnRButtonDown(nFlags,point);
}

/*
	OnHScroll()
*/
VIRTUAL void CListViewEx::OnHScroll(UINT nSBCode,UINT nPos,CScrollBar* pScrollBar)
{
	// necessario per l'editing del campo
	if(GetFocus()!=this)
		SetFocus();

	// classe base
	CListView::OnHScroll(nSBCode,nPos,pScrollBar);
}

/*
	OnVScroll()
*/
VIRTUAL void CListViewEx::OnVScroll(UINT nSBCode,UINT nPos,CScrollBar* pScrollBar)
{
	// necessario per l'editing del campo
	if(GetFocus()!=this)
		SetFocus();
	
	// classe base
	CListView::OnVScroll(nSBCode,nPos,pScrollBar);
}

/*
	SelectItem()

	Imposta l'elemento come selezionato.
	Se la selezione riesce, imposta l'indice interno (a base 0) relativo alla riga corrente.
*/
int CListViewEx::SelectItem(int nItem)
{
	// controlla l'indice
	if(nItem >= 0 && nItem < GetListCtrl().GetItemCount())
	{
		LV_ITEM lvitem;

		if(GetCurrentItem() >= 0)
		{
			// elimina la selezione per la riga corrente
			memset(&lvitem,'\0',sizeof(LV_ITEM));
			lvitem.mask      = LVIF_STATE;
			lvitem.iItem     = GetCurrentItem();
			lvitem.iSubItem  = 0;
			lvitem.state     = 0;
			lvitem.stateMask = LVIS_FOCUSED | LVIS_SELECTED;
			GetListCtrl().SetItemState(GetCurrentItem(),&lvitem);

			//if(GetListCtrl().SetItemState(GetCurrentItem(),&lvitem))
			{
				// imposta come selezionata la riga specificata
				memset(&lvitem,'\0',sizeof(LV_ITEM));
				lvitem.mask      = LVIF_STATE;
				lvitem.iItem     = nItem;
				lvitem.iSubItem  = 0;
				lvitem.state     = LVIS_FOCUSED | LVIS_SELECTED;
				lvitem.stateMask = LVIS_FOCUSED | LVIS_SELECTED;
				if(GetListCtrl().SetItemState(nItem,&lvitem))
				{
					// si assicura che la riga sia visibile ed aggiorna l'indice interno
					SetRedraw(FALSE);
					GetListCtrl().EnsureVisible(nItem,TRUE);
					SetRedraw(TRUE);
					SetCurrentItem(nItem);
				}
			}
		}
	}

	return(GetCurrentItem());
}

/*
	OnSelectItem()
*/
VIRTUAL int CListViewEx::OnSelectItem(int /*nItem*/)
{
	// da ridefinire nella derivata
	return(0);
}

/*
	OnSelectItem()
*/
VIRTUAL int CListViewEx::OnSelectItem(CPoint /*point*/)
{
	// da ridefinire nella derivata
	return(0);
}

/*
	SetCurrentItem()

	Aggiorna l'indice interno (a base 0) relativo alla riga corrente.
	Notare che la riga corrente puo' non corrispondere con quella selezionata (focused).
*/
int CListViewEx::SetCurrentItem(int nItem)
{
	if(nItem >= 0 && nItem < GetListCtrl().GetItemCount())
		m_nCurrentItem = nItem;

	return(m_nCurrentItem);
}

/*
	GetCurrentItem()

	Restituisce l'indice interno relativo alla riga corrente.
	Notare che la riga corrente puo' non corrispondere con quella selezionata (focused).
*/
int CListViewEx::GetCurrentItem(LPSTR lpszItem/*=NULL*/,UINT nItemSize/*=(UINT)-1*/)
{
	// ricava l'item specificato
	if(lpszItem && nItemSize!=(UINT)-1)
	{
		memset(lpszItem,'\0',nItemSize);
		LV_ITEM lvitem = {0};
		lvitem.mask       = LVIF_TEXT;
		lvitem.iItem      = m_nCurrentItem;
		lvitem.iSubItem   = 0;
		lvitem.pszText    = lpszItem;
		lvitem.cchTextMax = nItemSize-1;
		GetListCtrl().GetItem(&lvitem);
	}
	// ricava l'item corrente
	else
	{
		int nItem = -1;
		if(GetListCtrl().GetSelectedCount() > 0)
			 nItem = GetListCtrl().GetNextItem(nItem,LVNI_SELECTED);
		if(nItem > 0)
			m_nCurrentItem = nItem;
	}

	return(m_nCurrentItem);
}

/*
	GetColumnCount()

	Ricava il numero di colonne del controllo.
*/
int CListViewEx::GetColumnCount()
{
     CHeaderCtrl* pHeader = (CHeaderCtrl*)GetListCtrl().GetDlgItem(0);
     return(pHeader ? pHeader->GetItemCount() : 0);
}

/*
	AutoSizeColumns()

	Dimensiona automaticamente le colonne in base al contenuto/etichetta, da chiamare dopo aver riempito il controllo.
	Usare solo se il controllo si trova in modalita' report.
*/
void CListViewEx::AutoSizeColumns(int nWidthArray[]/*=NULL*/,int nCol/*=-1*/,int nWidth/*=0*/)
{
	SetRedraw(FALSE);

	// deve trovarsi in modalita' report
	if((GetStyle() & LVS_REPORT))
	{
		int nMinCol = nCol < 0 ? 0 : nCol;
		int nMaxCol = nCol < 0 ? GetColumnCount()-1 : nCol;
		
		for(nCol = nMinCol; nCol <= nMaxCol; nCol++) 
		{
			if(nWidthArray)
				nWidth = nWidthArray[nCol];

			if(nWidth <= 0)
			{
				GetListCtrl().SetColumnWidth(nCol,LVSCW_AUTOSIZE);
				int nWidthAutosize = GetListCtrl().GetColumnWidth(nCol);

				GetListCtrl().SetColumnWidth(nCol,LVSCW_AUTOSIZE_USEHEADER);
				int nWidthUseHeader = GetListCtrl().GetColumnWidth(nCol);

				GetListCtrl().SetColumnWidth(nCol,max(MIN_COL_WIDTH,max(nWidthAutosize,nWidthUseHeader)));
			}
			else
				GetListCtrl().SetColumnWidth(nCol,nWidth);
		}
		
		RecalcHeaderTips();
	}

	SetRedraw(TRUE);
}

/*
	GetColumnsSize()

	Ricava la dimensione delle colonne del controllo.
*/
int CListViewEx::GetColumnsSize(int nWidthArray[],int nCol/*=-1*/)
{
	// deve trovarsi in modalita' report
	if((GetStyle() & LVS_REPORT))
	{
		int nMinCol = nCol < 0 ? 0 : nCol;
		int nMaxCol = nCol < 0 ? GetColumnCount()-1 : nCol;
		
		for(nCol = nMinCol; nCol <= nMaxCol; nCol++) 
			nWidthArray[nCol] = GetListCtrl().GetColumnWidth(nCol);
	}

	return(GetColumnCount());
}

/*
	AddHeaderToolTip()
*/
BOOL CListViewEx::AddHeaderToolTip(int nCol,LPCTSTR pTooltipText/*=NULL*/)
{
	char buf[TOOLTIP_TEXT_LENGTH+1] = {0};

	// controlla il numero della colonna
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	if(nCol >= pHeader->GetItemCount())
		return(FALSE);

	// deve trovarsi in modalita' report
	if((GetStyle() & LVS_TYPEMASK)!=LVS_REPORT)
		return(FALSE);

	// get the header height
	RECT rect;
	pHeader->GetClientRect(&rect);
	RECT rctooltip;
	rctooltip.top = 0;
	rctooltip.bottom = rect.bottom;

	// now get the left and right border of the column
	rctooltip.left = 0;// - GetScrollPos(SB_HORZ);
	for(int i = 0; i < nCol; i++)
		rctooltip.left += GetListCtrl().GetColumnWidth(i);
	rctooltip.right = rctooltip.left + GetListCtrl().GetColumnWidth(nCol);

	// se non viene specificato un testo, utilizza il nome della colonna
	if(!pTooltipText)
	{
		LV_COLUMN lvcolumn = {0};
		lvcolumn.mask       = LVCF_TEXT;
		lvcolumn.pszText    = buf;
		lvcolumn.cchTextMax = TOOLTIP_TEXT_LENGTH;
		if(!GetListCtrl().GetColumn(nCol,&lvcolumn))
			return(FALSE);
	}

	// elimina e reinserisce perche' il tooltip puo' venir (re)impostato varie volte
	m_ToolTipCtrl.DelTool(GetDlgItem(0),nCol+1);
	m_ToolTipCtrl.AddTool(GetDlgItem(0),pTooltipText ? pTooltipText : buf,&rctooltip,nCol+1);
	
	return(TRUE);
}

/*
	RecalcHeaderTips()
*/
void CListViewEx::RecalcHeaderTips(void)
{
	// update the tooltip info
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	
	RECT rect;
	pHeader->GetClientRect(&rect);
	RECT rctooltip;
	rctooltip.top = 0;
	rctooltip.bottom = rect.bottom;
	rctooltip.left = 0;// - GetScrollPos(SB_HORZ);

	CToolInfo toolinfo;
	toolinfo.cbSize = sizeof(toolinfo);
	
	// cycle through the tooltipinfo for each column
	int numcol = pHeader->GetItemCount();
	//for(int col = 0; col <= numcol; col++ )
	for(int col = 0; col < numcol; col++ )
	{
		m_ToolTipCtrl.GetToolInfo(toolinfo,pHeader,col+1);
		rctooltip.right = rctooltip.left + GetListCtrl().GetColumnWidth(col);
		toolinfo.rect = rctooltip;
		m_ToolTipCtrl.SetToolInfo (&toolinfo);
		rctooltip.left += GetListCtrl().GetColumnWidth(col);
	}
}

/*
	EditSubLabel()

	Editing del campo a riga,colonna.

	Restituisce un puntatore temporaneo (CEdit*) all'oggetto usato per l'editing. L'oggetto, di tipo
	CInPlaceEdit, si distrugge automaticamente a meno che non venga impostato il flag relativo.

	Al termine dell'editing, l'oggetto (CInPlaceEdit) invia il messaggio LVN_ENDLABELEDIT, generando
	la chiamata al gestore OnEndLabelEdit(), che aggiorna il campo della lista con SetItemText().
	Se la classe derivata gestisce gli elementi della lista in proprio (LVN_GETDISPINFO), l'aggiornamento
	con SetItemText() obbliga la lista a creare un nuovo elemento per il campo, mentre l'elemento gestito
	in proprio non viene modificato. In tal caso ridefinire, nella derivata, il gestore per LVN_ENDLABELEDIT
	e aggiornare l'elemento in proprio.
	
	BUG: blocco totale spostando la ruota del mouse durante l'editing (dare uno sguardo a OnMouseWheel())
*/
#ifdef IDC_IN_PLACE_EDIT
CEdit* CListViewEx::EditSubLabel(int nRow,int nCol)
{
	// Make sure that nRow is visible
	if(!GetListCtrl().EnsureVisible(nRow,TRUE))
		return(NULL);

	// Make sure that nCol is valid
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	int nColumnCount = pHeader->GetItemCount();
	if(nCol >= nColumnCount || GetListCtrl().GetColumnWidth(nCol) < 5)
		return(NULL);

// BUGFIX : when columns are moved
// When user change column's order by dragging header Control, original code has some bug.
// Edit Box is not show at correct position or HitTestEx return incoreect column number.
#if 0
	// Get the column offset
	int offset = 0;
	for(int i=0; i < nCol; i++)
		offset += GetListCtrl().GetColumnWidth(i);
#else
	// Get the column offset
	int offset = 0;
	// Array of Column prder by Sang-il, Lee
	INT *piColumnArray = new INT[nColumnCount];
	GetListCtrl().GetColumnOrderArray(piColumnArray);
	for(int i=0; nCol!=piColumnArray[i]; i++)
		offset += GetListCtrl().GetColumnWidth(piColumnArray[i]);
	// delete Array
	delete [] piColumnArray;
#endif
// ENDBUGFIX

	CRect rect;
	GetListCtrl().GetItemRect(nRow,&rect,LVIR_BOUNDS);

	// Now scroll if we need to expose the column
	CRect rcClient;
	GetClientRect(&rcClient);
	if(offset + rect.left < 0 || offset + rect.left > rcClient.right)
	{
		CSize size;
		size.cx = offset + rect.left;
		size.cy = 0;
		GetListCtrl().Scroll(size);
		rect.left -= size.cx;
	}
	rect.left += offset + 4;
	rect.right = rect.left + GetListCtrl().GetColumnWidth(nCol) - 3;
	if(rect.right > rcClient.right)
		rect.right = rcClient.right;

	// Get Column alignment
	LV_COLUMN lvcol = {0};
	lvcol.mask = LVCF_FMT;
	GetListCtrl().GetColumn(nCol,&lvcol);
	DWORD dwStyle;
	if((lvcol.fmt & LVCFMT_JUSTIFYMASK)==LVCFMT_LEFT)
		dwStyle = ES_LEFT;
	else if((lvcol.fmt & LVCFMT_JUSTIFYMASK)==LVCFMT_RIGHT)
		dwStyle = ES_RIGHT;
	else
		dwStyle = ES_CENTER;
	dwStyle |= WS_BORDER|WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL;
	
	CEdit *pEdit = new CInPlaceEdit(nRow,nCol,GetListCtrl().GetItemText(nRow,nCol));
	if(pEdit)
		pEdit->Create(dwStyle,rect,this,IDC_IN_PLACE_EDIT);

	return(pEdit);
}
#endif

/*
	OnEndLabelEdit()
	
	Intercetta il messaggio LVN_ENDLABELEDIT, inviato dall'oggetto utilizzado per l'editing del campo
	(CInPlaceEdit).
	Se la classe derivata gestisce gli elementi della lista in proprio (LVN_GETDISPINFO), l'aggiornamento
	con SetItemText() obbliga la lista a creare un nuovo elemento per il campo, mentre l'elemento gestito
	in proprio non viene modificato. In tal caso ridefinire, nella derivata, il gestore per LVN_ENDLABELEDIT
	e aggiornare l'elemento in proprio.
*/
#ifdef IDC_IN_PLACE_EDIT
VIRTUAL void CListViewEx::OnEndLabelEdit(NMHDR* pNMHDR,LRESULT* pResult)
{
	LV_DISPINFO *plvDispInfo = (LV_DISPINFO*)pNMHDR;
	LV_ITEM *plvItem = &plvDispInfo->item;

	if(plvItem->pszText)
		GetListCtrl().SetItemText(plvItem->iItem,plvItem->iSubItem,plvItem->pszText);

	*pResult = FALSE;
}
#endif

/*
	HitTestEx()

	Ricava la riga (HitTest()) e la colonna a cui e' stato effettuato il click.
*/
int CListViewEx::HitTestEx(CPoint &point, int *col) const
{
	int colnum = 0;
	int row = GetListCtrl().HitTest( point, NULL );
	
	if(col) *col = -1; // 0;

	// Make sure that the ListView is in LVS_REPORT
	if((GetWindowLong(m_hWnd,GWL_STYLE) & LVS_TYPEMASK)!=LVS_REPORT)
		return row;

	// Get the top and bottom row visible
	row = GetListCtrl().GetTopIndex();
	int bottom = row + GetListCtrl().GetCountPerPage();
	if(bottom > GetListCtrl().GetItemCount())
		bottom = GetListCtrl().GetItemCount();
	
// BUGFIX : when columns are moved
// When user change column's order by dragging header Control, original code has some bug.
// Edit Box is not show at correct position or HitTestEx return incoreect column number.
#if 0
	// Get the number of columns
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	int nColumnCount = pHeader->GetItemCount();

	// Loop through the visible rows
	for( ;row <=bottom;row++)
	{
		// Get bounding rect of item and check whether point falls in it.
		CRect rect;
		GetListCtrl().GetItemRect(row,&rect,LVIR_BOUNDS);
		if(rect.PtInRect(point))
		{
			// Now find the column
			for(colnum = 0; colnum < nColumnCount; colnum++)
			{
				int colwidth = GetListCtrl().GetColumnWidth(colnum);
				if(point.x >= rect.left && point.x <= (rect.left + colwidth))
				{
					if(col) *col = colnum;
					return row;
				}
				rect.left += colwidth;
			}
		}
	}

	return -1;
#else
	// Get the number of columns
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	int nColumnCount = pHeader->GetItemCount();

	// Loop through the visible rows

	// Array of Column prder by Sang-il, Lee
	INT* piColumnArray = new INT[nColumnCount];
	((CListCtrl*)this)->GetColumnOrderArray(piColumnArray);

	for( ;row <=bottom;row++)
	{
		// Get bounding rect of item and check whether point falls in it.
		CRect rect;
		GetListCtrl().GetItemRect(row,&rect,LVIR_BOUNDS);
		if(rect.PtInRect(point))
		{
			// Now find the column
			for(colnum = 0; colnum < nColumnCount; colnum++)
			{
				int colwidth = GetListCtrl().GetColumnWidth(piColumnArray[colnum]);
				if( point.x >= rect.left && point.x <= (rect.left + colwidth))
				{
					if(col) *col = piColumnArray[colnum];
					TRACE("HitTestEx() col=%d\n",*col);
					delete [] piColumnArray;
					return row;
				}
				rect.left += colwidth;
			}
		}
	}
	delete [] piColumnArray;

	return -1;
#endif
}

/*
	SetRedraw()

	Evita lo sfarfallio durente il refresh della lista.
*/
void CListViewEx::SetRedraw(BOOL bRedraw)
{
	if(!bRedraw)
	{
		if(m_nRedrawCount++ <= 0)
			GetListCtrl().SetRedraw(FALSE);
	}
	else
	{
		if(--m_nRedrawCount <= 0)
		{
			GetListCtrl().SetRedraw(TRUE);
			m_nRedrawCount = 0;
			Invalidate();
		}
	}
}
