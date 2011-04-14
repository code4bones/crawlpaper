/*
	CToolBarCtrlEx.cpp
	Classe derivata per la toolbar (MFC).
	Luca Piergentili, 30/08/00
	lpiergentili@yahoo.com
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "window.h"
#include "CNodeList.h"
#include "CToolBarCtrlEx.h"

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

/*
	CToolBarCtrlEx()
*/
CToolBarCtrlEx::CToolBarCtrlEx()
{
	m_pToolBarButtonsList = new CToolBarButtonList();
	m_pTbArray = NULL;
	m_nToolBarButtonIndex = m_nToolBarStringIndex = 0;
	memset(m_szToolBarTextStrings,'\0',sizeof(m_szToolBarTextStrings));
}

/*
	~CToolBarCtrlEx()
*/
CToolBarCtrlEx::~CToolBarCtrlEx()
{
	if(m_pToolBarButtonsList)
		delete m_pToolBarButtonsList,m_pToolBarButtonsList = NULL;
	if(m_pTbArray)
		delete [] m_pTbArray,m_pTbArray = NULL;
}

/*
	Create()

	Crea la toolbar.

	DWORD	dwStyle		flag per lo stile
	CWnd*	pWndParent	puntatore alla finestra padre
	UINT		nID			id risorsa del dialogo
*/
BOOL CToolBarCtrlEx::Create(DWORD dwStyle,CWnd* pWndParent,UINT nID)
{
	RECT rect = {0,0,0,0};
	BOOL bCreated = CToolBarCtrl::Create(dwStyle,rect,pWndParent,nID);
	
	// se e' stato specificato il flag per la creazione dei tooltips,
	// aggiunge quello per visualizzarli anche se in secondo piano
	if(bCreated && (dwStyle & TBSTYLE_TOOLTIPS))
	{
		CToolTipCtrl* pToolTipCtrl = CToolBarCtrl::GetToolTips();
		if(pToolTipCtrl)
		{
			LONG lStyle = ::GetWindowLong(pToolTipCtrl->m_hWnd,GWL_STYLE);
			if(lStyle!=0L)
			{
				lStyle |= TTS_ALWAYSTIP;
				::SetWindowLong(pToolTipCtrl->m_hWnd,GWL_STYLE,lStyle);
			}
		}
	}
	
	return(bCreated);
}

/*
	Add()

	Aggiunge un bottone alla toolbar.

	UINT	nID = 0					id risorsa del bottone (0 se si tratta di un separatore)
	UINT nToolTipID = 0				id risorsa della stringa da utilizzare per il tooltip del bottone
	BYTE fsState = TBSTATE_ENABLED	status (abilitato/disabilitato, indeterminato se si tratta di un separatore)
*/
BOOL CToolBarCtrlEx::Add(UINT nID,UINT nToolTipID,UINT nTextID,BYTE fsState)
{
	BOOL bAdded = FALSE;

	if(m_pToolBarButtonsList)
	{
		TOOLBARBUTTON* t = new TOOLBARBUTTON;
		if(t)
		{
			t->button.iBitmap   = nID==0 ? NULL : m_nToolBarButtonIndex++;
			t->button.idCommand = nID;
			t->button.fsState   = nID==0 ? (BYTE)TBSTATE_INDETERMINATE : fsState;
			t->button.fsStyle   = nID==0 ? (BYTE)TBSTYLE_SEP : (BYTE)TBSTYLE_BUTTON; //|(BYTE)TBSTYLE_AUTOSIZE;
			t->button.dwData    = 0;
			t->button.iString   = nTextID==0 ? NULL : m_nToolBarStringIndex++;
			t->tooltip          = nToolTipID;
			t->text			= nTextID;

			bAdded = m_pToolBarButtonsList->Add(t);
		}
	}

	return(bAdded);
}

/*
	Attach()

	Collega la toolbar al controllo.

	UINT	nID					l'id del controllo
	const CSize& sizeButton		dimensione (in pixel) del bottone (+2 rispetto al bitmap)
	const CSize& sizeBitmap		dimensione (in pixel) del bitmap usato per il bottone (-2 rispetto al bottone)
*/
BOOL CToolBarCtrlEx::Attach(const CSize& sizeButton,const CSize& sizeBitmap,UINT nID/*=(UINT)-1L*/,UINT nIDCold/*=(UINT)-1L*/,UINT nIDHot/*=(UINT)-1L*/)
{
	BOOL bAttached = FALSE;

	if(m_pToolBarButtonsList)
	{
		int nButtonsCount = m_pToolBarButtonsList->Count();

		if(nButtonsCount > 0)
		{
			m_pTbArray = new TBBUTTON[nButtonsCount];
			
			if(m_pTbArray)
			{
				TOOLBARBUTTON* t;
				char szResourceString[32];
				int nOfs = 0;
				int n;

				memset(m_szToolBarTextStrings,'\0',sizeof(m_szToolBarTextStrings));

				for(int i = 0; i < nButtonsCount; i++)
				{
					if((t = (TOOLBARBUTTON*)m_pToolBarButtonsList->GetAt(i))!=(TOOLBARBUTTON*)NULL)
					{
						memcpy((void*)&m_pTbArray[i],(void*)&t->button,sizeof(TBBUTTON));
						if(t->text!=0L)
						{
							if((n = ::LoadString(NULL,t->text,szResourceString,sizeof(szResourceString)-1))!=0)
							{
								if(nOfs+n < sizeof(m_szToolBarTextStrings)-2)
								{
									memcpy(m_szToolBarTextStrings+nOfs,szResourceString,n);
									nOfs += n+1;
								}
							}
						}
					}
					else
						memset((void*)&m_pTbArray[i],'\0',sizeof(TBBUTTON));
				}

				if(nIDCold!=(UINT)-1L && nIDHot!=(UINT)-1L)
				{
					CImageList imageList;
					CBitmap bitmap;

					// lista immagini standard
					bitmap.LoadBitmap(nIDCold);
					imageList.Create(sizeBitmap.cx,sizeBitmap.cy,ILC_COLORDDB|ILC_MASK,nButtonsCount,1);
					imageList.Add(&bitmap,RGB(255,0,255));
					this->SendMessage(TB_SETIMAGELIST,0,(LPARAM)imageList.m_hImageList);
					imageList.Detach();
					bitmap.Detach();

					// lista immagini 'hot'
					bitmap.LoadBitmap(nIDHot);
					imageList.Create(sizeBitmap.cx,sizeBitmap.cy,ILC_COLORDDB|ILC_MASK,nButtonsCount,1);
					imageList.Add(&bitmap,RGB(255,0,255));
					this->SendMessage(TB_SETHOTIMAGELIST,0,(LPARAM)imageList.m_hImageList);
					imageList.Detach();
					bitmap.Detach();
					bAttached = TRUE;
				}
				else if(nID!=(UINT)-1L)
				{
					CToolBarCtrl::SetBitmapSize(sizeButton);
					CToolBarCtrl::SetBitmapSize(sizeBitmap);
					CToolBarCtrl::AddBitmap(nButtonsCount,nID);
					bAttached = TRUE;
				}

				if(bAttached)
				{
					if(m_szToolBarTextStrings[0]!='\0')
						CToolBarCtrl::AddStrings(m_szToolBarTextStrings);
					CToolBarCtrl::AddButtons(nButtonsCount,m_pTbArray);
					CToolBarCtrl::AutoSize();
				}
			}
		}
	}

	return(bAttached);
}

/*
	EnableButton()

	Abilita o disabilita il bottone relativo all'id.

	UINT nID		id risorsa del bottone.
	BOOL bState	status (TRUE=abilitato, FALSE=disabilitato)
*/
void CToolBarCtrlEx::EnableButton(UINT nID,BOOL bState)
{
	if(m_pToolBarButtonsList)
	{
		ITERATOR iter;
		TOOLBARBUTTON* t;
		
		if((iter = m_pToolBarButtonsList->First())!=(ITERATOR)NULL)
		{
			do
			{
				t = (TOOLBARBUTTON*)iter->data;

				if(t->button.idCommand==(int)nID)
				{
					CToolBarCtrl::EnableButton(nID,bState);
					break;
				}

				iter = m_pToolBarButtonsList->Next(iter);
			
			} while(iter!=(ITERATOR)NULL);
		}
	}
}

/*
	IsButtonEnabled()

	Restituisce lo status del bottone relativo all'id.

	UINT nID	id risorsa del bottone
*/
BOOL CToolBarCtrlEx::IsButtonEnabled(UINT nID)
{
	if(m_pToolBarButtonsList)
	{
		ITERATOR iter;
		TOOLBARBUTTON* t;
		
		if((iter = m_pToolBarButtonsList->First())!=(ITERATOR)NULL)
		{
			do
			{
				t = (TOOLBARBUTTON*)iter->data;

				if(t->button.idCommand==(int)nID)
					return(CToolBarCtrl::IsButtonEnabled(nID));

				iter = m_pToolBarButtonsList->Next(iter);
			
			} while(iter!=(ITERATOR)NULL);
		}
	}

	return(FALSE);
}

/*
	GetToolTipID()

	Restituisce l'id risorsa per la stringa del tooltip associata al bottone della toolbar.

	UINT nID	id risorsa del bottone
*/
UINT CToolBarCtrlEx::GetToolTipID(UINT nID)
{
	if(m_pToolBarButtonsList)
	{
		ITERATOR iter;
		TOOLBARBUTTON* t;
		
		if((iter = m_pToolBarButtonsList->First())!=(ITERATOR)NULL)
		{
			do
			{
				t = (TOOLBARBUTTON*)iter->data;

				if(t->button.idCommand==(int)nID)
					return(t->tooltip);

				iter = m_pToolBarButtonsList->Next(iter);
			
			} while(iter!=(ITERATOR)NULL);
		}

	}

	return(0);
}
