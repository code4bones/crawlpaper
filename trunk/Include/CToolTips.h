/*
	CToolTips.h
	Luca Piergentili, 18/04/03
	lpiergentili@yahoo.com
*/
#ifndef _CTOOLTIPS_H
#define _CTOOLTIPS_H 1

#include "window.h"
#include "CNodeList.h"
#include "CToolTipCtrlEx.h"

/*	
	CToolTipsList
	Classe per i tooltip.
*/
#define TOOLTIPLIST_TEXT 256

struct TOOLTIPLIST {
	UINT control_id;
	UINT string_id;
	char tooltip_text[TOOLTIPLIST_TEXT+1];
};

class CToolTipsList : public CNodeList
{
public:
	CToolTipsList() {}
	virtual ~CToolTipsList() {CNodeList::DeleteAll();}
	BOOL PreDelete(ITERATOR iter)
	{
		if((TOOLTIPLIST*)iter->data)
			delete ((TOOLTIPLIST*)iter->data);
		iter->data = (TOOLTIPLIST*)NULL;
		return(TRUE);
	}
	int Size(void) {return(sizeof(TOOLTIPLIST));}
#ifdef _DEBUG
	const char* Signature(void) {return("CToolTipsList");}
#endif
};

class CToolTips : public CToolTipCtrlEx
{
public:
	CToolTips()
	{
		m_pTooltip = NULL;
		m_pTooltipList = NULL;
	}

	~CToolTips()
	{
		if(m_pTooltip)
			 delete m_pTooltip,m_pTooltip = NULL;
		if(m_pTooltipList)
			 delete m_pTooltipList,m_pTooltipList = NULL;
	}

	BOOL Create(CWnd* pWnd)
	{
		BOOL bCreated = FALSE;

		if((m_pTooltipList = new CToolTipsList())!=(CToolTipsList*)NULL)
		{
			if((m_pTooltip = new CToolTipCtrlEx())!=(CToolTipCtrlEx*)NULL)
			{
				bCreated = m_pTooltip->Create(pWnd);
				m_pWnd = pWnd;
			}
		}

		return(bCreated);
	}

	BOOL Add(UINT nControlID,UINT nStringID,UINT nSize/* = 250 */)
	{
		BOOL bAdded = FALSE;

		if(m_pTooltipList)
		{
			if(m_pTooltip)
			{
				TOOLTIPLIST* t = new TOOLTIPLIST;
				if(t)
				{
					t->control_id = nControlID;
					t->string_id  = nStringID;
					::LoadString(AfxGetInstanceHandle(),t->string_id,t->tooltip_text,TOOLTIPLIST_TEXT);
					m_pTooltipList->Add(t);
					if(nSize!=0)
					{
#if _MSC_VER >= 1200
						m_pTooltip->SetMaxTipWidth(nSize);
#else
						m_pTooltip->SendMessage(TTM_SETMAXTIPWIDTH,0,nSize);
#endif
					}
					CWnd* p = m_pWnd->GetDlgItem(t->control_id);
					m_pTooltip->AddWindowTool(p,t->tooltip_text);
					bAdded = TRUE;
				}
			}
		}

		return(bAdded);
	}

private:
	CToolTipCtrlEx*	m_pTooltip;
	CToolTipsList*		m_pTooltipList;
	CWnd*			m_pWnd;
};

#endif // _CTOOLTIPS_H
