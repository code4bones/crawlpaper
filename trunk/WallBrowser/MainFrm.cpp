/*
	MainFrm.cpp
	Classe per il frame principale.
	Luca Piergentili, 30/07/01
	lpiergentili@yahoo.com

	WallBrowser - the smart picture browser
	http://www.crawlpaper.com/
	Copyright © 1998-2003 Luca Piergentili, all rights reserved.
	CrawlPaper is a registered name, all rights reserved. 
	
	This program is free software; you can redistribute it and/or modify it under the terms
	of the GNU General Public License as published by the Free Software Foundation; either
	version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
	without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
	See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along with this program;
	if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
	MA 02111-1307 USA
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include "window.h"
#include "win32api.h"
#include "CNodeList.h"
#include "CComboToolBar.h"
#include "CImageFactory.h"
#include "CRectEx.h"
#include "CWinAppEx.h"
#include "CWndLayered.h"
#include "MainFrm.h"
#include "WallBrowserDoc.h"
#include "WallBrowserDriveView.h"
#include "WallBrowserFileView.h"
#include "WallBrowserMessages.h"
#include "WallBrowserStretchView.h"
#include "WallBrowserLayeredDlg.h"
#include "resource.h"

#include "traceexpr.h"
//#define _TRACE_FLAG	_TRFLAG_TRACEOUTPUT
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

IMPLEMENT_DYNCREATE(CMainFrame,CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame,CFrameWnd)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_INITMENUPOPUP()
	
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE,OnUIFileSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_MOVE,OnUIFileMove)
	ON_UPDATE_COMMAND_UI(ID_FILE_DELETE,OnUIFileDelete)
	
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY,OnUIEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE,OnUIEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO,OnUIEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO,OnUIEditRedo)
	
	ON_UPDATE_COMMAND_UI(ID_VIEW_PREVIOUS_PICTURE,OnUIViewPrevious)
	ON_UPDATE_COMMAND_UI(ID_VIEW_NEXT_PICTURE,OnUIViewNext)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOMIN,OnUIViewZoomIn)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOMOUT,OnUIViewZoomOut)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOMRESET,OnUIViewZoomReset)
	
	ON_UPDATE_COMMAND_UI(ID_TOOLS_SETWALLPAPER,OnUIToolsSetWallPaper)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_RESETWALLPAPER,OnUIToolsResetWallPaper)
	
	ON_MESSAGE(WM_FILE_SAVE_AS,OnFileSaveAs)
	ON_MESSAGE(WM_FILE_SAVE,OnFileSave)
	ON_MESSAGE(WM_FILE_MOVE,OnFileMove)
	ON_MESSAGE(WM_FILE_DELETE,OnFileDelete)
	
	ON_MESSAGE(WM_EDIT_COPY,OnEditCopy)
	ON_MESSAGE(WM_EDIT_PASTE,OnEditPaste)
	ON_MESSAGE(WM_EDIT_UNDO,OnEditUndo)
	ON_MESSAGE(WM_EDIT_REDO,OnEditRedo)

	ON_COMMAND(ID_VIEW_SCROLL,OnViewScroll)
	ON_COMMAND(ID_VIEW_STRETCH,OnViewStretch)
	ON_COMMAND(ID_VIEW_FILESPLITTER,OnViewFileSplitter)
	ON_COMMAND(ID_VIEW_DIRSPLITTER,OnViewDirSplitter)
	ON_MESSAGE(WM_TOGGLE_FILESPLITTER,OnViewToggleFileSplitter)
	ON_MESSAGE(WM_TOGGLE_DIRSPLITTER,OnViewToggleDirSplitter)
	ON_MESSAGE(WM_VIEW_PREVIOUS_PICTURE,OnViewPrevious)
	ON_MESSAGE(WM_VIEW_NEXT_PICTURE,OnViewNext)
	ON_COMMAND(ID_VIEW_ZOOMIN,OnViewZoomIn)
	ON_COMMAND(ID_VIEW_ZOOMOUT,OnViewZoomOut)
	ON_COMMAND(ID_VIEW_ZOOMRESET,OnViewZoomReset)

	ON_MESSAGE(WM_GET_STATUSBAR,OnGetStatusBar)
	ON_MESSAGE(WM_GET_FILESPLITTER_SIZE,OnGetFileSplitterSize)
	ON_MESSAGE(WM_GET_DIRSPLITTER_SIZE,OnGetDirSplitterSize)
	
	ON_MESSAGE(WM_TOOLS_SETWALLPAPER,OnToolsSetWallPaper)
	ON_MESSAGE(WM_TOOLS_RESETWALLPAPER,OnToolsResetWallPaper)
	ON_MESSAGE(WM_TOOLS_LIBRARY,OnToolsLibrary)

	ON_MESSAGE(WM_IMAGE_MIRROR_H,OnImageMirrorH)
	ON_MESSAGE(WM_IMAGE_MIRROR_V,OnImageMirrorV)
	ON_MESSAGE(WM_IMAGE_ROTATE_90_LEFT,OnImageRotate90Left)
	ON_MESSAGE(WM_IMAGE_ROTATE_90_RIGHT,OnImageRotate90Right)
	ON_MESSAGE(WM_IMAGE_ROTATE_180,OnImageRotate180)
	ON_MESSAGE(WM_IMAGE_DESKEW,OnImageDeskew)
	ON_MESSAGE(WM_IMAGE_SIZE,OnImageSize)
	ON_MESSAGE(WM_IMAGE_POSTERIZE,OnImagePosterize)
	ON_MESSAGE(WM_IMAGE_MOSAIC,OnImageMosaic)
	ON_MESSAGE(WM_IMAGE_BLUR,OnImageBlur)
	ON_MESSAGE(WM_IMAGE_MEDIAN,OnImageMedian)
	ON_MESSAGE(WM_IMAGE_SHARPEN,OnImageSharpen)
	ON_MESSAGE(WM_IMAGE_DESPECKLE,OnImageDespeckle)
	ON_MESSAGE(WM_IMAGE_NOISE,OnImageNoise)
	ON_MESSAGE(WM_IMAGE_EMBOSS,OnImageEmboss)
	ON_MESSAGE(WM_IMAGE_EDGEENHANCE,OnImageEdgeEnhance)
	ON_MESSAGE(WM_IMAGE_FINDEDGE,OnImageFindEdge)
	ON_MESSAGE(WM_IMAGE_EROSION,OnImageErosion)
	ON_MESSAGE(WM_IMAGE_DILATE,OnImageDilate)
	
	ON_MESSAGE(WM_COLOR_HALFTONE,OnColorHalftone)
	ON_MESSAGE(WM_COLOR_GRAYSCALE,OnColorGrayscale)
	ON_MESSAGE(WM_COLOR_INVERT,OnColorInvert)
	ON_MESSAGE(WM_COLOR_BRIGHTNESS,OnColorBrightness)
	ON_MESSAGE(WM_COLOR_CONTRAST,OnColorContrast)
	ON_MESSAGE(WM_COLOR_HISTOGRAM_CONTRAST,OnColorHistogramContrast)
	ON_MESSAGE(WM_COLOR_HUE,OnColorHue)
	ON_MESSAGE(WM_COLOR_SATURATION_H,OnColorSaturationH)
	ON_MESSAGE(WM_COLOR_SATURATION_V,OnColorSaturationV)
	ON_MESSAGE(WM_COLOR_EQUALIZE,OnColorEqualize)
	ON_MESSAGE(WM_COLOR_INTENSITY,OnColorIntensity)
	ON_MESSAGE(WM_COLOR_INTENSITY_DETECT,OnColorIntensityDetect)
	ON_MESSAGE(WM_COLOR_GAMMA_CORRECTION,OnColorGammaCorrection)
	
	ON_COMMAND(ID_OPTIONS_AUTOSIZECOLS,OnOptionsAutoSizeCols)
	ON_COMMAND(ID_OPTIONS_TRANSPARENCY,OnOptionsTransparency)
	ON_COMMAND(ID_OPTIONS_DRAWWITHSTRETCHDIBITS,OnOptionsDrawWithStretchDIBits)
	ON_COMMAND(ID_OPTIONS_DRAWWITHSTRETCHBLT,OnOptionsDrawWithStretchBlt)
	ON_COMMAND(ID_OPTIONS_DRAWWITHVFW,OnOptionsDrawWithVfw)
	ON_COMMAND(ID_OPTIONS_CONFIRMFILEDELETE,OnOptionsConfirmFileDelete)
	ON_COMMAND(ID_OPTIONS_DELETETORECYCLE,OnOptionsDeleteToRecycleBin)
END_MESSAGE_MAP()

// identificatori per i pannelli della status bar
static UINT nIndicators[] = {
	ID_SEPARATOR,
	ID_INDICATOR_FILES,
	ID_INDICATOR_PICTURES,
	ID_INDICATOR_FILENAME,
	ID_INDICATOR_ZOOM,
	ID_INDICATOR_RATIO,
	ID_INDICATOR_INFO,
	ID_INDICATOR_MEM,
	ID_INDICATOR_LIBRARY
};

/*
	CMainFrame()
*/
CMainFrame::CMainFrame()
{
	m_nFileSplitterCurrent = FILE_SPLITTER_SIZE;
	m_nDirSplitterCurrent = DIR_SPLITTER_SIZE;

	// inizializza il flag per l'abilitazione dei menu
	m_bAutoMenuEnable = FALSE;

	m_bFileSaveAs = FALSE;
	m_bFileSave = FALSE;
	m_bFileMove = FALSE;
	m_bFileDelete = FALSE;
	m_bEditCopy = FALSE;
	m_bEditPaste = FALSE;
	m_bEditUndo = FALSE;
	m_bEditRedo = FALSE;
	m_bViewPrevious = FALSE;
	m_bViewNext = FALSE;
	m_bWallPaper = FALSE;
	m_nViewType = DEFAULT_VIEWTYPE;
	m_nDrawMode = DEFAULT_DRAWMODE;
	m_bConfirmFileDelete = TRUE;
	m_bDeleteToRecycleBin = TRUE;
	m_bAutoSizeCols = TRUE;

	m_bImageMirrorH = FALSE;
	m_bImageMirrorV = FALSE;
	m_bImageRotate90Left = FALSE;
	m_bImageRotate90Right = FALSE;
	m_bImageRotate180 = FALSE;
	m_bImageDeskew = FALSE;
	m_bImageSize = FALSE;
	m_bImagePosterize = FALSE;
	m_bImageMosaic = FALSE;
	m_bImageBlur = FALSE;
	m_bImageMedian = FALSE;
	m_bImageSharpen = FALSE;
	m_bImageDespeckle = FALSE;
	m_bImageNoise = FALSE;
	m_bImageEmboss = FALSE;
	m_bImageEdgeEnhance = FALSE;
	m_bImageFindEdge = FALSE;
	m_bImageErosion = FALSE;
	m_bImageDilate = FALSE;
	
	m_bColorHalftone = FALSE;
	m_bColorGrayscale = FALSE;
	m_bColorInvert = FALSE;
	m_bColorBrightness = FALSE;
	m_bColorContrast = FALSE;
	m_bColorHistogramContrast = FALSE;
	m_bColorHue = FALSE;
	m_bColorSaturationH = FALSE;
	m_bColorSaturationV = FALSE;
	m_bColorEqualize = FALSE;
	m_bColorIntensity = FALSE;
	m_bColorIntensityDetect = FALSE;
	m_bColorGammaCorrection = FALSE;
}

/*
	OnCreate()
*/
int CMainFrame::OnCreate(LPCREATESTRUCT lpcs)
{
	CWinAppEx* pWinAppEx = NULL;

	// classe base
	if(CFrameWnd::OnCreate(lpcs)==-1)
		return(-1);

	// toolbar principale
	if(!m_wndMainToolBar.Create(this))
		return(-1);
	if(!m_wndMainToolBar.LoadToolBar(IDR_MAINFRAME))
		return(-1);

	// stile toolbar
	m_wndMainToolBar.SetBarStyle(m_wndMainToolBar.GetBarStyle()|CBRS_TOOLTIPS|CBRS_FLYBY|CBRS_SIZE_DYNAMIC);
	m_wndMainToolBar.ModifyStyle(0,TBSTYLE_FLAT);

	// combo box della toolbar

	// ricava l'indice dell'id relativo al combo nella toolbar
	int index = 0;
	while(m_wndMainToolBar.GetItemID(index)!=ID_TOOLS_LIBRARY)
		index++;

	// converte il bottone in un separatore e ricava la posizione relativa
	m_wndMainToolBar.SetButtonInfo(index,ID_TOOLS_LIBRARY,TBBS_SEPARATOR,COMBO_WIDTH);
	CRect rect;
	m_wndMainToolBar.GetItemRect(index,&rect);

	// dimensiona l'area per il combo
	rect.top += 2;
	rect.bottom += 200;

	// crea il combo
	if(!m_wndMainToolBar.m_wndCombo.Create(WS_CHILD|WS_VISIBLE|CBS_AUTOHSCROLL|CBS_DROPDOWNLIST|CBS_HASSTRINGS,rect,&m_wndMainToolBar,IDC_COMBO_ON_TOOLBAR))
		return(FALSE);

	// imposta il font per il combo
	HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	if(hFont==NULL)
		hFont = (HFONT)GetStockObject(ANSI_VAR_FONT);
	m_wndMainToolBar.m_wndCombo.SendMessage(WM_SETFONT,(WPARAM)hFont);

	m_wndMainToolBar.m_wndCombo.ShowWindow(SW_SHOW);

	// riempie il combo con i nomi delle librerie disponibili
	char* p;
	CImageFactory ImageFactory;
	while((p = (char*)ImageFactory.EnumLibraryNames())!=NULL)
		m_wndMainToolBar.m_wndCombo.AddString(p);
	m_wndMainToolBar.m_wndCombo.SetCurSel(0);

	if((pWinAppEx = (CWinAppEx*)AfxGetApp())!=(CWinAppEx*)NULL)
	{
		char szLibraryName[_MAX_PATH+1] = {0};
		strcpyn(szLibraryName,pWinAppEx->m_Config.GetString(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_CURRENTLIBRARY_KEY),sizeof(szLibraryName));
		int nIndex = 0;
		if((nIndex = m_wndMainToolBar.m_wndCombo.FindStringExact(-1,szLibraryName))==CB_ERR)
			nIndex = m_wndMainToolBar.m_wndCombo.AddString(szLibraryName);
		m_wndMainToolBar.m_wndCombo.SetCurSel(nIndex);
	}

	// imposta il messaggio che il combo inviera alla selezione
	m_wndMainToolBar.SetMessage(this->m_hWnd,WM_TOOLS_LIBRARY);

	// status bar
	if(!m_wndStatusBar.Create(this))
		return(-1);
	if(!m_wndStatusBar.SetIndicators(nIndicators,sizeof(nIndicators)/sizeof(UINT)))
		return(-1);

	// carica la configurazione
	if(pWinAppEx)
	{
		// posizione e stato della finestra
		WINDOWPLACEMENT wp;
		memset(&wp,'\0',sizeof(WINDOWPLACEMENT));
		
		wp.length					= sizeof(WINDOWPLACEMENT);
		wp.flags					= pWinAppEx->m_Config.GetNumber(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_WINDOWPLACEMENT_FLAGS_KEY);
		wp.showCmd				= pWinAppEx->m_Config.GetNumber(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_WINDOWPLACEMENT_SHOWCMD_KEY);
		wp.ptMinPosition.x			= pWinAppEx->m_Config.GetNumber(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_WINDOWPLACEMENT_MINPOSX_KEY);
		wp.ptMinPosition.y			= pWinAppEx->m_Config.GetNumber(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_WINDOWPLACEMENT_MINPOSY_KEY);
		wp.ptMaxPosition.x			= pWinAppEx->m_Config.GetNumber(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_WINDOWPLACEMENT_MAXPOSX_KEY);
		wp.ptMaxPosition.y			= pWinAppEx->m_Config.GetNumber(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_WINDOWPLACEMENT_MAXPOSY_KEY);
		wp.rcNormalPosition.left		= pWinAppEx->m_Config.GetNumber(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_WINDOWPLACEMENT_NPOSLEFT_KEY);
		wp.rcNormalPosition.top		= pWinAppEx->m_Config.GetNumber(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_WINDOWPLACEMENT_NPOSTOP_KEY);
		wp.rcNormalPosition.right	= pWinAppEx->m_Config.GetNumber(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_WINDOWPLACEMENT_NPOSRIGHT_KEY);
		wp.rcNormalPosition.bottom	= pWinAppEx->m_Config.GetNumber(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_WINDOWPLACEMENT_NPOSBOTTOM_KEY);

		// ripristina lo stato precedente
		if(!(wp.flags & WPF_RESTORETOMAXIMIZED) && (wp.rcNormalPosition.left!=0) && (wp.rcNormalPosition.top!=0) && (wp.rcNormalPosition.right!=0) && (wp.rcNormalPosition.bottom!=0))
			::SetWindowPlacement(this->m_hWnd,&wp);

		// opzioni
		m_nDrawMode = pWinAppEx->m_Config.GetNumber(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_DRAWMODE_KEY);
		if(m_nDrawMode!=DRAW_STRETCHDIBITS && m_nDrawMode!=DRAW_STRETCHBLT && m_nDrawMode!=DRAW_VFWDRAWDIB)
			m_nDrawMode = DRAW_STRETCHDIBITS;
		m_bConfirmFileDelete = pWinAppEx->m_Config.GetNumber(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_CONFIRMFILEDELETE_KEY);
		m_bDeleteToRecycleBin = pWinAppEx->m_Config.GetNumber(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_DELETEFILESTORECYCLEBIN_KEY);
		m_bAutoSizeCols = pWinAppEx->m_Config.GetNumber(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_AUTOSIZECOLUMNS_KEY);
		m_nViewType = pWinAppEx->m_Config.GetNumber(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_VIEWTYPE_KEY);
	}

	// trasparenza
	if(pWinAppEx->m_Config.GetNumber(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_LAYERED_KEY))
	{
		BYTE nLayered = (BYTE)pWinAppEx->m_Config.GetNumber(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_LAYEREDVALUE_KEY);
		if(nLayered > LWA_ALPHA_MIN && nLayered < LWA_ALPHA_MAX)
		{
			BOOL bLayer = TRUE;
			if(nLayered < LWA_ALPHA_INVISIBLE)
				bLayer = ::MessageBoxResourceEx(this->m_hWnd,MB_YESNO|MB_ICONQUESTION,WALLBROWSER_PROGRAM_NAME,IDS_QUESTION_LAYERED,nLayered,LWA_ALPHA_MIN,LWA_ALPHA_MAX)==IDYES;
			if(bLayer)
				m_wndLayered.SetLayer(this->m_hWnd,nLayered);
		}
	}

	return(0);
}

/*
	OnCreateClient()
*/
BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/,CCreateContext* pContext)
{
	// ricava le dimensioni degli splitter
	CWinAppEx* pWinAppEx = (CWinAppEx*)AfxGetApp();
	if(pWinAppEx)
	{
		m_nFileSplitterCurrent = pWinAppEx->m_Config.GetNumber(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_FILESPLITTER_KEY);
		m_nDirSplitterCurrent = pWinAppEx->m_Config.GetNumber(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_DIRSPLITTER_KEY);
	}

	// crea gli splitter e le viste relative

	// splitter per l'albero delle directory
	//	|----|----|
	//	|    |    |
	//	|    |    |
	//	|    |    |
	//	|----|----|
	if(!m_wndDirSplitter.CreateStatic(this,1,2))
		return(FALSE);

	// splitter per la lista dei file
	//	|----|----|
	//	|    |    |
	//	|    |----|
	//	|    |    |
	//	|----|----|
	if(!m_wndFileSplitter.CreateStatic(&m_wndDirSplitter,2,1,WS_CHILD | WS_VISIBLE | WS_BORDER,m_wndDirSplitter.IdFromRowCol(0,1)))
		return(FALSE);

	// crea la vista per la lista dei file
	//	|----|----|
	//	|    | x  |
	//	|    |----|
	//	|    |    |
	//	|----|----|
	if(!m_wndFileSplitter.CreateView(0,0,RUNTIME_CLASS(CWallBrowserFileView),CSize(0,0),pContext))
		return(FALSE);

	// crea la vista per la visualizzazione dell'immagine
	//	|----|----|
	//	|    | x  |
	//	|    |----|
	//	|    | y  |
	//	|----|----|
	if(!m_wndFileSplitter.CreateView(1,0,RUNTIME_CLASS(CWallBrowserStretchView),CSize(0,0),pContext))
		return(FALSE);

	// crea la vista per l'albero delle directory
	// <pContext> specifica la classe da utilizzare (CWallBrowserDriveView), impostata durante
	// la creazione del template, vedi la InitInstance() dell'applicazione (WallBrowser.cpp)
	//	|----|----|
	//	|    | x  |
	//	| z  |----|
	//	|    | y  |
	//	|----|----|
	if(!m_wndDirSplitter.CreateView(0,0,pContext->m_pNewViewClass,CSize(0,0),pContext))
		return(FALSE);

	// dimensiona gli splitter
	int nMin = DIR_SPLITTER_MINSIZE;
	m_wndDirSplitter.SetColumnInfo(0,m_nDirSplitterCurrent,nMin);
	m_wndDirSplitter.RecalcLayout();
	nMin = FILE_SPLITTER_MINSIZE;
	m_wndFileSplitter.SetRowInfo(0,m_nFileSplitterCurrent,nMin);
	m_wndFileSplitter.RecalcLayout();

	// imposta la vista attiva
	SetActiveView((CView*)m_wndDirSplitter.GetPane(0,0),FALSE);

	return(TRUE);
}

/*
	OnClose()
*/
void CMainFrame::OnClose(void)
{
	// salva la configurazione
	CWinAppEx* pWinAppEx = (CWinAppEx*)AfxGetApp();
	if(pWinAppEx)
	{
		// posizione e stato della finestra
		WINDOWPLACEMENT wp;
		memset(&wp,'\0',sizeof(WINDOWPLACEMENT));
		wp.length = sizeof(WINDOWPLACEMENT);
		::GetWindowPlacement(this->m_hWnd,&wp);

		if(wp.flags & WPF_RESTORETOMAXIMIZED)
			pWinAppEx->m_Config.UpdateNumber(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_WINDOWPLACEMENT_SHOW_KEY,SW_SHOWMAXIMIZED);
		else
			pWinAppEx->m_Config.UpdateNumber(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_WINDOWPLACEMENT_SHOW_KEY,SW_SHOW);

		pWinAppEx->m_Config.UpdateNumber(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_WINDOWPLACEMENT_FLAGS_KEY,wp.flags);
		pWinAppEx->m_Config.UpdateNumber(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_WINDOWPLACEMENT_SHOWCMD_KEY,wp.showCmd);
		pWinAppEx->m_Config.UpdateNumber(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_WINDOWPLACEMENT_MINPOSX_KEY,wp.ptMinPosition.x);
		pWinAppEx->m_Config.UpdateNumber(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_WINDOWPLACEMENT_MINPOSY_KEY,wp.ptMinPosition.y);
		pWinAppEx->m_Config.UpdateNumber(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_WINDOWPLACEMENT_MAXPOSX_KEY,wp.ptMaxPosition.x);
		pWinAppEx->m_Config.UpdateNumber(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_WINDOWPLACEMENT_MAXPOSY_KEY,wp.ptMaxPosition.y);
		pWinAppEx->m_Config.UpdateNumber(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_WINDOWPLACEMENT_NPOSLEFT_KEY,wp.rcNormalPosition.left);
		pWinAppEx->m_Config.UpdateNumber(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_WINDOWPLACEMENT_NPOSTOP_KEY,wp.rcNormalPosition.top);
		pWinAppEx->m_Config.UpdateNumber(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_WINDOWPLACEMENT_NPOSRIGHT_KEY,wp.rcNormalPosition.right);
		pWinAppEx->m_Config.UpdateNumber(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_WINDOWPLACEMENT_NPOSBOTTOM_KEY,wp.rcNormalPosition.bottom);

		// dimensione degli splitter
		int nMin = FILE_SPLITTER_MINSIZE;
		m_wndFileSplitter.GetRowInfo(0,m_nFileSplitterCurrent,nMin);
		pWinAppEx->m_Config.UpdateNumber(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_FILESPLITTER_KEY,m_nFileSplitterCurrent);
		nMin = DIR_SPLITTER_MINSIZE;
		m_wndDirSplitter.GetColumnInfo(0,m_nDirSplitterCurrent,nMin);
		pWinAppEx->m_Config.UpdateNumber(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_DIRSPLITTER_KEY,m_nDirSplitterCurrent);

		// opzioni
		if(m_nDrawMode!=DRAW_STRETCHDIBITS && m_nDrawMode!=DRAW_STRETCHBLT && m_nDrawMode!=DRAW_VFWDRAWDIB)
			m_nDrawMode = DRAW_STRETCHDIBITS;
		pWinAppEx->m_Config.UpdateNumber(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_DRAWMODE_KEY,m_nDrawMode);
		pWinAppEx->m_Config.UpdateNumber(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_CONFIRMFILEDELETE_KEY,m_bConfirmFileDelete);
		pWinAppEx->m_Config.UpdateNumber(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_DELETEFILESTORECYCLEBIN_KEY,m_bDeleteToRecycleBin);
		pWinAppEx->m_Config.UpdateNumber(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_AUTOSIZECOLUMNS_KEY,m_bAutoSizeCols);

		pWinAppEx->m_Config.Save();
	}

	CFrameWnd::OnClose();
}

/*
	OnInitMenuPopup()
*/
void CMainFrame::OnInitMenuPopup(CMenu* pMenu,UINT nIndex,BOOL bSysMenu)
{
	if(!bSysMenu)
	{
		// File
		if(pMenu->GetMenuState(ID_FILE_SAVE_AS,MF_BYCOMMAND)!=0xFFFFFFFF)
			pMenu->EnableMenuItem(ID_FILE_SAVE_AS,m_bFileSaveAs ? MF_ENABLED : MF_GRAYED);
		if(pMenu->GetMenuState(ID_FILE_SAVE,MF_BYCOMMAND)!=0xFFFFFFFF)
			pMenu->EnableMenuItem(ID_FILE_SAVE,m_bFileSave ? MF_ENABLED : MF_GRAYED);
		if(pMenu->GetMenuState(ID_FILE_MOVE,MF_BYCOMMAND)!=0xFFFFFFFF)
			pMenu->EnableMenuItem(ID_FILE_MOVE,m_bFileMove ? MF_ENABLED : MF_GRAYED);
		if(pMenu->GetMenuState(ID_FILE_DELETE,MF_BYCOMMAND)!=0xFFFFFFFF)
			pMenu->EnableMenuItem(ID_FILE_DELETE,m_bFileDelete ? MF_ENABLED : MF_GRAYED);
		
		// Edit
		if(pMenu->GetMenuState(ID_EDIT_COPY,MF_BYCOMMAND)!=0xFFFFFFFF)
			pMenu->EnableMenuItem(ID_EDIT_COPY,m_bEditCopy ? MF_ENABLED : MF_GRAYED);
		if(pMenu->GetMenuState(ID_EDIT_PASTE,MF_BYCOMMAND)!=0xFFFFFFFF)
			pMenu->EnableMenuItem(ID_EDIT_PASTE,m_bEditPaste && ::IsClipboardFormatAvailable(CF_DIB) ? MF_ENABLED : MF_GRAYED);
		if(pMenu->GetMenuState(ID_EDIT_UNDO,MF_BYCOMMAND)!=0xFFFFFFFF)
			pMenu->EnableMenuItem(ID_EDIT_UNDO,m_bEditUndo ? MF_ENABLED : MF_GRAYED);
		if(pMenu->GetMenuState(ID_EDIT_REDO,MF_BYCOMMAND)!=0xFFFFFFFF)
			pMenu->EnableMenuItem(ID_EDIT_REDO,m_bEditRedo ? MF_ENABLED : MF_GRAYED);

		// View
		if(pMenu->GetMenuState(ID_VIEW_PREVIOUS_PICTURE,MF_BYCOMMAND)!=0xFFFFFFFF)
			pMenu->EnableMenuItem(ID_VIEW_PREVIOUS_PICTURE,m_bViewPrevious ? MF_ENABLED : MF_GRAYED);
		if(pMenu->GetMenuState(ID_VIEW_NEXT_PICTURE,MF_BYCOMMAND)!=0xFFFFFFFF)
			pMenu->EnableMenuItem(ID_VIEW_NEXT_PICTURE,m_bViewNext ? MF_ENABLED : MF_GRAYED);

		if(pMenu->GetMenuState(ID_VIEW_ZOOMIN,MF_BYCOMMAND)!=0xFFFFFFFF)
			pMenu->EnableMenuItem(ID_VIEW_ZOOMIN,m_nViewType==VIEWTYPE_SCROLL ? MF_ENABLED : MF_GRAYED);
		if(pMenu->GetMenuState(ID_VIEW_ZOOMOUT,MF_BYCOMMAND)!=0xFFFFFFFF)
			pMenu->EnableMenuItem(ID_VIEW_ZOOMOUT,m_nViewType==VIEWTYPE_SCROLL ? MF_ENABLED : MF_GRAYED);
		if(pMenu->GetMenuState(ID_VIEW_ZOOMRESET,MF_BYCOMMAND)!=0xFFFFFFFF)
			pMenu->EnableMenuItem(ID_VIEW_ZOOMRESET,m_nViewType==VIEWTYPE_SCROLL ? MF_ENABLED : MF_GRAYED);

		if(pMenu->GetMenuState(ID_VIEW_FILESPLITTER,MF_BYCOMMAND)!=0xFFFFFFFF)
		{
			int nCurrent = 0,nMin = 0;
			m_wndFileSplitter.GetRowInfo(0,nCurrent,nMin);
			pMenu->CheckMenuItem(ID_VIEW_FILESPLITTER,nCurrent > 0 ? MF_CHECKED : MF_UNCHECKED);
		}
		if(pMenu->GetMenuState(ID_VIEW_DIRSPLITTER,MF_BYCOMMAND)!=0xFFFFFFFF)
		{
			int nCurrent = 0,nMin = 0;
			m_wndDirSplitter.GetColumnInfo(0,nCurrent,nMin);
			pMenu->CheckMenuItem(ID_VIEW_DIRSPLITTER,nCurrent > 0 ? MF_CHECKED : MF_UNCHECKED);
		}
		if(pMenu->GetMenuState(ID_VIEW_STRETCH,MF_BYCOMMAND)!=0xFFFFFFFF)
			pMenu->CheckMenuItem(ID_VIEW_STRETCH,m_nViewType==0 ? MF_CHECKED : MF_UNCHECKED);
		if(pMenu->GetMenuState(ID_VIEW_SCROLL,MF_BYCOMMAND)!=0xFFFFFFFF)
			pMenu->CheckMenuItem(ID_VIEW_SCROLL,m_nViewType==1 ? MF_CHECKED : MF_UNCHECKED);

		// Tools
		if(pMenu->GetMenuState(ID_TOOLS_SETWALLPAPER,MF_BYCOMMAND)!=0xFFFFFFFF)
			pMenu->EnableMenuItem(ID_TOOLS_SETWALLPAPER,m_bWallPaper? MF_ENABLED : MF_GRAYED);
		if(pMenu->GetMenuState(ID_TOOLS_RESETWALLPAPER,MF_BYCOMMAND)!=0xFFFFFFFF)
			pMenu->EnableMenuItem(ID_TOOLS_RESETWALLPAPER,m_bWallPaper? MF_ENABLED : MF_GRAYED);

		BOOL bLoaded = FALSE;
		CWallBrowserDoc* pDoc = (CWallBrowserDoc*)GetActiveDocument();
		if(pDoc)
			bLoaded = pDoc->GetPictureFlag();

		// Image
		if(pMenu->GetMenuState(ID_IMAGE_MIRROR_H,MF_BYCOMMAND)!=0xFFFFFFFF)
			pMenu->EnableMenuItem(ID_IMAGE_MIRROR_H,bLoaded ? (m_bImageMirrorH ? MF_ENABLED : MF_GRAYED) : MF_GRAYED);
		if(pMenu->GetMenuState(ID_IMAGE_MIRROR_V,MF_BYCOMMAND)!=0xFFFFFFFF)
			pMenu->EnableMenuItem(ID_IMAGE_MIRROR_V,bLoaded ? (m_bImageMirrorV ? MF_ENABLED : MF_GRAYED) : MF_GRAYED);
		if(pMenu->GetMenuState(ID_IMAGE_ROTATE_90_LEFT,MF_BYCOMMAND)!=0xFFFFFFFF)
			pMenu->EnableMenuItem(ID_IMAGE_ROTATE_90_LEFT,bLoaded ? (m_bImageRotate90Left ? MF_ENABLED : MF_GRAYED) : MF_GRAYED);
		if(pMenu->GetMenuState(ID_IMAGE_ROTATE_90_RIGHT,MF_BYCOMMAND)!=0xFFFFFFFF)
			pMenu->EnableMenuItem(ID_IMAGE_ROTATE_90_RIGHT,bLoaded ? (m_bImageRotate90Right ? MF_ENABLED : MF_GRAYED) : MF_GRAYED);
		if(pMenu->GetMenuState(ID_IMAGE_ROTATE_180,MF_BYCOMMAND)!=0xFFFFFFFF)
			pMenu->EnableMenuItem(ID_IMAGE_ROTATE_180,bLoaded ? (m_bImageRotate180 ? MF_ENABLED : MF_GRAYED) : MF_GRAYED);
		if(pMenu->GetMenuState(ID_IMAGE_DESKEW,MF_BYCOMMAND)!=0xFFFFFFFF)
			pMenu->EnableMenuItem(ID_IMAGE_DESKEW,bLoaded ? (m_bImageDeskew ? MF_ENABLED : MF_GRAYED) : MF_GRAYED);
		if(pMenu->GetMenuState(ID_IMAGE_SIZE,MF_BYCOMMAND)!=0xFFFFFFFF)
			pMenu->EnableMenuItem(ID_IMAGE_SIZE,bLoaded ? (m_bImageSize ? MF_ENABLED : MF_GRAYED) : MF_GRAYED);
		if(pMenu->GetMenuState(ID_IMAGE_POSTERIZE,MF_BYCOMMAND)!=0xFFFFFFFF)
			pMenu->EnableMenuItem(ID_IMAGE_POSTERIZE,bLoaded ? (m_bImagePosterize ? MF_ENABLED : MF_GRAYED) : MF_GRAYED);
		if(pMenu->GetMenuState(ID_IMAGE_MOSAIC,MF_BYCOMMAND)!=0xFFFFFFFF)
			pMenu->EnableMenuItem(ID_IMAGE_MOSAIC,bLoaded ? (m_bImageMosaic ? MF_ENABLED : MF_GRAYED) : MF_GRAYED);
		if(pMenu->GetMenuState(ID_IMAGE_BLUR,MF_BYCOMMAND)!=0xFFFFFFFF)
			pMenu->EnableMenuItem(ID_IMAGE_BLUR,bLoaded ? (m_bImageBlur ? MF_ENABLED : MF_GRAYED) : MF_GRAYED);
		if(pMenu->GetMenuState(ID_IMAGE_MEDIAN,MF_BYCOMMAND)!=0xFFFFFFFF)
			pMenu->EnableMenuItem(ID_IMAGE_MEDIAN,bLoaded ? (m_bImageMedian ? MF_ENABLED : MF_GRAYED) : MF_GRAYED);
		if(pMenu->GetMenuState(ID_IMAGE_SHARPEN,MF_BYCOMMAND)!=0xFFFFFFFF)
			pMenu->EnableMenuItem(ID_IMAGE_SHARPEN,bLoaded ? (m_bImageSharpen ? MF_ENABLED : MF_GRAYED) : MF_GRAYED);
		if(pMenu->GetMenuState(ID_IMAGE_DESPECKLE,MF_BYCOMMAND)!=0xFFFFFFFF)
			pMenu->EnableMenuItem(ID_IMAGE_DESPECKLE,bLoaded ? (m_bImageDespeckle ? MF_ENABLED : MF_GRAYED) : MF_GRAYED);
		if(pMenu->GetMenuState(ID_IMAGE_NOISE,MF_BYCOMMAND)!=0xFFFFFFFF)
			pMenu->EnableMenuItem(ID_IMAGE_NOISE,bLoaded ? (m_bImageNoise ? MF_ENABLED : MF_GRAYED) : MF_GRAYED);
		if(pMenu->GetMenuState(ID_IMAGE_EMBOSS,MF_BYCOMMAND)!=0xFFFFFFFF)
			pMenu->EnableMenuItem(ID_IMAGE_EMBOSS,bLoaded ? (m_bImageEmboss ? MF_ENABLED : MF_GRAYED) : MF_GRAYED);
		if(pMenu->GetMenuState(ID_IMAGE_EDGE_ENHANCE,MF_BYCOMMAND)!=0xFFFFFFFF)
			pMenu->EnableMenuItem(ID_IMAGE_EDGE_ENHANCE,bLoaded ? (m_bImageEdgeEnhance ? MF_ENABLED : MF_GRAYED) : MF_GRAYED);
		if(pMenu->GetMenuState(ID_IMAGE_FIND_EDGE,MF_BYCOMMAND)!=0xFFFFFFFF)
			pMenu->EnableMenuItem(ID_IMAGE_FIND_EDGE,bLoaded ? (m_bImageFindEdge ? MF_ENABLED : MF_GRAYED) : MF_GRAYED);
		if(pMenu->GetMenuState(ID_IMAGE_EROSION,MF_BYCOMMAND)!=0xFFFFFFFF)
			pMenu->EnableMenuItem(ID_IMAGE_EROSION,bLoaded ? (m_bImageErosion ? MF_ENABLED : MF_GRAYED) : MF_GRAYED);
		if(pMenu->GetMenuState(ID_IMAGE_DILATE,MF_BYCOMMAND)!=0xFFFFFFFF)
			pMenu->EnableMenuItem(ID_IMAGE_DILATE,bLoaded ? (m_bImageDilate ? MF_ENABLED : MF_GRAYED) : MF_GRAYED);

		// Color
		if(pMenu->GetMenuState(ID_COLOR_HALFTONE,MF_BYCOMMAND)!=0xFFFFFFFF)
			pMenu->EnableMenuItem(ID_COLOR_HALFTONE,bLoaded ? (m_bColorHalftone ? MF_ENABLED : MF_GRAYED) : MF_GRAYED);
		if(pMenu->GetMenuState(ID_COLOR_GRAYSCALE,MF_BYCOMMAND)!=0xFFFFFFFF)
			pMenu->EnableMenuItem(ID_COLOR_GRAYSCALE,bLoaded ? (m_bColorGrayscale ? MF_ENABLED : MF_GRAYED) : MF_GRAYED);
		if(pMenu->GetMenuState(ID_COLOR_INVERT,MF_BYCOMMAND)!=0xFFFFFFFF)
			pMenu->EnableMenuItem(ID_COLOR_INVERT,bLoaded ? (m_bColorInvert ? MF_ENABLED : MF_GRAYED) : MF_GRAYED);
		if(pMenu->GetMenuState(ID_COLOR_BRIGHTNESS,MF_BYCOMMAND)!=0xFFFFFFFF)
			pMenu->EnableMenuItem(ID_COLOR_BRIGHTNESS,bLoaded ? (m_bColorBrightness ? MF_ENABLED : MF_GRAYED) : MF_GRAYED);
		if(pMenu->GetMenuState(ID_COLOR_CONTRAST,MF_BYCOMMAND)!=0xFFFFFFFF)
			pMenu->EnableMenuItem(ID_COLOR_CONTRAST,bLoaded ? (m_bColorContrast ? MF_ENABLED : MF_GRAYED) : MF_GRAYED);
		if(pMenu->GetMenuState(ID_COLOR_HISTOGRAM_CONTRAST,MF_BYCOMMAND)!=0xFFFFFFFF)
			pMenu->EnableMenuItem(ID_COLOR_HISTOGRAM_CONTRAST,bLoaded ? (m_bColorHistogramContrast ? MF_ENABLED : MF_GRAYED) : MF_GRAYED);
		if(pMenu->GetMenuState(ID_COLOR_HUE,MF_BYCOMMAND)!=0xFFFFFFFF)
			pMenu->EnableMenuItem(ID_COLOR_HUE,bLoaded ? (m_bColorHue ? MF_ENABLED : MF_GRAYED) : MF_GRAYED);
		if(pMenu->GetMenuState(ID_COLOR_SATURATION_H,MF_BYCOMMAND)!=0xFFFFFFFF)
			pMenu->EnableMenuItem(ID_COLOR_SATURATION_H,bLoaded ? (m_bColorSaturationH ? MF_ENABLED : MF_GRAYED) : MF_GRAYED);
		if(pMenu->GetMenuState(ID_COLOR_SATURATION_V,MF_BYCOMMAND)!=0xFFFFFFFF)
			pMenu->EnableMenuItem(ID_COLOR_SATURATION_V,bLoaded ? (m_bColorSaturationV ? MF_ENABLED : MF_GRAYED) : MF_GRAYED);
		if(pMenu->GetMenuState(ID_COLOR_EQUALIZE,MF_BYCOMMAND)!=0xFFFFFFFF)
			pMenu->EnableMenuItem(ID_COLOR_EQUALIZE,bLoaded ? (m_bColorEqualize ? MF_ENABLED : MF_GRAYED) : MF_GRAYED);
		if(pMenu->GetMenuState(ID_COLOR_INTENSITY,MF_BYCOMMAND)!=0xFFFFFFFF)
			pMenu->EnableMenuItem(ID_COLOR_INTENSITY,bLoaded ? (m_bColorIntensity ? MF_ENABLED : MF_GRAYED) : MF_GRAYED);
		if(pMenu->GetMenuState(ID_COLOR_INTENSITY_DETECT,MF_BYCOMMAND)!=0xFFFFFFFF)
			pMenu->EnableMenuItem(ID_COLOR_INTENSITY_DETECT,bLoaded ? (m_bColorIntensityDetect ? MF_ENABLED : MF_GRAYED) : MF_GRAYED);
		if(pMenu->GetMenuState(ID_COLOR_GAMMA_CORRECTION,MF_BYCOMMAND)!=0xFFFFFFFF)
			pMenu->EnableMenuItem(ID_COLOR_GAMMA_CORRECTION,bLoaded ? (m_bColorGammaCorrection ? MF_ENABLED : MF_GRAYED) : MF_GRAYED);

		// Options
		CWinAppEx* pWinAppEx = (CWinAppEx*)AfxGetApp();
		if(pWinAppEx)
		{
			if(pMenu->GetMenuState(ID_OPTIONS_AUTOSIZECOLS,MF_BYCOMMAND)!=0xFFFFFFFF)
				pMenu->CheckMenuItem(ID_OPTIONS_AUTOSIZECOLS,m_bAutoSizeCols ? MF_CHECKED : MF_UNCHECKED);
			if(pMenu->GetMenuState(ID_OPTIONS_DRAWWITHSTRETCHDIBITS,MF_BYCOMMAND)!=0xFFFFFFFF)
				pMenu->CheckMenuItem(ID_OPTIONS_DRAWWITHSTRETCHDIBITS,m_nDrawMode==DRAW_STRETCHDIBITS ? MF_CHECKED : MF_UNCHECKED);
			if(pMenu->GetMenuState(ID_OPTIONS_DRAWWITHSTRETCHBLT,MF_BYCOMMAND)!=0xFFFFFFFF)
				pMenu->CheckMenuItem(ID_OPTIONS_DRAWWITHSTRETCHBLT,m_nDrawMode==DRAW_STRETCHBLT ? MF_CHECKED : MF_UNCHECKED);
			if(pMenu->GetMenuState(ID_OPTIONS_DRAWWITHVFW,MF_BYCOMMAND)!=0xFFFFFFFF)
				pMenu->CheckMenuItem(ID_OPTIONS_DRAWWITHVFW,m_nDrawMode==DRAW_VFWDRAWDIB ? MF_CHECKED : MF_UNCHECKED);
			if(pMenu->GetMenuState(ID_OPTIONS_CONFIRMFILEDELETE,MF_BYCOMMAND)!=0xFFFFFFFF)
				pMenu->CheckMenuItem(ID_OPTIONS_CONFIRMFILEDELETE,m_bConfirmFileDelete ? MF_CHECKED : MF_UNCHECKED);
			if(pMenu->GetMenuState(ID_OPTIONS_DELETETORECYCLE,MF_BYCOMMAND)!=0xFFFFFFFF)
				pMenu->CheckMenuItem(ID_OPTIONS_DELETETORECYCLE,m_bDeleteToRecycleBin ? MF_CHECKED : MF_UNCHECKED);
		}
	}

	CFrameWnd::OnInitMenuPopup(pMenu,nIndex,bSysMenu);
}

/*
	OnCmdMsg()
*/
BOOL CMainFrame::OnCmdMsg(UINT nID,int nCode,void* pExtra,AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// route to standard command targets first
	if(CFrameWnd::OnCmdMsg(nID,nCode,pExtra,pHandlerInfo))
		return(TRUE);

	// route to inactive views second
	CWallBrowserDoc* pDoc = (CWallBrowserDoc*)GetActiveDocument();
	if(pDoc)
	{
		if(pDoc->RouteCmdToAllViews(GetActiveView(),nID,nCode,pExtra,pHandlerInfo))
		return(TRUE);
	}

	return(FALSE);
}

/*
	OnUpdateFrameTitle()
*/
void CMainFrame::OnUpdateFrameTitle(BOOL /*bAddToTitle*/)
{
	if((GetStyle() & FWS_ADDTOTITLE)==0)
		return;

	::SetWindowText(m_hWnd,m_strTitle);
}

/*
	OnViewScroll()
*/
void CMainFrame::OnViewScroll(void)
{
	// imposta il tipo di vista
	m_nViewType = VIEWTYPE_SCROLL;

	// deve salvare la configurazione dato che le viste recuperano i valori nello stesso modo
	CWinAppEx* pWinAppEx = (CWinAppEx*)AfxGetApp();
	if(pWinAppEx)
	{
		pWinAppEx->m_Config.UpdateNumber(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_VIEWTYPE_KEY,m_nViewType);
		pWinAppEx->m_Config.SaveKey(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_VIEWTYPE_KEY);
	}

	// notifica il cambio alla vista
	CWallBrowserDoc* pDoc = (CWallBrowserDoc*)GetActiveDocument();
	if(pDoc)
	{
		TRACE("%s(%d): sending ONUPDATE_FLAG_VIEWTYPE\n",__FILE__,__LINE__);
		pDoc->UpdateAllViews(NULL,ONUPDATE_FLAG_VIEWTYPE);
	}
}

/*
	OnViewStretch()
*/
void CMainFrame::OnViewStretch(void)
{
	// imposta il tipo di vista
	m_nViewType = VIEWTYPE_STRETCH;

	// deve salvare la configurazione dato che le viste recuperano i valori nello stesso modo
	CWinAppEx* pWinAppEx = (CWinAppEx*)AfxGetApp();
	if(pWinAppEx)
	{
		pWinAppEx->m_Config.UpdateNumber(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_VIEWTYPE_KEY,m_nViewType);
		pWinAppEx->m_Config.SaveKey(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_VIEWTYPE_KEY);
	}

	// notifica il cambio alla vista
	CWallBrowserDoc* pDoc = (CWallBrowserDoc*)GetActiveDocument();
	if(pDoc)
	{
		TRACE("%s(%d): sending ONUPDATE_FLAG_VIEWTYPE\n",__FILE__,__LINE__);
		pDoc->UpdateAllViews(NULL,ONUPDATE_FLAG_VIEWTYPE);
	}
}

/*
	OnViewToggleFileSplitter()
*/
LONG CMainFrame::OnViewToggleFileSplitter(UINT /*wParam*/,LONG /*lParam*/)
{
	// ricava la dimensione corrente
	int nCurrent = 0,nMin = 0;
	m_wndFileSplitter.GetRowInfo(0,nCurrent,nMin);
	
	// imposta la nuova dimensione
	if(nCurrent <= 0)
	{
		nCurrent = m_nFileSplitterCurrent;
		if(nCurrent <= 0)
			nCurrent = FILE_SPLITTER_SIZE;
		m_nFileSplitterCurrent = 0;
	}
	else if(nCurrent > 0)
	{
		m_nFileSplitterCurrent = nCurrent;
		nCurrent = 0;
	}

	// aggiorna
	m_wndFileSplitter.SetRowInfo(0,nCurrent,FILE_SPLITTER_MINSIZE);
	m_wndFileSplitter.RecalcLayout();

	return(0L);
}

/*
	OnViewToggleDirSplitter()
*/
LONG CMainFrame::OnViewToggleDirSplitter(UINT /*wParam*/,LONG /*lParam*/)
{
	// ricava la dimensione corrente
	int nCurrent = 0,nMin = 0;
	m_wndDirSplitter.GetColumnInfo(0,nCurrent,nMin);
	
	// imposta la nuova dimensione
	if(nCurrent <= 0)
	{
		nCurrent = m_nDirSplitterCurrent;
		if(nCurrent <= 0)
			nCurrent = DIR_SPLITTER_SIZE;
		m_nDirSplitterCurrent = 0;
	}
	else if(nCurrent > 0)
	{
		m_nDirSplitterCurrent = nCurrent;
		nCurrent = 0;
	}

	// aggiorna
	m_wndDirSplitter.SetColumnInfo(0,nCurrent,DIR_SPLITTER_MINSIZE);
	m_wndDirSplitter.RecalcLayout();

	return(0L);
}

/*
	OnViewZoomIn()
*/
void CMainFrame::OnViewZoomIn(void)
{
	// notifica il cambio alla vista
	CWallBrowserDoc* pDoc = (CWallBrowserDoc*)GetActiveDocument();
	if(pDoc)
	{
		TRACE("%s(%d): sending ONUPDATE_FLAG_ZOOMIN\n",__FILE__,__LINE__);
		pDoc->UpdateAllViews(NULL,ONUPDATE_FLAG_ZOOMIN);
	}
}

/*
	OnViewZoomOut()
*/
void CMainFrame::OnViewZoomOut(void)
{
	// notifica il cambio alla vista
	CWallBrowserDoc* pDoc = (CWallBrowserDoc*)GetActiveDocument();
	if(pDoc)
	{
		TRACE("%s(%d): sending ONUPDATE_FLAG_ZOOMOUT\n",__FILE__,__LINE__);
		pDoc->UpdateAllViews(NULL,ONUPDATE_FLAG_ZOOMOUT);
	}
}

/*
	OnViewZoomReset()
*/
void CMainFrame::OnViewZoomReset(void)
{
	// notifica il cambio alla vista
	CWallBrowserDoc* pDoc = (CWallBrowserDoc*)GetActiveDocument();
	if(pDoc)
	{
		TRACE("%s(%d): sending ONUPDATE_FLAG_ZOOMRESET\n",__FILE__,__LINE__);
		pDoc->UpdateAllViews(NULL,ONUPDATE_FLAG_ZOOMRESET);
	}
}

/*
	OnGetFileSplitterSize()
*/
LONG CMainFrame::OnGetFileSplitterSize(UINT /*wParam*/,LONG /*lParam*/)
{
	int nCurrent = 0,nMin = 0;
	m_wndDirSplitter.GetRowInfo(0,nCurrent,nMin);
	return((LONG)nCurrent);
}

/*
	OnGetDirSplitterSize()
*/
LONG CMainFrame::OnGetDirSplitterSize(UINT /*wParam*/,LONG /*lParam*/)
{
	int nCurrent = 0,nMin = 0;
	m_wndDirSplitter.GetColumnInfo(0,nCurrent,nMin);
	return((LONG)nCurrent);
}

/*
	OnToolsLibrary()
*/
LONG CMainFrame::OnToolsLibrary(UINT /*wParam*/,LONG lParam)
{
	// imposta la libreria corrente
	CWallBrowserDoc* pDoc = (CWallBrowserDoc*)GetActiveDocument();
	if(pDoc)
		pDoc->SetLibrary((LPCSTR)lParam);
	
	return(0L);
}

/*
	OnOptionsAutoSizeCols()
*/
void CMainFrame::OnOptionsAutoSizeCols(void)
{
	m_bAutoSizeCols = !m_bAutoSizeCols;

	// deve salvare la configurazione dato che le viste recuperano i valori nello stesso modo
	CWinAppEx* pWinAppEx = (CWinAppEx*)AfxGetApp();
	if(pWinAppEx)
	{
		pWinAppEx->m_Config.UpdateNumber(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_AUTOSIZECOLUMNS_KEY,m_bAutoSizeCols);
		pWinAppEx->m_Config.SaveKey(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_AUTOSIZECOLUMNS_KEY);
	}
}

/*
	OnOptionsTransparency()
*/
void CMainFrame::OnOptionsTransparency(void)
{
	CWinAppEx* pWinAppEx = (CWinAppEx*)AfxGetApp();
	if(pWinAppEx)
	{
		CWallBrowserConfig* pConfig = &(pWinAppEx->m_Config);
		CWallBrowserLayeredDlg dlg(this,pConfig);
		if(dlg.DoModal()==IDOK)
		{
			// deve salvare la configurazione dato che le viste recuperano i valori nello stesso modo
			pWinAppEx->m_Config.UpdateNumber(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_LAYERED_KEY,dlg.m_bLayered);
			pWinAppEx->m_Config.SaveKey(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_LAYERED_KEY);
			pWinAppEx->m_Config.UpdateNumber(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_LAYEREDVALUE_KEY,dlg.m_nLayered);
			pWinAppEx->m_Config.SaveKey(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_LAYEREDVALUE_KEY);

			// aggiorna i valori per la trasparenza
			if(pWinAppEx->m_Config.GetNumber(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_LAYERED_KEY))
			{
				BYTE nLayered = (BYTE)pWinAppEx->m_Config.GetNumber(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_LAYEREDVALUE_KEY);
				if(nLayered > LWA_ALPHA_MIN && nLayered < LWA_ALPHA_MAX)
				{
					BOOL bLayer = TRUE;
					if(nLayered < LWA_ALPHA_INVISIBLE)
						bLayer = ::MessageBoxResourceEx(this->m_hWnd,MB_YESNO|MB_ICONQUESTION,WALLBROWSER_PROGRAM_NAME,IDS_QUESTION_LAYERED,nLayered,LWA_ALPHA_MIN,LWA_ALPHA_MAX)==IDYES;
					if(bLayer)
						m_wndLayered.SetLayer(this->m_hWnd,nLayered);
				}
				else
					m_wndLayered.Reset();
			}
			else
				m_wndLayered.Reset();
		}
	}
}

/*
	OnOptionsDrawWithStretchDIBits()
*/
void CMainFrame::OnOptionsDrawWithStretchDIBits(void)
{
	m_nDrawMode = DRAW_STRETCHDIBITS;

	OnSaveDrawFlag();

	// notifica il cambio alla vista
	CWallBrowserDoc* pDoc = (CWallBrowserDoc*)GetActiveDocument();
	if(pDoc)
	{
		TRACE("%s(%d): sending ONUPDATE_FLAG_DRAWSTRETCHDIBITS\n",__FILE__,__LINE__);
		pDoc->UpdateAllViews(NULL,ONUPDATE_FLAG_DRAWSTRETCHDIBITS);
	}
}

/*
	OnOptionsDrawWithStretchBlt()
*/
void CMainFrame::OnOptionsDrawWithStretchBlt(void)
{
	m_nDrawMode = DRAW_STRETCHBLT;

	OnSaveDrawFlag();

	// notifica il cambio alla vista
	CWallBrowserDoc* pDoc = (CWallBrowserDoc*)GetActiveDocument();
	if(pDoc)
	{
		TRACE("%s(%d): sending ONUPDATE_FLAG_DRAWSTRETCHBLT\n",__FILE__,__LINE__);
		pDoc->UpdateAllViews(NULL,ONUPDATE_FLAG_DRAWSTRETCHBLT);
	}
}

/*
	OnOptionsDrawWithVfw()
*/
void CMainFrame::OnOptionsDrawWithVfw(void)
{
	m_nDrawMode = DRAW_VFWDRAWDIB;

	OnSaveDrawFlag();

	// notifica il cambio alla vista
	CWallBrowserDoc* pDoc = (CWallBrowserDoc*)GetActiveDocument();
	if(pDoc)
	{
		TRACE("%s(%d): sending ONUPDATE_FLAG_DRAWVFWDRAWDIB\n",__FILE__,__LINE__);
		pDoc->UpdateAllViews(NULL,ONUPDATE_FLAG_DRAWVFWDRAWDIB);
	}
}

/*
	OnSaveDrawFlag()
*/
void CMainFrame::OnSaveDrawFlag(void)
{
	// deve salvare la configurazione dato che le viste recuperano i valori nello stesso modo
	CWinAppEx* pWinAppEx = (CWinAppEx*)AfxGetApp();
	if(pWinAppEx)
	{
		pWinAppEx->m_Config.UpdateNumber(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_DRAWMODE_KEY,m_nDrawMode);
		pWinAppEx->m_Config.SaveKey(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_DRAWMODE_KEY);
	}
}

/*
	OnOptionsConfirmFileDelete()
*/
void CMainFrame::OnOptionsConfirmFileDelete(void)
{
	m_bConfirmFileDelete = !m_bConfirmFileDelete;

	// deve salvare la configurazione dato che le viste recuperano i valori nello stesso modo
	CWinAppEx* pWinAppEx = (CWinAppEx*)AfxGetApp();
	if(pWinAppEx)
	{
		pWinAppEx->m_Config.UpdateNumber(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_CONFIRMFILEDELETE_KEY,m_bConfirmFileDelete);
		pWinAppEx->m_Config.SaveKey(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_CONFIRMFILEDELETE_KEY);
	}
}

/*
	OnOptionsDeleteToRecycleBin()
*/
void CMainFrame::OnOptionsDeleteToRecycleBin(void)
{
	m_bDeleteToRecycleBin = !m_bDeleteToRecycleBin;

	// deve salvare la configurazione dato che le viste recuperano i valori nello stesso modo
	CWinAppEx* pWinAppEx = (CWinAppEx*)AfxGetApp();
	if(pWinAppEx)
	{
		pWinAppEx->m_Config.UpdateNumber(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_DELETEFILESTORECYCLEBIN_KEY,m_bDeleteToRecycleBin);
		pWinAppEx->m_Config.SaveKey(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_DELETEFILESTORECYCLEBIN_KEY);
	}
}
