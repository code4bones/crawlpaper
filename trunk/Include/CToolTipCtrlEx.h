/*
	CToolTipCtrlEx.h
	Classe derivata per gestire il bug presente nella principale (MFC).
	Luca Piergentili, 17/08/00
	lpiergentili@yahoo.com

	Basata su "Programmare Windows 95 con MFC" di Jeff Prosise.
*/
#ifndef _CTOOLTIPCTRLEX_H
#define _CTOOLTIPCTRLEX_H 1

#include "CNodeList.h"
#include "window.h"

#define TOOLTIP_SMALL_WIDTH			150
#define TOOLTIP_REASONABLE_WIDTH		260
#define TOOLTIP_LARGE_WIDTH			400
#define TOOLTIP_XXL_WIDTH			525
#define TOOLTIP_REASONABLE_DELAYTIME	30000L

/*	
	TOOLTIPITEM
	struttura per l'elemento della lista per i tooltip
*/
struct TOOLTIPITEM {
	UINT		nCtrlID;
	UINT		nStringID;
	char*	pszText;
	BOOL		bExtended;
};

/*	
	CToolTipList
	Classe per la lista per i tooltip.
*/
class CToolTipList : public CNodeList
{
public:
	CToolTipList() : CNodeList() {}
	virtual ~CToolTipList() {CNodeList::DeleteAll();}
	
	void* Create(void)
	{
		return(new TOOLTIPITEM);
	}
	void* Initialize(void* pVoid)
	{
		TOOLTIPITEM* pData = (TOOLTIPITEM*)pVoid;
		if(!pData)
			pData = (TOOLTIPITEM*)Create();
		if(pData)
		{
			pData->nCtrlID	 = (UINT)-1L;
			pData->nStringID = (UINT)-1L;
			pData->pszText	 = NULL;
			pData->bExtended = FALSE;
		}
		return(pData);
	}
	BOOL PreDelete(ITERATOR iter)
	{
		TOOLTIPITEM* pData = (TOOLTIPITEM*)iter->data;
		if(pData)
		{
			if(pData->pszText)
				delete [] pData->pszText,pData->pszText = NULL;
			delete pData,pData = NULL;
		}
		return(TRUE);
	}
	int Size(void) {return(sizeof(TOOLTIPITEM));}
#ifdef _DEBUG
	const char* Signature(void) {return("CToolTipList");}
#endif
};

/*
	CToolTipCtrlEx
*/
class CToolTipCtrlEx : public CToolTipCtrl
{
public:
	// gestisce in proprio l'allocazione del testo a partire dall'id della risorsa relativa
	BOOL			AddTooltip	(CWnd* pWnd,UINT nCtrlID,UINT nStringID);

	// il chiamante deve gestire l'allocazione del testo
	BOOL			AddWindowTool	(CWnd* pWnd,LPCSTR pszText,UINT nIDTool = 0/* non utilizzato */);
	BOOL			AddRectTool	(CWnd* pWnd,LPCSTR pszText,LPCRECT lpRect,UINT nIDTool);
	
	// imposta la dimensione massima della finestra per il tooltip, non quella del testo
	void			SetWidth		(int nSize);

	inline DWORD	GetDelay		(void) {return(GetDelayTime(TTDT_AUTOPOP));}
	inline void	SetDelay		(DWORD dwStayDelay) {SetDelayTime(TTDT_AUTOPOP,dwStayDelay);}

private:
	CToolTipList	m_TooltipList;
};

#endif // _CTOOLTIPCTRLEX_H
