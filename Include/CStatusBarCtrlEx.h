/*
	CStatusBarCtrlEx.h
	Classe derivata per la statusbar (MFC).
	Luca Piergentili, 30/08/00
	lpiergentili@yahoo.com
*/
#ifndef _CSTATUSBARCTRLEX_H
#define _CSTATUSBARCTRLEX_H 1

#include "window.h"
#include "CNodeList.h"

/*
	STATUSBARPART
	struttura per il pannello della statusbar
*/
struct STATUSBARPART {
	int size;
};

/*
	CStatusBarPartList
	Classe per la lista dei pannelli della statusbar.
*/
class CStatusBarPartList : public CNodeList
{
public:
	CStatusBarPartList() : CNodeList() {}
	virtual ~CStatusBarPartList() {CNodeList::DeleteAll();}

	void* Create(void)
	{
		STATUSBARPART* pData = new STATUSBARPART;
		return(pData);
	}
	
	void* Initialize(void* pVoid)
	{
		STATUSBARPART* pData = (STATUSBARPART*)pVoid;
		if(!pData)
			pData = (STATUSBARPART*)Create();
		if(pData)
			memset(pData,'\0',sizeof(STATUSBARPART));
		return(pData);
	}

	BOOL PreDelete(ITERATOR iter)
	{
		if((STATUSBARPART*)iter->data)
		{
			delete ((STATUSBARPART*)iter->data);
			iter->data = (STATUSBARPART*)NULL;
		}
		return(TRUE);
	}

	int Size(void) {return(sizeof(STATUSBARPART));}

#ifdef _DEBUG
	const char* Signature(void) {return("CStatusBarPartList");}
#endif
};

/*
	CStatusBarCtrlEx
	Classe derivata per la statusbar.
*/
class CStatusBarCtrlEx : public CStatusBarCtrl
{
public:
	CStatusBarCtrlEx();
	virtual ~CStatusBarCtrlEx();

	BOOL		Create	(DWORD dwStyle,CWnd* pWndParent,UINT nID);
	BOOL		Add		(int nSize);
	BOOL		Attach	(void);
	void		SetPanel	(LPCSTR lpcszText,UINT nPanel);

private:
	CStatusBarPartList*	m_pPartList;
	int*				m_pArray;
};

#endif // _CSTATUSBARCTRLEX_H
