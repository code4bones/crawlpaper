/*
	COleDropTargetEx.cpp
	Classe derivata per la gestione del drag & drop via OLE (MFC).
	Luca Piergentili, 07/09/00
	lpiergentili@yahoo.com
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include "strings.h"
#include "window.h"
#include <afxole.h>
#include <afxdisp.h>
#include "CNodeList.h"
#include "COleDropTargetEx.h"

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

// se si chiama piu' di una volta la AfxOleInit() nello stesso thread MFC da errore
static BOOL g_bAfxOleInitialized = FALSE;

/*
	COleDropTargetEx()
*/
COleDropTargetEx::COleDropTargetEx()
{
	m_pWnd = NULL;
	m_nMsg = 0L;
	m_bEnabled = TRUE;
	m_defaultDropEffect = DROPEFFECT_COPY|DROPEFFECT_MOVE|DROPEFFECT_LINK|DROPEFFECT_SCROLL;
}

/*
	~COleDropTargetEx()
*/
COleDropTargetEx::~COleDropTargetEx()
{
	Detach();
}

/*
	Attach()

	L'inizializzazione delle DLL usate da OLE, con la chiamata a AfxOleInit(), deve essere effettuata
	una sola volta all'interno del processo, altrimenti da errore (ad esempio un applicazione basata
	su dialogo che lancia un dialogo modale che a sua volta usa la classe). Se invece lo stesso processo 
	ha piu' threads che necessitano usare il codice, ogni thread deve effettuare una nuova chiamata a 
	AfxOleInit(). In altre parole, non piu' di una chiamata per thread e una chiamata per ogni thread.
*/
BOOL COleDropTargetEx::Attach(CWnd* pWnd,UINT nMsg,BOOL bForceInit/* = FALSE*/)
{
	// la variabile statica viene condivisa da tutte le istanze della classe nello stesso processo
	BOOL bAttached = g_bAfxOleInitialized;

	// i differenti threads devono impostare il flag per forzare la chiamata e verificare in proprio di usare una sola istanza della classe
	// nota: se si tratta di dialoghi creati con threads UI, al trattarsi di nuovi threads devono forzare la chiamata, non cosi' per i
	// dialoghi modali di un processo che abbia gia' inizializzato OLE
	if(!g_bAfxOleInitialized)
	{
		g_bAfxOleInitialized = bAttached = AfxOleInit();
		TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"COleDropTargetEx::Attach(): AfxOleInit()=%s\n",g_bAfxOleInitialized ? "ok" : "failed"));
	}
	else
	{
		bAttached = bForceInit ? AfxOleInit() : TRUE;
		TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"COleDropTargetEx::Attach(): AfxOleInit()=already initialized\n"));
	}
	
	if(bAttached)
	{
		m_pWnd = pWnd;
		m_nMsg = nMsg;

		if(m_pWnd)
		{
			bAttached = COleDropTarget::Register(m_pWnd);
			TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"COleDropTargetEx::Register(): %s\n",bAttached ? "ok" : "failed"));
		}
	}

	return(bAttached);
}

/*
	Detach()
*/
void COleDropTargetEx::Detach(void)
{
	if(m_pWnd)
	{
		m_pWnd = NULL;
		COleDropTarget::Revoke();
		TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"COleDropTargetEx::Revoke()\n"));
	}
}

/*
	AddClipboardFormat()
*/
UINT COleDropTargetEx::AddClipboardFormat(LPCSTR lpcszClipboardFormat)
{
	UINT id = 0L;

	if((id = ::RegisterClipboardFormat(lpcszClipboardFormat))!=0L)
	{
		CLIPBOARDFORMAT* c = (CLIPBOARDFORMAT*)m_listClipboardFormats.Add();
		if(c)
		{
			c->id = id;
			strcpyn(c->format,lpcszClipboardFormat,MAX_FORMAT+1);
		}
	}

	return(id);
}

/*
	CheckForRegisteredFormats()
*/
DROPEFFECT COleDropTargetEx::CheckForRegisteredFormats(COleDataObject* pDataObject,UINT* id/*=NULL*/)
{
	BOOL bEnable = FALSE;
	if(id)
		*id = 0;

	if(pDataObject)
	{
		ITERATOR iter;
		CLIPBOARDFORMAT* c;
		
		if((iter = m_listClipboardFormats.First())!=(ITERATOR)NULL)
		{
			do
			{
				c = (CLIPBOARDFORMAT*)iter->data;

				if(c->id > 0)
					if((bEnable = pDataObject->IsDataAvailable((CLIPFORMAT)c->id))!=FALSE)
					{
						if(id)
							*id = c->id;
						break;
					}

				iter = m_listClipboardFormats.Next(iter);
			
			} while(iter!=(ITERATOR)NULL);
		}
	}

	return(bEnable ? m_defaultDropEffect : DROPEFFECT_NONE);
}

/*
	OnDragEnter()
*/
DROPEFFECT COleDropTargetEx::OnDragEnter(CWnd* pWnd,COleDataObject* pDataObject,DWORD/*dwKeyState*/,CPoint/*point*/)
{
	TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"COleDropTargetEx::OnDragEnter(): this=%ld, over=%ld\n",m_pWnd->m_hWnd,pWnd->m_hWnd));
	return(m_bEnabled ? CheckForRegisteredFormats(pDataObject) : DROPEFFECT_NONE);
}

/*
	OnDragOver()
*/	
DROPEFFECT COleDropTargetEx::OnDragOver(CWnd* pWnd,COleDataObject* pDataObject,DWORD /*dwKeyState*/,CPoint /*point*/)
{
	TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"COleDropTargetEx::OnDragOver(): this=%ld, over=%ld\n",m_pWnd->m_hWnd,pWnd->m_hWnd));
	return(m_bEnabled ? CheckForRegisteredFormats(pDataObject) : DROPEFFECT_NONE);
}

/*
	OnDrop()
*/
BOOL COleDropTargetEx::OnDrop(CWnd* /*pWnd*/,COleDataObject* pDataObject,DROPEFFECT/*dropEffect*/,CPoint/*point*/)
{
	BOOL bDropped = FALSE;
	UINT id = 0L;

	if(CheckForRegisteredFormats(pDataObject,&id)!=DROPEFFECT_NONE)
	{
		STGMEDIUM	sm = {0};
		sm.tymed = TYMED_HGLOBAL;

		if(id > 0L)
			bDropped = pDataObject->GetData((CLIPFORMAT)id,&sm);
		
		if(bDropped)
		{
			memset(m_szOleData,'\0',sizeof(m_szOleData));
			lstrcpyn(m_szOleData,(LPCSTR)::GlobalLock(sm.hGlobal),sizeof(m_szOleData));
			::GlobalFree(sm.hGlobal);
				
			if(m_pWnd && m_nMsg!=0L)
				::PostMessage(m_pWnd->m_hWnd,m_nMsg,0L,(LPARAM)m_szOleData);
		}
	}

	return(bDropped);
}

/*
	COleDropSourceEx()
*/
COleDropSourceEx::COleDropSourceEx()
{
	m_dropEffect = DROPEFFECT_NONE;
}

/*
	GiveFeedback()
*/
SCODE COleDropSourceEx::GiveFeedback(DROPEFFECT dropEffect)
{
#ifdef _DEBUG
	
	if(dropEffect==DROPEFFECT_NONE)
	{
		TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"COleDropSourceEx::GiveFeedback(): %s\n","DROPEFFECT_NONE"));
	}
	else if(dropEffect & DROPEFFECT_COPY)
	{
		TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"COleDropSourceEx::GiveFeedback(): %s\n","DROPEFFECT_COPY"));
	}
	else if(dropEffect & DROPEFFECT_MOVE)
	{
		TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"COleDropSourceEx::GiveFeedback(): %s\n","DROPEFFECT_MOVE"));
	}
	else if(dropEffect & DROPEFFECT_LINK)
	{
		TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"COleDropSourceEx::GiveFeedback(): %s\n","DROPEFFECT_LINK"));
	}
	else if(dropEffect & DROPEFFECT_SCROLL)
	{
		TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"COleDropSourceEx::GiveFeedback(): %s\n","DROPEFFECT_SCROLL"));
	}
	else
	{
		TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"COleDropSourceEx::GiveFeedback(): %ld (unknown)\n",(DWORD)dropEffect));
	}
#endif

	m_dropEffect = dropEffect;

	// COleDropSource: don't change the cursor until drag is officially started
	return(m_bDragStarted ? DRAGDROP_S_USEDEFAULTCURSORS : S_OK);
}

/*
	GetDropEffect()
*/
DROPEFFECT COleDropSourceEx::GetDropEffect(void)
{
	return(m_dropEffect);
}
