/*
	CDialogEx.h
	Classe derivata per la finestra di dialogo (MFC).
	Luca Piergentili, 30/08/00
	lpiergentili@yahoo.com
*/
#ifndef _CDIALOGEX_H
#define _CDIALOGEX_H 1

#include "strings.h"
#include "window.h"
#include "win32api.h"
#include "CSync.h"
#include "CNodeList.h"
#include "CToolBarCtrlEx.h"
#include "CStatusBarCtrlEx.h"
#include "CToolTipCtrlEx.h"
#include "CPPTooltip.h"
#include "CDialogEx.config.h"
// resize
#ifdef _RESIZABLE_DIALOG_STYLE
  #include "Resizable.h"
  #include "ResizableLayout.h"
  #include "ResizableGrip.h"
  #include "ResizableMinMax.h"
  #include "ResizableState.h"
#endif

/*
	IDLIST
	struttura per la lista degli id della gui
*/
#ifdef _RESIZABLE_DIALOG_STYLE
struct IDLIST {
	int id;
};
#endif

/*	
	CIDList
	Classe per la lista degli id della gui.
*/
#ifdef _RESIZABLE_DIALOG_STYLE
class CIDList : public CNodeList
{
public:
	CIDList() : CNodeList() {}
	virtual ~CIDList() {CNodeList::DeleteAll();}
	void* Create(void)
	{
		return(new IDLIST);
	}	
	void* Initialize(void* pVoid)
	{
		IDLIST* pData = (IDLIST*)pVoid;
		if(!pData)
			pData = (IDLIST*)Create();
		if(pData)
			memset(pData,'\0',sizeof(IDLIST));
		return(pData);
	}
	BOOL PreDelete(ITERATOR iter)
	{
		if((IDLIST*)iter->data)
			delete ((IDLIST*)iter->data),iter->data = (IDLIST*)NULL;
		return(TRUE);
	}
	int Size(void) {return(sizeof(IDLIST));}
#ifdef _DEBUG
	const char* Signature(void) {return("CIDList");}
#endif
};
#endif

/*
	FILELIST
	struttura per la lista dei nomi file
*/
struct FILELIST {
	char file[_MAX_FILEPATH+1];
	DWORD attribute;
};

/*	
	CFileList
	Classe per la lista dei nomi file.
*/
class CFileList : public CNodeList
{
public:
	CFileList() : CNodeList() {}
	virtual ~CFileList() {CNodeList::DeleteAll();}
	void* Create(void)
	{
		return(new FILELIST);
	}	
	void* Initialize(void* pVoid)
	{
		FILELIST* pData = (FILELIST*)pVoid;
		if(!pData)
			pData = (FILELIST*)Create();
		if(pData)
			memset(pData,'\0',sizeof(FILELIST));
		return(pData);
	}
	BOOL PreDelete(ITERATOR iter)
	{
		if((FILELIST*)iter->data)
			delete ((FILELIST*)iter->data),iter->data = (FILELIST*)NULL;
		return(TRUE);
	}
	int Size(void) {return(sizeof(FILELIST));}
#ifdef _DEBUG
	const char* Signature(void) {return("CFileList");}
#endif
};

/*
	CDialogEx
	Classe estesa per la finestra di dialogo (gestisce l'icona, il menu, la toolbar, la statusbar, i tooltips,
	Esc, tasti funzione (F1...F12), istanze multiple, drag and drop, etc.).

	Utilizzo:

	- Derivare da CDialogEx.
	- Sovrascrivere la virtuale CDialogEx::OnInitDialog(), tale membro verra chiamato una sola volta alla
	  inizializzazione del dialogo. All'inizio della OnInitDialog() chiamare la CDialogEx::OnInitDialog()
	  controllando il codice di ritorno (per FALSE chiudere il dialogo, per TRUE aprirlo).
	- Per gestire le istanze multiple sovrascrivere la virtuale CDialogEx::OnInitDialogEx(), tale membro verra'
	  chiamato da ogni nuova istanza che venga lanciata.
	  Nella OnInitDialogEx(), da chiamare al termine della OnInitDialog(), eseguire le inizializzazioni necessarie
	  per ogni nuova istanza. Restituendo TRUE la nuova istanza viene chiusa, con FALSE viene aperta. Il codice di
	  ritorno e' sempre e solo per l'apertura/chiusura della nuova istanza, non per indicare al chiamante (ad es.
	  la OnInitDialog()) di terminare. In tal caso chiamare direttamente EndDialog().
	- Definire sempre gli identificativi IDOK e IDCANCEL e gestire solo il primo, IDCANCEL viene associato
	  automaticamente alla CDialogEx::OnExit(). Definire quindi una OnOk() e non una OnCancel() (nella mappa dei
	  messaggi associare solo IDOK).
	- Sovrascrivere la virtuale CDialogEx::OnExit(), tale membro verra chiamato per ogni richiesta di terminazione
	  (menu di sistema, Alt+F4, etc.). Al termine della funzione chiamare sempre la classe base (CDialogEx::OnExit()).
	- Per la cattura della tastiera sovrascrivere le virtuali CDialogEx::OnReturnKey(), OnEscapeKey(), OnFunctionKey(),
	  OnAltFunctionKey(), OnVirtualKey().
	- La ridefinizione delle OnPaint() e OnQueryDragIcon() originali (CDialog::) non e' necessaria dato che vengono
	  gestite il proprio.
	- L'istanza in corso puo' sapere se e' la unica in esecuzione o meno chiamando GetInstanceCount(). Per la descrizione
	  completa della gestione delle istanze multiple (controllate da OnInitDialogEx()) vedi le note sul costruttore.
	- Per l'utilizzo del drag and drop, ridefinire la virtuale CDialogEx::DragAndDrop() e da essa chiamare la
	  CDialogEx::GetDroppedFile() per recuperare i nomi dei files ricevuti.
*/
#define CDialogBaseClass CDialog

class CXDialogEx;
class CDialogEx : public CDialogBaseClass
#ifdef _RESIZABLE_DIALOG_STYLE
,public CResizableLayout,
public CResizableGrip,
public CResizableMinMax,
public CResizableState
#endif
{
	DECLARE_DYNCREATE(CDialogEx)

	friend class CXDialogEx;							// per il passaggio dei dati tra istanze

protected:										// provide default constructor only for dynamic creation, private/protected to prevent it from being called from outside the class implementation
	CDialogEx() {::MessageBox(NULL,"PANIC! This shouldn't happen!","CDialogEx()",MB_OK|MB_ICONERROR|MB_TASKMODAL|MB_SETFOREGROUND|MB_TOPMOST);}
	
public:
	CDialogEx(UINT		nTemplateID,					// id risorsa del dialogo
			UINT		nToolbarID = 0L,				// id risorsa per la toolbar
			UINT		nStatusbarID = 0L,				// id risorsa per la status bar
			UINT		nIconID = 0L,					// id risorsa dell'icona
			UINT		nMenuID = 0L,					// id risorsa del menu
#ifdef _RESIZABLE_DIALOG_STYLE
			BOOL		bAllowResize = FALSE,			// per dimensionare il dialogo
#endif
			BOOL		bAllowSnap = FALSE,				// per abilitare lo snap to screen border
			BOOL		bAllowDragAndDrop = FALSE,		// per abilitare il drag and drop
			LPCSTR	lpcszUniqueName = NULL,			// nome unico per istanza
			BOOL		bAllowMultipleInstances = FALSE,	// per abilitare piu' istanze
			HWND		hWndParent = NULL);				// ptr alla finestra padre
	
	virtual ~CDialogEx();

	// inizializzazione/terminazione
	virtual BOOL		OnInitDialog			(void);
	virtual BOOL		OnInitDialogEx			(UINT /*nInstanceCount*/ = (UINT)-1L,LPCSTR /*lpcszCommandLine*/ = NULL) {return(TRUE);}
	virtual void		OnExit				(void);
	virtual void		OnExit				(UINT nExitCode);
	virtual void		OnOk					(void) {CDialogBaseClass::OnOK();}
	virtual void		OnCancel				(void) {CDialogBaseClass::OnCancel();}

	// drag and drop
	inline void		AllowDragAndDrop		(BOOL bFlag) {m_bAllowDragAndDrop = bFlag; ::DragAcceptFiles(this->m_hWnd,m_bAllowDragAndDrop);}
	virtual void		OnDropFiles			(void) {}
	UINT				GetDroppedCount		(void);
	BOOL				GetDroppedFile			(LPSTR lpszFileName,UINT nSize,DWORD* pdwAttribute);
	
	// tastiera/mouse
	virtual BOOL		PreTranslateMessage		(MSG* msg);
	virtual BOOL		OnReturnKey			(void) {return(FALSE);}
	virtual BOOL		OnEscapeKey			(void) {return(TRUE);}
	virtual BOOL		OnFunctionKey			(WPARAM /*wFunctionKey*/,BOOL /*bShift*/, BOOL /*bCtrl*/) {return(FALSE);}
	virtual BOOL		OnAltFunctionKey		(WPARAM /*wFunctionKey*/) {return(FALSE);}
	virtual BOOL		OnVirtualKey			(WPARAM /*wVirtualKey*/,BOOL /*bShift*/,BOOL /*bCtrl*/,BOOL /*bAlt*/) {return(FALSE);}
	inline void		SetAltAcceleratorKeys	(LPCSTR lpcszKeys) {strcpyn(m_szAltKeyChars,lpcszKeys,sizeof(m_szAltKeyChars));}
	void				OnLButtonDown			(UINT nFlags,CPoint point);
		
	// icona
	void				SetIcon				(UINT nIconID);
	void				SetIcon				(LPCSTR lpcszFileName);
	HICON			GetIcon				(void) const {return(m_hIconLarge);}

	// visibilita'
	inline BOOL		IsVisible				(void) const {return(m_bVisible);}
	inline void		SetVisible			(BOOL bFlag) {m_bVisible = bFlag;}
	
	// menu
	void				SetMenu				(UINT nMenuID);
	
	// titolo
	inline LPCSTR		GetWindowTitle			(void) {return(m_szWindowTitle);}
	void				SetWindowTitle			(LPCSTR lpcszTitle) {strcpyn(m_szWindowTitle,lpcszTitle,sizeof(m_szWindowTitle)); SetWindowText(m_szWindowTitle);}
	void				SetWindowTitle			(UINT nID,LPCSTR lpcszText = NULL)
	{
		char szTitle[128] = {0};
		if(lpcszText)
			::FormatResourceStringEx(szTitle,sizeof(szTitle),nID,lpcszText);
		else
			::FormatResourceString(szTitle,sizeof(szTitle),nID);
		
		SetWindowTitle(szTitle);
	}
	
	// resize
#ifdef _RESIZABLE_DIALOG_STYLE
	inline BOOL		IsDialogResizeEnabled	(void) const {return(m_bUseResizeStyle);}
	inline void		AllowMinTrackSize		(BOOL bAllowMinTrackSize) {m_bAllowMinTrackSize = bAllowMinTrackSize;}
	inline void		AddId				(int nID) {IDLIST* id; if((id = (IDLIST*)m_listID.Add())!=(IDLIST*)NULL) id->id = nID;}
#endif
	
	// snap
	inline BOOL		IsSnapToScreenEnabled	(void) const {return(m_bUseSnapToScreen);}

	// toolbar
	BOOL				AddToolBarButton		(UINT nButtonID,UINT nToolTipID = 0,UINT nTextID = 0,BYTE fsStatus = TBSTATE_ENABLED);
	BOOL				AddToolBarSeparator		(void);
	BOOL				AttachToolBar			(const CSize& sizeButton,const CSize& sizeBitmap,UINT nToolBarID = (UINT)-1L,UINT nColdToolBarID = (UINT)-1L,UINT nHotToolBarID = (UINT)-1L);
	void				EnableToolBarButton		(UINT nButtonID,BOOL bState);
	BOOL				IsToolBarButtonEnabled	(UINT nButtonID);

	// statusbar
	BOOL				AddStatusBarPanel		(int nSize);
	BOOL				AttachStatusBar		(void);
	void				SetStatusBarPanel		(LPCSTR lpcszText,UINT nPanel);
	CWnd*			GetMessageBar			(void) {return(m_pStatusBar);}

	// tooltip
	BOOL				AddToolTip			(UINT nCtrlID,UINT nStringID,UINT nWidth = TOOLTIP_REASONABLE_WIDTH);
	BOOL				AddExtendedToolTip		(UINT nCtrlID,LPCSTR lpcszText,LPCSTR lpcszCssStyles = NULL);
	BOOL				ModifyToolTip			(UINT nCtrlID,LPCSTR lpcszText);
	BOOL				RemoveToolTip			(UINT nCtrlID);
	inline CPPToolTip*	GetExtendedToolTipCtrl	(void) const {return(m_pTooltipEx);}

	// istanze
	int				GetInstanceCount		(void) {return(m_nInstanceCount);}

protected:
	// visualizzazione
	virtual void		OnWindowPosChanging		(WINDOWPOS FAR* lpWndPos);
	virtual void		OnPaint				(void);
	virtual HCURSOR	OnQueryDragIcon		(void);

	// drag and drop
	LONG				OnDragDrop			(UINT wParam,LONG lParam);

	// tooltip
	BOOL				OnToolbarToolTip		(UINT nID,NMHDR* pNmhdr,LRESULT* /*pResult*/);
	
	// resize
#ifdef _RESIZABLE_DIALOG_STYLE
	virtual void		OnGetMinMaxInfo		(MINMAXINFO FAR* lpMMI);
	virtual void		OnSize				(UINT nType,int cx,int cy);
	virtual void		OnDestroy				(void);
	virtual int		OnCreate				(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL		OnEraseBkgnd			(CDC* pDC);
	virtual CWnd*		GetResizableWnd		(void) {return(this);}
	void				EnableSaveRestore		(LPCTSTR pszSection,BOOL bRectOnly = FALSE);
#endif

	HICON			m_hIconSmall;				// icone per il dialogo
	HICON			m_hIconLarge;

private:
	BOOL				m_bVisible;				// flag per visibilita' dialogo (MFC visualizza quando gli pare a lui)
	char				m_szWindowTitle[256];
	UINT				m_nExitCode;				// codice d'uscita
	char				m_szAltKeyChars[32];		// caratteri da intercettare con il tasto Alt (Alt+F, Alt+S, etc.)
	CSyncProcesses*	m_pMutexInstance;			// mutex per gestione istanze
	UINT				m_nInstanceCount;			// numero di istanze
	BOOL				m_bAllowMultipleInstances;	// flag per permettere istanze multiple
	char				m_szUniqueName[_MAX_PATH+1];	// nome unico per il mutex/finestra interna
	CXDialogEx*		m_pWndInternal;			// finestra interna
	char				m_szCommandLine[_MAX_PATH+1];	// linea di comando
	CFileList			m_listDroppedFiles;			// lista per i files ricevuti
	BOOL				m_bAllowDragAndDrop;		// flag per drag and drop
	UINT				m_nDropCount;				// tot files ricevuti via drap and drop
	UINT				m_nIconID;				// id dell'icona
	UINT				m_nMenuID;				// id del menu
	UINT				m_nToolbarID;				// id toolbar
	UINT				m_nStatusbarID;			// id status bar
	CToolBarCtrlEx*	m_pToolBar;				// toolbar
	CStatusBarCtrlEx*	m_pStatusBar;				// statusbar
	CToolTipCtrlEx*	m_pTooltip;				// tooltip
	CPPToolTip*		m_pTooltipEx;				// tooltip esteso (balloon)
	CToolTipList*		m_pListTooltips;			// lista per i tooltips
#ifdef _RESIZABLE_DIALOG_STYLE
	BOOL				m_bUseResizeStyle;			// per il resize
	BOOL				m_bAllowMinTrackSize;
	DWORD			m_dwGripTempState;
	BOOL				m_bEnableSaveRestore;
	BOOL				m_bRectOnly;
	CString			m_strSection;
	CIDList			m_listID;
#endif
	BOOL				m_bUseSnapToScreen;			// per lo snap
	int				m_nSnapYOffset;
	int				m_nSnapXOffset;

	DECLARE_MESSAGE_MAP()
};

/*	
	CXDialogEx
	Classe per la finestra interna, usata per la gestione delle istanze multiple.
*/
class CXDialogEx : public CWnd
{
public:
	CXDialogEx(LPCSTR lpcszUniqueName,CDialogEx* pDialogEx,CWnd* pWnd);
	virtual ~CXDialogEx();

	LONG OnCopyData(UINT wParam,LONG lParam);

protected:
	virtual void OnDestroy(void);

private:
	CDialogEx* m_pDialogEx;

	DECLARE_MESSAGE_MAP()
};

#endif // _CDIALOGEX_H
