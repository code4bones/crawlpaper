/*
	MainFrm.h
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
#ifndef _MAINFRM_H
#define _MAINFRM_H 1

#include "window.h"
#include "CComboToolBar.h"
#include "CWinAppEx.h"
#include "CWndLayered.h"
#include "WallBrowserMessages.h"

#define COMBO_WIDTH			100
#define FILE_SPLITTER_SIZE	100
#define FILE_SPLITTER_MINSIZE	10
#define DIR_SPLITTER_SIZE	200
#define DIR_SPLITTER_MINSIZE	10

class CMainFrame : public CFrameWnd
{
	DECLARE_DYNCREATE(CMainFrame)

public:
	CMainFrame();
	virtual ~CMainFrame() {}

	int			OnCreate				(LPCREATESTRUCT lpcs);
	BOOL			OnCreateClient			(LPCREATESTRUCT lpcs,CCreateContext* pContext);
	void			OnClose				(void);
	void			OnInitMenuPopup		(CMenu* pMenu,UINT nIndex,BOOL bSysMenu);
	BOOL			OnCmdMsg				(UINT nID,int nCode,void* pExtra,AFX_CMDHANDLERINFO* pHandlerInfo);
	void			OnUpdateFrameTitle		(BOOL bAddToTitle);

	// command UI (per abilitare/disabilitare gli elementi della GUI)
	inline void	OnUIFileSaveAs			(CCmdUI* pCmdUI)			{pCmdUI->Enable(m_bFileSaveAs);}
	inline void	OnUIFileSave			(CCmdUI* pCmdUI)			{pCmdUI->Enable(m_bFileSave);}
	inline void	OnUIFileMove			(CCmdUI* pCmdUI)			{pCmdUI->Enable(m_bFileMove);}
	inline void	OnUIFileDelete			(CCmdUI* pCmdUI)			{pCmdUI->Enable(m_bFileDelete);}
	inline void	OnUIEditCopy			(CCmdUI* pCmdUI)			{pCmdUI->Enable(m_bEditCopy);}
	inline void	OnUIEditPaste			(CCmdUI* pCmdUI)			{pCmdUI->Enable(m_bEditPaste && ::IsClipboardFormatAvailable(CF_DIB));}
	inline void	OnUIEditUndo			(CCmdUI* pCmdUI)			{pCmdUI->Enable(m_bEditUndo);}
	inline void	OnUIEditRedo			(CCmdUI* pCmdUI)			{pCmdUI->Enable(m_bEditRedo);}
	inline void	OnUIViewPrevious		(CCmdUI* pCmdUI)			{pCmdUI->Enable(m_bViewPrevious);}
	inline void	OnUIViewNext			(CCmdUI* pCmdUI)			{pCmdUI->Enable(m_bViewNext);}
	inline void	OnUIViewZoomIn			(CCmdUI* pCmdUI)			{pCmdUI->Enable(m_nViewType==VIEWTYPE_SCROLL);}
	inline void	OnUIViewZoomOut		(CCmdUI* pCmdUI)			{pCmdUI->Enable(m_nViewType==VIEWTYPE_SCROLL);}
	inline void	OnUIViewZoomReset		(CCmdUI* pCmdUI)			{pCmdUI->Enable(m_nViewType==VIEWTYPE_SCROLL);}
	inline void	OnUIToolsSetWallPaper	(CCmdUI* pCmdUI)			{pCmdUI->Enable(m_bWallPaper);}
	inline void	OnUIToolsResetWallPaper	(CCmdUI* pCmdUI)			{pCmdUI->Enable(m_bWallPaper);}
	
	// gestori dei comandi
	inline LONG	OnFileSaveAs			(UINT wParam,LONG /*lParam*/)	{m_bFileSaveAs = (BOOL)wParam; return(0L);}
	inline LONG	OnFileSave			(UINT wParam,LONG /*lParam*/)	{m_bFileSave = (BOOL)wParam; return(0L);}
	inline LONG	OnFileMove			(UINT wParam,LONG /*lParam*/)	{m_bFileMove = (BOOL)wParam; return(0L);}
	inline LONG	OnFileDelete			(UINT wParam,LONG /*lParam*/)	{m_bFileDelete = (BOOL)wParam; return(0L);}

	inline LONG	OnEditCopy			(UINT wParam,LONG /*lParam*/)	{m_bEditCopy = (BOOL)wParam; return(0L);}
	inline LONG	OnEditPaste			(UINT wParam,LONG /*lParam*/)	{m_bEditPaste = (BOOL)wParam; return(0L);}
	inline LONG	OnEditUndo			(UINT wParam,LONG /*lParam*/)	{m_bEditUndo = (BOOL)wParam; return(0L);}
	inline LONG	OnEditRedo			(UINT wParam,LONG /*lParam*/)	{m_bEditRedo = (BOOL)wParam; return(0L);}

	void			OnViewScroll			(void);
	void			OnViewStretch			(void);
	inline void	OnViewFileSplitter		(void)					{SendMessage(WM_TOGGLE_FILESPLITTER,0,0L);}
	inline void	OnViewDirSplitter		(void)					{SendMessage(WM_TOGGLE_DIRSPLITTER,0,0L);}
	LONG			OnViewToggleFileSplitter	(UINT wParam,LONG lParam);
	LONG			OnViewToggleDirSplitter	(UINT wParam,LONG lParam);
	inline LONG	OnViewPrevious			(UINT wParam,LONG /*lParam*/)	{m_bViewPrevious = (BOOL)wParam; return(0L);}
	inline LONG	OnViewNext			(UINT wParam,LONG /*lParam*/)	{m_bViewNext = (BOOL)wParam; return(0L);}
	void			OnViewZoomIn			(void);
	void			OnViewZoomOut			(void);
	void			OnViewZoomReset		(void);

	inline LRESULT	OnGetStatusBar			(WPARAM /*wParam*/,LPARAM /*lParam*/) {return((LRESULT)&m_wndStatusBar);}
	LONG			OnGetFileSplitterSize	(UINT wParam,LONG lParam);
	LONG			OnGetDirSplitterSize	(UINT wParam,LONG lParam);

	inline LONG	OnToolsSetWallPaper		(UINT wParam,LONG /*lParam*/)	{m_bWallPaper = (BOOL)wParam; return(0L);}
	inline LONG	OnToolsResetWallPaper	(UINT wParam,LONG /*lParam*/)	{m_bWallPaper = (BOOL)wParam; return(0L);}
	LONG			OnToolsLibrary			(UINT wParam,LONG lParam);
	
	inline LONG	OnImageMirrorH			(UINT wParam,LONG /*lParam*/) {m_bImageMirrorH = (BOOL)wParam; return(0L);}
	inline LONG	OnImageMirrorV			(UINT wParam,LONG /*lParam*/) {m_bImageMirrorV = (BOOL)wParam; return(0L);}
	inline LONG	OnImageRotate90Left		(UINT wParam,LONG /*lParam*/) {m_bImageRotate90Left = (BOOL)wParam; return(0L);}
	inline LONG	OnImageRotate90Right	(UINT wParam,LONG /*lParam*/) {m_bImageRotate90Right = (BOOL)wParam; return(0L);}
	inline LONG	OnImageRotate180		(UINT wParam,LONG /*lParam*/) {m_bImageRotate180 = (BOOL)wParam; return(0L);}
	inline LONG	OnImageDeskew			(UINT wParam,LONG /*lParam*/) {m_bImageDeskew = (BOOL)wParam; return(0L);}
	inline LONG	OnImageSize			(UINT wParam,LONG /*lParam*/) {m_bImageSize = (BOOL)wParam; return(0L);}
	inline LONG	OnImagePosterize		(UINT wParam,LONG /*lParam*/) {m_bImagePosterize = (BOOL)wParam; return(0L);}
	inline LONG	OnImageMosaic			(UINT wParam,LONG /*lParam*/) {m_bImageMosaic = (BOOL)wParam; return(0L);}
	inline LONG	OnImageBlur			(UINT wParam,LONG /*lParam*/) {m_bImageBlur = (BOOL)wParam; return(0L);}
	inline LONG	OnImageMedian			(UINT wParam,LONG /*lParam*/) {m_bImageMedian = (BOOL)wParam; return(0L);}
	inline LONG	OnImageSharpen			(UINT wParam,LONG /*lParam*/) {m_bImageSharpen = (BOOL)wParam; return(0L);}
	inline LONG	OnImageDespeckle		(UINT wParam,LONG /*lParam*/) {m_bImageDespeckle = (BOOL)wParam; return(0L);}
	inline LONG	OnImageNoise			(UINT wParam,LONG /*lParam*/) {m_bImageNoise = (BOOL)wParam; return(0L);}
	inline LONG	OnImageEmboss			(UINT wParam,LONG /*lParam*/) {m_bImageEmboss = (BOOL)wParam; return(0L);}
	inline LONG	OnImageEdgeEnhance		(UINT wParam,LONG /*lParam*/) {m_bImageEdgeEnhance = (BOOL)wParam; return(0L);}
	inline LONG	OnImageFindEdge		(UINT wParam,LONG /*lParam*/) {m_bImageFindEdge = (BOOL)wParam; return(0L);}
	inline LONG	OnImageErosion			(UINT wParam,LONG /*lParam*/) {m_bImageErosion = (BOOL)wParam; return(0L);}
	inline LONG	OnImageDilate			(UINT wParam,LONG /*lParam*/) {m_bImageDilate = (BOOL)wParam; return(0L);}
	
	inline LONG	OnColorHalftone		(UINT wParam,LONG /*lParam*/) {m_bColorHalftone = (BOOL)wParam; return(0L);}
	inline LONG	OnColorGrayscale		(UINT wParam,LONG /*lParam*/) {m_bColorGrayscale = (BOOL)wParam; return(0L);}
	inline LONG	OnColorInvert			(UINT wParam,LONG /*lParam*/) {m_bColorInvert = (BOOL)wParam; return(0L);}
	inline LONG	OnColorBrightness		(UINT wParam,LONG /*lParam*/) {m_bColorBrightness = (BOOL)wParam; return(0L);}
	inline LONG	OnColorContrast		(UINT wParam,LONG /*lParam*/) {m_bColorContrast = (BOOL)wParam; return(0L);}
	inline LONG	OnColorHistogramContrast	(UINT wParam,LONG /*lParam*/) {m_bColorHistogramContrast = (BOOL)wParam; return(0L);}
	inline LONG	OnColorHue			(UINT wParam,LONG /*lParam*/) {m_bColorHue = (BOOL)wParam; return(0L);}
	inline LONG	OnColorSaturationH		(UINT wParam,LONG /*lParam*/) {m_bColorSaturationH = (BOOL)wParam; return(0L);}
	inline LONG	OnColorSaturationV		(UINT wParam,LONG /*lParam*/) {m_bColorSaturationV = (BOOL)wParam; return(0L);}
	inline LONG	OnColorEqualize		(UINT wParam,LONG /*lParam*/) {m_bColorEqualize = (BOOL)wParam; return(0L);}
	inline LONG	OnColorIntensity		(UINT wParam,LONG /*lParam*/) {m_bColorIntensity = (BOOL)wParam; return(0L);}
	inline LONG	OnColorIntensityDetect	(UINT wParam,LONG /*lParam*/) {m_bColorIntensityDetect = (BOOL)wParam; return(0L);}
	inline LONG	OnColorGammaCorrection	(UINT wParam,LONG /*lParam*/) {m_bColorGammaCorrection = (BOOL)wParam; return(0L);}

	void			OnOptionsAutoSizeCols		(void);
	void			OnOptionsTransparency		(void);
	void			OnOptionsDrawWithStretchDIBits(void);
	void			OnOptionsDrawWithStretchBlt	(void);
	void			OnOptionsDrawWithVfw		(void);
	void			OnSaveDrawFlag				(void);
	void			OnOptionsConfirmFileDelete	(void);
	void			OnOptionsDeleteToRecycleBin	(void);

private:
	CWndLayered	m_wndLayered;

	CSplitterWnd	m_wndDirSplitter;
	CSplitterWnd	m_wndFileSplitter;
	int			m_nFileSplitterCurrent;
	int			m_nDirSplitterCurrent;

	CComboToolBar	m_wndMainToolBar;
	CStatusBar	m_wndStatusBar;

	BOOL			m_bFileSaveAs;
	BOOL			m_bFileSave;
	BOOL			m_bFileMove;
	BOOL			m_bFileDelete;
	BOOL			m_bEditCopy;
	BOOL			m_bEditPaste;
	BOOL			m_bEditUndo;
	BOOL			m_bEditRedo;
	BOOL			m_bViewPrevious;
	BOOL			m_bViewNext;
	int			m_nViewType;
	BOOL			m_bWallPaper;
	BOOL			m_nDrawMode;
	BOOL			m_bConfirmFileDelete;
	BOOL			m_bDeleteToRecycleBin;
	BOOL			m_bAutoSizeCols;

	BOOL			m_bImageMirrorH;
	BOOL			m_bImageMirrorV;
	BOOL			m_bImageRotate90Left;
	BOOL			m_bImageRotate90Right;
	BOOL			m_bImageRotate180;
	BOOL			m_bImageDeskew;
	BOOL			m_bImageSize;
	BOOL			m_bImagePosterize;
	BOOL			m_bImageMosaic;
	BOOL			m_bImageBlur;
	BOOL			m_bImageMedian;
	BOOL			m_bImageSharpen;
	BOOL			m_bImageDespeckle;
	BOOL			m_bImageNoise;
	BOOL			m_bImageEmboss;
	BOOL			m_bImageEdgeEnhance;
	BOOL			m_bImageFindEdge;
	BOOL			m_bImageErosion;
	BOOL			m_bImageDilate;
	
	BOOL			m_bColorHalftone;
	BOOL			m_bColorGrayscale;
	BOOL			m_bColorInvert;
	BOOL			m_bColorBrightness;
	BOOL			m_bColorContrast;
	BOOL			m_bColorHistogramContrast;
	BOOL			m_bColorHue;
	BOOL			m_bColorSaturationH;
	BOOL			m_bColorSaturationV;
	BOOL			m_bColorEqualize;
	BOOL			m_bColorIntensity;
	BOOL			m_bColorIntensityDetect;
	BOOL			m_bColorGammaCorrection;

	DECLARE_MESSAGE_MAP()
};

#endif // _MAINFRM_H
