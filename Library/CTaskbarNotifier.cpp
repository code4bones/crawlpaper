/*
	CTaskbarNotifier.cpp
	Classe per la finestra popup sulla taskbar (MFC).
	Ripresa e modificata dall'originale di John O'Byrne (TaskbarNotifier.cpp, CreateRgnFromBitmap() by Davide Pizzolato).
	Luca Piergentili, 13/06/03
	lpiergentili@yahoo.com
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include <stdlib.h>
#include "strings.h"
#include "window.h"
#include "win32api.h"
#include <shellapi.h>
#include "CWndLayered.h"
#include "CToolTipCtrlEx.h"
#include "CTaskbarNotifier.h"

// da definire nell'header (CTaskbarNotifier.h)
#ifdef _LOAD_ALL_IMAGE_FORMATS
  #include "CImageFactory.h"
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

// id di default per i timer interni
#define IDT_HIDDEN				-1
#define IDT_APPEARING			20
#define IDT_WAITING				21
#define IDT_DISAPPEARING			22
#define IDT_SCROLLING			23

// id per il tooltip
#define ID_TOOLTIP_CONTROL		1965

BEGIN_MESSAGE_MAP(CTaskbarNotifier,CWnd)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_SETCURSOR()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_MESSAGE(WM_MOUSEHOVER,OnMouseHover)
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
END_MESSAGE_MAP()

/*
	CTaskbarNotifier()
*/
CTaskbarNotifier::CTaskbarNotifier()
{
	m_bIsValid			= FALSE;
	m_bIsTopMost			= TRUE;
	m_pWndParent			= NULL;
	m_hWndParent			= NULL;
	m_nMsg				= 0L;
	m_nAnimStatus			= IDT_HIDDEN;
	m_nIDT_APPEARING		= IDT_APPEARING;
	m_nIDT_WAITING			= IDT_WAITING;
	m_nIDT_DISAPPEARING		= IDT_DISAPPEARING;
	m_nIDT_SCROLLING		= IDT_SCROLLING;
	m_nTimerAppearing		= (UINT)-1;
	m_nTimerWaiting		= (UINT)-1;
	m_nTimerDisappearing	= (UINT)-1;
	m_nTimerScrolling		= (UINT)-1;

	m_hCursor				= NULL;

	m_bHandleMouseClick		= FALSE;
	m_bMouseIsOver			= FALSE;

	m_strText				= "";
	m_strTextFiller		= "";
	m_rcText.SetRect(0,0,0,0);
	m_nDefaultTextFormat	= DT_LEFT|DT_NOPREFIX|DT_WORDBREAK|DT_END_ELLIPSIS; // allineamento (DT_LEFT) + fissi (DT_NOPREFIX | DT_WORDBREAK | DT_END_ELLIPSIS)
	m_crNormalTextColor		= RGB(133,146,181);
	m_crSelectedTextColor	= RGB(10,36,106);
	m_bScrollText			= FALSE;
	m_nScrollDirection		= 0;
	m_dwScrollSpeed		= 200L;
	memset(m_szToolTipText,'\0',sizeof(m_szToolTipText));

#ifdef _LOAD_ALL_IMAGE_FORMATS
	m_pImage				= NULL;
	memset(m_szLibraryName,'\0',sizeof(m_szLibraryName));
#endif
	m_bBitmapLoaded		= FALSE;
	m_hBitmapRegion		= NULL;
	m_nBitmapWidth			= 0;
	m_nBitmapHeight		= 0;
	
	m_nXPos				= -1;
	m_nYPos				= -1;
	m_nTaskbarPlacement		= ABE_BOTTOM;
	OSVERSIONINFO osvi = {0};
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	::GetVersionEx(&osvi);
	if(osvi.dwPlatformId==VER_PLATFORM_WIN32_NT)
		m_dwTimerPrecision	= 10L;
	else
		m_dwTimerPrecision	= 50L;
	m_dwTimeToStay			= 0L;
	m_dwShowEvents			= 0L;
	m_dwHideEvents			= 0L;
	m_nCurrentPosX			= 0;
	m_nCurrentPosY			= 0;
	m_nCurrentWidth		= 0;
	m_nCurrentHeight		= 0;
	m_nIncrementShow		= 0;
	m_nIncrementHide		= 0;
	
	SetDefaultFont();
}

/*
	~CTaskbarNotifier()
*/
CTaskbarNotifier::~CTaskbarNotifier()
{
	ResetTimers();
	ResetWindow();
	ResetBitmap();
	
	// per la derivazione da CWnd
	DestroyWindow();
}

/*
	ResetTimers()
*/
void CTaskbarNotifier::ResetTimers(void)
{
	if(!m_bIsValid)
		return;

	if(m_nTimerAppearing!=(UINT)-1)
	{
		::KillTimer(CWnd::m_hWnd,m_nTimerAppearing);
		m_nTimerAppearing = (UINT)-1;
	}
	if(m_nTimerWaiting!=(UINT)-1)
	{
		::KillTimer(CWnd::m_hWnd,m_nTimerWaiting);
		m_nTimerWaiting = (UINT)-1;
	}
	if(m_nTimerDisappearing!=(UINT)-1)
	{
		::KillTimer(CWnd::m_hWnd,m_nTimerDisappearing);
		m_nTimerDisappearing = (UINT)-1;
	}
	if(m_nTimerScrolling!=(UINT)-1)
	{
		::KillTimer(CWnd::m_hWnd,m_nTimerScrolling);
		m_nTimerScrolling = (UINT)-1;
	}
}

/*
	ResetWindow()
*/
void CTaskbarNotifier::ResetWindow(void)
{
	if(!m_bIsValid)
		return;

	if(::IsWindow(this->GetSafeHwnd()))
		ShowWindow(SW_HIDE);
	
	m_nAnimStatus = IDT_HIDDEN;
}

/*
	ResetBitmap()
*/
void CTaskbarNotifier::ResetBitmap(void)
{
	if(!m_bIsValid)
		return;

	m_bitmapBackground.DeleteObject();
	m_nBitmapWidth = m_nBitmapHeight = 0;
	m_bBitmapLoaded = FALSE;
}

/*
	Create()
*/
BOOL CTaskbarNotifier::Create(CWnd *pWndParent)
{
	if(m_bIsValid)
		return(TRUE);

	m_pWndParent = pWndParent;
	if(m_pWndParent)
		m_hWndParent = m_pWndParent->GetSafeHwnd();
	
	// registra la classe
	WNDCLASSEX wcx = {0};
	wcx.cbSize		= sizeof(wcx);
	wcx.lpfnWndProc	= AfxWndProc;
	wcx.style			= CS_DBLCLKS|CS_SAVEBITS;
	wcx.cbClsExtra		= 0;
	wcx.cbWndExtra		= 0;
	wcx.hInstance		= AfxGetInstanceHandle();
	wcx.hIcon			= NULL;
	wcx.hCursor		= LoadCursor(NULL,IDC_ARROW);
	wcx.hbrBackground	= ::GetSysColorBrush(COLOR_WINDOW);
	wcx.lpszMenuName	= NULL;
	wcx.lpszClassName	= "TaskbarNotifierClass";
	wcx.hIconSm		= NULL;
	::RegisterClassEx(&wcx);
	
	// crea la finestra
	if((m_bIsValid = CreateEx(WS_EX_TOPMOST,"TaskbarNotifierClass",NULL,WS_POPUP,0,0,0,0,m_pWndParent->GetSafeHwnd(),NULL))==TRUE)
	{
		// imposta l'handle della finestra per la trasparenza
		SetWindow(CWnd::m_hWnd);

		// crea il tooltip impostando l'area sulla dimensione dell'intera finestra
		m_wndToolTip.Create(this,TTS_ALWAYSTIP);
		m_wndToolTip.SetWidth(TOOLTIP_REASONABLE_WIDTH);
		m_wndToolTip.SetDelay(TOOLTIP_REASONABLE_DELAYTIME);
		CRect rect(0,0,0,0);
		m_wndToolTip.AddRectTool(this,"taskbarpopup",&rect,ID_TOOLTIP_CONTROL);
		
		ShowWindow(SW_HIDE);
	}

	return(m_bIsValid);
}

/*
	Show()
*/
BOOL CTaskbarNotifier::Show(LPCTSTR lpcszText,DWORD dwTimeToShow /*= 600L*/,DWORD dwTimeToStay /*= 4000L*/,DWORD dwTimeToHide /*= 200L*/,int nXPos /*= -1*/,int nYPos /*= -1*/)
{
	BOOL bShowed = FALSE;
	
	if(!m_bIsValid)
		return(bShowed);

	// calcola le coordinate dello schermo e della taskbar
	TASKBARPOS tbi;
	::GetTaskBarPos(&tbi);
	
	if(tbi.hWnd && m_bBitmapLoaded)
	{
		// sdraia i timers e la finestra attiva
		ResetTimers();
		ResetWindow();

		// imposta le posizioni assolute per la finestra, se specificate
		m_nXPos = nXPos;
		m_nYPos = nYPos;

		// imposta il testo
		// se si trova il modalita' scrolling ed il testo termina senza spazi, aggiunge un separatore per
		// far si che la fine del testo non venga visualizzata attaccata all'inizio durante lo scroll
		m_strText.Format("%s%s",lpcszText,lpcszText[strlen(lpcszText)-1]!=' ' && m_bScrollText ? " " : "");

		// imposta il filler se la lunghezza del testo non e' sufficente
		if(m_bScrollText)
		{
			CPaintDC dc(this);
			CFont *pOldFont;
			dc.SetTextColor(m_crNormalTextColor);
			pOldFont = dc.SelectObject(&m_fntNormal);
			CSize size(0,0);
			do {
				size = dc.GetTextExtent(m_strText);
				if(size.cx < m_rcText.Width())
				{
					m_strText += m_strTextFiller;
					size = dc.GetTextExtent(m_strText);
				}
			} while(size.cx < m_rcText.Width());
		}

		// verifica la posizione della taskbar
		UINT nBitmapSize = 0L;
		m_nTaskbarPlacement = tbi.nTaskbarPlacement;
		switch(m_nTaskbarPlacement)
		{
			case ABE_TOP:
			case ABE_BOTTOM:
				nBitmapSize = m_nBitmapHeight;
				break;
			case ABE_LEFT:
			case ABE_RIGHT:
				nBitmapSize = m_nBitmapWidth;
				break;
		}
		
		// imposta il tempo di permanenza
		// se viene specificata una posizione assoluta per la finestra, dato che in tal caso non effettua l'apparizione
		// a tendina, aggiunge al tempo di permanenza per la finestra popup quello per la scomparsa
		m_dwTimeToStay = dwTimeToStay;
		if(m_nXPos >= 0 && m_nYPos >= 0 && m_dwTimeToStay!=(DWORD)-1L)
			m_dwTimeToStay += dwTimeToHide;

		// calcola l'incremento (in pixel) ed il valore per il timer per l'apparizione a tendina
		if(dwTimeToShow > m_dwTimerPrecision)
		{
			UINT nEvents = min(DIV(dwTimeToShow,m_dwTimerPrecision),nBitmapSize);
			m_dwShowEvents = DIV(dwTimeToShow,nEvents);
			m_nIncrementShow = DIV(nBitmapSize,nEvents);
		}
		else
		{
			m_dwShowEvents = m_dwTimerPrecision;
			m_nIncrementShow = nBitmapSize;
		}

		// calcola l'incremento (in pixel) ed il valore per il timer per la scomparsa a tendina
		// se viene specificata una posizione assoluta per la finestra, dato che in tal caso non effettua la scomparsa
		// a tendina, imposta il tempo di scomparsa a 0 in modo tale che durante l'elaborazione del timer relativo la
		// finestra venga chiusa immediatamente
		if(m_nXPos >= 0 && m_nYPos >= 0)
			dwTimeToHide = 0L;
		else
		{
			if(dwTimeToHide > m_dwTimerPrecision)
			{
				UINT nEvents = min(DIV(dwTimeToHide,m_dwTimerPrecision),nBitmapSize);
				m_dwHideEvents = DIV(dwTimeToHide,nEvents);
				m_nIncrementHide = DIV(nBitmapSize,nEvents);
			}
			else
			{
				m_dwShowEvents = m_dwTimerPrecision;
				m_nIncrementHide = nBitmapSize;
			}
		}

		// inizializza i valori relativi alle coordinate della finestra per l'animazione
		// distingue a seconda se e' stata specificata una posizione assoluta per la finestra
		m_nCurrentPosX = m_nCurrentPosY = m_nCurrentWidth = m_nCurrentHeight = 0;
		if(m_nXPos >= 0 && m_nYPos >= 0)
		{
			m_nCurrentPosX = m_nXPos;
			m_nCurrentPosY = m_nYPos;
			m_nCurrentWidth = m_nBitmapWidth;
			m_nCurrentHeight = m_nBitmapHeight;
		}
		else
		{
			if(m_nTaskbarPlacement==ABE_RIGHT)
			{
				m_nCurrentPosX = tbi.rc.left;
				m_nCurrentPosY = tbi.rc.bottom - m_nBitmapHeight;
				m_nCurrentWidth = 0;
				m_nCurrentHeight = m_nBitmapHeight;
			}
			else if(m_nTaskbarPlacement==ABE_LEFT)
			{
				m_nCurrentPosX = tbi.rc.right;
				m_nCurrentPosY = tbi.rc.bottom - m_nBitmapHeight;
				m_nCurrentWidth = 0;
				m_nCurrentHeight = m_nBitmapHeight;
			}
			else if(m_nTaskbarPlacement==ABE_TOP)
			{
				m_nCurrentPosX = tbi.rc.right - m_nBitmapWidth;
				m_nCurrentPosY = tbi.rc.bottom;
				m_nCurrentWidth = m_nBitmapWidth;
				m_nCurrentHeight = 0;
			}
			else if(m_nTaskbarPlacement==ABE_BOTTOM)
			{
				// taskbar is on the bottom or invisible
				m_nCurrentPosX = tbi.rc.right - m_nBitmapWidth;
				m_nCurrentPosY = tbi.rc.top;
				m_nCurrentWidth = m_nBitmapWidth;
				m_nCurrentHeight = 0;
			}
		}

		// imposta il flag per il primo piano 
		ShowWindow(SW_SHOWNOACTIVATE);
		SetWindowPos(m_bIsTopMost ? &wndTopMost : &wndNoTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);

		// crea il timer per l'apparizione della finestra
		m_nTimerAppearing = ::SetTimer(CWnd::m_hWnd,m_nIDT_APPEARING,m_dwShowEvents,NULL);

		// crea il timer per lo scrolling del testo
		if(m_bScrollText)
			m_nTimerScrolling = ::SetTimer(CWnd::m_hWnd,m_nIDT_SCROLLING,m_dwScrollSpeed,NULL);

		bShowed = TRUE;
	}

	return(bShowed);
}

/*
	UnShow()
*/
BOOL CTaskbarNotifier::UnShow(void)
{
	BOOL bUnShowed = FALSE;

	if(!m_bIsValid)
		return(bUnShowed);

	// scarica la finestra
	if(m_bBitmapLoaded && m_nAnimStatus!=IDT_HIDDEN)
	{
		// deve resettare i timer dato che gestisce in proprio la scomparsa della finestra
		ResetTimers();

		// cicla fino a che la finestra non scompare
		// se e' stata specificata una posizione assoluta per la finestra la chiude immediatamente
		BOOL bVisible = (m_nXPos >= 0 && m_nYPos >= 0) ? FALSE : TRUE;

		for(int i=0; bVisible && i < 100; i++)
		{
			switch(m_nTaskbarPlacement)
			{
				case ABE_BOTTOM:
					if(m_nCurrentHeight > 0)
					{
						m_nCurrentPosY += m_nIncrementHide;
						m_nCurrentHeight -= m_nIncrementHide;
					}
					else
						bVisible = FALSE;
					break;

				case ABE_TOP:
					if (m_nCurrentHeight > 0)
						m_nCurrentHeight -= m_nIncrementHide;
					else
						bVisible = FALSE;
					break;

				case ABE_LEFT:
					if (m_nCurrentWidth > 0)
						m_nCurrentWidth -= m_nIncrementHide;
					else
						bVisible = FALSE;
					break;

				case ABE_RIGHT:
					if (m_nCurrentWidth > 0)
					{
						m_nCurrentPosX += m_nIncrementHide;
						m_nCurrentWidth -= m_nIncrementHide;
					}
					else
						bVisible = FALSE;
					break;
			}

			// incrementa la velocita' per la scomparsa e riposiziona la finestra
			m_nIncrementHide++;
			SetWindowPos(m_bIsTopMost ? &wndTopMost : &wndNoTopMost,m_nCurrentPosX,m_nCurrentPosY,m_nCurrentWidth,m_nCurrentHeight,SWP_NOACTIVATE);
		}

		ResetWindow();
	}

	return(TRUE);
}

/*
	SetBitmap()
*/
BOOL CTaskbarNotifier::SetBitmap(UINT nBitmapID,BYTE R /*= -1*/,BYTE G /*= -1*/,BYTE B /*= -1*/)
{
	// carica il bitmap a partire dalla risorsa
	BOOL bLoaded = FALSE;

	if(!m_bIsValid)
		return(bLoaded);

	ResetBitmap();	
	
	if(m_bitmapBackground.LoadBitmap(nBitmapID))
		bLoaded = SetBitmap(R,G,B);

	return(bLoaded);
}

/*
	SetBitmap()
*/
BOOL CTaskbarNotifier::SetBitmap(LPCTSTR lpcszFileName,BYTE R /*= -1*/,BYTE G /*= -1*/,BYTE B /*= -1*/)
{
	// carica il bitmap a partire dal file
	BOOL bLoaded = FALSE;

	if(!m_bIsValid)
		return(bLoaded);
	
	ResetBitmap();
	
	HBITMAP hBitmap = (HBITMAP)::LoadImage(NULL,lpcszFileName,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);

#ifdef _LOAD_ALL_IMAGE_FORMATS
	// formato non supportato, carica con l'interfaccia
	if(!hBitmap)
	{
		if(!m_pImage)
			m_pImage = m_ImageFactory.Create(m_szLibraryName,sizeof(m_szLibraryName));
		if(m_pImage)
			if(m_pImage->Load(lpcszFileName))
				hBitmap = m_pImage->GetBitmap();
	}
#endif

	if(hBitmap)
	{
		m_bitmapBackground.Attach(hBitmap);
		bLoaded = SetBitmap(R,G,B);
	}

	return(bLoaded);
}

/*
	SetBitmap()
*/
BOOL CTaskbarNotifier::SetBitmap(BYTE R,BYTE G,BYTE B)
{
	if(!m_bIsValid)
		return(FALSE);

	// carica (realmente) il bitmap
	BITMAP bm = {0};
	
	if(::GetObject(m_bitmapBackground.GetSafeHandle(),sizeof(bm),&bm)!=0)
	{
		m_nBitmapWidth = bm.bmWidth;
		m_nBitmapHeight = bm.bmHeight;
		m_rcText.SetRect(0,0,bm.bmWidth,bm.bmHeight);
		
		if(R!=-1 && G!=-1 && B!=-1)
		{
			// no need to delete the HRGN, SetWindowRgn() owns it after being called
			if((m_hBitmapRegion = CreateRgnFromBitmap((HBITMAP)m_bitmapBackground.GetSafeHandle(),RGB(R,G,B)))!=(HRGN)NULL)
				m_bBitmapLoaded = SetWindowRgn(m_hBitmapRegion,TRUE)!=0;
		}
	}

	return(m_bBitmapLoaded);
}

/*
	SetFont()
*/
void CTaskbarNotifier::SetFont(LPCTSTR lpcszFont,int nSize,int nNormalStyle,int nSelectedStyle)
{
	if(!m_bIsValid)
		return;

	LOGFONT lf = {0};

	m_fntNormal.DeleteObject();
	m_fntNormal.CreatePointFont(nSize,lpcszFont);
	m_fntNormal.GetLogFont(&lf);

//	lf.lfWeight = (nNormalStyle & TN_TEXT_BOLD) ? FW_BOLD : FW_DONTCARE;
	lf.lfWeight = (nNormalStyle & TN_TEXT_BOLD) ? FW_BOLD : FW_NORMAL;
	lf.lfItalic = (nNormalStyle & TN_TEXT_ITALIC) ? TRUE : FALSE;
	lf.lfUnderline = (nNormalStyle & TN_TEXT_UNDERLINE) ?  TRUE : FALSE;

	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfOutPrecision = OUT_TT_PRECIS;
	lf.lfQuality = PROOF_QUALITY;

	m_fntNormal.DeleteObject();
	m_fntNormal.CreateFontIndirect(&lf);
	
//	lf.lfWeight = (nSelectedStyle & TN_TEXT_BOLD) ? FW_BOLD : FW_DONTCARE;
	lf.lfWeight = (nSelectedStyle & TN_TEXT_BOLD) ? FW_BOLD : FW_NORMAL;
	lf.lfItalic = (nSelectedStyle & TN_TEXT_ITALIC) ? TRUE : FALSE;
	lf.lfUnderline = (nSelectedStyle & TN_TEXT_UNDERLINE) ? TRUE : FALSE;

	m_fntSelected.DeleteObject();
	m_fntSelected.CreateFontIndirect(&lf);
}

/*
	SetDefaultFont()
*/
void CTaskbarNotifier::SetDefaultFont(void)
{
	if(!m_bIsValid)
		return;

	LOGFONT lf = {0};
	CFont *pFont = CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT));
	pFont->GetLogFont(&lf);
	m_fntNormal.DeleteObject();
	m_fntSelected.DeleteObject();
	m_fntNormal.CreateFontIndirect(&lf);
	lf.lfUnderline = TRUE;
	m_fntSelected.CreateFontIndirect(&lf);
}

/*
	SetTextColor()
*/
void CTaskbarNotifier::SetTextColor(COLORREF crNormalTextColor,COLORREF crSelectedTextColor)
{
	if(!m_bIsValid)
		return;

	m_crNormalTextColor = crNormalTextColor;
	m_crSelectedTextColor = crSelectedTextColor;
	RedrawWindow();
}

/*
	SetTextScroll()
*/
void CTaskbarNotifier::SetTextScroll(BOOL bScroll,int nDirection/* = -1, -1=left, 1=right, 0=none */,DWORD dwSpeed/* = 200L*/)
{
	if(!m_bIsValid)
		return;

	m_bScrollText = bScroll;
	m_nScrollDirection = nDirection;
	m_dwScrollSpeed = dwSpeed >= 50L && dwSpeed <= 1000L ? dwSpeed : 200L;
	
	if(m_bScrollText)
	{
		if(m_nDefaultTextFormat & DT_WORDBREAK)
		{
			m_nDefaultTextFormat &= ~DT_WORDBREAK;
			m_nDefaultTextFormat |= DT_SINGLELINE;
		}
		if(m_nDefaultTextFormat & DT_END_ELLIPSIS)
			m_nDefaultTextFormat &= ~DT_END_ELLIPSIS;
	}
	else
	{
		if(m_nDefaultTextFormat & DT_SINGLELINE)
		{
			m_nDefaultTextFormat &= ~DT_SINGLELINE;
			m_nDefaultTextFormat |= DT_WORDBREAK;
		}
	}
}

/*
	CreateRgnFromBitmap()
*/
HRGN CTaskbarNotifier::CreateRgnFromBitmap(HBITMAP hBitmap,COLORREF color)
{
	HRGN hRgn = NULL;
	RGNDATAHEADER* pRgnData = NULL;

	if(!m_bIsValid)
		return(hRgn);

	if(hBitmap)
	{
		BITMAP bm = {0};
		if(::GetObject(hBitmap,sizeof(bm),&bm)!=0) // get bitmap attributes
		{
			CDC dcBmp;
			dcBmp.CreateCompatibleDC(GetDC());	// create a memory device context for the bitmap
			dcBmp.SelectObject(hBitmap);		// select the bitmap in the device context

			const DWORD RDHDR = sizeof(RGNDATAHEADER);
			const DWORD MAXBUF = 40; // size of one block in RECTs (i.e. MAXBUF*sizeof(RECT) in bytes)
			LPRECT pRects = NULL;
			DWORD dwBlocks = 0L; // number of allocated blocks
			INT i,j; // current position in mask image
			INT nFirst = 0; // left position of current scan line where mask was found
			BOOL	bWasFirst = FALSE; // set when if mask was found in current scan line
			BOOL	bIsMask = FALSE; // set when current color is mask color

			// allocate memory for region data
			pRgnData = (RGNDATAHEADER*)new BYTE[RDHDR + ((++dwBlocks) * MAXBUF * sizeof(RECT))];
			if(pRgnData)
			{
				memset(pRgnData,'\0',RDHDR + (dwBlocks * MAXBUF * sizeof(RECT)));
				
				// fill it by default
				pRgnData->dwSize = RDHDR;
				pRgnData->iType = RDH_RECTANGLES;
				pRgnData->nCount = 0;

				for(i = 0; i < bm.bmHeight; i++)
					for(j = 0; j < bm.bmWidth; j++ )
					{
						// get color
						bIsMask = (dcBmp.GetPixel(j,bm.bmHeight-i-1)!=color);
						
						// place part of scan line as RECT region if transparent color found after mask color or
						// mask color found at the end of mask image
						if(bWasFirst && ((bIsMask && (j==(bm.bmWidth - 1))) || (bIsMask ^ (j < bm.bmWidth))))
						{
							// get offset to RECT array if RGNDATA buffer
							pRects = (LPRECT)((LPBYTE)pRgnData + RDHDR);
							
							// save current RECT
							pRects[pRgnData->nCount++] = CRect(nFirst,bm.bmHeight - i - 1,j + (j==(bm.bmWidth-1)),bm.bmHeight - i);
							
							// if buffer full reallocate it
							if(pRgnData->nCount >= dwBlocks * MAXBUF)
							{
								LPBYTE pRgnDataNew = new BYTE[RDHDR + ((++dwBlocks) * MAXBUF * sizeof(RECT))];
								if(pRgnDataNew)
								{
									memcpy(pRgnDataNew,pRgnData,RDHDR + ((dwBlocks - 1) * MAXBUF * sizeof(RECT)));
									delete [] pRgnData;
									pRgnData = (RGNDATAHEADER*)pRgnDataNew;
								}
								else
								{
									hRgn = NULL;
									goto done;
								}
							}

							bWasFirst = FALSE;
						}
						else if(!bWasFirst && bIsMask ) // set bWasFirst when mask is found
						{
							nFirst = j;
							bWasFirst = TRUE;
						}
					}
				
				dcBmp.DeleteDC(); //release the bitmap
				
				// create region
				//hRgn = ::ExtCreateRegion(NULL,RDHDR + (pRgnData->nCount * sizeof(RECT)),(LPRGNDATA)pRgnData);
				// under WinNT the ExtCreateRegion returns NULL (by Fable@aramszu.net)
				
				// ExtCreateRegion replacement:
				hRgn = ::CreateRectRgn(0,0,0,0);
				if(hRgn)
				{
					pRects = (LPRECT)((LPBYTE)pRgnData + RDHDR);
					for(i = 0; i < (int)pRgnData->nCount; i++)
					{
						HRGN hr = ::CreateRectRgn(pRects[i].left,pRects[i].top,pRects[i].right,pRects[i].bottom);
						if(hr)
						{
							int nRet = ::CombineRgn(hRgn,hRgn,hr,RGN_OR);
							::DeleteObject(hr);
							if(nRet==ERROR)
							{
								hRgn = NULL;
								goto done;
							}
						}
						else
						{
							hRgn = NULL;
							goto done;
						}
					}
				}
				// ExtCreateRegion replacement
			}
		}
	}

done:
	
	if(pRgnData)
		delete [] pRgnData;
	
	return(hRgn);
}

/*
	OnMouseMove()
*/
void CTaskbarNotifier::OnMouseMove(UINT nFlags,CPoint point)
{
	m_trackMouseEvent.cbSize = sizeof(TRACKMOUSEEVENT);
	m_trackMouseEvent.dwFlags = TME_LEAVE | TME_HOVER;
	m_trackMouseEvent.hwndTrack = CWnd::m_hWnd;
	m_trackMouseEvent.dwHoverTime = HOVER_DEFAULT; //1;

	::_TrackMouseEvent(&m_trackMouseEvent);

	CWnd::OnMouseMove(nFlags,point);
}

/*
	OnLButtonUp()
*/
void CTaskbarNotifier::OnLButtonUp(UINT nFlags,CPoint point)
{
	if(m_bHandleMouseClick)
	{
		// notify the parent window that the notifier popup was clicked
		if(m_nMsg!=0L)
		{
			m_nOnLButtonUpFlags = nFlags;
			m_ptOnLButtonUpPoint.x = point.x;
			m_ptOnLButtonUpPoint.y = point.y;
			if(m_hWndParent)
				//m_pWndParent->PostMessage(m_nMsg,(WPARAM)m_nOnLButtonUpFlags,(LPARAM)&m_ptOnLButtonUpPoint);
				::PostMessage(m_hWndParent,m_nMsg,(WPARAM)m_nOnLButtonUpFlags,(LPARAM)&m_ptOnLButtonUpPoint);
		}
	}

	CWnd::OnLButtonUp(nFlags,point);
}

/*
	OnMouseHover()
*/
LRESULT CTaskbarNotifier::OnMouseHover(WPARAM /*wParam*/,LPARAM /*lParam*/)
{
	if(m_bHandleMouseClick)
		if(!m_bMouseIsOver)
		{
			m_bMouseIsOver = TRUE;
			RedrawWindow();
		}

	return(0L);
}

/*
	OnMouseLeave()
*/
LRESULT CTaskbarNotifier::OnMouseLeave(WPARAM /*wParam*/,LPARAM /*lParam*/)
{
	if(m_bHandleMouseClick)
		if(m_bMouseIsOver)
		{
			m_bMouseIsOver = FALSE;
			RedrawWindow();
		}

	return(0L);
}

/*
	OnEraseBkgnd()
*/
BOOL CTaskbarNotifier::OnEraseBkgnd(CDC* pDC)
{
	CDC memDC;
	CBitmap *pOldBitmap;

	memDC.CreateCompatibleDC(pDC);
	pOldBitmap = memDC.SelectObject(&m_bitmapBackground);
	pDC->BitBlt(0,0,m_nCurrentWidth,m_nCurrentHeight,&memDC,0,0,SRCCOPY);
	memDC.SelectObject(pOldBitmap);

	return(TRUE);
}

/*
	OnPaint()
*/
void CTaskbarNotifier::OnPaint(void)
{
	CPaintDC dc(this);
	CFont *pOldFont;
		
	if(m_bHandleMouseClick)
	{
		if(m_bMouseIsOver)
		{
			dc.SetTextColor(m_crSelectedTextColor);
			pOldFont = dc.SelectObject(&m_fntSelected);
		}
		else
		{
			dc.SetTextColor(m_crNormalTextColor);
			pOldFont = dc.SelectObject(&m_fntNormal);
		}
	}
	else
	{
		dc.SetTextColor(m_crNormalTextColor);
		pOldFont = dc.SelectObject(&m_fntNormal);
	}

	dc.SetBkMode(TRANSPARENT); 
	dc.DrawText(m_strText,m_rcText,m_nDefaultTextFormat);
	dc.SelectObject(pOldFont);
}

/*
	OnSetCursor()
*/
BOOL CTaskbarNotifier::OnSetCursor(CWnd* pWnd,UINT nHitTest,UINT nMsg)
{
	BOOL bHalt = TRUE;

	if(nHitTest==HTCLIENT && m_bHandleMouseClick)
	{
		if(!m_hCursor)
			m_hCursor = ::LoadCursor(NULL,MAKEINTRESOURCE(32649));
		::SetCursor(m_hCursor);
		bHalt = TRUE;
	}
	else
		bHalt = CWnd::OnSetCursor(pWnd,nHitTest,nMsg);

	return(bHalt);
}

/*
	OnTimer()
*/
void CTaskbarNotifier::OnTimer(UINT nIDEvent)
{
	if(nIDEvent==m_nIDT_APPEARING)
	{
		BOOL bAppeared = TRUE;
		m_nAnimStatus = m_nIDT_APPEARING;

		switch(m_nTaskbarPlacement)
		{
			case ABE_BOTTOM:
				if(m_nCurrentHeight < m_nBitmapHeight)
				{
					m_nCurrentPosY -= m_nIncrementShow;
					m_nCurrentHeight += m_nIncrementShow;
					bAppeared = FALSE;
				}
				break;
	
			case ABE_TOP:
				if(m_nCurrentHeight < m_nBitmapHeight)
				{
					m_nCurrentHeight += m_nIncrementShow;
					bAppeared = FALSE;
				}
				break;
	
			case ABE_LEFT:
				if(m_nCurrentWidth < m_nBitmapWidth)
				{
					m_nCurrentWidth += m_nIncrementShow;
					bAppeared = FALSE;
				}
				break;
	
			case ABE_RIGHT:
				if(m_nCurrentWidth < m_nBitmapWidth)
				{
					m_nCurrentPosX -= m_nIncrementShow;
					m_nCurrentWidth += m_nIncrementShow;
					bAppeared = FALSE;
				}
				break;
		}

		SetWindowPos(m_bIsTopMost ? &wndTopMost : &wndNoTopMost,m_nCurrentPosX,m_nCurrentPosY,m_nCurrentWidth,m_nCurrentHeight,SWP_NOACTIVATE);

		if(bAppeared)
		{
			if(m_nTimerAppearing!=(UINT)-1)
			{
				::KillTimer(CWnd::m_hWnd,m_nTimerAppearing);
				m_nTimerAppearing = (UINT)-1;
			}
			if(m_nTimerWaiting!=(UINT)-1)
				::KillTimer(CWnd::m_hWnd,m_nTimerWaiting);
			if(m_dwTimeToStay!=(DWORD)-1L)
				m_nTimerWaiting = ::SetTimer(CWnd::m_hWnd,m_nIDT_WAITING,m_dwTimeToStay,NULL);
			else
				m_nTimerWaiting = (UINT)-1L;
			m_nAnimStatus = m_nIDT_WAITING;

			// aggiorna il testo del tooltip
			if(m_szToolTipText[0]=='\0' || strcmp(m_szToolTipText,"")==0)
				;
			else
			{
				m_wndToolTip.UpdateTipText(m_szToolTipText,this,ID_TOOLTIP_CONTROL);
				CRect rect(0,0,m_nBitmapWidth,m_nBitmapHeight);
				m_wndToolTip.SetToolRect(this,ID_TOOLTIP_CONTROL,&rect);
				m_wndToolTip.Activate(TRUE);
			}
		}
	}
	else if(nIDEvent==m_nIDT_WAITING)
	{
		if(m_nTimerWaiting!=(UINT)-1)
		{
			::KillTimer(CWnd::m_hWnd,m_nTimerWaiting);
			m_nTimerWaiting = (UINT)-1;
		}
		
		if(m_nTimerDisappearing!=(UINT)-1)
			::KillTimer(CWnd::m_hWnd,m_nTimerDisappearing);
		if(m_dwHideEvents > 0L)
			m_nTimerDisappearing = ::SetTimer(CWnd::m_hWnd,m_nIDT_DISAPPEARING,m_dwHideEvents,NULL);
		else
			ResetWindow();
	}
	else if(nIDEvent==m_nIDT_DISAPPEARING)
	{
		BOOL bDisappeared = TRUE;
		m_nAnimStatus = m_nIDT_DISAPPEARING;
	
		switch(m_nTaskbarPlacement)
		{
			case ABE_BOTTOM:
				if(m_nCurrentHeight > 0)
				{
					m_nCurrentPosY += m_nIncrementHide;
					m_nCurrentHeight -= m_nIncrementHide;
					bDisappeared = FALSE;
				}
				break;
	
			case ABE_TOP:
				if(m_nCurrentHeight > 0)
				{
					m_nCurrentHeight -= m_nIncrementHide;
					bDisappeared = FALSE;
				}
				break;
	
			case ABE_LEFT:
				if(m_nCurrentWidth > 0)
				{
					m_nCurrentWidth -= m_nIncrementHide;
					bDisappeared = FALSE;
				}
				break;
	
			case ABE_RIGHT:
				if(m_nCurrentWidth > 0)
				{
					m_nCurrentPosX += m_nIncrementHide;
					m_nCurrentWidth -= m_nIncrementHide;
					bDisappeared = FALSE;
				}
				break;
		}

		if(bDisappeared)
		{
			if(m_nTimerDisappearing!=(UINT)-1)
			{
				::KillTimer(CWnd::m_hWnd,m_nTimerDisappearing);
				m_nTimerDisappearing = (UINT)-1;
			}

			ResetWindow();
		}
	
		SetWindowPos(m_bIsTopMost ? &wndTopMost : &wndNoTopMost,m_nCurrentPosX,m_nCurrentPosY,m_nCurrentWidth,m_nCurrentHeight,SWP_NOACTIVATE);
	}
	else if(nIDEvent==m_nIDT_SCROLLING)
	{
		if(m_bScrollText && m_nAnimStatus==(int)m_nIDT_WAITING)
		{
			char szText[1024];
			strcpyn(szText,LPCSTR(m_strText),sizeof(szText));
			strrot(szText,m_nScrollDirection,1);
			m_strText.Format("%s",szText);
			InvalidateRect(m_rcText,TRUE);
		}
	}
	else
		CWnd::OnTimer(nIDEvent);
}

/*
	OnClose()
*/
void CTaskbarNotifier::OnClose(void)
{
	// impedisce che la finestra venga chiusa (ad es. con ALT+F4)
}

/*
	OnDestroy()
*/
void CTaskbarNotifier::OnDestroy(void)
{
	CWnd::OnDestroy();
}
