/*
	CStatusBarCtrlEx.cpp
	Classe derivata per la statusbar (MFC).
	Luca Piergentili, 30/08/00
	lpiergentili@yahoo.com
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include "window.h"
#include "CNodeList.h"
#include "CStatusBarCtrlEx.h"

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
	CStatusBarCtrlEx()
*/
CStatusBarCtrlEx::CStatusBarCtrlEx()
{
	m_pPartList = new CStatusBarPartList();
	m_pArray = NULL;
}

/*
	~CStatusBarCtrlEx()
*/
CStatusBarCtrlEx::~CStatusBarCtrlEx()
{
	if(m_pPartList)
		delete m_pPartList,m_pPartList = NULL;
	
	if(m_pArray)
		delete [] m_pArray,m_pArray = NULL;
}

/*
	Create()
	
	Crea la statusbar.
	
	DWORD	dwStyle		flag per stile
	CWnd*	pWndParent	ptr alla finestra padre
	UINT		nID			id risorsa del dialogo
*/
BOOL CStatusBarCtrlEx::Create(DWORD dwStyle,CWnd* pWndParent,UINT nID)
{
	RECT rect = {0,0,0,0};
	return(CStatusBarCtrl::Create(dwStyle,rect,pWndParent,nID));
}

/*
	Add()

	Aggiunge un pannello alla statusbar.

	UINT size		dimensione del pannello
*/
BOOL CStatusBarCtrlEx::Add(int nSize)
{
	BOOL bFlag = FALSE;

	if(m_pPartList)
	{
		STATUSBARPART* s = new STATUSBARPART;
		if(s)
		{
			s->size = nSize;
			bFlag = m_pPartList->Add(s);
		}
	}

	return(bFlag);
}

/*
	Attach()

	Collega la statusbar al dialogo.
*/
BOOL CStatusBarCtrlEx::Attach(void)
{
	BOOL bFlag = FALSE;

	if(m_pPartList)
	{
		int nTot = m_pPartList->Count();

		if(nTot > 0)
		{
			m_pArray = new int[nTot];
			
			if(m_pArray)
			{
				STATUSBARPART* s;

				for(int i = 0; i < nTot; i++)
				{
					if((s = (STATUSBARPART*)m_pPartList->GetAt(i))!=(STATUSBARPART*)NULL)
						m_pArray[i] = s->size;
					else
						m_pArray[i] = 0;
				}

				CStatusBarCtrl::SetParts(nTot,m_pArray);

				bFlag = TRUE;
			}
		}
	}

	return(bFlag);
}

/*
	SetPanel()

	Imposta il testo del pannello della statusbar.

	LPCSTR lpcszText	testo per il pannello
	UINT nPanel		indice del pannello da impostare (base 0)
*/
void CStatusBarCtrlEx::SetPanel(LPCSTR lpcszText,UINT nPanel)
{
	CStatusBarCtrl::SetText(lpcszText,nPanel,0);
}
