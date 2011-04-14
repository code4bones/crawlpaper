/*
	CDialogEx.cpp
	Classe derivata per la finestra di dialogo (MFC).
	Luca Piergentili, 30/08/00
	lpiergentili@yahoo.com

	Alcune parti di codice relative al menu esteso e al ridimensionamento sono tratte
	dagli esempi relativi a BCMenu (Brent Corkum) e ResizableLib (Paolo Messina).
	L'attaccamento automatico al bordo dello schermo e' tratto dal codice di François Gagné.
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include <stdio.h>
#include "strings.h"
#include "window.h"
#include "win32api.h"
#include "CSync.h"
#include "CToolBarCtrlEx.h"
#include "CStatusBarCtrlEx.h"
#include "CToolTipCtrlEx.h"
#include "CPPTooltip.h"
#include "CDialogEx.h"
// ridimensionamento
#ifdef _RESIZABLE_DIALOG_STYLE
  #include "Resizable.h"
  #include "ResizableLayout.h"
  #include "ResizableGrip.h"
  #include "ResizableMinMax.h"
  #include "ResizableState.h"
#endif

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

BEGIN_MESSAGE_MAP(CXDialogEx,CWnd)
	ON_WM_DESTROY()
	ON_MESSAGE(WM_COPYDATA,OnCopyData)
END_MESSAGE_MAP()

/*
	CXDialogEx()
*/
CXDialogEx::CXDialogEx(LPCSTR lpcszUniqueName,CDialogEx* pDialogEx,CWnd* pWnd)
{
	// puntatore a CDialogEx per la callback
	m_pDialogEx = pDialogEx;

	// crea la finestra interna
	CWnd::CreateEx(0,AfxRegisterWndClass(CS_BYTEALIGNCLIENT|CS_BYTEALIGNWINDOW,0,0,0),lpcszUniqueName,0,1,1,1,1,(pWnd ? pWnd->m_hWnd : NULL),NULL,NULL);
}

/*
	~CXDialogEx()
*/
CXDialogEx::~CXDialogEx()
{
	// per la derivazione da CWnd
	DestroyWindow();
}

/*
	OnDestroy()
*/
void CXDialogEx::OnDestroy(void)
{
	CWnd::OnDestroy();
}

/*
	OnCopyData()

	Callback per il messaggio WM_COPYDATA, inviato dalla OnInitDialog() della nuova istanza per passare all'istanza
	in corso la linea di comando.
	Restituendo 0 l'istanza in corso impedisce l'esecuzione della nuova istanza, con 1 permette l'esecuzione della
	nuova istanza.
	La callback viene chiamata se e' stata specificato TRUE per il flag relativo all'esecuzione delle istanze multiple
	al costruttore di CDialogEx. In ogni caso l'istanza in corso puo' sempre decidere, a seconda del valore di ritorno
	di OnInitDialogEx(), se farsi carico della linea di comando della nuova istanza e terminarla o permettere che la
	nuova istanza venga eseguita. Se la OnInitDialogEx() dell'istanza in corso restituisce TRUE, la nuova istanza viene
	terminata, se restituisce FALSE, viene eseguita.
*/
LONG CXDialogEx::OnCopyData(UINT wParam,LONG lParam)
{
	LONG lRet = 0L;
	UINT nInstanceCount = (UINT)wParam; // numero istanza
	COPYDATASTRUCT* pCopydatastruct = (COPYDATASTRUCT*)lParam;
	LPSTR lpCommandLine = (LPSTR)pCopydatastruct->lpData;

	if(lpCommandLine)
	{
		// chiama la OnInitDialogEx() dell'istanza in corso con la linea di comando della nuova
		char* pCommandLine = new char[_MAX_PATH+1];
		if(pCommandLine)
		{
			memset(pCommandLine,'\0',_MAX_PATH+1);
			memcpy(pCommandLine,lpCommandLine,pCopydatastruct->cbData >= _MAX_PATH+1 ? _MAX_PATH+1-1 : pCopydatastruct->cbData);
			lRet = (LONG)!m_pDialogEx->OnInitDialogEx(nInstanceCount,pCommandLine);
			delete [] pCommandLine;
		}
	}

	return(lRet);
}

BEGIN_MESSAGE_MAP(CDialogEx,CDialogBaseClass)
	ON_WM_WINDOWPOSCHANGING()					// visibilita'
	ON_WM_LBUTTONDOWN()							// spostamento del dialogo con un click sull'area client
#ifdef _RESIZABLE_DIALOG_STYLE					// resize
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
#endif
	ON_WM_PAINT()								// disegno
	ON_WM_QUERYDRAGICON()						// icona
	ON_MESSAGE(WM_DROPFILES,OnDragDrop)			// drag and drop
	ON_NOTIFY_EX(TTN_NEEDTEXT,0,OnToolbarToolTip)	// tooltip per la toolbar
	ON_BN_CLICKED(IDCANCEL,OnExit)				// uscita
END_MESSAGE_MAP()

IMPLEMENT_DYNCREATE(CDialogEx,CDialogBaseClass)

/*
	CDialogEx()

	La gestione delle istanze multiple avviene nel seguente modo:

	- se il flag relativo viene passato a FALSE, la OnInitDialog() restituisce FALSE, indicando al
	  chiamante che il dialogo deve essere chiuso
	- se il flag relativo viene passato a TRUE, la OnInitDialog() restituisce il valore ritornato
	  dalla callback della finestra interna
	  tale valore e' quello restituito dalla (virtuale) OnInitDialogEx() dell'istanza in corso:
	  per TRUE la OnInitDialog() restituisce FALSE, indicando al chiamante che il dialogo deve
	  essere chiuso, per FALSE la OnInitDialog() restituisce TRUE, indicando al chiamante che il
	  dialogo deve essere aperto (per dialogo si intende la nuova istanza)

	in altre parole, l'istanza in corso, anche se viene passato il flag per le istanze multiple a
	TRUE, puo' sempre decidere se permettere l'esecuzione della nuova istanza (facendo restituire
	FALSE alla OnInitDialogEx()) o se farsi carico dei parametri passati alla nuova istanza e
	impedirne l'esecuzione (facendo restituire TRUE alla OnInitDialogEx())
*/
CDialogEx::CDialogEx(	UINT		nTemplateID,
					UINT		nToolbarID/* = 0L */,
					UINT		nStatusbarID/* = 0L */,
					UINT		nIconID/* = 0L */,
					UINT		nMenuID/* = 0L */,
#ifdef _RESIZABLE_DIALOG_STYLE
					BOOL		bAllowResize/* = FALSE*/,
#endif
					BOOL		bAllowSnap/* = FALSE*/,
					BOOL		bAllowDragAndDrop/* = FALSE */,
					LPCSTR	lpcszUniqueName/* = NULL */,
					BOOL		bAllowMultipleInstances/* = FALSE */,
					HWND		hWndParent/* = NULL */) : CDialogBaseClass(nTemplateID,CWnd::FromHandle(hWndParent))
{
	// per evitare che la zoccola di MFC visualizzi il dialogo quando decide lei, il flag
	// relativo deve venir impostato a FALSE nel costruttore della derivata con SetVisible()
	m_bVisible = TRUE;

	// titolo
	memset(m_szWindowTitle,'\0',sizeof(m_szWindowTitle));
	
	// codice d'uscita
	m_nExitCode = IDOK;

	// combinazioni Alt+<lettera> da intercettare definite dal chiamante
	memset(m_szAltKeyChars,'\0',sizeof(m_szAltKeyChars));

	// icona
	m_nIconID = nIconID;
	m_hIconSmall = m_hIconLarge = (HICON)NULL;
	
	// menu
	m_nMenuID = nMenuID;

	// toolbar
	m_nToolbarID = nToolbarID;

	// status bar
	m_nStatusbarID = nStatusbarID;

	// resize
#ifdef _RESIZABLE_DIALOG_STYLE
	m_bUseResizeStyle = bAllowResize;
	m_bAllowMinTrackSize = TRUE;
	m_bEnableSaveRestore = FALSE;
	m_dwGripTempState = 1;
#endif

	// snap to screen
	m_bUseSnapToScreen = bAllowSnap;
	m_nSnapYOffset = m_nSnapXOffset = 20;

	// drag and drop
	m_bAllowDragAndDrop = bAllowDragAndDrop;
	m_nDropCount = 0;

	// imposta il nome unico per il mutex/finestra interna
	if(lpcszUniqueName)
		strcpyn(m_szUniqueName,lpcszUniqueName,sizeof(m_szUniqueName));
	else
	{	
		// il nome deve sempre essere lo stesso a fronte del medesimo processo
		// se non viene passato un nome, assume il nome del file del processo corrente, anche se cio' non
		// garantisce la univocita', dato che il processo puo' essere lanciato da percorsi differenti
		::GetModuleFileName(NULL,m_szUniqueName,sizeof(m_szUniqueName));
		
		// dato che il nome del mutex segue le regole dei nomi file, elimina i caratteri non validi
		int n = strlen(m_szUniqueName);
		for(int i = 0; i < n; i++)
			if(m_szUniqueName[i]=='\\' || m_szUniqueName[i]==':')
				m_szUniqueName[i] = '_';
	}
	
	// mutex per le istanze multiple
	m_pMutexInstance = new CSyncProcesses(m_szUniqueName);
	m_nInstanceCount = 1;
	m_bAllowMultipleInstances = bAllowMultipleInstances;
	
	// finestra interna
	m_pWndInternal = NULL;

	// toolbar
	m_pToolBar = NULL;
	
	// statusbar
	m_pStatusBar = NULL;
	
	// tooltip
	m_pTooltip = NULL;
	m_pTooltipEx = NULL;
	m_pListTooltips = NULL;
}

/*
	~CDialogEx()
*/
CDialogEx::~CDialogEx()
{
	// icone
	if(m_hIconSmall)
		::DestroyIcon(m_hIconSmall),m_hIconSmall = NULL;
	if(m_hIconLarge)
		::DestroyIcon(m_hIconLarge),m_hIconLarge = NULL;

	// elimina il mutex per l'istanza
	if(m_pMutexInstance)
		delete m_pMutexInstance,m_pMutexInstance = NULL;

	// elimina la finestra interna
	if(m_pWndInternal)
		delete m_pWndInternal,m_pWndInternal = NULL;

	// elimina le aggiunte
	if(m_pToolBar)
		delete m_pToolBar,m_pToolBar = NULL;
	if(m_pStatusBar)
		delete m_pStatusBar,m_pStatusBar = NULL;
	if(m_pTooltip)
		 delete m_pTooltip,m_pTooltip = NULL;
	if(m_pTooltipEx)
		 delete m_pTooltipEx,m_pTooltipEx = NULL;
	if(m_pListTooltips)
		 delete m_pListTooltips,m_pListTooltips = NULL;
}

/*
	OnWindowPosChanging()
	
	Per evitare che la zoccola di MFC visualizzi il dialogo quando decide lei (visibilita')
	e per i giochetti sul riallineamento della finestra ai bordi dello schermo.
*/
void CDialogEx::OnWindowPosChanging(WINDOWPOS FAR* lpWndPos) 
{
	// visibilita'
	if(!m_bVisible)
		lpWndPos->flags &= ~SWP_SHOWWINDOW;

	// per attaccarsi automaticamente ai bordi dello schermo ed impedire l'uscita della finestra
	// dall'area utile dello schermo (includendo la taskbar)
	// modificato dall'originale di François Gagné (http://www.codeproject.com\dialog\snapdialog.asp)
	if(m_bUseSnapToScreen)
	{
		// calcola le coordinate dello schermo e della taskbar
		TASKBARPOS tbi;
		::GetTaskBarPos(&tbi);

		if(tbi.hWnd)
		{
			// area occupata dalla finestra e dimensioni
			CRect rcThisWindow(0,0,0,0);
			GetWindowRect(&rcThisWindow);
			int nThisWindowWidth = rcThisWindow.right - rcThisWindow.left;
			int nThisWindowHeight = rcThisWindow.bottom - rcThisWindow.top;

			// ricava il posizionamento della taskbar
			CRect rcTaskbar(0,0,0,0);
			CRect rcScreen(0,0,tbi.nScreenWidth,tbi.nScreenHeight);
	
			// top
			if(tbi.rc.top <= 0 && tbi.rc.left <= 0 && tbi.rc.right >= rcScreen.right)
			{	
				rcTaskbar.top = tbi.rc.bottom - tbi.rc.top;
			}
			// bottom
			else if(tbi.rc.top > 0 && tbi.rc.left <= 0)
			{
				rcTaskbar.bottom = tbi.rc.bottom - tbi.rc.top;
			}
			// right
			else if(tbi.rc.top <= 0 && tbi.rc.left > 0)
			{
				rcTaskbar.right = tbi.rc.right - tbi.rc.left;
			}
			// left
			else
			{
				rcTaskbar.left = tbi.rc.right - tbi.rc.left;
			}

			// (re)imposta le coordinate (x,y) della finestra per riposizionarla allineata al bordo (left,top,right,bottom) dello schermo
			//
			// snap to left border
			if(lpWndPos->x >= -m_nSnapXOffset + rcTaskbar.left && lpWndPos->x <= rcTaskbar.left + m_nSnapXOffset)
			{
				lpWndPos->x = rcTaskbar.left;
			}
			// snap to top border
			if(lpWndPos->y >= -m_nSnapYOffset && lpWndPos->y <= rcTaskbar.top + m_nSnapYOffset) {
				lpWndPos->y = rcTaskbar.top;
			}
			// snap to right border
			if(lpWndPos->x + nThisWindowWidth <= rcScreen.right - rcTaskbar.right + m_nSnapXOffset && lpWndPos->x + nThisWindowWidth >= rcScreen.right - rcTaskbar.right - m_nSnapXOffset) {
				lpWndPos->x = rcScreen.right - rcTaskbar.right - nThisWindowWidth;
			}
			// snap tp bottom border
			if( lpWndPos->y + nThisWindowHeight <= rcScreen.bottom - rcTaskbar.bottom + m_nSnapYOffset && lpWndPos->y + nThisWindowHeight >= rcScreen.bottom - rcTaskbar.bottom - m_nSnapYOffset) {
				lpWndPos->y = rcScreen.bottom - rcTaskbar.bottom - nThisWindowHeight;
			}

			// (re)imposta le coordinate (x,y) della finestra per evitare che venga portata fuori dall'area dello schermo
			// la finestra viene (ri)allineata tenendo in considerazione lo spazio occupato dalla taskbar
			// se la taskbar e' occultata, fa fare un 'saltino' (pari al gap specificato per l'allineamento) in corrispondenza
			// di dove dovrebbe iniziare l'area occupata dalla taskbar (al momento non visibile) continuando poi fino al bordo
			//
			// per il 'saltino'
			if(rcTaskbar.right-rcTaskbar.left < 0)
				rcTaskbar.left = 0;
			// (ri)allinea al bordo a sinistra
			if(lpWndPos->x < rcScreen.left+rcTaskbar.left)
				lpWndPos->x = rcScreen.left+rcTaskbar.left;
			
			// per il 'saltino'
			if(rcTaskbar.bottom-rcTaskbar.top < 0)
				rcTaskbar.top = 0;
			// (ri)allinea al bordo in alto
			if(lpWndPos->y < rcScreen.top+rcTaskbar.top)
				lpWndPos->y = rcScreen.top+rcTaskbar.top;

			// per il 'saltino'
			if(rcTaskbar.left-rcTaskbar.right < 0)
				rcTaskbar.right = 0;
			// (ri)allinea al bordo a destra
			if(lpWndPos->x + lpWndPos->cx > rcScreen.right-rcTaskbar.right)
			{
				int n = (lpWndPos->x + lpWndPos->cx) - (rcScreen.right-rcTaskbar.right);
				lpWndPos->x -= n;
			}

			// per il 'saltino'
			if(rcTaskbar.top-rcTaskbar.bottom < 0)
				rcTaskbar.bottom = 0;
			// (ri)allinea al bordo in basso
			if(lpWndPos->y + lpWndPos->cy > rcScreen.bottom-rcTaskbar.bottom)
			{
				int n = (lpWndPos->y + lpWndPos->cy) - (rcScreen.bottom-rcTaskbar.bottom);
				lpWndPos->y -= n;
			}
		}
	}
	
	CDialogBaseClass::OnWindowPosChanging(lpWndPos);
}

/*
	OnPaint()

	Visualizzazione del dialogo.
*/
void CDialogEx::OnPaint(void)
{
	if(CWnd::IsIconic())
	{
		CPaintDC dc(this);
		CWnd::SendMessage(WM_ICONERASEBKGND,(WPARAM)dc.GetSafeHdc(),0);
		
		CRect rect;
		CWnd::GetClientRect(&rect);

		int cxIcon = ::GetSystemMetrics(SM_CXICON);
		int cyIcon = ::GetSystemMetrics(SM_CYICON);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x,y,m_hIconSmall);
	}
	else
		CDialogBaseClass::OnPaint();
}

/*
	OnQueryDragIcon()

	Restituisce l'handle relativo all'icona di default.
*/
HCURSOR CDialogEx::OnQueryDragIcon(void)
{
	return((HCURSOR)m_hIconSmall);
}

/*
	OnInitDialog()

	Inizializzazione del dialogo.
	Il chiamante, se redefinisce la virtuale, deve chiamare la classe base all'inizio e controllare
	il codice di ritorno prima di continuare (per FALSE chiudere il dialogo, per TRUE aprirlo).
*/
BOOL CDialogEx::OnInitDialog(void)
{
	// chiama il gestore originale
	CDialogBaseClass::OnInitDialog();

	// prova a bloccare il mutex per verificare se esistono altre istanze in esecuzione
	if(!m_pMutexInstance->Lock(SYNC_1_SECS_TIMEOUT))
		m_nInstanceCount++;

	// se al caricamento gia' esiste un istanza in esecuzione, a seconda del flag, invia il messaggio
	// con la linea di comando all'istanza in esecuzione o termina
	// il valore restituito dalla callback dell'istanza in esecuzione determina se viene lanciata una
	// nuova istanza (TRUE) o meno (FALSE)
	if(m_nInstanceCount > 1)
	{
		// permette piu' istanze, la callback (attraverso il valore di ritorno della OnInitDialogEx())
		// decide se permettere o meno l'esecuzione della nuova istanza
		if(m_bAllowMultipleInstances)
		{
			// cerca la finestra dell'istanza gia' in esecuzione
			HWND hWnd = ::FindWindow(NULL,m_szUniqueName);
			if(hWnd)
			{
				COPYDATASTRUCT copydatastruct;
				copydatastruct.dwData = 0L;
				strcpyn(m_szCommandLine,::GetCommandLine(),sizeof(m_szCommandLine));
				copydatastruct.cbData = strlen(m_szCommandLine);
				copydatastruct.lpData = m_szCommandLine;

				// invia la linea di comando all'istanza gia' in esecuzione
				if(::SendMessage(hWnd,WM_COPYDATA,(WPARAM)m_nInstanceCount,(LPARAM)&copydatastruct)==0L)
					return(FALSE);
			}
			else
				return(FALSE);
		}
		else // solo un istanza alla volta, termina
		{
			CString str;
			str.Format("%s already running, only one instance at time.",m_pMutexInstance->GetName());
			::MessageBox(NULL,str,m_pMutexInstance->GetName(),MB_OK|MB_ICONWARNING|MB_TASKMODAL|MB_SETFOREGROUND|MB_TOPMOST);
			return(FALSE);
		}
	}

	// crea la finestra interna per la gestione delle istanze multiple
	m_pWndInternal = new CXDialogEx(m_szUniqueName,this,AfxGetMainWnd());

	// imposta l'icona
	this->SetIcon(m_nIconID);

	// carica il menu
	this->SetMenu(m_nMenuID);
	
	// notifica al sistema la capacita' di ricevere files via drag & drop
	::DragAcceptFiles(this->m_hWnd,m_bAllowDragAndDrop);

	return(TRUE);
}

/*
	OnExit()

	Termina il dialogo con un codice di ritorno.
*/
void CDialogEx::OnExit(UINT nExitCode)
{
	m_nExitCode = nExitCode;
	CDialogEx::OnExit();
}

/*
	OnExit()

	Termina il dialogo.
*/
void CDialogEx::OnExit(void)
{
	// sblocca il mutex per l'istanza (non eliminare l'oggetto qui ma nel distruttore)
	if(m_nInstanceCount <= 1)
	{
		m_pMutexInstance->Unlock();
		m_nInstanceCount = 1;
	}

	// elimina la finestra interna
	if(m_pWndInternal)
		delete m_pWndInternal,m_pWndInternal = NULL;

	// elimina le aggiunte
	if(m_pToolBar)
		delete m_pToolBar,m_pToolBar = NULL;
	if(m_pStatusBar)
		delete m_pStatusBar,m_pStatusBar = NULL;
	if(m_pTooltip)
		 delete m_pTooltip,m_pTooltip = NULL;
	if(m_pTooltipEx)
		 delete m_pTooltipEx,m_pTooltipEx = NULL;
	if(m_pListTooltips)
		 delete m_pListTooltips,m_pListTooltips = NULL;


	// chiama il gestore originale
	CDialogBaseClass::EndDialog(m_nExitCode);
}

/*
	OnDragDrop()

	Carica i files/directory ricevuti tramite drag & drop.
*/
LONG CDialogEx::OnDragDrop(UINT wParam,LONG /*lParam*/)
{
	HDROP hDrop = (HDROP)wParam;

	// ricava il totale dei file droppati
	m_nDropCount = ::DragQueryFile(hDrop,(UINT)0xFFFFFFFF,NULL,0);

	// azzera la lista per i nomi dei files
	m_listDroppedFiles.RemoveAll();

	// copia i nomi dei files nella lista
	FILELIST* f;
	for(int i = 0; i < (int)m_nDropCount; i++)
	{
		if((f = (FILELIST*)m_listDroppedFiles.Add())!=(FILELIST*)NULL)
		{
			::DragQueryFile(hDrop,i,f->file,_MAX_PATH+1);
			f->attribute = ::GetFileAttributes(f->file);
		}
	}
	
	// notifica alla shell il termine della query
	::DragFinish(hDrop);

	OnDropFiles();
	
	return(0L);
}

/*
	GetDroppedCount()

	Restituisce il numero di files droppati.
*/
UINT CDialogEx::GetDroppedCount(void)
{
	return(m_nDropCount);
}

/*
	GetDroppedFile()

	Restituisce i nomi dei files droppati.
*/
BOOL CDialogEx::GetDroppedFile(LPSTR lpszFileName,UINT nSize,DWORD* pdwAttribute)
{
	static BOOL bFirstCall = TRUE;
	BOOL bDropped = FALSE;

	// prima chiamata
	if(bFirstCall)
		bFirstCall = FALSE;

	// chiamate successive (restituisce quanto presente nella lista)
	if(!bFirstCall)
	{
		static int nCount = -1;

		// controlla che la lista contenga elementi
		if(m_listDroppedFiles.Count() > 0)
		{
			if(nCount==-1)
				nCount = 0;
			
			// ricava il nome del file
			FILELIST* f;
			if((f = (FILELIST*)m_listDroppedFiles.GetAt(nCount))!=(FILELIST*)NULL)
			{
				strcpyn(lpszFileName,f->file,nSize);
				*pdwAttribute = f->attribute;
				nCount++;
				bDropped = TRUE;
			}
			else
			{
				m_listDroppedFiles.RemoveAll();
				bFirstCall = TRUE;
				nCount = -1;
			}
		}
		else
		{
			m_listDroppedFiles.RemoveAll();
			bFirstCall = TRUE;
			nCount = -1;
		}
	}

	return(bDropped);
}

/*
	PreTranslateMessage()

	Intercetta la pressione dei tasti.
*/
BOOL CDialogEx::PreTranslateMessage(MSG* msg)
{
	if(m_pTooltipEx)
		m_pTooltipEx->RelayEvent(msg);

	BOOL bTranslated = FALSE;
	BOOL bShift = ::GetKeyState(VK_SHIFT) < 0;
	BOOL bCtrl = ::GetKeyState(VK_CONTROL) < 0;
	BOOL bAlt = ::GetKeyState(VK_MENU) < 0;

	if(msg->message==WM_SYSKEYDOWN)
	{
		if(0x20000000 & msg->lParam)
		{
			switch(msg->wParam)
			{
				case VK_F1:
				case VK_F2:
				case VK_F3:
				case VK_F4:
				case VK_F5:
				case VK_F6:
				case VK_F7:
				case VK_F8:
				case VK_F9:
				case VK_F10:
				case VK_F11:
				case VK_F12:
					bTranslated = OnAltFunctionKey(msg->wParam);
					break;
				default:
					if(strchr(m_szAltKeyChars,(char)msg->wParam))
						bTranslated = OnVirtualKey(msg->wParam,bShift,bCtrl,bAlt);
					break;
			}
		}
		else
		{
			//if(msg->wParam==VK_F10) perche' solo per F10 ???
				bTranslated = OnFunctionKey(msg->wParam,bShift,bCtrl);
		}
	}
	else if(msg->message==WM_KEYDOWN)
	{
		switch(msg->wParam)
		{
			case VK_RETURN:
				bTranslated = OnReturnKey();
				break;
			case VK_ESCAPE:
				bTranslated = OnEscapeKey();
				break;
			case VK_F1:
			case VK_F2:
			case VK_F3:
			case VK_F4:
			case VK_F5:
			case VK_F6:
			case VK_F7:
			case VK_F8:
			case VK_F9:
			case VK_F10:
			case VK_F11:
			case VK_F12:
				bTranslated = OnFunctionKey(msg->wParam,bShift,bCtrl);
				break;
			default:
				bTranslated = OnVirtualKey(msg->wParam,bShift,bCtrl,bAlt);
				break;
		}
	}
	
	if(!bTranslated)
		bTranslated = CDialogBaseClass::PreTranslateMessage(msg);
	
	return(bTranslated);
}

/*
	OnLButtonDown()

	Intercetta la pressione del tasto sinistro del mouse per permettere lo spostamento del dialogo
	cliccando su una qualsiasi area (non solo tramite il click sulla barra del titolo).
	Occhio che si intende sempre e solo il click su un area non occupata da un controllo.
*/
void CDialogEx::OnLButtonDown(UINT nFlags,CPoint point)
{
	// classe base
	CDialogBaseClass::OnLButtonDown(nFlags,point);
	
	// cambia il cursore e muove
	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEALL));
	SendMessage(WM_NCLBUTTONDOWN,(WPARAM)HTCAPTION,MAKELPARAM(point.x,point.y));
	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	PostMessage(WM_LBUTTONUP);
}

/*
	SetIcon()

	Imposta l'icona per il dialogo.
*/
void CDialogEx::SetIcon(UINT nIconID)
{
	if(nIconID > 0L)
	{
		if(m_hIconSmall)
			::DestroyIcon(m_hIconSmall),m_hIconSmall = NULL;
		m_hIconSmall = (HICON)::LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(nIconID),IMAGE_ICON,16,16,LR_DEFAULTCOLOR);
		if(m_hIconSmall)
			CWnd::SetIcon(m_hIconSmall,FALSE);

		if(m_hIconLarge)
			::DestroyIcon(m_hIconLarge),m_hIconLarge = NULL;
		m_hIconLarge = (HICON)::LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(nIconID),IMAGE_ICON,32,32,LR_DEFAULTCOLOR);
		if(m_hIconLarge)
			CWnd::SetIcon(m_hIconLarge,TRUE);
	}
}

/*
	SetIcon()

	Imposta l'icona per il dialogo.
*/
void CDialogEx::SetIcon(LPCSTR lpcszFileName)
{
	if(lpcszFileName)
	{
		HICON hIcon;
		
		hIcon = (HICON)::LoadImage(NULL,lpcszFileName,IMAGE_ICON,16,16,LR_LOADFROMFILE);
		if(hIcon)
		{
			if(m_hIconSmall)
				::DestroyIcon(m_hIconSmall),m_hIconSmall = hIcon;
			CWnd::SetIcon(m_hIconSmall,FALSE);
		}
		
		hIcon = (HICON)::LoadImage(NULL,lpcszFileName,IMAGE_ICON,32,32,LR_LOADFROMFILE);
		if(hIcon)
		{
			if(m_hIconLarge)
				::DestroyIcon(m_hIconLarge),m_hIconLarge = hIcon;
			CWnd::SetIcon(m_hIconLarge,TRUE);
		}
		
		/*{
			SHFILEINFO sh = {0};
			::SHGetFileInfo(lpcszFileName,0,&sh,sizeof(sh),SHGFI_ICON|SHGFI_LARGEICON);
			if(sh.hIcon)
				CWnd::SetIcon(sh.hIcon,TRUE);
		}*/
	}
}

/*
	SetMenu()

	Carica il menu del dialogo.
*/
void CDialogEx::SetMenu(UINT nMenuID)
{
	if(nMenuID > 0L)
	{
		CMenu menu;
		menu.LoadMenu(nMenuID);
		CWnd::SetMenu(&menu);
		CWnd::DrawMenuBar();
		menu.Detach();
	}
}

/*
	OnCreate()
*/
#ifdef _RESIZABLE_DIALOG_STYLE
int CDialogEx::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	// classe base
	if(CDialogBaseClass::OnCreate(lpCreateStruct)==-1)
		return(-1);

	// se e' stato abilitato il ridimensionamento
	if(m_bUseResizeStyle)
	{
		// child dialogs don't want resizable border or size grip,
		// nor they can handle the min/max size constraints
		BOOL bChild = GetStyle() & WS_CHILD;

		if(!bChild)
		{
			// keep client area
			CRect rect;
			GetClientRect(&rect);
			
			// set resizable style
			ModifyStyle(DS_MODALFRAME,WS_POPUP|WS_THICKFRAME);
			
			// adjust size to reflect new style
			CMenu* pMenu = NULL;
			pMenu = GetMenu();

			::AdjustWindowRectEx(&rect,GetStyle(),::IsMenu(pMenu->GetSafeHmenu()),GetExStyle());

			SetWindowPos(NULL,0,0,rect.Width(),rect.Height(),SWP_FRAMECHANGED|SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOREPOSITION);

			// set the initial size as the min track size
			if(m_bAllowMinTrackSize)
				SetMinTrackSize(rect.Size());
			else
				ResetMinTrackSize();
		}

		// create and init the size-grip
		if(!CreateSizeGrip(!bChild))
			return(-1);
	}

	return(0);
}
#endif

/*
	OnDestroy()
*/
#ifdef _RESIZABLE_DIALOG_STYLE
void CDialogEx::OnDestroy(void)
{
	// se e' stato abilitato il ridimensionamento
	if(m_bUseResizeStyle)
	{
		if(m_bEnableSaveRestore)
			SaveWindowRect(m_strSection,m_bRectOnly);

		// remove child windows
		RemoveAllAnchors();
	}

	// classe base
	CDialogBaseClass::OnDestroy();
}
#endif

/*
	OnSize()
*/
#ifdef _RESIZABLE_DIALOG_STYLE
void CDialogEx::OnSize(UINT nType,int cx,int cy) 
{
	// chiama il gestore originale
	CWnd::OnSize(nType,cx,cy);

	// se e' stato abilitato il ridimensionamento
	if(m_bUseResizeStyle)
	{
		// arrangement not needed
		if(nType==SIZE_MAXHIDE || nType==SIZE_MAXSHOW)
			return;

		// nasconde/visualizza il grip a seconda dello stato
		if(nType==SIZE_MAXIMIZED)
			HideSizeGrip(&m_dwGripTempState);
		else
			ShowSizeGrip(&m_dwGripTempState);

		// update grip and layout
		UpdateSizeGrip();
		ArrangeLayout();

		// per far si che i controlli che sforano durante durante il ridimensionamento della finestra
		// vengano nascosti/visualizzati automaticamente
		// calcola l'altezza dei controlli conosciuti (menu, toolbar e statusbar se presenti) e di quelli
		// specificati dal chiamante, se tale dimensione supera la dimensione attuale del dialogo nasconde
		// i controlli che non vengono eliminati automaticamente durante il ridimensionamento
		// se viene specificata una dimensione minima in teoria bisognerebbe controllare che tale dimensione
		// non sia inferiore a quella dei controlli, ma per adesso andiamo alla carlona...
		if(!IsMinTrackSizeEnabled())
		{
			CRect rect;
			int nTotHeight = 0;
			ITERATOR iter;
			IDLIST* id;
			CWnd* pWnd;
			
			// calcola l'altezza totale dei controlli specificati dal chiamante
			if((iter = m_listID.First())!=(ITERATOR)NULL)
			{
				do
				{
					id = (IDLIST*)iter->data;
				
					if(id)
					{
						// per aggiungere un gap arbitrario, passare un valore negativo
						if(id->id < 0)
						{
							nTotHeight += id->id;
						}
						else
						{
							if((pWnd = GetDlgItem(id->id))!=(CWnd*)NULL)
								if(::GetClientRect(pWnd->GetSafeHwnd(),&rect))
									nTotHeight += rect.bottom;
						}
					}

					iter = m_listID.Next(iter);
				
				} while(iter!=(ITERATOR)NULL);
			}

			// calcola l'altezza totale dei controlli conosciuti
			CMenu* pMenu = NULL;
			pMenu = GetMenu();

			if(pMenu->GetSafeHmenu())
			{
				::GetClientRect((HWND)pMenu->GetSafeHmenu(),&rect);
				nTotHeight += rect.bottom;
			}
			if(m_pToolBar)
			{
				m_pToolBar->GetClientRect(&rect);
				nTotHeight += rect.bottom;
			}
			if(m_pStatusBar)
			{
				m_pStatusBar->GetClientRect(&rect);
				nTotHeight += rect.bottom;
			}

			// ricava l'altezza totale del dialogo
			GetClientRect(&rect);
			
			// inserire qui la visualizzazione/rimozione dei controlli che sforano durante il ridimensionamento
			if(m_pToolBar)
			{
				if(nTotHeight > rect.bottom)
					 m_pToolBar->ShowWindow(SW_HIDE);
				else
					m_pToolBar->ShowWindow(SW_SHOW);
			}

			// alcuni controlli statici (le diciture di testo vicino ai campi) a volte non vengono ridisegnati o rimangono sporchi
			// penso che dipenda dalle modalita' per la GUI utilizzate da XP, comunque sia non avendo modo di provare su un altra
			// piattaforma per risolvere basta far ridisegnare il dialogo
			Invalidate(TRUE);
		}
	}
}
#endif

/*
	OnGetMinMaxInfo()
*/
#ifdef _RESIZABLE_DIALOG_STYLE
void CDialogEx::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	// se e' stato abilitato il ridimensionamento
	if(m_bUseResizeStyle)
		MinMaxInfo(lpMMI);
	else // classe base
		CWnd::OnGetMinMaxInfo(lpMMI);
}
#endif

/*
	EnableSaveRestore()
	
	NOTE: this must be called after setting the layout to have the dialog and its controls displayed properly
*/
#ifdef _RESIZABLE_DIALOG_STYLE
void CDialogEx::EnableSaveRestore(LPCTSTR pszSection, BOOL bRectOnly)
{
	m_strSection = pszSection;

	m_bEnableSaveRestore = TRUE;
	m_bRectOnly = bRectOnly;

	// restore immediately
	LoadWindowRect(pszSection, bRectOnly);
}
#endif

/*
	OnEraseBkgnd()
*/
#ifdef _RESIZABLE_DIALOG_STYLE
BOOL CDialogEx::OnEraseBkgnd(CDC* pDC) 
{
	// se e' stato abilitato il ridimensionamento
	if(m_bUseResizeStyle)
	{
		// Windows XP doesn't like clipping regions ...try this!
		EraseBackground(pDC);
		return(TRUE);

		//ClipChildren(pDC); // old-method (for safety)
		//return CDialogBaseClass::OnEraseBkgnd(pDC);
	}
	else // classe base
		return(CDialogBaseClass::OnEraseBkgnd(pDC));
}
#endif

/*
	AddToolBarButton()

	Aggiunge un bottone alla toolbar.
*/
BOOL CDialogEx::AddToolBarButton(UINT nButtonID,UINT nToolTipID,UINT nTextID,BYTE fsStatus)
{
	if(!m_pToolBar)
	{
		m_pToolBar = new CToolBarCtrlEx();
		if(m_pToolBar)
			m_pToolBar->Create(WS_CHILD|WS_VISIBLE|CCS_TOP|TBSTYLE_TOOLTIPS|TBSTYLE_FLAT/*|TBSTYLE_AUTOSIZE|TBSTYLE_WRAPABLE*/,this,m_nToolbarID);
	}

	return(m_pToolBar ? m_pToolBar->Add(nButtonID,nToolTipID,nTextID,fsStatus) : FALSE);
}

/*
	AddToolBarSeparator()

	Aggiunge un separatore tra i bottoni della toolbar.
*/
BOOL CDialogEx::AddToolBarSeparator(void)
{
	return(AddToolBarButton(0,0,0,TBSTATE_INDETERMINATE));
}

/*
	AttachToolBar()

	Collega la toolbar al controllo.
	Riceve gli id della toolbar, la dimensione (in pixel) del bottone (+2 rispetto al bitmap)
	e la dimensione (in pixel) del bitmap usato per il bottone (-2 rispetto al bottone).
*/
BOOL CDialogEx::AttachToolBar(const CSize& sizeButton,const CSize& sizeBitmap,UINT nToolBarID/*=(UINT)-1L*/,UINT nColdToolBarID/*=(UINT)-1L*/,UINT nHotToolBarID/*=(UINT)-1L*/)
{
	return(m_pToolBar ? m_pToolBar->Attach(sizeButton,sizeBitmap,nToolBarID,nColdToolBarID,nHotToolBarID) : FALSE);
}

/*
	EnableToolBarButton()

	Abilita (TRUE) o disabilita (FALSE) il bottone relativo all'id.
*/
void CDialogEx::EnableToolBarButton(UINT nButtonID,BOOL bState)
{
	if(m_pToolBar)
		m_pToolBar->EnableButton(nButtonID,bState);
}

/*
	IsToolBarButtonEnabled()

	Restituisce lo status del bottone relativo all'id.
*/
BOOL CDialogEx::IsToolBarButtonEnabled(UINT nButtonID)
{
	return(m_pToolBar ? m_pToolBar->IsButtonEnabled(nButtonID) : FALSE);
}

/*
	OnToolbarToolTip()

	Utilizzata per recuperare il testo del tooltip per i bottoni della toolbar.
*/
BOOL CDialogEx::OnToolbarToolTip(UINT nID,NMHDR* pNmhdr,LRESULT* /*pResult*/)
{
	#define TOOLTIP_MAX_TEXT 80
	static char text[TOOLTIP_MAX_TEXT];
	LPTOOLTIPTEXT tooltiptext = (LPTOOLTIPTEXT)pNmhdr;
	nID = pNmhdr->idFrom;

	if(nID)
	{
		UINT nToolTipID = 0;

		if(m_pToolBar)
			nToolTipID = m_pToolBar->GetToolTipID(nID);

		if(nToolTipID > 0)
		{
			memset(text,'\0',sizeof(text));
			::LoadString(AfxGetInstanceHandle(),nToolTipID,text,sizeof(text)-1);
			strcpyn(tooltiptext->szText,text,TOOLTIP_MAX_TEXT);
		}
		else
			strcpyn(tooltiptext->szText,"?",TOOLTIP_MAX_TEXT);

		return(TRUE);
	}
	else
		return(FALSE);
}

/*
	AddStatusBarPanel()

	Aggiunge un pannello alla statusbar.
	Riceve la dimensione del pannello (-1 per dimensionamento automatico rispetto all'esistente).
*/
BOOL CDialogEx::AddStatusBarPanel(int nSize)
{
	if(!m_pStatusBar)
	{
		m_pStatusBar = new CStatusBarCtrlEx();
		if(m_pStatusBar)
		{
			// se viene abilitato il resize dinamico e non viene specificato lo stile SBARS_SIZEGRIP la status bar coprirebbe il grip
			DWORD dwStyle = WS_CHILD|WS_VISIBLE|CCS_BOTTOM;
			if(m_bUseResizeStyle)
				dwStyle |= SBARS_SIZEGRIP;
			m_pStatusBar->Create(dwStyle,this,m_nStatusbarID);
		}
	}

	return(m_pStatusBar ? m_pStatusBar->Add(nSize) : FALSE);
}

/*
	AttachStatusBar()

	Collega la statusbar al dialogo.
*/
BOOL CDialogEx::AttachStatusBar(void)
{
	return(m_pStatusBar ? m_pStatusBar->Attach() : FALSE);
}

/*
	SetStatusBarPanel()
	
	Imposta il testo del pannello della statusbar.
	Riceve il testo per il pannello e l'indice del pannello da impostare (base 0).
*/
void CDialogEx::SetStatusBarPanel(LPCSTR lpcszText,UINT nPanel)
{
	if(m_pStatusBar)
		m_pStatusBar->SetPanel(lpcszText,nPanel);
}

/*
	AddToolTip()

	Aggiunge il tooltip al controllo.
*/
BOOL CDialogEx::AddToolTip(UINT nCtrlID,UINT nStringID,UINT nWidth/* = TOOLTIP_REASONABLE_WIDTH*/)
{
	BOOL bAdded = FALSE;

	// crea (se necessario) la lista per i tooltips
	if(!m_pListTooltips)
		m_pListTooltips = new CToolTipList();
	if(!m_pListTooltips)
		return(bAdded);

	// crea (se necessario) l'oggetto per il tooltip
	if(!m_pTooltip)
	{
		m_pTooltip = new CToolTipCtrlEx();
		if(m_pTooltip)
			if(m_pTooltip->Create(this,TTS_ALWAYSTIP))
			{
				m_pTooltip->SetDelay(TOOLTIP_REASONABLE_DELAYTIME);
				if(nWidth!=0)
					m_pTooltip->SetWidth(nWidth);
			}
	}
	
	// associa ed aggiunge il controllo specificato
	if(m_pTooltip)
	{
		TOOLTIPITEM* t = new TOOLTIPITEM;
		if(t)
		{
			t->nCtrlID	= nCtrlID;
			t->nStringID	= nStringID;
			t->pszText	= NULL;
			t->bExtended	= FALSE;
			char szText[2048] = {0};
			int nLen = ::LoadString(AfxGetInstanceHandle(),t->nStringID,szText,sizeof(szText)-1);
			if(nLen > 0)
			{
				t->pszText = new char[nLen+1];
				if(t->pszText)
				{
					strcpyn(t->pszText,szText,nLen+1);
					m_pListTooltips->Add(t);
					bAdded = m_pTooltip->AddWindowTool(GetDlgItem(t->nCtrlID),t->pszText);
				}
			}
		}
	}

	return(bAdded);
}

/*
	AddExtendedToolTip()

	Aggiunge il tooltip al controllo.
*/
BOOL CDialogEx::AddExtendedToolTip(UINT nCtrlID,LPCSTR lpcszText,LPCSTR lpcszCssStyles/* = NULL*/)
{
	BOOL bAdded = FALSE;

	// crea (se necessario) la lista per i tooltips
	if(!m_pListTooltips)
		m_pListTooltips = new CToolTipList();
	if(!m_pListTooltips)
		return(bAdded);

	// crea (se necessario) l'oggetto per il tooltip
	if(!m_pTooltipEx)
	{
		m_pTooltipEx = new CPPToolTip();
		if(m_pTooltipEx)
			if(m_pTooltipEx->Create(this->m_hWnd))
			{
				if(lpcszCssStyles)
					m_pTooltipEx->SetCssStyles(lpcszCssStyles);
				m_pTooltipEx->SetNotify();
				m_pTooltipEx->SetBehaviour(PPTOOLTIP_CLOSE_LEAVEWND|PPTOOLTIP_NOCLOSE_OVER|PPTOOLTIP_DISABLE_AUTOPOP);
			}
	}

	// associa ed aggiunge il controllo specificato
	if(m_pTooltipEx)
	{
		TOOLTIPITEM* t = new TOOLTIPITEM;
		if(t)
		{
			t->nCtrlID	= nCtrlID;
			t->nStringID	= (UINT)-1L;
			int nLen		= strlen(lpcszText);
			t->pszText	= new char[nLen+1];
			t->bExtended	= TRUE;
			if(t->pszText)
			{
				strcpyn(t->pszText,lpcszText,nLen+1);
				m_pListTooltips->Add(t);
				m_pTooltipEx->AddTool(GetDlgItem(t->nCtrlID),t->pszText);
				bAdded = TRUE;
			}
		}
	}

	return(bAdded);
}

/*
	ModifyToolTip()

	Modifica il testo del tooltip.
*/
BOOL CDialogEx::ModifyToolTip(UINT nCtrlID,LPCSTR lpcszText)
{
	BOOL bModify = FALSE;

	// controlla l'esistenza della lista per i tooltips
	if(!m_pListTooltips)
		return(bModify);

	// controlla l'esistenza dell'oggetto per il tooltip
	if(!m_pTooltip)
		return(bModify);
	
	// modifica
	ITERATOR iter;
	TOOLTIPITEM* t;
	if((iter = m_pListTooltips->First())!=(ITERATOR)NULL)
	{
		do
		{
			t = (TOOLTIPITEM*)iter->data;
			
			if(t)
				if(t->nCtrlID==nCtrlID)
				{
					// bug: al momento non supportato...
					if(t->bExtended)
						return(bModify);
						
					int nLen = 0;
					if(t->pszText)
					{
						if(strlen(lpcszText) > strlen(t->pszText))
							delete [] t->pszText,t->pszText = NULL;
						else
							nLen = strlen(t->pszText);
					}
					if(!t->pszText)
					{
						nLen = strlen(lpcszText);
						t->pszText = new char[nLen+1];
					}
					if(t->pszText)
					{
						strcpyn(t->pszText,lpcszText,nLen+1);
						m_pTooltip->UpdateTipText(t->pszText,GetDlgItem(t->nCtrlID));
						bModify = TRUE;
					}
				}

			iter = m_pListTooltips->Next(iter);
		
		} while(iter!=(ITERATOR)NULL);
	}

	return(bModify);
}

/*
	RemoveToolTip()

	Elimina il tooltip.
*/
BOOL CDialogEx::RemoveToolTip(UINT nCtrlID)
{
	BOOL bRemoved = FALSE;

	// controlla l'esistenza della lista per i tooltips
	if(!m_pListTooltips)
		return(bRemoved);

	// controlla l'esistenza dell'oggetto per il tooltip
	if(!m_pTooltip)
		return(bRemoved);
	
	// elimina
	ITERATOR iter;
	TOOLTIPITEM* t;
	if((iter = m_pListTooltips->First())!=(ITERATOR)NULL)
	{
		do
		{
			t = (TOOLTIPITEM*)iter->data;
		
			if(t)
				if(t->nCtrlID==nCtrlID)
				{
					if(t->bExtended)
					{
						m_pTooltipEx->RemoveTool(GetDlgItem(t->nCtrlID));
					}
					else
					{
						m_pTooltip->DelTool(GetDlgItem(t->nCtrlID));
						m_pListTooltips->Erase(iter);
					}
					bRemoved = TRUE;
					break;
				}

			iter = m_pListTooltips->Next(iter);
		
		} while(iter!=(ITERATOR)NULL);
	}

	return(bRemoved);
}
