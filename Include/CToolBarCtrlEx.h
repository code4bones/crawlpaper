/*
	CToolBarCtrlEx.h
	Classe derivata per la toolbar (MFC).
	Luca Piergentili, 30/08/00
	lpiergentili@yahoo.com
*/
#ifndef _CTOOLBARCTRLEX_H
#define _CTOOLBARCTRLEX_H 1

#include "window.h"
#include "CNodeList.h"

/*
	TOOLBARBUTTON
	struttura per il bottone della toolbar
*/
struct TOOLBARBUTTON {
	TBBUTTON button;
	UINT tooltip;
	UINT text;
};

/*
	CToolBarButtonList
	Classe per la lista dei bottoni presenti nella toolbar.
*/
class CToolBarButtonList : public CNodeList
{
public:
	CToolBarButtonList() : CNodeList() {}
	virtual ~CToolBarButtonList() {CNodeList::DeleteAll();}
	void* Create(void)
	{
		return(new TOOLBARBUTTON);
	}	
	void* Initialize(void* pVoid)
	{
		TOOLBARBUTTON* pData = (TOOLBARBUTTON*)pVoid;
		if(!pData)
			pData = (TOOLBARBUTTON*)Create();
		if(pData)
			memset(pData,'\0',sizeof(TOOLBARBUTTON));
		return(pData);
	}
	BOOL PreDelete(ITERATOR iter)
	{
		if((TOOLBARBUTTON*)iter->data)
		{
			delete ((TOOLBARBUTTON*)iter->data);
			iter->data = (TOOLBARBUTTON*)NULL;
		}
		return(TRUE);
	}
	int Size(void) {return(sizeof(TOOLBARBUTTON));}
#ifdef _DEBUG
	const char* Signature(void) {return("CToolBarButtonList");}
#endif
};

/*
	CToolBarCtrlEx
	Classe derivata per la toolbar.
*/
class CToolBarCtrlEx : public CToolBarCtrl
{
public:
	CToolBarCtrlEx();
	virtual ~CToolBarCtrlEx();

	BOOL		Create		(DWORD dwStyle,CWnd* pWndParent,UINT nID);
	BOOL		Add			(UINT nID = 0,UINT nToolTipID = 0,UINT nTextID = 0,BYTE fsState = TBSTATE_ENABLED);
	BOOL		Attach		(const CSize& sizeButton,const CSize& sizeBitmap,UINT nID = (UINT)-1L,UINT nIDCold = (UINT)-1L,UINT nIDHot = (UINT)-1L);
	void		EnableButton	(UINT nID,BOOL bState);
	BOOL		IsButtonEnabled(UINT nID);
	UINT		GetToolTipID	(UINT nID);

private:
	CToolBarButtonList*		m_pToolBarButtonsList;
	TBBUTTON*				m_pTbArray;
	UINT					m_nToolBarButtonIndex;
	UINT					m_nToolBarStringIndex;
	char					m_szToolBarTextStrings[256];
};

#endif // _CTOOLBARCTRLEX_H

