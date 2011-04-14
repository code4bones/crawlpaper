/*
	CListViewEx.h
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
#ifndef _CLISTVIEWEX_H
#define _CLISTVIEWEX_H 1

#include "window.h"
#include <afxcview.h>
#include "CNodeList.h"

#include "resource.h"
#ifdef IDC_IN_PLACE_EDIT
  #include "CInPlaceEdit.h"
#else
  #include "macro.h"
  #pragma message("\t\t\t"__FILE__"("STR(__LINE__)"): warning: IDC_IN_PLACE_EDIT not defined, compiling without support for in place editing")
#endif

// dimensione minima della colonna
#define MIN_COL_WIDTH 100

// numero di pixel per lo scroll oriz.
#define HSCROLL_PIXEL_TO_SCROLL 10

// dimensione massima per il testo del tooltip
#define MAX_TOOLTIP_TEXT		256
#define TOOLTIP_TEXT_LENGTH	80
/*
	ICONINDEX
	struttura per la lista delle icone per gli elementi della lista
*/
struct ICONINDEX {
	int		index;
	HICON	icon;
	char		ext[_MAX_EXT+1];
};

/*
	CIconIndexList
	classe per la lista delle icone per gli elementi della lista
*/
class CIconIndexList : public CNodeList
{
public:
	CIconIndexList() : CNodeList() {}
	virtual ~CIconIndexList() {CNodeList::DeleteAll();}
	
	void* Create(void)
	{
		ICONINDEX* pData = new ICONINDEX;
		return(pData);
	}
	
	void* Initialize(void* pVoid)
	{
		ICONINDEX* pData = (ICONINDEX*)pVoid;
		if(!pData)
			pData = (ICONINDEX*)Create();
		if(pData)
			memset(pData,'\0',sizeof(ICONINDEX));
		return(pData);
	}

	BOOL PreDelete(ITERATOR iter)
	{
		if((ICONINDEX*)iter->data)
		{
			if(((ICONINDEX*)iter->data)->icon)
				::DestroyIcon(((ICONINDEX*)iter->data)->icon);
			delete (ICONINDEX*)iter->data;
			iter->data = (ICONINDEX*)NULL;
		}
		return(TRUE);
	}
	
	int Size(void) {return(sizeof(ICONINDEX));}

#ifdef _DEBUG
	const char* Signature(void) {return("CIconIndexList");}
#endif
};

/*
	CListViewEx
*/
class CListViewEx : public CListView
{
	DECLARE_DYNCREATE(CListViewEx)

public:
	CListViewEx();
	virtual ~CListViewEx() {}

	virtual BOOL	PreTranslateMessage	(MSG* pMsg);
	virtual BOOL	PreCreateWindow	(CREATESTRUCT& cs);
	virtual int	OnCreate			(LPCREATESTRUCT lpcs);
	virtual void	OnClose			(void);
	virtual void	OnDestroy			(void);
	virtual void	OnActivateView		(BOOL bActivate,CView* pActivateView,CView* pDeactiveView);
	virtual void	OnInitialUpdate	(void);
	virtual void	OnUpdate			(CView* pView,LPARAM lHint,CObject* pHint);
	virtual BOOL	OnNotify			(WPARAM wParam,LPARAM lParam,LRESULT* pResult);
	virtual void	OnChar			(UINT nChar,UINT nRepCnt,UINT nFlags);
	virtual void	OnKeyDown			(UINT nChar,UINT nRepCnt,UINT nFlags);
	virtual void	OnLButtonDown		(UINT nFlags,CPoint point);
	virtual void	OnRButtonDown		(UINT nFlags,CPoint point);
	virtual void	OnHScroll			(UINT nSBCode,UINT nPos,CScrollBar* pScrollBar);
	virtual void	OnVScroll			(UINT nSBCode,UINT nPos,CScrollBar* pScrollBar);

	int			SelectItem		(int nItem);
	virtual int	OnSelectItem		(int nItem);
	virtual int	OnSelectItem		(CPoint point);
	int			SetCurrentItem		(int nItem);
	int			GetCurrentItem		(LPSTR lpszItem = NULL,UINT nItemSize = (UINT)-1);
	
	int			GetColumnCount		(void);
	void			AutoSizeColumns	(int nWidthArray[] = NULL,int nCol = -1,int nWidth = 0);
	int			GetColumnsSize		(int nWidthArray[],int nCol = -1);

	BOOL			AddHeaderToolTip	(int nCol,LPCTSTR pTooltipText = NULL);
	void			RecalcHeaderTips	(void);

#ifdef IDC_IN_PLACE_EDIT
	CEdit*		EditSubLabel		(int nRow,int nCol);
	virtual void	OnEndLabelEdit		(NMHDR* pNMHDR,LRESULT* pResult);
#endif

	int			HitTestEx			(CPoint &point,int* col) const;

	void			SetRedraw			(BOOL bRedraw);

private:
	int			m_nCurrentItem;
	int			m_nRedrawCount;
	CToolTipCtrl	m_ToolTipCtrl;

	DECLARE_MESSAGE_MAP();
};

#endif // _CLISTVIEWEX_H
